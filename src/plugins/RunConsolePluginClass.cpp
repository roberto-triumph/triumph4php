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
#include <MvcEditorString.h>
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

wxString mvceditor::CliCommandClass::CmdLine() const {
	wxString line;
	if (!Executable.IsEmpty()) {
		line += Executable;
	}
	if (!Arguments.IsEmpty()) {
		line += wxT(" ") + Arguments;
	}
	return line;
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
		"The executable is the program that will be run.\n"
		"The arguments are the entire string of arguments that will be given to the executable.\n"
		"The description is a bit of text that will be displayed in the \n"
		"toolbars for this command. \n"
		"If 'Wait For Arguments' is checked, then when this command is \n"
		"run a new console will be opened but the command will not be \n"
		"automatically run, you can then enter in run time arguments in the command. "
		"If 'Show In Toolbar' is checked, then this saved command will appear in the\n"
		"bottom toolbar, enabling you to run it with one mouse click."
	);
	help = wxGetTranslation(help);
	wxMessageBox(help, _("CLI Command Help"), wxOK, this);
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

void mvceditor::CliCommandListDialogClass::OnHelpButton(wxCommandEvent& event) {
	wxString help = wxString::FromAscii(
		"This dialog shows the list of all saved commands.\n"
		"You can edit a command by double-cliking on it or by clicking the \n"
		"Edit button. Note that the commands appear in the toolbar in the order \n"
		"shown. Re-ordering the commands by clicking the 'up' or 'down' buttons \n"
		"will affect the order of the command buttons in the toolbar."
	);
	help = wxGetTranslation(help);
	wxMessageBox(help, _("CLI Command Help"), wxOK, this);
}

mvceditor::RunConsolePanelClass::RunConsolePanelClass(wxWindow* parent, int id, 
													   mvceditor::StatusBarWithGaugeClass* gauge, 
													   mvceditor::RunConsolePluginClass& plugin)
	: RunConsolePanelGeneratedClass(parent, id)
	, CommandString()
	, ProcessWithHeartbeat(*this)
	, Gauge(gauge)
	, Plugin(plugin)
	, FileNameHits()
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

void  mvceditor::RunConsolePanelClass::SetToRunCommand(const mvceditor::CliCommandClass& command) {
	
	// command is a file name, lets run it through PHP
	// cannot run new files that have not been saved yet
	wxString cmdLine = command.CmdLine();
	if (!cmdLine.empty()) {
		CommandString = cmdLine;
		TransferDataToWindow();

		// if user chose the 'with arguments' then do not proceed let the user put in arguments
		if (command.WaitForArguments) {
			Command->SetInsertionPointEnd();
			Command->SetFocus();
		}
		else {
			wxCommandEvent evt;
			RunCommand(evt);
		}
	}
	else {
		CommandString = wxT("");
		wxMessageBox(_("PHP script needs to be saved in order to run it."));
	}
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
	FileNameHits.clear();
}

void  mvceditor::RunConsolePanelClass::OnProcessFailed(wxCommandEvent& event) {
	wxString output = event.GetString();

	// if no output, do not append.  This will allow the user the ability to select the text when the process is silent
	if (!output.IsEmpty()) {
		AppendText(output);
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
		AppendText(output);
	}
	Gauge->IncrementGauge(IdProcessGauge, StatusBarWithGaugeClass::INDETERMINATE_MODE);
}

void  mvceditor::RunConsolePanelClass::OnProcessComplete(wxCommandEvent& event) { 
	wxString output = event.GetString();

	// if no output, do not append.  This will allow the user the ability to select the text when the process is silent
	if (!output.IsEmpty()) {
		AppendText(output);
	}
	Gauge->StopGauge(IdProcessGauge);
	CurrentPid = 0;
	Command->Enable(true);
	RunButton->SetLabel(_("Run"));
}

void mvceditor::RunConsolePanelClass::OnStoreButton(wxCommandEvent& event) {
	mvceditor::CliCommandClass newCommand;
	wxString fullLine = Command->GetValue();
	fullLine.Trim();
	if (!fullLine.IsEmpty()) {
		int spacePos = fullLine.Find(wxT(" "));
		if (spacePos != wxNOT_FOUND) {
			newCommand.Executable = fullLine.Mid(0, spacePos);
			newCommand.Arguments = fullLine.Mid(spacePos + 1);
			newCommand.ShowInToolbar = false;
			newCommand.WaitForArguments = false;
			
			// prime the description with the base name in case
			// executable is a full path
			size_t pos = newCommand.Executable.Length();
			for (; pos > 0; pos--) {
				if (newCommand.Executable[pos] == wxT('/') || newCommand.Executable[pos] == wxT('\\')) {

					// don't include the slash
					pos++;
					break;
				}
			}
			newCommand.Description = newCommand.Executable.Mid(pos);
		}
		else {
			newCommand.Executable = fullLine;
		}

		// using NULL so that the dialog is centered on the screen
		mvceditor::CliCommandEditDialogClass dialog(NULL, wxID_ANY, newCommand);
		if (dialog.ShowModal() == wxID_OK) {
			Plugin.AddCommand(newCommand);
			Plugin.PersistCommands();
		}
	}
}

wxString mvceditor::RunConsolePanelClass::GetCommand() const {
	return Command->GetValue();
}

void mvceditor::RunConsolePanelClass::AppendText(const wxString& text) {
	mvceditor::FinderClass finder;
	finder.Mode = mvceditor::FinderClass::REGULAR_EXPRESSION;
	finder.CaseSensitive = false;
	finder.Expression = FileNameRegularExpression();
	
	// this way so that gcc does not think that good is an unused variable
	bool prep;
	prep = finder.Prepare();
	wxASSERT(prep);
	
	UnicodeString uniText = mvceditor::WxToIcu(text);
	int32_t totalLength = uniText.length();

	wxFont regularFont = OutputWindow->GetFont();
	regularFont.SetUnderlined(false);
	wxTextAttr normalAttr(*wxBLACK, wxNullColour, regularFont);

	wxFont underlinedFont = OutputWindow->GetFont();
	underlinedFont.SetUnderlined(true);
	wxTextAttr fileAttr(*wxBLUE, wxNullColour, underlinedFont);

	int32_t index = 0;
	while (index >= 0 && index < totalLength) {
		mvceditor::FileNameHitClass hit;
		if (finder.FindNext(uniText, index) && finder.GetLastMatch(hit.StartIndex, hit.Length)) {
			if (hit.StartIndex > index) {
			
				// render the text prior to the hit as normal
				OutputWindow->SetDefaultStyle(normalAttr);
				UnicodeString beforeHit(uniText, index, hit.StartIndex - index);
				OutputWindow->AppendText(mvceditor::IcuToWx(beforeHit));
			}
			wxString hitTrimmed = text.Mid(hit.StartIndex, hit.Length);
			wxString hitContents = text.Mid(hit.StartIndex, hit.Length);

			// render the hit. a final check to make sure the hit is an actual file
			// trim because the regular expression may contain a space at the beginning or the end
			hitTrimmed.Trim(false).Trim(true);
			if (wxFileName::FileExists(hitTrimmed)) {
				OutputWindow->SetDefaultStyle(fileAttr);
				
				// store it so that we can use it during mouse hover
				FileNameHits.push_back(hit);
			}
			else {
				OutputWindow->SetDefaultStyle(normalAttr);
			}
			OutputWindow->AppendText(hitContents);
			index = hit.StartIndex + hit.Length + 1;

			if ((hit.StartIndex + hit.Length) < totalLength) {
				
				// render the ending boundary that was taken by the regular expression
				OutputWindow->SetDefaultStyle(normalAttr);
				UnicodeString afterHit(uniText, hit.StartIndex + hit.Length, 1);
				OutputWindow->AppendText(mvceditor::IcuToWx(afterHit));
			}
		}
		else {
			OutputWindow->SetDefaultStyle(normalAttr);
			UnicodeString noHit(uniText, index);
			OutputWindow->AppendText(mvceditor::IcuToWx(noHit));
			index = -1;
		}
	}
}

UnicodeString mvceditor::RunConsolePanelClass::FileNameRegularExpression() {
	wxPlatformInfo info;
	std::vector<wxString> allExtensions;
	std::vector<wxString> phpExtensions = Plugin.App.Structs.GetPhpFileExtensions();
	std::vector<wxString> cssExtensions = Plugin.App.Structs.GetCssFileExtensions();
	std::vector<wxString> sqlExtensions = Plugin.App.Structs.GetSqlFileExtensions();
	for (size_t i = 0; i < phpExtensions.size(); ++i) {
		allExtensions.push_back(phpExtensions[i]);
	}
	for (size_t i = 0; i < cssExtensions.size(); ++i) {
		allExtensions.push_back(cssExtensions[i]);
	}
	for (size_t i = 0; i < sqlExtensions.size(); ++i) {
		allExtensions.push_back(sqlExtensions[i]);
	}
	wxString extensionsRegEx;
	for (size_t i = 0; i < allExtensions.size(); ++i) {
		
		// turn wilcards into proper regular expression syntax
		wxString ext = allExtensions[i];
		ext.Replace(wxT("."), wxT("\\."));
		ext.Replace(wxT("*"), wxT(".*"));
		ext.Replace(wxT("?"), wxT(".?"));
		extensionsRegEx += ext;
		if (i < (allExtensions.size() - 1)) {
			extensionsRegEx += wxT("|");
		}
	}
	extensionsRegEx = wxT("(") + extensionsRegEx  + wxT(")"); 
	
	UnicodeString uniRegEx = UNICODE_STRING_SIMPLE("(?mi)");
	if (info.GetOperatingSystemId() == wxOS_WINDOWS_NT) {
		
		// pattern C:\\folder1\\folder2
		// we need to escape backslashes so 1 escaped literal backslash=4 backslashes
		uniRegEx += UNICODE_STRING_SIMPLE("(^|\\s)");
		uniRegEx += UNICODE_STRING_SIMPLE("([A-Za-z]\\:[\\\\\\w_. ]+)\\\\");
		uniRegEx += mvceditor::WxToIcu(extensionsRegEx);
		uniRegEx += UNICODE_STRING_SIMPLE("($|\\s)"); 
		
	}
	else {
		uniRegEx += UNICODE_STRING_SIMPLE("(^|\\s)/");
		uniRegEx += UNICODE_STRING_SIMPLE("([\\w_. /]+)");
		uniRegEx += mvceditor::WxToIcu(extensionsRegEx);
		uniRegEx += UNICODE_STRING_SIMPLE("($|\\s)"); 
	}
	return uniRegEx;
}

void mvceditor::RunConsolePanelClass::OnMouseMotion(wxMouseEvent& event) {
	mvceditor::FileNameHitClass mouseHit = HitAt(event);
	bool isCursorHand = mouseHit.Length > 0;
	if (isCursorHand) {
		wxCursor cursor(wxCURSOR_HAND);
		OutputWindow->SetCursor(cursor);
	}
	else {
		OutputWindow->SetCursor(wxNullCursor);
		event.Skip();
	}
}

void mvceditor::RunConsolePanelClass::OnLeftDown(wxMouseEvent& event) {
	mvceditor::FileNameHitClass mouseHit = HitAt(event);
	if (mouseHit.Length > 0) {
		wxString output = OutputWindow->GetValue();
		wxString fileName = output.Mid(mouseHit.StartIndex, mouseHit.Length);

		// trim because the regular expression may contain a space at the beginning or the end
		fileName.Trim(false).Trim(true);
		if (wxFileName::FileExists(fileName)) {
			Plugin.LoadPage(fileName);
		}
	}
	event.Skip();
}

mvceditor::FileNameHitClass mvceditor::RunConsolePanelClass::HitAt(wxMouseEvent& event) {
	mvceditor::FileNameHitClass mouseHit;
	wxTextCoord x, y;
	wxTextCtrlHitTestResult result = OutputWindow->HitTest(event.GetPosition(), &x, &y);
	if (wxTE_HT_ON_TEXT == result) {
		int32_t pos = (int32_t)OutputWindow->XYToPosition(x, y);
		for (size_t i = 0; i < FileNameHits.size(); ++i) {
			mvceditor::FileNameHitClass hit = FileNameHits[i];
			if (pos > hit.StartIndex && pos < (hit.StartIndex + hit.Length)) {
				mouseHit = hit;
				break;
			}
		}
	}
	return mouseHit;
}

mvceditor::RunConsolePluginClass::RunConsolePluginClass(mvceditor::AppClass& app)
	: PluginClass(app)
	, RunCliMenuItem(NULL)
	, RunCliWithArgsMenuItem(NULL)
	, RunCliInNewWindowMenuItem(NULL)
	, RunCliWithArgsInNewWindowMenuItem(NULL) 
	, CommandToolbar(NULL) {
}

void mvceditor::RunConsolePluginClass::AddNewMenu(wxMenuBar* menuBar) {
	wxMenu* cliMenu = new wxMenu(0);
	RunCliMenuItem = new wxMenuItem(cliMenu, mvceditor::MENU_RUN_PHP + 0, _("Run As CLI\tF7"), 
		_("Run File As a PHP Command Line Script"), wxITEM_NORMAL);
	cliMenu->Append(RunCliMenuItem);
	RunCliWithArgsMenuItem = new wxMenuItem(cliMenu, mvceditor::MENU_RUN_PHP + 1, _("Run As CLI With Arguments\tSHIFT+F7"), 
		_("Run File As a PHP Command Line Script With Arguments"), wxITEM_NORMAL);
	cliMenu->Append(RunCliWithArgsMenuItem);
	RunCliInNewWindowMenuItem = new wxMenuItem(cliMenu, mvceditor::MENU_RUN_PHP + 2, _("Run As CLI In New Window\tCTRL+F7"), 
		_("Run File As a PHP Command Line Script In a New Window"), wxITEM_NORMAL);
	cliMenu->Append(RunCliInNewWindowMenuItem);
	RunCliWithArgsInNewWindowMenuItem = new wxMenuItem(cliMenu, mvceditor::MENU_RUN_PHP + 3, 
		_("Run As CLI In New Window With Arguments\tCTRL+SHIFT+F7"), 
		_("Run File As a PHP Command Line Script In a New Window With Arguments"), wxITEM_NORMAL);
	cliMenu->Append(RunCliWithArgsInNewWindowMenuItem);
	cliMenu->AppendSeparator();
	cliMenu->Append(mvceditor::MENU_RUN_PHP + 4, 
		_("Saved CLI Commands"),
		_("Open a dialog that shows the saved CLI commands"),
		wxITEM_NORMAL);
	menuBar->Append(cliMenu, _("CLI"));
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

		// cannot run new files that have not been saved yet
		if (!code->IsNew()) {
			mvceditor::CliCommandClass cmd;
			cmd.Executable = GetEnvironment()->Php.PhpExecutablePath;
			cmd.Arguments = code->GetFileName();
			cmd.WaitForArguments = (mvceditor::MENU_RUN_PHP + 1) == event.GetId();
			bool inNewWindow = false;

			// if theres a window already opened, just re-run the selected window.
			// make sure that the selected window is a run console panel. if we don't explictly
			// check the name, the program will crash. Because we are typecasting we 
			// need to be careful
			if (IsToolsWindowSelectedByName(wxT("mvceditor::RunConsolePanelClass"))) {
				int selection = GetToolsNotebook()->GetSelection();
				RunConsolePanelClass* runConsolePanel = (mvceditor::RunConsolePanelClass*)GetToolsNotebook()->GetPage(selection);
				inNewWindow = cmd.CmdLine() != runConsolePanel->GetCommand();
			}
			RunCommand(cmd, inNewWindow);
		}
		else {
			wxMessageBox(_("PHP script needs to be saved in order to run it."));
		}
	}	
}

void mvceditor::RunConsolePluginClass::OnRunFileAsCliInNewWindow(wxCommandEvent& event) {
	CodeControlClass* code = GetCurrentCodeControl();
	if (code) {

		// cannot run new files that have not been saved yet
		if (!code->IsNew()) {
			mvceditor::CliCommandClass cmd;
			cmd.Executable = GetEnvironment()->Php.PhpExecutablePath;
			cmd.Arguments = code->GetFileName();
			cmd.WaitForArguments = (mvceditor::MENU_RUN_PHP + 3) == event.GetId();
			RunCommand(cmd, true);
		}
		else {
			wxMessageBox(_("PHP script needs to be saved in order to run it."));
		}
	}
}

void mvceditor::RunConsolePluginClass::RunCommand(const mvceditor::CliCommandClass& command, bool inNewWindow) {
	if (inNewWindow) {
		RunConsolePanelClass* window = new RunConsolePanelClass(GetToolsNotebook(), ID_WINDOW_CONSOLE, 
			GetStatusBarWithGauge(), *this);

		// set the name so that we can know which window pointer can be safely cast this panel back to the RunConsolePanelClass
		if (AddToolsWindow(window, _("Run"), wxT("mvceditor::RunConsolePanelClass"))) {
			window->SetToRunCommand(command);
		}
	}
	else {

		// if theres a window already opened, just re-run the selected window.
		int selection = GetToolsNotebook()->GetSelection();
		RunConsolePanelClass* runConsolePanel = NULL;

		// make sure that the selected window is a run console panel. if we don't explictly
		// check the name, the program will crash.
		if (IsToolsWindowSelectedByName(wxT("mvceditor::RunConsolePanelClass"))) {
			runConsolePanel = (mvceditor::RunConsolePanelClass*)GetToolsNotebook()->GetPage(selection);
		}
		else {
			runConsolePanel = new RunConsolePanelClass(GetToolsNotebook(), ID_WINDOW_CONSOLE, 
				GetStatusBarWithGauge(), *this);
			
			// set the name so that we can know which window pointer can be safely cast this panel back to the RunConsolePanelClass
			AddToolsWindow(runConsolePanel, _("Run"), wxT("mvceditor::RunConsolePanelClass"));
		}
		if (runConsolePanel) {
			
			// window already created, will just re-run the command that's already there
			runConsolePanel->SetToRunCommand(command);
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
	FillCommandPanel();
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
	FillCommandPanel();
}

void mvceditor::RunConsolePluginClass::AddCommand(const mvceditor::CliCommandClass& command) {
	CliCommands.push_back(command);
}

void mvceditor::RunConsolePluginClass::FillCommandPanel() {
	bool added = false;
	if (CommandToolbar == NULL) {
		CommandToolbar = new wxAuiToolBar(GetMainWindow(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 
			  wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxAUI_TB_TEXT | wxAUI_TB_HORZ_TEXT);
		added = true;
		CommandToolbar->SetToolBitmapSize(wxSize(16,16));
		CommandToolbar->SetOverflowVisible(false);
	}
	else {
		AuiManager->DetachPane(CommandToolbar);
	}
	CommandToolbar->Clear();	
	for (size_t i = 0; i < CliCommands.size(); ++i) {
		mvceditor::CliCommandClass cmd = CliCommands[i];
		if (cmd.ShowInToolbar) {
			wxString desc = cmd.Description;

			// the ID is the index; we will use the index so that the
			// handler know which command to execute
			CommandToolbar->AddTool(mvceditor::MENU_RUN_PHP + 5 + i, desc, wxArtProvider::GetBitmap(
				wxART_EXECUTABLE_FILE, wxART_TOOLBAR, wxSize(16, 16)), _("Run in CLI Mode"), wxITEM_NORMAL);
		}
	}
	CommandToolbar->Realize();
	if (CommandToolbar->GetToolCount() > 0) {	
		wxAuiPaneInfo paneInfo;
		paneInfo.Bottom().Layer(2)
			.CaptionVisible(false).CloseButton(false)
			.Gripper(false).DockFixed(true).PaneBorder(false)
			.Floatable(false).Row(0).Position(0);
		AuiManager->AddPane(CommandToolbar, paneInfo);
	}
	AuiManager->Update();
}

void mvceditor::RunConsolePluginClass::OnCommandButtonClick(wxCommandEvent& event) {
	size_t index = (size_t)event.GetId();
	index = index - (mvceditor::MENU_RUN_PHP + 5);
	if (index >= 0 && index < CliCommands.size()) {
		mvceditor::CliCommandClass cmd = CliCommands[index];
		RunCommand(cmd, true);
	}
}

void mvceditor::RunConsolePluginClass::LoadPage(const wxString& fileName) {
	GetNotebook()->LoadPage(fileName);
}

mvceditor::FileNameHitClass::FileNameHitClass()
	: StartIndex(0)
	, Length(0) {

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

	// take up all the rest of the IDs for the command buttons
	EVT_MENU_RANGE(mvceditor::MENU_RUN_PHP + 5, mvceditor::MENU_RUN_PHP + 55, 
	mvceditor::RunConsolePluginClass::OnCommandButtonClick)
END_EVENT_TABLE()