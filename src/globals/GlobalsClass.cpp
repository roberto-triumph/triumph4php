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
#include <globals/GlobalsClass.h>
#include <globals/Assets.h>
#include <wx/tokenzr.h>

mvceditor::GlobalsClass::GlobalsClass()
	: Environment()
	, TagCache()
	, UrlTagFinder()
	, SqlResourceFinder()
	, DatabaseTags()
	, Projects()
	, CurrentUrl() 
	, ChosenBrowser()
	, PhpFileExtensionsString(wxT("*.php"))	
	, CssFileExtensionsString(wxT("*.css"))
	, SqlFileExtensionsString(wxT("*.sql"))
	, MiscFileExtensionsString(wxT("*.js;*.html;*.yml;*.xml;*.txt"))
	, TagCacheDbFileName(mvceditor::TagCacheAsset())
	, DetectorCacheDbFileName(mvceditor::DetectorCacheAsset()) 
	, ResourceCacheSession()
	, DetectorCacheSession() {
}

void mvceditor::GlobalsClass::Close() {
	TagCache.Clear();
	UrlTagFinder.ClearSession();
	SqlResourceFinder.ClearSession();
	ResourceCacheSession.close();
	DetectorCacheSession.close();
}

std::vector<mvceditor::SourceClass> mvceditor::GlobalsClass::AllEnabledSources() const {
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

std::vector<mvceditor::SourceClass> mvceditor::GlobalsClass::AllEnabledPhpSources() const {
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

std::vector<wxFileName> mvceditor::GlobalsClass::AllEnabledSourceDirectories() const {
	std::vector<wxFileName> allSourceDirectories;
	std::vector<mvceditor::ProjectClass>::const_iterator it;
	std::vector<mvceditor::SourceClass>::const_iterator src;
	for (it = Projects.begin(); it != Projects.end(); ++it) {
		if (it->IsEnabled) {
			for (src = it->Sources.begin(); src != it->Sources.end(); ++src) {
				allSourceDirectories.push_back(src->RootDirectory);
			}
		}
	}
	return allSourceDirectories;
}

std::vector<mvceditor::ProjectClass> mvceditor::GlobalsClass::AllEnabledProjects() const {
	std::vector<mvceditor::ProjectClass> enabledProjects;
	std::vector<mvceditor::ProjectClass>::const_iterator it;
	for (it = Projects.begin(); it != Projects.end(); ++it) {
		if (it->IsEnabled) {
			enabledProjects.push_back(*it);
		}
	}
	return enabledProjects;
}

bool mvceditor::GlobalsClass::HasSources() const {
	bool hasSources = false;
	for (size_t i = 0; i < Projects.size() && !hasSources; ++i) {
		mvceditor::ProjectClass project = Projects[i];
		hasSources = project.IsEnabled && project.HasSources();
	}
	return hasSources;
}

std::vector<wxString> mvceditor::GlobalsClass::GetPhpFileExtensions() const {
	std::vector<wxString> wildcards;
	wxStringTokenizer tokenizer(PhpFileExtensionsString, wxT(";"));
	while (tokenizer.HasMoreTokens()) {
		wxString wildcard = tokenizer.NextToken();
		wildcards.push_back(wildcard);
	}
	return wildcards;
}


std::vector<wxString> mvceditor::GlobalsClass::GetCssFileExtensions() const {
	std::vector<wxString> wildcards;
	wxStringTokenizer tokenizer(CssFileExtensionsString, wxT(";"));
	while (tokenizer.HasMoreTokens()) {
		wxString wildcard = tokenizer.NextToken();
		wildcards.push_back(wildcard);
	}
	return wildcards;
}

std::vector<wxString> mvceditor::GlobalsClass::GetSqlFileExtensions() const {
	std::vector<wxString> wildcards;
	wxStringTokenizer tokenizer(SqlFileExtensionsString, wxT(";"));
	while (tokenizer.HasMoreTokens()) {
		wxString wildcard = tokenizer.NextToken();
		wildcards.push_back(wildcard);
	}
	return wildcards;
}

std::vector<wxString> mvceditor::GlobalsClass::GetMiscFileExtensions() const {
	std::vector<wxString> wildcards;
	wxStringTokenizer tokenizer(MiscFileExtensionsString, wxT(";"));
	while (tokenizer.HasMoreTokens()) {
		wxString wildcard = tokenizer.NextToken();
		wildcards.push_back(wildcard);
	}
	return wildcards;
}

std::vector<wxString> mvceditor::GlobalsClass::GetNonPhpFileExtensions() const {
	std::vector<wxString> wildcards;
	std::vector<wxString> css = GetCssFileExtensions();
	std::vector<wxString> sql = GetSqlFileExtensions();
	std::vector<wxString> misc = GetMiscFileExtensions();

	wildcards.insert(wildcards.end(), css.begin(), css.end());
	wildcards.insert(wildcards.end(), sql.begin(), sql.end());
	wildcards.insert(wildcards.end(), misc.begin(), misc.end());
	return wildcards;
}

std::vector<wxString> mvceditor::GlobalsClass::GetAllSourceFileExtensions() const {
	std::vector<wxString> wildcards;
	std::vector<wxString> php = GetPhpFileExtensions();
	std::vector<wxString> css = GetCssFileExtensions();
	std::vector<wxString> sql = GetSqlFileExtensions();
	std::vector<wxString> misc = GetMiscFileExtensions();

	wildcards.insert(wildcards.end(), php.begin(), php.end());
	wildcards.insert(wildcards.end(), css.begin(), css.end());
	wildcards.insert(wildcards.end(), sql.begin(), sql.end());
	wildcards.insert(wildcards.end(), misc.begin(), misc.end());
	return wildcards;
}

bool mvceditor::GlobalsClass::IsAPhpSourceFile(const wxString& fullPath) const {
	bool isPhp = false;
	std::vector<mvceditor::ProjectClass>::const_iterator it;
	for (it = Projects.begin(); it != Projects.end() && !isPhp; ++it) {
		isPhp = it->IsAPhpSourceFile(fullPath);
	}
	return isPhp;
}

bool mvceditor::GlobalsClass::HasAPhpExtension(const wxString& fullPath) const {
	std::vector<wxString> wildcards = GetPhpFileExtensions();
	bool found = false;
	for (size_t i = 0; i < wildcards.size(); ++i) {
		if (wxMatchWild(wildcards[i], fullPath)) {
			found = true;
			break;
		}
	}
	return found;
}

bool mvceditor::GlobalsClass::HasASqlExtension(const wxString& fullPath) const {
	std::vector<wxString> wildcards = GetSqlFileExtensions();
	bool found = false;
	for (size_t i = 0; i < wildcards.size(); ++i) {
		if (wxMatchWild(wildcards[i], fullPath)) {
			found = true;
			break;
		}
	}
	return found;
}

bool mvceditor::GlobalsClass::HasACssExtension(const wxString& fullPath) const {
	std::vector<wxString> wildcards = GetCssFileExtensions();
	bool found = false;
	for (size_t i = 0; i < wildcards.size(); ++i) {
		if (wxMatchWild(wildcards[i], fullPath)) {
			found = true;
			break;
		}
	}
	return found;
}

bool mvceditor::GlobalsClass::HasAMiscExtension(const wxString& fullPath) const {
	std::vector<wxString> wildcards = GetMiscFileExtensions();
	bool found = false;
	for (size_t i = 0; i < wildcards.size(); ++i) {
		if (wxMatchWild(wildcards[i], fullPath)) {
			found = true;
			break;
		}
	}
	return found;
}

wxString mvceditor::GlobalsClass::RelativeFileName(const wxString &fullPath, wxString& projectLabel) const {
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

void mvceditor::GlobalsClass::ClearDetectedInfos() {
	std::vector<mvceditor::DatabaseTagClass>::iterator it = DatabaseTags.begin();
	while(it != DatabaseTags.end()) {
		if (it->IsDetected) {
			it = DatabaseTags.erase(it);
		}
		else {
			++it;
		}
	}
}

void mvceditor::GlobalsClass::AssignFileExtensions(mvceditor::ProjectClass &project) const {
	project.PhpFileExtensions = GetPhpFileExtensions();
	project.CssFileExtensions = GetCssFileExtensions();
	project.SqlFileExtensions = GetSqlFileExtensions();
	project.MiscFileExtensions = GetMiscFileExtensions();
}

std::vector<mvceditor::TemplateFileTagClass> mvceditor::GlobalsClass::CurrentTemplates() const {
	std::vector<mvceditor::TemplateFileTagClass> templates;
	wxFileName detectorDbFileName = mvceditor::DetectorCacheAsset();
	if (detectorDbFileName.IsOk()) {
		mvceditor::TemplateFileTagFinderClass fileTags;
		fileTags.Init(detectorDbFileName);
		templates = fileTags.All();
	}
	return templates;
}

std::vector<mvceditor::DatabaseTagClass> mvceditor::GlobalsClass::AllEnabledDatabaseTags() const {
	std::vector<mvceditor::DatabaseTagClass> enabled;
	std::vector<mvceditor::DatabaseTagClass>::const_iterator tag;
	for (tag = DatabaseTags.begin(); tag != DatabaseTags.end(); ++tag) {
		if (tag->IsEnabled) {
			enabled.push_back(*tag);
		}
	}
	return enabled;
}