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
#ifndef CODEIGNITER_PLUGIN_CLASS_H
#define CODEIGNITER_PLUGIN_CLASS_H

#include <PluginClass.h>
#include <php_frameworks/ProjectClass.h>
#include <php_frameworks/FrameworkDetectorClass.h>

namespace mvceditor {

/**
 * This is a plugin that is designed to test the plugin mechanism. 
 * This is also useful for prototyping of a feature or debugging (instead of
 * repeating steps just put the code in the Go() method )
 */
class CodeIgniterPluginClass : public PluginClass {
public:

	CodeIgniterPluginClass();
		
	/**
	 * This plugin will create its own menu. 
	 *
	 * @param wxMenuBar* the menu bar to insert the new menu to
	 */
	virtual void AddNewMenu(wxMenuBar* menuBar);

	/**
	 * Add shortcuts to the config files.
	 */
	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);
	
private:	

	/**
	 * When a project is opened; we will read the list of project's config files
	 * and will populate the code igniter menu with the config file names.
	 */
	void OnProjectOpened(wxCommandEvent& event);

	void OnMenuItem(wxCommandEvent& event);
	
	/**
	 * adds the entries in ConfigFiles to the menu.
	 */
	void UpdateMenu();

	/**
	 * This is the list of config files that were parsed from the result
	 * of the detection command.
	 * The key is user-friendly name; the value is the absolute path to the
	 * corresponding config file.
	 */
	std::map<wxString, wxString> ConfigFiles;

	/**
	 * The menu to add the config file entries to.
	 * This class will not own this pointer.
	 */
	wxMenu* CodeIgniterMenu;

	/**
	 * Saving the menu bar so that the code igniter menu can be removed when a project is not
	 * a code igniter project.
	 */
	wxMenuBar* MenuBar;
	
	DECLARE_EVENT_TABLE()
};

}

#endif