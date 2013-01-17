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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef __MVCEDITOR_CONFIGFILESFEATURECLASS_H__
#define __MVCEDITOR_CONFIGFILESFEATURECLASS_H__

#include <features/FeatureClass.h>
#include <globals/ConfigTagClass.h>
#include <vector>

namespace mvceditor {

/**
 * This feature will take all of the detected config files and
 * will populate a menu with config files from each project.
 * The config files are detected with the help of ConfigDetectorActionClass
 * and are read with the help of ConfigTagFinderClass.
 */
class ConfigFilesFeatureClass : public mvceditor::FeatureClass {

public:

	ConfigFilesFeatureClass(mvceditor::AppClass& app);

	void AddNewMenu(wxMenuBar* menuBar);

private:

	/**
	 * pair together a project name and the config files for that
	 * project, that way we can build the submenus.
	 */
	struct ConfigPair {
		wxString ProjectLabel;
		std::vector<mvceditor::ConfigTagClass> ConfigTags;	
	};

	std::vector<ConfigPair> ConfigPairs;

	/**
	 * Read all of the detected config tags into memory; that way we can
	 * assign them a menu ID (the index into this vector will be used
	 * as the menu ID).
	 */
	std::vector<mvceditor::ConfigTagClass> ConfigTags;

	/**
	 * we will attach this to the menu bar; it will be
	 * owned up by wxMenuBar
	 */
	wxMenu* ConfigMenu;

	/**
	 * recreate the menu based from the loaded ConfigTags.
	 */
	void RebuildMenu();

	/**
	 * when the config detector has completed, load all of the
	 * projects' config files and build the menu.
	 */
	void OnConfigDetectorComplete(wxCommandEvent& event);

	/**
	 * when the config detectors have finished running, load all of the
	 * projects' config files and build the menu.
	 */
	void OnConfigFilesDetected(wxCommandEvent& event);

	/**
	 * When a menu item is selected; open the corresponding config
	 * file
	 */
	void OnConfigMenuItem(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

}

#endif