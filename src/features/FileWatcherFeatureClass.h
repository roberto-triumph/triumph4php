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
#ifndef SRC_FEATURES_FILEWATCHERFEATURECLASS_H_
#define SRC_FEATURES_FILEWATCHERFEATURECLASS_H_

#include <wx/fswatcher.h>
#include <map>
#include <vector>
#include "actions/ActionClass.h"
#include "features/FeatureClass.h"

namespace t4p {
// forward declaration, defined below
class VolumeListEventClass;

/**
 * A feature that monitors files for changes and propagates
 * events to the rest of the editor.
 *
 * Only files that are part of enabled projects are watched.
 * Files will be monitored using wxWigets
 * wxFileSystemWatcher, which efficiently notifies our application when a
 * file inside a project changes.
 * For now, projects are watched only if they reside in a local volume.
 * projects in a network share will not be watched because
 * underlying file system does not propagate events properly. Take, for
 * example, a linux network share that is mounted
 * in a MSW machine. In this case, fs events that happen in a subdirectory
 * of a watched directory are not propagated to MSW, even though
 * we tell MSW to watch for directory changes recursively.
 *
 * Note that we do not directly propagate wxFileSystemWatcher events
 * to the rest of the app as they come in. This feature will buffer
 * FS events a bit, wait until events stop coming in, and then notify
 * the rest of the app. This is because a file save may generate
 * more than 1 FS event depending on how it is saved by an application,
 * for example, a file may be flushed multiple times to disk. Also, a
 * deep directory recursive copy will generate many FS events; this feature
 * will "collapse" these into a single NEW_DIR event.
 */
class FileWatcherFeatureClass : public t4p::FeatureClass {
	public:
	/**
	 * Flag to turn this feature on or off, we want this so that in case
	 * there are bugs with this feature users can turn this feature off.
	 */
	bool Enabled;

	FileWatcherFeatureClass(t4p::AppClass& app);

	void LoadPreferences(wxConfigBase* config);

	/**
	 * @param fullPath the full path to add to the list of string of full paths of
	 *        files that are opened in this editor. This feature
	 *        will send different events based on whether or
	 *        not a file is open (EVENT_APP_FILE_EXTERNALLY_MODIFIED)
	 *        This method should be called every time the user opens a file
	 */
	void TrackOpenedFile(wxString fullPath);

	/**
	 * @param fullPath the full path to remove from the list of string of full paths of
	 *        files that are opened in this editor. This feature
	 *        will send different events based on whether or
	 *        not a file is open (EVENT_APP_FILE_EXTERNALLY_MODIFIED)
	 *        This method should be called every time the user closes a file.
	 */
	void UntrackOpenedFile(wxString fullPath);

	private:
	/**
	 * when the app starts then start the watches
	 */
	void OnAppReady(wxCommandEvent& event);

	/**
	 * when the app stops then stop the watches
	 */
	void OnAppExit(wxCommandEvent& event);

	/**
	 * when the timer is up then handle the files that the fs watcher notified us
	 * that were changed
	 */
	void OnTimer(wxTimerEvent& event);

	/**
	 * special handling for files that are not open.
	 */
	void HandleNonOpenedFiles(const std::vector<wxString>& openedFiles, std::map<wxString, wxString>& pathsRenamed);

	/**
	 * when a file has been externally modified / added / deleted we need to
	 * upate the tag cache.
	 */
	void OnFsWatcher(wxFileSystemWatcherEvent& event);

	/**
	 * when projects have been enabled/disabled we need to watch the newly enabled projects
	 */
	void OnPreferencesSaved(wxCommandEvent& event);

	/**
	 * start watching all enabled projects' source directories.
	 */
	void StartWatch();

	/**
	 * when we encounter a watch error, it may be because one of the projects' sources has
	 * been deleted. Prompt the user on what to do.
	 */
	void HandleWatchError();

	/**
	 * we will look for remote (network drives).  if any source directories
	 * are in network drives, we will not add them to the watch, as watches on network
	 * directories fail to notify of file changes inside of sub-directories.
	 */
	void OnVolumeListComplete(t4p::VolumeListEventClass& event);

	/**
	 * timer that we will use to see if file system watcher events have been captured. in this timer's
	 * event handler we will process the file system watcher events.
	 */
	wxTimer Timer;

	/**
	 * object that will notify us when a file has been modified outside the editor.
	 * This class will own the pointer
	 * on MSW, wxFileSystemWatcher.RemoveAll does not actually remove the old
	 * watches.
	 * that is why we are using a pointer; deleting the object does remove the
	 * old watches
	 * see http://trac.wxwidgets.org/ticket/12847
	 */
	wxFileSystemWatcher* FsWatcher;

	/**
	 * files that were recently modified or deleted outside the editor.  We accumulate the
	 * files that we were notified of by FS watcher, then after 1 sec we
	 * actually tell the editor of the files.  we do this because we may get several
	 * events at once depending on how files are externally modified.
	 *
	 * new files (files that were created outside the editor) will be in the modified
	 * list.
	 * the modified list will only contain files (not directories)
	 */
	std::map<wxString, int> FilesExternallyCreated;
	std::map<wxString, int> FilesExternallyModified;
	std::map<wxString, int> FilesExternallyDeleted;
	std::map<wxString, int> DirsExternallyCreated;
	std::map<wxString, int> DirsExternallyModified;
	std::map<wxString, int> DirsExternallyDeleted;

	/**
	 * the paths that were recently modified or deleted outside the editor
	 * these paths could be either files or directories, we don't know
	 * what they are since that info is not part of wxFileSystemWatcherEvent
	 * This is a map because we may receive multiple events for a single
	 * file or dir
	 */
	std::map<wxString, int> PathsExternallyCreated;
	std::map<wxString, int> PathsExternallyModified;
	std::map<wxString, int> PathsExternallyDeleted;

	// key is from path, value is to path
	std::map<wxString, wxString> PathsExternallyRenamed;

	/**
	 * the last time that we got an event from wxFileSystemWatcher.
	 * we will use this to trigger our app events only after some time
	 * has passed; if a big directory is being copied we want to wait until
	 * all of the directory has been copied before we attempt to update our
	 * PHP tags.
	 */
	wxDateTime LastWatcherEventTime;

	/**
	 *  list of string of full paths of
	 *  files that are opened in this editor. This feature
	 *  will send different events based on whether or
	 *  not a file is open (EVENT_APP_FILE_EXTERNALLY_MODIFIED)
	 */
	std::vector<wxString> OpenedFiles;

	/**
	 * will be set to TRUE if the watcher saw an error event.  We may get
	 * error events when a project source dir has been deleted. In this case,
	 * we want to prompt the user on what action to take
	 */
	bool IsWatchError;

	DECLARE_EVENT_TABLE()
};

/**
 * A class that lists the LOCAL file system volumes. This
 * is used only on windows in order to find the volumes
 * on a system; we determine if a source directory is located in a network
 * share and if so we don't watch it for changes (because we can't,
 * file changes in network drives don't work properly when
 * watching entire directory structures). This action will
 * return the local, writable drives only, meaning that any project
 * not one of the local writable drives means that the project
 * is in a remote drive or removable media.
 *
 * This action generates event of type t4p::EVENT_ACTION_VOLUME_LIST
 */
class VolumeListActionClass : public t4p::ActionClass {
	public:
	VolumeListActionClass(t4p::RunningThreadsClass& runningThreads, int eventId);

	protected:
	void BackgroundWork();

	void DoCancel();

	wxString GetLabel() const;
};

extern const wxEventType EVENT_ACTION_VOLUME_LIST;

typedef void (wxEvtHandler::*VolumeListEventClassFunction)(VolumeListEventClass&);

#define EVT_ACTION_VOLUME_LIST(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_ACTION_VOLUME_LIST, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent(VolumeListEventClassFunction, & fn), (wxObject *) NULL),

class VolumeListEventClass : public wxEvent {
	public:
	/**
	 * List of the LOCAL volumes that are mounted. this will
	 * not include remote drives
	 */
	std::vector<wxString> LocalVolumes;

	VolumeListEventClass(int id, const std::vector<wxString>& allVolumes);


	wxEvent* Clone() const;
};
}  // namespace t4p

#endif  // SRC_FEATURES_FILEWATCHERFEATURECLASS_H_
