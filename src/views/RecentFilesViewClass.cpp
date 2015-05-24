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
#include <views/RecentFilesViewClass.h>
#include <Triumph.h>

t4p::RecentFilesViewClass::RecentFilesViewClass(t4p::RecentFilesFeatureClass& feature)
	: FeatureViewClass()
	, Feature(feature) {
	RecentFilesMenu = NULL;
}

t4p::RecentFilesViewClass::~RecentFilesViewClass() {
	Feature.FileHistory.RemoveMenu(RecentFilesMenu);
}

void t4p::RecentFilesViewClass::AddFileMenuItems(wxMenu* fileMenu) {

	// ATTN: possible problem. according to wxWidgets docs
	//     Append the submenu to the parent menu after you have added your menu items, or accelerators may
	//     not be registered properly.
	//
	// wont worry about that for now since project menu items don't have accelerators automatically
	RecentFilesMenu = new wxMenu();
	Feature.FileHistory.UseMenu(RecentFilesMenu);
	fileMenu->Append(t4p::MENU_RECENT_FILES + MAX_RECENT_FILES + 1, _("Recent Files"),  RecentFilesMenu, _("Recent Files"));
}

void t4p::RecentFilesViewClass::SavePreferences() {
	Feature.SavePreferences();
}

void t4p::RecentFilesViewClass::OnRecentFileMenu(wxCommandEvent &event) {
	size_t id = (size_t)event.GetId();
	if (id >= (size_t)t4p::MENU_RECENT_FILES && id < (t4p::MENU_RECENT_FILES + Feature.FileHistory.GetCount())) {
		size_t index = id - t4p::MENU_RECENT_FILES;
		wxString fileName = Feature.FileHistory.GetHistoryFile(index);
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
			LoadCodeControl(fileName);
		}
		if (remove) {
			if (index < Feature.FileHistory.GetCount()) {
				Feature.FileHistory.RemoveFileFromHistory(index);
				SavePreferences();
			}
		}
	}
}

void t4p::RecentFilesViewClass::OnAppFileOpened(t4p::CodeControlEventClass& event) {
	wxString fileName = event.GetCodeControl()->GetFileName();
	Feature.FileHistory.AddFileToHistory(fileName);
	SavePreferences();
}

void t4p::RecentFilesViewClass::OnAppFileCreated(wxCommandEvent& event) {
	wxString fileName = event.GetString();
	Feature.FileHistory.AddFileToHistory(fileName);
	SavePreferences();
}

BEGIN_EVENT_TABLE(t4p::RecentFilesViewClass, t4p::FeatureViewClass)

	/**
	* Since there could be 1...N recent file items we cannot listen to one menu item's event
	* we have to listen to all menu events
	*/
	EVT_MENU_RANGE(t4p::MENU_RECENT_FILES, t4p::MENU_RECENT_FILES + t4p::MAX_RECENT_FILES, t4p::RecentFilesViewClass::OnRecentFileMenu)
	EVT_APP_FILE_OPEN(t4p::RecentFilesViewClass::OnAppFileOpened)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_FILE_CREATED, t4p::RecentFilesViewClass::OnAppFileCreated)
END_EVENT_TABLE()
