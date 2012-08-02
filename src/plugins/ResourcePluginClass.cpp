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
#include <plugins/ResourcePluginClass.h>
#include <windows/StringHelperClass.h>
#include <MvcEditorErrors.h>
#include <MvcEditorAssets.h>
#include <Events.h>
#include <MvcEditor.h>
#include <wx/artprov.h>
#include <wx/filename.h>
#include <wx/valgen.h>

static int ID_COUNT_FILES_GAUGE = wxNewId();

mvceditor::ResourceFileReaderClass::ResourceFileReaderClass(wxEvtHandler& handler, mvceditor::RunningThreadsClass& runningThreads) 
	: BackgroundFileReaderClass(handler, runningThreads)
	, PhpFileFilters() 
	, ResourceCache(NULL) 
	, ProjectQueue()
	, CurrentResourceDbFileName() {
}

bool mvceditor::ResourceFileReaderClass::InitProjectQueue(mvceditor::ResourceCacheClass* resourceCache, 
														const std::vector<mvceditor::ProjectClass>& projects) {
	while (!ProjectQueue.empty()) {
		ProjectQueue.pop();
	}
	ResourceCache = resourceCache;
	for (size_t i = 0; i < projects.size(); ++i) {
		if (projects[i].IsEnabled && !projects[i].AllPhpSources().empty()) {
			ProjectQueue.push(projects[i]);
		}
	}
	return !ProjectQueue.empty();
}

bool mvceditor::ResourceFileReaderClass::InitForFile(mvceditor::ResourceCacheClass* resourceCache, 
														const wxString& fullPath) {
	while (!ProjectQueue.empty()) {
		ProjectQueue.pop();
	}
	wxFileName fileName(fullPath);
	if (!fileName.FileExists()) {
		return false;
	}
	PhpFileFilters.clear();

	// break up name into dir + name, add name to file filters
	mvceditor::SourceClass src;
	src.RootDirectory.AssignDir(fullPath);
	src.SetIncludeWildcards(fileName.GetFullName());
	PhpFileFilters.push_back(fileName.GetFullName());

	std::vector<mvceditor::SourceClass> srcs;
	srcs.push_back(src);
	if (Init(srcs)) {
		ResourceCache = resourceCache;
		return true;
	}
	return false;
}

bool mvceditor::ResourceFileReaderClass::FileRead(mvceditor::DirectorySearchClass& search) {
	ResourceCache->WalkGlobal(CurrentResourceDbFileName.GetFullPath(), search, PhpFileFilters);
	return false;
}

bool mvceditor::ResourceFileReaderClass::FileMatch(const wxString& file) {
	bool matchedFilter = false;
	for (size_t i = 0; i < PhpFileFilters.size(); ++i) {
		wxString filter = PhpFileFilters[i];
		matchedFilter = !wxIsWild(filter) || wxMatchWild(filter, file);
		if (matchedFilter) {
			break;
		}
	}
	return matchedFilter;
}

bool mvceditor::ResourceFileReaderClass::MoreProjects() const {
	return !ProjectQueue.empty();
}

bool mvceditor::ResourceFileReaderClass::ReadNextProject() {
	bool next = false;
	if (!ProjectQueue.empty()) {
		mvceditor::ProjectClass project = ProjectQueue.front();
		ProjectQueue.pop();
		std::vector<mvceditor::SourceClass> sources = project.AllPhpSources();
		if (Init(sources)) {			
			PhpFileFilters = project.GetPhpFileExtensions();
			CurrentResourceDbFileName = project.ResourceDbFileName;
			if (!ResourceCache->IsInitGlobal(CurrentResourceDbFileName)) {
				ResourceCache->InitGlobal(CurrentResourceDbFileName);
			}
			next = true;
		}
	}
	return next;
}

mvceditor::NativeFunctionsFileReaderClass::NativeFunctionsFileReaderClass(wxEvtHandler& handler, mvceditor::RunningThreadsClass& runningThreads) 
	: ThreadWithHeartbeatClass(handler, runningThreads)
	, ResourceCache(NULL)
{

}

bool mvceditor::NativeFunctionsFileReaderClass::Init(mvceditor::ResourceCacheClass* resourceCache) {
	wxFileName nativeFile = mvceditor::NativeFunctionsAsset();
	bool good = false;
	if (nativeFile.FileExists()) {
		ResourceCache = resourceCache;
		wxThreadError err = CreateSingleInstance();
		if (err == wxTHREAD_NO_ERROR) {
			SignalStart();
			good = true;
		}
	}
	return good;
}

void mvceditor::NativeFunctionsFileReaderClass::Entry() {
	ResourceCache->BuildResourceCacheForNativeFunctionsGlobal();
	SignalEnd();
}

mvceditor::ResourcePluginClass::ResourcePluginClass(mvceditor::AppClass& app)
	: PluginClass(app)
	, ResourceFileReader(*this, RunningThreads)
	, NativeFunctionsReader(*this, RunningThreads)
	, JumpToText()
	, ProjectIndexMenu(NULL)
	, State(FREE) 
	, HasCodeLookups(false)
	, HasFileLookups(false) {
	IndexingDialog = NULL;
}

void mvceditor::ResourcePluginClass::AddSearchMenuItems(wxMenu* searchMenu) {
	ProjectIndexMenu = searchMenu->Append(mvceditor::MENU_RESOURCE + 0, _("Index"), _("Index the project"));
	searchMenu->Append(mvceditor::MENU_RESOURCE + 1, _("Jump To Resource Under Cursor"), _("Jump To Resource that is under the cursor"));
	searchMenu->Append(mvceditor::MENU_RESOURCE + 2, _("Search for Resource..."), _("Search for a class, method, or function"));
	ProjectIndexMenu->Enable(App.Structs.HasSources() && FREE == State);
}

void mvceditor::ResourcePluginClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_RESOURCE + 0] = wxT("Resource-Index Project");
	menuItemIds[mvceditor::MENU_RESOURCE + 1] = wxT("Resource-Jump To Resource Under Cursor");
	menuItemIds[mvceditor::MENU_RESOURCE + 2] = wxT("Resource-Search For Resource ...");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::ResourcePluginClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	toolBar->AddTool(mvceditor::MENU_RESOURCE + 0, wxT("Index"), wxArtProvider::GetBitmap(
		wxART_EXECUTABLE_FILE, wxART_TOOLBAR, wxSize(16, 16)), wxT("Index"), wxITEM_NORMAL);
}

void mvceditor::ResourcePluginClass::AddCodeControlClassContextMenuItems(wxMenu* menu) {
	menu->Append(mvceditor::MENU_RESOURCE + 3, _("Jump To Source"));
}

void mvceditor::ResourcePluginClass::OnProjectsUpdated(wxCommandEvent& event) {
	HasCodeLookups = false;
	HasFileLookups = false;
	if (ResourceFileReader.IsRunning()) {
		ResourceFileReader.StopReading();
	}
	GetResourceCache()->Clear();
	GetResourceCache()->SetVersion(GetEnvironment()->Php.Version);
	ProjectIndexMenu->Enable(App.Structs.HasSources() && FREE == State);
	GetStatusBarWithGauge()->AddGauge(_("Preparing Resource Index"), ID_COUNT_FILES_GAUGE, 
			StatusBarWithGaugeClass::INDETERMINATE_MODE, wxGA_HORIZONTAL);
	if (NativeFunctionsReader.Init(GetResourceCache())) {
		State = INDEXING_NATIVE_FUNCTIONS;		
	}
	else {
		GetStatusBarWithGauge()->StopGauge(ID_COUNT_FILES_GAUGE);
	}
}

void mvceditor::ResourcePluginClass::OnEnvironmentUpdated(wxCommandEvent& event) {
	GetResourceCache()->SetVersion(GetEnvironment()->Php.Version);
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourcePluginClass::SearchForResources(const wxString& text) {
	mvceditor::ResourceCacheClass* resourceCache = GetResourceCache();
	std::vector<mvceditor::ResourceClass> matches;

	if (!NeedToIndex(text)) {	

		// if we know the indexing has already taken place lets just do the lookup; it will be quick.
		matches = resourceCache->CollectNearMatchResourcesFromAll(mvceditor::StringHelperClass::wxToIcu(text));

		// no need to show jump to results for native functions
		RemoveNativeMatches(matches);
	}
	return matches;
}

void mvceditor::ResourcePluginClass::OnWorkInProgress(wxCommandEvent& event) {
	GetStatusBarWithGauge()->IncrementGauge(ID_COUNT_FILES_GAUGE, StatusBarWithGaugeClass::INDETERMINATE_MODE);
	if (IndexingDialog && IndexingDialog->IsShown()) {
		IndexingDialog->Increment();
	}
}

void mvceditor::ResourcePluginClass::OnWorkComplete(wxCommandEvent& event) {
	if (ResourceFileReader.MoreProjects()) {

		// if the project queue is not empty then start reading the next project.
		bool hasNext = false;
		while (ResourceFileReader.MoreProjects() && !hasNext) {
			hasNext = ResourceFileReader.ReadNextProject();
		}
		if (hasNext) {
			mvceditor::BackgroundFileReaderClass::StartError error = mvceditor::BackgroundFileReaderClass::NONE;
			ResourceFileReader.StartReading(error);
			wxASSERT(mvceditor::BackgroundFileReaderClass::NONE == error);
			return;
		}
	}
	GetStatusBarWithGauge()->StopGauge(ID_COUNT_FILES_GAUGE);
	if (IndexingDialog) {
		IndexingDialog->Destroy();
		IndexingDialog = NULL;
	}
	mvceditor::ResourceSearchClass resourceSearch(mvceditor::StringHelperClass::wxToIcu(JumpToText));

	if (INDEXING_NATIVE_FUNCTIONS == State) {
		mvceditor::BackgroundFileReaderClass::StartError error = mvceditor::BackgroundFileReaderClass::NONE;
		if (ResourceFileReader.StartReading(error)) {
			State = INDEXING_PROJECT;
			GetStatusBarWithGauge()->AddGauge(_("Indexing Projects"), ID_COUNT_FILES_GAUGE, 
				StatusBarWithGaugeClass::INDETERMINATE_MODE, wxGA_HORIZONTAL);
		}
		else if (mvceditor::BackgroundFileReaderClass::ALREADY_RUNNING == error) {
			wxMessageBox(_("Indexing is already taking place. Please wait."), wxT("Warning"), wxICON_EXCLAMATION);
			State = FREE;
		}
		else if (mvceditor::BackgroundFileReaderClass::NO_RESOURCES == error) {
			mvceditor::EditorLogError(mvceditor::LOW_RESOURCES);
			State = FREE;
		}
	}
	else if (INDEXING_PROJECT == State || GOTO == State) {

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
		else if (INDEXING_PROJECT == previousState) {
			wxCommandEvent indexedEvent(mvceditor::EVENT_APP_PROJECT_INDEXED);
			App.EventSink.Publish(indexedEvent);
		}
	}
}

void mvceditor::ResourcePluginClass::StartIndex() {
	if (App.Structs.HasSources()) {

		//prevent two finds at a time
		if (FREE == State) { 
			ResourceCacheClass* resourceCache = GetResourceCache();
			if (ResourceFileReader.InitProjectQueue(resourceCache, App.Structs.Projects) && ResourceFileReader.ReadNextProject()) {
					mvceditor::BackgroundFileReaderClass::StartError error = mvceditor::BackgroundFileReaderClass::NONE;
					if (ResourceFileReader.StartReading(error)) {
						State = INDEXING_PROJECT;
						GetStatusBarWithGauge()->AddGauge(_("Indexing Projects"),
							ID_COUNT_FILES_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE,
							wxGA_HORIZONTAL);
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
					else if (mvceditor::BackgroundFileReaderClass::ALREADY_RUNNING == error) {
						wxMessageBox(_("Indexing is already taking place. Please wait."), wxT("Warning"), wxICON_EXCLAMATION);
					}
					else if (mvceditor::BackgroundFileReaderClass::NO_RESOURCES == error) {
						mvceditor::EditorLogError(mvceditor::LOW_RESOURCES);
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

void mvceditor::ResourcePluginClass::OnProjectIndex(wxCommandEvent& event) {
	StartIndex();
}

void mvceditor::ResourcePluginClass::OnJump(wxCommandEvent& event) {

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

void mvceditor::ResourcePluginClass::OnSearchForResource(wxCommandEvent& event) {
	if (NeedToIndex(wxT("FakeClass"))) {
		JumpToText = wxT("");
		StartIndex();

		// set the state here so that when indexing is done we know to
		// open the search dialog.
		State = GOTO;
		return;
	}
	std::vector<mvceditor::ResourceClass> chosenResources;
	wxString term;
	mvceditor::ResourceSearchDialogClass dialog(GetMainWindow(), *this, term, chosenResources);
	if (dialog.ShowModal() == wxOK) {
		for (size_t i = 0; i < chosenResources.size(); ++i) {
			LoadPageFromResource(term, chosenResources[i]);
		}
	}
}


void mvceditor::ResourcePluginClass::LoadPageFromResource(const wxString& finderQuery, const mvceditor::ResourceClass& resource) {
	mvceditor::ResourceSearchClass resourceSearch(mvceditor::StringHelperClass::wxToIcu(finderQuery));
	GetNotebook()->LoadPage(resource.FullPath.GetFullPath());
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

void mvceditor::ResourcePluginClass::OnUpdateUi(wxUpdateUIEvent& event) {
	ProjectIndexMenu->Enable(App.Structs.HasSources() && FREE == State);
	event.Skip();
}

bool mvceditor::ResourcePluginClass::NeedToIndex(const wxString& finderQuery) const {
	mvceditor::ResourceSearchClass resourceSearch(mvceditor::StringHelperClass::wxToIcu(finderQuery));
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

void mvceditor::ResourcePluginClass::OpenFile(wxString fileName) {
	GetNotebook()->LoadPage(fileName);
}

void mvceditor::ResourcePluginClass::OnCmdReIndex(wxCommandEvent& event) {

	// only index when there is a project open
	if (App.Structs.HasSources()) {
		StartIndex();
	}
}


void mvceditor::ResourcePluginClass::OnAppFileClosed(wxCommandEvent& event) {

	// only index when there is a project open
	// need to make sure that the file that was closed is in the opened project
	// as well. don't want single-leaf files to be parsed for resources .. or
	// do we?
	wxString fileName = event.GetString();
	if (!App.Structs.IsAPhpSourceFile(fileName)) {
		ResourceFileReader.InitForFile(GetResourceCache(), fileName);
		mvceditor::BackgroundFileReaderClass::StartError error;

		// show user the error? not for now as they cannot do anything about it
		ResourceFileReader.StartReading(error);
	}
}

void mvceditor::ResourcePluginClass::RemoveNativeMatches(std::vector<mvceditor::ResourceClass>& matches) const {
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

mvceditor::ResourceSearchDialogClass::ResourceSearchDialogClass(wxWindow* parent, ResourcePluginClass& resource,
																wxString& term, 
																std::vector<mvceditor::ResourceClass>& chosenResources)
	: ResourceSearchDialogGeneratedClass(parent)
	, ResourcePlugin(resource)
	, ChosenResources(chosenResources) 
	, MatchedResources() {
	wxGenericValidator termValidator(&term);
	TransferDataToWindow();
}

void mvceditor::ResourceSearchDialogClass::OnSearchText(wxCommandEvent& event) {
	wxString text = SearchText->GetValue();
	if (text.Length() > 3) {
		if (!ResourcePlugin.NeedToIndex(text)) {
			MatchedResources = ResourcePlugin.SearchForResources(text);
			if (!MatchedResources.empty()) {
				Results->Freeze();
				ShowJumpToResults(text, MatchedResources);
				Results->Thaw();
			}
			else {
				Results->Clear();
				ResultsLabel->SetLabel(_("No matches found"));
			}
		}
	}	
}

void mvceditor::ResourceSearchDialogClass::OnSearchEnter(wxCommandEvent& event) {
	if (MatchedResources.size() == 1) {
		TransferDataFromWindow();
		ChosenResources.clear();
		ChosenResources.push_back(MatchedResources[0]);
		EndModal(wxOK);
	}
}

void mvceditor::ResourceSearchDialogClass::ShowJumpToResults(const wxString& finderQuery, const std::vector<mvceditor::ResourceClass>& allMatches) {
	wxArrayString files;
	mvceditor::ResourceSearchClass resourceSearch(mvceditor::StringHelperClass::wxToIcu(finderQuery));
	for (size_t i = 0; i < allMatches.size(); ++i) {
		files.Add(allMatches[i].FullPath.GetFullPath());
	}
	Results->Clear();
	
	// dont show the project path to the user
	for (size_t i = 0; i < files.GetCount(); ++i) {
		wxString shortName = ResourcePlugin.App.Structs.RelativeFileName(files[i]);
		if (mvceditor::ResourceSearchClass::FILE_NAME != resourceSearch.GetResourceType() &&
			mvceditor::ResourceSearchClass::FILE_NAME_LINE_NUMBER != resourceSearch.GetResourceType()) {
			UnicodeString res = allMatches[i].ClassName + UNICODE_STRING_SIMPLE("::") + allMatches[i].Identifier;
			shortName += wxT("  (") + mvceditor::StringHelperClass::IcuToWx(res) + wxT(")");
		}
		Results->Append(shortName);
	}
	ResultsLabel->SetLabel(wxString::Format(_("Found %d files. Please choose file(s) to open."), allMatches.size()));
}

void mvceditor::ResourceSearchDialogClass::OnOkButton(wxCommandEvent& event) {
	TransferDataFromWindow();
	ChosenResources.clear();
	for (size_t i = 0; i < Results->GetCount(); ++i) {
		if (Results->IsChecked(i)) {
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
		"::print (would match all methods in all classes that start with 'print' )"
	);
	help = wxGetTranslation(help);
	wxMessageBox(help, _("Help"), wxOK);	
}

void mvceditor::ResourceSearchDialogClass::OnSearchKeyDown(wxKeyEvent& event) {
	int keyCode = event.GetKeyCode();
	size_t selection = Results->GetSelection();
	if (keyCode == WXK_DOWN) {		
		if (!Results->IsEmpty() && selection >= 0 && selection < (Results->GetCount() - 1)) {
			Results->SetSelection(selection + 1);
		}
		else if (!Results->IsEmpty() && selection >= 0) {

			// cycle back to the beginning
			Results->SetSelection(0);
		}
		SearchText->SetFocus();
	}
	else if (keyCode == WXK_UP) {
		if (!Results->IsEmpty() && selection > 0 && selection < Results->GetCount()) {
			Results->SetSelection(selection - 1);
		}
		else if (!Results->IsEmpty() && selection == 0) {

			// cycle back to the end
			Results->SetSelection(Results->GetCount() - 1);
		}
		SearchText->SetFocus();
	}
	else {
		event.Skip();
	}
}

void mvceditor::ResourceSearchDialogClass::OnResultsDoubleClick(wxCommandEvent& event) {
	TransferDataFromWindow();
	ChosenResources.clear();
	size_t selection = event.GetSelection();
	if (selection >= 0 && selection < Results->GetCount()) {
		ChosenResources.push_back(MatchedResources[selection]);
	}
	if (ChosenResources.empty()) {
		return;
	}
	EndModal(wxOK);
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

BEGIN_EVENT_TABLE(mvceditor::ResourcePluginClass, wxEvtHandler)
	EVT_MENU(mvceditor::MENU_RESOURCE + 0, mvceditor::ResourcePluginClass::OnProjectIndex)
	EVT_MENU(mvceditor::MENU_RESOURCE + 1, mvceditor::ResourcePluginClass::OnJump)
	EVT_MENU(mvceditor::MENU_RESOURCE + 2, mvceditor::ResourcePluginClass::OnSearchForResource)
	EVT_MENU(mvceditor::MENU_RESOURCE + 3, mvceditor::ResourcePluginClass::OnJump)
	EVT_UPDATE_UI(wxID_ANY, mvceditor::ResourcePluginClass::OnUpdateUi)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_FILE_READ_COMPLETE, mvceditor::ResourcePluginClass::OnWorkComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_COMPLETE, mvceditor::ResourcePluginClass::OnWorkComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::ResourcePluginClass::OnWorkInProgress)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_PROJECTS_UPDATED, mvceditor::ResourcePluginClass::OnProjectsUpdated)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_CMD_RE_INDEX, mvceditor::ResourcePluginClass::OnCmdReIndex)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_FILE_CLOSED, mvceditor::ResourcePluginClass::OnAppFileClosed)
END_EVENT_TABLE()
