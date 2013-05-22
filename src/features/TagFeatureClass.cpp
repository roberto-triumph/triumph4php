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
#include <features/TagFeatureClass.h>
#include <globals/String.h>
#include <globals/Errors.h>
#include <globals/Assets.h>
#include <globals/Events.h>
#include <actions/TagWipeActionClass.h>
#include <globals/TagList.h>
#include <MvcEditor.h>
#include <wx/artprov.h>
#include <wx/filename.h>
#include <wx/valgen.h>

static int ID_EVENT_TAG_CACHE_SEARCH = wxNewId();
static int ID_SEARCH_TIMER = wxNewId();

mvceditor::TagFeatureClass::TagFeatureClass(mvceditor::AppClass& app)
	: FeatureClass(app)
	, JumpToText()
	, ProjectIndexMenu(NULL)
	, CacheState(CACHE_STALE) {
	IndexingDialog = NULL;
}

void mvceditor::TagFeatureClass::AddSearchMenuItems(wxMenu* searchMenu) {
	ProjectIndexMenu = searchMenu->Append(mvceditor::MENU_RESOURCE + 0, _("Index"), _("Index the project"));
	searchMenu->Append(mvceditor::MENU_RESOURCE + 1, _("Jump To Resource Under Cursor\tF12"), _("Jump To Resource that is under the cursor"));
	searchMenu->Append(mvceditor::MENU_RESOURCE + 2, _("Search for Resource...\tCTRL+R"), _("Search for a class, method, or function"));
	ProjectIndexMenu->Enable(App.Globals.HasSources());
}

void mvceditor::TagFeatureClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_RESOURCE + 0] = wxT("Resource-Index Project");
	menuItemIds[mvceditor::MENU_RESOURCE + 1] = wxT("Resource-Jump To Resource Under Cursor");
	menuItemIds[mvceditor::MENU_RESOURCE + 2] = wxT("Resource-Search For Resource ...");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::TagFeatureClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	wxBitmap bmp = mvceditor::IconImageAsset(wxT("tag-projects"));
	toolBar->AddTool(mvceditor::MENU_RESOURCE + 0, wxT("Tag Projects"), bmp, _("Create tags for the enabled projects"), wxITEM_NORMAL);
}

void mvceditor::TagFeatureClass::AddCodeControlClassContextMenuItems(wxMenu* menu) {
	menu->Append(mvceditor::MENU_RESOURCE + 3, _("Jump To Source"));
}

void mvceditor::TagFeatureClass::OnAppStartSequenceComplete(wxCommandEvent& event) {
	CacheState = CACHE_OK;
	if (IndexingDialog) {
		IndexingDialog->Destroy();
		IndexingDialog = NULL;
	}
}

void mvceditor::TagFeatureClass::OnProjectWipeAndIndex(wxCommandEvent& event) {
	if (App.Sequences.TagCacheWipeAndIndex()) {
		IndexingDialog = new mvceditor::GaugeDialogClass(GetMainWindow(), _("Indexing"), _("Wiping and Rebuilding Index"));
		IndexingDialog->Show();
		IndexingDialog->Start();
	}
	else {
		mvceditor::EditorLogWarning(mvceditor::WARNING_OTHER, _("Please wait until the running background task ends."));
	}
}

void mvceditor::TagFeatureClass::OnJump(wxCommandEvent& event) {

	// jump to selected resources
	CodeControlClass* codeControl = GetCurrentCodeControl();
	if (codeControl) {

		// if the cursor is in the middle of an identifier, find the end of the
		// current identifier; that way we can know the full name of the tag we want
		// to get
		int currentPos = codeControl->GetCurrentPos();
		int startPos = codeControl->WordStartPosition(currentPos, true);
		int endPos = codeControl->WordEndPosition(currentPos, true);
		wxString term = codeControl->GetTextRange(startPos, endPos);
	
		std::vector<mvceditor::TagClass> matches = codeControl->GetCurrentSymbolResource();
		mvceditor::TagListRemoveNativeMatches(matches);
		if (!matches.empty()) {
			UnicodeString res = matches[0].ClassName + UNICODE_STRING_SIMPLE("::") + matches[0].Identifier;
			if (matches.size() == 1) {
				LoadPageFromResource(term, matches[0]);
			}
			else {

				// if we have more than one match, lets pick the match from the same project
				// that the currently opened file is in.
				wxFileName openedFile = codeControl->GetFileName();
				int tagProjectMatchCount = 0;
				mvceditor::TagClass tagProjectMatch;
				if (openedFile.IsOk() && !codeControl->IsNew()) {
					std::vector<mvceditor::ProjectClass>::const_iterator project;
					std::vector<mvceditor::TagClass>::const_iterator tag;
					for (tag = matches.begin(); tag != matches.end(); ++tag) {
						for (project = App.Globals.Projects.begin(); project != App.Globals.Projects.end(); ++project) {
							if (project->IsAPhpSourceFile(openedFile.GetFullPath()) && project->IsAPhpSourceFile(tag->FullPath)) {
								tagProjectMatch = *tag;
								tagProjectMatchCount++;
							}
						}
					}
				}
				if (tagProjectMatchCount == 1) {
					LoadPageFromResource(term, tagProjectMatch);
				}
				else {
					std::vector<mvceditor::TagClass> chosenResources;
					mvceditor::TagSearchDialogClass dialog(GetMainWindow(), *this, term, chosenResources);
					dialog.Prepopulate(term, matches);
					if (dialog.ShowModal() == wxOK) {
						for (size_t i = 0; i < chosenResources.size(); ++i) {
							LoadPageFromResource(JumpToText, chosenResources[i]);
						}
					}
				}
			}
		}
	}
}

void mvceditor::TagFeatureClass::OnSearchForResource(wxCommandEvent& event) {
	std::vector<mvceditor::TagClass> chosenResources;
	wxString term;
	mvceditor::TagSearchDialogClass dialog(GetMainWindow(), *this, term, chosenResources);
	if (dialog.ShowModal() == wxOK) {
		for (size_t i = 0; i < chosenResources.size(); ++i) {
			LoadPageFromResource(term, chosenResources[i]);
		}
	}
}


void mvceditor::TagFeatureClass::LoadPageFromResource(const wxString& finderQuery, const mvceditor::TagClass& tag) {
	mvceditor::TagSearchClass tagSearch(mvceditor::WxToIcu(finderQuery));
	wxFileName fileName = tag.FileName();
	if (!fileName.FileExists()) {
		mvceditor::EditorLogWarning(mvceditor::WARNING_OTHER, _("File Not Found:") + fileName.GetFullPath());
		return;
	}
	GetNotebook()->LoadPage(tag.GetFullPath());
	CodeControlClass* codeControl = GetCurrentCodeControl();
	if (codeControl) {
		int32_t position, 
			length;
		bool found = mvceditor::ParsedTagFinderClass::GetResourceMatchPosition(tag, codeControl->GetSafeText(), position, length);
		if (mvceditor::TagSearchClass::FILE_NAME_LINE_NUMBER == tagSearch.GetResourceType()) {
				
			// scintilla line numbers start at zero. use the ensure method so that the line is shown in the 
			// center of the screen
			int pos = codeControl->PositionFromLine(tagSearch.GetLineNumber() - 1);
			codeControl->SetSelectionAndEnsureVisible(pos, pos);
			codeControl->GotoLine(tagSearch.GetLineNumber() - 1);
		}
		if (mvceditor::TagSearchClass::FILE_NAME == tagSearch.GetResourceType()) {
				
			// nothing; just open the file but don't scroll down to any place
		}
		else if (found) {
			codeControl->SetSelectionAndEnsureVisible(position, position + length);
		}
	}
}

void mvceditor::TagFeatureClass::OnUpdateUi(wxUpdateUIEvent& event) {
	ProjectIndexMenu->Enable(App.Globals.HasSources());
	event.Skip();
}

void mvceditor::TagFeatureClass::OpenFile(wxString fileName) {
	GetNotebook()->LoadPage(fileName);
}

void mvceditor::TagFeatureClass::OnAppFileClosed(wxCommandEvent& event) {

	// only index when there is a project open
	// need to make sure that the file that was closed is in the opened project
	// as well.
	// ATTN: don't want single-leaf files to be parsed for resources .. or
	// do we?
	wxString fileName = event.GetString();
	std::vector<mvceditor::ProjectClass>::const_iterator project;
	bool isFileFromProject = false;
	for (project = App.Globals.Projects.begin(); project != App.Globals.Projects.end(); ++project) {

		if (project->IsEnabled && project->IsAPhpSourceFile(fileName)) {
			isFileFromProject = true;
			break;
		}
	}
	if (isFileFromProject) {
		mvceditor::ProjectTagActionClass* tagAction = new mvceditor::ProjectTagActionClass(App.RunningThreads, mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE);
		tagAction->InitForFile(App.Globals, fileName);
		App.RunningThreads.Queue(tagAction);
	}

	// Notebook class assigns unique IDs to CodeControlClass objects
	// the event ID is the ID of the code control that was closed
	// this needs to be the same as mvceditor::CodeControlClass::GetIdString
	wxString idString = wxString::Format(wxT("File_%d"), event.GetId());
	App.Globals.TagCache.RemoveWorking(idString);
	
	wxString fileToDelete = fileName;
	if (fileToDelete.IsEmpty()) {
		fileToDelete = idString;
	}
}

wxString mvceditor::TagFeatureClass::CacheStatus() {
	if (CACHE_OK == CacheState) {
		return _("OK");
	}
	return _("Stale");
}

void mvceditor::TagFeatureClass::OnWorkingCacheComplete(mvceditor::WorkingCacheCompleteEventClass& event) {
	wxString fileIdentifier = event.GetFileIdentifier();
	bool good = App.Globals.TagCache.RegisterWorking(fileIdentifier, event.WorkingCache);
	if (!good) {

		// already there
		App.Globals.TagCache.ReplaceWorking(fileIdentifier, event.WorkingCache);
	}
	event.Skip();
}

void mvceditor::TagFeatureClass::OnAppFileSaved(mvceditor::FileSavedEventClass& event) {
	mvceditor::CodeControlClass* codeControl = GetCurrentCodeControl();
	if (codeControl && codeControl->GetDocumentMode() == mvceditor::CodeControlClass::PHP) {

		// persist the resources to the "global" cache
		// this is needed so that if the url detector is triggered while 
		// a file is opened the url detector gets the latest resources
		// do this before doing the working cache so that the global cache is up-to-date
		// when the working cache finis5hes
		wxString fileName = event.GetCodeControl()->GetFileName();
		std::vector<mvceditor::ProjectClass>::const_iterator project;
		
		mvceditor::ProjectTagActionClass* tagAction = new mvceditor::ProjectTagActionClass(App.RunningThreads, mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE);
		tagAction->InitForFile(App.Globals, fileName);
		App.RunningThreads.Queue(tagAction);

		UnicodeString text = codeControl->GetSafeText();

		// we need to differentiate between new and opened files (the 'true' arg)
		mvceditor::WorkingCacheBuilderClass* builder = new mvceditor::WorkingCacheBuilderClass(App.RunningThreads, wxID_ANY);
		builder->Update(
			App.Globals,
			codeControl->GetFileName(),
			codeControl->GetIdString(),
			text, 
			codeControl->IsNew(),
			App.Globals.Environment.Php.Version);
		App.RunningThreads.Queue(builder);
	}	
	event.Skip();
}

void mvceditor::TagFeatureClass::OnAppFileOpened(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl = GetCurrentCodeControl();
	if (codeControl && codeControl->GetDocumentMode() == mvceditor::CodeControlClass::PHP) {
		UnicodeString text = codeControl->GetSafeText();

		// we need to differentiate between new and opened files (the 'true' arg)
		mvceditor::WorkingCacheBuilderClass* builder = new mvceditor::WorkingCacheBuilderClass(App.RunningThreads, wxID_ANY);
		builder->Update(
			App.Globals,
			codeControl->GetFileName(),
			codeControl->GetIdString(),
			text, 
			codeControl->IsNew(),
			App.Globals.Environment.Php.Version);
		App.RunningThreads.Queue(builder);
	}
	event.Skip();
}

mvceditor::TagSearchDialogClass::TagSearchDialogClass(wxWindow* parent, 
																TagFeatureClass& tag,
																wxString& term,
																std::vector<mvceditor::TagClass>& chosenResources)
	: TagSearchDialogGeneratedClass(parent)
	, ResourceFeature(tag)
	, ChosenResources(chosenResources) 
	, MatchedResources() 
	, Timer(this, ID_SEARCH_TIMER)
	, LastInput(term)
	, ActionId() {
	wxGenericValidator termValidator(&term);
	TransferDataToWindow();
	CacheStatusLabel->SetLabel(wxT("Cache Status: ") + tag.CacheStatus());
	SearchText->SetFocus();

	ProjectChoice->Append(_("All Enabled Projects"), (void*)NULL);
	for (size_t i = 0; i < tag.App.Globals.Projects.size(); ++i) {
		if (tag.App.Globals.Projects[i].IsEnabled) {

			// should be ok to reference this vector since it wont change because this is a 
			// modal dialog
			ProjectChoice->Append(tag.App.Globals.Projects[i].Label, &tag.App.Globals.Projects[i]);
		}
	}
	ProjectChoice->Select(0);

	// so that the thread can give the matching tags back to us
	ResourceFeature.App.RunningThreads.AddEventHandler(this);
}

mvceditor::TagSearchDialogClass::~TagSearchDialogClass() {
	ResourceFeature.App.RunningThreads.RemoveEventHandler(this);
}

void mvceditor::TagSearchDialogClass::OnSearchText(wxCommandEvent& event) {
	if (Timer.IsRunning()) {
		if (0 != ActionId) {
			ResourceFeature.App.RunningThreads.CancelAction(ActionId);
		}
		ActionId = 0;
		return;
	}
	Timer.Start(300, wxTIMER_ONE_SHOT);
}

void mvceditor::TagSearchDialogClass::OnTimerComplete(wxTimerEvent& event) {
	wxString text = SearchText->GetValue();
	std::vector<mvceditor::ProjectClass*> projects;
	bool showAllProjects = ProjectChoice->GetSelection() == 0;
	if (!showAllProjects) {
		projects.push_back((mvceditor::ProjectClass*)ProjectChoice->GetClientData(ProjectChoice->GetSelection()));
	}
	else {

		// the first item in the wxChoice will not have client data; the "all" option
		for (size_t i = 1; i < ProjectChoice->GetCount(); ++i) {
			projects.push_back((mvceditor::ProjectClass*) ProjectChoice->GetClientData(i));
		}
	}
	LastInput = text;
	SearchForResources(text, projects);
}

void mvceditor::TagSearchDialogClass::OnSearchEnter(wxCommandEvent& event) {
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

void mvceditor::TagSearchDialogClass::ShowJumpToResults(const wxString& finderQuery, const std::vector<mvceditor::TagClass>& allMatches) {
	wxArrayString files;
	for (size_t i = 0; i < allMatches.size(); ++i) {
		files.Add(allMatches[i].GetFullPath());
	}
	MatchesList->Clear();
	bool showAllProjects = ProjectChoice->GetSelection() == 0;
	mvceditor::ProjectClass* selectedProject = NULL;
	if (!showAllProjects) {
		selectedProject = (mvceditor::ProjectClass*)ProjectChoice->GetClientData(ProjectChoice->GetSelection());
	}
	
	// dont show the project path to the user
	for (size_t i = 0; i < files.GetCount(); ++i) {
		wxString projectLabel;
		wxString relativeName;
		if (showAllProjects) {
			relativeName = ResourceFeature.App.Globals.RelativeFileName(files[i], projectLabel);
		}
		else {
			relativeName = selectedProject->RelativeFileName(files[i]);
			projectLabel = selectedProject->Label;
		}
		wxString matchLabel;
		mvceditor::TagClass match = allMatches[i];
		if (mvceditor::TagClass::MEMBER == match.Type || mvceditor::TagClass::METHOD == match.Type ||
			mvceditor::TagClass::CLASS_CONSTANT == match.Type) {
			matchLabel += mvceditor::IcuToWx(match.ClassName);
			matchLabel += wxT("::");
			matchLabel += mvceditor::IcuToWx(match.Identifier);
		}
		else if (mvceditor::TagClass::CLASS == match.Type || mvceditor::TagClass::FUNCTION == match.Type
			|| mvceditor::TagClass::DEFINE == match.Type) {
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
	if (!MatchesList->IsEmpty()) {
		MatchesList->Select(0);
	}
	MatchesLabel->SetLabel(wxString::Format(_("Found %d files for %s. Please choose file(s) to open."), 
		allMatches.size(), (const char*)finderQuery.c_str()));
}

void mvceditor::TagSearchDialogClass::OnOkButton(wxCommandEvent& event) {
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

void mvceditor::TagSearchDialogClass::OnCancelButton(wxCommandEvent& event) {
	EndModal(wxCANCEL);
}

void mvceditor::TagSearchDialogClass::Prepopulate(const wxString& term, const std::vector<mvceditor::TagClass> &matches) {
	MatchedResources = matches;
	SearchText->SetValue(term);
	ShowJumpToResults(term, MatchedResources);
}
	
void mvceditor::TagSearchDialogClass::OnHelpButton(wxCommandEvent& event) {
	
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
		"The tag cache will be stale when the application is first opened and\n"
		"will be OK after you have indexed the projects. "
	);
	help = wxGetTranslation(help);
	wxMessageBox(help, _("Resource Search Help"), wxOK, this);	
}

void mvceditor::TagSearchDialogClass::OnSearchKeyDown(wxKeyEvent& event) {
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

void mvceditor::TagSearchDialogClass::OnMatchesListDoubleClick(wxCommandEvent& event) {
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

void mvceditor::TagSearchDialogClass::OnMatchesListKeyDown(wxKeyEvent& event) {
	if (event.GetKeyCode() == WXK_RETURN) {
		wxCommandEvent cmdEvt;
		OnSearchEnter(cmdEvt);
	}
	else {
		event.Skip();
	}
}

void mvceditor::TagSearchDialogClass::OnProjectChoice(wxCommandEvent& event) {
	OnSearchText(event);
}

void mvceditor::TagSearchDialogClass::SearchForResources(const wxString& text, std::vector<mvceditor::ProjectClass*> projects) {
	mvceditor::TagCacheSearchActionClass* action =  new
		mvceditor::TagCacheSearchActionClass(ResourceFeature.App.RunningThreads, ID_EVENT_TAG_CACHE_SEARCH);
	std::vector<wxFileName> dirs;
	for (std::vector<mvceditor::ProjectClass*>::const_iterator p = projects.begin(); p != projects.end(); ++p) {
		std::vector<mvceditor::SourceClass>::const_iterator src;
		for (src = (*p)->Sources.begin(); src != (*p)->Sources.end(); ++src) {
			dirs.push_back(src->RootDirectory);
		}
	}
	action->SetSearch(ResourceFeature.App.Globals, text, dirs);
	ActionId = ResourceFeature.App.RunningThreads.Queue(action);
}

void mvceditor::TagSearchDialogClass::OnTagCacheSearchComplete(mvceditor::TagCacheSearchCompleteEventClass &event) {
	MatchedResources = event.Tags;
	if (LastInput != mvceditor::IcuToWx(event.SearchString)) {

		// only show results from the last query
		return;
	}
	if (!MatchedResources.empty()) {
			MatchesList->Freeze();
			ShowJumpToResults(mvceditor::IcuToWx(event.SearchString), MatchedResources);
			MatchesList->Thaw();
		}
		else {
			MatchesList->Clear();
			MatchesLabel->SetLabel(wxString::Format(_("No matches found for %s"), (const char*)LastInput.c_str()));
		}
}

BEGIN_EVENT_TABLE(mvceditor::TagFeatureClass, wxEvtHandler)
	EVT_MENU(mvceditor::MENU_RESOURCE + 0, mvceditor::TagFeatureClass::OnProjectWipeAndIndex)
	EVT_MENU(mvceditor::MENU_RESOURCE + 1, mvceditor::TagFeatureClass::OnJump)
	EVT_MENU(mvceditor::MENU_RESOURCE + 2, mvceditor::TagFeatureClass::OnSearchForResource)
	EVT_MENU(mvceditor::MENU_RESOURCE + 3, mvceditor::TagFeatureClass::OnJump)
	EVT_UPDATE_UI(wxID_ANY, mvceditor::TagFeatureClass::OnUpdateUi)

	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_FILE_CLOSED, mvceditor::TagFeatureClass::OnAppFileClosed)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_FILE_OPENED, mvceditor::TagFeatureClass::OnAppFileOpened)

	// we will treat file new and file opened the same
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_FILE_NEW, mvceditor::TagFeatureClass::OnAppFileOpened)
	EVT_FEATURE_FILE_SAVED(mvceditor::TagFeatureClass::OnAppFileSaved)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_SEQUENCE_COMPLETE, mvceditor::TagFeatureClass::OnAppStartSequenceComplete)

	EVT_WORKING_CACHE_COMPLETE(wxID_ANY, mvceditor::TagFeatureClass::OnWorkingCacheComplete)
END_EVENT_TABLE()


BEGIN_EVENT_TABLE(mvceditor::TagSearchDialogClass, TagSearchDialogGeneratedClass)
	EVENT_TAG_CACHE_SEARCH_COMPLETE(ID_EVENT_TAG_CACHE_SEARCH, mvceditor::TagSearchDialogClass::OnTagCacheSearchComplete)
	EVT_TIMER(ID_SEARCH_TIMER, mvceditor::TagSearchDialogClass::OnTimerComplete)
END_EVENT_TABLE()