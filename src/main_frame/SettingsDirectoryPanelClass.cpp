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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <main_frame/SettingsDirectoryPanelClass.h>
#include <widgets/DirPickerValidatorClass.h>
#include <wx/stdpaths.h>
#include <wx/msgdlg.h>

/**
 * @return wxFileName the location of the settings directory when it is in the
 *         same directory as the application executable
 */
static wxFileName AppDir() {
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName execFileName(paths.GetExecutablePath());
	wxFileName tempDir;
	tempDir.AssignDir(execFileName.GetPath());

	// since we distribute the binary in  a "bin/" dir
	// lets remove it so that the settings dir is in the top-level
	// of the installation directory
	tempDir.RemoveLastDir();
	tempDir.AppendDir(wxT(".triumph4php"));
	return tempDir;
}

/**
 * @return wxFileName the location of the settings directory when it is in the
 *         user data directory
 */
static wxFileName UserDir() {
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName tempDir;
	tempDir.AssignDir(paths.GetUserConfigDir());
	tempDir.AppendDir(wxT(".triumph4php"));
	return tempDir;
}

t4p::SettingsDirectoryPanelClass::SettingsDirectoryPanelClass(wxWindow* parent, int id, wxFileName& settingsDir)
: SettingsDirectoryGeneratedPanelClass(parent, id) {
	t4p::DirPickerValidatorClass validator(&settingsDir);
	SettingsDirectory->SetValidator(validator);
	SettingsDirectory->SetName(wxT("Settings directory"));

	wxFileName appDir = AppDir();
	wxFileName userDir = UserDir();

	if (settingsDir == appDir) {
		ApplicationDirectory->SetValue(1);
		UserDataDirectory->SetValue(0);
		CustomDirectory->SetValue(0);
		SettingsDirectory->Enable(false);
	}
	else if (settingsDir == userDir) {
		ApplicationDirectory->SetValue(0);
		UserDataDirectory->SetValue(1);
		CustomDirectory->SetValue(0);
		SettingsDirectory->Enable(false);
	}
	else {
		ApplicationDirectory->SetValue(0);
		UserDataDirectory->SetValue(0);
		CustomDirectory->SetValue(1);
		SettingsDirectory->Enable(true);
	}
}

void t4p::SettingsDirectoryPanelClass::OnUserDataDir(wxCommandEvent& event) {
	wxFileName userDir = UserDir();
	SettingsDirectory->SetPath(userDir.GetPath());
	SettingsDirectory->Enable(false);
	if (!userDir.DirExists()) {
		bool created = wxMkdir(userDir.GetPath(), 0777);
		if (!created) {
			wxMessageBox(wxT("Could not create directory: ") + userDir.GetPath(), wxT("Error"));
			return;
		}
	}
	else if (!userDir.IsDirWritable()) {
		wxMessageBox(wxT("Directory is not writable: ") + userDir.GetPath(), wxT("Error"));
		return;
	}
}

void t4p::SettingsDirectoryPanelClass::OnAppDir(wxCommandEvent& event) {
	wxFileName appDir = AppDir();
	SettingsDirectory->SetPath(appDir.GetPath());
	SettingsDirectory->Enable(false);
	if (!appDir.DirExists()) {
		bool created = wxMkdir(appDir.GetPath(), 0777);
		if (!created) {
			wxMessageBox(wxT("Could not create directory: ") + appDir.GetPath(), wxT("Error"));
			return;
		}
	}
	else if (!appDir.IsDirWritable()) {
		wxMessageBox(wxT("Directory is not writable: ") + appDir.GetPath(), wxT("Error"));
		return;
	}
}

void t4p::SettingsDirectoryPanelClass::OnCustomDir(wxCommandEvent& event) {
	SettingsDirectory->SetPath(wxT(""));
	SettingsDirectory->Enable(true);
}

bool t4p::SettingsDirectoryPanelClass::TransferDataFromWindow() {
	wxWindow::TransferDataFromWindow();

	// make sure new settings dir exists and is writable
	if (!wxFileName::DirExists(SettingsDirectory->GetPath())) {
		wxMessageBox(_("Settings directory does not exist."), _("Settings Directory"));
		return false;
	}
	if (!wxFileName::IsDirWritable(SettingsDirectory->GetPath())) {
		wxMessageBox(_("Settings directory is not writable."), _("Settings Directory"));
		return false;
	}
	return true;
}
