/**
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
 */
#ifndef __RUNCONSOLEVIEWCLASS_H
#define __RUNCONSOLEVIEWCLASS_H

#include <views/wxformbuilder/RunConsoleFeatureForms.h>
#include <views/FeatureViewClass.h>
#include <features/RunConsoleFeatureClass.h>
#include <vector>

namespace t4p {
// forward declaration, defined below
class RunConsoleViewClass;

/**
 * This is the dialog where the user can edit a
 * run console command.
 */
class CliCommandEditDialogClass : public CliCommandEditDialogGeneratedClass {
	public:
	CliCommandEditDialogClass(wxWindow* parent, int id, t4p::CliCommandClass& command);

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
	CliCommandListDialogClass(wxWindow* parent, int id, std::vector<t4p::CliCommandClass>& commands);


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
	void OnHelpButton(wxCommandEvent& event);

	private:
	/**
	 * List of commands that are bound to this dialog. They will only be
	 * modified when the user clicks OK
	 */
	std::vector<t4p::CliCommandClass>& Commands;

	/**
	 * List of commands that the user modifies.
	 */
	std::vector<t4p::CliCommandClass> EditedCommands;

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
	 * @param gauge to show progress to the user. This class will NOT own the pointer.
	 * @param feature the feature, used to add commands
	 * @param view the view, used to add commands to the toolbar
	 */
	RunConsolePanelClass(wxWindow* parent, int id,
		t4p::StatusBarWithGaugeClass* gauge, t4p::RunConsoleFeatureClass& feature,
		t4p::RunConsoleViewClass& view);

	/**
	 * Set to run the given command
	 *
	 * @param cmdLine entire command line to run
	 * @param workingDirectory the startng directory where the command line runs
	 * @aparam waitForArgument if TRUE then
	 */
	void SetToRunCommand(const wxString& cmdLine, const wxFileName& workingDirectory, bool waitForArguments);

	/**
	 * Handle the 'Run' button
	 */
	void RunCommand(wxCommandEvent& event);

	/**
	 * when the page is closed perform cleanup. This means
	 * clean up the child process and any open gauges
	 */
	void OnPageClose(wxAuiNotebookEvent& evt);

	/**
	 * When the user clicks on the store button save the current command.
	 */
	void OnStoreButton(wxCommandEvent& event);

	/**
	 * @return the command that is being run or that the user
	 *  is typing in
	 */
	wxString GetCommand() const;

	private:
	/**
	 * The command to run
	 */
	wxString CommandString;

	/**
	 * The command's CWD / PWD
	 */
	wxFileName  WorkingDirectory;

	/**
	 * Used to run the process asynchronously
	 */
	ProcessWithHeartbeatClass ProcessWithHeartbeat;

	/**
	 * To show progress to the user.
	 */
	StatusBarWithGaugeClass* Gauge;

	/**
	 * To save the current command to be persisted.
	 */
	RunConsoleFeatureClass& Feature;

	/**
	 * To update the cli command toolbar after the commands
	 * are persisted
	 */
	RunConsoleViewClass& View;

	/**
	 * Save the hits so that we can traverse through them
	 */
	std::vector<t4p::FileNameHitClass> FileNameHits;

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

	/**
	 * @param text to append to the output. This method will format filenames
	 * so that they are distinguishable from other text.
	 */
	void AppendText(const wxString& text);

	/**
	 * @return the regular expression that will match a file name. The
	 * matche files will have PHP, CSS, or SQL extensions as allowed
	 * by the current project's file name wildcards.
	 */
	UnicodeString FileNameRegularExpression();

	/**
	 * on mouse events, check to see if the mouse is hovered over a file name
	 * if so, then change the cursor
	 */
	void OnMouseMotion(wxMouseEvent& event);

	/**
	 * Opens the file that was clicked on
	 */
	void OnLeftDown(wxMouseEvent& event);

	/**
	 * Get the filename hit from the mouse event
	 *
	 * @param evt the mouse position
	 * @return the filename hit that is located at the mouse position
	 */
	t4p::FileNameHitClass HitAt(wxMouseEvent& evt);

	DECLARE_EVENT_TABLE()
};

class RunConsoleViewClass : public FeatureViewClass {
	public:
	/**
	 * Constructor
 	 */
	RunConsoleViewClass(t4p::RunConsoleFeatureClass& feature);

	/**
	 * Add menu items
	 */
	void AddNewMenu(wxMenuBar* menuBar);

	/**
	 * Add items to the toolbar
	 */
	void AddToolBarItems(wxAuiToolBar* toolBar);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);

	/**
	 * add the bottom toolbar panel
	 */
	void AddWindows();

	/**
	 * synchronized the command panel buttons with those
	 * from the Commands list. Any buttons for the removed
	 * commands are removed and buttons for new commands are added.
	 */
	void FillCommandPanel();

	/**
	 * Open a file.
	 */
	void LoadPage(const wxString& fileName);

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
	 * @param command runs the given command and shows the output
	 *        panel. Note that the command may not actually start running
	 *        if waitForArguments is set.
	 * @param workingDirectory the command's CWD / PWD
	 * @param waitForAguments boolean if true then command will not run
	 *        if false, command will run right away
	 * @param inNewWindow, if TRUE then a new console output window will
	 * be created. Otherwise, the current output window may be used.
	 */
	void RunCommand(const wxString& cmdLine, const wxFileName& workingDirectory, bool waitForArguments, bool inNewWindow);

	// update the menu when files are loaded / closed
	void OnAppFileClosed(t4p::CodeControlEventClass& event);
	void OnAppFileOpened(t4p::CodeControlEventClass& event);
	void OnAppFileNew(t4p::CodeControlEventClass& event);

	/**
	 * disable menus when source code notebook is empty
	 */
	void MenuUpdate(bool isClosingPage);

	/**
	 * show the dialog CRUD for commands
	 */
	void OnRunSavedCommands(wxCommandEvent& event);

	/**
	 * When another feature wants to run a process; it will send a command
	 * event of type EVENT_CMD_RUN_COMMAND. This method will handle
	 * these events from other features and start the process.
	 */
	void OnAppCommandRun(wxCommandEvent& event);

	/**
	 * Handles the click of the saved command buttons
	 */
	void OnCommandButtonClick(wxCommandEvent& evt);

	/**
	 * The list of commands to be persisted.
	 */
	t4p::RunConsoleFeatureClass& Feature;

	/**
	 * Our menu items
	 */
	wxMenuItem* RunCliMenuItem;
	wxMenuItem* RunCliWithArgsMenuItem;
	wxMenuItem* RunCliInNewWindowMenuItem;
	wxMenuItem* RunCliWithArgsInNewWindowMenuItem;

	wxAuiToolBar* CommandToolbar;

	DECLARE_EVENT_TABLE()
};
}
#endif  // __RUNCONSOLEFEATURECLASS_H
