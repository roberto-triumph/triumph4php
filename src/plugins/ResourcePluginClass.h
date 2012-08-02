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
#include <queue>

namespace mvceditor {

// these are defined at the bottom
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
	ResourceFileReaderClass(wxEvtHandler& handler, mvceditor::RunningThreadsClass& runningThreads);

	/**
	 * prepare to iterate through all files of the given projects
	 * that match the given wildcard.
	 *
	 * @param resourceCache the existing resources, any new resources will be added to this cache
	 * @param projects the directories to be scanned (recursively)
	 * @return bool false if none of the projects are enabled or none of the projects have a PHP source directory
	 */
	bool InitProjectQueue(ResourceCacheClass* resourceCache, const std::vector<mvceditor::ProjectClass>& projects);

	/**
	 * prepare to iterate through the given file. The name part of the given file must match the wildcard.
	 * This method can be used to update the resources once a file has been modified on disk.
	 *
	 * @param resourceCache the existing resources, any new resources will be added to this cache
	 * @param fullPath file to be scanned (full path, including name).
	 * @return bool false file does not exist
	 */
	bool InitForFile(ResourceCacheClass* resourceCache, const wxString& fullPath);

	/**
	 * @return TRUE if there are more projects in the queue
	 */
	bool MoreProjects() const;

	/**
	 * initialize the next project in the queue to be read. After a call to this method, the background
	 * thread can be started.
	 * @return TRUE if the project has at least one source directory that exists
	 */
	bool ReadNextProject();

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

	/**
	 * Queue of projects to be indexed.
	 */
	std::queue<mvceditor::ProjectClass> ProjectQueue;

	/**
	 * full path to the location where the current parsed resources are being
	 * written to.
	 */
	wxFileName CurrentResourceDbFileName;
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
	NativeFunctionsFileReaderClass(wxEvtHandler& handler, mvceditor::RunningThreadsClass& runningThreads);

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
	
	ResourcePluginClass(mvceditor::AppClass& app);

	void AddSearchMenuItems(wxMenu* searchMenu);

	void AddToolBarItems(wxAuiToolBar* toolBar);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);
	
	void AddCodeControlClassContextMenuItems(wxMenu* menu);
	
	/**
	 * Searches for a file that matches the given text. 
	 * This wont be a straight equals search; it will be a "near match"
	 * as defined by ResourceFinderClass::CollectNearMathResources()
	 * This method assumes that index is up-to-date.

	 * @param text search string to query for
	 * @return the matching resources
	 * @see ResourceFinderClass::CollectNearMathResources
	 */
	std::vector<mvceditor::ResourceClass> SearchForResources(const wxString& text);
	
	/**
	 * @param wxString full path to the file that will be opened.
	 */
	void OpenFile(wxString fileName);

	/**
	 * Trigger the start of the indexing background thread.
	 */
	void StartIndex();

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
	
private:

	void OnProjectsUpdated(wxCommandEvent& event);

	void OnAppFileClosed(wxCommandEvent& event);

	void OnCmdReIndex(wxCommandEvent& event);
	
	void OnEnvironmentUpdated(wxCommandEvent& event);
	
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
	 * remove matches for php buil-in functions. as we dont want a source file to
	 * open.
	 * @param matches any native mataches from this given vector will be removed
	 */
	void RemoveNativeMatches(std::vector<mvceditor::ResourceClass>& matches) const;

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
	 * when a 'jump to resource' is done and we need to index a project, we
	 * need to keep the search string so that after indexing we can
	 * search the index.
	 */
	wxString JumpToText;
	
	/**
	 * The "Index project" menu item
	 * @var wxMenuItem* 
	 */
	wxMenuItem* ProjectIndexMenu;

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

class ResourceSearchDialogClass : public ResourceSearchDialogGeneratedClass {
public:

	ResourceSearchDialogClass(wxWindow* parent, ResourcePluginClass& resource, wxString& term, 
		std::vector<mvceditor::ResourceClass>& chosenResources);
		
	/**
	 * @param text to place in the search text
	 * @param matches to place in the results list
	 */
	void Prepopulate(const wxString& text, const std::vector<mvceditor::ResourceClass>& matches);

protected:

	/**
	 * When user types in text perform searched
	 */
	void OnSearchText(wxCommandEvent& event);

	/**
	 * If user hits enter when there is one result only,
	 * then jump to the result
	 */
	void OnSearchEnter(wxCommandEvent& event);

	/**
	 * When the OK button is clicked we will open the files
	 */
	void OnOkButton(wxCommandEvent& event);
	void OnCancelButton(wxCommandEvent& event);

	/**
	 * When the user clicks the help button help text will be shown
	 */
	void OnHelpButton(wxCommandEvent& event);

	void OnSearchKeyDown(wxKeyEvent& event);

	void OnResultsDoubleClick(wxCommandEvent& event);
	
private:

	/**
	 * The resource plugin reference.  The dialog will use this reference to actually perform the search.
	 * 
	 * @var ResourcePluginClass
	 */
	ResourcePluginClass& ResourcePlugin;

	/**
	 * Handle the results of the resource lookups.
	 */
	void ShowJumpToResults(const wxString& finderQuery, const std::vector<mvceditor::ResourceClass>& matches);

	/**
	 * List that will get populated with the files to be opened. These are the
	 * files that the user selects.
	 */
	std::vector<mvceditor::ResourceClass>& ChosenResources;

	/**
	 * results of the most recent search
	 */
	std::vector<mvceditor::ResourceClass> MatchedResources;
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
