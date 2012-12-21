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
#include <actions/ProjectResourceActionClass.h>
#include <code_control/ResourceCacheBuilderClass.h>
#include <globals/Assets.h>

mvceditor::ProjectResourceActionClass::ProjectResourceActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId) 
	: ActionClass(runningThreads, eventId)
	, Projects()
	, DirectorySearch()
	, Version(pelet::PHP_53) 
	, GlobalCache(NULL)
	, DoTouchedProjects(false) {
}

mvceditor::ProjectResourceActionClass::~ProjectResourceActionClass() {
	BackgroundCleanup();
}

bool mvceditor::ProjectResourceActionClass::InitForFile(const mvceditor::ProjectClass& project, const wxString& fullPath, pelet::Versions version) {
	wxFileName fileName(fullPath);
	if (!fileName.FileExists()) {
		return false;
	}
	Version = version;

	// break up name into dir + name, add name to file filters
	mvceditor::SourceClass src;
	src.RootDirectory.AssignDir(fullPath);
	src.SetIncludeWildcards(fileName.GetFullName());
	std::vector<wxString> phpFileExtensions;
	phpFileExtensions.push_back(fileName.GetFullName());
	std::vector<wxString> miscFileExtensions;

	std::vector<mvceditor::SourceClass> srcs;
	srcs.push_back(src);
	if (DirectorySearch.Init(srcs)) {
		wxASSERT_MSG(GlobalCache == NULL, _("cache pointer has not been cleaned up"));
		GlobalCache = new mvceditor::GlobalCacheClass();
		GlobalCache->Init(project.ResourceDbFileName, phpFileExtensions, miscFileExtensions, version, 1024);
		return true;
	}
	return false;
}

void mvceditor::ProjectResourceActionClass::SetTouchedProjects(const std::vector<mvceditor::ProjectClass>& touchedProjects) {
	DoTouchedProjects = true;
	Projects = touchedProjects;
}

bool mvceditor::ProjectResourceActionClass::Init(mvceditor::GlobalsClass& globals) {
	Version = globals.Environment.Php.Version;
	
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
	if (!Projects.empty()) {
		SetStatus(_("Indexing Projects"));
	}
	return !Projects.empty();
}


void mvceditor::ProjectResourceActionClass::BackgroundWork() {
	if (DirectorySearch.More() && Projects.empty()) {
		IterateDirectory();
	}
	else {
		IterateProjects();
	}
}

void mvceditor::ProjectResourceActionClass::IterateDirectory() {
	
	// careful to test for destroy first
	while (!IsCancelled() && DirectorySearch.More()) {
		GlobalCache->Walk(DirectorySearch);
		if (!DirectorySearch.More()) {

			// the event handler will own the pointer to the global cache
			if (!IsCancelled()) {

				// eventId will be set by the PostEvent method
				mvceditor::GlobalCacheCompleteEventClass evt(wxID_ANY, GlobalCache);
				PostEvent(evt);
			}
			else {
				
				// if the thread is being stopped the the handler will not receive the
				// event, we shouldn't send the event and clean the pointer ourselves
				delete GlobalCache;
			}
			GlobalCache = NULL;
		}
	}
}

void mvceditor::ProjectResourceActionClass::IterateProjects() {
	for (size_t i = 0; !IsCancelled() && i < Projects.size(); ++i) {
		mvceditor::ProjectClass project = Projects[i];
		std::vector<wxString> miscFileExtensions = project.AllNonPhpExtensions();	
		std::vector<mvceditor::SourceClass> sources = project.AllSources();

		if (DirectorySearch.Init(sources)) {
			wxASSERT_MSG(GlobalCache == NULL, _("cache pointer has not been cleaned up"));
			if (GlobalCache) {
				delete GlobalCache;
			}
			SetStatus(_("Updating Index for ") + project.Label);
			GlobalCache = new mvceditor::GlobalCacheClass;
			GlobalCache->Init(project.ResourceDbFileName, project.PhpFileExtensions, miscFileExtensions, Version, 1024);
			IterateDirectory();
		}
	}
}

void mvceditor::ProjectResourceActionClass::BackgroundCleanup() {
	if (GlobalCache) {

		// if the thread was stopped by the user (via a program exit), then we still 
		// own this pointer because we have not generated the event
		delete GlobalCache;
		GlobalCache = NULL;
	}
}

wxString mvceditor::ProjectResourceActionClass::GetLabel() const {
	return _("Project Resource Parsing");
}

mvceditor::ResourceCacheInitActionClass::ResourceCacheInitActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: InitializerActionClass(runningThreads, eventId) {

}

void mvceditor::ResourceCacheInitActionClass::Work(mvceditor::GlobalsClass &globals) {

	// need to clear the entire cache, then add only the newly enabled projects
	globals.ResourceCache.Clear();
	pelet::Versions version = globals.Environment.Php.Version;
	std::vector<wxString> otherFileExtensions = globals.GetNonPhpFileExtensions();
	
	// the resource cache will own all of the global cache pointers
	mvceditor::GlobalCacheClass* nativeCache = new mvceditor::GlobalCacheClass;
	nativeCache->Init(mvceditor::NativeFunctionsAsset(), globals.GetPhpFileExtensions(), otherFileExtensions, version);
	globals.ResourceCache.RegisterGlobal(nativeCache);
	
	std::vector<mvceditor::ProjectClass>::const_iterator project;
	for (project = globals.Projects.begin(); project != globals.Projects.end(); ++project) {
		if (project->IsEnabled && !project->AllPhpSources().empty()) {

			// register the project resource DB file now so that it is available for code completion
			// even though we know it is stale. The user is notified that the
			// cache is stale and may not have all of the results
			// the resource cache will own these pointers
			mvceditor::GlobalCacheClass* projectCache = new mvceditor::GlobalCacheClass;
			projectCache->Init(project->ResourceDbFileName, project->PhpFileExtensions, otherFileExtensions, version);
			globals.ResourceCache.RegisterGlobal(projectCache);
		}
	}
}

wxString mvceditor::ResourceCacheInitActionClass::GetLabel() const {
	return _("Resource cache initialization");
}