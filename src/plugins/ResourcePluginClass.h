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
#ifndef __resourcepluginclass__
#define __resourcepluginclass__

#include <PluginClass.h>
#include <plugins/wxformbuilder/ResourcePluginGeneratedClass.h>
#include <plugins/BackgroundFileReaderClass.h>
#include <search/ResourceFinderClass.h>
#include <wx/string.h>

namespace mvceditor {

// these are defined at the bottom
class ResourcePluginPanelClass;
class IndexingDialogClass;

/**
 * This class will take care of iterating through all files in a project
 * and parsing the resources so that queries to ResourceFinderClass
 * will work.
 */
class ResourceFileReaderClass : public BackgroundFileReaderClass {

public:

	/**
	 * @param handler will receive EVENT_FILE_* and EVENT_WORK_* events when all 
	 * files have been iterated through.
	 */
	ResourceFileReaderClass(wxEvtHandler& handler);

	/**
	 * prepare to iterate through all files of the given directory
	 * that match the given wildcard.
	 *
	 * @param resourceCache the existing resources, any new resources will be added to this cache
	 * @param projectPath the directory to be scanned (recursively)
	 * @param phpFileFilters the list of PHP file extensions to look for resources in
	 * @return bool false if project root path does not exist
	 */
	bool InitForProject(ResourceCacheClass* resourceCache, const wxString& projectPath, const std::vector<wxString>& phpFileFilters);

	/**
	 * prepare to iterate through the given file. The name part of the given file must match the wildcard.
	 * This method can be used to update the resources once a file has been modified on disk.
	 *
	 * @param resourceCache the existing resources, any new resources will be added to this cache
	 * @param fullPath file to be scanned (full path, including name).
	 * @return bool false file does not exist
	 */
	bool InitForFile(ResourceCacheClass* resourceCache, const wxString& fullPath);

protected:

	/**
	 * Files will be parsed for resouces in a background thread.
	 */
	virtual bool FileRead(DirectorySearchClass& search);

	/**
	 * Resources will only look for PHP files.
	 */
	virtual bool FileMatch(const wxString& file);

private:

	/**
	 * The filters that match files to be parsed for resources.
	 */
	std::vector<wxString> PhpFileFilters;

	/**
	 * the global cache; the parsed resources will be copied to this cache object
	 */
	ResourceCacheClass* ResourceCache;
};

/**
 * This class will take care of loading the php native functions assets file in 
 * a background thread.
 */
class NativeFunctionsFileReaderClass : public ThreadWithHeartbeatClass {

public:

	/**
	 * @param handler will receive EVENT_FILE_* and EVENT_WORK_* events when all 
	 * files have been iterated through.
	 */
	NativeFunctionsFileReaderClass(wxEvtHandler& handler);

	/**
	 * prepare to iterate through the file that has the PHP native functions.
	 *
	 * @param resourceCache the existing resource cache, any new resources will be added to this cache
	 * @return bool false if native functions file does not exist or thread could not be started
	 */
	bool Init(ResourceCacheClass* resourceCache);

private:

	/**
	 * the global cache; the parsed resources will be copied to this cache object
	 */
	ResourceCacheClass* ResourceCache;

	void Entry();
};
	
class ResourcePluginClass : public PluginClass {

public:

	/**
	 * the text being searched for. Any text entered here is not assumed to be any specific type;
	 * a resource search will be done as per normal ResourceFinderClass rules.
	 * 
	 * @var wxString
	 * @see ResourceFinderClass
	 */
	wxString JumpToText;
	
	/**
	 * create a resource plugin.
	 */
	ResourcePluginClass();

	void AddProjectMenuItems(wxMenu* projectMenu);

	void AddToolBarItems(wxAuiToolBar* toolBar);
	
	void AddWindows();

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);
	
	void AddCodeControlClassContextMenuItems(wxMenu* menu);
	
	/**
	 * Searches for a file that matches FileText.  if a single match is found, then the file is opened / brought to the front.
	 * If multiple files match, then user will be prompted to pick a file to open.
	 */
	void SearchForFiles();
	
	/**
	 * Searches for a file that matches JumpToText.  if a single match is found, then the file is opened / brought to the front.
	 * If multiple files match, then user will be prompted to pick a file to open.
	 */
	void SearchForResources();
	
	/**
	 * @param wxString full path to the file that will be opened.
	 */
	void OpenFile(wxString fileName);

	/**
	 * Trigger the start of the indexing background thread.
	 */
	void StartIndex();
	
private:

	void OnProjectOpened(wxCommandEvent& event);

	void OnAppFileClosed(wxCommandEvent& event);

	void OnCmdReIndex(wxCommandEvent& event);
	
	void OnEnvironmentUpdated(wxCommandEvent& event);

	/**
	 * Handle the results of the resource lookups.
	 */
	void ShowJumpToResults(const wxString& finderQuery, const std::vector<mvceditor::ResourceClass>& matches);
	
	/**
	 * Toggle various widgets on or off based on the application state. 
	 */
	void OnUpdateUi(wxUpdateUIEvent& event);
	
	/**
	 * Recurse through project files and gather resources.
	 */
	void OnProjectIndex(wxCommandEvent& event);
	
	/**
	 * Handle to Go To Source menu event
	 */
	void OnJump(wxCommandEvent& event);
	
	/**
	 * Handle to Search For Resource menu event
	 */
	void OnSearchForResource(wxCommandEvent& event);

	/**
	 * During file iteration we will pulse the gauge.
	 */
	void OnWorkInProgress(wxCommandEvent& event);

	/**
	 * Once the file parsing finishes alert the user.
	 */
	void OnWorkComplete(wxCommandEvent& event);
	
	/**
	 * Opens the page and sets the cursor on the function/method/property/file that was searched for by the
	 * resource finder
	 * 
	 * @param finderQuery will be used to get the line number to scroll to
	 * @param resource the resource to load
	 */
	void LoadPageFromResource(const wxString& finderQuery, const ResourceClass& resource);
	
	/**
	* Returns true if files in the project have NOT already been cached by the resource finder. This does not
	* necesaarily mean that the resource finder has parsed them; if so far all resource lookups have been for
	* file names then the resource finder has not parsed a single file.  What it does mean is that the next call
	* to ResourceFinderClass::LocateResourceInFileSystem will be really slow because the appropriate cache has
	* not been built. (ie file lookups will be fast after the first file lookup, class lookups will be
	* slow until the second file lookup)
	*
	* @return bool
	*/
	bool NeedToIndex(const wxString& finderQuery) const;
	
	/**
	 * When user changes a page update the FilesCombo box on the ResourcePluginPanel
	 * 
	 * @param wxAuiNotebookEvent& event
	 */
	void OnPageChanged(wxAuiNotebookEvent& event);
	
	/**
	 * When user closes all page clear the FilesCombo box on the ResourcePluginPanel
	 * 
	 * @param wxAuiNotebookEvent& event
	 */
	void OnPageClosed(wxAuiNotebookEvent& event);

	 /**
	  * The various states control what this plugin does.
	  * Because indexing runs in a background thread we need to save
	  * whether or not the user triggered an index or triggered a lookup
	  */
	 enum States {

		/**
		 * background thread is not running
		 */
		FREE,

		/**
		 * background thread is running; used during initial project opening
		 */
		INDEXING_NATIVE_FUNCTIONS,

		/**
		 * background thread is running; used triggered an index operation only
		 */
		INDEXING_PROJECT,

		/**
		 * background thread is running; used triggered a resource lookup
		 * once thread finishes results will be shown to the user
		 */
		GOTO
	};
	
	/**
	 * Used to iterate through directories
	 * 
	 * @var DirectorySearch
	 */
	ResourceFileReaderClass ResourceFileReader;

	/**
	 * to load the file that contains all of the PHP native functions
	 * see mvceditor::NativeFunctionsAssets
	 */
	NativeFunctionsFileReaderClass NativeFunctionsReader;
	
	/**
	 * The "Index project" menu item
	 * @var wxMenuItem* 
	 */
	wxMenuItem* ProjectIndexMenu;

	/**
	 * When a tab changes we must update the FilesCombo combo box
	 */
	ResourcePluginPanelClass* ResourcePluginPanel;

	/**
	 * A more prominent status bar that indicated progress
	 * this dialog will only be alive while project is being indexed
	 */
	IndexingDialogClass* IndexingDialog;
	
	/**
	 * To check if parsing is happening at the moment; and what files we are parsing.
	 */
	States State;

	/**
	* Flag that will store when files have been parsed.
	* @var bool
	*/
	bool HasCodeLookups;

	/**
	* Flag that will store when file names have been walked over and cached.
	* @var bool
	*/
	bool HasFileLookups;

	DECLARE_EVENT_TABLE()
};

class ResourcePluginPanelClass : public ResourcePluginGeneratedPanelClass {
public:

	ResourcePluginPanelClass(wxWindow* parent, ResourcePluginClass& resource);
	
	/**
	 * Set the focus on the search control
	 */
	void FocusOnSearchControl();
	
	/**
	 * @param fileName the file name to put in the files combo box
	 */
	void ChangeToFileName(wxString fileName);

	/**
	 * Files won't be removed from the combo right away; only the oldest
	 * will be removed.
	 * @param NotebookClass* the notebook of opened files this class will NOT own the pointer
	 */
	void RemoveClosedFiles(NotebookClass* notebook);	
	
protected:

	/**
	 * When a file is chosen make the notebook show the file
	 * @param wxCommandEvent& the event
	 */
	void OnFilesComboCombobox(wxCommandEvent& event);
	
	/**
	 * When the user presses enter iniate a file search for the entered text.
	 * @param wxCommandEvent& the event
	 */
	void OnFilesComboTextEnter(wxCommandEvent& event);
	
	/**
	 * When the user clicks the help button help text will be shown
	 * @param wxCommandEvent& the event
	 */
	void OnHelpButtonClick(wxCommandEvent& event);
	
private:

	/**
	 * The resource plugin reference.  The dialog will use this reference to actually perform the search.
	 * 
	 * @var ResourcePluginClass
	 */
	ResourcePluginClass& ResourcePlugin;
};

/**
 * A small dialog so that the user knows the editor is indexing.  Creating a full-fledged
 * dialog because the lower status bar might not be enough feedback.
 */
class IndexingDialogClass : public IndexingDialogGeneratedClass {

public:

	IndexingDialogClass(wxWindow* parent);

	/**
	 * initialize this dialog's gauge (in pulse mode)
	 */
	void Start();

	/**
	 * increment this dialog's gauge
	 */
	void Increment();

protected:

	void OnHideButton(wxCommandEvent& event);

};
}
#endif // __resourcepluginclass__
