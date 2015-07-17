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
#ifndef T4P_FEATURECLASS_H
#define T4P_FEATURECLASS_H
 
#include <globals/EnvironmentClass.h>
#include <globals/ProjectClass.h>
#include <widgets/StatusBarWithGaugeClass.h>
#include <globals/GlobalsClass.h>
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
	MENU_DEBUGGER =  wxID_HIGHEST + 12300,
	MENU_FILE_CABINET = wxID_HIGHEST + 12400,
	MENU_NOTEBOOK_PANE = wxID_HIGHEST + 12500,
	MENU_FILE_OPERATIONS = wxID_HIGHEST + 12600,
	MENU_NAVIGATION = wxID_HIGHEST + 12700,
	MENU_END = wxID_HIGHEST + 12800,
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
	 * This method will be called during application startup; the feature should load the preferences from persistent 
	 * storage (confg) here
	 * 
	 * @param wxConfigBase* the config where settings are stored.
	 */
	virtual void LoadPreferences(wxConfigBase* config);

private:
	
};

}

#endif