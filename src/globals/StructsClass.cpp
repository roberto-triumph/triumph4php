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
#include <globals/StructsClass.h>
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
	, PhpFileExtensionsString(wxT("*.php"))	
	, CssFileExtensionsString(wxT("*.css"))
	, SqlFileExtensionsString(wxT("*.sql"))
	, MiscFileExtensionsString(wxT("*.js;*.html;*.yml;*.xml;*.txt"))
	, Frameworks() {
}

std::vector<mvceditor::SourceClass> mvceditor::StructsClass::AllEnabledSources() const {
	std::vector<mvceditor::SourceClass> allSources;
	std::vector<mvceditor::ProjectClass>::const_iterator it;
	for (it = Projects.begin(); it != Projects.end(); ++it) {
		if (it->IsEnabled) {
			for (size_t j = 0; j < it->Sources.size(); ++j) {
				allSources.push_back(it->Sources[j]);
			}
		}
	}
	return allSources;
}

std::vector<mvceditor::SourceClass> mvceditor::StructsClass::AllEnabledPhpSources() const {
	std::vector<mvceditor::SourceClass> allSources;
	std::vector<mvceditor::ProjectClass>::const_iterator it;
	std::vector<mvceditor::SourceClass>::const_iterator src;
	for (it = Projects.begin(); it != Projects.end(); ++it) {
		if (it->IsEnabled) {
			std::vector<mvceditor::SourceClass> phpSources = it->AllPhpSources();
			allSources.insert(allSources.end(), phpSources.begin(), phpSources.end());
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
	wxStringTokenizer tokenizer(PhpFileExtensionsString, wxT(";"));
	while (tokenizer.HasMoreTokens()) {
		wxString wildcard = tokenizer.NextToken();
		wildcards.push_back(wildcard);
	}
	return wildcards;
}


std::vector<wxString> mvceditor::StructsClass::GetCssFileExtensions() const {
	std::vector<wxString> wildcards;
	wxStringTokenizer tokenizer(CssFileExtensionsString, wxT(";"));
	while (tokenizer.HasMoreTokens()) {
		wxString wildcard = tokenizer.NextToken();
		wildcards.push_back(wildcard);
	}
	return wildcards;
}

std::vector<wxString> mvceditor::StructsClass::GetSqlFileExtensions() const {
	std::vector<wxString> wildcards;
	wxStringTokenizer tokenizer(SqlFileExtensionsString, wxT(";"));
	while (tokenizer.HasMoreTokens()) {
		wxString wildcard = tokenizer.NextToken();
		wildcards.push_back(wildcard);
	}
	return wildcards;
}

std::vector<wxString> mvceditor::StructsClass::GetMiscFileExtensions() const {
	std::vector<wxString> wildcards;
	wxStringTokenizer tokenizer(MiscFileExtensionsString, wxT(";"));
	while (tokenizer.HasMoreTokens()) {
		wxString wildcard = tokenizer.NextToken();
		wildcards.push_back(wildcard);
	}
	return wildcards;
}

std::vector<wxString> mvceditor::StructsClass::GetNonPhpFileExtensions() const {
	std::vector<wxString> wildcards;
	std::vector<wxString> css = GetCssFileExtensions();
	std::vector<wxString> sql = GetSqlFileExtensions();
	std::vector<wxString> misc = GetMiscFileExtensions();

	wildcards.insert(wildcards.end(), css.begin(), css.end());
	wildcards.insert(wildcards.end(), sql.begin(), sql.end());
	wildcards.insert(wildcards.end(), misc.begin(), misc.end());
	return wildcards;
}

bool mvceditor::StructsClass::IsAPhpSourceFile(const wxString& fullPath) const {
	bool isPhp = false;
	std::vector<mvceditor::ProjectClass>::const_iterator it;
	for (it = Projects.begin(); it != Projects.end() && !isPhp; ++it) {
		isPhp = it->IsAPhpSourceFile(fullPath);
	}
	return isPhp;
}

wxString mvceditor::StructsClass::RelativeFileName(const wxString &fullPath, wxString& projectLabel) const {
	wxString relativeName;
	std::vector<mvceditor::ProjectClass>::const_iterator it;
	for (it = Projects.begin(); it != Projects.end() && relativeName.IsEmpty(); ++it) {
		relativeName = it->RelativeFileName(fullPath);
		if (!relativeName.IsEmpty()) {
			projectLabel = it->Label; 
		}
	}
	return relativeName;
}

void mvceditor::StructsClass::ClearDetectedInfos() {
	std::vector<mvceditor::DatabaseInfoClass>::iterator info = Infos.begin();
	while(info != Infos.end()) {
		if (info->IsDetected) {
			info = Infos.erase(info);
		}
		else {
			++info;
		}
	}
}

void mvceditor::StructsClass::AssignFileExtensions(mvceditor::ProjectClass &project) const {
	project.PhpFileExtensions = GetPhpFileExtensions();
	project.CssFileExtensions = GetCssFileExtensions();
	project.SqlFileExtensions = GetSqlFileExtensions();
	project.MiscFileExtensions = GetMiscFileExtensions();
}