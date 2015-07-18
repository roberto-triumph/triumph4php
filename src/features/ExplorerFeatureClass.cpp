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

#include <features/ExplorerFeatureClass.h>
#include <globals/Assets.h>
#include <globals/Events.h>
#include <globals/FileName.h>
#include <globals/Number.h>
#include <search/Directory.h>
#include <Triumph.h>
#include <wx/file.h>
#include <algorithm>

/**
 * comparator function that compares 2 filenames by using the full name
 * only (wxFileName::GetFullName())
 */
static bool FileNameCmp(const wxFileName& a, const wxFileName& b) {
	wxString aName = a.GetFullName();
	wxString bName = b.GetFullName();
	return aName.compare(bName) < 0;
}

/**
 * comparator function that compares 2 filenames by using the full name
 * only (wxFileName::GetDirs().Last())
 */
static bool DirNameCmp(const wxFileName& a, const wxFileName& b) {
	if (a.GetDirCount() == 0) {
		return true;
	}
	if (b.GetDirCount() == 0) {
		return false;
	}
	wxString aName = a.GetDirs().Last();
	wxString bName = b.GetDirs().Last();
	return aName.compare(bName) < 0;
}

t4p::FileListingClass::FileListingClass(wxEvtHandler& handler)
: wxEvtHandler()
, WorkingDir()
, Files()
, Dirs()
, TotalFiles(0)
, TotalSubDirs(0)
, RunningThreads()
, Handler(handler)
, Watcher(NULL) {

	RunningThreads.SetMaxThreads(1);
	RunningThreads.AddEventHandler(this);
}

t4p::FileListingClass::~FileListingClass() {
	RunningThreads.RemoveEventHandler(this);
	RunningThreads.Shutdown();
	if (Watcher) {
		delete Watcher;
	}
}

void t4p::FileListingClass::OnFsWatcher(wxFileSystemWatcherEvent& event) {
	wxFileName modFile = event.GetNewPath();
	if (modFile.GetPathWithSep() != WorkingDir.GetPathWithSep()) {

		// event from directory we are not showing
		return;
	}
	if (event.GetChangeType() == wxFSW_EVENT_WARNING && event.GetWarningType() == wxFSW_WARNING_OVERFLOW) {

		// restart the watch
		delete Watcher;
		Watcher = new wxFileSystemWatcher();
		Watcher->SetOwner(this);
		Watcher->Add(WorkingDir, wxFSW_EVENT_CREATE | wxFSW_EVENT_DELETE | wxFSW_EVENT_RENAME | wxFSW_EVENT_WARNING | wxFSW_EVENT_ERROR);
	}
	else if (event.GetChangeType() == wxFSW_EVENT_ERROR) {

		// restart the watch
		delete Watcher;
		Watcher = new wxFileSystemWatcher();
		Watcher->SetOwner(this);
		Watcher->Add(WorkingDir, wxFSW_EVENT_CREATE | wxFSW_EVENT_DELETE | wxFSW_EVENT_RENAME | wxFSW_EVENT_WARNING | wxFSW_EVENT_ERROR);
	}

	// naive implementation for now, just refresh the entire dir
	// this is because we have labels to update, and the
	// items must be kept sorted (first dirs, then files)
	// each sorted, AND taking the filters into account
	else if (event.GetChangeType() == wxFSW_EVENT_CREATE
		|| event.GetChangeType() == wxFSW_EVENT_DELETE
		|| event.GetChangeType() == wxFSW_EVENT_RENAME) {
		wxPostEvent(&Handler, event);
	}
}

void t4p::FileListingClass::StartDelete(const std::vector<wxFileName>& dirs, const std::vector<wxFileName>& files) {
	t4p::ExplorerModifyActionClass* action = new t4p::ExplorerModifyActionClass(RunningThreads, ID_EXPLORER_MODIFY);
	action->SetFilesToRemove(dirs, files);
	RunningThreads.Queue(action);
}

void t4p::FileListingClass::StartRefresh(const wxFileName& dir, const std::vector<wxString>& filterExtensions, bool doHidden) {
	t4p::ExplorerFileSystemActionClass* action = new t4p::ExplorerFileSystemActionClass(RunningThreads, ID_EXPLORER_LIST_ACTION);
	action->Directory(dir, filterExtensions, doHidden);
	RunningThreads.Queue(action);
}

void t4p::FileListingClass::StartRename(const wxFileName& oldFile, const wxString& newName) {
	t4p::ExplorerModifyActionClass* action = new t4p::ExplorerModifyActionClass(RunningThreads, ID_EXPLORER_MODIFY);
	action->SetFileToRename(oldFile, newName);
	RunningThreads.Queue(action);
}

void t4p::FileListingClass::OnExplorerListComplete(t4p::ExplorerEventClass& event) {
	Files = event.Files;
	Dirs = event.SubDirs;
	TotalFiles = event.TotalFiles;
	TotalSubDirs = event.TotalSubDirs;

	// only recreate the watch when the explorer shows a new dir
	// this method will be called as a result of an external file
	// watcher event (new/delete file), if there are many events in
	// quick succession a crash would happen. in reality we
	// only need to recreate the watch when the explorer is being
	// pointed into a different directory than previous
	bool changedDir = WorkingDir != event.Dir;
	if (changedDir && Watcher) {
		delete Watcher;
		Watcher = NULL;
	}

	// storing the new working dir AFTER the comparison to see if
	// its changed
	WorkingDir = event.Dir;
	if (!Watcher) {
		Watcher = new wxFileSystemWatcher();
		Watcher->SetOwner(this);
		Watcher->Add(WorkingDir, wxFSW_EVENT_CREATE | wxFSW_EVENT_DELETE | wxFSW_EVENT_RENAME | wxFSW_EVENT_WARNING | wxFSW_EVENT_ERROR);
	}
	wxPostEvent(&Handler, event);
}

void t4p::FileListingClass::OnExplorerModifyComplete(t4p::ExplorerModifyEventClass& event) {
	wxPostEvent(&Handler, event);
}

t4p::ExplorerFeatureClass::ExplorerFeatureClass(t4p::AppClass& app)
	: FeatureClass(app) {
	wxPlatformInfo info;
	switch (info.GetOperatingSystemId()) {
		case wxOS_WINDOWS_NT:
			FileManagerExecutable = wxT("C:\\windows\\system32\\explorer.exe");
			ShellExecutable = wxT("C:\\windows\\system32\\cmd.exe");
			break;
		case wxOS_UNIX:
		case wxOS_UNIX_LINUX:
			FileManagerExecutable = wxT("/usr/bin/nautilus");
			ShellExecutable = wxT("/bin/sh");
			break;
		case wxOS_MAC_OSX_DARWIN:
			FileManagerExecutable = wxT("/usr/bin/open");
			ShellExecutable = wxT("/Applications/Utilities/Terminal.app/Contents/MacOS/Terminal");
			break;
		default:
			FileManagerExecutable = wxT("C:\\windows\\system32\\explorer.exe");
			ShellExecutable = wxT("C:\\windows\\system32\\cmd.exe");
	}
}

void t4p::ExplorerFeatureClass::LoadPreferences(wxConfigBase *config) {
	wxString s;
	config->Read(wxT("/Explorer/FileManagerExecutable"), &s);

	// when setting, make sure to have the defaults in case there is nothing
	// in the config yet
	if (!s.IsEmpty()) {
		FileManagerExecutable.Assign(s);
	}
	s = wxT("");
	config->Read(wxT("/Explorer/ShellExecutable"), &s);
	if (!s.IsEmpty()) {
		ShellExecutable.Assign(s);
	}
}

void t4p::ExplorerFeatureClass::OnAppPreferencesSaved(wxCommandEvent& event) {
	wxConfigBase *config = wxConfig::Get();
	config->Write(wxT("/Explorer/FileManagerExecutable"), FileManagerExecutable.GetFullPath());
	config->Write(wxT("/Explorer/ShellExecutable"), ShellExecutable.GetFullPath());
}

std::vector<wxFileName> t4p::ExplorerFeatureClass::EnabledSourceDirectories() const {
	return App.Globals.AllEnabledSourceDirectories();
}

std::vector<t4p::SourceClass> t4p::ExplorerFeatureClass::EnabledSources() const {
	return App.Globals.AllEnabledSources();
}

t4p::ExplorerEventClass::ExplorerEventClass(int eventId, const wxFileName& dir, const std::vector<wxFileName>& files,
												  const std::vector<wxFileName>& subDirs, const wxString& error, int totalFiles,
												  int totalSubDirs)
: wxEvent(eventId, t4p::EVENT_EXPLORER)
, Dir()
, Files()
, SubDirs()
, Error()
, TotalFiles(totalFiles)
, TotalSubDirs(totalSubDirs) {

	// clone filenames they contain wxStrings; no thread-safe
	Dir = t4p::FileNameCopy(dir);
	Files = t4p::DeepCopyFileNames(files);
	SubDirs = t4p::DeepCopyFileNames(subDirs);
	Error = Error.c_str();
}

wxEvent* t4p::ExplorerEventClass::Clone() const {
	t4p::ExplorerEventClass* event = new t4p::ExplorerEventClass(GetId(), Dir, Files, SubDirs, Error, TotalFiles, TotalSubDirs);
	return event;
}

t4p::ExplorerFileSystemActionClass::ExplorerFileSystemActionClass(t4p::RunningThreadsClass& runningThreads, int eventId)
: ActionClass(runningThreads, eventId)
, Dir()
, Extensions()
, DoHidden(false) {

}

wxString t4p::ExplorerFileSystemActionClass::GetLabel() const {
	return wxT("Explorer");
}

void t4p::ExplorerFileSystemActionClass::Directory(const wxFileName& dir, const std::vector<wxString>& extensions, bool doHidden) {
	Dir = t4p::FileNameCopy(dir);
	Extensions.clear();
	t4p::DeepCopy(Extensions, extensions);
	DoHidden = doHidden;
}

void t4p::ExplorerFileSystemActionClass::BackgroundWork() {
	std::vector<wxFileName> files;
	std::vector<wxFileName> subDirs;
	wxString error;
	int totalFiles = 0;
	int totalSubDirs = 0;
	wxString name;
	int flags = DoHidden ? (wxDIR_FILES | wxDIR_HIDDEN) : wxDIR_FILES;
	wxDir fileDir;
	if (fileDir.Open(Dir.GetPath())) {
		if (fileDir.GetFirst(&name, wxEmptyString, flags)) {
			totalFiles++;
			if (MatchesWildcards(name)) {
				wxFileName f(Dir.GetPath(), name);
				files.push_back(f);
			}
			while (fileDir.GetNext(&name) && !IsCancelled()) {
				totalFiles++;
				if (MatchesWildcards(name)) {
					wxFileName nextFile(Dir.GetPath(), name);
					files.push_back(nextFile);
				}
			}
		}
	}
	else {
		error = _("Could not open directory:") + Dir.GetFullPath();
	}

	// now get sub directories
	if (!IsCancelled()) {
		flags = DoHidden ? (wxDIR_DIRS | wxDIR_HIDDEN) : wxDIR_DIRS;
		wxDir dirDir;
		if (dirDir.Open(Dir.GetFullPath())) {
			if (dirDir.GetFirst(&name, wxEmptyString, flags)) {
				totalSubDirs++;
				wxFileName d;
				d.AssignDir(Dir.GetPathWithSep() + name);
				subDirs.push_back(d);
				while (dirDir.GetNext(&name) && !IsCancelled()) {
					totalSubDirs++;
					wxFileName nextDir;
					nextDir.AssignDir(Dir.GetPathWithSep() + name);
					subDirs.push_back(nextDir);
				}
			}
		}
		else {
			error = _("Could not open directory:") + Dir.GetFullPath();
		}
	}
	if (!IsCancelled()) {
		std::sort(files.begin(), files.end(), FileNameCmp);
		std::sort(subDirs.begin(), subDirs.end(), DirNameCmp);

		// PostEvent() will set the correct ID
		t4p::ExplorerEventClass evt(wxID_ANY, Dir, files, subDirs, error, totalFiles, totalSubDirs);
		PostEvent(evt);
	}
}

bool t4p::ExplorerFileSystemActionClass::MatchesWildcards(const wxString& fileName) {
	if (Extensions.empty()) {
		return true;
	}
	for (size_t i = 0; i < Extensions.size(); ++i) {
		if (wxMatchWild(Extensions[i], fileName)) {
			return true;
		}
	}
	return false;
}

t4p::ExplorerModifyActionClass::ExplorerModifyActionClass(t4p::RunningThreadsClass& runningThreads,
																int eventId)
: ActionClass(runningThreads, eventId)
, Action(NONE)
, Dirs()
, Files()
, OldFile()
, NewName() {
}

void t4p::ExplorerModifyActionClass::SetFilesToRemove(const std::vector<wxFileName>& dirs, const std::vector<wxFileName>& files) {
	Action = DELETE_FILES_DIRS;

	// make sure to clone
	Dirs = t4p::DeepCopyFileNames(dirs);
	Files = t4p::DeepCopyFileNames(files);
}

void t4p::ExplorerModifyActionClass::SetFileToRename(const wxFileName& file, const wxString& newName) {
	Action = RENAME_FILE;

	// make sure to clone
	OldFile.Assign(file.GetFullPath());
	NewName = newName.c_str();
}

void t4p::ExplorerModifyActionClass::BackgroundWork() {
	wxFileName parentDir;
	wxString name;
	bool totalSuccess = true;
	std::vector<wxFileName> dirsDeleted;
	std::vector<wxFileName> dirsNotDeleted;
	std::vector<wxFileName> filesDeleted;
	std::vector<wxFileName> filesNotDeleted;
	if (t4p::ExplorerModifyActionClass::DELETE_FILES_DIRS == Action) {
		std::vector<wxFileName>::iterator d;
		for (d = Dirs.begin(); d != Dirs.end(); ++d) {
			bool success = t4p::RecursiveRmDir(d->GetPath());
			if (success) {
				wxFileName wxFileName;
				wxFileName.AssignDir(d->GetPath());
				dirsDeleted.push_back(wxFileName);
			}
			else {
				wxFileName wxFileName;
				wxFileName.AssignDir(d->GetPath());
				dirsNotDeleted.push_back(wxFileName);
			}
			totalSuccess &= success;
		}
		std::vector<wxFileName>::iterator f;
		for (f = Files.begin(); f != Files.end(); ++f) {
			bool success = wxRemoveFile(f->GetFullPath());
			if (success) {
				wxFileName deletedFile(f->GetFullPath());
				filesDeleted.push_back(deletedFile);
			}
			else {
				wxFileName deletedFile(f->GetFullPath());
				filesNotDeleted.push_back(deletedFile);
			}
			totalSuccess &= success;
		}
		t4p::ExplorerModifyEventClass modEvent(GetEventId(),
			dirsDeleted, filesDeleted, dirsNotDeleted, filesNotDeleted, totalSuccess);
		PostEvent(modEvent);
	}
	else if (t4p::ExplorerModifyActionClass::RENAME_FILE == Action) {
		wxFileName destFile(OldFile.GetPath(), NewName);
		bool success = wxRenameFile(OldFile.GetFullPath(), destFile.GetFullPath(), false);

		t4p::ExplorerModifyEventClass modEvent(GetEventId(),
			OldFile, NewName, success);
		PostEvent(modEvent);
	}
}
wxString t4p::ExplorerModifyActionClass::GetLabel() const {
	return wxT("File System Modification");
}

t4p::ExplorerModifyEventClass::ExplorerModifyEventClass(int eventId, const wxFileName &oldFile,
															  const wxString &newName, bool success)
: wxEvent(eventId, t4p::EVENT_EXPLORER_MODIFY)
, OldFile(oldFile.GetFullPath())
, NewName(newName.c_str())
, DirsDeleted()
, FilesDeleted()
, DirsNotDeleted()
, FilesNotDeleted()
, Action(t4p::ExplorerModifyActionClass::RENAME_FILE)
, Success(success) {

}

t4p::ExplorerModifyEventClass::ExplorerModifyEventClass(int eventId,
															  const std::vector<wxFileName>& dirsDeleted, const std::vector<wxFileName>& filesDeleted,
															  const std::vector<wxFileName>& dirsNotDeleted, const std::vector<wxFileName>& filesNotDeleted,
															  bool success)
: wxEvent(eventId, t4p::EVENT_EXPLORER_MODIFY)
, OldFile()
, NewName()
, DirsDeleted()
, FilesDeleted()
, DirsNotDeleted()
, FilesNotDeleted()
, Action(t4p::ExplorerModifyActionClass::DELETE_FILES_DIRS)
, Success(success) {
	DirsDeleted = t4p::DeepCopyFileNames(dirsDeleted);
	DirsNotDeleted = t4p::DeepCopyFileNames(dirsNotDeleted);
	FilesDeleted = t4p::DeepCopyFileNames(filesDeleted);
	FilesNotDeleted = t4p::DeepCopyFileNames(filesNotDeleted);
}

wxFileName t4p::ExplorerModifyEventClass::GetParentDir() const {
	wxFileName parentDir;
	if (Action == t4p::ExplorerModifyActionClass::RENAME_FILE) {
		parentDir.AssignDir(OldFile.GetPath());
		return parentDir;
	}
	else if (!DirsDeleted.empty()) {
		parentDir.AssignDir(DirsDeleted[0].GetPath());
		parentDir.RemoveLastDir();
		return parentDir;
	}
	else if (!DirsNotDeleted.empty()) {
		parentDir.AssignDir(DirsNotDeleted[0].GetPath());
		parentDir.RemoveLastDir();
		return parentDir;
	}
	else if (!FilesDeleted.empty()) {
		parentDir.AssignDir(FilesDeleted[0].GetPath());
		return parentDir;
	}
	else if (!FilesNotDeleted.empty()) {
		parentDir.AssignDir(FilesNotDeleted[0].GetPath());
		return parentDir;
	}
	return parentDir;
}

wxEvent* t4p::ExplorerModifyEventClass::Clone() const {
	if (Action == t4p::ExplorerModifyActionClass::RENAME_FILE) {
		return new t4p::ExplorerModifyEventClass(
			GetId(), OldFile, NewName, Success
		);
	}
	return new t4p::ExplorerModifyEventClass(
		GetId(), DirsDeleted, FilesDeleted, DirsNotDeleted, FilesNotDeleted, Success
	);
}

const wxEventType t4p::EVENT_EXPLORER = wxNewEventType();
const wxEventType t4p::EVENT_EXPLORER_MODIFY = wxNewEventType();
const int t4p::ID_EXPLORER_MODIFY = wxNewId();
const int t4p::ID_EXPLORER_LIST_ACTION = wxNewId();

BEGIN_EVENT_TABLE(t4p::ExplorerFeatureClass, t4p::FeatureClass)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::ExplorerFeatureClass::OnAppPreferencesSaved)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_EXTERNALLY_UPDATED, t4p::ExplorerFeatureClass::OnAppPreferencesSaved)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::FileListingClass, wxEvtHandler)
	EVT_EXPLORER_COMPLETE(t4p::ID_EXPLORER_LIST_ACTION, t4p::FileListingClass::OnExplorerListComplete)
	EVT_EXPLORER_MODIFY_COMPLETE(t4p::ID_EXPLORER_MODIFY, t4p::FileListingClass::OnExplorerModifyComplete)
	EVT_FSWATCHER(wxID_ANY, t4p::FileListingClass::OnFsWatcher)
END_EVENT_TABLE()
