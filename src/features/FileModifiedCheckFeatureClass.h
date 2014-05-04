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
#ifndef __T4P_FILEMODIFIEDCHECKFEATURECLASS_H__
#define __T4P_FILEMODIFIEDCHECKFEATURECLASS_H__

#include <features/FeatureClass.h>
#include <actions/FileModifiedCheckActionClass.h>
#include <wx/fswatcher.h>

namespace t4p {

// forward declaration, defined below
class VolumeListEventClass;

/**
 * A feature that checks to see if any files that are currently
 * opened have been externally modified and / or deleted. If so, we
 * will prompt the reader to save and or reload them.
 * 
 * Files from any enabled projects will be monitored using wxWigets
 * wxFileSystemWatcher, which efficiently notifies our application when a 
 * file inside a project changes.  In some cases, we will use "polling"
 * ie. checking for file modified times on files. We use polling
 * when a file that is not part of a project is opened. We also use polling
 * when a file that is not in a network drive is opened.
 */
class FileModifiedCheckFeatureClass : public t4p::FeatureClass {

public:

	FileModifiedCheckFeatureClass(t4p::AppClass& app);


private:

	/**
	 * when the app starts then start the timer
	 */
	void OnAppReady(wxCommandEvent& event);

	/**
	 * when the app stops then stop the timer
	 */
	void OnAppExit(wxCommandEvent& event);

	/**
	 * In case the file that was opened is not part of a project
	 * OR it is in a network drive, create a watch for it
	 */
	void OnAppFileOpened(t4p::CodeControlEventClass& event);

	/**
	 * In case the file that was closed and is not part of a project
	 * OR it is in a network drive, remove the watch we created for it
	 */
	void OnAppFileClosed(t4p::CodeControlEventClass& event);
	
	/**
	 * when the timer is up then handle the files that the fs watcher notified us 
	 * that were changed
	 */
	void OnTimer(wxTimerEvent& event);

	/**
	 * handle the files that are not part of the watched directories
	 * ie we will check the file modified times
	 */
	void OpenedCodeControlCheck();

	/**
	 * special handling for files that are open. For open files that were externally modified
	 * we will prompt the user to take action
	 */
	void HandleOpenedFiles(std::map<wxString, t4p::CodeControlClass*>& openedFiles, std::map<wxString, wxString>& pathsRenamed);

	/**
	 * special handling for files that are not open.
	 */
	void HandleNonOpenedFiles(std::map<wxString, t4p::CodeControlClass*>& openedFiles, std::map<wxString, wxString>& pathsRenamed);

	/**
	 * prompt the user to reload modified files
	 */
	void FilesModifiedPrompt(std::map<wxString, t4p::CodeControlClass*>& filesToPrompt);

	/**
	 * prompt the user to save the deleted files
	 */
	void FilesDeletedPrompt(std::map<wxString, t4p::CodeControlClass*>& openedFiles, std::map<wxString, int>& deletedFiles);

	/**
	 * prompt the user to open or close the renamed files
	 */
	void FilesRenamedPrompt(std::map<wxString, t4p::CodeControlClass*>& openedFiles, std::map<wxString, wxString>& pathsRenamed);

	/**
	 * when a file has been externally modified / added / deleted we need to
	 * upate the tag cache
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
	 * when the user puts this app in the foreground, check for
	 * file modifications.  maybe the user went to another editor
	 * and modified one of the files that is opened in triumph. Note that we
	 * will check the file modified times in a background thread
	 */
	void OnActivateApp(wxCommandEvent& event);

	/**
	 * After we check the file modified times in a background thread this
	 * method gets called. here we will prompt the user to reload/close the
	 * files that were modified outside of triumph
	 */
	void OnFileModifiedPollComplete(t4p::FilesModifiedEventClass& event);

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
	 * to check the modified time of the opened files that we poll (files outside
	 * watched directories, or files in network drives).
	 * 
	 * Note that we don't continuously poll, we only poll the files when 
	 * the user has re-activated the app.  This is because the user might
	 * have gone to another app to change the file (ie. went to a terminal
	 * and performed an svn up)
	 * 
	 * This is how most editors check for file modifications
	 */
	std::vector<wxString> FilesToPoll;

	/**
	 * the last time that we got an event from wxFileSystemWatcher.
	 * we will use this to trigger our app events only after some time
	 * has passed; if a big directory is being copied we want to wait until
	 * all of the directory has been copied before we attempt to update our
	 * PHP tags.
	 */
	wxDateTime LastWatcherEventTime;
	
	/**
	 * will be set to TRUE if the watcher saw an error event.  We may get 
	 * error events when a project source dir has been deleted. In this case, 
	 * we want to prompt the user on what action to take
	 */
	bool IsWatchError;
	
	/**
	 * flag to prevent multiple modified dialogs during activate app event.
	 * needed for linux
	 */
	bool JustReactivated;
	
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
    wxStaticCastEvent( VolumeListEventClassFunction, & fn ), (wxObject *) NULL ),

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

}

#endif