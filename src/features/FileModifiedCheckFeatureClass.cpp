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
#include <MvcEditor.h>
#include <globals/Errors.h>
#include <globals/Events.h>
#include <wx/choicdlg.h>
#include <wx/volume.h>
#include <algorithm>
#include <map>

static int ID_FILE_MODIFIED_CHECK = wxNewId();
static int ID_FILE_MODIFIED_POLL = wxNewId();

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
static void CollapseDirsFiles(mvceditor::TagCacheClass& tagCache, std::map<wxString, int>& paths, std::map<wxString, int>& collapsedDirs, std::map<wxString, int>& collapsedFiles) {
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
static std::map<wxString, mvceditor::CodeControlClass*> OpenedFiles(mvceditor::NotebookClass* notebook) {
	std::map<wxString, mvceditor::CodeControlClass*> openedFiles;
	if (!notebook) {
		return openedFiles;
	}
	size_t size = notebook->GetPageCount();
	if (size > 0) {

		// loop through all of the opened files to get the files to
		// be checked
		// no need to check new files as they are not yet in the file system
		for (size_t i = 0; i < size; ++i) {
			mvceditor::CodeControlClass* ctrl = notebook->GetCodeControl(i);
			if (ctrl && !ctrl->IsNew()) {
				openedFiles[ctrl->GetFileName()] = ctrl;
			}
		}
	}
	return openedFiles;
}

mvceditor::FileModifiedCheckFeatureClass::FileModifiedCheckFeatureClass(mvceditor::AppClass& app)
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
, AllVolumes()
, NetworkVolumes()
, FilesToPoll()
, LastWatcherEventTime() 
, IsWatchError(false) {
	FsWatcher = NULL;
	LastWatcherEventTime = wxDateTime::Now();
}

void mvceditor::FileModifiedCheckFeatureClass::OnAppReady(wxCommandEvent& event) {
#ifdef __WXMSW__
	
	// on windows, we check for network drives
	mvceditor::VolumeListActionClass* volumeAction = new mvceditor::VolumeListActionClass(App.RunningThreads, wxID_ANY);
	App.RunningThreads.Queue(volumeAction);
#else

	// add the enabled projects to the watch list
	StartWatch();
#endif
	Timer.Start(250, wxTIMER_CONTINUOUS);
	PollTimer.Start(1000, wxTIMER_CONTINUOUS);
}

void mvceditor::FileModifiedCheckFeatureClass::OnAppExit(wxCommandEvent& event) {
	Timer.Stop();
	PollTimer.Stop();

	// unregister ourselves as the event handler from watcher 
	FsWatcher->SetOwner(NULL);
	delete FsWatcher;
	FsWatcher = NULL;
}

void mvceditor::FileModifiedCheckFeatureClass::OnAppFileOpened(wxCommandEvent& event) {
	wxString fullPath = event.GetString();
	bool isInSources = false;
	bool doAddWatch = false;

	// In case the file that was opened is not part of a project, we want to setup a 
	// watch for it.
	std::vector<mvceditor::SourceClass> sources = App.Globals.AllEnabledSources();
	std::vector<mvceditor::SourceClass>::const_iterator src;
	for (src = sources.begin(); src != sources.end(); ++src) {
		if (src->IsInRootDirectory(fullPath)) {
			isInSources = true;
		}
	}
	if (!isInSources) {
		doAddWatch = true;
	}
	wxFileName fileName(fullPath);
	if (!doAddWatch) {

		// OR it is in a network drive, create a watch for it
		// since sources from network drives are not added to the watch
		if (fileName.HasVolume()) {
			wxString vol = fileName.GetVolume() + fileName.GetVolumeSeparator();
			if (std::find(NetworkVolumes.begin(), NetworkVolumes.end(), vol) != NetworkVolumes.end()) {
				doAddWatch = true;
			}
		}
	}
	if (doAddWatch && fileName.FileExists()) {
		FilesToPoll.push_back(fileName);
	}
}

void mvceditor::FileModifiedCheckFeatureClass::OnAppFileClosed(mvceditor::CodeControlEventClass& event) {
	mvceditor::CodeControlClass* ctrl = event.GetCodeControl();
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

void mvceditor::FileModifiedCheckFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {
	
	// on MSW, wxFileSystemWatcher.RemoveAll does not actually remove the old 
	// watches.
	// that is why we are using a pointer; deleting the object does remove the
	// old watches
	// see http://trac.wxwidgets.org/ticket/12847
	FsWatcher->SetOwner(NULL);
	delete FsWatcher;
	StartWatch();
}

void mvceditor::FileModifiedCheckFeatureClass::StartWatch() {
	IsWatchError = false;
	FsWatcher = new wxFileSystemWatcher();
	FsWatcher->SetOwner(this);

	std::vector<mvceditor::SourceClass> sources = App.Globals.AllEnabledPhpSources();
	std::vector<mvceditor::SourceClass>::const_iterator source;
	for (source = sources.begin(); source != sources.end(); ++source) {
		wxFileName sourceDir = source->RootDirectory;

		// check to see if source directory is in a remote volume (network drive)
		// if any source directories
		// are in network drives, we will not add them to the watch, as watches on network
		// directories fail to notify of file changes inside of sub-directories.
		bool doAdd = true;
		if (sourceDir.HasVolume()) {
			doAdd = std::find(NetworkVolumes.begin(), NetworkVolumes.end(), sourceDir.GetVolume()) == NetworkVolumes.end();
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

void mvceditor::FileModifiedCheckFeatureClass::OnVolumeListComplete(mvceditor::VolumeListEventClass& event) {
	AllVolumes = event.AllVolumes;
	NetworkVolumes = event.NetworkVolumes;
	StartWatch();
}

void mvceditor::FileModifiedCheckFeatureClass::OnTimer(wxTimerEvent& event) {
	wxDateTime now = wxDateTime::Now();
	wxTimeSpan span = now.Subtract(LastWatcherEventTime);
	if (span.GetSeconds() <= 1) {

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
	
	// oddity. int linux sometimes we get rename events where the old and new paths
	// are the same. in this case, lets treat them as modifications
	std::map<wxString, wxString>::iterator rename = pathsRenamed.begin();
	while (rename != pathsRenamed.end()) {
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
	
	std::map<wxString, mvceditor::CodeControlClass*> openedFiles = OpenedFiles(GetNotebook());

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
	Timer.Start(250, wxTIMER_CONTINUOUS);
}

void mvceditor::FileModifiedCheckFeatureClass::HandleOpenedFiles(std::map<wxString, mvceditor::CodeControlClass*>& openedFiles, std::map<wxString, wxString>& pathsRenamed) {

	// if the file that was modified is one of the opened files, we need to prompt the user
	// to see if they want to reload the new version
	std::map<wxString, int>::const_iterator f;
	std::map<wxString, mvceditor::CodeControlClass*> filesToPrompt;
	for (f = FilesExternallyModified.begin(); f != FilesExternallyModified.end(); ++f) {
		wxString fullPath = f->first;
		wxFileName fileName(fullPath);
		bool isOpened = openedFiles.find(fullPath) != openedFiles.end();
		if (isOpened && fileName.GetModificationTime() != openedFiles[fullPath]->GetFileOpenedDateTime()) {
			
			// file is opened, but since file modified time is newer than what the code 
			// control read in then it means that the file was modified externally
			// checking the timestamps also lets us skip  files that are saved inside of the editor 
			// by the user clicking "save" on mvc editor. do nothing
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
		std::map<wxString, mvceditor::CodeControlClass*>::iterator openedFile;
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
	std::map<wxString, mvceditor::CodeControlClass*> openedFilesRenamed;
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

void mvceditor::FileModifiedCheckFeatureClass::HandleNonOpenedFiles(std::map<wxString, mvceditor::CodeControlClass*>& openedFiles, std::map<wxString, wxString>& pathsRenamed) {
	std::map<wxString, int>::iterator f;
	for (f = FilesExternallyModified.begin(); f != FilesExternallyModified.end(); ++f) {
		wxString fullPath = f->first;
		bool isOpened = openedFiles.find(fullPath) != openedFiles.end();
		if (!isOpened) {
			// file is not open. notify the app that a file was externally modified
			wxCommandEvent modifiedEvt(mvceditor::EVENT_APP_FILE_EXTERNALLY_MODIFIED);
			modifiedEvt.SetString(fullPath);
			App.EventSink.Publish(modifiedEvt);
		}
	}
	for (f = FilesExternallyCreated.begin(); f != FilesExternallyCreated.end(); ++f) {
		wxString fullPath = f->first;
		wxCommandEvent modifiedEvt(mvceditor::EVENT_APP_FILE_EXTERNALLY_CREATED);
		modifiedEvt.SetString(fullPath);
		App.EventSink.Publish(modifiedEvt);
	}
	for (f = FilesExternallyDeleted.begin(); f != FilesExternallyDeleted.end(); ++f) {
		wxString fullPath = f->first;
		bool isOpened = openedFiles.find(fullPath) != openedFiles.end();
		if (!isOpened) {

			// file is not open. notify the app that a file was externally deleted
			wxCommandEvent modifiedEvt(mvceditor::EVENT_APP_FILE_DELETED);
			modifiedEvt.SetString(fullPath);
			App.EventSink.Publish(modifiedEvt);
		}
	}
	for (f = DirsExternallyCreated.begin(); f != DirsExternallyCreated.end(); ++f) {
		wxCommandEvent modifiedEvt(mvceditor::EVENT_APP_DIR_CREATED);
		modifiedEvt.SetString(f->first);
		App.EventSink.Publish(modifiedEvt);
	}
	for (f = DirsExternallyDeleted.begin(); f != DirsExternallyDeleted.end(); ++f) {
		wxCommandEvent modifiedEvt(mvceditor::EVENT_APP_DIR_DELETED);
		modifiedEvt.SetString(f->first);
		App.EventSink.Publish(modifiedEvt);
	}
	std::map<wxString, wxString>::iterator pair;
	for (pair = pathsRenamed.begin(); pair != pathsRenamed.end(); ++pair) {
		
		// figure out if we renamed a file or a dir
		if (wxFileName::DirExists(pair->second)) {
			mvceditor::RenameEventClass renameEvt(mvceditor::EVENT_APP_DIR_RENAMED, pair->first, pair->second);
			App.EventSink.Publish(renameEvt);
		}
		else {
			mvceditor::RenameEventClass renameEvt(mvceditor::EVENT_APP_FILE_RENAMED, pair->first, pair->second);
			App.EventSink.Publish(renameEvt);
		}
	}
}

void mvceditor::FileModifiedCheckFeatureClass::FilesModifiedPrompt(std::map<wxString, mvceditor::CodeControlClass*>& filesToPrompt) {
	if (filesToPrompt.empty()) {
		return;
	}
	wxArrayString choices;
	std::map<wxString, mvceditor::CodeControlClass*>::iterator it;
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
		choices, wxOK | wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
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
			wxCommandEvent reloadEvt(mvceditor::EVENT_APP_FILE_REVERTED);
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

void mvceditor::FileModifiedCheckFeatureClass::FilesDeletedPrompt(std::map<wxString, mvceditor::CodeControlClass*>& openedFiles, 
																  std::map<wxString, int>& deletedFiles) {
	std::map<wxString, int>::const_iterator file;
	wxString files;
	bool deletingOpened = false;
	for (file = deletedFiles.begin(); file != deletedFiles.end(); ++ file) {
		wxString fullPath = file->first;

		// find the control for the file
		if (openedFiles.end() != openedFiles.find(fullPath)) {
			mvceditor::CodeControlClass* ctrl = openedFiles[fullPath];
			ctrl->TreatAsNew();
			files += fullPath + wxT("\n");
			deletingOpened = true;
		}
		// send the deleted file event to the app
		wxCommandEvent deleteEvt(mvceditor::EVENT_APP_FILE_DELETED);
		deleteEvt.SetString(fullPath);
		App.EventSink.Publish(deleteEvt);		
	}

	// only show a message if a file that is being edited was deleted
	if (deletingOpened) {
		wxString message;
		message += _("The following files have been deleted externally.\n");
		message += _("You will need to save the file to store the contents.\n\n");
		message += files;
		int opts = wxICON_QUESTION;
		wxMessageBox(message, _("Warning"), opts, GetMainWindow());
	}
}

void mvceditor::FileModifiedCheckFeatureClass::OnFsWatcher(wxFileSystemWatcherEvent& event) {
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

void mvceditor::FileModifiedCheckFeatureClass::HandleWatchError() {
	FsWatcher->SetOwner(NULL);
	delete FsWatcher;

	wxString dirsDeleted;

	// check to see if the error was due to a watched directory being deleted
	std::vector<mvceditor::SourceClass> sources = App.Globals.AllEnabledPhpSources();
	std::vector<mvceditor::SourceClass>::const_iterator source;
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

void mvceditor::FileModifiedCheckFeatureClass::OnPollTimer(wxTimerEvent& event) {
	if (FilesToPoll.empty()) {
		return;
	}
	mvceditor::NotebookClass* notebook = GetNotebook();
	
	std::vector<mvceditor::FileModifiedTimeClass> fileMods;

	// loop through all of the opened files to get the files to
	// be checked
	// no need to check new files as they are not yet in the file system
	for (size_t i = 0; i < FilesToPoll.size(); ++i) {
		mvceditor::FileModifiedTimeClass fileMod;
		fileMod.FileName = FilesToPoll[i];
		for (size_t j = 0; j < notebook->GetPageCount(); ++j) {

			// get the last modified time from the file at the time we opened the file
			mvceditor::CodeControlClass* ctrl = notebook->GetCodeControl(j);
			if (ctrl->GetFileName() == FilesToPoll[i].GetFullPath()) {
				fileMod.ModifiedTime = ctrl->GetFileOpenedDateTime();
				fileMods.push_back(fileMod);
				break;
			}
		}
	}
	if (!fileMods.empty()) {

		// stop the timer that way we dont check files again until the user answers some questions
		PollTimer.Stop();
		mvceditor::FileModifiedCheckActionClass* action = 
			new mvceditor::FileModifiedCheckActionClass(App.RunningThreads, ID_FILE_MODIFIED_POLL);
		action->SetFiles(fileMods);
		App.RunningThreads.Queue(action);
	}
}

void mvceditor::FileModifiedCheckFeatureClass::OnFilesCheckComplete(mvceditor::FilesModifiedEventClass& event) {
    mvceditor::NotebookClass* notebook = GetNotebook();
    size_t notebookSize = notebook->GetPageCount();
    if (notebookSize > 0 && (event.Modified.size() + event.Deleted.size()) > 0) {
			std::map<wxString, mvceditor::CodeControlClass*> openedFilesWithCodeControl;
			for (size_t i = 0; i < notebookSize; ++i) {
				mvceditor::CodeControlClass* ctrl = notebook->GetCodeControl(i);
				openedFilesWithCodeControl[ctrl->GetFileName()] = ctrl;
			}

            if (!event.Modified.empty()) {

				// group the modified file with its code control. then we will prompt the user
				// which files they want to keep/revert
				std::map<wxString, mvceditor::CodeControlClass*> filesToPrompt;
				for (size_t i = 0; i < event.Modified.size(); ++i) {
					wxString fullPath = event.Modified[i].GetFullPath();
					if (openedFilesWithCodeControl.count(fullPath) > 0) {
						filesToPrompt[fullPath] = openedFilesWithCodeControl[fullPath];
					}
				}
				FilesModifiedPrompt(filesToPrompt);
            }
            if (!event.Deleted.empty()) {
					std::map<wxString, int> deletedFiles;
					for (size_t i = 0; i < event.Deleted.size(); ++i) {
						deletedFiles[event.Deleted[i].GetFullPath()] = 1;
					}
                    FilesDeletedPrompt(openedFilesWithCodeControl, deletedFiles);
            }
    }
    PollTimer.Start(1000, wxTIMER_CONTINUOUS);
}

mvceditor::VolumeListActionClass::VolumeListActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
: ActionClass(runningThreads, eventId) {
}

void mvceditor::VolumeListActionClass::BackgroundWork() {
#ifdef __WXMSW__

	wxArrayString allVols = wxFSVolume::GetVolumes(wxFS_VOL_MOUNTED, wxFS_VOL_REMOVABLE | wxFS_VOL_READONLY);
	

	std::vector<wxString> allVolumes,
		networkVolumes;
	for (size_t i = 0; i < allVols.GetCount(); ++i) {
		wxFSVolume vol(allVols[i]);
		allVolumes.push_back(allVols[i]);
		if (vol.GetFlags() & wxFS_VOL_REMOTE) {
			networkVolumes.push_back(allVols[i]);
		}
	}
	mvceditor::VolumeListEventClass evt(GetEventId(), allVolumes, networkVolumes);
	PostEvent(evt);
#endif
}

wxString mvceditor::VolumeListActionClass::GetLabel() const {
	return wxT("Volume List");
}

mvceditor::VolumeListEventClass::VolumeListEventClass(int id, 
													  const std::vector<wxString>& allVolumes, 
													  const std::vector<wxString>& networkVolumes)
: wxEvent(id, mvceditor::EVENT_ACTION_VOLUME_LIST)
, AllVolumes()
, NetworkVolumes() {
	mvceditor::DeepCopy(AllVolumes, allVolumes);
	mvceditor::DeepCopy(NetworkVolumes, networkVolumes);
}	


wxEvent* mvceditor::VolumeListEventClass::Clone() const {
	return new mvceditor::VolumeListEventClass(GetId(), AllVolumes, NetworkVolumes);
}

const wxEventType mvceditor::EVENT_ACTION_VOLUME_LIST = wxNewEventType();

BEGIN_EVENT_TABLE(mvceditor::FileModifiedCheckFeatureClass, mvceditor::FeatureClass)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_READY, mvceditor::FileModifiedCheckFeatureClass::OnAppReady)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_EXIT, mvceditor::FileModifiedCheckFeatureClass::OnAppExit)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_FILE_OPENED, mvceditor::FileModifiedCheckFeatureClass::OnAppFileOpened)
	EVT_APP_FILE_CLOSED(mvceditor::FileModifiedCheckFeatureClass::OnAppFileClosed)
	EVT_TIMER(ID_FILE_MODIFIED_CHECK, mvceditor::FileModifiedCheckFeatureClass::OnTimer)
	EVT_TIMER(ID_FILE_MODIFIED_POLL, mvceditor::FileModifiedCheckFeatureClass::OnPollTimer)
	EVT_FILES_EXTERNALLY_MODIFIED_COMPLETE(ID_FILE_MODIFIED_POLL, mvceditor::FileModifiedCheckFeatureClass::OnFilesCheckComplete)
	EVT_FSWATCHER(wxID_ANY, mvceditor::FileModifiedCheckFeatureClass::OnFsWatcher)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_PREFERENCES_SAVED, mvceditor::FileModifiedCheckFeatureClass::OnPreferencesSaved)
	EVT_ACTION_VOLUME_LIST(wxID_ANY, mvceditor::FileModifiedCheckFeatureClass::OnVolumeListComplete)
END_EVENT_TABLE()
