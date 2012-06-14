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
#include <MvcEditor.h>
#include <wx/artprov.h>
#include <wx/filename.h>
#include <wx/sstream.h>
#include <wx/valgen.h>

static const int ID_PROCESS = wxNewId();
static const int ID_WINDOW_CONSOLE = wxNewId();

mvceditor::CliCommandClass::CliCommandClass()
	: Executable()
	, Arguments()
	, Description()
	, WaitForArguments(false)
	, ShowInToolbar(false) {
}

void mvceditor::CliCommandClass::Copy(const mvceditor::CliCommandClass& src) {
	Executable = src.Executable;
	Arguments = src.Arguments;
	Description = src.Description;
	WaitForArguments = src.WaitForArguments;
	ShowInToolbar = src.ShowInToolbar;
}

mvceditor::CliCommandEditDialogClass::CliCommandEditDialogClass(wxWindow* parent, int id, mvceditor::CliCommandClass& command)
	: CliCommandEditDialogGeneratedClass(parent, id) {
	wxGenericValidator executableValidator(&command.Executable);
	Executable->SetValidator(executableValidator);
	wxGenericValidator argumentsValidator(&command.Arguments);
	Arguments->SetValidator(argumentsValidator);
	wxGenericValidator descriptionValidator(&command.Description);
	Description->SetValidator(descriptionValidator);
	wxGenericValidator waitValidator(&command.WaitForArguments);
	WaitForArguments->SetValidator(waitValidator);
	wxGenericValidator showValidator(&command.ShowInToolbar);
	ShowInToolbar->SetValidator(showValidator);

	// in case executable contains a full path
	// we will allow Executable to contain relative binaries if they
	// already exist in the user's or system path
	if (wxFileName::FileExists(command.Executable)) {
		ExecutableFilePicker->SetPath(command.Executable);
	}
	TransferDataToWindow();
	Executable->SetFocus();

}
void mvceditor::CliCommandEditDialogClass::OnOkButton(wxCommandEvent& event) {
	if (TransferDataFromWindow()) {
		EndModal(wxID_OK);
	}
}

void mvceditor::CliCommandEditDialogClass::OnHelpButton(wxCommandEvent& event) {
	wxString help = wxString::FromAscii(
		"Store a command for future use.\n"
		"The command is the entire command line that will be executed.\n"
		"The description is a bit of text that will be displayed in the \n"
		"toolbars for this command. \n"
		"If 'Wait For Arguments' is checked, then when this command is \n"
		"run a new console will be opened but the command will not be \n"
		"automatically run, you can enter in run time arguments in the command. "
	);
	help = wxGetTranslation(help);
	wxMessageBox(help, _("Help"));
}

void mvceditor::CliCommandEditDialogClass::OnFileChanged(wxFileDirPickerEvent& event) {
	Executable->SetValue(event.GetPath());
}

mvceditor::CliCommandListDialogClass::CliCommandListDialogClass(wxWindow* parent, int id, std::vector<mvceditor::CliCommandClass>& commands)
	: CliCommandListDialogGeneratedClass(parent, id)	
	, Commands(commands)
	, EditedCommands(commands) {
	FillList();
}

void mvceditor::CliCommandListDialogClass::FillList() {
	for (size_t i = 0; i < EditedCommands.size(); ++i) {
		CommandsList->Append(EditedCommands[i].Description);
	}
	if (!EditedCommands.empty()) {
		CommandsList->SetSelection(0);
	}
}

void mvceditor::CliCommandListDialogClass::OnUpButton(wxCommandEvent& event) {
	size_t selection = (size_t) CommandsList->GetSelection();
	if (selection > 0 && selection < EditedCommands.size()) {
		mvceditor::CliCommandClass tmp = EditedCommands[selection];
		EditedCommands[selection] = EditedCommands[selection - 1];
		EditedCommands[selection - 1] = tmp;

		CommandsList->SetSelection(selection - 1);
		CommandsList->SetString(selection - 1, EditedCommands[selection - 1].Description);
		CommandsList->SetString(selection, EditedCommands[selection].Description);
	}
}

void mvceditor::CliCommandListDialogClass::OnDownButton(wxCommandEvent& event) {
	size_t selection = (size_t) CommandsList->GetSelection();
	if (selection >= 0 && selection < (EditedCommands.size() - 1)) {
		mvceditor::CliCommandClass tmp = EditedCommands[selection];
		EditedCommands[selection] = EditedCommands[selection + 1];
		EditedCommands[selection + 1] = tmp;

		CommandsList->SetSelection(selection + 1);
		CommandsList->SetString(selection + 1, EditedCommands[selection + 1].Description);
		CommandsList->SetString(selection, EditedCommands[selection].Description);
	}
}

void mvceditor::CliCommandListDialogClass::OnAddButton(wxCommandEvent& event) {
	mvceditor::CliCommandClass newCommand;
	mvceditor::CliCommandEditDialogClass dialog(this, wxID_ANY, newCommand);
	if (dialog.ShowModal() == wxID_OK) {
		EditedCommands.push_back(newCommand);
		CommandsList->Append(newCommand.Description);
		CommandsList->SetSelection(EditedCommands.size() - 1);
	}
}

void mvceditor::CliCommandListDialogClass::OnDeleteButton(wxCommandEvent& event) {
	size_t selection = (size_t) CommandsList->GetSelection();
	if (selection >= 0 && selection < EditedCommands.size()) {
		CommandsList->Delete(selection);
		EditedCommands.erase(EditedCommands.begin() + selection);
		
		if (selection >= EditedCommands.size() && !EditedCommands.empty()) {
			CommandsList->SetSelection(EditedCommands.size() - 1);
		}
		else if (selection >= 0 && !EditedCommands.empty()) {
			CommandsList->SetSelection(selection);
		}
	}
}

void mvceditor::CliCommandListDialogClass::OnListDoubleClick(wxCommandEvent& event) {
	size_t selection = (size_t) event.GetSelection();
	if (selection >= 0 && selection < EditedCommands.size()) {
		mvceditor::CliCommandClass cliCommand = EditedCommands[selection];
		mvceditor::CliCommandEditDialogClass dialog(this, wxID_ANY, cliCommand);
		if (dialog.ShowModal() == wxID_OK) {
			EditedCommands[selection] = cliCommand;
			CommandsList->SetString(selection, cliCommand.Description);
		}
	}
}

void mvceditor::CliCommandListDialogClass::OnEditButton(wxCommandEvent& event) {
	size_t selection = (size_t) CommandsList->GetSelection();
	if (selection >= 0 && selection < EditedCommands.size()) {
		mvceditor::CliCommandClass cliCommand = EditedCommands[selection];
		mvceditor::CliCommandEditDialogClass dialog(this, wxID_ANY, cliCommand);
		if (dialog.ShowModal() == wxID_OK) {
			EditedCommands[selection] = cliCommand;
			CommandsList->SetString(selection, cliCommand.Description);
		}
	}
}

void mvceditor::CliCommandListDialogClass::OnOkButton(wxCommandEvent& event) {
	Commands = EditedCommands;
	EndModal(wxID_OK);
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

			// stale PID??
			mvceditor::EditorLogError(mvceditor::ROGUE_PROCESS, 
				wxString::Format(wxT("Process ID: %ld."), CurrentPid));
		}
		
		Gauge->StopGauge(IdProcessGauge);
		CurrentPid = 0;
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
	toolsMenu->Append(mvceditor::MENU_RUN_PHP + 4, 
		_("Saved Commands"),
		_("Open a dialog that shows the saved commands"),
		wxITEM_NORMAL);
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
	
	// if theres a window already opened, just re-run the selected window.
	int selection = GetToolsNotebook()->GetSelection();
	RunConsolePanelClass* runConsolePanel = NULL;

	// make sure that the selected window is a run console panel. if we don't explictly
	// check the name, the program will crash.
	if (IsToolsWindowSelectedByName(wxT("mvceditor::RunConsolePanelClass"))) {
		runConsolePanel = (mvceditor::RunConsolePanelClass*)GetToolsNotebook()->GetPage(selection);
	}
	if (selection >= 0 && runConsolePanel) {
		
		// window already created, will just re-run the command that's already there
		runConsolePanel->SetFocusOnCommandText();
		
		// if user chose the 'with arguments' then do not proceed let the user put in arguments
		if ((mvceditor::MENU_RUN_PHP + 1) != event.GetId()) {
			runConsolePanel->RunCommand(event);	
		}
	}
	else if (code) {
		RunConsolePanelClass* runConsolePanel = new RunConsolePanelClass(GetToolsNotebook(), GetEnvironment(), 
			GetStatusBarWithGauge(), ID_WINDOW_CONSOLE);

		// set the name so that we can know which window pointer can be safely cast this panel back to the RunConsolePanelClass
		if (AddToolsWindow(runConsolePanel, _("Run"), wxT("mvceditor::RunConsolePanelClass"))) {
			runConsolePanel->SetToRunFile(code->GetFileName());
			runConsolePanel->SetFocusOnCommandText();
			
			// if user chose the 'with arguments' then do not proceed let the user put in arguments
			if ((mvceditor::MENU_RUN_PHP + 1) != event.GetId()) {
				runConsolePanel->RunCommand(event);
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
	toolBar->AddTool(mvceditor::MENU_RUN_PHP + 0, _("Run"), wxArtProvider::GetBitmap(
		wxART_EXECUTABLE_FILE, wxART_TOOLBAR, wxSize(16, 16)), _("Run"));
}

void mvceditor::RunConsolePluginClass::OnRunSavedCommands(wxCommandEvent& event) {
	mvceditor::CliCommandListDialogClass dialog(GetMainWindow(), wxID_ANY, CliCommands);
	if (dialog.ShowModal() == wxID_OK) {
		PersistCommands();
	}
}

void mvceditor::RunConsolePluginClass::LoadPreferences(wxConfigBase* config) {
	long index;
	wxString groupName;
	bool found = config->GetFirstGroup(groupName, index);
	while (found) {
		if (groupName.Find(wxT("CliCommand_")) >= 0) {
			mvceditor::CliCommandClass newCommand;
			wxString key = groupName + wxT("/Executable");
			newCommand.Executable = config->Read(key);
			key = groupName + wxT("/Arguments");
			newCommand.Arguments = config->Read(key);
			key = groupName + wxT("/Description");
			newCommand.Description = config->Read(key);
			key = groupName + wxT("/ShowInToolbar");
			config->Read(key, &newCommand.ShowInToolbar);
			key = groupName + wxT("/WaitForArguments");
			config->Read(key, &newCommand.WaitForArguments);

			CliCommands.push_back(newCommand);
		}
		found = config->GetNextGroup(groupName, index);
	}
}

void mvceditor::RunConsolePluginClass::PersistCommands() {
	wxConfigBase* config = wxConfig::Get();

	// delete any previous commands that are in the config
	wxString groupName;
	long index = 0;
	if (config->GetFirstGroup(groupName, index)) {
		do {
			if (groupName.Find(wxT("CliCommand_")) == 0) {
				config->DeleteGroup(groupName);
			}
		} while (config->GetNextGroup(groupName, index));
	}
	for (size_t i = 0; i < CliCommands.size(); ++i) {
		wxString key = wxString::Format(wxT("CliCommand_%d/Executable"), i);
		config->Write(key, CliCommands[i].Executable);
		key = wxString::Format(wxT("CliCommand_%d/Arguments"), i);
		config->Write(key, CliCommands[i].Arguments);
		key = wxString::Format(wxT("CliCommand_%d/Description"), i);
		config->Write(key, CliCommands[i].Description);
		key = wxString::Format(wxT("CliCommand_%d/ShowInToolbar"), i);
		config->Write(key, CliCommands[i].ShowInToolbar);
		key = wxString::Format(wxT("CliCommand_%d/WaitForArguments"), i);
		config->Write(key, CliCommands[i].WaitForArguments);
	}
	config->Flush();
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
	EVT_MENU(mvceditor::MENU_RUN_PHP + 4, mvceditor::RunConsolePluginClass::OnRunSavedCommands)
	EVT_UPDATE_UI(wxID_ANY, mvceditor::RunConsolePluginClass::OnUpdateUi)
END_EVENT_TABLE()