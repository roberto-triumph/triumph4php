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
#include <plugins/ProjectPluginClass.h>
#include <widgets/NonEmptyTextValidatorClass.h>
#include <MvcEditorErrors.h>
#include <MvcEditor.h>
#include <wx/artprov.h>
#include <wx/platinfo.h>
#include <wx/filename.h>
#include <wx/fileconf.h>
#include <wx/valgen.h>
#include <wx/dir.h>
#include <wx/dirdlg.h>
#include <algorithm>

static const int ID_FRAMEWORK_DETECTION_GAUGE = wxNewId();

mvceditor::ProjectPluginClass::ProjectPluginClass(mvceditor::AppClass& app) 
	: PluginClass(app)
	, PhpFrameworks(*this, app.RunningThreads, app.Structs.Environment) 
	, DirectoriesToDetect() {
	wxPlatformInfo info;
	switch (info.GetOperatingSystemId()) {
		case wxOS_WINDOWS_NT:
			ExplorerExecutable = wxT("explorer.exe");
			break;
		case wxOS_UNIX:
		case wxOS_UNIX_LINUX:
			ExplorerExecutable = wxT("nautilus");
			break;
		default:
			ExplorerExecutable = wxT("explorer");
	}
}

mvceditor::ProjectPluginClass::~ProjectPluginClass() {
	PhpFrameworks.Stop();
}

void mvceditor::ProjectPluginClass::AddFileMenuItems(wxMenu* fileMenu) {
	fileMenu->Append(mvceditor::MENU_PROJECT + 3, _("Projects"), _("Add additional source directories to the current project"), wxITEM_NORMAL);
	fileMenu->Append(mvceditor::MENU_PROJECT + 1, _("Explore"), _("Open An explorer window in the Project Root"), wxITEM_NORMAL);
	fileMenu->Append(mvceditor::MENU_PROJECT + 2, _("Explore Open File"), _("Open An explorer window in the currently opened file"), wxITEM_NORMAL);
}


void mvceditor::ProjectPluginClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_PROJECT + 1] = wxT("Project-Explore");
	menuItemIds[mvceditor::MENU_PROJECT + 2] = wxT("Project-Explore File");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::ProjectPluginClass::AddToolBarItems(wxAuiToolBar* toolbar) {
	wxBitmap bitmap = wxArtProvider::GetBitmap(wxART_FOLDER_OPEN, wxART_TOOLBAR, wxSize(16, 16));
	toolbar->AddTool(mvceditor::MENU_PROJECT + 1, _("Explore"), bitmap, _("Open An explorer window in the Project Root"));
	bitmap = wxArtProvider::GetBitmap(wxART_FOLDER_OPEN, wxART_TOOLBAR, wxSize(16, 16));
	toolbar->AddTool(mvceditor::MENU_PROJECT + 2, _("Explore Open File"), bitmap, _("Open An explorer window in the currently opened file"));
}

void mvceditor::ProjectPluginClass::LoadPreferences(wxConfigBase* config) {
	config->Read(wxT("/Project/ExplorerExecutable"), &ExplorerExecutable);
	App.Structs.PhpFileExtensionsString = config->Read(wxT("/Project/PhpFileExtensions"));
	App.Structs.CssFileExtensionsString = config->Read(wxT("/Project/CssFileExtensions"));
	App.Structs.SqlFileExtensionsString = config->Read(wxT("/Project/SqlFileExtensions"));
	App.Structs.MiscFileExtensionsString = config->Read(wxT("/Project/MiscFileExtensions"));
	
	App.Structs.Projects.clear();
	wxString key;
	long index;
	int projectIndex = 0;
	bool next = config->GetFirstGroup(key, index);
	while (next) {
		if (key.Find(wxT("Project_")) == 0) {
			mvceditor::ProjectClass newProject;
			int sourcesCount = 0;

			wxString keyLabel = wxString::Format(wxT("/Project_%d/Label"), projectIndex);
			wxString keyEnabled = wxString::Format(wxT("/Project_%d/IsEnabled"), projectIndex);
			wxString keyResourceDbFileName = wxString::Format(wxT("/Project_%d/ResourceDbFileName"), projectIndex);
			wxString keySourceCount = wxString::Format(wxT("/Project_%d/SourceCount"), projectIndex);
			config->Read(keyLabel, &newProject.Label);
			config->Read(keyEnabled, &newProject.IsEnabled);
			newProject.ResourceDbFileName.Assign(config->Read(keyResourceDbFileName));
			config->Read(keySourceCount, &sourcesCount);
			for (int j = 0; j < sourcesCount; ++j) {
				wxString keyRootPath = wxString::Format(wxT("/Project_%d/Source_%d_RootDirectory"), projectIndex, j);
				wxString keyInclude = wxString::Format(wxT("/Project_%d/Source_%d_IncludeWildcards"), projectIndex, j);
				wxString keyExclude = wxString::Format(wxT("/Project_%d/Source_%d_ExcludeWildcards"), projectIndex, j);

				mvceditor::SourceClass src;
				wxString rootDir = config->Read(keyRootPath);
				wxString includeWildcards = config->Read(keyInclude);
				wxString excludeWildcards = config->Read(keyExclude);
				
				src.RootDirectory.AssignDir(rootDir);
				src.SetIncludeWildcards(includeWildcards);
				src.SetExcludeWildcards(excludeWildcards);
				if (src.RootDirectory.IsOk()) {
					newProject.AddSource(src);
				}
			}
			if (newProject.HasSources()) {
				App.Structs.AssignFileExtensions(newProject);
				App.Structs.Projects.push_back(newProject);
				projectIndex++;
			}
		}
		next = config->GetNextGroup(key, index);
	}
}

void mvceditor::ProjectPluginClass::SavePreferences(wxCommandEvent& event) {
	wxConfigBase* config = wxConfig::Get();
	config->Write(wxT("/Project/ExplorerExecutable"), ExplorerExecutable);
	config->Write(wxT("/Project/PhpFileExtensions"), App.Structs.PhpFileExtensionsString);
	config->Write(wxT("/Project/CssFileExtensions"), App.Structs.CssFileExtensionsString);
	config->Write(wxT("/Project/SqlFileExtensions"), App.Structs.SqlFileExtensionsString);
	config->Write(wxT("/Project/MiscFileExtensions"), App.Structs.MiscFileExtensionsString);

	// remove all project from the config
	wxString key;
	long index = 0;
	bool next = config->GetFirstGroup(key, index);
	std::vector<wxString> keysToDelete;
	while (next) {
		if (key.Find(wxT("Project_")) == 0) {
			keysToDelete.push_back(key);
		}
		next = config->GetNextGroup(key, index);
	}
	for (size_t i = 0; i < keysToDelete.size(); ++i) {
		config->DeleteGroup(keysToDelete[i]);
	}
	
	for (size_t i = 0; i < App.Structs.Projects.size(); ++i) {
		mvceditor::ProjectClass project = App.Structs.Projects[i];
		wxString keyLabel = wxString::Format(wxT("/Project_%d/Label"), i);
		wxString keyEnabled = wxString::Format(wxT("/Project_%d/IsEnabled"), i);
		wxString keyResourceDbFileName = wxString::Format(wxT("/Project_%d/ResourceDbFileName"), i);	
		wxString keySourceCount = wxString::Format(wxT("/Project_%d/SourceCount"), i);
		config->Write(keyLabel, project.Label);
		config->Write(keyEnabled, project.IsEnabled);
		config->Write(keyResourceDbFileName, project.ResourceDbFileName.GetFullPath());
		config->Write(keySourceCount, (int)project.Sources.size());
		for (size_t j = 0; j < project.Sources.size(); ++j) {
			mvceditor::SourceClass source = project.Sources[j];			
			wxString keyRootPath = wxString::Format(wxT("/Project_%d/Source_%d_RootDirectory"), i, j);
			wxString keyInclude = wxString::Format(wxT("/Project_%d/Source_%d_IncludeWildcards"), i, j);
			wxString keyExclude = wxString::Format(wxT("/Project_%d/Source_%d_ExcludeWildcards"), i, j);			
			config->Write(keyRootPath, source.RootDirectory.GetFullPath());
			config->Write(keyInclude, source.IncludeWildcardsString());
			config->Write(keyExclude, source.ExcludeWildcardsString());
		}
	}

	// also, update the projects to have new file extesions
	std::vector<mvceditor::ProjectClass>::iterator project;
	for (project = App.Structs.Projects.begin(); project != App.Structs.Projects.end(); ++project) {
		App.Structs.AssignFileExtensions(*project);
	}
}

void mvceditor::ProjectPluginClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
	ProjectPluginPanelClass* panel = new ProjectPluginPanelClass(parent, *this);
	parent->AddPage(panel, wxT("Project"));
}

void mvceditor::ProjectPluginClass::OnProjectExplore(wxCommandEvent& event) {
	std::vector<mvceditor::SourceClass> enabledSources = App.Structs.AllEnabledSources();
	if (!enabledSources.empty()) {
		wxFileName dir = enabledSources[0].RootDirectory;
		wxString cmd;
		cmd += ExplorerExecutable;
		cmd += wxT(" \"");
		cmd += dir.GetPath();
		cmd += wxT("\"");
		long result = wxExecute(cmd);
		if (!result) {
			mvceditor::EditorLogError(mvceditor::BAD_EXPLORER_EXCUTABLE, cmd);
		}
	}
	else {
		wxMessageBox(_("Need to define a project first in order for Explore to work."), _("Project Explore"));
	}
}

void mvceditor::ProjectPluginClass::OnProjectExploreOpenFile(wxCommandEvent& event) {
	CodeControlClass* code = GetCurrentCodeControl();
	if (code != NULL && !code->GetFileName().IsEmpty()) {
		wxFileName fileName(code->GetFileName());
		wxString cmd;
		cmd += ExplorerExecutable;
		cmd += wxT(" \"");
		cmd += fileName.GetPath();
		cmd += wxT("\"");
		long result = wxExecute(cmd);
		if (!result) {
			mvceditor::EditorLogError(mvceditor::BAD_EXPLORER_EXCUTABLE, cmd);
		}
	}
	else {
		wxMessageBox(_("Need to open a file first."), _("Project Explore"));
	}
}

void mvceditor::ProjectPluginClass::OnAppReady(wxCommandEvent& event) {
	DirectoriesToDetect.clear();
	std::vector<mvceditor::SourceClass> allSources = App.Structs.AllEnabledSources();
	if (!allSources.empty()) {

		for (size_t i = 1; i < allSources.size(); ++i) {
			DirectoriesToDetect.push_back(allSources[i].RootDirectory.GetPath());
		}
		bool started = PhpFrameworks.Init(allSources[0].RootDirectory.GetPath());
		if (!started) {
			mvceditor::EditorLogError(mvceditor::BAD_PHP_EXECUTABLE, App.Structs.Environment.Php.PhpExecutablePath); 
			DirectoriesToDetect.clear();
		}
		else {
			mvceditor::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
			gauge->AddGauge(_("Framework Detection"), ID_FRAMEWORK_DETECTION_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE, 0);
		}
	}
	else {

		// still notify the plugins of the new project at program startup
		// when the app starts the default project is opened
		wxCommandEvent evt(mvceditor::EVENT_APP_PROJECTS_UPDATED);
		App.EventSink.Publish(evt);
	}
}

void mvceditor::ProjectPluginClass::OnFrameworkDetectionComplete(wxCommandEvent& event) {
	bool finished = true;

	// detection on the next directory; skipping any errors
	while (!DirectoriesToDetect.empty() && finished) {
		wxString nextDirectory = DirectoriesToDetect.back();
		DirectoriesToDetect.pop_back();
		if (PhpFrameworks.Init(nextDirectory)) {
			finished = false;
		}
	}
	if (finished) {	
		wxCommandEvent projectOpenedEvent(mvceditor::EVENT_APP_PROJECTS_UPDATED);
		App.EventSink.Publish(projectOpenedEvent);
		mvceditor::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
		gauge->StopGauge(ID_FRAMEWORK_DETECTION_GAUGE);
	}
}

void mvceditor::ProjectPluginClass::OnFrameworkDetectionFailed(wxCommandEvent& event) {
	mvceditor::EditorLogError(mvceditor::BAD_PHP_EXECUTABLE, event.GetString());
	bool finished = true;

	// detection on the next directory; skipping any errors
	while (!DirectoriesToDetect.empty() && finished) {
		wxString nextDirectory = DirectoriesToDetect.back();
		DirectoriesToDetect.pop_back();
		if (PhpFrameworks.Init(nextDirectory)) {
			finished = false;
		}
	}
	if (finished) {
		wxCommandEvent projectOpenedEvent(mvceditor::EVENT_APP_PROJECTS_UPDATED);
		App.EventSink.Publish(projectOpenedEvent);
		mvceditor::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
		gauge->StopGauge(ID_FRAMEWORK_DETECTION_GAUGE);
	}
}

void mvceditor::ProjectPluginClass::OnFrameworkDetectionInProgress(wxCommandEvent& event) {
	mvceditor::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
	gauge->IncrementGauge(ID_FRAMEWORK_DETECTION_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE);
}

void mvceditor::ProjectPluginClass::OnFrameworkFound(mvceditor::FrameworkFoundEventClass& event) {
	App.Structs.Frameworks.push_back(event.Framework);
}

void mvceditor::ProjectPluginClass::OnProjectDefine(wxCommandEvent& event) {
	std::vector<mvceditor::ProjectClass> removedProjects;
	mvceditor::ProjectListDialogClass dialog(GetMainWindow(), App.Structs.Projects, removedProjects);
	if (wxOK == dialog.ShowModal()) {
		std::vector<mvceditor::ProjectClass>::iterator project;

		// delete the cache files for the projects the user has removed
		// before deleting the file, must disconnect from the SQLite database
		mvceditor::ResourceCacheClass* cache = GetResourceCache();
		for (project = removedProjects.begin(); project != removedProjects.end(); ++project) {
			cache->RemoveGlobal(project->ResourceDbFileName);
			project->RemoveResourceDb();
		}

		// for new projects we need to fill in the file extensions
		// the rest of the app assumes they are already filled in
		for (project = App.Structs.Projects.begin(); project != App.Structs.Projects.end(); ++project) {

			// need to set these; as they set in app load too
			App.Structs.AssignFileExtensions(*project);
		}

		wxCommandEvent evt;
		SavePreferences(evt);
		wxConfigBase* config = wxConfig::Get();
		config->Flush();

		// re-start the project opening cycle
		// clear the detected framework info
		App.Structs.Frameworks.clear();

		// only remove the database infos that were detected from
		// PHP frameworks, leave the ones that the user created intact
		App.Structs.ClearDetectedInfos();
		wxCommandEvent readyEvt(mvceditor::EVENT_APP_READY);
		OnAppReady(readyEvt);
	}
}

mvceditor::ProjectPluginPanelClass::ProjectPluginPanelClass(wxWindow *parent, mvceditor::ProjectPluginClass &projectPlugin) 
: ProjectPluginGeneratedPanelClass(parent) {
	NonEmptyTextValidatorClass explorerValidator(&projectPlugin.ExplorerExecutable, Label);
	ExplorerExecutable->SetValidator(explorerValidator);

	NonEmptyTextValidatorClass phpFileExtensionsValidator(&projectPlugin.App.Structs.PhpFileExtensionsString, PhpLabel);
	PhpFileExtensions->SetValidator(phpFileExtensionsValidator);

	NonEmptyTextValidatorClass cssFileExtensionsValidator(&projectPlugin.App.Structs.CssFileExtensionsString, CssLabel);
	CssFileExtensions->SetValidator(cssFileExtensionsValidator);

	NonEmptyTextValidatorClass sqlFileExtensionsValidator(&projectPlugin.App.Structs.SqlFileExtensionsString, SqlLabel);
	SqlFileExtensions->SetValidator(sqlFileExtensionsValidator);
	
	NonEmptyTextValidatorClass miscFileExtensionsValidator(&projectPlugin.App.Structs.MiscFileExtensionsString, MiscLabel);
	MiscFileExtensions->SetValidator(miscFileExtensionsValidator);
}

void mvceditor::ProjectPluginPanelClass::OnFileChanged(wxFileDirPickerEvent& event) {
	ExplorerExecutable->SetValue(event.GetPath());
	event.Skip();
}

mvceditor::ProjectDefinitionDialogClass::ProjectDefinitionDialogClass(wxWindow* parent, mvceditor::ProjectClass& project)
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

void mvceditor::ProjectDefinitionDialogClass::OnAddSource(wxCommandEvent& event) {
	mvceditor::SourceClass newSrc;
	mvceditor::ProjectSourceDialogClass dialog(this, newSrc);
	if (wxOK == dialog.ShowModal()) {
		EditedProject.AddSource(newSrc);
		SourcesList->Append(newSrc.RootDirectory.GetFullPath());
	}
}

void mvceditor::ProjectDefinitionDialogClass::OnEditSource(wxCommandEvent& event) {
	size_t selected = SourcesList->GetSelection();
	if (selected >= 0 && selected < EditedProject.Sources.size()) {
		mvceditor::SourceClass src = EditedProject.Sources[selected];
		mvceditor::ProjectSourceDialogClass dialog(this, src);
		if (wxOK == dialog.ShowModal()) {
			SourcesList->SetString(selected, src.RootDirectory.GetFullPath());
			EditedProject.Sources[selected] = src;
		}
	}
}

void mvceditor::ProjectDefinitionDialogClass::OnRemoveSource(wxCommandEvent& event) {
	size_t selected = SourcesList->GetSelection();
	if (selected >= 0 && selected < EditedProject.Sources.size()) {
		mvceditor::SourceClass src = EditedProject.Sources[selected];
		wxString msg = _("Are you sure you wish to remove the source? ");
		msg += src.RootDirectory.GetFullPath();
		msg += wxT("\n");
		msg += 
			_("MVC Editor will no longer open or index files in the directory. Note that the directory is not actually deleted from the file system");
		wxString caption = _("Remove Project Source");
		int response = wxMessageBox(msg, caption, wxYES_NO, this);
		if (wxYES == response) {
			SourcesList->Delete(selected);
			EditedProject.Sources.erase(EditedProject.Sources.begin() + selected);
		}
	}
}

void mvceditor::ProjectDefinitionDialogClass::OnOkButton(wxCommandEvent& event) {
	TransferDataFromWindow();
	if (!EditedProject.HasSources()) {
		wxMessageBox(_("Project must have at least one source directory"));
		return;
	}
	Project = EditedProject;
	EndModal(wxOK);
}

void mvceditor::ProjectDefinitionDialogClass::OnCancelButton(wxCommandEvent& event) {
	EndModal(wxCANCEL);
}

void mvceditor::ProjectDefinitionDialogClass::OnSourcesListDoubleClick(wxCommandEvent& event) {
	size_t selected = event.GetSelection();
	if (selected >= 0 && selected < EditedProject.Sources.size()) {
		mvceditor::SourceClass src = EditedProject.Sources[selected];
		mvceditor::ProjectSourceDialogClass dialog(this, src);
		if (wxOK == dialog.ShowModal()) {
			SourcesList->SetString(selected, src.RootDirectory.GetFullPath());
			EditedProject.Sources[selected] = src;
		}
	}
	event.Skip();
}

void mvceditor::ProjectDefinitionDialogClass::Populate() {
	for (size_t i = 0; i < EditedProject.Sources.size(); ++i) {
		SourcesList->Append(EditedProject.Sources[i].RootDirectory.GetFullPath());
	}
}

mvceditor::ProjectSourceDialogClass::ProjectSourceDialogClass(wxWindow* parent, mvceditor::SourceClass& source)
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

void mvceditor::ProjectSourceDialogClass::OnOkButton(wxCommandEvent& event) {
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

void mvceditor::ProjectSourceDialogClass::OnCancelButton(wxCommandEvent& event) {
	EndModal(wxCANCEL);
}

mvceditor::ProjectListDialogClass::ProjectListDialogClass(wxWindow* parent, std::vector<mvceditor::ProjectClass>& projects,
														  std::vector<mvceditor::ProjectClass>& removedProjects)
	: ProjectListDialogGeneratedClass(parent)
	, Projects(projects)
	, EditedProjects(projects)
	, RemovedProjects(removedProjects) {
	Populate();
	if (!ProjectsList->IsEmpty()) {
		ProjectsList->SetSelection(0);
	}
}

void mvceditor::ProjectListDialogClass::OnProjectsListDoubleClick(wxCommandEvent& event) {
	size_t selection = event.GetSelection();
	if (selection >= 0 && selection < EditedProjects.size()) {
		mvceditor::ProjectClass project = EditedProjects[selection];
		mvceditor::ProjectDefinitionDialogClass dialog(this, project);
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

void mvceditor::ProjectListDialogClass::OnProjectsListCheckbox(wxCommandEvent& event) {
	size_t selection = event.GetSelection();
	if (selection >= 0 && selection < EditedProjects.size()) {
		mvceditor::ProjectClass project = EditedProjects[selection];
		project.IsEnabled = ProjectsList->IsChecked(selection);
		EditedProjects[selection] = project;
	}
	event.Skip();
}

void mvceditor::ProjectListDialogClass::OnAddButton(wxCommandEvent& event) {
	mvceditor::ProjectClass project;
	mvceditor::ProjectDefinitionDialogClass dialog(this, project);
	if (wxOK == dialog.ShowModal()) {
		AddProject(project);
	}
}

void mvceditor::ProjectListDialogClass::AddProject(const mvceditor::ProjectClass& project) {
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

void mvceditor::ProjectListDialogClass::OnRemoveButton(wxCommandEvent& event) {
	size_t selection = ProjectsList->GetSelection();
	if (selection >= 0 && selection < EditedProjects.size()) {
		mvceditor::ProjectClass project = EditedProjects[selection];
		wxString msg = _("Are you sure you wish to remove the project\n\n");
		msg += project.Label;
		msg += wxT("\n\n");
		msg += _("MVC Editor will no longer open or index files in any sources of this project. Note that no directories are actually deleted from the file system");
		wxString caption = _("Remove Project");
		int response = wxMessageBox(msg, caption, wxYES_NO);
		if (wxYES == response) {
			RemovedProjects.push_back(EditedProjects[selection]);
			EditedProjects.erase(EditedProjects.begin() + selection);
			size_t oldSelection = ProjectsList->GetSelection();
			ProjectsList->Delete(selection);
			if (oldSelection < ProjectsList->GetCount()) {
				ProjectsList->SetSelection(oldSelection);
			}
			else if (!ProjectsList->IsEmpty()) {
				ProjectsList->SetSelection(ProjectsList->GetCount() - 1);
			}
		}
	}
}

void mvceditor::ProjectListDialogClass::OnEditButton(wxCommandEvent& event) {
	size_t selection = ProjectsList->GetSelection();
	if (selection >= 0 && selection < EditedProjects.size()) {
		mvceditor::ProjectClass project = EditedProjects[selection];
		mvceditor::ProjectDefinitionDialogClass dialog(this, project);
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

void mvceditor::ProjectListDialogClass::OnOkButton(wxCommandEvent& event) {
	Projects = EditedProjects;
	for (size_t i = 0; i < Projects.size(); ++i) {
		Projects[i].MakeResourceDbFileName();
	}
	EndModal(wxOK);
}

void mvceditor::ProjectListDialogClass::OnCancelButton(wxCommandEvent& event) {
	EndModal(wxCANCEL);
}

void mvceditor::ProjectListDialogClass::Populate() {
	ProjectsList->Clear();
	wxPlatformInfo info = wxPlatformInfo::Get();
	for (size_t i = 0; i < EditedProjects.size(); ++i) {
		mvceditor::ProjectClass project = EditedProjects[i];
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

void mvceditor::ProjectListDialogClass::OnAddFromDirectoryButton(wxCommandEvent& event) {
	wxString rootDir = wxDirSelector(_("Choose the directory where your projects are located"), wxEmptyString,
		wxDD_DIR_MUST_EXIST, wxDefaultPosition, this);
	if (!rootDir.IsEmpty()) {
		wxDir dir(rootDir);
		if (dir.IsOpened()) {
			wxString fileName;
			std::vector<wxString> subDirs;
			bool cont = dir.GetFirst(&fileName, wxEmptyString, wxDIR_DIRS);
			while (cont) {
				subDirs.push_back(fileName);
				cont = dir.GetNext(&fileName);
			}
			cont = !subDirs.empty();
			if (!subDirs.empty()) {
				int res = wxMessageBox(
					wxString::Format(_("There are %d projects. Add them all?"), subDirs.size()), 
					_("Add Projects From Directory"), wxCENTRE | wxYES_NO,  this);
				cont = wxYES == res;
			}
			if (cont) {
				for (size_t i = 0; i < subDirs.size(); ++i) {
					mvceditor::ProjectClass project;
					mvceditor::SourceClass newSrc;
					newSrc.RootDirectory.AssignDir(rootDir);
					newSrc.RootDirectory.AppendDir(subDirs[i]);
					newSrc.SetIncludeWildcards(wxT("*.*"));
					newSrc.SetExcludeWildcards(wxT(""));
					project.AddSource(newSrc);
					project.IsEnabled = true;
					project.Label = subDirs[i];
				
					AddProject(project);
				}
			}
			else if (subDirs.empty()) {
				wxMessageBox(_("Directory does not contain any sub-directories"), 
					_("Add Projects From Directory"), wxCENTRE, this);
			}
		}
	}
}

void mvceditor::ProjectListDialogClass::OnHelpButton(wxCommandEvent& event) {
	wxString help = wxString::FromAscii(
		"Add: Will show the new project dialog\n"
		"Remove: Will remove the selected project\n"
		"Edit: Will edit the selected project\n"
		"Add Multiple: Will create one project for each sub-directory of a given directory\n"
	);
	help = wxGetTranslation(help);
	wxMessageBox(help, _("Defined Projects Help"), wxCENTRE, this);
}

BEGIN_EVENT_TABLE(mvceditor::ProjectPluginClass, wxEvtHandler)
	EVT_MENU(mvceditor::MENU_PROJECT + 1, mvceditor::ProjectPluginClass::OnProjectExplore)
	EVT_MENU(mvceditor::MENU_PROJECT + 2, mvceditor::ProjectPluginClass::OnProjectExploreOpenFile)
	EVT_MENU(mvceditor::MENU_PROJECT + 3, mvceditor::ProjectPluginClass::OnProjectDefine)

	EVT_FRAMEWORK_FOUND(mvceditor::ProjectPluginClass::OnFrameworkFound)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_FRAMEWORK_DETECTION_COMPLETE, mvceditor::ProjectPluginClass::OnFrameworkDetectionComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_FRAMEWORK_DETECTION_FAILED, mvceditor::ProjectPluginClass::OnFrameworkDetectionFailed)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_PROCESS_IN_PROGRESS, mvceditor::ProjectPluginClass::OnFrameworkDetectionInProgress)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_PREFERENCES_UPDATED, mvceditor::ProjectPluginClass::SavePreferences)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_READY, mvceditor::ProjectPluginClass::OnAppReady)
END_EVENT_TABLE()