/**
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
 */
#ifndef SRC_VIEWS_PROJECTVIEWCLASS_H_
#define SRC_VIEWS_PROJECTVIEWCLASS_H_

#include <wx/filepicker.h>
#include <vector>
#include "features/ProjectFeatureClass.h"
#include "views/FeatureViewClass.h"
#include "views/wxformbuilder/ProjectFeatureForms.h"

namespace t4p {
class ProjectViewClass : public FeatureViewClass {
	public:
	/**
	 * Create a new instance of ProjectViewClass.
	 */
	ProjectViewClass(t4p::ProjectFeatureClass& feature);

	/**
	 * Add menu items to the file menu
	 */
	void AddFileMenuItems(wxMenu* fileMenu);

	/**
	 * Add a preference sheet for the user to enter the explorer executable.
	 */
	void AddPreferenceWindow(wxBookCtrlBase* parent);

	private:
	/**
	 * close all projects and all resources that depend on it
	 */
	void CleanupProjects();

	/**
	 * Open up a dialog so that the user can add more source directories
	 */
	void OnProjectDefine(wxCommandEvent& event);

	/**
	 * Open up a "simple" dialog so that the user create a new project
	 */
	void OnCreateNewProject(wxCommandEvent& event);

	t4p::ProjectFeatureClass& Feature;

	DECLARE_EVENT_TABLE()
};

class ProjectPreferencesPanelClass : public ProjectPreferencesGeneratedPanelClass {
	public:
	/**
	 * Construct a new instance
	 */
	ProjectPreferencesPanelClass(wxWindow* parent, ProjectFeatureClass& projectFeature);
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
	ProjectDefinitionDialogClass(wxWindow* parent, t4p::ProjectClass& project);

	private:
	/**
	 * Reference that gets updated only when the user clicks OK
	 */
	t4p::ProjectClass& Project;

	/**
	 * The project that the user edits
	 */
	t4p::ProjectClass EditedProject;

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
	ProjectSourceDialogClass(wxWindow* parent, t4p::SourceClass& source);

	private:
	/**
	 * The source that will get updated when the user clicks OK
	 */
	t4p::SourceClass& Source;

	/**
	 * The source being edited by the user
	 */
	t4p::SourceClass EditedSource;
};

/**
 * Dialog to add, remove, edit, or enable/disable projects.
 */
class ProjectListDialogClass : public ProjectListDialogGeneratedClass {
	protected:
	void OnSelectAllButton(wxCommandEvent& event);
	void OnProjectsListDoubleClick(wxCommandEvent& event);
	void OnProjectsListCheckbox(wxCommandEvent& event);
	void OnAddButton(wxCommandEvent& event);
	void OnRemoveButton(wxCommandEvent& event);
	void OnEditButton(wxCommandEvent& event);
	void OnOkButton(wxCommandEvent& event);
	void OnCancelButton(wxCommandEvent& event);
	void OnAddFromDirectoryButton(wxCommandEvent& event);
	void OnHelpButton(wxCommandEvent& event);

	public:
	/**
	 * @param project the list of projects the user will edit / remove / add to
	 * @param removedProjects the list of projects that the user has removed. This list will
	 *        get populated only when the user clicks OK (and has removed a project)
	 * @param touchedProjects the list of projects that have been . This list will
	 *        get populated only when the user clicks OK (and has touched a project). "Touched"
	 *        means that at the project's source directories list has been changed in any way;
	 *        if a project has a new source, a project has one fewer source, a source's
	 *        include/exclude wildcards have been changed. This list also includes any completely
	 *        new projects as well.
	 */
	ProjectListDialogClass(wxWindow* parent, std::vector<t4p::ProjectClass>& projects,
		std::vector<t4p::ProjectClass>& removedProjects,
		std::vector<t4p::ProjectClass>& touchedProjects);

	private:
	/**
	 * The list that will get updated once the user clicks OK
	 */
	std::vector<t4p::ProjectClass>& Projects;

	/**
	 * The projects being edited by the user.
	 */
	std::vector<t4p::ProjectClass> EditedProjects;

	/**
	 * The list of projects that the user has decided to remove. This list will
	 * get populated only when the user clicks OK (and has removed a project)
	 */
	std::vector<t4p::ProjectClass>& RemovedProjects;

	/**
	 * The list of projects that the user has added/modified/removed source directories. This list will
	 * get populated only when the user clicks OK (and has touched a project)
	 */
	std::vector<t4p::ProjectClass>& TouchedProjects;

	/**
	 * add the project labels to the check list box
	 */
	void Populate();

	private:
	/**
	 * @param project to add to both the edited data structure and the GUI list
	 */
	void AddProject(const t4p::ProjectClass& project);
};

/**
 * class to show a checklist of choices and allow to user to select one or more.
 */
class MultipleSelectDialogClass : public MultipleSelectDialogGeneratedClass {
	public:
	/**
	 * @param parent the parent window
	 * @param title the title of the dialog
	 * @Param caption text shown in the dialog
	 * @param choices the items to display
	 * @param selections the indices of the chosen items will be pushed into this vector
	 */
	MultipleSelectDialogClass(wxWindow* parent, const wxString& title, const wxString& caption, std::vector<wxString>& choices, std::vector<int>& selections);

	protected:
	void OnOkButton(wxCommandEvent& event);

	void OnCancelButton(wxCommandEvent& event);

	private:
	std::vector<int>& Selections;
};
}  // namespace t4p

#endif  // SRC_VIEWS_PROJECTVIEWCLASS_H_
