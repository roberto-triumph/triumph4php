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
#ifndef T4P_ConfigFilesFeatureViewClass_H__
#define T4P_ConfigFilesFeatureViewClass_H__

#include <features/views/FeatureViewClass.h>
#include <actions/ActionClass.h>

namespace t4p {

// forward declaration, defined in another file
class ConfigFilesFeatureClass;

/**
 * The ConfigFiles view will populate the menu of items
 * based on the config files that were parsed.  
 */
class ConfigFilesViewClass : public t4p::FeatureViewClass {

public:
	
	ConfigFilesViewClass(t4p::ConfigFilesFeatureClass& feature);
	
	void AddNewMenu(wxMenuBar* menuBar);

	private:
	
	/**
	 * rebuilds the config files menu based on the current config
	 * tags.
	 */
	void RebuildMenu();
	
	/**
	 * when the detector cache has been loaded rebuild the menu
	 */
	void OnDetectorDbInitComplete(t4p::ActionEventClass& event);
	
	/**
	 * when the config detectors have finished running, load all of the
	 * projects' config files and build the menu.
	 */
	void OnConfigFilesDetected(t4p::ActionEventClass& event);
	
	/**
	 * The feature gives this view the list of config tags
	 * in all current projects.
	 */
	t4p::ConfigFilesFeatureClass& Feature;
	
	/**
	 * Store the menu that we use to put the config file
	 * menu items in. Since its a wxWindow pointer, it is 
	 * owned by the menu bar.
	 */
	wxMenu* ConfigMenu;

	DECLARE_EVENT_TABLE()
};

}

#endif