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
#include <plugins/RunConsolePluginClass.h>
#include <MvcEditorErrors.h>
#include <wx/artprov.h>
#include <wx/filename.h>
#include <wx/sstream.h>
#include <wx/valgen.h>

static const int ID_PROCESS = wxNewId();
static const int ID_TOOLBAR_RUN = wxNewId();
const int ID_TOOLBAR_BROWSER = wxNewId();
static const int ID_WINDOW_CONSOLE = wxNewId();

static void ExternalBrowser(const wxString& browserName, const wxString& url, mvceditor::EnvironmentClass* environment) {
	wxFileName webBrowserPath  = environment->WebBrowsers[browserName];
	if (!webBrowserPath.IsOk()) {
		mvceditor::EditorLogWarning(mvceditor::BAD_WEB_BROWSER_EXECUTABLE, webBrowserPath.GetFullPath());
		return;
	}
	wxString cmd = wxT("\"") + webBrowserPath.GetFullPath() + wxT("\""); 
	cmd += wxT(" \"");
	cmd += url;
	cmd += wxT("\"");
			
	// TODO track this PID ... ?
	// what about when user closes the external browser ?
	// make the browser its own process so that it stays alive if the editor program is exited
	// if we dont do this the browser thinks it crashed and will tell the user so
	long pid = wxExecute(cmd, wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER);
	if (pid <= 0) {
		mvceditor::EditorLogWarning(mvceditor::BAD_WEB_BROWSER_EXECUTABLE, cmd);
	}
}

mvceditor::RunConsolePanelClass::RunConsolePanelClass(wxWindow* parent, EnvironmentClass* environment, StatusBarWithGaugeClass* gauge, int id)
	: RunConsolePanelGeneratedClass(parent, id)
	, CommandString()
	, ProcessWithHeartbeat(*this)
	, Environment(environment)
	, Gauge(gauge)
	, CurrentPid(0) {
	wxGenericValidator commandValidator(&CommandString);
	Command->SetValidator(commandValidator);
	IdProcessGauge = wxNewId();

	// need to attach to the notebook page close event here
	// so that we can cleanup the child process AND also remove any open gauge
	// can't do this in destructor because we need to guarantee
	// that the gauge pointer is valid
	parent->Connect(wxID_ANY, wxID_ANY, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, 
		wxAuiNotebookEventHandler(mvceditor::RunConsolePanelClass::OnPageClose), NULL, this);
}

void mvceditor::RunConsolePanelClass::OnPageClose(wxAuiNotebookEvent& evt) {
	int selected = evt.GetSelection();
	wxAuiNotebook* ctrl = (wxAuiNotebook*)evt.GetEventObject();
	if (ctrl->GetPage(selected) == this) {

		// make sure we kill any running processes
		if (CurrentPid > 0) {
			ProcessWithHeartbeat.Stop(CurrentPid);
		}
		Gauge->StopGauge(IdProcessGauge);
		GetParent()->Disconnect(wxID_ANY, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, 
			wxAuiNotebookEventHandler(mvceditor::RunConsolePanelClass::OnPageClose), NULL, this);
	}
	evt.Skip();
}

void  mvceditor::RunConsolePanelClass::SetToRunFile(const wxString& fullPath) {
	
	// command is a file name, lets run it through PHP
	// cannot run new files that have not been saved yet
	if (!fullPath.empty()) {
		CommandString = Environment->Php.PhpExecutablePath + wxT(" ") + fullPath;
		TransferDataToWindow();
	}
	else {
		CommandString = wxT("");
		wxMessageBox(_("PHP script needs to be saved in order to run it."));
	}
}

void  mvceditor::RunConsolePanelClass::SetFocusOnCommandText() {
	Command->SetFocus();
}

void  mvceditor::RunConsolePanelClass::RunCommand(wxCommandEvent& event) {
	
	// do not run a process if one is already running. the 'Run' button
	// converts to a 'stop' button when a process is running.
	if (!CurrentPid && TransferDataFromWindow() && !CommandString.IsEmpty()) {
		Command->Enable(false);
		RunButton->SetLabel(_("Stop"));
		if (ProcessWithHeartbeat.Init(CommandString, ID_PROCESS, CurrentPid)) {
			Gauge->AddGauge(_("Running Process"), IdProcessGauge, StatusBarWithGaugeClass::INDETERMINATE_MODE, 0);	
		}
		else {
			Command->Enable(true);
			RunButton->SetLabel(_("Start"));
		}
	}
	else if (CurrentPid > 0) {
		bool stopped = ProcessWithHeartbeat.Stop(CurrentPid);		
		if (!stopped) {
			mvceditor::EditorLogError(mvceditor::ROGUE_PROCESS, 
				wxString::Format(wxT("Process ID: %ld."), CurrentPid));

			// stale PID??
			Gauge->StopGauge(IdProcessGauge);
			CurrentPid = 0;
		}
		Command->Enable(true);
		RunButton->SetLabel(_("Start"));
	}
}

void  mvceditor::RunConsolePanelClass::OnClear(wxCommandEvent& event) {
	OutputWindow->ChangeValue(wxT(""));
}

void  mvceditor::RunConsolePanelClass::OnProcessFailed(wxCommandEvent& event) {
	wxString output = event.GetString();

	// if no output, do not append.  This will allow the user the ability to select the text when the process is silent
	if (!output.IsEmpty()) {
		OutputWindow->AppendText(output);
	}
	Gauge->StopGauge(IdProcessGauge);
	CurrentPid = 0;
	Command->Enable(true);
	RunButton->SetLabel(_("Run"));
}

void  mvceditor::RunConsolePanelClass::OnProcessInProgress(wxCommandEvent& event) {
	wxString output = ProcessWithHeartbeat.GetProcessOutput(CurrentPid);

	// if no output, do not append.  This will allow the user the ability to select the text when the process is silent
	if (!output.IsEmpty()) {
		OutputWindow->AppendText(output);
	}
	Gauge->IncrementGauge(IdProcessGauge, StatusBarWithGaugeClass::INDETERMINATE_MODE);
}

void  mvceditor::RunConsolePanelClass::OnProcessComplete(wxCommandEvent& event) { 
	wxString output = event.GetString();

	// if no output, do not append.  This will allow the user the ability to select the text when the process is silent
	if (!output.IsEmpty()) {
		OutputWindow->AppendText(output);
	}
	Gauge->StopGauge(IdProcessGauge);
	CurrentPid = 0;
	Command->Enable(true);
	RunButton->SetLabel(_("Run"));
}

mvceditor::RunConsolePluginClass::RunConsolePluginClass()
	: PluginClass()
	, RunCliMenuItem(NULL)
	, RunCliWithArgsMenuItem(NULL)
	, RunCliInNewWindowMenuItem(NULL)
	, RunCliWithArgsInNewWindowMenuItem(NULL)
	, RunInBrowser(NULL)
	, BrowserComboBox(NULL) 
	, PhpFrameworks(NULL) {
}

mvceditor::RunConsolePluginClass::~RunConsolePluginClass() {
	if (PhpFrameworks) {
		delete PhpFrameworks;
	}
}


void mvceditor::RunConsolePluginClass::AddToolsMenuItems(wxMenu* toolsMenu) {
	RunCliMenuItem = new wxMenuItem(toolsMenu, mvceditor::MENU_RUN_PHP + 0, _("Run As CLI\tF7"), 
		_("Run File As a PHP Command Line Script"), wxITEM_NORMAL);
	toolsMenu->Append(RunCliMenuItem);
	RunCliWithArgsMenuItem = new wxMenuItem(toolsMenu, mvceditor::MENU_RUN_PHP + 1, _("Run As CLI With Arguments\tSHIFT+F7"), 
		_("Run File As a PHP Command Line Script With Arguments"), wxITEM_NORMAL);
	toolsMenu->Append(RunCliWithArgsMenuItem);
	RunCliInNewWindowMenuItem = new wxMenuItem(toolsMenu, mvceditor::MENU_RUN_PHP + 2, _("Run As CLI In New Window\tCTRL+F7"), 
		_("Run File As a PHP Command Line Script In a New Window"), wxITEM_NORMAL);
	toolsMenu->Append(RunCliInNewWindowMenuItem);
	RunCliWithArgsInNewWindowMenuItem = new wxMenuItem(toolsMenu, mvceditor::MENU_RUN_PHP + 3, 
		_("Run As CLI In New Window With Arguments\tCTRL+SHIFT+F7"), 
		_("Run File As a PHP Command Line Script In a New Window With Arguments"), wxITEM_NORMAL);
	toolsMenu->Append(RunCliWithArgsInNewWindowMenuItem);
	RunInBrowser = new wxMenuItem(toolsMenu, mvceditor::MENU_RUN_PHP + 4, 
		_("Run In Web Browser"),
		_("Run the script in the chosen Web Browser"), wxITEM_NORMAL);
	toolsMenu->Append(RunInBrowser);
}

void mvceditor::RunConsolePluginClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_RUN_PHP + 0] = wxT("Console-Run");
	menuItemIds[mvceditor::MENU_RUN_PHP + 1] = wxT("Console-Run With Arguments");
	menuItemIds[mvceditor::MENU_RUN_PHP + 2] = wxT("Console-Run In New Window");
	menuItemIds[mvceditor::MENU_RUN_PHP + 3] = wxT("Console-Run-In New Window With Arguments");
	menuItemIds[mvceditor::MENU_RUN_PHP + 4] = wxT("Console-Run-In Web Browser");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::RunConsolePluginClass::LoadPreferences(wxConfigBase* config) {
	
	// dont use the config; use the Environment that has already been seeded with 
	// the proper data
	mvceditor::EnvironmentClass* environment = GetEnvironment();
	std::map<wxString, wxFileName> webBrowsers = environment->WebBrowsers;
	for (std::map<wxString, wxFileName>::const_iterator it = webBrowsers.begin(); it != webBrowsers.end(); ++it) {
		BrowserComboBox->AppendString(it->first);
	}
	if (!webBrowsers.empty()) {
		BrowserComboBox->Select(0);
	}
}

void mvceditor::RunConsolePluginClass::OnRunFileAsCli(wxCommandEvent& event) {
	CodeControlClass* code = GetCurrentCodeControl();
	
	// TODO: need to hash out the what happens when the user runs a script when the run button is clicked
	// the user has filled in run arguments the run button kills them
	if (code) {
		
		// right now dont really care which window, just want to reuse an existing one. that's why all windows
		// are created with the same ID
		wxWindow* window = wxWindow::FindWindowById(ID_WINDOW_CONSOLE, GetToolsNotebook());
		if (NULL != window) {
			RunConsolePanelClass* runConsolePanel = (RunConsolePanelClass*)window;
			
			// window already created
			SetFocusToToolsWindow(runConsolePanel);
			runConsolePanel->SetToRunFile(code->GetFileName());
			runConsolePanel->SetFocusOnCommandText();
			
			// if user chose the 'with arguments' then do not proceed let the user put in arguments
			if ((mvceditor::MENU_RUN_PHP + 1) != event.GetId()) {
				runConsolePanel->RunCommand(event);	
			}
		}
		else {
			RunConsolePanelClass* runConsolePanel = new RunConsolePanelClass(GetToolsNotebook(), GetEnvironment(), 
				GetStatusBarWithGauge(), ID_WINDOW_CONSOLE);	
			if (AddToolsWindow(runConsolePanel, _("Run"))) {
				runConsolePanel->SetToRunFile(code->GetFileName());
				runConsolePanel->SetFocusOnCommandText();
				
				// if user chose the 'with arguments' then do not proceed let the user put in arguments
				if ((mvceditor::MENU_RUN_PHP + 1) != event.GetId()) {
					runConsolePanel->RunCommand(event);
				}
			}
		}
	}
}

void mvceditor::RunConsolePluginClass::OnRunFileAsCliInNewWindow(wxCommandEvent& event) {
	CodeControlClass* code = GetCurrentCodeControl();
	if (code) {
		RunConsolePanelClass* window = new RunConsolePanelClass(GetToolsNotebook(), GetEnvironment(), 
			GetStatusBarWithGauge(), ID_WINDOW_CONSOLE);
		if (AddToolsWindow(window, _("Run"))) {
			window->SetToRunFile(code->GetFileName());
			window->SetFocusOnCommandText();
						
			// if user chose the 'with arguments' then do not proceed let the user put in arguments
			if ((mvceditor::MENU_RUN_PHP + 3) != event.GetId()) {
				window->RunCommand(event);
			}
		}
	}
}

void mvceditor::RunConsolePluginClass::OnUpdateUi(wxUpdateUIEvent& event) {
	bool hasEditors = NULL != GetCurrentCodeControl();
	RunCliMenuItem->Enable(hasEditors);
	RunCliInNewWindowMenuItem->Enable(hasEditors);
	RunCliWithArgsMenuItem->Enable(hasEditors);
	RunCliWithArgsInNewWindowMenuItem->Enable(hasEditors);
	event.Skip();
}

void mvceditor::RunConsolePluginClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	toolBar->AddTool(ID_TOOLBAR_RUN, _("Run"), wxArtProvider::GetBitmap(
		wxART_EXECUTABLE_FILE, wxART_TOOLBAR, wxSize(16, 16)), _("Run"));

	wxBitmap bitmap = wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE, wxART_TOOLBAR, wxSize(16, 16));
	toolBar->AddTool(ID_TOOLBAR_BROWSER, _("Web Browser"), bitmap, _("Run On a Web Browser"));
	
	// to be filled in after the config is read
	wxArrayString choices;
	BrowserComboBox = new wxComboBox(toolBar, wxID_NEW, wxT(""), wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	toolBar->AddControl(BrowserComboBox);
}

void mvceditor::RunConsolePluginClass::OnRunInWebBrowser(wxCommandEvent& event) {
	mvceditor::CodeControlClass* currentCodeControl = GetCurrentCodeControl();
	if (!currentCodeControl) {
		return;
	}
	wxString fileName = currentCodeControl->GetFileName();
	mvceditor::EnvironmentClass* environment = GetEnvironment();
	if (PhPFrameworks().Identifiers.empty()) {
		wxString browserName = BrowserComboBox->GetValue();
		if (!wxFileName::FileExists(fileName)) {
			mvceditor::EditorLogWarning(mvceditor::INVALID_FILE, fileName);
			return;
		}

		// turn file name into a url in the default manner (by calculating from the vhost document root)
		wxString url = environment->Apache.GetUrl(fileName);
		if (url.IsEmpty()) {
			mvceditor::EditorLogWarning(mvceditor::INVALID_FILE, _("File is not under web root"));	
			return;
		}	
		ExternalBrowser(browserName, url, environment);
	}
	else if (!PhpFrameworks) {
		PhpFrameworks = new PhpFrameworkDetectorClass(*this, *environment);
		PhpFrameworks->Identifiers = PhPFrameworks().Identifiers;
		PhpFrameworks->InitUrlDetector(GetProject()->GetRootPath(), fileName);
	}
	else {
		PhpFrameworks->InitUrlDetector(GetProject()->GetRootPath(), fileName);
	}
}

void mvceditor::RunConsolePluginClass::OnUrlDetectionComplete(mvceditor::UrlDetectedEventClass& event) {
	mvceditor::CodeControlClass* currentCodeControl = GetCurrentCodeControl();
	if (!currentCodeControl) {
		return;
	}
	wxString fileName = currentCodeControl->GetFileName();
	wxString browserName = BrowserComboBox->GetValue();
	mvceditor::EnvironmentClass* environment = GetEnvironment();
	if (!event.Urls.empty()) {
		mvceditor::UrlChoiceClass urlChoice(event.Urls, fileName, environment);
		mvceditor::ChooseUrlDialogClass dialog(GetMainWindow(), urlChoice);
		if (wxOK == dialog.ShowModal()) {
			wxString chosenUrl = urlChoice.ChosenUrl();
			ExternalBrowser(browserName, chosenUrl, environment);
			
			// TODO: save this choice into a toolbar dropdown so that the user can easily access it
			// multiple times.
		}
	}
	else {
		
		// no URLs means that the file can be accessed normally
		// turn file name into a url in the default manner (by calculating from the vhost document root)
		wxString url = environment->Apache.GetUrl(fileName);
		if (url.IsEmpty()) {
			mvceditor::EditorLogWarning(mvceditor::INVALID_FILE, _("File is not under web root"));	
			return;
		}	
		ExternalBrowser(browserName, url, environment);
	}
}

mvceditor::UrlChoiceClass::UrlChoiceClass(const std::vector<wxString>& urls, const wxString& fileName, mvceditor::EnvironmentClass* environment)
	: UrlList()
	, Extra() 
	, ChosenIndex(0) {
	wxString url = environment->Apache.GetUrl(fileName);
		
	// we only want the virtual host name, as the url we get from the framework's routing url scheme
	wxString host = url.Mid(7); // 7 = length of "http://"
	host = host.Mid(0, host.Find(wxT("/")));
	host = wxT("http://") + host + wxT("/");
	for (size_t i = 0; i < urls.size(); ++i) {
		wxString item = host + urls[i];
		UrlList.Add(item);
	}
}

wxString mvceditor::UrlChoiceClass::ChosenUrl() const {
	wxString chosenUrl;
	if ((size_t)ChosenIndex < UrlList.size()) {
		chosenUrl = UrlList[ChosenIndex] + Extra;
	}
	return chosenUrl;
}

mvceditor::ChooseUrlDialogClass::ChooseUrlDialogClass(wxWindow* parent, mvceditor::UrlChoiceClass& urlChoice)
	: ChooseUrlDialogGeneratedClass(parent, wxID_ANY)
	, UrlChoice(urlChoice) {
	UrlList->Append(UrlChoice.UrlList);
	UrlList->SetSelection(UrlChoice.ChosenIndex);
	
	wxGenericValidator extraValidator(&UrlChoice.Extra);
	Extra->SetValidator(extraValidator);
	TransferDataToWindow();
	
	if (!urlChoice.UrlList.IsEmpty()) {
		wxString label= _("Complete URL: ") + urlChoice.UrlList[0];
		CompleteLabel->SetLabel(label);
	}
}

void mvceditor::ChooseUrlDialogClass::OnOkButton(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow()) {
		UrlChoice.ChosenIndex = UrlList->GetSelection();
		EndModal(wxOK);
	}
}

void mvceditor::ChooseUrlDialogClass::OnUpdateUi(wxUpdateUIEvent& event) {
	wxString url = UrlList->GetStringSelection() + Extra->GetValue();
	wxString label= _("Complete URL: ") + url;
	CompleteLabel->SetLabel(label);
	event.Skip();
}
	
void mvceditor::RunConsolePluginClass::OnUrlDetectionFailed(wxCommandEvent& event) {
	mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, event.GetString());
}

BEGIN_EVENT_TABLE(mvceditor::RunConsolePanelClass, wxPanel) 
	EVT_COMMAND(ID_PROCESS, mvceditor::EVENT_PROCESS_COMPLETE, mvceditor::RunConsolePanelClass::OnProcessComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_PROCESS_IN_PROGRESS, mvceditor::RunConsolePanelClass::OnProcessInProgress)
	EVT_COMMAND(ID_PROCESS, mvceditor::EVENT_PROCESS_FAILED, mvceditor::RunConsolePanelClass::OnProcessFailed)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::RunConsolePluginClass, wxEvtHandler) 
	EVT_MENU(mvceditor::MENU_RUN_PHP + 0, mvceditor::RunConsolePluginClass::OnRunFileAsCli)
	EVT_MENU(mvceditor::MENU_RUN_PHP + 1, mvceditor::RunConsolePluginClass::OnRunFileAsCli)
	EVT_MENU(mvceditor::MENU_RUN_PHP + 2, mvceditor::RunConsolePluginClass::OnRunFileAsCliInNewWindow)
	EVT_MENU(mvceditor::MENU_RUN_PHP + 3, mvceditor::RunConsolePluginClass::OnRunFileAsCliInNewWindow)
	EVT_MENU(mvceditor::MENU_RUN_PHP + 4, mvceditor::RunConsolePluginClass::OnRunInWebBrowser)
	EVT_TOOL(ID_TOOLBAR_RUN, mvceditor::RunConsolePluginClass::OnRunFileAsCli)
	EVT_TOOL(ID_TOOLBAR_BROWSER, mvceditor::RunConsolePluginClass::OnRunInWebBrowser)
	EVT_UPDATE_UI(wxID_ANY, mvceditor::RunConsolePluginClass::OnUpdateUi)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_FRAMEWORK_URL_FAILED, mvceditor::RunConsolePluginClass::OnUrlDetectionFailed)
	EVT_FRAMEWORK_URL_COMPLETE(mvceditor::RunConsolePluginClass::OnUrlDetectionComplete)
END_EVENT_TABLE()