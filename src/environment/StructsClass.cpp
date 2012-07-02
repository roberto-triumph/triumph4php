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
#include <environment/StructsClass.h>
#include <wx/tokenzr.h>

mvceditor::StructsClass::StructsClass()
	: Environment()
	, ResourceCache()
	, UrlResourceFinder()
	, SqlResourceFinder()
	, Infos()
	, CurrentViewInfos() 
	, Projects()
	, CurrentUrl() 
	, PhpFileFiltersString(wxT("*.php"))	
	, CssFileFiltersString(wxT("*.css"))
	, SqlFileFiltersString(wxT("*.sql"))
	, Frameworks() {
}


std::vector<mvceditor::SourceClass> mvceditor::StructsClass::AllEnabledSources() const {
	std::vector<mvceditor::SourceClass> allSources;
	for (size_t i = 0; i < Projects.size(); ++i) {
		mvceditor::ProjectClass project = Projects[i];
		if (project.IsEnabled) {
			for (size_t j = 0; j < project.Sources.size(); ++j) {
				allSources.push_back(project.Sources[j]);
			}
		}
	}
	return allSources;
}

bool mvceditor::StructsClass::HasSources() const {
	bool hasSources = false;
	for (size_t i = 0; i < Projects.size() && !hasSources; ++i) {
		mvceditor::ProjectClass project = Projects[i];
		hasSources = project.IsEnabled && project.HasSources();
	}
	return hasSources;
}

std::vector<wxString> mvceditor::StructsClass::GetPhpFileExtensions() const {
	std::vector<wxString> wildcards;
	wxStringTokenizer tokenizer(PhpFileFiltersString, wxT(";"));
	while (tokenizer.HasMoreTokens()) {
		wxString wildcard = tokenizer.NextToken();
		wildcards.push_back(wildcard);
	}
	return wildcards;
}


std::vector<wxString> mvceditor::StructsClass::GetCssFileExtensions() const {
	std::vector<wxString> wildcards;
	wxStringTokenizer tokenizer(CssFileFiltersString, wxT(";"));
	while (tokenizer.HasMoreTokens()) {
		wxString wildcard = tokenizer.NextToken();
		wildcards.push_back(wildcard);
	}
	return wildcards;
}

std::vector<wxString> mvceditor::StructsClass::GetSqlFileExtensions() const {
	std::vector<wxString> wildcards;
	wxStringTokenizer tokenizer(SqlFileFiltersString, wxT(";"));
	while (tokenizer.HasMoreTokens()) {
		wxString wildcard = tokenizer.NextToken();
		wildcards.push_back(wildcard);
	}
	return wildcards;
}

bool mvceditor::StructsClass::IsAPhpSourceFile(const wxString& fullPath) const {
	bool isPhp = false;
	for (size_t i = 0; i < Projects.size() && !isPhp; ++i) {
		mvceditor::ProjectClass project = Projects[i];
		isPhp = project.IsAPhpSourceFile(fullPath);
	}
	return isPhp;
}

wxString mvceditor::StructsClass::RelativeFileName(const wxString &fullPath) const {
	wxString relativeName;
	for (size_t i = 0; i < Projects.size() && relativeName.IsEmpty(); ++i) {
		mvceditor::ProjectClass project = Projects[i];
		relativeName = project.RelativeFileName(fullPath);
	}
	return relativeName;
}


wxString mvceditor::StructsClass::FirstDirectory() const {
	wxString fullPath;
	for (size_t i = 0; i < Projects.size(); ++i) {
		mvceditor::ProjectClass project = Projects[i];
		if (project.IsEnabled && project.HasSources()) {
			fullPath = project.Sources[0].RootDirectory.GetPath();
			break;
		}
	}
	return fullPath;
}