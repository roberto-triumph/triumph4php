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
#ifndef __RESOURCEFEATURECLASS_H__
#define __RESOURCEFEATURECLASS_H__

#include <features/FeatureClass.h>
#include <features/wxformbuilder/ResourceFeatureForms.h>
#include <features/BackgroundFileReaderClass.h>
#include <search/ResourceFinderClass.h>
#include <code_control/ResourceCacheBuilderClass.h>
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
	 * @param runningThreads will receive EVENT_FILE_* and EVENT_WORK_* events when all 
	 * files have been iterated through.
	 */
	ResourceFileReaderClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);

	/**
	 * prepare to iterate through the given file. The name part of the given file must match the wildcard.
	 * This method can be used to update the resources once a file has been modified on disk.
	 *
	 * @param project the project that holds the file
	 * @param fullPath file to be scanned (full path, including name).
	 * @param version the version of PHP to check against
	 * @return bool false file does not exist
	 */
	bool InitForFile(const mvceditor::ProjectClass& project, const wxString& fullPath, pelet::Versions version);

	/**
	 * initialize the next project in the queue to be read. After a call to this method, the background
	 * thread can be started.
	 * @return TRUE if the project has at least one source directory that exists
	 */
	bool InitProject(const mvceditor::ProjectClass& project, pelet::Versions version);

protected:

	/**
	 * Files will be parsed for resouces in a background thread.
	 */
	bool BackgroundFileRead(DirectorySearchClass& search);

	/**
	 * Resources will only look for PHP files.
	 */
	bool BackgroundFileMatch(const wxString& file);

	void BackgroundCleanup();

private:

	/**
	 * the version of PHP to parse against
	 */
	pelet::Versions Version;

	/**
	 * This class will not own this pointer. the pointer will be created by this class
	 * but it will be posted via an event and the event handler will own it.
	 */
	mvceditor::GlobalCacheClass* GlobalCache;
};

/**
 * event that is generated when all resources databases have been wiped
 */
const wxEventType EVENT_WIPE_COMPLETE = wxNewEventType();

/**
 * Class to 'wipe' resource databases (empty all of their contents)
 */
class ResourceFileWipeThreadClass : public mvceditor::ThreadWithHeartbeatClass {
	
public:

	ResourceFileWipeThreadClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);
	
	bool Init(const std::vector<mvceditor::ProjectClass>& projects);
	
protected:
	
	void BackgroundWork();
	
private:
		
	/**
	 * The db files that need to be wiped.
	 */
	std::vector<wxFileName> ResourceDbFileNames;
	
};

class ResourceFeatureClass : public FeatureClass {

public:
	
	ResourceFeatureClass(mvceditor::AppClass& app);

	void AddSearchMenuItems(wxMenu* searchMenu);

	void AddToolBarItems(wxAuiToolBar* toolBar);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);
	
	void AddCodeControlClassContextMenuItems(wxMenu* menu);
	
	/**
	 * Searches for a file that matches the given text. 
	 * This wont be a straight equals search; it will be a "near match"
	 * as defined by ResourceFinderClass::CollectNearMathResources()
	 * when the given text is more than 2 characters long, and will
	 * be an exact search as defined by CollectFullyQualifiedResources when the 
	 * text is 2 characters long
	 * This method assumes that index is up-to-date.

	 * @param text search string to query for
	 * @return the matching resources
	 * @see ResourceFinderClass::CollectNearMacthResources
	 * @see ResourceFinderClass::CollectFullyQualifiedResources
	 */
	std::vector<mvceditor::ResourceClass> SearchForResources(const wxString& text);
	
	/**
	 * @param wxString full path to the file that will be opened.
	 */
	void OpenFile(wxString fileName);

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
	 * returns a short string describing the status of the cache.
	 */
	wxString CacheStatus();
	
private:

	/**
	 * Trigger the start of the indexing background thread.
	 */
	void StartIndex();

	void OnProjectsUpdated(wxCommandEvent& event);

	void OnAppFileClosed(wxCommandEvent& event);

	void OnAppReady(wxCommandEvent& event);

	void OnCmdReIndex(wxCommandEvent& event);
	
	/**
	 * Toggle various widgets on or off based on the application state. 
	 */
	void OnUpdateUi(wxUpdateUIEvent& event);
	
	/**
	 * Wipe the cache and re-index all of the enabled projects.
	 */
	void OnProjectWipeAndIndex(wxCommandEvent& event);
	
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
	 * When the global cache has been parsed add it to the resource cache
	 * so that it is available for code completion
	 */
	void OnGlobalCacheComplete(mvceditor::GlobalCacheCompleteEventClass& event);
	
	/**
	 * Opens the page and sets the cursor on the function/method/property/file that was searched for by the
	 * resource finder
	 * 
	 * @param finderQuery will be used to get the line number to scroll to
	 * @param resource the resource to load
	 */
	void LoadPageFromResource(const wxString& finderQuery, const ResourceClass& resource);

	/**
	 * remove matches for php built-in functions. as we dont want a source file to
	 * open.
	 * @param matches any native mataches from this given vector will be removed
	 */
	void RemoveNativeMatches(std::vector<mvceditor::ResourceClass>& matches) const;
	
	/**
	 * prepare to iterate through all files of the given projects
	 * that match the given wildcard.
	 *
	 * @param projects the directories to be scanned (recursively)
	 * @return bool false if none of the projects are enabled or none of the projects have a PHP source directory
	 */
	bool InitProjectQueue(const std::vector<mvceditor::ProjectClass>& projects);

	/**
	 * prepare to iterate through the given file. The name part of the given file must match the wildcard.
	 * This method can be used to update the resources once a file has been modified on disk.
	 *
	 * @param project the project that holds the file
	 * @param fullPath file to be scanned (full path, including name).
	 * @param version the version of PHP to check against
	 * @return bool false file does not exist
	 */
	bool InitForFile(const mvceditor::ProjectClass& project, const wxString& fullPath, pelet::Versions version);

	/**
	 * @return TRUE if there are more projects in the queue
	 */
	bool MoreProjects() const;

	/**
	 * initialize the next project in the queue to be read. After a call to this method, the background
	 * thread can be started.
	 * @param version the version of PHP to check against
	 * @param projectLabel the label of the project that will be read next is set here
	 * @return mvceditor::ResourceFileReaderClass* if the project has at least one source directory that exists
	 *         otherwise NULL is returned. 
	 *         since the returned pointer is a thread; the pointer will delete itself
	 */
	mvceditor::ResourceFileReaderClass* ReadNextProject(pelet::Versions version, wxString& projectLabel);
	
	/**
	 * after the projects have been wiped, star the index process
	 */
	void OnWipeComplete(wxCommandEvent& event);
	
	/**
	 * This method will get called by the WorkingCacheBuilderClass when parsing of the
	 * code in this control has been completed.
	 */
	void OnWorkingCacheComplete(mvceditor::WorkingCacheCompleteEventClass& event);
	
	/**
	 * This method will check to see if the current document is "dirty" and if so it will
	 * start re-parsing the document in the background
	 */
	void OnTimer(wxTimerEvent& event);

	 /**
	  * The various states control what this feature does.
	  * Because indexing runs in a background thread we need to save
	  * whether or not the user triggered an index or triggered a lookup
	  */
	 enum States {

		/**
		 * background thread is not running
		 */
		FREE,

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
	 * Used to control how often to check for resource re-parsing
	 */
	wxTimer Timer;

	/**
	 * The current source code is parsed in the background so that a symbol table can be
	 * built in the backgroun without slowing the user down. The builder will
	 * notify us with the new cache via an event when it has completed building the
	 * symbol table.
	 */
	WorkingCacheBuilderClass* WorkingCacheBuilder;
	
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
	
	/**
	 * Queue of projects to be indexed.
	 */
	std::queue<mvceditor::ProjectClass> ProjectQueue;

	wxThreadIdType RunningThreadId;

	DECLARE_EVENT_TABLE()
};

class ResourceSearchDialogClass : public ResourceSearchDialogGeneratedClass {
public:

	ResourceSearchDialogClass(wxWindow* parent, ResourceFeatureClass& resource, wxString& term, 
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

	void OnMatchesListDoubleClick(wxCommandEvent& event);

	void OnMatchesListKeyDown(wxKeyEvent& event);
	
private:

	/**
	 * The resource feature reference.  The dialog will use this reference to actually perform the search.
	 * 
	 * @var ResourceFeatureClass
	 */
	ResourceFeatureClass& ResourcePlugin;

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
#endif // __RESOURCEFEATURECLASS_H__
