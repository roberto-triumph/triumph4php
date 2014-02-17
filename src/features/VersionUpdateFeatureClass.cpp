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
#include <features/VersionUpdateFeatureClass.h>
#include <globals/Assets.h>
#include <Triumph.h>
#include <curl/curl.h>
#include <wx/ffile.h>
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


 /* writes to a stream */
static size_t curl_ostream_write(void* ptr, size_t size, size_t nmemb, void* stream) {
    size_t bytes = size * nmemb;
    wxOutputStream* ostream = (wxOutputStream*)stream;
    if (ostream) {
        ostream->Write(ptr, bytes);
        return ostream->LastWrite();
    }
    return 0;
}

t4p::VersionUpdateFeatureClass::VersionUpdateFeatureClass(t4p::AppClass& app)
: FeatureClass(app)
, NextCheckTime()
, Timer(this, ID_VERSION_FEATURE_TIMER) {

}

void t4p::VersionUpdateFeatureClass::AddHelpMenuItems(wxMenu* helpMenu) {
	helpMenu->Append(t4p::MENU_VERSION_UPDATE, _("Check for updates"),
		_("Check for new version of Triumph"), wxITEM_NORMAL);
}

void t4p::VersionUpdateFeatureClass::AddPreferenceWindow(wxBookCtrlBase* parent) {

	VersionUpdatePreferencesPanelClass* panel = new t4p::VersionUpdatePreferencesPanelClass(
		parent, App.Preferences);
	parent->AddPage(panel, _("Check for updates"));
}

void t4p::VersionUpdateFeatureClass::LoadPreferences(wxConfigBase* config) {
	wxString readTime;
	config->Read(wxT("VersionUpdates/NextCheckTime"), &readTime);
	if (!NextCheckTime.ParseDateTime(readTime)) {
		NextCheckTime = wxDateTime::Now();
	}
}

void t4p::VersionUpdateFeatureClass::OnAppReady(wxCommandEvent& event) {
	if (App.Preferences.CheckForUpdates) {
		Timer.Start(1000 * 1, wxTIMER_CONTINUOUS);
	}
	CURLcode code = curl_global_init(CURL_GLOBAL_DEFAULT);
	bool good = 0 == code;
	wxUnusedVar(good);
	wxASSERT_MSG(good, _("curl did not initialize"));
}

void t4p::VersionUpdateFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {
	if (App.Preferences.CheckForUpdates) {
		if (!Timer.IsRunning()) {
			Timer.Start(1000 * 1, wxTIMER_CONTINUOUS);
		}
	}
	else {
		Timer.Stop();
	}
}

void t4p::VersionUpdateFeatureClass::OnPreferencesExternallyUpdated(wxCommandEvent& event) {
	if (App.Preferences.CheckForUpdates) {
		if (!Timer.IsRunning()) {
			Timer.Start(1000 * 1, wxTIMER_CONTINUOUS);
		}
	}
	else {
		Timer.Stop();
	}
}

void t4p::VersionUpdateFeatureClass::OnTimer(wxTimerEvent& event) {
	
	// is it time to check for a new version
	wxDateTime now = wxDateTime::Now();
	if (NextCheckTime.IsEarlierThan(now)) {
		Timer.Stop();
		wxString currentVersion = GetCurrentVersion();
		t4p::VersionUpdateActionClass* action = new t4p::VersionUpdateActionClass(
			App.RunningThreads, ID_EVENT_VERSION_UPDATES, currentVersion
		);
		App.RunningThreads.Queue(action);
	}
}

void t4p::VersionUpdateFeatureClass::OnUpdateCheckComplete(wxCommandEvent& event) {
	wxString nextVersion = event.GetString();
	
	wxString currentVersion = GetCurrentVersion();
	if (!nextVersion.empty()) {
		VersionUpdateDialogClass dialog(GetMainWindow(), wxID_ANY,
			App.RunningThreads, currentVersion,
			true, nextVersion);
		dialog.ShowModal();
	}

	// do another check a week from today
	NextCheckTime = wxDateTime::Now();
	wxDateSpan week(0, 0, 1, 0);
	NextCheckTime.Add(week);

	// write the check time to the config
	// NOTE: we check if settings dir has been set, if it has not
	// been set, then we cannot write the config to a file 
	// because the user has not chosen a settings directory
	// since the version check is triggered during the app ready
	// event, during the very first run the user has not yet
	// chosen a settings dir, and the config is not usable
	// TODO: we should prevent config from being read
	// before the user has chosen a settings dir, or
	// provide a default settings dir
	wxFileName settingsDir = t4p::SettingsDirAsset();
	if (settingsDir.IsOk()) {
		wxConfigBase* config = wxConfig::Get();
	
		config->Write(wxT("VersionUpdates/NextCheckTime"), NextCheckTime.FormatISOCombined(' '));
		config->Flush();
	}

	Timer.Start(1000 * 20, wxTIMER_CONTINUOUS);
}

void t4p::VersionUpdateFeatureClass::OnHelpCheckForUpdates(wxCommandEvent& event) {
	wxString currentVersion  = GetCurrentVersion();
	t4p::VersionUpdateDialogClass dialog(GetMainWindow(), 
			wxID_ANY, App.RunningThreads, currentVersion, false,
			wxEmptyString);
	dialog.ShowModal();
}

wxString t4p::VersionUpdateFeatureClass::GetCurrentVersion() const {

	// version info is stored in a file
	// for releases, the distribution script will properly fill in the
	// version number using git describe
	wxString currentVersion;
	wxFileName versionFileName = t4p::VersionFileAsset();
	wxFFile file(versionFileName.GetFullPath());
	if (file.IsOpened()) {
		file.ReadAll(&currentVersion);
	}
	currentVersion.Trim(true);
	return currentVersion;
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

t4p::VersionUpdateActionClass::VersionUpdateActionClass(t4p::RunningThreadsClass& runningThreads, int eventId, 
															  const wxString& currentVersion)
: ActionClass(runningThreads, eventId)
, CurrentVersion(currentVersion.c_str()) {
}

void t4p::VersionUpdateActionClass::BackgroundWork() {
	long statusCode = 0;
	wxString newVersion = GetNewVersion(CurrentVersion, statusCode);
	
	wxCommandEvent evt(EVENT_VERSION_CHECK);
	evt.SetString(newVersion);
	evt.SetInt(statusCode);
	PostEvent(evt);
}

wxString t4p::VersionUpdateActionClass::GetNewVersion(const wxString& currentVersion, long& statusCode) {

	// T4P_UPDATE_HOST is a define from the premake script
	// it will be different in debug vs release
	const char* host = T4P_STR(T4P_UPDATE_HOST);
	std::string fullUrl = "http://";
	fullUrl += host;
	fullUrl += "/updates.php";
	
	std::string data = "v=";
	data += t4p::WxToChar(currentVersion);
	const char* buf = data.c_str();
	const char* urlBuf = fullUrl.c_str();
	
	wxStringOutputStream ostream;
	CURL* curl = curl_easy_init();
	
	curl_easy_setopt(curl, CURLOPT_URL, urlBuf);
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_ostream_write);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ostream);
	
	CURLcode ret = curl_easy_perform(curl);
	wxString newVersion;
	if (0 == ret) {
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
		if (statusCode == 200) {
			wxString contents = ostream.GetString();
			if (!contents.empty()) {

				// the update query can return
				// 1. A single line: "UP_TO_DATE"
				// or 
				// 2. 3 lines seperated by newlines:
				// "NEW_VERSION"
				// new version string
				// new version release date
				wxStringTokenizer tok(contents);
				wxString line = tok.NextToken();
				if (line.CmpNoCase(wxT("NEW_VERSION")) == 0) {
					newVersion = tok.NextToken();
				}
			}
		}
		else {
			newVersion = t4p::CharToWx(curl_easy_strerror(ret));
			
		}
	}
	curl_easy_cleanup(curl);
	return newVersion;
}

wxString t4p::VersionUpdateActionClass::GetLabel() const {
	return wxT("Version updates");
}

BEGIN_EVENT_TABLE(t4p::VersionUpdateFeatureClass, t4p::FeatureClass)
	EVT_MENU(t4p::MENU_VERSION_UPDATE + 0, t4p::VersionUpdateFeatureClass::OnHelpCheckForUpdates)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_READY, t4p::VersionUpdateFeatureClass::OnAppReady)
	EVT_TIMER(ID_VERSION_FEATURE_TIMER, t4p::VersionUpdateFeatureClass::OnTimer)
	EVT_COMMAND(ID_EVENT_VERSION_UPDATES, EVENT_VERSION_CHECK, t4p::VersionUpdateFeatureClass::OnUpdateCheckComplete)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::VersionUpdateFeatureClass::OnPreferencesSaved)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_EXTERNALLY_UPDATED, t4p::VersionUpdateFeatureClass::OnPreferencesExternallyUpdated)
END_EVENT_TABLE()


BEGIN_EVENT_TABLE(t4p::VersionUpdateDialogClass, wxDialog)
	EVT_TIMER(ID_VERSION_DIALOG_TIMER, t4p::VersionUpdateDialogClass::OnTimer)
	EVT_COMMAND(ID_EVENT_VERSION_UPDATES_ON_DIALOG, EVENT_VERSION_CHECK, t4p::VersionUpdateDialogClass::OnUpdateCheckComplete)
END_EVENT_TABLE()
