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
 * @copyright  2015 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <views/FileOperationsViewClass.h>
#include <widgets/NotebookClass.h>
#include <Triumph.h>
#include <widgets/AuiManager.h>
#include <wx/artprov.h>
#include <wx/choicdlg.h>


static const int ID_STATUS_BAR_TIMER = wxNewId();

/**
 * @param notebooks
 * @param mainWindow
 * @return boolean TRUE if the user has decided to save or ignore the changes.
 *         When this function returns FALSE, it means that the user wants to
 *         cancel the app exit. (ie does not want to quit)
 */
static bool SaveAllModifiedPages(std::vector<t4p::NotebookClass*> notebooks, wxWindow* mainWindow) {
	std::vector<int> modifiedPageIndexes;
	std::vector<wxString> modifiedPageNames;
	std::vector<int> notebookIndexes;
	bool changed = true;

	for (size_t n = 0; n < notebooks.size(); n++) {
		t4p::NotebookClass* notebook = notebooks[n];
		for (size_t p = 0; p < notebook->GetPageCount(); ++p) {
			if (notebook->IsPageModified(p)) {
				modifiedPageIndexes.push_back(modifiedPageNames.size());
				notebookIndexes.push_back(n);
				wxString pageName = notebook->GetPageText(p);
				if (pageName.EndsWith(wxT("*"))) {
					pageName = pageName.SubString(0, pageName.size() - 2);
				}
				modifiedPageNames.push_back(pageName);
			}
		}
	}
	if (modifiedPageIndexes.size()) {
		wxArrayString modifiedPageNamesArray;
		for (size_t i = 0; i < modifiedPageNames.size(); ++i) {
			modifiedPageNamesArray.Add(modifiedPageNames[i]);
		}
		wxMultiChoiceDialog dialog(mainWindow, wxT(
			"Do you wish to save any files before exiting?"),
			wxT("Save Files"), modifiedPageNamesArray,
			wxDEFAULT_DIALOG_STYLE | wxOK | wxCANCEL | wxRESIZE_BORDER);
		dialog.Center();
		if (wxID_CANCEL == dialog.ShowModal()) {
			changed = false;
		} else {
			wxArrayInt selections = dialog.GetSelections();
			for (size_t i = 0; i < selections.size(); ++i) {
				t4p::NotebookClass* notebook = notebooks[notebookIndexes[selections[i]]];
				notebook->SavePage(modifiedPageIndexes[selections[i]], false);
			}
		}
	}
	return changed;
}

t4p::FileOperationsViewClass::FileOperationsViewClass(t4p::FileOperationsFeatureClass& feature)
: FeatureViewClass()
, Feature(feature)
, StatusBarTimer(this, ID_STATUS_BAR_TIMER) {
}

void t4p::FileOperationsViewClass::AddFileMenuItems(wxMenu* fileMenu) {
	// use stock menu IDs so that default shortcuts are enabled on each different OS
	fileMenu->Append(t4p::MENU_FILE_OPERATIONS + 0, _("New &PHP File\tCTRL+N"), _("Create a new PHP File"), wxITEM_NORMAL);
	fileMenu->Append(t4p::MENU_FILE_OPERATIONS + 1, _("New S&QL File"), _("Create a new SQL File"), wxITEM_NORMAL);
	fileMenu->Append(t4p::MENU_FILE_OPERATIONS + 2, _("New &CSS File"), _("Create a new CSS File"), wxITEM_NORMAL);
	fileMenu->Append(t4p::MENU_FILE_OPERATIONS + 3, _("New Te&xt File"), _("Create a new text file"), wxITEM_NORMAL);
	fileMenu->Append(wxID_OPEN, _("&Open"), _("Open a file"), wxITEM_NORMAL);
	fileMenu->Append(wxID_SAVE, _("&Save"), _("Save the current file to disk"), wxITEM_NORMAL);
	fileMenu->Append(wxID_SAVEAS, _("Save &As"), _("Save the current contents to a different file on disk"), wxITEM_NORMAL);
	fileMenu->Append(t4p::MENU_FILE_OPERATIONS + 4, _("Save A&ll\tCTRL+SHIFT+S"), _("Save all opened files to disk"), wxITEM_NORMAL);
	fileMenu->Append(t4p::MENU_FILE_OPERATIONS + 5, _("&Revert"), _("Reload the file from Disk"), wxITEM_NORMAL);
	fileMenu->Append(wxID_CLOSE, _("&Close"), _("Close the current file"), wxITEM_NORMAL);

	fileMenu->Enable(wxID_SAVE, false);
	fileMenu->Enable(wxID_SAVEAS, false);
	fileMenu->Enable(t4p::MENU_FILE_OPERATIONS + 4, false);  // save all
	fileMenu->Enable(t4p::MENU_FILE_OPERATIONS + 5, false);  // revert
	fileMenu->Enable(wxID_CLOSE, false);
}

void t4p::FileOperationsViewClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	toolBar->AddTool(t4p::MENU_FILE_OPERATIONS + 6, _("Save"), wxArtProvider::GetBitmap(
		wxART_FILE_SAVE, wxART_TOOLBAR, wxSize(16, 16)), _("Save"));
	toolBar->EnableTool(t4p::MENU_FILE_OPERATIONS + 6, false);
}

void t4p::FileOperationsViewClass::AddKeyboardShortcuts(std::vector<t4p::DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[t4p::MENU_FILE_OPERATIONS + 0] = wxT("File-New PHP File");
	menuItemIds[wxID_OPEN] = wxT("File-Open");
	menuItemIds[wxID_SAVE] = wxT("File-Save");
	menuItemIds[wxID_SAVEAS] = wxT("File-Save As");
	menuItemIds[t4p::MENU_FILE_OPERATIONS + 4] = wxT("File-Save All");
	menuItemIds[t4p::MENU_FILE_OPERATIONS + 5] = wxT("File-Revert");
	menuItemIds[wxID_EXIT] = wxT("File-Exit");

	AddDynamicCmd(menuItemIds, shortcuts);

	// ATTN: do this here, since we want the close file shortcut to
	// be different per OS
	// so that in Mac, we use the same close shortcut as other Editors
	wxPlatformInfo info;
	wxMenuItem* item = MenuBar->FindItem(wxID_CLOSE);
	wxASSERT_MSG(item, wxT("Menu item not found wxID_CLOSE"));
	t4p::DynamicCmdClass cmd(item, wxT("File-Close"));
	if (info.GetOperatingSystemId() & wxOS_WINDOWS_NT) {
		cmd.AddShortcut(wxT("CTRL+F4"));
	} else {
		cmd.AddShortcut(wxT("CTRL+W"));
	}
	shortcuts.push_back(cmd);
}

void t4p::FileOperationsViewClass::OnAppReady(wxCommandEvent& event) {
	StartStatusBarTimer();
}

void t4p::FileOperationsViewClass::OnFileSave(wxCommandEvent& event) {
	t4p::NotebookClass* notebook = NULL;
	t4p::CodeControlClass* codeControl = NULL;
	if (GetCurrentCodeControlWithNotebook(&codeControl, &notebook)) {
		notebook->SaveCurrentPage();
	}
}

void t4p::FileOperationsViewClass::OnFilePhpNew(wxCommandEvent& event) {
	CreateCodeControl("", t4p::FILE_TYPE_PHP);
}

void t4p::FileOperationsViewClass::OnFileSqlNew(wxCommandEvent& event) {
	CreateCodeControl("", t4p::FILE_TYPE_SQL);
}

void t4p::FileOperationsViewClass::OnFileCssNew(wxCommandEvent& event) {
	CreateCodeControl("", t4p::FILE_TYPE_CSS);
}

void t4p::FileOperationsViewClass::OnFileTextNew(wxCommandEvent& event) {
	CreateCodeControl("", t4p::FILE_TYPE_TEXT);
}

void t4p::FileOperationsViewClass::OnFileOpen(wxCommandEvent& event) {
	t4p::NotebookClass* notebook = NULL;
	t4p::CodeControlClass* codeControl = NULL;
	if (GetCurrentCodeControlWithNotebook(&codeControl, &notebook)) {
		notebook->LoadPage();
	}
}

void t4p::FileOperationsViewClass::OnFileSaveAs(wxCommandEvent& event) {
	t4p::NotebookClass* notebook = NULL;
	t4p::CodeControlClass* codeControl = NULL;
	if (GetCurrentCodeControlWithNotebook(&codeControl, &notebook)) {
		notebook->SaveCurrentPageAsNew();
	}
}

void t4p::FileOperationsViewClass::OnFileSaveAll(wxCommandEvent& event) {
	t4p::NotebookClass* notebook = NULL;
	t4p::CodeControlClass* codeControl = NULL;
	if (GetCurrentCodeControlWithNotebook(&codeControl, &notebook)) {
		notebook->SaveAllModifiedPagesWithoutPrompting();
	}
}

void t4p::FileOperationsViewClass::FileOpen(const std::vector<wxString>& filenames) {
	t4p::NotebookClass* notebook = NULL;
	t4p::CodeControlClass* codeControl = NULL;
	if (GetCurrentCodeControlWithNotebook(&codeControl, &notebook)) {
		notebook->LoadPages(filenames);
	}
}
void t4p::FileOperationsViewClass::FileOpenPosition(const wxString& fullPath, int startingPos, int length) {
	LoadCodeControl(fullPath);
	CodeControlClass* codeControl = GetCurrentCodeControl();
	if (codeControl) {
		codeControl->SetSelectionAndEnsureVisible(startingPos, startingPos + length);
	}
}

void t4p::FileOperationsViewClass::FileOpenLine(const wxString& fullPath, int lineNumber) {
	LoadCodeControl(fullPath);
	CodeControlClass* codeControl = GetCurrentCodeControl();
	if (codeControl) {
		codeControl->GotoLineAndEnsureVisible(lineNumber);
	}
}

void t4p::FileOperationsViewClass::OnFileClose(wxCommandEvent& event) {
	t4p::NotebookClass* notebook = NULL;
	t4p::CodeControlClass* codeControl = NULL;
	if (GetCurrentCodeControlWithNotebook(&codeControl, &notebook)) {
		notebook->CloseCurrentPage();
	}
}

void t4p::FileOperationsViewClass::OnFileRevert(wxCommandEvent& event) {
	CodeControlClass* code = GetCurrentCodeControl();
	if (NULL != code && !code->IsNew()) {
		int res = wxMessageBox(_("Reload file and lose all changes?"), _("Triumph"), wxICON_QUESTION | wxYES_NO, GetMainWindow());
		if (wxYES == res) {
			int currentLine = code->GetCurrentLine();
			code->Freeze();
			code->Revert();

			if (currentLine <= code->GetLineCount()) {
				// stc uses zero-based line numbers, this method
				// accepts 1-based line numbers
				code->GotoLineAndEnsureVisible(currentLine + 1);
			}
			code->Thaw();

			t4p::CodeControlEventClass revertEvt(t4p::EVENT_APP_FILE_REVERTED, code);
			Feature.App.EventSink.Publish(revertEvt);
		}
	}
}

void t4p::FileOperationsViewClass::EnableSave(wxStyledTextEvent& event) {
	ToolBar->EnableTool(t4p::MENU_FILE_OPERATIONS + 6, true);
	ToolBar->Refresh();
	MenuUpdate(false);

	t4p::NotebookClass* notebook = NULL;
	t4p::CodeControlClass* codeControl = NULL;
	if (GetCurrentCodeControlWithNotebook(&codeControl, &notebook)) {
		notebook->MarkPageAsModified(event.GetId());
	}
}

void t4p::FileOperationsViewClass::DisableSave(wxStyledTextEvent& event) {
	ToolBar->EnableTool(t4p::MENU_FILE_OPERATIONS + 6, false);
	ToolBar->Refresh();

	MenuUpdate(false);

	t4p::NotebookClass* notebook = NULL;
	t4p::CodeControlClass* codeControl = NULL;
	if (GetCurrentCodeControlWithNotebook(&codeControl, &notebook)) {
		notebook->MarkPageAsNotModified(event.GetId());
	}
}

void t4p::FileOperationsViewClass::SaveCurrentFile(wxCommandEvent& event) {
	t4p::NotebookClass* notebook = NULL;
	t4p::CodeControlClass* codeControl = NULL;
	if (GetCurrentCodeControlWithNotebook(&codeControl, &notebook)) {
		notebook->SaveCurrentPage();
	}
}

void t4p::FileOperationsViewClass::OnAppFilePageChanged(t4p::CodeControlEventClass& event) {
	UpdateStatusBar();
	MenuUpdate(false);

	t4p::NotebookClass* notebook = NULL;
	t4p::CodeControlClass* codeControl = NULL;
	if (GetCurrentCodeControlWithNotebook(&codeControl, &notebook)) {
		int newPage = notebook->GetPageIndex(event.GetCodeControl());
		bool isPageModified = notebook->IsPageModified(newPage);
		ToolBar->EnableTool(t4p::MENU_FILE_OPERATIONS + 6, isPageModified);
		ToolBar->Refresh();
	}
	event.Skip();
}

void t4p::FileOperationsViewClass::OnCmdFileOpen(t4p::OpenFileCommandEventClass& event) {
	if (event.LineNumber > 0) {
		FileOpenLine(event.FullPath, event.LineNumber);
	} else {
		FileOpenPosition(event.FullPath, event.StartingPos, event.Length);
	}
}

void t4p::FileOperationsViewClass::StartStatusBarTimer() {
	StatusBarTimer.Start(250, wxTIMER_CONTINUOUS);
}

void t4p::FileOperationsViewClass::OnStatusBarTimer(wxTimerEvent& event) {
	/*
	 * Update the status bar info (line, column)
	 * at specific intervals instead of in a EVT_STC_UPDATEUI event.
	 * Updating the status bar text triggers a refresh of the whole
	 * status bar (and it seems that the entire app is refreshed too)
	 * and it makes the app feel sluggish.
	 */
	UpdateStatusBar();
}


void t4p::FileOperationsViewClass::OnAppFileClosed(t4p::CodeControlEventClass& event) {
	// in case all notebook tabs have been closed, we need to refresh the cursor position
	UpdateStatusBar();
	MenuUpdate(true);
}

void t4p::FileOperationsViewClass::OnAppFileNew(t4p::CodeControlEventClass& event) {
	MenuUpdate(false);
}

void t4p::FileOperationsViewClass::OnAppFileOpened(t4p::CodeControlEventClass& event) {
	MenuUpdate(false);
}

void t4p::FileOperationsViewClass::UpdateStatusBar() {
	CodeControlClass* codeControl = GetCurrentCodeControl();
	if (codeControl) {
		int pos = codeControl->GetCurrentPos();

		// scintilla lines and columns are zero-based
		// we want to be 1-based (human friendly)
		// but offset we want to be 0-based
		int line = codeControl->LineFromPosition(pos) + 1;
		int column = codeControl->GetColumn(pos) + 1;

		wxString s = wxString::Format(wxT("Line:%d Column:%d Offset:%d"), line, column, pos);
		GetStatusBarWithGauge()->SetColumn1Text(s);
	} else {
		GetStatusBarWithGauge()->SetColumn1Text(wxEmptyString);
	}
}

void t4p::FileOperationsViewClass::MenuUpdate(bool isClosingPage) {
	bool hasEditors = false;
	int currentPage = -1;
	t4p::NotebookClass* notebook = NULL;
	t4p::CodeControlClass* codeControl = NULL;
	if (GetCurrentCodeControlWithNotebook(&codeControl, &notebook)) {
		if (isClosingPage) {
			// since we get the close event BEFORE the code control
			// is removed
			hasEditors = notebook->GetPageCount() > 1;
		} else {
			hasEditors = notebook->GetPageCount() > 0;
		}

		currentPage = notebook->GetSelection();
	}
		// if at least one document needs to be saved, enable the save all menu
	bool someModified = false;
	std::vector<t4p::CodeControlClass*> codeCtrls = AllCodeControls();
	for (size_t i = 0; i < codeCtrls.size(); ++i) {
		t4p::CodeControlClass* ctrl = codeCtrls[i];
		if (ctrl) {
			someModified |= ctrl->GetModify();
		}
	}

	MenuBar->Enable(wxID_SAVE, hasEditors && notebook && notebook->IsPageModified(currentPage));
	MenuBar->Enable(wxID_SAVEAS, hasEditors && NULL != codeControl);
	MenuBar->Enable(wxID_CLOSE, hasEditors);
	MenuBar->Enable(t4p::MENU_FILE_OPERATIONS + 4, someModified);  // save all
	MenuBar->Enable(t4p::MENU_FILE_OPERATIONS + 5,
		hasEditors && notebook && notebook->IsPageModified(currentPage) &&
		NULL != codeControl && !codeControl->IsNew()
	);  // revert
}

void t4p::FileOperationsViewClass::OnAppFrameClose(wxNotifyEvent& event) {
	bool doVeto = false;

	// if we have any files that are saved using privilege mode
	// lets tell the user to save them first.  we don't
	// want to close the app while the async save process
	// is running
	bool needsElevation = NeedsElevatedSave();
	if (needsElevation) {
		wxMessageBox(
			_("You have modified files that need to be saved with escalated privileges.\n") +
			_("Please save those files or discard the changes before exiting"),
			_("Triumph"));
		doVeto = true;
	}
	if (!doVeto) {
		std::vector<t4p::NotebookClass*> notebooks = t4p::AuiAllCodeNotebooks(*AuiManager);
		doVeto = !SaveAllModifiedPages(notebooks, GetMainWindow());
	}

	if (doVeto) {
		event.Veto();
	}
}

bool t4p::FileOperationsViewClass::NeedsElevatedSave() {
	bool needsElevation = false;
	std::vector<t4p::CodeControlClass*> codeCtrls = AllCodeControls();
	for (size_t i = 0; i < codeCtrls.size(); i++) {
		t4p::CodeControlClass* ctrl = codeCtrls[i];
		wxString filename = ctrl->GetFileName();
		if (!ctrl->IsNew() && ctrl->IsModified()
			&& !filename.empty()
			&& wxFileName::FileExists(filename)
			&& !wxFileName::IsFileWritable(filename)) {
			needsElevation = true;
			break;
		}
	}
	return needsElevation;
}

BEGIN_EVENT_TABLE(t4p::FileOperationsViewClass, t4p::FeatureViewClass)
	EVT_MENU(t4p::MENU_FILE_OPERATIONS + 0, t4p::FileOperationsViewClass::OnFilePhpNew)
	EVT_MENU(t4p::MENU_FILE_OPERATIONS + 1, t4p::FileOperationsViewClass::OnFileSqlNew)
	EVT_MENU(t4p::MENU_FILE_OPERATIONS + 2, t4p::FileOperationsViewClass::OnFileCssNew)
	EVT_MENU(t4p::MENU_FILE_OPERATIONS + 3, t4p::FileOperationsViewClass::OnFileTextNew)
	EVT_MENU(wxID_OPEN, t4p::FileOperationsViewClass::OnFileOpen)
	EVT_MENU(wxID_SAVE, t4p::FileOperationsViewClass::OnFileSave)
	EVT_MENU(wxID_SAVEAS, t4p::FileOperationsViewClass::OnFileSaveAs)
	EVT_MENU(t4p::MENU_FILE_OPERATIONS + 4, t4p::FileOperationsViewClass::OnFileSaveAll)
	EVT_MENU(t4p::MENU_FILE_OPERATIONS + 5, t4p::FileOperationsViewClass::OnFileRevert)
	EVT_MENU(wxID_CLOSE, t4p::FileOperationsViewClass::OnFileClose)
	EVT_MENU(t4p::MENU_FILE_OPERATIONS + 6, t4p::FileOperationsViewClass::OnFileSave)

	EVT_STC_SAVEPOINTREACHED(wxID_ANY, t4p::FileOperationsViewClass::DisableSave)
	EVT_STC_SAVEPOINTLEFT(wxID_ANY, t4p::FileOperationsViewClass::EnableSave)
	EVT_CMD_FILE_OPEN(t4p::FileOperationsViewClass::OnCmdFileOpen)

	EVT_TIMER(ID_STATUS_BAR_TIMER, t4p::FileOperationsViewClass::OnStatusBarTimer)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_READY, t4p::FileOperationsViewClass::OnAppReady)

	EVT_APP_FILE_PAGE_CHANGED(t4p::FileOperationsViewClass::OnAppFilePageChanged)
	EVT_APP_FILE_CLOSED(t4p::FileOperationsViewClass::OnAppFileClosed)
	EVT_APP_FILE_NEW(t4p::FileOperationsViewClass::OnAppFileNew)
	EVT_APP_FILE_OPEN(t4p::FileOperationsViewClass::OnAppFileOpened)
	EVT_APP_FRAME_CLOSE(t4p::FileOperationsViewClass::OnAppFrameClose)
END_EVENT_TABLE()
