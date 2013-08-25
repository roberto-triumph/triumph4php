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
#include <search/RecursiveDirTraverserClass.h>
#include <globals/Assets.h>

mvceditor::ProjectTagActionClass::ProjectTagActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId) 
	: GlobalActionClass(runningThreads, eventId)
	, Projects()
	, DirectorySearch()
	, TagFinderList()
	, DoTouchedProjects(false) 
	, FilesCompleted(0) 
	, FilesTotal(0) {
}

void mvceditor::ProjectTagActionClass::SetTouchedProjects(const std::vector<mvceditor::ProjectClass>& touchedProjects) {
	DoTouchedProjects = true;
	Projects = touchedProjects;
}

bool mvceditor::ProjectTagActionClass::Init(mvceditor::GlobalsClass& globals) {
	SetStatus(_("Tag Cache"));
	SetProgressMode(mvceditor::ActionClass::DETERMINATE);
	
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
	for (size_t i = 0; !IsCancelled() && i < Projects.size(); ++i) {
		FilesCompleted = 0;
		FilesTotal = 0;

		mvceditor::ProjectClass project = Projects[i];
		if (DirectorySearch.Init(project.AllSources(), mvceditor::DirectorySearchClass::PRECISE)) {
			FilesTotal = DirectorySearch.GetTotalFileCount();
			SetStatus(_("Tag Cache / ") + project.Label);			
			IterateDirectory();
		}
	}
}

void mvceditor::ProjectTagActionClass::IterateDirectory() {
	
	// careful to test for destroy first
	while (!IsCancelled() && DirectorySearch.More()) {
		TagFinderList.Walk(DirectorySearch);

		// if we have a total file count it means we want to send progress events
		if (FilesTotal > 0) {

			// we will try to send at most 100 events, this is in case we have big
			// projects with 10,000+ files we dont want to flood the system with events
			// that will barely be noticeable in the gauge.
			FilesCompleted++;
			double newProgress = (FilesCompleted * 1.0) / FilesTotal;
			int newProgressWhole = (int)floor(newProgress * 100);

			// we dont want to send the progress=0 event more than once
			if (newProgressWhole < 1) {
				newProgressWhole = 1;
			}
			SetPercentComplete(newProgressWhole);
		}

		if (!DirectorySearch.More()) {
			if (!IsCancelled()) {

				// eventId will be set by the PostEvent method
				mvceditor::TagFinderListCompleteEventClass evt(wxID_ANY);
				PostEvent(evt);
			}
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

mvceditor::ProjectTagDirectoryActionClass::ProjectTagDirectoryActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: GlobalActionClass(runningThreads, eventId) 
	, Project()
	, Dir() 
	, TagFinderList() {

}

void mvceditor::ProjectTagDirectoryActionClass::SetDirToParse(const wxString& path) {
	Dir.AssignDir(path.c_str());
}

bool mvceditor::ProjectTagDirectoryActionClass::Init(mvceditor::GlobalsClass& globals) {

	// get the project that the directory is in
	// TODO: what if the directory is in more than 1 project?
	std::vector<mvceditor::ProjectClass>::const_iterator project;
	std::vector<mvceditor::SourceClass>::const_iterator src;
	bool isDirFromProject = false;
	Dir.Normalize();
	wxString dirWithSep = Dir.GetPathWithSep();
	for (project = globals.Projects.begin(); !isDirFromProject && project != globals.Projects.end(); ++project) {
		if (project->IsEnabled) {
			for (src = project->Sources.begin(); src != project->Sources.end(); ++src) {
				if (src->IsInRootDirectory(dirWithSep)) {
					isDirFromProject = true;
					Project = *project;
					break;
				}
			}
		}
	}
	if (isDirFromProject) {
		TagFinderList.InitGlobalTag(globals.TagCacheDbFileName, globals.GetPhpFileExtensions(), globals.GetNonPhpFileExtensions(), globals.Environment.Php.Version);

	}
	return isDirFromProject; 
}

void mvceditor::ProjectTagDirectoryActionClass::BackgroundWork() {
	SetStatus(_("Tag Cache Re-tag"));
	if (!Dir.DirExists()) {
		return;
	}
	Dir.Normalize();
	wxString dirWithSep = Dir.GetPathWithSep();
	std::vector<mvceditor::SourceClass>::iterator src;
	for (src = Project.Sources.begin(); src != Project.Sources.end(); ++src) {
		if (src->IsInRootDirectory(dirWithSep)) {

			// this specific sequence is needed so that the source_id
			// is set properly in the database
			TagFinderList.TagParser.BeginSearch(src->RootDirectory.GetPath());
			
			std::vector<wxString> fullPaths;
			mvceditor::RecursiveDirTraverserClass traverser(fullPaths);
			wxDir dir;
			if (dir.Open(Dir.GetFullPath())) {
				dir.Traverse(traverser, wxEmptyString, wxDIR_DIRS | wxDIR_FILES);
				for (size_t i = 0; i < fullPaths.size(); ++i) {
					TagFinderList.TagParser.Walk(fullPaths[i]);
				}			
			}
			TagFinderList.TagParser.EndSearch();
			break;
		}
	}
}

wxString mvceditor::ProjectTagDirectoryActionClass::GetLabel() const {
	return wxT("Tag Cache Directory");
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
	wxString fullPath = FileName.GetFullPath();
	for (src = Project.Sources.begin(); src != Project.Sources.end(); ++src) {
		if (src->Contains(fullPath)) {

			// this specific sequence is needed so that the source_id
			// is set properly in the database
			TagFinderList.TagParser.BeginSearch(src->RootDirectory.GetPath());
			TagFinderList.TagParser.Walk(fullPath);
			TagFinderList.TagParser.EndSearch();
			break;
		}
	}
}

wxString mvceditor::ProjectTagSingleFileActionClass::GetLabel() const {
	return wxT("Tag Cache Single File");
}

mvceditor::ProjectTagSingleFileRenameActionClass::ProjectTagSingleFileRenameActionClass(mvceditor::RunningThreadsClass& runningThreads,
																						int eventId)
: GlobalActionClass(runningThreads, eventId)
, OldFileName()
, NewFileName()
, TagFinderList()
, Project() {

}

void mvceditor::ProjectTagSingleFileRenameActionClass::SetPaths(const wxString& oldPath, const wxString& newPath) {
	OldFileName.Assign(oldPath);
	NewFileName.Assign(newPath);
}

bool mvceditor::ProjectTagSingleFileRenameActionClass::Init(mvceditor::GlobalsClass& globals) {
	TagFinderList.InitGlobalTag(globals.TagCacheDbFileName, globals.GetPhpFileExtensions(), globals.GetNonPhpFileExtensions(), globals.Environment.Php.Version);
	std::vector<mvceditor::ProjectClass>::const_iterator project;
	std::vector<mvceditor::SourceClass>::const_iterator src;
	wxString path = OldFileName.GetFullPath();
	bool found = false;
	for (project = globals.Projects.begin(); !found && project != globals.Projects.end(); ++project) {
		if (project->IsEnabled) {
			for (src = project->Sources.begin(); src != project->Sources.end(); ++src) {
				if (src->IsInRootDirectory(path)) {
					found = true;
					Project = *project;
					break;
				}
			}
		}
	}
	return TagFinderList.IsTagFinderInit;
}

void mvceditor::ProjectTagSingleFileRenameActionClass::BackgroundWork() {

	// checking to see if this rename was an actual permanent rename as opposed to
	// file shuffling by apps.
	// some apps (notably VIM) will do unexpected things like rename a file when 
	// they open it, then right away create a new file with the original name. if we captured a file rename
	// but the old file still exists, assume that the old file was put right back. 
	bool oldFileExists = OldFileName.FileExists();
	
	// check to see if the original file was previosuly parsed
	// if the original file was previosuly parsed, just rename the file (no need to retag the file)
	if (!oldFileExists && TagFinderList.TagFinder.HasFullPath(OldFileName.GetFullPath())) {
		TagFinderList.TagParser.RenameFile(OldFileName, NewFileName);
	}
	else if (!oldFileExists) {
		
		// tag the file since we have never seen it. this could be the same
		// for example, when file with a non-php extension is renamed to have a 
		// php extension
		// this specific sequence is needed so that the source_id
		// is set properly in the database
		std::vector<mvceditor::SourceClass>::iterator src;
		for (src = Project.Sources.begin(); src != Project.Sources.end(); ++src) {
			wxString newFullPath = NewFileName.GetFullPath();
			if (src->Contains(newFullPath)) {
				TagFinderList.TagParser.BeginSearch(src->RootDirectory.GetPath());
				TagFinderList.TagParser.Walk(newFullPath);
				TagFinderList.TagParser.EndSearch();
			}
		}
	}
}

wxString mvceditor::ProjectTagSingleFileRenameActionClass::GetLabel() const {
	return wxT("Project Tag File Rename");
}

mvceditor::ProjectTagDirectoryRenameActionClass::ProjectTagDirectoryRenameActionClass(mvceditor::RunningThreadsClass& runningThreads,
																						int eventId)
: GlobalActionClass(runningThreads, eventId)
, OldDirectory()
, NewDirectory()
, TagFinderList() {

}

void mvceditor::ProjectTagDirectoryRenameActionClass::SetPaths(const wxString& oldPath, const wxString& newPath) {
	OldDirectory.AssignDir(oldPath);
	NewDirectory.AssignDir(newPath);
}

bool mvceditor::ProjectTagDirectoryRenameActionClass::Init(mvceditor::GlobalsClass& globals) {
	TagFinderList.InitGlobalTag(globals.TagCacheDbFileName, globals.GetPhpFileExtensions(), globals.GetNonPhpFileExtensions(), globals.Environment.Php.Version);
	return TagFinderList.IsTagFinderInit;
}

void mvceditor::ProjectTagDirectoryRenameActionClass::BackgroundWork() {
	TagFinderList.TagParser.RenameDir(OldDirectory, NewDirectory);
}

wxString mvceditor::ProjectTagDirectoryRenameActionClass::GetLabel() const {
	return wxT("Project Tag Directory Rename");
}