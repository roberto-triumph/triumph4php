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

class ResourcePluginPanelClass;

/**
 * This class will take care of iterating through all files in a project
 * and parsing the resources so that queries to ResourceFinderClass
 * will work.
 * We will get around concurrency problems by copying the resource finder
 * objects; this way the background thread will have its own copy without
 * needing to synchronize.
 */
class ResourceFileReaderClass : public BackgroundFileReaderClass {

public:

	/**
	 * @param handler will receive EVENT_FILE_* and EVENT_WORK_* events when all 
	 * files have been iterated through.
	 */
	ResourceFileReaderClass(wxEvtHandler& handler);

	/**
	 * prepare to iterate through the file that has the PHP native functions.
	 *
	 * @param resourceCache the existing resources
	 * @return bool false if native functions file does not exist
	 */
	bool InitForNativeFunctionsFile(ResourceCacheClass* resourceCache);

	/**
	 * prepare to iterate through all files of the given directory
	 * that match the given wildcard.
	 *
	 * @param resourceCache the existing resources
	 * @param projectPath the directory to be scanned (recursively)
	 * @param phpFileFilters the list of PHP file extensions to look for resources in
	 * @return bool false if project root path does not exist
	 */
	bool InitForProject(ResourceCacheClass* resourceCache, const wxString& projectPath, const std::vector<wxString>& phpFileFilters);

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
	
	void OnProjectOpened();
	
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
	 * The "Index project" menu item
	 * @var wxMenuItem* 
	 */
	wxMenuItem* ProjectIndexMenu;

	/**
	 * When a tab changes we must update the FilesCombo combo box
	 */
	ResourcePluginPanelClass* ResourcePluginPanel;
	
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

}
#endif // __resourcepluginclass__
