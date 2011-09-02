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


int ID_MENU_PROJECT_EXPLORE = mvceditor::PluginClass::newMenuId();
int ID_MENU_PROJECT_EXPLORE_OPEN_FILE = mvceditor::PluginClass::newMenuId();
int ID_TOOLBAR_PROJECT_EXPLORE = wxNewId();
int ID_TOOLBAR_PROJECT_EXPLORE_OPEN_FILE = wxNewId();


mvceditor::ProjectPluginClass::ProjectPluginClass() 
	: PluginClass() {
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
}

void mvceditor::ProjectPluginClass::AddToolBarItems(wxAuiToolBar* toolbar) {
	wxBitmap bitmap = wxArtProvider::GetBitmap(wxART_FOLDER_OPEN, wxART_TOOLBAR, wxSize(16, 16));
	toolbar->AddTool(ID_TOOLBAR_PROJECT_EXPLORE, _("Explore"), bitmap, _("Open An explorer window in the Project Root"));
	bitmap = wxArtProvider::GetBitmap(wxART_FOLDER_OPEN, wxART_TOOLBAR, wxSize(16, 16));
	toolbar->AddTool(ID_TOOLBAR_PROJECT_EXPLORE_OPEN_FILE, _("Explore Open File"), bitmap, _("Open An explorer window in the currently opened file"));
}

void mvceditor::ProjectPluginClass::LoadPreferences(wxConfigBase* config) {
	config->Read(wxT("Project/ExplorerExecutable"), &ExplorerExecutable);
}

void mvceditor::ProjectPluginClass::SavePreferences(wxConfigBase* config) {
	config->Write(wxT("Project/ExplorerExecutable"), ExplorerExecutable);
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
END_EVENT_TABLE()