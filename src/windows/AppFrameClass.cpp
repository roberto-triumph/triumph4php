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
 * @author     $Author: robertop2004@gmail.com $
 * @date       $Date: 2011-08-13 20:31:56 -0700 (Sat, 13 Aug 2011) $
 * @version    $Rev: 596 $ 
 */
#include <AppVersion.h>
#include <windows/AppFrameClass.h>
#include <windows/PreferencesDialogClass.h>
#include <widgets/StatusBarWithGaugeClass.h>
#include <plugins/EnvironmentPluginClass.h>
#include <plugins/FindInFilesPluginClass.h>
#include <plugins/FinderPluginClass.h>
#include <plugins/ProjectPluginClass.h>
#include <plugins/OutlineViewPluginClass.h>
#include <plugins/ResourcePluginClass.h>
#include <plugins/RunConsolePluginClass.h>
#include <plugins/WebBrowserPanelClass.h>
#include <plugins/LintPluginClass.h>

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

mvceditor::AppFrameClass::AppFrameClass(wxWindow* parent)
		: AppFrameGeneratedClass(parent)
		, Plugins()
		, Project(NULL)
		, Environment()
		, Preferences() {
	PreferencesClass::InitConfig();
	StatusBarWithGaugeClass* gauge = new StatusBarWithGaugeClass(this);
	SetStatusBar(gauge);
	
	AuiManager.SetManagedWindow(this);
	ToolBar = new wxAuiToolBar(this, ID_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxAUI_TB_TEXT | wxAUI_TB_DEFAULT_STYLE);
	
	// when the notebook is empty we want to accept dragged files
	FileDropTarget = new FileDropTargetClass(Notebook);
	Notebook->SetDropTarget(FileDropTarget);
	Notebook->CodeControlOptions = &Preferences.CodeControlOptions;
	
	// ATTN: for some reason must remove and re-insert menu item in order to change the icon
	wxMenuItem* projectOpenMenuItem = MenuBar->FindItem(ID_PROJECT_OPEN);
	projectOpenMenuItem = ProjectMenu->Remove(projectOpenMenuItem);
	projectOpenMenuItem->SetBitmap(wxArtProvider::GetBitmap(wxART_FOLDER_OPEN, wxART_MENU));
	ProjectMenu->Insert(0, projectOpenMenuItem);
	ToolsNotebook = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
		wxAUI_NB_TOP | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB | wxAUI_NB_TAB_MOVE);
	
	
	// For instances when the user does not open a project, we still want code assist to work for native
	// php functions.
	// be careful when moving the project creation code around, as the order of creation of the 
	// project vs. the various dialogs matters (especially the tools notebook)
	ProjectOptionsClass options;
	options.RootPath = wxT("");
	options.Framework = GENERIC;
	CreateProject(options);
	
	
	Environment.LoadFromConfig();
	
	// order of loading is defined.  first load plugins, then load user options
	LoadPlugins();
	Preferences.Load(this);		
	wxConfigBase* config = wxConfigBase::Get();
	for (size_t i = 0; i < Plugins.size(); ++i) {
		Plugins[i]->LoadPreferences(config);
	}
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
}

mvceditor::AppFrameClass::~AppFrameClass() {
	DeletePlugins();
	CloseProject();
	AuiManager.UnInit();
}

mvceditor::StatusBarWithGaugeClass* mvceditor::AppFrameClass::GetStatusBarWithGauge() {
	return (StatusBarWithGaugeClass*)GetStatusBar();
}

mvceditor::ProjectClass* mvceditor::AppFrameClass::GetProject() {
	return Project;
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
	wxCommandEvent pluginEvent(EVENT_PLUGIN_FILE_SAVED);
	pluginEvent.SetString(codeControl->GetFileName());
	pluginEvent.SetEventObject(codeControl);
	for (size_t i = Plugins.size() - 1; i < Plugins.size(); i++) {
		Plugins[i]->ProcessEvent(pluginEvent);
	}
}

void mvceditor::AppFrameClass::OnFileNew(wxCommandEvent& event) {
	Notebook->AddMvcEditorPage();
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
	int exitCode;
	{
		PreferencesDialogClass prefDialog(this, Preferences, GetMenuBar());
		for (size_t i = 0; i < Plugins.size(); ++i) {
			Plugins[i]->AddPreferenceWindow(prefDialog.GetBookCtrl());
		}
		prefDialog.Prepare();
		exitCode = prefDialog.ShowModal();
		if (wxOK == exitCode) {
			Notebook->RefreshCodeControlOptions();
		}
	}
	if (wxOK == exitCode) {
		Preferences.LoadKeyboardShortcuts(this);
		wxConfigBase* config = wxConfigBase::Get();
		for (size_t i = 0; i < Plugins.size(); ++i) {
			Plugins[i]->SavePreferences(config);
		}
		config->Flush();
	}
}

void mvceditor::AppFrameClass::OnEditContentAssist(wxCommandEvent& event) {
	CodeControlClass* page = Notebook->GetCurrentCodeControl();
	if (page) {
		page->HandleAutoCompletion(true);
	}
}

void mvceditor::AppFrameClass::OnEditCallTip(wxCommandEvent& event) {
	CodeControlClass* page = Notebook->GetCurrentCodeControl();
	if (page) {
		page->HandleCallTip(wxT('('));
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

void mvceditor::AppFrameClass::PhpSourceCodeCtrlPageChanged(wxAuiNotebookEvent& event) {
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
		ProjectOpen(dialog.GetPath());
	}
}

void mvceditor::AppFrameClass::ProjectOpen(const wxString& directoryPath) {
	ProjectOptionsClass options;
	options.RootPath = directoryPath;
	options.Framework = GENERIC;
	bool destroy = Notebook->SaveAllModifiedPages();
	if (destroy) {
		Notebook->CloseAllPages();
		CloseProject();
		CreateProject(options);
		SetTitle(_("MVC Editor - Open Project - ") + Project->GetRootPath());
	}
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
	for (size_t i = 0; i < Plugins.size(); ++i) {
		Plugins[i]->AddToolBarItems(ToolBar);
		Plugins[i]->AddWindows();
	}
	ToolBar->EnableTool(ID_TOOLBAR_SAVE, false);
	ToolBar->Realize();
}

void mvceditor::AppFrameClass::CreateProject(const ProjectOptionsClass& options) {
	Project = ProjectClass::Factory(options);
	Project->GetResourceFinder()->BuildResourceCacheForNativeFunctions();
	Notebook->SetProject(Project);
	for (size_t i = 0; i < Plugins.size(); ++i) {
		Plugins[i]->SetProject(Project);
	}
}

void mvceditor::AppFrameClass::CloseProject() {
	AuiManager.GetPane(ToolsNotebook).Hide();
	AuiManager.Update();
	while (ToolsNotebook->GetPageCount()) {
		ToolsNotebook->DeletePage(0);
	}
	Notebook->SetProject(NULL);
	for (size_t i = 0; i < Plugins.size(); ++i) {
		Plugins[i]->SetProject(NULL);
	}
	if (Project) {
		delete Project;
		Project = NULL;
	}
}

void mvceditor::AppFrameClass::LoadPlugins() {
	PluginClass* plugin = new RunConsolePluginClass();
	Plugins.push_back(plugin);
	plugin = new FinderPluginClass();
	Plugins.push_back(plugin);
	plugin = new FindInFilesPluginClass();
	Plugins.push_back(plugin);
	plugin = new ResourcePluginClass();
	Plugins.push_back(plugin);
	plugin = new WebBrowserPluginClass();
	Plugins.push_back(plugin);
	plugin = new EnvironmentPluginClass();
	Plugins.push_back(plugin);
	plugin = new ProjectPluginClass();
	Plugins.push_back(plugin);
	plugin = new OutlineViewPluginClass();
	Plugins.push_back(plugin);
	plugin = new LintPluginClass();
	Plugins.push_back(plugin);
	
	// test plugin need to find a quicker way to toggling it ON / OFF
	//plugin = new TestPluginClass();
	//Plugins.push_back(plugin);
	
	// propagate GUI events to plugins, so that they can handle menu events themselves
	// plugin menus
	for (size_t i = 0; i < Plugins.size(); ++i) {
		Plugins[i]->SetProject(Project);
		Plugins[i]->InitWindow(GetStatusBarWithGauge(), Notebook, ToolsNotebook, &AuiManager);
		Plugins[i]->InitState(&Environment);
		PushEventHandler(Plugins[i]);
		
		//  when adding the separators, we dont want a separator at the very end
		// we dont need separators if the plugin did not add any menu items
		size_t oldEditMenuCount = EditMenu->GetMenuItemCount();
		Plugins[i]->AddEditMenuItems(EditMenu);
		if (oldEditMenuCount != EditMenu->GetMenuItemCount() && oldEditMenuCount > 0) {
			EditMenu->InsertSeparator(oldEditMenuCount);
		}
		size_t oldProjectMenuCount = ProjectMenu->GetMenuItemCount();
		Plugins[i]->AddProjectMenuItems(ProjectMenu);
		if (oldProjectMenuCount != ProjectMenu->GetMenuItemCount() && oldProjectMenuCount > 0) {
			ProjectMenu->InsertSeparator(oldProjectMenuCount);
		}		
		size_t oldToolsMenuCount = ToolsMenu->GetMenuItemCount();
		Plugins[i]->AddToolsMenuItems(ToolsMenu);
		if (oldToolsMenuCount != ToolsMenu->GetMenuItemCount() && oldToolsMenuCount > 0) {
			ToolsMenu->InsertSeparator(oldToolsMenuCount);
		}
		Plugins[i]->AddNewMenu(GetMenuBar());
	}
	
	// move preferences menu to the end, similar to most other programs
	wxMenuItem* preferencesMenu = EditMenu->Remove(ID_EDIT_PREFERENCES);
	EditMenu->Append(preferencesMenu);
}

void mvceditor::AppFrameClass::DeletePlugins() {
	for (size_t i = 0; i < Plugins.size(); ++i) {
		PopEventHandler();
	}
	
	// if i delete in the same loop as the PopEventHandler, wx assertions fail.
	for (size_t i = 0; i < Plugins.size(); ++i) {
		delete Plugins[i];
	}
	Plugins.clear();
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
	Notebook->GetCurrentCodeControl()->Undo();
}

void mvceditor::AppFrameClass::OnRedo(wxCommandEvent& event) {
	Notebook->GetCurrentCodeControl()->Redo();
}

void mvceditor::AppFrameClass::OnCutLine(wxCommandEvent& event) {
	Notebook->GetCurrentCodeControl()->CmdKeyExecute(wxSTC_CMD_LINECUT);
}

void mvceditor::AppFrameClass::OnDuplicateLine(wxCommandEvent& event) {
	Notebook->GetCurrentCodeControl()->CmdKeyExecute(wxSTC_CMD_LINEDUPLICATE);
}

void mvceditor::AppFrameClass::OnDeleteLine(wxCommandEvent& event) {
	Notebook->GetCurrentCodeControl()->CmdKeyExecute(wxSTC_CMD_LINEDELETE);
}

void mvceditor::AppFrameClass::OnTransposeLine(wxCommandEvent& event) {
	Notebook->GetCurrentCodeControl()->CmdKeyExecute(wxSTC_CMD_LINETRANSPOSE);
}

void mvceditor::AppFrameClass::OnCopyLine(wxCommandEvent& event) {
	Notebook->GetCurrentCodeControl()->CmdKeyExecute(wxSTC_CMD_LINECOPY);
}

void mvceditor::AppFrameClass::OnLowecase(wxCommandEvent& event) {
	Notebook->GetCurrentCodeControl()->CmdKeyExecute(wxSTC_CMD_LOWERCASE);
}

void mvceditor::AppFrameClass::OnUppercase(wxCommandEvent& event) {
	Notebook->GetCurrentCodeControl()->CmdKeyExecute(wxSTC_CMD_UPPERCASE);
}

void mvceditor::AppFrameClass::OnCodeControlUpdate(wxStyledTextEvent& event) {
	CodeControlClass* codeControl = Notebook->GetCurrentCodeControl();
	if (codeControl) {
		int pos = codeControl->GetCurrentPos();
		int line = codeControl->LineFromPosition(pos);
		int column = codeControl->GetColumn(pos);
		GetStatusBar()->SetStatusText(wxString::Format(wxT("Line:%d Column:%d Offset:%d"), line, column, pos));		
	}
}

BEGIN_EVENT_TABLE(mvceditor::AppFrameClass, AppFrameGeneratedClass)
	EVT_STC_SAVEPOINTREACHED(wxID_ANY, mvceditor::AppFrameClass::DisableSave)
	EVT_STC_SAVEPOINTLEFT(wxID_ANY, mvceditor::AppFrameClass::EnableSave)
	EVT_STC_UPDATEUI(wxID_ANY, mvceditor::AppFrameClass::OnCodeControlUpdate)
	EVT_AUINOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK, 
			mvceditor::AppFrameClass::PhpSourceCodeCtrlPageChanged)
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
END_EVENT_TABLE()