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
#ifndef __T4P_FEATURECLASS_H__
#define __T4P_FEATURECLASS_H__
 
#include <globals/EnvironmentClass.h>
#include <globals/ProjectClass.h>
#include <widgets/NotebookClass.h>
#include <widgets/StatusBarWithGaugeClass.h>
#include <main_frame/PreferencesClass.h>
#include <wx/event.h>
#include <wx/propdlg.h>
#include <vector>

namespace t4p {
	
// forward declaration to prevent #include of the AppClass
class AppClass;

// forward declaration to prevent #include of the TagCacheClass
class TagCacheClass;

/**
 * ATTN: Use this enum to build the Feature menus
 * Since wxKeyBinder uses menu IDs to serialize the shortcuts, the menu IDs
 * cannot change (else KeyBinder will not bind the shortcut; or worse it may
 * crash.  For now we will use this enum to make sure the IDs are consistent
 * across app versions; and to prevent ID collisions. 
 * Be sure to NOT modify the enum values; enum items may be added
 * but make sure to not change the previous values, items may be deleted but if they are
 * do NOT change the values of the remanining items; else menu shortcuts
 * will not work (and the app may crash).
 * At some point, a more 'dynamic' shortcut system will need to be written,  since
 * wxKeyBinder does not handle dynamic menus well.
 */
enum MenuIds {
	// start way out here to prevent collisions with default menus
	// and action IDs ID_EVENT_ACTION_*
	MENU_START = wxID_HIGHEST + 10000,
	MENU_FINDER = wxID_HIGHEST + 10000,
	MENU_FIND_IN_FILES = wxID_HIGHEST + 10100,
	MENU_SQL = wxID_HIGHEST + 10200,
	MENU_RUN_PHP = wxID_HIGHEST + 10300,
	MENU_RESOURCE = wxID_HIGHEST + 10400,
	MENU_PROJECT = wxID_HIGHEST + 10500,
	MENU_OUTLINE = wxID_HIGHEST + 10600,
	MENU_LINT_PHP = wxID_HIGHEST + 10700,
	MENU_ENVIRONMENT = wxID_HIGHEST + 10800,
	MENU_EDITOR_MESSAGES = wxID_HIGHEST + 10900,
	MENU_CODE_IGNITER = wxID_HIGHEST + 11000,
	MENU_RUN_BROWSER = wxID_HIGHEST + 11100,
	MENU_TEMPLATE_FILES = wxID_HIGHEST + 11200,
	MENU_RECENT_FILES = wxID_HIGHEST + 11300,
	MENU_DETECTORS = wxID_HIGHEST + 11400,
	CONFIG_DETECTORS = wxID_HIGHEST + 11500,
	MENU_EXPLORER = wxID_HIGHEST + 11600,
	MENU_VERSION_UPDATE = wxID_HIGHEST + 11700,
	MENU_TOTAL_SEARCH = wxID_HIGHEST + 11800,
	MENU_DOC_COMMENT = wxID_HIGHEST + 11900,
	MENU_BEHAVIOR = wxID_HIGHEST + 12000,
	MENU_CHANGELOG = wxID_HIGHEST + 12100,
	MENU_BOOKMARK =  wxID_HIGHEST + 12200,
	MENU_END = wxID_HIGHEST + 12300
};

/**
 * Feature examples:
 * SVN
 * Text modifiers (up case, lowercase, code beautifiers)
 * Run script
 * File operations Save/Save As../Revert
 * Find
 * Find In Files
 * Project Errors (PHP lint)
 * TODOs 
 * 'Outline' view
 * Log viewer (mysql, apache, php error, other?)
 * Test (PHPUnit, Simple test)
 * PECL installer
 * 
 * 
 * Window locations: Left (skinny window), Bottom (wide window), Main (Center). Features can create as many windows
 * as they see fit (maybe in any location??).
 * 
 * Actions: Feature can define actions, menu items for those actions, 
 *          Also, context menu items for those actions, feature will be given the selected text
 * 
 * Toolbars: Features will be given the ability to define toolbar buttons or other controls, either left-justified or right-justified.
 * 
 * 
 * Features will be given:
 * 1) The GlobalsClass that holds all application-wide data structures. 
 * 2) StatusBarWithGaugeClass to show progress to the user
 * 3) The tools window notebook.  features can add their windows
 * 4) The code notebook.  Features can query which is the currently opened file, any selected text, etc...
 * 5) An instance of Environment class; contains the apache configuration that is installed in the system. Note that the 
 *    Apache class may not be initialized to point at a proper config file.
 * 6) Note that unless specified, all pointers that a feature is given (Notebook, status bar, etc.. )will be taken care of by the 
 *    application and the feature SHOULD NOT delete them.  However, any pointers created by the feature will need to be deleted
 *    by the feature itself.  wxWindow pointers usually do not need to be deleted because the wxWidgets framework manages them.
 *  
 * Lifecycle:
 *  + One instance of feature per application.  features are created during program startup and deleted during application exit.
 *  + During startup, AddMenuItems method is called. The feature can override this method to create menu items.
 *    Features inherit from wxEvtHandler, so event tables or Connect method may be used to listen for menu or any other window) events
 *
 * Events:
 * The application has an "Event Sink", any interesting events are posted to the event sink. These can be
 * editor events (like, for example, when a file has been opened, when settings have changed, etc).  Also,
 * ** some ** window events are put in the event sink; all menu events, toolbar events and some AUI events.
 * All features are automatically added as handles for the event sink; what this all means is that a feature
 * can implement menu items and toolbar buttons by adding event table entries as if it were the main
 * frame. 
 * The event sink is a two-way communication, features can send commands to the event sink or they can
 * handle commands from other features as well.
 *
 * See EventSinkClass for more info.
 * 
 */
class FeatureClass : public wxEvtHandler {

public:

	
	/**
	 * The application event dispatcher. Also holds all 'global' structures and settings.
	 */
	AppClass& App;

	/**
	 * Constructor
	 * @param app the handler that may receive events from features.
	 * Note that at the start, any window pointers are not yet initialized
 	 */
	FeatureClass(AppClass& app);
	
	/**
	 * Destructor.  May be overidden by sub classes.
	 */
	virtual ~FeatureClass();

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
	 */
	void InitWindow(StatusBarWithGaugeClass* statusBarWithGauge, NotebookClass* notebook, wxAuiNotebook* toolsNotebook, 
		wxAuiNotebook* outlineNotebook, wxAuiManager* auiManager, wxMenuBar* menuBar);
		
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
	 * This method will be called during application startup; the feature should load the preferences from persistent 
	 * storage (confg) here
	 * 
	 * @param wxConfigBase* the config where settings are stored.
	 */
	virtual void LoadPreferences(wxConfigBase* config);
	
	/**
	 * Subclasses can override this method to create their own shortcuts that will get serialized /deserialized
	 * properly; also by using this method the shortcuts will get registered properly; plus it will allow the user to
	 * edit the shortcuts via the preferences dialog.
	 * @param shortcuts the list of shortcuts to add to
	 */
	virtual void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);
	
protected:
	
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
	 * Get the currently selected code control. This may be NULL if the editor's content pane is
	 * focused on something other than a code control.
	 * 
	 * @return CodeControlClass* the code control that has focus; can be NULL
	 */
	CodeControlClass* GetCurrentCodeControl() const;
	
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
	 * The source code control notebook. Guaranteed to be not null.
	 * 
	 * @return NotebookClass*
	 */
	NotebookClass* GetNotebook() const;

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
	 * The application. Has the environment of the computer (apache, ...)
	 * 
	 * @return EnvironmentClass
	 */
	 EnvironmentClass* GetEnvironment();
	 
	 /**
	  * Creates a new code control that is primed with the global editor
	  * options. code control will be tied to the application code Notebook.
	  * 
	  * @param tabName the name that will go on the tab of the new page
	  *        This can be empty; if empty then a default message is shown
	  * @param mode the document mode that the control will be editing
	  * @return CodeControlClass* this class will own the pointer, DONT delete it
	  */
	 CodeControlClass* CreateCodeControl(const wxString& tabName, t4p::CodeControlClass::Mode mode) const;

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
	void AddDynamicCmd(std::map<int, wxString> menuItemIds,std::vector<DynamicCmdClass>& shortcuts);
	 
	/**
	 * The AUI Manager is needed in cases where the different windows are repositioned programatically and the entire AUI
	 * needs to be re-drawn.
	 * 
	 * @var wxAuiManager*
	 */
	wxAuiManager* AuiManager;

	private:
	
	/**
	 * The widget that feature uses to display status to the user.
	 * 
	 * @var StatusBarWithGaugeClass*
	 */
	StatusBarWithGaugeClass* StatusBarWithGauge;

	/**
	 * The container for the source code windows.
	 * 
	 * @var NotebookClass*
	 */
	NotebookClass* Notebook;
	
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
	 * The Application-wide menu bar.
	 */
	wxMenuBar* MenuBar;
	
};

}

#endif