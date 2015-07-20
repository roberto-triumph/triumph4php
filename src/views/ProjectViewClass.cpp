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
#include <views/ProjectViewClass.h>
#include <widgets/NonEmptyTextValidatorClass.h>
#include <globals/Errors.h>
#include <globals/Assets.h>
#include <globals/Number.h>
#include <Triumph.h>
#include <wx/artprov.h>
#include <wx/platinfo.h>
#include <wx/filename.h>
#include <wx/fileconf.h>
#include <wx/valgen.h>
#include <wx/dir.h>
#include <wx/dirdlg.h>
#include <wx/choicdlg.h>
#include <algorithm>

t4p::ProjectViewClass::ProjectViewClass(t4p::ProjectFeatureClass& feature)
	: FeatureViewClass()
	, Feature(feature) {
}

void t4p::ProjectViewClass::AddFileMenuItems(wxMenu* fileMenu) {
	fileMenu->Append(t4p::MENU_PROJECT + 1, _("New Project"), _("Create a new project from a source directory"), wxITEM_NORMAL);
	fileMenu->Append(t4p::MENU_PROJECT + 0, _("Show Projects"), _("See the created projects, and add additional source directories to the current project"), wxITEM_NORMAL);
}

void t4p::ProjectViewClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
	ProjectPreferencesPanelClass* panel = new ProjectPreferencesPanelClass(parent, Feature);
	parent->AddPage(panel, wxT("Project"));
}

void t4p::ProjectViewClass::OnProjectDefine(wxCommandEvent& event) {
	// make sure that no existing project index or wipe action is running
	// as we will re-trigger an index if the user makes any modifications to
	// the project sources
	if (Feature.App.Sequences.Running()) {
		wxString msg = wxString::FromAscii(
			"There is an existing background task running. Since the changes "
			"made from this dialog may re-trigger a project index sequence, "
			"you may not make modifications until the existing background task ends.\n"
			"Would you like to stop the current background tasks? If you answer no, the "
			"projects dialog will not be opened.");
		msg = wxGetTranslation(msg);
		int ret = wxMessageBox(msg, _("Warning"), wxICON_WARNING | wxYES_NO, GetMainWindow());
		if (wxYES != ret) {
			return;
		}

		// user said yes, we should stop the running tasks
		Feature.App.Sequences.Stop();
		Feature.App.RunningThreads.StopAll();
	}

	std::vector<t4p::ProjectClass> removedProjects, touchedProjects;
	t4p::ProjectListDialogClass dialog(GetMainWindow(), Feature.App.Globals.Projects, removedProjects, touchedProjects);
	if (wxOK == dialog.ShowModal()) {
		std::vector<t4p::ProjectClass>::iterator project;

		wxCommandEvent evt(t4p::EVENT_APP_PREFERENCES_SAVED);
		Feature.App.EventSink.Publish(evt);
		wxConfigBase* config = wxConfig::Get();
		config->Flush();

		// order here is important for
		// suppression to work properly .. not the best
		// but it'll do for now
		// what happens when the user removes a project and
		// adds the same project back at the same time?
		if (!removedProjects.empty()) {
			t4p::ProjectEventClass removedEvt(t4p::EVENT_APP_PROJECTS_REMOVED, removedProjects);
			Feature.App.EventSink.Publish(removedEvt);
		}

		if (!touchedProjects.empty()) {
			t4p::ProjectEventClass updateEvt(t4p::EVENT_APP_PROJECTS_UPDATED, touchedProjects);
			Feature.App.EventSink.Publish(updateEvt);
		}


		// signal that this app has modified the config file, that way the external
		// modification check fails and the user will not be prompted to reload the config
		Feature.App.UpdateConfigModifiedTime();

		// start the sequence that will update all global data structures
		// delete the cache files for the projects the user has removed
		// remove tags from deleted projects
		// if at least 1 project was re-enabled, then prompt for re-tagging
		// note that we always want to start the sequence, because
		// the sequence disables projects that were removed.
		// if user only disabled projects, no need to prompt since no projects
		// will be re-tagged
		if (!touchedProjects.empty()) {
			wxString msg = wxString::FromAscii(
				"Would you like to re-tag your newly enabled projects at this time?");
			msg = wxGetTranslation(msg);
			int ret = wxMessageBox(msg, _("Tag projects"), wxICON_QUESTION | wxYES_NO, GetMainWindow());
			if (wxNO == ret) {
				// user does not want to re-tag newly enabled projects
				touchedProjects.clear();
			}
		}
		Feature.App.Sequences.ProjectDefinitionsUpdated(touchedProjects, removedProjects);
	}
}

void t4p::ProjectViewClass::OnCreateNewProject(wxCommandEvent& event) {
	// make sure that no existing project index or wipe action is running
	// as we will re-trigger an index if the user makes any modifications to
	// the project sources
	if (Feature.App.Sequences.Running()) {
		wxString msg = wxString::FromAscii(
			"There is an existing background task running. Since the changes "
			"made from this dialog may re-trigger a project index sequence, "
			"you may not make modifications until the existing background task ends.\n"
			"Would you like to stop the current background tasks? If you answer no, the "
			"projects dialog will not be opened.");
		msg = wxGetTranslation(msg);
		int ret = wxMessageBox(msg, _("Warning"), wxICON_WARNING | wxYES_NO, GetMainWindow());
		if (wxYES != ret) {
			return;
		}

		// user said yes, we should stop the running tasks
		Feature.App.Sequences.Stop();
		Feature.App.RunningThreads.StopAll();
	}

	wxDirDialog dirDialog(GetMainWindow(), _("Choose Project Root Directory"));
	if (dirDialog.ShowModal() == wxID_OK) {
		wxString dir = dirDialog.GetPath();

		// start the sequence that will update all global data structures
		// delete the cache files for the projects the user has removed
		// remove tags from deleted projects
		// if at least 1 project was re-enabled, then prompt for re-tagging
		// note that we always want to start the sequence, because
		// the sequence disables projects that were removed.
		// if user only disabled projects, no need to prompt since no projects
		// will be re-tagged
		wxString msg = wxString::FromAscii(
			"Would you like to re-tag your newly created project at this time?\n"
			"Tagging a project allows code completion, jump to resource,\n"
			"and search for resources to work properly.");
		msg = wxGetTranslation(msg);
		int ret = wxMessageBox(msg, _("Tag projects"), wxICON_QUESTION | wxYES_NO, GetMainWindow());
		bool doTag = wxYES == ret;
		Feature.CreateProject(dir, doTag);
	}
}

t4p::ProjectPreferencesPanelClass::ProjectPreferencesPanelClass(wxWindow *parent, t4p::ProjectFeatureClass &projectFeature)
: ProjectPreferencesGeneratedPanelClass(parent) {
	NonEmptyTextValidatorClass phpFileExtensionsValidator(&projectFeature.App.Globals.FileTypes.PhpFileExtensionsString, PhpLabel);
	PhpFileExtensions->SetValidator(phpFileExtensionsValidator);

	NonEmptyTextValidatorClass cssFileExtensionsValidator(&projectFeature.App.Globals.FileTypes.CssFileExtensionsString, CssLabel);
	CssFileExtensions->SetValidator(cssFileExtensionsValidator);

	NonEmptyTextValidatorClass sqlFileExtensionsValidator(&projectFeature.App.Globals.FileTypes.SqlFileExtensionsString, SqlLabel);
	SqlFileExtensions->SetValidator(sqlFileExtensionsValidator);

	NonEmptyTextValidatorClass jsFileExtensionsValidator(&projectFeature.App.Globals.FileTypes.JsFileExtensionsString, JsLabel);
	JsFileExtensions->SetValidator(jsFileExtensionsValidator);

	NonEmptyTextValidatorClass configFileExtensionsValidator(&projectFeature.App.Globals.FileTypes.ConfigFileExtensionsString, ConfigLabel);
	ConfigFileExtensions->SetValidator(configFileExtensionsValidator);

	NonEmptyTextValidatorClass crontabFileExtensionsValidator(&projectFeature.App.Globals.FileTypes.CrontabFileExtensionsString, CrontabLabel);
	CrontabFileExtensions->SetValidator(crontabFileExtensionsValidator);

	NonEmptyTextValidatorClass yamlFileExtensionsValidator(&projectFeature.App.Globals.FileTypes.YamlFileExtensionsString, YamlLabel);
	YamlFileExtensions->SetValidator(yamlFileExtensionsValidator);

	NonEmptyTextValidatorClass xmlFileExtensionsValidator(&projectFeature.App.Globals.FileTypes.XmlFileExtensionsString, XmlLabel);
	XmlFileExtensions->SetValidator(xmlFileExtensionsValidator);

	NonEmptyTextValidatorClass rubyFileExtensionsValidator(&projectFeature.App.Globals.FileTypes.RubyFileExtensionsString, RubyLabel);
	RubyFileExtensions->SetValidator(rubyFileExtensionsValidator);

	NonEmptyTextValidatorClass luaFileExtensionsValidator(&projectFeature.App.Globals.FileTypes.LuaFileExtensionsString, LuaLabel);
	LuaFileExtensions->SetValidator(luaFileExtensionsValidator);

	NonEmptyTextValidatorClass markdownFileExtensionsValidator(&projectFeature.App.Globals.FileTypes.MarkdownFileExtensionsString, MarkdownLabel);
	MarkdownFileExtensions->SetValidator(markdownFileExtensionsValidator);

	NonEmptyTextValidatorClass bashFileExtensionsValidator(&projectFeature.App.Globals.FileTypes.BashFileExtensionsString, BashLabel);
	BashFileExtensions->SetValidator(bashFileExtensionsValidator);

	NonEmptyTextValidatorClass diffFileExtensionsValidator(&projectFeature.App.Globals.FileTypes.DiffFileExtensionsString, DiffLabel);
	DiffFileExtensions->SetValidator(diffFileExtensionsValidator);


	NonEmptyTextValidatorClass miscFileExtensionsValidator(&projectFeature.App.Globals.FileTypes.MiscFileExtensionsString, MiscLabel);
	MiscFileExtensions->SetValidator(miscFileExtensionsValidator);
}

t4p::ProjectDefinitionDialogClass::ProjectDefinitionDialogClass(wxWindow* parent, t4p::ProjectClass& project)
	: ProjectDefinitionDialogGeneratedClass(parent)
	, Project(project)
	, EditedProject(project) {
	wxGenericValidator labelValidator(&EditedProject.Label);
	Label->SetValidator(labelValidator);
	Populate();
	TransferDataToWindow();
	if (!SourcesList->IsEmpty()) {
		SourcesList->SetSelection(0);
	}
	Label->SetFocus();
}

void t4p::ProjectDefinitionDialogClass::OnAddSource(wxCommandEvent& event) {
	t4p::SourceClass newSrc;
	t4p::ProjectSourceDialogClass dialog(this, newSrc);
	if (wxOK == dialog.ShowModal()) {
		EditedProject.AddSource(newSrc);
		SourcesList->Append(newSrc.RootDirectory.GetFullPath());
	}
}

void t4p::ProjectDefinitionDialogClass::OnEditSource(wxCommandEvent& event) {
	size_t selected = SourcesList->GetSelection();
	if (selected < EditedProject.Sources.size()) {
		t4p::SourceClass src = EditedProject.Sources[selected];
		t4p::ProjectSourceDialogClass dialog(this, src);
		if (wxOK == dialog.ShowModal()) {
			SourcesList->SetString(selected, src.RootDirectory.GetFullPath());
			EditedProject.Sources[selected] = src;
		}
	}
}

void t4p::ProjectDefinitionDialogClass::OnRemoveSource(wxCommandEvent& event) {
	int selected = SourcesList->GetSelection();
	if (t4p::NumberLessThan(selected, EditedProject.Sources.size())) {
		t4p::SourceClass src = EditedProject.Sources[selected];
		wxString msg = _("Are you sure you wish to remove the source? ");
		msg += src.RootDirectory.GetFullPath();
		msg += wxT("\n");
		msg +=
			_("Triumph will no longer open or index files in the directory. Note that the directory is not actually deleted from the file system");
		wxString caption = _("Remove Project Source");
		int response = wxMessageBox(msg, caption, wxYES_NO, this);
		if (wxYES == response) {
			SourcesList->Delete(selected);
			EditedProject.Sources.erase(EditedProject.Sources.begin() + selected);
		}
	}
}

void t4p::ProjectDefinitionDialogClass::OnOkButton(wxCommandEvent& event) {
	TransferDataFromWindow();
	if (!EditedProject.HasSources()) {
		wxMessageBox(_("Project must have at least one source directory"));
		return;
	}
	Project = EditedProject;
	EndModal(wxOK);
}

void t4p::ProjectDefinitionDialogClass::OnCancelButton(wxCommandEvent& event) {
	EndModal(wxCANCEL);
}

void t4p::ProjectDefinitionDialogClass::OnSourcesListDoubleClick(wxCommandEvent& event) {
	int selected = event.GetSelection();
	if (t4p::NumberLessThan(selected, EditedProject.Sources.size())) {
		t4p::SourceClass src = EditedProject.Sources[selected];
		t4p::ProjectSourceDialogClass dialog(this, src);
		if (wxOK == dialog.ShowModal()) {
			SourcesList->SetString(selected, src.RootDirectory.GetFullPath());
			EditedProject.Sources[selected] = src;
		}
	}
	event.Skip();
}

void t4p::ProjectDefinitionDialogClass::Populate() {
	for (size_t i = 0; i < EditedProject.Sources.size(); ++i) {
		SourcesList->Append(EditedProject.Sources[i].RootDirectory.GetFullPath());
	}
}

t4p::ProjectSourceDialogClass::ProjectSourceDialogClass(wxWindow* parent, t4p::SourceClass& source)
	: ProjectSourceDialogGeneratedClass(parent)
	, Source(source)
	, EditedSource(source) {
	RootDirectory->SetPath(source.RootDirectory.GetFullPath());
	IncludeWildcards->SetValue(EditedSource.IncludeWildcardsString());
	ExcludeWildcards->SetValue(EditedSource.ExcludeWildcardsString());

	if (EditedSource.IncludeWildcardsString().IsEmpty()) {
		IncludeWildcards->SetValue(wxT("*.*"));
	}
	RootDirectory->SetFocus();
}

void t4p::ProjectSourceDialogClass::OnOkButton(wxCommandEvent& event) {
	if (IncludeWildcards->IsEmpty()) {
		wxMessageBox(_("Include wildcards must not be empty."));
		return;
	}
	wxString path = RootDirectory->GetPath();
	if (!wxFileName::DirExists(path)) {
		wxMessageBox(_("Root directory must exist."));
		return;
	}
	Source.RootDirectory.AssignDir(path);
	Source.SetIncludeWildcards(IncludeWildcards->GetValue());
	Source.SetExcludeWildcards(ExcludeWildcards->GetValue());
	EndModal(wxOK);
}

void t4p::ProjectSourceDialogClass::OnCancelButton(wxCommandEvent& event) {
	EndModal(wxCANCEL);
}

t4p::ProjectListDialogClass::ProjectListDialogClass(wxWindow* parent, std::vector<t4p::ProjectClass>& projects,
														  std::vector<t4p::ProjectClass>& removedProjects,
														  std::vector<t4p::ProjectClass>& touchedProjects)
	: ProjectListDialogGeneratedClass(parent)
	, Projects(projects)
	, EditedProjects(projects)
	, RemovedProjects(removedProjects)
	, TouchedProjects(touchedProjects) {
	Populate();
	if (!ProjectsList->IsEmpty()) {
		ProjectsList->SetSelection(0);
	}
}

void t4p::ProjectListDialogClass::OnSelectAllButton(wxCommandEvent& event) {
	// this will be a toggle button; if all items are checked then
	// this button should de-check them
	bool isAllChecked = true;
	for (size_t i = 0; i < ProjectsList->GetCount(); i++) {
		isAllChecked &= ProjectsList->IsChecked(i);
	}

	for (size_t i = 0; i < ProjectsList->GetCount(); i++) {
		ProjectsList->Check(i, !isAllChecked);

		// Check does not emit an event, need to update the project data structure
		EditedProjects[i].IsEnabled = !isAllChecked;
	}
}

void t4p::ProjectListDialogClass::OnProjectsListDoubleClick(wxCommandEvent& event) {
	int selection = event.GetSelection();
	if (t4p::NumberLessThan(selection, EditedProjects.size())) {
		t4p::ProjectClass project = EditedProjects[selection];
		t4p::ProjectDefinitionDialogClass dialog(this, project);
		if (wxOK == dialog.ShowModal()) {
			EditedProjects[selection] = project;
			wxString label = project.Label;

			// escape any ampersands in the label, list box needs them escaped
			// only happens in win32
			wxPlatformInfo info = wxPlatformInfo::Get();
			if (info.GetOperatingSystemId() == wxOS_WINDOWS_NT) {
				label.Replace(wxT("&"), wxT("&&"));
			}
			ProjectsList->SetString(selection, label);

			// on linux, setting the string causes the checkbox to not
			// be drawn and it makes it look like the project is
			// disabled
			ProjectsList->Check(selection, EditedProjects[selection].IsEnabled);
		}
	}
	event.Skip();
}

void t4p::ProjectListDialogClass::OnProjectsListCheckbox(wxCommandEvent& event) {
	int selection = event.GetSelection();
	if (t4p::NumberLessThan(selection, EditedProjects.size())) {
		t4p::ProjectClass project = EditedProjects[selection];
		project.IsEnabled = ProjectsList->IsChecked(selection);
		EditedProjects[selection] = project;
	}
	event.Skip();
}

void t4p::ProjectListDialogClass::OnAddButton(wxCommandEvent& event) {
	t4p::ProjectClass project;
	t4p::ProjectDefinitionDialogClass dialog(this, project);
	if (wxOK == dialog.ShowModal()) {
		AddProject(project);
	}
}

void t4p::ProjectListDialogClass::AddProject(const t4p::ProjectClass& project) {
	EditedProjects.push_back(project);
	wxString label = project.Label;

	// escape any ampersands in the label, list box needs them escaped
	// only happens in win32
	wxPlatformInfo info = wxPlatformInfo::Get();
	if (info.GetOperatingSystemId() == wxOS_WINDOWS_NT) {
		label.Replace(wxT("&"), wxT("&&"));
	}
	ProjectsList->Append(label);
	ProjectsList->Check(ProjectsList->GetCount() - 1, true);
}

void t4p::ProjectListDialogClass::OnRemoveButton(wxCommandEvent& event) {
	wxArrayInt selections;
	ProjectsList->GetSelections(selections);
	if (selections.GetCount() > 0) {
		wxString projectLabels;
		for (size_t i = 0 ; i <  selections.GetCount(); ++i) {
			size_t selection = selections[i];
			if (selection < EditedProjects.size()) {
				t4p::ProjectClass project = EditedProjects[selection];
				projectLabels += project.Label;
				projectLabels += wxT("\n");
			}
		}

		wxString msg = _("Are you sure you wish to remove the projects\n\n");
		msg += projectLabels;
		msg += wxT("\n");
		msg += _("Triumph will no longer open or index files in any sources of this project. Note that no directories are actually deleted from the file system");
		wxString caption = _("Remove Projects");
		int response = wxMessageBox(msg, caption, wxYES_NO);
		if (wxYES == response) {
			// copy all the items that are NOT to be removed into a new vector
			std::vector<t4p::ProjectClass> remainingProjects;
			wxArrayString remainingLabels;

			for (size_t i = 0; i < EditedProjects.size(); ++i) {
				// we dont find the index in the selected list means that the
				// user does NOT want to remove it
				if (selections.Index(i) == wxNOT_FOUND) {
					remainingProjects.push_back(EditedProjects[i]);
					remainingLabels.Add(EditedProjects[i].Label);
				} else {
					RemovedProjects.push_back(EditedProjects[i]);
				}
			}
			EditedProjects = remainingProjects;
			ProjectsList->Set(remainingLabels);
			for (size_t i = 0; i < EditedProjects.size(); ++i) {
				ProjectsList->Check(i, EditedProjects[i].IsEnabled);
			}
		}
	}
}

void t4p::ProjectListDialogClass::OnEditButton(wxCommandEvent& event) {
	wxArrayInt selections;
	ProjectsList->GetSelections(selections);
	if (selections.GetCount() != 1) {
		wxMessageBox(_("Please select a single project to edit."));
		return;
	}
	int selection = selections[0];
	if (t4p::NumberLessThan(selection, EditedProjects.size())) {
		t4p::ProjectClass project = EditedProjects[selection];
		t4p::ProjectDefinitionDialogClass dialog(this, project);
		if (wxOK == dialog.ShowModal()) {
			EditedProjects[selection] = project;
			wxString label = project.Label;

			// escape any ampersands in the label, list box needs them escaped
			// only happens in win32
			wxPlatformInfo info = wxPlatformInfo::Get();
			if (info.GetOperatingSystemId() == wxOS_WINDOWS_NT) {
				label.Replace(wxT("&"), wxT("&&"));
			}
			ProjectsList->SetString(selection, label);

			// on linux, setting the string causes the checkbox to not
			// be drawn and it makes it look like the project is
			// disabled
			ProjectsList->Check(selection, EditedProjects[selection].IsEnabled);
		}
	}
}

void t4p::ProjectListDialogClass::OnOkButton(wxCommandEvent& event) {
	// go thorough the edited projects and see which ones actually changed
	// here, Projects is the original list and EditedProjects is the list that the
	// user modified
	std::vector<t4p::ProjectClass>::const_iterator project;
	std::vector<t4p::ProjectClass>::iterator editedProject;
	for (editedProject = EditedProjects.begin(); editedProject != EditedProjects.end(); ++editedProject) {
		// if a project is disabled then we wont need to update the cache so we can
		// leave it as not touched.
		// short circuiting the loop since comparing source lists was shown to be expensive
		// under a profiler.
		if (!editedProject->IsEnabled) {
			continue;
		}

		// if we dont find the edited project in the original project, then it has been changed
		// by the reverse, if we find the edited project in the original projects list then it has
		/// NOT changed
		bool touched = true;
		for (project = Projects.begin(); project != Projects.end(); ++project) {
			bool isSame = t4p::CompareSourceLists(project->Sources, editedProject->Sources);
			bool hasBeenReEnabled = editedProject->IsEnabled && !project->IsEnabled;

			// even if 2 projects are the same, if the project went from disabled to enabled
			// we want to label it as touched so that parsing takes place
			if (isSame && !hasBeenReEnabled) {
				touched = false;
				break;
			}
		}
		if (touched) {
			TouchedProjects.push_back(*editedProject);
		}
	}

	Projects = EditedProjects;
	EndModal(wxOK);
}

void t4p::ProjectListDialogClass::OnCancelButton(wxCommandEvent& event) {
	EndModal(wxCANCEL);
}

void t4p::ProjectListDialogClass::Populate() {
	ProjectsList->Clear();
	wxPlatformInfo info = wxPlatformInfo::Get();
	for (size_t i = 0; i < EditedProjects.size(); ++i) {
		t4p::ProjectClass project = EditedProjects[i];
		wxString label = project.Label;

		// escape any ampersands in the label, list box needs them escaped
		// only happens in win32
		if (info.GetOperatingSystemId() == wxOS_WINDOWS_NT) {
			label.Replace(wxT("&"), wxT("&&"));
		}
		ProjectsList->Append(label);
		ProjectsList->Check(ProjectsList->GetCount() - 1, project.IsEnabled);
	}
}

void t4p::ProjectListDialogClass::OnAddFromDirectoryButton(wxCommandEvent& event) {
	wxString rootDir = wxDirSelector(_("Choose the directory where your projects are located"), wxEmptyString,
		wxDD_DIR_MUST_EXIST, wxDefaultPosition, this);
	if (!rootDir.IsEmpty()) {
		wxDir dir(rootDir);
		if (dir.IsOpened()) {
			wxString fileName;
			wxArrayString subDirs;
			bool cont = dir.GetFirst(&fileName, wxEmptyString, wxDIR_DIRS);
			while (cont) {
				subDirs.Add(fileName);
				cont = dir.GetNext(&fileName);
			}
			std::sort(subDirs.begin(), subDirs.end());
			wxArrayInt selections;
			cont = !subDirs.IsEmpty();
			if (!subDirs.IsEmpty()) {
				int choice = wxGetSelectedChoices(
					selections,
					wxString::Format(_("There are %ld projects. Please choose directories to create projects for"), subDirs.size()),
					_("Add Multiple"),
					subDirs,
					this);
				if (choice > 0) {
					cont = !selections.IsEmpty();
				}
			}
			if (cont) {
				for (size_t i = 0; i < selections.size(); ++i) {
					wxString chosenSubDir = subDirs[selections[i]];

					t4p::ProjectClass project;
					t4p::SourceClass newSrc;
					newSrc.RootDirectory.AssignDir(rootDir);
					newSrc.RootDirectory.AppendDir(chosenSubDir);
					newSrc.SetIncludeWildcards(wxT("*.*"));
					newSrc.SetExcludeWildcards(wxT(""));
					project.AddSource(newSrc);
					project.IsEnabled = true;
					project.Label = chosenSubDir;

					AddProject(project);
				}
			} else if (subDirs.empty()) {
				wxMessageBox(_("Directory does not contain any sub-directories"),
					_("Add Projects From Directory"), wxCENTRE, this);
			}
		}
	}
}

void t4p::ProjectListDialogClass::OnHelpButton(wxCommandEvent& event) {
	wxString help = wxString::FromAscii(
		"Add: Will show the new project dialog\n"
		"Remove: Will remove the selected project\n"
		"Edit: Will edit the selected project\n"
		"Add Multiple: Will create one project for each sub-directory of a given directory\n"
	);
	help = wxGetTranslation(help);
	wxMessageBox(help, _("Defined Projects Help"), wxCENTRE, this);
}

t4p::MultipleSelectDialogClass::MultipleSelectDialogClass(wxWindow* parent, const wxString& title, const wxString& caption,
																std::vector<wxString>& choices, std::vector<int>& selections)
: MultipleSelectDialogGeneratedClass(parent, wxID_ANY, title)
, Selections(selections) {
	Label->SetLabel(caption);
	for (size_t i = 0; i < choices.size(); ++i) {
		Checklist->Append(choices[i]);
	}
}

void t4p::MultipleSelectDialogClass::OnOkButton(wxCommandEvent& event) {
	for (size_t i = 0; i < Checklist->GetCount(); ++i) {
		if (Checklist->IsChecked(i)) {
			Selections.push_back(i);
		}
	}
	EndModal(wxOK);
}

void t4p::MultipleSelectDialogClass::OnCancelButton(wxCommandEvent& event) {
	EndModal(wxCANCEL);
}

BEGIN_EVENT_TABLE(t4p::ProjectViewClass, t4p::FeatureViewClass)
	EVT_MENU(t4p::MENU_PROJECT + 0, t4p::ProjectViewClass::OnProjectDefine)
	EVT_MENU(t4p::MENU_PROJECT + 1, t4p::ProjectViewClass::OnCreateNewProject)
END_EVENT_TABLE()
