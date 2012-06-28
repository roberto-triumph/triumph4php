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
#include <wx/stdpaths.h>
#include <algorithm>

static const int ID_FRAMEWORK_DETECTION_GAUGE = wxNewId();
static const int MAX_PROJECT_HISTORY = 5;

mvceditor::ProjectPluginClass::ProjectPluginClass() 
	: PluginClass()
	, PhpFileFiltersString()
	, CssFileFiltersString()
	, SqlFileFiltersString()
	, History(MAX_PROJECT_HISTORY, mvceditor::MENU_PROJECT)
	, PhpFrameworks(NULL)
	, RecentProjectsMenu(NULL) {
	PhpFileFiltersString = wxT("*.php");
	CssFileFiltersString = wxT("*.css");
	SqlFileFiltersString = wxT("*.sql");
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

void mvceditor::ProjectPluginClass::AddProjectMenuItems(wxMenu* projectMenu) {
	projectMenu->Append(mvceditor::MENU_PROJECT + MAX_PROJECT_HISTORY + 1, _("Explore"), _("Open An explorer window in the Project Root"), wxITEM_NORMAL);
	projectMenu->Append(mvceditor::MENU_PROJECT + MAX_PROJECT_HISTORY + 2, _("Explore Open File"), _("Open An explorer window in the currently opened file"), wxITEM_NORMAL);
	RecentProjectsMenu = new wxMenu(0);
	History.UseMenu(RecentProjectsMenu);
	projectMenu->Append(mvceditor::MENU_PROJECT + MAX_PROJECT_HISTORY + 3, _("Recent Projects"), RecentProjectsMenu, _("Open An explorer window in the Project Root"));
}

void mvceditor::ProjectPluginClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_PROJECT + MAX_PROJECT_HISTORY + 1] = wxT("Project-Explore");
	menuItemIds[mvceditor::MENU_PROJECT + MAX_PROJECT_HISTORY + 2] = wxT("Project-Explore File");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::ProjectPluginClass::AddToolBarItems(wxAuiToolBar* toolbar) {
	wxBitmap bitmap = wxArtProvider::GetBitmap(wxART_FOLDER_OPEN, wxART_TOOLBAR, wxSize(16, 16));
	toolbar->AddTool(mvceditor::MENU_PROJECT + MAX_PROJECT_HISTORY + 1, _("Explore"), bitmap, _("Open An explorer window in the Project Root"));
	bitmap = wxArtProvider::GetBitmap(wxART_FOLDER_OPEN, wxART_TOOLBAR, wxSize(16, 16));
	toolbar->AddTool(mvceditor::MENU_PROJECT + MAX_PROJECT_HISTORY + 2, _("Explore Open File"), bitmap, _("Open An explorer window in the currently opened file"));
}

void mvceditor::ProjectPluginClass::LoadPreferences(wxConfigBase* config) {
	config->Read(wxT("/Project/ExplorerExecutable"), &ExplorerExecutable);
	PhpFileFiltersString = config->Read(wxT("/Project/PhpFileFilters"), wxT("*.php"));
	CssFileFiltersString = config->Read(wxT("/Project/CssFileFilters"), wxT("*.css"));
	SqlFileFiltersString = config->Read(wxT("/Project/SqlFileFilters"), wxT("*.sql"));
	mvceditor::ProjectClass* project = GetProject();
	if (project) {
		project->SetPhpFileExtensionsString(PhpFileFiltersString);
		project->SetCssFileExtensionsString(CssFileFiltersString);
		project->SetSqlFileExtensionsString(SqlFileFiltersString);
	}


	// read the recent projects from a separate config file
	// the decision to keep the recent projects separate from the settings config file is because
	// the recent projects file changes more often, and there may be cases when config files can
	// be copied between different computer / people without affecting the recent files list
	wxStandardPaths paths;
	wxString recentFileConfigFilePath = paths.GetUserConfigDir() + wxFileName::GetPathSeparator() + wxT(".mvc_editor_projects.ini");
	wxFileName fileName(recentFileConfigFilePath);
	if (fileName.FileExists()) {
		wxFileInputStream input(recentFileConfigFilePath);
		if (input.IsOk()) {
			wxFileConfig recentFileConfig(input);
			History.Load(recentFileConfig);
		}
	}
	SyncMenu();
}

void mvceditor::ProjectPluginClass::SavePreferences(wxCommandEvent& event) {
	wxConfigBase* config = wxConfig::Get();
	config->Write(wxT("/Project/ExplorerExecutable"), ExplorerExecutable);
	config->Write(wxT("/Project/PhpFileFilters"), PhpFileFiltersString);
	config->Write(wxT("/Project/CssFileFilters"), CssFileFiltersString);
	config->Write(wxT("/Project/SqlFileFilters"), SqlFileFiltersString);
	
	mvceditor::ProjectClass* project = GetProject();
	if (project) {
		project->SetPhpFileExtensionsString(PhpFileFiltersString);
		project->SetCssFileExtensionsString(CssFileFiltersString);
		project->SetSqlFileExtensionsString(SqlFileFiltersString);
	}
}

void mvceditor::ProjectPluginClass::PersistProjectList() {

	// recent projects file will have 1 group per project; like this
	// 
	// [project_1]
	// Path = /home/user/projects/project_one
	//
	// [project_2]
	// Path = /home/user/projects/project_two
	//
	wxStandardPaths paths;
	wxString recentFileConfigFilePath = paths.GetUserConfigDir() + wxFileName::GetPathSeparator() + wxT(".mvc_editor_projects.ini");

	// attempted to use the style wxCONFIG_USE_NO_ESCAPE_CHARACTERS  but there seems to be no way to set this
	// flag when reading the file in
	wxFileConfig recentFileConfig(wxT("mvc_editor"), wxEmptyString, recentFileConfigFilePath, wxEmptyString, 
		wxCONFIG_USE_LOCAL_FILE);

	History.Save(recentFileConfig);
	wxFileOutputStream output(recentFileConfigFilePath);
	recentFileConfig.Save(output);
}

void mvceditor::ProjectPluginClass::SyncMenu() {
	if (RecentProjectsMenu) {
		//History.AddFilesToMenu(RecentProjectsMenu);
		
		// ATTN: possible problem. according to wxWidgets docs
		//     Append the submenu to the parent menu after you have added your menu items, or accelerators may 
		//     not be registered properly.
		//
		// wont worry about that for now since project menu items don't have accelerators automatically
	}
}

void mvceditor::ProjectPluginClass::OnMenu(wxCommandEvent& event) {
	size_t id = (size_t)event.GetId();
	if (id >= (size_t)mvceditor::MENU_PROJECT && id < (mvceditor::MENU_PROJECT + History.GetCount())) {
		size_t index = id - mvceditor::MENU_PROJECT;
		wxString project = History.GetHistoryFile(index);
		bool remove = false;
		if (wxFileName::FileExists(project)) {
			int ret = wxMessageBox(_("Path is not a directory. Remove from Recent list?\n") + project, _("Warning"), 
				wxCENTER | wxICON_ERROR | wxYES_NO);
			remove = wxYES == ret;
		}
		else if (!wxFileName::DirExists(project)) {
			int ret = wxMessageBox(_("Path no longer exists. Remove from Recent list?\n") + project, _("Warning"), 
				wxCENTER | wxICON_ERROR | wxYES_NO);
			remove = wxYES == ret;
		}
		else {
			ProjectOpen(project);
		}
		if (remove) {
			if (index < History.GetCount()) {
				History.RemoveFileFromHistory(index);
				PersistProjectList();
			}
		}
	}
	else {
		event.Skip();
	}
}

void mvceditor::ProjectPluginClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
	ProjectPluginPanelClass* panel = new ProjectPluginPanelClass(parent, *this);
	parent->AddPage(panel, wxT("Project"));
}

void mvceditor::ProjectPluginClass::OnProjectExplore(wxCommandEvent& event) {
	ProjectClass* project = GetProject();
	if (project != NULL && !GetProject()->GetRootPath().IsEmpty()) {
		wxString cmd;
		cmd += ExplorerExecutable;
		cmd += wxT(" \"");
		cmd += GetProject()->GetRootPath();
		cmd += wxT("\"");
		long result = wxExecute(cmd);
		if (!result) {
			mvceditor::EditorLogError(mvceditor::BAD_EXPLORER_EXCUTABLE, cmd);
		}
	}
	else {
		wxMessageBox(_("Need to open a project first in order for Explore to work."), _("Project Explore"));
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

void mvceditor::ProjectPluginClass::ProjectOpen(const wxString& directoryPath) {
	if (!PhpFrameworks.get()) {
		PhpFrameworks.reset(new mvceditor::PhpFrameworkDetectorClass(*this, RunningThreads, *GetEnvironment()));
	}
	CloseProject();
	ProjectOptionsClass options;
	options.RootPath = directoryPath;
	App->Project = new ProjectClass(options);

	// on startup the editor is in "non-project" mode (no root path)
	if (!directoryPath.IsEmpty()) {
		bool started = PhpFrameworks->Init(directoryPath);
		if (!started) {
			mvceditor::EditorLogError(mvceditor::BAD_PHP_EXECUTABLE, GetEnvironment()->Php.PhpExecutablePath); 
		}
		else {
			mvceditor::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
			gauge->AddGauge(_("Framework Detection"), ID_FRAMEWORK_DETECTION_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE, 0);
		}
	}
	else {
	
		App->Project->SetPhpFileExtensionsString(PhpFileFiltersString);
		App->Project->SetCssFileExtensionsString(CssFileFiltersString);
		App->Project->SetSqlFileExtensionsString(SqlFileFiltersString);
		
		// still notify the plugins of the new project at program startup
		// when the app starts the "empty" project is opened
		wxCommandEvent evt(mvceditor::EVENT_APP_PROJECT_OPENED);
		App->EventSink.Publish(evt);
	}
}

void mvceditor::ProjectPluginClass::CloseProject() {
	if (App->Project) {
		PhpFrameworks->ConfigFiles.clear();
		PhpFrameworks->Databases.clear();
		PhpFrameworks->Identifiers.clear();
		PhpFrameworks->Resources.clear();

		App->PhpFrameworks.ConfigFiles.clear();
		App->PhpFrameworks.Databases.clear();
		App->PhpFrameworks.Identifiers.clear();
		App->PhpFrameworks.Resources.clear();
		delete App->Project;
		App->Project = NULL;
		wxCommandEvent closeEvent(mvceditor::EVENT_APP_PROJECT_CLOSED);
		App->EventSink.Publish(closeEvent);
	}
}

void mvceditor::ProjectPluginClass::OnCmdProjectOpen(wxCommandEvent& event) {
	ProjectOpen(event.GetString());
}

void mvceditor::ProjectPluginClass::OnFrameworkDetectionComplete(wxCommandEvent& event) {
	App->PhpFrameworks.ConfigFiles = PhpFrameworks->ConfigFiles;
	App->PhpFrameworks.Databases = PhpFrameworks->Databases;
	App->PhpFrameworks.Identifiers = PhpFrameworks->Identifiers;
	App->PhpFrameworks.Resources = PhpFrameworks->Resources;
	
	App->Project->SetPhpFileExtensionsString(PhpFileFiltersString);
	App->Project->SetCssFileExtensionsString(CssFileFiltersString);
	App->Project->SetSqlFileExtensionsString(SqlFileFiltersString);
	
	wxString projectRoot = App->Project->GetRootPath();
	projectRoot.Trim();
	History.AddFileToHistory(projectRoot);
	PersistProjectList();
	
	wxCommandEvent projectOpenedEvent(mvceditor::EVENT_APP_PROJECT_OPENED);
	App->EventSink.Publish(projectOpenedEvent);
	mvceditor::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
	gauge->StopGauge(ID_FRAMEWORK_DETECTION_GAUGE);
}

void mvceditor::ProjectPluginClass::OnFrameworkDetectionFailed(wxCommandEvent& event) {
	mvceditor::EditorLogError(mvceditor::BAD_PHP_EXECUTABLE, event.GetString());

	App->PhpFrameworks.ConfigFiles = PhpFrameworks->ConfigFiles;
	App->PhpFrameworks.Databases = PhpFrameworks->Databases;
	App->PhpFrameworks.Identifiers = PhpFrameworks->Identifiers;
	App->PhpFrameworks.Resources = PhpFrameworks->Resources;
	
	App->Project->SetPhpFileExtensionsString(PhpFileFiltersString);
	App->Project->SetCssFileExtensionsString(CssFileFiltersString);
	App->Project->SetSqlFileExtensionsString(SqlFileFiltersString);
	
	wxString projectRoot = App->Project->GetRootPath();
	projectRoot.Trim();
	History.AddFileToHistory(projectRoot);
	PersistProjectList();

	// still need to set the project even if the project detection fails
	// pretty much all code depends on having a Project pointer
	wxCommandEvent projectOpenedEvent(mvceditor::EVENT_APP_PROJECT_OPENED);
	App->EventSink.Publish(projectOpenedEvent);
	mvceditor::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
	gauge->StopGauge(ID_FRAMEWORK_DETECTION_GAUGE);
}

void mvceditor::ProjectPluginClass::OnFrameworkDetectionInProgress(wxCommandEvent& event) {
	mvceditor::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
	gauge->IncrementGauge(ID_FRAMEWORK_DETECTION_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE);
}


mvceditor::ProjectPluginPanelClass::ProjectPluginPanelClass(wxWindow *parent, mvceditor::ProjectPluginClass &projectPlugin) 
: ProjectPluginGeneratedPanelClass(parent) {
	NonEmptyTextValidatorClass explorerValidator(&projectPlugin.ExplorerExecutable, Label);
	ExplorerExecutable->SetValidator(explorerValidator);

	NonEmptyTextValidatorClass phpFileFiltersValidator(&projectPlugin.PhpFileFiltersString, PhpLabel);
	PhpFileFilters->SetValidator(phpFileFiltersValidator);

	NonEmptyTextValidatorClass cssFileFiltersValidator(&projectPlugin.CssFileFiltersString, CssLabel);
	CssFileFilters->SetValidator(cssFileFiltersValidator);

	NonEmptyTextValidatorClass sqlFileFiltersValidator(&projectPlugin.SqlFileFiltersString, SqlLabel);
	SqlFileFilters->SetValidator(sqlFileFiltersValidator);
	
}

void mvceditor::ProjectPluginPanelClass::OnFileChanged(wxFileDirPickerEvent& event) {
	ExplorerExecutable->SetValue(event.GetPath());
	event.Skip();
}

BEGIN_EVENT_TABLE(mvceditor::ProjectPluginClass, wxEvtHandler)
	EVT_MENU(mvceditor::MENU_PROJECT + MAX_PROJECT_HISTORY + 1, mvceditor::ProjectPluginClass::OnProjectExplore)
	EVT_MENU(mvceditor::MENU_PROJECT + MAX_PROJECT_HISTORY + 2, mvceditor::ProjectPluginClass::OnProjectExploreOpenFile)

	/**
	 * Since there could be 1...N recent project menu items we cannot listen to one menu item's event
	 * we have to listen to all menu events
	 */
	EVT_MENU_RANGE(mvceditor::MENU_PROJECT, mvceditor::MENU_PROJECT + MAX_PROJECT_HISTORY, mvceditor::ProjectPluginClass::OnMenu)

	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_FRAMEWORK_DETECTION_COMPLETE, mvceditor::ProjectPluginClass::OnFrameworkDetectionComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_FRAMEWORK_DETECTION_FAILED, mvceditor::ProjectPluginClass::OnFrameworkDetectionFailed)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_PROCESS_IN_PROGRESS, mvceditor::ProjectPluginClass::OnFrameworkDetectionInProgress)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_CMD_OPEN_PROJECT, mvceditor::ProjectPluginClass::OnCmdProjectOpen)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_PREFERENCES_UPDATED, mvceditor::ProjectPluginClass::SavePreferences)
END_EVENT_TABLE()