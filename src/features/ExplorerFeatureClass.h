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

	ExplorerFeatureClass(mvceditor::AppClass& app);

	/**
	 * Add menu items to the file menu
	 */
	void AddFileMenuItems(wxMenu* fileMenu);

	/**
	 * Add items to the toolbar
	 */
	void AddToolBarItems(wxAuiToolBar* toolbar);

	/**
	 * Add keyboard shortcuts
	 */
	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);

private:

	/**
	 * Handler for the Project .. Explore menu 
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
	 * when the explorer action has finished fetching the files,
	 * display them. this handler will only refresh the right panel.
	 */
	void OnExplorerReportComplete(mvceditor::ExplorerEventClass& event);

	DECLARE_EVENT_TABLE()
};

class ModalExplorerPanelClass : public ModalExplorerGeneratedPanel {
public:
	ModalExplorerPanelClass(wxWindow* parent, int id, mvceditor::AppClass& app);
	~ModalExplorerPanelClass();

	void RefreshDir(const wxFileName& dir);
	
	void ShowDir(const wxFileName& dir, const std::vector<wxFileName>& files, const std::vector<wxFileName>& dirs, int totalFiles, int totalSubDirs);

	void ShowReport(const wxFileName& dir, const std::vector<wxFileName>& files, const std::vector<wxFileName>& dirs, int totalFiles, int totalSubDirs);

private:

	/**
	 * the directory being shown on the left hand side
	 */
	wxFileName CurrentListDir;

	/**
	 * the directory being shown on the right hand side
	 */
	wxFileName CurrentReportDir;
	
	/**
	 * will be owned by the list control
	 */
	wxImageList* ListImageList;

	/**
	 * will be owned by the list control
	 */
	wxImageList* ReportImageList;

	/**
	 * to get projects list and tag cache
	 */
	mvceditor::AppClass& App;

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
		LIST_FILE_TEXT,
		LIST_FILE_OTHER
	};

	enum ReportImages {
		REPORT_FILE_PHP,
		REPORT_DIR,
		REPORT_FILE_SQL,
		REPORT_FILE_CSS,
		REPORT_FILE_TEXT,
		REPORT_FILE_OTHER
	};

	// events handlers for the left lsit
	void OnListItemSelected(wxListEvent& event);
	void OnListItemActivated(wxListEvent& event);
	void OnListItemRightClick(wxListEvent& event);
	void OnListEndLabelEdit(wxListEvent& event);

	// event handlers for the context menu on the left files list
	void OnListMenuOpen(wxCommandEvent& event);
	void OnListMenuRename(wxCommandEvent& event);
	void OnListMenuDelete(wxCommandEvent& event);
	void OnExplorerModifyComplete(mvceditor::ExplorerModifyEventClass& event);
	void OnListKeyDown(wxKeyEvent& event);

	// event handler for the right list
	void OnReportItemActivated(wxListEvent& event);

	// event handler for the combo box
	void OnDirectoryEnter(wxCommandEvent& event);

	void OnParentButtonClick(wxCommandEvent& event);

	void OnFilterButtonLeftDown(wxMouseEvent& event);

	void OnFilterMenuCheck(wxCommandEvent& event);

	std::vector<wxString> FilterFileExtensions();

	void ListFiles(const std::vector<wxFileName>& files);

	void ListDirectories(const std::vector<wxFileName>& dirs);

	bool OpenIfListFile(const wxString& text);

	bool OpenIfReportFile(const wxString& text);

	int ListImageId(const wxFileName& fileName);
	int ReportImageId(const wxFileName& fileName);

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
		DELETE_FILE,
		DELETE_DIRECTORY,
		RENAME
	};

	ExplorerModifyActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);

	/**
	 * @param dir the directory to be deleted (recursively)
	 */
	void SetDirToRemove(const wxFileName& dir);

	/**
	 * @param file the directory to be deleted
	 */
	void SetFileToRemove(const wxFileName& file);

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
	 * set when Action == DELETE_DIRECTORY
	 */
	wxFileName Dir;

	/**
	 * set when Action == DELETE_FILE or RENAME
	 */
	wxFileName File;

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
	 * the dir that where the modification took place
	 * ie the parent dir
	 */
	wxFileName ParentDir;

	/**
	 * name of the item renamed or removed
	 */
	wxString Name;

	/**
	 * the new name, if the modification was a rename
	 */
	wxString NewName;

	/**
	 * the type of modification to perform
	 */
	mvceditor::ExplorerModifyActionClass::Actions Action;

	/**
	 * TRUE if the delete/rename was successful.
	 */
	bool Success;

	ExplorerModifyEventClass(int eventId, const wxFileName& dir, const wxString& name, 
		const wxString& newName, mvceditor::ExplorerModifyActionClass::Actions action, bool success);

	wxEvent* Clone() const;
};

}

#endif