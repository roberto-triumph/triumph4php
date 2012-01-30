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
#include <windows/StringHelperClass.h>
#include <wx/stdpaths.h>
#include <wx/config.h>
#include <wx/tokenzr.h>

mvceditor::DetectorActionClass::DetectorActionClass(wxEvtHandler& handler) 
	: wxEvtHandler()
	, Error(NONE)
	, ErrorMessage()
	, Process(*this)
	, Handler(handler)
	, OutputFile()
	, CurrentPid(0) {
}

mvceditor::DetectorActionClass::~DetectorActionClass() {
	if (CurrentPid > 0) {
		Process.Stop(CurrentPid);
	}
}

bool mvceditor::DetectorActionClass::Init(int id, const EnvironmentClass& environment, const wxString& projectRootPath, const wxString& identifier, 
		wxString extra) {
	Error = NONE;
	ErrorMessage = wxT("");
	Clear();

	wxString action = GetAction();

	// the detection script
	wxStandardPaths paths;
	wxFileName pathExecutableFileName(paths.GetExecutablePath());
	wxString scriptFullPath = pathExecutableFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) +
		wxT("..") + wxFileName::GetPathSeparator() +
		wxT("src") + wxFileName::GetPathSeparator() +
		wxT("php_frameworks") + wxFileName::GetPathSeparator() +
		wxT("MvcEditorFrameworkApp.php");
	wxFileName scriptFileName(scriptFullPath);
	scriptFileName.Normalize();

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
	wxString cmd = wxT("\"") + environment.Php.PhpExecutablePath + wxT("\"") + wxT(" \"") + scriptFileName.GetFullPath() + wxT("\"") + args;
	return Process.Init(cmd, id, CurrentPid);
}

void mvceditor::DetectorActionClass::InitFromFile(wxString fileName) {
	OutputFile = wxFileName::wxFileName(fileName);
	Response();
}

void mvceditor::DetectorActionClass::OnProcessComplete(wxCommandEvent& event) {
	if (OutputFile.FileExists()) {
		Response();
	}
	CurrentPid = 0;
	wxPostEvent(&Handler, event);
	wxRemoveFile(OutputFile.GetFullPath());
}

void mvceditor::DetectorActionClass::OnProcessFailed(wxCommandEvent& event) {
	Error = PROCESS_FAILED;
	ErrorMessage = event.GetString();
	CurrentPid = 0;
	wxPostEvent(&Handler, event);
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
		info.Name = mvceditor::StringHelperClass::wxToIcu(s);
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
		resource.Resource = mvceditor::StringHelperClass::wxToIcu(resResource);
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
	wxFileInputStream stream(OutputFile.GetFullPath());
	wxFileConfig result(stream);
	
	long index = 0;
	wxString entryName;
	bool hasNext = result.GetFirstEntry(entryName, index);
	while (ret && hasNext) {
		wxString url;
		ret = result.Read(entryName, &url);
		if (ret) {
			Urls.push_back(url);
		}
	}
	return ret;
}

BEGIN_EVENT_TABLE(mvceditor::DetectorActionClass, wxEvtHandler)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_PROCESS_COMPLETE, mvceditor::DetectorActionClass::OnProcessComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_PROCESS_FAILED, mvceditor::DetectorActionClass::OnProcessFailed)
END_EVENT_TABLE()