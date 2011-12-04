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
#include <wx/artprov.h>
#include <wx/platinfo.h>
#include <wx/filename.h>
#include <wx/fileconf.h>
#include <wx/stdpaths.h>
#include <algorithm>

int ID_MENU_PROJECT_EXPLORE = mvceditor::PluginClass::newMenuId();
int ID_MENU_PROJECT_EXPLORE_OPEN_FILE = mvceditor::PluginClass::newMenuId();
int ID_MENU_PROJECT_RECENT = mvceditor::PluginClass::newMenuId();
int ID_MENU_PROJECT_SUB_MENU = wxNewId();

// this ID signals the start of the recent project menu IDs
// don't want to use the wxNewId() function because we want to use a continuous 
// set of IDs for the recent projects menu items
// if we used wxNewId() then we could have ID collisions with the code that call 
// wxNewId() next
int ID_MENU_PROJECT_ITEM = 2000;
int ID_TOOLBAR_PROJECT_EXPLORE = wxNewId();
int ID_TOOLBAR_PROJECT_EXPLORE_OPEN_FILE = wxNewId();


mvceditor::ProjectPluginClass::ProjectPluginClass() 
	: PluginClass()
	, RecentProjects()
	, RecentProjectsMenu(NULL)
	, MAX_RECENT_PROJECTS(5) {
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
	projectMenu->Append(ID_MENU_PROJECT_EXPLORE, _("Explore"), _("Open An explorer window in the Project Root"), wxITEM_NORMAL);
	projectMenu->Append(ID_MENU_PROJECT_EXPLORE_OPEN_FILE, _("Explore Open File"), _("Open An explorer window in the currently opened file"), wxITEM_NORMAL);
	RecentProjectsMenu = new wxMenu(0);
	projectMenu->Append(ID_MENU_PROJECT_SUB_MENU, _("Recent Projects"), RecentProjectsMenu, _("Open An explorer window in the Project Root"));
	SyncMenu();
}

void mvceditor::ProjectPluginClass::AddToolBarItems(wxAuiToolBar* toolbar) {
	wxBitmap bitmap = wxArtProvider::GetBitmap(wxART_FOLDER_OPEN, wxART_TOOLBAR, wxSize(16, 16));
	toolbar->AddTool(ID_TOOLBAR_PROJECT_EXPLORE, _("Explore"), bitmap, _("Open An explorer window in the Project Root"));
	bitmap = wxArtProvider::GetBitmap(wxART_FOLDER_OPEN, wxART_TOOLBAR, wxSize(16, 16));
	toolbar->AddTool(ID_TOOLBAR_PROJECT_EXPLORE_OPEN_FILE, _("Explore Open File"), bitmap, _("Open An explorer window in the currently opened file"));
}

void mvceditor::ProjectPluginClass::LoadPreferences(wxConfigBase* config) {
	config->Read(wxT("Project/ExplorerExecutable"), &ExplorerExecutable);

	// read the recent projects from a separate config file
	// the decision to keep the recent projects separate from the settings config file is because
	// the recent projects file changes more often, and there may be cases when config files can
	// be copied between different computer / people without affecting the recent files list
	//
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
	wxFileName fileName(recentFileConfigFilePath);
	if (fileName.FileExists()) {
		wxFileInputStream input(recentFileConfigFilePath);
		if (input.IsOk()) {
			wxFileConfig recentFileConfig(input);
			wxString groupName;
			long index;
			bool good =  recentFileConfig.GetFirstGroup(groupName, index);
			while (good) {
				wxString pathKey = groupName + wxT("/Path");
				wxString projectPath;
				if (recentFileConfig.Read(pathKey, &projectPath)) {
					RecentProjects.push_back(projectPath);
				}
				good = recentFileConfig.GetNextGroup(groupName, index);
			}
		}
	}
	SyncMenu();
}

void mvceditor::ProjectPluginClass::SavePreferences(wxConfigBase* config) {
	config->Write(wxT("Project/ExplorerExecutable"), ExplorerExecutable);
}

void mvceditor::ProjectPluginClass::OnProjectOpened() {
	wxString projectRoot = GetProject()->GetRootPath();
	projectRoot.Trim();
	if (projectRoot.IsEmpty()) {

		// this happens when the application starts
		return;
	}
	std::vector<wxString>::iterator found = std::find(RecentProjects.begin(), RecentProjects.end(), projectRoot);
	if (found != RecentProjects.end() && RecentProjects.size() < (size_t)MAX_RECENT_PROJECTS) {
		RecentProjects.erase(found);
		RecentProjects.insert(RecentProjects.begin(), projectRoot);
	}
	else if (RecentProjects.size() < (size_t)MAX_RECENT_PROJECTS) {
		RecentProjects.push_back(projectRoot);
	}
	SyncMenu();
	PersistProjectList();
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

	// remove all existing items; this is in case the list has shrunk from last time it was persisted
	recentFileConfig.DeleteAll();
	wxString groupName;
	for (size_t i = 0; i < RecentProjects.size(); i++) {
		recentFileConfig.SetPath(wxString::Format(wxT("/Project_%d"), i));
		recentFileConfig.Write(wxT("Path"), RecentProjects[i]);
	}
	wxFileOutputStream output(recentFileConfigFilePath);
	recentFileConfig.Save(output);
}

void mvceditor::ProjectPluginClass::SyncMenu() {
	if (RecentProjectsMenu) {
		wxMenuItemList list = RecentProjectsMenu->GetMenuItems();
		for (size_t i = 0; i < list.size(); i++) {
			RecentProjectsMenu->Delete(list[i]);
		}
		for (size_t i = 0; i < RecentProjects.size(); i++) {
			RecentProjectsMenu->Append(ID_MENU_PROJECT_ITEM + i, RecentProjects[i], _("Open the project at ") + RecentProjects[i]);
		}
		
		// ATTN: possible problem. according to wxWidgets docs
		//     Append the submenu to the parent menu after you have added your menu items, or accelerators may 
		//     not be registered properly.
		//
		// wont worry about that for now since project menu items don't have accelerators automatically
	}
}

void mvceditor::ProjectPluginClass::OnMenu(wxCommandEvent& event) {
	int id = event.GetId();
	if (id >= ID_MENU_PROJECT_ITEM && id < (ID_MENU_PROJECT_ITEM + MAX_RECENT_PROJECTS)) {
		wxString project = RecentProjectsMenu->GetLabelText(id);
		
		// on linux project seems to have an extra space
		project.Trim();
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
			wxCommandEvent cmd(EVENT_APP_OPEN_PROJECT);
			cmd.SetString(project);
			cmd.SetId(wxID_ANY);
			AppEvent(cmd);
		}
		if (remove) {
			std::vector<wxString>::iterator it = std::find(RecentProjects.begin(), RecentProjects.end(), project);
			if (it != RecentProjects.end()) {
				RecentProjects.erase(it);
				SyncMenu();
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
	parent->AddPage(panel, wxT("Explorer"));
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
			wxMessageBox(_("Explorer could not be invoked: ") + cmd);
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
			wxMessageBox(_("Explorer could not be invoked: ") + cmd);
		}
	}
	else {
		wxMessageBox(_("Need to open a file first."), _("Project Explore"));
	}
}

mvceditor::ProjectPluginPanelClass::ProjectPluginPanelClass(wxWindow *parent, mvceditor::ProjectPluginClass &projectPlugin) 
: ProjectPluginGeneratedPanelClass(parent) {
	NonEmptyTextValidatorClass validator(&projectPlugin.ExplorerExecutable, Label);
	ExplorerExecutable->SetValidator(validator);
}

void mvceditor::ProjectPluginPanelClass::OnFileChanged(wxFileDirPickerEvent& event) {
	ExplorerExecutable->SetValue(event.GetPath());
	event.Skip();
}

BEGIN_EVENT_TABLE(mvceditor::ProjectPluginClass, wxEvtHandler)
	EVT_MENU(ID_MENU_PROJECT_EXPLORE, mvceditor::ProjectPluginClass::OnProjectExplore)
	EVT_MENU(ID_MENU_PROJECT_EXPLORE_OPEN_FILE, mvceditor::ProjectPluginClass::OnProjectExploreOpenFile)
	EVT_MENU(ID_TOOLBAR_PROJECT_EXPLORE, mvceditor::ProjectPluginClass::OnProjectExplore)
	EVT_MENU(ID_TOOLBAR_PROJECT_EXPLORE_OPEN_FILE, mvceditor::ProjectPluginClass::OnProjectExploreOpenFile)

	/**
	 * Since there could be 1...N recent project menu items we cannot listen to one menu item's event
	 * we have to listen to all menu events
	 */
	EVT_MENU(wxID_ANY, mvceditor::ProjectPluginClass::OnMenu)
END_EVENT_TABLE()