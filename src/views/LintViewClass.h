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

#ifndef __LINTVIEWCLASS_H__
#define __LINTVIEWCLASS_H__

#include <vector>
#include <views/FeatureViewClass.h>
#include <globals/Events.h>
#include <views/wxformbuilder/LintFeatureForms.h>

namespace t4p {

// forward declaration, defined below
class LintViewClass;

/**
 * This panel will show the list of lint errors.
 */
class LintResultsPanelClass : public LintResultsGeneratedPanelClass {
	
public:

	LintResultsPanelClass(wxWindow *parent, int id, t4p::LintFeatureClass& feature, 
		t4p::LintViewClass& lintView, wxWindow* topWindow);
	
	/**
	 * @param doEnable TRUE if the button should be enabled
	 */
	void EnableRunButton(bool doEnable);
	
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
	void PrintSummary(int totalFiles, int errorFiles, int skippedFiles);

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
	
	void OnRunButton(wxCommandEvent& event);
	
	void OnHelpButton(wxCommandEvent& event);
	
	void OnSuppressionButton(wxCommandEvent& event);
	
	void OnErrorContextMenu(wxDataViewEvent& event);
	
	void OnCopyFile(wxCommandEvent& event);
	
	void OnCopyError(wxCommandEvent& event);
	
	void OnAddSuppression(wxCommandEvent& event);

private:

	t4p::LintFeatureClass& Feature;
	
	t4p::LintViewClass& View;
	
	/**
	 * to display the help dialog centered on the screen
	 * and not just centered on the lint results panel
	 */
	wxWindow* TopWindow;

	int TotalFiles;

	int ErrorFiles;
	
	int SkippedFiles;
	
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
class LintViewClass : public FeatureViewClass {
	
public:
	
	t4p::LintFeatureClass& Feature;

	LintViewClass(t4p::LintFeatureClass& feature);

	void AddPreferenceWindow(wxBookCtrlBase* parent);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);
	
	/**
	 * start a lint projects action. only 1 action at a time can be started.
	 * (doesn't make sense to run multiple lints simulatenously)
	 */
	void StartLint();
	
	void ShowSuppressionPanel();
	
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

	void OnHelpClick(wxCommandEvent& event);
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
	void OnKeyDown(wxKeyEvent& event);
	
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
