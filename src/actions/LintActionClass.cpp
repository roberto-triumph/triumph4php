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
#include <wx/ffile.h>

/**
 * any files bigger than this size will NOT be
 * linted.  This is because linters build ASTs
 * and those can get really big for big files
 */
const static int MAX_PARSE_FILESIZE = 5 * 1024 * 1024; // 5 MB

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

t4p::LintOptionsClass::LintOptionsClass()
: CheckOnSave(true)
, CheckUninitializedVariables(true)
, CheckUnknownIdentifiers(false)
, CheckGlobalScopeVariables(false) {

}

t4p::LintOptionsClass::LintOptionsClass(const t4p::LintOptionsClass& src)
: CheckOnSave(true)
, CheckUninitializedVariables(true)
, CheckUnknownIdentifiers(false)
, CheckGlobalScopeVariables(false) {
	Copy(src);
}

t4p::LintOptionsClass& t4p::LintOptionsClass::operator=(const t4p::LintOptionsClass& src)  {
	Copy(src);
	return *this;
}

void t4p::LintOptionsClass::Copy(const t4p::LintOptionsClass& src) {
	CheckOnSave = src.CheckOnSave;
	CheckUninitializedVariables = src.CheckUninitializedVariables;
	CheckUnknownIdentifiers = src.CheckUnknownIdentifiers;
	CheckGlobalScopeVariables = src.CheckGlobalScopeVariables;
	CheckFunctionArgumentCount = src.CheckFunctionArgumentCount;
}

t4p::ParserDirectoryWalkerClass::ParserDirectoryWalkerClass(const t4p::LintOptionsClass& options,
		const wxFileName& suppressionFile) 
: WithErrors(0)
, WithNoErrors(0) 
, WithSkip(0)
, Options(options)
, Parser() 	
, VariableLinterOptions()
, VariableLinter()
, IdentifierLinter()
, CallLinter()
, SuppressionFile(suppressionFile)
, Suppressions()
, HasLoadedSuppressions(false)
, LastResults()
, VariableResults()
, IdentifierResults()
, CallResults()
{
	VariableLinterOptions.CheckGlobalScope = Options.CheckGlobalScopeVariables;
}

void t4p::ParserDirectoryWalkerClass::Init(t4p::TagCacheClass& tagCache) {
	VariableLinter.Init(tagCache);
	IdentifierLinter.Init(tagCache);
	CallLinter.Init(tagCache);
}

void t4p::ParserDirectoryWalkerClass::OverrideIdentifierCheck(bool doIdentifierCheck) {
	Options.CheckUnknownIdentifiers = doIdentifierCheck;
}

void t4p::ParserDirectoryWalkerClass::ResetTotals() {
	WithErrors = 0;
	WithNoErrors = 0;
	WithSkip = 0;
}

void t4p::ParserDirectoryWalkerClass::SetVersion(pelet::Versions version) {
	Parser.SetVersion(version);
	VariableLinterOptions.Version = version;
	VariableLinter.SetOptions(VariableLinterOptions);
	IdentifierLinter.SetVersion(version);
	CallLinter.SetVersion(version);
}

bool t4p::ParserDirectoryWalkerClass::Walk(const wxString& fileName) {
	bool ret = false;
	LastResults.Error = UNICODE_STRING_SIMPLE("");
	LastResults.LineNumber = 0;
	LastResults.CharacterPosition = 0;
	VariableResults.clear();
	IdentifierResults.clear();
	CallResults.clear();
	
	wxFileName fileToCheck(fileName);
	
	// skip files that re really big
	// we don't want to build a huge AST
	if (fileToCheck.GetSize() > MAX_PARSE_FILESIZE) {
		WithSkip++;
		return ret;
	}
	
	
	// load suppressions if we have not done so
	// doing it here to prevent file reads in the foreground thread
	if (!HasLoadedSuppressions && SuppressionFile.FileExists()) {
		std::vector<UnicodeString> loadErrors; // not sure how to propagate these errors
		Suppressions.Init(SuppressionFile, loadErrors);
		HasLoadedSuppressions = true;
	}
	else if (!HasLoadedSuppressions) {

		// no file== don't bother trying to load the suppression file
		// (so that we don't generate file not found errors)
		HasLoadedSuppressions = true;
	}
	
	// check to see if the all suppressions for a file are 
	// suppressed. if so, then no need to parse the file
	
	UnicodeString target; // for the "all" suppression, target is not needed
	if (Suppressions.ShouldIgnore(fileToCheck, target, t4p::SuppressionRuleClass::SKIP_ALL)) {
		return ret;
	}

	// file is not in suppressions, the user wants to see errors
	wxFFile file(fileName, wxT("rb"));
	bool hasErrors = false;
	if (!Parser.LintFile(file.fp(), t4p::WxToIcu(fileName), LastResults)) {
		hasErrors = true;
	}
	if (Options.CheckUninitializedVariables) {
		if (VariableLinter.ParseFile(fileToCheck, VariableResults)) {
			hasErrors = true;
		}
	}
	if (Options.CheckUnknownIdentifiers) {
		if (IdentifierLinter.ParseFile(fileToCheck, IdentifierResults)) {
			hasErrors = true;
		}
	}
	if (Options.CheckFunctionArgumentCount) {
		if (CallLinter.ParseFile(fileToCheck, CallResults)) {
			hasErrors = true;
		}
	}

	if (!hasErrors) {
		WithNoErrors++;
	}
	else {
		WithErrors++;
		ret = true;
	}
	return ret;
}

std::vector<pelet::LintResultsClass> t4p::ParserDirectoryWalkerClass::GetLastErrors() {
	
	// did the file have syntax errors? these are not
	// suppressable
	std::vector<pelet::LintResultsClass> allResults;
	if (!LastResults.Error.isEmpty()) {
		allResults.push_back(LastResults);
	}
	
	for (size_t i = 0; i < VariableResults.size(); ++i) {
		t4p::PhpVariableLintResultClass variableResult = VariableResults[i];
		
		// did the user supress uninitialized variable results?
		wxFileName wxf(t4p::IcuToWx(variableResult.File));
		if (!Suppressions.ShouldIgnore(wxf, variableResult.VariableName, 
			t4p::SuppressionRuleClass::SKIP_UNINITIALIZED_VAR)) {
				
			pelet::LintResultsClass lintResult;
			lintResult.Error = UNICODE_STRING_SIMPLE("Uninitialized variable ") + variableResult.VariableName;
			lintResult.File = t4p::IcuToChar(variableResult.File);
			lintResult.UnicodeFilename = variableResult.File;
			lintResult.LineNumber = variableResult.LineNumber;
			lintResult.CharacterPosition = variableResult.Pos;
			allResults.push_back(lintResult);
		}
	}
	for (size_t i = 0; i < IdentifierResults.size(); ++i) {
		
		t4p::PhpIdentifierLintResultClass identifierResult = IdentifierResults[i];
		
		// did the user supress unkown class/method/function results?
		wxFileName wxf(t4p::IcuToWx(identifierResult.File));
		bool suppressedClass = Suppressions.ShouldIgnore(
			wxf, identifierResult.Identifier,  t4p::SuppressionRuleClass::SKIP_UNKNOWN_CLASS
		);
		bool suppressedMethod = Suppressions.ShouldIgnore(
			wxf, identifierResult.Identifier,  t4p::SuppressionRuleClass::SKIP_UNKNOWN_METHOD
		);
		bool suppressedFunction = Suppressions.ShouldIgnore(
			wxf, identifierResult.Identifier,  t4p::SuppressionRuleClass::SKIP_UNKNOWN_FUNCTION
		);
		bool suppressedProperty = Suppressions.ShouldIgnore(
			wxf, identifierResult.Identifier,  t4p::SuppressionRuleClass::SKIP_UNKNOWN_PROPERTY
		);	
		
		bool isSuppressed = false;
		pelet::LintResultsClass lintResult;
		if (t4p::PhpIdentifierLintResultClass::UNKNOWN_CLASS == identifierResult.Type) {
			lintResult.Error = UNICODE_STRING_SIMPLE("Unknown class ") + identifierResult.Identifier;
			isSuppressed = suppressedClass;
		}
		else if (t4p::PhpIdentifierLintResultClass::UNKNOWN_METHOD == identifierResult.Type) {
			lintResult.Error = UNICODE_STRING_SIMPLE("Unknown method ") + identifierResult.Identifier;
			isSuppressed = suppressedMethod;
		}
		else if (t4p::PhpIdentifierLintResultClass::UNKNOWN_FUNCTION == identifierResult.Type) {
			lintResult.Error = UNICODE_STRING_SIMPLE("Unknown function ") + identifierResult.Identifier;
			isSuppressed = suppressedFunction;
		}
		else if (t4p::PhpIdentifierLintResultClass::UNKNOWN_PROPERTY == identifierResult.Type) {
			lintResult.Error = UNICODE_STRING_SIMPLE("Unknown property ") + identifierResult.Identifier;
			isSuppressed = suppressedProperty;
		}
		
		if (!isSuppressed) {
			lintResult.File = t4p::IcuToChar(identifierResult.File);
			lintResult.UnicodeFilename = identifierResult.File;
			lintResult.LineNumber = identifierResult.LineNumber;
			lintResult.CharacterPosition = identifierResult.Pos;
			allResults.push_back(lintResult);
		}
	}
	
	for (size_t i = 0; i < CallResults.size(); ++i) {
		t4p::PhpFunctionCallLintResultClass callResult = CallResults[i];
		
		// did the user supress function argument count mismatch errors?
		wxFileName wxf(t4p::IcuToWx(callResult.File));
		if (!Suppressions.ShouldIgnore(wxf, callResult.Identifier, 
			t4p::SuppressionRuleClass::SKIP_FUNCTION_ARGUMENT_MISMATCH)) {
				
			pelet::LintResultsClass lintResult;
			lintResult.File = t4p::IcuToChar(callResult.File);
			lintResult.UnicodeFilename = callResult.File;
			lintResult.LineNumber = callResult.LineNumber;
			lintResult.CharacterPosition = callResult.Pos;
			
			int minCapacity = 500;
			int32_t written = 0;
			UChar* buf = lintResult.Error.getBuffer(minCapacity);
			if (callResult.Type == t4p::PhpFunctionCallLintResultClass::TOO_FEW_ARGS) {
				written = u_sprintf(buf,
					"Missing arguments to function `%S`: expected %d but calling with %d arguments",
					callResult.Identifier.getTerminatedBuffer(),
					callResult.ExpectedCount,
					callResult.ActualCount
				);
			}
			else if (callResult.Type == t4p::PhpFunctionCallLintResultClass::TOO_MANY_ARGS) {
				written = u_sprintf(buf,
					"Too many arguments to function `%S`: expected %d but calling with %d arguments",
					callResult.Identifier.getTerminatedBuffer(),
					callResult.ExpectedCount,
					callResult.ActualCount
				);
			}
			lintResult.Error.releaseBuffer(written);
			
			allResults.push_back(lintResult);
		}
	}
	return allResults;
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
	TagCache.RegisterDefault(globals);
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
		TagCache.RegisterDefault(globals);
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