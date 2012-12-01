/**
 * This software is released under the terms of the MIT License
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <features/ResourceFeatureClass.h>
#include <globals/String.h>
#include <globals/Errors.h>
#include <globals/Assets.h>
#include <globals/Events.h>
#include <MvcEditor.h>
#include <wx/artprov.h>
#include <wx/filename.h>
#include <wx/valgen.h>

static int ID_RESOURCE_READER_GAUGE = wxNewId();
static int ID_WIPE_THREAD = wxNewId();

mvceditor::ResourceFileWipeThreadClass::ResourceFileWipeThreadClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: ThreadWithHeartbeatClass(runningThreads, eventId) 
	, ResourceDbFileNames() {
		
}

bool mvceditor::ResourceFileWipeThreadClass::Init(const std::vector<mvceditor::ProjectClass>& projects) {
	std::vector<mvceditor::ProjectClass>::const_iterator it;
	for (it = projects.begin(); it != projects.end(); ++it) {
		if (it->IsEnabled && it->ResourceDbFileName.IsOk()) {
			ResourceDbFileNames.push_back(it->ResourceDbFileName);
		}
	}
	return !ResourceDbFileNames.empty();
}

void mvceditor::ResourceFileWipeThreadClass::BackgroundWork() {
	std::vector<wxFileName>::iterator it;
	for (it = ResourceDbFileNames.begin(); it != ResourceDbFileNames.end() && !TestDestroy(); ++it) {
		mvceditor::ResourceFinderClass resourceFinder;
		resourceFinder.InitFile(*it);
		resourceFinder.Wipe();
	}
	if (!TestDestroy()) {
		wxCommandEvent evt(mvceditor::EVENT_WIPE_COMPLETE);
		PostEvent(evt);
	}
}

mvceditor::ResourceFeatureClass::ResourceFeatureClass(mvceditor::AppClass& app)
	: FeatureClass(app)
	, JumpToText()
	, ProjectIndexMenu(NULL)
	, Timer()
	, WorkingCacheBuilder(NULL)
	, State(FREE) 
	, HasCodeLookups(false)
	, HasFileLookups(false) 
	, RunningThreadId(0) {
	IndexingDialog = NULL;
}

void mvceditor::ResourceFeatureClass::AddSearchMenuItems(wxMenu* searchMenu) {
	ProjectIndexMenu = searchMenu->Append(mvceditor::MENU_RESOURCE + 0, _("Index"), _("Index the project"));
	searchMenu->Append(mvceditor::MENU_RESOURCE + 1, _("Jump To Resource Under Cursor\tF12"), _("Jump To Resource that is under the cursor"));
	searchMenu->Append(mvceditor::MENU_RESOURCE + 2, _("Search for Resource...\tCTRL+R"), _("Search for a class, method, or function"));
	ProjectIndexMenu->Enable(App.Globals.HasSources() && FREE == State);
}

void mvceditor::ResourceFeatureClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_RESOURCE + 0] = wxT("Resource-Index Project");
	menuItemIds[mvceditor::MENU_RESOURCE + 1] = wxT("Resource-Jump To Resource Under Cursor");
	menuItemIds[mvceditor::MENU_RESOURCE + 2] = wxT("Resource-Search For Resource ...");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::ResourceFeatureClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	toolBar->AddTool(mvceditor::MENU_RESOURCE + 0, wxT("Index"), wxArtProvider::GetBitmap(
		wxART_EXECUTABLE_FILE, wxART_TOOLBAR, wxSize(16, 16)), wxT("Index"), wxITEM_NORMAL);
}

void mvceditor::ResourceFeatureClass::AddCodeControlClassContextMenuItems(wxMenu* menu) {
	menu->Append(mvceditor::MENU_RESOURCE + 3, _("Jump To Source"));
}

void mvceditor::ResourceFeatureClass::OnAppReady(wxCommandEvent& event) {

	// setup the timer that will be used to build the symbol table
	// in the background for the opened files.
	WorkingCacheBuilder = new mvceditor::WorkingCacheBuilderClass(App.RunningThreads, wxNewId());
	Timer.SetOwner(this);
	Timer.Start(2000, wxTIMER_CONTINUOUS);
	wxThreadIdType threadId;
	if (wxTHREAD_NO_ERROR != WorkingCacheBuilder->Init(threadId)) {
		delete WorkingCacheBuilder;
		WorkingCacheBuilder = NULL;
	}
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFeatureClass::SearchForResources(const wxString& text) {
	mvceditor::ResourceCacheClass* resourceCache = GetResourceCache();
	std::vector<mvceditor::ResourceClass> matches;
	bool exactOnly = text.Length() <= 2;
	if (exactOnly) {
		matches = resourceCache->CollectFullyQualifiedResourceFromAll(mvceditor::WxToIcu(text));
	}
	else {
		matches = resourceCache->CollectNearMatchResourcesFromAll(mvceditor::WxToIcu(text));
	}

	// no need to show jump to results for native functions
	// TODO: CollectNearResourceMatches shows resources from files that were recently deleted
	// need to hide them / remove them
	RemoveNativeMatches(matches);
	if (matches.empty() && !text.Contains(wxT("."))) {
		
		// dot == search for files
		matches = resourceCache->CollectNearMatchResourcesFromAll(mvceditor::WxToIcu(text + wxT(".")));
		RemoveNativeMatches(matches);
	}
	return matches;
}

void mvceditor::ResourceFeatureClass::OnWorkInProgress(wxCommandEvent& event) {
	GetStatusBarWithGauge()->IncrementGauge(ID_RESOURCE_READER_GAUGE, StatusBarWithGaugeClass::INDETERMINATE_MODE);
	if (IndexingDialog && IndexingDialog->IsShown()) {
		IndexingDialog->Increment();
	}
}

void mvceditor::ResourceFeatureClass::OnWorkComplete(wxCommandEvent& event) {
	RunningThreadId = 0;
	GetStatusBarWithGauge()->StopGauge(ID_RESOURCE_READER_GAUGE);
	if (IndexingDialog) {
		IndexingDialog->Destroy();
		IndexingDialog = NULL;
	}
	mvceditor::ResourceSearchClass resourceSearch(mvceditor::WxToIcu(JumpToText));
	if (INDEXING_PROJECT == State || GOTO == State) {

		// figure out what resources have been cached, so that next time we can jump
		// to the results without creating a new background thread
		if (mvceditor::ResourceSearchClass::CLASS_NAME == resourceSearch.GetResourceType() ||
			mvceditor::ResourceSearchClass::CLASS_NAME_METHOD_NAME == resourceSearch.GetResourceType()) {
			HasCodeLookups = true;
			HasFileLookups = true;
		}
		else if (mvceditor::ResourceSearchClass::FILE_NAME == resourceSearch.GetResourceType() ||
			mvceditor::ResourceSearchClass::FILE_NAME_LINE_NUMBER == resourceSearch.GetResourceType()) {
			HasFileLookups = true;
		}

		// if we indexed because of a user query; need to show the user the results.
		States previousState = State;
		State = FREE;
		std::vector<mvceditor::ResourceClass> chosenResources;
		if (GOTO == previousState && !JumpToText.IsEmpty()) {
			std::vector<mvceditor::ResourceClass> matches = SearchForResources(JumpToText);
			if (matches.size() == 1) {
				LoadPageFromResource(JumpToText, matches[0]);
			}
			else if (!matches.empty()) {
				mvceditor::ResourceSearchDialogClass dialog(GetMainWindow(), *this, JumpToText, chosenResources);
				dialog.Prepopulate(JumpToText, matches);
				if (dialog.ShowModal() == wxOK) {
					for (size_t i = 0; i < chosenResources.size(); ++i) {
						LoadPageFromResource(JumpToText, chosenResources[i]);
					}
				}
			}
		}
		else if (GOTO == previousState && JumpToText.IsEmpty()) {
			wxString term;
			mvceditor::ResourceSearchDialogClass dialog(GetMainWindow(), *this, term, chosenResources);
			if (dialog.ShowModal() == wxOK) {
				for (size_t i = 0; i < chosenResources.size(); ++i) {
					LoadPageFromResource(JumpToText, chosenResources[i]);
				}
			}
		}
	}
}

void mvceditor::ResourceFeatureClass::StartIndex() {
	if (App.Globals.HasSources()) {

		//prevent two finds at a time
		if (FREE == State) { 
			mvceditor::ProjectResourceActionClass* thread = new ProjectResourceActionClass(App.RunningThreads, mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE);
			if (thread->Init(App.Globals)) {
				RunningThreadId = 0;
				wxThreadError err = thread->CreateSingleInstance(RunningThreadId);
				if (wxTHREAD_NO_ERROR == err) {
					State = INDEXING_PROJECT;
					GetStatusBarWithGauge()->AddGauge(_("Indexing "), ID_RESOURCE_READER_GAUGE, 
						StatusBarWithGaugeClass::INDETERMINATE_MODE, wxGA_HORIZONTAL);
					if (!HasCodeLookups) {

						// empty resouce cache, indexing will take a significant amount of time. make the 
						// app more 'responsive' by showing a bigger gauge
						if (!IndexingDialog) {
							IndexingDialog = new mvceditor::IndexingDialogClass(NULL);
						}
						IndexingDialog->Show();
						IndexingDialog->Start();
					}

				}
				else {
					if (wxTHREAD_NO_RESOURCE == err) {
						mvceditor::EditorLogError(mvceditor::LOW_RESOURCES);
					}
					State = FREE;
					delete thread;
				}
			}
			else {
				wxMessageBox(_("Invalid Project Path"), wxT("Warning"), wxICON_EXCLAMATION);
			}
		}
		else {
			wxMessageBox(_("Indexing is already taking place. Please wait."), wxT("Warning"), wxICON_EXCLAMATION);
		}
	}
	else {
		wxMessageBox(_("This feature can only be used when you open project"), wxT("Warning"), wxICON_EXCLAMATION);
	}
}

void mvceditor::ResourceFeatureClass::OnProjectWipeAndIndex(wxCommandEvent& event) {
	if (RunningThreadId > 0) {
		App.RunningThreads.Stop(RunningThreadId);
		RunningThreadId = 0;
		State = FREE;
	}
	
	mvceditor::ResourceFileWipeThreadClass* thread = new mvceditor::ResourceFileWipeThreadClass(App.RunningThreads, ID_WIPE_THREAD);
	bool  wipeStarted = false;
	if  (thread->Init(App.Globals.Projects)) {
		wxThreadIdType threadId;
		wxThreadError error = thread->CreateSingleInstance(threadId);
		if (wxTHREAD_NO_ERROR == error) {
			wipeStarted = true;
			GetStatusBarWithGauge()->AddGauge(_("Indexing Projects"),
							ID_RESOURCE_READER_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE, wxGA_HORIZONTAL);
		}
	}
	if (!wipeStarted) {

		// no projects, just start the indexing process
		delete thread;
		StartIndex();
	}
}

void mvceditor::ResourceFeatureClass::OnWipeComplete(wxCommandEvent& event) {
	GetStatusBarWithGauge()->StopGauge(ID_RESOURCE_READER_GAUGE);
	StartIndex();
}

void mvceditor::ResourceFeatureClass::OnJump(wxCommandEvent& event) {

	// jump to selected resources
	CodeControlClass* codeControl = GetCurrentCodeControl();
	if (codeControl) {

		// if the cursor is in the middle of an identifier, find the end of the
		// current identifier; that way we can know the full name of the resource we want
		// to get
		int currentPos = codeControl->GetCurrentPos();
		int startPos = codeControl->WordStartPosition(currentPos, true);
		int endPos = codeControl->WordEndPosition(currentPos, true);
		wxString term = codeControl->GetTextRange(startPos, endPos);
	
		std::vector<mvceditor::ResourceClass> matches = codeControl->GetCurrentSymbolResource();
		RemoveNativeMatches(matches);
		if (!matches.empty()) {
			UnicodeString res = matches[0].ClassName + UNICODE_STRING_SIMPLE("::") + matches[0].Identifier;
			if (matches.size() == 1) {
				LoadPageFromResource(term, matches[0]);
			}
			else {
				std::vector<mvceditor::ResourceClass> chosenResources;
				mvceditor::ResourceSearchDialogClass dialog(GetMainWindow(), *this, term, chosenResources);
				if (dialog.ShowModal() == wxOK) {
					for (size_t i = 0; i < chosenResources.size(); ++i) {
						LoadPageFromResource(JumpToText, chosenResources[i]);
					}
				}
			}
		}
		else if (NeedToIndex(term)) {
			
			// maybe cache has not been created or user has not indexed the project, lets index the project and try again			
			JumpToText = term;
			StartIndex();

			// set the state here so that when indexing is done we know to
			// open the matches.
			State = GOTO;
		}
	}
}

void mvceditor::ResourceFeatureClass::OnSearchForResource(wxCommandEvent& event) {
	std::vector<mvceditor::ResourceClass> chosenResources;
	wxString term;
	mvceditor::ResourceSearchDialogClass dialog(GetMainWindow(), *this, term, chosenResources);
	if (dialog.ShowModal() == wxOK) {
		for (size_t i = 0; i < chosenResources.size(); ++i) {
			LoadPageFromResource(term, chosenResources[i]);
		}
	}
}


void mvceditor::ResourceFeatureClass::LoadPageFromResource(const wxString& finderQuery, const mvceditor::ResourceClass& resource) {
	mvceditor::ResourceSearchClass resourceSearch(mvceditor::WxToIcu(finderQuery));
	wxFileName fileName = resource.FileName();
	if (!fileName.FileExists()) {
		mvceditor::EditorLogWarning(mvceditor::WARNING_OTHER, _("File no longer exists:") + fileName.GetFullPath());
	}
	GetNotebook()->LoadPage(resource.GetFullPath());
	CodeControlClass* codeControl = GetCurrentCodeControl();
	if (codeControl) {
		int32_t position, 
			length;
		bool found = mvceditor::ResourceFinderClass::GetResourceMatchPosition(resource, codeControl->GetSafeText(), position, length);
		if (mvceditor::ResourceSearchClass::FILE_NAME_LINE_NUMBER == resourceSearch.GetResourceType()) {
				
			// scintilla line numbers start at zero. use the ensure method so that the line is shown in the 
			// center of the screen
			int pos = codeControl->PositionFromLine(resourceSearch.GetLineNumber() - 1);
			codeControl->SetSelectionAndEnsureVisible(pos, pos);
			codeControl->GotoLine(resourceSearch.GetLineNumber() - 1);
		}
		if (mvceditor::ResourceSearchClass::FILE_NAME == resourceSearch.GetResourceType()) {
				
			// nothing; just open the file but don't scroll down to any place
		}
		else if (found) {
			codeControl->SetSelectionAndEnsureVisible(position, position + length);
		}
	}
}

void mvceditor::ResourceFeatureClass::OnUpdateUi(wxUpdateUIEvent& event) {
	ProjectIndexMenu->Enable(App.Globals.HasSources() && FREE == State);
	event.Skip();
}

bool mvceditor::ResourceFeatureClass::NeedToIndex(const wxString& finderQuery) const {
	mvceditor::ResourceSearchClass resourceSearch(mvceditor::WxToIcu(finderQuery));
	if ((mvceditor::ResourceSearchClass::CLASS_NAME == resourceSearch.GetResourceType() ||
		mvceditor::ResourceSearchClass::CLASS_NAME_METHOD_NAME == resourceSearch.GetResourceType()) && !HasCodeLookups)  {
		return true;
	}
	else if ((mvceditor::ResourceSearchClass::FILE_NAME == resourceSearch.GetResourceType() ||
		mvceditor::ResourceSearchClass::FILE_NAME_LINE_NUMBER == resourceSearch.GetResourceType()) && !HasFileLookups) {
		return true;
	}
	return false;
}

void mvceditor::ResourceFeatureClass::OpenFile(wxString fileName) {
	GetNotebook()->LoadPage(fileName);
}

void mvceditor::ResourceFeatureClass::OnAppFileClosed(wxCommandEvent& event) {

	// only index when there is a project open
	// need to make sure that the file that was closed is in the opened project
	// as well. don't want single-leaf files to be parsed for resources .. or
	// do we?
	wxString fileName = event.GetString();
	std::vector<mvceditor::ProjectClass>::const_iterator project;
	pelet::Versions version = GetEnvironment()->Php.Version;
	for (project = App.Globals.Projects.begin(); project != App.Globals.Projects.end(); ++project) {

		if (project->IsAPhpSourceFile(fileName)) {
			mvceditor::ProjectResourceActionClass* thread = new mvceditor::ProjectResourceActionClass(App.RunningThreads, mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE);
			thread->InitForFile(*project, fileName, version);

			// show user the error? not for now as they cannot do anything about it
			wxThreadError err = thread->CreateSingleInstance(RunningThreadId);
			if (wxTHREAD_NO_ERROR == err) {
				delete thread;
			}
			else {

				// file can only belong to one project?
				break;
			}
		}
	}

	// Notebook class assigns unique IDs to CodeControlClass objects
	// the event ID is the ID of the code control that was closed
	// this needs to be the same as mvceditor::CodeControlClass::GetIdString
	wxString idString = wxString::Format(wxT("File_%d"), event.GetId());
	App.Globals.ResourceCache.RemoveWorking(idString);
}

void mvceditor::ResourceFeatureClass::RemoveNativeMatches(std::vector<mvceditor::ResourceClass>& matches) const {
	std::vector<mvceditor::ResourceClass>::iterator it = matches.begin();
	while (it != matches.end()) {
		if (it->IsNative) {
			it = matches.erase(it);
		}
		else {
			it++;
		}
	}
}

wxString mvceditor::ResourceFeatureClass::CacheStatus() {
	if (HasCodeLookups && HasFileLookups) {
		return _("OK");
	}
	return _("Stale");
}

void mvceditor::ResourceFeatureClass::OnWorkingCacheComplete(mvceditor::WorkingCacheCompleteEventClass& event) {
	wxString fileIdentifier = event.GetFileIdentifier();
	bool good = App.Globals.ResourceCache.RegisterWorking(fileIdentifier, event.WorkingCache);
	if (!good) {

		// already there
		App.Globals.ResourceCache.ReplaceWorking(fileIdentifier, event.WorkingCache);
	}
	event.Skip();
}

void mvceditor::ResourceFeatureClass::OnTimer(wxTimerEvent& event) {
	if (WorkingCacheBuilder) {
		mvceditor::CodeControlClass* codeControl = GetCurrentCodeControl();
		if (codeControl && codeControl->GetDocumentMode() == mvceditor::CodeControlClass::PHP) {
			UnicodeString text = codeControl->GetSafeText();

			// we need to differentiate between new and opened files (the 'true' arg)
			WorkingCacheBuilder->Update(
				codeControl->GetFileName(),
				codeControl->GetIdString(),
				text, 
				codeControl->IsNew(),
				App.Globals.Environment.Php.Version);
		}
	}
	event.Skip();
}

mvceditor::ResourceSearchDialogClass::ResourceSearchDialogClass(wxWindow* parent, ResourceFeatureClass& resource,
																wxString& term,
																std::vector<mvceditor::ResourceClass>& chosenResources)
	: ResourceSearchDialogGeneratedClass(parent)
	, ResourceFeature(resource)
	, ChosenResources(chosenResources) 
	, MatchedResources() {
	wxGenericValidator termValidator(&term);
	TransferDataToWindow();
	CacheStatusLabel->SetLabel(wxT("Cache Status: ") + resource.CacheStatus());
	SearchText->SetFocus();
}

void mvceditor::ResourceSearchDialogClass::OnSearchText(wxCommandEvent& event) {
	wxString text = SearchText->GetValue();
	if (text.Length() >= 2) {
		MatchedResources = ResourceFeature.SearchForResources(text);
		if (!MatchedResources.empty()) {
			MatchesList->Freeze();
			ShowJumpToResults(text, MatchedResources);
			MatchesList->Thaw();
		}
		else {
			MatchesList->Clear();
			MatchesLabel->SetLabel(_("No matches found"));
		}
	}
}

void mvceditor::ResourceSearchDialogClass::OnSearchEnter(wxCommandEvent& event) {
	if (MatchedResources.size() == 1) {

		// if there is only match, just take the user to it
		TransferDataFromWindow();
		ChosenResources.clear();
		ChosenResources.push_back(MatchedResources[0]);
		EndModal(wxOK);
	}
	else {
		wxArrayInt checks;
		for (size_t i = 0; i < MatchesList->GetCount(); ++i) {
			if (MatchesList->IsChecked(i)) {
				checks.Add(i);
			}
		}
		if (checks.Count() > 1) {
		
			// open the checked items
			for (size_t i = 0; i < checks.Count(); ++i) {
				size_t matchIndex = checks.Item(i);
				if (matchIndex >= 0 && matchIndex < MatchedResources.size()) {
					ChosenResources.push_back(MatchedResources[matchIndex]);
				}
			}
			EndModal(wxOK);
		}
		else {
			// no checked items, take the user to the
			// selected item
			size_t selection = MatchesList->GetSelection();
			if (selection >= 0 && selection < MatchedResources.size()) {
				ChosenResources.push_back(MatchedResources[selection]);
				EndModal(wxOK);
			}
		}

	}
}

void mvceditor::ResourceSearchDialogClass::ShowJumpToResults(const wxString& finderQuery, const std::vector<mvceditor::ResourceClass>& allMatches) {
	wxArrayString files;
	mvceditor::ResourceSearchClass resourceSearch(mvceditor::WxToIcu(finderQuery));
	for (size_t i = 0; i < allMatches.size(); ++i) {
		files.Add(allMatches[i].GetFullPath());
	}
	MatchesList->Clear();
	
	// dont show the project path to the user
	for (size_t i = 0; i < files.GetCount(); ++i) {
		wxString projectLabel;
		wxString relativeName = ResourceFeature.App.Globals.RelativeFileName(files[i], projectLabel);
		wxString matchLabel;
		mvceditor::ResourceClass match = allMatches[i];
		if (mvceditor::ResourceClass::MEMBER == match.Type || mvceditor::ResourceClass::METHOD == match.Type ||
			mvceditor::ResourceClass::CLASS_CONSTANT == match.Type) {
			matchLabel += mvceditor::IcuToWx(match.ClassName);
			matchLabel += wxT("::");
			matchLabel += mvceditor::IcuToWx(match.Identifier);
		}
		else if (mvceditor::ResourceClass::CLASS == match.Type || mvceditor::ResourceClass::FUNCTION == match.Type
			|| mvceditor::ResourceClass::DEFINE == match.Type) {
			matchLabel += mvceditor::IcuToWx(match.Identifier);
		}
		else {
			matchLabel += mvceditor::IcuToWx(match.Identifier);
		}
		matchLabel += wxT(" - ");
		matchLabel += relativeName;
		matchLabel +=  wxT("  (") + projectLabel + wxT(")");
		MatchesList->Append(matchLabel);
	}
	MatchesLabel->SetLabel(wxString::Format(_("Found %d files. Please choose file(s) to open."), allMatches.size()));
}

void mvceditor::ResourceSearchDialogClass::OnOkButton(wxCommandEvent& event) {
	TransferDataFromWindow();
	ChosenResources.clear();
	for (size_t i = 0; i < MatchesList->GetCount(); ++i) {
		if (MatchesList->IsChecked(i)) {
			ChosenResources.push_back(MatchedResources[i]);
		}
	}
	if (ChosenResources.empty()) {
		return;
	}
	EndModal(wxOK);
}

void mvceditor::ResourceSearchDialogClass::OnCancelButton(wxCommandEvent& event) {
	EndModal(wxCANCEL);
}

void mvceditor::ResourceSearchDialogClass::Prepopulate(const wxString& term, const std::vector<mvceditor::ResourceClass> &matches) {
	MatchedResources = matches;
	SearchText->SetValue(term);
	ShowJumpToResults(term, MatchedResources);
}
	
void mvceditor::ResourceSearchDialogClass::OnHelpButton(wxCommandEvent& event) {
	
  wxString help = wxString::FromAscii("Type in a file name, file name:page number, "
		"class name,  or class name::method name. The resulting page will then be opened.\n\nExamples:\n\n"
		"user.php\n"
		"user.php:129\n"
		"User\n"
		"User::login\n\n"
		"You can search with partial files names\n"
		"ser.php (would match user.php)\n\n"
		"You can search with partial class names\n"
		"Use (would match Use, User, UserClass, ...)\n\n"
		"You can search entire class names\n"
		"User:: (would match all methods including inherited methods, from User class)\n\n"
		"You can search all methods\n"
		"::print (would match all methods in all classes that start with 'print' )\n\n"
		"Cache Status:\n"
		"The resource cache will be stale when the application is first opened and\n"
		"will be OK after you have indexed the projects. "
	);
	help = wxGetTranslation(help);
	wxMessageBox(help, _("Resource Search Help"), wxOK, this);	
}

void mvceditor::ResourceSearchDialogClass::OnSearchKeyDown(wxKeyEvent& event) {
	int keyCode = event.GetKeyCode();
	size_t selection = MatchesList->GetSelection();
	if (keyCode == WXK_DOWN) {		
		if (!MatchesList->IsEmpty() && selection >= 0 && selection < (MatchesList->GetCount() - 1)) {
			MatchesList->SetSelection(selection + 1);
		}
		else if (!MatchesList->IsEmpty() && selection >= 0) {

			// cycle back to the beginning
			MatchesList->SetSelection(0);
		}
		SearchText->SetFocus();
	}
	else if (keyCode == WXK_UP) {
		if (!MatchesList->IsEmpty() && selection > 0 && selection < MatchesList->GetCount()) {
			MatchesList->SetSelection(selection - 1);
		}
		else if (!MatchesList->IsEmpty() && selection == 0) {

			// cycle back to the end
			MatchesList->SetSelection(MatchesList->GetCount() - 1);
		}
		SearchText->SetFocus();
	}
	else {
		event.Skip();
	}
}

void mvceditor::ResourceSearchDialogClass::OnMatchesListDoubleClick(wxCommandEvent& event) {
	TransferDataFromWindow();
	ChosenResources.clear();
	size_t selection = event.GetSelection();
	if (selection >= 0 && selection < MatchesList->GetCount()) {
		ChosenResources.push_back(MatchedResources[selection]);
	}
	if (ChosenResources.empty()) {
		return;
	}
	EndModal(wxOK);
}

void mvceditor::ResourceSearchDialogClass::OnMatchesListKeyDown(wxKeyEvent& event) {
	if (event.GetKeyCode() == WXK_RETURN) {
		wxCommandEvent cmdEvt;
		OnSearchEnter(cmdEvt);
	}
	else {
		event.Skip();
	}
}

mvceditor::IndexingDialogClass::IndexingDialogClass(wxWindow* parent) 
	: IndexingDialogGeneratedClass(parent) {
}

void mvceditor::IndexingDialogClass::OnHideButton(wxCommandEvent &event) {
	Hide();
}

void mvceditor::IndexingDialogClass::Start() {
	Gauge->Pulse();
}

void mvceditor::IndexingDialogClass::Increment() {
	Gauge->Pulse();
}

BEGIN_EVENT_TABLE(mvceditor::ResourceFeatureClass, wxEvtHandler)
	EVT_MENU(mvceditor::MENU_RESOURCE + 0, mvceditor::ResourceFeatureClass::OnProjectWipeAndIndex)
	EVT_MENU(mvceditor::MENU_RESOURCE + 1, mvceditor::ResourceFeatureClass::OnJump)
	EVT_MENU(mvceditor::MENU_RESOURCE + 2, mvceditor::ResourceFeatureClass::OnSearchForResource)
	EVT_MENU(mvceditor::MENU_RESOURCE + 3, mvceditor::ResourceFeatureClass::OnJump)
	EVT_UPDATE_UI(wxID_ANY, mvceditor::ResourceFeatureClass::OnUpdateUi)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE, mvceditor::EVENT_WORK_COMPLETE, mvceditor::ResourceFeatureClass::OnWorkComplete)
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::ResourceFeatureClass::OnWorkInProgress)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_FILE_CLOSED, mvceditor::ResourceFeatureClass::OnAppFileClosed)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_READY, mvceditor::ResourceFeatureClass::OnAppReady)
	EVT_COMMAND(ID_WIPE_THREAD, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::ResourceFeatureClass::OnWorkInProgress)
	EVT_COMMAND(ID_WIPE_THREAD, mvceditor::EVENT_WIPE_COMPLETE, mvceditor::ResourceFeatureClass::OnWipeComplete)

	EVT_WORKING_CACHE_COMPLETE(wxID_ANY, mvceditor::ResourceFeatureClass::OnWorkingCacheComplete)
	EVT_TIMER(wxID_ANY, mvceditor::ResourceFeatureClass::OnTimer)
END_EVENT_TABLE()
