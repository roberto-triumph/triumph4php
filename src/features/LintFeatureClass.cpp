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

t4p::LintResultsEventClass::LintResultsEventClass(int eventId, const std::vector<pelet::LintResultsClass>& lintResults)
	: wxEvent(eventId, t4p::EVENT_LINT_ERROR) 
	, LintResults(lintResults) {
}

wxEvent* t4p::LintResultsEventClass::Clone() const {
	t4p::LintResultsEventClass* cloned = new t4p::LintResultsEventClass(GetId(), LintResults);
	return cloned;
}

t4p::LintResultsSummaryEventClass::LintResultsSummaryEventClass(int eventId, int totalFiles, int errorFiles)
	: wxEvent(eventId, t4p::EVENT_LINT_SUMMARY)
	, TotalFiles(totalFiles)
	, ErrorFiles(errorFiles) {

}

wxEvent* t4p::LintResultsSummaryEventClass::Clone() const {
	return new t4p::LintResultsSummaryEventClass(GetId(), TotalFiles, ErrorFiles);
}


t4p::ParserDirectoryWalkerClass::ParserDirectoryWalkerClass(t4p::TagCacheClass& tagCache, const t4p::LintFeatureOptionsClass& options) 
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

void t4p::ParserDirectoryWalkerClass::ResetTotals() {
	WithErrors = 0;
	WithNoErrors = 0;
}

void t4p::ParserDirectoryWalkerClass::SetVersion(pelet::Versions version) {
	Parser.SetVersion(version);
	VariableLinterOptions.Version = version;
	VariableLinter.SetOptions(VariableLinterOptions);
	IdentifierLinter.SetVersion(version);
}

bool t4p::ParserDirectoryWalkerClass::Walk(const wxString& fileName) {
	bool ret = false;
	LastResults.Error = UNICODE_STRING_SIMPLE("");
	LastResults.LineNumber = 0;
	LastResults.CharacterPosition = 0;
	VariableResults.clear();
	IdentifierResults.clear();

	wxFFile file(fileName, wxT("rb"));
	bool hasErrors = false;
	if (!Parser.LintFile(file.fp(), t4p::WxToIcu(fileName), LastResults)) {
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

std::vector<pelet::LintResultsClass> t4p::ParserDirectoryWalkerClass::GetLastErrors() {
	std::vector<pelet::LintResultsClass> allResults;
	if (!LastResults.Error.isEmpty()) {
		allResults.push_back(LastResults);
	}
	for (size_t i = 0; i < VariableResults.size(); ++i) {
		pelet::LintResultsClass lintResult;
		t4p::PhpVariableLintResultClass variableResult = VariableResults[i];
		lintResult.Error = UNICODE_STRING_SIMPLE("Uninitialized variable ") + variableResult.VariableName;
		lintResult.File = t4p::IcuToChar(variableResult.File);
		lintResult.UnicodeFilename = variableResult.File;
		lintResult.LineNumber = variableResult.LineNumber;
		lintResult.CharacterPosition = variableResult.Pos;
		allResults.push_back(lintResult);
	}
	for (size_t i = 0; i < IdentifierResults.size(); ++i) {
		pelet::LintResultsClass lintResult;
		t4p::PhpIdentifierLintResultClass identifierResult = IdentifierResults[i];
		if (t4p::PhpIdentifierLintResultClass::UNKNOWN_CLASS == identifierResult.Type) {
			lintResult.Error = UNICODE_STRING_SIMPLE("Unknown class ") + identifierResult.Identifier;
		}
		else if (t4p::PhpIdentifierLintResultClass::UNKNOWN_METHOD == identifierResult.Type) {
			lintResult.Error = UNICODE_STRING_SIMPLE("Unknown method ") + identifierResult.Identifier;
		}
		else if (t4p::PhpIdentifierLintResultClass::UNKNOWN_PROPERTY == identifierResult.Type) {
			lintResult.Error = UNICODE_STRING_SIMPLE("Unknown property ") + identifierResult.Identifier;
		}
		else if (t4p::PhpIdentifierLintResultClass::UNKNOWN_FUNCTION == identifierResult.Type) {
			lintResult.Error = UNICODE_STRING_SIMPLE("Unknown function ") + identifierResult.Identifier;
		}
		lintResult.File = t4p::IcuToChar(identifierResult.File);
		lintResult.UnicodeFilename = identifierResult.File;
		lintResult.LineNumber = identifierResult.LineNumber;
		lintResult.CharacterPosition = identifierResult.Pos;
		allResults.push_back(lintResult);
	}
	return allResults;
}

t4p::LintBackgroundFileReaderClass::LintBackgroundFileReaderClass(t4p::RunningThreadsClass& runningThreads, 
																		int eventId,
																		const t4p::LintFeatureOptionsClass& options)
	: BackgroundFileReaderClass(runningThreads, eventId)
	, TagCache()
	, ParserDirectoryWalker(TagCache, options) {
		
}

bool t4p::LintBackgroundFileReaderClass::InitDirectoryLint(std::vector<t4p::SourceClass> sources,
																 t4p::GlobalsClass& globals) {
	bool good = false;
	if (Init(sources)) {
		TagCache.RegisterDefault(globals);
		ParserDirectoryWalker.SetVersion(globals.Environment.Php.Version);
		ParserDirectoryWalker.ResetTotals();
		good = true;
	}
	return good;
}

bool t4p::LintBackgroundFileReaderClass::BackgroundFileRead(DirectorySearchClass &search) {
	bool error = search.Walk(ParserDirectoryWalker);
	std::vector<pelet::LintResultsClass> lintErrors = ParserDirectoryWalker.GetLastErrors();
	if (error && !lintErrors.empty()) {
		t4p::LintResultsEventClass lintResultsEvent(GetEventId(), lintErrors);
		PostEvent(lintResultsEvent);
	}
	if (!search.More() && !IsCancelled()) {
		int totalFiles = ParserDirectoryWalker.WithErrors + ParserDirectoryWalker.WithNoErrors;
		int errorFiles = ParserDirectoryWalker.WithErrors;
		t4p::LintResultsSummaryEventClass summaryEvent(GetEventId(), totalFiles, errorFiles);
		PostEvent(summaryEvent);
	}
	return !error;
}

bool t4p::LintBackgroundFileReaderClass::BackgroundFileMatch(const wxString& file) {
	return true;
}

void t4p::LintBackgroundFileReaderClass::LintTotals(int& totalFiles, int& errorFiles) {
	totalFiles = ParserDirectoryWalker.WithErrors + ParserDirectoryWalker.WithNoErrors;
	errorFiles = ParserDirectoryWalker.WithErrors;
}

wxString t4p::LintBackgroundFileReaderClass::GetLabel() const {
	return wxT("Lint Directories");
}

t4p::LintBackgroundSingleFileClass::LintBackgroundSingleFileClass(t4p::RunningThreadsClass& runningThreads, 
																		int eventId,
																		const t4p::LintFeatureOptionsClass& options)
	: ActionClass(runningThreads, eventId)
	, FileName()
	, TagCache()
	, ParserDirectoryWalker(TagCache, options) {
		
}

bool t4p::LintBackgroundSingleFileClass::Init(const wxFileName& fileName, t4p::GlobalsClass& globals) {
	
	bool good = false;
	if (globals.FileTypes.HasAPhpExtension(fileName.GetFullPath())) {
		
		// need to be thread safe and deep clone 
		FileName.Assign(fileName.GetFullPath());
		TagCache.RegisterDefault(globals);
		ParserDirectoryWalker.SetVersion(globals.Environment.Php.Version);
		ParserDirectoryWalker.ResetTotals();
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
	/*
	if (!search.More() && !IsCancelled()) {
		int totalFiles = ParserDirectoryWalker.WithErrors + ParserDirectoryWalker.WithNoErrors;
		int errorFiles = ParserDirectoryWalker.WithErrors;
		t4p::LintResultsSummaryEventClass summaryEvent(GetEventId(), totalFiles, errorFiles);
		PostEvent(summaryEvent);
	}*/
}

wxString t4p::LintBackgroundSingleFileClass::GetLabel() const {
	return wxT("Lint Single File");
}

t4p::LintResultsPanelClass::LintResultsPanelClass(wxWindow *parent, int id, t4p::NotebookClass* notebook,
														t4p::LintFeatureClass& feature)
	: LintResultsGeneratedPanelClass(parent, id) 
	, Notebook(notebook) 
	, Feature(feature)
	, TotalFiles(0)
	, ErrorFiles(0) {
	HelpButton->SetBitmap(
		wxArtProvider::GetBitmap(wxART_HELP, wxART_BUTTON, wxSize(16, 16))
	);
	
	ErrorsList->AppendTextColumn(_("File"), wxDATAVIEW_CELL_INERT,
		wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	ErrorsList->AppendTextColumn(_("Line  Number"), wxDATAVIEW_CELL_INERT,
		wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	ErrorsList->AppendTextColumn(_("Error"), wxDATAVIEW_CELL_INERT,
		wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
}

void t4p::LintResultsPanelClass::AddErrors(const std::vector<pelet::LintResultsClass>& lintErrors) {
	std::vector<pelet::LintResultsClass>::const_iterator it;
	for (it = lintErrors.begin(); it != lintErrors.end(); ++it) {
		pelet::LintResultsClass lintError = *it;
		
		Feature.LintErrors.push_back(lintError);
		
		wxString wxFile = t4p::IcuToWx(lintError.UnicodeFilename);
		wxString errorMsg = t4p::IcuToWx(lintError.Error);
		wxVector<wxVariant> values;
		values.push_back(wxFile);
		values.push_back(wxString::Format(wxT("%d"), lintError.LineNumber));
		values.push_back(errorMsg);
		
		ErrorsList->AppendItem(values);
	}
}

void t4p::LintResultsPanelClass::ClearErrors() {
	ErrorsList->DeleteAllItems();
	Feature.LintErrors.clear();
	Feature.VariableResults.clear();
	Feature.IdentifierResults.clear();
}

void t4p::LintResultsPanelClass::RemoveErrorsFor(const wxString& fileName) {
	
	// remove the lint result data structures as well as the 
	// display list
	std::vector<pelet::LintResultsClass>::iterator it = Feature.LintErrors.begin();
	int i = 0;

	UnicodeString uniFileName = t4p::WxToIcu(fileName);
	bool found = false;
	while (it != Feature.LintErrors.end()) {
		if (it->UnicodeFilename == uniFileName) {
			it = Feature.LintErrors.erase(it);
			if (i >= 0 && i < ErrorsList->GetItemCount()) {
				ErrorsList->DeleteItem(i);
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
		if (ErrorFiles > 0) {
			ErrorFiles--;
		}
		UpdateSummary();
	}
}

void t4p::LintResultsPanelClass::PrintSummary(int totalFiles, int errorFiles) {
	TotalFiles = totalFiles;
	ErrorFiles = errorFiles;
	UpdateSummary();
}

void t4p::LintResultsPanelClass::UpdateSummary() {
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

void t4p::LintResultsPanelClass::OnRowActivated(wxDataViewEvent& event) {
	int index = ErrorsList->GetSelectedRow();
	GoToAndDisplayLintError(index);
}

void t4p::LintResultsPanelClass::GoToAndDisplayLintError(int index) {
	pelet::LintResultsClass results = Feature.LintErrors[index];

	wxString file = t4p::IcuToWx(results.UnicodeFilename);
	Notebook->LoadPage(file);
	Notebook->GetCurrentCodeControl()->MarkLintErrorAndGoto(results);
}

void t4p::LintResultsPanelClass::SelectNextError() {
	int selected = ErrorsList->GetSelectedRow();
	if (selected != wxNOT_FOUND && (selected  + 1) < ErrorsList->GetItemCount()) {
		ErrorsList->SelectRow(selected + 1);
		GoToAndDisplayLintError(selected + 1);
	}
	else if (ErrorsList->GetItemCount() > 0) {
		ErrorsList->SelectRow(0);
		GoToAndDisplayLintError(0);
	}
}

void t4p::LintResultsPanelClass::SelectPreviousError() {
	int selected = ErrorsList->GetSelectedRow();
	if (selected != wxNOT_FOUND && (selected  - 1) >= 0) {
		ErrorsList->SelectRow(selected - 1);
		GoToAndDisplayLintError(selected - 1);
	}
	else if (ErrorsList->GetItemCount() > 0) {
		ErrorsList->SelectRow(ErrorsList->GetItemCount() - 1);
		GoToAndDisplayLintError(ErrorsList->GetItemCount() - 1);
	}
}

void t4p::LintResultsPanelClass::ShowLintError(int index) {
	if (index < 0 || (size_t)index >= Feature.LintErrors.size()) {
		return;
	}
	t4p::CodeControlClass* codeControl = Notebook->GetCurrentCodeControl();
	if (!codeControl) {
		return;
	}
	wxString fileName = codeControl->GetFileName();
	pelet::LintResultsClass result = Feature.LintErrors[index];
	wxString resultFile = t4p::CharToWx(result.File.c_str());
	if (resultFile != fileName) {
		return;
	}
	codeControl->MarkLintError(result);
}

void t4p::LintResultsPanelClass::IncrementErrorFileCount() {
	ErrorFiles++;
	UpdateSummary();
}

void t4p::LintResultsPanelClass::OnHelpButton(wxCommandEvent& event) {
	LintHelpDialogGeneratedDialogClass dialog(this);
	dialog.ShowModal();
}

t4p::LintFeatureClass::LintFeatureClass(t4p::AppClass& app) 
	: FeatureClass(app)
	, Options()
	, LintErrors()
	, VariableResults()
	, IdentifierResults()
	, RunningActionId(0) {
}

void t4p::LintFeatureClass::AddViewMenuItems(wxMenu* viewMenu) {
	viewMenu->Append(t4p::MENU_LINT_PHP + 0, _("Lint Check"), _("Performs syntax check on the current project"), wxITEM_NORMAL);
	viewMenu->Append(t4p::MENU_LINT_PHP + 1, _("Show Next Lint Error\tF4"), _("Selects the next lint error in the code window"), wxITEM_NORMAL);
	viewMenu->Append(t4p::MENU_LINT_PHP + 2, _("Show Previous Lint Error\tSHIFT+F4"), _("Selects the previous lint error in the code window"), wxITEM_NORMAL);
}

void t4p::LintFeatureClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	wxBitmap bmp = t4p::BitmapImageAsset(wxT("lint-check"));
	toolBar->AddTool(t4p::MENU_LINT_PHP + 0, _("Lint Check"), bmp, _("Performs syntax check on the current project"));
}

void t4p::LintFeatureClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[t4p::MENU_LINT_PHP + 0] = wxT("Lint Check - Lint Check Project");
	menuItemIds[t4p::MENU_LINT_PHP + 1] = wxT("Lint Check - Show Next Lint Error");
	menuItemIds[t4p::MENU_LINT_PHP + 2] = wxT("Lint Check - Show Previous Lint Error");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void t4p::LintFeatureClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
	parent->AddPage(
		new t4p::LintPreferencesPanelClass(parent, *this),
		_("PHP Lint Check"));
}

void t4p::LintFeatureClass::LoadPreferences(wxConfigBase* config) {
	config->Read(wxT("/LintCheck/CheckOnSave"), &Options.CheckOnSave);
	config->Read(wxT("/LintCheck/CheckUninitializedVariables"), &Options.CheckUninitializedVariables);
	config->Read(wxT("/LintCheck/CheckUnknownIdentifiers"), &Options.CheckUnknownIdentifiers);
	config->Read(wxT("/LintCheck/CheckGlobalScopeVariables"), &Options.CheckGlobalScopeVariables);
}
void t4p::LintFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {
	wxConfigBase* config = wxConfig::Get();
	config->Write(wxT("/LintCheck/CheckOnSave"), Options.CheckOnSave);
	config->Write(wxT("/LintCheck/CheckUninitializedVariables"), Options.CheckUninitializedVariables);
	config->Write(wxT("/LintCheck/CheckUnknownIdentifiers"), Options.CheckUnknownIdentifiers);
	config->Write(wxT("/LintCheck/CheckGlobalScopeVariables"), Options.CheckGlobalScopeVariables);
}

void t4p::LintFeatureClass::OnLintMenu(wxCommandEvent& event) {
	if (RunningActionId > 0) {
		wxMessageBox(_("There is already another lint check that is active. Please wait for it to finish."), _("Lint Check"));
		return;
	}
	if (App.Globals.HasSources()) {
		t4p::LintBackgroundFileReaderClass* reader = new t4p::LintBackgroundFileReaderClass(App.RunningThreads, ID_LINT_READER, Options);
		std::vector<t4p::SourceClass> phpSources = App.Globals.AllEnabledPhpSources();

		// output an error if a source directory no longer exists
		std::vector<t4p::SourceClass>::const_iterator source;
		for (source = phpSources.begin(); source != phpSources.end(); ++source) {
			if (!source->Exists()) {
				t4p::EditorLogError(t4p::ERR_INVALID_DIRECTORY, 
					source->RootDirectory.GetPath()
				);
			}
		}
		if (reader->InitDirectoryLint(phpSources, App.Globals)) {
			RunningActionId = App.RunningThreads.Queue(reader);
			t4p::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
			gauge->AddGauge(_("Lint Check"), ID_LINT_RESULTS_GAUGE, t4p::StatusBarWithGaugeClass::INDETERMINATE_MODE, wxGA_HORIZONTAL);
			
			// create / open the outline window
			wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
			if (window) {
				t4p::LintResultsPanelClass* resultsPanel = (t4p::LintResultsPanelClass*) window;
				resultsPanel->ClearErrors();
				SetFocusToToolsWindow(resultsPanel);
			}
			else {
				t4p::LintResultsPanelClass* resultsPanel = new LintResultsPanelClass(GetToolsNotebook(), ID_LINT_RESULTS_PANEL, 
						GetNotebook(), *this);
				wxBitmap lintBitmap = t4p::BitmapImageAsset(wxT("lint-check"));
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

void t4p::LintFeatureClass::OnNextLintError(wxCommandEvent& event) {
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	if (window) {
		t4p::LintResultsPanelClass* resultsPanel = (t4p::LintResultsPanelClass*) window;
		resultsPanel->SelectNextError();
	}
}

void t4p::LintFeatureClass::OnPreviousLintError(wxCommandEvent& event) {
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	if (window) {
		t4p::LintResultsPanelClass* resultsPanel = (t4p::LintResultsPanelClass*) window;
		resultsPanel->SelectPreviousError();
	}
}

void t4p::LintFeatureClass::OnLintError(t4p::LintResultsEventClass& event) {
	std::vector<pelet::LintResultsClass> results = event.LintResults;
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	if (window) {
		t4p::LintResultsPanelClass* resultsPanel = (t4p::LintResultsPanelClass*) window;
		resultsPanel->AddErrors(results);
	}
}

void t4p::LintFeatureClass::OnLintErrorAfterSave(t4p::LintResultsEventClass& event) {
	std::vector<pelet::LintResultsClass> results = event.LintResults;
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	t4p::LintResultsPanelClass* resultsPanel = NULL;
	t4p::CodeControlClass* codeControl = GetCurrentCodeControl();
	if (window) {
		resultsPanel = (t4p::LintResultsPanelClass*) window;

		// do not set focus the list, just the overlay
        // of the error
        int i = LintErrors.size(); 
        resultsPanel->AddErrors(results);
       resultsPanel->IncrementErrorFileCount(); 
		resultsPanel->ShowLintError(i);
	}
	else if (codeControl && results.empty()) {
		
		// no errors!
		codeControl->ClearLintErrors();
	}
	else if (codeControl && !results.empty()) {

		// if the lint results panel is not open, just show
		// a small panel instead of creating the tools window,
		// which will startle the user

		// lines from scintilla are 0 based, but lint results lines are 1-based
		int firstVisibleLine = codeControl->GetFirstVisibleLine() + 1;
		int lastVisibleLine = firstVisibleLine + codeControl->LinesOnScreen();
		if (results[0].LineNumber < firstVisibleLine || results[0].LineNumber >= lastVisibleLine) {
			
			// the error is out of view show a message, remove any other existing message
			
			// freeze thaw the code control so that the popup is 
			// not drawn while its being moved into place
			// in linux, freezing already happens internally so we don't
			// want to do it here 
			wxPlatformInfo info;
			if (info.GetOperatingSystemId() == wxOS_WINDOWS_NT) {
				codeControl->Freeze();
			}
			
			wxWindow* old = wxWindow::FindWindowById(ID_LINT_ERROR_PANEL, codeControl);
			if (old) {
				old->Destroy();
			}
			t4p::LintErrorPanelClass* errorPanel = new t4p::LintErrorPanelClass(codeControl, ID_LINT_ERROR_PANEL, results);
			wxPoint point = codeControl->PointFromPosition(codeControl->GetCurrentPos());
			point.y = point.y - errorPanel->GetSize().GetY();
			if (point.y < 0) {
				point.y = 0;
			}
			errorPanel->SetPosition(point);
			errorPanel->SetFocus();
			
			if (info.GetOperatingSystemId() == wxOS_WINDOWS_NT) {
				codeControl->Thaw();
			}
		}
		else {

			// the error is in view, just show an annotation
			codeControl->MarkLintError(results[0]);
		}
	}
}

void t4p::LintFeatureClass::OnLintFileComplete(wxCommandEvent& event) {

}

void t4p::LintFeatureClass::OnLintComplete(t4p::ActionEventClass& event) {
	t4p::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
	gauge->StopGauge(ID_LINT_RESULTS_GAUGE);
	RunningActionId = 0;
}

void t4p::LintFeatureClass::OnLintProgress(t4p::ActionProgressEventClass& event) {
	t4p::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
	gauge->IncrementGauge(ID_LINT_RESULTS_GAUGE, StatusBarWithGaugeClass::INDETERMINATE_MODE);
}

void t4p::LintFeatureClass::OnFileSaved(t4p::CodeControlEventClass& event) {
	t4p::CodeControlClass* codeControl = event.GetCodeControl();
	wxString fileName = codeControl->GetFileName();
	codeControl->ClearLintErrors();
	bool hasErrors = !LintErrors.empty();

	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	t4p::LintResultsPanelClass* resultsPanel = NULL;
	if (window) {
		resultsPanel = (t4p::LintResultsPanelClass*) window;
	}

	// if user has configure to do lint check on saving or user is cleaning up
	// errors (after they manually lint checked the project) then re-check
	if (hasErrors || Options.CheckOnSave) {
		std::vector<pelet::LintResultsClass> lintResults;
		t4p::LintBackgroundSingleFileClass* thread = new t4p::LintBackgroundSingleFileClass(App.RunningThreads, ID_LINT_READER_SAVE, Options);
		bool good = thread->Init(fileName, App.Globals);
	
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

void t4p::LintFeatureClass::OnNotebookPageClosed(wxAuiNotebookEvent& event) {
	wxAuiNotebook* notebook = GetToolsNotebook();
	int selection = event.GetSelection();
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	if (notebook->GetPage(selection) == window) {
		if (RunningActionId > 0) {
			App.RunningThreads.CancelAction(RunningActionId);
			RunningActionId = 0;
			t4p::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
			gauge->StopGauge(ID_LINT_RESULTS_GAUGE);
		}
		
		// dont need to show the errors no need to keep the errors
		LintErrors.clear();
	}
}

void t4p::LintFeatureClass::OnLintSummary(t4p::LintResultsSummaryEventClass& event) {
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	if (window) {
		t4p::LintResultsPanelClass* resultsPanel = (t4p::LintResultsPanelClass*) window;
		resultsPanel->PrintSummary(event.TotalFiles, event.ErrorFiles);
	}
}

t4p::LintPreferencesPanelClass::LintPreferencesPanelClass(wxWindow* parent,
																t4p::LintFeatureClass& feature)
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

t4p::LintErrorPanelClass::LintErrorPanelClass(t4p::CodeControlClass* parent, int id, const std::vector<pelet::LintResultsClass>& results)
: LintErrorGeneratedPanelClass(parent, id) 
, CodeControl(parent)
, LintResults(results) {
	wxString error = t4p::IcuToWx(results[0].Error);
	error += wxString::Format(wxT(" on line %d, offset %d"), results[0].LineNumber, results[0].CharacterPosition);
	ErrorLabel->SetLabel(error);
}

void t4p::LintErrorPanelClass::OnKeyDown(wxKeyEvent& event) {
	if (event.GetKeyCode() == WXK_SPACE) {
		CodeControl->MarkLintErrorAndGoto(LintResults[0]);
		CallAfter(&t4p::LintErrorPanelClass::DoDestroy);
		return;
	}
	CallAfter(&t4p::LintErrorPanelClass::DoDestroy);
}

void t4p::LintErrorPanelClass::OnGoToLink(wxHyperlinkEvent& event) {
	CodeControl->MarkLintErrorAndGoto(LintResults[0]);
	CallAfter(&t4p::LintErrorPanelClass::DoDestroy);
}

void t4p::LintErrorPanelClass::OnDismissLink(wxHyperlinkEvent& event) {
	CallAfter(&t4p::LintErrorPanelClass::DoDestroy);
}

void t4p::LintErrorPanelClass::DoDestroy() {
	this->Destroy();
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
}

BEGIN_EVENT_TABLE(t4p::LintFeatureClass, wxEvtHandler) 
	EVT_MENU(t4p::MENU_LINT_PHP + 0, t4p::LintFeatureClass::OnLintMenu)
	EVT_MENU(t4p::MENU_LINT_PHP + 1, t4p::LintFeatureClass::OnNextLintError)
	EVT_MENU(t4p::MENU_LINT_PHP + 2, t4p::LintFeatureClass::OnPreviousLintError)
	EVT_COMMAND(ID_LINT_READER, EVENT_FILE_READ,  t4p::LintFeatureClass::OnLintFileComplete)
	EVT_ACTION_PROGRESS(ID_LINT_READER, t4p::LintFeatureClass::OnLintProgress)
	EVT_ACTION_COMPLETE(ID_LINT_READER, t4p::LintFeatureClass::OnLintComplete)
	EVT_APP_FILE_SAVED(t4p::LintFeatureClass::OnFileSaved)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::LintFeatureClass::OnPreferencesSaved)
	EVT_AUINOTEBOOK_PAGE_CLOSE(t4p::ID_TOOLS_NOTEBOOK, t4p::LintFeatureClass::OnNotebookPageClosed)
	EVT_LINT_ERROR(ID_LINT_READER, t4p::LintFeatureClass::OnLintError)
	EVT_LINT_ERROR(ID_LINT_READER_SAVE, t4p::LintFeatureClass::OnLintErrorAfterSave)
	EVT_LINT_SUMMARY(ID_LINT_READER, t4p::LintFeatureClass::OnLintSummary)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::LintResultsPanelClass, LintResultsGeneratedPanelClass )
	EVT_DATAVIEW_ITEM_ACTIVATED(LintResultsGeneratedPanelClass::ID_ERRORS_LIST, t4p::LintResultsPanelClass::OnRowActivated)
END_EVENT_TABLE()
