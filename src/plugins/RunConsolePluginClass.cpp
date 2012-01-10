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
static const int ID_WINDOW_CONSOLE = wxNewId();

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
	, RunCliWithArgsInNewWindowMenuItem(NULL) {
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
}

void mvceditor::RunConsolePluginClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_RUN_PHP + 0] = wxT("Console-Run");
	menuItemIds[mvceditor::MENU_RUN_PHP + 1] = wxT("Console-Run With Arguments");
	menuItemIds[mvceditor::MENU_RUN_PHP + 2] = wxT("Console-Run In New Window");
	menuItemIds[mvceditor::MENU_RUN_PHP + 3] = wxT("Console-Run-In New Window With Arguments");
	AddDynamicCmd(menuItemIds, shortcuts);
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
	EVT_TOOL(ID_TOOLBAR_RUN, mvceditor::RunConsolePluginClass::OnRunFileAsCli)
	EVT_UPDATE_UI(wxID_ANY, mvceditor::RunConsolePluginClass::OnUpdateUi)
END_EVENT_TABLE()