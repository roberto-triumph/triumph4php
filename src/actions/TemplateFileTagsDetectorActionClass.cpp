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

t4p::TemplateFileTagsDetectorParamsClass::TemplateFileTagsDetectorParamsClass()
	: PhpExecutablePath()
	, PhpIncludePath()
	, ScriptName()
	, SourceDir()
	, DetectorDbFileName()
	, OutputDbFileName() {
}

wxString t4p::TemplateFileTagsDetectorParamsClass::BuildCmdLine() const {
	wxString cmdLine;
	cmdLine = wxT("\"") + PhpExecutablePath + wxT("\"") +
		wxT(" -d include_path=") + wxT("\"") + PhpIncludePath.GetPath() + wxT("\"") +
		wxT(" ") + wxT("\"") + ScriptName.GetFullPath() + wxT("\"") +
		wxT(" --sourceDir=") + wxT("\"") + SourceDir.GetPath() + wxT("\"") +
		wxT(" --detectorDbFileName=") + wxT("\"") + DetectorDbFileName.GetFullPath() + wxT("\"") +
		wxT(" --outputDbFileName=") + wxT("\"") + OutputDbFileName + wxT("\"");
	return cmdLine;
}

t4p::TemplateFileTagsDetectorActionClass::TemplateFileTagsDetectorActionClass(t4p::RunningThreadsClass& runningThreads, int eventId)
	: wxEvtHandler()
	, GlobalActionClass(runningThreads, eventId)
	, Process(*this)
	, ParamsQueue() {
}

bool t4p::TemplateFileTagsDetectorActionClass::Init(t4p::GlobalsClass& globals) {
	if (globals.Environment.Php.NotInstalled()) {
		return false;
	}
	SetStatus(_("Template File Tags Detect"));
	while (!ParamsQueue.empty()) {
		ParamsQueue.pop();
	}
	std::vector<wxString> detectorScripts = DetectorScripts();

	std::vector<t4p::ProjectClass>::const_iterator project;
	std::vector<t4p::SourceClass>::const_iterator source;
	std::vector<wxString>::const_iterator scriptName;

	// need to call each template files detector once for each different source directory
	// that's why there's 3 loops
	for (project = globals.Projects.begin(); project != globals.Projects.end(); ++project) {
		if (project->IsEnabled && project->HasSources()) {
			for (source = project->Sources.begin(); source != project->Sources.end(); ++source) {
				if (source->Exists()) {
					for (scriptName = detectorScripts.begin(); scriptName != detectorScripts.end(); ++scriptName) {
						t4p::TemplateFileTagsDetectorParamsClass params;
						params.PhpExecutablePath = globals.Environment.Php.PhpExecutablePath;
						params.PhpIncludePath = t4p::PhpDetectorsBaseAsset();
						params.ScriptName = *scriptName;
						params.SourceDir = source->RootDirectory;
						params.DetectorDbFileName = globals.DetectorCacheDbFileName.GetFullPath();
						params.OutputDbFileName = globals.DetectorCacheDbFileName.GetFullPath();
						if (params.ScriptName.FileExists()) {
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

bool t4p::TemplateFileTagsDetectorActionClass::DoAsync() {
	return false;
}

wxString t4p::TemplateFileTagsDetectorActionClass::GetLabel() const {
	return wxT("Template File Tags Detector");
}

void t4p::TemplateFileTagsDetectorActionClass::BackgroundWork() {
	// nothing is done in the background, we use ProcessWithHeartbeatClass
	// here
}

bool t4p::TemplateFileTagsDetectorActionClass::NextDetection() {
	if (ParamsQueue.empty()) {
		return false;
	}
	t4p::TemplateFileTagsDetectorParamsClass params = ParamsQueue.front();
	ParamsQueue.pop();

	wxArrayString dirs = params.SourceDir.GetDirs();
	if (!dirs.IsEmpty()) {
		SetStatus(_("Template File Tag Detect / ") + dirs.back());
	}
	wxString cmdLine = params.BuildCmdLine();
	long pid;
	return Process.Init(cmdLine, wxFileName(), ID_TEMPLATE_FILE_TAGS_DETECTOR_PROCESS, pid);
}

std::vector<wxString> t4p::TemplateFileTagsDetectorActionClass::DetectorScripts() {
	std::vector<wxString> scripts;
	t4p::RecursiveDirTraverserClass traverser(scripts);
	wxDir globalDir;
	if (globalDir.Open(t4p::TemplateFilesDetectorsGlobalAsset().GetFullPath())) {
		globalDir.Traverse(traverser, wxEmptyString, wxDIR_DIRS | wxDIR_FILES);
	}
	wxDir localDir;
	if (localDir.Open(t4p::TemplateFileTagsDetectorsLocalAsset().GetFullPath())) {
		localDir.Traverse(traverser, wxEmptyString, wxDIR_DIRS | wxDIR_FILES);
	}
	return  scripts;
}

void t4p::TemplateFileTagsDetectorActionClass::OnProcessComplete(wxCommandEvent &event) {
	if (ParamsQueue.empty()) {
		SignalEnd();
	}
	else {
		NextDetection();
	}
}

void t4p::TemplateFileTagsDetectorActionClass::OnProcessFailed(wxCommandEvent &event) {
	wxString msg = event.GetString();
	wxString extensionMissingErr = wxT("requires the PDO and pdo_sqlite PHP extensions.");
	if (msg.Find(extensionMissingErr) != wxNOT_FOUND) {
		t4p::EditorLogError(t4p::ERR_MISSING_PHP_EXTENSIONS, msg);
	}
	else {
		t4p::EditorLogErrorFix(event.GetString(), _("There is an unexpected error in the detector. Was it modified?"));
	}
	if (ParamsQueue.empty()) {
		SignalEnd();
	}
	else {
		NextDetection();
	}
}


BEGIN_EVENT_TABLE(t4p::TemplateFileTagsDetectorActionClass, wxEvtHandler)
	EVT_COMMAND(ID_TEMPLATE_FILE_TAGS_DETECTOR_PROCESS, t4p::EVENT_PROCESS_COMPLETE, t4p::TemplateFileTagsDetectorActionClass::OnProcessComplete)
	EVT_COMMAND(ID_TEMPLATE_FILE_TAGS_DETECTOR_PROCESS, t4p::EVENT_PROCESS_FAILED, t4p::TemplateFileTagsDetectorActionClass::OnProcessFailed)
END_EVENT_TABLE()
