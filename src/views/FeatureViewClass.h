/**
 * @copyright  2015 Roberto Perpuly
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
#ifndef SRC_VIEWS_FEATUREVIEWCLASS_H_
#define SRC_VIEWS_FEATUREVIEWCLASS_H_

#include <wx/aui/aui.h>
#include <wx/event.h>
#include <map>
#include <vector>
#include "globals/FileTypeClass.h"
#include "main_frame/PreferencesClass.h"

namespace t4p {
// forward declaration, defined in another file
class StatusBarWithGaugeClass;
class NotebookClass;
class CodeControlClass;
class CodeControlEventClass;

/**
 * A feature view encapsulates the GUI of the application; it
 * has access to the main frame, the tools, outline, and code notebooks,
 * the status bar gauge, and menus.
 *
 *
 * Window locations: Left (skinny window), Bottom (wide window), Main (Center). Features can create
 * as many windows as they see fit (in any location).
 *
 * Actions: Feature can define actions, menu items for those actions,
 *          Also, context menu items for those actions, feature will be given the selected text
 *
 * Toolbars: Features will be given the ability to define toolbar buttons or other controls, either
 * left-justified or right-justified.
 *
 * Feature views will be given:
 * 1) StatusBarWithGaugeClass to show progress to the user
 * 2) The tools window notebook.  features can add their windows
 * 3) The code notebook.  Features can query which is the currently opened file, any
 *    selected text, etc...
 * 4) Note that unless specified, all pointers that a feature view is given (Notebook, status
 *    bar, etc.. ) will be taken care of by the application and the feature SHOULD NOT delete
 *    them.  However, any pointers created by the feature view will need to be deleted
 *    by the feature itself.  wxWindow pointers usually do not need to be deleted
 *    because the wxWidgets framework manages them.
 *
 * Lifecycle:
 * There is at most 1 instance of any one feature view class, however there
 * may be zero instances in Mac OS X where an application may be running without
 * a main frame.
 *
 * This class is meant to be inherited by all classes that desire to add
 * dialogs, panels, or menu items to the app. They should contain very little
 * application logic; that should be delegated to other classes.
 */
class FeatureViewClass : public wxEvtHandler {
	public:
	FeatureViewClass();

	/**
	 * Set the windows. All of these pointers will NOT be
	 * owned by this class. The caller will still retain ownership.
	 *
	 * @param StatusBarWithGaugeClass& statusBarWithGauge the status bar.
	 * @param NotebookClass& notebook the opened source code files
	 * @param wxAuiNotebook& toolsNotebook the parent window for all feature windows
	 * @param wxAuiNotebook& outlineNotebook the parent window for all outline type windows (left side)
	 * @param wxAuiManager auiManager the AUI manager used to update the frame
	 * @param wxMenuBar* menuBar the application menu bar
	 * @param wxToolBar* toolBar the application tool bar
	 */
	void InitWindow(StatusBarWithGaugeClass* statusBarWithGauge,
		wxAuiNotebook* toolsNotebook,
		wxAuiNotebook* outlineNotebook, wxAuiManager* auiManager,
		wxMenuBar* menuBar, wxAuiToolBar* toolBar);

	/**
	 * Add menu items to the view menu for this feature. Remeber to use the MenuIds enum when building
	 * menu items.
	 *
	 * @param wxMenu* menu the view menu to add items to.
	 */
	virtual void AddViewMenuItems(wxMenu* viewMenu);

	/**
	 * Add menu items to the search menu for this feature. Remeber to use the MenuIds enum when building
	 * menu items.
	 *
	 * @param wxMenu* menu the view menu to add items to.
	 */
	virtual void AddSearchMenuItems(wxMenu* searchMenu);

	/**
	 * Add menu items to the file menu for this feature. Remeber to use the MenuIds enum when building
	 * menu items.
	 *
	 * @param wxMenu* menu the tools menu to add items to.
	 */
	virtual void AddFileMenuItems(wxMenu* fileMenu);

	/**
	 * Add menu items to the edit menu for this feature. Remeber to use the MenuIds enum when building
	 * menu items.
	 *
	 * @param wxMenu* menu the tools menu to add items to.
	 */
	virtual void AddEditMenuItems(wxMenu* editMenu);

	/**
	 * Add menu items to the help menu for this feature. Remeber to use the MenuIds enum when building
	 * menu items.
	 *
	 * @param wxMenu* menu the tools menu to add items to.
	 */
	virtual void AddHelpMenuItems(wxMenu* helpMenu);

	/**
	 * Feature may create its own menu. The feature should override this method if it desires to create an entirely new menu.
	 * Remeber to use the MenuIds enum when building
	 * menu items.
	 * @param wxMenuBar* the menu bar to insert the new menu to
	 */
	virtual void AddNewMenu(wxMenuBar* menuBar);

	/**
	 * Adds items to the toolbar.  These items will be left aligned.
	 */
	virtual void AddToolBarItems(wxAuiToolBar* toolBar);

	/**
	 * Adds an arbritary window to the application. Use the AuiManager property  (AuiManager.AddPane) to add
	 * items. No need to call AuiManager.Update(), the application will do it.
	 */
	virtual void AddWindows();

	/**
	 * Add a tab to the preferences window. This method is invoked only when the user chooses Edit ... Preferences
	 *
	 * @param wxBookCtrlBase* the parent that will contain all preference dialogs.  Once the feature's window is added, the
	 * parent will take care of deletion. Note that to add a dialog you will need to call wxBookCtrlBase::AddPage
	 */
	virtual void AddPreferenceWindow(wxBookCtrlBase* parent);

	/**
	 * This method will be called every time the user right-clicks on an active code control window. Features may define
	 * special menu items  to be shown to the user when the user right-clicks on a code control. The active code control
	 * can then be accessed via the NotebookClass.
	 *
	 * @param wxMenu* menu the context menu to add items to.
	 */
	virtual void AddCodeControlClassContextMenuItems(wxMenu* menu);

	/**
	 * Subclasses can override this method to create their own shortcuts that will get serialized /deserialized
	 * properly; also by using this method the shortcuts will get registered properly; plus it will allow the user to
	 * edit the shortcuts via the preferences dialog.
	 * @param shortcuts the list of shortcuts to add to
	 */
	virtual void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);

	/**
	 * Opens an existing file, or if the file is already opened, just sets it to be
	 * the active page.
	 *
	 * A word about error handling:
	 * The full path must exist; if not then the user will be shown a warning message
	 * about the file not existing.
	 *
	 * @param fullPath the full path of the file to open
	 *
	 */
	void LoadCodeControl(const wxString& fullPath);

	/**
	 * Search all opened code controls for the code control that contains the
	 * contents of the given file.
	 *
	 * @param fullPath full path of the file to look for
	 * @return CodeControlClass* the code control that contains the contents of
	 *         the given file, or NULL if the file is not open.
	 */
	t4p::CodeControlClass* FindCodeControl(const wxString& fullPath) const;

	/**
	 * @return all of the opened code controls. Be very careful with
	 *         these pointers, as the user can close files at any time
	 *         and the pointers will be deleted.  You should not store
	 *         the pointers at all.
	 */
	std::vector<t4p::CodeControlClass*> AllCodeControls() const;

	/**
	 * @return vector containing full paths to all of the files that are opened.
	 *         If the editor has opened buffers that are not yet saved to a file,
	 *         this method will also return those; this means that the returned vector
	 *         may contain strings that are not valid full paths.
	 */
	std::vector<wxString> AllOpenedFiles() const;

	/**
	 * Get the currently selected code control.  This is not necessarily the
	 * window that has focus, as in wxWindow::FindFocus(). We
	 * cannot ask which code control has focus because we may
	 * want to know the LAST code control to have focus in certain cases;
	 * for example if the user clicked on a toolbar button, the focus will
	 * be on the toolbar button, but what we really want to know is which
	 * code control had focus before the user clicked the button.
	 *
	 * Be very careful with the returned pointer, as the user can close files at any time
	 * and the pointers will be deleted.  You should not store
 	 * the pointer at all.
	 *
	 * @return CodeControlClass* the code control that has or last had focus; can be NULL
	 *         if there are no files currently open.
	 */
	CodeControlClass* GetCurrentCodeControl() const;

	/**
	 * Get the currently selected code control along with its notebook. The same rules as
	 * GetCurrentCodeControl() apply. This method will set the notebook and code control pointers to
	 * the out parameters.
	 *
	 * Be very careful with the returned pointers, as the user can close files at any time
	 * and the pointers will be deleted.  You should not store these pointers at all.
	 *
	 * @param [out] CodeControlClass* the code control that has or last had focus; can be NULL
	 * @param [out] NotebookClass* the notebook that is the parent of the code control that has
	 * focus; can be NULL
	 * @return bool TRUE if we could get the code control that is being focused on or last had focus. If TRUE, then
	 *         the codeCtrl and notebook params will be written to and will be valid.
	 */
	bool GetCurrentCodeControlWithNotebook(t4p::CodeControlClass** codeCtrl, t4p::NotebookClass** notebook) const;

	/**
	 * tell the app to close the given code control immediately.  The code control
	 * will be removed from its notebook, and its contents will be cleared from
	 * memory.
	 *
	 * This method should only be called after the user has verified that its
	 * OK to close, since the contents are not saved.
	 *
	 * @param codeCtrl the code control to close
	 */
	void CloseCodeControl(t4p::CodeControlClass* codeCtrl);

	protected:
	/**
	 * This is a helper method that will add each of the given menu items as a
	 * shortcut.  The map will contain the menu Item IDs; each of these IDs will
	 * be used to lookup the Menu Item in the Menu Bar, and a DynamicCmd will be
	 * created based on the menu item. The map value (wxString) will be used as
	 * the DynamicCmd's identifier.
	 * For example, if the map contains
	 *
	 *   menuItems[wxID_OPEN] = "Open-File"
	 *   menuItems[wxID_CLOSE] = "Close-File"
	 *
	 * Then this method will create 2 DynamicCmds, assuming that the menu bar has
	 * menu items with the IDs wxID_OPEN and wxID_CLOSE.  "Open-File" will be the
	 * identifier for the first command and "Close-File" will be the identifier for
	 * the second command.
	 * If a menu item is not found, and assertion is triggered.
	 */
	void AddDynamicCmd(std::map<int, wxString> menuItemIds, std::vector<DynamicCmdClass>& shortcuts);

	/**
	 * Finds the tools window with the given window ID and returns it.
	 *
	 * @param int windowId the window ID to look for
	 * @return wxWindow* the window, could be NULL if window was not found
	 */
	wxWindow* FindToolsWindow(int windowId) const;

		/**
	 * Finds the outline window with the given window ID and returns it.
	 *
	 * @param int windowId the window ID to look for
	 * @return wxWindow* the window, could be NULL if window was not found
	 */
	wxWindow* FindOutlineWindow(int windowId) const;

	/**
	 * Check to see if the given window is the tools window that's currently active
	 * @param int windowId
	 * @return bool true if the window with the given windowId is the selected (active) tools window.
	 */
	bool IsToolsWindowSelected(int windowId) const;

	/**
	 * Check to see if the given window is the tools window that's currently active
	 * This method is useful when a feature creates multiple tools windows
	 *
	 * @param wxString name the window name to check (wxWindow::GetName())
	 * @return bool true if the selected (active) tools window's name is equal to the given name.
	 */
	bool IsToolsWindowSelectedByName(const wxString& name) const;

	/**
	 * Check to see if the given window is the outline window that's currently active
	 * @param int windowId
	 * @return bool true if the window with the given windowId is the selected (active) outline window.
	 */
	bool IsOutlineWindowSelected(int windowId) const;

	/**
	 * Returns the top-level window.
	 *
	 * @return wxWindow* the main window
	 */
	wxWindow* GetMainWindow() const;

		/**
	 * Do NOT delete the pointer
	 * @return wxAuiNotebook* the parent of all tool windows. guaranteed to be not null.
	 */
	wxAuiNotebook* GetToolsNotebook() const;

	/**
	 * Do NOT delete the pointer
	 * @return wxAuiNotebook* the parent of all outline windows. guaranteed to be not null.
	 */
	wxAuiNotebook* GetOutlineNotebook() const;

	/**
	 * Set the given page to be the selected page for the tools notebook
	 * @param wxWindow the window that the tools notebook will be visible
	 */
	void SetFocusToToolsWindow(wxWindow* window);

	/**
	 * Set the given page to be the selected page for the outline notebook
	 * @param wxWindow the window that the tools notebook will be visible
	 */
	void SetFocusToOutlineWindow(wxWindow* window);

	/**
	 * Add a window to the tool notebook. Once added, this class will take care of memory management for the window pointer.
	 *
	 * @param wxWindow* window the window to add
	 * @param wxString tabName the name that will show up in the window's tab
	 * @param wxString windowName the name that will be given to the window (wxWindow::SetName)
	 * @param bitmap the image that gets place in the notebook tab for this window
	 * @return bool true if window was added.
	 */
	bool AddToolsWindow(wxWindow* window, wxString tabName, wxString windowName = wxEmptyString, const wxBitmap& bitmap = wxNullBitmap);

	/**
	 * Add a window to the outline notebook. Once added, this class will take care of memory management for the window pointer.
	 *
	 * @param wxWindow* window the window to add
	 * @param wxString name the name that will show up in the window's tab
	 * @param bitmap the image that gets place in the notebook tab for this window
	 * @return bool true if window was added.
	 */
	bool AddOutlineWindow(wxWindow* window, wxString name, const wxBitmap& bitmap = wxNullBitmap);

	/**
	 * Creates a new code control that is primed with the global editor
	 * options. code control will be tied to the application code Notebook.
	 *
	 * @param tabName the name that will go on the tab of the new page
	 *        This can be empty; if empty then a default message is shown
	 * @param mode the document mode that the control will be editing
	 * @return CodeControlClass* this class will own the pointer, DONT delete it
	 */
	CodeControlClass* CreateCodeControl(const wxString& tabName, t4p::FileType type) const;

	/**
	 * @param codeCtrl the code control to get the text for
	 * @return the text that is in the notebook tab for this code control
	 *         empty string is the given code control is not part of the
	 *         code notebook.
	 */
	 wxString GetCodeNotebookTabText(t4p::CodeControlClass* codeCtrl);

	/**
	 * Search all opened code controls for the code control that contains the
	 * contents of the given file. If the code control is found, it is focused
	 * on.
	 *
	 * @param fullPath full path of the file to look for
	 * @return CodeControlClass* the code control that contains the contents of
	 *         the given file, or NULL if the file is not open.
	 */
	t4p::CodeControlClass* FindCodeControlAndSelect(const wxString& fullPath) const;


	/**
	 * Returns the text that's currently selected in the currently active code control.
	 *
	 * @retun wxString the selected text.  If no text is selected, or there is not opened code control,
	 *        an empty string is returned
	 */
	wxString GetSelectedText() const;

	/**
	 * The status bar that the feature can use to display a gauge to the user. Do NOT delete the pointer.
	 *
	 * @return StatusBarWithGaugeClass*
	 */
	StatusBarWithGaugeClass* GetStatusBarWithGauge() const;

	/**
	 * The AUI Manager is needed in cases where the different windows are repositioned programatically and the entire AUI
	 * needs to be re-drawn.
	 *
	 * @var wxAuiManager*
	 */
	wxAuiManager* AuiManager;

	/**
	 * The Application-wide menu bar.
	 */
	wxMenuBar* MenuBar;

	/**
	 * The Application-wide tool bar.
	 */
	wxAuiToolBar* ToolBar;

	private:
	/**
	 * The widget that feature uses to display status to the user.
	 *
	 * @var StatusBarWithGaugeClass*
	 */
	StatusBarWithGaugeClass* StatusBarWithGauge;

	/**
	 * Parent container that will hold all features' tools windows.
	 *
	 * @var wxAuiNotebook*
	 */
	wxAuiNotebook* ToolsNotebook;

	/**
	 * Parent container that will hold all features' outline windows.
	 *
	 * @var wxAuiNotebook*
	 */
	wxAuiNotebook* OutlineNotebook;

	/**
	 * This class will not own this pointer. This pointer
	 * may be invalid for a little while, as the user closes
	 * a code control.
	 *
	 * @var t4p::CodeControlClass the code control that has focus.
	 */
	t4p::CodeControlClass* CurrentFocusCodeControl;

	/**
	 * Keep track of the code control that has focus. We
	 * cannot ask which code control has focus because we may
	 * want to know the LAST code control to have focus in certain cases;
	 * for example if the user clicked on a toolbar button, the focus will
	 * be on the toolbar button, but what we really want to know is which
	 * code control had focus before the user clicked the button.
	 *
	 * @param event
	 */
	void OnAppFilePageChanged(t4p::CodeControlEventClass& event);

	DECLARE_EVENT_TABLE()
};
}  // namespace t4p

#endif  // SRC_VIEWS_FEATUREVIEWCLASS_H_
