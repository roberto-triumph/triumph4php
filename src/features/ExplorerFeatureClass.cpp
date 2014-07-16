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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */

#include <features/ExplorerFeatureClass.h>
#include <globals/Assets.h>
#include <globals/Events.h>
#include <globals/FileName.h>
#include <search/Directory.h>
#include <widgets/FilePickerValidatorClass.h>
#include <Triumph.h>
#include <wx/file.h>
#include <wx/wupdlock.h>
#include <wx/stdpaths.h>
#include <wx/artprov.h>
#include <algorithm>

static int ID_EXPLORER_PANEL = wxNewId();
static int ID_EXPLORER_OUTLINE = wxNewId();
static int ID_EXPLORER_LIST_ACTION = wxNewId();
static int ID_EXPLORER_LIST_OPEN = wxNewId();
static int ID_EXPLORER_LIST_RENAME = wxNewId();
static int ID_EXPLORER_LIST_DELETE = wxNewId();
static int ID_EXPLORER_LIST_OPEN_PARENT = wxNewId();
static int ID_EXPLORER_LIST_CREATE_PHP = wxNewId();
static int ID_EXPLORER_LIST_CREATE_SQL = wxNewId();
static int ID_EXPLORER_LIST_CREATE_CSS = wxNewId();
static int ID_EXPLORER_LIST_CREATE_JS = wxNewId();
static int ID_EXPLORER_LIST_CREATE_TEXT = wxNewId();
static int ID_EXPLORER_LIST_CREATE_DIRECTORY = wxNewId();
static int ID_EXPLORER_LIST_SHELL = wxNewId();
static int ID_EXPLORER_LIST_FILE_MANAGER = wxNewId();
static int ID_EXPLORER_MODIFY = wxNewId();
static int ID_FILTER_ALL_SOURCE = wxNewId();
static int ID_FILTER_ALL = wxNewId();
static int ID_FILTER_PHP = wxNewId();
static int ID_FILTER_CSS = wxNewId();
static int ID_FILTER_SQL = wxNewId();
static int ID_FILTER_JS = wxNewId();

/**
 * comparator function that compares 2 filenames by using the full name
 * only (wxFileName::GetFullName())
 */
static bool FileNameCmp(const wxFileName& a, const wxFileName& b) {
	wxString aName = a.GetFullName();
	wxString bName = b.GetFullName();
	return aName.compare(bName) < 0;
}

/**
 * comparator function that compares 2 filenames by using the full name
 * only (wxFileName::GetDirs().Last())
 */
static bool DirNameCmp(const wxFileName& a, const wxFileName& b) {
	if (a.GetDirCount() == 0) {
		return true;
	}
	if (b.GetDirCount() == 0) {
		return false;
	}
	wxString aName = a.GetDirs().Last();
	wxString bName = b.GetDirs().Last();
	return aName.compare(bName) < 0;
}

t4p::FileListingClass::FileListingClass(wxEvtHandler& handler) 
: wxEvtHandler()
, WorkingDir()
, Files()
, Dirs()
, TotalFiles(0)
, TotalSubDirs(0)
, RunningThreads()
, Handler(handler)
, Watcher(NULL) {
	
	RunningThreads.SetMaxThreads(1);
	RunningThreads.AddEventHandler(this);
}

t4p::FileListingClass::~FileListingClass() {
	RunningThreads.RemoveEventHandler(this);
	RunningThreads.Shutdown();
	if (Watcher) {
		delete Watcher;
	}
}

void t4p::FileListingClass::OnFsWatcher(wxFileSystemWatcherEvent& event) {
	wxFileName modFile = event.GetNewPath();
	if (modFile.GetPathWithSep() != WorkingDir.GetPathWithSep()) {

		// event from directory we are not showing
		return;
	}
	if (event.GetChangeType() == wxFSW_EVENT_WARNING && event.GetWarningType() == wxFSW_WARNING_OVERFLOW) {
		
		// restart the watch
		delete Watcher;
		Watcher = new wxFileSystemWatcher();
		Watcher->SetOwner(this);
		Watcher->Add(WorkingDir, wxFSW_EVENT_CREATE | wxFSW_EVENT_DELETE | wxFSW_EVENT_RENAME | wxFSW_EVENT_WARNING | wxFSW_EVENT_ERROR);
	}
	else if (event.GetChangeType() == wxFSW_EVENT_ERROR) {
		
		// restart the watch
		delete Watcher;
		Watcher = new wxFileSystemWatcher();
		Watcher->SetOwner(this);
		Watcher->Add(WorkingDir, wxFSW_EVENT_CREATE | wxFSW_EVENT_DELETE | wxFSW_EVENT_RENAME | wxFSW_EVENT_WARNING | wxFSW_EVENT_ERROR);
	}

	// naive implementation for now, just refresh the entire dir
	// this is because we have labels to update, and the 
	// items must be kept sorted (first dirs, then files)
	// each sorted, AND taking the filters into account
	else if (event.GetChangeType() == wxFSW_EVENT_CREATE
		|| event.GetChangeType() == wxFSW_EVENT_DELETE
		|| event.GetChangeType() == wxFSW_EVENT_RENAME) {
		wxPostEvent(&Handler, event);
	}
}

void t4p::FileListingClass::StartDelete(const std::vector<wxFileName>& dirs, const std::vector<wxFileName>& files) {
	t4p::ExplorerModifyActionClass* action = new t4p::ExplorerModifyActionClass(RunningThreads, ID_EXPLORER_MODIFY);
	action->SetFilesToRemove(dirs, files);
	RunningThreads.Queue(action);
}

void t4p::FileListingClass::StartRefresh(const wxFileName& dir, const std::vector<wxString>& filterExtensions, bool doHidden) {
	t4p::ExplorerFileSystemActionClass* action = new t4p::ExplorerFileSystemActionClass(RunningThreads, ID_EXPLORER_LIST_ACTION);
	action->Directory(dir, filterExtensions, doHidden);
	RunningThreads.Queue(action);
}

void t4p::FileListingClass::StartRename(const wxFileName& oldFile, const wxString& newName) {
	t4p::ExplorerModifyActionClass* action = new t4p::ExplorerModifyActionClass(RunningThreads, ID_EXPLORER_MODIFY);
	action->SetFileToRename(oldFile, newName);
	RunningThreads.Queue(action);
}

void t4p::FileListingClass::OnExplorerListComplete(t4p::ExplorerEventClass& event) {
	Files = event.Files;
	Dirs = event.SubDirs;
	TotalFiles = event.TotalFiles;
	TotalSubDirs = event.TotalSubDirs;

	// only recreate the watch when the explorer shows a new dir
	// this method will be called as a result of an external file
	// watcher event (new/delete file), if there are many events in
	// quick succession a crash would happen. in reality we 
	// only need to recreate the watch when the explorer is being 
	// pointed into a different directory than previous
	bool changedDir = WorkingDir != event.Dir;
	if (changedDir && Watcher) {
		delete Watcher;
		Watcher = NULL;
	}

	// storing the new working dir AFTER the comparison to see if
	// its changed
	WorkingDir = event.Dir;
	if (!Watcher) {
		Watcher = new wxFileSystemWatcher();
		Watcher->SetOwner(this);
		Watcher->Add(WorkingDir, wxFSW_EVENT_CREATE | wxFSW_EVENT_DELETE | wxFSW_EVENT_RENAME | wxFSW_EVENT_WARNING | wxFSW_EVENT_ERROR);
	}
	wxPostEvent(&Handler, event);
}

void t4p::FileListingClass::OnExplorerModifyComplete(t4p::ExplorerModifyEventClass& event) {
	wxPostEvent(&Handler, event);
}

t4p::ExplorerFeatureClass::ExplorerFeatureClass(t4p::AppClass& app) 
	: FeatureClass(app) {
	wxPlatformInfo info;
	switch (info.GetOperatingSystemId()) {
		case wxOS_WINDOWS_NT:
			FileManagerExecutable = wxT("C:\\windows\\system32\\explorer.exe");
			ShellExecutable = wxT("C:\\windows\\system32\\cmd.exe");
			break;
		case wxOS_UNIX:
		case wxOS_UNIX_LINUX:
			FileManagerExecutable = wxT("/usr/bin/nautilus");
			ShellExecutable = wxT("/bin/sh");
			break;
		default:
			FileManagerExecutable = wxT("C:\\windows\\system32\\explorer.exe");
			ShellExecutable = wxT("C:\\windows\\system32\\cmd.exe");
	}
}


void t4p::ExplorerFeatureClass::AddViewMenuItems(wxMenu* viewMenu) {
	viewMenu->Append(t4p::MENU_EXPLORER + 1, _("Explore Open File\tCTRL+ALT+E"), _("Open An explorer window in the currently opened file"), wxITEM_NORMAL);
	viewMenu->Append(t4p::MENU_EXPLORER + 2, _("Explore Sources\tCTRL+SHIFT+E"), _("Open An explorer window"), wxITEM_NORMAL);
	viewMenu->Append(t4p::MENU_EXPLORER + 4, _("Explorer In Outline"), _("Open An explorer outline"), wxITEM_NORMAL);
}


void t4p::ExplorerFeatureClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[t4p::MENU_EXPLORER + 1] = wxT("Project-Explore File");
	menuItemIds[t4p::MENU_EXPLORER + 2] = wxT("Project-Explore Sources");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void t4p::ExplorerFeatureClass::AddWindows() {
	ExplorerToolBar = new wxAuiToolBar(GetMainWindow(), wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE |
                                         wxAUI_TB_TEXT |
                                         wxAUI_TB_HORZ_TEXT);
	ExplorerToolBar->SetFont(App.Preferences.ApplicationFont);
	ExplorerToolBar->SetToolBitmapSize(wxSize(16,16));

	wxBitmap bmp = t4p::BitmapImageAsset(wxT("explore"));
	wxBitmap bmpOpen = t4p::BitmapImageAsset(wxT("explore-open-document"));

	ExplorerToolBar->AddTool(t4p::MENU_EXPLORER + 1, _("Explore Open File"), bmpOpen, _("Open An explorer window in the currently opened file"));
	ExplorerToolBar->AddTool(t4p::MENU_EXPLORER + 2, _("Explore Projects"), bmp, _("Open An explorer window in the Project Root"));
	ExplorerToolBar->SetToolDropDown(t4p::MENU_EXPLORER + 2, true);
	ExplorerToolBar->SetOverflowVisible(false);
	ExplorerToolBar->Realize();

	AuiManager->AddPane(ExplorerToolBar, wxAuiPaneInfo()
		.ToolbarPane().Top().Row(2).Position(1)
		.LeftDockable(false).RightDockable(false)
		.Gripper(false).CaptionVisible(false).CloseButton(false).DockFixed(true)
		.PaneBorder(true).Floatable(false)
	);
}

void t4p::ExplorerFeatureClass::LoadPreferences(wxConfigBase *config) {
	wxString s;
	config->Read(wxT("/Explorer/FileManagerExecutable"), &s);

	// when setting, make sure to have the defaults in case there is nothing
	// in the config yet 
	if (!s.IsEmpty()) {
		FileManagerExecutable.Assign(s);
	}
	s = wxT("");
	config->Read(wxT("/Explorer/ShellExecutable"), &s);
	if (!s.IsEmpty()) {
		ShellExecutable.Assign(s);
	}
}

void t4p::ExplorerFeatureClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
	ExplorerOptionsPanelClass* panel = new ExplorerOptionsPanelClass(parent, wxID_ANY, *this);
	parent->AddPage(panel, wxT("Explorer"));
}

void t4p::ExplorerFeatureClass::OnExplorerProjectMenu(wxCommandEvent& event) {
	wxWindow* window = FindToolsWindow(ID_EXPLORER_PANEL);
	t4p::ModalExplorerPanelClass* panel = NULL;
	if (!window) {
		panel =  new t4p::ModalExplorerPanelClass(GetToolsNotebook(), ID_EXPLORER_PANEL, *this, GetNotebook());
		AddToolsWindow(panel, _("Explorer"));
	}
	else {
		panel = (t4p::ModalExplorerPanelClass*)window;
		SetFocusToToolsWindow(panel);
	}
	
	// show the selected project
	size_t index = event.GetId() - t4p::MENU_EXPLORER - 3;
	if (index >= 0 && index < SourceDirs.size()) {
		panel->RefreshDir(SourceDirs[index].RootDirectory);
	}
	else if (!SourceDirs.empty()) {
		panel->FocusOnSourcesList();
	}
	else {

		// as a fallback if the user has not created any projects
		// just go to the user's home dir
		wxStandardPaths paths = wxStandardPaths::Get();
		wxString documentsDirString = paths.GetDocumentsDir();
		wxFileName documentsDir;
		documentsDir.AssignDir(documentsDirString);
		panel->RefreshDir(documentsDir);
	}
}

void t4p::ExplorerFeatureClass::OnProjectExplore(wxCommandEvent& event) {
	SourceDirs = App.Globals.AllEnabledSources();
	wxCommandEvent cmdEvt;
	if (SourceDirs.size() == 1) {
	
		// only 1 source dir, just open it now
		cmdEvt.SetId(t4p::MENU_EXPLORER + 3);
	}
	OnExplorerProjectMenu(cmdEvt);
}

void t4p::ExplorerFeatureClass::OnProjectOutline(wxCommandEvent& event) {
	wxWindow* window = FindToolsWindow(ID_EXPLORER_OUTLINE);
	t4p::ExplorerOutlinePanelClass* panel = NULL;
	if (!window) {
		panel =  new t4p::ExplorerOutlinePanelClass(GetOutlineNotebook(), ID_EXPLORER_OUTLINE, *this, GetNotebook());
		AddOutlineWindow(panel, _("Explorer"));
	}
	else {
		panel = (t4p::ExplorerOutlinePanelClass*)window;
		SetFocusToOutlineWindow(panel);
	}
	
	panel->FillSourcesList(App.Globals.AllEnabledSourceDirectories());
	
	// show the first project
	SourceDirs = App.Globals.AllEnabledSources();
	if (!SourceDirs.empty()) {
		panel->RefreshDir(SourceDirs[0].RootDirectory);
	}
	else {

		// as a fallback if the user has not created any projects
		// just go to the user's home dir
		wxStandardPaths paths = wxStandardPaths::Get();
		wxString documentsDirString = paths.GetDocumentsDir();
		wxFileName documentsDir;
		documentsDir.AssignDir(documentsDirString);
		panel->RefreshDir(documentsDir);
	}
}

void t4p::ExplorerFeatureClass::OnExplorerToolDropDown(wxAuiToolBarEvent& event) {
	if (!event.IsDropDownClicked()) {
		return;
	}
	ExplorerToolBar->SetToolSticky(event.GetId(), true);
		
	// gather all enabled source directories
	SourceDirs = App.Globals.AllEnabledSources();
	
	// create the popup menu that contains all the available browser names
	wxMenu projectMenu;
	for (size_t index = 0; index < SourceDirs.size(); ++index) {
		projectMenu.Append(t4p::MENU_EXPLORER + 3 + index, SourceDirs[index].RootDirectory.GetDirs().Last());
	}
	
	// line up our menu with the button
	wxRect rect = ExplorerToolBar->GetToolRect(event.GetId());
	wxPoint pt = ExplorerToolBar->ClientToScreen(rect.GetBottomLeft());
	pt = GetMainWindow()->ScreenToClient(pt);
	GetMainWindow()->PopupMenu(&projectMenu, pt);

	// make sure the button is "un-stuck"
	ExplorerToolBar->SetToolSticky(event.GetId(), false);
}

void t4p::ExplorerFeatureClass::OnProjectExploreOpenFile(wxCommandEvent& event) {
	t4p::CodeControlClass* codeCtrl = GetCurrentCodeControl();
	if (!codeCtrl) {
		return;
	}
	wxString openFile = codeCtrl->GetFileName();
	if (openFile.empty()) {
		return;
	}
	wxWindow* window = FindToolsWindow(ID_EXPLORER_PANEL);
	t4p::ModalExplorerPanelClass* panel = NULL;
	if (!window) {
		panel =  new t4p::ModalExplorerPanelClass(GetToolsNotebook(), ID_EXPLORER_PANEL, *this, GetNotebook());
		AddToolsWindow(panel, _("Explorer"));
	}
	else {
		panel = (t4p::ModalExplorerPanelClass*)window;
		SetFocusToToolsWindow(panel);
	}
	wxFileName fileName(openFile);
	wxFileName dir;
	dir.AssignDir(fileName.GetPath());
	panel->RefreshDir(dir);
}

void t4p::ExplorerFeatureClass::OnAppPreferencesSaved(wxCommandEvent& event) {
	t4p::ModalExplorerPanelClass* panel = NULL;
	wxWindow* window = FindToolsWindow(ID_EXPLORER_PANEL);
	if (window) {
		std::vector<wxFileName> sourceDirs = App.Globals.AllEnabledSourceDirectories();	
	
		panel = (t4p::ModalExplorerPanelClass*)window;
		panel->FillSourcesList(sourceDirs);
	}

	wxConfigBase *config = wxConfig::Get();
	config->Write(wxT("/Explorer/FileManagerExecutable"), FileManagerExecutable.GetFullPath());
	config->Write(wxT("/Explorer/ShellExecutable"), ShellExecutable.GetFullPath());
}

void t4p::ExplorerFeatureClass::OnAppProjectCreated(wxCommandEvent& event) {
	t4p::ModalExplorerPanelClass* panel = NULL;
	wxWindow* window = FindToolsWindow(ID_EXPLORER_PANEL);
	if (!window) {
		panel =  new t4p::ModalExplorerPanelClass(GetToolsNotebook(), ID_EXPLORER_PANEL, *this, GetNotebook());
		AddToolsWindow(panel, _("Explorer"));
	}
	else {
		panel = (t4p::ModalExplorerPanelClass*)window;
	}
	wxString dir = event.GetString();
	wxFileName projectDir;
	projectDir.AssignDir(dir);
	panel->RefreshDir(projectDir);
}

static void SetExplorerAccelerators(wxListCtrl* ctrl) {
	wxAcceleratorEntry entries[5];
    entries[0].Set(wxACCEL_NORMAL, WXK_F2, ID_EXPLORER_LIST_RENAME);
    entries[1].Set(wxACCEL_NORMAL, WXK_BACK, ID_EXPLORER_LIST_OPEN_PARENT);
	entries[2].Set(wxACCEL_ALT, WXK_LEFT, ID_EXPLORER_LIST_OPEN_PARENT);
	entries[3].Set(wxACCEL_NORMAL, WXK_RETURN, ID_EXPLORER_LIST_OPEN);
	entries[4].Set(wxACCEL_NORMAL, WXK_DELETE, ID_EXPLORER_LIST_DELETE);
	wxAcceleratorTable table(5, entries);
    ctrl->SetAcceleratorTable(table);
}

t4p::FileListingWidgetClass::FileListingWidgetClass(wxListCtrl* list, wxImageList* imageList, t4p::FileListingClass* fileListing,
	wxWindow* parentPanel, t4p::ExplorerFeatureClass* feature)
: wxEvtHandler()
, List(list)
, FilesImageList(imageList) 
, FileListing(fileListing) 
, ParentPanel(parentPanel)
, Feature(feature) {
	List->Connect(wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler(t4p::FileListingWidgetClass::OnListEndLabelEdit), NULL, this);
	List->Connect(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler(t4p::FileListingWidgetClass::OnListItemRightClick), NULL, this);
	List->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(t4p::FileListingWidgetClass::OnListRightDown), NULL, this);

	ParentPanel->Connect(ID_EXPLORER_LIST_RENAME, wxEVT_MENU, wxCommandEventHandler(t4p::FileListingWidgetClass::OnListMenuRename), NULL, this);
	ParentPanel->Connect(ID_EXPLORER_LIST_DELETE, wxEVT_MENU, wxCommandEventHandler(t4p::FileListingWidgetClass::OnListMenuDelete), NULL, this);
	ParentPanel->Connect(ID_EXPLORER_LIST_CREATE_PHP, wxEVT_MENU, wxCommandEventHandler(t4p::FileListingWidgetClass::OnListMenuCreateNew), NULL, this);
	ParentPanel->Connect(ID_EXPLORER_LIST_CREATE_SQL, wxEVT_MENU, wxCommandEventHandler(t4p::FileListingWidgetClass::OnListMenuCreateNew), NULL, this);
	ParentPanel->Connect(ID_EXPLORER_LIST_CREATE_CSS, wxEVT_MENU, wxCommandEventHandler(t4p::FileListingWidgetClass::OnListMenuCreateNew), NULL, this);
	ParentPanel->Connect(ID_EXPLORER_LIST_CREATE_JS, wxEVT_MENU, wxCommandEventHandler(t4p::FileListingWidgetClass::OnListMenuCreateNew), NULL, this);
	ParentPanel->Connect(ID_EXPLORER_LIST_CREATE_TEXT, wxEVT_MENU, wxCommandEventHandler(t4p::FileListingWidgetClass::OnListMenuCreateNew), NULL, this);
	ParentPanel->Connect(ID_EXPLORER_LIST_CREATE_DIRECTORY, wxEVT_MENU, wxCommandEventHandler(t4p::FileListingWidgetClass::OnListMenuCreateDirectory), NULL, this);
	ParentPanel->Connect(ID_EXPLORER_LIST_SHELL, wxEVT_MENU, wxCommandEventHandler(t4p::FileListingWidgetClass::OnListMenuShell), NULL, this);
	ParentPanel->Connect(ID_EXPLORER_LIST_FILE_MANAGER, wxEVT_MENU, wxCommandEventHandler(t4p::FileListingWidgetClass::OnListMenuFileManager), NULL, this);
}

t4p::FileListingWidgetClass::~FileListingWidgetClass() {
	List->Disconnect(wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler(t4p::FileListingWidgetClass::OnListEndLabelEdit), NULL, this);
	List->Disconnect(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler(t4p::FileListingWidgetClass::OnListItemRightClick), NULL, this);
	List->Disconnect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(t4p::FileListingWidgetClass::OnListRightDown), NULL, this);

	ParentPanel->Disconnect(ID_EXPLORER_LIST_RENAME, wxEVT_MENU, wxCommandEventHandler(t4p::FileListingWidgetClass::OnListMenuRename), NULL, this);
	ParentPanel->Disconnect(ID_EXPLORER_LIST_DELETE, wxEVT_MENU, wxCommandEventHandler(t4p::FileListingWidgetClass::OnListMenuDelete), NULL, this);
	ParentPanel->Disconnect(ID_EXPLORER_LIST_CREATE_PHP, wxEVT_MENU, wxCommandEventHandler(t4p::FileListingWidgetClass::OnListMenuCreateNew), NULL, this);
	ParentPanel->Disconnect(ID_EXPLORER_LIST_CREATE_SQL, wxEVT_MENU, wxCommandEventHandler(t4p::FileListingWidgetClass::OnListMenuCreateNew), NULL, this);
	ParentPanel->Disconnect(ID_EXPLORER_LIST_CREATE_CSS, wxEVT_MENU, wxCommandEventHandler(t4p::FileListingWidgetClass::OnListMenuCreateNew), NULL, this);
	ParentPanel->Disconnect(ID_EXPLORER_LIST_CREATE_JS, wxEVT_MENU, wxCommandEventHandler(t4p::FileListingWidgetClass::OnListMenuCreateNew), NULL, this);
	ParentPanel->Disconnect(ID_EXPLORER_LIST_CREATE_TEXT, wxEVT_MENU, wxCommandEventHandler(t4p::FileListingWidgetClass::OnListMenuCreateNew), NULL, this);
	ParentPanel->Disconnect(ID_EXPLORER_LIST_CREATE_DIRECTORY, wxEVT_MENU, wxCommandEventHandler(t4p::FileListingWidgetClass::OnListMenuCreateDirectory), NULL, this);
	ParentPanel->Disconnect(ID_EXPLORER_LIST_SHELL, wxEVT_MENU, wxCommandEventHandler(t4p::FileListingWidgetClass::OnListMenuShell), NULL, this);
	ParentPanel->Disconnect(ID_EXPLORER_LIST_FILE_MANAGER, wxEVT_MENU, wxCommandEventHandler(t4p::FileListingWidgetClass::OnListMenuFileManager), NULL, this);
}

void t4p::FileListingWidgetClass::ShowDir() {
	wxFileName currentDir = FileListing->WorkingDir;
	std::vector<wxFileName> files = FileListing->Files;
	std::vector<wxFileName>& dirs = FileListing->Dirs;
												 
	List->DeleteAllItems();
	int newRowNumber = 0;

	// add the parent dir item
	wxFileName parentDir;
	parentDir.AssignDir(currentDir.GetPath());
	if (parentDir.GetDirCount() > 1) {
		parentDir.RemoveLastDir();
		if (parentDir.IsOk()) {
			wxListItem parentColumn;
			parentColumn.SetColumn(0);
			parentColumn.SetId(newRowNumber);
			parentColumn.SetImage(LIST_PARENT_FOLDER);
			parentColumn.SetMask(wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT);
			parentColumn.SetText(wxT(".."));
			List->InsertItem(parentColumn);
			newRowNumber++;
		}
	}

	// add all sub-directories
	std::vector<t4p::ProjectClass>::const_iterator p;
	std::vector<wxFileName>::const_iterator dir;
	for (dir = dirs.begin(); dir != dirs.end(); ++dir) {
		wxListItem column1;
		column1.SetColumn(0);
		column1.SetId(newRowNumber);
		column1.SetImage(LIST_FOLDER);
		column1.SetMask(wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT);
		column1.SetText(dir->GetDirs().Last());
		List->InsertItem(column1);

		newRowNumber++;
	}

	// add all files
	std::vector<wxFileName>::const_iterator file;
	for (file = files.begin(); file != files.end(); ++file) {
		wxListItem column1;
		column1.SetColumn(0);
		column1.SetId(newRowNumber);
		column1.SetImage(ListImageId(*file));
		column1.SetMask(wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT);
		column1.SetText(file->GetFullName());
			
		List->InsertItem(column1);

		newRowNumber++;
	}
	
	// set the second item to be selected (first other than parent)
	// except if the dir is empty
	if (List->GetItemCount() >= 2) {
		List->SetItemState(1, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
	}
	else if (List->GetItemCount() >= 1) {
		List->SetItemState(0, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
	}
	if (List->GetWindowStyle() & wxLC_REPORT) {
		List->SetColumnWidth(0, wxLIST_AUTOSIZE);
	}
	List->SetFocus();
}

void t4p::FileListingWidgetClass::OnListItemRightClick(wxListEvent& event) {
	long index = event.GetIndex();
	if (index != wxNOT_FOUND) {
		wxMenu menu;
		menu.Append(ID_EXPLORER_LIST_OPEN, _("Open\tENTER"), _("Open the file in the editor"), wxITEM_NORMAL);

		// cannot delete or rename the parent dir item
		if (index > 0) {
            menu.Append(ID_EXPLORER_LIST_OPEN_PARENT, _("Open parent directory\tBACK"), _("Open the parent directory"), wxITEM_NORMAL);
			menu.Append(ID_EXPLORER_LIST_RENAME, _("Rename\tF2"), _("Rename the file"), wxITEM_NORMAL);
			menu.Append(ID_EXPLORER_LIST_DELETE, _("Delete\tDEL"), _("Delete the file"), wxITEM_NORMAL);
			menu.Append(ID_EXPLORER_LIST_SHELL, _("Open Shell Here"), _("Open an external shell to this directory"), wxITEM_NORMAL);
			menu.Append(ID_EXPLORER_LIST_FILE_MANAGER, _("Open File Manager Here"), _("Opens the Operating System's file manager to this directory"), wxITEM_NORMAL);
		}
		List->PopupMenu(&menu, wxDefaultPosition);
	}
	event.Skip();
}

void t4p::FileListingWidgetClass::OnListRightDown(wxMouseEvent& event) {

	// if the right mouse button was clicked on an item let the context menu handler
	// handle this event
	wxPoint point = event.GetPosition();
	int flags = 0;
	long index = List->HitTest(point, flags, NULL);
	if (index != wxNOT_FOUND && (flags & wxLIST_HITTEST_ONITEM)) {
		event.Skip();
		return;
	}

	// if we get here then the user clicked on an empty area of the list control
	// it means user wants to create a new file
	wxMenu menu;
	menu.Append(ID_EXPLORER_LIST_CREATE_PHP, _("New PHP File"), _("Create a new PHP file in this directory"), wxITEM_NORMAL);
	menu.Append(ID_EXPLORER_LIST_CREATE_SQL, _("New SQL File"), _("Create a new SQL file in this directory"), wxITEM_NORMAL);
	menu.Append(ID_EXPLORER_LIST_CREATE_CSS, _("New CSS File"), _("Create a new CSS file in this directory"), wxITEM_NORMAL);
	menu.Append(ID_EXPLORER_LIST_CREATE_JS, _("New JS File"), _("Create a new JS file in this directory"), wxITEM_NORMAL);
	menu.Append(ID_EXPLORER_LIST_CREATE_TEXT, _("New text File"), _("Create a new text file in this directory"), wxITEM_NORMAL);
	menu.Append(ID_EXPLORER_LIST_CREATE_DIRECTORY, _("New Directory"), _("Create a new directory in this directory"), wxITEM_NORMAL);
	menu.Append(ID_EXPLORER_LIST_SHELL, _("Open Shell Here"), _("Open an external shell to this directory"), wxITEM_NORMAL);
	menu.Append(ID_EXPLORER_LIST_FILE_MANAGER, _("Open File Manager Here"), _("Opens the Operating System's file manager to this directory"), wxITEM_NORMAL);
	List->PopupMenu(&menu, event.GetPosition());
}

void t4p::FileListingWidgetClass::OnListMenuRename(wxCommandEvent& event) {
	long index = -1;
	index = List->GetNextItem(index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

	// dont allow the parent dir to be renamed
	if (index > 0) {
		
		// set the null accelerator table, so that the backspace is captured
		// by the text control
		List->SetAcceleratorTable(wxNullAcceleratorTable);

		wxTextCtrl* ctrl = List->EditLabel(index);
		
		// change the selection. select only the file name not the
		// extension. if the file only has an extension, we
		// leave it alone (by default the entire string is selected)
		wxString val = ctrl->GetValue();
		size_t index = val.find_last_of(wxT('.'));
		if (index > 0 && index != std::string::npos) {
			ctrl->SetSelection(0, index);
		}
	}
}

void t4p::FileListingWidgetClass::OnListMenuDelete(wxCommandEvent& event) {
	long index = -1;
	index = List->GetNextItem(index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	std::vector<wxFileName> dirs;
	std::vector<wxFileName> files;
	while (index != wxNOT_FOUND) {
		
		// dont allow the parent dir to be deleted
		if (index > 0) {
			wxString name = List->GetItemText(index);
			wxString fullPath = FileListing->WorkingDir.GetPathWithSep() + name;
			if (wxFileName::FileExists(fullPath)) {
				files.push_back(wxFileName(fullPath));
			}
			else if (wxFileName::DirExists(fullPath)) {

				// need to recursively delete a directory
				wxFileName dirToDelete;
				dirToDelete.AssignDir(fullPath);
				dirs.push_back(dirToDelete);
			}
		}
		index = List->GetNextItem(index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	}

	// 1 file = don;'t ask for confirmation 
	bool doDelete = false;
	if ((dirs.size() + files.size()) == 1) {
		doDelete = true;
	}
	else if (!dirs.empty() || !files.empty()) {
		wxString msg = _("Are you sure you want to delete the following?\n\n");
		std::vector<wxFileName>::const_iterator f;
		for (f = dirs.begin(); f != dirs.end(); ++f) {
			msg += f->GetPath() + wxT("\n");
		}
		for (f = files.begin(); f != files.end(); ++f) {
			msg += f->GetFullPath() + wxT("\n");
		}
		int res = wxMessageBox(msg, _("Confirm Delete"), wxCENTRE | wxYES_NO);
		doDelete = wxYES == res;
	}
	if (doDelete) {

		// perform the deletion in the background
		FileListing->StartDelete(dirs, files);
	}
}

void t4p::FileListingWidgetClass::OnListMenuCreateNew(wxCommandEvent& event) {
	wxString ext;
	std::vector<wxString> extensions;
	wxString dialogTitle;
	if (event.GetId() == ID_EXPLORER_LIST_CREATE_PHP) {
		extensions = Feature->App.Globals.FileTypes.GetPhpFileExtensions();
		if (extensions.empty()) {
			ext = wxT("*.php");
		}
		else {
			ext = extensions[0];
		}
		dialogTitle =_("Create a new PHP file");
	}
	else if (event.GetId() == ID_EXPLORER_LIST_CREATE_SQL) {
		extensions = Feature->App.Globals.FileTypes.GetSqlFileExtensions();
		if (extensions.empty()) {
			ext = wxT("*.sql");
		}
		else {
			ext = extensions[0];
		}
		dialogTitle =_("Create a new SQL file");
	}
	else if (event.GetId() == ID_EXPLORER_LIST_CREATE_CSS) {
		extensions = Feature->App.Globals.FileTypes.GetCssFileExtensions();
		if (extensions.empty()) {
			ext = wxT("*.css");
		}
		else {
			ext = extensions[0];
		}
		dialogTitle =_("Create a new CSS file");
	}
	else if (event.GetId() == ID_EXPLORER_LIST_CREATE_JS) {
		extensions = Feature->App.Globals.FileTypes.GetJsFileExtensions();
		if (extensions.empty()) {
			ext = wxT("*.js");
		}
		else {
			ext = extensions[0];
		}
		dialogTitle =_("Create a new JS file");
	}
	else if (event.GetId() == ID_EXPLORER_LIST_CREATE_TEXT) {
		ext = wxT("*.txt");
		dialogTitle =_("Create a new text file");
	}
	else if (event.GetId() == ID_EXPLORER_LIST_CREATE_DIRECTORY) {
		ext = wxT("");
		dialogTitle =_("Create a directory");
	}
	ext.Replace(wxT("*"), wxT("New File"));

	wxString newName = ext;
	t4p::ExplorerNewFileDialogClass newFileDialog(NULL, dialogTitle, FileListing->WorkingDir.GetPathWithSep(), newName);
	if (newFileDialog.ShowModal() == wxOK) {
		wxFileName newFileName(FileListing->WorkingDir.GetPath(), newName);
		wxFile file;
		if (file.Create(newFileName.GetFullPath())) {

			// list ctrl is tricky, for columns we must insertItem() then setItem() for the next columns
			int newRowNumber = List->GetItemCount();
			wxListItem column1;
			column1.SetColumn(0);
			column1.SetId(newRowNumber);
			column1.SetImage(ListImageId(newFileName));
			column1.SetMask(wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT);
			column1.SetText(newName);
			List->InsertItem(column1);

			// open the new file
			t4p::OpenFileCommandEventClass evt(newFileName.GetFullPath());
			Feature->App.EventSink.Publish(evt);
		}
		else {
			wxMessageBox(_("Could not create file: ") + newName);
		}
	}
}

void t4p::FileListingWidgetClass::OnListMenuCreateDirectory(wxCommandEvent& event) {
	wxString newName = ::wxGetTextFromUser(_("Please enter a directory name"), _("Create New Directory"), wxT(""));
	wxString forbidden = wxFileName::GetForbiddenChars();
	if (newName.find_first_of(forbidden, 0) != std::string::npos) {
		wxMessageBox(_("Please enter valid a directory name"));
		return;
	}
	if (newName.IsEmpty()) {
		return;
	}
	wxFileName newDir;
	newDir.AssignDir(FileListing->WorkingDir.GetPath());
	newDir.AppendDir(newName);
	if (newDir.DirExists()) {
		wxMessageBox(_("Directory already exists with that name. Please enter another name."));
		return;
	}
	if (wxMkdir(newDir.GetPath())) {

		// list ctrl is tricky, for columns we must insertItem() then setItem() for the next columns
		int newRowNumber = List->GetItemCount();
		wxListItem column1;
		column1.SetColumn(0);
		column1.SetId(newRowNumber);
		column1.SetImage(LIST_FOLDER);
		column1.SetMask(wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT);
		column1.SetText(newName);
		List->InsertItem(column1);
	}
	else {
		wxMessageBox(_("Could not create directory: ") + newName);
	}
}

void t4p::FileListingWidgetClass::OnListMenuShell(wxCommandEvent& event) {
	wxString cmd = Feature->ShellExecutable.GetFullPath();
	wxPlatformInfo info;
	if (info.GetOperatingSystemId() & wxOS_WINDOWS_NT) {
		cmd += wxT(" /k cd \"") + FileListing->WorkingDir.GetPath() + wxT("\"");
	}
	wxExecute(cmd, wxEXEC_ASYNC);
}

void t4p::FileListingWidgetClass::OnListMenuFileManager(wxCommandEvent& event) {
	wxString cmd = Feature->FileManagerExecutable.GetFullPath();
	wxPlatformInfo info;
	if (info.GetOperatingSystemId() & wxOS_WINDOWS_NT) {
		cmd += wxT(" ") + FileListing->WorkingDir.GetPath();
	}
	wxExecute(cmd, wxEXEC_ASYNC);
}

void t4p::FileListingWidgetClass::OnListEndLabelEdit(wxListEvent& event) {
	
	// ideally we dont need to query the file system, but cant seem to get the 
	// item image to tell if selected item is a dir or not
	wxString newName = event.GetLabel();
	long index = event.GetIndex();
	wxString name = List->GetItemText(index);
	if (newName.find_first_of(wxFileName::GetForbiddenChars(), 0) != std::string::npos) {
		event.Veto();
		return;
	}
	else if (newName.IsEmpty()) {
		event.Veto();
		return;
	}

	// dont attempt rename if they are new and old name are the same
	else if (newName != name && !newName.IsEmpty()) {	
		
		wxFileName sourceFile(FileListing->WorkingDir.GetPath(), name);
		wxFileName destFile(FileListing->WorkingDir.GetPath(), newName);
		
		FileListing->StartRename(sourceFile, newName);

		// put back the accelerators for rename / parent
		SetExplorerAccelerators(List);
	}
	else {

		// put back the accelerators for rename / parent
		SetExplorerAccelerators(List);
		event.Veto();
	}
}

int t4p::FileListingWidgetClass::ListImageId(const wxFileName& fileName) {
	wxString fullPath = fileName.GetFullPath();
	if (Feature->App.Globals.FileTypes.HasAPhpExtension(fullPath)) {
		return t4p::IMGLIST_PHP;
	}
	if (Feature->App.Globals.FileTypes.HasASqlExtension(fullPath)) {
		return t4p::IMGLIST_SQL;
	} 
	if (Feature->App.Globals.FileTypes.HasACssExtension(fullPath)) {
		return t4p::IMGLIST_CSS;
	}
	if (Feature->App.Globals.FileTypes.HasAJsExtension(fullPath)) {
		return t4p::IMGLIST_JS;
	}
	if (Feature->App.Globals.FileTypes.HasAConfigExtension(fullPath)) {
		return t4p::IMGLIST_CONFIG;
	}
	if (Feature->App.Globals.FileTypes.HasACrontabExtension(fullPath)) {
		return t4p::IMGLIST_CRONTAB;
	} 
	if (Feature->App.Globals.FileTypes.HasAYamlExtension(fullPath)) {
		return t4p::IMGLIST_YAML;
	}
	if (Feature->App.Globals.FileTypes.HasAXmlExtension(fullPath)) {
		return t4p::IMGLIST_XML;
	}
	if (Feature->App.Globals.FileTypes.HasARubyExtension(fullPath)) {
		return t4p::IMGLIST_RUBY;
	}
	if (Feature->App.Globals.FileTypes.HasALuaExtension(fullPath)) {
		return t4p::IMGLIST_LUA;
	}
	if (Feature->App.Globals.FileTypes.HasAMarkdownExtension(fullPath)) {
		return t4p::IMGLIST_MARKDOWN;
	} 
	if (Feature->App.Globals.FileTypes.HasABashExtension(fullPath)) {
		return t4p::IMGLIST_BASH;
	}
	if (Feature->App.Globals.FileTypes.HasADiffExtension(fullPath)) {
		return t4p::IMGLIST_DIFF;
	}
	if (Feature->App.Globals.FileTypes.HasAMiscExtension(fullPath)) {
		return t4p::IMGLIST_MISC;
	}
	return t4p::IMGLIST_NONE;
}

t4p::ModalExplorerPanelClass::ModalExplorerPanelClass(wxWindow* parent, int id, t4p::ExplorerFeatureClass& feature, 
	t4p::NotebookClass* notebook)
: ModalExplorerGeneratedPanelClass(parent, id) 
, FilesImageList(NULL)
, SourcesImageList(NULL)
, Feature(feature) 
, Notebook(notebook)
, FilterChoice(ID_FILTER_ALL) {
	FileListing = new t4p::FileListingClass(*this);
	FileListingWidget = new t4p::FileListingWidgetClass(List, FilesImageList, FileListing, this, &Feature);
	FilesImageList = new wxImageList(16, 16);
	
	t4p::FillWithFileType(*FilesImageList);
	FilesImageList->Add(t4p::BitmapImageAsset(wxT("folder-horizontal")));
	FilesImageList->Add(t4p::BitmapImageAsset(wxT("arrow-up")));
	List->AssignImageList(FilesImageList, wxIMAGE_LIST_SMALL);

	SourcesImageList = new wxImageList(16, 16);
	SourcesImageList->Add(t4p::BitmapImageAsset(wxT("folder-horizontal")));
	SourcesList->AssignImageList(SourcesImageList, wxIMAGE_LIST_SMALL);

	FilterButton->SetBitmapLabel(t4p::BitmapImageAsset(wxT("filter")));
	ParentButton->SetBitmapLabel(t4p::BitmapImageAsset(wxT("arrow-up")));
	RefreshButton->SetBitmapLabel(t4p::BitmapImageAsset(wxT("outline-refresh")));
	HelpButton->SetBitmap(
		wxArtProvider::GetBitmap(wxART_HELP, wxART_BUTTON, wxSize(16, 16))
	);

	std::vector<wxFileName> sourceDirs = Feature.App.Globals.AllEnabledSourceDirectories();	
	FillSourcesList(sourceDirs);

	SetExplorerAccelerators(List);
}

t4p::ModalExplorerPanelClass::~ModalExplorerPanelClass() {
	delete FileListingWidget;
	delete FileListing;
}

void t4p::ModalExplorerPanelClass::RefreshDir(const wxFileName& dir) {
	ListLabel->SetLabel(wxT(""));
	
	// when user choose ALL show hidden files too
	FileListing->StartRefresh(dir, FilterFileExtensions(), ID_FILTER_ALL == FilterChoice);
}

void t4p::ModalExplorerPanelClass::OnExplorerListComplete(t4p::ExplorerEventClass& event) {
	ShowDir();
}

void t4p::ModalExplorerPanelClass::OnRefreshClick(wxCommandEvent& event) {
	FileListing->StartRefresh(FileListing->WorkingDir, FilterFileExtensions(), ID_FILTER_ALL == FilterChoice);
}

void t4p::ModalExplorerPanelClass::OnHelpButton(wxCommandEvent& event) {
	ExplorerHelpGeneratedDialogClass dialog(this);
	dialog.ShowModal();
}

void t4p::ModalExplorerPanelClass::OnDirectoryEnter(wxCommandEvent& event) {
	wxFileName nextDir;
	nextDir.AssignDir(Directory->GetValue());
	RefreshDir(nextDir);
}

void t4p::ModalExplorerPanelClass::OnParentButtonClick(wxCommandEvent& event) {
	wxFileName curDir;
	curDir.AssignDir(Directory->GetValue());
	if (!curDir.IsOk()) {
		return;
	}

	// root directories don't have parents
	if (curDir.GetDirCount() > 0) {
		curDir.RemoveLastDir();
		if (curDir.IsOk()) {	
			RefreshDir(curDir);
		}
	}
}

void t4p::ModalExplorerPanelClass::ShowDir() {
	FileListingWidget->ShowDir();
	
	Directory->SetValue(FileListing->WorkingDir.GetPath());

	std::vector<wxFileName> files = FileListing->Files;
	std::vector<wxFileName>& dirs = FileListing->Dirs;
	int totalFiles = FileListing->TotalFiles;
	
	wxString label;
	if (files.size() == (size_t)totalFiles) {
		label = wxString::Format(wxT("%ld Files, %ld Directories"), files.size(), dirs.size());
	}
	else {
		label = wxString::Format(wxT("%ld Files, %ld Directories (%ld not shown)"), files.size(), dirs.size(), totalFiles - files.size());
	}
	ListLabel->SetLabel(label);

}

void t4p::ModalExplorerPanelClass::OnListMenuOpen(wxCommandEvent& event) {
	long index = -1;
	index = List->GetNextItem(index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (index != -1) {
		wxString name = List->GetItemText(index);

		// ideally we dont need to query the file system, but cant seem to get the 
		// item image to tell if selected item is a dir or not
		wxString fullPath = FileListing->WorkingDir.GetPathWithSep() + name;
		if (wxFileName::FileExists(fullPath)) {
			t4p::OpenFileCommandEventClass evt(fullPath);
			Feature.App.EventSink.Publish(evt);
		}
		else if (wxFileName::DirExists(fullPath)) {
			wxFileName nextDir;
			nextDir.AssignDir(FileListing->WorkingDir.GetPath());
			if (name == wxT("..")) {
				nextDir.RemoveLastDir();
			}
			else {
				nextDir.AppendDir(name);
			}
			RefreshDir(nextDir);
		}
	}
}

void t4p::ModalExplorerPanelClass::OnListItemActivated(wxListEvent& event) {
	wxString text = event.GetText();
	wxFileName nextDir;
	if (text == wxT("..")) {
		nextDir.AssignDir(FileListing->WorkingDir.GetPath());
		nextDir.RemoveLastDir();

		// parent dir click
		RefreshDir(nextDir);
		return;
	}
	else {
		
	}
	if (!OpenIfListFile(text)) {
		nextDir.AssignDir(FileListing->WorkingDir.GetPath());
		nextDir.AppendDir(text);
		RefreshDir(nextDir);
	}
}

bool t4p::ModalExplorerPanelClass::OpenIfListFile(const wxString& text) {
	
	// ideally we dont need to query the file system, but cant seem to get the 
	// item image to tell if selected item is a dir or not
	wxString fullPath = FileListing->WorkingDir.GetPathWithSep() + text;
	if (wxFileName::FileExists(fullPath)) {
		t4p::OpenFileCommandEventClass evt(fullPath);
		Feature.App.EventSink.Publish(evt);
		return true;
	}
	return false;
}

void t4p::ModalExplorerPanelClass::OnExplorerModifyComplete(t4p::ExplorerModifyEventClass &event) {
	if (FileListing->WorkingDir.GetPath() != event.GetParentDir().GetPath()) {

		// user is looking at another dir. no need to update the list items
		return;
	}

	wxWindowUpdateLocker updateLocker(this);
	if (t4p::ExplorerModifyActionClass::DELETE_FILES_DIRS == event.Action) {
		std::vector<wxFileName>::iterator f;

		// find the directories that were deleted and remove them from the list control
		for (f = event.DirsDeleted.begin(); f != event.DirsDeleted.end(); ++f) {
			
			// TODO FindItem is case insensitive but the file system may be case sensitive
			// not sure how to solve this (removing the item with the 'wrong' case)
			long index = List->FindItem(-1, f->GetDirs().Last());
			if (index != wxNOT_FOUND) {
				List->DeleteItem(index);
			}
		}
		for (f = event.FilesDeleted.begin(); f != event.FilesDeleted.end(); ++f) {
			
			// TODO FindItem is case insensitive but the file system may be case sensitive
			// not sure how to solve this (removing the item with the 'wrong' case)
			long index = List->FindItem(-1, f->GetFullName());
			if (index != wxNOT_FOUND) {
				List->DeleteItem(index);
			}
		}
		if (!event.DirsNotDeleted.empty() || !event.FilesNotDeleted.empty()) {
			wxString msg;
			if (!event.FilesNotDeleted.empty()) {
				msg += _("Could not delete the following files");
				for (f = event.DirsNotDeleted.begin(); f != event.DirsNotDeleted.end(); ++f) {
					msg += f->GetPath() + wxT("\n");
				}
			}
			if (!event.DirsNotDeleted.empty()) {
				msg += _("Could not delete the following directories");
				for (f = event.FilesNotDeleted.begin(); f != event.FilesNotDeleted.end(); ++f) {
					msg += f->GetFullPath() + wxT("\n");
				}
			}
			wxMessageBox(msg, _("Delete"));
		}
	}
	else if (t4p::ExplorerModifyActionClass::RENAME_FILE == event.Action) {
		wxFileName destFile(event.OldFile.GetPath(), event.NewName);

		if (!event.Success && wxFileName::DirExists(destFile.GetFullPath())) {

			// revert the item back to the original name
			long index = List->FindItem(-1, event.NewName);
			if (index != wxNOT_FOUND) {
				List->SetItemText(index, event.OldFile.GetFullName());
			}
			wxMessageBox(_("A directory with that name already exists"), _("Rename"));
		}
		else if (!event.Success) {
			
			// revert the item back to the original name
			long index = List->FindItem(-1, event.NewName);
			if (index != wxNOT_FOUND) {
				List->SetItemText(index, event.OldFile.GetFullName());
			}
			wxMessageBox(_("A file with that name already exists"), _("Rename"));
		}
		else if (event.Success) {
			RenamePrompt(event.OldFile, event.NewName);
		}
	}
}

void t4p::ModalExplorerPanelClass::OnFilterButtonLeftDown(wxMouseEvent& event) {
	wxPoint point = event.GetPosition();
	if (FilterButton->HitTest(point) == wxHT_WINDOW_INSIDE) {
		wxString allExtensions = Feature.App.Globals.FileTypes.PhpFileExtensionsString + wxT(";") +
			Feature.App.Globals.FileTypes.CssFileExtensionsString + wxT(";") +
			Feature.App.Globals.FileTypes.SqlFileExtensionsString  + wxT(";") + 
			Feature.App.Globals.FileTypes.JsFileExtensionsString  + wxT(";") + 
			Feature.App.Globals.FileTypes.MiscFileExtensionsString;

		wxString allFiles = wxT("*");
		wxString phpExtensions = Feature.App.Globals.FileTypes.PhpFileExtensionsString;
		wxString cssExtensions = Feature.App.Globals.FileTypes.CssFileExtensionsString;
		wxString sqlExtensions = Feature.App.Globals.FileTypes.SqlFileExtensionsString;
		wxString jsExtensions = Feature.App.Globals.FileTypes.JsFileExtensionsString;
		wxMenu menu;
	
		wxMenuItem* item;
		item = menu.AppendRadioItem(ID_FILTER_ALL_SOURCE, wxString::Format(wxT("Source Code Files (%s)"), allExtensions.c_str()), _("Show All Source Code Files"));
		item->Check(ID_FILTER_ALL_SOURCE == FilterChoice);

		item = menu.AppendRadioItem(ID_FILTER_ALL, wxString::Format(wxT("All Files (%s)"), allFiles.c_str()), _("Show All Files"));
		item->Check(ID_FILTER_ALL == FilterChoice);

		item = menu.AppendRadioItem(ID_FILTER_PHP, wxString::Format(wxT("PHP Files (%s)"), phpExtensions.c_str()), _("Show PHP Files"));
		item->Check(ID_FILTER_PHP == FilterChoice);

		item = menu.AppendRadioItem(ID_FILTER_CSS, wxString::Format(wxT("CSS Files (%s)"), cssExtensions.c_str()), _("Show CSS Files"));
		item->Check(ID_FILTER_CSS == FilterChoice);

		item = menu.AppendRadioItem(ID_FILTER_SQL, wxString::Format(wxT("SQL Files (%s)"), sqlExtensions.c_str()), _("Show SQL Files"));
		item->Check(ID_FILTER_SQL == FilterChoice);

		item = menu.AppendRadioItem(ID_FILTER_JS, wxString::Format(wxT("JS Files (%s)"), jsExtensions.c_str()), _("Show JS Files"));
		item->Check(ID_FILTER_JS == FilterChoice);

		FilterButton->PopupMenu(&menu);
	}

	// according to docs, always allow default processing of mouse down events to take place
	//
	// The handler of this event should normally call event.Skip() to allow the default processing to take 
	// place as otherwise the window under mouse wouldn't get the focus.
	event.Skip();
}

std::vector<wxString> t4p::ModalExplorerPanelClass::FilterFileExtensions() {
	std::vector<wxString> extensions;
	if (ID_FILTER_ALL_SOURCE == FilterChoice) {
		extensions = Feature.App.Globals.FileTypes.GetAllSourceFileExtensions();
	}
	else if (ID_FILTER_PHP == FilterChoice) {
		extensions = Feature.App.Globals.FileTypes.GetPhpFileExtensions();
	}
	else if (ID_FILTER_CSS == FilterChoice) {
		extensions = Feature.App.Globals.FileTypes.GetCssFileExtensions();
	}
	else if (ID_FILTER_SQL == FilterChoice) {
		extensions = Feature.App.Globals.FileTypes.GetSqlFileExtensions();
	}
	else if (ID_FILTER_JS == FilterChoice) {
		extensions = Feature.App.Globals.FileTypes.GetJsFileExtensions();
	}

	// no extension == ID_FILTER_ALL
	return extensions;
}

void t4p::ModalExplorerPanelClass::OnFilterMenuCheck(wxCommandEvent& event) {
	FilterChoice = event.GetId();
	wxFileName dir;
	dir.AssignDir(Directory->GetValue());
	RefreshDir(dir);
}

void t4p::ModalExplorerPanelClass::OnSourceActivated(wxListEvent& event) {
	long selection = event.GetIndex();
	std::vector<wxFileName> sourceDirs = Feature.App.Globals.AllEnabledSourceDirectories();
	if (selection >= 0 && selection < (long)sourceDirs.size()) {
		RefreshDir(sourceDirs[selection]);
	}
}

void t4p::ModalExplorerPanelClass::FillSourcesList(const std::vector<wxFileName> &sourceDirs) {
	SourcesList->DeleteAllItems();
	std::vector<wxFileName>::const_iterator dir;
	for (dir = sourceDirs.begin(); dir != sourceDirs.end(); ++dir) {
		
		// list ctrl is tricky, for columns we must insertItem() then setItem() for the next columns
		int newRowNumber = SourcesList->GetItemCount();
		wxString newName = dir->GetDirs().Last();
		wxListItem column1;
		column1.SetColumn(0);
		column1.SetId(newRowNumber);
		column1.SetImage(SOURCE_FOLDER);
		column1.SetMask(wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT);
		column1.SetText(newName);
		SourcesList->InsertItem(column1);
	}
	if (sourceDirs.size() == 1) {
		SourcesLabel->SetLabel(wxString::Format(wxT("%d Source Directory"), (int)sourceDirs.size()));
	}
	else {
		SourcesLabel->SetLabel(wxString::Format(wxT("%d Source Directories"), (int)sourceDirs.size()));
	}
}

void t4p::ModalExplorerPanelClass::FocusOnSourcesList() {
	if (SourcesList->GetItemCount()) {
		SourcesList->SetItemState(0, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
	}
	SourcesList->SetFocus();
}

void t4p::ModalExplorerPanelClass::RenamePrompt(const wxFileName& oldFile, const wxString& newName) {
	
	// get the code control for the file that was renamed
	// if the old file is not opened don't bother the user
	t4p::CodeControlClass* ctrl = Notebook->FindCodeControl(oldFile.GetFullPath());
	if (!ctrl) {
		return;
	}
	
	// ask the user whether they want to
	// 1. open the new file and close the old one
	// 2. open the new file and keep the old one open
	// 3. don't open the new one and close the old one 
	wxArrayString choices;
	choices.Add(_("Open the new file and close the old one"));
	choices.Add(_("Open the new file and keep the old one open"));
	choices.Add(_("Don't open the new one and close the old file"));
	
	wxFileName newFile(oldFile.GetPath(), newName);
	wxSingleChoiceDialog choiceDialog(NULL, 
			oldFile.GetFullPath() + 
			_("\nhas been renamed to \n") +
			newFile.GetFullPath() +
			_("\nWhat would you like to do?"),
			_("File Rename"), 
			choices
		);
	choiceDialog.SetWindowStyle(wxCENTER | wxOK);
	choiceDialog.SetSize(choiceDialog.GetSize().GetWidth(), choiceDialog.GetSize().GetHeight() + 40);
	if (wxID_OK == choiceDialog.ShowModal()) {
		int sel = choiceDialog.GetSelection();
		if (0 == sel || 2 == sel) {
			Notebook->DeletePage(Notebook->GetPageIndex(ctrl));
		}
		if (0 == sel || 1 == sel) {
			Notebook->LoadPage(newFile.GetFullPath());
		}
		if (1 == sel) {
			ctrl->TreatAsNew();
		}
	}
}

void t4p::ModalExplorerPanelClass::OnFsWatcher(wxFileSystemWatcherEvent& event) {
	RefreshDir(FileListing->WorkingDir);
}

t4p::ExplorerOutlinePanelClass::ExplorerOutlinePanelClass(wxWindow* parent, int id, t4p::ExplorerFeatureClass& feature, 
	t4p::NotebookClass* notebook)
: ExplorerOutlineGeneratedPanelClass(parent, id) 
, FilesImageList(NULL)
, Feature(feature) 
, Notebook(notebook)
, FilterChoice(ID_FILTER_ALL) {
	FileListing = new t4p::FileListingClass(*this);
	FileListingWidget = new t4p::FileListingWidgetClass(List, FilesImageList, FileListing, this, &Feature);
	FilesImageList = new wxImageList(16, 16);
	
	t4p::FillWithFileType(*FilesImageList);
	FilesImageList->Add(t4p::BitmapImageAsset(wxT("folder-horizontal")));
	FilesImageList->Add(t4p::BitmapImageAsset(wxT("arrow-up")));
	List->AssignImageList(FilesImageList, wxIMAGE_LIST_SMALL);

	FilterButton->SetBitmapLabel(t4p::BitmapImageAsset(wxT("filter")));
	ParentButton->SetBitmapLabel(t4p::BitmapImageAsset(wxT("arrow-up")));
	RefreshButton->SetBitmapLabel(t4p::BitmapImageAsset(wxT("outline-refresh")));
	HelpButton->SetBitmap(
		wxArtProvider::GetBitmap(wxART_HELP, wxART_BUTTON, wxSize(16, 16))
	);
	
	List->DeleteAllColumns();
	List->InsertColumn(0, _(""));
	

	SetExplorerAccelerators(List);
}

t4p::ExplorerOutlinePanelClass::~ExplorerOutlinePanelClass() {
	delete FileListingWidget;
	delete FileListing;
}

void t4p::ExplorerOutlinePanelClass::FillSourcesList(const std::vector<wxFileName>& sourceDirs) {
	Directory->Clear();
	std::vector<wxFileName>::const_iterator source;
	for (source = sourceDirs.begin(); source != sourceDirs.end(); ++source) {
		Directory->Append(source->GetPathWithSep());
	}
}


void t4p::ExplorerOutlinePanelClass::RefreshDir(const wxFileName& dir) {
	ListLabel->SetLabel(wxT(""));
	
	// when user choose ALL show hidden files too
	FileListing->StartRefresh(dir, FilterFileExtensions(), ID_FILTER_ALL == FilterChoice);
}

void t4p::ExplorerOutlinePanelClass::OnExplorerListComplete(t4p::ExplorerEventClass& event) {
	ShowDir();
}

void t4p::ExplorerOutlinePanelClass::OnRefreshClick(wxCommandEvent& event) {
	FileListing->StartRefresh(FileListing->WorkingDir, FilterFileExtensions(), ID_FILTER_ALL == FilterChoice);
}

void t4p::ExplorerOutlinePanelClass::OnHelpButton(wxCommandEvent& event) {
	ExplorerHelpGeneratedDialogClass dialog(this);
	dialog.ShowModal();
}

void t4p::ExplorerOutlinePanelClass::OnDirectoryEnter(wxCommandEvent& event) {
	wxFileName nextDir;
	nextDir.AssignDir(Directory->GetValue());
	RefreshDir(nextDir);
}

void t4p::ExplorerOutlinePanelClass::OnDirectorySelected(wxCommandEvent& event) {
	wxString value = event.GetString();
	wxFileName nextDir;
	nextDir.AssignDir(value);
	RefreshDir(nextDir);
}

void t4p::ExplorerOutlinePanelClass::OnParentButtonClick(wxCommandEvent& event) {
	wxFileName curDir;
	curDir.AssignDir(Directory->GetValue());
	if (!curDir.IsOk()) {
		return;
	}

	// root directories don't have parents
	if (curDir.GetDirCount() > 0) {
		curDir.RemoveLastDir();
		if (curDir.IsOk()) {	
			RefreshDir(curDir);
		}
	}
}

void t4p::ExplorerOutlinePanelClass::ShowDir() {
	FileListingWidget->ShowDir();
	
	Directory->SetValue(FileListing->WorkingDir.GetPath());

	std::vector<wxFileName> files = FileListing->Files;
	std::vector<wxFileName>& dirs = FileListing->Dirs;
	int totalFiles = FileListing->TotalFiles;
	
	wxString label;
	if (files.size() == (size_t)totalFiles) {
		label = wxString::Format(wxT("%ld Files, %ld Directories"), files.size(), dirs.size());
	}
	else {
		label = wxString::Format(wxT("%ld Files, %ld Directories (%ld not shown)"), files.size(), dirs.size(), totalFiles - files.size());
	}
	ListLabel->SetLabel(label);

}

void t4p::ExplorerOutlinePanelClass::OnListMenuOpen(wxCommandEvent& event) {
	long index = -1;
	index = List->GetNextItem(index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (index != -1) {
		wxString name = List->GetItemText(index);

		// ideally we dont need to query the file system, but cant seem to get the 
		// item image to tell if selected item is a dir or not
		wxString fullPath = FileListing->WorkingDir.GetPathWithSep() + name;
		if (wxFileName::FileExists(fullPath)) {
			t4p::OpenFileCommandEventClass evt(fullPath);
			Feature.App.EventSink.Publish(evt);
		}
		else if (wxFileName::DirExists(fullPath)) {
			wxFileName nextDir;
			nextDir.AssignDir(FileListing->WorkingDir.GetPath());
			if (name == wxT("..")) {
				nextDir.RemoveLastDir();
			}
			else {
				nextDir.AppendDir(name);
			}
			RefreshDir(nextDir);
		}
	}
}

void t4p::ExplorerOutlinePanelClass::OnListItemActivated(wxListEvent& event) {
	wxString text = event.GetText();
	wxFileName nextDir;
	if (text == wxT("..")) {
		nextDir.AssignDir(FileListing->WorkingDir.GetPath());
		nextDir.RemoveLastDir();

		// parent dir click
		RefreshDir(nextDir);
		return;
	}
	else {
		
	}
	if (!OpenIfListFile(text)) {
		nextDir.AssignDir(FileListing->WorkingDir.GetPath());
		nextDir.AppendDir(text);
		RefreshDir(nextDir);
	}
}

bool t4p::ExplorerOutlinePanelClass::OpenIfListFile(const wxString& text) {
	
	// ideally we dont need to query the file system, but cant seem to get the 
	// item image to tell if selected item is a dir or not
	wxString fullPath = FileListing->WorkingDir.GetPathWithSep() + text;
	if (wxFileName::FileExists(fullPath)) {
		t4p::OpenFileCommandEventClass evt(fullPath);
		Feature.App.EventSink.Publish(evt);
		return true;
	}
	return false;
}

void t4p::ExplorerOutlinePanelClass::OnExplorerModifyComplete(t4p::ExplorerModifyEventClass &event) {
	if (FileListing->WorkingDir.GetPath() != event.GetParentDir().GetPath()) {

		// user is looking at another dir. no need to update the list items
		return;
	}

	wxWindowUpdateLocker updateLocker(this);
	if (t4p::ExplorerModifyActionClass::DELETE_FILES_DIRS == event.Action) {
		std::vector<wxFileName>::iterator f;

		// find the directories that were deleted and remove them from the list control
		for (f = event.DirsDeleted.begin(); f != event.DirsDeleted.end(); ++f) {
			
			// TODO FindItem is case insensitive but the file system may be case sensitive
			// not sure how to solve this (removing the item with the 'wrong' case)
			long index = List->FindItem(-1, f->GetDirs().Last());
			if (index != wxNOT_FOUND) {
				List->DeleteItem(index);
			}
		}
		for (f = event.FilesDeleted.begin(); f != event.FilesDeleted.end(); ++f) {
			
			// TODO FindItem is case insensitive but the file system may be case sensitive
			// not sure how to solve this (removing the item with the 'wrong' case)
			long index = List->FindItem(-1, f->GetFullName());
			if (index != wxNOT_FOUND) {
				List->DeleteItem(index);
			}
		}
		if (!event.DirsNotDeleted.empty() || !event.FilesNotDeleted.empty()) {
			wxString msg;
			if (!event.FilesNotDeleted.empty()) {
				msg += _("Could not delete the following files");
				for (f = event.DirsNotDeleted.begin(); f != event.DirsNotDeleted.end(); ++f) {
					msg += f->GetPath() + wxT("\n");
				}
			}
			if (!event.DirsNotDeleted.empty()) {
				msg += _("Could not delete the following directories");
				for (f = event.FilesNotDeleted.begin(); f != event.FilesNotDeleted.end(); ++f) {
					msg += f->GetFullPath() + wxT("\n");
				}
			}
			wxMessageBox(msg, _("Delete"));
		}
	}
	else if (t4p::ExplorerModifyActionClass::RENAME_FILE == event.Action) {
		wxFileName destFile(event.OldFile.GetPath(), event.NewName);

		if (!event.Success && wxFileName::DirExists(destFile.GetFullPath())) {

			// revert the item back to the original name
			long index = List->FindItem(-1, event.NewName);
			if (index != wxNOT_FOUND) {
				List->SetItemText(index, event.OldFile.GetFullName());
			}
			wxMessageBox(_("A directory with that name already exists"), _("Rename"));
		}
		else if (!event.Success) {
			
			// revert the item back to the original name
			long index = List->FindItem(-1, event.NewName);
			if (index != wxNOT_FOUND) {
				List->SetItemText(index, event.OldFile.GetFullName());
			}
			wxMessageBox(_("A file with that name already exists"), _("Rename"));
		}
		else if (event.Success) {
			RenamePrompt(event.OldFile, event.NewName);
		}
	}
}

void t4p::ExplorerOutlinePanelClass::OnFilterButtonLeftDown(wxMouseEvent& event) {
	wxPoint point = event.GetPosition();
	if (FilterButton->HitTest(point) == wxHT_WINDOW_INSIDE) {
		wxString allExtensions = Feature.App.Globals.FileTypes.PhpFileExtensionsString + wxT(";") +
			Feature.App.Globals.FileTypes.CssFileExtensionsString + wxT(";") +
			Feature.App.Globals.FileTypes.SqlFileExtensionsString  + wxT(";") + 
			Feature.App.Globals.FileTypes.JsFileExtensionsString  + wxT(";") + 
			Feature.App.Globals.FileTypes.MiscFileExtensionsString;

		wxString allFiles = wxT("*");
		wxString phpExtensions = Feature.App.Globals.FileTypes.PhpFileExtensionsString;
		wxString cssExtensions = Feature.App.Globals.FileTypes.CssFileExtensionsString;
		wxString sqlExtensions = Feature.App.Globals.FileTypes.SqlFileExtensionsString;
		wxString jsExtensions = Feature.App.Globals.FileTypes.JsFileExtensionsString;
		wxMenu menu;
	
		wxMenuItem* item;
		item = menu.AppendRadioItem(ID_FILTER_ALL_SOURCE, wxString::Format(wxT("Source Code Files (%s)"), allExtensions.c_str()), _("Show All Source Code Files"));
		item->Check(ID_FILTER_ALL_SOURCE == FilterChoice);

		item = menu.AppendRadioItem(ID_FILTER_ALL, wxString::Format(wxT("All Files (%s)"), allFiles.c_str()), _("Show All Files"));
		item->Check(ID_FILTER_ALL == FilterChoice);

		item = menu.AppendRadioItem(ID_FILTER_PHP, wxString::Format(wxT("PHP Files (%s)"), phpExtensions.c_str()), _("Show PHP Files"));
		item->Check(ID_FILTER_PHP == FilterChoice);

		item = menu.AppendRadioItem(ID_FILTER_CSS, wxString::Format(wxT("CSS Files (%s)"), cssExtensions.c_str()), _("Show CSS Files"));
		item->Check(ID_FILTER_CSS == FilterChoice);

		item = menu.AppendRadioItem(ID_FILTER_SQL, wxString::Format(wxT("SQL Files (%s)"), sqlExtensions.c_str()), _("Show SQL Files"));
		item->Check(ID_FILTER_SQL == FilterChoice);

		item = menu.AppendRadioItem(ID_FILTER_JS, wxString::Format(wxT("JS Files (%s)"), jsExtensions.c_str()), _("Show JS Files"));
		item->Check(ID_FILTER_JS == FilterChoice);

		FilterButton->PopupMenu(&menu);
	}

	// according to docs, always allow default processing of mouse down events to take place
	//
	// The handler of this event should normally call event.Skip() to allow the default processing to take 
	// place as otherwise the window under mouse wouldn't get the focus.
	event.Skip();
}

std::vector<wxString> t4p::ExplorerOutlinePanelClass::FilterFileExtensions() {
	std::vector<wxString> extensions;
	if (ID_FILTER_ALL_SOURCE == FilterChoice) {
		extensions = Feature.App.Globals.FileTypes.GetAllSourceFileExtensions();
	}
	else if (ID_FILTER_PHP == FilterChoice) {
		extensions = Feature.App.Globals.FileTypes.GetPhpFileExtensions();
	}
	else if (ID_FILTER_CSS == FilterChoice) {
		extensions = Feature.App.Globals.FileTypes.GetCssFileExtensions();
	}
	else if (ID_FILTER_SQL == FilterChoice) {
		extensions = Feature.App.Globals.FileTypes.GetSqlFileExtensions();
	}
	else if (ID_FILTER_JS == FilterChoice) {
		extensions = Feature.App.Globals.FileTypes.GetJsFileExtensions();
	}

	// no extension == ID_FILTER_ALL
	return extensions;
}

void t4p::ExplorerOutlinePanelClass::OnFilterMenuCheck(wxCommandEvent& event) {
	FilterChoice = event.GetId();
	wxFileName dir;
	dir.AssignDir(Directory->GetValue());
	RefreshDir(dir);
}

void t4p::ExplorerOutlinePanelClass::RenamePrompt(const wxFileName& oldFile, const wxString& newName) {
	
	// get the code control for the file that was renamed
	// if the old file is not opened don't bother the user
	t4p::CodeControlClass* ctrl = Notebook->FindCodeControl(oldFile.GetFullPath());
	if (!ctrl) {
		return;
	}
	
	// ask the user whether they want to
	// 1. open the new file and close the old one
	// 2. open the new file and keep the old one open
	// 3. don't open the new one and close the old one 
	wxArrayString choices;
	choices.Add(_("Open the new file and close the old one"));
	choices.Add(_("Open the new file and keep the old one open"));
	choices.Add(_("Don't open the new one and close the old file"));
	
	wxFileName newFile(oldFile.GetPath(), newName);
	wxSingleChoiceDialog choiceDialog(NULL, 
			oldFile.GetFullPath() + 
			_("\nhas been renamed to \n") +
			newFile.GetFullPath() +
			_("\nWhat would you like to do?"),
			_("File Rename"), 
			choices
		);
	choiceDialog.SetWindowStyle(wxCENTER | wxOK);
	choiceDialog.SetSize(choiceDialog.GetSize().GetWidth(), choiceDialog.GetSize().GetHeight() + 40);
	if (wxID_OK == choiceDialog.ShowModal()) {
		int sel = choiceDialog.GetSelection();
		if (0 == sel || 2 == sel) {
			Notebook->DeletePage(Notebook->GetPageIndex(ctrl));
		}
		if (0 == sel || 1 == sel) {
			Notebook->LoadPage(newFile.GetFullPath());
		}
		if (1 == sel) {
			ctrl->TreatAsNew();
		}
	}
}

void t4p::ExplorerOutlinePanelClass::OnFsWatcher(wxFileSystemWatcherEvent& event) {
	RefreshDir(FileListing->WorkingDir);
}

t4p::ExplorerEventClass::ExplorerEventClass(int eventId, const wxFileName& dir, const std::vector<wxFileName>& files, 
												  const std::vector<wxFileName>& subDirs, const wxString& error, int totalFiles,
												  int totalSubDirs)
: wxEvent(eventId, t4p::EVENT_EXPLORER)
, Dir()
, Files()
, SubDirs()
, Error() 
, TotalFiles(totalFiles) 
, TotalSubDirs(totalSubDirs) {
	
	// clone filenames they contain wxStrings; no thread-safe
	Dir = t4p::FileNameCopy(dir);
	Files = t4p::DeepCopyFileNames(files);
	SubDirs = t4p::DeepCopyFileNames(subDirs);
	Error = Error.c_str();
}

wxEvent* t4p::ExplorerEventClass::Clone() const {
	t4p::ExplorerEventClass* event = new t4p::ExplorerEventClass(GetId(), Dir, Files, SubDirs, Error, TotalFiles, TotalSubDirs);
	return event;
}

t4p::ExplorerFileSystemActionClass::ExplorerFileSystemActionClass(t4p::RunningThreadsClass& runningThreads, int eventId)
: ActionClass(runningThreads, eventId) 
, Dir()
, Extensions()
, DoHidden(false) {

}

wxString t4p::ExplorerFileSystemActionClass::GetLabel() const {
	return wxT("Explorer");
}

void t4p::ExplorerFileSystemActionClass::Directory(const wxFileName& dir, const std::vector<wxString>& extensions, bool doHidden) {
	Dir = t4p::FileNameCopy(dir);
	Extensions.clear(); 
	t4p::DeepCopy(Extensions, extensions);
	DoHidden = doHidden;
}

void t4p::ExplorerFileSystemActionClass::BackgroundWork() {
	std::vector<wxFileName> files;
	std::vector<wxFileName> subDirs;
	wxString error;
	int totalFiles = 0;
	int totalSubDirs = 0;
	wxString name;
	int flags = DoHidden ? (wxDIR_FILES | wxDIR_HIDDEN) : wxDIR_FILES;
	wxDir fileDir;
	if (fileDir.Open(Dir.GetPath())) {
		if (fileDir.GetFirst(&name, wxEmptyString, flags)) {
			totalFiles++;
			if (MatchesWildcards(name)) {
				wxFileName f(Dir.GetPath(), name);
				files.push_back(f);
			}
			while (fileDir.GetNext(&name) && !IsCancelled()) {
				totalFiles++;
				if (MatchesWildcards(name)) {
					wxFileName nextFile(Dir.GetPath(), name);
					files.push_back(nextFile);
				}
			}
		}
	}
	else {
		error = _("Could not open directory:") + Dir.GetFullPath();
	}

	// now get sub directories
	if (!IsCancelled()) {
		flags = DoHidden ? (wxDIR_DIRS | wxDIR_HIDDEN) : wxDIR_DIRS;
		wxDir dirDir;
		if (dirDir.Open(Dir.GetFullPath())) {
			if (dirDir.GetFirst(&name, wxEmptyString, flags)) {
				totalSubDirs++;
				wxFileName d;
				d.AssignDir(Dir.GetPathWithSep() + name);
				subDirs.push_back(d);
				while (dirDir.GetNext(&name) && !IsCancelled()) {
					totalSubDirs++;
					wxFileName nextDir;
					nextDir.AssignDir(Dir.GetPathWithSep() + name);
					subDirs.push_back(nextDir);
				}
			}
		}
		else {
			error = _("Could not open directory:") + Dir.GetFullPath();
		}
	}
	if (!IsCancelled()) {
		std::sort(files.begin(), files.end(), FileNameCmp);
		std::sort(subDirs.begin(), subDirs.end(), DirNameCmp);

		// PostEvent() will set the correct ID
		t4p::ExplorerEventClass evt(wxID_ANY, Dir, files, subDirs, error, totalFiles, totalSubDirs);
		PostEvent(evt);
	}
}

bool t4p::ExplorerFileSystemActionClass::MatchesWildcards(const wxString& fileName) {
	if (Extensions.empty()) {
		return true;
	}
	for (size_t i = 0; i < Extensions.size(); ++i) {
		if (wxMatchWild(Extensions[i], fileName)) {
			return true;
		}
	}
	return false;
}

t4p::ExplorerModifyActionClass::ExplorerModifyActionClass(t4p::RunningThreadsClass& runningThreads,
																int eventId)
: ActionClass(runningThreads, eventId) 
, Action(NONE)
, Dirs()
, Files()
, OldFile()
, NewName() {
}

void t4p::ExplorerModifyActionClass::SetFilesToRemove(const std::vector<wxFileName>& dirs, const std::vector<wxFileName>& files) {
	Action = DELETE_FILES_DIRS;

	// make sure to clone
	Dirs = t4p::DeepCopyFileNames(dirs);
	Files = t4p::DeepCopyFileNames(files);
}

void t4p::ExplorerModifyActionClass::SetFileToRename(const wxFileName& file, const wxString& newName) {
	Action = RENAME_FILE;

	// make sure to clone
	OldFile.Assign(file.GetFullPath());
	NewName = newName.c_str();
}

void t4p::ExplorerModifyActionClass::BackgroundWork() {
	wxFileName parentDir;
	wxString name;
	bool totalSuccess = true;
	std::vector<wxFileName> dirsDeleted;
	std::vector<wxFileName> dirsNotDeleted;
	std::vector<wxFileName> filesDeleted;
	std::vector<wxFileName> filesNotDeleted;
	if (t4p::ExplorerModifyActionClass::DELETE_FILES_DIRS == Action) {
		std::vector<wxFileName>::iterator d;
		for (d = Dirs.begin(); d != Dirs.end(); ++d) {
			bool success = t4p::RecursiveRmDir(d->GetPath());
			if (success) {
				wxFileName wxFileName;
				wxFileName.AssignDir(d->GetPath());
				dirsDeleted.push_back(wxFileName);
			}
			else {
				wxFileName wxFileName;
				wxFileName.AssignDir(d->GetPath());
				dirsNotDeleted.push_back(wxFileName);
			}
			totalSuccess &= success;
		}
		std::vector<wxFileName>::iterator f;
		for (f = Files.begin(); f != Files.end(); ++f) {
			bool success = wxRemoveFile(f->GetFullPath());
			if (success) {
				wxFileName deletedFile(f->GetFullPath());
				filesDeleted.push_back(deletedFile);
			}
			else {
				wxFileName deletedFile(f->GetFullPath());
				filesNotDeleted.push_back(deletedFile);
			}
			totalSuccess &= success;
		}
		t4p::ExplorerModifyEventClass modEvent(GetEventId(), 
			dirsDeleted, filesDeleted, dirsNotDeleted, filesNotDeleted, totalSuccess);
		PostEvent(modEvent);
	}
	else if (t4p::ExplorerModifyActionClass::RENAME_FILE == Action) {
		wxFileName destFile(OldFile.GetPath(), NewName);
		bool success = wxRenameFile(OldFile.GetFullPath(), destFile.GetFullPath(), false);

		t4p::ExplorerModifyEventClass modEvent(GetEventId(), 
			OldFile, NewName, success);
		PostEvent(modEvent);
	}
}
wxString t4p::ExplorerModifyActionClass::GetLabel() const {
	return wxT("File System Modification");
}

t4p::ExplorerModifyEventClass::ExplorerModifyEventClass(int eventId, const wxFileName &oldFile, 
															  const wxString &newName, bool success) 
: wxEvent(eventId, t4p::EVENT_EXPLORER_MODIFY)
, OldFile(oldFile.GetFullPath())
, NewName(newName.c_str())
, DirsDeleted()
, FilesDeleted()
, DirsNotDeleted()
, FilesNotDeleted()
, Action(t4p::ExplorerModifyActionClass::RENAME_FILE)
, Success(success)
{

}

t4p::ExplorerModifyEventClass::ExplorerModifyEventClass(int eventId, 
															  const std::vector<wxFileName>& dirsDeleted, const std::vector<wxFileName>& filesDeleted,
															  const std::vector<wxFileName>& dirsNotDeleted, const std::vector<wxFileName>& filesNotDeleted,
															  bool success)
: wxEvent(eventId, t4p::EVENT_EXPLORER_MODIFY)
, OldFile()
, NewName()
, DirsDeleted()
, FilesDeleted()
, DirsNotDeleted()
, FilesNotDeleted()
, Action(t4p::ExplorerModifyActionClass::DELETE_FILES_DIRS)
, Success(success)
{
	DirsDeleted = t4p::DeepCopyFileNames(dirsDeleted);
	DirsNotDeleted = t4p::DeepCopyFileNames(dirsNotDeleted);
	FilesDeleted = t4p::DeepCopyFileNames(filesDeleted);
	FilesNotDeleted = t4p::DeepCopyFileNames(filesNotDeleted);
}

wxFileName t4p::ExplorerModifyEventClass::GetParentDir() const {
	wxFileName parentDir;
	if (Action == t4p::ExplorerModifyActionClass::RENAME_FILE) {
		parentDir.AssignDir(OldFile.GetPath());
		return parentDir;
	}
	else if (!DirsDeleted.empty()) {
		parentDir.AssignDir(DirsDeleted[0].GetPath());
		parentDir.RemoveLastDir();
		return parentDir;
	}
	else if (!DirsNotDeleted.empty()) {
		parentDir.AssignDir(DirsNotDeleted[0].GetPath());
		parentDir.RemoveLastDir();
		return parentDir;
	}
	else if (!FilesDeleted.empty()) {
		parentDir.AssignDir(FilesDeleted[0].GetPath());
		return parentDir;
	}
	else if (!FilesNotDeleted.empty()) {
		parentDir.AssignDir(FilesNotDeleted[0].GetPath());
		return parentDir;
	}
	return parentDir;
}

wxEvent* t4p::ExplorerModifyEventClass::Clone() const {
	if (Action == t4p::ExplorerModifyActionClass::RENAME_FILE) {
		return new t4p::ExplorerModifyEventClass(
			GetId(), OldFile, NewName, Success
		);
	}
	return new t4p::ExplorerModifyEventClass(
		GetId(), DirsDeleted, FilesDeleted, DirsNotDeleted, FilesNotDeleted, Success
	);
}

t4p::ExplorerOptionsPanelClass::ExplorerOptionsPanelClass(wxWindow* parent, int id, t4p::ExplorerFeatureClass& feature)
	: ExplorerOptionsGeneratedPanelClass(parent, id)
	, Feature(feature) {
	t4p::FilePickerValidatorClass fileManagerValidator(&feature.FileManagerExecutable);
	FileManager->SetValidator(fileManagerValidator);
	t4p::FilePickerValidatorClass shellValidator(&feature.ShellExecutable);
	Shell->SetValidator(shellValidator);	

	TransferDataToWindow();
}


t4p::ExplorerNewFileDialogClass::ExplorerNewFileDialogClass(wxWindow* parent, const wxString& title, 
	const wxString& currentDir, wxString& fileName)
: ExplorerNewFileGeneratedDialogClass(parent, wxID_ANY, title)
, CurrentDir(currentDir)
, FileName(fileName) {
	
	FileNameText->SetValue(fileName);
	size_t index = FileName.find_last_of(wxT('.'));
	FileNameText->SetFocus();
	FileNameText->SetSelection(0, 0);
	if (index > 0 && index != std::string::npos) {
		FileNameText->SetSelection(0, index);
	}
	else {
		FileNameText->SetSelection(0, FileName.Length() - 1);
	}
}

void t4p::ExplorerNewFileDialogClass::OnOkButton(wxCommandEvent& event) {
	wxString newName = FileNameText->GetValue();
	wxString forbidden = wxFileName::GetForbiddenChars();
	if (newName.find_first_of(forbidden, 0) != std::string::npos) {
		wxMessageBox(_("Please enter valid a file name"), _("Error"));
		return;
	}
	if (newName.IsEmpty()) {
		wxMessageBox(_("Please enter a file name"), _("Error"));
		return;
	}
	wxFileName newFileName(CurrentDir, newName);
	if (newFileName.FileExists()) {
		wxMessageBox(_("File name already exists. Please enter another name."), _("Error"));
		return;
	}
	FileName = newName;
	EndModal(wxOK);
}

void t4p::ExplorerNewFileDialogClass::OnTextEnter(wxCommandEvent& event) {
	OnOkButton(event);
}


const wxEventType t4p::EVENT_EXPLORER = wxNewEventType();
const wxEventType t4p::EVENT_EXPLORER_MODIFY = wxNewEventType();

BEGIN_EVENT_TABLE(t4p::ExplorerFeatureClass, t4p::FeatureClass)
	EVT_MENU(t4p::MENU_EXPLORER + 1, t4p::ExplorerFeatureClass::OnProjectExploreOpenFile)
	EVT_MENU(t4p::MENU_EXPLORER + 2, t4p::ExplorerFeatureClass::OnProjectExplore)
	EVT_MENU(t4p::MENU_EXPLORER + 4, t4p::ExplorerFeatureClass::OnProjectOutline)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(t4p::MENU_EXPLORER + 2, t4p::ExplorerFeatureClass::OnExplorerToolDropDown)
	EVT_MENU_RANGE(t4p::MENU_EXPLORER + 3, t4p::MENU_EXPLORER + 50, t4p::ExplorerFeatureClass::OnExplorerProjectMenu)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::ExplorerFeatureClass::OnAppPreferencesSaved)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_EXTERNALLY_UPDATED, t4p::ExplorerFeatureClass::OnAppPreferencesSaved)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PROJECT_CREATED, t4p::ExplorerFeatureClass::OnAppProjectCreated)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::ModalExplorerPanelClass, ModalExplorerGeneratedPanelClass)
	EVT_MENU(ID_EXPLORER_LIST_OPEN, t4p::ModalExplorerPanelClass::OnListMenuOpen)
	EVT_MENU(ID_EXPLORER_LIST_OPEN_PARENT, t4p::ModalExplorerPanelClass::OnParentButtonClick)
	EVT_EXPLORER_MODIFY_COMPLETE(ID_EXPLORER_MODIFY, t4p::ModalExplorerPanelClass::OnExplorerModifyComplete)

	EVT_MENU(ID_FILTER_ALL, t4p::ModalExplorerPanelClass::OnFilterMenuCheck)
	EVT_MENU(ID_FILTER_ALL_SOURCE, t4p::ModalExplorerPanelClass::OnFilterMenuCheck)
	EVT_MENU(ID_FILTER_PHP, t4p::ModalExplorerPanelClass::OnFilterMenuCheck)
	EVT_MENU(ID_FILTER_CSS, t4p::ModalExplorerPanelClass::OnFilterMenuCheck)
	EVT_MENU(ID_FILTER_SQL, t4p::ModalExplorerPanelClass::OnFilterMenuCheck)
	EVT_MENU(ID_FILTER_JS, t4p::ModalExplorerPanelClass::OnFilterMenuCheck)
	EVT_FSWATCHER(wxID_ANY, t4p::ModalExplorerPanelClass::OnFsWatcher)
	
	EVT_EXPLORER_COMPLETE(ID_EXPLORER_LIST_ACTION, t4p::ModalExplorerPanelClass::OnExplorerListComplete)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::ExplorerOutlinePanelClass, ExplorerOutlineGeneratedPanelClass)
	EVT_MENU(ID_EXPLORER_LIST_OPEN, t4p::ExplorerOutlinePanelClass::OnListMenuOpen)
	EVT_MENU(ID_EXPLORER_LIST_OPEN_PARENT, t4p::ExplorerOutlinePanelClass::OnParentButtonClick)
	EVT_EXPLORER_MODIFY_COMPLETE(ID_EXPLORER_MODIFY, t4p::ExplorerOutlinePanelClass::OnExplorerModifyComplete)

	EVT_MENU(ID_FILTER_ALL, t4p::ExplorerOutlinePanelClass::OnFilterMenuCheck)
	EVT_MENU(ID_FILTER_ALL_SOURCE, t4p::ExplorerOutlinePanelClass::OnFilterMenuCheck)
	EVT_MENU(ID_FILTER_PHP, t4p::ExplorerOutlinePanelClass::OnFilterMenuCheck)
	EVT_MENU(ID_FILTER_CSS, t4p::ExplorerOutlinePanelClass::OnFilterMenuCheck)
	EVT_MENU(ID_FILTER_SQL, t4p::ExplorerOutlinePanelClass::OnFilterMenuCheck)
	EVT_MENU(ID_FILTER_JS, t4p::ExplorerOutlinePanelClass::OnFilterMenuCheck)
	EVT_FSWATCHER(wxID_ANY, t4p::ExplorerOutlinePanelClass::OnFsWatcher)
	
	EVT_EXPLORER_COMPLETE(ID_EXPLORER_LIST_ACTION, t4p::ExplorerOutlinePanelClass::OnExplorerListComplete)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::FileListingClass, wxEvtHandler)
	EVT_EXPLORER_COMPLETE(ID_EXPLORER_LIST_ACTION, t4p::FileListingClass::OnExplorerListComplete)
	EVT_EXPLORER_MODIFY_COMPLETE(ID_EXPLORER_MODIFY, t4p::FileListingClass::OnExplorerModifyComplete)
	EVT_FSWATCHER(wxID_ANY, t4p::FileListingClass::OnFsWatcher)
END_EVENT_TABLE()
