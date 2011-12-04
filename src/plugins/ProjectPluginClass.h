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
#ifndef PROJECTPLUGINCLASS_H
#define PROJECTPLUGINCLASS_H

#include <PluginClass.h>
#include <plugins/wxformbuilder/ProjectPluginGeneratedPanelClass.h>
#include <wx/filepicker.h>

namespace mvceditor {


class ProjectPluginClass : public PluginClass {

public:

	/**
	 * the location of the "file explorer" binary.  This is dependent per OS.
	 * @var wxString
	 */
	wxString ExplorerExecutable;

	/**
	 * Create a new instance of ProjectPluginClass.
	 */
	ProjectPluginClass();

	/**
	 * Add menu items to the project menu
	 */
	void AddProjectMenuItems(wxMenu* projectMenu);

	/**
	 * Add items to the toolbar
	 */
	void AddToolBarItems(wxAuiToolBar* toolbar);

	/**
	 * Add a preference sheet for the user to enter the explorer executable.
	 */
	void AddPreferenceWindow(wxBookCtrlBase* parent);

	/**
	 * Load the preferences from persistent storage 
	 */
	void LoadPreferences(wxConfigBase* config);

	/**
	 * Save the preferences to persistent storage 
	 */
	void SavePreferences(wxConfigBase* config);

	void OnProjectOpened();

private:

	/**
	 * Handler for the Project .. Explore menu 
	 * @param wxCommandEvent& event 
	 */
	void OnProjectExplore(wxCommandEvent& event);

	/**
	 * Handler for the Project .. Explore Open File menu 
	 * @param wxCommandEvent& event 
	 */
	void OnProjectExploreOpenFile(wxCommandEvent& event);

	/**
	 * Since there could be 1...N recent project menu items we cannot listen to one menu item's event
	 * we have to listen to all menu events
	 */
	void OnMenu(wxCommandEvent& event);

	/**
	 * updates the recent projects sub-menu with the projects from the internal list
	 */
	void SyncMenu();

	/**
	 * Will write the RecentProjects list to disk.
	 */
	void PersistProjectList();

	/**
	 * List of recently opened projects
	 */
	std::vector<wxString> RecentProjects;

	/**
	 * Sub-Menu for the recent projects 
	 */
	wxMenu* RecentProjectsMenu;

	int MAX_RECENT_PROJECTS;

	DECLARE_EVENT_TABLE()
	
};

class ProjectPluginPanelClass : public ProjectPluginGeneratedPanelClass {
	
public:

	/**
	 * Construct a new instance
	 */
	ProjectPluginPanelClass(wxWindow* parent, ProjectPluginClass& projectPlugin);

protected:

	/**
	 * When a new file is chosen, update the textbox.
	 */
	void OnFileChanged(wxFileDirPickerEvent& event);
};

}

#endif