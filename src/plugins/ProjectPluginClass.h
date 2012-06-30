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
#include <wx/docview.h>

namespace mvceditor {


class ProjectPluginClass : public PluginClass {

public:

	/**
	 * the location of the "file explorer" binary.  This is dependent per OS.
	 * @var wxString
	 */
	wxString ExplorerExecutable;

	/**
	 * Serialized PHP file filters string from the config
	 */
	wxString PhpFileFiltersString;

	/**
	 * Serialized CSS file filters string from the config
	 */
	wxString CssFileFiltersString;

	/**
	 * Serialized SQL file filters string from the config
	 */
	wxString SqlFileFiltersString;

	/**
	 * Create a new instance of ProjectPluginClass.
	 */
	ProjectPluginClass();

	/**
	 * Opens the given directory as a project.
	 */
	void ProjectOpen(const wxString& directoryPath);

	/**
	 * Add menu items to the project menu
	 */
	void AddProjectMenuItems(wxMenu* projectMenu);
	void AddFileMenuItems(wxMenu* fileMenu);

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

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);

private:

	/**
	 * Save the preferences to persistent storage 
	 */
	void SavePreferences(wxCommandEvent& event);

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
	 * handler for the open project command that can be published by any
	 * other plugin
	 */
	void OnCmdProjectOpen(wxCommandEvent& event);

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
	 * This is the callback that gets called when the PHP framework detectors have 
	 * successfully run
	 */
	void OnFrameworkDetectionComplete(wxCommandEvent& event);
	void OnFrameworkDetectionInProgress(wxCommandEvent& event);
	void OnFrameworkDetectionFailed(wxCommandEvent& event);

	/**
	 * close project and all resources that depend on it
	 */
	void CloseProject();

	/**
	 * Open up a dialog so that the user can add more source directories
	 */
	void OnProjectDefine(wxCommandEvent& event);

	/**
	 * List of recently opened projects
	 */
	wxFileHistory History;

	/**
	 * This object will be used to detct the various PHP framework artifacts (resources,
	 * database connections, route URLs).
	 */
	std::auto_ptr<PhpFrameworkDetectorClass> PhpFrameworks;

	/**
	 * Sub-Menu for the recent projects 
	 */
	wxMenu* RecentProjectsMenu;

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

class ProjectDefinitionDialogClass : public ProjectDefinitionDialogGeneratedClass {

protected:

	void OnAddSource(wxCommandEvent& event);
	void OnEditSource(wxCommandEvent& event);
	void OnRemoveSource(wxCommandEvent& event);
	void OnOkButton(wxCommandEvent& event);
	void OnCancelButton(wxCommandEvent& event);
	void OnSourcesListDoubleClick(wxCommandEvent& event);
public:

	ProjectDefinitionDialogClass(wxWindow* parent, mvceditor::ProjectClass& project);

private:

	/**
	 * Reference that gets updated only when the user clicks OK
	 */
	mvceditor::ProjectClass& Project;

	/**
	 * The project that the user edits
	 */
	mvceditor::ProjectClass EditedProject;

	/**
	 * add the project sources to the list box.
	 */
	void Populate();
};

class ProjectSourceDialogClass : public ProjectSourceDialogGeneratedClass {

protected:

	void OnOkButton(wxCommandEvent& event);
	void OnCancelButton(wxCommandEvent& event);

public:

	ProjectSourceDialogClass(wxWindow* parent, mvceditor::SourceClass& source);

private:

	/**
	 * The source that will get updated when the user clicks OK
	 */
	mvceditor::SourceClass& Source;

	/**
	 * The source being edited by the user
	 */
	mvceditor::SourceClass EditedSource;
};

}

#endif