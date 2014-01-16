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

#include <MvcEditor.h>
#include <globals/Events.h>
#include <unicode/unistr.h>
#include <wx/artprov.h>
#include <wx/valgen.h>
#include <wx/tokenzr.h>
#include <wx/ffile.h>

const int ID_LINT_RESULTS_PANEL = wxNewId();
const int ID_LINT_RESULTS_GAUGE = wxNewId();
const int ID_LINT_READER = wxNewId();
const int ID_LINT_READER_SAVE = wxNewId();
const int ID_LINT_ERROR_PANEL = wxNewId();

mvceditor::LintResultsEventClass::LintResultsEventClass(int eventId, const std::vector<pelet::LintResultsClass>& lintResults)
	: wxEvent(eventId, mvceditor::EVENT_LINT_ERROR) 
	, LintResults(lintResults) {
}

wxEvent* mvceditor::LintResultsEventClass::Clone() const {
	mvceditor::LintResultsEventClass* cloned = new mvceditor::LintResultsEventClass(GetId(), LintResults);
	return cloned;
}

mvceditor::LintResultsSummaryEventClass::LintResultsSummaryEventClass(int eventId, int totalFiles, int errorFiles)
	: wxEvent(eventId, mvceditor::EVENT_LINT_SUMMARY)
	, TotalFiles(totalFiles)
	, ErrorFiles(errorFiles) {

}

wxEvent* mvceditor::LintResultsSummaryEventClass::Clone() const {
	return new mvceditor::LintResultsSummaryEventClass(GetId(), TotalFiles, ErrorFiles);
}


mvceditor::ParserDirectoryWalkerClass::ParserDirectoryWalkerClass(mvceditor::TagCacheClass& tagCache, const mvceditor::LintFeatureOptionsClass& options) 
: WithErrors(0)
, WithNoErrors(0) 
, Options(options)
, Parser() 	
, VariableLinterOptions()
, VariableLinter()
, IdentifierLinter(tagCache)
, LastResults()
, VariableResults()
, IdentifierResults()
{
	VariableLinterOptions.CheckGlobalScope = Options.CheckGlobalScopeVariables;
}

void mvceditor::ParserDirectoryWalkerClass::ResetTotals() {
	WithErrors = 0;
	WithNoErrors = 0;
}

void mvceditor::ParserDirectoryWalkerClass::SetVersion(pelet::Versions version) {
	Parser.SetVersion(version);
	VariableLinterOptions.Version = version;
	VariableLinter.SetOptions(VariableLinterOptions);
	IdentifierLinter.SetVersion(version);
}

bool mvceditor::ParserDirectoryWalkerClass::Walk(const wxString& fileName) {
	bool ret = false;
	LastResults.Error = UNICODE_STRING_SIMPLE("");
	LastResults.LineNumber = 0;
	LastResults.CharacterPosition = 0;
	VariableResults.clear();
	IdentifierResults.clear();

	wxFFile file(fileName, wxT("rb"));
	bool hasErrors = false;
	if (!Parser.LintFile(file.fp(), mvceditor::WxToIcu(fileName), LastResults)) {
		hasErrors = true;
	}
	wxFileName wxf(fileName);
	if (Options.CheckUninitializedVariables) {
		if (VariableLinter.ParseFile(wxf, VariableResults)) {
			hasErrors = true;
		}
	}
	if (Options.CheckUnknownIdentifiers) {
		if (IdentifierLinter.ParseFile(wxf, IdentifierResults)) {
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

std::vector<pelet::LintResultsClass> mvceditor::ParserDirectoryWalkerClass::GetLastErrors() {
	std::vector<pelet::LintResultsClass> allResults;
	if (!LastResults.Error.isEmpty()) {
		allResults.push_back(LastResults);
	}
	for (size_t i = 0; i < VariableResults.size(); ++i) {
		pelet::LintResultsClass lintResult;
		mvceditor::PhpVariableLintResultClass variableResult = VariableResults[i];
		lintResult.Error = UNICODE_STRING_SIMPLE("Uninitialized variable ") + variableResult.VariableName;
		lintResult.File = mvceditor::IcuToChar(variableResult.File);
		lintResult.UnicodeFilename = variableResult.File;
		lintResult.LineNumber = variableResult.LineNumber;
		lintResult.CharacterPosition = variableResult.Pos;
		allResults.push_back(lintResult);
	}
	for (size_t i = 0; i < IdentifierResults.size(); ++i) {
		pelet::LintResultsClass lintResult;
		mvceditor::PhpIdentifierLintResultClass identifierResult = IdentifierResults[i];
		if (mvceditor::PhpIdentifierLintResultClass::UNKNOWN_CLASS == identifierResult.Type) {
			lintResult.Error = UNICODE_STRING_SIMPLE("Unknown class ") + identifierResult.Identifier;
		}
		else if (mvceditor::PhpIdentifierLintResultClass::UNKNOWN_METHOD == identifierResult.Type) {
			lintResult.Error = UNICODE_STRING_SIMPLE("Unknown method ") + identifierResult.Identifier;
		}
		else if (mvceditor::PhpIdentifierLintResultClass::UNKNOWN_PROPERTY == identifierResult.Type) {
			lintResult.Error = UNICODE_STRING_SIMPLE("Unknown property ") + identifierResult.Identifier;
		}
		else if (mvceditor::PhpIdentifierLintResultClass::UNKNOWN_FUNCTION == identifierResult.Type) {
			lintResult.Error = UNICODE_STRING_SIMPLE("Unknown function ") + identifierResult.Identifier;
		}
		lintResult.File = mvceditor::IcuToChar(identifierResult.File);
		lintResult.UnicodeFilename = identifierResult.File;
		lintResult.LineNumber = identifierResult.LineNumber;
		lintResult.CharacterPosition = identifierResult.Pos;
		allResults.push_back(lintResult);
	}
	return allResults;
}

mvceditor::LintBackgroundFileReaderClass::LintBackgroundFileReaderClass(mvceditor::RunningThreadsClass& runningThreads, 
																		int eventId,
																		const mvceditor::LintFeatureOptionsClass& options)
	: BackgroundFileReaderClass(runningThreads, eventId)
	, TagCache()
	, ParserDirectoryWalker(TagCache, options) {
		
}

bool mvceditor::LintBackgroundFileReaderClass::InitDirectoryLint(std::vector<mvceditor::SourceClass> sources,
																 mvceditor::GlobalsClass& globals) {
	bool good = false;
	if (Init(sources)) {
		TagCache.RegisterDefault(globals);
		ParserDirectoryWalker.SetVersion(globals.Environment.Php.Version);
		ParserDirectoryWalker.ResetTotals();
		good = true;
	}
	return good;
}

bool mvceditor::LintBackgroundFileReaderClass::InitSingleFileLint(const wxFileName& fileName, mvceditor::GlobalsClass& globals) {
	
	bool good = false;
	if (globals.HasAPhpExtension(fileName.GetFullPath())) {
		mvceditor::SourceClass src;
		src.SetIncludeWildcards(fileName.GetFullName());
		src.RootDirectory.AssignDir(fileName.GetPath());
		std::vector<mvceditor::SourceClass> srcs;
		srcs.push_back(src);
		if (Init(srcs)) {
			TagCache.RegisterDefault(globals);
			ParserDirectoryWalker.SetVersion(globals.Environment.Php.Version);
			ParserDirectoryWalker.ResetTotals();
			good = true;
		}
	}
	return good;
}

bool mvceditor::LintBackgroundFileReaderClass::BackgroundFileRead(DirectorySearchClass &search) {
	bool error = search.Walk(ParserDirectoryWalker);
	std::vector<pelet::LintResultsClass> lintErrors = ParserDirectoryWalker.GetLastErrors();
	if (error && !lintErrors.empty()) {
		mvceditor::LintResultsEventClass lintResultsEvent(GetEventId(), lintErrors);
		PostEvent(lintResultsEvent);
	}
	if (!search.More() && !IsCancelled()) {
		int totalFiles = ParserDirectoryWalker.WithErrors + ParserDirectoryWalker.WithNoErrors;
		int errorFiles = ParserDirectoryWalker.WithErrors;
		mvceditor::LintResultsSummaryEventClass summaryEvent(GetEventId(), totalFiles, errorFiles);
		PostEvent(summaryEvent);
	}
	return !error;
}

bool mvceditor::LintBackgroundFileReaderClass::BackgroundFileMatch(const wxString& file) {
	return true;
}

void mvceditor::LintBackgroundFileReaderClass::LintTotals(int& totalFiles, int& errorFiles) {
	totalFiles = ParserDirectoryWalker.WithErrors + ParserDirectoryWalker.WithNoErrors;
	errorFiles = ParserDirectoryWalker.WithErrors;
}

wxString mvceditor::LintBackgroundFileReaderClass::GetLabel() const {
	return wxT("Lint Files");
}

mvceditor::LintResultsPanelClass::LintResultsPanelClass(wxWindow *parent, int id, mvceditor::NotebookClass* notebook,
														mvceditor::LintFeatureClass& feature)
	: LintResultsGeneratedPanelClass(parent, id) 
	, Notebook(notebook) 
	, Feature(feature)
	, TotalFiles(0)
	, ErrorFiles(0) {
			
}

void mvceditor::LintResultsPanelClass::AddErrors(const std::vector<pelet::LintResultsClass>& lintErrors) {
	std::vector<pelet::LintResultsClass>::const_iterator it;
	for (it = lintErrors.begin(); it != lintErrors.end(); ++it) {
		pelet::LintResultsClass lintError = *it;
		wxString err = mvceditor::IcuToWx(lintError.Error);
		wxString line;
		int capacity = lintError.Error.length() + lintError.UnicodeFilename.length() + 50;
		UnicodeString msg;
		int written = u_sprintf(msg.getBuffer(capacity), 
			"%.*S on %.*S line %d near Position %d\n", 
			lintError.Error.length(),
			lintError.Error.getBuffer(),
			lintError.UnicodeFilename.length(),
			lintError.UnicodeFilename.getBuffer(),
			lintError.LineNumber,
			lintError.CharacterPosition
		);
		msg.releaseBuffer(written);
		wxString wxMsg = mvceditor::IcuToWx(msg);
		Feature.LintErrors.push_back(lintError);
		ErrorsList->AppendString(wxMsg);
	}
}

void mvceditor::LintResultsPanelClass::ClearErrors() {
	ErrorsList->Clear();
	Feature.LintErrors.clear();
	Feature.VariableResults.clear();
	Feature.IdentifierResults.clear();
}

void mvceditor::LintResultsPanelClass::RemoveErrorsFor(const wxString& fileName) {
	
	// remove the lint result data structures as well as the 
	// display list
	std::vector<pelet::LintResultsClass>::iterator it = Feature.LintErrors.begin();
	int i = 0;

	UnicodeString uniFileName = mvceditor::WxToIcu(fileName);
	bool found = false;
	while (it != Feature.LintErrors.end()) {
		if (it->UnicodeFilename == uniFileName) {
			it = Feature.LintErrors.erase(it);
			if (i > 0 && ErrorsList->GetCount() < (size_t)i) {
				ErrorsList->Delete(i);
			}
			i--;
			found = true;
		}
		else {
			i++;
			it++;
		}
	}
	if (found) {
		ErrorFiles--;
		UpdateSummary();
	}
}

void mvceditor::LintResultsPanelClass::PrintSummary(int totalFiles, int errorFiles) {
	TotalFiles = totalFiles;
	ErrorFiles = errorFiles;
	UpdateSummary();
}

void mvceditor::LintResultsPanelClass::UpdateSummary() {
	if (0 == ErrorFiles) {
		this->Label->SetLabel(
			wxString::Format(_("No errors found; checked %d files"), TotalFiles)
		);
	}
	else {
		this->Label->SetLabel(
			wxString::Format(_("Found %d files with errors; checked %d files"), ErrorFiles, TotalFiles)
		);
	}
}

void mvceditor::LintResultsPanelClass::OnListDoubleClick(wxCommandEvent& event) {
	int index = event.GetInt();
	GoToAndDisplayLintError(index);
}

void mvceditor::LintResultsPanelClass::GoToAndDisplayLintError(int index) {
	pelet::LintResultsClass results = Feature.LintErrors[index];

	wxString file = mvceditor::IcuToWx(results.UnicodeFilename);
	Notebook->LoadPage(file);
	Notebook->GetCurrentCodeControl()->MarkLintErrorAndGoto(results);
}

void mvceditor::LintResultsPanelClass::SelectNextError() {
	int selected = ErrorsList->GetSelection();
	if (selected != wxNOT_FOUND && ((unsigned int)selected  + 1) < ErrorsList->GetCount()) {
		ErrorsList->SetSelection(selected + 1);
		GoToAndDisplayLintError(selected + 1);
	}
	else if (ErrorsList->GetCount() > 0) {
		ErrorsList->SetSelection(0);
		GoToAndDisplayLintError(0);
	}
}

void mvceditor::LintResultsPanelClass::SelectPreviousError() {
	int selected = ErrorsList->GetSelection();
	if (selected != wxNOT_FOUND && (selected  - 1) >= 0) {
		ErrorsList->SetSelection(selected - 1);
		GoToAndDisplayLintError(selected - 1);
	}
	else if (ErrorsList->GetCount() > 0) {
		ErrorsList->SetSelection(ErrorsList->GetCount() - 1);
		GoToAndDisplayLintError(ErrorsList->GetCount() - 1);
	}
}

void mvceditor::LintResultsPanelClass::ShowLintError(int index) {
	mvceditor::CodeControlClass* codeControl = Notebook->GetCurrentCodeControl();
	if (!codeControl) {
		return;
	}
	wxString fileName = codeControl->GetFileName();
	pelet::LintResultsClass result = Feature.LintErrors[index];
	wxString resultFile = mvceditor::CharToWx(result.File.c_str());
	if (resultFile != fileName) {
		return;
	}
	codeControl->MarkLintError(result);

	// lines from scintilla are 0 based, but lint results lines are 1-based
	int firstVisibleLine = codeControl->GetFirstVisibleLine() + 1;
	int lastVisibleLine = firstVisibleLine + codeControl->LinesOnScreen();
	if (result.LineNumber < firstVisibleLine || result.LineNumber >= lastVisibleLine) {
		
		// the error is out of view show a message, remove any other existing message
		codeControl->Freeze();
		wxWindow* old = wxWindow::FindWindowById(ID_LINT_ERROR_PANEL, codeControl);
		if (old) {
			old->Destroy();
		}
		std::vector<pelet::LintResultsClass> results;
		results.push_back(result);
		mvceditor::LintErrorPanelClass* errorPanel = new mvceditor::LintErrorPanelClass(codeControl, ID_LINT_ERROR_PANEL, results);
		wxPoint point = codeControl->PointFromPosition(codeControl->GetCurrentPos());
		point.y = point.y - errorPanel->GetSize().GetY();
		if (point.y < 0) {
			point.y = 0;
		}
		errorPanel->SetPosition(point);
		errorPanel->SetFocus();
		codeControl->Thaw();
	}
}

mvceditor::LintFeatureClass::LintFeatureClass(mvceditor::AppClass& app) 
	: FeatureClass(app)
	, Options()
	, LintErrors()
	, VariableResults()
	, IdentifierResults()
	, RunningActionId(0) {
}

void mvceditor::LintFeatureClass::AddViewMenuItems(wxMenu* viewMenu) {
	viewMenu->Append(mvceditor::MENU_LINT_PHP + 0, _("Lint Check"), _("Performs syntax check on the current project"), wxITEM_NORMAL);
	viewMenu->Append(mvceditor::MENU_LINT_PHP + 1, _("Show Next Lint Error\tF4"), _("Selects the next lint error in the code window"), wxITEM_NORMAL);
	viewMenu->Append(mvceditor::MENU_LINT_PHP + 2, _("Show Previous Lint Error\tSHIFT+F4"), _("Selects the previous lint error in the code window"), wxITEM_NORMAL);
}

void mvceditor::LintFeatureClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	wxBitmap bmp = mvceditor::IconImageAsset(wxT("lint-check"));
	toolBar->AddTool(mvceditor::MENU_LINT_PHP + 0, _("Lint Check"), bmp, _("Performs syntax check on the current project"));
}

void mvceditor::LintFeatureClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_LINT_PHP + 0] = wxT("Lint Check - Lint Check Project");
	menuItemIds[mvceditor::MENU_LINT_PHP + 1] = wxT("Lint Check - Show Next Lint Error");
	menuItemIds[mvceditor::MENU_LINT_PHP + 2] = wxT("Lint Check - Show Previous Lint Error");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::LintFeatureClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
	parent->AddPage(
		new mvceditor::LintPreferencesPanelClass(parent, *this),
		_("PHP Lint Check"));
}

void mvceditor::LintFeatureClass::LoadPreferences(wxConfigBase* config) {
	config->Read(wxT("/LintCheck/CheckOnSave"), &Options.CheckOnSave);
	config->Read(wxT("/LintCheck/CheckUninitializedVariables"), &Options.CheckUninitializedVariables);
	config->Read(wxT("/LintCheck/CheckUnknownIdentifiers"), &Options.CheckUnknownIdentifiers);
	config->Read(wxT("/LintCheck/CheckGlobalScopeVariables"), &Options.CheckGlobalScopeVariables);
}
void mvceditor::LintFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {
	wxConfigBase* config = wxConfig::Get();
	config->Write(wxT("/LintCheck/CheckOnSave"), Options.CheckOnSave);
	config->Write(wxT("/LintCheck/CheckUninitializedVariables"), Options.CheckUninitializedVariables);
	config->Write(wxT("/LintCheck/CheckUnknownIdentifiers"), Options.CheckUnknownIdentifiers);
	config->Write(wxT("/LintCheck/CheckGlobalScopeVariables"), Options.CheckGlobalScopeVariables);
}

void mvceditor::LintFeatureClass::OnLintMenu(wxCommandEvent& event) {
	if (RunningActionId > 0) {
		wxMessageBox(_("There is already another lint check that is active. Please wait for it to finish."), _("Lint Check"));
		return;
	}
	if (App.Globals.HasSources()) {
		mvceditor::LintBackgroundFileReaderClass* reader = new mvceditor::LintBackgroundFileReaderClass(App.RunningThreads, ID_LINT_READER, Options);
		std::vector<mvceditor::SourceClass> phpSources = App.Globals.AllEnabledPhpSources();

		// output an error if a source directory no longer exists
		std::vector<mvceditor::SourceClass>::const_iterator source;
		for (source = phpSources.begin(); source != phpSources.end(); ++source) {
			if (!source->Exists()) {
				mvceditor::EditorLogError(mvceditor::ERR_INVALID_DIRECTORY, 
					source->RootDirectory.GetPath()
				);
			}
		}
		if (reader->InitDirectoryLint(phpSources, App.Globals)) {
			RunningActionId = App.RunningThreads.Queue(reader);
			mvceditor::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
			gauge->AddGauge(_("Lint Check"), ID_LINT_RESULTS_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE, wxGA_HORIZONTAL);
			
			// create / open the outline window
			wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
			if (window) {
				mvceditor::LintResultsPanelClass* resultsPanel = (mvceditor::LintResultsPanelClass*) window;
				resultsPanel->ClearErrors();
				SetFocusToToolsWindow(resultsPanel);
			}
			else {
				mvceditor::LintResultsPanelClass* resultsPanel = new LintResultsPanelClass(GetToolsNotebook(), ID_LINT_RESULTS_PANEL, 
						GetNotebook(), *this);
				wxBitmap lintBitmap = mvceditor::IconImageAsset(wxT("lint-check"));
				AddToolsWindow(resultsPanel, _("Lint Check"), wxEmptyString, lintBitmap);
				SetFocusToToolsWindow(resultsPanel);
			}
		}
		else {
			delete reader;
		}
	}
	else {
		wxMessageBox(_("You must have an open project."), _("Lint Check"));
	}
}

void mvceditor::LintFeatureClass::OnNextLintError(wxCommandEvent& event) {
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	if (window) {
		mvceditor::LintResultsPanelClass* resultsPanel = (mvceditor::LintResultsPanelClass*) window;
		resultsPanel->SelectNextError();
	}
}

void mvceditor::LintFeatureClass::OnPreviousLintError(wxCommandEvent& event) {
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	if (window) {
		mvceditor::LintResultsPanelClass* resultsPanel = (mvceditor::LintResultsPanelClass*) window;
		resultsPanel->SelectPreviousError();
	}
}

void mvceditor::LintFeatureClass::OnLintError(mvceditor::LintResultsEventClass& event) {
	std::vector<pelet::LintResultsClass> results = event.LintResults;
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	if (window) {
		mvceditor::LintResultsPanelClass* resultsPanel = (mvceditor::LintResultsPanelClass*) window;
		resultsPanel->AddErrors(results);
	}
}

void mvceditor::LintFeatureClass::OnLintErrorAfterSave(mvceditor::LintResultsEventClass& event) {
	std::vector<pelet::LintResultsClass> results = event.LintResults;
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	mvceditor::LintResultsPanelClass* resultsPanel = NULL;
	if (window) {
		resultsPanel = (mvceditor::LintResultsPanelClass*) window;
	}
	else {
		resultsPanel = new mvceditor::LintResultsPanelClass(GetMainWindow(), ID_LINT_RESULTS_PANEL,
			GetNotebook(), *this);
		wxBitmap lintBitmap = mvceditor::IconImageAsset(wxT("lint-check"));
		GetToolsNotebook()->AddPage(resultsPanel, _("Lint Check"), false, lintBitmap);
	}

	// do not set focus the list, just the overlay
	// of the error
	int i = LintErrors.size();
	resultsPanel->AddErrors(results);
	resultsPanel->ShowLintError(i);
}

void mvceditor::LintFeatureClass::OnLintFileComplete(wxCommandEvent& event) {

}

void mvceditor::LintFeatureClass::OnLintComplete(mvceditor::ActionEventClass& event) {
	mvceditor::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
	gauge->StopGauge(ID_LINT_RESULTS_GAUGE);
	RunningActionId = 0;
}

void mvceditor::LintFeatureClass::OnLintProgress(mvceditor::ActionProgressEventClass& event) {
	mvceditor::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
	gauge->IncrementGauge(ID_LINT_RESULTS_GAUGE, StatusBarWithGaugeClass::INDETERMINATE_MODE);
}

void mvceditor::LintFeatureClass::OnFileSaved(mvceditor::CodeControlEventClass& event) {
	mvceditor::CodeControlClass* codeControl = event.GetCodeControl();
	wxString fileName = codeControl->GetFileName();
	codeControl->ClearLintErrors();
	bool hasErrors = !LintErrors.empty();

	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	mvceditor::LintResultsPanelClass* resultsPanel = NULL;
	if (window) {
		resultsPanel = (mvceditor::LintResultsPanelClass*) window;
	}

	// if user has configure to do lint check on saving or user is cleaning up
	// errors (after they manually lint checked the project) then re-check
	if (hasErrors || Options.CheckOnSave) {
		std::vector<pelet::LintResultsClass> lintResults;
		mvceditor::LintBackgroundFileReaderClass* thread = new mvceditor::LintBackgroundFileReaderClass(App.RunningThreads, ID_LINT_READER_SAVE, Options);
		bool good = thread->InitSingleFileLint(fileName, App.Globals);
	
		// handle the case where user has saved a file but has not clicked
		// on the Lint project button.
		if (resultsPanel) {
			resultsPanel->RemoveErrorsFor(fileName);
		}
		if (good) {
			App.RunningThreads.Queue(thread);
		}
		else {
			delete thread;
		}
	}
}

void mvceditor::LintFeatureClass::OnNotebookPageClosed(wxAuiNotebookEvent& event) {
	wxAuiNotebook* notebook = GetToolsNotebook();
	int selection = event.GetSelection();
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	if (notebook->GetPage(selection) == window) {
		if (RunningActionId > 0) {
			App.RunningThreads.CancelAction(RunningActionId);
			RunningActionId = 0;
			mvceditor::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
			gauge->StopGauge(ID_LINT_RESULTS_GAUGE);
		}
		
		// dont need to show the errors no need to keep the errors
		LintErrors.clear();
	}
}

void mvceditor::LintFeatureClass::OnLintSummary(mvceditor::LintResultsSummaryEventClass& event) {
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	if (window) {
		mvceditor::LintResultsPanelClass* resultsPanel = (mvceditor::LintResultsPanelClass*) window;
		resultsPanel->PrintSummary(event.TotalFiles, event.ErrorFiles);
	}
}

mvceditor::LintPreferencesPanelClass::LintPreferencesPanelClass(wxWindow* parent,
																mvceditor::LintFeatureClass& feature)
	: LintPreferencesGeneratedPanelClass(parent, wxID_ANY)
	, Feature(feature) {
	wxGenericValidator checkValidator(&Feature.Options.CheckOnSave);
	CheckOnSave->SetValidator(checkValidator);

	wxGenericValidator checkUninitializedValidator(&Feature.Options.CheckUninitializedVariables);
	CheckUnitializedVariables->SetValidator(checkUninitializedValidator);

	wxGenericValidator checkGlobalValidator(&Feature.Options.CheckGlobalScopeVariables);
	CheckUnitializedGlobalVariables->SetValidator(checkGlobalValidator);

	wxGenericValidator checkIdentifiersValidator(&Feature.Options.CheckUnknownIdentifiers);
	CheckUnknownIdentifiers->SetValidator(checkIdentifiersValidator);
}

mvceditor::LintErrorPanelClass::LintErrorPanelClass(mvceditor::CodeControlClass* parent, int id, const std::vector<pelet::LintResultsClass>& results)
: LintErrorGeneratedPanelClass(parent, id) 
, CodeControl(parent)
, LintResults(results) {
	wxString error = mvceditor::IcuToWx(results[0].Error);
	error += wxString::Format(wxT(" on line %d, offset %d"), results[0].LineNumber, results[0].CharacterPosition);
	ErrorLabel->SetLabel(error);
}

void mvceditor::LintErrorPanelClass::OnKeyDown(wxKeyEvent& event) {
	if (event.GetKeyCode() == WXK_ESCAPE) {
		CallAfter(&mvceditor::LintErrorPanelClass::DoDestroy);
		return;
	}
	if (event.GetKeyCode() == WXK_SPACE) {

		// scintilla lines are 0-based, lint error lines are 1 based
		CodeControl->GotoLine(LintResults[0].LineNumber - 1);
		CallAfter(&mvceditor::LintErrorPanelClass::DoDestroy);
		return;
	}
	event.Skip();
}

void mvceditor::LintErrorPanelClass::OnGoToLink(wxHyperlinkEvent& event) {

	// scintilla lines are 0-based, lint error lines are 1 based
	CodeControl->GotoLine(LintResults[0].LineNumber - 1);
	CallAfter(&mvceditor::LintErrorPanelClass::DoDestroy);
}

void mvceditor::LintErrorPanelClass::OnDismissLink(wxHyperlinkEvent& event) {
	CallAfter(&mvceditor::LintErrorPanelClass::DoDestroy);
}

void mvceditor::LintErrorPanelClass::DoDestroy() {
	this->Destroy();
}


mvceditor::LintFeatureOptionsClass::LintFeatureOptionsClass()
: CheckOnSave(true)
, CheckUninitializedVariables(true)
, CheckUnknownIdentifiers(false)
, CheckGlobalScopeVariables(false) {

}

mvceditor::LintFeatureOptionsClass::LintFeatureOptionsClass(const mvceditor::LintFeatureOptionsClass& src)
: CheckOnSave(true)
, CheckUninitializedVariables(true)
, CheckUnknownIdentifiers(false)
, CheckGlobalScopeVariables(false) {
	Copy(src);
}

mvceditor::LintFeatureOptionsClass& mvceditor::LintFeatureOptionsClass::operator=(const mvceditor::LintFeatureOptionsClass& src)  {
	Copy(src);
	return *this;
}

void mvceditor::LintFeatureOptionsClass::Copy(const mvceditor::LintFeatureOptionsClass& src) {
	CheckOnSave = src.CheckOnSave;
	CheckUninitializedVariables = src.CheckUninitializedVariables;
	CheckUnknownIdentifiers = src.CheckUnknownIdentifiers;
	CheckGlobalScopeVariables = src.CheckGlobalScopeVariables;
}

BEGIN_EVENT_TABLE(mvceditor::LintFeatureClass, wxEvtHandler) 
	EVT_MENU(mvceditor::MENU_LINT_PHP + 0, mvceditor::LintFeatureClass::OnLintMenu)
	EVT_MENU(mvceditor::MENU_LINT_PHP + 1, mvceditor::LintFeatureClass::OnNextLintError)
	EVT_MENU(mvceditor::MENU_LINT_PHP + 2, mvceditor::LintFeatureClass::OnPreviousLintError)
	EVT_COMMAND(ID_LINT_READER, EVENT_FILE_READ,  mvceditor::LintFeatureClass::OnLintFileComplete)
	EVT_ACTION_PROGRESS(ID_LINT_READER, mvceditor::LintFeatureClass::OnLintProgress)
	EVT_ACTION_COMPLETE(ID_LINT_READER, mvceditor::LintFeatureClass::OnLintComplete)
	EVT_APP_FILE_SAVED(mvceditor::LintFeatureClass::OnFileSaved)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_PREFERENCES_SAVED, mvceditor::LintFeatureClass::OnPreferencesSaved)
	EVT_AUINOTEBOOK_PAGE_CLOSE(mvceditor::ID_TOOLS_NOTEBOOK, mvceditor::LintFeatureClass::OnNotebookPageClosed)
	EVT_LINT_ERROR(ID_LINT_READER, mvceditor::LintFeatureClass::OnLintError)
	EVT_LINT_ERROR(ID_LINT_READER_SAVE, mvceditor::LintFeatureClass::OnLintErrorAfterSave)
	EVT_LINT_SUMMARY(ID_LINT_READER, mvceditor::LintFeatureClass::OnLintSummary)
END_EVENT_TABLE()