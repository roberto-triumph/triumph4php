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

mvceditor::LintResultsEventClass::LintResultsEventClass(int eventId, const pelet::LintResultsClass& lintResults)
	: wxEvent(eventId, mvceditor::EVENT_LINT_ERROR) 
	, LintResults(lintResults) {
}

wxEvent* mvceditor::LintResultsEventClass::Clone() const {
	return new mvceditor::LintResultsEventClass(GetId(), LintResults);
}

mvceditor::LintResultsSummaryEventClass::LintResultsSummaryEventClass(int eventId, int totalFiles, int errorFiles)
	: wxEvent(eventId, mvceditor::EVENT_LINT_SUMMARY)
	, TotalFiles(totalFiles)
	, ErrorFiles(errorFiles) {

}

wxEvent* mvceditor::LintResultsSummaryEventClass::Clone() const {
	return new mvceditor::LintResultsSummaryEventClass(GetId(), TotalFiles, ErrorFiles);
}


mvceditor::ParserDirectoryWalkerClass::ParserDirectoryWalkerClass() 
	: LastResults()	
	, WithErrors(0)
	, WithNoErrors(0)
	, Parser() {
		
}

void mvceditor::ParserDirectoryWalkerClass::ResetTotals() {
	WithErrors = 0;
	WithNoErrors = 0;
}

void mvceditor::ParserDirectoryWalkerClass::SetVersion(pelet::Versions version) {
	Parser.SetVersion(version);
}

bool mvceditor::ParserDirectoryWalkerClass::Walk(const wxString& fileName) {
	bool ret = false;
	LastResults.Error = UNICODE_STRING_SIMPLE("");
	LastResults.LineNumber = 0;
	LastResults.CharacterPosition = 0;

	wxFFile file(fileName, wxT("rb"));
	if (Parser.LintFile(file.fp(), mvceditor::WxToIcu(fileName), LastResults)) {
		WithNoErrors++;
	}
	else {
		WithErrors++;
		ret = true;
	}
	return ret;
}

mvceditor::LintBackgroundFileReaderClass::LintBackgroundFileReaderClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: BackgroundFileReaderClass(runningThreads, eventId)
	, ParserDirectoryWalker() {
		
}

bool mvceditor::LintBackgroundFileReaderClass::InitDirectoryLint(std::vector<mvceditor::SourceClass> sources,
																  const mvceditor::EnvironmentClass& environment) {
	bool good = false;
	if (Init(sources)) {
		ParserDirectoryWalker.SetVersion(environment.Php.Version);
		ParserDirectoryWalker.ResetTotals();
		good = true;
	}
	return good;
}

bool mvceditor::LintBackgroundFileReaderClass::LintSingleFile(const wxString& fileName, const mvceditor::GlobalsClass& globals, 
															  const mvceditor::EnvironmentClass& environment, pelet::LintResultsClass& results) {

	// ATTN: use a local instance of ParserClass so that this method is thread safe
	// and can be run when a background thread is already running.
	bool error = false;
	if (globals.HasAPhpExtension(fileName)) {
		ParserDirectoryWalkerClass walker;
		walker.SetVersion(environment.Php.Version);
		error = walker.Walk(fileName);
		if (error) {
			results.Copy(walker.LastResults);
		}
	}
	return error;
}

bool mvceditor::LintBackgroundFileReaderClass::BackgroundFileRead(DirectorySearchClass &search) {
	bool error = search.Walk(ParserDirectoryWalker);
	if (error) {
		mvceditor::LintResultsEventClass lintResultsEvent(ID_LINT_READER, ParserDirectoryWalker.LastResults);
		PostEvent(lintResultsEvent);
	}
	if (!search.More() && !IsCancelled()) {
		int totalFiles = ParserDirectoryWalker.WithErrors + ParserDirectoryWalker.WithNoErrors;
		int errorFiles = ParserDirectoryWalker.WithErrors;
		mvceditor::LintResultsSummaryEventClass summaryEvent(ID_LINT_READER, totalFiles, errorFiles);
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
														std::vector<pelet::LintResultsClass>& lintErrors)
	: LintResultsGeneratedPanelClass(parent, id) 
	, Notebook(notebook) 
	, LintErrors(lintErrors)
	, TotalFiles(0)
	, ErrorFiles(0) {
			
}

void mvceditor::LintResultsPanelClass::AddError(const pelet::LintResultsClass& lintError) {
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
	LintErrors.push_back(lintError);
	ErrorsList->AppendString(wxMsg);
}

void mvceditor::LintResultsPanelClass::ClearErrors() {
	ErrorsList->Clear();
	LintErrors.clear();
}

void mvceditor::LintResultsPanelClass::RemoveErrorsFor(const wxString& fileName) {
	
	// remove the lint result data structures as well as the 
	// display list
	std::vector<pelet::LintResultsClass>::iterator it = LintErrors.begin();
	int i = 0;

	UnicodeString uniFileName = mvceditor::WxToIcu(fileName);
	bool found = false;
	while (it != LintErrors.end()) {
		if (it->UnicodeFilename == uniFileName) {
			it = LintErrors.erase(it);
			ErrorsList->Delete(i);
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

void mvceditor::LintResultsPanelClass::AddErrorsFor(const wxString& fileName, const pelet::LintResultsClass& lintResult) {
	// remove the lint result data structures as well as the 
	// display list
	std::vector<pelet::LintResultsClass>::iterator it = LintErrors.begin();
	int i = 0;
	bool found = false;
	UnicodeString uniFileName = mvceditor::WxToIcu(fileName);
	while (it != LintErrors.end()) {
		if (it->UnicodeFilename == uniFileName) {
			it = LintErrors.erase(it);
			ErrorsList->Delete(i);
			i--;
			found = true;
		}
		else {
			i++;
			it++;
		}
	}
	AddError(lintResult);

	// if this file has an error for the first time, increment the counter
	if (!found) {
		ErrorFiles++;
	}
	UpdateSummary();
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
	DisplayLintError(index);
}

void mvceditor::LintResultsPanelClass::DisplayLintError(int index) {
	pelet::LintResultsClass results = LintErrors[index];

	wxString file = mvceditor::IcuToWx(results.UnicodeFilename);
	Notebook->LoadPage(file);
	Notebook->GetCurrentCodeControl()->MarkLintError(results);
}

void mvceditor::LintResultsPanelClass::SelectNextError() {
	int selected = ErrorsList->GetSelection();
	if (selected != wxNOT_FOUND && ((unsigned int)selected  + 1) < ErrorsList->GetCount()) {
		ErrorsList->SetSelection(selected + 1);
		DisplayLintError(selected + 1);
	}
	else if (ErrorsList->GetCount() > 0) {
		ErrorsList->SetSelection(0);
		DisplayLintError(0);
	}
}

void mvceditor::LintResultsPanelClass::SelectPreviousError() {
	int selected = ErrorsList->GetSelection();
	if (selected != wxNOT_FOUND && (selected  - 1) >= 0) {
		ErrorsList->SetSelection(selected - 1);
		DisplayLintError(selected - 1);
	}
	else if (ErrorsList->GetCount() > 0) {
		ErrorsList->SetSelection(ErrorsList->GetCount() - 1);
		DisplayLintError(ErrorsList->GetCount() - 1);
	}
}

mvceditor::LintFeatureClass::LintFeatureClass(mvceditor::AppClass& app) 
	: FeatureClass(app)
	, CheckOnSave(true)
	, RunningActionId(0)
	, LintErrors() {
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
	config->Read(wxT("/LintCheck/CheckOnSave"), &CheckOnSave);
}
void mvceditor::LintFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {
	wxConfigBase* config = wxConfig::Get();
	config->Write(wxT("/LintCheck/CheckOnSave"), CheckOnSave);
}

void mvceditor::LintFeatureClass::OnLintMenu(wxCommandEvent& event) {
	if (RunningActionId > 0) {
		wxMessageBox(_("There is already another lint check that is active. Please wait for it to finish."), _("Lint Check"));
		return;
	}
	if (App.Globals.HasSources()) {
		mvceditor::LintBackgroundFileReaderClass* reader = new mvceditor::LintBackgroundFileReaderClass(App.RunningThreads, ID_LINT_READER);
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
		if (reader->InitDirectoryLint(phpSources, *GetEnvironment())) {
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
				mvceditor::LintResultsPanelClass* resultsPanel = new LintResultsPanelClass(GetToolsNotebook(), ID_LINT_RESULTS_PANEL, GetNotebook(), LintErrors);
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
	pelet::LintResultsClass results = event.LintResults;
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	if (window) {
		mvceditor::LintResultsPanelClass* resultsPanel = (mvceditor::LintResultsPanelClass*) window;
		resultsPanel->AddError(results);
	}
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
	if (hasErrors || CheckOnSave) {
		pelet::LintResultsClass lintResults;
		mvceditor::LintBackgroundFileReaderClass* thread = new mvceditor::LintBackgroundFileReaderClass(App.RunningThreads, ID_LINT_READER);
		bool error = thread->LintSingleFile(fileName, App.Globals, *GetEnvironment(), lintResults);
		if (error) {
			
			// handle the case where user has saved a file but has not clicked
			// on the Lint project button.
			if (resultsPanel) {

				// remove lint results for this file from the display list	
				// and put the new error
				resultsPanel->AddErrorsFor(fileName, lintResults);
			}
			if (codeControl) {
				int previousPos = codeControl->GetCurrentPos();
				codeControl->MarkLintError(lintResults);
			
				// diplaying the lint causes things to be redrawn; put the cursor 
				// where the user left it
				codeControl->GotoPos(previousPos);
				codeControl->SetFocus();

				wxString error = mvceditor::IcuToWx(lintResults.Error);
				error += wxString::Format(wxT(" on line %d, offset %d"), lintResults.LineNumber, lintResults.CharacterPosition);
				codeControl->CallTipShow(previousPos, error);
			}
		}
		else if (resultsPanel) {
			resultsPanel->RemoveErrorsFor(fileName);
		}
		delete thread;
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
	wxGenericValidator checkValidator(&Feature.CheckOnSave);
	CheckOnSave->SetValidator(checkValidator);
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
	EVT_LINT_SUMMARY(ID_LINT_READER, mvceditor::LintFeatureClass::OnLintSummary)
END_EVENT_TABLE()