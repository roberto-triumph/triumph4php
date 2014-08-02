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
#include <features/FeatureClass.h>
#include <Triumph.h>
#include <main_frame/PreferencesDialogClass.h>
#include <widgets/StatusBarWithGaugeClass.h>
#include <globals/Assets.h>
#include <wx/artprov.h>
#include <wx/choicdlg.h>
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/aboutdlg.h>
#include <vector>

static int ID_TOOLBAR_SAVE = wxNewId();
static int ID_TOOLBAR = wxNewId();
static int ID_SEQUENCE_GAUGE = wxNewId();
static int ID_STATUS_BAR_TIMER = wxNewId();

t4p::MainFrameClass::MainFrameClass(const std::vector<t4p::FeatureClass*>& features,
										t4p::AppClass& app,
										t4p::PreferencesClass& preferences)
	: MainFrameGeneratedClass(NULL)
	, AuiManager()
	, StatusBarTimer(this, ID_STATUS_BAR_TIMER)
	, Features(features)
	, Listener(this)
	, App(app)
	, Preferences(preferences)
	, ToolBar(NULL)
	, ToolsNotebook(NULL)
	, OutlineNotebook(NULL) {
	AuiManager.SetManagedWindow(this);
	StatusBarWithGaugeClass* gauge = new StatusBarWithGaugeClass(this);
	SetStatusBar(gauge);

	App.EventSink.PushHandler(&Listener);
	App.RunningThreads.AddEventHandler(this);
	App.SqliteRunningThreads.AddEventHandler(this);
	ToolBar = new wxAuiToolBar(this, ID_TOOLBAR, wxDefaultPosition, wxDefaultSize, 
		  wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_TEXT | wxAUI_TB_HORZ_TEXT);
	
	// when the notebook is empty we want to accept dragged files
	Notebook->SetDropTarget(new FileDropTargetClass(Notebook));
	Notebook->CodeControlOptions = &Preferences.CodeControlOptions;
	Notebook->Globals = &App.Globals;
	Notebook->EventSink = &App.EventSink;
	
	ToolsNotebook = new wxAuiNotebook(this, t4p::ID_TOOLS_NOTEBOOK, wxDefaultPosition, wxDefaultSize, 
		wxAUI_NB_TOP | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB | wxAUI_NB_TAB_MOVE);
	OutlineNotebook = new wxAuiNotebook(this, t4p::ID_OUTLINE_NOTEBOOK, wxDefaultPosition, wxDefaultSize, 
		wxAUI_NB_TOP | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB | wxAUI_NB_TAB_MOVE);
		
	SetApplicationFont();
	CreateToolBarButtons();
	
	// setup the bottom "tools" pane, the main content pane, and the toolbar on top
	AuiManager.AddPane(Notebook, wxAuiPaneInfo().Name(wxT("content")).CentrePane(
		).PaneBorder(true).Gripper(false).Floatable(false).Resizable(true));
	AuiManager.AddPane(ToolsNotebook, wxAuiPaneInfo().Name(wxT("tools")).Bottom().Caption(
		_("Tools")).Floatable(false).MinSize(-1, 260).Hide().Layer(1));
	AuiManager.AddPane(OutlineNotebook, wxAuiPaneInfo().Name(wxT("outline")).Left().Caption(
        _("Outlines")).Floatable(false).MinSize(260, -1).Hide());
	
	DefaultKeyboardShortcuts();
}

t4p::MainFrameClass::~MainFrameClass() {
	AuiManager.UnInit();
}

t4p::StatusBarWithGaugeClass* t4p::MainFrameClass::GetStatusBarWithGauge() {
	return (StatusBarWithGaugeClass*)GetStatusBar();
}

void t4p::MainFrameClass::OnClose(wxCloseEvent& event) {
	bool destroy = true;
	if (event.CanVeto()) {
		
		// if we have any files that are saved using privilege mode
		// lets tell the user to save them first.  we don't
		// want to close the app while the async save process
		// is running
		for (size_t i = 0; i < Notebook->GetPageCount(); i++) {
			t4p::CodeControlClass* ctrl = Notebook->GetCodeControl(i);
			wxString filename = ctrl->GetFileName();
			if (!ctrl->IsNew() && ctrl->IsModified() 
				&& !filename.empty() 
				&& wxFileName::FileExists(filename)
				&& !wxFileName::IsFileWritable(filename)) {
				wxMessageBox(
					_("You have modified files that need to be saved with escalated privileges.\n") +
					_("Please save those files or discard the changes before exiting"), 
					_("Triumph")
				);
				destroy = false;
				break;
			}
		}
		if (destroy) {
			destroy = Notebook->SaveAllModifiedPages();
		}
	}
	if (destroy) {

		// delete the DropTarget that was created in the constructor
		Notebook->SetDropTarget(NULL);

		wxCommandEvent exitEvent(t4p::EVENT_APP_EXIT);
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
		App.SqliteRunningThreads.RemoveEventHandler(this);
		App.RunningThreads.Shutdown();
		App.SqliteRunningThreads.Shutdown();
		
		// cleanup all open code controls and tabs. this is because
		// we want to destroy those items because they may have
		// threads that are running
		Notebook->CloseAllPages();

		// delete the features first so that we can destroy
		// the windows without worrying if the features
		// may access them.
		// only delete features until after we close the notebook code pages,
		// so that the features can get the file closed events and perform
		// any cleanup
		App.DeleteFeatures();

		while (ToolsNotebook->GetPageCount() > 0) {
			ToolsNotebook->DeletePage(0);
		}
		while (OutlineNotebook->GetPageCount() > 0) {
			OutlineNotebook->DeletePage(0);
		}
		event.Skip();
	}
}

void t4p::MainFrameClass::OnFileSave(wxCommandEvent& event) {
	Notebook->SaveCurrentPage();
}

void t4p::MainFrameClass::OnFilePhpNew(wxCommandEvent& event) {
	Notebook->AddTriumphPage(t4p::CodeControlClass::PHP);
}

void t4p::MainFrameClass::OnFileSqlNew(wxCommandEvent& event) {
	Notebook->AddTriumphPage(t4p::CodeControlClass::SQL);
}

void t4p::MainFrameClass::OnFileCssNew(wxCommandEvent& event) {
	Notebook->AddTriumphPage(t4p::CodeControlClass::CSS);
}

void t4p::MainFrameClass::OnFileTextNew(wxCommandEvent& event) {
	Notebook->AddTriumphPage(t4p::CodeControlClass::TEXT);
}

void t4p::MainFrameClass::OnFileOpen(wxCommandEvent& event) {
	Notebook->LoadPage();
}

void t4p::MainFrameClass::OnFileSaveAs(wxCommandEvent& event) {
	Notebook->SaveCurrentPageAsNew();
}

void t4p::MainFrameClass::OnFileSaveAll(wxCommandEvent& event) {
	Notebook->SaveAllModifiedPagesWithoutPrompting();
}

void t4p::MainFrameClass::FileOpen(const std::vector<wxString>& filenames) {
	Notebook->LoadPages(filenames);
}
void t4p::MainFrameClass::FileOpenPosition(const wxString& fullPath, int startingPos, int length) {
	Notebook->LoadPage(fullPath);
	CodeControlClass* codeControl = Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->SetSelectionAndEnsureVisible(startingPos, startingPos + length);
	}
}

void t4p::MainFrameClass::FileOpenLine(const wxString& fullPath, int lineNumber) {
	Notebook->LoadPage(fullPath);
	CodeControlClass* codeControl = Notebook->GetCurrentCodeControl();
	if (codeControl) {
		
		// scintilla line numbers are 1 based, ours are 1 based coz they come
		// from the user
		codeControl->GotoLine(lineNumber - 1);
	}
}
	
void t4p::MainFrameClass::OnFileClose(wxCommandEvent& event) {
	Notebook->CloseCurrentPage();
}

void t4p::MainFrameClass::OnFileExit(wxCommandEvent& event) {
	Close();
}

void t4p::MainFrameClass::OnFileRevert(wxCommandEvent& event) {
	CodeControlClass* code = Notebook->GetCurrentCodeControl();
	if (NULL != code && !code->IsNew()) {
		int res = wxMessageBox(_("Reload file and lose all changes?"), _("Triumph"), wxICON_QUESTION | wxYES_NO, this);
		if (wxYES == res) {
			code->Revert();

			t4p::CodeControlEventClass revertEvt(t4p::EVENT_APP_FILE_REVERTED, code);
			App.EventSink.Publish(revertEvt);
		}
	}
}

void t4p::MainFrameClass::OnEditCut(wxCommandEvent& event) {

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

void t4p::MainFrameClass::OnEditCopy(wxCommandEvent& event) {

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

void t4p::MainFrameClass::OnEditPaste(wxCommandEvent& event) {

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

void t4p::MainFrameClass::OnEditSelectAll(wxCommandEvent& event) {

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

void t4p::MainFrameClass::OnEditPreferences(wxCommandEvent& event) {

	// make sure that no existing project index or wipe action is running
	// as we will re-trigger an index if the user makes any modifications to
	// the project sources
	if (App.Sequences.Running()) {
		wxString msg = wxString::FromAscii(
			"There is an existing background task running. Since the changes "
			"made from this dialog may re-trigger a project index sequence, "
			"you may not make modifications until the existing background task ends.\n"
			"Would you like to stop the current background tasks? If you answer no, the "
			"preferences dialog will not be opened."
		);
		msg = wxGetTranslation(msg);
		int ret = wxMessageBox(msg, _("Warning"), wxICON_WARNING | wxYES_NO, this);
		if (wxYES != ret) {
			return;
		}

		// user said yes, we should stop the running tasks
		App.Sequences.Stop();
		App.RunningThreads.StopAll();
		App.SqliteRunningThreads.StopAll();
	}

	App.StopConfigModifiedCheck();

	wxFileName settingsDir = t4p::SettingsDirAsset();
	bool changedSettingsDir = false;
	bool needsRetag = false;
	PreferencesDialogClass prefDialog(this, App.Globals, Preferences, settingsDir, changedSettingsDir, needsRetag);
	
	for (size_t i = 0; i < Features.size(); ++i) {
		Features[i]->AddPreferenceWindow(prefDialog.GetBookCtrl());
	}
	prefDialog.Prepare();
	int exitCode = prefDialog.ShowModal();
	if (wxOK == exitCode) {
		App.SavePreferences(settingsDir, changedSettingsDir);

		// since preferences setting can affect php and url detection
		// if the user changed them we must re-detect all tags
		// for example if the user changed PHP version new classes may become
		// parseable; new urls may come from a new virtual host that was just
		// entered
		// we guard with a boolean because we dont want to retag when the 
		// user changes a setting that does not affect tags, like changing the
		// background color
		if (needsRetag || changedSettingsDir) {
			wxString msg = wxString::FromAscii(
				"You have made a change that affects PHP resource tagging. Would "
				"you like to re-tag your enabled projects at this time?"
			);
			msg = wxGetTranslation(msg);
			int ret = wxMessageBox(msg, _("Tag Projects"), wxICON_QUESTION | wxYES_NO, this);
			if (wxYES == ret) {
				std::vector<t4p::ProjectClass> emptyProjects;
				App.Sequences.ProjectDefinitionsUpdated(App.Globals.AllEnabledProjects(), emptyProjects);
			}
		}
	}
}

void t4p::MainFrameClass::OnViewToggleOutline(wxCommandEvent& event) {
	wxAuiPaneInfo& info = AuiManager.GetPane(OutlineNotebook);
	info.Show(!info.IsShown());
	AuiManager.Update();
	OutlineNotebook->SetFocus();
}

void t4p::MainFrameClass::OnViewToggleTools(wxCommandEvent& event) {
	wxAuiPaneInfo& info = AuiManager.GetPane(ToolsNotebook);
	info.Show(!info.IsShown());
	AuiManager.Update();
	ToolsNotebook->SetFocus();
}

void t4p::MainFrameClass::PreferencesSaved() {
	Preferences.EnableSelectedProfile(this);
	Notebook->RefreshCodeControlOptions();
}

void t4p::MainFrameClass::SetApplicationFont() {
	
	// ATTN: this method will only work on startup (before the
	// main frame is drawn. have not found a way to have the font
	// changes apply to all windows /panels that are already
	// drawn on the screen

	// so that all dialogs / panels use the same font
	// ATTN: on linux, default fonts are too big
	//       this code makes them smaller
	wxPlatformInfo info;
	if (info.GetOperatingSystemId() != wxOS_WINDOWS_NT) {
		SetFont(Preferences.ApplicationFont);
		
		// so that the tabs use the same font
		Notebook->SetFont(Preferences.ApplicationFont);
		Notebook->SetNormalFont(Preferences.ApplicationFont);
		ToolsNotebook->SetFont(Preferences.ApplicationFont);
		ToolsNotebook->SetNormalFont(Preferences.ApplicationFont);
		OutlineNotebook->SetFont(Preferences.ApplicationFont);
		OutlineNotebook->SetNormalFont(Preferences.ApplicationFont);
		
		// so that the toolbar buttons use the same font
		ToolBar->SetFont(Preferences.ApplicationFont);
	}
}

void t4p::MainFrameClass::PreferencesExternallyUpdated() {
	PreferencesSaved();
}

void t4p::MainFrameClass::OnEditContentAssist(wxCommandEvent& event) {
	CodeControlClass* page = Notebook->GetCurrentCodeControl();
	if (page) {
		page->HandleAutoCompletion();
	}
}

void t4p::MainFrameClass::OnEditCallTip(wxCommandEvent& event) {
	CodeControlClass* page = Notebook->GetCurrentCodeControl();
	if (page) {
		page->HandleCallTip(0, true);
	}
}

void t4p::MainFrameClass::EnableSave(wxStyledTextEvent& event) {
	ToolBar->EnableTool(ID_TOOLBAR_SAVE, true);
	ToolBar->Refresh();
	
	// if I don't call this then the stretch sizer on the toolbar disappears
	// before was using ToolBar->AddStretchSpacer(1); which needed the Layout call
	//Layout();  commenting this out because it is causing find/find in files replace all matches in file to not work
	MenuBar->Enable(wxID_SAVEAS, true);
	MenuBar->Enable(ID_FILE_REVERT, true);

	// if at least one document needs to be saved, enable the save all menu
	bool someModified = false;
	for (size_t i = 0; i < Notebook->GetPageCount(); ++i) {
		t4p::CodeControlClass* ctrl = Notebook->GetCodeControl(i);
		if (ctrl) {
			someModified |= ctrl->GetModify();
		}
	}
	MenuBar->Enable(ID_FILE_SAVE_ALL, someModified);
	Notebook->MarkPageAsModified(event.GetId());
}

void t4p::MainFrameClass::DisableSave(wxStyledTextEvent& event) {
	ToolBar->EnableTool(ID_TOOLBAR_SAVE, false);
	ToolBar->Refresh();
	
	// if I don't call this then the stretch sizer on the toolbar disappears
	// before was using ToolBar->AddStretchSpacer(1); which needed the Layout call
	//Layout(); commenting this out because it is causing find/find in files replace all matches in file to not work
	MenuBar->Enable(wxID_SAVEAS, false);
	MenuBar->Enable(ID_FILE_REVERT, false);

	bool someModified = false;
	for (size_t i = 0; i < Notebook->GetPageCount(); ++i) {
		t4p::CodeControlClass* ctrl = Notebook->GetCodeControl(i);
		if (ctrl) {
			someModified |= ctrl->GetModify();
		}
	}
	MenuBar->Enable(ID_FILE_SAVE_ALL, someModified);
	Notebook->MarkPageAsNotModified(event.GetId());
}

void t4p::MainFrameClass::OnContentNotebookPageChanged(wxAuiNotebookEvent& event) {
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

void t4p::MainFrameClass::OnHelpAbout(wxCommandEvent& event) {
	wxAboutDialogInfo info;
	info.SetCopyright(wxT("(c)2009-2013 Roberto Perpuly"));
	info.SetDescription(wxT("Triumph is an Integrated Development Environment for PHP Web Applications"));
	info.SetName(wxT("Triumph"));
	
	// version info is stored in a file
	// for releases, the distribution script will properly fill in the
	// version number using git describe
	wxString version;
	wxFileName versionFileName = t4p::VersionFileAsset();
	wxFFile file(versionFileName.GetFullPath());
	if (file.IsOpened()) {
		file.ReadAll(&version);
	}
	info.SetVersion(version);
	wxAboutBox(info);
}
void t4p::MainFrameClass::OnHelpCredits(wxCommandEvent& event) {
	CreditsDialogClass dialog(this);
	dialog.ShowModal();
}

void t4p::MainFrameClass::OnHelpLicense(wxCommandEvent& event) {
	LicenseDialogClass dialog(this, wxID_ANY, _("License"), wxDefaultPosition);
	dialog.ShowModal();
}

void t4p::MainFrameClass::OnHelpManual(wxCommandEvent& event) {
	wxLaunchDefaultBrowser(wxT("http://docs.triumph4php.com/"), 0);
}

void t4p::MainFrameClass::SaveCurrentFile(wxCommandEvent& event) {
	Notebook->SaveCurrentPage();
}

void t4p::MainFrameClass::OnUpdateUi(wxUpdateUIEvent& event) {
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

void t4p::MainFrameClass::CreateToolBarButtons() {
	ToolBar->AddTool(ID_TOOLBAR_SAVE, _("Save"), wxArtProvider::GetBitmap(
		wxART_FILE_SAVE, wxART_TOOLBAR, wxSize(16, 16)), _("Save"));
	ToolBar->EnableTool(ID_TOOLBAR_SAVE, false);
}

void t4p::MainFrameClass::AuiManagerUpdate() {
	AuiManager.GetPane(wxT("content")).Hide();
	AuiManager.GetPane(wxT("tools")).Hide();
	AuiManager.GetPane(wxT("outline")).Hide();
	
	AuiManager.GetPane(wxT("tools")).Bottom().Layer(0).Row(0).Position(0);
	AuiManager.GetPane(wxT("outline")).Left().Layer(0).Row(0).Position(0);
	AuiManager.GetPane(wxT("content")).Show();
	Layout();
	
	AuiManager.Update();
}

void t4p::MainFrameClass::LoadFeature(t4p::FeatureClass* feature) {
	
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

	size_t oldHelpMenuCount = HelpMenu->GetMenuItemCount();
	feature->AddHelpMenuItems(HelpMenu);
	if (oldHelpMenuCount != HelpMenu->GetMenuItemCount() && oldHelpMenuCount > 0) {
		HelpMenu->InsertSeparator(oldHelpMenuCount);
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

void t4p::MainFrameClass::RealizeToolbar() {
	ToolBar->Realize();
	
	AuiManager.AddPane(ToolBar, wxAuiPaneInfo()
		.ToolbarPane().Top()
		.Row(1).Position(1)
		.LeftDockable(false).RightDockable(false)
		.Gripper(false).CaptionVisible(false).CloseButton(false).DockFixed(true)
		.PaneBorder(true).Floatable(false)
		.MinSize(860, ToolBar->GetSize().GetHeight())
	);
}

void t4p::MainFrameClass::OnContextMenu(wxContextMenuEvent& event) {
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
		
		bool isTextSelected = !codeWindow->GetSelectedText().IsEmpty();
		contextMenu.Enable(wxID_CUT, isTextSelected);
		contextMenu.Enable(wxID_COPY, isTextSelected);
		contextMenu.Enable(wxID_PASTE, codeWindow->CanPaste());
		contextMenu.Enable(wxID_UNDO, codeWindow->CanUndo());
		contextMenu.Enable(wxID_REDO, codeWindow->CanRedo());
		PopupMenu(&contextMenu);

		codeWindow->SetAsHidden(false);
	}
	else {
		
		// if a sub panel has its own handler, use it
		event.Skip();
	}
}

void t4p::MainFrameClass::OnUndo(wxCommandEvent& event) {
	t4p::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->Undo();
	}
}

void t4p::MainFrameClass::OnRedo(wxCommandEvent& event) {
	t4p::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->Redo();
	}
}

void t4p::MainFrameClass::OnCutLine(wxCommandEvent& event) {
	t4p::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_LINECUT);
	}
}

void t4p::MainFrameClass::OnDuplicateLine(wxCommandEvent& event) {
	t4p::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_LINEDUPLICATE);
	}
}

void t4p::MainFrameClass::OnDeleteLine(wxCommandEvent& event) {
	t4p::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_LINEDELETE);
	}
}

void t4p::MainFrameClass::OnTransposeLine(wxCommandEvent& event) {
	t4p::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_LINETRANSPOSE);
	}
}

void t4p::MainFrameClass::OnCopyLine(wxCommandEvent& event) {
	t4p::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_LINECOPY);
	}
}

void t4p::MainFrameClass::OnLowecase(wxCommandEvent& event) {
	t4p::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_LOWERCASE);
	}
}

void t4p::MainFrameClass::OnUppercase(wxCommandEvent& event) {
	t4p::CodeControlClass* codeControl =  Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->CmdKeyExecute(wxSTC_CMD_UPPERCASE);
	}
}

void t4p::MainFrameClass::UpdateStatusBar() {
	CodeControlClass* codeControl = Notebook->GetCurrentCodeControl();
	if (codeControl) {
		int pos = codeControl->GetCurrentPos();

		// scintilla lines and columns are zero-based 
		// we want to be 1-based (human friendly)
		// but offset we want to be 0-based
		int line = codeControl->LineFromPosition(pos) + 1; 
		int column = codeControl->GetColumn(pos) + 1;
		
		wxString s = wxString::Format(wxT("Line:%d Column:%d Offset:%d"), line, column, pos);
		GetStatusBarWithGauge()->SetColumn1Text(s);
		
	}
	else {
		GetStatusBarWithGauge()->SetColumn1Text(wxEmptyString);
	}
}

void t4p::MainFrameClass::DefaultKeyboardShortcuts() {

	// ATTN: when a new menu item is added to the form builder 
	// we will need to add an entry here so that shortcuts
	// work properly
	std::map<int, wxString>defaultMenus;
	defaultMenus[ID_FILE_PHP_NEW] = wxT("File-New PHP File");
	defaultMenus[wxID_OPEN] = wxT("File-Open");
	defaultMenus[wxID_SAVE] = wxT("File-Save");
	defaultMenus[wxID_SAVEAS] = wxT("File-Save As");
	defaultMenus[ID_FILE_SAVE_ALL] = wxT("File-Save All");
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
	defaultMenus[ID_VIEW_TOGGLE_TOOLS] = wxT("View-Tools");
	defaultMenus[ID_VIEW_TOGGLE_OUTLINE] = wxT("View-Outline");

	defaultMenus[ID_ABOUT] = wxT("Help-About");
	wxMenuBar* menuBar = GetMenuBar();
	for (std::map<int, wxString>::iterator it = defaultMenus.begin(); it != defaultMenus.end(); ++it) {
		wxMenuItem* item = menuBar->FindItem(it->first);
		wxASSERT_MSG(item, wxT("Menu item not found:") + it->second);
		t4p::DynamicCmdClass cmd(item, it->second);
		Preferences.DefaultKeyboardShortcutCmds.push_back(cmd);
	}
}

void t4p::MainFrameClass::OnAnyMenuCommandEvent(wxCommandEvent& event) {
	App.EventSink.Publish(event);
}

void t4p::MainFrameClass::OnAnyWizardEvent(wxWizardEvent& event) {
	App.EventSink.Publish(event);
}

void t4p::MainFrameClass::OnAnyAuiNotebookEvent(wxAuiNotebookEvent& event) {
	if (event.GetEventType() == wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED) {
		if (t4p::ID_CODE_NOTEBOOK == event.GetId()) {
			
		}
		else if (t4p::ID_TOOLS_NOTEBOOK == event.GetId()) {
			size_t count = ToolsNotebook->GetPageCount();

			// this event is received AFTER the page is removed
			if (count <= 0) {
				AuiManager.GetPane(ToolsNotebook).Hide();
				AuiManager.Update();
			}
		}
		else if (t4p::ID_OUTLINE_NOTEBOOK == event.GetId()) {
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

void t4p::MainFrameClass::OnAnyAuiToolbarEvent(wxAuiToolBarEvent& event) {
	App.EventSink.Publish(event);

	// ATTN: not skipping these event for now, in wxWidgets 2.9 if the tooldropdown event
	// is skipped then a crash will occur
	if (!event.IsDropDownClicked()) {
		event.Skip();
	}
}

void t4p::MainFrameClass::UpdateTitleBar() {
	if (Notebook->GetPageCount() > 0) {
		t4p::CodeControlClass* codeControl = Notebook->GetCurrentCodeControl();
		if (codeControl) {
			wxString fileName = codeControl->GetFileName();
			if (fileName.IsEmpty()) {
				
				// file name empty means this is a new file, use the tab text
				fileName = Notebook->GetPageText(Notebook->GetPageIndex(codeControl));
			}
			SetTitle(_("Triumph4PHP: ") + fileName);
		}
	}
	else {
		SetTitle(_("Triumph4PHP"));
	}
}

void t4p::MainFrameClass::OnSequenceStart(wxCommandEvent& event) {
	t4p::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
	if (!gauge->HasGauge(ID_SEQUENCE_GAUGE)) {
		gauge->AddGauge(_(""), ID_SEQUENCE_GAUGE, t4p::StatusBarWithGaugeClass::INDETERMINATE_MODE, wxGA_HORIZONTAL);
	}
}

void t4p::MainFrameClass::OnDeterminateProgress(t4p::SequenceProgressEventClass& event) {
	t4p::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
	if (event.Mode == t4p::ActionClass::INDETERMINATE) {
		gauge->SwitchMode(ID_SEQUENCE_GAUGE, t4p::StatusBarWithGaugeClass::INDETERMINATE_MODE, 0, 0);
		if (!event.Message.IsEmpty()) {
			gauge->IncrementAndRenameGauge(ID_SEQUENCE_GAUGE, event.Message, t4p::StatusBarWithGaugeClass::INDETERMINATE_MODE);
		}
		else {
			gauge->IncrementGauge(ID_SEQUENCE_GAUGE, t4p::StatusBarWithGaugeClass::INDETERMINATE_MODE);
		}
	}
	else {
		if (event.PercentComplete == 0) {
			
			// the start, turn the gauge into determinate mode
			gauge->SwitchMode(ID_SEQUENCE_GAUGE, t4p::StatusBarWithGaugeClass::DETERMINATE_MODE, 0, 100);
		}
		gauge->UpdateGauge(ID_SEQUENCE_GAUGE, event.PercentComplete);
		if (!event.Message.IsEmpty()) {
			gauge->RenameGauge(ID_SEQUENCE_GAUGE, event.Message);
		}
	}
}

void t4p::MainFrameClass::OnSequenceComplete(wxCommandEvent& event) {
	t4p::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
	gauge->StopGauge(ID_SEQUENCE_GAUGE);
}

void t4p::MainFrameClass::StartStatusBarTimer() {
	StatusBarTimer.Start(250, wxTIMER_CONTINUOUS);
}

void t4p::MainFrameClass::OnStatusBarTimer(wxTimerEvent& event) {
	
	/*
	 * Update the status bar info (line, column)
	 * at specific intervals instead of in a EVT_STC_UPDATEUI event.
	 * Updating the status bar text triggers a refresh of the whole
	 * status bar (and it seems that the entire app is refreshed too)
	 * and it makes the app feel sluggish.
	 */
	UpdateStatusBar();
}

t4p::AppEventListenerForFrameClass::AppEventListenerForFrameClass(t4p::MainFrameClass* mainFrame)
	: wxEvtHandler()
	, MainFrame(mainFrame) {

}

void t4p::AppEventListenerForFrameClass::OnCmdFileOpen(t4p::OpenFileCommandEventClass& event) {
	if (event.LineNumber > 0) {
		MainFrame->FileOpenLine(event.FullPath, event.LineNumber);
	}
	else {
		MainFrame->FileOpenPosition(event.FullPath, event.StartingPos, event.Length);
	}
}

void t4p::AppEventListenerForFrameClass::OnPreferencesSaved(wxCommandEvent& event) {
	MainFrame->PreferencesSaved();
}

void t4p::AppEventListenerForFrameClass::OnPreferencesExternallyUpdated(wxCommandEvent& event) {
	MainFrame->PreferencesExternallyUpdated();
}

void t4p::AppEventListenerForFrameClass::OnAppReady(wxCommandEvent& event) {
	MainFrame->PreferencesExternallyUpdated();
	MainFrame->StartStatusBarTimer();
}

void t4p::AppEventListenerForFrameClass::OnCodeNotebookPageChanged(wxAuiNotebookEvent& event) {
	MainFrame->UpdateTitleBar();
}

void t4p::AppEventListenerForFrameClass::OnAppFileCreated(wxCommandEvent& event) {
	MainFrame->UpdateTitleBar();
}

void t4p::AppEventListenerForFrameClass::OnCodeNotebookPageClosed(wxAuiNotebookEvent& event) {
	MainFrame->UpdateTitleBar();

	// in case all notebook tabs have been closed, we need to refresh the cursor position
	MainFrame->UpdateStatusBar();
}

void t4p::AppEventListenerForFrameClass::OnAppRequestUserAttention(wxCommandEvent& event) {
	MainFrame->RequestUserAttention();
}


BEGIN_EVENT_TABLE(t4p::MainFrameClass,  MainFrameGeneratedClass)
	EVT_STC_SAVEPOINTREACHED(wxID_ANY, t4p::MainFrameClass::DisableSave)
	EVT_STC_SAVEPOINTLEFT(wxID_ANY, t4p::MainFrameClass::EnableSave)
	EVT_UPDATE_UI(wxID_ANY, t4p::MainFrameClass::OnUpdateUi)
	EVT_CONTEXT_MENU(t4p::MainFrameClass::OnContextMenu)
	EVT_TIMER(ID_STATUS_BAR_TIMER, t4p::MainFrameClass::OnStatusBarTimer)

	// these are context menu handlers; the menu handlers are already accounted for
	// in the MainFrameGeneratedClass
	EVT_MENU(wxID_UNDO, t4p::MainFrameClass::OnUndo)
	EVT_MENU(wxID_REDO, t4p::MainFrameClass::OnRedo)
	EVT_MENU(ID_TOOLBAR_SAVE, t4p::MainFrameClass::SaveCurrentFile)

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
	EVT_MENU_RANGE(t4p::MENU_START, t4p::MENU_END, t4p::MainFrameClass::OnAnyMenuCommandEvent)
	EVT_TOOL_RANGE(t4p::MENU_START, t4p::MENU_END, t4p::MainFrameClass::OnAnyMenuCommandEvent)

	// make sure notebook events are propagated to the app event sink
	EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, t4p::MainFrameClass::OnAnyAuiNotebookEvent)
	EVT_AUINOTEBOOK_PAGE_CHANGING(wxID_ANY, t4p::MainFrameClass::OnAnyAuiNotebookEvent)
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, t4p::MainFrameClass::OnAnyAuiNotebookEvent)
	EVT_AUINOTEBOOK_PAGE_CLOSED(wxID_ANY, t4p::MainFrameClass::OnAnyAuiNotebookEvent) 

	// make sure the tool bar events are propagated to the app event sink
	EVT_AUITOOLBAR_BEGIN_DRAG(wxID_ANY, t4p::MainFrameClass::OnAnyAuiToolbarEvent)
	EVT_AUITOOLBAR_MIDDLE_CLICK(wxID_ANY, t4p::MainFrameClass::OnAnyAuiToolbarEvent)
	EVT_AUITOOLBAR_OVERFLOW_CLICK(wxID_ANY, t4p::MainFrameClass::OnAnyAuiToolbarEvent)
	EVT_AUITOOLBAR_RIGHT_CLICK(wxID_ANY, t4p::MainFrameClass::OnAnyAuiToolbarEvent)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(wxID_ANY, t4p::MainFrameClass::OnAnyAuiToolbarEvent)

	// make sure wizard events are propagated
	EVT_WIZARD_BEFORE_PAGE_CHANGED(wxID_ANY, t4p::MainFrameClass::OnAnyWizardEvent)
	EVT_WIZARD_CANCEL(wxID_ANY, t4p::MainFrameClass::OnAnyWizardEvent)
	EVT_WIZARD_FINISHED(wxID_ANY, t4p::MainFrameClass::OnAnyWizardEvent)
	EVT_WIZARD_HELP(wxID_ANY, t4p::MainFrameClass::OnAnyWizardEvent)
	EVT_WIZARD_PAGE_CHANGED(wxID_ANY, t4p::MainFrameClass::OnAnyWizardEvent)
	EVT_WIZARD_PAGE_CHANGING(wxID_ANY, t4p::MainFrameClass::OnAnyWizardEvent)
	EVT_WIZARD_PAGE_SHOWN(wxID_ANY, t4p::MainFrameClass::OnAnyWizardEvent)


	// make sure to show status of running sequences
	EVT_COMMAND(wxID_ANY, t4p::EVENT_SEQUENCE_START, t4p::MainFrameClass::OnSequenceStart)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_SEQUENCE_COMPLETE, t4p::MainFrameClass::OnSequenceComplete)
	EVT_SEQUENCE_PROGRESS(t4p::MainFrameClass::OnDeterminateProgress)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::AppEventListenerForFrameClass, wxEvtHandler)
	EVT_CMD_FILE_OPEN(t4p::AppEventListenerForFrameClass::OnCmdFileOpen)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_FILE_CREATED, t4p::AppEventListenerForFrameClass::OnAppFileCreated)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::AppEventListenerForFrameClass::OnPreferencesSaved)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_EXTERNALLY_UPDATED, t4p::AppEventListenerForFrameClass::OnPreferencesExternallyUpdated)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_READY, t4p::AppEventListenerForFrameClass::OnAppReady)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_CMD_APP_USER_ATTENTION, t4p::AppEventListenerForFrameClass::OnAppRequestUserAttention)
	EVT_AUINOTEBOOK_PAGE_CHANGED(t4p::ID_CODE_NOTEBOOK, t4p::AppEventListenerForFrameClass::OnCodeNotebookPageChanged)
	EVT_AUINOTEBOOK_PAGE_CLOSED(t4p::ID_CODE_NOTEBOOK, t4p::AppEventListenerForFrameClass::OnCodeNotebookPageClosed)
END_EVENT_TABLE()