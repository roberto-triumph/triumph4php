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
#include <actions/UrlTagDetectorActionClass.h>
#include <search/RecursiveDirTraverserClass.h>
#include <globals/Errors.h>
#include <globals/Assets.h>
#include <soci/sqlite3/soci-sqlite3.h>

static int ID_URL_TAG_DETECTOR_PROCESS = wxNewId();

mvceditor::UrlTagFinderInitActionClass::UrlTagFinderInitActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: InitializerGlobalActionClass(runningThreads, eventId) {

}

void mvceditor::UrlTagFinderInitActionClass::Work(mvceditor::GlobalsClass& globals) {
	SetStatus(_("Url Tag Finder Init"));
	globals.DetectorCacheSession.close();
	globals.DetectorCacheSession.open(*soci::factory_sqlite3(), 
		mvceditor::WxToChar(globals.DetectorCacheDbFileName.GetFullPath()));
	globals.UrlTagFinder.InitSession(&globals.DetectorCacheSession);
}

wxString mvceditor::UrlTagFinderInitActionClass::GetLabel() const {
	return wxT("URL Tag Finder Init");
}

mvceditor::UrlTagDetectorParamsClass::UrlTagDetectorParamsClass() 
	: PhpExecutablePath()
	, PhpIncludePath()
	, ScriptName()
	, SourceDir()
	, ResourceDbFileName()
	, RootUrl()
	, OutputDbFileName() {

}

wxString mvceditor::UrlTagDetectorParamsClass::BuildCmdLine() const {
	wxString cmdLine;
	cmdLine = wxT("\"") + PhpExecutablePath + wxT("\"") + 
		wxT(" -d include_path=") + wxT("\"") + PhpIncludePath.GetPath() + wxT("\"") + 
		wxT(" ") + wxT("\"") + ScriptName.GetFullPath() + wxT("\"") + 
		wxT(" --sourceDir=") + wxT("\"") + SourceDir.GetPath() + wxT("\"") + 
		wxT(" --resourceDbFileName=") + wxT("\"") + ResourceDbFileName.GetFullPath() + wxT("\"") + 
		wxT(" --host=") + wxT("\"") + RootUrl + wxT("\"") +
		wxT(" --outputDbFileName=") + wxT("\"") + OutputDbFileName + wxT("\"");
	return cmdLine;
}

mvceditor::UrlTagDetectorActionClass::UrlTagDetectorActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: wxEvtHandler()
	, GlobalActionClass(runningThreads, eventId)
	, Process(*this)
	, ParamsQueue() {

}

bool mvceditor::UrlTagDetectorActionClass::Init(mvceditor::GlobalsClass& globals) {
	SetStatus(_("Url Tag Detect"));
	while (!ParamsQueue.empty()) {
		ParamsQueue.pop();
	}
	std::vector<wxString> detectorScripts = DetectorScripts();

	std::vector<mvceditor::ProjectClass>::const_iterator project;
	std::vector<mvceditor::SourceClass>::const_iterator source;
	std::vector<wxString>::const_iterator scriptName;

	// need to call each url detector once for each different source directory
	// that's why there's 3 loops
	for (project = globals.Projects.begin(); project != globals.Projects.end(); ++project) {
		if (project->IsEnabled) {
			for (source = project->Sources.begin(); source != project->Sources.end(); ++source) {
				if (source->Exists()) {
					for (scriptName = detectorScripts.begin(); scriptName != detectorScripts.end(); ++scriptName) {
						mvceditor::UrlTagDetectorParamsClass params;
						params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
						params.PhpIncludePath = mvceditor::PhpDetectorsBaseAsset();
						params.ScriptName = *scriptName;
						params.SourceDir = source->RootDirectory;
						params.ResourceDbFileName = globals.TagCacheDbFileName.GetFullPath();
						params.RootUrl = globals.Environment.Apache.GetUrl(source->RootDirectory.GetPath());
						params.OutputDbFileName = globals.DetectorCacheDbFileName.GetFullPath();
						if (!params.RootUrl.IsEmpty()) {
							ParamsQueue.push(params);
						}
					}
				}
			}
		}
	}
	bool started = false;
	if (!ParamsQueue.empty()) {
		
		// start the first external process
		started = NextDetection();
	}
	return started;
}

bool mvceditor::UrlTagDetectorActionClass::DoAsync() {
	return false;
}

wxString mvceditor::UrlTagDetectorActionClass::GetLabel() const {
	return wxT("URL Detectors");
}

void mvceditor::UrlTagDetectorActionClass::BackgroundWork() {
	// nothing is done in the background, we use ProcessWithHeartbeatClass
	// here
}

bool mvceditor::UrlTagDetectorActionClass::NextDetection() {
	if (ParamsQueue.empty()) {
		return false;
	}
	mvceditor::UrlTagDetectorParamsClass params = ParamsQueue.front();
	ParamsQueue.pop();
	wxArrayString dirs = params.SourceDir.GetDirs();
	if (!dirs.IsEmpty()) {
		SetStatus(_("Url Tag Detect / ") +  dirs.back());
	}
	wxString cmdLine = params.BuildCmdLine();
	long pid;
	return Process.Init(cmdLine, ID_URL_TAG_DETECTOR_PROCESS, pid);
}

std::vector<wxString> mvceditor::UrlTagDetectorActionClass::DetectorScripts() {
	std::vector<wxString> scripts;
	mvceditor::RecursiveDirTraverserClass traverser(scripts);
	wxDir globalDir;
	if (globalDir.Open(mvceditor::UrlTagDetectorsGlobalAsset().GetFullPath())) {
		globalDir.Traverse(traverser, wxEmptyString, wxDIR_DIRS | wxDIR_FILES);
	}
	wxDir localDir;
	if (localDir.Open(mvceditor::UrlTagDetectorsLocalAsset().GetFullPath())) {
		localDir.Traverse(traverser, wxEmptyString, wxDIR_DIRS | wxDIR_FILES);
	}
	return  scripts;
}

void mvceditor::UrlTagDetectorActionClass::OnProcessComplete(wxCommandEvent &event) {
	if (ParamsQueue.empty()) {
		wxCommandEvent completeEvent(mvceditor::EVENT_WORK_COMPLETE);
		PostEvent(completeEvent);
	}
	else {
		NextDetection();
	}
}

void mvceditor::UrlTagDetectorActionClass::OnProcessFailed(wxCommandEvent &event) {
	mvceditor::EditorLogError(mvceditor::WARNING_OTHER, event.GetString());
	if (ParamsQueue.empty()) {
		wxCommandEvent completeEvent(mvceditor::EVENT_WORK_COMPLETE);
		PostEvent(completeEvent);
	}
	else {
		NextDetection();
	}
}

void mvceditor::UrlTagDetectorActionClass::OnProcessInProgress(wxCommandEvent &event) {
	wxCommandEvent inProgressEvent(mvceditor::EVENT_WORK_IN_PROGRESS);
	PostEvent(inProgressEvent);
}

BEGIN_EVENT_TABLE(mvceditor::UrlTagDetectorActionClass, wxEvtHandler) 
	EVT_COMMAND(ID_URL_TAG_DETECTOR_PROCESS, mvceditor::EVENT_PROCESS_IN_PROGRESS, mvceditor::UrlTagDetectorActionClass::OnProcessInProgress)
	EVT_COMMAND(ID_URL_TAG_DETECTOR_PROCESS, mvceditor::EVENT_PROCESS_COMPLETE, mvceditor::UrlTagDetectorActionClass::OnProcessComplete)
	EVT_COMMAND(ID_URL_TAG_DETECTOR_PROCESS, mvceditor::EVENT_PROCESS_FAILED, mvceditor::UrlTagDetectorActionClass::OnProcessFailed)
END_EVENT_TABLE()