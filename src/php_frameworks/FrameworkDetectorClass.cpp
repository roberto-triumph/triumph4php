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
#include <php_frameworks/FrameworkDetectorClass.h>
#include <MvcEditorAssets.h>
#include <windows/StringHelperClass.h>
#include <MvcEditorErrors.h>
#include <wx/stdpaths.h>
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/tokenzr.h>

static const int ID_DETECT_FRAMEWORK = wxNewId();
static const int ID_DETECT_DATABASE = wxNewId();
static const int ID_DETECT_CONFIG = wxNewId();
static const int ID_DETECT_RESOURCES = wxNewId();
static const int ID_DETECT_URL = wxNewId();
static const int ID_DETECT_VIEW_FILES = wxNewId();
static const int ID_DETECT_TEMPLATE_VARIABLES = wxNewId();

mvceditor::ResponseThreadWithHeartbeatClass::ResponseThreadWithHeartbeatClass(mvceditor::DetectorActionClass& action) 
	: ThreadWithHeartbeatClass(action) 
	, Action(action) 
	, OutputFile() {
}

bool mvceditor::ResponseThreadWithHeartbeatClass::Init(wxFileName outputFile) {
	OutputFile = outputFile;
	if (wxTHREAD_NO_ERROR == CreateSingleInstance()) {
		SignalStart();
		return true;
	}
	return false;
}

void mvceditor::ResponseThreadWithHeartbeatClass::Entry() {
	if (OutputFile.FileExists()) {
		Action.Response();
	}
	SignalEnd();
}

mvceditor::DetectorActionClass::DetectorActionClass(wxEvtHandler& handler) 
	: wxEvtHandler()
	, Error(NONE)
	, ErrorMessage()
	, Process(*this)
	, ResponseThread(*this)
	, Handler(handler)
	, OutputFile()
	, CurrentPid(0)
	, CurrentId(0) {
}

mvceditor::DetectorActionClass::~DetectorActionClass() {
	if (CurrentPid > 0) {
		Process.Stop(CurrentPid);
	}
	ResponseThread.KillInstance();
}

bool mvceditor::DetectorActionClass::Init(int id, const EnvironmentClass& environment, const wxString& projectRootPath, const wxString& identifier, 
		std::map<wxString, wxString> moreParams) {
	Error = NONE;
	ErrorMessage = wxT("");
	Clear();
	ResponseThread.KillInstance();

	CurrentId = id;
	wxString action = GetAction();
	wxFileName scriptFileName = mvceditor::PhpDetectorsAsset();
	wxStandardPaths paths;

	// the temporary file where the output will go
	// make it a unique name
	wxString tmpDir = paths.GetTempDir();
	OutputFile.AssignDir(tmpDir);
	wxLongLong time = wxGetLocalTimeMillis();
	wxString tmpName = action + wxString::Format(wxT("_%s.ini"), time.ToString().c_str());
	OutputFile.SetFullName(tmpName);
	OutputFile.Normalize();	
		
	wxString args =
		wxT(" --action=\"") + action  + wxT("\"") +
		wxT(" --dir=\"") + projectRootPath  + wxT("\"") +
		wxT(" --identifier=\"") + identifier + wxT("\"") +
		wxT(" --output=\"") + OutputFile.GetFullPath() + wxT("\"");
	for (std::map<wxString, wxString>::iterator it = moreParams.begin(); it != moreParams.end(); ++it) {
		args += wxT(" --") + it->first + wxT("=\"") + it->second + wxT("\"");
	}
		
	wxString cmd = wxT("\"") + environment.Php.PhpExecutablePath + wxT("\"") + wxT(" \"") + scriptFileName.GetFullPath() + wxT("\"") + args;
	return Process.Init(cmd, id, CurrentPid);
}

void mvceditor::DetectorActionClass::InitFromFile(wxString fileName) {
	OutputFile = wxFileName::wxFileName(fileName);
	Response();
}

void mvceditor::DetectorActionClass::OnProcessComplete(wxCommandEvent& event) {
	CurrentPid = 0;

	// kick off response parsing in a background thread.
	// any running thread was stopped in Init()
	ResponseThread.Init(OutputFile);
}

void mvceditor::DetectorActionClass::OnProcessFailed(wxCommandEvent& event) {
	Error = PROCESS_FAILED;
	ErrorMessage = event.GetString();
	CurrentPid = 0;
	wxPostEvent(&Handler, event);
}

void mvceditor::DetectorActionClass::OnWorkInProgress(wxCommandEvent& event) {
	
	// users expect an EVENT_PROCESS_IN_PROGRESS event but this handler
	// handles both EVENT_PROCESS_IN_PROGRESS AND EVENT_WORK_IN_PROGRESS
	wxCommandEvent inProgressEvent(mvceditor::EVENT_PROCESS_IN_PROGRESS);
	wxPostEvent(&Handler, inProgressEvent);
}

void mvceditor::DetectorActionClass::OnWorkComplete(wxCommandEvent& event) {

	// users expect an EVENT_PROCESS_COMPLETE event
	wxCommandEvent completeEvent(mvceditor::EVENT_PROCESS_COMPLETE);
	completeEvent.SetId(CurrentId);
	wxPostEvent(&Handler, completeEvent);
	wxRemoveFile(OutputFile.GetFullPath());
}

mvceditor::FrameworkDetectorActionClass::FrameworkDetectorActionClass(wxEvtHandler& handler)
	: DetectorActionClass(handler)
	, Frameworks() {

}

void mvceditor::FrameworkDetectorActionClass::Clear() {
	Frameworks.clear();
}

wxString mvceditor::FrameworkDetectorActionClass::GetAction() {
	return wxT("isUsedBy");
}

bool mvceditor::FrameworkDetectorActionClass::Response() {
	bool ret = true;
	wxFileInputStream stream(OutputFile.GetFullPath());
	wxFileConfig result(stream);
	size_t count = result.GetNumberOfEntries();
	for (size_t i = 0; i < count; i++) {
		wxString key = wxString::Format(wxT("/framework_%d"), i);
		wxString val;
		ret = result.Read(key, &val);
		if (!ret) {
			Error = BAD_CONTENT;
			break;
		}
		Frameworks.push_back(val);	
	}
	return ret;
}

mvceditor::DatabaseDetectorActionClass::DatabaseDetectorActionClass(wxEvtHandler& handler)
	: DetectorActionClass(handler)
	, Databases() {

}

void mvceditor::DatabaseDetectorActionClass::Clear() {
	Databases.clear();
}

wxString mvceditor::DatabaseDetectorActionClass::GetAction() {
	return wxT("databaseInfo");
}

bool mvceditor::DatabaseDetectorActionClass::Response() {
	bool ret = true;
	wxFileInputStream stream(OutputFile.GetFullPath());
	wxFileConfig result(stream);
	wxString groupName = wxT("");
	long index = 0;		
	bool next = result.GetFirstGroup(groupName, index);
	while (next) {
		mvceditor::DatabaseInfoClass info;
		groupName = wxT("/") + groupName + wxT("/");
		
		// don't use wxFileConfig.SetPath method. it seems to mess with the group iteration
		wxString s = result.Read(groupName + wxT("Host"));
		info.Host = mvceditor::StringHelperClass::wxToIcu(s);
		s = result.Read(groupName + wxT("/User"));
		info.User = mvceditor::StringHelperClass::wxToIcu(s);
		s = result.Read(groupName + wxT("Password"));
		info.Password = mvceditor::StringHelperClass::wxToIcu(s);
		s = result.Read(groupName + wxT("DatabaseName"));
		info.DatabaseName = mvceditor::StringHelperClass::wxToIcu(s);
		s = result.Read(groupName + wxT("FileName"));
		info.FileName = mvceditor::StringHelperClass::wxToIcu(s);
		s = result.Read(groupName + wxT("Name"));
		info.Label = mvceditor::StringHelperClass::wxToIcu(s);
		wxString driverString = result.Read(groupName + wxT("Driver"));
		if (driverString.CmpNoCase(wxT("MYSQL")) == 0) {
			info.Driver = mvceditor::DatabaseInfoClass::MYSQL;
		}
		else {
			Error = UNIMPLEMENTED;
			ret = false;
			break;
		}
		result.Read(groupName + wxT("Port"), &info.Port);
		info.IsDetected = true;
		Databases.push_back(info);
		next = result.GetNextGroup(groupName, index);
	}
	return ret;
}

mvceditor::ConfigFilesDetectorActionClass::ConfigFilesDetectorActionClass(wxEvtHandler& handler) 
	: DetectorActionClass(handler)
	, ConfigFiles() {

}

void mvceditor::ConfigFilesDetectorActionClass::Clear() {
	ConfigFiles.clear();
}

wxString mvceditor::ConfigFilesDetectorActionClass::GetAction() {
	return wxT("configFiles");
}

bool mvceditor::ConfigFilesDetectorActionClass::Response() {
	bool ret = true;
	wxFileInputStream stream(OutputFile.GetFullPath());
	wxFileConfig result(stream);
	
	long index = 0;
	wxString entryName;
	bool hasNext = result.GetFirstEntry(entryName, index);
	while (hasNext) {
		wxString configFilePath;
		ret = result.Read(entryName, &configFilePath);
		if (!ret) {
			Error = BAD_CONTENT;
			break;
		}
		ConfigFiles[entryName] = configFilePath;
		hasNext = result.GetNextEntry(entryName, index);
	}
	return ret;
}

mvceditor::ResourcesDetectorActionClass::ResourcesDetectorActionClass(wxEvtHandler& handler)
	: DetectorActionClass(handler)
	, Resources() {

}

void mvceditor::ResourcesDetectorActionClass::Clear() {
	Resources.clear();
}

wxString mvceditor::ResourcesDetectorActionClass::GetAction() {
	return wxT("resources");
}

bool mvceditor::ResourcesDetectorActionClass::Response() {
	bool ret = true;
	wxFileInputStream stream(OutputFile.GetFullPath());
	wxFileConfig result(stream);
	
	long index = 0;
	wxString groupName;
	bool hasNext = result.GetFirstGroup(groupName, index);
	while (hasNext) {
		wxString configFilePath;
		wxString resResource = result.Read(groupName + wxT("/Resource"));
		wxString resIdentifier = result.Read(groupName + wxT("/Identifier"));
		wxString resReturnType = result.Read(groupName + wxT("/ReturnType"));
		wxString resSignature = result.Read(groupName + wxT("/Signature"));
		wxString resType = result.Read(groupName + wxT("/Type"));
		wxString resComment = result.Read(groupName + wxT("/Comment"));
		
		mvceditor::ResourceClass resource;
		UnicodeString res = mvceditor::StringHelperClass::wxToIcu(resResource);
		int32_t scopePos = res.indexOf(UNICODE_STRING_SIMPLE("::"));
		UnicodeString className;
		if (scopePos > 0) {
			className.setTo(res, 0, scopePos);			
			resource.ClassName = className;
		}
		resource.Identifier = mvceditor::StringHelperClass::wxToIcu(resIdentifier);	
		resource.ReturnType = mvceditor::StringHelperClass::wxToIcu(resReturnType);
		resource.Signature = mvceditor::StringHelperClass::wxToIcu(resSignature);
		resource.Comment = mvceditor::StringHelperClass::wxToIcu(resComment);
		bool found = false;
		if (resType.CmpNoCase(wxT("CLASS")) == 0) {
			resource.Type = mvceditor::ResourceClass::CLASS;
			found = true;
		}
		else if (resType.CmpNoCase(wxT("CLASS_CONSTANT")) == 0) {
			resource.Type = mvceditor::ResourceClass::CLASS_CONSTANT;
			found = true;
		}
		else if (resType.CmpNoCase(wxT("DEFINE")) == 0) {
			resource.Type = mvceditor::ResourceClass::DEFINE;
			found = true;
		}
		else if (resType.CmpNoCase(wxT("FUNCTION")) == 0) {
			resource.Type = mvceditor::ResourceClass::FUNCTION;
			found = true;
		}
		else if (resType.CmpNoCase(wxT("MEMBER")) == 0) {
			resource.Type = mvceditor::ResourceClass::MEMBER;
			found = true;
		}
		else if (resType.CmpNoCase(wxT("METHOD")) == 0) {
			resource.Type = mvceditor::ResourceClass::METHOD;
			found = true;
		}
		if (!found || resResource.IsEmpty() || resIdentifier.IsEmpty()) {
			Error = BAD_CONTENT;
			ret = false;
			break;
		}
		Resources.push_back(resource);
		hasNext = result.GetNextGroup(groupName, index);
	}
	return ret;
}

mvceditor::UrlDetectorActionClass::UrlDetectorActionClass(wxEvtHandler& handler) 
	: DetectorActionClass(handler)
	, Urls() {
		
}

void mvceditor::UrlDetectorActionClass::Clear() {
	Urls.clear();
}

wxString mvceditor::UrlDetectorActionClass::GetAction() {
	return wxT("makeUrls");
}

bool mvceditor::UrlDetectorActionClass::Response() {
	bool ret = true;

	// decided against using INI for this particular action; since there could be many URLS
	// the MvcEditor INI parsing is not so great; a measly 2000+ entries takes 5 seconds to parse
	// in INI form.
	wxFileInputStream stream(OutputFile.GetFullPath());
	wxTextInputStream textStream(stream);
	wxStringTokenizer tok;
	while (!stream.Eof()) {
		tok.SetString(textStream.ReadLine(), wxT(",\n"));
		if (tok.HasMoreTokens()) {
			mvceditor::UrlResourceClass newUrl(tok.NextToken());
			if (tok.HasMoreTokens()) {
				newUrl.FileName.Assign(tok.NextToken());
				if (tok.HasMoreTokens()) {
					newUrl.ClassName = tok.NextToken();
					if (tok.HasMoreTokens()) {
						newUrl.MethodName = tok.NextToken();
						if (newUrl.Url.GetServer().IsEmpty() || !newUrl.FileName.IsOk()) {
							Error = BAD_CONTENT;
							ret = false;
							break;
						}
						else {
							Urls.push_back(newUrl);
						}
					}
				}
			}
		}
	}
	return ret;
}

mvceditor::ViewFilesDetectorActionClass::ViewFilesDetectorActionClass(wxEvtHandler& handler) 
	: DetectorActionClass(handler) {
		
}

void mvceditor::ViewFilesDetectorActionClass::Clear() {
	ViewFiles.clear();
}

wxString mvceditor::ViewFilesDetectorActionClass::GetAction() {
	return wxT("viewFiles");
}

bool mvceditor::ViewFilesDetectorActionClass::Response() {
	bool ret = true;
	wxFileInputStream stream(OutputFile.GetFullPath());
	wxFileConfig result(stream);
	
	long index = 0;
	wxString entryName;
	bool hasNext = result.GetFirstEntry(entryName, index);
	while (ret && hasNext) {
		wxString fullPath;
		ret = result.Read(entryName, &fullPath);
		if (ret) {

			// let invalid files through. we will alert the user later.
			ViewFiles.push_back(fullPath);
		}
		else {
			Error = BAD_CONTENT;
			ret = false;
			break;
		}
		hasNext = result.GetNextEntry(entryName, index);
	}
	return ret;
}

mvceditor::TemplateVariablesDetectorActionClass::TemplateVariablesDetectorActionClass(wxEvtHandler& handler) 
	: DetectorActionClass(handler) {
		
}

void mvceditor::TemplateVariablesDetectorActionClass::Clear() {
	TemplateVariables.clear();
}

wxString mvceditor::TemplateVariablesDetectorActionClass::GetAction() {
	return wxT("templateVariables");
}

bool mvceditor::TemplateVariablesDetectorActionClass::Response() {
	bool ret = true;
	wxFileInputStream stream(OutputFile.GetFullPath());
	wxFileConfig result(stream);
	
	long index = 0;
	wxString entryName;
	bool hasNext = result.GetFirstEntry(entryName, index);
	while (ret && hasNext) {
		wxString varName;
		ret = result.Read(entryName, &varName);
		if (ret) {
			TemplateVariables.push_back(varName);
		}
		else {
			Error = BAD_CONTENT;
			ret = false;
			break;
		}
		hasNext = result.GetNextEntry(entryName, index);
	}
	return ret;
}

mvceditor::PhpFrameworkDetectorClass::PhpFrameworkDetectorClass(wxEvtHandler& handler, const mvceditor::EnvironmentClass& environment)
	: wxEvtHandler()
	, Identifiers()
	, ConfigFiles()
	, Databases()
	, Resources()
	, FrameworkDetector(*this)
	, ConfigFilesDetector(*this)
	, DatabaseDetector(*this)
	, ResourcesDetector(*this)
	, UrlDetector(*this)
	, ViewFilesDetector(*this)
	, TemplateVariablesDetector(*this)
	, FrameworkIdentifiersLeftToDetect()
	, UrlsDetected()
	, ViewFilesDetected()
	, TemplateVariablesDetected()
	, Handler(handler)
	, Environment(environment)
	, ProjectRootPath() {
		
}

void mvceditor::PhpFrameworkDetectorClass::Clear() {
	Identifiers.clear();
	ConfigFiles.clear();
	Databases.clear();
	Resources.clear();
	FrameworkIdentifiersLeftToDetect.clear();
	UrlsDetected.clear();
	ViewFilesDetected.clear();
	TemplateVariablesDetected.clear();
}

bool mvceditor::PhpFrameworkDetectorClass::Init(const wxString& dir) {
	ProjectRootPath = dir;
	std::map<wxString, wxString> moreParams;
	return FrameworkDetector.Init(ID_DETECT_FRAMEWORK, Environment, dir, wxT(""), moreParams);
}

bool mvceditor::PhpFrameworkDetectorClass::InitUrlDetector(const wxString& dir, const wxString& resourceCacheFileName, const wxString& baseUrl) {
	UrlsDetected.clear();
	FrameworkIdentifiersLeftToDetect.clear();
	ProjectRootPath = dir;
	for (size_t i = 0; i < Identifiers.size(); ++i) {
		std::vector<wxString> next;
		next.push_back(Identifiers[i]);
		next.push_back(resourceCacheFileName);
		next.push_back(baseUrl);
		FrameworkIdentifiersLeftToDetect.push_back(next);
	}
	if (!FrameworkIdentifiersLeftToDetect.empty()) {
		NextUrlDetection();
	}
	return !Identifiers.empty();
}

bool mvceditor::PhpFrameworkDetectorClass::InitViewFilesDetector(const wxString& dir, const wxString& url, const wxFileName& callStackFile) {
	ViewFilesDetected.clear();
	FrameworkIdentifiersLeftToDetect.clear();
	ProjectRootPath = dir;
	for (size_t i = 0; i < Identifiers.size(); ++i) {
		std::vector<wxString> next;
		next.push_back(Identifiers[i]);
		next.push_back(url);
		next.push_back(callStackFile.GetFullPath());
		FrameworkIdentifiersLeftToDetect.push_back(next);
	}
	if (!FrameworkIdentifiersLeftToDetect.empty()) {
		NextViewFileDetection();
	}
	return !Identifiers.empty();
}

bool mvceditor::PhpFrameworkDetectorClass::InitTemplateVariablesDetector(const wxString& dir, const wxFileName& callStackFile) {
	TemplateVariablesDetected.clear();
	FrameworkIdentifiersLeftToDetect.clear();
	ProjectRootPath = dir;
	for (size_t i = 0; i < Identifiers.size(); ++i) {
		std::vector<wxString> next;
		next.push_back(Identifiers[i]);
		next.push_back(callStackFile.GetFullPath());
		FrameworkIdentifiersLeftToDetect.push_back(next);
	}
	if (!FrameworkIdentifiersLeftToDetect.empty()) {
		NextTemplateVariableDetection();
	}
	return !Identifiers.empty();
}


void mvceditor::PhpFrameworkDetectorClass::NextDetection() {
	
	// check the queue, if no more detections need to be done then
	// we are done
	if (!FrameworkIdentifiersLeftToDetect.empty()) {
		std::vector<wxString> next = FrameworkIdentifiersLeftToDetect.back();
		FrameworkIdentifiersLeftToDetect.pop_back();
		
		wxString framework = next[0];
		wxString action = next[1];
		std::map<wxString, wxString> moreParams;
		if (action == wxT("databaseInfo")) {
			DatabaseDetector.Init(ID_DETECT_DATABASE, Environment, ProjectRootPath, framework, moreParams);
		}
		else if (action == wxT("configFiles")) {
			ConfigFilesDetector.Init(ID_DETECT_CONFIG, Environment, ProjectRootPath, framework, moreParams);
		}
		else if (action == wxT("resources")) {
			ResourcesDetector.Init(ID_DETECT_RESOURCES, Environment, ProjectRootPath, framework, moreParams);
		}
	}
	else {	
		wxCommandEvent completeEvent(mvceditor::EVENT_FRAMEWORK_DETECTION_COMPLETE);
		wxPostEvent(&Handler, completeEvent);
	}
}

void mvceditor::PhpFrameworkDetectorClass::NextUrlDetection() {
	if (!FrameworkIdentifiersLeftToDetect.empty()) {
		std::vector<wxString> next = FrameworkIdentifiersLeftToDetect.back();
		FrameworkIdentifiersLeftToDetect.pop_back();
		
		wxString framework = next[0];
		wxString fileName = next[1];
		wxString baseUrl = next[2];
		std::map<wxString, wxString> moreParams;
		moreParams[wxT("file")] = fileName;
		moreParams[wxT("host")] = baseUrl;
		UrlDetector.Init(ID_DETECT_URL, Environment, ProjectRootPath, framework, moreParams);
	}
	else {
		mvceditor::UrlDetectedEventClass urlEvent(UrlsDetected);
		wxPostEvent(&Handler, urlEvent);
	}
}

void mvceditor::PhpFrameworkDetectorClass::NextViewFileDetection() {
	if (!FrameworkIdentifiersLeftToDetect.empty()) {
		std::vector<wxString> next = FrameworkIdentifiersLeftToDetect.back();
		FrameworkIdentifiersLeftToDetect.pop_back();
		
		wxString framework = next[0];
		wxString url = next[1];
		wxString callStackFile = next[2];
		std::map<wxString, wxString> moreParams;
		moreParams[wxT("url")] = url;
		moreParams[wxT("file")] = callStackFile;
		ViewFilesDetector.Init(ID_DETECT_VIEW_FILES, Environment, ProjectRootPath, framework, moreParams);
	}
	else {
		mvceditor::ViewFilesDetectedEventClass viewFilesEvent(ViewFilesDetected);
		wxPostEvent(&Handler, viewFilesEvent);
	}
}

void mvceditor::PhpFrameworkDetectorClass::NextTemplateVariableDetection() {
	if (!FrameworkIdentifiersLeftToDetect.empty()) {
		std::vector<wxString> next = FrameworkIdentifiersLeftToDetect.back();
		FrameworkIdentifiersLeftToDetect.pop_back();
		
		wxString framework = next[0];
		wxString callStackFile = next[1];
		std::map<wxString, wxString> moreParams;
		moreParams[wxT("file")] = callStackFile;
		TemplateVariablesDetector.Init(ID_DETECT_TEMPLATE_VARIABLES, Environment, ProjectRootPath, framework, moreParams);
	}
	else {
		mvceditor::TemplateVariablesDetectedEventClass templateVariablesEvent(TemplateVariablesDetected);
		wxPostEvent(&Handler, templateVariablesEvent);
	}
}

void mvceditor::PhpFrameworkDetectorClass::OnFrameworkDetectionComplete(wxCommandEvent& event) {

	// put the identifiers in the queue. For each identifier we want to detect its
	// db connections, config files, and resources
	Identifiers = FrameworkDetector.Frameworks;
	if (FrameworkDetector.Error != mvceditor::DetectorActionClass::NONE) {
		wxString response = event.GetString();
		mvceditor::EditorLogError(mvceditor::PROJECT_DETECTION, response);
	}
		
	// fill the detection queue
	for (size_t i = 0; i < Identifiers.size(); i++) {
		std::vector<wxString> next;
		next.push_back(Identifiers[i]);
		next.push_back(wxT("databaseInfo"));
		FrameworkIdentifiersLeftToDetect.push_back(next);
		
		next.clear();
		next.push_back(Identifiers[i]);
		next.push_back(wxT("configFiles"));
		FrameworkIdentifiersLeftToDetect.push_back(next);
		
		next.clear();
		next.push_back(Identifiers[i]);
		next.push_back(wxT("resources"));
		FrameworkIdentifiersLeftToDetect.push_back(next);
	}
	
	// the NextDetection will properly notify any of our handlers (for DETECTION_* events) if the identifier queue
	// is empty
	NextDetection();
}

void mvceditor::PhpFrameworkDetectorClass::OnDatabaseDetectionComplete(wxCommandEvent& event) {
	Databases.insert(Databases.end(), DatabaseDetector.Databases.begin(), DatabaseDetector.Databases.end());
	if (mvceditor::DetectorActionClass::NONE != DatabaseDetector.Error) {
		wxString response = event.GetString();
		mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, response);
	}
	NextDetection();
}

void mvceditor::PhpFrameworkDetectorClass::OnConfigFilesDetectionComplete(wxCommandEvent& event) {
	std::map<wxString, wxString>::iterator it = ConfigFilesDetector.ConfigFiles.begin();
	for (; it != ConfigFilesDetector.ConfigFiles.end(); ++it) {
		ConfigFiles[it->first] = it->second;
	}
	if (mvceditor::DetectorActionClass::NONE != ConfigFilesDetector.Error) {
		wxString response = event.GetString();
		mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, response);
	}
	NextDetection();
}

void mvceditor::PhpFrameworkDetectorClass::OnResourcesDetectionComplete(wxCommandEvent& event) {
	Resources.insert(Resources.end(), ResourcesDetector.Resources.begin(), ResourcesDetector.Resources.end());
	if (mvceditor::DetectorActionClass::NONE != ResourcesDetector.Error) {
		wxString response = event.GetString();
		mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, response);
	}
	NextDetection();
}

void mvceditor::PhpFrameworkDetectorClass::OnDetectionFailed(wxCommandEvent& event) {
	wxCommandEvent failedEvent(mvceditor::EVENT_FRAMEWORK_DETECTION_FAILED);
	failedEvent.SetString(event.GetString());
	wxPostEvent(&Handler, failedEvent);
}

void mvceditor::PhpFrameworkDetectorClass::OnUrlDetectionComplete(wxCommandEvent& event) {
	UrlsDetected.insert(UrlsDetected.end(), UrlDetector.Urls.begin(),UrlDetector.Urls.end());
	NextUrlDetection();
}

void mvceditor::PhpFrameworkDetectorClass::OnUrlDetectionFailed(wxCommandEvent& event) {
	wxCommandEvent failedEvent(mvceditor::EVENT_FRAMEWORK_URL_FAILED);
	failedEvent.SetString(event.GetString());
	wxPostEvent(&Handler, failedEvent);
}

void mvceditor::PhpFrameworkDetectorClass::OnWorkInProgress(wxCommandEvent& event) {
	wxPostEvent(&Handler, event);
}

void mvceditor::PhpFrameworkDetectorClass::OnViewFileDetectionComplete(wxCommandEvent& event) {
	ViewFilesDetected.insert(ViewFilesDetected.begin(), ViewFilesDetector.ViewFiles.begin(), ViewFilesDetector.ViewFiles.end());
	NextViewFileDetection();
}

void mvceditor::PhpFrameworkDetectorClass::OnViewFileDetectionFailed(wxCommandEvent& event) {
	wxCommandEvent failedEvent(mvceditor::EVENT_FRAMEWORK_VIEW_FILES_FAILED);
	failedEvent.SetString(event.GetString());
	wxPostEvent(&Handler, failedEvent);
}

void mvceditor::PhpFrameworkDetectorClass::OnTemplateVariablesDetectionComplete(wxCommandEvent& event) {
	TemplateVariablesDetected.insert(TemplateVariablesDetected.begin(), TemplateVariablesDetector.TemplateVariables.begin(), 
		TemplateVariablesDetector.TemplateVariables.end());
	NextTemplateVariableDetection();
}

void mvceditor::PhpFrameworkDetectorClass::OnTemplateVariablesDetectionFailed(wxCommandEvent& event) {
	wxCommandEvent failedEvent(mvceditor::EVENT_FRAMEWORK_VIEW_FILES_FAILED);
	failedEvent.SetString(event.GetString());
	wxPostEvent(&Handler, failedEvent);
}

mvceditor::UrlDetectedEventClass::UrlDetectedEventClass(std::vector<mvceditor::UrlResourceClass> urls) 
	: wxEvent(wxID_ANY, mvceditor::EVENT_FRAMEWORK_URL_COMPLETE) 
	, Urls(urls) {
}

wxEvent* mvceditor::UrlDetectedEventClass::Clone() const {
	wxEvent* cloned = new mvceditor::UrlDetectedEventClass(Urls);
	return cloned;
}

mvceditor::ViewFilesDetectedEventClass::ViewFilesDetectedEventClass(std::vector<wxString> viewFiles)
	: wxEvent(wxID_ANY, mvceditor::EVENT_FRAMEWORK_VIEW_FILES_COMPLETE)
	, ViewFiles(viewFiles) {
		
}

wxEvent* mvceditor::ViewFilesDetectedEventClass::Clone() const {
	wxEvent* cloned = new mvceditor::ViewFilesDetectedEventClass(ViewFiles);
	return cloned;
}

mvceditor::TemplateVariablesDetectedEventClass::TemplateVariablesDetectedEventClass(std::vector<wxString> templateVariables)
	: wxEvent(wxID_ANY, mvceditor::EVENT_FRAMEWORK_TEMPLATE_VARIABLES_COMPLETE)
	, TemplateVariables(templateVariables) {
		
}

wxEvent* mvceditor::TemplateVariablesDetectedEventClass::Clone() const {
	wxEvent* cloned = new mvceditor::TemplateVariablesDetectedEventClass(TemplateVariables);
	return cloned;
}

const wxEventType mvceditor::EVENT_FRAMEWORK_DETECTION_COMPLETE = wxNewEventType();
const wxEventType mvceditor::EVENT_FRAMEWORK_DETECTION_FAILED = wxNewEventType();
const wxEventType mvceditor::EVENT_FRAMEWORK_URL_COMPLETE = wxNewEventType();
const wxEventType mvceditor::EVENT_FRAMEWORK_URL_FAILED = wxNewEventType();
const wxEventType mvceditor::EVENT_FRAMEWORK_VIEW_FILES_COMPLETE = wxNewEventType();
const wxEventType mvceditor::EVENT_FRAMEWORK_VIEW_FILES_FAILED = wxNewEventType();
const wxEventType mvceditor::EVENT_FRAMEWORK_TEMPLATE_VARIABLES_COMPLETE = wxNewEventType();
const wxEventType mvceditor::EVENT_FRAMEWORK_TEMPLATE_VARIABLES_FAILED = wxNewEventType();

BEGIN_EVENT_TABLE(mvceditor::DetectorActionClass, wxEvtHandler)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_PROCESS_COMPLETE, mvceditor::DetectorActionClass::OnProcessComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_PROCESS_FAILED, mvceditor::DetectorActionClass::OnProcessFailed)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_PROCESS_IN_PROGRESS, mvceditor::DetectorActionClass::OnWorkInProgress)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_COMPLETE, mvceditor::DetectorActionClass::OnWorkComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::DetectorActionClass::OnWorkInProgress)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::PhpFrameworkDetectorClass, wxEvtHandler)
	EVT_COMMAND(ID_DETECT_FRAMEWORK, mvceditor::EVENT_PROCESS_COMPLETE, mvceditor::PhpFrameworkDetectorClass::OnFrameworkDetectionComplete)
	EVT_COMMAND(ID_DETECT_DATABASE, mvceditor::EVENT_PROCESS_COMPLETE, mvceditor::PhpFrameworkDetectorClass::OnDatabaseDetectionComplete)
	EVT_COMMAND(ID_DETECT_CONFIG, mvceditor::EVENT_PROCESS_COMPLETE, mvceditor::PhpFrameworkDetectorClass::OnConfigFilesDetectionComplete)
	EVT_COMMAND(ID_DETECT_RESOURCES, mvceditor::EVENT_PROCESS_COMPLETE, mvceditor::PhpFrameworkDetectorClass::OnResourcesDetectionComplete)
	EVT_COMMAND(ID_DETECT_URL, mvceditor::EVENT_PROCESS_COMPLETE, mvceditor::PhpFrameworkDetectorClass::OnUrlDetectionComplete)
	EVT_COMMAND(ID_DETECT_VIEW_FILES, mvceditor::EVENT_PROCESS_COMPLETE, mvceditor::PhpFrameworkDetectorClass::OnViewFileDetectionComplete)
	EVT_COMMAND(ID_DETECT_TEMPLATE_VARIABLES, mvceditor::EVENT_PROCESS_COMPLETE, mvceditor::PhpFrameworkDetectorClass::OnTemplateVariablesDetectionComplete)
	
	EVT_COMMAND(ID_DETECT_FRAMEWORK, mvceditor::EVENT_PROCESS_FAILED, mvceditor::PhpFrameworkDetectorClass::OnDetectionFailed)
	EVT_COMMAND(ID_DETECT_DATABASE, mvceditor::EVENT_PROCESS_FAILED, mvceditor::PhpFrameworkDetectorClass::OnDetectionFailed)
	EVT_COMMAND(ID_DETECT_CONFIG, mvceditor::EVENT_PROCESS_FAILED, mvceditor::PhpFrameworkDetectorClass::OnDetectionFailed)
	EVT_COMMAND(ID_DETECT_RESOURCES, mvceditor::EVENT_PROCESS_FAILED, mvceditor::PhpFrameworkDetectorClass::OnDetectionFailed)
	EVT_COMMAND(ID_DETECT_URL, mvceditor::EVENT_PROCESS_FAILED, mvceditor::PhpFrameworkDetectorClass::OnUrlDetectionFailed)
	EVT_COMMAND(ID_DETECT_VIEW_FILES, mvceditor::EVENT_PROCESS_FAILED, mvceditor::PhpFrameworkDetectorClass::OnViewFileDetectionFailed)
	EVT_COMMAND(ID_DETECT_TEMPLATE_VARIABLES, mvceditor::EVENT_PROCESS_FAILED, mvceditor::PhpFrameworkDetectorClass::OnTemplateVariablesDetectionFailed)
	
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_PROCESS_IN_PROGRESS, mvceditor::PhpFrameworkDetectorClass::OnWorkInProgress)
END_EVENT_TABLE()