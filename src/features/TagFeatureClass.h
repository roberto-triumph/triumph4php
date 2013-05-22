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
#include <features/wxformbuilder/TagFeatureForms.h>
#include <features/BackgroundFileReaderClass.h>
#include <search/TagFinderClass.h>
#include <actions/ProjectTagActionClass.h>
#include <actions/TagCacheSearchActionClass.h>
#include <code_control/ResourceCacheBuilderClass.h>
#include <widgets/GaugeDialogClass.h>
#include <wx/string.h>
#include <queue>

namespace mvceditor {


class TagFeatureClass : public FeatureClass {

public:
	
	TagFeatureClass(mvceditor::AppClass& app);

	void AddSearchMenuItems(wxMenu* searchMenu);

	void AddToolBarItems(wxAuiToolBar* toolBar);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);
	
	void AddCodeControlClassContextMenuItems(wxMenu* menu);
		
	/**
	 * @param wxString full path to the file that will be opened.
	 */
	void OpenFile(wxString fileName);

	/** 
	 * returns a short string describing the status of the cache.
	 */
	wxString CacheStatus();
	
private:

	void OnProjectsUpdated(wxCommandEvent& event);

	void OnAppFileClosed(wxCommandEvent& event);

	void OnAppStartSequenceComplete(wxCommandEvent& event);
	
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
	 * Opens the page and sets the cursor on the function/method/property/file that was searched for by the
	 * tag finder
	 * 
	 * @param finderQuery will be used to get the line number to scroll to
	 * @param tag the tag to load
	 */
	void LoadPageFromResource(const wxString& finderQuery, const TagClass& tag);
	
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
	 * This method will get called by the WorkingCacheBuilderClass when parsing of the
	 * code in this control has been completed.
	 */
	void OnWorkingCacheComplete(mvceditor::WorkingCacheCompleteEventClass& event);
	
	/**
	 * This method will start re-parsing the document in the background. this will allow the 
	 * code completion to be up-to-date after a file is saved.
	 */
	void OnAppFileSaved(mvceditor::FileSavedEventClass& event);

	/**
	 * This method will start re-parsing the document in the background. this will allow the 
	 * code completion to be up-to-date after a file is *opened*
	 */
	void OnAppFileOpened(wxCommandEvent& event);

	/**
	 * when the tag cache has been searched, display the matching tags in the dialog so that
	 * the user can choose which to open
	 */
	void OnTagCacheSearchComplete(mvceditor::TagCacheSearchCompleteEventClass& event);

	/**
	 * when a 'jump to tag' is done and we need to index a project, we
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
	mvceditor::GaugeDialogClass* IndexingDialog;

	/**
	 * cache will be considered stale at app start. once all projects have
	 * been indexed then it will be considered as good; though this is a real
	 * naive status as it does not take file system changes from external processes
	 */
	enum CacheStatus {
		CACHE_STALE,
		CACHE_OK
	} CacheState;
	
	DECLARE_EVENT_TABLE()
};

/**
 * the tag searching will be done in a background thread.
 */
class TagSearchDialogClass : public TagSearchDialogGeneratedClass {
public:

	/**
	 * @param parent the parent window
	 * @param tag the feature, used to perform the search logic
	 * @param term string to prepopulate the input box
	 * @param chosenResources out parameter, the list of resources that the user chose
	 */
	TagSearchDialogClass(wxWindow* parent, 
		TagFeatureClass& tag, wxString& term, 
		std::vector<mvceditor::TagClass>& chosenResources);
	
	~TagSearchDialogClass();

	/**
	 * @param text to place in the search text
	 * @param matches to place in the results list
	 */
	void Prepopulate(const wxString& text, const std::vector<mvceditor::TagClass>& matches);

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

	void OnProjectChoice(wxCommandEvent& event);
	
private:

	/**
	 * Handle the results of the tag lookups.
	 */
	void ShowJumpToResults(const wxString& finderQuery, const std::vector<mvceditor::TagClass>& matches);

	/**
	 * *STARTS* a search for a file that matches the given text. 
	 * This wont be a straight equals search; it will be a "near match"
	 * as defined by ParsedTagFinderClass::CollectNearMathResources()
	 * when the given text is more than 2 characters long, and will
	 * be an exact search as defined by CollectFullyQualifiedResources when the 
	 * text is 2 characters long
	 * This method assumes that index is up-to-date.

	 * @param text search string to query for
	 * @param projects matches from these projects will be kept; all others will be erased.
	 *        this method will not own the project pointers
	 * @return the matching resources
	 * @see ParsedTagFinderClass::CollectNearMacthResources
	 * @see ParsedTagFinderClass::CollectFullyQualifiedResources
	 */
	void SearchForResources(const wxString& text, std::vector<mvceditor::ProjectClass*> projects);

	/**
	 * when the timer completes, perform the search
	 */
	void OnTimerComplete(wxTimerEvent& event);

	/**
	 * when the tag cache has been searched, display the matching tags in the dialog so that
	 * the user can choose which to open
	 */
	void OnTagCacheSearchComplete(mvceditor::TagCacheSearchCompleteEventClass& event);

	/**
	 * The tag feature reference.  The dialog will use this reference to actually perform the search.
	 * 
	 * @var TagFeatureClass
	 */
	TagFeatureClass& ResourceFeature;

	/**
	 * List that will get populated with the files to be opened. These are the
	 * files that the user selects.
	 */
	std::vector<mvceditor::TagClass>& ChosenResources;

	/**
	 * results of the most recent search
	 */
	std::vector<mvceditor::TagClass> MatchedResources;

	/**
	 * we use a timer to see when the user has stopped typing. on a keypress,
	 * we start a timer (if it hasn't already been started).
	 */
	wxTimer Timer;

	/**
	 * the last search input. when the timer is up, if the text contents have
	 * changed we actually perform the search
	 */
	wxString LastInput;

	/**
	 * we will cancel an action if the user has changed input, that way the always see
	 * results for the most recent search
	 */
	int ActionId;

	DECLARE_EVENT_TABLE()
};

}
#endif // __RESOURCEFEATURECLASS_H__
