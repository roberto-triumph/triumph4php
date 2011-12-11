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
#include <unicode/unistr.h>
#include <wx/artprov.h>

const int ID_LINT_MENU_ITEM = mvceditor::PluginClass::newMenuId();
const int ID_LINT_TOOLBAR_ITEM = mvceditor::PluginClass::newMenuId();
const int ID_LINT_ERROR_COMMAND = mvceditor::PluginClass::newMenuId();
const int ID_LINT_RESULTS_PANEL = mvceditor::PluginClass::newMenuId();
const int ID_LINT_RESULTS_GAUGE = mvceditor::PluginClass::newMenuId();

mvceditor::ParserDirectoryWalkerClass::ParserDirectoryWalkerClass() 
	: LastResults()
	, PhpFileFilters()
	, WithErrors(0)
	, WithNoErrors(0)
	, Parser() {
		
}

void mvceditor::ParserDirectoryWalkerClass::ResetTotals() {
	WithErrors = 0;
	WithNoErrors = 0;
}

bool mvceditor::ParserDirectoryWalkerClass::Walk(const wxString& fileName) {
	bool ret = false;
	bool matchedFilter = false;
	for (size_t i = 0; i < PhpFileFilters.size(); ++i) {
		wxString filter = PhpFileFilters[i];
		matchedFilter = !wxIsWild(filter) || wxMatchWild(filter, fileName);
		if (matchedFilter) {
			break;
		}
	}
	if (matchedFilter) {
		LastResults.Error = UNICODE_STRING_SIMPLE("");
		LastResults.LineNumber = 0;
		LastResults.CharacterPosition = 0;
		if (Parser.LintFile(fileName, LastResults)) {
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

bool mvceditor::LintBackgroundFileReaderClass::BeginDirectoryLint(const wxString& directory, const std::vector<wxString>& phpFileFilters, mvceditor::BackgroundFileReaderClass::StartError& error) {
	bool good = false;
	PhpFileFilters = phpFileFilters;
	error = mvceditor::BackgroundFileReaderClass::NONE;
	if (Init(directory)) {
		ParserDirectoryWalker.ResetTotals();
		ParserDirectoryWalker.PhpFileFilters = PhpFileFilters;
		if (StartReading(error)) {
			good = true;
		}
	}
	return good;
}

bool mvceditor::LintBackgroundFileReaderClass::LintSingleFile(const wxString& fileName) {

	// ATTN: use a local instance of ParserClass so that this method is thread safe
	// and can be run when a background thread is already running.
	ParserDirectoryWalkerClass walker;
	walker.PhpFileFilters.clear();
	bool error = walker.Walk(fileName);
	if (error) {
		wxCommandEvent evt(EVENT_LINT_ERROR, ID_LINT_ERROR_COMMAND);

		// ATTN: will need to be DELETED!!
		mvceditor::LintResultsClass* clonedResults = new mvceditor::LintResultsClass;
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
		mvceditor::LintResultsClass* clonedResults = new mvceditor::LintResultsClass;
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
														std::vector<mvceditor::LintResultsClass*>& lintErrors)
	: LintResultsGeneratedPanelClass(parent, id) 
	, Notebook(notebook) 
	, LintErrors(lintErrors) {
			
}

mvceditor::LintResultsPanelClass::~LintResultsPanelClass() {
	ClearErrors();
}

void mvceditor::LintResultsPanelClass::AddError(LintResultsClass* lintError) {
	wxString err = StringHelperClass::IcuToWx(lintError->Error);
	wxString line;
	line.Printf(_("%s on %s line %d near Position %d\n"), 
		err.c_str(), 
		lintError->File.c_str(), 
		lintError->LineNumber,
		lintError->CharacterPosition);
	LintErrors.push_back(lintError);
	ErrorsList->AppendString(line);
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
	std::vector<mvceditor::LintResultsClass*>::iterator it = LintErrors.begin();
	int i = 0;
	while (it != LintErrors.end()) {
		if (fileName == (*it)->File) {
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
	mvceditor::LintResultsClass* results = LintErrors[index];
	wxString file = results->File;
	Notebook->LoadPage(file);
	Notebook->GetCurrentCodeControl()->MarkLintError(*results);
}


mvceditor::LintPluginClass::LintPluginClass() 
	: PluginClass()
	, LintBackgroundFileReader(*this)
	, LintErrors() {
}

void mvceditor::LintPluginClass::AddProjectMenuItems(wxMenu* projectMenu) {
	projectMenu->Append(ID_LINT_MENU_ITEM, _("Lint Check"), _("Performs syntax check on the current project"), wxITEM_NORMAL);
}

void mvceditor::LintPluginClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	wxBitmap bitmap = wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE, wxART_TOOLBAR, wxSize(16, 16));
	toolBar->AddTool(ID_LINT_TOOLBAR_ITEM, _("Lint Check"), bitmap, _("Performs syntax check on the current project"));
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
		if (LintBackgroundFileReader.BeginDirectoryLint(rootPath, phpFileFilters, error)) {
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

void mvceditor::LintPluginClass::OnLintError(wxCommandEvent& event) {
	mvceditor::LintResultsClass* results = (mvceditor::LintResultsClass*)event.GetClientData();
	
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

void mvceditor::LintPluginClass::OnFileSaved(wxCommandEvent &event) {
	mvceditor::CodeControlClass* codeControl = (mvceditor::CodeControlClass*)event.GetEventObject();
	wxString fileName = event.GetString();
	codeControl->ClearLintErrors();

	// remove lint results for this file fromt the display list
	wxWindow* window = FindToolsWindow(ID_LINT_RESULTS_PANEL);
	LintResultsPanelClass* lintResultsPanel = NULL;
	if (window) {
		lintResultsPanel = (LintResultsPanelClass*)window;
		lintResultsPanel->RemoveErrorsFor(fileName);
	}

	bool error = LintBackgroundFileReader.LintSingleFile(fileName);
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

BEGIN_EVENT_TABLE(mvceditor::LintPluginClass, wxEvtHandler) 
	EVT_MENU(ID_LINT_MENU_ITEM, mvceditor::LintPluginClass::OnLintMenu)
	EVT_MENU(ID_LINT_TOOLBAR_ITEM, mvceditor::LintPluginClass::OnLintMenu)
	EVT_COMMAND(wxID_ANY, EVENT_LINT_ERROR,  mvceditor::LintPluginClass::OnLintError)
	EVT_COMMAND(wxID_ANY, EVENT_FILE_READ,  mvceditor::LintPluginClass::OnLintFileComplete)
	EVT_COMMAND(wxID_ANY, EVENT_PLUGIN_FILE_SAVED,  mvceditor::LintPluginClass::OnFileSaved)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::LintPluginClass::OnTimer)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_COMPLETE, mvceditor::LintPluginClass::OnLintComplete)
END_EVENT_TABLE()