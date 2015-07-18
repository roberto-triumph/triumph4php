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
#include <features/EnvironmentFeatureClass.h>
#include <Triumph.h>
#include <wx/filename.h>
#include <wx/string.h>

const wxEventType t4p::EVENT_APACHE_FILE_READ_COMPLETE = wxNewEventType();

t4p::ApacheFileReadCompleteEventClass::ApacheFileReadCompleteEventClass(int eventId, const t4p::ApacheClass &apache)
	: wxEvent(eventId, t4p::EVENT_APACHE_FILE_READ_COMPLETE)
	, Apache(apache) {

}

wxEvent* t4p::ApacheFileReadCompleteEventClass::Clone() const {
	t4p::ApacheFileReadCompleteEventClass* evt = new t4p::ApacheFileReadCompleteEventClass(GetId(), Apache);
	return evt;
}

t4p::ApacheFileReaderClass::ApacheFileReaderClass(t4p::RunningThreadsClass& runningThreads, int eventId)
: BackgroundFileReaderClass(runningThreads, eventId)
	, ApacheResults() {
}

bool t4p::ApacheFileReaderClass::Init(const wxString& startDirectory) {
	ApacheResults.ManualConfiguration = false;
	ApacheResults.ClearMappings();
	return BackgroundFileReaderClass::Init(startDirectory);
}

wxString t4p::ApacheFileReaderClass::GetLabel() const {
	return wxT("Apache File Reader");
}

bool t4p::ApacheFileReaderClass::BackgroundFileMatch(const wxString& file) {
	return true;
}

bool t4p::ApacheFileReaderClass::BackgroundFileRead(t4p::DirectorySearchClass& search) {
	bool ret = false;
	if (search.Walk(ApacheResults)) {
		ret = true;
	}
	if (!search.More() && !IsCancelled()) {
		
		// when we are done recursing, parse the matched files
		std::vector<wxString> possibleConfigFiles = search.GetMatchedFiles();
		
		// there may be multiple files, at this point just exist as soon as we find one file
		// that we can recognize as a config file
		bool found = false;
		for (size_t i = 0; i <  possibleConfigFiles.size(); ++i) {
			if (ApacheResults.SetHttpdPath(possibleConfigFiles[i])) {
				found = true;
				break;
			}
		}

		// send the event once we have searched all files
		if (found) {
			t4p::ApacheFileReadCompleteEventClass evt(wxID_ANY, ApacheResults);
			PostEvent(evt);
		}
	}
	return ret;
}

t4p::EnvironmentFeatureClass::EnvironmentFeatureClass(t4p::AppClass& app)
	: FeatureClass(app) {
}

void t4p::EnvironmentFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {
	wxConfigBase* config = wxConfigBase::Get();
	App.Globals.Environment.SaveToConfig(config);
	if (App.Globals.Environment.Php.IsAuto && App.Globals.Environment.Php.Installed) {
		App.Globals.Environment.Php.AutoDetermine();
	}

	// signal that this app has modified the config file, that way the external
	// modification check fails and the user will not be prompted to reload the config
	App.UpdateConfigModifiedTime();
}

BEGIN_EVENT_TABLE(t4p::EnvironmentFeatureClass, wxEvtHandler) 
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::EnvironmentFeatureClass::OnPreferencesSaved) 	
END_EVENT_TABLE()
