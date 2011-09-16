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
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/utils.h>
#include <wx/sstream.h>
#include <wx/fileconf.h>
#include <vector>

mvceditor::ProjectOptionsClass::ProjectOptionsClass()  
	: RootPath() { 
}
	
mvceditor::ProjectOptionsClass::ProjectOptionsClass(const ProjectOptionsClass& other) { 
	RootPath = other.RootPath; 
}

mvceditor::DatabaseInfoClass::DatabaseInfoClass()
	: Host()
	, User()
	, Password()
	, DatabaseName()
	, FileName()
	, Name()
	, Driver(MYSQL)
	, Port() {
		
}

mvceditor::DatabaseInfoClass::DatabaseInfoClass(const mvceditor::DatabaseInfoClass& other) 
	: Host()
	, User()
	, Password()
	, DatabaseName()
	, FileName()
	, Name()
	, Driver(MYSQL)
	, Port() {
	Copy(other);
}

void mvceditor::DatabaseInfoClass::Copy(const mvceditor::DatabaseInfoClass& src) {
	Host = src.Host;
	User = src.User;
	Password = src.Password;
	DatabaseName = src.DatabaseName;
	FileName = src.FileName;
	Name = src.Name;
	Driver = src.Driver;
	Port = src.Port;
}

mvceditor::ProjectClass::ProjectClass(const mvceditor::ProjectOptionsClass& options)
	: Options(options)
	, ResourceFinder()
	, FrameworkIdentifiers()
	, Databases() {
	ResourceFinder.FilesFilter = GetPhpFileExtensions();
}

wxString mvceditor::ProjectClass::GetPhpKeywords() const {
	return wxT("php if else elseif do while for foreach switch ")
	  wxT("case break continue default function return public private protected ")
	  wxT("class interface extends implements static final const true false ")
	  wxT("NULL global array echo empty eval exit isset list print unset __LINE__ ")
	  wxT("__FILE__ __DIR__ __FUNCTION__ __CLASS__ __METHOD__ __NAMESPACE__ ")
	  wxT("require require_once include include_once stdClass parent self abstract ")
	  wxT("clone namespace use as new bool boolean float double real string int ")
	  wxT("integer");
}

wxString  mvceditor::ProjectClass::GetRootPath() const { 
	return Options.RootPath; 
}

void mvceditor::ProjectClass::Detect(bool useTest) {
	FrameworkIdentifiers.clear();
	
	wxString action = wxT("isUsedBy");
	
	// no identifier because we are detecting them here we don't know them
	wxString resultString = Ask(action, wxT(""));
	wxStringInputStream stream(resultString);
	wxFileConfig result(stream);
	size_t count = result.GetNumberOfEntries();
	for (size_t i = 0; i < count; i++) {
		wxString key = wxString::Format(wxT("/framework_%d"), i);
		wxString val;
		val = result.Read(key);
		
		// omit the test databases; only used for unit testing the ProjectClass
		if (!useTest  && val.CmpNoCase(wxT("Test")) != 0) {
			FrameworkIdentifiers.push_back(val);
		}
		else if (useTest) {
			FrameworkIdentifiers.push_back(val);
		}
	}
}

std::vector<mvceditor::DatabaseInfoClass> mvceditor::ProjectClass::DatabaseInfo() {
	if (Databases.size()) {
		return Databases;
	}
	for (size_t i = 0; i < FrameworkIdentifiers.size(); i++) {
		wxString identifier = FrameworkIdentifiers[i];
		wxString resultString = AskDatabaseInfo(identifier);
		wxStringInputStream stream(resultString);
		wxFileConfig result(stream);
		wxString groupName = wxT("");
		long index = 0;		
		bool next = result.GetFirstGroup(groupName, index);
		while (next) {
			mvceditor::DatabaseInfoClass info;
			groupName = wxT("/") + groupName + wxT("/");
			
			// dont use wxFileConfig.SetPath method. it seems to mess with the group iteration
			info.Host = result.Read(groupName + wxT("Host"));
			info.User = result.Read(groupName + wxT("/User"));
			info.Password = result.Read(groupName + wxT("Password"));
			info.DatabaseName = result.Read(groupName + wxT("DatabaseName"));
			info.FileName = result.Read(groupName + wxT("FileName"));
			info.Name = result.Read(groupName + wxT("Name"));
			wxString driverString = result.Read(groupName + wxT("Driver"));
			if (driverString.CmpNoCase(wxT("MYSQL"))) {
				info.Driver = mvceditor::DatabaseInfoClass::MYSQL;
			}
			else if (driverString.CmpNoCase(wxT("POSTGRESQL"))) {
				info.Driver = mvceditor::DatabaseInfoClass::POSTGRESQL;
			}
			else if (driverString.CmpNoCase(wxT("SQLITE"))) {
				info.Driver = mvceditor::DatabaseInfoClass::SQLITE;
			}
			else {
				info.Driver = mvceditor::DatabaseInfoClass::MYSQL;
				// TODO error handling
			}
			result.Read(groupName + wxT("Port"), &info.Port);
			Databases.push_back(info);
			next = result.GetNextGroup(groupName, index);
		}
	}
	return Databases;
}

wxString mvceditor::ProjectClass::Sanitize(const wxString& arg) const {
	wxString ret = arg;
	ret.Replace(wxT("'"), wxT("\\'"), true);
	return wxT("'") + ret + wxT("'");
}

wxString mvceditor::ProjectClass::GetPhpFileExtensions() const {
	return wxT("*.php");
}

wxString mvceditor::ProjectClass::GetPhpExecutable() const {
	return wxT("php");
}

mvceditor::ResourceFinderClass* mvceditor::ProjectClass::GetResourceFinder() {
	return &ResourceFinder;
}

wxString mvceditor::ProjectClass::AskDatabaseInfo(const wxString& identifier) const {
	return Ask(wxT("databaseInfo"), identifier);
}

wxString mvceditor::ProjectClass::Ask(const wxString& action, const wxString& identifier) const {
	wxStandardPaths paths;
	wxFileName pathExecutableFileName(paths.GetExecutablePath());
	wxString scriptFileName = pathExecutableFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) +
		wxT("..") + wxFileName::GetPathSeparator() +
		wxT("src") + wxFileName::GetPathSeparator() +
		wxT("php_frameworks") + wxFileName::GetPathSeparator() +
		wxT("MvcEditorFrameworkApp.php");
	wxString args =
		wxT(" --action=") + Sanitize(action) + 
		wxT(" --dir=") + Sanitize(GetRootPath()) +
		wxT(" --identifier=" + Sanitize(identifier));
	wxString cmd = GetPhpExecutable() + wxT(" ") + Sanitize(scriptFileName) + args;
	wxArrayString output;
	wxExecute(cmd, output, wxEXEC_SYNC);
	wxString cat = wxT("");
	bool doCat = false;
	for (size_t i = 0; i < output.Count(); i++) {
		if (doCat) {
			cat += output[i];
			cat += wxT("\n");
		}
		if (output[i] == wxT("-----START-MVC-EDITOR-----")) {
			doCat = true;
		}
	}
	return cat;
}