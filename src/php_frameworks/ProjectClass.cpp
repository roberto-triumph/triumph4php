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
#include <php_frameworks/ProjectClass.h>
#include <windows/StringHelperClass.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/utils.h>
#include <wx/sstream.h>
#include <wx/tokenzr.h>
#include <wx/fileconf.h>
#include <vector>

mvceditor::DetectorClass::DetectorClass(wxEvtHandler& handler) 
	: wxEvtHandler()
	, Handler(handler)
	, Error(NONE)
	, ErrorMessage()
	, Process(*this)
	, OutputFile()
	, CurrentPid(0) {
}

mvceditor::DetectorClass::~DetectorClass() {
	if (CurrentPid > 0) {
		Process.Stop(CurrentPid);
	}
}

bool mvceditor::DetectorClass::Init(int id, const EnvironmentClass& environment, const wxString& projectRootPath, const wxString& identifier) {
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

void mvceditor::DetectorClass::InitFromFile(wxString fileName) {
	OutputFile = wxFileName::wxFileName(fileName);
	Response();
}

void mvceditor::DetectorClass::OnProcessComplete(wxCommandEvent& event) {
	if (OutputFile.FileExists()) {
		Response();
	}
	CurrentPid = 0;
	wxPostEvent(&Handler, event);
}

void mvceditor::DetectorClass::OnProcessFailed(wxCommandEvent& event) {
	Error = PROCESS_FAILED;
	ErrorMessage = event.GetString();
	CurrentPid = 0;
	wxPostEvent(&Handler, event);
}

mvceditor::FrameworkDetectorClass::FrameworkDetectorClass(wxEvtHandler& handler)
	: DetectorClass(handler)
	, Frameworks() {

}

void mvceditor::FrameworkDetectorClass::Clear() {
	Frameworks.clear();
}

wxString mvceditor::FrameworkDetectorClass::GetAction() {
	return wxT("isUsedBy");
}

bool mvceditor::FrameworkDetectorClass::Response() {
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

mvceditor::DatabaseDetectorClass::DatabaseDetectorClass(wxEvtHandler& handler)
	: DetectorClass(handler)
	, Databases() {

}

void mvceditor::DatabaseDetectorClass::Clear() {
	Databases.clear();
}

wxString mvceditor::DatabaseDetectorClass::GetAction() {
	return wxT("databaseInfo");
}

bool mvceditor::DatabaseDetectorClass::Response() {
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

mvceditor::ConfigFilesDetectorClass::ConfigFilesDetectorClass(wxEvtHandler& handler) 
	: DetectorClass(handler)
	, ConfigFiles() {

}

void mvceditor::ConfigFilesDetectorClass::Clear() {
	ConfigFiles.clear();
}

wxString mvceditor::ConfigFilesDetectorClass::GetAction() {
	return wxT("configFiles");
}

bool mvceditor::ConfigFilesDetectorClass::Response() {
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

mvceditor::ResourcesDetectorClass::ResourcesDetectorClass(wxEvtHandler& handler)
	: DetectorClass(handler)
	, Resources() {

}

void mvceditor::ResourcesDetectorClass::Clear() {
	Resources.clear();
}

wxString mvceditor::ResourcesDetectorClass::GetAction() {
	return wxT("resources");
}

bool mvceditor::ResourcesDetectorClass::Response() {
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

mvceditor::ProjectOptionsClass::ProjectOptionsClass()  
	: RootPath() {
}
	
mvceditor::ProjectOptionsClass::ProjectOptionsClass(const ProjectOptionsClass& other) { 
	RootPath = other.RootPath; 
}

mvceditor::ProjectClass::ProjectClass(const mvceditor::ProjectOptionsClass& options)
	: Options(options)
	, ResourceFinder()
	, Databases()
	, PhpFileFilters()
	, CssFileFilters()
	, SqlFileFilters() {
}

wxString  mvceditor::ProjectClass::GetRootPath() const { 
	return Options.RootPath; 
}

mvceditor::SqlResourceFinderClass* mvceditor::ProjectClass::GetSqlResourceFinder() {
	return &SqlResourceFinder;
}

std::vector<mvceditor::DatabaseInfoClass> mvceditor::ProjectClass::DatabaseInfo() const {
	return Databases;
}

void mvceditor::ProjectClass::PushDatabaseInfo(const mvceditor::DatabaseInfoClass& info) {
	Databases.push_back(info);
}

wxString mvceditor::ProjectClass::GetPhpFileExtensionsString() const {
	wxString all;
	for (size_t i = 0; i < PhpFileFilters.size(); ++i) {
		all.Append(PhpFileFilters[i]);
		if (i < (PhpFileFilters.size() - 1)) {
			all.Append(wxT(';'));
		}
	}
	return all;
}

std::vector<wxString> mvceditor::ProjectClass::GetPhpFileExtensions() const {
	std::vector<wxString> cpy;
	for (size_t i = 0; i < PhpFileFilters.size(); ++i) {
		wxString s;
		s.Append(PhpFileFilters[i]);
		cpy.push_back(s);
	}
	return cpy;
}

void mvceditor::ProjectClass::SetPhpFileExtensionsString(wxString wildcardString) {
	wxStringTokenizer tokenizer(wildcardString, wxT(";"));
	while (tokenizer.HasMoreTokens()) {
		wxString wildcard = tokenizer.NextToken();
		PhpFileFilters.push_back(wildcard);
	}
	ResourceFinder.FileFilters = PhpFileFilters;
}

wxString mvceditor::ProjectClass::GetCssFileExtensionsString() const {
	wxString all;
	for (size_t i = 0; i < CssFileFilters.size(); ++i) {
		all.Append(CssFileFilters[i]);
		if (i < (CssFileFilters.size() - 1)) {
			all.Append(wxT(';'));
		}
	}
	return all;
}

std::vector<wxString> mvceditor::ProjectClass::GetCssFileExtensions() const {
	std::vector<wxString> cpy;
	for (size_t i = 0; i < CssFileFilters.size(); ++i) {
		wxString s;
		s.Append(CssFileFilters[i]);
		cpy.push_back(s);
	}
	return cpy;
}

void mvceditor::ProjectClass::SetCssFileExtensionsString(wxString wildcardString) {
	wxStringTokenizer tokenizer(wildcardString, wxT(";"));
	while (tokenizer.HasMoreTokens()) {
		wxString wildcard = tokenizer.NextToken();
		CssFileFilters.push_back(wildcard);
	}
}

wxString mvceditor::ProjectClass::GetSqlFileExtensionsString() const {
	wxString all;
	for (size_t i = 0; i < SqlFileFilters.size(); ++i) {
		all.Append(SqlFileFilters[i]);
		if (i < (SqlFileFilters.size() - 1)) {
			all.Append(wxT(';'));
		}
	}
	return all;
}
std::vector<wxString> mvceditor::ProjectClass::GetSqlFileExtensions() const {
	std::vector<wxString> cpy;
	for (size_t i = 0; i <SqlFileFilters.size(); ++i) {
		wxString s;
		s.Append(SqlFileFilters[i]);
		cpy.push_back(s);
	}
	return cpy;
}

void mvceditor::ProjectClass::SetSqlFileExtensionsString(wxString wildcardString) {
	wxStringTokenizer tokenizer(wildcardString, wxT(";"));
	while (tokenizer.HasMoreTokens()) {
		wxString wildcard = tokenizer.NextToken();
		SqlFileFilters.push_back(wildcard);
	}
}

BEGIN_EVENT_TABLE(mvceditor::DetectorClass, wxEvtHandler)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_PROCESS_COMPLETE, mvceditor::DetectorClass::OnProcessComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_PROCESS_FAILED, mvceditor::DetectorClass::OnProcessFailed)
END_EVENT_TABLE()