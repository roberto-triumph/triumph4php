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
#include <main_frame/MainFrameClass.h>
#include <AppVersion.h>
#include <features/FeatureClass.h>
#include <MvcEditor.h>
#include <main_frame/PreferencesDialogClass.h>
#include <widgets/StatusBarWithGaugeClass.h>

#include <wx/artprov.h>
#include <wx/choicdlg.h>
#include <wx/filename.h>
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
static int ID_SEQUENCE_GAUGE = wxNewId();

mvceditor::MainFrameClass::MainFrameClass(const std::vector<mvceditor::FeatureClass*>& features,
										mvceditor::AppClass& app,
										mvceditor::PreferencesClass& preferences)
	: MainFrameGeneratedClass(NULL)
	, Features(features)
	, Listener(this)
	, App(app)
	, Preferences(preferences)
	, ToolBar(NULL)
	, ToolsNotebook(NULL)
	, OutlineNotebook(NULL) {
	StatusBarWithGaugeClass* gauge = new StatusBarWithGaugeClass(this);
	SetStatusBar(gauge);

	App.EventSink.PushHandler(&Listener);
	App.RunningThreads.AddEventHandler(this);
	
	AuiManager.SetManagedWindow(this);
	ToolBar = new wxAuiToolBar(this, ID_TOOLBAR, wxDefaultPosition, wxDefaultSize, 
		  wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_TEXT | wxAUI_TB_HORZ_TEXT);
	
	// when the notebook is empty we want to accept dragged files
	Notebook->SetDropTarget(new FileDropTargetClass(Notebook));
	Notebook->CodeControlOptions = &Preferences.CodeControlOptions;
	Notebook->Globals = &App.Globals;
	Notebook->EventSink = &App.EventSink;
	
	ToolsNotebook = new wxAuiNotebook(this, mvceditor::ID_TOOLS_NOTEBOOK, wxDefaultPosition, wxDefaultSize, 
		wxAUI_NB_TOP | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB | wxAUI_NB_TAB_MOVE);
	OutlineNotebook = new wxAuiNotebook(this, mvceditor::ID_OUTLINE_NOTEBOOK, wxDefaultPosition, wxDefaultSize, 
		wxAUI_NB_TOP | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB | wxAUI_NB_TAB_MOVE);
	CreateToolBarButtons();
	
	// setup the bottom "tools" pane, the main content pane, and the toolbar on top
	AuiManager.AddPane(Notebook, wxAuiPaneInfo().CentrePane(
		).PaneBorder(true).Gripper(false).Floatable(false).Resizable(true));
	AuiManager.AddPane(ToolsNotebook, wxAuiPaneInfo().Bottom().Caption(
		_("Tools")).Floatable(false).MinSize(-1, 260).Hide().Layer(1));
	AuiManager.AddPane(OutlineNotebook, wxAuiPaneInfo().Left().Caption(
        _("Outlines")).Floatable(false).MinSize(260, -1).Hide());
	
	AuiManager.AddPane(ToolBar, wxAuiPaneInfo().Top(
		).CaptionVisible(false).CloseButton(false).Gripper(
		false).DockFixed(true).PaneBorder(false).Floatable(false).Row(0).Position(0));
	DefaultKeyboardShortcuts();
}

mvceditor::MainFrameClass::~MainFrameClass() {
	AuiManager.UnInit();
}

mvceditor::StatusBarWithGaugeClass* mvceditor::MainFrameClass::GetStatusBarWithGauge() {
	return (StatusBarWithGaugeClass*)GetStatusBar();
}

void mvceditor::MainFrameClass::OnClose(wxCloseEvent& event) {
	bool destroy = true;
	if (event.CanVeto()) {
		destroy = Notebook->SaveAllModifiedPages();
	}
	if (destroy) {
		wxCommandEvent exitEvent(mvceditor::EVENT_APP_EXIT);
		App.EventSink.Publish(exitEvent);
		
		// need to detach the window from the keyboard BEFORE the window is invalidated
		Preferences.KeyProfiles.GetSelProfile()->DetachAll();	

		// need to remove the logging to the messages tab. We need to do this
		// so that because a log message will try to be written to the 
		// messages grid but the frame is about to be destroyed
		wxLog* logger = wxLog::SetActiveTarget(NULL);
		if (logger) {
			delete logger;
		}
		wxLog::DontCreateOnDemand();
		App.Sequences.Stop();

		App.RunningThreads.RemoveEventHandler(this);
		App.RunningThreads.StopAll();
		

		// delete the features first so that we can destroy
		// the windows without worrying if the features
		// may access them.
		App.DeleteFeatures();
		

		// cleanup all open code controls and tabs. this is because
		// we want to destroy those items because they may have
		// threads that are running
		Notebook->CloseAllPages();
		while (ToolsNotebook->GetPageCount() > 0) {
			ToolsNotebook->DeletePage(0);
		}
		while (OutlineNotebook->GetPageCount() > 0) {
			OutlineNotebook->DeletePage(0);
		}
		event.Skip();
	}
}

void mvceditor::MainFrameClass::OnFileSave(wxCommandEvent& event) {
	Notebook->SaveCurrentPage();
	mvceditor::CodeControlClass* codeControl = Notebook->GetCurrentCodeControl();
	mvceditor::FileSavedEventClass featureEvent(codeControl);

	for (size_t i = 0; i < Features.size(); i++) {
		wxPostEvent(Features[i], featureEvent);
	}
}

void mvceditor::MainFrameClass::OnFilePhpNew(wxCommandEvent& event) {
	Notebook->AddMvcEditorPage(mvceditor::CodeControlClass::PHP);
}

void mvceditor::MainFrameClass::OnFileSqlNew(wxCommandEvent& event) {
	Notebook->AddMvcEditorPage(mvceditor::CodeControlClass::SQL);
}

void mvceditor::MainFrameClass::OnFileCssNew(wxCommandEvent& event) {
	Notebook->AddMvcEditorPage(mvceditor::CodeControlClass::CSS);
}

void mvceditor::MainFrameClass::OnFileTextNew(wxCommandEvent& event) {
	Notebook->AddMvcEditorPage(mvceditor::CodeControlClass::TEXT);
}

void mvceditor::MainFrameClass::OnFileOpen(wxCommandEvent& event) {
	Notebook->LoadPage();
}

void mvceditor::MainFrameClass::OnFileSaveAs(wxCommandEvent& event) {
	Notebook->SaveCurrentPageAsNew();
}

void mvceditor::MainFrameClass::FileOpen(const std::vector<wxString>& filenames) {
	Notebook->LoadPages(filenames);
}

void mvceditor::MainFrameClass::OnFileClose(wxCommandEvent& event) {
	Notebook->CloseCurrentPage();
}

void mvceditor::MainFrameClass::OnFileExit(wxCommandEvent& event) {
	Close();
}

void mvceditor::MainFrameClass::OnFileRevert(wxCommandEvent& event) {
	CodeControlClass* code = Notebook->GetCurrentCodeControl();
	if (NULL != code && !code->IsNew()) {
		int res = wxMessageBox(_("Reload file and lose all changes?"), _("MVC Editor"), wxICON_QUESTION | wxYES_NO, this);
		if (wxYES == res) {
			code->Revert();
		}
	}
}

void mvceditor::MainFrameClass::OnEditCut(wxCommandEvent& event) {

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

void mvceditor::MainFrameClass::OnEditCopy(wxCommandEvent& event) {

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

void mvceditor::MainFrameClass::OnEditPaste(wxCommandEvent& event) {

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

void mvceditor::MainFrameClass::OnEditSelectAll(wxCommandEvent& event) {

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

void mvceditor::MainFrameClass::OnEditPreferences(wxCommandEvent& event) {
	App.StopConfigModifiedCheck();

	PreferencesDialogClass prefDialog(this, Preferences);
	for (size_t i = 0; i < Features.size(); ++i) {
		Features[i]->AddPreferenceWindow(prefDialog.GetBookCtrl());
	}
	prefDialog.Prepare();
	int exitCode = prefDialog.ShowModal();
	if (wxOK == exitCode) {
		wxCommandEvent evt(mvceditor::EVENT_APP_PREFERENCES_SAVED);
		App.EventSink.Publish(evt);		
	}

	// signal that this app has modified the config file, that way the external
	// modification check fails and the user will not be prompted to reload the config
	App.UpdateConfigModifiedTime();
}

void mvceditor::MainFrameClass::PreferencesSaved() {
	Preferences.EnableSelectedProfile(this);
	Notebook->RefreshCodeControlOptions();
}

void mvceditor::MainFrameClass::PreferencesExternallyUpdated() {
	Preferences.EnableSelectedProfile(this);
	Notebook->RefreshCodeControlOptions();
}

void mvceditor::MainFrameClass::OnEditContentAssist(wxCommandEvent& event) {
	CodeControlClass* page = Notebook->GetCurrentCodeControl();
	if (page) {
		page->HandleAutoCompletion();
	}
}

void mvceditor::MainFrameClass::OnEditCallTip(wxCommandEvent& event) {
	CodeControlClass* page = Notebook->GetCurrentCodeControl();
	if (page) {
		page->HandleCallTip(0, true);
	}
}

void mvceditor::MainFrameClass::EnableSave(wxStyledTextEvent& event) {
	ToolBar->EnableTool(ID_TOOLBAR_SAVE, true);
	ToolBar->Refresh();
	
	// if I don't call this then the stretch sizer on the toolbar disappears
	// before was using ToolBar->AddStretchSpacer(1); which needed the Layout call
	//Layout();  commenting this out because it is causing find/find in files replace all matches in file to not work
	MenuBar->Enable(wxID_SAVEAS, true);
	MenuBar->Enable(ID_FILE_REVERT, true);
	Notebook->MarkPageAsModified(event.GetId());
}

void mvceditor::MainFrameClass::DisableSave(wxStyledTextEvent& event) {
	ToolBar->EnableTool(ID_TOOLBAR_SAVE, false);
	ToolBar->Refresh();
	
	// if I don't call this then the stretch sizer on the toolbar disappears
	// before was using ToolBar->AddStretchSpacer(1); which needed the Layout call
	//Layout(); commenting this out because it is causing find/find in files replace all matches in file to not work
	MenuBar->Enable(wxID_SAVEAS, false);
	MenuBar->Enable(ID_FILE_REVERT, false);
	Notebook->MarkPageAsNotModified(event.GetId());
}

void mvceditor::MainFrameClass::OnContentNotebookPageChanged(wxAuiNotebookEvent& event) {
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

void mvceditor::MainFrameClass::OnHelpAbout(wxCommandEvent& event) {
	wxAboutDialogInfo info;
	info.AddDeveloper(wxT("Roberto Perpuly"));
	info.SetCopyright(wxT("(c)2009-2011 Roberto Perpuly"));
	info.SetDescription(wxT("MVC Editor is an Integrated Development Environment for PHP Web Applications"));
	info.SetName(wxT("MVC Editor"));
	info.SetVersion(wxString::Format(wxT("%s.%s.%s"), wxT(APP_MAJOR_VERSION), wxT(APP_MINOR_VERSION), wxT(APP_BUILD_NUMBER)));
	wxAboutBox(info);
}

void mvceditor::MainFrameClass::SaveCurrentFile(wxCommandEvent& event) {
	Notebook->SaveCurrentPage();
}

void mvceditor::MainFrameClass::OnUpdateUi(wxUpdateUIEvent& event) {
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

void mvceditor::MainFrameClass::CreateToolBarButtons() {
	ToolBar->AddTool(ID_TOOLBAR_SAVE, _("Save"), wxArtProvider::GetBitmap(
		wxART_FILE_SAVE, wxART_TOOLBAR, wxSize(16, 16)), _("Save"));
	ToolBar->EnableTool(ID_TOOLBAR_SAVE, false);
	ToolBar->Realize();
}

void mvceditor::MainFrameClass::AuiManagerUpdate() {
	ToolBar->Realize();
	AuiManager.Update();
}

void mvceditor::MainFrameClass::LoadFeature(mvceditor::FeatureClass* feature) {
	
	// propagate GUI events to features, so that they can handle menu events themselves
	// feature menus
	feature->InitWindow(GetStatusBarWithGauge(), Notebook, ToolsNotebook, OutlineNotebook, &AuiManager, GetMenuBar());
	
	//  when adding the separators, we dont want a separator at the very end
	// we dont need separators if the feature did not add any menu items
	size_t oldFileMenuCount = FileMenu->GetMenuItemCount();
	feature->AddFileMenuItems(FileMenu);
	if (oldFileMenuCount != FileMenu->GetMenuItemCount() && oldFileMenuCount > 0) {
		FileMenu->InsertSeparator(oldFileMenuCount);
	}

	size_t oldEditMenuCount = EditMenu->GetMenuItemCount();
	feature->AddEditMenuItems(EditMenu);
	if (oldEditMenuCount != EditMenu->GetMenuItemCount() && oldEditMenuCount > 0) {
		EditMenu->InsertSeparator(oldEditMenuCount);
	}

	size_t oldViewMenuCount = ViewMenu->GetMenuItemCount();
	feature->AddViewMenuItems(ViewMenu);
	if (oldViewMenuCount != ViewMenu->GetMenuItemCount() && oldViewMenuCount > 0) {
		ViewMenu->InsertSeparator(oldViewMenuCount);
	}

	size_t oldSearchMenuCount = SearchMenu->GetMenuItemCount();
	feature->AddSearchMenuItems(SearchMenu);
	if (oldSearchMenuCount != SearchMenu->GetMenuItemCount() && oldSearchMenuCount > 0) {
		SearchMenu->InsertSeparator(oldSearchMenuCount);
	}

	wxMenuBar* menuBar = GetMenuBar();
	feature->AddNewMenu(menuBar);

	// new menus may have been added; push the Help menu all the way to the end
	int helpIndex = menuBar->FindMenu(_("&Help"));
	if (helpIndex != wxNOT_FOUND) {
		menuBar->Remove(helpIndex);
		menuBar->Insert(menuBar->GetMenuCount(), HelpMenu, _("&Help"));
	}
	
	// move preferences menu to the end, similar to most other programs
	wxMenuItem* preferencesMenu = EditMenu->Remove(ID_EDIT_PREFERENCES);
	EditMenu->Append(preferencesMenu);

	// move exit menu to the end, similar to most other programs
	wxMenuItem* exitMenu = FileMenu->Remove(wxID_EXIT);
	FileMenu->Append(exitMenu);

	feature->AddToolBarItems(ToolBar);
	feature->AddWindows();
}

void mvceditor::MainFrameClass::OnContextMenu(wxContextMenuEvent& event) {
	CodeControlClass* codeWindow = Notebook->GetCurrentCodeControl();
	
	// only show the user if and only if
	// user clicked inside of the code control
	if (codeWindow != NULL && event.GetEventObject() == codeWindow) {

		// make sure to turn off any call tips; they hide the popup menu
		codeWindow->SetAsHidden(true);
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
		for (size_t i = 0; i < Features.size(); ++i) {
			Features[i]->AddCodeControlClassContextMenuItems(&contextMenu);
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

		codeWindow->SetAsHidden(false);
	}
	else {
		
		// if a sub panel has its own handler, use it
		event.Skip();
	}
}

void mvceditor::MainFrameClass::OnUndo(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->Undo();
	}
}

void mvceditor::MainFrameClass::OnRedo(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->Redo();
	}
}

void mvceditor::MainFrameClass::OnCutLine(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_LINECUT);
	}
}

void mvceditor::MainFrameClass::OnDuplicateLine(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_LINEDUPLICATE);
	}
}

void mvceditor::MainFrameClass::OnDeleteLine(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_LINEDELETE);
	}
}

void mvceditor::MainFrameClass::OnTransposeLine(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_LINETRANSPOSE);
	}
}

void mvceditor::MainFrameClass::OnCopyLine(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_LINECOPY);
	}
}

void mvceditor::MainFrameClass::OnLowecase(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_LOWERCASE);
	}
}

void mvceditor::MainFrameClass::OnUppercase(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_UPPERCASE);
	}
}

void mvceditor::MainFrameClass::OnCodeControlUpdate(wxStyledTextEvent& event) {
	UpdateStatusBar();
}

void mvceditor::MainFrameClass::UpdateStatusBar() {
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
	else {
		GetStatusBarWithGauge()->SetColumn1Text(wxEmptyString);
	}
}

void mvceditor::MainFrameClass::DefaultKeyboardShortcuts() {

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
	defaultMenus[ID_ABOUT] = wxT("Help-About");
	wxMenuBar* menuBar = GetMenuBar();
	for (std::map<int, wxString>::iterator it = defaultMenus.begin(); it != defaultMenus.end(); ++it) {
		wxMenuItem* item = menuBar->FindItem(it->first);
		wxASSERT_MSG(item, wxT("Menu item not found:") + it->second);
		mvceditor::DynamicCmdClass cmd(item, it->second);
		Preferences.DefaultKeyboardShortcutCmds.push_back(cmd);
	}
}

void mvceditor::MainFrameClass::OnAnyMenuCommandEvent(wxCommandEvent& event) {
	App.EventSink.Publish(event);
}

void mvceditor::MainFrameClass::OnAnyAuiNotebookEvent(wxAuiNotebookEvent& event) {
	if (event.GetEventType() == wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED) {
		if (mvceditor::ID_CODE_NOTEBOOK == event.GetId()) {
			
		}
		else if (mvceditor::ID_TOOLS_NOTEBOOK == event.GetId()) {
			size_t count = ToolsNotebook->GetPageCount();

			// this event is received AFTER the page is removed
			if (count <= 0) {
				AuiManager.GetPane(ToolsNotebook).Hide();
				AuiManager.Update();
			}
		}
		else if (mvceditor::ID_OUTLINE_NOTEBOOK == event.GetId()) {
			size_t count = OutlineNotebook->GetPageCount();

			// this event is received AFTER the page is removed
			if (count <= 0) {
				AuiManager.GetPane(OutlineNotebook).Hide();
				AuiManager.Update();
			}
		}
	}
	App.EventSink.Publish(event);
	event.Skip();
}

void mvceditor::MainFrameClass::OnAnyAuiToolbarEvent(wxAuiToolBarEvent& event) {
	App.EventSink.Publish(event);
	event.Skip();
}

void mvceditor::MainFrameClass::UpdateTitleBar() {
	if (Notebook->GetPageCount() > 0) {
		mvceditor::CodeControlClass* codeControl = Notebook->GetCurrentCodeControl();
		if (codeControl) {
			wxString fileName = codeControl->GetFileName();
			if (fileName.IsEmpty()) {
				
				// file name empty means this is a new file, use the tab text
				fileName = Notebook->GetPageText(Notebook->GetPageIndex(codeControl));
			}
			SetTitle(_("MVC Editor: ") + fileName);
		}
	}
	else {
		SetTitle(_("MVC Editor"));
	}
}

void mvceditor::MainFrameClass::OnSequenceStart(wxCommandEvent& event) {
	mvceditor::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
	wxString title = App.Sequences.GetStatus();
	if (!gauge->HasGauge(ID_SEQUENCE_GAUGE)) {
		gauge->AddGauge(title, ID_SEQUENCE_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE, wxGA_HORIZONTAL);
	}
	else {
		gauge->IncrementAndRenameGauge(ID_SEQUENCE_GAUGE, title, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE);
	}
}

void mvceditor::MainFrameClass::OnSequenceInProgress(wxCommandEvent& event) {
	mvceditor::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
	wxString title = App.Sequences.GetStatus();
	gauge->IncrementAndRenameGauge(ID_SEQUENCE_GAUGE, title, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE);
}

void mvceditor::MainFrameClass::OnSequenceComplete(wxCommandEvent& event) {
	mvceditor::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
	gauge->StopGauge(ID_SEQUENCE_GAUGE);
}

mvceditor::AppEventListenerForFrameClass::AppEventListenerForFrameClass(mvceditor::MainFrameClass* mainFrame)
	: wxEvtHandler()
	, MainFrame(mainFrame) {

}

void mvceditor::AppEventListenerForFrameClass::OnCmdFileOpen(wxCommandEvent& event) {
	std::vector<wxString> filenames;
	filenames.push_back(event.GetString());
	MainFrame->FileOpen(filenames);
}

void mvceditor::AppEventListenerForFrameClass::OnPreferencesSaved(wxCommandEvent& event) {
	MainFrame->PreferencesSaved();
}

void mvceditor::AppEventListenerForFrameClass::OnPreferencesExternallyUpdated(wxCommandEvent& event) {
	MainFrame->PreferencesExternallyUpdated();
}

void mvceditor::AppEventListenerForFrameClass::OnAppReady(wxCommandEvent& event) {
	MainFrame->PreferencesExternallyUpdated();
}

void mvceditor::AppEventListenerForFrameClass::OnCodeNotebookPageChanged(wxAuiNotebookEvent& event) {
	MainFrame->UpdateTitleBar();
}

void mvceditor::AppEventListenerForFrameClass::OnAppFileCreated(wxCommandEvent& event) {
	MainFrame->UpdateTitleBar();
}

void mvceditor::AppEventListenerForFrameClass::OnCodeNotebookPageClosed(wxAuiNotebookEvent& event) {
	MainFrame->UpdateTitleBar();

	// in case all notebook tabs have been closed, we need to refresh the cursor position
	MainFrame->UpdateStatusBar();
}


BEGIN_EVENT_TABLE(mvceditor::MainFrameClass,  MainFrameGeneratedClass)
	EVT_STC_SAVEPOINTREACHED(wxID_ANY, mvceditor::MainFrameClass::DisableSave)
	EVT_STC_SAVEPOINTLEFT(wxID_ANY, mvceditor::MainFrameClass::EnableSave)
	EVT_STC_UPDATEUI(wxID_ANY, mvceditor::MainFrameClass::OnCodeControlUpdate)
	EVT_UPDATE_UI(wxID_ANY, mvceditor::MainFrameClass::OnUpdateUi)
	EVT_CONTEXT_MENU(mvceditor::MainFrameClass::OnContextMenu)

	// these are context menu handlers; the menu handlers are already accounted for
	// in the MainFrameGeneratedClass
	EVT_MENU(wxID_UNDO, mvceditor::MainFrameClass::OnUndo)
	EVT_MENU(wxID_REDO, mvceditor::MainFrameClass::OnRedo)
	EVT_MENU(ID_CUT_LINE, mvceditor::MainFrameClass::OnCutLine)
	EVT_MENU(ID_DUPLICATE_LINE, mvceditor::MainFrameClass::OnDuplicateLine)
	EVT_MENU(ID_DELETE_LINE, mvceditor::MainFrameClass::OnDeleteLine)
	EVT_MENU(ID_TRANSPOSE_LINE, mvceditor::MainFrameClass::OnTransposeLine)
	EVT_MENU(ID_COPY_LINE, mvceditor::MainFrameClass::OnCopyLine)
	EVT_MENU(ID_LOWERCASE, mvceditor::MainFrameClass::OnLowecase)
	EVT_MENU(ID_UPPERCASE, mvceditor::MainFrameClass::OnUppercase)
	EVT_MENU(ID_TOOLBAR_SAVE, mvceditor::MainFrameClass::SaveCurrentFile)

	// ATTN: STOP! DO NOT HANDLE ANY APP EVENTS HERE! SEE AppEventListenerForFrameClass

	// we want to propagate these events to the features, we will do so here
	// at first, wxWindow::PushEventHandler was used (each feature was added to the event chain of
	// the app frame), but that function could no longer be used when the event sink was 
	// introduced; since for any feature that did not handle the event the app frame would get the event
	// (because the feature had the frame in its handler chain)
	// for now we will only restrict to the feature menus, since MainFrameGeneratedClass already handles
	// some events I could not just do EVT_MENU(wxID_ANY,...); putting this line would mean
	// that OnAnyMenuCommandEvent would need to handle all of the MainFrame menus (and adding a new menu item
	// to the app frame menus would involve modifying OnAnyMenuCommandEvent AND MainFrameGeneratedClass
	EVT_MENU_RANGE(mvceditor::MENU_START, mvceditor::MENU_END, mvceditor::MainFrameClass::OnAnyMenuCommandEvent)
	EVT_TOOL_RANGE(mvceditor::MENU_START, mvceditor::MENU_END, mvceditor::MainFrameClass::OnAnyMenuCommandEvent)

	// make sure notebook events are propagated to the app event sink
	EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, mvceditor::MainFrameClass::OnAnyAuiNotebookEvent)
	EVT_AUINOTEBOOK_PAGE_CHANGING(wxID_ANY, mvceditor::MainFrameClass::OnAnyAuiNotebookEvent)
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, mvceditor::MainFrameClass::OnAnyAuiNotebookEvent)
	EVT_AUINOTEBOOK_PAGE_CLOSED(wxID_ANY, mvceditor::MainFrameClass::OnAnyAuiNotebookEvent) 
	
	// make sure the tool bar events are propagated to the app event sink
	EVT_AUITOOLBAR_BEGIN_DRAG(wxID_ANY, mvceditor::MainFrameClass::OnAnyAuiToolbarEvent)
	EVT_AUITOOLBAR_MIDDLE_CLICK(wxID_ANY, mvceditor::MainFrameClass::OnAnyAuiToolbarEvent)
	EVT_AUITOOLBAR_OVERFLOW_CLICK(wxID_ANY, mvceditor::MainFrameClass::OnAnyAuiToolbarEvent)
	EVT_AUITOOLBAR_RIGHT_CLICK(wxID_ANY, mvceditor::MainFrameClass::OnAnyAuiToolbarEvent)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(wxID_ANY, mvceditor::MainFrameClass::OnAnyAuiToolbarEvent)

	// make sure to show status of running sequences
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_SEQUENCE_START, mvceditor::MainFrameClass::OnSequenceStart)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_SEQUENCE_IN_PROGRESS, mvceditor::MainFrameClass::OnSequenceInProgress)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_SEQUENCE_COMPLETE, mvceditor::MainFrameClass::OnSequenceComplete)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::AppEventListenerForFrameClass, wxEvtHandler)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_CMD_FILE_OPEN, mvceditor::AppEventListenerForFrameClass::OnCmdFileOpen)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_FILE_CREATED, mvceditor::AppEventListenerForFrameClass::OnAppFileCreated)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_PREFERENCES_SAVED, mvceditor::AppEventListenerForFrameClass::OnPreferencesSaved)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_PREFERENCES_EXTERNALLY_UPDATED, mvceditor::AppEventListenerForFrameClass::OnPreferencesExternallyUpdated)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_READY, mvceditor::AppEventListenerForFrameClass::OnAppReady)
	EVT_AUINOTEBOOK_PAGE_CHANGED(mvceditor::ID_CODE_NOTEBOOK, mvceditor::AppEventListenerForFrameClass::OnCodeNotebookPageChanged)
	EVT_AUINOTEBOOK_PAGE_CLOSED(mvceditor::ID_CODE_NOTEBOOK, mvceditor::AppEventListenerForFrameClass::OnCodeNotebookPageClosed)
END_EVENT_TABLE()