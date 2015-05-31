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
#include <views/FeatureViewClass.h>
#include <features/FeatureClass.h>
#include <Triumph.h>
#include <main_frame/PreferencesDialogClass.h>
#include <widgets/StatusBarWithGaugeClass.h>
#include <widgets/NotebookClass.h>
#include <globals/Assets.h>
#include <wx/artprov.h>
#include <wx/choicdlg.h>
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/aboutdlg.h>
#include <wx/stc/stc.h>
#include <vector>

static int ID_TOOLBAR = wxNewId();
static int ID_SEQUENCE_GAUGE = wxNewId();

/**
 * Returns the code control that has focus.
 * @param mainWindow
 * @return the code control that has focus, or NULL if focus is on
 *         another window
 */
static t4p::CodeControlClass* FindFocusedCodeControl(wxWindow* mainWindow) {
	t4p::CodeControlClass* codeCtrl = NULL;
	wxWindow* focusWindow = wxWindow::FindFocus();
	if (!focusWindow) {
		return codeCtrl;
	}

	// we don't just want to cast since we don't know if the focused window
	// is a code control. We find out if the focus is anywhere inside a
	// notebook; either the notebook itself, or one of its tabs or borders.
	// the focused window is in one of the code notebooks
	std::vector<t4p::NotebookClass*> notebooks = t4p::CodeNotebooks(mainWindow);
	for (size_t i = 0; i < notebooks.size(); ++i) {
		t4p::NotebookClass* notebook = notebooks[i];
		if (focusWindow == notebook || notebook->IsDescendant(focusWindow)) {
			codeCtrl = notebook->GetCurrentCodeControl();
		}
	}
	return codeCtrl;
}

static bool FindFocusedCodeControlWithNotebook(wxWindow* mainWindow,
		t4p::CodeControlClass** codeCtrl, t4p::NotebookClass** notebook) {
	bool found = false;
	*codeCtrl = FindFocusedCodeControl(mainWindow);
	if (!(*codeCtrl)) {
		return found;
	}

	// get the notebook parent for the focused code control
	std::vector<t4p::NotebookClass*> notebooks = t4p::CodeNotebooks(mainWindow);
	for (size_t i = 0; i < notebooks.size(); ++i) {
		if (notebooks[i]->GetPageIndex(*codeCtrl) != wxNOT_FOUND) {
			*notebook = notebooks[i];
			found = true;
		}
	}
	return found;
}

t4p::MainFrameClass::MainFrameClass(const std::vector<t4p::FeatureViewClass*>& featureViews,
										t4p::AppClass& app)
	: MainFrameGeneratedClass(NULL)
	, AuiManager()
	, FeatureViews(featureViews)
	, Listener(this)
	, App(app)
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

	t4p::NotebookClass* codeNotebook = new t4p::NotebookClass(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		wxAUI_NB_CLOSE_ON_ACTIVE_TAB | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_TAB_MOVE | wxAUI_NB_WINDOWLIST_BUTTON,

		// very important that the name be given, this is how we find the
		// notebooks when iterating through the frame's children
		"NotebookClass"
	);
	codeNotebook->InitApp(
		&App.Preferences.CodeControlOptions,
		&App.Preferences,
		&App.Globals,
		&App.EventSink
	);

	ToolsNotebook = new wxAuiNotebook(this, t4p::ID_TOOLS_NOTEBOOK, wxDefaultPosition, wxDefaultSize,
		wxAUI_NB_TOP | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB | wxAUI_NB_TAB_MOVE);
	OutlineNotebook = new wxAuiNotebook(this, t4p::ID_OUTLINE_NOTEBOOK, wxDefaultPosition, wxDefaultSize,
		wxAUI_NB_TOP | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB | wxAUI_NB_TAB_MOVE);

	SetApplicationFont();

	// setup the bottom "tools" pane, the main content pane, and the toolbar on top
	AuiManager.AddPane(codeNotebook, wxAuiPaneInfo().Name(wxT("content")).CentrePane(
		).PaneBorder(true).Gripper(false).Floatable(false).Resizable(true));
	AuiManager.AddPane(ToolsNotebook, wxAuiPaneInfo().Name(wxT("tools")).Bottom().Caption(
		_("Tools")).Floatable(false).MinSize(-1, 260).Hide().Layer(0));
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

		wxNotifyEvent evtFrame(t4p::EVENT_APP_FRAME_CLOSE);
		App.EventSink.Publish(evtFrame);
		destroy = evtFrame.IsAllowed();
	}
	if (destroy) {
		wxCommandEvent exitEvent(t4p::EVENT_APP_EXIT);
		App.EventSink.Publish(exitEvent);

		// need to detach the window from the keyboard BEFORE the window is invalidated
		// clearing the menu shortcuts because the menu is about to be gone
		App.Preferences.KeyProfiles.GetSelProfile()->DetachAll();
		App.Preferences.ClearAllShortcuts();

		// need to remove the logging to the messages tab. We need to do this
		// so that because a log message will try to be written to the
		// messages grid but the frame is about to be destroyed
		wxLog* logger = wxLog::SetActiveTarget(NULL);
		if (logger) {
			delete logger;
		}
		wxLog::DontCreateOnDemand();
		App.Sequences.Stop();

		App.EventSink.RemoveHandler(&Listener);
		App.RunningThreads.RemoveEventHandler(this);
		App.SqliteRunningThreads.RemoveEventHandler(this);
		App.DeleteFeatureViews();

		// cleanup all open code controls and tabs. this is because
		// we want to destroy those items because they may have
		// threads that are running

		while (ToolsNotebook->GetPageCount() > 0) {
			ToolsNotebook->DeletePage(0);
		}
		while (OutlineNotebook->GetPageCount() > 0) {
			OutlineNotebook->DeletePage(0);
		}
		event.Skip();
	}
}

void t4p::MainFrameClass::CreateNewCodeCtrl() {
	std::vector<t4p::NotebookClass*> notebooks = t4p::CodeNotebooks(this);
	if (!notebooks.empty()) {
		notebooks[0]->AddTriumphPage(t4p::FILE_TYPE_PHP);
	}
}

void t4p::MainFrameClass::OnFileExit(wxCommandEvent& event) {
	if (Close()) {

		// on mac, we don't close the app when the user closes the
		// main frame. *But* this handler is for the exit menu item,
		// this means that the user really wants to stop the app.
		App.SetExitOnFrameDelete(true);
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
	PreferencesDialogClass prefDialog(this, App.Globals, App.Preferences, settingsDir, changedSettingsDir, needsRetag);
	App.AddPreferencesWindows(prefDialog.GetBookCtrl());
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
	App.Preferences.EnableSelectedProfile(this);
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
	if (info.GetOperatingSystemId() == wxOS_UNIX_LINUX) {
		SetFont(App.Preferences.ApplicationFont);

		// so that the tabs use the same font
		ToolsNotebook->SetFont(App.Preferences.ApplicationFont);
		ToolsNotebook->SetNormalFont(App.Preferences.ApplicationFont);
		OutlineNotebook->SetFont(App.Preferences.ApplicationFont);
		OutlineNotebook->SetNormalFont(App.Preferences.ApplicationFont);

		// so that the toolbar buttons use the same font
		ToolBar->SetFont(App.Preferences.ApplicationFont);
	}
}

void t4p::MainFrameClass::PreferencesExternallyUpdated() {
	PreferencesSaved();
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

void t4p::MainFrameClass::OnUpdateUi(wxUpdateUIEvent& event) {
	App.EventSink.Publish(event);
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

void t4p::MainFrameClass::LoadFeatureView(t4p::FeatureViewClass& view) {

	// propagate GUI events to features, so that they can handle menu events themselves
	// feature menus
	view.InitWindow(GetStatusBarWithGauge(), ToolsNotebook, OutlineNotebook,
		&AuiManager, GetMenuBar(), ToolBar);

	//  when adding the separators, we dont want a separator at the very end
	// we dont need separators if the feature did not add any menu items
	size_t oldFileMenuCount = FileMenu->GetMenuItemCount();
	view.AddFileMenuItems(FileMenu);
	if (oldFileMenuCount != FileMenu->GetMenuItemCount() && oldFileMenuCount > 0) {
		FileMenu->InsertSeparator(oldFileMenuCount);
	}

	size_t oldEditMenuCount = EditMenu->GetMenuItemCount();
	view.AddEditMenuItems(EditMenu);
	if (oldEditMenuCount != EditMenu->GetMenuItemCount() && oldEditMenuCount > 0) {
		EditMenu->InsertSeparator(oldEditMenuCount);
	}

	size_t oldViewMenuCount = ViewMenu->GetMenuItemCount();
	view.AddViewMenuItems(ViewMenu);
	if (oldViewMenuCount != ViewMenu->GetMenuItemCount() && oldViewMenuCount > 0) {
		ViewMenu->InsertSeparator(oldViewMenuCount);
	}

	size_t oldSearchMenuCount = SearchMenu->GetMenuItemCount();
	view.AddSearchMenuItems(SearchMenu);
	if (oldSearchMenuCount != SearchMenu->GetMenuItemCount() && oldSearchMenuCount > 0) {
		SearchMenu->InsertSeparator(oldSearchMenuCount);
	}

	size_t oldHelpMenuCount = HelpMenu->GetMenuItemCount();
	view.AddHelpMenuItems(HelpMenu);
	if (oldHelpMenuCount != HelpMenu->GetMenuItemCount() && oldHelpMenuCount > 0) {
		HelpMenu->InsertSeparator(oldHelpMenuCount);
	}

	wxMenuBar* menuBar = GetMenuBar();
	view.AddNewMenu(menuBar);

	// new menus may have been added; push the Help menu all the way to the end
	int helpIndex = menuBar->FindMenu(_("&Help"));
	if (helpIndex != wxNOT_FOUND) {
		menuBar->Remove(helpIndex);
		menuBar->Insert(menuBar->GetMenuCount(), HelpMenu, _("&Help"));
	}

	// move preferences menu to the end, similar to most other programs
	wxMenuItem* preferencesMenu = EditMenu->Remove(wxID_PREFERENCES);
	EditMenu->Append(preferencesMenu);

	// move exit menu to the end, similar to most other programs
	wxMenuItem* exitMenu = FileMenu->Remove(wxID_EXIT);
	FileMenu->Append(exitMenu);

	view.AddToolBarItems(ToolBar);
	view.AddWindows();
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
	CodeControlClass* codeWindow = FindFocusedCodeControl(this);
	
	// only show the user if and only if
	// user clicked inside of the code control
	if (codeWindow != NULL && event.GetEventObject() == codeWindow) {

		// make sure to turn off any call tips; they hide the popup menu
		codeWindow->SetAsHidden(true);
		wxMenu contextMenu;
		for (size_t i = 0; i < FeatureViews.size(); ++i) {
			FeatureViews[i]->AddCodeControlClassContextMenuItems(&contextMenu);
		}
		PopupMenu(&contextMenu);
		codeWindow->SetAsHidden(false);
	}
	else {

		// if a sub panel has its own handler, use it
		event.Skip();
	}
}

void t4p::MainFrameClass::DefaultKeyboardShortcuts() {

	// ATTN: when a new menu item is added to the form builder
	// we will need to add an entry here so that shortcuts
	// work properly
	std::map<int, wxString>defaultMenus;

	defaultMenus[wxID_PREFERENCES] = wxT("Edit-Preferences");
	defaultMenus[ID_VIEW_TOGGLE_TOOLS] = wxT("View-Tools");
	defaultMenus[ID_VIEW_TOGGLE_OUTLINE] = wxT("View-Outline");

	defaultMenus[wxID_ABOUT] = wxT("Help-About");
	wxMenuBar* menuBar = GetMenuBar();
	for (std::map<int, wxString>::iterator it = defaultMenus.begin(); it != defaultMenus.end(); ++it) {
		wxMenuItem* item = menuBar->FindItem(it->first);
		wxASSERT_MSG(item, wxT("Menu item not found:") + it->second);
		t4p::DynamicCmdClass cmd(item, it->second);
		App.Preferences.DefaultKeyboardShortcutCmds.push_back(cmd);
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
		if (t4p::ID_TOOLS_NOTEBOOK == event.GetId()) {
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
		else {
			// this is a code notebook
			// for code notebooks; we want to keep 1, but if the user
			// removes pages from the 2-N notebook, then we kill the
			// empty notebook
			std::vector<t4p::NotebookClass*> notebooks = t4p::CodeNotebooks(this);
			wxAuiNotebook* codeNotebook = wxDynamicCast(event.GetEventObject(), wxAuiNotebook);
			if (notebooks.size() > 1 && codeNotebook) {
				size_t count = codeNotebook->GetPageCount();
				if (count <= 0) {
					AuiManager.DetachPane(codeNotebook);

					// don't delete now; wait until events are processed
					CallAfter(&t4p::MainFrameClass::DeleteEmptyCodeNotebooks);
				}
			}
		}
	}
	App.EventSink.Publish(event);
	event.Skip();
}

void t4p::MainFrameClass::DeleteEmptyCodeNotebooks() {

	// we want to keep 1 code notebook, even if empty
	std::vector<t4p:: NotebookClass*> notebooks = t4p::CodeNotebooks(this);
	std::vector<t4p:: NotebookClass*>::iterator it = notebooks.begin();
	while (it != notebooks.end() && notebooks.size() > 1) {
		if ((*it)->GetPageCount() <= 0) {
			(*it)->Destroy();
			it = notebooks.erase(it);
		}
		else {
			++it;
		}
	}
	AuiManager.Update();
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
	t4p::CodeControlClass* codeControl = NULL;
	t4p::NotebookClass* notebook = NULL;
	if (!FindFocusedCodeControlWithNotebook(this, &codeControl, &notebook)) {
		SetTitle(_("Triumph4PHP"));
		return;
	}
	wxString fileName = codeControl->GetFileName();
	if (fileName.IsEmpty()) {

		// file name empty means this is a new file, use the tab text
		fileName = notebook->GetPageText(notebook->GetPageIndex(codeControl));
	}
	SetTitle(_("Triumph4PHP: ") + fileName);
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

void t4p::MainFrameClass::OnStcSavePointReached(wxStyledTextEvent& event) {
	App.EventSink.Publish(event);
}

void t4p::MainFrameClass::OnStcSavedPointLeft(wxStyledTextEvent& event) {
	App.EventSink.Publish(event);
}

t4p::AppEventListenerForFrameClass::AppEventListenerForFrameClass(t4p::MainFrameClass* mainFrame)
	: wxEvtHandler()
	, MainFrame(mainFrame) {

}

void t4p::AppEventListenerForFrameClass::OnPreferencesSaved(wxCommandEvent& event) {
	MainFrame->PreferencesSaved();
}

void t4p::AppEventListenerForFrameClass::OnPreferencesExternallyUpdated(wxCommandEvent& event) {
	MainFrame->PreferencesExternallyUpdated();
}

void t4p::AppEventListenerForFrameClass::OnAppReady(wxCommandEvent& event) {
	MainFrame->PreferencesExternallyUpdated();
}

void t4p::AppEventListenerForFrameClass::OnAppFilePageChanged(t4p::CodeControlEventClass& event) {
	MainFrame->UpdateTitleBar();
}

void t4p::AppEventListenerForFrameClass::OnAppFileCreated(wxCommandEvent& event) {
	MainFrame->UpdateTitleBar();
}

void t4p::AppEventListenerForFrameClass::OnAppFileClosed(t4p::CodeControlEventClass& event) {
	MainFrame->UpdateTitleBar();
}

void t4p::AppEventListenerForFrameClass::OnAppRequestUserAttention(wxCommandEvent& event) {
	MainFrame->RequestUserAttention();
}


BEGIN_EVENT_TABLE(t4p::MainFrameClass,  MainFrameGeneratedClass)
	EVT_CONTEXT_MENU(t4p::MainFrameClass::OnContextMenu)

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

	EVT_MENU(wxID_OPEN, t4p::MainFrameClass::OnAnyMenuCommandEvent)
	EVT_MENU(wxID_SAVE, t4p::MainFrameClass::OnAnyMenuCommandEvent)
	EVT_MENU(wxID_SAVEAS, t4p::MainFrameClass::OnAnyMenuCommandEvent)
	EVT_MENU(wxID_CLOSE, t4p::MainFrameClass::OnAnyMenuCommandEvent)
	EVT_MENU(wxID_CUT, t4p::MainFrameClass::OnAnyMenuCommandEvent)
	EVT_MENU(wxID_COPY, t4p::MainFrameClass::OnAnyMenuCommandEvent)
	EVT_MENU(wxID_PASTE, t4p::MainFrameClass::OnAnyMenuCommandEvent)
	EVT_MENU(wxID_SELECTALL, t4p::MainFrameClass::OnAnyMenuCommandEvent)
	EVT_MENU(wxID_UNDO, t4p::MainFrameClass::OnAnyMenuCommandEvent)
	EVT_MENU(wxID_REDO, t4p::MainFrameClass::OnAnyMenuCommandEvent)

	EVT_TOOL(wxID_OPEN, t4p::MainFrameClass::OnAnyMenuCommandEvent)
	EVT_TOOL(wxID_SAVE, t4p::MainFrameClass::OnAnyMenuCommandEvent)
	EVT_TOOL(wxID_SAVEAS, t4p::MainFrameClass::OnAnyMenuCommandEvent)
	EVT_TOOL(wxID_CLOSE, t4p::MainFrameClass::OnAnyMenuCommandEvent)

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

	// make sure some STC events are propagated
	EVT_STC_SAVEPOINTREACHED(wxID_ANY, t4p::MainFrameClass::OnStcSavePointReached)
	EVT_STC_SAVEPOINTLEFT(wxID_ANY, t4p::MainFrameClass::OnStcSavedPointLeft)
	EVT_UPDATE_UI(wxID_ANY, t4p::MainFrameClass::OnUpdateUi)

	// make sure to show status of running sequences
	EVT_COMMAND(wxID_ANY, t4p::EVENT_SEQUENCE_START, t4p::MainFrameClass::OnSequenceStart)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_SEQUENCE_COMPLETE, t4p::MainFrameClass::OnSequenceComplete)
	EVT_SEQUENCE_PROGRESS(t4p::MainFrameClass::OnDeterminateProgress)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::AppEventListenerForFrameClass, wxEvtHandler)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_FILE_CREATED, t4p::AppEventListenerForFrameClass::OnAppFileCreated)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::AppEventListenerForFrameClass::OnPreferencesSaved)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_EXTERNALLY_UPDATED, t4p::AppEventListenerForFrameClass::OnPreferencesExternallyUpdated)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_READY, t4p::AppEventListenerForFrameClass::OnAppReady)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_CMD_APP_USER_ATTENTION, t4p::AppEventListenerForFrameClass::OnAppRequestUserAttention)
	EVT_APP_FILE_PAGE_CHANGED(t4p::AppEventListenerForFrameClass::OnAppFilePageChanged)
	EVT_APP_FILE_CLOSED(t4p::AppEventListenerForFrameClass::OnAppFileClosed)
END_EVENT_TABLE()
