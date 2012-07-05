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

// defined below
class ProjectPluginPanelClass;

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
	ProjectPluginClass(mvceditor::AppClass& app);
	
	~ProjectPluginClass();

	/**
	 * Opens the given directory as a project.
	 */
	void ProjectOpen(const wxString& directoryPath);

	/**
	 * Opens the 'default' project. It will be the one that was opened on the last
	 * run of the program.
	 */
	void ProjectOpenDefault();

	/**
	 * Add menu items to the file menu
	 */
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
	 * This is the callback that gets called when the PHP framework detectors have 
	 * successfully run
	 */
	void OnFrameworkFound(mvceditor::FrameworkFoundEventClass& event);
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
	 * This object will be used to detct the various PHP framework artifacts (resources,
	 * database connections, route URLs).
	 */
	mvceditor::PhpFrameworkDetectorClass PhpFrameworks;

	/**
	 * a 'queue' of folders to perform framework detection on
	 */
	std::vector<wxString> DirectoriesToDetect;


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

/**
 * Dialog to edit a single project (add multiple sources to a project)
 */
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

/**
 * Dialog to add a source directory to a project
 */
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

/**
 * Dialog to add, remove, edit, or enable/disable projects.
 */
class ProjectListDialogClass : public ProjectListDialogGeneratedClass {

protected:

	void OnProjectsListDoubleClick(wxCommandEvent& event);
	void OnProjectsListCheckbox(wxCommandEvent& event);
	void OnAddButton(wxCommandEvent& event);
	void OnRemoveButton(wxCommandEvent& event);
	void OnEditButton(wxCommandEvent& event);
	void OnOkButton(wxCommandEvent& event);
	void OnCancelButton(wxCommandEvent& event);

public:

	ProjectListDialogClass(wxWindow* parent, std::vector<mvceditor::ProjectClass>& projects);

private:

	/**
	 * The list that will get updated once the user clicks OK
	 */
	std::vector<mvceditor::ProjectClass>& Projects;

	/**
	 * The projects being edited by the user.
	 */
	std::vector<mvceditor::ProjectClass> EditedProjects;

	/**
	 * add the project labels to the check list box
	 */
	void Populate();

};

}

#endif