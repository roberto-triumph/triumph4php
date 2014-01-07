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
#ifndef __MVCEDITOR_EXPLORERFEATURECLASS_H__
#define __MVCEDITOR_EXPLORERFEATURECLASS_H__

#include <features/FeatureClass.h>
#include <features/wxformbuilder/ExplorerFeatureForms.h>
#include <actions/ActionClass.h>
#include <wx/imaglist.h>
#include <wx/fswatcher.h>

namespace mvceditor {

// forward declaration, defined below
class ExplorerEventClass;
class ExplorerModifyEventClass;
/**
 * The explorer feature shows the user the files and directories
 * for all defined projects.  Additionally, it can display folders
 * and files for any path in the file system.
 */
class ExplorerFeatureClass : public mvceditor::FeatureClass {

public:
	
	/**
	 * executable of the operating system file manager
	 */
	wxFileName FileManagerExecutable;

	/**
	 * executable of the operating system shell
	 */
	wxFileName ShellExecutable;

	ExplorerFeatureClass(mvceditor::AppClass& app);

	/**
	 * Add menu items to the view menu
	 */
	void AddViewMenuItems(wxMenu* viewMenu);

	/**
	 * Add a new toolbar for explorer items only
	 */
	void AddWindows();

	/**
	 * Add keyboard shortcuts
	 */
	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);

	void LoadPreferences(wxConfigBase* config);

	void AddPreferenceWindow(wxBookCtrlBase* parent);
	
private:

	/**
	 * Handler for the Project .. Explore Open File menu 
	 * @param wxCommandEvent& event 
	 */
	void OnProjectExplore(wxCommandEvent& event);

	/**
	 * Handler for the Project .. Explore Open File menu 
	 * @param wxCommandEvent& event 
	 */
	void OnProjectExploreOpenFile(wxCommandEvent& event);
	
	/**
	 * when the explorer action has finished fetching the files,
	 * display them. this handler will refresh both the left and right panels.
	 */
	void OnExplorerListComplete(mvceditor::ExplorerEventClass& event);
	
	/**
	 * when the explorer tool button is clicked show any open projects
	 * so that the user can choose which project to explore
	 */
	void OnExplorerToolDropDown(wxAuiToolBarEvent& event);
	
	/**
	 * menu handler for all of the project source dir menu items
	 * when one of these items is clicked the explorer pane will be
	 * set to the source dir
	 */
	void OnExplorerProjectMenu(wxCommandEvent& event);

	/**
	 * when projects list is updated, we need to update our sources list
	 */
	void OnAppPreferencesSaved(wxCommandEvent& event);

	/**
	 * when a new project is created, show it in the explorer panel
	 */
	void OnAppProjectCreated(wxCommandEvent& event);
	
	/**
	 * toolbar to hold the explorer buttons
	 */
	wxAuiToolBar* ExplorerToolBar;
	
	/**
	 * the source directories shown in the project menu
	 */
	std::vector<mvceditor::SourceClass> SourceDirs;

	DECLARE_EVENT_TABLE()
};

class ModalExplorerPanelClass : public ModalExplorerGeneratedPanelClass {
public:
	ModalExplorerPanelClass(wxWindow* parent, int id, mvceditor::ExplorerFeatureClass& feature);
	~ModalExplorerPanelClass();

	void RefreshDir(const wxFileName& dir);
	
	void ShowDir(const wxFileName& dir, const std::vector<wxFileName>& files, const std::vector<wxFileName>& dirs, int totalFiles, int totalSubDirs);

	void FillSourcesList(const std::vector<wxFileName>& sourceDirs);

	void FocusOnSourcesList();

private:

	/**
	 * the directory being shown on the right hand side
	 */
	wxFileName CurrentListDir;
	
	/**
	 * will be owned by the list control
	 */
	wxImageList* FilesImageList;

	/**
	 * will be owned by the list control
	 */
	wxImageList* SourcesImageList;

	/**
	 * we will watch for new/deleted files and update the panel automatically
	 * using a pointer because wxFileSystemWatcher::Remove() is buggy on MSW
	 * we will create a new instance on every directory change
	 * this class will own the pointer
	 */
	wxFileSystemWatcher* Watcher;

	/**
	 * to get projects list and tag cache
	 */
	mvceditor::ExplorerFeatureClass& Feature;
	
	/**
	 * this object gets its own background thread to
	 * read directories in the background
	 */
	mvceditor::RunningThreadsClass RunningThreads;

	/**
	 * the currently selected filter menu item
	 */
	int FilterChoice;

	enum ListImages {
		LIST_FOLDER,
		LIST_PARENT_FOLDER,
		LIST_FILE_PHP,
		LIST_FILE_SQL,
		LIST_FILE_CSS,
		LIST_FILE_JS,
		LIST_FILE_TEXT,
		LIST_FILE_OTHER
	};

	enum SourceImages {
		SOURCE_FOLDER
	};

	// events handlers for the files list
	void OnListItemActivated(wxListEvent& event);
	void OnListItemRightClick(wxListEvent& event);
	void OnListEndLabelEdit(wxListEvent& event);
	void OnListRightDown(wxMouseEvent& event);

	// event handlers for the context menu on the files list
	void OnListMenuOpen(wxCommandEvent& event);
	void OnListMenuRename(wxCommandEvent& event);
	void OnListMenuDelete(wxCommandEvent& event);
	void OnListMenuCreateNew(wxCommandEvent& event);
	void OnListMenuCreateDirectory(wxCommandEvent& event);
	void OnListMenuShell(wxCommandEvent& event);
	void OnListMenuFileManager(wxCommandEvent& event);
	void OnExplorerModifyComplete(mvceditor::ExplorerModifyEventClass& event);
	void OnListKeyDown(wxKeyEvent& event);

	// event handler for the combo box
	void OnDirectoryEnter(wxCommandEvent& event);

	// event handlers for the sources list
	void OnSourceActivated(wxListEvent& event);

	// handlers for the buttons
	void OnParentButtonClick(wxCommandEvent& event);
	void OnFilterButtonLeftDown(wxMouseEvent& event);
	void OnFilterMenuCheck(wxCommandEvent& event);
	void OnRefreshClick(wxCommandEvent& event);

	// when a file is added/remove update the panel
	void OnFsWatcher(wxFileSystemWatcherEvent& event);

	std::vector<wxString> FilterFileExtensions();

	void ListFiles(const std::vector<wxFileName>& files);

	void ListDirectories(const std::vector<wxFileName>& dirs);

	bool OpenIfListFile(const wxString& text);

	int ListImageId(const wxFileName& fileName);

	DECLARE_EVENT_TABLE()
};

extern const wxEventType EVENT_EXPLORER;

typedef void (wxEvtHandler::*ExplorerEventClassFunction)(mvceditor::ExplorerEventClass&);

#define EVT_EXPLORER_COMPLETE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_EXPLORER, id, -1, \
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
class ExplorerFileSystemActionClass : public mvceditor::ActionClass {

public:
	ExplorerFileSystemActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);

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
class ExplorerModifyActionClass : public mvceditor::ActionClass {

public:

	enum Actions {
		NONE,
		DELETE_FILES_DIRS,
		RENAME_FILE
	};

	ExplorerModifyActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);

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

typedef void (wxEvtHandler::*ExplorerModifyEventClassFunction)(mvceditor::ExplorerModifyEventClass&);

#define EVT_EXPLORER_MODIFY_COMPLETE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_EXPLORER_MODIFY, id, -1, \
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
	mvceditor::ExplorerModifyActionClass::Actions Action;

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

/**
 * Panel that shows the explorer options: locations of Operation System shell, file manager
 */
class ExplorerOptionsPanelClass : public ExplorerOptionsGeneratedPanelClass {

public:

	ExplorerOptionsPanelClass(wxWindow* parent, int id, mvceditor::ExplorerFeatureClass& feature);	

private:

	mvceditor::ExplorerFeatureClass& Feature;
};

}

#endif