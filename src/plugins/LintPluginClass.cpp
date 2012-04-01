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
#include <plugins/LintPluginClass.h>
#include <windows/StringHelperClass.h>
#include <MvcEditorErrors.h>
#include <MvcEditor.h>
#include <Events.h>
#include <unicode/unistr.h>
#include <wx/artprov.h>
#include <wx/valgen.h>
#include <wx/tokenzr.h>

const int ID_LINT_ERROR_COMMAND = wxNewId();
const int ID_LINT_RESULTS_PANEL = wxNewId();
const int ID_LINT_RESULTS_GAUGE = wxNewId();

mvceditor::ParserDirectoryWalkerClass::ParserDirectoryWalkerClass() 
	: LastResults()	
	, WithErrors(0)
	, WithNoErrors(0)
	, Parser()
	, PhpFileFilters()
	, IgnoreFileFiltersRegEx()
	, IgnoreFileFilters() {
		
}

void mvceditor::ParserDirectoryWalkerClass::ResetTotals() {
	WithErrors = 0;
	WithNoErrors = 0;
}

void mvceditor::ParserDirectoryWalkerClass::SetFilters(std::vector<wxString> phpFileFilters, wxString ignoreFileFilters) {
	PhpFileFilters = phpFileFilters;
	IgnoreFileFilters = ignoreFileFilters;
	IgnoreFileFilters.Trim(false);
	IgnoreFileFilters.Trim(true);
	if (!IgnoreFileFilters.IsEmpty()) {
		wxString ignoreFilesRegEx = mvceditor::FindInFilesClass::CreateFilesFilterRegEx(IgnoreFileFilters);
		IgnoreFileFiltersRegEx.Compile(ignoreFilesRegEx, wxRE_EXTENDED | wxRE_ICASE);
	}
}

bool mvceditor::ParserDirectoryWalkerClass::Walk(const wxString& fileName) {
	bool ret = false;
	bool matchedFilter = false;
	bool matchedIgnoreFilter = false;

	// check both filters
	// ignore file filters may be empty; when filter string is empty the regEex is irrelevant
	if (!IgnoreFileFilters.IsEmpty() && IgnoreFileFiltersRegEx.IsValid()) {
		matchedIgnoreFilter = IgnoreFileFiltersRegEx.Matches(fileName);
	}
	for (size_t i = 0; i < PhpFileFilters.size(); ++i) {
		wxString filter = PhpFileFilters[i];
		matchedFilter = !wxIsWild(filter) || wxMatchWild(filter, fileName);
		if (matchedFilter) {
			break;
		}
	}

	// ignore filter overrides the include filter
	if (matchedFilter && !matchedIgnoreFilter) {
		LastResults.Error = UNICODE_STRING_SIMPLE("");
		LastResults.LineNumber = 0;
		LastResults.CharacterPosition = 0;

		wxFFile file(fileName, wxT("rb"));
		if (Parser.LintFile(file.fp(), mvceditor::StringHelperClass::wxToIcu(fileName), LastResults)) {
			WithNoErrors++;
		}
		else {
			WithErrors++;
			ret = true;
		}
	}
	return ret;
}

mvceditor::LintBackgroundFileReaderClass::LintBackgroundFileReaderClass(wxEvtHandler& handler)
	: BackgroundFileReaderClass(handler)
	, ParserDirectoryWalker()
	, PhpFileFilters() {
		
}

bool mvceditor::LintBackgroundFileReaderClass::BeginDirectoryLint(const wxString& directory, const std::vector<wxString>& phpFileFilters, 
																  const wxString& ignoreFileFilters, mvceditor::BackgroundFileReaderClass::StartError& error) {
	bool good = false;
	error = mvceditor::BackgroundFileReaderClass::NONE;
	if (Init(directory)) {
		ParserDirectoryWalker.SetFilters(phpFileFilters, ignoreFileFilters);
		ParserDirectoryWalker.ResetTotals();
		if (StartReading(error)) {
			good = true;
		}
	}
	return good;
}

bool mvceditor::LintBackgroundFileReaderClass::LintSingleFile(const wxString& fileName, 
	  const std::vector<wxString>& phpFileFilters, const wxString& ignoreFileFilters) {

	// ATTN: use a local instance of ParserClass so that this method is thread safe
	// and can be run when a background thread is already running.
	ParserDirectoryWalkerClass walker;
	walker.SetFilters(phpFileFilters, ignoreFileFilters);
	bool error = walker.Walk(fileName);
	if (error) {
		wxCommandEvent evt(EVENT_LINT_ERROR, ID_LINT_ERROR_COMMAND);

		// ATTN: will need to be DELETED!!
		pelet::LintResultsClass* clonedResults = new pelet::LintResultsClass;
		clonedResults->Copy(walker.LastResults);
		evt.SetClientData((void*)clonedResults);
		wxPostEvent(&Handler, evt);
	}
	return error;
}

bool mvceditor::LintBackgroundFileReaderClass::FileRead(DirectorySearchClass &search) {
	bool error = search.Walk(ParserDirectoryWalker);
	if (error) {
		wxCommandEvent evt(EVENT_LINT_ERROR, ID_LINT_ERROR_COMMAND);

		// ATTN: will need to be DELETED!!
		pelet::LintResultsClass* clonedResults = new pelet::LintResultsClass;
		clonedResults->Copy(ParserDirectoryWalker.LastResults);
		evt.SetClientData((void*)clonedResults);
		wxPostEvent(&Handler, evt);
	}
	return !error;
}

bool mvceditor::LintBackgroundFileReaderClass::FileMatch(const wxString& file) {
	return true;
}

void mvceditor::LintBackgroundFileReaderClass::LintTotals(int& totalFiles, int& errorFiles) {
	totalFiles = ParserDirectoryWalker.WithErrors + ParserDirectoryWalker.WithNoErrors;
	errorFiles = ParserDirectoryWalker.WithErrors;
}

mvceditor::LintResultsPanelClass::LintResultsPanelClass(wxWindow *parent, int id, mvceditor::NotebookClass* notebook,
														std::vector<pelet::LintResultsClass*>& lintErrors)
	: LintResultsGeneratedPanelClass(parent, id) 
	, Notebook(notebook) 
	, LintErrors(lintErrors) {
			
}

mvceditor::LintResultsPanelClass::~LintResultsPanelClass() {
	ClearErrors();
}

void mvceditor::LintResultsPanelClass::AddError(pelet::LintResultsClass* lintError) {
	wxString err = StringHelperClass::IcuToWx(lintError->Error);
	wxString line;
	int capacity = lintError->Error.length() + lintError->UnicodeFilename.length() + 50;
	UnicodeString msg;
	int written = u_sprintf(msg.getBuffer(capacity), 
		"%S on %S line %d near Position %d\n", 
		lintError->Error.getTerminatedBuffer(),
		lintError->UnicodeFilename.getTerminatedBuffer(),
		lintError->LineNumber,
		lintError->CharacterPosition
	);
	msg.releaseBuffer(written);
	wxString wxMsg = mvceditor::StringHelperClass::IcuToWx(msg);
	LintErrors.push_back(lintError);
	ErrorsList->AppendString(wxMsg);
}

void mvceditor::LintResultsPanelClass::ClearErrors() {
	ErrorsList->Clear();

	// ATTN: VERY IMPORTANT to delete the pointers before we clear the vector
	for(size_t i = 0; i < LintErrors.size(); i++) {
		delete LintErrors[i];
	}
	LintErrors.clear();
}

void mvceditor::LintResultsPanelClass::RemoveErrorsFor(const wxString& fileName) {
	
	// remove the lint result data structures as well as the 
	// display list
	std::vector<pelet::LintResultsClass*>::iterator it = LintErrors.begin();
	int i = 0;

	UnicodeString uniFileName = mvceditor::StringHelperClass::wxToIcu(fileName);
	while (it != LintErrors.end()) {
		if ((*it)->UnicodeFilename == uniFileName) {
			delete *it;
			it = LintErrors.erase(it);
			ErrorsList->Delete(i);
			i--;
		}
		else {
			i++;
			it++;
		}
	}
}

void mvceditor::LintResultsPanelClass::PrintSummary(int totalFiles, int errorFiles) {
	if (0 == errorFiles) {
		this->Label->SetLabel(
			wxString::Format(_("No errors found; checked %d files"), totalFiles)
		);
	}
	else {
		this->Label->SetLabel(
			wxString::Format(_("Found %d files with errors; checked %d files"), errorFiles, totalFiles)
		);
	}
}

void mvceditor::LintResultsPanelClass::OnListDoubleClick(wxCommandEvent& event) {
	int index = event.GetInt();
	DisplayLintError(index);
}

void mvceditor::LintResultsPanelClass::DisplayLintError(int index) {
	pelet::LintResultsClass* results = LintErrors[index];

	wxString file = mvceditor::StringHelperClass::IcuToWx(results->UnicodeFilename);
	Notebook->LoadPage(file);
	Notebook->GetCurrentCodeControl()->MarkLintError(*results);
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

mvceditor::LintPluginClass::LintPluginClass() 
	: PluginClass()
	, IgnoreFiles() 
	, CheckOnSave(true)
	, LintBackgroundFileReader(*this)
	, LintErrors() {
}

void mvceditor::LintPluginClass::AddProjectMenuItems(wxMenu* projectMenu) {
	projectMenu->Append(mvceditor::MENU_LINT_PHP + 0, _("Lint Check"), _("Performs syntax check on the current project"), wxITEM_NORMAL);
	projectMenu->Append(mvceditor::MENU_LINT_PHP + 1, _("Show Next Lint Error\tF4"), _("Selects the next lint error in the code window"), wxITEM_NORMAL);
	projectMenu->Append(mvceditor::MENU_LINT_PHP + 2, _("Show Previous Lint Error\tSHIFT+F4"), _("Selects the previous lint error in the code window"), wxITEM_NORMAL);
}

void mvceditor::LintPluginClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	wxBitmap bitmap = wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE, wxART_TOOLBAR, wxSize(16, 16));
	toolBar->AddTool(mvceditor::MENU_LINT_PHP + 0, _("Lint Check"), bitmap, _("Performs syntax check on the current project"));
}

void mvceditor::LintPluginClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_LINT_PHP + 0] = wxT("Lint Check - Lint Check Project");
	menuItemIds[mvceditor::MENU_LINT_PHP + 1] = wxT("Lint Check - Show Next Lint Error");
	menuItemIds[mvceditor::MENU_LINT_PHP + 2] = wxT("Lint Check - Show Previous Lint Error");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::LintPluginClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
	parent->AddPage(
		new mvceditor::LintPluginPreferencesPanelClass(parent, *this),
		_("PHP Lint Check"));
}

void mvceditor::LintPluginClass::LoadPreferences(wxConfigBase* config) {
	IgnoreFiles = config->Read(wxT("LintCheck/IgnoreFiles"));
	config->Read(wxT("/LintCheck/CheckOnSave"), &CheckOnSave);
}
void mvceditor::LintPluginClass::SavePreferences(wxCommandEvent& event) {
	wxConfigBase* config = wxConfig::Get();
	config->Write(wxT("/LintCheck/IgnoreFiles"), IgnoreFiles);
	config->Write(wxT("/LintCheck/CheckOnSave"), CheckOnSave);
}

void mvceditor::LintPluginClass::OnLintMenu(wxCommandEvent& event) {
	if (LintBackgroundFileReader.IsRunning()) {
		wxMessageBox(_("There is already another lint check that is active. Please wait for it to finish."), _("Lint Check"));
		return;
	}
	mvceditor::ProjectClass *project = GetProject();
	if (project) {
		wxString rootPath = project->GetRootPath();
		std::vector<wxString> phpFileFilters = project->GetPhpFileExtensions();
		mvceditor::BackgroundFileReaderClass::StartError error;

		// use the OR functionality of the filters by replacing newlines with semicolons
		// need to remove any whitespace
		wxString ignoreFilesRegEx(IgnoreFiles);
		ignoreFilesRegEx.Trim(false);
		ignoreFilesRegEx.Trim(true);
		ignoreFilesRegEx.Replace(wxT("\n"), wxT(";"));		
		if (LintBackgroundFileReader.BeginDirectoryLint(rootPath, phpFileFilters, ignoreFilesRegEx, error)) {
			mvceditor::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
			gauge->AddGauge(_("Lint Check"), ID_LINT_RESULTS_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE, wxGA_HORIZONTAL);
			
			// create / open the outline window
			wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
			LintResultsPanelClass* lintResultsPanel = NULL;
			if (window) {
				lintResultsPanel = (LintResultsPanelClass*)window;
				lintResultsPanel->ClearErrors();
				SetFocusToToolsWindow(lintResultsPanel);
			}
			else {
				lintResultsPanel = new LintResultsPanelClass(GetToolsNotebook(), ID_LINT_RESULTS_PANEL, GetNotebook(), LintErrors);
				AddToolsWindow(lintResultsPanel, _("Lint Check"));
				SetFocusToToolsWindow(lintResultsPanel);
			}
		}
		else if (error == mvceditor::BackgroundFileReaderClass::ALREADY_RUNNING)  {
			wxMessageBox(_("There is already another lint check that is active. Please wait for it to finish."), _("Lint Check"));
		}
		else if (error == mvceditor::BackgroundFileReaderClass::NO_RESOURCES)  {
			mvceditor::EditorLogError(mvceditor::LOW_RESOURCES);
		}
		else {
			wxMessageBox(_("Could not start parsing. Does project root path have files?"), _("Lint Check"));
		}
	}
	else {
		wxMessageBox(_("You must have an open project."), _("Lint Check"));
	}
}

void mvceditor::LintPluginClass::OnNextLintError(wxCommandEvent& event) {
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	LintResultsPanelClass* lintResultsPanel = NULL;
	if (window) {
		lintResultsPanel = (LintResultsPanelClass*)window;
		lintResultsPanel->SelectNextError();
	}
}

void mvceditor::LintPluginClass::OnPreviousLintError(wxCommandEvent& event) {
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	LintResultsPanelClass* lintResultsPanel = NULL;
	if (window) {
		lintResultsPanel = (LintResultsPanelClass*)window;
		lintResultsPanel->SelectPreviousError();
	}
}

void mvceditor::LintPluginClass::OnLintError(wxCommandEvent& event) {
	pelet::LintResultsClass* results = (pelet::LintResultsClass*)event.GetClientData();
	
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	LintResultsPanelClass* lintResultsPanel = NULL;
	if (window) {
		lintResultsPanel = (LintResultsPanelClass*)window;
		lintResultsPanel->AddError(results);
	}
}

void mvceditor::LintPluginClass::OnLintFileComplete(wxCommandEvent& event) {
	
}

void mvceditor::LintPluginClass::OnLintComplete(wxCommandEvent& event) {
	mvceditor::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
	gauge->StopGauge(ID_LINT_RESULTS_GAUGE);
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	LintResultsPanelClass* lintResultsPanel = NULL;
	if (window) {
		lintResultsPanel = (LintResultsPanelClass*)window;
		int totalFiles = 0;
		int errorFiles = 0;
		LintBackgroundFileReader.LintTotals(totalFiles, errorFiles);
		lintResultsPanel->PrintSummary(totalFiles, errorFiles);
	}
}

void mvceditor::LintPluginClass::OnTimer(wxCommandEvent& event) {
	mvceditor::StatusBarWithGaugeClass* gauge = GetStatusBarWithGauge();
	gauge->IncrementGauge(ID_LINT_RESULTS_GAUGE, StatusBarWithGaugeClass::INDETERMINATE_MODE);	
}

void mvceditor::LintPluginClass::OnFileSaved(mvceditor::FileSavedEventClass& event) {
	mvceditor::CodeControlClass* codeControl = event.GetCodeControl();
	wxString fileName = codeControl->GetFileName();
	codeControl->ClearLintErrors();

	bool hasErrors = !LintErrors.empty();

	// remove lint results for this file from the display list
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	LintResultsPanelClass* lintResultsPanel = NULL;
	if (window) {
		lintResultsPanel = (LintResultsPanelClass*)window;
		lintResultsPanel->RemoveErrorsFor(fileName);
	}

	// if user has configure to do lint check on saving or user is cleanin up
	// errors (after they manually lint checked the project) then re-check
	if (hasErrors || CheckOnSave) {
		std::vector<wxString> phpFileFilters = GetProject()->GetPhpFileExtensions();
		
		// use the OR functionality of the filters by replacing newlines with semicolons
		wxString ignoreFilesRegEx(IgnoreFiles);
		ignoreFilesRegEx.Trim(false);
		ignoreFilesRegEx.Trim(true);
		ignoreFilesRegEx.Replace(wxT("\n"), wxT(";"));

		bool error = LintBackgroundFileReader.LintSingleFile(fileName, phpFileFilters, ignoreFilesRegEx);
		if (error) {
			
			// handle the case where user has saved a file but has not clicked
			// on the Lint project button.
			if (!window) {
				lintResultsPanel = new LintResultsPanelClass(GetToolsNotebook(), ID_LINT_RESULTS_PANEL, GetNotebook(), LintErrors);
				AddToolsWindow(lintResultsPanel, _("Lint Check"));
				SetFocusToToolsWindow(lintResultsPanel);
			}
			int previousPos = -1;
			mvceditor::CodeControlClass* codeControl = GetCurrentCodeControl();
			if (codeControl) {
				previousPos = GetCurrentCodeControl()->GetCurrentPos();
			}
			
			// we know that there are lint error events to be consumed
			ProcessPendingEvents();
			lintResultsPanel->DisplayLintError(LintErrors.size() - 1);

			// diplaying the lint causes things to be redrawn; put the cursor 
			// where the user left it
			if (previousPos >= 0 && codeControl) {
				codeControl->GotoPos(previousPos);
			}
		}
	}
}

mvceditor::LintPluginPreferencesPanelClass::LintPluginPreferencesPanelClass(wxWindow* parent,
																			mvceditor::LintPluginClass& plugin)
	: LintPluginPreferencesGeneratedPanelClass(parent, wxID_ANY)
	, Plugin(plugin) {
	wxGenericValidator checkValidator(&Plugin.CheckOnSave);
	wxGenericValidator ignoreValidator(&Plugin.IgnoreFiles);
	CheckOnSave->SetValidator(checkValidator);
	IgnoreFiles->SetValidator(ignoreValidator);
}

BEGIN_EVENT_TABLE(mvceditor::LintPluginClass, wxEvtHandler) 
	EVT_MENU(mvceditor::MENU_LINT_PHP + 0, mvceditor::LintPluginClass::OnLintMenu)
	EVT_MENU(mvceditor::MENU_LINT_PHP + 1, mvceditor::LintPluginClass::OnNextLintError)
	EVT_MENU(mvceditor::MENU_LINT_PHP + 2, mvceditor::LintPluginClass::OnPreviousLintError)
	EVT_COMMAND(wxID_ANY, EVENT_LINT_ERROR,  mvceditor::LintPluginClass::OnLintError)
	EVT_COMMAND(wxID_ANY, EVENT_FILE_READ,  mvceditor::LintPluginClass::OnLintFileComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::LintPluginClass::OnTimer)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_COMPLETE, mvceditor::LintPluginClass::OnLintComplete)
	EVT_PLUGIN_FILE_SAVED(mvceditor::LintPluginClass::OnFileSaved)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_PREFERENCES_UPDATED, mvceditor::LintPluginClass::SavePreferences)
END_EVENT_TABLE()