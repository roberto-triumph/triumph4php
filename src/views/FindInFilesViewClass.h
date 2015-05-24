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
#ifndef T4P_FINDINFFILESVIEWCLASS_H__
#define T4P_FINDINFFILESVIEWCLASS_H__

#include <views/wxformbuilder/FindInFilesFeatureForms.h>
#include <features/FindInFilesFeatureClass.h>
#include <widgets/ComboBoxHistoryClass.h>
#include <views/FeatureViewClass.h>
#include <vector>

namespace t4p {

// forward declaration, defined below
class FindInFilesViewClass;

/** Implementing FindInFilesPanelGeneratedClass */
class FindInFilesResultsPanelClass : public FindInFilesResultsPanelGeneratedClass {
protected:
	// Handlers for FindInFilesResultsPanelClass events.
	void OnReplaceButton(wxCommandEvent& event);
	void OnReplaceAllInFileButton(wxCommandEvent& event);
	void OnReplaceInAllFilesButton(wxCommandEvent& event);
	void OnFileHit(t4p::FindInFilesHitEventClass& event);
	void OnStopButton(wxCommandEvent& event);
	void OnRowActivated(wxDataViewEvent& event);
	void OnCopySelectedButton(wxCommandEvent& event);
	void OnCopyAllButton(wxCommandEvent& event);
	void OnFindInFilesComplete(wxCommandEvent& event);
	void OnNextHitButton(wxCommandEvent& event);
	void OnPreviousHitButton(wxCommandEvent& event);
	void OnRegExReplaceHelpButton(wxCommandEvent& event);
	void OnReplaceTextEnter(wxCommandEvent& event);

		
public:

	/**
	 * Construct a new FindInFilesResultsPanelClass 
	 * 
	 * @param wxWindow* parent the parent window
	 * @param view the view class gives us access to the opened files, so that we can search their contents
	 * @param StatusBarWithGaugeClass* gauge the object used to create gauge for showing progress. The pointer will NOT be managed 
	 *        (deleted) by this class. 
	 * @param runningThreads manages the background search thread
	 */
	FindInFilesResultsPanelClass(wxWindow* parent, t4p::FindInFilesViewClass& view, StatusBarWithGaugeClass* gauge,
		t4p::RunningThreadsClass& runningThreads);
	
	~FindInFilesResultsPanelClass();
	
	/**
	 * Start a file search.
	 * 
	 * @param FindInFilesClass findInFiles the search expression
	 * @param wxString path the directory to search in
	 * @param bool if TRUE then hidden files will be searched
	 */
	void Find(const FindInFilesClass& findInFiles, bool doHiddenFiles);
	
	/**
	 * Stops a currently running search. It will clean up 
	 * the gauge also, this means that it must be called while the gauge is valid (ie NOT
	 * at program end)
	 */
	void Stop();

	/**
	 * Move the cursor to the next find in files match.
	 * Will set the cursor to the position of the match, opening the file
	 * it if it's not already open.
	 */
	void ShowNextMatch();
	
	/**
	 * Move the cursor to the previous find in files match.
	 * Will set the cursor to the position of the match, opening the file
	 * it if it's not already open.
	 */
	void ShowPreviousMatch();

private:

	/**
	 * To find matches in files.
	 * 
	 * @var FindInFilesClass
	 */
	FindInFilesClass FindInFiles;

	/**
	 * The matches (results of the find)
	 */
	std::vector<t4p::FindInFilesHitClass> AllHits;

	/**
	 * keeps track of the background thread
	 */
	t4p::RunningThreadsClass& RunningThreads;
		
	/**
	 * To open the files
	 */
	t4p::FindInFilesViewClass& View;
	
	/**
	 * To display status bar to the user
	 * @var StatusBarWithGaugeClass
	 */
	StatusBarWithGaugeClass* Gauge;
	
	/**
	 * The number of files that contained at least one match.
	 * 
	 * @var int
	 */
	int MatchedFiles;

	/**
	 * The unique identifier for the gauge.
	 */
	int FindInFilesGaugeId;

	/**
	 * to stop the find in in files action if this panel is closed.
	 */
	int RunningActionId;

	/**
	 * Need to save the insertion point of the replace combo boxes; in Win32
	 * GetInsertionPoint() of combobox fails when it does not have focus.
	 * The insertion is needed to have the cursor show up properly when
	 * the user clicks on the regex help buttons
	 */
	int CurrentInsertionPointReplace;
	
	/**
	 * Enable the controls that allow the user to replace hits or stop searches.
	 * 
	 * @param bool enableStopButton if true, the stop button is enabled, else it is disabled
	 * @param bool enableReplaceButtons if true, the replace buttons are enabled, else they are disabled
	 * @param bool enableCopyButtons if true the copy to clipboard buttos are enabled, else they are disabled
	 */
	void EnableButtons(bool enableStopButton, bool enableReplaceButtons, bool enableCopyButtons);

	/**
	 * Place the given string in the results status label
	 * 
	 * @param wxString message to show to the user
	 */
	void SetStatus(const wxString& status);
			
	/**
	 * Retutns the number of files that had matches. 
	 * 
	 * @return int the number of files under FindPath that had at least one match for this Expression
	 */
	int GetNumberOfMatchedFiles();
	
	/**
	 * Timer handler.
	 * 
	 * @param t4p::ActionProgressEventClass& event
	 */
	 void OnActionProgress(t4p::ActionProgressEventClass& event);

	 /**
	  * Search in the files that are opened. We want to do this so that we only show the most up-to-date
	  * hits
	  */
	 void FindInOpenedFiles();

	/**
 	 * Shows the i'th match. Will set the cursor to the position of the match, opening the file
	 * it if it's not already open.
	 *
	 * @param i index of hit to show. i is 0-based
	 */
	void ShowMatch(int i);
	
	/**
 	 * Shows the i'th match. Will set the cursor to the position of the match, opening the file
	 * it if it's not already open. Additionally, it will scroll the ith match into
	 * view.  This is useful when the user cycles through the matches with the keyboard
	 * shortcut; the proper match is selected and shown into view
	 *
	 * @param i index of hit to show. i is 0-based
	 */
	void ShowMatchAndEnsureVisible(int i);

	void OnActionComplete(t4p::ActionEventClass& event);

	/**
	 * handling of the replace regex help menu
	 */
	void InsertReplaceRegExSymbol(wxCommandEvent& event);
	
	/**
	 * Need to save the insertion point of the Find and replace combo boxes; in Win32
	 * GetInsertionPoint() of combobox fails when it does not have focus.
	 * The insertion is needed to have the cursor show up properly when
	 * the user clicks on the regex help buttons
	 */
	void OnKillFocusReplaceText(wxFocusEvent& event);
	
	DECLARE_EVENT_TABLE()
};

class FindInFilesViewClass : public FeatureViewClass {

public:
	
	/**
	 * Holds previously entered searches. We need these to persist longer than the dialog so that
	 * the user's inputs are saved.
	 */
	ComboBoxHistoryClass FindHistory;
	
	/**
	 * Holds previously entered replacements. We need these to persist longer than the dialog so that
	 * the user's inputs are saved.
	 */
	ComboBoxHistoryClass ReplaceHistory;

	/**
	 * Holds previously entered directories to search in. We need these to persist longer than the dialog so that
	 * the user's inputs are saved.
	 */
	ComboBoxHistoryClass DirectoriesHistory;
	
	/**
	 * Holds previously entered file filters. We need these to persist longer than the dialog so that
	 * the user's inputs are saved.
	 */
	ComboBoxHistoryClass FilesHistory;
	
	/**
	 * Constructor
	 */
	FindInFilesViewClass(t4p::FindInFilesFeatureClass& app);
	
	/**
	 * Add menu items to the search menu for this feature.
	 * 
	 * @param wxMenu* menu the tools menu to add items to.
	 */
	void AddSearchMenuItems(wxMenu* searchMenu);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);

private:
	
	/**
	 * show the find dialog 
	 */
	void OnEditFindInFiles(wxCommandEvent& event);

	void OnEditFindInFilesNext(wxCommandEvent& event);

	void OnEditFindInFilesPrevious(wxCommandEvent& event);
	
	/**
	 * When a tools notebook tab is closed, if its a find in files results tab
	 * the the search will be stopped AND the gauge must be cleaned up.
	 */
	void OnToolsNotebookPageClosed(wxAuiNotebookEvent& event);
	
	t4p::FindInFilesFeatureClass& Feature;
	
	/**
	 * Safer to keep a pointer to these, rather than attempt to type cast.
	 * Because the tools notebook can contain many types of panels.
	 * When a tools notebook tab is closed, if its a find in files results tab
	 * the the search will be stopped AND the gauge must be cleaned up.
	 * Note that because these are window pointers, we wont own the pointers
	 * wxWidgets will clean them up.
	 */
	std::vector<t4p::FindInFilesResultsPanelClass*> ResultsPanels;
	
	DECLARE_EVENT_TABLE()
};

class FindInFilesDialogClass: public FindInFilesDialogGeneratedClass {
	
public:

	FindInFilesDialogClass(wxWindow* parent, FindInFilesFeatureClass& feature,
		FindInFilesViewClass& view);

protected:

	virtual void OnOkButton(wxCommandEvent& event);
	virtual void OnCancelButton(wxCommandEvent& event);
	void OnRegExFindHelpButton(wxCommandEvent& event);
	void OnRegExReplaceHelpButton(wxCommandEvent& event);
	void OnDirChanged(wxFileDirPickerEvent& event);

private:

	FindInFilesFeatureClass& Feature;
	FindInFilesViewClass& View;

	int CurrentInsertionPointFind;

	int CurrentInsertionPointReplace;
	
	/** 
	 * since this panel handles EVT_TEXT_ENTER, we need to handle the
	 * tab traversal ourselves otherwise tab travesal wont work
	 */
	void OnKeyDown(wxKeyEvent& event);
	
	/**
	 * Need to save the insertion point of the Find and replace combo boxes; in Win32
	 * GetInsertionPoint() of combobox fails when it does not have focus.
	 * The insertion is needed to have the cursor show up properly when
	 * the user clicks on the regex help buttons
	 */
	void OnKillFocusFindText(wxFocusEvent& event);
	void OnKillFocusReplaceText(wxFocusEvent& event);

	/**
	 * handle the regular expression popup menu. Will add the symbol
	 * the the expression textbox value.
	 */
	void InsertRegExSymbol(wxCommandEvent& event);

	/**
	 * handle the regular expression replace popup menu. Will add the symbol
	 * the the replace textbox value.
	 */
	void InsertReplaceRegExSymbol(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

}
#endif // __FINDINFFILESFEATURECLASS_H__
