/**
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
 */
#include "features/FileWatcherFeatureClass.h"
#include <wx/platinfo.h>
#include <wx/volume.h>
#include <algorithm>
#include <map>
#include <vector>
#include "globals/Errors.h"
#include "globals/Events.h"
#include "Triumph.h"

static int ID_FILE_MODIFIED_CHECK = wxNewId();

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
static void CollapseDirsFiles(t4p::TagCacheClass& tagCache, std::map<wxString, int>& paths,
                              std::map<wxString, int>& collapsedDirs, std::map<wxString, int>& collapsedFiles) {
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
        } else if (wxFileName::FileExists(*path) || tagCache.HasFullPath(*path)) {
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

t4p::FileWatcherFeatureClass::FileWatcherFeatureClass(t4p::AppClass& app)
    : FeatureClass(app)
    , Enabled(true)
    , Timer(this, ID_FILE_MODIFIED_CHECK)
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
    , LastWatcherEventTime()
    , OpenedFiles()
    , IsWatchError(false) {
    FsWatcher = NULL;
    LastWatcherEventTime = wxDateTime::Now();

    // by default, disable on mac os x because wxFileSystemWatcher
    // users kqueue for fs notifications, and this requires an open
    // file handle for each patch being watched.
    wxPlatformInfo info;
    if (info.GetOperatingSystemId() == wxOS_MAC_OSX_DARWIN) {
        Enabled = false;
    }
}

void t4p::FileWatcherFeatureClass::LoadPreferences(wxConfigBase* config) {
    config->Read(wxT("FileWatcher/Enabled"), &Enabled, Enabled);
}

void t4p::FileWatcherFeatureClass::OnAppReady(wxCommandEvent& event) {
#ifdef __WXMSW__

    // on windows, we check for network drives
    t4p::VolumeListActionClass* volumeAction = new t4p::VolumeListActionClass(App.RunningThreads, wxID_ANY);
    App.RunningThreads.Queue(volumeAction);
#else

    // add the enabled projects to the watch list
    StartWatch();
#endif
    Timer.Start(250, wxTIMER_CONTINUOUS);
}

void t4p::FileWatcherFeatureClass::OnAppExit(wxCommandEvent& event) {
    Timer.Stop();

    // unregister ourselves as the event handler from watcher
    if (FsWatcher) {
        FsWatcher->SetOwner(NULL);
        delete FsWatcher;
        FsWatcher = NULL;
    }
}

void t4p::FileWatcherFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {
    wxConfigBase* config = wxConfig::Get();
    config->Write(wxT("FileWatcher/Enabled"), Enabled);

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

void t4p::FileWatcherFeatureClass::StartWatch() {
    IsWatchError = false;
    if (!Enabled) {
        return;
    }
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
        bool doAdd = App.Globals.IsInLocalVolume(sourceDir);
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

void t4p::FileWatcherFeatureClass::OnVolumeListComplete(t4p::VolumeListEventClass& event) {
    App.Globals.LocalVolumes = event.LocalVolumes;
    StartWatch();
}

void t4p::FileWatcherFeatureClass::OnTimer(wxTimerEvent& event) {
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
        } else {
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
            PathsExternallyModified[rename->first] = 1;
            std::map<wxString, wxString>::iterator toErase = rename;
            rename++;
            pathsRenamed.erase(toErase->first);
        } else {
            ++rename;
        }
    }

    HandleNonOpenedFiles(OpenedFiles, pathsRenamed);
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


void t4p::FileWatcherFeatureClass::HandleNonOpenedFiles(const std::vector<wxString>& openedFiles, std::map<wxString, wxString>& pathsRenamed) {
    std::map<wxString, int>::iterator f;
    for (f = FilesExternallyModified.begin(); f != FilesExternallyModified.end(); ++f) {
        wxString fullPath = f->first;
        bool isOpened = std::find(openedFiles.begin(), openedFiles.end(), fullPath) != openedFiles.end();
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
        bool isOpened = std::find(openedFiles.begin(), openedFiles.end(), fullPath) != openedFiles.end();
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
        } else {
            t4p::RenameEventClass renameEvt(t4p::EVENT_APP_FILE_RENAMED, pair->first, pair->second);
            App.EventSink.Publish(renameEvt);
        }
    }
}

void t4p::FileWatcherFeatureClass::OnFsWatcher(wxFileSystemWatcherEvent& event) {
    LastWatcherEventTime = wxDateTime::Now();
    wxString path = event.GetPath().GetFullPath();
    wxFileName fileName = event.GetPath();
    if (wxFSW_EVENT_MODIFY == event.GetChangeType()) {
        PathsExternallyModified[path] = 1;
    } else if (wxFSW_EVENT_CREATE == event.GetChangeType()) {
        PathsExternallyCreated[path] = 1;
    } else if (wxFSW_EVENT_DELETE == event.GetChangeType()) {
        PathsExternallyDeleted[path] = 1;
    } else if (wxFSW_EVENT_RENAME == event.GetChangeType()) {
        PathsExternallyRenamed[path] = event.GetNewPath().GetFullPath();
    } else if (wxFSW_EVENT_WARNING == event.GetChangeType()) {
        // too many files being added/removed
        // this is probably a big directory being added / removed
        // hopefully the root directory is caught
    } else if (wxFSW_EVENT_ERROR == event.GetChangeType()) {
        // in MSW, an error event could be due to the watched directoty being deleted / renamed.
        // in this case, we need to restart the watch
        IsWatchError = true;
        wxASSERT_MSG(false, event.GetErrorDescription());
    }
}

void t4p::FileWatcherFeatureClass::HandleWatchError() {
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
                           "and remove the source directories.\n");
        msg += dirsDeleted;
        msg = wxGetTranslation(msg);
        wxMessageBox(msg, _("Warning"), wxICON_WARNING | wxOK, NULL);
    }

    StartWatch();
}

void t4p::FileWatcherFeatureClass::TrackOpenedFile(wxString fullPath) {
    OpenedFiles.push_back(fullPath);
}

void t4p::FileWatcherFeatureClass::UntrackOpenedFile(wxString fullPath) {
    std::vector<wxString>::iterator it = std::find(OpenedFiles.begin(), OpenedFiles.end(), fullPath);

    while (it != OpenedFiles.end()) {
        it = OpenedFiles.erase(it);
        it = std::find(it, OpenedFiles.end(), fullPath);
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

BEGIN_EVENT_TABLE(t4p::FileWatcherFeatureClass, t4p::FeatureClass)
    EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_READY, t4p::FileWatcherFeatureClass::OnAppReady)
    EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_EXIT, t4p::FileWatcherFeatureClass::OnAppExit)
    EVT_TIMER(ID_FILE_MODIFIED_CHECK, t4p::FileWatcherFeatureClass::OnTimer)
    EVT_FSWATCHER(wxID_ANY, t4p::FileWatcherFeatureClass::OnFsWatcher)
    EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::FileWatcherFeatureClass::OnPreferencesSaved)
    EVT_ACTION_VOLUME_LIST(wxID_ANY, t4p::FileWatcherFeatureClass::OnVolumeListComplete)
END_EVENT_TABLE()
