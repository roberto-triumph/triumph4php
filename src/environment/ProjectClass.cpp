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
#include <environment/ProjectClass.h>
#include <wx/tokenzr.h>
#include <MvcEditorAssets.h>
#include <vector>

mvceditor::ProjectClass::ProjectClass()
	: Label()
	, Sources()
	, IsEnabled(true)
	, PhpFileFilters()
	, CssFileFilters()
	, SqlFileFilters()
	, ResourceDbFileName() {
}

mvceditor::ProjectClass::ProjectClass(const mvceditor::ProjectClass& project)
	: Label(project.Label)
	, Sources(project.Sources)
	, IsEnabled(project.IsEnabled)
	, PhpFileFilters(project.PhpFileFilters)
	, CssFileFilters(project.CssFileFilters)
	, SqlFileFilters(project.SqlFileFilters) 
	, ResourceDbFileName(project.ResourceDbFileName) {
}

void mvceditor::ProjectClass::operator=(const mvceditor::ProjectClass& project) {
	Label = project.Label;
	Sources = project.Sources;
	IsEnabled = project.IsEnabled;
	PhpFileFilters = project.PhpFileFilters;
	CssFileFilters = project.CssFileFilters;
	SqlFileFilters = project.SqlFileFilters;
	ResourceDbFileName = project.ResourceDbFileName;
}

void mvceditor::ProjectClass::AddSource(const mvceditor::SourceClass& src) { 
	Sources.push_back(src);
}

void mvceditor::ProjectClass::ClearSources() {
	Sources.clear();
}

std::vector<mvceditor::SourceClass> mvceditor::ProjectClass::AllPhpSources() const {
	std::vector<mvceditor::SourceClass> phpSources;
	for (size_t i = 0; i < Sources.size(); ++i) {
		mvceditor::SourceClass phpSrc;
		phpSrc.RootDirectory = Sources[i].RootDirectory;
		phpSrc.SetIncludeWildcards(GetPhpFileExtensionsString());
		phpSources.push_back(phpSrc);
	}
	return phpSources;
}

bool mvceditor::ProjectClass::IsAPhpSourceFile(const wxString& fullPath) const {
	bool matches = false;
	
	// a file is considered a PHP file if its in a source directory and it matches
	// the PHP file extensions
	std::vector<mvceditor::SourceClass> phpSources = AllPhpSources();
	for (size_t i = 0; i < phpSources.size() && !matches; ++i) {
		matches = phpSources[i].Contains(fullPath);
	}
	return matches;
}

bool mvceditor::ProjectClass::HasSources() const {
	return !Sources.empty();
}

wxString mvceditor::ProjectClass::RelativeFileName(const wxString& fullPath) const {
	wxString relativeName;
	for (size_t i = 0; i < Sources.size(); ++i) {
		wxString root = Sources[i].RootDirectory.GetFullPath();
		if (fullPath.Find(root) == 0) {
			relativeName = fullPath.Mid(root.Length());
			break;
		}
	}
	return relativeName;
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

bool mvceditor::ProjectClass::MakeResourceDbFileName() {
	if (ResourceDbFileName.IsOk()) {
		return true;
	}
	wxString tempDb = wxFileName::CreateTempFileName(mvceditor::ConfigDirAsset().GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
	ResourceDbFileName.Assign(tempDb);
	return ResourceDbFileName.IsOk();
}

void mvceditor::ProjectClass::RemoveResourceDb() {
	if (ResourceDbFileName.FileExists()) {
		wxRemoveFile(ResourceDbFileName.GetFullPath());
	}
}