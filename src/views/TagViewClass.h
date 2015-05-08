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
#ifndef __RESOURCEVIEWCLASS_H__
#define __RESOURCEVIEWCLASS_H__

#include <views/FeatureViewClass.h>
#include <features/TagFeatureClass.h>
#include <views/wxformbuilder/TagFeatureForms.h>
#include <code_control/ResourceCacheBuilderClass.h>
#include <widgets/GaugeDialogClass.h>
#include <wx/string.h>

namespace t4p {


class TagViewClass : public FeatureViewClass {

public:
	
	TagViewClass(t4p::TagFeatureClass& feature);

	void AddSearchMenuItems(wxMenu* searchMenu);

	void AddToolBarItems(wxAuiToolBar* toolBar);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);
	
	void AddCodeControlClassContextMenuItems(wxMenu* menu);
		
	/**
	 * @param wxString full path to the file that will be opened.
	 */
	void OpenFile(wxString fileName);
	
private:

	void OnProjectsUpdated(wxCommandEvent& event);

	void OnAppFileClosed(t4p::CodeControlEventClass& event);
	
	void OnAppExit(wxCommandEvent& event);
	
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
	bool InitForFile(const t4p::ProjectClass& project, const wxString& fullPath, pelet::Versions version);
	
	/**
	 * after the file was parsed re-start the timer.  this gets called always, where as
	 * OnWorkingCacheComplete does not get called when the file contains invalid syntax (no
	 * symbol table could be created)
	 */
	void OnActionComplete(t4p::ActionEventClass& event);

	/**
	 * This method will start re-parsing the document in the background. this will allow the 
	 * code completion to be up-to-date after a file is *opened*
	 */
	void OnAppFileOpened(t4p::CodeControlEventClass& event);

	/**
	 * This method will start re-parsing the document in the background. this will allow the 
	 * code completion to be up-to-date after a file is *reverted*. The logic is a bit 
	 * different than OnAppFileOpened, when a file is reverted we will re-tag the file and 
	 * rebuild the symbiol table, while when we open a file we dont need to re-tag the file
	 * because it has not changed.
	 */
	void OnAppFileReverted(t4p::CodeControlEventClass& event);

	/**
	 * when the tag cache has been searched, display the matching tags in the dialog so that
	 * the user can choose which to open
	 */
	void OnTagCacheSearchComplete(t4p::TagCacheSearchCompleteEventClass& event);

	/**
	 * when the user clicks on a editor hyperlink, take them to the matches php tag.
	 */
	void OnCodeControlHotspotClick(wxStyledTextEvent& event);
	
	/**
	 * when the timer expires, start a re-parsing of the newest content in the current
	 * code control.  we want to refresh the symbol tables
	 */
	void OnTimerComplete(wxTimerEvent& event);
	
	/**
	 * This method will get called by the WorkingCacheBuilderClass when parsing of the
	 * code in this control has been completed.
	 */
	void OnWorkingCacheComplete(t4p::WorkingCacheCompleteEventClass& event);
	
	void OnAppStartSequenceComplete(wxCommandEvent& event);
	
	/**
	 * most application logic is stored here.
	 */
	t4p::TagFeatureClass& Feature;
	
	/**
	 * a timer to parse the currently opened file from time to time
	 * that way we can get the newest variable names
	 */
	wxTimer Timer;

	/**
	 * The "Index project" menu item
	 * @var wxMenuItem* 
	 */
	wxMenuItem* ProjectIndexMenu;

	/**
	 * A more prominent status bar that indicated progress
	 * this dialog will only be alive while project is being indexed
	 */
	t4p::GaugeDialogClass* IndexingDialog;
	
	DECLARE_EVENT_TABLE()
};

/**
 * the tag searching will be done in a background thread.
 */
class TagSearchDialogClass : public TagSearchDialogGeneratedClass {
public:

	/**
	 * @param parent the parent window
	 * @param globals to get the project list
	 * @param cacheStatus the string that describes the cache state (shown to the user)
	 * @param term string to prepopulate the input box
	 * @param chosenResources out parameter, the list of resources that the user chose
	 */
	TagSearchDialogClass(wxWindow* parent, 
		t4p::GlobalsClass& globals, wxString cacheStatus, wxString& term, 
		std::vector<t4p::TagClass>& chosenResources);
	
	~TagSearchDialogClass();

	/**
	 * @param text to place in the search text
	 * @param matches to place in the results list
	 */
	void Prepopulate(const wxString& text, const std::vector<t4p::TagClass>& matches);

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
	void ShowJumpToResults(const wxString& finderQuery, const std::vector<t4p::TagClass>& matches);

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
	void SearchForResources(const wxString& text, std::vector<t4p::ProjectClass*> projects);

	/**
	 * when the timer completes, perform the search
	 */
	void OnTimerComplete(wxTimerEvent& event);

	/**
	 * when the tag cache has been searched, display the matching tags in the dialog so that
	 * the user can choose which to open
	 */
	void OnTagCacheSearchComplete(t4p::TagCacheSearchCompleteEventClass& event);

	/**
	 * use our own thread to handle search for tag results in the backgroudn
	 */
	t4p::RunningThreadsClass RunningThreads;

	/**
	 * The tag feature reference.  The dialog will use this reference to actually perform the search.
	 * 
	 * @var TagFeatureClass
	 */
	t4p::GlobalsClass& Globals;

	/**
	 * List that will get populated with the files to be opened. These are the
	 * files that the user selects.
	 */
	std::vector<t4p::TagClass>& ChosenResources;

	/**
	 * results of the most recent search
	 */
	std::vector<t4p::TagClass> MatchedResources;

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

	DECLARE_EVENT_TABLE()
};

}
#endif // __RESOURCEFEATURECLASS_H__
