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
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <actions/ProjectFrameworkDetectionActionClass.h>
#include <globals/Errors.h>
#include <globals/Assets.h>

mvceditor::ProjectFrameworkDetectionActionClass::ProjectFrameworkDetectionActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: ActionClass(runningThreads, eventId) 
	, PhpFrameworks(NULL) 
	, RunningThreads(runningThreads) {

}

mvceditor::ProjectFrameworkDetectionActionClass::~ProjectFrameworkDetectionActionClass() {
	if (PhpFrameworks) {
		PhpFrameworks->Stop();
		delete PhpFrameworks;
	}
}

bool mvceditor::ProjectFrameworkDetectionActionClass::DoAsync() {
	return false;
}

bool mvceditor::ProjectFrameworkDetectionActionClass::Init(mvceditor::GlobalsClass& globals) {
	if (PhpFrameworks) {
		delete PhpFrameworks;
		PhpFrameworks = NULL;
	}

	// clear the detected framework info
	globals.Frameworks.clear();

	// only remove the database infos that were detected from
	// PHP frameworks, leave the ones that the user created intact
	globals.ClearDetectedInfos();

	DirectoriesToDetect.clear();
	std::vector<mvceditor::SourceClass> allSources = globals.AllEnabledSources();
	bool started = false;
	PhpFrameworks = new mvceditor::PhpFrameworkDetectorClass(*this, RunningThreads, globals.Environment);
	if (!allSources.empty()) {		
		for (size_t i = 1; i < allSources.size(); ++i) {
			DirectoriesToDetect.push_back(allSources[i].RootDirectory.GetPath());
		}
		started = PhpFrameworks->Init(allSources[0].RootDirectory.GetPath());
		if (!started) {
			mvceditor::EditorLogError(mvceditor::BAD_PHP_EXECUTABLE, globals.Environment.Php.PhpExecutablePath); 
			DirectoriesToDetect.clear();
		}
		else {
			SetStatus(_("Detecting Frameworks for ") + allSources[0].RootDirectory.GetName());
		}
	}
	if (!started) {
		delete PhpFrameworks;
		PhpFrameworks = NULL;

		// nothing to do we are done. this needs to be called so that we can move on
		// to the next step in the sequence
		wxCommandEvent evt(mvceditor::EVENT_WORK_COMPLETE);
		PostEvent(evt);
	}
	return true;
}

void mvceditor::ProjectFrameworkDetectionActionClass::OnFrameworkDetectionComplete(wxCommandEvent& event) {
	bool finished = true;

	// detection on the next directory; skipping any errors
	while (!DirectoriesToDetect.empty() && finished) {
		wxString nextDirectory = DirectoriesToDetect.back();
		DirectoriesToDetect.pop_back();
		if (PhpFrameworks->Init(nextDirectory)) {
			finished = false;
			wxFileName fileName(nextDirectory);
			SetStatus(_("Detecting Frameworks for ") + fileName.GetName());
		}
	}
	if (finished) {
		delete PhpFrameworks;
		PhpFrameworks = NULL;

		// nothing to do we are done. this needs to be called so that we can move on
		// to the next step in the sequence
		wxCommandEvent evt(mvceditor::EVENT_WORK_COMPLETE);
		PostEvent(evt);
	}
}

void mvceditor::ProjectFrameworkDetectionActionClass::OnFrameworkDetectionFailed(wxCommandEvent& event) {
	mvceditor::EditorLogError(mvceditor::BAD_PHP_EXECUTABLE, event.GetString());
	bool finished = true;

	// detection on the next directory; skipping any errors
	while (!DirectoriesToDetect.empty() && finished) {
		wxString nextDirectory = DirectoriesToDetect.back();
		DirectoriesToDetect.pop_back();
		if (PhpFrameworks->Init(nextDirectory)) {
			finished = false;
			wxFileName fileName(nextDirectory);
			SetStatus(_("Detecting Frameworks for ") + fileName.GetName());
		}
	}
	if (finished) {
		delete PhpFrameworks;
		PhpFrameworks = NULL;

		// nothing to do we are done. this needs to be called so that we can move on
		// to the next step in the sequence
		wxCommandEvent evt(mvceditor::EVENT_WORK_COMPLETE);
		PostEvent(evt);
	}
}

void mvceditor::ProjectFrameworkDetectionActionClass::OnFrameworkDetectionInProgress(wxCommandEvent& event) {
	wxCommandEvent newEvt(mvceditor::EVENT_WORK_IN_PROGRESS);
	PostEvent(newEvt);
}

void mvceditor::ProjectFrameworkDetectionActionClass::BackgroundWork() {
	// do nothing in the background, since this class uses a wxExecute that 
	// is already asynchronous
}

void mvceditor::ProjectFrameworkDetectionActionClass::OnFrameworkFound(mvceditor::FrameworkFoundEventClass& event) {
	PostEvent(event);
}

wxString mvceditor::ProjectFrameworkDetectionActionClass::GetLabel() const {
	return _("MVC Framework Detection");
}

BEGIN_EVENT_TABLE(mvceditor::ProjectFrameworkDetectionActionClass, wxEvtHandler)
	EVT_FRAMEWORK_FOUND(mvceditor::ProjectFrameworkDetectionActionClass::OnFrameworkFound)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_FRAMEWORK_DETECTION_COMPLETE, mvceditor::ProjectFrameworkDetectionActionClass::OnFrameworkDetectionComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_FRAMEWORK_DETECTION_FAILED, mvceditor::ProjectFrameworkDetectionActionClass::OnFrameworkDetectionFailed)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_PROCESS_IN_PROGRESS, mvceditor::ProjectFrameworkDetectionActionClass::OnFrameworkDetectionInProgress)
END_EVENT_TABLE()
