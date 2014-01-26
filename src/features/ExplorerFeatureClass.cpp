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
#include <MvcEditor.h>
#include <wx/file.h>
#include <wx/wupdlock.h>
#include <wx/stdpaths.h>
#include <algorithm>

static int ID_EXPLORER_PANEL = wxNewId();
static int ID_EXPLORER_LIST_ACTION = wxNewId();
static int ID_EXPLORER_LIST_OPEN = wxNewId();
static int ID_EXPLORER_LIST_RENAME = wxNewId();
static int ID_EXPLORER_LIST_DELETE = wxNewId();
static int ID_EXPLORER_LIST_CREATE_PHP = wxNewId();
static int ID_EXPLORER_LIST_CREATE_SQL = wxNewId();
static int ID_EXPLORER_LIST_CREATE_CSS = wxNewId();
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

mvceditor::ExplorerFeatureClass::ExplorerFeatureClass(mvceditor::AppClass& app) 
	: FeatureClass(app) {
	wxPlatformInfo info;
	switch (info.GetOperatingSystemId()) {
		case wxOS_WINDOWS_NT:
			FileManagerExecutable = wxT("explorer.exe");
			ShellExecutable = wxT("cmd.exe");
			break;
		case wxOS_UNIX:
		case wxOS_UNIX_LINUX:
			FileManagerExecutable = wxT("nautilus");
			ShellExecutable = wxT("sh");
			break;
		default:
			FileManagerExecutable = wxT("explorer");
			ShellExecutable = wxT("sh");
	}
}

void mvceditor::ExplorerFeatureClass::AddViewMenuItems(wxMenu* viewMenu) {
	viewMenu->Append(mvceditor::MENU_EXPLORER + 1, _("Explore Open File\tCTRL+ALT+E"), _("Open An explorer window in the currently opened file"), wxITEM_NORMAL);
	viewMenu->Append(mvceditor::MENU_EXPLORER + 2, _("Explore Sources\tCTRL+SHIFT+E"), _("Open An explorer window"), wxITEM_NORMAL);
}


void mvceditor::ExplorerFeatureClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_EXPLORER + 1] = wxT("Project-Explore File");
	menuItemIds[mvceditor::MENU_EXPLORER + 2] = wxT("Project-Explore Sources");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::ExplorerFeatureClass::AddWindows() {
	ExplorerToolBar = new wxAuiToolBar(GetMainWindow(), wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE |
                                         wxAUI_TB_TEXT |
                                         wxAUI_TB_HORZ_TEXT);
	ExplorerToolBar->SetFont(App.Preferences.ApplicationFont);
    ExplorerToolBar->SetToolBitmapSize(wxSize(16,16));
    
	wxBitmap bmp = mvceditor::IconImageAsset(wxT("explore"));
	wxBitmap bmpOpen = mvceditor::IconImageAsset(wxT("explore-open-document"));
	
	
	ExplorerToolBar->AddTool(mvceditor::MENU_EXPLORER + 1, _("Explore Open File"), bmpOpen, _("Open An explorer window in the currently opened file"));
	ExplorerToolBar->AddTool(mvceditor::MENU_EXPLORER + 2, _("Explore Projects"), bmp, _("Open An explorer window in the Project Root"));
	ExplorerToolBar->SetToolDropDown(mvceditor::MENU_EXPLORER + 2, true);
	ExplorerToolBar->SetOverflowVisible(false);
    ExplorerToolBar->Realize();

	AuiManager->AddPane(ExplorerToolBar, wxAuiPaneInfo()
		.ToolbarPane().Top().Row(1).Position(2)
		.LeftDockable(false).RightDockable(false)
		.Gripper(false).CaptionVisible(false).CloseButton(false).DockFixed(true)
		.PaneBorder(true).Floatable(false)
	);
}

void mvceditor::ExplorerFeatureClass::LoadPreferences(wxConfigBase *config) {
	wxString s;
	config->Read(wxT("/Explorer/FileManagerExecutable"), &s);
	FileManagerExecutable.Assign(s);
	s = wxT("");
	config->Read(wxT("/Explorer/ShellExecutable"), &s);
	ShellExecutable.Assign(s);
}

void mvceditor::ExplorerFeatureClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
	ExplorerOptionsPanelClass* panel = new ExplorerOptionsPanelClass(parent, wxID_ANY, *this);
	parent->AddPage(panel, wxT("Explorer"));
}

void mvceditor::ExplorerFeatureClass::OnExplorerProjectMenu(wxCommandEvent& event) {
	wxWindow* window = FindToolsWindow(ID_EXPLORER_PANEL);
	mvceditor::ModalExplorerPanelClass* panel = NULL;
	if (!window) {
		panel =  new mvceditor::ModalExplorerPanelClass(GetToolsNotebook(), ID_EXPLORER_PANEL, *this);
		AddToolsWindow(panel, _("Explorer"));
	}
	else {
		panel = (mvceditor::ModalExplorerPanelClass*)window;
		SetFocusToToolsWindow(panel);
	}
	
	// show the selected project
	size_t index = event.GetId() - mvceditor::MENU_EXPLORER - 3;
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
		panel->RefreshDir(paths.GetDocumentsDir());
	}
}

void mvceditor::ExplorerFeatureClass::OnProjectExplore(wxCommandEvent& event) {
	SourceDirs = App.Globals.AllEnabledSources();
	wxCommandEvent cmdEvt;
	if (SourceDirs.size() == 1) {
	
		// only 1 source dir, just open it now
		cmdEvt.SetId(mvceditor::MENU_EXPLORER + 3);
	}
	OnExplorerProjectMenu(cmdEvt);
}

void mvceditor::ExplorerFeatureClass::OnExplorerToolDropDown(wxAuiToolBarEvent& event) {
	if (!event.IsDropDownClicked()) {
		return;
	}
	ExplorerToolBar->SetToolSticky(event.GetId(), true);
		
	// gather all enabled source directories
	SourceDirs = App.Globals.AllEnabledSources();
	
	// create the popup menu that contains all the available browser names
	wxMenu projectMenu;
	for (size_t index = 0; index < SourceDirs.size(); ++index) {
		projectMenu.Append(mvceditor::MENU_EXPLORER + 3 + index, SourceDirs[index].RootDirectory.GetDirs().Last());
	}
	
	// line up our menu with the button
	wxRect rect = ExplorerToolBar->GetToolRect(event.GetId());
	wxPoint pt = ExplorerToolBar->ClientToScreen(rect.GetBottomLeft());
	pt = GetMainWindow()->ScreenToClient(pt);
	GetMainWindow()->PopupMenu(&projectMenu, pt);

	// make sure the button is "un-stuck"
	ExplorerToolBar->SetToolSticky(event.GetId(), false);
}

void mvceditor::ExplorerFeatureClass::OnProjectExploreOpenFile(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeCtrl = GetCurrentCodeControl();
	if (!codeCtrl) {
		return;
	}
	wxString openFile = codeCtrl->GetFileName();
	if (openFile.empty()) {
		return;
	}
	wxWindow* window = FindToolsWindow(ID_EXPLORER_PANEL);
	mvceditor::ModalExplorerPanelClass* panel = NULL;
	if (!window) {
		panel =  new mvceditor::ModalExplorerPanelClass(GetToolsNotebook(), ID_EXPLORER_PANEL, *this);
		AddToolsWindow(panel, _("Explorer"));
	}
	else {
		panel = (mvceditor::ModalExplorerPanelClass*)window;
		SetFocusToToolsWindow(panel);
	}
	wxFileName fileName(openFile);
	wxFileName dir;
	dir.AssignDir(fileName.GetPath());
	panel->RefreshDir(dir);
}

void mvceditor::ExplorerFeatureClass::OnExplorerListComplete(mvceditor::ExplorerEventClass& event) {
	mvceditor::ModalExplorerPanelClass* panel = NULL;
	wxWindow* window = FindToolsWindow(ID_EXPLORER_PANEL);
	if (window) {
		panel = (mvceditor::ModalExplorerPanelClass*)window;
		panel->ShowDir(event.Dir, event.Files, event.SubDirs, event.TotalFiles, event.TotalSubDirs);
	}
}

void mvceditor::ExplorerFeatureClass::OnAppPreferencesSaved(wxCommandEvent& event) {
	mvceditor::ModalExplorerPanelClass* panel = NULL;
	wxWindow* window = FindToolsWindow(ID_EXPLORER_PANEL);
	if (window) {
		std::vector<wxFileName> sourceDirs = App.Globals.AllEnabledSourceDirectories();	
	
		panel = (mvceditor::ModalExplorerPanelClass*)window;
		panel->FillSourcesList(sourceDirs);
	}

	wxConfigBase *config = wxConfig::Get();
	config->Write(wxT("/Explorer/FileManagerExecutable"), FileManagerExecutable.GetFullPath());
	config->Write(wxT("/Explorer/ShellExecutable"), ShellExecutable.GetFullPath());
}

mvceditor::ModalExplorerPanelClass::ModalExplorerPanelClass(wxWindow* parent, int id, mvceditor::ExplorerFeatureClass& feature)
: ModalExplorerGeneratedPanelClass(parent, id) 
, CurrentListDir()
, FilesImageList(NULL)
, SourcesImageList(NULL)
, Watcher(NULL)
, Feature(feature) 
, RunningThreads()
, FilterChoice(ID_FILTER_ALL) {
	FilesImageList = new wxImageList(16, 16);
	FilesImageList->Add(mvceditor::IconImageAsset(wxT("folder-horizontal")));
	FilesImageList->Add(mvceditor::IconImageAsset(wxT("arrow-up")));
	FilesImageList->Add(mvceditor::IconImageAsset(wxT("document-php")));
	FilesImageList->Add(mvceditor::IconImageAsset(wxT("document-sql")));
	FilesImageList->Add(mvceditor::IconImageAsset(wxT("document-css")));
	FilesImageList->Add(mvceditor::IconImageAsset(wxT("document-text")));
	FilesImageList->Add(mvceditor::IconImageAsset(wxT("document-blank")));
	List->AssignImageList(FilesImageList, wxIMAGE_LIST_SMALL);

	SourcesImageList = new wxImageList(16, 16);
	SourcesImageList->Add(mvceditor::IconImageAsset(wxT("folder-horizontal")));
	SourcesList->AssignImageList(SourcesImageList, wxIMAGE_LIST_SMALL);

	FilterButton->SetBitmapLabel(mvceditor::IconImageAsset(wxT("filter")));
	ParentButton->SetBitmapLabel(mvceditor::IconImageAsset(wxT("arrow-up")));
	RefreshButton->SetBitmapLabel(mvceditor::IconImageAsset(wxT("outline-refresh")));

	RunningThreads.SetMaxThreads(1);
	RunningThreads.AddEventHandler(this);
	RunningThreads.AddEventHandler(&Feature);

	std::vector<wxFileName> sourceDirs = feature.App.Globals.AllEnabledSourceDirectories();	
	FillSourcesList(sourceDirs);
}

mvceditor::ModalExplorerPanelClass::~ModalExplorerPanelClass() {
	RunningThreads.RemoveEventHandler(this);
	RunningThreads.RemoveEventHandler(&Feature);
	RunningThreads.Shutdown();
	if (Watcher) {
		delete Watcher;
	}
}

void mvceditor::ModalExplorerPanelClass::RefreshDir(const wxFileName& dir) {
	ListLabel->SetLabel(wxT(""));
	mvceditor::ExplorerFileSystemActionClass* action = new mvceditor::ExplorerFileSystemActionClass(RunningThreads, ID_EXPLORER_LIST_ACTION);

	// when user choose ALL show hidden files too
	action->Directory(dir, FilterFileExtensions(), ID_FILTER_ALL == FilterChoice);
	RunningThreads.Queue(action);
}

void mvceditor::ModalExplorerPanelClass::OnRefreshClick(wxCommandEvent& event) {
	RefreshDir(CurrentListDir);
}

void mvceditor::ModalExplorerPanelClass::OnDirectoryEnter(wxCommandEvent& event) {
	wxFileName nextDir;
	nextDir.AssignDir(Directory->GetValue());
	RefreshDir(nextDir);
}

void mvceditor::ModalExplorerPanelClass::OnParentButtonClick(wxCommandEvent& event) {
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

void mvceditor::ModalExplorerPanelClass::OnListItemActivated(wxListEvent& event) {
	wxString text = event.GetText();
	wxFileName nextDir;
	if (text == wxT("..")) {
		nextDir.AssignDir(CurrentListDir.GetPath());
		nextDir.RemoveLastDir();

		// parent dir click
		RefreshDir(nextDir);
		return;
	}
	else {
		
	}
	if (!OpenIfListFile(text)) {
		nextDir.AssignDir(CurrentListDir.GetPath());
		nextDir.AppendDir(text);
		RefreshDir(nextDir);
	}	
}

void mvceditor::ModalExplorerPanelClass::OnListItemRightClick(wxListEvent& event) {
	long index = event.GetIndex();
	if (index != wxNOT_FOUND) {
		wxMenu menu;
		menu.Append(ID_EXPLORER_LIST_OPEN, _("Open\tENTER"), _("Open the file in the editor"), wxITEM_NORMAL);

		// cannot delete or rename the parent dir item
		if (index > 0) {
			menu.Append(ID_EXPLORER_LIST_RENAME, _("Rename\tF2"), _("Rename the file"), wxITEM_NORMAL);
			menu.Append(ID_EXPLORER_LIST_DELETE, _("Delete\tDEL"), _("Delete the file"), wxITEM_NORMAL);
			menu.Append(ID_EXPLORER_LIST_SHELL, _("Open Shell Here"), _("Open an external shell to this directory"), wxITEM_NORMAL);
			menu.Append(ID_EXPLORER_LIST_FILE_MANAGER, _("Open File Manager Here"), _("Opens the Operating System's file manager to this directory"), wxITEM_NORMAL);
		}
		this->PopupMenu(&menu, wxDefaultPosition);
	}
	event.Skip();
}

void mvceditor::ModalExplorerPanelClass::OnListRightDown(wxMouseEvent& event) {

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
	menu.Append(ID_EXPLORER_LIST_CREATE_TEXT, _("New text File"), _("Create a new text file in this directory"), wxITEM_NORMAL);
	menu.Append(ID_EXPLORER_LIST_CREATE_DIRECTORY, _("New Directory"), _("Create a new directory in this directory"), wxITEM_NORMAL);
	menu.Append(ID_EXPLORER_LIST_SHELL, _("Open Shell Here"), _("Open an external shell to this directory"), wxITEM_NORMAL);
	menu.Append(ID_EXPLORER_LIST_FILE_MANAGER, _("Open File Manager Here"), _("Opens the Operating System's file manager to this directory"), wxITEM_NORMAL);
	this->PopupMenu(&menu, event.GetPosition());
}

void mvceditor::ModalExplorerPanelClass::OnListKeyDown(wxKeyEvent& event) {
	int code = event.GetKeyCode();
	wxCommandEvent evt;
	if (WXK_DELETE == code) {
		OnListMenuDelete(evt);
	}
	else if (WXK_F2 == code) {
		OnListMenuRename(evt);
	}
	else if (WXK_RETURN == code) {
		OnListMenuOpen(evt);
	}
	else if (WXK_LEFT == code && event.GetModifiers() == wxMOD_ALT) {
		
		// ALT+left == go to parent dir
		wxFileName curDir;
		curDir.AssignDir(Directory->GetValue());

		// root directories don't have parents
		if (curDir.IsOk() && curDir.GetDirCount() > 0) {
			curDir.RemoveLastDir();
			RefreshDir(curDir);
		}
	}
	else {
		event.Skip();
	}
}

void mvceditor::ModalExplorerPanelClass::ShowDir(const wxFileName& currentDir, const std::vector<wxFileName>& files, const std::vector<wxFileName>& dirs,
												 int totalFiles, int totalSubDirs) {
	CurrentListDir = currentDir;
	Directory->SetValue(currentDir.GetPath());

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
	std::vector<mvceditor::ProjectClass>::const_iterator p;
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
	wxString label;
	if (files.size() == (size_t)totalFiles) {
		label = wxString::Format(wxT("%ld Files, %ld Directories"), files.size(), dirs.size());
	}
	else {
		label = wxString::Format(wxT("%ld Files, %ld Directories (%ld not shown)"), files.size(), dirs.size(), totalFiles - files.size());
	}
	ListLabel->SetLabel(label);

	// set the second item to be selected (first other than parent)
	// except if the dir is empty
	if (List->GetItemCount() >= 2) {
		List->SetItemState(1, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
	}
	else if (List->GetItemCount() >= 1) {
		List->SetItemState(0, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
	}
	List->SetFocus();

	if (Watcher) {
		delete Watcher;
	}
	Watcher = new wxFileSystemWatcher();
	Watcher->SetOwner(this);
	Watcher->Add(currentDir, wxFSW_EVENT_CREATE | wxFSW_EVENT_DELETE | wxFSW_EVENT_RENAME | wxFSW_EVENT_WARNING | wxFSW_EVENT_ERROR);
}

bool mvceditor::ModalExplorerPanelClass::OpenIfListFile(const wxString& text) {

	
	// ideally we dont need to query the file system, but cant seem to get the 
	// item image to tell if selected item is a dir or not
	wxString fullPath = CurrentListDir.GetPathWithSep() + text;
	if (wxFileName::FileExists(fullPath)) {
		mvceditor::OpenFileCommandEventClass evt(fullPath);
		Feature.App.EventSink.Publish(evt);
		return true;
	}
	return false;
}

int mvceditor::ModalExplorerPanelClass::ListImageId(const wxFileName& fileName) {
	wxString fullPath = fileName.GetFullPath();
	if (Feature.App.Globals.HasAPhpExtension(fullPath)) {
		return LIST_FILE_PHP;
	}
	if (Feature.App.Globals.HasASqlExtension(fullPath)) {
		return LIST_FILE_SQL;
	} 
	if (Feature.App.Globals.HasACssExtension(fullPath)) {
		return LIST_FILE_CSS;
	}
	if (Feature.App.Globals.HasAMiscExtension(fullPath)) {
		return LIST_FILE_TEXT;
	}
	return LIST_FILE_OTHER;
}

void mvceditor::ModalExplorerPanelClass::OnListMenuOpen(wxCommandEvent& event) {
	long index = -1;
	index = List->GetNextItem(index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (index != -1) {
		wxString name = List->GetItemText(index);

		// ideally we dont need to query the file system, but cant seem to get the 
		// item image to tell if selected item is a dir or not
		wxString fullPath = CurrentListDir.GetPathWithSep() + name;
		if (wxFileName::FileExists(fullPath)) {
			mvceditor::OpenFileCommandEventClass evt(fullPath);
			Feature.App.EventSink.Publish(evt);
		}
		else if (wxFileName::DirExists(fullPath)) {
			wxFileName nextDir;
			nextDir.AssignDir(CurrentListDir.GetPath());
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

void mvceditor::ModalExplorerPanelClass::OnListMenuRename(wxCommandEvent& event) {
	long index = -1;
	index = List->GetNextItem(index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

	// dont allow the parent dir to be renamed
	if (index > 0) {
		List->EditLabel(index);
	}
}

void mvceditor::ModalExplorerPanelClass::OnListMenuDelete(wxCommandEvent& event) {
	long index = -1;
	index = List->GetNextItem(index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	std::vector<wxFileName> dirs;
	std::vector<wxFileName> files;
	while (index != wxNOT_FOUND) {
		
		// dont allow the parent dir to be deleted
		if (index > 0) {
			wxString name = List->GetItemText(index);
			wxString fullPath = CurrentListDir.GetPathWithSep() + name;
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
		mvceditor::ExplorerModifyActionClass* action = new mvceditor::ExplorerModifyActionClass(RunningThreads, ID_EXPLORER_MODIFY);
		action->SetFilesToRemove(dirs, files);
		RunningThreads.Queue(action);
	}
}

void mvceditor::ModalExplorerPanelClass::OnListMenuCreateNew(wxCommandEvent& event) {
	wxString ext;
	std::vector<wxString> extensions;
	wxString dialogTitle;
	if (event.GetId() == ID_EXPLORER_LIST_CREATE_PHP) {
		extensions = Feature.App.Globals.GetPhpFileExtensions();
		if (extensions.empty()) {
			ext = wxT("*.php");
		}
		else {
			ext = extensions[0];
		}
		dialogTitle =_("Create a new PHP file");
	}
	else if (event.GetId() == ID_EXPLORER_LIST_CREATE_SQL) {
		extensions = Feature.App.Globals.GetSqlFileExtensions();
		if (extensions.empty()) {
			ext = wxT("*.sql");
		}
		else {
			ext = extensions[0];
		}
		dialogTitle =_("Create a new SQL file");
	}
	else if (event.GetId() == ID_EXPLORER_LIST_CREATE_CSS) {
		extensions = Feature.App.Globals.GetCssFileExtensions();
		if (extensions.empty()) {
			ext = wxT("*.css");
		}
		else {
			ext = extensions[0];
		}
		dialogTitle =_("Create a new CSS file");
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

	wxString newName = ::wxGetTextFromUser(_("Please enter a file name"), dialogTitle, ext);
	wxString forbidden = wxFileName::GetForbiddenChars();
	if (newName.find_first_of(forbidden, 0) != std::string::npos) {
		wxMessageBox(_("Please enter valid a file name"));
		return;
	}
	if (newName.IsEmpty()) {
		return;
	}
	wxFileName newFileName(CurrentListDir.GetPath(), newName);
	if (newFileName.FileExists()) {
		wxMessageBox(_("File name already exists. Please enter another name."));
		return;
	}
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
		mvceditor::OpenFileCommandEventClass evt(newFileName.GetFullPath());
		Feature.App.EventSink.Publish(evt);
	}
	else {
		wxMessageBox(_("Could not create file: ") + newName);
	}
}

void mvceditor::ModalExplorerPanelClass::OnListMenuCreateDirectory(wxCommandEvent& event) {
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
	newDir.AssignDir(CurrentListDir.GetPath());
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

void mvceditor::ModalExplorerPanelClass::OnListMenuShell(wxCommandEvent& event) {
	wxString cmd = Feature.ShellExecutable.GetFullPath();
	wxPlatformInfo info;
	if (info.GetOperatingSystemId() & wxOS_WINDOWS_NT) {
		cmd += wxT(" /k cd \"") + CurrentListDir.GetPath() + wxT("\"");
	}
	wxExecute(cmd, wxEXEC_ASYNC);
}

void mvceditor::ModalExplorerPanelClass::OnListMenuFileManager(wxCommandEvent& event) {
	wxString cmd = Feature.FileManagerExecutable.GetFullPath();
	wxPlatformInfo info;
	if (info.GetOperatingSystemId() & wxOS_WINDOWS_NT) {
		cmd += wxT(" ") + CurrentListDir.GetPath();
	}
	wxExecute(cmd, wxEXEC_ASYNC);	
}

void mvceditor::ModalExplorerPanelClass::OnListEndLabelEdit(wxListEvent& event) {
	
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
		
		wxFileName sourceFile(CurrentListDir.GetPath(), name);
		wxFileName destFile(CurrentListDir.GetPath(), newName);

		mvceditor::ExplorerModifyActionClass* action = new mvceditor::ExplorerModifyActionClass(RunningThreads, ID_EXPLORER_MODIFY);
		action->SetFileToRename(sourceFile, newName);
		RunningThreads.Queue(action);		
	}
	else {
		event.Veto();
	}
}

void mvceditor::ModalExplorerPanelClass::OnExplorerModifyComplete(mvceditor::ExplorerModifyEventClass &event) {
	if (CurrentListDir.GetPath() != event.GetParentDir().GetPath()) {

		// user is looking at another dir. no need to update the list items
		return;
	}

	wxWindowUpdateLocker updateLocker(this);
	if (mvceditor::ExplorerModifyActionClass::DELETE_FILES_DIRS == event.Action) {
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
	else if (mvceditor::ExplorerModifyActionClass::RENAME_FILE == event.Action) {
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
	}
}

void mvceditor::ModalExplorerPanelClass::OnFilterButtonLeftDown(wxMouseEvent& event) {
	wxPoint point = event.GetPosition();
	if (FilterButton->HitTest(point) == wxHT_WINDOW_INSIDE) {
		wxString allExtensions = Feature.App.Globals.PhpFileExtensionsString + wxT(";") +
			Feature.App.Globals.CssFileExtensionsString + wxT(";") +
			Feature.App.Globals.SqlFileExtensionsString  + wxT(";") + 
			Feature.App.Globals.MiscFileExtensionsString;

		wxString allFiles = wxT("*");
		wxString phpExtensions = Feature.App.Globals.PhpFileExtensionsString;
		wxString cssExtensions = Feature.App.Globals.CssFileExtensionsString;
		wxString sqlExtensions = Feature.App.Globals.SqlFileExtensionsString;
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

		FilterButton->PopupMenu(&menu);
	}

	// according to docs, always allow default processing of mouse down events to take place
	//
	// The handler of this event should normally call event.Skip() to allow the default processing to take 
	// place as otherwise the window under mouse wouldn't get the focus.
	event.Skip();
}

std::vector<wxString> mvceditor::ModalExplorerPanelClass::FilterFileExtensions() {
	std::vector<wxString> extensions;
	if (ID_FILTER_ALL_SOURCE == FilterChoice) {
		extensions = Feature.App.Globals.GetAllSourceFileExtensions();
	}
	else if (ID_FILTER_PHP == FilterChoice) {
		extensions = Feature.App.Globals.GetPhpFileExtensions();
	}
	else if (ID_FILTER_CSS == FilterChoice) {
		extensions = Feature.App.Globals.GetCssFileExtensions();
	}
	else if (ID_FILTER_SQL == FilterChoice) {
		extensions = Feature.App.Globals.GetSqlFileExtensions();
	}

	// no extension == ID_FILTER_ALL
	return extensions;
}

void mvceditor::ModalExplorerPanelClass::OnFilterMenuCheck(wxCommandEvent& event) {
	FilterChoice = event.GetId();
	wxFileName dir;
	dir.AssignDir(Directory->GetValue());
	RefreshDir(dir);
}

void mvceditor::ModalExplorerPanelClass::OnSourceActivated(wxListEvent& event) {
	long selection = event.GetIndex();
	std::vector<wxFileName> sourceDirs = Feature.App.Globals.AllEnabledSourceDirectories();
	if (selection >= 0 && selection < (long)sourceDirs.size()) {
		RefreshDir(sourceDirs[selection]);
	}
}

void mvceditor::ModalExplorerPanelClass::FillSourcesList(const std::vector<wxFileName> &sourceDirs) {
	SourcesList->DeleteAllItems();
	std::vector<wxFileName>::const_iterator dir;
	for (dir = sourceDirs.begin(); dir != sourceDirs.end(); ++dir) {
		
		// list ctrl is tricky, for columns we must insertItem() then setItem() for the next columns
		int newRowNumber = SourcesList->GetItemCount();
		wxString newName = dir->GetDirs().Last();
		wxListItem column1;
		column1.SetColumn(0);
		column1.SetId(newRowNumber);
		column1.SetImage(LIST_FOLDER);
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

void mvceditor::ModalExplorerPanelClass::FocusOnSourcesList() {
	if (SourcesList->GetItemCount()) {
		SourcesList->SetItemState(0, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
	}
	SourcesList->SetFocus();
}

void mvceditor::ModalExplorerPanelClass::OnFsWatcher(wxFileSystemWatcherEvent& event) {
	wxFileName modFile = event.GetNewPath();
	if (modFile.GetPathWithSep() != CurrentListDir.GetPathWithSep()) {

		// event from directory we are not showing
		return;
	}
	if (event.GetChangeType() == wxFSW_EVENT_WARNING && event.GetWarningType() == wxFSW_WARNING_OVERFLOW) {
		
		// restart the watch
		delete Watcher;
		Watcher = new wxFileSystemWatcher();
		Watcher->SetOwner(this);
		Watcher->Add(CurrentListDir, wxFSW_EVENT_CREATE | wxFSW_EVENT_DELETE | wxFSW_EVENT_RENAME | wxFSW_EVENT_WARNING | wxFSW_EVENT_ERROR);
	}
	else if (event.GetChangeType() == wxFSW_EVENT_ERROR) {
		
		// restart the watch
		delete Watcher;
		Watcher = new wxFileSystemWatcher();
		Watcher->SetOwner(this);
		Watcher->Add(CurrentListDir, wxFSW_EVENT_CREATE | wxFSW_EVENT_DELETE | wxFSW_EVENT_RENAME | wxFSW_EVENT_WARNING | wxFSW_EVENT_ERROR);
	}

	// naive implementation for now, just refresh the entire dir
	// this is because we have labels to update, and the 
	// items must be kept sorted (first dirs, then files)
	// each sorted, AND taking the filters into account
	else if (event.GetChangeType() == wxFSW_EVENT_CREATE
		|| event.GetChangeType() == wxFSW_EVENT_DELETE
		|| event.GetChangeType() == wxFSW_EVENT_RENAME) {
		RefreshDir(CurrentListDir);
	}
}

mvceditor::ExplorerEventClass::ExplorerEventClass(int eventId, const wxFileName& dir, const std::vector<wxFileName>& files, 
												  const std::vector<wxFileName>& subDirs, const wxString& error, int totalFiles,
												  int totalSubDirs)
: wxEvent(eventId, mvceditor::EVENT_EXPLORER)
, Dir()
, Files()
, SubDirs()
, Error() 
, TotalFiles(totalFiles) 
, TotalSubDirs(totalSubDirs) {
	
	// clone filenames they contain wxStrings; no thread-safe
	Dir = mvceditor::FileNameCopy(dir);
	Files = mvceditor::DeepCopyFileNames(files);
	SubDirs = mvceditor::DeepCopyFileNames(subDirs);
	Error = Error.c_str();
}

wxEvent* mvceditor::ExplorerEventClass::Clone() const {
	mvceditor::ExplorerEventClass* event = new mvceditor::ExplorerEventClass(GetId(), Dir, Files, SubDirs, Error, TotalFiles, TotalSubDirs);
	return event;
}

mvceditor::ExplorerFileSystemActionClass::ExplorerFileSystemActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
: ActionClass(runningThreads, eventId) 
, Dir()
, Extensions()
, DoHidden(false) {

}

wxString mvceditor::ExplorerFileSystemActionClass::GetLabel() const {
	return wxT("Explorer");
}

void mvceditor::ExplorerFileSystemActionClass::Directory(const wxFileName& dir, const std::vector<wxString>& extensions, bool doHidden) {
	Dir = mvceditor::FileNameCopy(dir);
	Extensions.clear(); 
	mvceditor::DeepCopy(Extensions, extensions);
	DoHidden = doHidden;
}

void mvceditor::ExplorerFileSystemActionClass::BackgroundWork() {
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
		mvceditor::ExplorerEventClass evt(wxID_ANY, Dir, files, subDirs, error, totalFiles, totalSubDirs);
		PostEvent(evt);
	}
}

bool mvceditor::ExplorerFileSystemActionClass::MatchesWildcards(const wxString& fileName) {
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

mvceditor::ExplorerModifyActionClass::ExplorerModifyActionClass(mvceditor::RunningThreadsClass& runningThreads,
																int eventId)
: ActionClass(runningThreads, eventId) 
, Action(NONE)
, Dirs()
, Files()
, OldFile()
, NewName() {
}

void mvceditor::ExplorerModifyActionClass::SetFilesToRemove(const std::vector<wxFileName>& dirs, const std::vector<wxFileName>& files) {
	Action = DELETE_FILES_DIRS;

	// make sure to clone
	Dirs = mvceditor::DeepCopyFileNames(dirs);
	Files = mvceditor::DeepCopyFileNames(files);
}

void mvceditor::ExplorerModifyActionClass::SetFileToRename(const wxFileName& file, const wxString& newName) {
	Action = RENAME_FILE;

	// make sure to clone
	OldFile.Assign(file.GetFullPath());
	NewName = newName.c_str();
}

void mvceditor::ExplorerModifyActionClass::BackgroundWork() {
	wxFileName parentDir;
	wxString name;
	bool totalSuccess = true;
	std::vector<wxFileName> dirsDeleted;
	std::vector<wxFileName> dirsNotDeleted;
	std::vector<wxFileName> filesDeleted;
	std::vector<wxFileName> filesNotDeleted;
	if (mvceditor::ExplorerModifyActionClass::DELETE_FILES_DIRS == Action) {
		std::vector<wxFileName>::iterator d;
		for (d = Dirs.begin(); d != Dirs.end(); ++d) {
			bool success = mvceditor::RecursiveRmDir(d->GetPath());
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
		mvceditor::ExplorerModifyEventClass modEvent(GetEventId(), 
			dirsDeleted, filesDeleted, dirsNotDeleted, filesNotDeleted, totalSuccess);
		PostEvent(modEvent);
	}
	else if (mvceditor::ExplorerModifyActionClass::RENAME_FILE == Action) {
		wxFileName destFile(OldFile.GetPath(), NewName);
		bool success = wxRenameFile(OldFile.GetFullPath(), destFile.GetFullPath(), false);

		mvceditor::ExplorerModifyEventClass modEvent(GetEventId(), 
			OldFile, NewName, success);
		PostEvent(modEvent);
	}
}
wxString mvceditor::ExplorerModifyActionClass::GetLabel() const {
	return wxT("File System Modification");
}

mvceditor::ExplorerModifyEventClass::ExplorerModifyEventClass(int eventId, const wxFileName &oldFile, 
															  const wxString &newName, bool success) 
: wxEvent(eventId, mvceditor::EVENT_EXPLORER_MODIFY)
, OldFile(oldFile.GetFullPath())
, NewName(newName.c_str())
, DirsDeleted()
, FilesDeleted()
, DirsNotDeleted()
, FilesNotDeleted()
, Action(mvceditor::ExplorerModifyActionClass::RENAME_FILE)
, Success(success)
{

}

mvceditor::ExplorerModifyEventClass::ExplorerModifyEventClass(int eventId, 
															  const std::vector<wxFileName>& dirsDeleted, const std::vector<wxFileName>& filesDeleted,
															  const std::vector<wxFileName>& dirsNotDeleted, const std::vector<wxFileName>& filesNotDeleted,
															  bool success)
: wxEvent(eventId, mvceditor::EVENT_EXPLORER_MODIFY)
, OldFile()
, NewName()
, DirsDeleted()
, FilesDeleted()
, DirsNotDeleted()
, FilesNotDeleted()
, Action(mvceditor::ExplorerModifyActionClass::DELETE_FILES_DIRS)
, Success(success)
{
	DirsDeleted = mvceditor::DeepCopyFileNames(dirsDeleted);
	DirsNotDeleted = mvceditor::DeepCopyFileNames(dirsNotDeleted);
	FilesDeleted = mvceditor::DeepCopyFileNames(filesDeleted);
	FilesNotDeleted = mvceditor::DeepCopyFileNames(filesNotDeleted);
}

wxFileName mvceditor::ExplorerModifyEventClass::GetParentDir() const {
	wxFileName parentDir;
	if (Action == mvceditor::ExplorerModifyActionClass::RENAME_FILE) {
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

wxEvent* mvceditor::ExplorerModifyEventClass::Clone() const {
	if (Action == mvceditor::ExplorerModifyActionClass::RENAME_FILE) {
		return new mvceditor::ExplorerModifyEventClass(
			GetId(), OldFile, NewName, Success
		);
	}
	return new mvceditor::ExplorerModifyEventClass(
		GetId(), DirsDeleted, FilesDeleted, DirsNotDeleted, FilesNotDeleted, Success
	);
}

mvceditor::ExplorerOptionsPanelClass::ExplorerOptionsPanelClass(wxWindow* parent, int id, mvceditor::ExplorerFeatureClass& feature)
	: ExplorerOptionsGeneratedPanelClass(parent, id)
	, Feature(feature) {
	mvceditor::FilePickerValidatorClass fileManagerValidator(&feature.FileManagerExecutable);
	FileManager->SetValidator(fileManagerValidator);
	mvceditor::FilePickerValidatorClass shellValidator(&feature.ShellExecutable);
	Shell->SetValidator(shellValidator);	

	TransferDataToWindow();
}

const wxEventType mvceditor::EVENT_EXPLORER = wxNewEventType();
const wxEventType mvceditor::EVENT_EXPLORER_MODIFY = wxNewEventType();

BEGIN_EVENT_TABLE(mvceditor::ExplorerFeatureClass, mvceditor::FeatureClass)
	EVT_MENU(mvceditor::MENU_EXPLORER + 1, mvceditor::ExplorerFeatureClass::OnProjectExploreOpenFile)
	EVT_MENU(mvceditor::MENU_EXPLORER + 2, mvceditor::ExplorerFeatureClass::OnProjectExplore)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(mvceditor::MENU_EXPLORER + 2, mvceditor::ExplorerFeatureClass::OnExplorerToolDropDown)
	EVT_MENU_RANGE(mvceditor::MENU_EXPLORER + 3, mvceditor::MENU_EXPLORER + 50, mvceditor::ExplorerFeatureClass::OnExplorerProjectMenu)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_PREFERENCES_SAVED, mvceditor::ExplorerFeatureClass::OnAppPreferencesSaved)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_PREFERENCES_EXTERNALLY_UPDATED, mvceditor::ExplorerFeatureClass::OnAppPreferencesSaved)
	
	EVT_EXPLORER_COMPLETE(ID_EXPLORER_LIST_ACTION, mvceditor::ExplorerFeatureClass::OnExplorerListComplete)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::ModalExplorerPanelClass, ModalExplorerGeneratedPanelClass)
	EVT_MENU(ID_EXPLORER_LIST_OPEN, mvceditor::ModalExplorerPanelClass::OnListMenuOpen)
	EVT_MENU(ID_EXPLORER_LIST_RENAME, mvceditor::ModalExplorerPanelClass::OnListMenuRename)
	EVT_MENU(ID_EXPLORER_LIST_DELETE, mvceditor::ModalExplorerPanelClass::OnListMenuDelete)
	EVT_MENU(ID_EXPLORER_LIST_CREATE_PHP, mvceditor::ModalExplorerPanelClass::OnListMenuCreateNew)
	EVT_MENU(ID_EXPLORER_LIST_CREATE_SQL, mvceditor::ModalExplorerPanelClass::OnListMenuCreateNew)
	EVT_MENU(ID_EXPLORER_LIST_CREATE_CSS, mvceditor::ModalExplorerPanelClass::OnListMenuCreateNew)
	EVT_MENU(ID_EXPLORER_LIST_CREATE_TEXT, mvceditor::ModalExplorerPanelClass::OnListMenuCreateNew)
	EVT_MENU(ID_EXPLORER_LIST_CREATE_DIRECTORY, mvceditor::ModalExplorerPanelClass::OnListMenuCreateDirectory)
	EVT_MENU(ID_EXPLORER_LIST_SHELL, mvceditor::ModalExplorerPanelClass::OnListMenuShell)
	EVT_MENU(ID_EXPLORER_LIST_FILE_MANAGER, mvceditor::ModalExplorerPanelClass::OnListMenuFileManager)
	EVT_EXPLORER_MODIFY_COMPLETE(ID_EXPLORER_MODIFY, mvceditor::ModalExplorerPanelClass::OnExplorerModifyComplete)

	EVT_MENU(ID_FILTER_ALL, mvceditor::ModalExplorerPanelClass::OnFilterMenuCheck)
	EVT_MENU(ID_FILTER_ALL_SOURCE, mvceditor::ModalExplorerPanelClass::OnFilterMenuCheck)
	EVT_MENU(ID_FILTER_PHP, mvceditor::ModalExplorerPanelClass::OnFilterMenuCheck)
	EVT_MENU(ID_FILTER_CSS, mvceditor::ModalExplorerPanelClass::OnFilterMenuCheck)
	EVT_MENU(ID_FILTER_SQL, mvceditor::ModalExplorerPanelClass::OnFilterMenuCheck)
	EVT_FSWATCHER(wxID_ANY, mvceditor::ModalExplorerPanelClass::OnFsWatcher)
END_EVENT_TABLE()
