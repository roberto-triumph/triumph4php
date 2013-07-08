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
#include <MvcEditor.h>
#include <algorithm>

static int ID_EXPLORER_PANEL = wxNewId();
static int ID_EXPLORER_LIST_ACTION = wxNewId();
static int ID_EXPLORER_REPORT_ACTION = wxNewId();
static int ID_EXPLORER_LIST_OPEN = wxNewId();
static int ID_EXPLORER_LIST_RENAME = wxNewId();
static int ID_EXPLORER_LIST_DELETE = wxNewId();
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
}

void mvceditor::ExplorerFeatureClass::AddFileMenuItems(wxMenu* fileMenu) {
	fileMenu->Append(mvceditor::MENU_EXPLORER + 1, _("Explore Open File"), _("Open An explorer window in the currently opened file"), wxITEM_NORMAL);
}


void mvceditor::ExplorerFeatureClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_EXPLORER + 1] = wxT("Project-Explore File");
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
		.PaneBorder(false).Floatable(false)
	);
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
}

void mvceditor::ExplorerFeatureClass::OnProjectExplore(wxCommandEvent& event) {
	SourceDirs = App.Globals.AllEnabledSources();
	if (SourceDirs.size() == 1) {
		
		// dont show menu, just open it now
		wxCommandEvent cmdEvt;
		cmdEvt.SetId(mvceditor::MENU_EXPLORER + 3);
		OnExplorerProjectMenu(cmdEvt);
		return;
	}
}

void mvceditor::ExplorerFeatureClass::OnExplorerToolDropDown(wxAuiToolBarEvent& event) {
	if (!event.IsDropDownClicked()) {
		event.Skip();
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

void mvceditor::ExplorerFeatureClass::OnExplorerReportComplete(mvceditor::ExplorerEventClass& event) {
	mvceditor::ModalExplorerPanelClass* panel = NULL;
	wxWindow* window = FindToolsWindow(ID_EXPLORER_PANEL);
	if (window) {
		panel = (mvceditor::ModalExplorerPanelClass*)window;
		panel->ShowReport(event.Dir, event.Files, event.SubDirs, event.TotalFiles, event.TotalSubDirs);
	}
}

mvceditor::ModalExplorerPanelClass::ModalExplorerPanelClass(wxWindow* parent, int id, mvceditor::ExplorerFeatureClass& feature)
: ModalExplorerGeneratedPanel(parent, id) 
, CurrentListDir()
, CurrentReportDir() 
, ListImageList(NULL)
, ReportImageList(NULL)
, Feature(feature) 
, RunningThreads()
, FilterChoice(ID_FILTER_ALL) {
	ListImageList = new wxImageList(16, 16);
	ListImageList->Add(mvceditor::IconImageAsset(wxT("folder-horizontal")));
	ListImageList->Add(mvceditor::IconImageAsset(wxT("arrow-up")));
	ListImageList->Add(mvceditor::IconImageAsset(wxT("document-php")));
	ListImageList->Add(mvceditor::IconImageAsset(wxT("document-sql")));
	ListImageList->Add(mvceditor::IconImageAsset(wxT("document-css")));
	ListImageList->Add(mvceditor::IconImageAsset(wxT("document-text")));
	ListImageList->Add(mvceditor::IconImageAsset(wxT("document-blank")));

	ReportImageList = new wxImageList(16, 16);
	ReportImageList->Add(mvceditor::IconImageAsset(wxT("document-php")));
	ReportImageList->Add(mvceditor::IconImageAsset(wxT("folder-horizontal")));
	ReportImageList->Add(mvceditor::IconImageAsset(wxT("document-sql")));
	ReportImageList->Add(mvceditor::IconImageAsset(wxT("document-css")));
	ReportImageList->Add(mvceditor::IconImageAsset(wxT("document-text")));
	ReportImageList->Add(mvceditor::IconImageAsset(wxT("document-blank")));


	List->AssignImageList(ListImageList, wxIMAGE_LIST_SMALL);

	Report->AssignImageList(ReportImageList, wxIMAGE_LIST_SMALL);
	Report->InsertColumn(0, _("Name"), wxLIST_FORMAT_LEFT, 300);

	FilterButton->SetBitmapLabel(mvceditor::IconImageAsset(wxT("filter")));
	ParentButton->SetBitmapLabel(mvceditor::IconImageAsset(wxT("arrow-up")));

	RunningThreads.SetMaxThreads(1);
	RunningThreads.AddEventHandler(this);
	RunningThreads.AddEventHandler(&Feature);
	
}

mvceditor::ModalExplorerPanelClass::~ModalExplorerPanelClass() {
	RunningThreads.RemoveEventHandler(this);
	RunningThreads.RemoveEventHandler(&Feature);
}

void mvceditor::ModalExplorerPanelClass::RefreshDir(const wxFileName& dir) {
	ListLabel->SetLabel(wxT(""));
	ReportLabel->SetLabel(wxT(""));
	mvceditor::ExplorerFileSystemActionClass* action = new mvceditor::ExplorerFileSystemActionClass(RunningThreads, ID_EXPLORER_LIST_ACTION);

	// when user choose ALL show hidden files too
	action->Directory(dir, FilterFileExtensions(), ID_FILTER_ALL == FilterChoice);
	RunningThreads.Queue(action);
}

void mvceditor::ModalExplorerPanelClass::OnDirectoryEnter(wxCommandEvent& event) {
	RefreshDir(Directory->GetValue());
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
		nextDir.AssignDir(CurrentListDir.GetPathWithSep() + text);
	}
	if (!OpenIfListFile(text)) {
		RefreshDir(nextDir);
	}	
}

void mvceditor::ModalExplorerPanelClass::OnListItemSelected(wxListEvent& event) {
	wxString text = event.GetText();
	if (text == wxT("..")) {
		return;
	}
	wxFileName nextDir;
	nextDir.AssignDir(CurrentListDir.GetPathWithSep() + text);
	if (wxFileName::DirExists(CurrentListDir.GetPathWithSep() + text)) {
		mvceditor::ExplorerFileSystemActionClass* action = new mvceditor::ExplorerFileSystemActionClass(RunningThreads, ID_EXPLORER_REPORT_ACTION);

		// when user choose ALL show hidden files too
		action->Directory(nextDir, FilterFileExtensions(), ID_FILTER_ALL == FilterChoice);
		RunningThreads.Queue(action);
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
		}

		this->PopupMenu(&menu, wxDefaultPosition);
	}
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
	else {
		event.Skip();
	}
}

void mvceditor::ModalExplorerPanelClass::ShowDir(const wxFileName& currentDir, const std::vector<wxFileName>& files, const std::vector<wxFileName>& dirs,
												 int totalFiles, int totalSubDirs) {
	Report->DeleteAllItems();
	CurrentReportDir.Clear();
	CurrentListDir = currentDir;

	Directory->SetValue(currentDir.GetPath());

	List->DeleteAllItems();
	int newRowNumber = 0;

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
	newRowNumber = Report->GetItemCount();
	if (newRowNumber > 1) {
		Report->SetItemState(1, wxLIST_STATE_SELECTED, wxLIST_MASK_STATE | wxLIST_MASK_TEXT);
	}
	else if (newRowNumber > 0) {
		Report->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_MASK_STATE | wxLIST_MASK_TEXT);
	}
	wxString label;
	if (files.size() == (size_t)totalFiles) {
		label = wxString::Format(wxT("%d Files, %d Directories"), files.size(), dirs.size());
	}
	else {
		label = wxString::Format(wxT("%d Files, %d Directories (%d not shown)"), files.size(), dirs.size(), totalFiles - files.size());
	}
	ListLabel->SetLabel(label);
	ReportLabel->SetLabel(wxT(""));
}

void mvceditor::ModalExplorerPanelClass::ShowReport(const wxFileName& dir, const std::vector<wxFileName>& files, const std::vector<wxFileName>& dirs,
													int totalFiles, int totalSubDirs) {
	Report->DeleteAllItems();
	CurrentReportDir = dir;
	ListDirectories(dirs);
	ListFiles(files);

	wxString label;
	if (files.size() == (size_t)totalFiles) {
		label = wxString::Format(wxT("%d Files, %d Directories"), files.size(), dirs.size());
	}
	else {
		label = wxString::Format(wxT("%d Files, %d Directories (%d not shown)"), files.size(), dirs.size(), totalFiles - files.size());
	}
	ReportLabel->SetLabel(label);
}

void mvceditor::ModalExplorerPanelClass::ListFiles(const std::vector<wxFileName>& files) {
	long newRowNumber = Report->GetItemCount();
	std::vector<wxFileName>::const_iterator file;
	for (file = files.begin(); file != files.end(); ++file) {
		
		// list ctrl is tricky, for columns we must insertItem() then setItem() for the next columns
		wxListItem column1;
		column1.SetColumn(0);
		column1.SetId(newRowNumber);
		column1.SetImage(ReportImageId(*file));
		column1.SetMask(wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT);
		column1.SetText(file->GetFullName());
		Report->InsertItem(column1);

		newRowNumber++;
	}
	newRowNumber = Report->GetItemCount();
	if (newRowNumber > 0) {
		Report->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_MASK_STATE | wxLIST_MASK_TEXT);
	}
}

void mvceditor::ModalExplorerPanelClass::ListDirectories(const std::vector<wxFileName>& dirs) {
	long newRowNumber = Report->GetItemCount();
	std::vector<wxFileName>::const_iterator dir;
	for (dir = dirs.begin(); dir != dirs.end(); ++dir) {
		
		// list ctrl is tricky, for columns we must insertItem() then setItem() for the next columns
		wxListItem column1;
		column1.SetColumn(0);
		column1.SetId(newRowNumber);
		column1.SetImage(REPORT_DIR);
		column1.SetMask(wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT);
		column1.SetText(dir->GetDirs().Last());
		Report->InsertItem(column1);

		newRowNumber++;
	}
	newRowNumber = Report->GetItemCount();
	if (newRowNumber > 0) {
		Report->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_MASK_STATE | wxLIST_MASK_TEXT);
	}
}

bool mvceditor::ModalExplorerPanelClass::OpenIfListFile(const wxString& text) {

	
	// ideally we dont need to query the file system, but cant seem to get the 
	// item image to tell if selected item is a dir or not
	wxString fullPath = CurrentListDir.GetPathWithSep() + text;
	if (wxFileName::FileExists(fullPath)) {
		wxCommandEvent evt(mvceditor::EVENT_CMD_FILE_OPEN);
		evt.SetString(fullPath);
		Feature.App.EventSink.Publish(evt);
		return true;
	}
	return false;
}

bool mvceditor::ModalExplorerPanelClass::OpenIfReportFile(const wxString& text) {
	
	// ideally we dont need to query the file system, but cant seem to get the 
	// item image to tell if selected item is a dir or not
	wxString fullPath = CurrentReportDir.GetPathWithSep() + text;
	if (wxFileName::FileExists(fullPath)) {
		wxCommandEvent evt(mvceditor::EVENT_CMD_FILE_OPEN);
		evt.SetString(fullPath);
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

int mvceditor::ModalExplorerPanelClass::ReportImageId(const wxFileName& fileName) {
	wxString fullPath = fileName.GetFullPath();
	if (Feature.App.Globals.HasAPhpExtension(fullPath)) {
		return REPORT_FILE_PHP;
	}
	if (Feature.App.Globals.HasASqlExtension(fullPath)) {
		return REPORT_FILE_SQL;
	} 
	if (Feature.App.Globals.HasACssExtension(fullPath)) {
		return REPORT_FILE_CSS;
	}
	if (Feature.App.Globals.HasAMiscExtension(fullPath)) {
		return REPORT_FILE_TEXT;
	}
	return REPORT_FILE_OTHER;
}

void mvceditor::ModalExplorerPanelClass::OnReportItemActivated(wxListEvent& event) {
	wxString text = event.GetText();
	if (text.empty()) {
		return;
	}
	if (!OpenIfReportFile(text)) {
		wxFileName nextDir;
		nextDir.AssignDir(CurrentReportDir.GetPathWithSep() + text);
		RefreshDir(nextDir);
	}
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
			wxCommandEvent evt(mvceditor::EVENT_CMD_FILE_OPEN);
			evt.SetString(fullPath);
			Feature.App.EventSink.Publish(evt);
		}
		else if (wxFileName::DirExists(fullPath)) {
			mvceditor::ExplorerFileSystemActionClass* action = new mvceditor::ExplorerFileSystemActionClass(RunningThreads, ID_EXPLORER_REPORT_ACTION);

			// when user choose ALL show hidden files too
			action->Directory(fullPath, FilterFileExtensions(), ID_FILTER_ALL == FilterChoice);
			RunningThreads.Queue(action);
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

	// dont allow the parent dir to be deleted
	if (index > 0) {
		wxString name = List->GetItemText(index);
		wxString fullPath = CurrentListDir.GetPathWithSep() + name;

		// perform the deletion in the background
		mvceditor::ExplorerModifyActionClass* action = new mvceditor::ExplorerModifyActionClass(RunningThreads, ID_EXPLORER_MODIFY);
		if (wxFileName::FileExists(fullPath)) {
			action->SetFileToRemove(wxFileName(fullPath));
		}
		else if (wxFileName::DirExists(fullPath)) {

			// need to recursively delete a directory
			wxFileName dirToDelete;
			dirToDelete.AssignDir(fullPath);
			action->SetDirToRemove(dirToDelete);
		}
		RunningThreads.Queue(action);
	}
}

void mvceditor::ModalExplorerPanelClass::OnListEndLabelEdit(wxListEvent& event) {
	
	// ideally we dont need to query the file system, but cant seem to get the 
	// item image to tell if selected item is a dir or not
	wxString newName = event.GetLabel();
	long index = event.GetIndex();
	wxString name = List->GetItemText(index);

	// dont attempt rename if they are new and old name are the same
	if (newName != name && !newName.IsEmpty()) {	
		
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
	if (CurrentListDir.GetPath() != event.ParentDir.GetPath()) {
		wxString a = CurrentListDir.GetPath();
		wxString b = event.ParentDir.GetPath();

		// user is looking at another dir. no need to update the list items
		return;
	}

	// TODO FindItem is case insensitive but the file system may be case sensitive
	long index = List->FindItem(-1, event.Name);
	if (wxNOT_FOUND == index) {
		return;
	}

	if (mvceditor::ExplorerModifyActionClass::DELETE_FILE == event.Action) {
		wxFileName fileName(event.ParentDir.GetPath(), event.Name);
		wxString fullPath = fileName.GetFullPath();
		if (event.Success) {
				List->DeleteItem(index);
			}
			else {
				wxMessageBox(_("File could not be deleted:\n") + fullPath, _("Delete File"));
			}
	}
	else if (mvceditor::ExplorerModifyActionClass::DELETE_DIRECTORY == event.Action) {
		wxFileName fileName;
		fileName.AssignDir(event.ParentDir.GetPath());
		fileName.AppendDir(event.Name);
		wxString fullPath = fileName.GetPath();
		if (event.Success) {
			List->DeleteItem(index);
		}
		else {
			wxMessageBox(_("Directory could not be deleted:\n") + fullPath, _("Delete Directory"));
		}
	}
	else if (mvceditor::ExplorerModifyActionClass::RENAME == event.Action) {
		wxFileName destFile(event.ParentDir.GetPath(), event.NewName);

		if (!event.Success && wxFileName::DirExists(destFile.GetFullPath())) {
			// revert the item back to the original name
			List->SetItemText(index, event.Name);

			wxMessageBox(_("A directory with that name already exists"), _("Rename"));			
		}
		else if (!event.Success) {
			// revert the item back to the original name
			List->SetItemText(index, event.Name);

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
, Dir()
, File()
, NewName() {
}

void mvceditor::ExplorerModifyActionClass::SetDirToRemove(const wxFileName &dir) {
	Action = DELETE_DIRECTORY;

	// make sure to clone
	Dir.AssignDir(dir.GetPath());
}

void mvceditor::ExplorerModifyActionClass::SetFileToRemove(const wxFileName& file) {
	Action = DELETE_FILE;

	// make sure to clone
	File.Assign(file.GetFullPath());
}

void mvceditor::ExplorerModifyActionClass::SetFileToRename(const wxFileName& file, const wxString& newName) {
	Action = RENAME;

	// make sure to clone
	File.Assign(file.GetFullPath());
	NewName = newName.c_str();
}

void mvceditor::ExplorerModifyActionClass::BackgroundWork() {
	wxFileName parentDir;
	wxString name;
	bool success = false;
	if (mvceditor::ExplorerModifyActionClass::DELETE_DIRECTORY == Action) {
		parentDir.AssignDir(Dir.GetPath());
		name = parentDir.GetDirs().Last();
		parentDir.RemoveLastDir();
	}
	else {
		parentDir.Assign(File.GetFullPath());
		name = parentDir.GetFullName();
	}

	// perform the action
	if (mvceditor::ExplorerModifyActionClass::DELETE_DIRECTORY == Action) {
		success = mvceditor::RecursiveRmDir(Dir.GetPath());
	}
	else if (mvceditor::ExplorerModifyActionClass::DELETE_FILE == Action) {
		success = wxRemoveFile(File.GetFullPath());
	}
	else if (mvceditor::ExplorerModifyActionClass::RENAME == Action) {
		wxFileName destFile(parentDir.GetPath(), NewName);
		success = wxRenameFile(File.GetFullPath(), destFile.GetFullPath(), false);
	}
	mvceditor::ExplorerModifyEventClass modEvent(GetEventId(), 
		parentDir, name, NewName, Action, success);
	PostEvent(modEvent);
}
wxString mvceditor::ExplorerModifyActionClass::GetLabel() const {
	return wxT("File System Modification");
}

mvceditor::ExplorerModifyEventClass::ExplorerModifyEventClass(int eventId, const wxFileName &dir, const wxString &name, const wxString &newName, mvceditor::ExplorerModifyActionClass::Actions action, bool success) 
: wxEvent(eventId, mvceditor::EVENT_EXPLORER_MODIFY)
, ParentDir(dir)
, Name(name)
, NewName(newName)
, Action(action)
, Success(success)
{

}

wxEvent* mvceditor::ExplorerModifyEventClass::Clone() const {
	return  new mvceditor::ExplorerModifyEventClass(
		GetId(), ParentDir, Name, NewName, Action, Success
	);
}

const wxEventType mvceditor::EVENT_EXPLORER = wxNewEventType();
const wxEventType mvceditor::EVENT_EXPLORER_MODIFY = wxNewEventType();

BEGIN_EVENT_TABLE(mvceditor::ExplorerFeatureClass, mvceditor::FeatureClass)
	EVT_MENU(mvceditor::MENU_EXPLORER + 1, mvceditor::ExplorerFeatureClass::OnProjectExploreOpenFile)
	EVT_MENU(mvceditor::MENU_EXPLORER + 2, mvceditor::ExplorerFeatureClass::OnProjectExplore)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(mvceditor::MENU_EXPLORER + 2, mvceditor::ExplorerFeatureClass::OnExplorerToolDropDown)
	EVT_MENU_RANGE(mvceditor::MENU_EXPLORER + 3, mvceditor::MENU_EXPLORER + 50, mvceditor::ExplorerFeatureClass::OnExplorerProjectMenu)
	
	EVT_EXPLORER_COMPLETE(ID_EXPLORER_LIST_ACTION, mvceditor::ExplorerFeatureClass::OnExplorerListComplete)
	EVT_EXPLORER_COMPLETE(ID_EXPLORER_REPORT_ACTION, mvceditor::ExplorerFeatureClass::OnExplorerReportComplete)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::ModalExplorerPanelClass, ModalExplorerGeneratedPanel)
	EVT_MENU(ID_EXPLORER_LIST_OPEN, mvceditor::ModalExplorerPanelClass::OnListMenuOpen)
	EVT_MENU(ID_EXPLORER_LIST_RENAME, mvceditor::ModalExplorerPanelClass::OnListMenuRename)
	EVT_MENU(ID_EXPLORER_LIST_DELETE, mvceditor::ModalExplorerPanelClass::OnListMenuDelete)
	EVT_EXPLORER_MODIFY_COMPLETE(ID_EXPLORER_MODIFY, mvceditor::ModalExplorerPanelClass::OnExplorerModifyComplete)

	EVT_MENU(ID_FILTER_ALL, mvceditor::ModalExplorerPanelClass::OnFilterMenuCheck)
	EVT_MENU(ID_FILTER_ALL_SOURCE, mvceditor::ModalExplorerPanelClass::OnFilterMenuCheck)
	EVT_MENU(ID_FILTER_PHP, mvceditor::ModalExplorerPanelClass::OnFilterMenuCheck)
	EVT_MENU(ID_FILTER_CSS, mvceditor::ModalExplorerPanelClass::OnFilterMenuCheck)
	EVT_MENU(ID_FILTER_SQL, mvceditor::ModalExplorerPanelClass::OnFilterMenuCheck)
END_EVENT_TABLE()