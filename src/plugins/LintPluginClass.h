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

#ifndef __LINTPLUGINCLASS_H__
#define __LINTPLUGINCLASS_H__

#include <vector>
#include <PluginClass.h>
#include <Events.h>
#include <search/DirectorySearchClass.h>
#include <plugins/BackgroundFileReaderClass.h>
#include <plugins/wxformbuilder/LintPluginGeneratedClass.h>

namespace mvceditor {

const wxEventType EVENT_LINT_ERROR = wxNewEventType();
/**
 * One EVENT_LINT_ERROR event will be generated once a PHP lint error has been encountered.
 * See pelet::ParserClass about LintResultClass instances.
 * An event will be generated only on errors; a clean file will not generate any errors.
 */
class LintResultsEventClass : public wxEvent {

public:

	/**
	 * The results for a single file.
	 */
	pelet::LintResultsClass LintResults;

	LintResultsEventClass(const pelet::LintResultsClass& lintResults);

	wxEvent* Clone() const;
};

typedef void (wxEvtHandler::*LintResultsEventClassFunction)(LintResultsEventClass&);

#define EVT_LINT_ERROR(fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_LINT_ERROR, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( LintResultsEventClassFunction, & fn ), (wxObject *) NULL ),
	
/** 
 * This class will help in parsing the large project. It will enable access
 * to DirectorySearch and easily parse many files.
 */
class ParserDirectoryWalkerClass : public DirectoryWalkerClass {
public:

	ParserDirectoryWalkerClass();
	
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
	 * 
	 */
	pelet::LintResultsClass LastResults;
	
	/**
	 * Running count of files that had parse errors.
	 */
	int WithErrors;
	
	/**
	 * Running count of files that had zero parse errors.
	 */
	int WithNoErrors;
	
private:

	pelet::ParserClass Parser;
};

/**
 * This class will allow us to perform parsing in a background thread.
 * 
 */
class LintBackgroundFileReaderClass : public BackgroundFileReaderClass {

public:	

	/**
	 * @param handler the object that will receive LINT_ERROR events
	 * 	      as well as WORK_* events
	 * @see mvceditor::ThreadWithHeartbeat class
	 */
	LintBackgroundFileReaderClass(wxEvtHandler& handler, mvceditor::RunningThreadsClass& runningThreads);
	
	/**
	 * Start the background thread.  Lint errors will be propagated as events.

	 * @param sources the locations and include/exclude wildcards of files that need to be parsed.  Parsing will
	 *        be recursive (sub directories will be parsed also).
	 * @param environment to know which PHP version to check against
	 * @param StartError& error the reason for a failure to start will be set here.
	 * return bool TRUE if and only if the thread was started.  If false, either thread could
	 * not be started or directory is not valid. 
	 */
	bool BeginDirectoryLint(std::vector<mvceditor::SourceClass> sources, const EnvironmentClass& environment, StartError& error);

	/**
	 * Lint checks the given file in the current thread.  This is a thread-safe method;
	 * it is safe to call this even when the thread started by BeginDirectoryLint() has not
	 * finished.
	 *
	 * Lint errors will be propagated as events.
	 * @param wxString fileName the full path of the file to lint
	 * @param structs needed to perform exclude wildcard checks
	 * @param environment to know which PHP version to check against
	 * @param results the lint error, will only be filled when there is a lint error
	 * @return TRUE if the file contains a lint error.
	 */
	bool LintSingleFile(const wxString& fileName, const mvceditor::StructsClass& structs, const EnvironmentClass& environment,
		pelet::LintResultsClass& results);

	/**
	 * Return a summary of the number of files that were lint'ed.
	 * Only use this method after the EVENT_WORK_COMPLETE event is dispatched.
	 * (ie dont cll this while the background thread is running).
	 *
	 * @param totalFiles the number of files checked will be set here
	 * @param erroFiles the number of files with lint errors will be set here
	 */
	void LintTotals(int& totalFiles, int& errorFiles);
	
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

	ParserDirectoryWalkerClass ParserDirectoryWalker;
};

/**
 * This panel will show the list of lint errors.
 */
class LintResultsPanelClass : public LintResultsGeneratedPanelClass {
	
public:

	LintResultsPanelClass(wxWindow *parent, int id, NotebookClass* notebook, std::vector<pelet::LintResultsClass>& lintErrors);

	~LintResultsPanelClass();
	
	/**
	 * adds to the list box widget AND the parseResults data structure
	 */
	void AddError(const pelet::LintResultsClass& lintError);
	
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
	 * result file (added via AddError()) will be opened, scrolled to the lint error line, and the 
	 * editor will be marked up.
	 */
	void DisplayLintError(int index);

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

	virtual void OnListDoubleClick(wxCommandEvent& event);

private:

	NotebookClass* Notebook;

	std::vector<pelet::LintResultsClass>& LintErrors;
};

/**
 * This is the class that will manage all of the UI buttons and menus
 * for the plugin
 */
class LintPluginClass : public PluginClass {
	
public:
	
	/**
	 * If TRUE, then when a file is saved; a lint check on that file
	 * will be performed.
	 */
	bool CheckOnSave;

	LintPluginClass(mvceditor::AppClass& app);
	
	~LintPluginClass();

	void AddPreferenceWindow(wxBookCtrlBase* parent);

	void LoadPreferences(wxConfigBase* config);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);

protected: 

	void AddViewMenuItems(wxMenu* viewMenu);
	
	void AddToolBarItems(wxAuiToolBar* toolBar);
	
private:

	void SavePreferences(wxCommandEvent& event);

	void OnLintMenu(wxCommandEvent& event);

	void OnNextLintError(wxCommandEvent& event);

	void OnPreviousLintError(wxCommandEvent& event);
	
	void OnLintError(mvceditor::LintResultsEventClass& event);

	void OnLintFileComplete(wxCommandEvent& event);

	void OnLintComplete(wxCommandEvent& event);
	
	void OnTimer(wxCommandEvent& event);
	
	void OnFileSaved(mvceditor::FileSavedEventClass& event);
	
	void OnNotebookPageClosed(wxAuiNotebookEvent& event);	

	LintBackgroundFileReaderClass LintBackgroundFileReader;

	/**
	 * This will hold all info about parse errors.
	 */
	std::vector<pelet::LintResultsClass> LintErrors;
	
	/**
	 * The panel that shows the lint errors. Note that this window
	 * may be deleted by the user at any time, be careful that
	 * you check that it is not null before accessing it
	 */
	mvceditor::LintResultsPanelClass* ResultsPanel;
	
	DECLARE_EVENT_TABLE()
};

class LintPluginPreferencesPanelClass : public LintPluginPreferencesGeneratedPanelClass {

public:

	LintPluginClass& Plugin;

	LintPluginPreferencesPanelClass(wxWindow* parent, LintPluginClass& plugin);
};

}

#endif
