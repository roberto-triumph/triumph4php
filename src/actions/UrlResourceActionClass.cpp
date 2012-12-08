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
#include <actions/UrlResourceActionClass.h>

mvceditor::UrlResourceActionClass::UrlResourceActionClass(
	mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: ActionClass(runningThreads, eventId)
	, PhpFrameworks(*this, runningThreads) 
	, DetectedFrameworks()
	, ResourceDbFileNames()
	, Environment() {

	// using connect instead of event tables so that there is no chance that an
	// event handler gets called after this instance is destroyed
	// the reason for doing this is that action are only alive for a short
	// period of time
	Connect(mvceditor::EVENT_FRAMEWORK_URL_COMPLETE, 
		(wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent(UrlDetectedEventClassFunction, &mvceditor::UrlResourceActionClass::OnUrlDetectionComplete), 
		NULL, this
	);
	Connect(mvceditor::EVENT_FRAMEWORK_URL_FAILED, wxCommandEventHandler(mvceditor::UrlResourceActionClass::OnUrlDetectionFailed), NULL, this);
	Connect(mvceditor::EVENT_PROCESS_IN_PROGRESS, wxCommandEventHandler(mvceditor::UrlResourceActionClass::OnUrlDetectionInProgress), NULL, this);
}

mvceditor::UrlResourceActionClass::~UrlResourceActionClass() {
	Disconnect(mvceditor::EVENT_FRAMEWORK_URL_COMPLETE, 
		(wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent(UrlDetectedEventClassFunction, &mvceditor::UrlResourceActionClass::OnUrlDetectionComplete), 
		NULL, this);
	Disconnect(mvceditor::EVENT_FRAMEWORK_URL_FAILED, wxCommandEventHandler(mvceditor::UrlResourceActionClass::OnUrlDetectionFailed), NULL, this);
	Disconnect(mvceditor::EVENT_PROCESS_IN_PROGRESS, wxCommandEventHandler(mvceditor::UrlResourceActionClass::OnUrlDetectionInProgress), NULL, this);
	PhpFrameworks.Stop();
}

bool mvceditor::UrlResourceActionClass::Init(mvceditor::GlobalsClass& globals) {
	globals.UrlResourceFinder.Urls.clear();
	globals.UrlResourceFinder.ChosenUrl.Reset();
	globals.UrlResourceFinder.Clear();
	
	Environment = globals.Environment;
	DetectedFrameworks = globals.Frameworks;

	// look to see if any source directory is a virtual host doc root
	bool started = false;
	std::vector<mvceditor::ProjectClass>::const_iterator project;
	for (project = globals.Projects.begin(); project != globals.Projects.end(); ++project) {
		if (project->IsEnabled) {
			std::vector<mvceditor::SourceClass>::const_iterator source;

			// a single project can have multiple directories. check each directory, and if 
			// that directory is a web root then get the URLs for that project.
			for (source = project->Sources.begin(); source != project->Sources.end(); ++source) {
				wxString rootDirFullPath = source->RootDirectory.GetFullPath();
				wxString projectRootUrl =  Environment.Apache.GetUrl(rootDirFullPath);
				if (!projectRootUrl.IsEmpty()) {
					ResourceDbFileNames.push(project->ResourceDbFileName.GetFullPath());
					RootUrls.push(projectRootUrl);
				}
			}
		}
	}

	if (!ResourceDbFileNames.empty()) {
		wxString resourceDbFileName = ResourceDbFileNames.front();
		wxString rootUrl = RootUrls.front();
		ResourceDbFileNames.pop();
		RootUrls.pop();
		started = PhpFrameworks.InitUrlDetector(DetectedFrameworks, resourceDbFileName, rootUrl, Environment);
		if (started) {
			SetStatus(_("Detecting URL routes for ") + rootUrl);
		}
	}
	if (!started) {

		// nothing to do we are done. this needs to be called so that we can move on
		// to the next step in the sequence
		wxCommandEvent evt(mvceditor::EVENT_WORK_COMPLETE);
		PostEvent(evt);
	}
	return started;
}

bool mvceditor::UrlResourceActionClass::DoAsync() {
	return false;
}

void mvceditor::UrlResourceActionClass::BackgroundWork() {
	// nothing here, work will be done asynchronously by wxExecute. see FrameworkDetectorClass
	// for more info
}

void mvceditor::UrlResourceActionClass::OnUrlDetectionFailed(wxCommandEvent& event) {
	bool done = true;
	while (!ResourceDbFileNames.empty()) {
		
		// move on to the next directory, skipping over failed directories
		wxString resourceDbFileName = ResourceDbFileNames.front();
		wxString rootUrl = RootUrls.front();
		ResourceDbFileNames.pop();
		RootUrls.pop();
		bool started = PhpFrameworks.InitUrlDetector(DetectedFrameworks, resourceDbFileName, rootUrl, Environment);
		if (started) {
			SetStatus(_("Detecting URL routes for ") + rootUrl);
			done = false;
			break;
		}
	}
	if (done) {

		// no more sources to check. show the error but move still signal that we are done
		wxCommandEvent doneEvent(mvceditor::EVENT_WORK_COMPLETE);
		PostEvent(doneEvent);
	}
}

void mvceditor::UrlResourceActionClass::OnUrlDetectionComplete(mvceditor::UrlDetectedEventClass& event) {

	// propagate the event so that the globalschangehandler class knows to update the globals
	PostEvent(event);

	bool done = true;
	while (!ResourceDbFileNames.empty()) {
		
		// move on to the next directory, skipping over failed directories
		wxString resourceDbFileName = ResourceDbFileNames.front();
		wxString rootUrl = RootUrls.front();
		ResourceDbFileNames.pop();
		RootUrls.pop();
		bool started = PhpFrameworks.InitUrlDetector(DetectedFrameworks, resourceDbFileName, rootUrl, Environment);
		if (started) {
			SetStatus(_("Detecting URL routes for ") + rootUrl);
			done = false;
			break;
		}
	}
	if (done) {
		// nothing to do we are done. this needs to be called so that we can move on
		// to the next step in the sequence
		wxCommandEvent evt(mvceditor::EVENT_WORK_COMPLETE);
		PostEvent(evt);
	}
}

void mvceditor::UrlResourceActionClass::OnUrlDetectionInProgress(wxCommandEvent& event) {
	wxCommandEvent progressEvent(mvceditor::EVENT_WORK_IN_PROGRESS);
	PostEvent(progressEvent);
}

wxString mvceditor::UrlResourceActionClass::GetLabel() const {
	return _("MVC Framework Routing Detection");
}