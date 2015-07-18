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
#include <views/VersionUpdateViewClass.h>
#include <globals/Assets.h>
#include <Triumph.h>
#include <wx/valgen.h>
#include <wx/tokenzr.h>
#include <wx/sstream.h>

// these macros will expand a macro into its
// these are needed to expand the update host which
// are given as macros by the premake script
#define T4P_STR_EXPAND(s) #s
#define T4P_STR(s) T4P_STR_EXPAND(s)

static int ID_VERSION_FEATURE_TIMER = wxNewId();
static int ID_VERSION_DIALOG_TIMER = wxNewId();
static int ID_EVENT_VERSION_UPDATES = wxNewId();
static int ID_EVENT_VERSION_UPDATES_ON_DIALOG = wxNewId();
static wxEventType EVENT_VERSION_CHECK = wxNewEventType();


t4p::VersionUpdateViewClass::VersionUpdateViewClass(t4p::VersionUpdateFeatureClass& feature)
: FeatureViewClass()
, Feature(feature)
, Timer(this, ID_VERSION_FEATURE_TIMER) {

}

void t4p::VersionUpdateViewClass::AddHelpMenuItems(wxMenu* helpMenu) {
	helpMenu->Append(t4p::MENU_VERSION_UPDATE, _("Check for updates"),
		_("Check for new version of Triumph"), wxITEM_NORMAL);
}

void t4p::VersionUpdateViewClass::AddPreferenceWindow(wxBookCtrlBase* parent) {

	VersionUpdatePreferencesPanelClass* panel = new t4p::VersionUpdatePreferencesPanelClass(
		parent, Feature.App.Preferences);
	parent->AddPage(panel, _("Check for updates"));
}

void t4p::VersionUpdateViewClass::OnAppReady(wxCommandEvent& event) {
	if (Feature.App.Preferences.CheckForUpdates) {
		Timer.Start(1000 * 1, wxTIMER_CONTINUOUS);
	}
}

void t4p::VersionUpdateViewClass::OnPreferencesSaved(wxCommandEvent& event) {
	if (Feature.App.Preferences.CheckForUpdates) {
		if (!Timer.IsRunning()) {
			Timer.Start(1000 * 1, wxTIMER_CONTINUOUS);
		}
	}
	else {
		Timer.Stop();
	}
}

void t4p::VersionUpdateViewClass::OnPreferencesExternallyUpdated(wxCommandEvent& event) {
	if (Feature.App.Preferences.CheckForUpdates) {
		if (!Timer.IsRunning()) {
			Timer.Start(1000 * 1, wxTIMER_CONTINUOUS);
		}
	}
	else {
		Timer.Stop();
	}
}

void t4p::VersionUpdateViewClass::OnTimer(wxTimerEvent& event) {

	// is it time to check for a new version
	wxDateTime now = wxDateTime::Now();
	if (Feature.NextCheckTime.IsEarlierThan(now)) {
		Timer.Stop();
		wxString currentVersion = Feature.GetCurrentVersion();
		t4p::VersionUpdateActionClass* action = new t4p::VersionUpdateActionClass(
			Feature.App.RunningThreads, ID_EVENT_VERSION_UPDATES, currentVersion
		);
		Feature.App.RunningThreads.Queue(action);
	}
}

void t4p::VersionUpdateViewClass::OnUpdateCheckComplete(wxCommandEvent& event) {
	wxString nextVersion = event.GetString();

	wxString currentVersion = Feature.GetCurrentVersion();
	if (!nextVersion.empty()) {
		VersionUpdateDialogClass dialog(GetMainWindow(), wxID_ANY,
			Feature.App.RunningThreads, currentVersion,
			true, nextVersion);
		dialog.ShowModal();
	}

	// do another check a week from today
	Feature.NextCheckTime = wxDateTime::Now();
	wxDateSpan week(0, 0, 1, 0);
	Feature.NextCheckTime.Add(week);

	// write the check time to the config
	// NOTE: we check if settings dir has been set, if it has not
	// been set, then we cannot write the config to a file
	// because the user has not chosen a settings directory
	// since the version check is triggered during the app ready
	// event, during the very first run the user has not yet
	// chosen a settings dir, and the config is not usable
	// TODO(roberto): we should prevent config from being read
	// before the user has chosen a settings dir, or
	// provide a default settings dir
	wxFileName settingsDir = t4p::SettingsDirAsset();
	if (settingsDir.IsOk()) {
		wxConfigBase* config = wxConfig::Get();

		config->Write(wxT("VersionUpdates/NextCheckTime"), Feature.NextCheckTime.FormatISOCombined(' '));
		config->Flush();
	}

	Timer.Start(1000 * 20, wxTIMER_CONTINUOUS);
}

void t4p::VersionUpdateViewClass::OnHelpCheckForUpdates(wxCommandEvent& event) {
	wxString currentVersion  = Feature.GetCurrentVersion();
	t4p::VersionUpdateDialogClass dialog(GetMainWindow(),
			wxID_ANY, Feature.App.RunningThreads, currentVersion, false,
			wxEmptyString);
	dialog.ShowModal();
}

t4p::VersionUpdatePreferencesPanelClass::VersionUpdatePreferencesPanelClass(wxWindow* parent,
																				  t4p::PreferencesClass& preferences)
: VersionUpdatePreferencesGeneratedPanelClass(parent, wxID_ANY) {
	wxGenericValidator checkValidator(&preferences.CheckForUpdates);
	CheckForUpdates->SetValidator(checkValidator);
	TransferDataToWindow();
}

t4p::VersionUpdateDialogClass::VersionUpdateDialogClass(wxWindow* parent, int id,
															  t4p::RunningThreadsClass& runningThreads,
															  const wxString& currentVersion,
															  bool showNewVersion,
															  wxString newVersion)
: VersionUpdateGeneratedDialogClass(parent, id)
, Timer(this, ID_VERSION_DIALOG_TIMER)
, RunningThreads(runningThreads)
, StartedCheck(false) {
	RunningThreads.AddEventHandler(this);
	CurrentVersion->SetLabel(currentVersion);
	if (showNewVersion) {
		Gauge->Show(false);
		NewVersion->SetLabel(newVersion);
		Result->SetLabel(wxT("New version available: ") + newVersion);
	}
	else {
		Timer.Start(200, wxTIMER_CONTINUOUS);
	}
}

void t4p::VersionUpdateDialogClass::OnTimer(wxTimerEvent& event) {
	if (!StartedCheck) {
		Result->SetLabel(wxT("Checking for new version"));
		ConnectToUpdateServer();
		StartedCheck = true;
	}
	else {
		Gauge->Pulse();
	}
}

void t4p::VersionUpdateDialogClass::OnUpdateCheckComplete(wxCommandEvent& event) {
	Gauge->Show(false);
	wxString newVersion = event.GetString();
	int statusCode = event.GetInt();

	bool foundNew = 200 == statusCode && !newVersion.empty();
	if (foundNew) {
		Result->SetLabel(wxT("New version available: ") + newVersion);
	}
	else if (statusCode == 200) {
		Result->SetLabel(wxT("Your version is up-to-date"));
		NewVersion->SetLabel(CurrentVersion->GetLabel());
	}
	else {
		Result->SetLabel(wxString::Format(wxT("Connection error: (%d) %s"), statusCode, newVersion));
	}
	Timer.Stop();
}

void t4p::VersionUpdateDialogClass::ConnectToUpdateServer() {
	t4p::VersionUpdateActionClass* action = new t4p::VersionUpdateActionClass(
		RunningThreads, ID_EVENT_VERSION_UPDATES_ON_DIALOG, CurrentVersion->GetLabel()
	);
	RunningThreads.Queue(action);
}

void t4p::VersionUpdateDialogClass::OnOkButton(wxCommandEvent& event) {
	RunningThreads.RemoveEventHandler(this);
	EndModal(wxOK);
}

BEGIN_EVENT_TABLE(t4p::VersionUpdateViewClass, t4p::FeatureViewClass)
	EVT_MENU(t4p::MENU_VERSION_UPDATE + 0, t4p::VersionUpdateViewClass::OnHelpCheckForUpdates)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_READY, t4p::VersionUpdateViewClass::OnAppReady)
	EVT_TIMER(ID_VERSION_FEATURE_TIMER, t4p::VersionUpdateViewClass::OnTimer)
	EVT_COMMAND(ID_EVENT_VERSION_UPDATES, EVENT_VERSION_CHECK, t4p::VersionUpdateViewClass::OnUpdateCheckComplete)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::VersionUpdateViewClass::OnPreferencesSaved)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_EXTERNALLY_UPDATED, t4p::VersionUpdateViewClass::OnPreferencesExternallyUpdated)
END_EVENT_TABLE()


BEGIN_EVENT_TABLE(t4p::VersionUpdateDialogClass, wxDialog)
	EVT_TIMER(ID_VERSION_DIALOG_TIMER, t4p::VersionUpdateDialogClass::OnTimer)
	EVT_COMMAND(ID_EVENT_VERSION_UPDATES_ON_DIALOG, EVENT_VERSION_CHECK, t4p::VersionUpdateDialogClass::OnUpdateCheckComplete)
END_EVENT_TABLE()
