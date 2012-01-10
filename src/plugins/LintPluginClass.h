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
#include <search/DirectorySearchClass.h>
#include <plugins/BackgroundFileReaderClass.h>
#include <plugins/wxformbuilder/LintPluginGeneratedClass.h>

namespace mvceditor {

/**
 * This event will get dispatched when a lint (syntax) error is encountered.  It will
 * be filled with the following info:
 * event.getClientData(): this is a pointer to a mvceditor::LintResultsClass instance.  
 * the event handler MUST delete the pointer!
 */
const wxEventType EVENT_LINT_ERROR = wxNewEventType();
	
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
	 * The last parsing results.
	 * 
	 */
	LintResultsClass LastResults;
	
	/**
	 * Running count of files that had parse errors.
	 */
	int WithErrors;
	
	/**
	 * Running count of files that had zero parse errors.
	 */
	int WithNoErrors;
	
private:

	mvceditor::ParserClass Parser;

	/**
	 * The file extensions we want to attempt to parse.
	 * Each item in the list is a wilcard suitable for passing into the wxMatchWild()
	 * function
	 */
	std::vector<wxString> PhpFileFilters;

	/**
	 * Regular expression that, when matched; will result in the file being
	 * skipped (won't be lint checked). IgnoreFileFiltersRegEx take precedence over the
	 * PhpFileFilters (if a file matches both, then file will be ignored).
	 */
	wxRegEx IgnoreFileFiltersRegEx;

	/**
	 * The 'string' version of the Ignore files reg ex. We will keep the string version
	 * around so that we can test for empty strings.
	 */
	wxString IgnoreFileFilters;
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
	LintBackgroundFileReaderClass(wxEvtHandler& handler);
	
	/**
	 * Start the background thread.  Lint errors will be propagated as events.
	 * @param wxString directory the location of files that need to be parsed.  Parsing will
	 *        be recursive (sub directories will be parsed also).
	 * @param wxString phpFileFilters a list of wildcard filters of files to parse. See wxIsWild() and
	 *        wxMatchWild() for description of valid wildcards (only '*' and '?' are wildcards).
	 * @param wxString ignoreFileFiltersRegEx regular expression of files to IGNORE. This 
	 *        overrides the phpFileFilters; if a file matches ignoreFileFilters and phpFileFilters then
	 *        the file will be ignored. 
	 * @param StartError& error the reason for a failure to start will be set here.
	 * return bool TRUE if and only if the thread was started.  If false, either thread could
	 * not be started or directory is not valid. 
	 */
	bool BeginDirectoryLint(const wxString& directory, const std::vector<wxString>& phpFileFilters, const wxString& ignoreFileFiltersRegEx, StartError& error);

	/**
	 * Lint checks the given file in the current thread.  This is a thread-safe method;
	 * it is safe to call this even when the thread started by BeginDirectoryLint() has not
	 * finished.
	 *
	 * Lint errors will be propagated as events.
	 * @param wxString phpFileFilters a list of wildcard filters of files to parse. See wxIsWild() and
	 *        wxMatchWild() for description of valid wildcards (only '*' and '?' are wildcards).
	 * @param wxString ignoreFileFiltersRegEx regular expression of files to IGNORE. This 
	 *        overrides the phpFileFilters; if a file matches ignoreFileFilters and phpFileFilters then
	 *        the file will be ignored. 
	 * @return TRUE if the file contains a lint error.
	 */
	bool LintSingleFile(const wxString& fileName, const std::vector<wxString>& phpFileFilters, const wxString& ignoreFileFiltersRegEx);

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
	bool FileRead(DirectorySearchClass& search);
	
	/**
	 * 
	 * will do nothing for now
	 */
	bool FileMatch(const wxString& file);
	

private:

	ParserDirectoryWalkerClass ParserDirectoryWalker;
			
	/**
	 * The file extensions we want to attempt to parse.
	 * Each item in the list is a wilcard suitable for passing into the wxMatchWild()
	 * function
	 */
	std::vector<wxString> PhpFileFilters;

	/**
	 * This is a list of wilcards that, when matched; will result in the file being
	 * skipped (won't be lint checked).
	 */
	std::vector<wxString> IgnoreFileFilters;
};

/**
 * This panel will show the list of lint errors.
 */
class LintResultsPanelClass : public LintResultsGeneratedPanelClass {
	
public:

	LintResultsPanelClass(wxWindow *parent, int id, NotebookClass* notebook, std::vector<LintResultsClass*>& lintErrors);

	~LintResultsPanelClass();
	
	/**
	 * adds to the list box widget AND the parseResults data structure
	 * This object will own the given pointer (will delete it).
	 */
	void AddError(LintResultsClass* lintError);
	
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
	 * Add the file counts to a label.
	 */
	void PrintSummary(int totalFiles, int errorFiles);

	virtual void OnListDoubleClick(wxCommandEvent& event);

private:

	NotebookClass* Notebook;

	std::vector<LintResultsClass*>& LintErrors;
};

/**
 * This is the class that will manage all of the UI buttons and menus
 * for the plugin
 */
class LintPluginClass : public PluginClass {
	
public:

	/**
	 * A newline separated list of wildcards that will be used
	 * to ignore certain files; files matching any of the wildcards
	 * will NOT be checked.
	 * Each wildcard can have either a '*' or '?'.
	 */
	wxString IgnoreFiles;
	
	/**
	 * If TRUE, then when a file is saved; a lint check on that file
	 * will be performed.
	 */
	bool CheckOnSave;

	LintPluginClass();

	void AddPreferenceWindow(wxBookCtrlBase* parent);

	void LoadPreferences(wxConfigBase* config);

	void SavePreferences(wxConfigBase* config);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);

protected: 

	void AddProjectMenuItems(wxMenu* projectMenu);
	
	void AddToolBarItems(wxAuiToolBar* toolBar);
	
private:

	void OnLintMenu(wxCommandEvent& event);
	
	void OnLintError(wxCommandEvent& event);

	void OnLintFileComplete(wxCommandEvent& event);

	void OnLintComplete(wxCommandEvent& event);
	
	void OnTimer(wxCommandEvent& event);
	
	void OnFileSaved(FileSavedEventClass& event);

	LintBackgroundFileReaderClass LintBackgroundFileReader;

	/**
	 * This will hold all info about parse errors. This class will own 
	 * the pointers themselves as well and will delete them when they
	 * are no longer needed.
	 */
	std::vector<mvceditor::LintResultsClass*>LintErrors;
	
	DECLARE_EVENT_TABLE()
};

class LintPluginPreferencesPanelClass : public LintPluginPreferencesGeneratedPanelClass {

public:

	LintPluginClass& Plugin;

	LintPluginPreferencesPanelClass(wxWindow* parent, LintPluginClass& plugin);
};

}

#endif
