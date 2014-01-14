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

#ifndef __LINTFEATURECLASS_H__
#define __LINTFEATURECLASS_H__

#include <vector>
#include <features/FeatureClass.h>
#include <globals/Events.h>
#include <actions/ActionClass.h>
#include <search/DirectorySearchClass.h>
#include <features/BackgroundFileReaderClass.h>
#include <features/wxformbuilder/LintFeatureForms.h>
#include <language/PhpVariableLintClass.h>
#include <language/PhpIdentifierLintClass.h>

namespace mvceditor {

// forward declaration, defined below
class LintFeatureClass;

const wxEventType EVENT_LINT_ERROR = wxNewEventType();
const wxEventType EVENT_LINT_SUMMARY = wxNewEventType();

/**
 * One EVENT_LINT_ERROR event will be generated once a PHP lint error has been encountered.
 * See pelet::ParserClass about LintResultClass instances.
 * An event will be generated only on errors; a clean file will not generate any errors.
 */
class LintResultsEventClass : public wxEvent {

public:

	/**
	 * The results for a single file. there could be multiple errors, undefined
	 * functions, uninitialized variables.
	 */
	std::vector<pelet::LintResultsClass> LintResults;

	LintResultsEventClass(int eventId, const std::vector<pelet::LintResultsClass>& lintResults);

	wxEvent* Clone() const;
};

class LintResultsSummaryEventClass : public wxEvent {

public:

	/**
	 * the number of files that were parsed
	 */
	int TotalFiles;

	/**
	 * the number of files with at lest one error
	 */
	int ErrorFiles;

	LintResultsSummaryEventClass(int eventId, int totalFiles, int errorFiles);

	wxEvent* Clone() const;

};

typedef void (wxEvtHandler::*LintResultsEventClassFunction)(LintResultsEventClass&);

#define EVT_LINT_ERROR(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_LINT_ERROR, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( LintResultsEventClassFunction, & fn ), (wxObject *) NULL ),
	
typedef void (wxEvtHandler::*LintResultsSummaryEventClassFunction)(LintResultsSummaryEventClass&);

#define EVT_LINT_SUMMARY(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_LINT_SUMMARY, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( LintResultsSummaryEventClassFunction, & fn ), (wxObject *) NULL ),

/** 
 * This class will help in parsing the large project. It will enable access
 * to DirectorySearch and easily parse many files.
 */
class ParserDirectoryWalkerClass : public DirectoryWalkerClass {
public:

	ParserDirectoryWalkerClass(mvceditor::TagCacheClass& tagCache);
	
	/**
	 * This is the method where the parsing will take place. Will return true
	 * if and only if there is a parse error.  
	 */
	virtual bool Walk(const wxString& file);

	/**
	 * set the file counters (WithErrors, WitNoErrors) back to zero
	 */
	void ResetTotals();
	
	/**
	 * @param list of wildcard strings to perform lint checks on. Each item in the list 
	 *        is a wilcard suitable for passing into the wxMatchWild() function
	 * @param ignoreFilters regex string of files to ignore. This is a string suitable
	 *        for the FindInFilesClass::CreateFilesFilterRegEx() method.
	 */
	void SetFilters(std::vector<wxString> includeFilters, wxString ignoreFilters);
	
	/**
	 * Set the version of PHP to lint against
	 */
	void SetVersion(pelet::Versions version);
	
	/**
	 * The last parsing results.
	 */
	std::vector<pelet::LintResultsClass> GetLastErrors();

	/**
	 * Running count of files that had parse errors.
	 */
	int WithErrors;
	
	/**
	 * Running count of files that had zero parse errors.
	 */
	int WithNoErrors;
	
private:

	// linters to perform different kinds of checks
	pelet::ParserClass Parser;
	mvceditor::PhpVariableLintOptionsClass VariableLinterOptions;
	mvceditor::PhpVariableLintClass VariableLinter;
	mvceditor::PhpIdentifierLintClass IdentifierLinter;

	// the results for each linter
	pelet::LintResultsClass LastResults;
	std::vector<mvceditor::PhpVariableLintResultClass> VariableResults;
	std::vector<mvceditor::PhpIdentifierLintResultClass> IdentifierResults;
	
};

/**
 * This class will allow us to perform parsing in a background thread.
 * 
 */
class LintBackgroundFileReaderClass : public BackgroundFileReaderClass {

public:	

	/**
	 * @param runningThreads the object that will receive LINT_ERROR events
	 * 	      as well as WORK_* events
	 * @see mvceditor::ThreadWithHeartbeat class
	 */
	LintBackgroundFileReaderClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);
	
	/**
	 * prepare to lint a list of directories
	 *
	 * @param sources the locations and include/exclude wildcards of files that need to be parsed.  Parsing will
	 *        be recursive (sub directories will be parsed also).
	 * @param globals to know which PHP version to check against, and to get the location of the tag cache files
	 * @return bool TRUE if sources is not empty
	 */
	bool InitDirectoryLint(std::vector<mvceditor::SourceClass> sources, mvceditor::GlobalsClass& globals);

	/**
	 * prepare to lint a list a single file
	 *
	 * @param fileName the full path of the file to lint
	 * @param globals nto know which PHP version to check against, and to get the location of the tag cache files
	 * @return TRUE if the filename is OK (a valid file name, PHP extension)
	 */
	bool InitSingleFileLint(const wxFileName& fileName, mvceditor::GlobalsClass& globals);

	/**
	 * Return a summary of the number of files that were lint'ed.
	 * Only use this method after the EVENT_ACTION_COMPLETE event is dispatched.
	 * (ie don't call this while the background thread is running).
	 *
	 * @param totalFiles the number of files checked will be set here
	 * @param erroFiles the number of files with lint errors will be set here
	 */
	void LintTotals(int& totalFiles, int& errorFiles);

	wxString GetLabel() const;
	
protected:

	/**
	 * Will parse the current file. 
	 * @return TRUE if file had ZERO parse errors
	 */
	bool BackgroundFileRead(DirectorySearchClass& search);
	
	/**
	 * 
	 * will do nothing for now
	 */
	bool BackgroundFileMatch(const wxString& file);

private:

	// needed by PhpIdentifierLintClass in order to find
	// function/class names
	mvceditor::TagCacheClass TagCache;

	ParserDirectoryWalkerClass ParserDirectoryWalker;
};

/**
 * This panel will show the list of lint errors.
 */
class LintResultsPanelClass : public LintResultsGeneratedPanelClass {
	
public:

	LintResultsPanelClass(wxWindow *parent, int id, NotebookClass* notebook, mvceditor::LintFeatureClass& feature);
	
	/**
	 * adds to the list box widget AND the global list
	 */
	void AddErrors(const std::vector<pelet::LintResultsClass>& lintErrors);
	
	/**
	 * deletes from the list box widget AND the parseResults data structure
	 */
	void ClearErrors();

	/**
	 * deletes from the list box widget AND the parseResults data structure all of
	 * the results for the given file.
	 */
	void RemoveErrorsFor(const wxString& fileName);

	/**
	 * Marks up the source code control window with the error that is located
	 * at the given index.  For example; if given index is 0 then the first lint 
	 * result file (added via AddError()) . This method will NOT scroll the errored
	 * line into view.
	 */
	void ShowLintError(int index);

	/**
	 * Marks up the source code control window with the error that is located
	 * at the given index.  For example; if given index is 0 then the first lint 
	 * result file (added via AddError()) will be opened, scrolled to the lint error line, and the 
	 * editor will be marked up. This method WILL scroll the errored
	 * line into view.
	 */
	void GoToAndDisplayLintError(int index);

	/**
	 * Will highlight the next error (from the one that is currently selected) in the lint results list AND 
	 * will markup the source control window appropriately.
	 */
	void SelectNextError();

	/**
	 * Will highlight the previous error (from the one that is currently selected) in the lint results list AND 
	 * will markup the source control window appropriately.
	 */
	void SelectPreviousError();

	/**
	 * Add the file counts to a label.
	 */
	void PrintSummary(int totalFiles, int errorFiles);

	/**
	 * updates the file count label based on errors that have been fixed or new errors that 
	 * have been introduced
	 */
	void UpdateSummary();
	
	virtual void OnListDoubleClick(wxCommandEvent& event);

private:

	NotebookClass* Notebook;

	mvceditor::LintFeatureClass& Feature;

	int TotalFiles;

	int ErrorFiles;
};

/**
 * This is a panel that tells the user that there is a syntax error
 * somewhere outside the visible part of a file.
 */
class LintErrorPanelClass : public LintErrorGeneratedPanelClass {

public:

	LintErrorPanelClass(mvceditor::CodeControlClass* parent, int id, const std::vector<pelet::LintResultsClass>& results);

private:

	void OnKeyDown(wxKeyEvent& event);
	void OnGoToLink(wxHyperlinkEvent& event);
	void OnDismissLink(wxHyperlinkEvent& event);

	void DoDestroy();

	/**
	 * to position the cursor at the spot of the error
	 * this class will not own the pointer
	 */
	mvceditor::CodeControlClass* CodeControl;

	/**
	 * the lint error to show
	 */
	std::vector<pelet::LintResultsClass> LintResults;
};

/**
 * This is the class that will manage all of the UI buttons and menus
 * for the feature
 */
class LintFeatureClass : public FeatureClass {
	
public:
	
	/**
	 * If TRUE, then when a file is saved; a lint check on that file
	 * will be performed.
	 */
	bool CheckOnSave;

	/**
	 * errors that have been encountered so far.
	 */
	std::vector<pelet::LintResultsClass> LintErrors;
	std::vector<mvceditor::PhpVariableLintResultClass> VariableResults;
	std::vector<mvceditor::PhpIdentifierLintResultClass> IdentifierResults;

	LintFeatureClass(mvceditor::AppClass& app);

	void AddPreferenceWindow(wxBookCtrlBase* parent);

	void LoadPreferences(wxConfigBase* config);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);

protected: 

	void AddViewMenuItems(wxMenu* viewMenu);
	
	void AddToolBarItems(wxAuiToolBar* toolBar);
	
private:

	void OnPreferencesSaved(wxCommandEvent& event);

	void OnLintMenu(wxCommandEvent& event);

	void OnNextLintError(wxCommandEvent& event);

	void OnPreviousLintError(wxCommandEvent& event);
	
	void OnLintError(mvceditor::LintResultsEventClass& event);

	void OnLintErrorAfterSave(mvceditor::LintResultsEventClass& event);

	void OnLintFileComplete(wxCommandEvent& event);

	void OnLintComplete(mvceditor::ActionEventClass& event);

	void OnLintSummary(mvceditor::LintResultsSummaryEventClass& event);
	
	void OnLintProgress(mvceditor::ActionProgressEventClass& event);
	
	void OnFileSaved(mvceditor::CodeControlEventClass& event);
	
	void OnNotebookPageClosed(wxAuiNotebookEvent& event);	

	/**
	 * to stop the lint action if the user closes the tab
	 */
	int RunningActionId;
	
	DECLARE_EVENT_TABLE()
};

class LintPreferencesPanelClass : public LintPreferencesGeneratedPanelClass {

public:

	LintFeatureClass& Feature;

	LintPreferencesPanelClass(wxWindow* parent, LintFeatureClass& feature);
};

}

#endif
