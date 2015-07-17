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
 * @copyright  2015 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef T4P_EXPLORERVIEWCLASS_H
#define T4P_EXPLORERVIEWCLASS_H

#include <views/FeatureViewClass.h>
#include <features/ExplorerFeatureClass.h>
#include <views/wxformbuilder/ExplorerFeatureForms.h>
#include <widgets/FileTypeImageList.h>

namespace t4p {

class ExplorerViewClass : public t4p::FeatureViewClass {

public:

	ExplorerViewClass(t4p::ExplorerFeatureClass& feature);
	
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

	void AddPreferenceWindow(wxBookCtrlBase* parent);
	
private:

	t4p::ExplorerFeatureClass& Feature;
	
	/**
	 * toolbar to hold the explorer buttons
	 */
	wxAuiToolBar* ExplorerToolBar;

	/**
	 * Handler for the Project .. Explore Open File menu 
	 * @param wxCommandEvent& event 
	 */
	void OnProjectExplore(wxCommandEvent& event);
	
	/**
	 * Handler for the View .. Explore in Outline menu 
	 * @param wxCommandEvent& event 
	 */
	void OnProjectOutline(wxCommandEvent& event);

	/**
	 * Handler for the Project .. Explore Open File menu 
	 * @param wxCommandEvent& event 
	 */
	void OnProjectExploreOpenFile(wxCommandEvent& event);
	
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
	 * when another feature wants to open a directory, the will send
	 * a command, and this feature will "open" the directory (show its
	 * contents).
	 */
	void OnCmdDirOpen(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

class FileListingWidgetClass : public wxEvtHandler {
	
	public:

	/**
	 * this class will not own any of these pointers
	 */
	FileListingWidgetClass(wxListCtrl* list, wxImageList* imageList, 
		t4p::FileListingClass* fileListing, wxWindow* parentPanel, 
		t4p::ExplorerFeatureClass* feature);
	~FileListingWidgetClass();
	
	void ShowDir();
	
	// event handlers for the context menu on the files list
	void OnListMenuRename(wxCommandEvent& event);
	void OnListMenuDelete(wxCommandEvent& event);
	void OnListMenuCreateNew(wxCommandEvent& event);
	void OnListMenuCreateDirectory(wxCommandEvent& event);
	void OnListMenuShell(wxCommandEvent& event);
	void OnListMenuFileManager(wxCommandEvent& event);
	
private:

	// events handlers for the files list
	void OnListItemRightClick(wxListEvent& event);
	void OnListEndLabelEdit(wxListEvent& event);
	void OnListRightDown(wxMouseEvent& event);

	// adds files or directories to the list control
	void ListFiles(const std::vector<wxFileName>& files);
	void ListDirectories(const std::vector<wxFileName>& dirs);

	int ListImageId(const wxFileName& fileName);

	/**
	 * the list where the files are drawn onto
	 */
	wxListCtrl* List;
	
	/**
	 * will be owned by the list control
	 */
	wxImageList* FilesImageList;
	
	/**
	 * the listing model; the object that "backs" the panel and performs
	 * the actions (in the background). this class will NOT
	 * own this pointer
	 */
	t4p::FileListingClass* FileListing;
	
	/**
	 * the list item activate event will be propagated to this event
	 * handler.
	 */
	wxEvtHandler* ParentPanel;
	
	/**
	 * this class will not own this pointer
	 */
	t4p::ExplorerFeatureClass* Feature;

	enum ListImages {
		LIST_FOLDER = t4p::IMGLIST_NONE + 1,
		LIST_PARENT_FOLDER
	};
};

class ModalExplorerPanelClass : public ModalExplorerGeneratedPanelClass {
	public:
	
	ModalExplorerPanelClass(wxWindow* parent, int id, t4p::ExplorerFeatureClass& feature, t4p::ExplorerViewClass& view);
	~ModalExplorerPanelClass();

	void FillSourcesList(const std::vector<wxFileName>& sourceDirs);

	void FocusOnSourcesList();

	void RefreshDir(const wxFileName& dir);
	
	void ShowDir();
	
private:


	/**
	 * the listing model; the object that "backs" the panel and performs
	 * the actions (in the background). this class owns the pointer
	 */
	t4p::FileListingClass* FileListing;
	
	/**
	* will be owned by the list control
	 */
	wxImageList* FilesImageList;
	
	/**
	 * will be owned by the list control
	 */
	wxImageList* SourcesImageList;

	/**
	 * to get projects list and tag cache
	 */
	t4p::ExplorerFeatureClass& Feature;
	
	/**
	 * the view  contains the opened code controls, used 
	 * during file renames
	 */
	t4p::ExplorerViewClass& View;
	
	/**
	 * this class will own the pointer
	 */
	t4p::FileListingWidgetClass* FileListingWidget;

	/**
	 * the currently selected filter menu item
	 */
	int FilterChoice;

	enum SourceImages {
		SOURCE_FOLDER
	};

	void OnExplorerModifyComplete(t4p::ExplorerModifyEventClass& event);


	// event handler for the combo box
	void OnDirectoryEnter(wxCommandEvent& event);

	// event handlers for the sources list
	void OnSourceActivated(wxListEvent& event);

	void OnListItemActivated(wxListEvent& event);
	void OnListMenuOpen(wxCommandEvent& event);
	void OnListKeyDown(wxKeyEvent& event);
	
	// handlers for the buttons
	void OnParentButtonClick(wxCommandEvent& event);
	void OnFilterButtonLeftDown(wxMouseEvent& event);
	void OnFilterMenuCheck(wxCommandEvent& event);
	void OnRefreshClick(wxCommandEvent& event);
	void OnHelpButton(wxCommandEvent& event);
	
	void RenamePrompt(const wxFileName& oldFile, const wxString& newName);
	
	void OnExplorerListComplete(t4p::ExplorerEventClass& event);
	void OnFsWatcher(wxFileSystemWatcherEvent& event);
	bool OpenIfListFile(const wxString& text);
	
	std::vector<wxString> FilterFileExtensions();

	DECLARE_EVENT_TABLE()
};

class ExplorerOutlinePanelClass : public ExplorerOutlineGeneratedPanelClass {
	public:
	
	ExplorerOutlinePanelClass(wxWindow* parent, int id, t4p::ExplorerFeatureClass& feature, t4p::ExplorerViewClass& view);
	~ExplorerOutlinePanelClass();

	void RefreshDir(const wxFileName& dir);
	
	void FillSourcesList(const std::vector<wxFileName>& sourceDirs);
	
	void ShowDir();
	
private:


	/**
	 * the listing model; the object that "backs" the panel and performs
	 * the actions (in the background). this class owns the pointer
	 */
	t4p::FileListingClass* FileListing;
	
	/**
	* will be owned by the list control
	 */
	wxImageList* FilesImageList;
	
	/**
	 * to get projects list and tag cache
	 */
	t4p::ExplorerFeatureClass& Feature;
	
	/**
	 * the view contains the opened code controls, used 
	 * during file renames
	 */
	t4p::ExplorerViewClass& View;
	
	/**
	 * this class will own the pointer
	 */
	t4p::FileListingWidgetClass* FileListingWidget;

	/**
	 * the currently selected filter menu item
	 */
	int FilterChoice;

	void OnExplorerModifyComplete(t4p::ExplorerModifyEventClass& event);


	// event handler for the combo box
	void OnDirectoryEnter(wxCommandEvent& event);
	void OnDirectorySelected(wxCommandEvent& event);

	void OnListItemActivated(wxListEvent& event);
	void OnListMenuOpen(wxCommandEvent& event);
	void OnListKeyDown(wxKeyEvent& event);
	
	// handlers for the buttons
	void OnParentButtonClick(wxCommandEvent& event);
	void OnFilterButtonLeftDown(wxMouseEvent& event);
	void OnFilterMenuCheck(wxCommandEvent& event);
	void OnRefreshClick(wxCommandEvent& event);
	void OnHelpButton(wxCommandEvent& event);
	
	void RenamePrompt(const wxFileName& oldFile, const wxString& newName);
	
	void OnExplorerListComplete(t4p::ExplorerEventClass& event);
	void OnFsWatcher(wxFileSystemWatcherEvent& event);
	bool OpenIfListFile(const wxString& text);
	
	std::vector<wxString> FilterFileExtensions();

	DECLARE_EVENT_TABLE()
};

/**
 * Panel that shows the explorer options: locations of Operation System shell, file manager
 */
class ExplorerOptionsPanelClass : public ExplorerOptionsGeneratedPanelClass {

public:

	ExplorerOptionsPanelClass(wxWindow* parent, int id, t4p::ExplorerFeatureClass& feature);	

};

/**
 * Use our own new file dialog; this pretty much just like 
 * wxGetTextFromUser() except that we will select only the file name and
 * not the extension.
 */
class ExplorerNewFileDialogClass : public ExplorerNewFileGeneratedDialogClass {
	
	public:
	
	ExplorerNewFileDialogClass(wxWindow* parent, const wxString& title, 
		const wxString& currentDir, wxString& fileName);
	
	protected:
	
	void OnOkButton(wxCommandEvent& event);
	void OnTextEnter(wxCommandEvent& event);
	
	private:
	
	/**
	 * the CurrentDir is used to check that the user does not enter
	 * a file name that already exists in the CurrentDir
	 */
	wxString CurrentDir;
	
	/**
	 * the name that was input by the user
	 */
	wxString& FileName;
};

}

#endif
