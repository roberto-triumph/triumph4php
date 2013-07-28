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
	, TagFinderList()
	, DoTouchedProjects(false) {
}

void mvceditor::ProjectTagActionClass::SetTouchedProjects(const std::vector<mvceditor::ProjectClass>& touchedProjects) {
	DoTouchedProjects = true;
	Projects = touchedProjects;
}

bool mvceditor::ProjectTagActionClass::Init(mvceditor::GlobalsClass& globals) {
	SetStatus(_("Tag Cache"));
	
	// ATTN: assumes that all projects have the same extension
	TagFinderList.InitGlobalTag(globals.TagCacheDbFileName, globals.GetPhpFileExtensions(), globals.GetNonPhpFileExtensions(), globals.Environment.Php.Version);

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
		TagFinderList.Walk(DirectorySearch);
		if (!DirectorySearch.More()) {
			if (!IsCancelled()) {

				// eventId will be set by the PostEvent method
				mvceditor::TagFinderListCompleteEventClass evt(wxID_ANY);
				PostEvent(evt);
			}
		}
	}
}

void mvceditor::ProjectTagActionClass::IterateProjects() {
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



wxString mvceditor::ProjectTagInitActionClass::GetLabel() const {
	return _("Project tags initialization");
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
	mvceditor::TagFinderListClass* tagFinderList = new mvceditor::TagFinderListClass;
	tagFinderList->InitNativeTag(mvceditor::NativeFunctionsAsset());
	tagFinderList->InitGlobalTag(globals.TagCacheDbFileName, globals.GetPhpFileExtensions(), otherFileExtensions, version);
	tagFinderList->InitDetectorTag(globals.DetectorCacheDbFileName);
	globals.TagCache.RegisterGlobal(tagFinderList);
}

mvceditor::ProjectTagSingleFileActionClass::ProjectTagSingleFileActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: GlobalActionClass(runningThreads, eventId) 
	, Project()
	, FileName() 
	, TagFinderList() {

}

void mvceditor::ProjectTagSingleFileActionClass::SetFileToParse(const wxString& fullPath) {
	FileName.Assign(fullPath.c_str());
}

bool mvceditor::ProjectTagSingleFileActionClass::Init(mvceditor::GlobalsClass& globals) {

	// get the project that the file is in
	// TODO: what if the file is in more than 1 project?
	std::vector<mvceditor::ProjectClass>::const_iterator project;
	bool isFileFromProject = false;
	for (project = globals.Projects.begin(); project != globals.Projects.end(); ++project) {
		if (project->IsEnabled && project->IsAPhpSourceFile(FileName.GetFullPath())) {
			isFileFromProject = true;
			Project = *project;
			break;
		}
	}
	if (isFileFromProject) {
		TagFinderList.InitGlobalTag(globals.TagCacheDbFileName, globals.GetPhpFileExtensions(), globals.GetNonPhpFileExtensions(), globals.Environment.Php.Version);

	}
	return isFileFromProject; 
}

void mvceditor::ProjectTagSingleFileActionClass::BackgroundWork() {
	SetStatus(_("Tag Cache Re-tag"));
	if (!FileName.FileExists()) {
		return;
	}
	std::vector<mvceditor::SourceClass>::iterator src;
	for (src = Project.Sources.begin(); src != Project.Sources.end(); ++src) {
		if (src->Contains(FileName.GetFullPath())) {

			// this specific sequence is needed so that the source_id
			// is set properly in the database
			TagFinderList.TagParser.BeginSearch(src->RootDirectory.GetPath());
			TagFinderList.TagParser.Walk(FileName.GetFullPath());
			TagFinderList.TagParser.EndSearch();
			break;
		}
	}
}

wxString mvceditor::ProjectTagSingleFileActionClass::GetLabel() const {
	return wxT("Tag Cache Single File");
}