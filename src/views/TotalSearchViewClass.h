/**
 * The MIT License
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
#ifndef T4P_TOTALSEARCHVIEWCLASS_H
#define T4P_TOTALSEARCHVIEWCLASS_H

#include <views/FeatureViewClass.h>
#include <features/TotalSearchFeatureClass.h>
#include <views/wxformbuilder/TotalSearchFeatureForms.h>
#include <wx/timer.h>

namespace t4p {

/**
 * The total search view will show the user a dialog
 * that implements "search as you type" functionality.
 * The search happens in the background, so that typing
 * into the search box does not feel laggy.
 */
class TotalSearchViewClass : public t4p::FeatureViewClass {

public:

	TotalSearchViewClass(t4p::TotalSearchFeatureClass& feature);

	void AddSearchMenuItems(wxMenu* searchMenu);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);

private:

	void OnTotalSearch(wxCommandEvent& event);

	t4p::TotalSearchFeatureClass& Feature;

	DECLARE_EVENT_TABLE()
};


/**
 * Class that allows the user to search for tags. It
 * will search as the user types in the query, providing
 * instant feedback
 */
class TotalSearchDialogClass : public TotalSearchDialogGeneratedClass {

public:

	TotalSearchDialogClass(wxWindow* parent, t4p::TotalSearchFeatureClass& feature,
		std::vector<t4p::TotalTagResultClass>& selectedTags, int& lineNumber);

private:

	void OnCancelButton(wxCommandEvent& event);
	void OnHelpButton(wxCommandEvent& event);
	void OnMatchesListKeyDown(wxKeyEvent& event);
	void OnMatchesListDoubleClick(wxCommandEvent& event);
	void OnOkButton(wxCommandEvent& event);
	void OnSearchEnter(wxCommandEvent& event);
	void OnSearchKeyDown(wxKeyEvent& event);
	void OnSearchComplete(t4p::TotalTagSearchCompleteEventClass& event);

	/**
	 * update the cache status label and internal flags
	 */
	void UpdateCacheStatus();

	void OnTimer(wxTimerEvent& event);

	void ChooseSelectedAndEnd(size_t selected);

	t4p::TotalSearchFeatureClass& Feature;

	/**
	 * The item last searched; we wont attempt to search
	 * the same thing twice consecutively since it will produce
	 * the same result
	 */
	wxString LastSearch;

	/**
	 * we will perform  a search every time the timer goes
	 * off.
	 */
	wxTimer Timer;

	/**
	 * we will perform the search in a background thread
	 * that way the user does not feel a slowdown while
	 * typing in thq query
	 */
	t4p::RunningThreadsClass RunningThreads;

	/**
	 * results from the last compeleted search
	 */
	std::vector<t4p::TotalTagResultClass> Results;

	/**
	 * The tags that were selected by the user
	 */
	std::vector<t4p::TotalTagResultClass>& SelectedTags;

	/**
	 * the line number to jump to (given by the user)
	 */
	int& LineNumber;

	/**
	 * if TRUE, the cache is being built (background tag cache
	 * action is running.  We don't want to query the tag
	 * cache in this case, since ti results in GUI lockups
	 * since our read query will be locked by the writes
	 */
	bool IsCacheBeingBuilt;

	/**
	 * if TRUE, the cache is empty. informational message is
	 * shown
	 */
	bool IsCacheEmpty;

	DECLARE_EVENT_TABLE()
};

}

#endif
