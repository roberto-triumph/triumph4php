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
#include <features/RecentFilesFeatureClass.h>
#include <Triumph.h>

static int MAX_RECENT_FILES = 10;


t4p::RecentFilesFeatureClass::RecentFilesFeatureClass(t4p::AppClass& app)
	: FeatureClass(app)
	, FileHistory(MAX_RECENT_FILES, t4p::MENU_RECENT_FILES) {
	RecentFilesMenu = NULL;
}

void t4p::RecentFilesFeatureClass::AddFileMenuItems(wxMenu* fileMenu) {

	// ATTN: possible problem. according to wxWidgets docs
	//     Append the submenu to the parent menu after you have added your menu items, or accelerators may
	//     not be registered properly.
	//
	// wont worry about that for now since project menu items don't have accelerators automatically
	RecentFilesMenu = new wxMenu();
	FileHistory.UseMenu(RecentFilesMenu);
	fileMenu->Append(t4p::MENU_RECENT_FILES + MAX_RECENT_FILES + 1, _("Recent Files"),  RecentFilesMenu, _("Recent Files"));
}

void t4p::RecentFilesFeatureClass::LoadPreferences(wxConfigBase* config) {
	while (RecentFilesMenu->GetMenuItemCount() > 0) {
		RecentFilesMenu->Delete(RecentFilesMenu->FindItemByPosition(0));
	}
	FileHistory.Load(*config);
}

void t4p::RecentFilesFeatureClass::SavePreferences() {
	wxConfigBase* config = wxConfigBase::Get();
	FileHistory.Save(*config);
	config->Flush();

	// signal that this app has modified the config file, that way the external
	// modification check fails and the user will not be prompted to reload the config
	App.UpdateConfigModifiedTime();
}

void t4p::RecentFilesFeatureClass::OnRecentFileMenu(wxCommandEvent &event) {
	size_t id = (size_t)event.GetId();
	if (id >= (size_t)t4p::MENU_RECENT_FILES && id < (t4p::MENU_RECENT_FILES + FileHistory.GetCount())) {
		size_t index = id - t4p::MENU_RECENT_FILES;
		wxString fileName = FileHistory.GetHistoryFile(index);
		bool remove = false;
		if (!wxFileName::FileExists(fileName)) {
			int ret = wxMessageBox(_("File no longer exists. Remove from Recent list?\n") + fileName, _("Warning"),
				wxCENTER | wxICON_ERROR | wxYES_NO);
			remove = wxYES == ret;
		}
		else if (wxFileName::DirExists(fileName)) {
			int ret = wxMessageBox(_("File is a directory. Remove from Recent list?\n") + fileName, _("Warning"),
				wxCENTER | wxICON_ERROR | wxYES_NO);
			remove = wxYES == ret;
		}
		else {
			GetNotebook()->LoadPage(fileName);
		}
		if (remove) {
			if (index < FileHistory.GetCount()) {
				FileHistory.RemoveFileFromHistory(index);
				SavePreferences();
			}
		}
	}
}

void t4p::RecentFilesFeatureClass::OnAppFileOpened(t4p::CodeControlEventClass& event) {
	wxString fileName = event.GetCodeControl()->GetFileName();
	FileHistory.AddFileToHistory(fileName);
	SavePreferences();
}

void t4p::RecentFilesFeatureClass::OnAppFileCreated(wxCommandEvent& event) {
	wxString fileName = event.GetString();
	FileHistory.AddFileToHistory(fileName);
	SavePreferences();
}

BEGIN_EVENT_TABLE(t4p::RecentFilesFeatureClass, wxEvtHandler)

	/**
	* Since there could be 1...N recent file items we cannot listen to one menu item's event
	* we have to listen to all menu events
	*/
	EVT_MENU_RANGE(t4p::MENU_RECENT_FILES, t4p::MENU_RECENT_FILES + MAX_RECENT_FILES, t4p::RecentFilesFeatureClass::OnRecentFileMenu)
	EVT_APP_FILE_OPEN(t4p::RecentFilesFeatureClass::OnAppFileOpened)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_FILE_CREATED, t4p::RecentFilesFeatureClass::OnAppFileCreated)
END_EVENT_TABLE()
