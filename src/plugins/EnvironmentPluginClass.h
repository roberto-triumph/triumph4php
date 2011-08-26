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
 * @author     $Author: robertop2004@gmail.com $
 * @date       $Date: 2011-06-10 17:19:31 -0700 (Fri, 10 Jun 2011) $
 * @version    $Rev: 555 $ 
 */
#ifndef __environmentpluginclass__
#define __environmentpluginclass__

#include <PluginClass.h>
#include <environment/EnvironmentClass.h>
#include <plugins/wxformbuilder/EnvironmentGeneratedDialogClass.h>

namespace mvceditor {
	
class EnvironmentDialogClass : public EnvironmentGeneratedDialogClass {

protected:
	// Handlers for EnvironmentGeneratedDialogClass events.
	void OnScanButton(wxCommandEvent& event);
	void OnOkButton(wxCommandEvent& event);

public:
	/** Constructor */
	EnvironmentDialogClass(wxWindow* parent, EnvironmentClass& environment);

private:
	
	/** 
	 * The configuration class
	 * 
	 * @var EnvironmentClass
	 */
	EnvironmentClass& Environment;
	
	/**
	 * Used to recurse through file system directories.
	 * 
	 * @var DirectorySearch
	 */
	DirectorySearchClass DirectorySearch;
	
	/**
	 * populate the dialog according to the ApacheClass settings
	 */
	void Populate();
	
	/**
	 * When idle and user clicked the search button, do the search
	 */
	void OnIdle(wxIdleEvent& event);
	
	/**
	 * Handle the file picker changed event.
	 */
	void OnPhpFileChanged(wxFileDirPickerEvent& event);
	
	enum States {
		FREE = 0,
		SEARCHING,
		OPENING_FILE
	};
	
	/**
	 * The current state.  We will be searching through directories in the same main application thread instead of
	 * another thread. a whole new thread is not really warranted in this case.
	 */
	States State;
	
	DECLARE_EVENT_TABLE()
};

/**
 * This plugin will handle the application stack configuration options.
 */
class EnvironmentPluginClass : public PluginClass {

public:

	EnvironmentPluginClass();
	
	/**
	 * Add the environment sub-menu.
	 * 
	 * @return wxMenuBar* the menu bar to add to.
	 */
	virtual void AddProjectMenuItems(wxMenu* projectMenu);
	
	/**
	 * Handle the menu item
	 */
	void OnMenuEnvironment(wxCommandEvent& event);
	
private:
	
	DECLARE_EVENT_TABLE()
};

}
#endif // __environmentpluginclass__
