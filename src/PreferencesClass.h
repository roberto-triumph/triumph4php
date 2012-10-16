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
#ifndef __preferencesclass__
#define __preferencesclass__

#include <wx/keybinder.h>
#include <wx/menuutils.h>
#include <code_control/CodeControlOptionsClass.h>

namespace mvceditor {

/**
 * This is a class that will MVC Editor will use to manage keyboard shortcuts.  
 * Default functionality of the Keybinder classes is not sufficient because
 * (1) MVC Editor menus are dynamic; they can be updated or removed when new 
 *     plugins
 * (2) MVC Editor menus can even change for different projects; we would like
 *     shortcuts to tolerant of menu items is not being attached to the menu bar.
 * (3) We want to de-couple the Menu from the shortcuts because default keybinder
 *     functionality uses Menu IDs; and Menu IDs in this app can change between
 *     app instances (depending on plugins and projects).
 */
class DynamicCmdClass {

public:

	/**
	 *
	 * @param item the menu item. This class will NOT own the pointer 
	 *        the menu item's ID need not be consistent across application
	 *        runs, but it must be unique (use wxNewId() function)
	 * @param identifier this is a string that is unique across the application.
	 * The menu item's label string will be used to populate the default shortcut.
	 * The menu item MUST live at least as long as this object.  For simplicity's
	 * sake, DynamicCmdClass instances will have application-wide scope, and the
	 * menu items should have application-wide scope also.
	 */
	DynamicCmdClass(wxMenuItem* item, const wxString& identifier);

	/**
	 * Add a key mapping to this command.  More than one can be added by calling this function
	 * multiple times.
	 * @param key the shortcut string to add (ie. CTRL+4, SHIFT+CTRL+P). See wxKeyBind for 
	 *  details on this string.
	 */
	void AddShortcut(const wxString& key);

	/**
	 * Copies this command's bindings.
	 * @return wxCmd* a NEW command pointer; memory management is left to the caller
	 */
	wxCmd* CloneCommand() const;

	/**
	 * clears all bindings. Usually this would be called when the bindings need to be changed due
	 * to user updates.
	 */
	void ClearShortcuts();

	/**
	 * @return the menu item's ID. This number is NOT safe to use for serialization
	 * across application runs, since the ID is not a compile-time value (IDs can
	 * be created by using wxNewId()).
	 */
	int GetId() const;

	/**
	 * @return the command's unque Identifier string. This string is safe to use for serialization
	 * across application runs.
	 */
	wxString GetIdentifier() const;

private:

	/**
	 * This is the binding; menu item-shortcut(s) combination.
	 */
	wxMenuCmd MenuCmd;

	/**
	 * String that uniquely identifies this shortcut
	 */
	wxString Identifier;
};

/**
 * This class is the composite set of all preferences/options that a user can change.
 */
class PreferencesClass {

public:

	/**
	 * Construct preferences. Creates on keyboard profile.
	 */
	PreferencesClass();
	
	/**
	 * Cleanup (delete) the keyboard profile pointers
	 */
	~PreferencesClass();

	/**
	 * initalizes the preferences
	 */
	void Init();
	
	/**
	 * Load the preferences from persistent storage.
	 * 
	 * @param wxConfigBase* config the config object to load info from
	 * @param wxFrame* we need a frame so we can attach keyboard listeners to it.
	 */
	void Load(wxConfigBase* config, wxFrame* frame);

	/**
	 * delete all of the saved shortcuts.  This would be done before we reload a config
	 * file that was changed externally.
	 */
	void ClearAllShortcuts();
	
	/**
	 * Save the preferences to persistent storage.
	 */
	void Save();

	/**
	* Apply the keyboard shortcuts to the window.  This process involves undoing any previous keyboard shortcuts
	* and attaching keyboard listeners to the given window. This m,ethod should get called whenver the keyboard
	* short cuts have changed; if this method does not get called then the shortcuts wont take effect.
	*/
	void EnableSelectedProfile(wxWindow* window);

	/**
	 * Set the config object where the settings are stored on wxConfigBase; after a call to InitConfig()
	 * any other source code can get the config object using wxConfigBase::Get
	 */
	static void InitConfig();
	
	/**
	 * The options for the source code editor
	 * 
	 * @var CodeControlOptionsClass
	 */
	CodeControlOptionsClass CodeControlOptions;

	/**
	 * The default keyboard shortcuts. These will never change; they will be used
	 * as a 'template' of sorts to create the multiple wxKeyProfile objects
	 * in the KeyProfiles array
	 * 
	 * @var std::vector<DynamicCmdClass>
	 */
	std::vector<DynamicCmdClass> DefaultKeyboardShortcutCmds;
	
	/**
	 * The keyboard shortcuts. Each 'set' consists of the default 
	 * shortcuts; but each set will have the same commands but may have 
	 * different key bindings for each command.  For example, "File-Open"
	 * command may be bound to "CTRL+O" on one set but it may be bound to
	 * "CTRL+SHIFT+O" on another set.
	 * 
	 * @var wxKeyProfileArray
	 */
	wxKeyProfileArray KeyProfiles;
};

}
#endif // __preferencesclass__
