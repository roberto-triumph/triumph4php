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
#include <globals/Number.h>
#include <actions/TagWipeActionClass.h>
#include <globals/TagList.h>
#include <Triumph.h>
#include <wx/artprov.h>
#include <wx/filename.h>
#include <wx/valgen.h>

static int ID_EVENT_TAG_CACHE_SEARCH = wxNewId();
static int ID_SEARCH_TIMER = wxNewId();
static int ID_REPARSE_TIMER = wxNewId();
static int ID_WORKING_CACHE = wxNewId();

t4p::TagFeatureClass::TagFeatureClass(t4p::AppClass& app)
	: FeatureClass(app)
	, Timer(this, ID_REPARSE_TIMER)
	, JumpToText()
	, ProjectIndexMenu(NULL)
	, CacheState(CACHE_STALE) {
	IndexingDialog = NULL;
}

void t4p::TagFeatureClass::AddSearchMenuItems(wxMenu* searchMenu) {
	ProjectIndexMenu = searchMenu->Append(t4p::MENU_RESOURCE + 0, _("Index"), _("Index the project"));
	searchMenu->Append(t4p::MENU_RESOURCE + 1, _("Jump To Resource Under Cursor\tF12"), _("Jump To Resource that is under the cursor"));
	ProjectIndexMenu->Enable(App.Globals.HasSources());
}

void t4p::TagFeatureClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[t4p::MENU_RESOURCE + 0] = wxT("Resource-Index Project");
	menuItemIds[t4p::MENU_RESOURCE + 1] = wxT("Resource-Jump To Resource Under Cursor");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void t4p::TagFeatureClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	wxBitmap bmp = t4p::BitmapImageAsset(wxT("tag-projects"));
	toolBar->AddTool(t4p::MENU_RESOURCE + 0, wxT("Tag Projects"), bmp, _("Create tags for the enabled projects"), wxITEM_NORMAL);
}

void t4p::TagFeatureClass::AddCodeControlClassContextMenuItems(wxMenu* menu) {
	menu->Append(t4p::MENU_RESOURCE + 3, _("Jump To Source"));
}

void t4p::TagFeatureClass::OnAppStartSequenceComplete(wxCommandEvent& event) {
	CacheState = CACHE_OK;
	if (IndexingDialog) {
		IndexingDialog->Destroy();
		IndexingDialog = NULL;
	}
	Timer.Start(500, wxTIMER_CONTINUOUS);
}

void t4p::TagFeatureClass::OnAppExit(wxCommandEvent& event) {
	Timer.Stop();
}

void t4p::TagFeatureClass::OnProjectWipeAndIndex(wxCommandEvent& event) {

	// stop the working cache timer because we are going to delete the rows from 
	// the database
	// we want to avoid locking the database
	Timer.Stop();
	if (App.Sequences.TagCacheWipeAndIndex(App.Globals.AllEnabledProjects())) {
		IndexingDialog = new t4p::GaugeDialogClass(GetMainWindow(), _("Indexing"), _("Wiping and Rebuilding Index"));
		IndexingDialog->Show();
		IndexingDialog->Start();
	}
	else {
		t4p::EditorLogWarningFix(_("Could not start wipe."), _("Please wait until the running background task ends."));
	}
}

void t4p::TagFeatureClass::OnJump(wxCommandEvent& event) {

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
	
		wxString matchError;
		std::vector<t4p::TagClass> matches = App.Globals.TagCache.GetTagsAtPosition(
			codeControl->GetIdString(), codeControl->GetSafeText(), endPos, 
			App.Globals.AllEnabledSourceDirectories(),
			App.Globals,
			matchError
		);
		if (!matchError.empty()) {
			GetStatusBarWithGauge()->SetColumn0Text(matchError);
		}
		
		t4p::TagListRemoveNativeMatches(matches);
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
				t4p::TagClass tagProjectMatch;
				if (openedFile.IsOk() && !codeControl->IsNew()) {
					std::vector<t4p::ProjectClass>::const_iterator project;
					std::vector<t4p::TagClass>::const_iterator tag;
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
					std::vector<t4p::TagClass> chosenResources;
					t4p::TagSearchDialogClass dialog(GetMainWindow(), App.Globals, CacheStatus(), term, chosenResources);
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

void t4p::TagFeatureClass::OnSearchForResource(wxCommandEvent& event) {
	std::vector<t4p::TagClass> chosenResources;
	wxString term;
	t4p::TagSearchDialogClass dialog(GetMainWindow(), App.Globals, CacheStatus(), term, chosenResources);
	if (dialog.ShowModal() == wxOK) {
		for (size_t i = 0; i < chosenResources.size(); ++i) {
			LoadPageFromResource(term, chosenResources[i]);
		}
	}
}


void t4p::TagFeatureClass::LoadPageFromResource(const wxString& finderQuery, const t4p::TagClass& tag) {
	t4p::TagSearchClass tagSearch(t4p::WxToIcu(finderQuery));
	wxFileName fileName = tag.FileName();
	if (!fileName.FileExists()) {
		t4p::EditorLogWarning(t4p::ERR_INVALID_FILE, fileName.GetFullPath());
		return;
	}
	GetNotebook()->LoadPage(tag.GetFullPath());
	CodeControlClass* codeControl = GetCurrentCodeControl();
	if (codeControl) {
		int32_t position, 
			length;
		bool found = t4p::ParsedTagFinderClass::GetResourceMatchPosition(tag, codeControl->GetSafeText(), position, length);
		if (t4p::TagSearchClass::FILE_NAME_LINE_NUMBER == tagSearch.GetResourceType()) {
				
			// scintilla line numbers start at zero. use the ensure method so that the line is shown in the 
			// center of the screen
			int pos = codeControl->PositionFromLine(tagSearch.GetLineNumber() - 1);
			codeControl->SetSelectionAndEnsureVisible(pos, pos);
			codeControl->GotoLineAndEnsureVisible(tagSearch.GetLineNumber());
		}
		if (t4p::TagSearchClass::FILE_NAME == tagSearch.GetResourceType()) {
				
			// nothing; just open the file but don't scroll down to any place
		}
		else if (found) {
			codeControl->SetSelectionAndEnsureVisible(position, position + length);
		}
	}
}

void t4p::TagFeatureClass::OnUpdateUi(wxUpdateUIEvent& event) {
	ProjectIndexMenu->Enable(App.Globals.HasSources());
	event.Skip();
}

void t4p::TagFeatureClass::OpenFile(wxString fileName) {
	GetNotebook()->LoadPage(fileName);
}

void t4p::TagFeatureClass::OnAppFileClosed(t4p::CodeControlEventClass& event) {

	// only index when there is a project open
	// need to make sure that the file that was closed is in the opened project
	// as well.
	// ATTN: don't want single-leaf files to be parsed for resources .. or
	// do we?
	t4p::CodeControlClass* codeCtrl = event.GetCodeControl();
	if (!codeCtrl) {
		return;
	}
	if (!codeCtrl->IsNew()) {
		wxString fileName = codeCtrl->GetFileName();
		t4p::ProjectTagSingleFileActionClass* tagAction = new t4p::ProjectTagSingleFileActionClass(App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_TAG_FINDER_LIST);
		tagAction->SetFileToParse(fileName);
		if (tagAction->Init(App.Globals)) {
			App.SqliteRunningThreads.Queue(tagAction);
		}
		else {
			delete tagAction;
		}
	}

	// Notebook class assigns unique IDs to CodeControlClass objects
	// the event ID is the ID of the code control that was closed
	App.Globals.TagCache.RemoveWorking(codeCtrl->GetIdString());
	
	if (codeCtrl->IsNew()) {

		// if a new file was closed but never saved remove it from the tag cache
		App.Globals.TagCache.DeleteFromFile(codeCtrl->GetIdString());
	}
}

wxString t4p::TagFeatureClass::CacheStatus() {
	if (CACHE_OK == CacheState) {
		return _("OK");
	}
	return _("Stale");
}

void t4p::TagFeatureClass::OnWorkingCacheComplete(t4p::WorkingCacheCompleteEventClass& event) {
	wxString fileIdentifier = event.GetFileIdentifier();
	bool good = App.Globals.TagCache.RegisterWorking(fileIdentifier, event.WorkingCache);
	if (!good) {

		// already there
		App.Globals.TagCache.ReplaceWorking(fileIdentifier, event.WorkingCache);
	}
	event.Skip();
}

void t4p::TagFeatureClass::OnActionComplete(t4p::ActionEventClass& event) {

	 // after the file was parsed re-start the timer.  this gets called always, where as
	 // OnWorkingCacheComplete does not get called when the file contains invalid syntax (no
	 // symbol table could be created)
	Timer.Start(500, wxTIMER_CONTINUOUS);
}

void t4p::TagFeatureClass::OnAppFileOpened(t4p::CodeControlEventClass& event) {
	t4p::CodeControlClass* codeControl = event.GetCodeControl();
	if (codeControl && codeControl->GetFileType() == t4p::FILE_TYPE_PHP) {
		UnicodeString text = codeControl->GetSafeText();

		// builder action could take a while (more than the timer)
		// stop the timer so that we dont queue up a builder actions before the previous one
		// finishes
		Timer.Stop();

		// we need to differentiate between new and opened files (the 'true' arg)
		t4p::WorkingCacheBuilderClass* builder = new t4p::WorkingCacheBuilderClass(App.SqliteRunningThreads, ID_WORKING_CACHE);
		builder->Update(
			App.Globals,
			codeControl->GetFileName(),
			codeControl->GetIdString(),
			text, 
			codeControl->IsNew(),
			App.Globals.Environment.Php.Version,

			// false ==  no need to parse tags from the file, because they will be the same as on the db
			// also, do not attempt to parse because we want to avoid sqlite db locking errors in case
			// that the project tag action is running in the background
			false);
		App.SqliteRunningThreads.Queue(builder);
	}
	else if (App.Globals.IsASourceFile(codeControl->GetFileName())) {
		
		// for other project files, we still want to "tag" them so that 
		// we record their name, that way total search filename lookups
		// will include these files
		t4p::ProjectTagSingleFileActionClass* tagAction = new t4p::ProjectTagSingleFileActionClass(App.SqliteRunningThreads, wxID_ANY);
		tagAction->SetFileToParse(codeControl->GetFileName());
		if (tagAction->Init(App.Globals)) {
			App.SqliteRunningThreads.Queue(tagAction);
		}
		else {
			delete tagAction;
		}
	}
	event.Skip();
}

void t4p::TagFeatureClass::OnAppFileReverted(t4p::CodeControlEventClass& event) {
	t4p::CodeControlClass* codeControl = event.GetCodeControl();
	if (codeControl && codeControl->GetFileType() == t4p::FILE_TYPE_PHP) {
		UnicodeString text = codeControl->GetSafeText();

		// builder action could take a while (more than the timer)
		// stop the timer so that we dont queue up a builder actions before the previous one
		// finishes
		Timer.Stop();

		// we need to differentiate between new and opened files (the 'true' arg)
		t4p::WorkingCacheBuilderClass* builder = new t4p::WorkingCacheBuilderClass(App.SqliteRunningThreads, ID_WORKING_CACHE);
		builder->Update(
			App.Globals,
			codeControl->GetFileName(),
			codeControl->GetIdString(),
			text, 
			codeControl->IsNew(),
			App.Globals.Environment.Php.Version,

			// The logic is a bit 
			//  different than OnAppFileOpened, when a file is reverted we will re-tag the file and 
			// rebuild the symbol table, while when we open a file we dont need to re-tag the file
			// because it has not changed.
			true);
		App.SqliteRunningThreads.Queue(builder);
	}
	event.Skip();
}

void t4p::TagFeatureClass::OnAppFileDeleted(wxCommandEvent& event) {

	// clean up the cache in a background thread
	std::vector<wxFileName> filesToDelete;
	filesToDelete.push_back(wxFileName(event.GetString()));
	t4p::TagDeleteFileActionClass* action =  new t4p::TagDeleteFileActionClass(App.SqliteRunningThreads, wxID_ANY,
		filesToDelete);
	action->Init(App.Globals);
	App.SqliteRunningThreads.Queue(action);
}

void t4p::TagFeatureClass::OnAppFileRenamed(t4p::RenameEventClass& event) {
	t4p::ProjectTagSingleFileRenameActionClass* action = new t4p::ProjectTagSingleFileRenameActionClass(App.SqliteRunningThreads, wxID_ANY);
	action->SetPaths(event.OldPath.GetFullPath(), event.NewPath.GetFullPath());
	action->Init(App.Globals);
	App.SqliteRunningThreads.Queue(action);
}

void t4p::TagFeatureClass::OnAppFileExternallyModified(wxCommandEvent& event) {

	// the file is assumed not be opened, we don't need to build the symbol table
	// just retag it
	// see the comment for EVENT_APP_FILE_EXTERNALLY_MODIFIED in Events.h
	// if the file is from an active project, then re-tag it
	// otherwise do nothing
	wxString fileName = event.GetString();
	t4p::ProjectTagSingleFileActionClass* tagAction = new t4p::ProjectTagSingleFileActionClass(App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_TAG_FINDER_LIST);
	tagAction->SetFileToParse(fileName);
	if (tagAction->Init(App.Globals)) {
		App.SqliteRunningThreads.Queue(tagAction);
	}
	else {
		delete tagAction;
	}
}

void t4p::TagFeatureClass::OnAppDirCreated(wxCommandEvent& event) {
	t4p::ProjectTagDirectoryActionClass* tagAction =  new t4p::ProjectTagDirectoryActionClass(App.SqliteRunningThreads, wxID_ANY);
	tagAction->SetDirToParse(event.GetString());
	if (tagAction->Init(App.Globals)) {
		App.SqliteRunningThreads.Queue(tagAction);
	}
	else {
		delete tagAction;
	}
}

void t4p::TagFeatureClass::OnAppDirDeleted(wxCommandEvent& event) {
	std::vector<wxFileName> dirsToDelete;
	wxFileName dir;
	dir.AssignDir(event.GetString());
	dirsToDelete.push_back(dir);
	t4p::TagDeleteDirectoryActionClass* tagAction =  new t4p::TagDeleteDirectoryActionClass(App.SqliteRunningThreads, wxID_ANY, dirsToDelete);
	if (tagAction->Init(App.Globals)) {
		App.SqliteRunningThreads.Queue(tagAction);
	}
	else {
		delete tagAction;
	}
}

void t4p::TagFeatureClass::OnAppDirRenamed(t4p::RenameEventClass& event) {
t4p::ProjectTagDirectoryRenameActionClass* action = new t4p::ProjectTagDirectoryRenameActionClass(App.SqliteRunningThreads, wxID_ANY);
	action->SetPaths(event.OldPath.GetPath(), event.NewPath.GetPath());
	action->Init(App.Globals);
	App.SqliteRunningThreads.Queue(action);
}

void t4p::TagFeatureClass::OnTimerComplete(wxTimerEvent& event) {
	t4p::CodeControlClass* codeControl = GetCurrentCodeControl();
	if (!codeControl) {
		return;
	}
	
	// if the contents of the code control have not changed since we last parsed
	// the code then don't do anything.
	// this will be most of the time, since programmers like to think!
	// this will also help with lowering the number of times we hit the tags database
	if (!codeControl->Touched()) {
		return;
	}
	codeControl->SetTouched(false);
	
	// builder action could take a while (more than the timer)
	// stop the timer so that we dont queue up a builder actions before the previous one
	// finishes
	Timer.Stop();
	t4p::WorkingCacheBuilderClass* builder = new t4p::WorkingCacheBuilderClass(App.SqliteRunningThreads, ID_WORKING_CACHE);
	builder->Update(
		App.Globals,
		codeControl->GetFileName(),
		codeControl->GetIdString(),
		codeControl->GetSafeText(), 
		codeControl->IsNew(),
		App.Globals.Environment.Php.Version,
		true);
	App.SqliteRunningThreads.Queue(builder);
}

void t4p::TagFeatureClass::OnCodeControlHotspotClick(wxStyledTextEvent& event) {
	t4p::CodeControlClass* ctrl = GetCurrentCodeControl();
	if (!ctrl) {
		return;
	}
	int pos = event.GetPosition();
	
	// if the cursor is in the middle of an identifier, find the end of the
	// current identifier; that way we can know the full name of the tag we want
	// to get
	int endPos = ctrl->WordEndPosition(pos, true);
	wxString matchError;
	std::vector<t4p::TagClass> matches = App.Globals.TagCache.GetTagsAtPosition(
		ctrl->GetIdString(), ctrl->GetSafeText(), endPos, 
		App.Globals.AllEnabledSourceDirectories(),
		App.Globals,
		matchError
	);
	if (!matches.empty() && !matches[0].GetFullPath().IsEmpty()) {
		LoadPageFromResource(wxT(""), matches[0]);
	}
	else if (!matchError.empty()) {
		GetStatusBarWithGauge()->SetColumn0Text(matchError);
	}
}

t4p::TagSearchDialogClass::TagSearchDialogClass(wxWindow* parent, 
													  t4p::GlobalsClass& globals,
													  wxString cacheStatus,
													  wxString& term,
													  std::vector<t4p::TagClass>& chosenResources)
	: TagSearchDialogGeneratedClass(parent)
	, RunningThreads()
	, Globals(globals)
	, ChosenResources(chosenResources) 
	, MatchedResources() 
	, Timer(this, ID_SEARCH_TIMER)
	, LastInput(term) {
	wxGenericValidator termValidator(&term);
	TransferDataToWindow();
	CacheStatusLabel->SetLabel(wxT("Cache Status: ") + cacheStatus);
	SearchText->SetFocus();

	ProjectChoice->Append(_("All Enabled Projects"), (void*)NULL);
	for (size_t i = 0; i < Globals.Projects.size(); ++i) {
		if (Globals.Projects[i].IsEnabled) {

			// should be ok to reference this vector since it wont change because this is a 
			// modal dialog
			ProjectChoice->Append(Globals.Projects[i].Label, &Globals.Projects[i]);
		}
	}
	ProjectChoice->Select(0);

	// so that the thread can give the matching tags back to us
	RunningThreads.AddEventHandler(this);
	RunningThreads.SetMaxThreads(1);
	Timer.Start(150, wxTIMER_CONTINUOUS);
}

t4p::TagSearchDialogClass::~TagSearchDialogClass() {
	RunningThreads.RemoveEventHandler(this);
	RunningThreads.Shutdown();
}

void t4p::TagSearchDialogClass::OnSearchText(wxCommandEvent& event) {
	// nothing will be done while the user is entering text
}

void t4p::TagSearchDialogClass::OnTimerComplete(wxTimerEvent& event) {
	wxString text = SearchText->GetValue();
	if (text == LastInput) {

		// nothing to do, results are already being shown
		return;
	}
	std::vector<t4p::ProjectClass*> projects;
	bool showAllProjects = ProjectChoice->GetSelection() == 0;
	if (!showAllProjects) {
		projects.push_back((t4p::ProjectClass*)ProjectChoice->GetClientData(ProjectChoice->GetSelection()));
	}
	else {

		// the first item in the wxChoice will not have client data; the "all" option
		for (size_t i = 1; i < ProjectChoice->GetCount(); ++i) {
			projects.push_back((t4p::ProjectClass*) ProjectChoice->GetClientData(i));
		}
	}
	LastInput = text;
	SearchForResources(text, projects);
}

void t4p::TagSearchDialogClass::OnSearchEnter(wxCommandEvent& event) {
	if (MatchedResources.empty()) {
		
		// dont dismiss the dialog when no tags are shown
		return;
	}
	Timer.Stop();
	RunningThreads.Shutdown();
		
	if (MatchedResources.size() == 1) {

		// if there is only match, just take the user to it
		TransferDataFromWindow();
		ChosenResources.clear();
		ChosenResources.push_back(MatchedResources[0]);
		EndModal(wxOK);
		return;
	}
	wxArrayInt checks;
	for (size_t i = 0; i < MatchesList->GetCount(); ++i) {
		if (MatchesList->IsChecked(i)) {
			checks.Add(i);
		}
	}
	if (checks.Count() > 1) {
	
		// open the checked items
		for (size_t i = 0; i < checks.Count(); ++i) {
			int matchIndex = checks.Item(i);
			if (t4p::NumberLessThan(matchIndex, MatchedResources.size())) {
				ChosenResources.push_back(MatchedResources[matchIndex]);
			}
		}
		EndModal(wxOK);
		return;
	}

	// no checked items, take the user to the
	// selected item
	int selection = MatchesList->GetSelection();
	if (t4p::NumberLessThan(selection, MatchedResources.size())) {
		ChosenResources.push_back(MatchedResources[selection]);
	}
	EndModal(wxOK);
}

void t4p::TagSearchDialogClass::ShowJumpToResults(const wxString& finderQuery, const std::vector<t4p::TagClass>& allMatches) {
	wxArrayString files;
	for (size_t i = 0; i < allMatches.size(); ++i) {
		files.Add(allMatches[i].GetFullPath());
	}
	MatchesList->Clear();
	bool showAllProjects = ProjectChoice->GetSelection() == 0;
	t4p::ProjectClass* selectedProject = NULL;
	if (!showAllProjects) {
		selectedProject = (t4p::ProjectClass*)ProjectChoice->GetClientData(ProjectChoice->GetSelection());
	}
	
	// dont show the project path to the user
	for (size_t i = 0; i < files.GetCount(); ++i) {
		wxString projectLabel;
		wxString relativeName;
		if (showAllProjects) {
			relativeName = Globals.RelativeFileName(files[i], projectLabel);
		}
		else {
			relativeName = selectedProject->RelativeFileName(files[i]);
			projectLabel = selectedProject->Label;
		}
		wxString matchLabel;
		t4p::TagClass match = allMatches[i];
		if (t4p::TagClass::MEMBER == match.Type || t4p::TagClass::METHOD == match.Type ||
			t4p::TagClass::CLASS_CONSTANT == match.Type) {
			matchLabel += t4p::IcuToWx(match.ClassName);
			matchLabel += wxT("::");
			matchLabel += t4p::IcuToWx(match.Identifier);
		}
		else if (t4p::TagClass::CLASS == match.Type || t4p::TagClass::FUNCTION == match.Type
			|| t4p::TagClass::DEFINE == match.Type) {
			matchLabel += t4p::IcuToWx(match.Identifier);
		}
		else {
			matchLabel += t4p::IcuToWx(match.Identifier);
		}
		matchLabel += wxT(" - ");
		matchLabel += relativeName;
		matchLabel +=  wxT("  (") + projectLabel + wxT(")");
		MatchesList->Append(matchLabel);
	}
	if (!MatchesList->IsEmpty()) {
		MatchesList->Select(0);
	}
	MatchesLabel->SetLabel(wxString::Format("Found %ld files for %s. Please choose file(s) to open.", 
		allMatches.size(), finderQuery));
}

void t4p::TagSearchDialogClass::OnOkButton(wxCommandEvent& event) {
	bool isChecked = false;
	for (size_t i = 0; i < MatchesList->GetCount(); ++i) {
		if (MatchesList->IsChecked(i)) {
			isChecked = true;
			break;
		}
	}
	if (!isChecked) {
		
		// nothing chosen
		return;
	}
	Timer.Stop();
	RunningThreads.Shutdown();
	TransferDataFromWindow();
	ChosenResources.clear();
	for (size_t i = 0; i < MatchesList->GetCount(); ++i) {
		if (MatchesList->IsChecked(i)) {
			ChosenResources.push_back(MatchedResources[i]);
		}
	}
	EndModal(wxOK);
}

void t4p::TagSearchDialogClass::OnCancelButton(wxCommandEvent& event) {
	Timer.Stop();
	RunningThreads.Shutdown();
	EndModal(wxCANCEL);
}

void t4p::TagSearchDialogClass::Prepopulate(const wxString& term, const std::vector<t4p::TagClass> &matches) {
	MatchedResources = matches;
	SearchText->SetValue(term);
	ShowJumpToResults(term, MatchedResources);
}
	
void t4p::TagSearchDialogClass::OnHelpButton(wxCommandEvent& event) {
	
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

void t4p::TagSearchDialogClass::OnSearchKeyDown(wxKeyEvent& event) {
	int keyCode = event.GetKeyCode();
	size_t selection = MatchesList->GetSelection();
	if (keyCode == WXK_DOWN) {		
		if (!MatchesList->IsEmpty() && selection < (MatchesList->GetCount() - 1)) {
			MatchesList->SetSelection(selection + 1);
		}
		else if (!MatchesList->IsEmpty()) {

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

void t4p::TagSearchDialogClass::OnMatchesListDoubleClick(wxCommandEvent& event) {
	int selection = event.GetSelection();
	if (selection == wxNOT_FOUND) {
		
		// no item chosen
		return;
	}
	Timer.Stop();
	RunningThreads.Shutdown();
	
	TransferDataFromWindow();
	ChosenResources.clear();
	
	if (t4p::NumberLessThan(selection, MatchesList->GetCount())) {
		ChosenResources.push_back(MatchedResources[selection]);
	}
	EndModal(wxOK);
}

void t4p::TagSearchDialogClass::OnMatchesListKeyDown(wxKeyEvent& event) {
	if (event.GetKeyCode() == WXK_RETURN) {
		wxCommandEvent cmdEvt;
		OnSearchEnter(cmdEvt);
	}
	else {
		event.Skip();
	}
}

void t4p::TagSearchDialogClass::OnProjectChoice(wxCommandEvent& event) {
	bool showAllProjects = event.GetSelection() == 0;
	std::vector<t4p::ProjectClass*> projects;
	if (!showAllProjects) {
		projects.push_back((t4p::ProjectClass*)ProjectChoice->GetClientData(event.GetSelection()));
	}
	else {

		// the first item in the wxChoice will not have client data; the "all" option
		for (size_t i = 1; i < ProjectChoice->GetCount(); ++i) {
			projects.push_back((t4p::ProjectClass*) ProjectChoice->GetClientData(i));
		}
	}
	SearchForResources(LastInput, projects);
}

void t4p::TagSearchDialogClass::SearchForResources(const wxString& text, std::vector<t4p::ProjectClass*> projects) {
	t4p::TagCacheSearchActionClass* action =  new
		t4p::TagCacheSearchActionClass(RunningThreads, ID_EVENT_TAG_CACHE_SEARCH);
	std::vector<wxFileName> dirs;
	for (std::vector<t4p::ProjectClass*>::const_iterator p = projects.begin(); p != projects.end(); ++p) {
		std::vector<t4p::SourceClass>::const_iterator src;
		for (src = (*p)->Sources.begin(); src != (*p)->Sources.end(); ++src) {
			dirs.push_back(src->RootDirectory);
		}
	}
	action->SetSearch(Globals, text, dirs);
	RunningThreads.Queue(action);
}

void t4p::TagSearchDialogClass::OnTagCacheSearchComplete(t4p::TagCacheSearchCompleteEventClass &event) {
	MatchedResources = event.Tags;
	if (!MatchedResources.empty()) {
		MatchesList->Freeze();
		ShowJumpToResults(t4p::IcuToWx(event.SearchString), MatchedResources);
		MatchesList->Thaw();
	}
	else {
		MatchesList->Clear();
		MatchesLabel->SetLabel(wxString::Format(_("No matches found for %s"), (const char*)LastInput.c_str()));
	}
}

BEGIN_EVENT_TABLE(t4p::TagFeatureClass, wxEvtHandler)
	EVT_MENU(t4p::MENU_RESOURCE + 0, t4p::TagFeatureClass::OnProjectWipeAndIndex)
	EVT_MENU(t4p::MENU_RESOURCE + 1, t4p::TagFeatureClass::OnJump)
	EVT_MENU(t4p::MENU_RESOURCE + 3, t4p::TagFeatureClass::OnJump)
	EVT_UPDATE_UI(wxID_ANY, t4p::TagFeatureClass::OnUpdateUi)

	EVT_APP_FILE_CLOSED(t4p::TagFeatureClass::OnAppFileClosed)
	EVT_APP_FILE_OPEN(t4p::TagFeatureClass::OnAppFileOpened)
	EVT_APP_FILE_REVERTED(t4p::TagFeatureClass::OnAppFileReverted)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_FILE_DELETED, t4p::TagFeatureClass::OnAppFileDeleted)
	EVT_APP_FILE_RENAMED(t4p::TagFeatureClass::OnAppFileRenamed)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_DIR_CREATED,  t4p::TagFeatureClass::OnAppDirCreated)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_DIR_DELETED,  t4p::TagFeatureClass::OnAppDirDeleted)
	EVT_APP_DIR_RENAMED(t4p::TagFeatureClass::OnAppDirRenamed)

	// we will treat file new and file opened the same
	EVT_APP_FILE_NEW(t4p::TagFeatureClass::OnAppFileOpened)
	
	// we will treat new exernal file and file external modified the same
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_FILE_EXTERNALLY_CREATED, t4p::TagFeatureClass::OnAppFileExternallyModified)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_FILE_EXTERNALLY_MODIFIED, t4p::TagFeatureClass::OnAppFileExternallyModified)
	

	EVT_COMMAND(wxID_ANY, t4p::EVENT_SEQUENCE_COMPLETE, t4p::TagFeatureClass::OnAppStartSequenceComplete)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_EXIT, t4p::TagFeatureClass::OnAppExit)

	EVT_TIMER(ID_REPARSE_TIMER, t4p::TagFeatureClass::OnTimerComplete)
	EVT_WORKING_CACHE_COMPLETE(ID_WORKING_CACHE, t4p::TagFeatureClass::OnWorkingCacheComplete)
	EVT_ACTION_COMPLETE(ID_WORKING_CACHE, t4p::TagFeatureClass::OnActionComplete)
	EVT_STC_HOTSPOT_CLICK(wxID_ANY, t4p::TagFeatureClass::OnCodeControlHotspotClick)
END_EVENT_TABLE()


BEGIN_EVENT_TABLE(t4p::TagSearchDialogClass, TagSearchDialogGeneratedClass)
	EVENT_TAG_CACHE_SEARCH_COMPLETE(ID_EVENT_TAG_CACHE_SEARCH, t4p::TagSearchDialogClass::OnTagCacheSearchComplete)
	EVT_TIMER(ID_SEARCH_TIMER, t4p::TagSearchDialogClass::OnTimerComplete)
END_EVENT_TABLE()