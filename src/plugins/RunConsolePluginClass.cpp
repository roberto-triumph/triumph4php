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
#include <wx/artprov.h>
#include <wx/filename.h>
#include <wx/sstream.h>
#include <wx/valgen.h>

int ID_MENU_RUN_CLI = mvceditor::PluginClass::newMenuId();
int ID_MENU_RUN_CLI_IN_NEW_WINDOW = mvceditor::PluginClass::newMenuId();
int ID_MENU_RUN_CLI_ARGS = mvceditor::PluginClass::newMenuId();
int ID_MENU_RUN_CLI_IN_NEW_WINDOW_ARGS = mvceditor::PluginClass::newMenuId();
int ID_CONSOLE = wxNewId();
int ID_PROCESS = wxNewId();
int ID_TOOLBAR_RUN = wxNewId();
int ID_WINDOW_CONSOLE = wxNewId();

BEGIN_EVENT_TABLE(mvceditor::RunConsolePanelClass, wxPanel) 
	EVT_TIMER(ID_CONSOLE,  mvceditor::RunConsolePanelClass::OnTimer)
	EVT_END_PROCESS(ID_PROCESS,  mvceditor::RunConsolePanelClass::OnEndProcess)
	EVT_IDLE(mvceditor::RunConsolePanelClass::OnIdle)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::RunConsolePluginClass, wxEvtHandler) 
	EVT_MENU(ID_MENU_RUN_CLI, mvceditor::RunConsolePluginClass::OnRunFileAsCli)
	EVT_MENU(ID_MENU_RUN_CLI_ARGS, mvceditor::RunConsolePluginClass::OnRunFileAsCli)
	EVT_MENU(ID_MENU_RUN_CLI_IN_NEW_WINDOW, mvceditor::RunConsolePluginClass::OnRunFileAsCliInNewWindow)
	EVT_MENU(ID_MENU_RUN_CLI_IN_NEW_WINDOW_ARGS, mvceditor::RunConsolePluginClass::OnRunFileAsCliInNewWindow)
	EVT_TOOL(ID_TOOLBAR_RUN, mvceditor::RunConsolePluginClass::OnRunFileAsCli)
	EVT_UPDATE_UI(wxID_ANY, mvceditor::RunConsolePluginClass::OnUpdateUi)
END_EVENT_TABLE()

 mvceditor::RunConsolePanelClass::RunConsolePanelClass(wxWindow* parent, EnvironmentClass* environment, StatusBarWithGaugeClass* gauge, int id)
	: RunConsolePanelGeneratedClass(parent, id)
	, CommandString()
	, Timer(this, ID_CONSOLE)
	, Process(NULL)
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
		if (Process) {
			Process->Detach();
		}
		Gauge->StopGauge(IdProcessGauge);
		GetParent()->Disconnect(wxID_ANY, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, 
			wxAuiNotebookEventHandler(mvceditor::RunConsolePanelClass::OnPageClose), NULL, this);
	}
	evt.Skip();
}

void  mvceditor::RunConsolePanelClass::SetToRunFile(const wxString& fullPath) {
	
	// command is a file name, lets run it through PHP
	CommandString = Environment->Php.PhpExecutablePath + wxT(" ") + fullPath;
	TransferDataToWindow();
}

void  mvceditor::RunConsolePanelClass::SetFocusOnCommandText() {
	Command->SetFocus();
}

void  mvceditor::RunConsolePanelClass::RunCommand(wxCommandEvent& event) {
	
	// do not run a process if one is already running. the 'Run' button
	// converts to a 'stop' button when a process is running.
	if (!Process && TransferDataFromWindow() && !CommandString.IsEmpty()) {
		Command->Enable(false);
		RunButton->SetLabel(_("Stop"));
		wxPlatformInfo platform;
		if (wxOS_WINDOWS_NT == platform.GetOperatingSystemId()) {
			
			// in windows, we will execute commands in the shell
			CommandString = wxT("cmd.exe /Q /C ") + CommandString;
		}
		Process = new wxProcess(this, ID_PROCESS);
		Process->Redirect();
		CurrentPid = wxExecute(CommandString, wxEXEC_ASYNC, Process);
		if (!CurrentPid || !Timer.Start( mvceditor::RunConsolePanelClass::POLL_INTERVAL, wxTIMER_CONTINUOUS)) {
			delete Process;
			Process = NULL;
			if (CurrentPid) {
				wxMessageBox(_("Could not start timer. (Internal Error)"));
			}
			Command->Enable(true);
			RunButton->SetLabel(_("Start"));
		}
		else {
			LastPulse = wxGetLocalTimeMillis();
			Gauge->AddGauge(_("Running Process"), IdProcessGauge, StatusBarWithGaugeClass::INDETERMINATE_MODE, 0);	
		}
	}
	else if (Process) {
		
		// dont use Process::GetPid(), it seems to always return zero.
		// http://forums.wxwidgets.org/viewtopic.php?t=13559
		int err = wxProcess::Kill(CurrentPid, wxSIGTERM);
		if (err != 0) {
			wxMessageBox(wxString::Format(wxT("Could not kill process %ld. Error (%d)"), CurrentPid, err));
		}
		Command->Enable(true);
		RunButton->SetLabel(_("Start"));
	}
}

void  mvceditor::RunConsolePanelClass::OnClear(wxCommandEvent& event) {
	OutputWindow->ChangeValue(wxT(""));
}

void  mvceditor::RunConsolePanelClass::OnTimer(wxTimerEvent& event) {
	wxWakeUpIdle();
}

void  mvceditor::RunConsolePanelClass::OnIdle(wxIdleEvent& event) {
	wxLongLong now = wxGetLocalTimeMillis();
	if ((now - LastPulse) > 250) {
		Gauge->IncrementGauge(IdProcessGauge, StatusBarWithGaugeClass::INDETERMINATE_MODE);
		LastPulse = now;
	}
	GetProcessOutput();
	
	// only request more idle events when there is more input to read.
	// be very careful about accessing Process pointer, it may be deleted
	// when the process ends.
	if (Process) {
		event.RequestMore(Process->IsInputAvailable() || Process->IsErrorAvailable());
	}
}

void  mvceditor::RunConsolePanelClass::OnEndProcess(wxProcessEvent& event) { 
	GetProcessOutput();
	Timer.Stop();
	Gauge->StopGauge(IdProcessGauge);
	delete Process;
	Process = NULL;
	CurrentPid = 0;
	//OutputWindow->AppendText(
	//	wxT("\n-------------------------------------------------------------\nProcess terminated at ") + 
	//	wxDateTime::Now().Format() +
	//	wxT("\n-------------------------------------------------------------\n"));
	Command->Enable(true);
	RunButton->SetLabel(_("Run"));
}

void  mvceditor::RunConsolePanelClass::GetProcessOutput() {
	if (Process && Process->GetInputStream()) {
		wxString output;
		while (Process && Process->IsInputAvailable()) {
			char ch = Process->GetInputStream()->GetC();
			if (isprint(ch) || isspace(ch)) {
				output.Append(ch);
			}
		}
		
		// if no output, do not append.  This will allow the user the ability to select the text when the process is silent
		if (!output.IsEmpty()) {
			OutputWindow->AppendText(output);
		}
	}
	if (Process && Process->GetErrorStream()) {
		wxString output;
		while (Process && Process->IsErrorAvailable()) {
			char ch = Process->GetErrorStream()->GetC();
			if (isprint(ch) || isspace(ch)) {
				output.Append(ch);
			}
		}
		
		// if no output, do not append.  This will allow the user the ability to select the text when the process is silent
		if (!output.IsEmpty()) {
			OutputWindow->AppendText(output);
		}
	}	
}

mvceditor::RunConsolePluginClass::RunConsolePluginClass()
	: PluginClass()
	, RunCliMenuItem(NULL)
	, RunCliWithArgsMenuItem(NULL)
	, RunCliInNewWindowMenuItem(NULL)
	, RunCliWithArgsInNewWindowMenuItem(NULL) {
}

void mvceditor::RunConsolePluginClass::AddToolsMenuItems(wxMenu* toolsMenu) {
	RunCliMenuItem = new wxMenuItem(toolsMenu, ID_MENU_RUN_CLI, _("Run As CLI\tF7"), 
		_("Run File As a PHP Command Line Script"), wxITEM_NORMAL);
	toolsMenu->Append(RunCliMenuItem);
	RunCliWithArgsMenuItem = new wxMenuItem(toolsMenu, ID_MENU_RUN_CLI_ARGS, _("Run As CLI With Arguments\tSHIFT+F7"), 
		_("Run File As a PHP Command Line Script With Arguments"), wxITEM_NORMAL);
	toolsMenu->Append(RunCliWithArgsMenuItem);
	RunCliInNewWindowMenuItem = new wxMenuItem(toolsMenu, ID_MENU_RUN_CLI_IN_NEW_WINDOW, _("Run As CLI In New Window\tCTRL+F7"), 
		_("Run File As a PHP Command Line Script In a New Window"), wxITEM_NORMAL);
	toolsMenu->Append(RunCliInNewWindowMenuItem);
	RunCliWithArgsInNewWindowMenuItem = new wxMenuItem(toolsMenu, ID_MENU_RUN_CLI_IN_NEW_WINDOW_ARGS, 
		_("Run As CLI In New Window With Arguments\tCTRL+SHIFT+F7"), 
		_("Run File As a PHP Command Line Script In a New Window With Arguments"), wxITEM_NORMAL);
	toolsMenu->Append(RunCliWithArgsInNewWindowMenuItem);
}

void mvceditor::RunConsolePluginClass::OnRunFileAsCli(wxCommandEvent& event) {
	CodeControlClass* code = GetCurrentCodeControl();
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
			if (ID_MENU_RUN_CLI_ARGS != event.GetId()) {
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
				if (ID_MENU_RUN_CLI_ARGS != event.GetId()) {
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
			if (ID_MENU_RUN_CLI_IN_NEW_WINDOW_ARGS != event.GetId()) {
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
}