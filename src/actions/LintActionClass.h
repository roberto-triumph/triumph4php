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
 * @copyright  2014 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef __T4PLINTACTIONCLASS_H__
#define __T4PLINTACTIONCLASS_H__

#include <actions/ActionClass.h>
#include <language/PhpVariableLintClass.h>
#include <language/PhpIdentifierLintClass.h>
#include <language/LintDirectoryWalkerClass.h>
#include <language/LintSuppressionClass.h>
#include <language/PhpFunctionCallLintClass.h>
#include <language/TagCacheClass.h>
#include <search/DirectorySearchClass.h>

namespace t4p {
	
// forward declaration, defined in another file
class GlobalsClass;

/**
 * event that will get generated when a PHP file contains a lint error
 * 1 lint error event is generated per file.
 */
extern const wxEventType EVENT_LINT_ERROR;

/**
 * event that gets generated at the end of a directory lint; it contains the 
 * number of files that were found to have errors
 */
extern const wxEventType EVENT_LINT_SUMMARY;

/**
 * the maximum amount of errored files to tolerate.
 * Any more files than this and we risk having too
 * many error instances in memory. Also, there is no
 * way the user is going through every single file
 */
extern const int LINT_ACTION_MAX_ERROR_FILES;

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
    wxStaticCastEvent( LintResultsEventClassFunction, & fn ), (wxObject *) NULL ),
	
typedef void (wxEvtHandler::*LintResultsSummaryEventClassFunction)(LintResultsSummaryEventClass&);

#define EVT_LINT_SUMMARY(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_LINT_SUMMARY, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( LintResultsSummaryEventClassFunction, & fn ), (wxObject *) NULL ),


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
		const t4p::LintOptionsClass& options, const wxFileName& suppressionFile);
	
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
		const t4p::LintOptionsClass& options, const wxFileName& suppressionFile);
	
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

}

#endif // __T4PLINTACTIONCLASS_H__
