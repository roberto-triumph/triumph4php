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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <features/FileModifiedCheckFeatureClass.h>
#include <Triumph.h>
#include <globals/Errors.h>
#include <globals/Events.h>
#include <wx/choicdlg.h>
#include <wx/volume.h>
#include <algorithm>
#include <map>

static int ID_FILE_MODIFIED_CHECK = wxNewId();
static int ID_FILE_MODIFIED_POLL = wxNewId();
static int ID_FILE_MODIFIED_ACTION = wxNewId();

/**
 * collapse a list of paths into files and directories.  The collapsing part is that
 * if list contains a directory and a file in that directory (or a sub-directory) the it is 
 * ignored.
 *
 * For example:
 * paths = { "/home/user/project1/index.php" => 1, "/home/user/project1/js/index.js" => 1, 
 *           "/home/user/project1/" => 1, "/home/user/project2/index.php" => 1 }
 *
 * Then, the final result will be
 *
 * collapsedDirs = { "/home/user/project1/" => 1 }
 * collapsedFiles = {"/home/user/project2/index.php" => 1 }
 *
 * @param tagCache to find out if a path is a file or a directory
 * @param paths the list of paths to be checked
 * @param collapsedDis the directories in paths
 * @param collapsedFiles the files in paths (except files in any of collapsedDirs)
 */
static void CollapseDirsFiles(t4p::TagCacheClass& tagCache, std::map<wxString, int>& paths, std::map<wxString, int>& collapsedDirs, std::map<wxString, int>& collapsedFiles) {
	std::map<wxString, int>::iterator mapIt;
	std::vector<wxString> sortedPaths;
	for (mapIt = paths.begin(); mapIt != paths.end(); ++mapIt) {
		sortedPaths.push_back(mapIt->first);
	}
	std::sort(sortedPaths.begin(), sortedPaths.end());

	std::vector<wxString>::iterator path;
	for (path = sortedPaths.begin(); path != sortedPaths.end(); ++path) {

		// we want to know if the path is a file or a directory
		// in the case of deletes, we can't query the file system as the file/dir no
		// longer exists
		// we can look it up in the tag cache instead
		if (wxFileName::DirExists(*path) || tagCache.HasDir(*path)) {
			wxFileName dirCreated;
			dirCreated.AssignDir(*path);
			
			// if a parent dir already exists, then it means that this is a subdir and we want to skip it
			size_t dirCount = dirCreated.GetDirCount();
			bool foundSubDir = false;
			for (size_t i = 0; i < dirCount; ++i) {
				if (collapsedDirs.find(dirCreated.GetPath()) != collapsedDirs.end()) {
					foundSubDir = true;
					break;
				}
				dirCreated.RemoveLastDir();
			}
			if (!foundSubDir) {
				collapsedDirs[*path] = 1;
			}
		}
		else if (wxFileName::FileExists(*path) || tagCache.HasFullPath(*path)) {
			
			// if the file's dir parent dir has been labeled as created, we want to skip it
			wxFileName fileCreated(*path);
			wxFileName fileDir;
			fileDir.AssignDir(fileCreated.GetPath());
			size_t dirCount = fileDir.GetDirCount();
			bool foundSubDir = false;
			for (size_t i = 0; i < dirCount; ++i) {
				if (collapsedDirs.find(fileDir.GetPath()) != collapsedDirs.end()) {
					foundSubDir = true;
					break;
				}
				fileDir.RemoveLastDir();
			}
			if (!foundSubDir) {
				collapsedFiles[*path] = 1;
			}
		}
	}
}

/**
 * @return map of the code controls that have opened files (as opposed to new files being edited)
 *         key is the full path of the file being edited, value is the code control itself
 */
static std::map<wxString, t4p::CodeControlClass*> OpenedFiles(t4p::NotebookClass* notebook) {
	std::map<wxString, t4p::CodeControlClass*> openedFiles;
	if (!notebook) {
		return openedFiles;
	}
	size_t size = notebook->GetPageCount();
	if (size > 0) {

		// loop through all of the opened files to get the files to
		// be checked
		// no need to check new files as they are not yet in the file system
		for (size_t i = 0; i < size; ++i) {
			t4p::CodeControlClass* ctrl = notebook->GetCodeControl(i);
			if (ctrl && !ctrl->IsNew()) {
				openedFiles[ctrl->GetFileName()] = ctrl;
			}
		}
	}
	return openedFiles;
}

t4p::FileModifiedCheckFeatureClass::FileModifiedCheckFeatureClass(t4p::AppClass& app)
: FeatureClass(app)
, Timer(this, ID_FILE_MODIFIED_CHECK)
, PollTimer(this, ID_FILE_MODIFIED_POLL)
, FilesExternallyCreated()
, FilesExternallyModified()
, FilesExternallyDeleted() 
, DirsExternallyCreated()
, DirsExternallyModified()
, DirsExternallyDeleted()
, PathsExternallyCreated()
, PathsExternallyModified()
, PathsExternallyDeleted() 
, PathsExternallyRenamed()
, LocalVolumes()
, FilesToPoll()
, LastWatcherEventTime() 
, PollDeleteCount(0)
, PollModifiedCount(0)
, IsWatchError(false)
, JustSaved(false) {
	FsWatcher = NULL;
	LastWatcherEventTime = wxDateTime::Now();
}

void t4p::FileModifiedCheckFeatureClass::OnAppReady(wxCommandEvent& event) {
#ifdef __WXMSW__
	
	// on windows, we check for network drives
	t4p::VolumeListActionClass* volumeAction = new t4p::VolumeListActionClass(App.RunningThreads, wxID_ANY);
	App.RunningThreads.Queue(volumeAction);
#else

	// add the enabled projects to the watch list
	StartWatch();
#endif
	Timer.Start(250, wxTIMER_CONTINUOUS);
	PollTimer.Start(1000, wxTIMER_CONTINUOUS);
}

void t4p::FileModifiedCheckFeatureClass::OnAppExit(wxCommandEvent& event) {
	Timer.Stop();
	PollTimer.Stop();

	// unregister ourselves as the event handler from watcher 
	if (FsWatcher) {
		FsWatcher->SetOwner(NULL);
		delete FsWatcher;
		FsWatcher = NULL;
	}
}

void t4p::FileModifiedCheckFeatureClass::OnAppFileOpened(t4p::CodeControlEventClass& event) {
	wxString fullPath = event.GetCodeControl()->GetFileName();
	bool isInSources = false;
	bool doPoll = false;

	// In case the file that was opened is not part of a project, we want to setup
	// polling for it.
	std::vector<t4p::SourceClass> sources = App.Globals.AllEnabledSources();
	std::vector<t4p::SourceClass>::const_iterator src;
	for (src = sources.begin(); src != sources.end(); ++src) {
		if (src->IsInRootDirectory(fullPath)) {
			isInSources = true;
		}
	}
	if (!isInSources) {
		doPoll = true;
	}
	wxFileName fileName(fullPath);
	if (!doPoll) {

		// OR it is in a network drive, create a polling for it
		// since sources from network drives are not added to the watch
		if (fileName.HasVolume()) {
			wxString vol = fileName.GetVolume() + wxT(":\\");
			if (std::find(LocalVolumes.begin(), LocalVolumes.end(), vol) == LocalVolumes.end()) {
				doPoll = true;
			}
		}
	}
	if (doPoll && fileName.FileExists()) {
		FilesToPoll.push_back(fileName);
	}
}

void t4p::FileModifiedCheckFeatureClass::OnAppFileClosed(t4p::CodeControlEventClass& event) {
	t4p::CodeControlClass* ctrl = event.GetCodeControl();
	if (!ctrl) {
		return;
	}
	wxString fileClosed = ctrl->GetFileName();
	std::vector<wxFileName>::iterator it = FilesToPoll.begin();
	while (it != FilesToPoll.end()) {
		if (it->GetFullPath() == fileClosed) {
			it = FilesToPoll.erase(it);
		}
		else {
			++it;
		}
	}
}

void t4p::FileModifiedCheckFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {
	
	// on MSW, wxFileSystemWatcher.RemoveAll does not actually remove the old 
	// watches.
	// that is why we are using a pointer; deleting the object does remove the
	// old watches
	// see http://trac.wxwidgets.org/ticket/12847
	if (FsWatcher) {
		FsWatcher->SetOwner(NULL);
		delete FsWatcher;
		FsWatcher = NULL;
	}
	StartWatch();
}

void t4p::FileModifiedCheckFeatureClass::StartWatch() {
	IsWatchError = false;
	FsWatcher = new wxFileSystemWatcher();
	FsWatcher->SetOwner(this);

	std::vector<t4p::SourceClass> sources = App.Globals.AllEnabledPhpSources();
	std::vector<t4p::SourceClass>::const_iterator source;
	for (source = sources.begin(); source != sources.end(); ++source) {
		wxFileName sourceDir = source->RootDirectory;

		// check to see if source directory is in a remote volume (network drive)
		// if any source directories
		// are in network drives, we will not add them to the watch, as watches on network
		// directories fail to notify of file changes inside of sub-directories.
		// note that at program start we query for the local hard drives, and
		// by deduction if the source is not in one of the local hard drives
		// it means that its in a network drive.
		bool doAdd = true;
		wxString sourceVol = sourceDir.GetVolume();
		wxString volWithSep = sourceVol + wxT(":\\");
		if (sourceDir.HasVolume()) {
			std::vector<wxString>::iterator begin = LocalVolumes.begin();
			std::vector<wxString>::iterator end = LocalVolumes.end();
			doAdd = std::find(begin, end, sourceVol) != end
				|| std::find(begin, end, volWithSep) != end;
		}
		if (doAdd) {
			int flags = wxFSW_EVENT_CREATE  | wxFSW_EVENT_DELETE  | wxFSW_EVENT_RENAME | wxFSW_EVENT_MODIFY | 
				wxFSW_EVENT_ERROR | wxFSW_EVENT_WARNING;
			
			if (sourceDir.DirExists()) {
				sourceDir.DontFollowLink();
				FsWatcher->AddTree(sourceDir, flags);
			}
		}
	}
}

void t4p::FileModifiedCheckFeatureClass::OnVolumeListComplete(t4p::VolumeListEventClass& event) {
	LocalVolumes = event.LocalVolumes;
	StartWatch();
}

void t4p::FileModifiedCheckFeatureClass::OnTimer(wxTimerEvent& event) {
	wxDateTime now = wxDateTime::Now();
	wxTimeSpan span = now.Subtract(LastWatcherEventTime);
	if (span.GetSeconds() <= 2) {

		// we are still getting file change events. let's wait until all
		// of the changes are done 
		return;
	}
	if (PathsExternallyCreated.empty() && PathsExternallyModified.empty() && PathsExternallyDeleted.empty() && PathsExternallyRenamed.empty()) {

		// nothing to do
		return;
	}
	Timer.Stop();

	// when a directory is created/deleted, we will get created/deleted events for
	// the directory itself plus one event for each new /deleted.
	// lets collapse the sub files
	CollapseDirsFiles(App.Globals.TagCache, PathsExternallyCreated, DirsExternallyCreated, FilesExternallyCreated);
	CollapseDirsFiles(App.Globals.TagCache, PathsExternallyModified, DirsExternallyModified, FilesExternallyModified);
	CollapseDirsFiles(App.Globals.TagCache, PathsExternallyDeleted, DirsExternallyDeleted, FilesExternallyDeleted);

	std::map<wxString, wxString> pathsRenamed = PathsExternallyRenamed;

	// clear out the paths. we do this here because when we process a file we may create a prompt dialog
	// that the user needs to take action and that may take a while. If we cleared the paths after the prompt
	// we may delete paths we have not processed yet 
	PathsExternallyCreated.clear();
	PathsExternallyModified.clear();
	PathsExternallyDeleted.clear();
	PathsExternallyRenamed.clear();
	
	// make sure deleted files are actually deleted. sometime editors will swap files 
	// instead of overwriting them, and this results in DELETE events
	std::map<wxString, int>::iterator it = FilesExternallyDeleted.begin();
	while (it != FilesExternallyDeleted.end()) {
		wxString fullPath = it->first;
		wxFileName fileName(fullPath);
		if (fileName.FileExists()) {
			std::map<wxString, int>::iterator toDelete = it;
			++it;
			FilesExternallyDeleted.erase(toDelete);
		}
		else {
			++it;
		}
	}
	
	std::map<wxString, wxString>::iterator rename = pathsRenamed.begin();
	while (rename != pathsRenamed.end()) {
		
		// make sure renamed files are actually renamed. sometime editors will swap files 
		// instead of overwriting them, and this results in RENAME events
		wxFileName fileName(rename->first);
		if (fileName.FileExists()) {
			std::map<wxString, wxString>::iterator toErase = rename;
			rename++;
			pathsRenamed.erase(toErase->first);
			continue;
		}
		
		// if a file has been renamed, then remove any modify events for it
		std::map<wxString, int>::iterator it = FilesExternallyModified.find(rename->first);
		if (it != FilesExternallyModified.end()) {
			FilesExternallyModified.erase(it);
		}
		it = FilesExternallyModified.find(rename->second);
		if (it != FilesExternallyModified.end()) {
			FilesExternallyModified.erase(it);
		}

		// oddity. int linux sometimes we get rename events where the old and new paths
		// are the same. in this case, lets treat them as modifications
		if (rename->first == rename->second) {
			std::map<wxString, wxString>::iterator toDeleteRename = rename;
			++rename;
			PathsExternallyModified[rename->first] = 1;
			pathsRenamed.erase(toDeleteRename);
		}
		else {
			++rename;
		}
	}
	
	std::map<wxString, t4p::CodeControlClass*> openedFiles = OpenedFiles(GetNotebook());

	HandleOpenedFiles(openedFiles, pathsRenamed);
	HandleNonOpenedFiles(openedFiles, pathsRenamed);
	if (IsWatchError) {
		HandleWatchError();
	}

	// clear out the files we have processed
	FilesExternallyCreated.clear();
	FilesExternallyModified.clear();
	FilesExternallyDeleted.clear();
	DirsExternallyCreated.clear();
	DirsExternallyModified.clear();
	DirsExternallyDeleted.clear();
	Timer.Start(1000, wxTIMER_CONTINUOUS);
}

void t4p::FileModifiedCheckFeatureClass::HandleOpenedFiles(std::map<wxString, t4p::CodeControlClass*>& openedFiles, std::map<wxString, wxString>& pathsRenamed) {

	// if the file that was modified is one of the opened files, we need to prompt the user
	// to see if they want to reload the new version
	std::map<wxString, int>::const_iterator f;
	std::map<wxString, t4p::CodeControlClass*> filesToPrompt;
	for (f = FilesExternallyModified.begin(); f != FilesExternallyModified.end(); ++f) {
		wxString fullPath = f->first;
		wxFileName fileName(fullPath);
		bool isOpened = openedFiles.find(fullPath) != openedFiles.end();
		if (isOpened && fileName.GetModificationTime() != openedFiles[fullPath]->GetFileOpenedDateTime()) {
			
			// file is opened, but since file modified time is newer than what the code 
			// control read in then it means that the file was modified externally
			// checking the timestamps also lets us skip  files that are saved inside of the editor 
			// by the user clicking "save" on Triumph. do nothing
			// because the cache will be updated via the EVT_APP_FILE_SAVED handler
			filesToPrompt[fullPath] = openedFiles[fullPath];
		}
	}
	if (!filesToPrompt.empty()) {
		FilesModifiedPrompt(filesToPrompt);
	}

	// check if an opened file was deleted
	std::map<wxString, int> openedFilesDeleted;
	std::map<wxString, int>::iterator deletedFile;
	for (deletedFile = FilesExternallyDeleted.begin(); deletedFile != FilesExternallyDeleted.end(); ++deletedFile) {
		if (openedFiles.find(deletedFile->first) != openedFiles.end()) {
			openedFilesDeleted[deletedFile->first] = 1;
		}
	}

	// since we collapse files in the event that a dir is deleted, we need to 
	// check to see if the opened file exists in a deleted directory
	if (!DirsExternallyDeleted.empty()) {
		std::map<wxString, t4p::CodeControlClass*>::iterator openedFile;
		for (openedFile = openedFiles.begin(); openedFile != openedFiles.end(); ++openedFile) {	
			wxFileName parentDir;
			parentDir.AssignDir(wxFileName(openedFile->first).GetPath());
			
			// if a parent dir already exists, then it means that this is a subdir and we want to skip it
			size_t dirCount = parentDir.GetDirCount();
			bool foundSubDir = false;
			for (size_t i = 0; i < dirCount; ++i) {
				if (DirsExternallyDeleted.find(parentDir.GetPath()) != DirsExternallyDeleted.end()) {
					foundSubDir = true;
					break;
				}
				parentDir.RemoveLastDir();
			}
			if (!foundSubDir) {
				openedFilesDeleted[openedFile->first] = 1;
			}
		}
	}

	// check for renames; an opened file rename will be handled as a deletion for now
	std::map<wxString, wxString>::iterator renamed;
	std::map<wxString, t4p::CodeControlClass*> openedFilesRenamed;
	for (renamed = pathsRenamed.begin(); renamed != pathsRenamed.end(); ++renamed) {
		wxString renamedFrom = renamed->first;
		if (openedFiles.find(renamedFrom) != openedFiles.end()) {
			openedFilesDeleted[renamedFrom] = 1;
		}
		
		// check for renames, but check the new paths
		// if a file was renamed and the new name is an opened file, treat it as modified
		wxString renamedTo = renamed->second;
		if (openedFiles.find(renamedTo) != openedFiles.end()) {
			openedFilesRenamed[renamedTo] = openedFiles[renamedTo];
		}
	}

	if (!openedFilesDeleted.empty()) {
		FilesDeletedPrompt(openedFiles, openedFilesDeleted);
	}
	
	if (!openedFilesRenamed.empty()) {
		FilesModifiedPrompt(openedFilesRenamed);
	}
}

void t4p::FileModifiedCheckFeatureClass::HandleNonOpenedFiles(std::map<wxString, t4p::CodeControlClass*>& openedFiles, std::map<wxString, wxString>& pathsRenamed) {
	std::map<wxString, int>::iterator f;
	for (f = FilesExternallyModified.begin(); f != FilesExternallyModified.end(); ++f) {
		wxString fullPath = f->first;
		bool isOpened = openedFiles.find(fullPath) != openedFiles.end();
		if (!isOpened) {
			// file is not open. notify the app that a file was externally modified
			wxCommandEvent modifiedEvt(t4p::EVENT_APP_FILE_EXTERNALLY_MODIFIED);
			modifiedEvt.SetString(fullPath);
			App.EventSink.Publish(modifiedEvt);
		}
	}
	for (f = FilesExternallyCreated.begin(); f != FilesExternallyCreated.end(); ++f) {
		wxString fullPath = f->first;
		wxCommandEvent modifiedEvt(t4p::EVENT_APP_FILE_EXTERNALLY_CREATED);
		modifiedEvt.SetString(fullPath);
		App.EventSink.Publish(modifiedEvt);
	}
	for (f = FilesExternallyDeleted.begin(); f != FilesExternallyDeleted.end(); ++f) {
		wxString fullPath = f->first;
		bool isOpened = openedFiles.find(fullPath) != openedFiles.end();
		if (!isOpened) {

			// file is not open. notify the app that a file was externally deleted
			wxCommandEvent modifiedEvt(t4p::EVENT_APP_FILE_DELETED);
			modifiedEvt.SetString(fullPath);
			App.EventSink.Publish(modifiedEvt);
		}
	}
	for (f = DirsExternallyCreated.begin(); f != DirsExternallyCreated.end(); ++f) {
		wxCommandEvent modifiedEvt(t4p::EVENT_APP_DIR_CREATED);
		modifiedEvt.SetString(f->first);
		App.EventSink.Publish(modifiedEvt);
	}
	for (f = DirsExternallyDeleted.begin(); f != DirsExternallyDeleted.end(); ++f) {
		wxCommandEvent modifiedEvt(t4p::EVENT_APP_DIR_DELETED);
		modifiedEvt.SetString(f->first);
		App.EventSink.Publish(modifiedEvt);
	}
	std::map<wxString, wxString>::iterator pair;
	for (pair = pathsRenamed.begin(); pair != pathsRenamed.end(); ++pair) {
		
		// figure out if we renamed a file or a dir
		if (wxFileName::DirExists(pair->second)) {
			t4p::RenameEventClass renameEvt(t4p::EVENT_APP_DIR_RENAMED, pair->first, pair->second);
			App.EventSink.Publish(renameEvt);
		}
		else {
			t4p::RenameEventClass renameEvt(t4p::EVENT_APP_FILE_RENAMED, pair->first, pair->second);
			App.EventSink.Publish(renameEvt);
		}
	}
}

void t4p::FileModifiedCheckFeatureClass::FilesModifiedPrompt(std::map<wxString, t4p::CodeControlClass*>& filesToPrompt) {
	if (filesToPrompt.empty()) {
		return;
	}
 	wxArrayString choices;
	std::map<wxString, t4p::CodeControlClass*>::iterator it;
	for (it = filesToPrompt.begin(); it != filesToPrompt.end(); ++it) {
		choices.Add(it->first);
	}
	wxString msg;
	if (filesToPrompt.size() == 1) {
		msg = _("1 File has been modified externally. Reload file and lose any changes?\n");
		msg += _("If checked, the file will be reloaded. If left unchecked, the file will not be reloaded, allowing you to overwrite the file.");
	}
	else {
		msg = _("Files have been modified externally. Reload files and lose any changes?\n");
		msg += _("The checked files will be reloaded. Unchecked files will not be reloaded, allowing you to overwrite the files.");
	}
	wxMultiChoiceDialog dialog(GetMainWindow(), msg, _("Files Externally Modified"), 
		choices, wxOK | wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxCENTRE);
	dialog.ShowModal();
	wxArrayInt selections = dialog.GetSelections();

	// the code below relies on there being only one code control for each file
	std::vector<wxString> revertedFiles;
	for (size_t i = 0; i < selections.size(); ++i) {
		wxString fileName(choices[selections[i]]);

		// find the control for the file and revert the contents
		if (filesToPrompt.find(fileName) != filesToPrompt.end()) {
			filesToPrompt[fileName]->Revert();

			// need to notify the app that the file was reloaded
			wxCommandEvent reloadEvt(t4p::EVENT_APP_FILE_REVERTED);
			reloadEvt.SetString(fileName);
			App.EventSink.Publish(reloadEvt);
			revertedFiles.push_back(fileName);
		}
	}

	// now handle the files that the user did NOT want to reload
	// if the user did not want to reload the file, update the  opened date time
	// so that we treat the file as up-to-date
	for (it = filesToPrompt.begin(); it != filesToPrompt.end(); ++it) {
		if (std::find(revertedFiles.begin(), revertedFiles.end(), it->first) == revertedFiles.end()) {
			wxFileName fn(it->first);
			it->second->UpdateOpenedDateTime(fn.GetModificationTime());
		}
	}
}

void t4p::FileModifiedCheckFeatureClass::FilesDeletedPrompt(std::map<wxString, t4p::CodeControlClass*>& openedFiles, 
																  std::map<wxString, int>& deletedFiles) {
	std::map<wxString, int>::const_iterator file;
	wxString files;
	bool deletingOpened = false;
	for (file = deletedFiles.begin(); file != deletedFiles.end(); ++ file) {
		wxString fullPath = file->first;

		// find the control for the file
		if (openedFiles.end() != openedFiles.find(fullPath)) {
			t4p::CodeControlClass* ctrl = openedFiles[fullPath];
			ctrl->TreatAsNew();
			files += fullPath + wxT("\n");
			deletingOpened = true;
		}
		// send the deleted file event to the app
		wxCommandEvent deleteEvt(t4p::EVENT_APP_FILE_DELETED);
		deleteEvt.SetString(fullPath);
		App.EventSink.Publish(deleteEvt);
	}

	// only show a message if a file that is being edited was deleted
	if (deletingOpened) {
		wxString message;
		message += _("The following files have been deleted externally.\n");
		message += _("You will need to save the file to store the contents.\n\n");
		message += files;
		int opts = wxICON_QUESTION | wxCENTRE;
		wxMessageBox(message, _("Warning"), opts, GetMainWindow());
	}
}

void t4p::FileModifiedCheckFeatureClass::OnFsWatcher(wxFileSystemWatcherEvent& event) {
	LastWatcherEventTime = wxDateTime::Now();
	wxString path = event.GetPath().GetFullPath();
	wxFileName fileName = event.GetPath();
	if (wxFSW_EVENT_MODIFY == event.GetChangeType()) {
		PathsExternallyModified[path] = 1;
	}
	else if (wxFSW_EVENT_CREATE == event.GetChangeType()) {
		PathsExternallyCreated[path] = 1;
	}
	else if (wxFSW_EVENT_DELETE == event.GetChangeType()) {
		PathsExternallyDeleted[path] = 1;
	}
	else if (wxFSW_EVENT_RENAME == event.GetChangeType()) {
		PathsExternallyRenamed[path] = event.GetNewPath().GetFullPath();
	}
	else if (wxFSW_EVENT_WARNING == event.GetChangeType()) {
	
		// too many files being added/removed
		// this is probably a big directory being added / removed
		// hopefully the root directory is caught
	}
	else if (wxFSW_EVENT_ERROR == event.GetChangeType()) {

		// in MSW, an error event could be due to the watched directoty being deleted / renamed.
		// in this case, we need to restart the watch
		IsWatchError = true;
		wxASSERT_MSG(false, event.GetErrorDescription());
	}
}

void t4p::FileModifiedCheckFeatureClass::HandleWatchError() {
	if (FsWatcher) {
		FsWatcher->SetOwner(NULL);
		delete FsWatcher;
		FsWatcher = NULL;
	}

	wxString dirsDeleted;

	// check to see if the error was due to a watched directory being deleted
	std::vector<t4p::SourceClass> sources = App.Globals.AllEnabledPhpSources();
	std::vector<t4p::SourceClass>::const_iterator source;
	for (source = sources.begin(); source != sources.end(); ++source) {
		if (!source->RootDirectory.DirExists()) {
			dirsDeleted += source->RootDirectory.GetPath() + wxT("\n");
		}
	}

	if (!dirsDeleted.IsEmpty()) {
		wxString msg = wxString::FromAscii(
			"The following source directories have been detected as deleted from your system. "
			"If you intend to delete those permanently, you should go to File ... Projects "
			"and remove the source directories.\n"
		);
		msg += dirsDeleted;
		msg = wxGetTranslation(msg);
		wxMessageBox(msg, _("Warning"), wxICON_WARNING | wxOK, GetMainWindow());
	}

	StartWatch();
}


void t4p::FileModifiedCheckFeatureClass::OnPollTimer(wxTimerEvent& event) {
	if (FilesToPoll.empty()) {
		return;
	}
	if (!App.IsActive()) {
		
		// dont bother checking for
		// files as the user wont be around to answer questions
		return;
	}	
	
	t4p::CodeControlClass* ctrl = GetCurrentCodeControl();
	if (!ctrl) {
		return;
	}
	wxFileName ctrlFileName(ctrl->GetFileName());
	
	// loop through all of the opened files to get the files to
	// be checked
	// for now, we will only check the file that
	// is being edited (the tab that is selected)
	bool found = false;
	for (size_t i = 0; i < FilesToPoll.size(); ++i) {
		if (ctrlFileName == FilesToPoll[i]) {
			found = true;
			break;
		}
	}
	if (!found) {
		return;
	}
	PollTimer.Stop();

	bool exists = ctrlFileName.FileExists();
	bool modified  = false;
	wxDateTime newModTime;
	if (exists) {
		newModTime = ctrlFileName.GetModificationTime();
		if (newModTime.IsValid() && !newModTime.IsEqualTo(ctrl->GetFileOpenedDateTime())) {
			modified = true;
		}
	}

	if (!exists && PollDeleteCount <= 2) {
		
		// files doesnt exist. it may actually, but it may be in
		// the process of being moved by an external editor.
		// increment the delete count, and try to read next
		// timer tick
		PollDeleteCount++;
	}
	else if (!exists && PollDeleteCount > 2) {
		
		// we tried to get the modified time but it has
		// not existed.  assume file was delted.
		PollFileModifiedPrompt(ctrlFileName, true);
		PollDeleteCount = 0;
	}
	else if (exists && modified && !JustSaved) {

		// file time has been updated; file has been modified
		PollFileModifiedPrompt(ctrlFileName, false);
	}
	else if (exists && modified && JustSaved) {

		// a file was just saved in this editor.  the modified
		// time changed because this app changed it. set the
		// file modified time
		ctrl->UpdateOpenedDateTime(newModTime);
	}
	JustSaved = false;
	PollTimer.Start();
}

void t4p::FileModifiedCheckFeatureClass::OnFileSaved(t4p::CodeControlEventClass& event) {
	JustSaved = true;
}

void t4p::FileModifiedCheckFeatureClass::PollFileModifiedPrompt(const wxFileName& fileName, bool isFileDeleted) {
	std::map<wxString, t4p::CodeControlClass*> codeControls;
	std::map<wxString, int> filesToPrompt;
	t4p::CodeControlClass* ctrl = GetCurrentCodeControl();
	if (!isFileDeleted && ctrl) {

		// group the modified file with its code control. then we will prompt the user
		// which files they want to keep/revert
		filesToPrompt[fileName.GetFullPath()] = 1;
		codeControls[fileName.GetFullPath()] = ctrl;
		FilesModifiedPrompt(codeControls);
	}
	else if (ctrl) {
		std::map<wxString, int> deletedFiles;
		filesToPrompt[fileName.GetFullPath()] = 1;
		codeControls[fileName.GetFullPath()] = ctrl;
		FilesDeletedPrompt(codeControls, filesToPrompt);
	}
}

void t4p::FileModifiedCheckFeatureClass::OnActivateApp(wxCommandEvent& event) {

	// when user comes back to the app after the app was put in the 
	// background, check for external file modifications, as the user
	// might have changed it
	// don't do this on linux; prevent double modified dialogs popping up
	// on linux
	wxPlatformInfo info;
	if (info.GetOperatingSystemId() != wxOS_UNIX_LINUX) {
		wxTimerEvent timerEvt;
		OnPollTimer(timerEvt);
	}
}

t4p::VolumeListActionClass::VolumeListActionClass(t4p::RunningThreadsClass& runningThreads, int eventId)
: ActionClass(runningThreads, eventId) {
}

void t4p::VolumeListActionClass::BackgroundWork() {
#ifdef __WXMSW__

	wxArrayString localVolArray = wxFSVolume::GetVolumes(wxFS_VOL_MOUNTED, 
		wxFS_VOL_REMOTE | wxFS_VOL_REMOVABLE | wxFS_VOL_READONLY);
	

	std::vector<wxString> localVols;
	for (size_t i = 0; i < localVolArray.GetCount(); ++i) {
		localVols.push_back(localVolArray[i]);
	}
	t4p::VolumeListEventClass evt(GetEventId(), localVols);
	PostEvent(evt);
#endif
}

void t4p::VolumeListActionClass::DoCancel() {
#ifdef __WXMSW__
	wxFSVolume::CancelSearch();
#endif
}

wxString t4p::VolumeListActionClass::GetLabel() const {
	return wxT("Volume List");
}

t4p::VolumeListEventClass::VolumeListEventClass(int id, 
												const std::vector<wxString>& localVolumes)
: wxEvent(id, t4p::EVENT_ACTION_VOLUME_LIST)
, LocalVolumes() {
	t4p::DeepCopy(LocalVolumes, localVolumes);
}	


wxEvent* t4p::VolumeListEventClass::Clone() const {
	return new t4p::VolumeListEventClass(GetId(), LocalVolumes);
}

const wxEventType t4p::EVENT_ACTION_VOLUME_LIST = wxNewEventType();

BEGIN_EVENT_TABLE(t4p::FileModifiedCheckFeatureClass, t4p::FeatureClass)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_READY, t4p::FileModifiedCheckFeatureClass::OnAppReady)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_EXIT, t4p::FileModifiedCheckFeatureClass::OnAppExit)
	EVT_APP_FILE_OPEN(t4p::FileModifiedCheckFeatureClass::OnAppFileOpened)
	EVT_APP_FILE_CLOSED(t4p::FileModifiedCheckFeatureClass::OnAppFileClosed)
	EVT_TIMER(ID_FILE_MODIFIED_CHECK, t4p::FileModifiedCheckFeatureClass::OnTimer)
	EVT_TIMER(ID_FILE_MODIFIED_POLL, t4p::FileModifiedCheckFeatureClass::OnPollTimer)
	EVT_FSWATCHER(wxID_ANY, t4p::FileModifiedCheckFeatureClass::OnFsWatcher)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::FileModifiedCheckFeatureClass::OnPreferencesSaved)
	EVT_ACTION_VOLUME_LIST(wxID_ANY, t4p::FileModifiedCheckFeatureClass::OnVolumeListComplete)
	EVT_APP_FILE_SAVED(t4p::FileModifiedCheckFeatureClass::OnFileSaved)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_ACTIVATED, t4p::FileModifiedCheckFeatureClass::OnActivateApp)
END_EVENT_TABLE()
