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
#include <algorithm>

t4p::GlobalsClass::GlobalsClass()
	: Environment()
	, TagCache()
	, UrlTagFinder()
	, SqlResourceFinder()
	, DatabaseTags()
	, Projects()
	, CurrentUrl() 
	, ChosenBrowser()
	, FileTypes()
	, TagCacheDbFileName(t4p::TagCacheAsset())
	, DetectorCacheDbFileName(t4p::DetectorCacheAsset()) 
	, ResourceCacheSession()
	, DetectorCacheSession()
	, LocalVolumes() {
}

void t4p::GlobalsClass::Close() {
	TagCache.Clear();
	UrlTagFinder.ClearSession();
	SqlResourceFinder.ClearSession();
	ResourceCacheSession.close();
	DetectorCacheSession.close();
}

std::vector<t4p::SourceClass> t4p::GlobalsClass::AllEnabledSources() const {
	std::vector<t4p::SourceClass> allSources;
	std::vector<t4p::ProjectClass>::const_iterator it;
	for (it = Projects.begin(); it != Projects.end(); ++it) {
		if (it->IsEnabled) {
			for (size_t j = 0; j < it->Sources.size(); ++j) {
				allSources.push_back(it->Sources[j]);
			}
		}
	}
	return allSources;
}

std::vector<t4p::SourceClass> t4p::GlobalsClass::AllEnabledPhpSources() const {
	std::vector<t4p::SourceClass> allSources;
	std::vector<t4p::ProjectClass>::const_iterator it;
	std::vector<t4p::SourceClass>::const_iterator src;
	for (it = Projects.begin(); it != Projects.end(); ++it) {
		if (it->IsEnabled) {
			std::vector<t4p::SourceClass> phpSources = it->AllPhpSources();
			allSources.insert(allSources.end(), phpSources.begin(), phpSources.end());
		}
	}
	return allSources;
}

std::vector<wxFileName> t4p::GlobalsClass::AllEnabledSourceDirectories() const {
	std::vector<wxFileName> allSourceDirectories;
	std::vector<t4p::ProjectClass>::const_iterator it;
	std::vector<t4p::SourceClass>::const_iterator src;
	for (it = Projects.begin(); it != Projects.end(); ++it) {
		if (it->IsEnabled) {
			for (src = it->Sources.begin(); src != it->Sources.end(); ++src) {
				allSourceDirectories.push_back(src->RootDirectory);
			}
		}
	}
	return allSourceDirectories;
}

std::vector<t4p::ProjectClass> t4p::GlobalsClass::AllEnabledProjects() const {
	std::vector<t4p::ProjectClass> enabledProjects;
	std::vector<t4p::ProjectClass>::const_iterator it;
	for (it = Projects.begin(); it != Projects.end(); ++it) {
		if (it->IsEnabled) {
			enabledProjects.push_back(*it);
		}
	}
	return enabledProjects;
}

bool t4p::GlobalsClass::HasSources() const {
	bool hasSources = false;
	for (size_t i = 0; i < Projects.size() && !hasSources; ++i) {
		t4p::ProjectClass project = Projects[i];
		hasSources = project.IsEnabled && project.HasSources();
	}
	return hasSources;
}

bool t4p::GlobalsClass::IsAPhpSourceFile(const wxString& fullPath) const {
	bool isPhp = false;
	std::vector<t4p::ProjectClass>::const_iterator it;
	for (it = Projects.begin(); it != Projects.end() && !isPhp; ++it) {
		isPhp = it->IsAPhpSourceFile(fullPath);
	}
	return isPhp;
}

bool t4p::GlobalsClass::IsASourceFile(const wxString& fullPath) const {
	if (!FileTypes.HasAnyExtension(fullPath)) {
		return false;
	}
	bool isInSource = false;
	std::vector<t4p::ProjectClass>::const_iterator it;
	for (it = Projects.begin(); it != Projects.end() && !isInSource; ++it) {
		isInSource = it->IsASourceFile(fullPath);
	}
	return isInSource;
}

wxString t4p::GlobalsClass::RelativeFileName(const wxString &fullPath, wxString& projectLabel) const {
	wxString relativeName;
	std::vector<t4p::ProjectClass>::const_iterator it;
	for (it = Projects.begin(); it != Projects.end() && relativeName.IsEmpty(); ++it) {
		relativeName = it->RelativeFileName(fullPath);
		if (!relativeName.IsEmpty()) {
			projectLabel = it->Label; 
		}
	}
	return relativeName;
}

void t4p::GlobalsClass::ClearDetectedInfos() {
	std::vector<t4p::DatabaseTagClass>::iterator it = DatabaseTags.begin();
	while(it != DatabaseTags.end()) {
		if (it->IsDetected) {
			it = DatabaseTags.erase(it);
		}
		else {
			++it;
		}
	}
}

void t4p::GlobalsClass::AssignFileExtensions(t4p::ProjectClass &project) const {
	project.PhpFileExtensions = FileTypes.GetPhpFileExtensions();
	project.CssFileExtensions = FileTypes.GetCssFileExtensions();
	project.SqlFileExtensions = FileTypes.GetSqlFileExtensions();
	project.MiscFileExtensions = FileTypes.GetMiscFileExtensions();
	project.JsFileExtensions = FileTypes.GetJsFileExtensions();
}

std::vector<t4p::TemplateFileTagClass> t4p::GlobalsClass::CurrentTemplates() const {
	std::vector<t4p::TemplateFileTagClass> templates;
	wxFileName detectorDbFileName = t4p::DetectorCacheAsset();
	if (detectorDbFileName.IsOk()) {
		t4p::TemplateFileTagFinderClass fileTags;
		fileTags.Init(detectorDbFileName);
		templates = fileTags.All();
	}
	return templates;
}

std::vector<t4p::DatabaseTagClass> t4p::GlobalsClass::AllEnabledDatabaseTags() const {
	std::vector<t4p::DatabaseTagClass> enabled;
	std::vector<t4p::DatabaseTagClass>::const_iterator tag;
	for (tag = DatabaseTags.begin(); tag != DatabaseTags.end(); ++tag) {
		if (tag->IsEnabled) {
			enabled.push_back(*tag);
		}
	}
	return enabled;
}

bool t4p::GlobalsClass::FindDatabaseTagByHash(const wxString& connectionHash, t4p::DatabaseTagClass& tag) const {
	bool found = false;
	UnicodeString icuHash = t4p::WxToIcu(connectionHash);
	std::vector<t4p::DatabaseTagClass>::const_iterator t;
	for (t = DatabaseTags.begin(); t != DatabaseTags.end(); ++t) {
		if (t->ConnectionHash() == icuHash) {
			found = true;
			tag = *t;
		}
	}
	return found;
}

bool t4p::GlobalsClass::IsInLocalVolume(const wxFileName& fileName) const {
	if (fileName.HasVolume()) {
		wxString vol = fileName.GetVolume();
		wxString volWithSep = vol + wxT(":\\");
		return std::find(LocalVolumes.begin(), LocalVolumes.end(), vol) != LocalVolumes.end()
			|| std::find(LocalVolumes.begin(), LocalVolumes.end(), volWithSep) != LocalVolumes.end();
	}
	
	// if file does not have a volume, then assume file is local (ie. linux
	// filenames dont have a volume)
	return true;
}
