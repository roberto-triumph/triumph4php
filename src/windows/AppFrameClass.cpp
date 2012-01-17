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
#include <windows/AppFrameClass.h>
#include <AppVersion.h>
#include <PluginClass.h>
#include <MvcEditor.h>
#include <windows/PreferencesDialogClass.h>
#include <widgets/StatusBarWithGaugeClass.h>

#include <wx/artprov.h>
#include <wx/choicdlg.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/wfstream.h>
#include <wx/aboutdlg.h>
#include <vector>

int ID_TOOLBAR_SAVE = wxNewId();
int ID_CUT_LINE = wxNewId();
int ID_DUPLICATE_LINE = wxNewId();
int ID_DELETE_LINE = wxNewId();
int ID_TRANSPOSE_LINE = wxNewId();
int ID_COPY_LINE = wxNewId();
int ID_LOWERCASE = wxNewId();
int ID_UPPERCASE = wxNewId();
int ID_MENU_MORE = wxNewId();
int ID_TOOLBAR = wxNewId();
int ID_TOOLS_WINDOW = wxNewId();

mvceditor::AppFrameClass::AppFrameClass(const std::vector<mvceditor::PluginClass*>& plugins,
		wxEvtHandler& appHandler, mvceditor::EnvironmentClass& environment,
		mvceditor::PreferencesClass& preferences, mvceditor::ResourceUpdateThreadClass& resourceUpdates)
	: AppFrameGeneratedClass(NULL)
	, Plugins(plugins)
	, AppHandler(appHandler)
	, Environment(environment)
	, Preferences(preferences)
	, ToolBar(NULL)
	, ToolsNotebook(NULL) {
	StatusBarWithGaugeClass* gauge = new StatusBarWithGaugeClass(this);
	SetStatusBar(gauge);
	
	AuiManager.SetManagedWindow(this);
	ToolBar = new wxAuiToolBar(this, ID_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxAUI_TB_TEXT | wxAUI_TB_DEFAULT_STYLE);
	
	// when the notebook is empty we want to accept dragged files
	Notebook->SetDropTarget(new FileDropTargetClass(Notebook));
	Notebook->CodeControlOptions = &Preferences.CodeControlOptions;
	Notebook->ResourceUpdates = &resourceUpdates;
	
	// ATTN: for some reason must remove and re-insert menu item in order to change the icon
	wxMenuItem* projectOpenMenuItem = MenuBar->FindItem(ID_PROJECT_OPEN);
	projectOpenMenuItem = ProjectMenu->Remove(projectOpenMenuItem);
	projectOpenMenuItem->SetBitmap(wxArtProvider::GetBitmap(wxART_FOLDER_OPEN, wxART_MENU));
	ProjectMenu->Insert(0, projectOpenMenuItem);
	ToolsNotebook = new wxAuiNotebook(this, ID_TOOLS_WINDOW, wxDefaultPosition, wxDefaultSize, 
		wxAUI_NB_TOP | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB | wxAUI_NB_TAB_MOVE);

	CreateToolBarButtons();
	
	// setup the bottom "tools" pane, the main content pane, and the toolbar on top
	AuiManager.AddPane(ToolBar, wxAuiPaneInfo().Top(
		).CaptionVisible(false).CloseButton(false).Gripper(
		false).DockFixed(true).PaneBorder(false).Floatable(false).Row(0).Position(0));		
		
	AuiManager.AddPane(Notebook, wxAuiPaneInfo().CentrePane(
		).PaneBorder(false).Gripper(false).Floatable(false));
	AuiManager.AddPane(ToolsNotebook, wxAuiPaneInfo().Bottom().Caption(
		_("Tools")).Floatable(false).MinSize(-1, 260).Hide());
	AuiManager.Update();

	DefaultKeyboardShortcuts();
}

mvceditor::AppFrameClass::~AppFrameClass() {
	AuiManager.UnInit();
}

mvceditor::StatusBarWithGaugeClass* mvceditor::AppFrameClass::GetStatusBarWithGauge() {
	return (StatusBarWithGaugeClass*)GetStatusBar();
}

void mvceditor::AppFrameClass::OnClose(wxCloseEvent& event) {
	bool destroy = true;
	if (event.CanVeto()) {
		destroy = Notebook->SaveAllModifiedPages();
	}
	if (destroy) {
		
		// need to detach the window from the keyboard BEFORE the window is invalidated
		Preferences.KeyProfiles.GetSelProfile()->DetachAll();	
		Destroy();
	}
}

void mvceditor::AppFrameClass::OnFileSave(wxCommandEvent& event) {
	Notebook->SaveCurrentPage();
	mvceditor::CodeControlClass* codeControl = Notebook->GetCurrentCodeControl();
	mvceditor::FileSavedEventClass pluginEvent(codeControl);

	for (size_t i = 0; i < Plugins.size(); i++) {
		wxPostEvent(Plugins[i], pluginEvent);
	}
}

void mvceditor::AppFrameClass::OnFilePhpNew(wxCommandEvent& event) {
	Notebook->AddMvcEditorPage();
	mvceditor::CodeControlClass* ctrl = Notebook->GetCurrentCodeControl();
	ctrl->SetDocumentMode(mvceditor::CodeControlClass::PHP);
}

void mvceditor::AppFrameClass::OnFileSqlNew(wxCommandEvent& event) {
	Notebook->AddMvcEditorPage();
	mvceditor::CodeControlClass* ctrl = Notebook->GetCurrentCodeControl();
	ctrl->SetDocumentMode(mvceditor::CodeControlClass::SQL);
}

void mvceditor::AppFrameClass::OnFileCssNew(wxCommandEvent& event) {
	Notebook->AddMvcEditorPage();
	mvceditor::CodeControlClass* ctrl = Notebook->GetCurrentCodeControl();
	ctrl->SetDocumentMode(mvceditor::CodeControlClass::CSS);
}

void mvceditor::AppFrameClass::OnFileTextNew(wxCommandEvent& event) {
	Notebook->AddMvcEditorPage();
	mvceditor::CodeControlClass* ctrl = Notebook->GetCurrentCodeControl();
	ctrl->SetDocumentMode(mvceditor::CodeControlClass::TEXT);
}

void mvceditor::AppFrameClass::OnFileOpen(wxCommandEvent& event) {
	Notebook->LoadPage();
}

void mvceditor::AppFrameClass::OnFileSaveAs(wxCommandEvent& event) {
	Notebook->SaveCurrentPageAsNew();
}

void mvceditor::AppFrameClass::FileOpen(const std::vector<wxString>& filenames) {
	Notebook->LoadPages(filenames);
}

void mvceditor::AppFrameClass::OnFileClose(wxCommandEvent& event) {
	Notebook->CloseCurrentPage();
}

void mvceditor::AppFrameClass::OnFileExit(wxCommandEvent& event) {
	Close();
}

void mvceditor::AppFrameClass::OnFileRevert(wxCommandEvent& event) {
	CodeControlClass* code = Notebook->GetCurrentCodeControl();
	if (NULL != code && !code->IsNew()) {
		int res = wxMessageBox(_("Reload file and lose all changes?"), _("MVC Editor"), wxICON_QUESTION | wxYES_NO, this);
		if (wxYES == res) {
			code->Revert();
		}
	}
}

void mvceditor::AppFrameClass::OnEditCut(wxCommandEvent& event) {

	// need to handle cut in all text controls
	wxWindow* obj = wxWindow::FindFocus();
	wxTextCtrl* t = wxDynamicCast(obj, wxTextCtrl);
	wxComboBox* combo = wxDynamicCast(obj, wxComboBox);
	wxStyledTextCtrl* stc = wxDynamicCast(obj, wxStyledTextCtrl);
	CodeControlClass* code = Notebook->GetCurrentCodeControl();
	if (t != NULL) {
		t->Cut();
	}
	else if (combo != NULL) {
		combo->Cut();
	}
	else if (stc != NULL) {
		stc->Cut();
	}
	else if (code != NULL) {
		code->Cut();
	}
	else {
		event.Skip();
	}
}

void mvceditor::AppFrameClass::OnEditCopy(wxCommandEvent& event) {

	// need to handle copy in all text controls
	wxWindow* obj = wxWindow::FindFocus();
	wxTextCtrl* t = wxDynamicCast(obj, wxTextCtrl);
	wxComboBox* combo = wxDynamicCast(obj, wxComboBox);
	wxStyledTextCtrl* stc = wxDynamicCast(obj, wxStyledTextCtrl);
	CodeControlClass* code = Notebook->GetCurrentCodeControl();
	if (t != NULL) {
		t->Copy();
	}
	else if (combo != NULL) {
		combo->Copy();
	}
	else if (stc != NULL) {
		stc->Copy();
	}
	else if (code != NULL) {
		code->Copy();
	}
	else {
		event.Skip();
	}
}

void mvceditor::AppFrameClass::OnEditPaste(wxCommandEvent& event) {

	// need to handle paste in all text controls
	wxWindow* obj = wxWindow::FindFocus();
	wxTextCtrl* t = wxDynamicCast(obj, wxTextCtrl);
	wxStyledTextCtrl* stc = wxDynamicCast(obj, wxStyledTextCtrl);
	wxComboBox* combo = wxDynamicCast(obj, wxComboBox);
	CodeControlClass* code = Notebook->GetCurrentCodeControl();
	if (t != NULL) {
		t->Paste();
	}
	else if (combo != NULL) {
		combo->Paste();
	}
	else if (stc != NULL) {
		stc->Paste();
	}
	else if (code != NULL) {
		code->Paste();
	}
	else {
		event.Skip();
	}
}

void mvceditor::AppFrameClass::OnEditSelectAll(wxCommandEvent& event) {

	// need to handle select All in all text controls
	wxWindow* obj = wxWindow::FindFocus();
	wxTextCtrl* t = wxDynamicCast(obj, wxTextCtrl);
	wxStyledTextCtrl* stc = wxDynamicCast(obj, wxStyledTextCtrl);
	wxComboBox* combo = wxDynamicCast(obj, wxComboBox);
	CodeControlClass* code = Notebook->GetCurrentCodeControl();
	if (t != NULL) {
		t->SelectAll();
	}
	else if (combo != NULL) {
		combo->SelectAll();
	}
	else if (stc != NULL) {
		stc->SelectAll();
	}
	else if (code != NULL) {
		code->SelectAll();
	}
	else {
		event.Skip();
	}
}

void mvceditor::AppFrameClass::OnEditPreferences(wxCommandEvent& event) {
	PreferencesDialogClass prefDialog(this, Preferences);
	for (size_t i = 0; i < Plugins.size(); ++i) {
		Plugins[i]->AddPreferenceWindow(prefDialog.GetBookCtrl());
	}
	prefDialog.Prepare();
	int exitCode = prefDialog.ShowModal();
	if (wxOK == exitCode) {
		Notebook->RefreshCodeControlOptions();
		wxCommandEvent evt(EVENT_APP_SAVE_PREFERENCES);
		wxPostEvent(&AppHandler, evt);
	}
}

void mvceditor::AppFrameClass::OnEditContentAssist(wxCommandEvent& event) {
	CodeControlClass* page = Notebook->GetCurrentCodeControl();
	if (page) {
		page->HandleAutoCompletion();
	}
}

void mvceditor::AppFrameClass::OnEditCallTip(wxCommandEvent& event) {
	CodeControlClass* page = Notebook->GetCurrentCodeControl();
	if (page) {
		page->HandleCallTip(0, true);
	}
}

void mvceditor::AppFrameClass::EnableSave(wxStyledTextEvent& event) {
	ToolBar->EnableTool(ID_TOOLBAR_SAVE, true);
	ToolBar->Refresh();
	
	// if I don't call this then the stretch sizer on the toolbar disappears
	// before was using ToolBar->AddStretchSpacer(1); which needed the Layout call
	//Layout();  commenting this out because it is causing find/find in files replace all matches in file to not work
	MenuBar->Enable(wxID_SAVEAS, true);
	MenuBar->Enable(ID_FILE_REVERT, true);
	Notebook->MarkPageAsModified(event.GetId());
}

void mvceditor::AppFrameClass::DisableSave(wxStyledTextEvent& event) {
	ToolBar->EnableTool(ID_TOOLBAR_SAVE, false);
	ToolBar->Refresh();
	
	// if I don't call this then the stretch sizer on the toolbar disappears
	// before was using ToolBar->AddStretchSpacer(1); which needed the Layout call
	//Layout(); commenting this out because it is causing find/find in files replace all matches in file to not work
	MenuBar->Enable(wxID_SAVEAS, false);
	MenuBar->Enable(ID_FILE_REVERT, false);
	Notebook->MarkPageAsNotModified(event.GetId());
}

void mvceditor::AppFrameClass::OnContentNotebookPageChanged(wxAuiNotebookEvent& event) {
	int newPage = event.GetSelection();
	bool isPageModified = Notebook->IsPageModified(newPage);
	ToolBar->EnableTool(ID_TOOLBAR_SAVE, isPageModified);
	ToolBar->Refresh();
	
	// if I don't call this then the stretch sizer on the toolbar disappears
	//Layout(); commenting this out because it is causing find/find in files replace all matches in file to not work
	MenuBar->Enable(wxID_SAVEAS, isPageModified);
	MenuBar->Enable(ID_FILE_REVERT, isPageModified);
	event.Skip();
}

void mvceditor::AppFrameClass::OnProjectOpen(wxCommandEvent& event) {
	wxDirDialog dialog(this, _("Choose Project Location"), wxT(""), 
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST | wxDD_CHANGE_DIR);
	if (wxID_OK == dialog.ShowModal()) {
		bool destroy = Notebook->SaveAllModifiedPages();
		if (destroy) {
			wxCommandEvent evt(EVENT_APP_OPEN_PROJECT);
			evt.SetString(dialog.GetPath());
			wxPostEvent(&AppHandler, evt);
		}
	}
}

void mvceditor::AppFrameClass::OnProjectOpened(mvceditor::ProjectClass* project, wxEvtHandler* appHandler) {
	Notebook->SetProject(project, appHandler);
	SetTitle(_("MVC Editor - Open Project [") + project->GetRootPath() + wxT("]"));
}

void mvceditor::AppFrameClass::OnProjectClosed(wxEvtHandler* appHandler) {
	Notebook->CloseAllPages();
	AuiManager.GetPane(ToolsNotebook).Hide();
	while (ToolsNotebook->GetPageCount()) {
		ToolsNotebook->DeletePage(0);
	}
	AuiManager.Update();
	Notebook->SetProject(NULL, appHandler);
}

void mvceditor::AppFrameClass::OnHelpAbout(wxCommandEvent& event) {
	wxAboutDialogInfo info;
	info.AddDeveloper(wxT("Roberto Perpuly"));
	info.SetCopyright(wxT("(c)2009-2011 Roberto Perpuly"));
	info.SetDescription(wxT("MVC Editor is an Integrated Development Environment for PHP Web Applications"));
	info.SetName(wxT("MVC Editor"));
	info.SetVersion(wxString::Format(wxT("%s.%s.%s"), wxT(APP_MAJOR_VERSION), wxT(APP_MINOR_VERSION), wxT(APP_BUILD_NUMBER)));
	wxAboutBox(info);
}

void mvceditor::AppFrameClass::SaveCurrentFile(wxCommandEvent& event) {
	Notebook->SaveCurrentPage();
}

void mvceditor::AppFrameClass::OnUpdateUi(wxUpdateUIEvent& event) {
	bool hasEditors = Notebook->GetPageCount() > 0;
	int currentPage = Notebook->GetSelection();
	CodeControlClass* code = Notebook->GetCurrentCodeControl();
	wxMenuItem* menuItem = MenuBar->FindItem(ID_EDIT_CONTENT_ASSIST);
	menuItem->Enable(hasEditors);
	menuItem = MenuBar->FindItem(ID_EDIT_CALL_TIP);
	menuItem->Enable(hasEditors);
	menuItem = MenuBar->FindItem(wxID_SAVE);
	menuItem->Enable(hasEditors && Notebook->IsPageModified(currentPage));
	menuItem = MenuBar->FindItem(wxID_SAVEAS);
	menuItem->Enable(hasEditors && NULL != code);
	menuItem = MenuBar->FindItem(ID_FILE_CLOSE);
	menuItem->Enable(hasEditors);
	menuItem = MenuBar->FindItem(ID_FILE_REVERT);
	menuItem->Enable(hasEditors && Notebook->IsPageModified(currentPage) && NULL != code && !code->IsNew());
	event.Skip();
}

void mvceditor::AppFrameClass::CreateToolBarButtons() {
	ToolBar->AddTool(ID_TOOLBAR_SAVE, _("Save"), wxArtProvider::GetBitmap(
		wxART_FILE_SAVE, wxART_TOOLBAR, wxSize(16, 16)), _("Save"));
	ToolBar->EnableTool(ID_TOOLBAR_SAVE, false);
	ToolBar->Realize();
}

void mvceditor::AppFrameClass::AuiManagerUpdate() {
	ToolBar->Realize();
	AuiManager.Update();
}

void mvceditor::AppFrameClass::LoadPlugin(mvceditor::PluginClass* plugin) {
	
	// propagate GUI events to plugins, so that they can handle menu events themselves
	// plugin menus
	plugin->InitWindow(GetStatusBarWithGauge(), Notebook, ToolsNotebook, &AuiManager, GetMenuBar());
	
	//  when adding the separators, we dont want a separator at the very end
	// we dont need separators if the plugin did not add any menu items
	size_t oldEditMenuCount = EditMenu->GetMenuItemCount();
	plugin->AddEditMenuItems(EditMenu);
	if (oldEditMenuCount != EditMenu->GetMenuItemCount() && oldEditMenuCount > 0) {
		EditMenu->InsertSeparator(oldEditMenuCount);
	}
	size_t oldProjectMenuCount = ProjectMenu->GetMenuItemCount();
	plugin->AddProjectMenuItems(ProjectMenu);
	if (oldProjectMenuCount != ProjectMenu->GetMenuItemCount() && oldProjectMenuCount > 0) {
		ProjectMenu->InsertSeparator(oldProjectMenuCount);
	}		
	size_t oldToolsMenuCount = ToolsMenu->GetMenuItemCount();
	plugin->AddToolsMenuItems(ToolsMenu);
	if (oldToolsMenuCount != ToolsMenu->GetMenuItemCount() && oldToolsMenuCount > 0) {
		ToolsMenu->InsertSeparator(oldToolsMenuCount);
	}
	wxMenuBar* menuBar = GetMenuBar();
	plugin->AddNewMenu(menuBar);

	// new menus may have been added; push the Help menu all the way to the end
	int helpIndex = menuBar->FindMenu(_("&Help"));
	if (helpIndex != wxNOT_FOUND) {
		menuBar->Remove(helpIndex);
		menuBar->Insert(menuBar->GetMenuCount(), HelpMenu, _("&Help"));
	}
	
	// move preferences menu to the end, similar to most other programs
	wxMenuItem* preferencesMenu = EditMenu->Remove(ID_EDIT_PREFERENCES);
	EditMenu->Append(preferencesMenu);

	plugin->AddToolBarItems(ToolBar);
	plugin->AddWindows();
}

void mvceditor::AppFrameClass::OnContextMenu(wxContextMenuEvent& event) {
	CodeControlClass* codeWindow = Notebook->GetCurrentCodeControl();
	
	// only show the user if and only if
	// user clicked inside of the code control
	if (codeWindow != NULL && event.GetEventObject() == codeWindow) {
		wxMenu contextMenu;
		contextMenu.Append(wxID_CUT, _("Cut"));
		contextMenu.Append(wxID_COPY, _("Copy"));
		contextMenu.Append(wxID_PASTE, _("Paste"));
		contextMenu.Append(wxID_UNDO, _("Undo"));
		contextMenu.Append(wxID_REDO, _("Redo"));
		contextMenu.Append(wxID_SELECTALL, _("Select All"));
		contextMenu.AppendSeparator();
		contextMenu.Append(wxID_FIND, _("Find"));
		contextMenu.AppendSeparator();
		for (size_t i = 0; i < Plugins.size(); ++i) {
			Plugins[i]->AddCodeControlClassContextMenuItems(&contextMenu);
		}
		
		// no need to delete moreMenu pointer, the contextMenu will delete it for us
		wxMenu* moreMenu = new wxMenu;
		moreMenu->Append(ID_CUT_LINE, _("Cut Line\tCTRL+L"));
		moreMenu->Append(ID_DUPLICATE_LINE, _("Duplicate Line\tCTRL+D"));
		moreMenu->Append(ID_DELETE_LINE, _("Delete Line\tCTRL+SHIFT+L"));
		moreMenu->Append(ID_TRANSPOSE_LINE, _("Transpose Line\tCTRL+T"));
		moreMenu->Append(ID_COPY_LINE, _("Copy Line\tCTRL+SHIFT+T"));
		moreMenu->Append(ID_LOWERCASE, _("Convert To Lower Case\tCTRL+U"));
		moreMenu->Append(ID_UPPERCASE, _("Convert To Upper Case\tCTRL+SHIFT+U"));
		contextMenu.Append(ID_MENU_MORE, _("More"), moreMenu);
		bool isTextSelected = !codeWindow->GetSelectedText().IsEmpty();
		contextMenu.Enable(wxID_CUT, isTextSelected);
		contextMenu.Enable(wxID_COPY, isTextSelected);
		contextMenu.Enable(wxID_PASTE, codeWindow->CanPaste());
		contextMenu.Enable(wxID_UNDO, codeWindow->CanUndo());
		contextMenu.Enable(wxID_REDO, codeWindow->CanRedo());
		moreMenu->Enable(ID_LOWERCASE, isTextSelected);
		moreMenu->Enable(ID_UPPERCASE, isTextSelected);
		PopupMenu(&contextMenu);
	}
	else {
		
		// if a sub panel has its own handler, use it
		event.Skip();
	}
}

void mvceditor::AppFrameClass::OnUndo(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->Undo();
	}
}

void mvceditor::AppFrameClass::OnRedo(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->Redo();
	}
}

void mvceditor::AppFrameClass::OnCutLine(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_LINECUT);
	}
}

void mvceditor::AppFrameClass::OnDuplicateLine(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_LINEDUPLICATE);
	}
}

void mvceditor::AppFrameClass::OnDeleteLine(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_LINEDELETE);
	}
}

void mvceditor::AppFrameClass::OnTransposeLine(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_LINETRANSPOSE);
	}
}

void mvceditor::AppFrameClass::OnCopyLine(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_LINECOPY);
	}
}

void mvceditor::AppFrameClass::OnLowecase(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_LOWERCASE);
	}
}

void mvceditor::AppFrameClass::OnUppercase(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_UPPERCASE);
	}
}

void mvceditor::AppFrameClass::OnCodeControlUpdate(wxStyledTextEvent& event) {
	CodeControlClass* codeControl = Notebook->GetCurrentCodeControl();
	if (codeControl) {
		int pos = codeControl->GetCurrentPos();

		// scintilla lines and columns are zero-based 
		// we want to be 1-based (human friendly)
		// but offset we want to be 0-based
		int line = codeControl->LineFromPosition(pos) + 1; 
		int column = codeControl->GetColumn(pos) + 1;
		GetStatusBarWithGauge()->SetColumn1Text(
			wxString::Format(wxT("Line:%d Column:%d Offset:%d"), line, column, pos)
		);
	}
}

void mvceditor::AppFrameClass::OnToolsNotebookPageClosed(wxAuiNotebookEvent& event) {
	size_t count = ToolsNotebook->GetPageCount();

	// this event is received AFTER the page is removed
	if (count <= 0) {
		AuiManager.GetPane(ToolsNotebook).Hide();
		AuiManager.Update();
	}
	event.Skip();
}

void mvceditor::AppFrameClass::DefaultKeyboardShortcuts() {

	// ATTN: when a new menu item is added to the form builder 
	// we will need to add an entry here so that shortcuts
	// work properly
	std::map<int, wxString>defaultMenus;
	defaultMenus[ID_FILE_PHP_NEW] = wxT("File-New PHP File");
	defaultMenus[wxID_OPEN] = wxT("File-Open");
	defaultMenus[wxID_SAVE] = wxT("File-Save");
	defaultMenus[wxID_SAVEAS] = wxT("File-Save As");
	defaultMenus[ID_FILE_REVERT] = wxT("File-Revert");
	defaultMenus[ID_FILE_CLOSE] = wxT("File-Close");
	defaultMenus[wxID_EXIT] = wxT("File-Exit");
	defaultMenus[wxID_CUT] = wxT("Edit-Cut");
	defaultMenus[wxID_COPY] = wxT("Edit-Copy");
	defaultMenus[wxID_PASTE] = wxT("Edit-Paste");
	defaultMenus[wxID_SELECTALL] = wxT("Edit-Select All");
	defaultMenus[ID_EDIT_CONTENT_ASSIST] = wxT("Edit-Content Assist");
	defaultMenus[ID_EDIT_CALL_TIP] = wxT("Edit-Call Tip");
	defaultMenus[ID_EDIT_PREFERENCES] = wxT("Edit-Preferences");
	defaultMenus[ID_PROJECT_OPEN] = wxT("Project-Open");
	defaultMenus[ID_ABOUT] = wxT("Help-About");
	wxMenuBar* menuBar = GetMenuBar();
	for (std::map<int, wxString>::iterator it = defaultMenus.begin(); it != defaultMenus.end(); ++it) {
		wxMenuItem* item = menuBar->FindItem(it->first);
		wxASSERT_MSG(item, wxT("Menu item not found:") + it->second);
		mvceditor::DynamicCmdClass cmd(item, it->second);
		Preferences.DefaultKeyboardShortcutCmds.push_back(cmd);
	}
}

BEGIN_EVENT_TABLE(mvceditor::AppFrameClass,  AppFrameGeneratedClass)
	EVT_STC_SAVEPOINTREACHED(wxID_ANY, mvceditor::AppFrameClass::DisableSave)
	EVT_STC_SAVEPOINTLEFT(wxID_ANY, mvceditor::AppFrameClass::EnableSave)
	EVT_STC_UPDATEUI(wxID_ANY, mvceditor::AppFrameClass::OnCodeControlUpdate)
	EVT_UPDATE_UI(wxID_ANY, mvceditor::AppFrameClass::OnUpdateUi)
	EVT_CONTEXT_MENU(mvceditor::AppFrameClass::OnContextMenu)
	EVT_MENU(wxID_UNDO, mvceditor::AppFrameClass::OnUndo)
	EVT_MENU(wxID_REDO, mvceditor::AppFrameClass::OnRedo)
	EVT_MENU(ID_CUT_LINE, mvceditor::AppFrameClass::OnCutLine)
	EVT_MENU(ID_DUPLICATE_LINE, mvceditor::AppFrameClass::OnDuplicateLine)
	EVT_MENU(ID_DELETE_LINE, mvceditor::AppFrameClass::OnDeleteLine)
	EVT_MENU(ID_TRANSPOSE_LINE, mvceditor::AppFrameClass::OnTransposeLine)
	EVT_MENU(ID_COPY_LINE, mvceditor::AppFrameClass::OnCopyLine)
	EVT_MENU(ID_LOWERCASE, mvceditor::AppFrameClass::OnLowecase)
	EVT_MENU(ID_UPPERCASE, mvceditor::AppFrameClass::OnUppercase)
	EVT_MENU(ID_TOOLBAR_SAVE, mvceditor::AppFrameClass::SaveCurrentFile)
	EVT_AUINOTEBOOK_PAGE_CLOSED(ID_TOOLS_WINDOW, mvceditor::AppFrameClass::OnToolsNotebookPageClosed)
END_EVENT_TABLE()