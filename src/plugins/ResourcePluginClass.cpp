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
#include <wx/artprov.h>
#include <wx/filename.h>
#include <wx/valgen.h>

static int ID_JUMP_TO_GAUGE = wxNewId();
static int ID_COUNT_FILES_GAUGE = wxNewId();
static int ID_TOOLBAR_INDEX = wxNewId();
static int ID_RESOURCE_PLUGIN_PANEL = wxNewId();
static int ID_CONTEXT_MENU_JUMP = wxNewId();

mvceditor::ResourceFileReaderClass::ResourceFileReaderClass(wxEvtHandler& handler) 
	: BackgroundFileReaderClass(handler)
	, PhpFileFilters()
	, ResourceCache(NULL) {
}

bool mvceditor::ResourceFileReaderClass::InitForNativeFunctionsFile(mvceditor::ResourceCacheClass* resourceCache) {
	ResourceCache = resourceCache;
	PhpFileFilters.clear();
	wxFileName nativeFunctionsFilePath = mvceditor::NativeFunctionsAsset();

	// use GetPath(); Init() needs a directory
	if (Init(nativeFunctionsFilePath.GetPath())) {
		
		// need to do this so that the resource finder attempts to parse the files
		PhpFileFilters.push_back(nativeFunctionsFilePath.GetFullName());
		return true;
	}
	return false;
}

bool mvceditor::ResourceFileReaderClass::InitForProject(mvceditor::ResourceCacheClass* resourceCache, 
														const wxString& projectRootPath, 
														const std::vector<wxString>& phpFileFilters) {
	PhpFileFilters.clear();
	if (Init(projectRootPath)) {
		ResourceCache = resourceCache;
		PhpFileFilters = phpFileFilters;
		return true;
	}
	return false;
}

bool mvceditor::ResourceFileReaderClass::FileRead(mvceditor::DirectorySearchClass& search) {
	ResourceCache->WalkGlobal(search, PhpFileFilters);
	if (!search.More()) {

		// very important to do this here on the background thread
		// that way when a search is done the cache won't have to do it
		// and lookups will be quicker
		ResourceCache->EnsureSortedGlobal();
	}
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

mvceditor::ResourcePluginClass::ResourcePluginClass()
	: PluginClass()
	, JumpToText()
	, ResourceFileReader(*this)
	, ProjectIndexMenu(NULL)
	, State(FREE) 
	, HasCodeLookups(false)
	, HasFileLookups(false) {
	ResourcePluginPanel = NULL;
}

void mvceditor::ResourcePluginClass::AddProjectMenuItems(wxMenu* projectMenu) {
	ProjectIndexMenu = projectMenu->Append(mvceditor::MENU_RESOURCE + 0, _("Index"), _("Index the project"));
	projectMenu->Append(mvceditor::MENU_RESOURCE + 1, _("Jump To Resource Under Cursor"), _("Jump To Resource that is under the cursor"));
	projectMenu->Append(mvceditor::MENU_RESOURCE + 2, _("Search for Resource..."), _("Set focus on the resource input"));
	ProjectIndexMenu->Enable(GetProject() && FREE == State);
}

void mvceditor::ResourcePluginClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_RESOURCE + 0] = wxT("Resource-Index Project");
	menuItemIds[mvceditor::MENU_RESOURCE + 1] = wxT("Resource-Jump To Resource Under Cursor");
	menuItemIds[mvceditor::MENU_RESOURCE + 2] = wxT("Resource-Search For Resource ...");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::ResourcePluginClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	toolBar->AddTool(ID_TOOLBAR_INDEX, wxT("Index"), wxArtProvider::GetBitmap(
		wxART_EXECUTABLE_FILE, wxART_TOOLBAR, wxSize(16, 16)), wxT("Index"), wxITEM_NORMAL);
}

void mvceditor::ResourcePluginClass::AddWindows() {
	ResourcePluginPanel = new ResourcePluginPanelClass(GetMainWindow(), *this);
	AuiManager->AddPane(ResourcePluginPanel, wxAuiPaneInfo().Top(
		).CaptionVisible(false).CloseButton(false).Gripper(
		false).DockFixed(true).PaneBorder(false).Floatable(
		false).Row(2));
}

void mvceditor::ResourcePluginClass::AddCodeControlClassContextMenuItems(wxMenu* menu) {
	menu->Append(ID_CONTEXT_MENU_JUMP, _("Jump To Source"));
}

void mvceditor::ResourcePluginClass::OnProjectOpened() {
	HasCodeLookups = false;
	HasFileLookups = false;
	if (ResourceFileReader.IsRunning()) {
		ResourceFileReader.StopReading();
	}
	if (ResourceFileReader.InitForNativeFunctionsFile(GetResourceCache())) {
		mvceditor::BackgroundFileReaderClass::StartError error = mvceditor::BackgroundFileReaderClass::NONE;
		if (ResourceFileReader.StartReading(error)) {
			State = INDEXING_NATIVE_FUNCTIONS;
			GetStatusBarWithGauge()->AddGauge(_("Preparing Resource Index"), ID_COUNT_FILES_GAUGE, 
				StatusBarWithGaugeClass::INDETERMINATE_MODE, wxGA_HORIZONTAL);
		}
		else if (mvceditor::BackgroundFileReaderClass::ALREADY_RUNNING == error) {
			wxMessageBox(_("Indexing is already taking place. Please wait."), wxT("Warning"), wxICON_EXCLAMATION);
		}
		else if (mvceditor::BackgroundFileReaderClass::NO_RESOURCES == error) {
			mvceditor::EditorLogError(mvceditor::LOW_RESOURCES);
		}
	}
}

void mvceditor::ResourcePluginClass::SearchForResources() {
	mvceditor::ResourceCacheClass* resourceCache = GetResourceCache();
	
	// don't bother searching when path or expression is not valid
	if (!resourceCache->PrepareAll(JumpToText)) {
		wxMessageBox(_("Invalid Expression"), wxT("Warning"), wxICON_EXCLAMATION);
		return;
	}
	
	// if we know the indexing has already taken place lets just do the lookup; it will be quick.
	if (!NeedToIndex(JumpToText)) {
		resourceCache->CollectNearMatchResourcesFromAll();
		std::vector<mvceditor::ResourceClass> matches = resourceCache->Matches();
		ShowJumpToResults(JumpToText, matches);
		return;
	}

	// need to do indexing; start the background process
	mvceditor::ProjectClass* project = GetProject();
	if (!project->GetRootPath().IsEmpty()) {

		//prevent two finds at a time
		if (FREE == State) { 

			// don't bother searching when path is not valid
			if (ResourceFileReader.InitForProject(resourceCache, project->GetRootPath(), 
				project->GetPhpFileExtensions())) {
					mvceditor::BackgroundFileReaderClass::StartError error = mvceditor::BackgroundFileReaderClass::NONE;
					if (ResourceFileReader.StartReading(error)) {
						State = GOTO;
						GetStatusBarWithGauge()->AddGauge(_("Searching For Resources"),
							ID_COUNT_FILES_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE,
							wxGA_HORIZONTAL);
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

void mvceditor::ResourcePluginClass::OnWorkInProgress(wxCommandEvent& event) {
	GetStatusBarWithGauge()->IncrementGauge(ID_COUNT_FILES_GAUGE, StatusBarWithGaugeClass::INDETERMINATE_MODE);
}

void mvceditor::ResourcePluginClass::OnWorkComplete(wxCommandEvent& event) {
	GetStatusBarWithGauge()->StopGauge(ID_COUNT_FILES_GAUGE);

	UnicodeString fileName,
		className, 
		methodName;
	int lineNumber;
	mvceditor::ResourceFinderClass::ResourceTypes type = mvceditor::ResourceFinderClass::ParseGoToResource(JumpToText,
		fileName, className, methodName, lineNumber);

	// figure out what resources have been cached, so that next time we can jump
	// to the results without creating a new background thread
	if (INDEXING_PROJECT == State || GOTO == State) {
		if (ResourceFinderClass::CLASS_NAME == type ||
			ResourceFinderClass::CLASS_NAME_METHOD_NAME == type) {
			HasCodeLookups = true;
			HasFileLookups = true;
		}
		else if (ResourceFinderClass::FILE_NAME == type ||
			ResourceFinderClass::FILE_NAME_LINE_NUMBER == type) {
			HasFileLookups = true;
		}
	}
	
	// if we indexed because of a user query; need to show the user the results.
	States previousState = State;
	State = FREE;
	if (GOTO == previousState) {
		mvceditor::ResourceCacheClass* resourceCache = GetResourceCache();
		resourceCache->PrepareAll(JumpToText);
		resourceCache->CollectNearMatchResourcesFromAll();
		std::vector<mvceditor::ResourceClass> matches = resourceCache->Matches();
		ShowJumpToResults(JumpToText, matches);
	}
	else if (INDEXING_PROJECT == previousState) {
		mvceditor::ProjectIndexedEventClass indexedEvent;
		AppEvent(indexedEvent);
	}
}

void mvceditor::ResourcePluginClass::ShowJumpToResults(const wxString& finderQuery, const std::vector<mvceditor::ResourceClass>& allMatches) {
	wxArrayString files;
	UnicodeString fileName,
		className, 
		methodName;
	int lineNumber = 0;
	mvceditor::ResourceFinderClass::ResourceTypes type = mvceditor::ResourceFinderClass::ParseGoToResource(finderQuery, 
		fileName, className, methodName, lineNumber);
	
	// no need to show jump to results for native functions
	std::vector<mvceditor::ResourceClass> nonNativeMatches = allMatches;
	std::vector<mvceditor::ResourceClass>::iterator it = nonNativeMatches.begin();
	while (it != nonNativeMatches.end()) {
		if (it->IsNative) {
			it = nonNativeMatches.erase(it);
		}
		else {
			it++;
		}
	}
	switch (nonNativeMatches.size()) {
		case 1:
			LoadPageFromResource(finderQuery, nonNativeMatches[0]);
			break;
		case 0:
			if (ResourceFinderClass::CLASS_NAME_METHOD_NAME == type) {
				wxMessageBox(_("Method Not Found: ") + finderQuery);
			}
			else if (ResourceFinderClass::CLASS_NAME == type) {
				wxMessageBox(_("Class Not Found: ") + finderQuery);
			}
			else {
				wxMessageBox(_("File Not Found: ") + finderQuery);
			}
			break;
		default:
			for (size_t i = 0; i < nonNativeMatches.size(); ++i) {
				files.Add(nonNativeMatches[i].GetFullPath());
			}
			
			// dont show the project path to the user
			for (size_t i = 0; i < files.GetCount(); ++i) {
				files[i].Replace(GetProject()->GetRootPath(), wxT(""));
				if (ResourceFinderClass::FILE_NAME != type &&
					ResourceFinderClass::FILE_NAME_LINE_NUMBER != type) {
					files[i] += wxT("  (") + mvceditor::StringHelperClass::IcuToWx(nonNativeMatches[i].Resource) + wxT(")");
				}
			}
			wxSingleChoiceDialog dialog(NULL,
				wxString::Format(_("Found %d files. Please choose file to open."), nonNativeMatches.size()),
				_("Resource Finder"), files, NULL, wxDEFAULT_DIALOG_STYLE| wxRESIZE_BORDER | wxOK | wxCANCEL);
			dialog.SetSize(wxSize(640, 120));
			if (wxID_OK == dialog.ShowModal()) {
				int selection = dialog.GetSelection();
				LoadPageFromResource(finderQuery, nonNativeMatches[selection]);	
			}
			break;
	}
}

void mvceditor::ResourcePluginClass::StartIndex() {
	mvceditor::ProjectClass* project = GetProject();
	if (project && !project->GetRootPath().IsEmpty()) {

		//prevent two finds at a time
		if (FREE == State) { 
			ResourceCacheClass* resourceCache =GetResourceCache();

			// don't bother searching when path or expression is not valid
			// need to do this so that the resource finder attempts to parse the files
			resourceCache->PrepareAll(wxT("FakeClass"));			
			if (ResourceFileReader.InitForProject(resourceCache, project->GetRootPath(), 
				project->GetPhpFileExtensions())) {
					mvceditor::BackgroundFileReaderClass::StartError error = mvceditor::BackgroundFileReaderClass::NONE;
					if (ResourceFileReader.StartReading(error)) {
						State = INDEXING_PROJECT;
						GetStatusBarWithGauge()->AddGauge(_("Indexing Project"),
							ID_COUNT_FILES_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE,
							wxGA_HORIZONTAL);
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
	wxWindow* mainWindow = GetMainWindow();
	if (codeControl) {
		std::vector<mvceditor::ResourceClass> matches = codeControl->GetCurrentSymbolResource();
		if (!matches.empty()) {
			wxString wxResource = mvceditor::StringHelperClass::IcuToWx(matches[0].Resource);
			ShowJumpToResults(wxResource, matches);	
		}
		else {
			
			// maybe cache has not been created or user has not indexed the project, lets index the project and try again			
			ResourcePluginPanelClass* window = (ResourcePluginPanelClass*)wxWindow::FindWindowById(ID_RESOURCE_PLUGIN_PANEL, mainWindow);
			window->FocusOnSearchControl();
			SearchForResources();
		}
	}
}

void mvceditor::ResourcePluginClass::OnSearchForResource(wxCommandEvent& event) {
	wxWindow* mainWindow = GetMainWindow();
	ResourcePluginPanelClass* window = (ResourcePluginPanelClass*)wxWindow::FindWindowById(ID_RESOURCE_PLUGIN_PANEL, mainWindow);
	window->ChangeToFileName(wxT(""));
	window->FocusOnSearchControl();
}


void mvceditor::ResourcePluginClass::LoadPageFromResource(const wxString& finderQuery, const mvceditor::ResourceClass& resource) {
	UnicodeString fileName,
		className, 
		methodName;
	int lineNumber = 0;
	mvceditor::ResourceFinderClass::ResourceTypes type = mvceditor::ResourceFinderClass::ParseGoToResource(finderQuery, 
		fileName, className, methodName, lineNumber);
	GetNotebook()->LoadPage(resource.GetFullPath());
	CodeControlClass* codeControl = GetCurrentCodeControl();
	if (codeControl) {
		int32_t position, 
			length;
		bool found = mvceditor::ResourceFinderClass::GetResourceMatchPosition(resource, codeControl->GetSafeText(), position, length);
		if (ResourceFinderClass::FILE_NAME_LINE_NUMBER == type) {
				
			// scintilla line numbers start at zero. use the ensure method so that the line is shown in the 
			// center of the screen
			int pos = codeControl->PositionFromLine(lineNumber - 1);
			codeControl->SetSelectionAndEnsureVisible(pos, pos);
			codeControl->GotoLine(lineNumber - 1);
		}
		if (ResourceFinderClass::FILE_NAME == type) {
				
			// nothing; just open the file but don't scroll down to any place
		}
		else if (found) {
			codeControl->SetSelectionAndEnsureVisible(position, position + length);
		}
	}
}

void mvceditor::ResourcePluginClass::OnUpdateUi(wxUpdateUIEvent& event) {
	ProjectIndexMenu->Enable(GetProject() && FREE == State);
	event.Skip();
}

bool mvceditor::ResourcePluginClass::NeedToIndex(const wxString& finderQuery) const {
	UnicodeString fileName,
		className, 
		methodName;
	int lineNumber = 0;
	mvceditor::ResourceFinderClass::ResourceTypes type = mvceditor::ResourceFinderClass::ParseGoToResource(finderQuery, 
		fileName, className, methodName, lineNumber);
	if ((ResourceFinderClass::CLASS_NAME == type ||
		ResourceFinderClass::CLASS_NAME_METHOD_NAME == type) && !HasCodeLookups)  {
		return true;
	}
	else if ((ResourceFinderClass::FILE_NAME == type ||
		ResourceFinderClass::FILE_NAME_LINE_NUMBER == type) && !HasFileLookups) {
		return true;
	}
	return false;
}

void mvceditor::ResourcePluginClass::OnPageChanged(wxAuiNotebookEvent& event) {
	size_t selection =  event.GetSelection();
	CodeControlClass* code = GetNotebook()->GetCodeControl(selection);
	if (code) {
		wxString fileName = code->GetFileName();
		ResourcePluginPanel->ChangeToFileName(fileName);
	}
	event.Skip();
}

void mvceditor::ResourcePluginClass::OnPageClosed(wxAuiNotebookEvent& event) {
	ResourcePluginPanel->RemoveClosedFiles(GetNotebook());
	event.Skip();
}

void mvceditor::ResourcePluginClass::OpenFile(wxString fileName) {
	GetNotebook()->LoadPage(fileName);
}

mvceditor::ResourcePluginPanelClass::ResourcePluginPanelClass(wxWindow* parent, ResourcePluginClass& resource)
	: ResourcePluginGeneratedPanelClass(parent, ID_RESOURCE_PLUGIN_PANEL)
	, ResourcePlugin(resource) {
	wxGenericValidator filesComboValidator(&ResourcePlugin.JumpToText);
	FilesCombo->SetValidator(filesComboValidator);
	
	HelpButton->SetBitmapLabel((wxArtProvider::GetBitmap(wxART_HELP, 
		wxART_TOOLBAR, wxSize(16, 16))));
}

void mvceditor::ResourcePluginPanelClass::FocusOnSearchControl() {
	TransferDataToWindow();
	FilesCombo->SetFocus();
}
	
/**
 * When a file is chosen make the notebook show the file
 * @param wxCommandEvent& the event
 */
void mvceditor::ResourcePluginPanelClass::OnFilesComboCombobox(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow()) {
		wxString fullPath = FilesCombo->GetValue();
		wxFileName fileName(fullPath);
		if (fileName.Normalize() && wxFileName::IsFileReadable(fileName.GetFullPath())) {
			ResourcePlugin.OpenFile(fileName.GetFullPath());
		}
		else {
			ResourcePlugin.SearchForResources();
		}
	}
}

/**
 * When the user presses enter iniate a file search for the entered text.
 * @param wxCommandEvent& the event
 */
void mvceditor::ResourcePluginPanelClass::OnFilesComboTextEnter(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow()) {
		wxString fullPath = FilesCombo->GetValue();
		wxFileName fileName(fullPath);
		if (fileName.Normalize() && wxFileName::IsFileReadable(fileName.GetFullPath())) {
			ResourcePlugin.OpenFile(fileName.GetFullPath());
		}
		else {
			ResourcePlugin.SearchForResources();
		}
	}
}


/**
 * When the user clicks the help button help text will be shown
 * @param wxCommandEvent& the event
 */
void mvceditor::ResourcePluginPanelClass::OnHelpButtonClick(wxCommandEvent& event) {
	
  wxString help = wxString::FromAscii("Type in a file name, file name:page number, "
		"class name,  or class name::method name. The resulting page will then ben opened.\n\nExamples:\n\n"
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

void mvceditor::ResourcePluginPanelClass::ChangeToFileName(wxString fileName) {

	// for now let's ignore the new (untitled) files
	if (!fileName.IsEmpty()) {
		FilesCombo->SetValue(fileName);
		wxArrayString values = FilesCombo->GetStrings();
		int foundIndex = values.Index(fileName);
		if (foundIndex == wxNOT_FOUND) {		
			FilesCombo->Append(fileName);	
		}
	}
	else {
		FilesCombo->SetValue(wxEmptyString);
	}
}

void mvceditor::ResourcePluginPanelClass::RemoveClosedFiles(mvceditor::NotebookClass* notebook) {
	int MAX_ITEMS = 18;
	int pageCount = notebook->GetPageCount();
	if (pageCount <= 0) {
		FilesCombo->SetValue(wxEmptyString);
	}

	// over a certain size, remove file names that are no longer open
	// we want to remove as little as possible so that it is easier for the 
	// user to get back to a file.
	for (size_t i = 0; i < FilesCombo->GetCount() && FilesCombo->GetCount() > (size_t)MAX_ITEMS; ++i) {
		wxString fileName = FilesCombo->GetString(i);
		bool found = false;
		for(size_t j = 0; j < notebook->GetPageCount(); ++j) {
			CodeControlClass* code = notebook->GetCodeControl(j);
			if (NULL != code) {
				if (fileName == code->GetFileName()) {
					found = true;
					
					// we are guaranteed to only have one instance of filename (file can be in at most 1 tab only).
					break;
				}
			}
		}
		if (!found) {
			FilesCombo->Delete(i);
			--i;
		}
	}
}

BEGIN_EVENT_TABLE(mvceditor::ResourcePluginClass, wxEvtHandler)
	EVT_MENU(ID_TOOLBAR_INDEX, mvceditor::ResourcePluginClass::OnProjectIndex)
	EVT_MENU(mvceditor::MENU_RESOURCE + 0, mvceditor::ResourcePluginClass::OnProjectIndex)
	EVT_MENU(mvceditor::MENU_RESOURCE + 1, mvceditor::ResourcePluginClass::OnJump)
	EVT_MENU(mvceditor::MENU_RESOURCE + 2, mvceditor::ResourcePluginClass::OnSearchForResource)
	EVT_MENU(ID_CONTEXT_MENU_JUMP, mvceditor::ResourcePluginClass::OnJump)
	EVT_UPDATE_UI(wxID_ANY, mvceditor::ResourcePluginClass::OnUpdateUi)
	EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, mvceditor::ResourcePluginClass::OnPageChanged)
	EVT_AUINOTEBOOK_PAGE_CLOSED(wxID_ANY, mvceditor::ResourcePluginClass::OnPageClosed)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_FILE_READ_COMPLETE, mvceditor::ResourcePluginClass::OnWorkComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::ResourcePluginClass::OnWorkInProgress)
END_EVENT_TABLE()