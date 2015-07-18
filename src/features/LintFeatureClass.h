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

#ifndef __LINTFEATURECLASS_H
#define __LINTFEATURECLASS_H

#include <vector>
#include <features/FeatureClass.h>
#include <globals/Events.h>
#include <actions/ActionClass.h>
#include <search/DirectorySearchClass.h>
#include <features/BackgroundFileReaderClass.h>
#include <language_php/PhpVariableLintClass.h>
#include <language_php/PhpIdentifierLintClass.h>
#include <language_php/LintSuppressionClass.h>
#include <language_php/PhpFunctionCallLintClass.h>
#include <language_php/TagCacheClass.h>

namespace t4p {

// forward declaration, defined below
class LintFeatureClass;

extern const wxEventType EVENT_LINT_ERROR;
extern const wxEventType EVENT_LINT_SUMMARY;

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
	 * the number of files with at least one error
	 */
	int ErrorFiles;

	/**
	 * the number of files that were skipped due to
	 * being larger than normal
	 */
	int SkippedFiles;

	LintResultsSummaryEventClass(int eventId, int totalFiles, int errorFiles, int skippedFiles);

	wxEvent* Clone() const;

};

typedef void (wxEvtHandler::*LintResultsEventClassFunction)(LintResultsEventClass&);

#define EVT_LINT_ERROR(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_LINT_ERROR, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent(LintResultsEventClassFunction, & fn), (wxObject *) NULL),

typedef void (wxEvtHandler::*LintResultsSummaryEventClassFunction)(LintResultsSummaryEventClass&);

#define EVT_LINT_SUMMARY(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_LINT_SUMMARY, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent(LintResultsSummaryEventClassFunction, & fn), (wxObject *) NULL),

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
	 * be indexed.
	 */
	bool CheckUnknownIdentifiers;

	/**
	 * if TRUE, then a check is done to make sure that the arguments
	 * in a function/method call match the number of arguments of the
	 * function/method declaration.  this requires that a project be indexed.
	 */
	bool CheckFunctionArgumentCount;

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
	 * @param bool to set the 'CheckUnknownIdentifiers' flag on the lint options
	 *        this method is useful to override the user's choice when we know
	 *        that perforning identifiers would result in many false positives;
	 *        for example when the user saves a file that exists outside of a
	 *        project. Such files use classes/methods/functions that are not in the
	 *        tag cache.
	 */
	void OverrideIdentifierCheck(bool doIdentifierCheck);

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

	/**
	 * Running count of files that were skipped because they
	 * are too big.
	 */
	int WithSkip;

private:

	// flags that control which checks to perform
	t4p::LintFeatureOptionsClass Options;

	// linters to perform different kinds of checks
	pelet::ParserClass Parser;
	t4p::PhpVariableLintOptionsClass VariableLinterOptions;
	t4p::PhpVariableLintClass VariableLinter;
	t4p::PhpIdentifierLintClass IdentifierLinter;
	t4p::PhpFunctionCallLintClass CallLinter;

	// to ignore files/classes/methods
	wxFileName SuppressionFile;
	t4p::LintSuppressionClass Suppressions;
	bool HasLoadedSuppressions;

	// the results for each linter
	pelet::LintResultsClass LastResults;
	std::vector<t4p::PhpVariableLintResultClass> VariableResults;
	std::vector<t4p::PhpIdentifierLintResultClass> IdentifierResults;
	std::vector<t4p::PhpFunctionCallLintResultClass> CallResults;

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
 * This is the class that will manage all of the UI buttons and menus
 * for the feature
 */
class LintFeatureClass : public FeatureClass {

public:

	t4p::LintFeatureOptionsClass Options;

	/**
	 * errors that have been encountered so far. there are 4 types
	 * of errors because there are different type of linters.
	 */
	std::vector<pelet::LintResultsClass> LintErrors;
	std::vector<t4p::PhpVariableLintResultClass> VariableResults;
	std::vector<t4p::PhpIdentifierLintResultClass> IdentifierResults;
	std::vector<t4p::PhpFunctionCallLintResultClass> CallResults;

	LintFeatureClass(t4p::AppClass& app);

	void LoadPreferences(wxConfigBase* config);

private:

	void OnPreferencesSaved(wxCommandEvent& event);

	void OnProjectsUpdated(t4p::ProjectEventClass& event);

	void OnProjectsRemoved(t4p::ProjectEventClass& event);

	void OnProjectCreated(wxCommandEvent& event);


	DECLARE_EVENT_TABLE()
};

}

#endif
