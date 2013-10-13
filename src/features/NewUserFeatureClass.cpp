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
#include <features/NewUserFeatureClass.h>
#include <globals/Assets.h>
#include <MvcEditor.h>
#include <wx/valgen.h>
#include <wx/stdpaths.h>
#include <wx/fileconf.h>

static int ID_NEW_USER_DIALOG = wxNewId();
static int ID_NEW_USER_TIMER = wxNewId();

mvceditor::NewUserFeatureClass::NewUserFeatureClass(mvceditor::AppClass& app)
: FeatureClass(app) 
, Timer(this, ID_NEW_USER_TIMER) {
}

void mvceditor::NewUserFeatureClass::OnAppReady(wxCommandEvent &event) {
	wxFileName settingsDir = App.Preferences.SettingsDir();
	if(!settingsDir.IsOk()) {
		Timer.Start(1000, wxTIMER_ONE_SHOT);
	}
}

void mvceditor::NewUserFeatureClass::OnTimer(wxTimerEvent& event) {
	wxFileName settingsDir;
	NewUserDialogClass dialog(GetMainWindow(), App.Globals, settingsDir);
	dialog.ShowModal();
	App.SavePreferences(settingsDir);

	// re trigger the app start sequence, that way the tag dbs get created
	// in the new location
	wxCommandEvent evt(mvceditor::EVENT_APP_READY);
	App.EventSink.Publish(evt);
	App.Sequences.AppStart();
}


mvceditor::NewUserDialogClass::NewUserDialogClass(wxWindow *parent, mvceditor::GlobalsClass &globals, wxFileName& configFileDir)
: NewUserDialogGeneratedClass(parent, ID_NEW_USER_DIALOG)
, Globals(globals)
, ConfigFileDir(configFileDir) {

	wxTextValidator phpFileExtensionsValidator(wxFILTER_EMPTY, &globals.PhpFileExtensionsString);
	PhpFileExtensions->SetValidator(phpFileExtensionsValidator);
	wxTextValidator sqlFileExtensionsValidator(wxFILTER_EMPTY, &globals.SqlFileExtensionsString);
	SqlFileExtensions->SetValidator(sqlFileExtensionsValidator);
	wxTextValidator cssFileExtensionsValidator(wxFILTER_EMPTY, &globals.CssFileExtensionsString);
	CssFileExtensions->SetValidator(cssFileExtensionsValidator);
	wxTextValidator miscFileExtensionsValidator(wxFILTER_EMPTY, &globals.MiscFileExtensionsString);
	MiscFileExtensions->SetValidator(miscFileExtensionsValidator);

	wxGenericValidator phpInstalledValidator(&globals.Environment.Php.NotInstalled);
	NoPhp->SetValidator(phpInstalledValidator);

	wxString label = UserDataDirectory->GetLabel();
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName tempDir;
		tempDir.AssignDir(paths.GetUserConfigDir());
		tempDir.AppendDir(wxT(".mvc-editor"));
	label += wxT(" (") + tempDir.GetPath() + wxT(")");
	UserDataDirectory->SetLabel(label);

	TransferDataToWindow();
}

void mvceditor::NewUserDialogClass::OnUpdateUi(wxUpdateUIEvent& event) {
	SettingsDirectory->Enable(CustomDirectory->GetValue());
	PhpExecutable->Enable(!NoPhp->GetValue());
}

void mvceditor::NewUserDialogClass::OnOkButton(wxCommandEvent& event) {
	TransferDataFromWindow();
	if (!NoPhp->GetValue()) {

		// only if the user has php installed do we check to see if the 
		// executable is good
		wxString phpPath = PhpExecutable->GetPath();
		if (!wxFileName::FileExists(phpPath)) {
			wxMessageBox(wxT("PHP Executable must exist."), wxT("Error"));
			return;
		}
	}
	if (CustomDirectory->GetValue()) {
		
		// if user chose to store settings in a custom directory it must exist
		wxString settingsDir = SettingsDirectory->GetPath();
		if (!wxFileName::DirExists(settingsDir)) {
			wxMessageBox(wxT("Custom settings directory must exist."), wxT("Error"));
			return;
		}
	}
	Globals.Environment.Php.PhpExecutablePath = PhpExecutable->GetPath();
	wxStandardPaths paths = wxStandardPaths::Get();
	if (ApplicationDirectory->GetValue()) {
		
		// create our subDir if it does not exist
		wxFileName tempDir;
		tempDir.AssignDir(paths.GetExecutablePath());

		// since we distribute the binary in  a "bin/" dir
		// lets remove it so that the settings dir is in the top-level
		// of the installation directory
		tempDir.RemoveLastDir();
		tempDir.AppendDir(wxT(".mvc-editor"));
		if (!tempDir.DirExists()) {
			bool created = wxMkdir(tempDir.GetPath(), 0777);
			if (!created) {
				wxMessageBox(wxT("Could not create directory: ") + tempDir.GetPath(), wxT("Error"));
				return;
			}
		}
		ConfigFileDir = tempDir;
	}
	else if (CustomDirectory->GetValue()) {
		ConfigFileDir.AssignDir(SettingsDirectory->GetPath());
	}
	else {

		// use the user data directory
		// create our subDir if it does not exist
		wxFileName tempDir;
		tempDir.AssignDir(paths.GetUserConfigDir());
		tempDir.AppendDir(wxT(".mvc-editor"));
		if (!tempDir.DirExists()) {
			bool created = wxMkdir(tempDir.GetPath(), 0777);
			if (!created) {
				wxMessageBox(wxT("Could not create directory: ") + tempDir.GetPath(), wxT("Error"));
				return;
			}
		}
		ConfigFileDir = tempDir;
	}
	if (NoPhp->GetValue()) {

		// no PHP then dont try to detect version
		Globals.Environment.Php.IsAuto = 0;
		Globals.Environment.Php.Version = pelet::PHP_54;
	}
	EndModal(wxOK);
}

BEGIN_EVENT_TABLE(mvceditor::NewUserDialogClass, wxDialog)
	EVT_UPDATE_UI(ID_NEW_USER_DIALOG, mvceditor::NewUserDialogClass::OnUpdateUi)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::NewUserFeatureClass, mvceditor::FeatureClass)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_READY, mvceditor::NewUserFeatureClass::OnAppReady)
	EVT_TIMER(ID_NEW_USER_TIMER, mvceditor::NewUserFeatureClass::OnTimer)
END_EVENT_TABLE()