/**
 * The MIT License
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
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <actions/ProjectTagActionClass.h>
#include <code_control/ResourceCacheBuilderClass.h>
#include <globals/Assets.h>

mvceditor::ProjectTagActionClass::ProjectTagActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId) 
	: GlobalActionClass(runningThreads, eventId)
	, Projects()
	, DirectorySearch()
	, TagCacheDbFileName()
	, Version(pelet::PHP_53) 
	, GlobalCache()
	, DoTouchedProjects(false) {
}

bool mvceditor::ProjectTagActionClass::InitForFile(mvceditor::GlobalsClass& globals, const wxString& fullPath) {
	wxFileName fileName(fullPath);
	bool ret = false;
	if (!fileName.FileExists()) {
		return ret;
	}
	Version = globals.Environment.Php.Version;

	// break up name into dir + name, add name to file filters
	mvceditor::SourceClass src;
	src.RootDirectory.AssignDir(fileName.GetPath());
	src.SetIncludeWildcards(fileName.GetFullName());
	std::vector<wxString> phpFileExtensions;
	phpFileExtensions.push_back(fileName.GetFullName());
	std::vector<wxString> miscFileExtensions;

	std::vector<mvceditor::SourceClass> srcs;
	srcs.push_back(src);
	if (DirectorySearch.Init(srcs)) {
		GlobalCache.InitGlobalTag(globals.TagCacheDbFileName, phpFileExtensions, miscFileExtensions, Version);
		ret = true;
	}
	return ret;
}

void mvceditor::ProjectTagActionClass::SetTouchedProjects(const std::vector<mvceditor::ProjectClass>& touchedProjects) {
	DoTouchedProjects = true;
	Projects = touchedProjects;
}

bool mvceditor::ProjectTagActionClass::Init(mvceditor::GlobalsClass& globals) {
	SetStatus(_("Tag Cache"));
	Version = globals.Environment.Php.Version;

	// not sure if wxFileName assignment is a complete clone, so use Assign() just in case
	// since we will access the filenames from multiple threads
	TagCacheDbFileName.Assign(globals.TagCacheDbFileName.GetFullPath());
	
	// if we were not given projects, scan all of them
	if (!DoTouchedProjects) {
		Projects.clear();
		std::vector<mvceditor::ProjectClass>::const_iterator project;
		for (project = globals.Projects.begin(); project != globals.Projects.end(); ++project) {
			if (project->IsEnabled && !project->AllPhpSources().empty()) {
				Projects.push_back(*project);
			}
		}
	}
	return !Projects.empty();
}


void mvceditor::ProjectTagActionClass::BackgroundWork() {
	if (DirectorySearch.More() && Projects.empty()) {
		IterateDirectory();
	}
	else {
		IterateProjects();
	}
}

void mvceditor::ProjectTagActionClass::IterateDirectory() {
	
	// careful to test for destroy first
	while (!IsCancelled() && DirectorySearch.More()) {
		GlobalCache.Walk(DirectorySearch);
		if (!DirectorySearch.More()) {
			if (!IsCancelled()) {

				// eventId will be set by the PostEvent method
				mvceditor::GlobalCacheCompleteEventClass evt(wxID_ANY);
				PostEvent(evt);
			}
		}
	}
}

void mvceditor::ProjectTagActionClass::IterateProjects() {
	std::vector<wxString> miscFileExtensions;
	std::vector<wxString> phpFileExtensions;
	
	// ATTN: assumes that all projects have the same extension
	if (!Projects.empty()) {
		phpFileExtensions = Projects[0].PhpFileExtensions;
		miscFileExtensions = Projects[0].AllNonPhpExtensions();	
	}
	GlobalCache.InitGlobalTag(TagCacheDbFileName, phpFileExtensions, miscFileExtensions, Version);
	for (size_t i = 0; !IsCancelled() && i < Projects.size(); ++i) {
		mvceditor::ProjectClass project = Projects[i];
		if (DirectorySearch.Init(project.AllSources())) {
			SetStatus(_("Tag Cache / ") + project.Label);			
			IterateDirectory();
		}
	}
}

wxString mvceditor::ProjectTagActionClass::GetLabel() const {
	return _("Project Resource Parsing");
}

mvceditor::ProjectTagInitActionClass::ProjectTagInitActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: InitializerGlobalActionClass(runningThreads, eventId) {

}

void mvceditor::ProjectTagInitActionClass::Work(mvceditor::GlobalsClass &globals) {
	SetStatus(_("Tag Cache Init"));

	// need to clear the entire cache, then add only the newly enabled projects
	globals.TagCache.Clear();
	pelet::Versions version = globals.Environment.Php.Version;
	std::vector<wxString> otherFileExtensions = globals.GetNonPhpFileExtensions();
	
	// the tag cache will own the global cache pointers
	// register the project tag DB file now so that it is available for code completion
	// even though we know it is stale. The user is notified that the
	// cache is stale and may not have all of the results
	// the tag cache will own these pointers
	mvceditor::GlobalCacheClass* globalCache = new mvceditor::GlobalCacheClass;
	globalCache->InitNativeTag(mvceditor::NativeFunctionsAsset());
	globalCache->InitGlobalTag(globals.TagCacheDbFileName, globals.GetPhpFileExtensions(), otherFileExtensions, version);
	globalCache->InitDetectorTag(globals.DetectorCacheDbFileName);
	globals.TagCache.RegisterGlobal(globalCache);
}

wxString mvceditor::ProjectTagInitActionClass::GetLabel() const {
	return _("Project tags initialization");
}