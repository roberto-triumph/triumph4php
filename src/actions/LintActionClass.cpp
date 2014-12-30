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
#include <actions/LintActionClass.h>
#include <globals/GlobalsClass.h>
#include <language/TagFinderList.h>
#include <globals/Assets.h>

/**
 * Set the global cache using the default settings (from globals). After a call
 * to this method, the cache is available for use by 
 * the ExpressionCompletionMatches and ResourceMatches methods
 * This method clones data structures where necessary, so that this
 * TagCache can be used from a separate thread than where globals
 * resides
 */
static void RegisterDefaultTagCache(t4p::GlobalsClass& globals, t4p::TagCacheClass& tagCache) {
	t4p::TagFinderListClass* cache = new t4p::TagFinderListClass;
	cache->InitGlobalTag(globals.TagCacheDbFileName, globals.FileTypes.GetPhpFileExtensions(), globals.FileTypes.GetMiscFileExtensions(),
		globals.Environment.Php.Version);
	cache->InitNativeTag(t4p::NativeFunctionsAsset());
	cache->InitDetectorTag(globals.DetectorCacheDbFileName);
	tagCache.RegisterGlobal(cache);
}

t4p::LintResultsEventClass::LintResultsEventClass(int eventId, const std::vector<pelet::LintResultsClass>& lintResults)
	: wxEvent(eventId, t4p::EVENT_LINT_ERROR) 
	, LintResults(lintResults) {
}

wxEvent* t4p::LintResultsEventClass::Clone() const {
	t4p::LintResultsEventClass* cloned = new t4p::LintResultsEventClass(GetId(), LintResults);
	return cloned;
}

t4p::LintResultsSummaryEventClass::LintResultsSummaryEventClass(int eventId, int totalFiles, int errorFiles, 
		int skippedFiles)
	: wxEvent(eventId, t4p::EVENT_LINT_SUMMARY)
	, TotalFiles(totalFiles)
	, ErrorFiles(errorFiles)
	, SkippedFiles(skippedFiles) {

}

wxEvent* t4p::LintResultsSummaryEventClass::Clone() const {
	return new t4p::LintResultsSummaryEventClass(GetId(), TotalFiles, ErrorFiles, SkippedFiles);
}

t4p::LintActionClass::LintActionClass(t4p::RunningThreadsClass& runningThreads, 
																		int eventId,
																		const t4p::LintOptionsClass& options,
																		const wxFileName& suppressionFile)
	: ActionClass(runningThreads, eventId)
	, TagCache()
	, ParserDirectoryWalker(options, suppressionFile)
	, Sources()
	, Search()
	, FilesCompleted(0) 
	, FilesTotal(0) {
		
}

bool t4p::LintActionClass::InitDirectoryLint(std::vector<t4p::SourceClass> sources,
																 t4p::GlobalsClass& globals) {
	bool good = false;
	Sources = sources;
	RegisterDefaultTagCache(globals, TagCache);
	ParserDirectoryWalker.SetVersion(globals.Environment.Php.Version);
	ParserDirectoryWalker.ResetTotals();
	ParserDirectoryWalker.Init(TagCache);
	
	SetStatus(_("Lint Check"));
	SetProgressMode(t4p::ActionClass::DETERMINATE);
	good = true;
	return good;
}

void t4p::LintActionClass::BackgroundWork() {
	
	if (Search.Init(Sources, t4p::DirectorySearchClass::PRECISE)) {
		FilesCompleted = 0;
		FilesTotal = Search.GetTotalFileCount();
		SetStatus(_("Lint Check"));
		IterateDirectory();
	}
	
	if (!IsCancelled()) {
		
		// send an event with summary of errors totals
		int totalFiles = ParserDirectoryWalker.WithErrors + ParserDirectoryWalker.WithNoErrors;
		int errorFiles = ParserDirectoryWalker.WithErrors;
		int skippedFiles = ParserDirectoryWalker.WithSkip;
		t4p::LintResultsSummaryEventClass summaryEvent(GetEventId(), totalFiles, errorFiles, skippedFiles);
		PostEvent(summaryEvent);
	}
}	

void t4p::LintActionClass::IterateDirectory() {
	while (!IsCancelled() && Search.More()) {
		bool error = Search.Walk(ParserDirectoryWalker);
		std::vector<pelet::LintResultsClass> lintErrors = ParserDirectoryWalker.GetLastErrors();
		if (error && !lintErrors.empty()) {
			t4p::LintResultsEventClass lintResultsEvent(GetEventId(), lintErrors);
			PostEvent(lintResultsEvent);
		}
		
		// we will try to send at most 100 events, this is in case we have big
		// projects with 10,000+ files we dont want to flood the system with events
		// that will barely be noticeable in the gauge.
		FilesCompleted++;
		double newProgress = (FilesCompleted * 1.0) / FilesTotal;
		int newProgressWhole = (int)floor(newProgress * 100);

		// we dont want to send the progress=0 event more than once
		if (newProgressWhole < 1) {
			newProgressWhole = 1;
		}
		SetPercentComplete(newProgressWhole);
		if (ParserDirectoryWalker.WithErrors > t4p::LINT_ACTION_MAX_ERROR_FILES) {
			
			// too many files with errors, something is not
			// right, just exit so that we don't
			// attempt to show the user thousands of errors
			break;
		}
	}
}

wxString t4p::LintActionClass::GetLabel() const {
	return wxT("Lint Directories");
}

t4p::LintBackgroundSingleFileClass::LintBackgroundSingleFileClass(t4p::RunningThreadsClass& runningThreads, 
																		int eventId,
																		const t4p::LintOptionsClass& options,
																		const wxFileName& suppressionFile)
	: ActionClass(runningThreads, eventId)
	, FileName()
	, TagCache()
	, ParserDirectoryWalker(options, suppressionFile) {
		
}

bool t4p::LintBackgroundSingleFileClass::Init(const wxFileName& fileName, t4p::GlobalsClass& globals) {
	
	bool good = false;
	if (globals.FileTypes.HasAPhpExtension(fileName.GetFullPath())) {
		
		// need to be thread safe and deep clone 
		FileName.Assign(fileName.GetFullPath());
		RegisterDefaultTagCache(globals, TagCache);
		ParserDirectoryWalker.SetVersion(globals.Environment.Php.Version);
		ParserDirectoryWalker.ResetTotals();
		ParserDirectoryWalker.Init(TagCache);
		
		if (!globals.IsAPhpSourceFile(fileName.GetFullPath())) {
			
			// when a file is not inside of a project, it will probably contain
			// functions and classes that are not in the tag cache; in this case
			// don't bother doing class/method/function identifier checks.
			ParserDirectoryWalker.OverrideIdentifierCheck(false);
		}
		good = true;
	}
	return good;
}

void t4p::LintBackgroundSingleFileClass::BackgroundWork() {
	bool error = ParserDirectoryWalker.Walk(FileName.GetFullPath());
	std::vector<pelet::LintResultsClass> lintErrors = ParserDirectoryWalker.GetLastErrors();
	if (error && !lintErrors.empty()) {
		t4p::LintResultsEventClass lintResultsEvent(GetEventId(), lintErrors);
		PostEvent(lintResultsEvent);
	}
}

wxString t4p::LintBackgroundSingleFileClass::GetLabel() const {
	return wxT("Lint Single File");
}

const wxEventType t4p::EVENT_LINT_ERROR = wxNewEventType();
const wxEventType t4p::EVENT_LINT_SUMMARY = wxNewEventType();
const int t4p::LINT_ACTION_MAX_ERROR_FILES = 100;