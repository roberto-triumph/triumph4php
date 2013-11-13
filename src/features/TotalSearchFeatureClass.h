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
#ifndef __MVCEDITOR_TOTALSEARCHFEATURECLASS_H__
#define __MVCEDITOR_TOTALSEARCHFEATURECLASS_H__

#include <features/FeatureClass.h>
#include <features/wxformbuilder/TotalSearchFeatureForms.h>
#include <actions/TotalTagSearchActionClass.h>
#include <actions/ActionClass.h>
#include <wx/timer.h>

namespace mvceditor {

/**
 * This feature allows the user to search for any PHP class,
 * function, file, or database table.
 * The class will search most of the tables in the tag cache
 * schema.
 */
class TotalSearchFeatureClass : public mvceditor::FeatureClass {

public:

	TotalSearchFeatureClass(mvceditor::AppClass& app);

	void AddSearchMenuItems(wxMenu* searchMenu);
	
	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);

private:

	void OnTotalSearch(wxCommandEvent& event);
	
	void OpenFileTag(const mvceditor::FileTagClass& fileTag, int lineNumber);
	
	void OpenPhpTag(const mvceditor::TagClass& tag);
	
	void OpenDbTable(const mvceditor::DatabaseTableTagClass& tableTag);
	
	void OpenDbData(const mvceditor::DatabaseTableTagClass& tableTag);

	DECLARE_EVENT_TABLE()
};


/**
 * Class that allows the user to search for tags. It
 * will search as the user types in the query, providing 
 * instant feedback
 */
class TotalSearchDialogClass : public TotalSearchDialogGeneratedClass {

public:

	TotalSearchDialogClass(wxWindow* parent, mvceditor::TotalSearchFeatureClass& feature,
		std::vector<mvceditor::TotalTagResultClass>& selectedTags, int& lineNumber);

private:
	
	void OnCancelButton(wxCommandEvent& event);
	void OnHelpButton(wxCommandEvent& event);
	void OnMatchesListKeyDown(wxKeyEvent& event);
	void OnMatchesListDoubleClick(wxListEvent& event);
	void OnOkButton(wxCommandEvent& event);
	void OnProjectChoice(wxCommandEvent& event);
	void OnSearchEnter(wxCommandEvent& event);
	void OnSearchKeyDown(wxKeyEvent& event);
	void OnSearchComplete(mvceditor::TotalTagSearchCompleteEventClass& event);

	void OnTimer(wxTimerEvent& event);
	
	void ChooseSelectedAndEnd(size_t selected);
	
	mvceditor::TotalSearchFeatureClass& Feature;
	
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
	mvceditor::RunningThreadsClass RunningThreads;
	
	/**
	 * results from the last compeleted search 
	 */
	std::vector<mvceditor::TotalTagResultClass> Results;
	
	/**
	 * The tags that were selected by the user
	 */
	std::vector<mvceditor::TotalTagResultClass>& SelectedTags;
	
	/**
	 * the line number to jump to (given by the user)
	 */
	int& LineNumber;
	
	enum Images {
		PHP_CLASS,
		PHP_METHOD,
		PHP_FUNCTION,
		FILE_PHP,
		FILE_SQL,
		FILE_CSS,
		FILE_MISC,
		DB_TABLE_DATA,
		DB_TABLE_DEFINITION
	};
	
	DECLARE_EVENT_TABLE()
};

}

#endif
