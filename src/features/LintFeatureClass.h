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
#include <language/LintSuppressionClass.h>

namespace t4p {

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
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_LINT_ERROR, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( LintResultsEventClassFunction, & fn ), (wxObject *) NULL ),
	
typedef void (wxEvtHandler::*LintResultsSummaryEventClassFunction)(LintResultsSummaryEventClass&);

#define EVT_LINT_SUMMARY(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_LINT_SUMMARY, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( LintResultsSummaryEventClassFunction, & fn ), (wxObject *) NULL ),

/**
 * Stores flags that determine how strict linting will be 
 */
class LintFeatureOptionsClass {

public:

	/**
	 * If TRUE, then when a file is saved; a lint check on that file
	 * will be performed.
	 */
	bool CheckOnSave;

	/**
	 * if TRUE, then a check for uninitialized variables is done
	 * it will done only on variables in a function or method.
	 */
	bool CheckUninitializedVariables;

	/**
	 * if TRUE, then methods, classes will be checked for existence
	 * when they are being called.  this requires that a project 
	 * be indexes.
	 */
	bool CheckUnknownIdentifiers;

	/**
	 * if TRUE, then a check for uninitialized variables is done
	 * on global variables
	 */
	bool CheckGlobalScopeVariables;

	LintFeatureOptionsClass();

	LintFeatureOptionsClass(const t4p::LintFeatureOptionsClass& src);

	t4p::LintFeatureOptionsClass& operator=(const t4p::LintFeatureOptionsClass& src);

	void Copy(const t4p::LintFeatureOptionsClass& src);
};

/** 
 * This class will help in parsing the large project. It will enable access
 * to DirectorySearch and easily parse many files.
 */
class ParserDirectoryWalkerClass : public DirectoryWalkerClass {
public:

	ParserDirectoryWalkerClass(const t4p::LintFeatureOptionsClass& options, 
		const wxFileName& suppressionFile);
	
	/**
	 * initializes the linters  with the tag cache.  The linters use the
	 * tag cache to lookup classes, functions, methods.
	 */
	void Init(t4p::TagCacheClass& tagCache);
	
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
	 * The last parsing results; minus any errors that were suppressed due
	 * to the suppression rules.
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

	// flags that control which checks to perform
	t4p::LintFeatureOptionsClass Options;

	// linters to perform different kinds of checks
	pelet::ParserClass Parser;
	t4p::PhpVariableLintOptionsClass VariableLinterOptions;
	t4p::PhpVariableLintClass VariableLinter;
	t4p::PhpIdentifierLintClass IdentifierLinter;
	
	// to ignore files/classes/methods
	wxFileName SuppressionFile;
	t4p::LintSuppressionClass Suppressions;
	bool HasLoadedSuppressions;

	// the results for each linter
	pelet::LintResultsClass LastResults;
	std::vector<t4p::PhpVariableLintResultClass> VariableResults;
	std::vector<t4p::PhpIdentifierLintResultClass> IdentifierResults;
	
};

/**
 * This class will allow us to perform parsing on an entire directory
 * in a background thread.
 */
class LintActionClass : public t4p::ActionClass {

public:	

	/**
	 * @param runningThreads the object that will receive LINT_ERROR events
	 * 	      as well as WORK_* events
	 * @param eventId the ID of the generated events
	 * @param options flags to control how strict linter will be
	 * @param suppressionFile the full path to the lint suppressions csv file.
	 *        used to get the files/classes/methods to ignore
	 * @see t4p::ActionClass class
	 */
	LintActionClass(t4p::RunningThreadsClass& runningThreads, int eventId,
		const t4p::LintFeatureOptionsClass& options, const wxFileName& suppressionFile);
	
	/**
	 * prepare to lint a list of directories
	 *
	 * @param sources the locations and include/exclude wildcards of files that need to be parsed.  Parsing will
	 *        be recursive (sub directories will be parsed also).
	 * @param globals to know which PHP version to check against, and to get the location of the tag cache files
	 * @return bool TRUE if sources is not empty
	 */
	bool InitDirectoryLint(std::vector<t4p::SourceClass> sources, t4p::GlobalsClass& globals);

	wxString GetLabel() const;
	
protected:

	/**
	 * Will iterate through each source and parse it for errors
	 */
	void BackgroundWork();
	
private:

	/**
	 * Will iterate the entire set of files in DirectorySearch.
	 * Will send events 1) on errors and 2) progress
	 */
	void IterateDirectory();

	
	/**
	 * needed by PhpIdentifierLintClass in order to find
	 * function/class names
	 */
	t4p::TagCacheClass TagCache;

	/**
	 * performs the lint logic on a single file
	 */
	ParserDirectoryWalkerClass ParserDirectoryWalker;
	
	/**
	 * The directories to be parsed for errors
	 */
	 std::vector<t4p::SourceClass> Sources;
	 
	 /**
	  * used to recurse through directories
	  */
	 t4p::DirectorySearchClass Search;
	
	// to show progress
	int FilesCompleted;
	int FilesTotal;
};

/**
 * This class will allow us to perform parsing on a single 
 * file in a background thread.
 */
class LintBackgroundSingleFileClass : public t4p::ActionClass {

public:

	/**
	 * @param runningThreads the object that will receive LINT_ERROR events
	 * 	      as well as WORK_* events
	 * @param eventId the ID of the generated events
	 * @param options flags to control how strict linter will be
	 * @param suppressionFile the full path to the lint suppressions file
	 *        the file contains list of files/classes/methods/functions to ignore
	 * @see t4p::ActionClass class
	 */
	LintBackgroundSingleFileClass(t4p::RunningThreadsClass& runningThreads, int eventId,
		const t4p::LintFeatureOptionsClass& options, const wxFileName& suppressionFile);
	
	/**
	 * prepare to lint a list a single file
	 *
	 * @param fileName the full path of the file to lint
	 * @param globals nto know which PHP version to check against, and to get the location of the tag cache files
	 * @return TRUE if the filename is OK (a valid file name, PHP extension)
	 */
	bool Init(const wxFileName& fileName, t4p::GlobalsClass& globals);

	wxString GetLabel() const;
	
protected:

	/**
	 * Will parse the current file. 
	 */
	void BackgroundWork();
	
private:

	/**
	 * the file to parse
	 */
	wxFileName FileName;

	// needed by PhpIdentifierLintClass in order to find
	// function/class names
	t4p::TagCacheClass TagCache;

	ParserDirectoryWalkerClass ParserDirectoryWalker;
};

/**
 * This panel will show the list of lint errors.
 */
class LintResultsPanelClass : public LintResultsGeneratedPanelClass {
	
public:

	LintResultsPanelClass(wxWindow *parent, int id, NotebookClass* notebook, t4p::LintFeatureClass& feature,
		wxWindow* topWindow);
	
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
	
	/**
	 * Adds 1 to the error file count. this is used when a file is saved right 
	 * after the user sees a lint error, fixes the error, but the linter finds another
	 * error
	 */
	void IncrementErrorFileCount();
	
	void OnRowActivated(wxDataViewEvent& event);
	
	void OnHelpButton(wxCommandEvent& event);
	
	void OnErrorContextMenu(wxDataViewEvent& event);
	
	void OnCopyFile(wxCommandEvent& event);
	
	void OnCopyError(wxCommandEvent& event);
	
	void OnAddSuppression(wxCommandEvent& event);

private:

	NotebookClass* Notebook;

	t4p::LintFeatureClass& Feature;
	
	/**
	 * to display the help dialog centered on the screen
	 * and not just centered on the lint results panel
	 */
	wxWindow* TopWindow;

	int TotalFiles;

	int ErrorFiles;
	
	DECLARE_EVENT_TABLE()
};

/**
 * This is a panel that tells the user that there is a syntax error
 * somewhere outside the visible part of a file.
 */
class LintErrorPanelClass : public LintErrorGeneratedPanelClass {

public:

	LintErrorPanelClass(t4p::CodeControlClass* parent, int id, const std::vector<pelet::LintResultsClass>& results);

private:

	void OnKeyDown(wxKeyEvent& event);
	void OnGoToLink(wxHyperlinkEvent& event);
	void OnDismissLink(wxHyperlinkEvent& event);

	void DoDestroy();

	/**
	 * to position the cursor at the spot of the error
	 * this class will not own the pointer
	 */
	t4p::CodeControlClass* CodeControl;

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
	
	t4p::LintFeatureOptionsClass Options;

	/**
	 * errors that have been encountered so far. there are 3 types
	 * of errors because there are different type of linters.
	 */
	std::vector<pelet::LintResultsClass> LintErrors;
	std::vector<t4p::PhpVariableLintResultClass> VariableResults;
	std::vector<t4p::PhpIdentifierLintResultClass> IdentifierResults;

	LintFeatureClass(t4p::AppClass& app);

	void AddPreferenceWindow(wxBookCtrlBase* parent);

	void LoadPreferences(wxConfigBase* config);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);

protected: 

	void AddViewMenuItems(wxMenu* viewMenu);
	
	void AddToolBarItems(wxAuiToolBar* toolBar);
	
private:

	void OnPreferencesSaved(wxCommandEvent& event);

	void OnLintMenu(wxCommandEvent& event);
	
	void OnLintSuppressionsMenu(wxCommandEvent& event);

	void OnNextLintError(wxCommandEvent& event);

	void OnPreviousLintError(wxCommandEvent& event);
	
	void OnLintError(t4p::LintResultsEventClass& event);

	void OnLintErrorAfterSave(t4p::LintResultsEventClass& event);

	void OnLintFileComplete(wxCommandEvent& event);

	void OnLintComplete(t4p::ActionEventClass& event);

	void OnLintSummary(t4p::LintResultsSummaryEventClass& event);
	
	void OnLintProgress(t4p::ActionProgressEventClass& event);
	
	void OnFileSaved(t4p::CodeControlEventClass& event);
	
	void OnNotebookPageClosed(wxAuiNotebookEvent& event);	
	
	void OnProjectsUpdated(t4p::ProjectEventClass& event);

	void OnProjectsRemoved(t4p::ProjectEventClass& event);
	
	void OnProjectCreated(wxCommandEvent& event);

	/**
	 * to stop the lint action if the user closes the tab
	 */
	int RunningActionId;
	
	DECLARE_EVENT_TABLE()
};

/**
 * Shows the user lint settings, things that affect
 * what checks to perform
 */
class LintPreferencesPanelClass : public LintPreferencesGeneratedPanelClass {

public:

	t4p::LintFeatureClass& Feature;

	LintPreferencesPanelClass(wxWindow* parent, t4p::LintFeatureClass& feature);
	

};

/**
 * Shows the user the list of suppression rules
 */
class LintSuppressionsPanelClass : public LintSuppressionsGeneratedPanelClass {

public:

	LintSuppressionsPanelClass(wxWindow* parent, int id, wxFileName suppressionFile,
		wxWindow* topWindow);
	
private:

	// event handlers
	void OnAddButton(wxCommandEvent& event);
	void OnEditButton(wxCommandEvent& event);
	void OnDeleteButton(wxCommandEvent& event);
	void OnDeleteAllButton(wxCommandEvent& event);
	void OnHelpButton(wxCommandEvent& event);
	void OnRowActivated(wxDataViewEvent& event);
	
	/**
	 * refresh viewable list from the suppressions file
	 */
	void PopulateList();
	
	/**
	 * adds the given rule to the list control
	 */
	void AppendRuleToList(const t4p::SuppressionRuleClass& rule);
	
	/**
	 * saves the suppression rules to the suppressions
	 * file
	 */
	void SaveList();
	
	/**
	 * the location of the suppressions file
	 */
	wxFileName SuppressionFile;
	
	/**
	 * the list of rules
	 */
	t4p::LintSuppressionClass Suppressions;
	
	/**
	 * errors when loading suppressions
	 */
	std::vector<UnicodeString> Errors;
	
	/**
	 * to display the help dialog centered on the screen
	 * and not just centered on the lint results panel
	 */
	wxWindow* TopWindow;
	
	DECLARE_EVENT_TABLE()
};

/**
 * Shows the user a form to add or edit a suppression rule
 */
class LintSuppressionRuleDialogClass : public LintSuppressionRuleGeneratedDialogClass {
	
public:

	LintSuppressionRuleDialogClass(wxWindow* parent, int id, t4p::SuppressionRuleClass& rule);
	
private:

	// events to prevent invalid inputs
	void OnTypeChoice(wxCommandEvent& event);
	void OnDirectoryRadio(wxCommandEvent& event);
	void OnFileRadio(wxCommandEvent& event);
	void OnOkButton(wxCommandEvent& event);
	
	/**
	 * the rule that will get modified once the user clicks OK
	 */
	t4p::SuppressionRuleClass& Rule;
	
	/**
	 * the rule being edited
	 */
	t4p::SuppressionRuleClass EditRule;
};

}

#endif
