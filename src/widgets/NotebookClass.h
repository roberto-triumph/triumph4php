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
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef MVCEDITORNOTEBOOKCLASS_H_
#define MVCEDITORNOTEBOOKCLASS_H_

#include <code_control/CodeControlClass.h>
#include <environment/ProjectClass.h>
#include <environment/StructsClass.h>
#include <Events.h>
#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <wx/dnd.h>
#include <vector>

namespace mvceditor {
/**
 * This class contains logic relating to the workings of the
 * notebook (source code editor tabs).
 */
class NotebookClass : public wxAuiNotebook {

public:

	/**
	 * Fonts, colors, and other code control settings
	 * Initially NULL, should get set by container (frame)
	 * This class will NOT delete the pointer
	 * 
	 * @var CodeControlOptionsClass*
	 */
	CodeControlOptionsClass* CodeControlOptions;
	
	/**
	 * This object is required by the source code controls
	 * This class will NOT own this pointer.
	 */
	StructsClass* Structs;
	
	/**
	 * Constructor. Parent is needed, all others are optional.
	 */
	NotebookClass(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize, 
		long style = wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_WINDOWLIST_BUTTON);
	
	/**
	 * Changes the text of the page tab to mark it having changes that have 
	 * NOT been saved.
	 * 
	 * @param int the window Id of the window to change.
	 */
	void MarkPageAsModified(int windowId);

	/**
	 * Changes the text of the page tab to mark it having changes that have 
	 * been saved.
	 * 
	 * @param int the window Id of the window to change.
	 */
	void MarkPageAsNotModified(int windowId);
	
	/**
	 * Save the given page to a file.  This method will prompt the user
	 * for a filename if the page is a new page.
	 * 
	 * @param int pageIndex the index of the page
	 * @return bool true if the page was saved successfully
	 */
	bool SavePage(int pageIndex);
	
	/**
	 * Saves the current page to a file. This method will prompt the user
	 * for a filename if the current page is a new page.
	 * 
	 * @return bool true if the page was saved successfully
	 */
	bool SaveCurrentPage();
	 
	/**
	 * This method will prompt the user for a filename and save the contents
	 * of the source code control to the new file. The state of the editor
	 * is not affected.
	 * 
	 * @return bool true if the page was saved successfully
	 */
	bool SaveCurrentPageAsNew();
	
	/**
	 * Create a new source code control, and selects it.
	 */
	void AddMvcEditorPage();
	
	/**
	 * Prompts the user to pick an existing file and loads the contents of the
	 * file to a new MVC editor page.
	 */
	void LoadPage();
	
	/**
	 * Loads the given file into this notebook. If the file is already opened then it is brought
	 * to the forefront (a file will never be loaded in two controls).
	 * File name must be fully qualified. 
	 */
	void LoadPage(const wxString& filename);
	
	/**
	 * Loads the given files into this notebook, one page for each file.
	 * File names must be fully qualified. 
	 */
	void LoadPages(const std::vector<wxString>& filenames);	
	
	/**
	 * Finds out whether the given page has changes that have not been saved.
	 * 
	 * @param int pageIndex the page to query
	 * @return bool true if the page is has changes that have not been saved
	 */
	bool IsPageModified(int pageIndex) const;
	
	/**
	 * Look through all open files and save the modified files. Prompts for 
	 * names for all new files.
	 * 
	 * @return bool false if the user cancels the save process.
	 */
	bool SaveAllModifiedPages();

	/**
	 * Get the source code control at the given index.
	 * 
	 * @param size_t pageIndex the page number to get
	 * @return mvceditor::CodeControlClass* the source code control window, NULL
	 * if pageIndex is invalid.
	 */
	CodeControlClass* GetCodeControl(size_t pageIndex) const;

	/**
	 * Get the currently visible source code control
	 * 
	 * @return MvcEditorCodeControlClass* the source code control window, NULL
	 * if notebook is empty.
	 */
	CodeControlClass* GetCurrentCodeControl() const;
	
	/**
	 * Closes all open pages.
	 */
	void CloseAllPages();
	
	/**
	 * Applies the code control options to all open pages. This call is needed when the options change.
	 * 
	 */
	void RefreshCodeControlOptions();
	
	/**
	 * Give code controls access to a single project. 
	 * Memory management of both pointers is  left to the caller of this method.
	 * @param project the opened project
	 * @param eventSink given to each instance of CodeControlClass that is created.
	 */
	void SetProject(ProjectClass* project, EventSinkClass* eventSink);
	
	/**
	 * Closes (deletes) the current page (prompts the user to save if the file has been modified)
	 */
	void CloseCurrentPage();
	
	/**
	 * Return the full paths of the file names
	 * @return vector<wxString>
	 */
	std::vector<wxString> GetOpenedFiles() const;
	
protected:
	
	/**
	 * Handle the Close Page event.  Will save the contents of the
	 * window being closed. If the window was not loaded from a file,
	 * this method will priompt the user for a file name.
	 * 
	 * @param wxAuiNotebook& event the close event
	 */
	void SavePageIfModified(wxAuiNotebookEvent& event);
	
	/**
	 * Handle the right-click button on the notebook tabs.  Shows a context
	 * menu.
	 */
	void ShowContextMenu(wxAuiNotebookEvent& event);
	
	/**
	 * Handle the 'Close All Tabs' event
	 */
	void OnCloseAllPages(wxCommandEvent& event);

	/**
	 * When a page is changed; enable/disable its tool tips
	 */
	void OnPageChanging(wxAuiNotebookEvent& event);

private:
	
	/**
	 * Creates the right-click context menu.
	 */
	void CreateContextMenu();
	
	/**
	 * Find out all the open files that have changes that have not been saved.
	 * 
	 * @param std::vector<wxString>& modifiedPageNames modified page names are pushed 
	 *        onto this parameter
	 * @param std::vector<int>&  modifiedPageIndexes modified pages indexes are pushed
	 *        onto this parameter
	 * @return bool true if any open files have changes that have not been
	 *  saved.
	 */
	bool GetModifiedPageNames(std::vector<wxString>& modifiedPageNames, std::vector<int>& modifiedPageIndexes) const;

	/**
	 * Creates the dialog wildcard string based on the project's settings.
	 */
	wxString CreateWildcardString() const;
	
	/**
	 * @return the wildcard  index according to the given mode
	 * the index is suitable to pass to the wxFileDialog::SetFilterIndex method
	 */
	int WilcardIndex(CodeControlClass::Mode mode);

	/**
	 * will use the close source code event to re-trigger project indexing.
	 */
	void OnNotebookPageClose(wxAuiNotebookEvent& evt);
	
	/**
	 * To keep track of any threads used for PHP parsing
	 */
	mvceditor::RunningThreadsClass RunningThreads;
	
	/**
	 * The context menu for handling right-click options
	 */
	wxMenu* ContextMenu;
	
	/**
	 * To find classes & files
	 * @var ProjectClass
	 */
	ProjectClass* Project;

	/**
	 * This object will be used to publish app events. When an editor tab is closed;
	 * we need to trigger project re-indexing since it has not been updated with
	 * the changes made to the file that was opened (the parsed resources were being 
	 * handled in a separate cache by ResourceCacheClass).
	 */
	EventSinkClass* EventSink;
	
	/**
	 * To give a friendly number to new files. 
	 */
	int NewPageNumber;	
	
	DECLARE_EVENT_TABLE()
	
};

/**
 * This class is needed to accept files dragged from the OS (file explorer)
 * This class will open the dragged files.
 */
class FileDropTargetClass : public wxFileDropTarget {

public:

	/**
	 * constructor
	 * @var NotebookClass* pointer to the notebook. The pointer will NOT be managed (deleted) by this class. 
	 */
	FileDropTargetClass(NotebookClass* notebook);

	/** 
	 * Called by wxWidgets when user drags a file to this application frame. All files dragged in will be opened
	 * into the notebook.
	 */
	virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& files);

private:

	NotebookClass* Notebook;
};

}
#endif /*MVCEDITORNOTEBOOKCLASS_H_*/
