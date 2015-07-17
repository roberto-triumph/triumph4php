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
#ifndef T4P_EXPLORERFEATURECLASS_H
#define T4P_EXPLORERFEATURECLASS_H

#include <features/FeatureClass.h>
#include <actions/ActionClass.h>
#include <wx/imaglist.h>
#include <wx/fswatcher.h>

namespace t4p {

// forward declaration, defined below
class ExplorerEventClass;
class ExplorerModifyEventClass;

class FileListingClass : public wxEvtHandler {
	
public:

	wxFileName WorkingDir;
	std::vector<wxFileName> Files;
	std::vector<wxFileName> Dirs;
	int TotalFiles;
	int TotalSubDirs;
	
	FileListingClass(wxEvtHandler& handler);
	~FileListingClass();
	
	void StartRefresh(const wxFileName& dir, const std::vector<wxString>& filterExtensions, bool doHidden);
	
	void StartRename(const wxFileName& oldFile, const wxString& newName);
	
	void StartDelete(const std::vector<wxFileName>& dirs, const std::vector<wxFileName>& files);
		
	private:
	
	// when a file is added/remove update the panel
	void OnFsWatcher(wxFileSystemWatcherEvent& event);
	
	/**
	 * when the explorer action has finished fetching the files,
	 * tells the handler that the file listing is up-to-date
	 */
	void OnExplorerListComplete(t4p::ExplorerEventClass& event);

	/**
	 * handler for when a file modification (new or delete) was performed
	 */
	void OnExplorerModifyComplete(t4p::ExplorerModifyEventClass& event);

	/**
	 * this object gets its own background thread to
	 * read directories in the background
	 */
	t4p::RunningThreadsClass RunningThreads;

	/**
	 * this event handler gets notified when the list of files or
	 * directories has changed
	 */
	wxEvtHandler& Handler;
	
	/**
	 * we will watch for new/deleted files and update the panel automatically
	 * using a pointer because wxFileSystemWatcher::Remove() is buggy on MSW
	 * we will create a new instance on every directory refresh
	 * this class will own the pointer
	 */
	wxFileSystemWatcher* Watcher;
	
	DECLARE_EVENT_TABLE()
};

/**
 * The explorer feature shows the user the files and directories
 * for all defined projects.  Additionally, it can display folders
 * and files for any path in the file system.
 */
class ExplorerFeatureClass : public t4p::FeatureClass {

public:
	
	/**
	 * executable of the operating system file manager
	 */
	wxFileName FileManagerExecutable;

	/**
	 * executable of the operating system shell
	 */
	wxFileName ShellExecutable;

	ExplorerFeatureClass(t4p::AppClass& app);

	void LoadPreferences(wxConfigBase* config);
	
	/**
	 * @return all of the enabled sources
	 */
	std::vector<t4p::SourceClass> EnabledSources() const;
	
	/**
	 * @return all of the enabled source directories
	 */
	std::vector<wxFileName> EnabledSourceDirectories() const;
	
private:

	/**
	 * when projects list is updated, we need to update our sources list
	 */
	void OnAppPreferencesSaved(wxCommandEvent& event);
	
	DECLARE_EVENT_TABLE()
};

extern const wxEventType EVENT_EXPLORER;

typedef void (wxEvtHandler::*ExplorerEventClassFunction)(t4p::ExplorerEventClass&);

#define EVT_EXPLORER_COMPLETE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_EXPLORER, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( ExplorerEventClassFunction, & fn ), (wxObject *) NULL ),


/**
 * results of a directory listing
 */
class ExplorerEventClass : public wxEvent {

public:

	/**
	 *  the dir that was searched
	 */
	wxFileName Dir;

	/**
	 * the files that are in dir
	 */
	std::vector<wxFileName> Files;

	/**
	 * the sub-directories that are in dir
	 */
	std::vector<wxFileName> SubDirs;

	/**
	 * filled in when an error occurs
	 */
	wxString Error;

	/**
	 * total number of files in the directory, includes files not
	 * shown due to extension filters
	 */
	int TotalFiles;

	/**
	 * total number of sub directories in the directory
	 */
	int TotalSubDirs;

	ExplorerEventClass(int eventId, const wxFileName& dir, const std::vector<wxFileName>& files, 
		const std::vector<wxFileName>& subDirs, const wxString& error, int totalFiles, int totalSubDirs);

	wxEvent* Clone() const;
};


/**
 * this action list files by reading the file system. the main point of this
 * action is to get files that we have not indexed yet for whatever reason.
 */
class ExplorerFileSystemActionClass : public t4p::ActionClass {

public:
	ExplorerFileSystemActionClass(t4p::RunningThreadsClass& runningThreads, int eventId);

	/**
	 * starts an action to read the files from the given directory.
	 * @param dir the directory to list
	 * @param extension list of extensions to show.  If empty all files are shown
	 * @param doHidden if TRUE hidden files/dirs are shown
	 */
	void Directory(const wxFileName& dir, const std::vector<wxString>& extensions, bool doHidden);

	wxString GetLabel() const;

protected:

	void BackgroundWork();
private:

	wxFileName Dir;

	/** 
	 * extensions to show.  if empty, we show all files
	 */
	std::vector<wxString> Extensions;

	bool DoHidden;

	/**
	 * @return bool TRUE if the given name matches any of the wildcards
	 */
	bool MatchesWildcards(const wxString& fileName);
};

/**
 * This action is used to modify the file system: remove files, remove
 * directories, and rename files / folders.
 *
 * The action will post EVENT_EXPLORER_MODIFY events
 */
class ExplorerModifyActionClass : public t4p::ActionClass {

public:

	enum Actions {
		NONE,
		DELETE_FILES_DIRS,
		RENAME_FILE
	};

	ExplorerModifyActionClass(t4p::RunningThreadsClass& runningThreads, int eventId);

	/**
	 * @param dirs the directories to be deleted (recursively)
	 * @param files the files to be deleted
	 */
	void SetFilesToRemove(const std::vector<wxFileName>& dirs, const std::vector<wxFileName>& files);

	/**
	 * @param file the file to be renamed.  thie can also be a
	  *       directory
	 */
	void SetFileToRename(const wxFileName& file, const wxString& newName);

protected:

	void BackgroundWork();

	wxString GetLabel() const;

	/**
	 * what type of modification to perform
	 */
	Actions Action;

	/**
	 * set when Action == DELETE
	 */
	std::vector<wxFileName> Dirs;

	/**
	 * set when Action == DELETE
	 */
	std::vector<wxFileName> Files;

	/**
	 * set when Action == RENAME
	 */
	wxFileName OldFile;

	/**
	 * set when Action == RENAME
	 */
	wxString NewName;
};

extern const wxEventType EVENT_EXPLORER_MODIFY;
extern const int ID_EXPLORER_LIST_ACTION;
extern const int ID_EXPLORER_MODIFY;

typedef void (wxEvtHandler::*ExplorerModifyEventClassFunction)(t4p::ExplorerModifyEventClass&);

#define EVT_EXPLORER_MODIFY_COMPLETE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_EXPLORER_MODIFY, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( ExplorerModifyEventClassFunction, & fn ), (wxObject *) NULL ),


/**
 * results of a file system modification (delete / rename)
 */
class ExplorerModifyEventClass : public wxEvent {

public:

	/**
	 * name of the item renamed
	 */
	wxFileName OldFile;

	/**
	 * the new name, if the modification was a rename
	 */
	wxString NewName;

	/**
	 * the directories that were successfully deleted
	 */
	std::vector<wxFileName> DirsDeleted;

	/**
	 * the files that were successfully deleted
	 */
	std::vector<wxFileName> FilesDeleted;

	/**
	 * the directories that could not be deleted
	 */
	std::vector<wxFileName> DirsNotDeleted;

	/**
	 * the files that were could not be deleted
	 */
	std::vector<wxFileName> FilesNotDeleted;

	/**
	 * the type of modification to perform
	 */
	t4p::ExplorerModifyActionClass::Actions Action;

	/**
	 * TRUE if the delete/rename was successful.
	 */
	bool Success;

	/**
	 * create a rename event
	 */
	ExplorerModifyEventClass(int eventId, const wxFileName& oldFile, 
		const wxString& newName, bool success);

	/**
	 * create a delete event
	 */
	ExplorerModifyEventClass(int eventId, 
		const std::vector<wxFileName>& dirsDeleted, const std::vector<wxFileName>& filesDeleted, 
		const std::vector<wxFileName>& dirsNotDeleted, const std::vector<wxFileName>& filesNotDeleted, 
		bool success);

	/**
	 * @return the directory where the changes took place
	 */
	wxFileName GetParentDir() const;

	wxEvent* Clone() const;
};

}

#endif