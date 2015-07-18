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
 * @copyright  2015 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef FILEOPERATIONSVIEWCLASS_H
#define FILEOPERATIONSVIEWCLASS_H

#include <views/FeatureViewClass.h>
#include <features/FileOperationsFeatureClass.h>
#include <globals/Events.h>
#include <wx/stc/stc.h>

namespace t4p {

/**
 * The FileOperationsViewClass view handles the menu operations
 * for "macro" file operations: load, save, revert.
 */
class FileOperationsViewClass : public t4p::FeatureViewClass {

public:

    FileOperationsViewClass(t4p::FileOperationsFeatureClass& feature);

	void AddFileMenuItems(wxMenu* fileMenu);

	void AddToolBarItems(wxAuiToolBar* toolBar);

	void AddKeyboardShortcuts(std::vector<t4p::DynamicCmdClass>& cmds);

	private:

	/**
	 * Loads the given files into the application, one page for each file.
	 * File names must be fully qualified.
	 */
	void FileOpen(const std::vector<wxString>& filenames);

	/**
	 * Loads the given file into the application, and goes to the
	 * position. highlights to the given length
	 */
	void FileOpenPosition(const wxString& fullPath, int startingPos, int length);

	/**
	 * Loads the given file into the application, and goes to the
	 * given line. line number is 1 based.
	 */
	void FileOpenLine(const wxString& fullPath, int lineNumber);

	// menu event handlers
	void OnFileSave(wxCommandEvent& event);
	void OnFilePhpNew(wxCommandEvent& event);
	void OnFileSqlNew(wxCommandEvent& event);
	void OnFileCssNew(wxCommandEvent& event);
	void OnFileTextNew(wxCommandEvent& event);
	void OnFileRun(wxCommandEvent& event);
	void OnFileOpen(wxCommandEvent& event);
	void OnFileSaveAs(wxCommandEvent& event);
	void OnFileSaveAll(wxCommandEvent& event);
	void OnFileRevert(wxCommandEvent& event);
	void OnFileClose(wxCommandEvent& event);

	/**
	 * When a page is modified, enable the save button
	 */
	void EnableSave(wxStyledTextEvent& event);

	/**
	 * When a page is saved, disable the save button
	 */
	void DisableSave(wxStyledTextEvent& event);

	/**
	 * Save the currently active tab.
	 */
	void SaveCurrentFile(wxCommandEvent& event);

	/**
	 * repaint the status bar; cursor line position
	 */
	void UpdateStatusBar();

	/**
	 * Creates the toolbar buttons
	 */
	void CreateToolBarButtons();

	/**
	 * after the application starts, start the status bar timer.
	 * The status bar timer will update the status bar info (line, column)
	 * at specific intervals instead of in a EVT_STC_UPDATEUI event.
	 * Updating the status bar text triggers a refresh of the whole
	 * status bar (and it seems that the entire app is refreshed too)
	 * and it makes the app feel sluggish.
	 */
	void StartStatusBarTimer();

	/**
	 * at regular intervals, update the status bar text
	 */
	void OnStatusBarTimer(wxTimerEvent& event);

	/**
	 * Handler for the FILE_OPEN app command
	 * In this handler, a notebook tab is added
	 */
	void OnCmdFileOpen(t4p::OpenFileCommandEventClass& event);

	/**
	 * Toggle various widgets on or off based on the application state.
	 */
	void MenuUpdate(bool isClosingPage);

	/**
	 * Start the timer that will notify us when to update the status bar
	 */
	void OnAppReady(wxCommandEvent& event);

	/**
	 * When a code notebook page changes, it means that a new source
	 * code that is active.  In this event, we need to upate the
	 * various metrics (cursor position, dirty state)
	 */
	void OnAppFilePageChanged(t4p::CodeControlEventClass& event);

	/**
	 * When a code notebook page closes, it means that a new source
	 * code that is active.  In this event, we need to upate the
	 * various metrics (cursor position, dirty state)
	 */
	void OnAppFileClosed(t4p::CodeControlEventClass& event);

	// update the menu bars on these events
	void OnAppFileOpened(t4p::CodeControlEventClass& event);
	void OnAppFileNew(t4p::CodeControlEventClass& event);

	/**
	 * When the app frame is closed, check to see if there are "dirty"
	 * files that need to be saved.
	 */
	void OnAppFrameClose(wxNotifyEvent& event);

	/**
	 * @return bool TRUE if there is at least one opened file that needs
	 *         to be saved using an elevated (privilege / root) access.
	 */
	bool NeedsElevatedSave();

	t4p::FileOperationsFeatureClass& Feature;

	/**
	 * timer to update the status bar at a regular intervals
	 */
	wxTimer StatusBarTimer;

	DECLARE_EVENT_TABLE()
};

}

#endif
