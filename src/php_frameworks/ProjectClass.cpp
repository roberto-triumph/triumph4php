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

mvceditor::ProjectOptionsClass::ProjectOptionsClass()  
	: RootPath() { 
}
	
mvceditor::ProjectOptionsClass::ProjectOptionsClass(const ProjectOptionsClass& other) { 
	RootPath = other.RootPath; 
}

mvceditor::ProjectClass::ProjectClass(const mvceditor::ProjectOptionsClass& options, 
		const mvceditor::EnvironmentClass& environment)
	: Environment(environment)
	, Options(options)
	, ResourceFinder()
	, Frameworks()
	, Databases() 
	, PhpFileFilters()
	, CssFileFilters()
	, SqlFileFilters() {
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

wxString mvceditor::ProjectClass::DetectFrameworkCommand() {
	Frameworks.clear();

	wxString action = wxT("isUsedBy");
	
	// no identifier because we are detecting them here we don't know them
	return Ask(action, wxT(""));
}

wxString mvceditor::ProjectClass::DetectDatabaseCommand(const wxString& frameworkIdentifier) {
	return Ask(wxT("databaseInfo"), frameworkIdentifier);
}

std::vector<mvceditor::DatabaseInfoClass> mvceditor::ProjectClass::DatabaseInfo() const {
	return Databases;
}

std::vector<wxString> mvceditor::ProjectClass::FrameworkIdentifiers() const {
	return Frameworks;
}

wxString mvceditor::ProjectClass::Sanitize(const wxString& arg) const {
	wxString ret = arg;
	ret.Replace(wxT("'"), wxT("\\'"), true);
	return wxT("'") + ret + wxT("'");
}

wxString mvceditor::ProjectClass::GetPhpExecutable() const {
	return Environment.Php.PhpExecutablePath;
}

mvceditor::ResourceFinderClass* mvceditor::ProjectClass::GetResourceFinder() {
	return &ResourceFinder;
}

mvceditor::SqlResourceFinderClass* mvceditor::ProjectClass::GetSqlResourceFinder() {
	return &SqlResourceFinder;
}

wxString mvceditor::ProjectClass::Ask(const wxString& action, const wxString& identifier) {
	wxStandardPaths paths;
	wxFileName pathExecutableFileName(paths.GetExecutablePath());
	wxString scriptFileName = pathExecutableFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) +
		wxT("..") + wxFileName::GetPathSeparator() +
		wxT("src") + wxFileName::GetPathSeparator() +
		wxT("php_frameworks") + wxFileName::GetPathSeparator() +
		wxT("MvcEditorFrameworkApp.php");
	wxString args =
		wxT(" --action=") + action + 
		wxT(" --dir=") + GetRootPath() +
		wxT(" --identifier=" + identifier);
	wxString cmd = GetPhpExecutable() + wxT(" ") + scriptFileName + args;
	return cmd;
}

bool mvceditor::ProjectClass::DetectFrameworkResponse(const wxString& resultString, mvceditor::ProjectClass::DetectError& error) {
	bool ret = true;
	wxString iniString = GetProcessOutput(resultString);

	// only return false when result is non empty but has invalid content
	if (!iniString.IsEmpty()) {
		wxStringInputStream stream(iniString);
		wxFileConfig result(stream);
		error = mvceditor::ProjectClass::NONE;
		size_t count = result.GetNumberOfEntries();
		ret = false;
		for (size_t i = 0; i < count; i++) {
			wxString key = wxString::Format(wxT("/framework_%d"), i);
			wxString val;
			ret = result.Read(key, &val);
			if (!ret) {
				error = mvceditor::ProjectClass::BAD_CONTENT;
				break;
			}
			Frameworks.push_back(val);	
		}
	}
	return ret;
}

bool mvceditor::ProjectClass::DetectDatabaseResponse(const wxString& resultString, mvceditor::ProjectClass::DetectError& error) {
	bool ret = true;
	wxString iniString = GetProcessOutput(resultString);

	// only return false when result is non empty but has invalid content
	if (!iniString.IsEmpty()) {
		wxStringInputStream stream(iniString);
		wxFileConfig result(stream);
		wxString groupName = wxT("");
		long index = 0;		
		bool next = result.GetFirstGroup(groupName, index);
		error = mvceditor::ProjectClass::NONE;
		if (iniString.IsEmpty() || !next) {
			error = mvceditor::ProjectClass::BAD_CONTENT;
		}
		else {
			ret = true;
		}
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
				error = mvceditor::ProjectClass::UNIMPLEMENTED;
				ret = false;
				break;
			}
			result.Read(groupName + wxT("Port"), &info.Port);
			Databases.push_back(info);
			next = result.GetNextGroup(groupName, index);
		}
	}
	return ret;
}

wxString mvceditor::ProjectClass::GetProcessOutput(const wxString& allOutput) {

	// get only the stuff after the marker; anything before the marker will
	// make the output invalid INI format
	const wxString START_MARKER = wxT("-----START-MVC-EDITOR-----");
	int index = allOutput.Find(START_MARKER);
	wxString resultString;
	if (index >= 0) {
		resultString = allOutput.Mid(index + START_MARKER.Length() + 1);
	}
	return resultString;
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