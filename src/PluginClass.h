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
#include <widgets/NotebookClass.h>
#include <widgets/StatusBarWithGaugeClass.h>
#include <wx/event.h>
#include <wx/propdlg.h>
#include <vector>

namespace mvceditor {

/**
 * These are events that will be published to all plugins.  Plugins may connect
 * and listen for these events.
 *
 * This event will be dispatched when a file is saved.  
 * event.GetString() will cotain the full path to the file.
 * event.GetEventObject() will contain a pointer to the code control window that
 *                        was saved.
 */
extern const wxEventType EVENT_PLUGIN_FILE_SAVED;

 /**
  * These events can be published by plugins; the application 
  * will listen for these events and act accordingly.
  */

/**
 * Tell the app to open a new project.
 * The command event should set the project root path with event.SetString()
 * Note that the app will do NOTHING if the path is invalid; the plugin should
 * make sure the path is valid.
 */
extern const wxEventType EVENT_APP_OPEN_PROJECT;

/**
 * Tell the app to save its state to the file system, and will 
 * also repaint any windows that are affected by the changes.
 */
extern const wxEventType EVENT_APP_SAVE_PREFERENCES;

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
	 * Set the windows
	 * 
	 * @param StatusBarWithGaugeClass& statusBarWithGauge the status bar.
	 * @param NotebookClass& notebook the opened source code files
	 * @param wxAuiNotebook& toolsNotebook the parent window for all plugin windows
	 * @param wxAuiManager auiManager the AUI manager used to update the frame
	 */
	void InitWindow(StatusBarWithGaugeClass* statusBarWithGauge, NotebookClass* notebook, wxAuiNotebook* toolsNotebook, 
		wxAuiManager* auiManager);
	
	/**
	 * Initialize application state
	 * 
	 * @param EnvironmentClass* the PHP app environment options. The pointer will NOT be owned by this class.
	 * @param appHandler event handler that may receive events from plugins. The pointer will NOT be owned by this class.
	 */
	void InitState(EnvironmentClass* environment, wxEvtHandler* appHandler);
		
	/**
	 * Add menu items to the tools menu for this plugin.
	 * 
	 * @param wxMenu* menu the tools menu to add items to.
	 */
	virtual void AddToolsMenuItems(wxMenu* toolsMenu);

	/**
	 * Add menu items to the edit menu for this plugin.
	 * 
	 * @param wxMenu* menu the tools menu to add items to.
	 */
	virtual void AddEditMenuItems(wxMenu* editMenu);
	
	/**
	 * Plugin may create its own menu. The plugin should override this method if it desires to create an entirely new menu.
	 * 
	 * @param wxMenuBar* the menu bar to insert the new menu to
	 */
	virtual void AddNewMenu(wxMenuBar* menuBar);
	
	/**
	 * Add menu items to the project menu for this plugin.
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
	 * This method is called whenever a project is opened. The project can be accessed via the GetProject() method. NOte
	 * that this method may be called at a time where the winodowing system may not yet be initialized.
	 */
	virtual void OnProjectOpened();
	
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
	
	/**
	 * Use this method instead of wxNewId() to get a unque ID for Menu items. If you use wxNewId() for menu items then
	 * they will not be stored properly during saving of the keyboard shortcut preferences. You only need to use 
	 * this method on menus that are placed in the Application MenuBar.
	 */
	static int newMenuId();
	
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
	 * Get the currently visible content pane. This may return NULL if the notebook is empty.
	 * 
	 * @return wxWindow* do not DELETE the returned pointer 
	 */
	wxWindow* GetCurrentContentPane() const;
	
	/**
	 * Do NOT delete the pointer
	 * @return wxWindow* the parent of all tool windows
	 */
	wxWindow* GetToolsParentWindow() const;
	
	/**
	 * The source code control notebook
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
	 EnvironmentClass* GetEnvironment() const;
	 
	 /**
	  * Creates a new code control without a parent but it is primed with the global editor
	  * options.
	  * 
	  * @param wxWindow* parent window
	  * @param int flags window creation flags
	  * @return CodeControlClass* caller will own the pointer
	  */
	 CodeControlClass* CreateCodeControl(wxWindow* parent, int flags) const;

	 /**
	  * Send an event to the application.  See above for possible events.
	  */
	void AppEvent(wxCommandEvent event);
	 
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
	wxEvtHandler* AppHandler;
	
	/**
	 * The current opened project. The pointer will NOT be owned by this class.
	 * 
	 * @var ProjectClass*
	 */	
	ProjectClass* Project;
	
	/**
	 * The development stack. The pointer will NOT be owned by this class.
	 * 
	 * @var EnvironmentClass
	 */
	EnvironmentClass* Environment;
	
	static int CurrentMenuId;
};

}
#endif