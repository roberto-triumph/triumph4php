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
#include <actions/UrlDetectorActionClass.h>
#include <globals/Errors.h>
#include <globals/Assets.h>

static int ID_URL_DETECTOR_PROCESS = wxNewId();

class DirTraverserClass : public wxDirTraverser {
public:

    DirTraverserClass(std::vector<wxString>& fullPaths) 
		: FullPaths(fullPaths) {
	}

    virtual wxDirTraverseResult OnFile(const wxString& filename)
    {
        FullPaths.push_back(filename);
        return wxDIR_CONTINUE;
    }

    virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname))
    {
        return wxDIR_CONTINUE;
    }

private:

    std::vector<wxString>& FullPaths;
};

mvceditor::UrlResourceFinderInitActionClass::UrlResourceFinderInitActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: InitializerActionClass(runningThreads, eventId) {

}

void mvceditor::UrlResourceFinderInitActionClass::Work(mvceditor::GlobalsClass& globals) {
	globals.UrlResourceFinder.Close();
	std::vector<mvceditor::ProjectClass>::const_iterator project;
	for (project = globals.Projects.begin(); project != globals.Projects.end(); ++project) {
		if (project->IsEnabled) {
			globals.UrlResourceFinder.AttachFile(project->DetectorDbFileName);
		}
	}
}

wxString mvceditor::UrlResourceFinderInitActionClass::GetLabel() const {
	return wxT("URL Detection Init");
}

mvceditor::UrlDetectorParamsClass::UrlDetectorParamsClass() 
	: PhpExecutablePath()
	, PhpIncludePath()
	, ScriptName()
	, SourceDir()
	, ResourceDbFileName()
	, RootUrl()
	, OutputDbFileName() {

}

wxString mvceditor::UrlDetectorParamsClass::BuildCmdLine() const {
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

mvceditor::UrlDetectorActionClass::UrlDetectorActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: ActionClass(runningThreads, eventId)
	, Process(*this)
	, ParamsQueue() {

}

bool mvceditor::UrlDetectorActionClass::Init(mvceditor::GlobalsClass& globals) {
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
		for (source = project->Sources.begin(); source != project->Sources.end(); ++source) {
			for (scriptName = detectorScripts.begin(); scriptName != detectorScripts.end(); ++scriptName) {
				mvceditor::UrlDetectorParamsClass params;
				params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
				params.PhpIncludePath = mvceditor::PhpDetectorsBaseAsset();
				params.ScriptName = *scriptName;
				params.SourceDir = source->RootDirectory;
				params.ResourceDbFileName = project->ResourceDbFileName.GetFullPath();
				params.RootUrl = globals.Environment.Apache.GetUrl(source->RootDirectory.GetPath());
				params.OutputDbFileName = project->DetectorDbFileName.GetFullPath();
				if (!params.RootUrl.IsEmpty()) {
					ParamsQueue.push(params);
				}
			}
		}
	}
	bool started = false;
	if (!ParamsQueue.empty()) {
		
		// start the first external process
		NextDetection();
		started = true;
	}
	return started;
}

bool mvceditor::UrlDetectorActionClass::DoAsync() {
	return false;
}

wxString mvceditor::UrlDetectorActionClass::GetLabel() const {
	return wxT("URL Detectors");
}

void mvceditor::UrlDetectorActionClass::BackgroundWork() {

}

void mvceditor::UrlDetectorActionClass::NextDetection() {
	if (ParamsQueue.empty()) {
		return;
	}
	mvceditor::UrlDetectorParamsClass params = ParamsQueue.front();
	ParamsQueue.pop();

	SetStatus(_("Determining Routes for ") + params.RootUrl);

	wxString cmdLine = params.BuildCmdLine();
	long pid;
	Process.Init(cmdLine, ID_URL_DETECTOR_PROCESS, pid);
}

std::vector<wxString> mvceditor::UrlDetectorActionClass::DetectorScripts() {
	std::vector<wxString> scripts;
	DirTraverserClass traverser(scripts);
	wxDir globalDir;
	if (globalDir.Open(mvceditor::UrlDetectorsGlobalAsset().GetFullPath())) {
		globalDir.Traverse(traverser, wxEmptyString, wxDIR_DIRS | wxDIR_FILES);
	}
	wxDir localDir;
	if (localDir.Open(mvceditor::UrlDetectorsLocalAsset().GetFullPath())) {
		localDir.Traverse(traverser, wxEmptyString, wxDIR_DIRS | wxDIR_FILES);
	}
	return  scripts;
}

void mvceditor::UrlDetectorActionClass::OnProcessComplete(wxCommandEvent &event) {
	if (ParamsQueue.empty()) {
		wxCommandEvent completeEvent(mvceditor::EVENT_WORK_COMPLETE);
		PostEvent(completeEvent);
	}
	else {
		NextDetection();
	}
}

void mvceditor::UrlDetectorActionClass::OnProcessFailed(wxCommandEvent &event) {
	mvceditor::EditorLogError(mvceditor::WARNING_OTHER, event.GetString());
	if (ParamsQueue.empty()) {
		wxCommandEvent completeEvent(mvceditor::EVENT_WORK_COMPLETE);
		PostEvent(completeEvent);
	}
	else {
		NextDetection();
	}
}

void mvceditor::UrlDetectorActionClass::OnProcessInProgress(wxCommandEvent &event) {
	wxCommandEvent inProgressEvent(mvceditor::EVENT_WORK_IN_PROGRESS);
	PostEvent(inProgressEvent);
}

BEGIN_EVENT_TABLE(mvceditor::UrlDetectorActionClass, mvceditor::ActionClass) 
	EVT_COMMAND(ID_URL_DETECTOR_PROCESS, mvceditor::EVENT_PROCESS_IN_PROGRESS, mvceditor::UrlDetectorActionClass::OnProcessInProgress)
	EVT_COMMAND(ID_URL_DETECTOR_PROCESS, mvceditor::EVENT_PROCESS_COMPLETE, mvceditor::UrlDetectorActionClass::OnProcessComplete)
END_EVENT_TABLE()