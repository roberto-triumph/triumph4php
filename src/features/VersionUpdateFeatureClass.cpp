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
#include <MvcEditor.h>
#include <wx/curl/http.h>
#include <wx/ffile.h>
#include <wx/valgen.h>
#include <wx/tokenzr.h>

// these macros will expand a macro into its 
// these are needed to expand the update host which
// are given as macros by the premake script
#define MVC_STR_EXPAND(s) #s
#define MVC_STR(s) MVC_STR_EXPAND(s)

static int ID_VERSION_FEATURE_TIMER = wxNewId();
static int ID_VERSION_DIALOG_TIMER = wxNewId();
static int ID_EVENT_VERSION_UPDATES = wxNewId();
static wxEventType EVENT_VERSION_CHECK = wxNewEventType();

// NOTE: copied from wxCurl, cannot use the one from wxCurl since its
// not exported
 /* writes to a stream */
static size_t wxcurl_stream_write(void* ptr, size_t size, size_t nmemb, void* stream) {
    size_t iRealSize = size * nmemb;
    wxOutputStream* pBuf = (wxOutputStream*)stream;
    if(pBuf) {
        pBuf->Write(ptr, iRealSize);
        return pBuf->LastWrite();
    }
    return 0;
}

mvceditor::VersionUpdateFeatureClass::VersionUpdateFeatureClass(mvceditor::AppClass& app)
: FeatureClass(app)
, NextCheckTime()
, Timer(this, ID_VERSION_FEATURE_TIMER) {

}

void mvceditor::VersionUpdateFeatureClass::AddHelpMenuItems(wxMenu* helpMenu) {
	helpMenu->Append(mvceditor::MENU_VERSION_UPDATE, _("Check for updates"),
		_("Check for new version of MVC Editor"), wxITEM_NORMAL);
}

void mvceditor::VersionUpdateFeatureClass::AddPreferenceWindow(wxBookCtrlBase* parent) {

	VersionUpdatePreferencesPanelClass* panel = new mvceditor::VersionUpdatePreferencesPanelClass(
		parent, App.Preferences);
	parent->AddPage(panel, _("Check for updates"));
}

void mvceditor::VersionUpdateFeatureClass::LoadPreferences(wxConfigBase* config) {
	wxString readTime;
	config->Read(wxT("VersionUpdates/NextCheckTime"), &readTime);
	if (!NextCheckTime.ParseDateTime(readTime)) {
		NextCheckTime = wxDateTime::Now();
	}
}

void mvceditor::VersionUpdateFeatureClass::OnAppReady(wxCommandEvent& event) {
	if (App.Preferences.CheckForUpdates) {
		Timer.Start(1000 * 1, wxTIMER_CONTINUOUS);
	}
}

void mvceditor::VersionUpdateFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {
	if (App.Preferences.CheckForUpdates) {
		if (!Timer.IsRunning()) {
			Timer.Start(1000 * 1, wxTIMER_CONTINUOUS);
		}
	}
	else {
		Timer.Stop();
	}
}

void mvceditor::VersionUpdateFeatureClass::OnPreferencesExternallyUpdated(wxCommandEvent& event) {
	if (App.Preferences.CheckForUpdates) {
		if (!Timer.IsRunning()) {
			Timer.Start(1000 * 1, wxTIMER_CONTINUOUS);
		}
	}
	else {
		Timer.Stop();
	}
}

void mvceditor::VersionUpdateFeatureClass::OnTimer(wxTimerEvent& event) {
	
	// is it time to check for a new version
	wxDateTime now = wxDateTime::Now();
	if (NextCheckTime.IsEarlierThan(now)) {
		Timer.Stop();
		wxString currentVersion = GetCurrentVersion();
		mvceditor::VersionUpdateActionClass* action = new mvceditor::VersionUpdateActionClass(
			App.RunningThreads, ID_EVENT_VERSION_UPDATES, currentVersion
		);
		App.RunningThreads.Queue(action);
	}
}

void mvceditor::VersionUpdateFeatureClass::OnUpdateCheckComplete(wxCommandEvent& event) {
	wxString nextVersion = event.GetString();
	int statusCode = event.GetInt();
	
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
	wxConfigBase* config = wxConfig::Get();
	config->Write(wxT("VersionUpdates/NextCheckTime"), NextCheckTime.FormatISOCombined(' '));
	config->Flush();

	Timer.Start(1000 * 20, wxTIMER_CONTINUOUS);
}

void mvceditor::VersionUpdateFeatureClass::OnHelpCheckForUpdates(wxCommandEvent& event) {
	wxString currentVersion  = GetCurrentVersion();
	mvceditor::VersionUpdateDialogClass dialog(GetMainWindow(), 
			wxID_ANY, App.RunningThreads, currentVersion, false,
			wxEmptyString);
	dialog.ShowModal();
}

wxString mvceditor::VersionUpdateFeatureClass::GetCurrentVersion() const {

	// version info is stored in a file
	// for releases, the distribution script will properly fill in the
	// version number using git describe
	wxString currentVersion;
	wxFileName versionFileName = mvceditor::VersionFileAsset();
	wxFFile file(versionFileName.GetFullPath());
	if (file.IsOpened()) {
		file.ReadAll(&currentVersion);
	}
	currentVersion.Trim(true);
	return currentVersion;
}

mvceditor::VersionUpdatePreferencesPanelClass::VersionUpdatePreferencesPanelClass(wxWindow* parent,
																				  mvceditor::PreferencesClass& preferences)
: VersionUpdatePreferencesGeneratedPanelClass(parent, wxID_ANY) {
	wxGenericValidator checkValidator(&preferences.CheckForUpdates);
	CheckForUpdates->SetValidator(checkValidator);
	TransferDataToWindow();
}

mvceditor::VersionUpdateDialogClass::VersionUpdateDialogClass(wxWindow* parent, int id, 
															  mvceditor::RunningThreadsClass& runningThreads,
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

void mvceditor::VersionUpdateDialogClass::OnTimer(wxTimerEvent& event) {
	if (!StartedCheck) {
		Result->SetLabel(wxT("Checking for new version"));
		ConnectToUpdateServer();
		StartedCheck = true;
	}
	else {
		Gauge->Pulse();
	}
}

void mvceditor::VersionUpdateDialogClass::OnUpdateCheckComplete(wxCommandEvent& event) {
	Gauge->Show(false);
	wxString newVersion = event.GetString();
	int statusCode = event.GetInt();

	bool foundNew = !newVersion.empty();
	NewVersion->SetLabel(newVersion);
	if (foundNew) {
		Result->SetLabel(wxT("New version available: ") + newVersion);
	}
	else if (statusCode == 200) {
		Result->SetLabel(wxT("Your version is up-to-date"));
		NewVersion->SetLabel(CurrentVersion->GetLabel());
	}
	else {
		Result->SetLabel(wxString::Format(wxT("Connection error: (%d)"), statusCode));
	}
	Timer.Stop();
}

void mvceditor::VersionUpdateDialogClass::ConnectToUpdateServer() {	
	mvceditor::VersionUpdateActionClass* action = new mvceditor::VersionUpdateActionClass(
		RunningThreads, ID_EVENT_VERSION_UPDATES, CurrentVersion->GetLabel()
	);
	RunningThreads.Queue(action);
}

void mvceditor::VersionUpdateDialogClass::OnOkButton(wxCommandEvent& event) {
	RunningThreads.RemoveEventHandler(this);
	EndModal(wxOK);
}

mvceditor::VersionUpdateActionClass::VersionUpdateActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId, 
															  const wxString& currentVersion)
: ActionClass(runningThreads, eventId)
, CurrentVersion(currentVersion.c_str()) {
}

void mvceditor::VersionUpdateActionClass::BackgroundWork() {
	int statusCode = 0;
	wxString newVersion = GetNewVersion(CurrentVersion, statusCode);
	
	wxCommandEvent evt(EVENT_VERSION_CHECK);
	evt.SetString(newVersion);
	evt.SetInt(statusCode);
	PostEvent(evt);
}

wxString mvceditor::VersionUpdateActionClass::GetNewVersion(const wxString& currentVersion, int& statusCode) {

	// MVCEDITOR_UPDATE_HOST is a define from the premake script
	// it will be different in debug vs release
	const char* host = MVC_STR(MVCEDITOR_UPDATE_HOST);
	std::string fullUrl = "http://";
	fullUrl += host;
	fullUrl += "/updates.php";
	
	std::string data = "v=";
	data += mvceditor::WxToChar(currentVersion);
	const char* buf = data.c_str();
	const char* urlBuf = fullUrl.c_str();
	
	wxStringOutputStream ostream;

	wxCurlBase curl;
	curl.SetOpt(CURLOPT_URL, urlBuf);
	curl.SetOpt(CURLOPT_POST, 1);
	curl.SetOpt(CURLOPT_POSTFIELDS, buf);
	curl.SetOpt(CURLOPT_POSTFIELDSIZE, data.length());
	curl.SetOpt(CURLOPT_WRITEFUNCTION, wxcurl_stream_write);
	curl.SetOpt(CURLOPT_WRITEDATA, &ostream);
	curl.Perform();

	statusCode = curl.GetResponseCode();
	wxString newVersion;
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
	return newVersion;
}

wxString mvceditor::VersionUpdateActionClass::GetLabel() const {
	return wxT("Version updates");
}

BEGIN_EVENT_TABLE(mvceditor::VersionUpdateFeatureClass, mvceditor::FeatureClass)
	EVT_MENU(mvceditor::MENU_VERSION_UPDATE + 0, mvceditor::VersionUpdateFeatureClass::OnHelpCheckForUpdates)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_READY, mvceditor::VersionUpdateFeatureClass::OnAppReady)
	EVT_TIMER(ID_VERSION_FEATURE_TIMER, mvceditor::VersionUpdateFeatureClass::OnTimer)
	EVT_COMMAND(ID_EVENT_VERSION_UPDATES, EVENT_VERSION_CHECK, mvceditor::VersionUpdateFeatureClass::OnUpdateCheckComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_PREFERENCES_SAVED, mvceditor::VersionUpdateFeatureClass::OnPreferencesSaved)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_PREFERENCES_EXTERNALLY_UPDATED, mvceditor::VersionUpdateFeatureClass::OnPreferencesExternallyUpdated)
END_EVENT_TABLE()


BEGIN_EVENT_TABLE(mvceditor::VersionUpdateDialogClass, wxDialog)
	EVT_TIMER(ID_VERSION_DIALOG_TIMER, mvceditor::VersionUpdateDialogClass::OnTimer)
	EVT_COMMAND(ID_EVENT_VERSION_UPDATES, EVENT_VERSION_CHECK, mvceditor::VersionUpdateDialogClass::OnUpdateCheckComplete)
END_EVENT_TABLE()