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
#include <views/LintViewClass.h>
#include <globals/String.h>
#include <globals/Errors.h>
#include <globals/Assets.h>
#include <widgets/UnicodeStringValidatorClass.h>
#include <widgets/Buttons.h>
#include <code_control/CodeControlClass.h>
#include <Triumph.h>
#include <globals/Events.h>
#include <globals/Number.h>
#include <unicode/unistr.h>
#include <widgets/DataViewGrid.h>
#include <wx/artprov.h>
#include <wx/valgen.h>
#include <wx/tokenzr.h>
#include <wx/ffile.h>
#include <wx/clipbrd.h>

const int ID_LINT_RESULTS_PANEL = wxNewId();
const int ID_LINT_SUPPRESSIONS_PANEL = wxNewId();
const int ID_LINT_RESULTS_GAUGE = wxNewId();
const int ID_LINT_READER = wxNewId();
const int ID_LINT_READER_SAVE = wxNewId();
const int ID_LINT_ERROR_PANEL = wxNewId();
const int ID_MENU_COPY_FILE = wxNewId();
const int ID_MENU_COPY_ERROR = wxNewId();
const int ID_MENU_ADD_SUPRESSION = wxNewId();

/**
 * the maximum amount of errored files to tolerate.
 * Any more files than this anf we risk having too
 * many error instances in memory. Also, there is 
 * way the user is going through every single file
 */
static const int MAX_LINT_ERROR_FILES = 100;

t4p::LintResultsPanelClass::LintResultsPanelClass(wxWindow *parent, int id,
														t4p::LintFeatureClass& feature,
														t4p::LintViewClass& view,
														wxWindow* topWindow)
	: LintResultsGeneratedPanelClass(parent, id) 
	, Feature(feature)
	, View(view)
	, TopWindow(topWindow)
	, TotalFiles(0)
	, ErrorFiles(0) {
	RunButton->SetBitmap(t4p::BitmapImageButtonPrepAsset(wxT("lint-check")));
	SuppressionButton->SetBitmap(t4p::BitmapImageButtonPrepAsset(wxT("lint-check-suppression")));
	HelpButtonIcon(HelpButton);
	
	ErrorsList->AppendTextColumn(_("File"), wxDATAVIEW_CELL_INERT,
		wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	ErrorsList->AppendTextColumn(_("Line  Number"), wxDATAVIEW_CELL_INERT,
		wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	ErrorsList->AppendTextColumn(_("Error"), wxDATAVIEW_CELL_INERT,
		wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
}

void t4p::LintResultsPanelClass::EnableRunButton(bool doEnable) {
	RunButton->Enable(doEnable);
	if (!doEnable) {
		this->Label->SetLabel(
			_("Lint check in progress")
		);
	}
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

void t4p::LintResultsPanelClass::PrintSummary(int totalFiles, int errorFiles, int skippedFiles) {
	TotalFiles = totalFiles;
	ErrorFiles = errorFiles;
	SkippedFiles = skippedFiles;
	UpdateSummary();
}

void t4p::LintResultsPanelClass::UpdateSummary() {
	if (0 == ErrorFiles) {
		this->Label->SetLabel(
			wxString::Format(_("No errors found; checked %d files"), TotalFiles)
		);
	}
	else if (ErrorFiles > MAX_LINT_ERROR_FILES) {
		this->Label->SetLabel(
			wxString::Format(_("Found more than %d files with errors; stopping lint check"), MAX_LINT_ERROR_FILES)
		);
	}
	else if (SkippedFiles > 0) {
		this->Label->SetLabel(
			wxString::Format(_("Found %d files with errors; checked %d files; skipped %d files"), 
			ErrorFiles, TotalFiles, SkippedFiles)
		);
	}
	else {
		this->Label->SetLabel(
			wxString::Format(_("Found %d files with errors; checked %d files"), ErrorFiles, TotalFiles)
		);
	}
	
	// now resize the columns so that the errors can be seen
	t4p::DataViewGridAutoSizeAllColumns(ErrorsList);
}

void t4p::LintResultsPanelClass::OnRowActivated(wxDataViewEvent& event) {
	int index = ErrorsList->GetSelectedRow();
	GoToAndDisplayLintError(index);
}

void t4p::LintResultsPanelClass::OnRunButton(wxCommandEvent& event) {
	View.StartLint();
}

void t4p::LintResultsPanelClass::OnSuppressionButton(wxCommandEvent& event) {
	View.ShowSuppressionPanel();
}

void t4p::LintResultsPanelClass::OnErrorContextMenu(wxDataViewEvent& event) {
	wxMenu menu;
	menu.Append(ID_MENU_COPY_FILE, _("Copy File"));
	menu.Append(ID_MENU_COPY_ERROR, _("Copy Error"));
	menu.Append(ID_MENU_ADD_SUPRESSION, _("Add Suppression"));
	
	PopupMenu(&menu);
}

void t4p::LintResultsPanelClass::OnCopyFile(wxCommandEvent& event) {
	int index = ErrorsList->GetSelectedRow();
	if (!t4p::NumberLessThan(index, Feature.LintErrors.size())) {
		return;
	}
	pelet::LintResultsClass results = Feature.LintErrors[index];
	wxString file = t4p::IcuToWx(results.UnicodeFilename);
	
	if (wxTheClipboard->Open()) {
		wxTheClipboard->AddData(new wxTextDataObject(file));
		wxTheClipboard->Close();
	}
}

void t4p::LintResultsPanelClass::OnCopyError(wxCommandEvent& event) {
	int index = ErrorsList->GetSelectedRow();
	if (!t4p::NumberLessThan(index, Feature.LintErrors.size())) {
		return;
	}
	pelet::LintResultsClass results = Feature.LintErrors[index];
	wxString error = t4p::IcuToWx(results.Error);
	if (wxTheClipboard->Open()) {
		wxTheClipboard->AddData(new wxTextDataObject(error));
		wxTheClipboard->Close();
	}
}

void t4p::LintResultsPanelClass::OnAddSuppression(wxCommandEvent& event) {
	int row = ErrorsList->GetSelectedRow();
	if (!t4p::NumberLessThan(row, Feature.LintErrors.size())) {
		return;
	}
	
	// the linters provide the error info directly, but we don't
	// get the specific lint errors at this point.
	t4p::SuppressionRuleClass rule;
	pelet::LintResultsClass results = Feature.LintErrors[row];
	if (results.Error.indexOf(UNICODE_STRING_SIMPLE("Unknown class "))  >= 0) {
		rule.Type = t4p::SuppressionRuleClass::SKIP_UNKNOWN_CLASS;
		
		// 14 == length of  'unknown class '
		results.Error.extractBetween(14, results.Error.length(), rule.Target);
	}
	if (results.Error.indexOf(UNICODE_STRING_SIMPLE("Unknown method ")) >= 0) {
		rule.Type = t4p::SuppressionRuleClass::SKIP_UNKNOWN_METHOD;
		
		// 15 == length of  'unknown method '
		results.Error.extractBetween(15, results.Error.length(), rule.Target);
	}
	if (results.Error.indexOf(UNICODE_STRING_SIMPLE("Unknown function ")) >= 0) {
		rule.Type = t4p::SuppressionRuleClass::SKIP_UNKNOWN_FUNCTION;
		
		// 17 = length of "unknown function "
		results.Error.extractBetween(17, results.Error.length(), rule.Target);
	}
	if (results.Error.indexOf(UNICODE_STRING_SIMPLE("Uninitialized variable ")) >= 0) {
		rule.Type = t4p::SuppressionRuleClass::SKIP_UNINITIALIZED_VAR;
		
		// 23 = length of "uninitialized variable "
		results.Error.extractBetween(23, results.Error.length(), rule.Target);
	}
	if (results.Error.indexOf(UNICODE_STRING_SIMPLE("Missing arguments to function `")) >= 0) {
		rule.Type = t4p::SuppressionRuleClass::SKIP_FUNCTION_ARGUMENT_MISMATCH;
		
		// 31 = length of "Missing arguments to function `"
		int32_t endTickPos = results.Error.indexOf('`', 32);
		results.Error.extractBetween(31, endTickPos, rule.Target);
	}
	if (results.Error.indexOf(UNICODE_STRING_SIMPLE("Too many arguments to function `")) >= 0) {
		rule.Type = t4p::SuppressionRuleClass::SKIP_FUNCTION_ARGUMENT_MISMATCH;
		
		// 32 = length of "Too many arguments to function `"
		int32_t endTickPos = results.Error.indexOf('`', 33);
		results.Error.extractBetween(32, endTickPos, rule.Target);
	}
	
	if (!rule.Target.isEmpty()) {
		
		if (rule.Type != t4p::SuppressionRuleClass::SKIP_UNINITIALIZED_VAR) {
			
			// for unknown class/method/function, default the location to be the 
			// source directory; if a class is not found in one file it will not
			// be found in any files of the project
			std::vector<t4p::ProjectClass>::const_iterator project;
			std::vector<t4p::SourceClass>::const_iterator src;
			bool isDirFromProject = false;
			
			for (project = Feature.App.Globals.Projects.begin(); !isDirFromProject && project != Feature.App.Globals.Projects.end(); ++project) {
				if (project->IsEnabled) {
					for (src = project->Sources.begin(); src != project->Sources.end(); ++src) {
						if (src->IsInRootDirectory(results.File)) {
							rule.Location.AssignDir(src->RootDirectory.GetPath());
							isDirFromProject = true;							
							break;
						}
					}
				}
			}
		}
		else {
			rule.Location.Assign(results.File);
		}
		
		t4p::LintSuppressionRuleDialogClass dialog(TopWindow, wxID_ANY, rule);
		if (dialog.ShowModal() == wxOK) {
		
			// now save the newly created
			t4p::LintSuppressionClass suppressions;
			std::vector<UnicodeString> errors;
			
			wxFileName suppressionFile = t4p::LintSuppressionsFileAsset();
			suppressions.Init(suppressionFile, errors);
			suppressions.Add(rule);
			if (!suppressions.Save(suppressionFile)) {
				t4p::EditorLogWarning(t4p::ERR_INVALID_FILE, suppressionFile.GetFullPath());
			}		
		}
	}
}

void t4p::LintResultsPanelClass::GoToAndDisplayLintError(int index) {
	pelet::LintResultsClass results = Feature.LintErrors[index];

	wxString file = t4p::IcuToWx(results.UnicodeFilename);
	View.LoadCodeControl(file);
	t4p::CodeControlClass* codeCtrl = View.GetCurrentCodeControl();
	if (codeCtrl) {
		codeCtrl->MarkLintErrorAndGoto(results);
	}
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
	if (!t4p::NumberLessThan(index, Feature.LintErrors.size())) {
		return;
	}
	t4p::CodeControlClass* codeControl = View.GetCurrentCodeControl();
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
	LintHelpDialogGeneratedDialogClass dialog(TopWindow);
	dialog.ShowModal();
}

t4p::LintViewClass::LintViewClass(t4p::LintFeatureClass& feature) 
	: FeatureViewClass()
	, Feature(feature)
	, RunningActionId(0) {
}

void t4p::LintViewClass::AddViewMenuItems(wxMenu* viewMenu) {
	viewMenu->Append(t4p::MENU_LINT_PHP + 0, _("Lint Check"), _("Performs syntax check on the current project"), wxITEM_NORMAL);
	viewMenu->Append(t4p::MENU_LINT_PHP + 1, _("Lint Suppressions"), _("Shows the lint suppression rules"), wxITEM_NORMAL);
	viewMenu->Append(t4p::MENU_LINT_PHP + 2, _("Show Next Lint Error\tF4"), _("Selects the next lint error in the code window"), wxITEM_NORMAL);
	viewMenu->Append(t4p::MENU_LINT_PHP + 3, _("Show Previous Lint Error\tSHIFT+F4"), _("Selects the previous lint error in the code window"), wxITEM_NORMAL);
}

void t4p::LintViewClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	wxBitmap bmp = t4p::BitmapImageAsset(wxT("lint-check"));
	toolBar->AddTool(t4p::MENU_LINT_PHP + 0, _("Lint Check"), bmp, _("Performs syntax check on the current project"));
}

void t4p::LintViewClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[t4p::MENU_LINT_PHP + 0] = wxT("Lint Check - Lint Check Project");
	menuItemIds[t4p::MENU_LINT_PHP + 1] = wxT("Lint Check - Show Suppressions");
	menuItemIds[t4p::MENU_LINT_PHP + 2] = wxT("Lint Check - Show Next Lint Error");
	menuItemIds[t4p::MENU_LINT_PHP + 3] = wxT("Lint Check - Show Previous Lint Error");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void t4p::LintViewClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
	parent->AddPage(
		new t4p::LintPreferencesPanelClass(parent, Feature),
		_("PHP Lint Check"));
}

void t4p::LintViewClass::OnLintMenu(wxCommandEvent& event) {
	if (RunningActionId > 0) {
		wxMessageBox(_("There is already another lint check that is active. Please wait for it to finish."), _("Lint Check"));
		return;
	}
	StartLint();
}

void t4p::LintViewClass::StartLint() {
	if (Feature.App.Globals.HasSources()) {
		t4p::LintActionClass* reader = new t4p::LintActionClass(
			Feature.App.RunningThreads, ID_LINT_READER, Feature.Options, t4p::LintSuppressionsFileAsset()
		);
		std::vector<t4p::SourceClass> phpSources = Feature.App.Globals.AllEnabledPhpSources();

		// output an error if a source directory no longer exists
		std::vector<t4p::SourceClass>::const_iterator source;
		for (source = phpSources.begin(); source != phpSources.end(); ++source) {
			if (!source->Exists()) {
				t4p::EditorLogError(t4p::ERR_INVALID_DIRECTORY, 
					source->RootDirectory.GetPath()
				);
			}
		}
		if (reader->InitDirectoryLint(phpSources, Feature.App.Globals)) {
			RunningActionId = Feature.App.RunningThreads.Queue(reader);
			t4p::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
			gauge->AddGauge(_("Lint Check"), ID_LINT_RESULTS_GAUGE, t4p::StatusBarWithGaugeClass::INDETERMINATE_MODE, wxGA_HORIZONTAL);
			
			// create / open the results window
			wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
			if (window) {
				t4p::LintResultsPanelClass* resultsPanel = (t4p::LintResultsPanelClass*) window;
				resultsPanel->ClearErrors();
				resultsPanel->EnableRunButton(false);
				SetFocusToToolsWindow(resultsPanel);
			}
			else {
				t4p::LintResultsPanelClass* resultsPanel = new LintResultsPanelClass(GetToolsNotebook(), ID_LINT_RESULTS_PANEL, 
						Feature, *this, GetMainWindow());
				wxBitmap lintBitmap = t4p::BitmapImageAsset(wxT("lint-check"));
				AddToolsWindow(resultsPanel, _("Lint Check"), wxEmptyString, lintBitmap);
				SetFocusToToolsWindow(resultsPanel);
				resultsPanel->EnableRunButton(false);
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

void t4p::LintViewClass::OnLintSuppressionsMenu(wxCommandEvent& event) {
	ShowSuppressionPanel();
}

void t4p::LintViewClass::ShowSuppressionPanel() {
	wxWindow* window = FindToolsWindow(ID_LINT_SUPPRESSIONS_PANEL);
	if (window) {
		SetFocusToToolsWindow(window);
	}
	else {
		t4p::LintSuppressionsPanelClass* panel = new t4p::LintSuppressionsPanelClass(
			GetToolsNotebook(), ID_LINT_SUPPRESSIONS_PANEL, t4p::LintSuppressionsFileAsset(),
			GetMainWindow()
		);
		wxBitmap lintBitmap = t4p::BitmapImageAsset(wxT("lint-check-suppression"));
		AddToolsWindow(panel, _("Lint Suppressions"), wxEmptyString, lintBitmap);
	}
}

void t4p::LintViewClass::OnNextLintError(wxCommandEvent& event) {
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	if (window) {
		t4p::LintResultsPanelClass* resultsPanel = (t4p::LintResultsPanelClass*) window;
		resultsPanel->SelectNextError();
	}
}

void t4p::LintViewClass::OnPreviousLintError(wxCommandEvent& event) {
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	if (window) {
		t4p::LintResultsPanelClass* resultsPanel = (t4p::LintResultsPanelClass*) window;
		resultsPanel->SelectPreviousError();
	}
}

void t4p::LintViewClass::OnLintError(t4p::LintResultsEventClass& event) {
	std::vector<pelet::LintResultsClass> results = event.LintResults;
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	if (window) {
		t4p::LintResultsPanelClass* resultsPanel = (t4p::LintResultsPanelClass*) window;
		resultsPanel->AddErrors(results);
	}
}

void t4p::LintViewClass::OnLintErrorAfterSave(t4p::LintResultsEventClass& event) {
	std::vector<pelet::LintResultsClass> results = event.LintResults;
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	t4p::LintResultsPanelClass* resultsPanel = NULL;
	t4p::CodeControlClass* codeControl = GetCurrentCodeControl();
	if (window) {
		resultsPanel = (t4p::LintResultsPanelClass*) window;

		// do not set focus the list, just the overlay
        // of the error
        int i = Feature.LintErrors.size(); 
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

void t4p::LintViewClass::OnLintFileComplete(wxCommandEvent& event) {

}

void t4p::LintViewClass::OnLintComplete(t4p::ActionEventClass& event) {
	t4p::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
	gauge->StopGauge(ID_LINT_RESULTS_GAUGE);
	RunningActionId = 0;
}

void t4p::LintViewClass::OnLintProgress(t4p::ActionProgressEventClass& event) {
	t4p::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
	if (event.PercentComplete == 0) {
			
			// the start, turn the gauge into determinate mode
			gauge->SwitchMode(ID_LINT_RESULTS_GAUGE, t4p::StatusBarWithGaugeClass::DETERMINATE_MODE, 0, 100);
	}
	gauge->UpdateGauge(ID_LINT_RESULTS_GAUGE, event.PercentComplete);
	if (!event.Message.IsEmpty()) {
		gauge->RenameGauge(ID_LINT_RESULTS_GAUGE, event.Message);
	}
}

void t4p::LintViewClass::OnFileSaved(t4p::CodeControlEventClass& event) {
	t4p::CodeControlClass* codeControl = event.GetCodeControl();
	wxString fileName = codeControl->GetFileName();
	codeControl->ClearLintErrors();
	bool hasErrors = !Feature.LintErrors.empty();

	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	t4p::LintResultsPanelClass* resultsPanel = NULL;
	if (window) {
		resultsPanel = (t4p::LintResultsPanelClass*) window;
	}

	// if user has configure to do lint check on saving or user is cleaning up
	// errors (after they manually lint checked the project) then re-check
	if (hasErrors || Feature.Options.CheckOnSave) {
		std::vector<pelet::LintResultsClass> lintResults;
		t4p::LintBackgroundSingleFileClass* thread = new t4p::LintBackgroundSingleFileClass(
			Feature.App.RunningThreads, ID_LINT_READER_SAVE, Feature.Options, t4p::LintSuppressionsFileAsset()			
		);
		bool good = thread->Init(fileName, Feature.App.Globals);
	
		// handle the case where user has saved a file but has not clicked
		// on the Lint project button.
		if (resultsPanel) {
			resultsPanel->RemoveErrorsFor(fileName);
		}
		if (good) {
			Feature.App.RunningThreads.Queue(thread);
		}
		else {
			delete thread;
		}
	}
}

void t4p::LintViewClass::OnNotebookPageClosed(wxAuiNotebookEvent& event) {
	wxAuiNotebook* notebook = GetToolsNotebook();
	int selection = event.GetSelection();
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	if (notebook->GetPage(selection) == window) {
		if (RunningActionId > 0) {
			Feature.App.RunningThreads.CancelAction(RunningActionId);
			RunningActionId = 0;
			t4p::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
			gauge->StopGauge(ID_LINT_RESULTS_GAUGE);
		}
		
		// dont need to show the errors no need to keep the errors
		Feature.LintErrors.clear();
	}
}

void t4p::LintViewClass::OnLintSummary(t4p::LintResultsSummaryEventClass& event) {
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	if (window) {
		t4p::LintResultsPanelClass* resultsPanel = (t4p::LintResultsPanelClass*) window;
		resultsPanel->PrintSummary(event.TotalFiles, event.ErrorFiles, event.SkippedFiles);
		resultsPanel->EnableRunButton(true);
	}
}


t4p::LintPreferencesPanelClass::LintPreferencesPanelClass(wxWindow* parent,
																t4p::LintFeatureClass& feature)
	: LintPreferencesGeneratedPanelClass(parent, wxID_ANY)
	, Feature(feature) {
		
	t4p::HelpButtonIcon(HelpButton);
	
	wxGenericValidator checkValidator(&Feature.Options.CheckOnSave);
	CheckOnSave->SetValidator(checkValidator);

	wxGenericValidator checkUninitializedValidator(&Feature.Options.CheckUninitializedVariables);
	CheckUnitializedVariables->SetValidator(checkUninitializedValidator);

	wxGenericValidator checkGlobalValidator(&Feature.Options.CheckGlobalScopeVariables);
	CheckUnitializedGlobalVariables->SetValidator(checkGlobalValidator);

	wxGenericValidator checkIdentifiersValidator(&Feature.Options.CheckUnknownIdentifiers);
	CheckUnknownIdentifiers->SetValidator(checkIdentifiersValidator);
	
	wxGenericValidator checkFunctionArgumentCountValidator(&Feature.Options.CheckFunctionArgumentCount);
	CheckFunctionArgumentCount->SetValidator(checkFunctionArgumentCountValidator);
}

void t4p::LintPreferencesPanelClass::OnHelpClick(wxCommandEvent& event) {
	LintOptionsHelpGeneratedDialogClass dialog(this);
	dialog.ShowModal();
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

t4p::LintSuppressionsPanelClass::LintSuppressionsPanelClass(wxWindow* parent, int id, wxFileName suppressionFile,
	wxWindow* topWindow)
: LintSuppressionsGeneratedPanelClass(parent, id)
, SuppressionFile(suppressionFile)
, Suppressions()
, Errors()
, TopWindow(topWindow)
{
	AddButton->SetBitmap(t4p::BitmapImageButtonPrepAsset(wxT("filter-add")));
	EditButton->SetBitmap(t4p::BitmapImageButtonPrepAsset(wxT("filter-edit")));
	DeleteButton->SetBitmap(t4p::BitmapImageButtonPrepAsset(wxT("filter-delete")));
	DeleteAllButton->SetBitmap(t4p::BitmapImageButtonPrepAsset(wxT("stop")));
	HelpButtonIcon(HelpButton);
	
	SuppressionsList->AppendTextColumn(_("Rule Type"), wxDATAVIEW_CELL_INERT,
		wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	SuppressionsList->AppendTextColumn(_("Rule Target"), wxDATAVIEW_CELL_INERT,
		wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	SuppressionsList->AppendTextColumn(_("Location"), wxDATAVIEW_CELL_INERT,
		wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);

	// what should happen when the file does not exist?
	// during very first time the file may not exist, just
	// dont show the user bogus "could not load" errors
	if (suppressionFile.FileExists()) {
		bool loaded = Suppressions.Init(SuppressionFile, Errors);
		if (!loaded) {
			wxMessageBox(_("Could not load suppressions file at ") + SuppressionFile.GetFullPath(), _("File Error"));
		}
	}
	PopulateList();
}

void t4p::LintSuppressionsPanelClass::OnAddButton(wxCommandEvent& event) {
	
	t4p::SuppressionRuleClass newRule;
	t4p::LintSuppressionRuleDialogClass dialog(TopWindow, wxID_ANY, newRule);
	if (wxOK == dialog.ShowModal()) {
		
		// update file and list control
		Suppressions.Add(newRule);
		SaveList();
		AppendRuleToList(newRule);
	}
}

void t4p::LintSuppressionsPanelClass::OnDeleteAllButton(wxCommandEvent& event) {
	int confirm = wxMessageBox(_("Are you sure you want to delete all suppressions?"), 
		_("Lint Suppressions"), wxYES | wxNO | wxCENTER);
	if (confirm == wxYES) {
		
		// update file and list control
		Suppressions.Rules.clear();
		SaveList();
		SuppressionsList->DeleteAllItems();
	}
}

void t4p::LintSuppressionsPanelClass::OnDeleteButton(wxCommandEvent& event) {
	int row = SuppressionsList->GetSelectedRow();
	if (!t4p::NumberLessThan(row, Suppressions.Rules.size())) {
		return;
	}
	
	// update file and list control
	Suppressions.Rules.erase(Suppressions.Rules.begin() + row);
	SaveList();
	SuppressionsList->DeleteItem(row);
}

void t4p::LintSuppressionsPanelClass::OnEditButton(wxCommandEvent& event) {
	int row = SuppressionsList->GetSelectedRow();
	if (!t4p::NumberLessThan(row, Suppressions.Rules.size())) {
		return;
	}
	t4p::SuppressionRuleClass editRule = Suppressions.Rules[row];
	t4p::LintSuppressionRuleDialogClass dialog(TopWindow, wxID_ANY, editRule);
	if (wxOK == dialog.ShowModal()) {
		
		// update file and list control
		Suppressions.Rules[row] = editRule;
		SaveList();
		
		wxString wxTarget = t4p::IcuToWx(editRule.Target);
		wxString wxFile = editRule.Location.IsDir() ? editRule.Location.GetPath() : editRule.Location.GetFullPath();
		wxString wxType = _("");
		switch (editRule.Type) {
		case t4p::SuppressionRuleClass::SKIP_UNKNOWN_CLASS:
			wxType = _("Skip Uknown Class");
			break;
		case t4p::SuppressionRuleClass::SKIP_UNKNOWN_METHOD:
			wxType = _("Skip Unknown Method");
			break;
		case t4p::SuppressionRuleClass::SKIP_UNKNOWN_PROPERTY:
			wxType = _("Skip Unknown Property");
			break;
		case t4p::SuppressionRuleClass::SKIP_UNKNOWN_FUNCTION:
			wxType = _("Skip Unknown Function");
			break;
		case t4p::SuppressionRuleClass::SKIP_UNINITIALIZED_VAR:
			wxType = _("Skip Uninitialized Variable");
			break;
		case t4p::SuppressionRuleClass::SKIP_FUNCTION_ARGUMENT_MISMATCH:
			wxType = _("Skip Function Argument Count Mismatch");
			break;
		case t4p::SuppressionRuleClass::SKIP_ALL:
			wxType = _("Skip All");
			break;
		}
		SuppressionsList->SetValue(wxType, row, 0);
		SuppressionsList->SetValue(wxTarget, row, 1);
		SuppressionsList->SetValue(wxFile, row, 2);
	}
}

void t4p::LintSuppressionsPanelClass::OnRowActivated(wxDataViewEvent& event) {
	wxCommandEvent evt;
	OnEditButton(evt);
}

void t4p::LintSuppressionsPanelClass::OnKeyDown(wxKeyEvent& event) {
	if (event.GetKeyCode() != WXK_DELETE) {
		event.Skip();
		return;
	}
	if (event.HasAnyModifiers()) {
		event.Skip();
		return;
	}
	wxCommandEvent cmdEvt;
	OnDeleteButton(cmdEvt);
}

void t4p::LintSuppressionsPanelClass::OnHelpButton(wxCommandEvent& event) {
	LintSuppressionsHelpGeneratedDialogClass dialog(TopWindow);
	dialog.ShowModal();
}

void t4p::LintSuppressionsPanelClass::PopulateList() {
	std::vector<t4p::SuppressionRuleClass>::const_iterator rule;
	for (rule = Suppressions.Rules.begin(); rule != Suppressions.Rules.end(); ++rule) {
		AppendRuleToList(*rule);
	}
	if (!Suppressions.Rules.empty()) {
		SuppressionsList->SelectRow(0);
	}
	
	// now resize the columns so that the suppressions can be seen
	t4p::DataViewGridAutoSizeAllColumns(SuppressionsList);
	
	// don't know why the first column is not auto-sized all the way
	// I have seen that on Mac OS X the first column is still truncated
	SuppressionsList->GetColumn(0)->SetWidth(200);
}

void t4p::LintSuppressionsPanelClass::AppendRuleToList(const t4p::SuppressionRuleClass& rule) {
	wxString wxTarget = t4p::IcuToWx(rule.Target);
	wxString wxFile = rule.Location.IsDir() ? rule.Location.GetPath() : rule.Location.GetFullPath();
	wxString wxType = _("");
	switch (rule.Type) {
	case t4p::SuppressionRuleClass::SKIP_UNKNOWN_CLASS:
		wxType = _("Skip Uknown Class");
		break;
	case t4p::SuppressionRuleClass::SKIP_UNKNOWN_METHOD:
		wxType = _("Skip Unknown Method");
		break;
	case t4p::SuppressionRuleClass::SKIP_UNKNOWN_PROPERTY:
		wxType = _("Skip Unknown Property");
		break;
	case t4p::SuppressionRuleClass::SKIP_UNKNOWN_FUNCTION:
		wxType = _("Skip Unknown Function");
		break;
	case t4p::SuppressionRuleClass::SKIP_UNINITIALIZED_VAR:
		wxType = _("Skip Uninitialized Variable");
		break;
	case t4p::SuppressionRuleClass::SKIP_FUNCTION_ARGUMENT_MISMATCH:
		wxType = _("Skip Function Argument Count Mismatch");
		break;
	case t4p::SuppressionRuleClass::SKIP_ALL:
		wxType = _("Skip All");
		break;
	}
	
	wxVector<wxVariant> values;
	values.push_back(wxType);
	values.push_back(wxTarget);
	values.push_back(wxFile);
	
	SuppressionsList->AppendItem(values);
}

void t4p::LintSuppressionsPanelClass::SaveList() {
	bool saved = Suppressions.Save(SuppressionFile);
	if (!saved) {
		wxMessageBox(_("Could not saved suppressions file at ") + SuppressionFile.GetFullPath(), _("File Error"));
	}
}

t4p::LintSuppressionRuleDialogClass::LintSuppressionRuleDialogClass(wxWindow* parent, int id, 
	t4p::SuppressionRuleClass& rule)
: LintSuppressionRuleGeneratedDialogClass(parent, id)
, Rule(rule)
, EditRule(rule)
{
	t4p::UnicodeStringValidatorClass targetValidator(&EditRule.Target, true);
	Target->SetValidator(targetValidator);
	
	switch (EditRule.Type) {
		case t4p::SuppressionRuleClass::SKIP_UNKNOWN_CLASS:
			Types->SetSelection(t4p::SuppressionRuleClass::SKIP_UNKNOWN_CLASS);
			break;
		case t4p::SuppressionRuleClass::SKIP_UNKNOWN_METHOD:
			Types->SetSelection(t4p::SuppressionRuleClass::SKIP_UNKNOWN_METHOD);
			break;
		case t4p::SuppressionRuleClass::SKIP_UNKNOWN_PROPERTY:
			Types->SetSelection(t4p::SuppressionRuleClass::SKIP_UNKNOWN_PROPERTY);
			break;
		case t4p::SuppressionRuleClass::SKIP_UNKNOWN_FUNCTION:
			Types->SetSelection(t4p::SuppressionRuleClass::SKIP_UNKNOWN_FUNCTION);
			break;
		case t4p::SuppressionRuleClass::SKIP_UNINITIALIZED_VAR:
			Types->SetSelection(t4p::SuppressionRuleClass::SKIP_UNINITIALIZED_VAR);
			break;
		case t4p::SuppressionRuleClass::SKIP_FUNCTION_ARGUMENT_MISMATCH:
			Types->SetSelection(t4p::SuppressionRuleClass::SKIP_FUNCTION_ARGUMENT_MISMATCH);
			break;
		case t4p::SuppressionRuleClass::SKIP_ALL:
			Types->SetSelection(t4p::SuppressionRuleClass::SKIP_ALL);
			
			// the user doesn't need to enter a target for suppression
			// rules of type 'all'
			Target->Enable(false);
			break;
	}
	
	if (EditRule.Location.IsDir()) {
		Directory->Enable(true);
		File->Enable(false);
		DirectoryRadio->SetValue(true);
		Directory->SetPath(EditRule.Location.GetPath());
	}
	else {
		Directory->Enable(false);
		File->Enable(true);
		FileRadio->SetValue(true);
		File->SetPath(EditRule.Location.GetFullPath());
	}
	
	TransferDataToWindow();
}

void t4p::LintSuppressionRuleDialogClass::OnDirectoryRadio(wxCommandEvent& event) {
	Directory->Enable(true);
	File->Enable(false);
}

void t4p::LintSuppressionRuleDialogClass::OnFileRadio(wxCommandEvent& event) {
	Directory->Enable(false);
	File->Enable(true);
}

void t4p::LintSuppressionRuleDialogClass::OnOkButton(wxCommandEvent& event) {
	if (DirectoryRadio->GetValue() && Directory->GetPath().IsEmpty()) {
		wxMessageBox(_("Please enter a directory"), _("Error"));
		return;
	}
	if (FileRadio->GetValue() && File->GetPath().IsEmpty()) {
		wxMessageBox(_("Please enter a file"), _("Error"));
		return;
	}
	int sel = Types->GetSelection();
	if (t4p::SuppressionRuleClass::SKIP_ALL != sel && Target->GetValue().IsEmpty()) {
		wxMessageBox(_("Please enter a target"), _("Error"));
		return;
	}
	
	
	switch (sel) {
	case 0:
		EditRule.Type = t4p::SuppressionRuleClass::SKIP_UNKNOWN_CLASS;
		break;
	case 1:
		EditRule.Type = t4p::SuppressionRuleClass::SKIP_UNKNOWN_METHOD;
		break;
	case 2:
		EditRule.Type = t4p::SuppressionRuleClass::SKIP_UNKNOWN_PROPERTY;
		break;
	case 3:
		EditRule.Type = t4p::SuppressionRuleClass::SKIP_UNKNOWN_FUNCTION;
		break;
	case 4:
		EditRule.Type = t4p::SuppressionRuleClass::SKIP_UNINITIALIZED_VAR;
		break;
	case 5:
		EditRule.Type = t4p::SuppressionRuleClass::SKIP_FUNCTION_ARGUMENT_MISMATCH;
		break;
	case 6:
		EditRule.Type = t4p::SuppressionRuleClass::SKIP_ALL;
		break;
	}
	
	TransferDataFromWindow();
	if (DirectoryRadio->GetValue()) {
		EditRule.Location.SetPath(Directory->GetPath());
	}
	else if (FileRadio->GetValue()) {
		EditRule.Location.Assign(File->GetPath());
	}
	
	Rule = EditRule;
	return EndModal(wxOK);
}

void t4p::LintSuppressionRuleDialogClass::OnTypeChoice(wxCommandEvent& event) {
	int type = event.GetSelection();
	
	// the user doesn't need to enter a target for suppression
	// rules of type 'all'
	Target->Enable(t4p::SuppressionRuleClass::SKIP_ALL != type);
}


BEGIN_EVENT_TABLE(t4p::LintViewClass, t4p::FeatureViewClass) 
	EVT_MENU(t4p::MENU_LINT_PHP + 0, t4p::LintViewClass::OnLintMenu)
	EVT_MENU(t4p::MENU_LINT_PHP + 1, t4p::LintViewClass::OnLintSuppressionsMenu)
	EVT_MENU(t4p::MENU_LINT_PHP + 2, t4p::LintViewClass::OnNextLintError)
	EVT_MENU(t4p::MENU_LINT_PHP + 3, t4p::LintViewClass::OnPreviousLintError)
	EVT_COMMAND(ID_LINT_READER, EVENT_FILE_READ,  t4p::LintViewClass::OnLintFileComplete)
	EVT_ACTION_PROGRESS(ID_LINT_READER, t4p::LintViewClass::OnLintProgress)
	EVT_ACTION_COMPLETE(ID_LINT_READER, t4p::LintViewClass::OnLintComplete)
	EVT_APP_FILE_SAVED(t4p::LintViewClass::OnFileSaved)
	EVT_AUINOTEBOOK_PAGE_CLOSE(t4p::ID_TOOLS_NOTEBOOK, t4p::LintViewClass::OnNotebookPageClosed)
	EVT_LINT_ERROR(ID_LINT_READER, t4p::LintViewClass::OnLintError)
	EVT_LINT_ERROR(ID_LINT_READER_SAVE, t4p::LintViewClass::OnLintErrorAfterSave)
	EVT_LINT_SUMMARY(ID_LINT_READER, t4p::LintViewClass::OnLintSummary)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::LintResultsPanelClass, LintResultsGeneratedPanelClass)
	EVT_DATAVIEW_ITEM_ACTIVATED(LintResultsGeneratedPanelClass::ID_ERRORS_LIST, 
		t4p::LintResultsPanelClass::OnRowActivated)
	EVT_DATAVIEW_ITEM_CONTEXT_MENU(LintResultsGeneratedPanelClass::ID_ERRORS_LIST,
		t4p::LintResultsPanelClass::OnErrorContextMenu)
	EVT_MENU(ID_MENU_COPY_FILE, t4p::LintResultsPanelClass::OnCopyFile)
	EVT_MENU(ID_MENU_COPY_ERROR, t4p::LintResultsPanelClass::OnCopyError)
	EVT_MENU(ID_MENU_ADD_SUPRESSION, t4p::LintResultsPanelClass::OnAddSuppression)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::LintSuppressionsPanelClass, LintSuppressionsGeneratedPanelClass)
	EVT_DATAVIEW_ITEM_ACTIVATED(LintSuppressionsGeneratedPanelClass::ID_SUPPRESSIONS_LIST, t4p::LintSuppressionsPanelClass::OnRowActivated)
END_EVENT_TABLE()
