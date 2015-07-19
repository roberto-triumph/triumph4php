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
	wxOutputStream* ostream = reinterpret_cast<wxOutputStream*>(stream);
	if (ostream) {
		ostream->Write(ptr, bytes);
		return ostream->LastWrite();
	}
	return 0;
}

t4p::VersionUpdateFeatureClass::VersionUpdateFeatureClass(t4p::AppClass& app)
: FeatureClass(app)
, NextCheckTime() {
}


void t4p::VersionUpdateFeatureClass::LoadPreferences(wxConfigBase* config) {
	wxString readTime;
	config->Read(wxT("VersionUpdates/NextCheckTime"), &readTime);
	if (!NextCheckTime.ParseDateTime(readTime)) {
		NextCheckTime = wxDateTime::Now();
	}
}

void t4p::VersionUpdateFeatureClass::OnAppReady(wxCommandEvent& event) {
	CURLcode code = curl_global_init(CURL_GLOBAL_DEFAULT);
	bool good = 0 == code;
	wxUnusedVar(good);
	wxASSERT_MSG(good, _("curl did not initialize"));
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
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_READY, t4p::VersionUpdateFeatureClass::OnAppReady)
END_EVENT_TABLE()
