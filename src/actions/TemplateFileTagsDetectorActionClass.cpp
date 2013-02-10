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
#include <actions/TemplateFileTagsDetectorActionClass.h>
#include <globals/Assets.h>
#include <globals/Errors.h>
#include <search/RecursiveDirTraverserClass.h>

static int ID_TEMPLATE_FILE_TAGS_DETECTOR_PROCESS = wxNewId();

mvceditor::TemplateFileTagsDetectorParamsClass::TemplateFileTagsDetectorParamsClass() 
	: PhpExecutablePath()
	, PhpIncludePath()
	, ScriptName()
	, SourceDir()
	, DetectorDbFileName()
	, OutputDbFileName() {

}

wxString mvceditor::TemplateFileTagsDetectorParamsClass::BuildCmdLine() const {
	wxString cmdLine;
	cmdLine = wxT("\"") + PhpExecutablePath + wxT("\"") + 
		wxT(" -d include_path=") + wxT("\"") + PhpIncludePath.GetPath() + wxT("\"") + 
		wxT(" ") + wxT("\"") + ScriptName.GetFullPath() + wxT("\"") + 
		wxT(" --sourceDir=") + wxT("\"") + SourceDir.GetPath() + wxT("\"") + 
		wxT(" --detectorDbFileName=") + wxT("\"") + DetectorDbFileName.GetFullPath() + wxT("\"") + 
		wxT(" --outputDbFileName=") + wxT("\"") + OutputDbFileName + wxT("\"");
	return cmdLine;
}

mvceditor::TemplateFileTagsDetectorActionClass::TemplateFileTagsDetectorActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: ActionClass(runningThreads, eventId)
	, Process(*this)
	, ParamsQueue() {

}

bool mvceditor::TemplateFileTagsDetectorActionClass::Init(mvceditor::GlobalsClass& globals) {
	SetStatus(_("Template File Tags Detect"));
	while (!ParamsQueue.empty()) {
		ParamsQueue.pop();
	}
	std::vector<wxString> detectorScripts = DetectorScripts();

	std::vector<mvceditor::ProjectClass>::const_iterator project;
	std::vector<mvceditor::SourceClass>::const_iterator source;
	std::vector<wxString>::const_iterator scriptName;

	// need to call each template files detector once for each different source directory
	// that's why there's 3 loops
	for (project = globals.Projects.begin(); project != globals.Projects.end(); ++project) {
		for (source = project->Sources.begin(); source != project->Sources.end(); ++source) {
			if (source->Exists()) {
				for (scriptName = detectorScripts.begin(); scriptName != detectorScripts.end(); ++scriptName) {
					mvceditor::TemplateFileTagsDetectorParamsClass params;
					params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
					params.PhpIncludePath = mvceditor::PhpDetectorsBaseAsset();
					params.ScriptName = *scriptName;
					params.SourceDir = source->RootDirectory;
					params.DetectorDbFileName = project->DetectorDbFileName.GetFullPath();
					params.OutputDbFileName = project->DetectorDbFileName.GetFullPath();
					if (params.ScriptName.FileExists()) {
						ParamsQueue.push(params);
					}
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

bool mvceditor::TemplateFileTagsDetectorActionClass::DoAsync() {
	return false;
}

wxString mvceditor::TemplateFileTagsDetectorActionClass::GetLabel() const {
	return wxT("Template File Tags Detector");
}

void mvceditor::TemplateFileTagsDetectorActionClass::BackgroundWork() {
	// nothing is done in the background, we use ProcessWithHeartbeatClass
	// here
}

void mvceditor::TemplateFileTagsDetectorActionClass::NextDetection() {
	if (ParamsQueue.empty()) {
		return;
	}
	mvceditor::TemplateFileTagsDetectorParamsClass params = ParamsQueue.front();
	ParamsQueue.pop();

	wxArrayString dirs = params.SourceDir.GetDirs();
	if (!dirs.IsEmpty()) {
		SetStatus(_("Template File Tag Detect / ") + dirs.back());
	}
	wxString cmdLine = params.BuildCmdLine();
	long pid;
	Process.Init(cmdLine, ID_TEMPLATE_FILE_TAGS_DETECTOR_PROCESS, pid);
}

std::vector<wxString> mvceditor::TemplateFileTagsDetectorActionClass::DetectorScripts() {
	std::vector<wxString> scripts;
	mvceditor::RecursiveDirTraverserClass traverser(scripts);
	wxDir globalDir;
	if (globalDir.Open(mvceditor::TemplateFilesDetectorsGlobalAsset().GetFullPath())) {
		globalDir.Traverse(traverser, wxEmptyString, wxDIR_DIRS | wxDIR_FILES);
	}
	wxDir localDir;
	if (localDir.Open(mvceditor::TemplateFilesDetectorsLocalAsset().GetFullPath())) {
		localDir.Traverse(traverser, wxEmptyString, wxDIR_DIRS | wxDIR_FILES);
	}
	return  scripts;
}

void mvceditor::TemplateFileTagsDetectorActionClass::OnProcessComplete(wxCommandEvent &event) {
	if (ParamsQueue.empty()) {
		wxCommandEvent completeEvent(mvceditor::EVENT_WORK_COMPLETE);
		PostEvent(completeEvent);
	}
	else {
		NextDetection();
	}
}

void mvceditor::TemplateFileTagsDetectorActionClass::OnProcessFailed(wxCommandEvent &event) {
	mvceditor::EditorLogError(mvceditor::WARNING_OTHER, event.GetString());
	if (ParamsQueue.empty()) {
		wxCommandEvent completeEvent(mvceditor::EVENT_WORK_COMPLETE);
		PostEvent(completeEvent);
	}
	else {
		NextDetection();
	}
}

void mvceditor::TemplateFileTagsDetectorActionClass::OnProcessInProgress(wxCommandEvent &event) {
	wxCommandEvent inProgressEvent(mvceditor::EVENT_WORK_IN_PROGRESS);
	PostEvent(inProgressEvent);
}

BEGIN_EVENT_TABLE(mvceditor::TemplateFileTagsDetectorActionClass, mvceditor::ActionClass) 
	EVT_COMMAND(ID_TEMPLATE_FILE_TAGS_DETECTOR_PROCESS, mvceditor::EVENT_PROCESS_IN_PROGRESS, mvceditor::TemplateFileTagsDetectorActionClass::OnProcessInProgress)
	EVT_COMMAND(ID_TEMPLATE_FILE_TAGS_DETECTOR_PROCESS, mvceditor::EVENT_PROCESS_COMPLETE, mvceditor::TemplateFileTagsDetectorActionClass::OnProcessComplete)
	EVT_COMMAND(ID_TEMPLATE_FILE_TAGS_DETECTOR_PROCESS, mvceditor::EVENT_PROCESS_FAILED, mvceditor::TemplateFileTagsDetectorActionClass::OnProcessFailed)
END_EVENT_TABLE()
