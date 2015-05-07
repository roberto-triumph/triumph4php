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
#include <features/LintFeatureClass.h>
#include <globals/String.h>
#include <globals/Errors.h>
#include <globals/Assets.h>

#include <Triumph.h>
#include <globals/Events.h>
#include <globals/Number.h>
#include <unicode/unistr.h>
#include <wx/tokenzr.h>
#include <wx/ffile.h>

/**
 * any files bigger than this size will NOT be
 * linted.  This is because linters build ASTs
 * and those can get really big for big files
 */
const static int MAX_PARSE_FILESIZE = 5 * 1024 * 1024; // 5 MB

/**
 * the maximum amount of errored files to tolerate.
 * Any more files than this anf we risk having too
 * many error instances in memory. Also, there is 
 * way the user is going through every single file
 */
const static int MAX_LINT_ERROR_FILES = 100;


/**
 * puts all of the given projects' source directories into the given map.
 * 
 * @param projects the projects to grab the sources from
 * @param map the destination of the source directories
 */
static void SourcesToMap(const std::vector<t4p::ProjectClass>& projects, std::map<wxString, int>& map) {
	std::vector<t4p::ProjectClass>::const_iterator project;
	std::vector<t4p::SourceClass>::const_iterator source;
	
	for (project = projects.begin(); project != projects.end(); ++project) {
		for (source = project->Sources.begin(); source != project->Sources.end(); ++source) {
			map[source->RootDirectory.GetPathWithSep()] = 1;
		}
	}
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


t4p::ParserDirectoryWalkerClass::ParserDirectoryWalkerClass(const t4p::LintFeatureOptionsClass& options,
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
																		const t4p::LintFeatureOptionsClass& options,
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
		if (ParserDirectoryWalker.WithErrors > MAX_LINT_ERROR_FILES) {
			
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
																		const t4p::LintFeatureOptionsClass& options,
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

t4p::LintFeatureClass::LintFeatureClass(t4p::AppClass& app) 
	: FeatureClass(app)
	, Options()
	, LintErrors()
	, VariableResults()
	, IdentifierResults() {
}

void t4p::LintFeatureClass::LoadPreferences(wxConfigBase* config) {
	config->Read(wxT("/LintCheck/CheckOnSave"), &Options.CheckOnSave);
	config->Read(wxT("/LintCheck/CheckUninitializedVariables"), &Options.CheckUninitializedVariables);
	config->Read(wxT("/LintCheck/CheckUnknownIdentifiers"), &Options.CheckUnknownIdentifiers);
	config->Read(wxT("/LintCheck/CheckGlobalScopeVariables"), &Options.CheckGlobalScopeVariables);
	config->Read(wxT("/LintCheck/CheckFunctionArgumentCount"), &Options.CheckFunctionArgumentCount);
}

void t4p::LintFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {
	wxConfigBase* config = wxConfig::Get();
	config->Write(wxT("/LintCheck/CheckOnSave"), Options.CheckOnSave);
	config->Write(wxT("/LintCheck/CheckUninitializedVariables"), Options.CheckUninitializedVariables);
	config->Write(wxT("/LintCheck/CheckUnknownIdentifiers"), Options.CheckUnknownIdentifiers);
	config->Write(wxT("/LintCheck/CheckGlobalScopeVariables"), Options.CheckGlobalScopeVariables);
	config->Write(wxT("/LintCheck/CheckFunctionArgumentCount"), Options.CheckFunctionArgumentCount);
}

void t4p::LintFeatureClass::OnProjectsRemoved(t4p::ProjectEventClass& event) {
	
	// when a project is removed then remove any suppression rules
	// that mention the projects' sources
	// question: what do we do when two different projects have the same
	// source; but only one of the projects is removed? do we remove the
	// suppression rule ? we only want to remove the rule if the
	// rule directory is mentioned in the removed projects AND it is 
	// not mentioned in any of the existing projects
	std::map<wxString, int> removedSources;
	SourcesToMap(event.Projects, removedSources);
	
	std::map<wxString, int> existingSources;
	SourcesToMap(App.Globals.Projects, existingSources);
	
	// see which removed soures are not in the projects that have remained
	std::map<wxString, int>::iterator oldSource;
	std::map<wxString, int> toRemove;
	for (oldSource = removedSources.begin(); oldSource != removedSources.end(); ++oldSource) {
		if (existingSources.find(oldSource->first) == existingSources.end()) {
			toRemove[oldSource->first] = 1;
		}
	}
	
	// finally, open the suppression rules and remove the old sources
	// don't care about rule loading errors
	t4p::LintSuppressionClass suppressions;
	std::vector<UnicodeString> errors;
	wxFileName suppressionFile = t4p::LintSuppressionsFileAsset();
	suppressions.Init(suppressionFile, errors);
	
	std::map<wxString, int>::const_iterator remove;
	for (remove = toRemove.begin(); remove != toRemove.end(); ++remove) {
		
		// remove the skip-all rules for the vendor dir (they were added
		// when the project was created)
		wxFileName fn;
		fn.AssignDir(remove->first);
		fn.AppendDir(wxT("vendor"));
		suppressions.RemoveRulesForDirectory(fn);
	}
	
	if (!suppressions.Save(suppressionFile)) {
		t4p::EditorLogWarning(t4p::ERR_INVALID_FILE, suppressionFile.GetFullPath());
	}
	
}

void t4p::LintFeatureClass::OnProjectsUpdated(t4p::ProjectEventClass& event) {
	
	// when a project is added/updated then add any suppression rules
	// that mention the projects' vendor directories.
	// we add automatic suppression to the vendor directories because
	// that's where composer puts the library code; and users 
	// will most likely not want to run lint checks on libs.
	//
	// question: what do we do when two different projects have the same
	// source? we only want to make at most 1 rule for a directory
	std::map<wxString, int> updatedSources;
	SourcesToMap(event.Projects, updatedSources);
	
	std::map<wxString, int> existingSources;
	SourcesToMap(App.Globals.Projects, existingSources);
	
	// see which updated sources have a composer  dir(vendor)
	std::map<wxString, int> vendorDirs;
	std::map<wxString, int>::iterator updatedSource;
	for (updatedSource = updatedSources.begin(); updatedSource != updatedSources.end(); ++updatedSource) {
		if (wxFileName::IsDirReadable(updatedSource->first + wxT("vendor"))) {
			vendorDirs[updatedSource->first + wxT("vendor")] = 1;
		}
	}
	
	// now add a suppression rule ONLY if the vendor dir is not already
	// in a rule
	// don't care about rule loading errors
	t4p::LintSuppressionClass suppressions;
	std::vector<UnicodeString> errors;
	wxFileName suppressionFile = t4p::LintSuppressionsFileAsset();
	suppressions.Init(suppressionFile, errors);
	
	std::map<wxString, int>::iterator vendorDir;
	bool addedRule = false;
	for (vendorDir = vendorDirs.begin(); vendorDir != vendorDirs.end(); ++vendorDir) {
		wxFileName fn;
		fn.AssignDir(vendorDir->first);
		if (suppressions.AddSkipAllRuleForDirectory(fn)) {
			addedRule = true;
		}
	}	
	
	if (addedRule && !suppressions.Save(suppressionFile)) {
		t4p::EditorLogWarning(t4p::ERR_INVALID_FILE, suppressionFile.GetFullPath());
	}
}

void t4p::LintFeatureClass::OnProjectCreated(wxCommandEvent& event) {
	
	// same as when projects are updated; add the vendor dir
	// to the suppression list
	wxFileName fn;
	fn.AssignDir(event.GetString());
	fn.AppendDir(wxT("vendor"));
	if (fn.DirExists()) {
		
		// don't care about rule loading errors
		t4p::LintSuppressionClass suppressions;
		std::vector<UnicodeString> errors;
		wxFileName suppressionFile = t4p::LintSuppressionsFileAsset();
		suppressions.Init(suppressionFile, errors);

		bool addedRule = suppressions.AddSkipAllRuleForDirectory(fn);
		if (addedRule && !suppressions.Save(suppressionFile)) {
			t4p::EditorLogWarning(t4p::ERR_INVALID_FILE, suppressionFile.GetFullPath());
		}
	}
}

t4p::LintFeatureOptionsClass::LintFeatureOptionsClass()
: CheckOnSave(true)
, CheckUninitializedVariables(true)
, CheckUnknownIdentifiers(false)
, CheckGlobalScopeVariables(false) {

}

t4p::LintFeatureOptionsClass::LintFeatureOptionsClass(const t4p::LintFeatureOptionsClass& src)
: CheckOnSave(true)
, CheckUninitializedVariables(true)
, CheckUnknownIdentifiers(false)
, CheckGlobalScopeVariables(false) {
	Copy(src);
}

t4p::LintFeatureOptionsClass& t4p::LintFeatureOptionsClass::operator=(const t4p::LintFeatureOptionsClass& src)  {
	Copy(src);
	return *this;
}

void t4p::LintFeatureOptionsClass::Copy(const t4p::LintFeatureOptionsClass& src) {
	CheckOnSave = src.CheckOnSave;
	CheckUninitializedVariables = src.CheckUninitializedVariables;
	CheckUnknownIdentifiers = src.CheckUnknownIdentifiers;
	CheckGlobalScopeVariables = src.CheckGlobalScopeVariables;
	CheckFunctionArgumentCount = src.CheckFunctionArgumentCount;
}

const wxEventType t4p::EVENT_LINT_ERROR = wxNewEventType();
const wxEventType t4p::EVENT_LINT_SUMMARY = wxNewEventType();


BEGIN_EVENT_TABLE(t4p::LintFeatureClass, wxEvtHandler) 
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::LintFeatureClass::OnPreferencesSaved)
	EVT_APP_PROJECTS_UPDATED(t4p::LintFeatureClass::OnProjectsUpdated)
	EVT_APP_PROJECTS_REMOVED(t4p::LintFeatureClass::OnProjectsRemoved)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PROJECT_CREATED, t4p::LintFeatureClass::OnProjectCreated)
END_EVENT_TABLE()
