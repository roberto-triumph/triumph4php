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
#ifndef __RunConsolePanelClass__
#define __RunConsolePanelClass__

/**
@file
Subclass of RunConsolePanelGeneratedClass, which is generated by wxFormBuilder.
*/

#include <plugins/wxformbuilder/RunConsolePluginGeneratedClass.h>
#include <PluginClass.h>
#include <widgets/ProcessWithHeartbeatClass.h>
#include <php_frameworks/FrameworkDetectorClass.h>
#include <vector>

/** Implementing RunConsolePanelGeneratedClass */

namespace mvceditor {

// defined below
class RunConsolePluginClass;

/**
 * Class that represents a command to be run in the 
 * 'console' (CLI).
 */
class CliCommandClass {

public:

	/**
	 * Binary to be run. This may be a full path, or just the
	 * binary name if the binary is already in the PATH.
	 */
	wxString Executable;

	/**
	 * all of the arguments to give to the executable
	 */
	wxString Arguments;

	/**
	 * A short human-friendly description of the command. This is
	 * given by the user; it is not guaranteed to be unique.
	 */
	wxString Description;

	/**
	 * if TRUE, then when the console window is created for this command
	 * the command will NOT be run. This gives the user a chance to
	 * add runtime arguments.
	 */
	bool WaitForArguments;

	/**
	 * if TRUE, this item will show up in the toolbar. Note that the order 
	 * is determined by the position in the list of commands.
	 */
	bool ShowInToolbar;
	
	CliCommandClass();

	void Copy(const CliCommandClass& src);

};

/**
 * This is the dialog where the user can edit a
 * run console command.
 */
class CliCommandEditDialogClass : public CliCommandEditDialogGeneratedClass {

public:

	CliCommandEditDialogClass(wxWindow* parent, int id, mvceditor::CliCommandClass& command);

protected:

	void OnOkButton(wxCommandEvent& event);
	void OnHelpButton(wxCommandEvent& event);
	void OnFileChanged(wxFileDirPickerEvent& event);

};

/**
 * This is the dialog where the user can re-order console commands, add, or remove
 * commands also.
 */
class CliCommandListDialogClass : public CliCommandListDialogGeneratedClass {

public:

	CliCommandListDialogClass(wxWindow* parent, int id, std::vector<mvceditor::CliCommandClass>& commands);


protected:

	// to manage the list of commands
	// UP & DOWN control the order of the commands
	// in the toolbar
	void OnUpButton(wxCommandEvent& event);
	void OnDownButton(wxCommandEvent& event);
	void OnAddButton(wxCommandEvent& event);
	void OnDeleteButton(wxCommandEvent& event);
	void OnOkButton(wxCommandEvent& event);
	void OnEditButton(wxCommandEvent& event);
	void OnListDoubleClick(wxCommandEvent& event);

private:

	/**
	 * List of commands that are bound to this dialog. They will only be 
	 * modified when the user clicks OK
	 */
	std::vector<mvceditor::CliCommandClass>& Commands;

	/**
	 * List of commands that the user modifies.
	 */
	std::vector<mvceditor::CliCommandClass> EditedCommands;

	
	/**
	 * populates the Commands listbox using the data 
	 * from Commands
	 */
	void FillList();
	
};
	
class RunConsolePanelClass : public RunConsolePanelGeneratedClass {
protected:
	
	/**
	 * Handle the clear button.
	 */
	void OnClear(wxCommandEvent& event);

public:
	
	/*
	 * @param parent the parent of this pane
	 * @param id window ID
	 * @param 
	 * @param gauge to show progress to the user. This class will NOT own the pointer.
	 * @param plugin the plugin, used to add commands
	 */
	RunConsolePanelClass(wxWindow* parent, int id, mvceditor::EnvironmentClass* environment,
		mvceditor::StatusBarWithGaugeClass* gauge, mvceditor::RunConsolePluginClass& plugin);

	/**
	 * Set to run the given PHP file
	 * 
	 * @param const wxString& fullPath full path to the PHP file to run
	 */
	void SetToRunFile(const wxString& fullPath);
	
	/**
	 * Handle the 'Run' button
	 */
	void RunCommand(wxCommandEvent& event);
	
	/**
	 * set the focus on the text input for the command.
	 */
	void SetFocusOnCommandText();

	/**
	 * when the page is closed perform cleanup. This means
	 * clean up the child process and any open gauges
	 */
	void OnPageClose(wxAuiNotebookEvent& evt);

	/**
	 * When the user clicks on the store button save the current command.
	 */
	void OnStoreButton(wxCommandEvent& event);

private:
		
	/**
	 * The command to run
	 */
	wxString CommandString;
	
	/**
	 * Used to run the process asynchronously
	 */
	ProcessWithHeartbeatClass ProcessWithHeartbeat;

	/**
	 * To get the File System location of PHP executable
	 * 
	 * @var EnvironmentClass*
	 */
	EnvironmentClass* Environment;

	/**
	 * To save the current command to be persisted.
	 */
	RunConsolePluginClass& Plugin;

	/**
	 * To show progress to the user.
	 */
	StatusBarWithGaugeClass* Gauge;
	
	/**
	 * the current PID of the async process.
	 */
	long int CurrentPid;
	
	/**
	 * Gauge id for this instance of RunConsole. Each run console will have its own gauge to update
	 * 
	 * @var int
	 */
	int IdProcessGauge;
	
	/**
	 * called when the process ends.
	 */
	void OnProcessComplete(wxCommandEvent& event);

	/**
	 * called when the process ends.
	 */
	void OnProcessFailed(wxCommandEvent& event);
	
	/**
	 * Check running process output. Will poll process output and display it.
	 * 
	 * @param wwCommandEvent& event
	 */
	void OnProcessInProgress(wxCommandEvent& event);
	
	DECLARE_EVENT_TABLE()
};

class RunConsolePluginClass : public PluginClass {

public:

	/**
	 * Constructor
 	 */
	RunConsolePluginClass();
	
	/**
	 * Add menu items
	 */
	void AddToolsMenuItems(wxMenu* toolsMenu);
	
	/**
	 * Add items to the toolbar
	 */
	void AddToolBarItems(wxAuiToolBar* toolBar);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);

	void LoadPreferences(wxConfigBase* config);

	/**
	 * Add a new command to the list (but does not persist
	 * the list to disk)
	 */
	void AddCommand(const mvceditor::CliCommandClass& command);

	/**
	 * Saves the CLI commands to the [global] config
	 */
	void PersistCommands();
	
private:
				
	/**
	 * handler for the menu
	 */
	void OnRunFileAsCli(wxCommandEvent& event);
	
	/**
	 * Always run in a new console window
	 */
	void OnRunFileAsCliInNewWindow(wxCommandEvent& event);
	
	/**
	 * disable menus when source code notebook is empty
	 */
	void OnUpdateUi(wxUpdateUIEvent& event);

	/**
	 * show the dialog CRUD for commands
	 */
	void OnRunSavedCommands(wxCommandEvent& event);

	/**
	 * The list of commands to be persisted.
	 */
	std::vector<mvceditor::CliCommandClass> CliCommands;
			
	/**
	 * Our menu items
	 */
	wxMenuItem* RunCliMenuItem;
	wxMenuItem* RunCliWithArgsMenuItem;
	wxMenuItem* RunCliInNewWindowMenuItem;
	wxMenuItem* RunCliWithArgsInNewWindowMenuItem;

	DECLARE_EVENT_TABLE()
};

}
#endif // __RunConsolePanelClass__
