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
#ifndef __mvceditorpluginclass__
#define __mvceditorpluginclass__
 
#include <environment/EnvironmentClass.h>
#include <php_frameworks/ProjectClass.h>
#include <php_frameworks/FrameworkDetectorClass.h>
#include <widgets/NotebookClass.h>
#include <widgets/ResourceCacheClass.h>
#include <widgets/StatusBarWithGaugeClass.h>
#include <PreferencesClass.h>
#include <wx/event.h>
#include <wx/propdlg.h>
#include <vector>

namespace mvceditor {
	
// forward declaration to prevent #include of the AppClass
class AppClass;

/**
 * ATTN: Use this enum to build the Plugin menus
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
	MENU_FINDER = 200,
	MENU_FIND_IN_FILES = 210,
	MENU_SQL = 220,
	MENU_RUN_PHP = 230,
	MENU_RESOURCE = 240,
	MENU_PROJECT = 250,
	MENU_OUTLINE = 260,
	MENU_LINT_PHP = 270,
	MENU_ENVIRONMENT = 280,
	MENU_EDITOR_MESSAGES = 290,
	MENU_CODE_IGNITER = 300
};

/**
 * Plugin examples:
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
 * Window locations: Left (skinny window), Bottom (wide window), Main (Center). Plugins can create as many windows
 * as they see fit (maybe in any location??).
 * 
 * Actions: Plugin can define actions, menu items for those actions, 
 *          Also, context menu items for those actions, plugin will be given the selected text
 * 
 * Toolbars: Plugins will be given the ability to define toolbar buttons or other controls, either left-justified or right-justified.
 * 
 * 
 * Plugins will be given:
 * 1) Project Class contains the current project 
 * 2) StatusBarWithGaugeClass to show progress to the user
 * 3) The tools window notebook.  plugins can add their windowx
 * 4) The code notebook.  Plugins can query which is the currently opened file, any selected text, etc...
 * 5) An instance of Environment class; contains the apache configuration that is installed in the system. Note that the 
 *    Apache class may not be initialized to point at a proper config file.
 * 6) Note that unless specified, all pointers that a plugin is given (Notebook, statubar, etc.. )will be taken care of by the 
 *    application and the plugin SHOULD NOT delete them.  However, any pointers created by the plugin will need to be deleted
 *    by the plugin itself.  Window pointers usually do not need to be deleted because of the wxWindgets framework.
 *  
 * Lifecycle:
 *  + One instance of plugin per application.  plugins are created during program startup and deleted during application exit.
 *  + During startup, AddMenuItems method is called. The plugin can override this method to create menu items.
 *    Plugins inherit from wxEvtHandler, so event tables or Connect method may be used to listen for menu or any other window) events.
 *  + When a project is opened, the plugin's setProject method gets called.
 * 
 */
class PluginClass : public wxEvtHandler {

public:

	/**
	 * Constructor
 	 */
	PluginClass();
	
	/**
	 * Destructor.  May be overidden by sub classes.
	 */
	virtual ~PluginClass();

	/**
	 * Set the windows. All of these pointers will NOT be
	 * owned by this class. The caller will still retain ownership.
	 * 
	 * @param StatusBarWithGaugeClass& statusBarWithGauge the status bar.
	 * @param NotebookClass& notebook the opened source code files
	 * @param wxAuiNotebook& toolsNotebook the parent window for all plugin windows
	 * @param wxAuiManager auiManager the AUI manager used to update the frame
	 * @param wxMenuBar* menuBar the application menu bar
	 */
	void InitWindow(StatusBarWithGaugeClass* statusBarWithGauge, NotebookClass* notebook, wxAuiNotebook* toolsNotebook, 
		wxAuiManager* auiManager, wxMenuBar* menuBar);
	
	/**
	 * Initialize application state
	 * 
	 * @param app the handler that may receive events from plugins. The pointer will NOT be owned by this class.
	 */
	void InitState(AppClass* app);
		
	/**
	 * Add menu items to the tools menu for this plugin. Remeber to use the MenuIds enum when building
	 * menu items.
	 * 
	 * @param wxMenu* menu the tools menu to add items to.
	 */
	virtual void AddToolsMenuItems(wxMenu* toolsMenu);

	/**
	 * Add menu items to the edit menu for this plugin. Remeber to use the MenuIds enum when building
	 * menu items.
	 * 
	 * @param wxMenu* menu the tools menu to add items to.
	 */
	virtual void AddEditMenuItems(wxMenu* editMenu);
	
	/**
	 * Plugin may create its own menu. The plugin should override this method if it desires to create an entirely new menu.
	 * Remeber to use the MenuIds enum when building
	 * menu items.
	 * @param wxMenuBar* the menu bar to insert the new menu to
	 */
	virtual void AddNewMenu(wxMenuBar* menuBar);
	
	/**
	 * Add menu items to the project menu for this plugin. Remeber to use the MenuIds enum when building
	 * menu items.
	 * 
	 * @param wxMenu* menu the tools menu to add items to.
	 */
	virtual void AddProjectMenuItems(wxMenu* projectMenu);

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
	 * @param wxBookCtrlBase* the parent that will contain all preference dialogs.  Once the plugin's window is added, the 
	 * parent will take care of deletion. Note that to add a dialog you will need to call wxBookCtrlBase::AddPage
	 */
	virtual void AddPreferenceWindow(wxBookCtrlBase* parent);
	
	/**
	 * This method will be called every time the user right-clicks on an active code control window. Plugins may define
	 * special menu items  to be shown to the user when the user right-clicks on a code control. The active code control 
	 * can then be accessed via the NotebookClass.
	 * 
	 * @param wxMenu* menu the context menu to add items to.
	 */
	virtual void AddCodeControlClassContextMenuItems(wxMenu* menu);
	
	/**
	 * This method will be called during application startup; the plugin should load the preferences from persistent 
	 * storage (confg) here
	 * 
	 * @param wxConfigBase* the config where settings are stored.
	 */
	virtual void LoadPreferences(wxConfigBase* config);
	
	/**
	 * This method will be called after the user changes the settings via Edit ... Preferences. the plugin should 
	 * save the preferences to persistent storage (confg) here
	 * 
	 * @param wxConfigBase* the config where settings should be stored.
	 */
	virtual void SavePreferences(wxConfigBase* config);
	
	/**
	 * This method is called whenever a project is opened. The project can be accessed via the GetProject() method. Note
	 * that this method may be called at a time where the winodowing system may not yet be initialized.
	 */
	virtual void OnProjectOpened();

	/**
	 * Subclasses can override this method to create their own shortcuts that will get serialized /deserialized
	 * properly; also by using this method the shortcuts will get registered properly; plus it will allow the user to
	 * edit the shortcuts via the preferences dialog.
	 * @param shortcuts the list of shortcuts to add to
	 */
	virtual void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);
	
	/**
	 * Set the plugin's reference to the given project. The caller will take care of memory management (this class should NOT
	 * delete the pointer).
	 * project can be NULL, in this case the previous project has been closed and a new one is about to be opened. In this
	 * case there will be 2 calls to SetProject() one for closing the project and one for opening the project.
	 */
	void SetProject(ProjectClass* project);
	
	/**
	 * The current opened project. Do NOT delete the pointer. Note that the Project may be NULL when there is no 
	 * open project.
	 * 
	 * @return ProjectClass*
	 */
	ProjectClass* GetProject() const;
	
protected:
	
	/**
	 * Add a window to the tool notebook. Once added, this class will take care of memory management for the window pointer.
	 * 
	 * @param wxWindow* window the window to add
	 * @param wxString name the name that will show up in the window's tab
	 * @return bool true if window was added.
	 */
	bool AddToolsWindow(wxWindow* window, wxString name);
	
	/**
	 * Finds the tools window with the given window ID and returns it.
	 * 
	 * @param int windowId the window ID to look for
	 * @return wxWindow* the window, could be NULL if window was not found
	 */
	wxWindow* FindToolsWindow(int windowId) const;
	
	/**
	 * Check to see if the given window is the tools window that's currently active
	 * @param int windowId
	 * @return bool true if the window with the given windowId is the selected (active) tools window.
	 */
	bool IsToolsWindowSelected(int windowId) const;
	
	/**
	 * Add a window to the 'main' content pane (the center pane). Once added, this class will take care of 
	 * memory management for the window pointer.
	 * 
	 * @param wxWindow any window
	 * @param wxString name the string that will show up in the tab
	 */
	bool AddContentWindow(wxWindow* window, const wxString& name);

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
	 * The source code control notebook. Guaranteed to be not null.
	 * 
	 * @return NotebookClass*
	 */
	NotebookClass* GetNotebook() const;

	/**
	 * The resource cache. Guaranteed to be not null. Do NOT delete the pointer
	 * 
	 * @return ResourceCacheClass*
	 */
	ResourceCacheClass* GetResourceCache();
	
	/**
	 * The detected framework resources
	 */
	PhpFrameworkDetectorClass& PhPFrameworks() const;
	
	/**
	 * Set the given page to be the selected page for the tools notebook
	 * @param wxWindow the window that the tools notebook will be visible
	 */
	void SetFocusToToolsWindow(wxWindow* window);
	
	/**
	 * Returns the text that's currently selected in the currently active code control.
	 * 
	 * @retun wxString the selected text.  If no text is selected, or there is not opened code control, 
	 *        an empty string is returned
	 */
	wxString GetSelectedText() const;
	
	/**
	 * The status bar that the plugin can use to display a gauge to the user. Do NOT delete the pointer.
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
	  * @return CodeControlClass* this class will own the pointer, DONT delete it
	  */
	 CodeControlClass* CreateCodeControl(const wxString& tabName) const;

	 /**
	  * Send an event to the application.  See above for possible events.
	  */
	void AppEvent(wxCommandEvent event);

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
	 * The widget that plugin uses to display status to the user.
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
	 * Parent container that will hold this plugin's windows.
	 * 
	 * @var wxAuiNotebook*
	 */
	wxAuiNotebook* ToolsNotebook;

	/**
	 * The application event dispatcher. The pointer will NOT be owned by this class.
	 */
	AppClass* App;
	
	/**
	 * The current opened project. The pointer will NOT be owned by this class.
	 * 
	 * @var ProjectClass*
	 */	
	ProjectClass* Project;

	/**
	 * The Application-wide menu bar.
	 */
	wxMenuBar* MenuBar;
	
};

extern const wxEventType EVENT_PLUGIN_FILE_SAVED;

/**
 * This is an event that will tell a plugin that a file has been saved.
 * The plugin will get a pointer to the CodeControl that was saved.
 * From the code control; the event handler can get the file name
 * or file contents if so desired.
 * Using a new event class as opposed to a wxCommandEvent because
 * wxCommandEvents propagate by default; but we do not want
 * these events to propagate.
 */
class FileSavedEventClass : public wxEvent {

public:

	/**
	 * @param codeControl caller will still own the pointer
	 */
	FileSavedEventClass(CodeControlClass* codeControl);

	/**
	 * @return the code control that was saved; do NOT delete the returned
	 * pointer.
	 */
	CodeControlClass* GetCodeControl() const;

	/** 
	 * required for sending with wxPostEvent()
	 */
    wxEvent* Clone() const;

private:

	CodeControlClass* CodeControl;
};

typedef void (wxEvtHandler::*FileSavedEventClassFunction)(FileSavedEventClass&);

#define EVT_PLUGIN_FILE_SAVED(fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_PLUGIN_FILE_SAVED, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( FileSavedEventClassFunction, & fn ), (wxObject *) NULL ),

}
#endif