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
#include <views/FeatureViewClass.h>
#include <widgets/NotebookClass.h>

/**
 * Returns the code control that has focus.
 * @param mainWindow
 * @return the code control that has focus, or NULL if focus is on
 *         another window
 */
static t4p::CodeControlClass* FindFocusedCodeControl(wxWindow* mainWindow) {
	t4p::CodeControlClass* codeCtrl = NULL;
	wxWindow* focusWindow = wxWindow::FindFocus();
	if (!focusWindow) {
		return codeCtrl;
	}

	// we don't just want to cast since we don't know if the focused window
	// is a code control. We find out if the focus is anywhere inside a
	// notebook; either the notebook itself, or one of its tabs or borders.
	// the focused window is in one of the code notebooks
	std::vector<t4p::NotebookClass*> notebooks = t4p::CodeNotebooks(mainWindow);
	for (size_t i = 0; i < notebooks.size(); ++i) {
		t4p::NotebookClass* notebook = notebooks[i];
		if (focusWindow == notebook || notebook->IsDescendant(focusWindow)) {
			codeCtrl = notebook->GetCurrentCodeControl();
		}
	}
	return codeCtrl;
}

static bool FindFocusedCodeControlWithNotebook(wxWindow* mainWindow,
		t4p::CodeControlClass** codeCtrl, t4p::NotebookClass** notebook) {
	bool found = false;
	*codeCtrl = FindFocusedCodeControl(mainWindow);
	if (!(*codeCtrl)) {
		return found;
	}

	// get the notebook parent for the focused code control
	std::vector<t4p::NotebookClass*> notebooks = t4p::CodeNotebooks(mainWindow);
	for (size_t i = 0; i < notebooks.size(); ++i) {
		if (notebooks[i]->GetPageIndex(*codeCtrl) != wxNOT_FOUND) {
			*notebook = notebooks[i];
			found = true;
		}
	}
	return found;
}

t4p::FeatureViewClass::FeatureViewClass() 
: wxEvtHandler() {
}

void t4p::FeatureViewClass::InitWindow(
		StatusBarWithGaugeClass* statusBarWithGauge, 
		wxAuiNotebook* toolsNotebook,
		wxAuiNotebook* outlineNotebook, wxAuiManager* auiManager, 
		wxMenuBar* menuBar, wxAuiToolBar* toolBar) {
	AuiManager = auiManager;
	StatusBarWithGauge = statusBarWithGauge;
	ToolsNotebook = toolsNotebook;
	OutlineNotebook = outlineNotebook;
	MenuBar = menuBar;
	ToolBar = toolBar;
}

void t4p::FeatureViewClass::AddViewMenuItems(wxMenu* viewMenu) {
}

void t4p::FeatureViewClass::AddSearchMenuItems(wxMenu* searchMenu) {
}

void t4p::FeatureViewClass::AddFileMenuItems(wxMenu* fileMenu) {
}

void t4p::FeatureViewClass::AddEditMenuItems(wxMenu* editMenu) {
}

void t4p::FeatureViewClass::AddHelpMenuItems(wxMenu* editMenu) {
}

void t4p::FeatureViewClass::AddNewMenu(wxMenuBar* menuBar) {
}

void t4p::FeatureViewClass::AddCodeControlClassContextMenuItems(wxMenu* menu) {
}

void t4p::FeatureViewClass::AddKeyboardShortcuts(std::vector<t4p::DynamicCmdClass>& shortcuts) {
}

void t4p::FeatureViewClass::AddWindows() {
}

void t4p::FeatureViewClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
}

void t4p::FeatureViewClass::AddToolBarItems(wxAuiToolBar* toolBar) {
}

void t4p::FeatureViewClass::AddDynamicCmd(std::map<int, wxString> menuItemIds, std::vector<t4p::DynamicCmdClass>& shortcuts) {
	for (std::map<int, wxString>::iterator it = menuItemIds.begin(); it != menuItemIds.end(); ++it) {
		wxMenuItem* item = MenuBar->FindItem(it->first);
		wxASSERT_MSG(item, wxT("Menu item not found:") + it->second);
		t4p::DynamicCmdClass cmd(item, it->second);
		shortcuts.push_back(cmd);
	}
}

wxWindow* t4p::FeatureViewClass::FindToolsWindow(int windowId) const {
	return wxWindow::FindWindowById(windowId, GetToolsNotebook());
}

wxWindow* t4p::FeatureViewClass::FindOutlineWindow(int windowId) const {
	return wxWindow::FindWindowById(windowId, GetOutlineNotebook());
}

bool t4p::FeatureViewClass::IsToolsWindowSelected(int windowId) const {
	wxWindow* window = wxWindow::FindWindowById(windowId, GetToolsNotebook());
	wxAuiPaneInfo info = AuiManager->GetPane(ToolsNotebook);
	if (!info.IsShown()) {
		
		// if the notebook itself is not shown, it means that the window
		// is hidden because its parent is hidden
		return false;
	}
	int windowIndex = ToolsNotebook->GetPageIndex(window); 
	return windowIndex != wxNOT_FOUND && windowIndex == ToolsNotebook->GetSelection();
}

bool t4p::FeatureViewClass::IsToolsWindowSelectedByName(const wxString& name) const {
	wxAuiPaneInfo info = AuiManager->GetPane(ToolsNotebook);
	if (!info.IsShown()) {
		
		// if the notebook itself is not shown, it means that the outline window
		// is hidden because its parent is hidden
		return false;
	}
	size_t selection = ToolsNotebook->GetSelection();
	if (selection < ToolsNotebook->GetPageCount()) {
		return ToolsNotebook->GetPage(selection)->GetName() == name;
	}
	return false;
}

bool t4p::FeatureViewClass::IsOutlineWindowSelected(int windowId) const {
	
	wxWindow* window = wxWindow::FindWindowById(windowId, GetOutlineNotebook());
	wxAuiPaneInfo info = AuiManager->GetPane(OutlineNotebook);
	if (!info.IsShown()) {
		
		// if the notebook itself is not shown, it means that the window
		// is hidden because its parent is hidden
		return false;
	}
	int windowIndex = OutlineNotebook->GetPageIndex(window); 
	return windowIndex != wxNOT_FOUND && windowIndex == OutlineNotebook->GetSelection();
}

wxAuiNotebook* t4p::FeatureViewClass::GetToolsNotebook() const {
	return ToolsNotebook;
}

wxAuiNotebook* t4p::FeatureViewClass::GetOutlineNotebook() const {
	return OutlineNotebook;
}

void t4p::FeatureViewClass::SetFocusToToolsWindow(wxWindow* window) {
	int index = ToolsNotebook->GetPageIndex(window);
	if (index != wxNOT_FOUND) {
		ToolsNotebook->SetSelection(index);
		if (NULL != AuiManager) {
			AuiManager->GetPane(ToolsNotebook).Show();
			AuiManager->Update();
		}
	}
}

void t4p::FeatureViewClass::SetFocusToOutlineWindow(wxWindow* window) {
	int index = OutlineNotebook->GetPageIndex(window);
	if (index != wxNOT_FOUND) {
		OutlineNotebook->SetSelection(index);
		if (NULL != AuiManager) {
			AuiManager->GetPane(OutlineNotebook).Show();
			AuiManager->Update();
		}
	}
}

bool t4p::FeatureViewClass::AddToolsWindow(wxWindow* window, wxString tabName, wxString windowName, const wxBitmap& bitmap) {
	if (!windowName.IsEmpty()) {
		window->SetName(windowName);
	}
	if (ToolsNotebook->AddPage(window, tabName, true, bitmap)) {
		if (NULL != AuiManager) {
			AuiManager->GetPane(ToolsNotebook).Show();
			AuiManager->Update();
		}
		return true;
	}
	return false;
}

bool t4p::FeatureViewClass::AddOutlineWindow(wxWindow* window, wxString name, const wxBitmap& bitmap) {
	if (OutlineNotebook->AddPage(window, name, true, bitmap)) {
		if (NULL != AuiManager) {
			AuiManager->GetPane(OutlineNotebook).Show();
			AuiManager->Update();
		}
		return true;
	}
	wxASSERT_MSG(false, wxT("could not add window to outline notebook"));
	return false;
}

t4p::CodeControlClass* t4p::FeatureViewClass::GetCurrentCodeControl() const {
	return FindFocusedCodeControl(GetMainWindow());
}

bool t4p::FeatureViewClass::GetCurrentCodeControlWithNotebook(
		t4p::CodeControlClass** codeCtrl, t4p::NotebookClass** notebook) const {
	return FindFocusedCodeControlWithNotebook(GetMainWindow(), codeCtrl, notebook);
}

wxWindow* t4p::FeatureViewClass::GetMainWindow() const {
	return ToolsNotebook->GetParent();
}

t4p::StatusBarWithGaugeClass* t4p::FeatureViewClass::GetStatusBarWithGauge() const {
	return StatusBarWithGauge;
}

wxString t4p::FeatureViewClass::GetSelectedText() const {
	CodeControlClass* page = GetCurrentCodeControl();
	wxString selectedText;
	if (page) {
		selectedText = page->GetSelectedText();
	}
	return selectedText;
}

t4p::CodeControlClass* t4p::FeatureViewClass::CreateCodeControl(const wxString& tabName, t4p::FileType type) const {
	t4p::NotebookClass* notebook = NULL;
	t4p::CodeControlClass* codeCtrl = NULL;
	t4p::CodeControlClass* newCtrl = NULL;
	if (!GetCurrentCodeControlWithNotebook(&codeCtrl, &notebook)) {

		// as a last resort, add to the first notebook
		std::vector<t4p::NotebookClass*> notebooks = t4p::CodeNotebooks(GetMainWindow());
		if (!notebooks.empty()) {
			notebook = notebooks[0];
		}
	}
	if (notebook) {
		notebook->AddTriumphPage(type);
		if (!tabName.IsEmpty()) {
			notebook->SetPageText(notebook->GetSelection(), tabName);
		}
		newCtrl = notebook->GetCurrentCodeControl();
	}
	return newCtrl;
}

void t4p::FeatureViewClass::LoadCodeControl(const wxString& fileName) {

	// is this file already loaded? if so, just set focus to it. need to look
	// at all of the notebooks
	t4p::CodeControlClass* existing = FindCodeControlAndSelect(fileName);
	if (existing) {
		return;
	}

	// file is not open, open it now. put the new file in the notebook
	// that currently has focus
	t4p::NotebookClass* currentNotebook = NULL;
	t4p::CodeControlClass* currentCode = NULL;
	if (GetCurrentCodeControlWithNotebook(&currentCode, &currentNotebook)) {
		currentNotebook->LoadPage(fileName);
		return;
	}

	// as a final precaution, put the file in the first notebook
	std::vector<t4p::NotebookClass*> notebooks = t4p::CodeNotebooks(GetMainWindow());
	if (!notebooks.empty()) {
		notebooks[0]->LoadPage(fileName);
	}
}

std::vector<t4p::CodeControlClass*> t4p::FeatureViewClass::AllCodeControls() const {
	std::vector<t4p::CodeControlClass*> ctrls;
	std::vector<t4p::NotebookClass*> notebooks = t4p::CodeNotebooks(GetMainWindow());
	for (size_t n = 0 ; n < notebooks.size(); ++n) {
		t4p::NotebookClass* notebook = notebooks[n];
		for (size_t p = 0; p < notebook->GetPageCount(); ++p) {
			t4p::CodeControlClass* ctrl = notebook->GetCodeControl(p);
			ctrls.push_back(ctrl);
		}
	}
	return ctrls;
}

t4p::CodeControlClass* t4p::FeatureViewClass::FindCodeControl(const wxString& fullPath) const {
	std::vector<t4p::NotebookClass*> notebooks = t4p::CodeNotebooks(GetMainWindow());
	t4p::CodeControlClass* code = NULL;
	for (size_t i = 0; i < notebooks.size(); ++i) {
		t4p::NotebookClass* notebook = notebooks[i];
		code = notebook->FindCodeControl(fullPath);
		if (code) {
			return code;
		}
	}
	return code;
}

std::vector<wxString> t4p::FeatureViewClass::AllOpenedFiles() const {
	std::vector<t4p::NotebookClass*> notebooks = t4p::CodeNotebooks(GetMainWindow());
	std::vector<wxString> allFiles;
	for (size_t i = 0; i < notebooks.size(); ++i) {
		t4p::NotebookClass* notebook = notebooks[i];
		std::vector<wxString> files = notebook->GetOpenedFiles();
		allFiles.insert(allFiles.end(), files.begin(), files.end());
	}
	return allFiles;
}

wxString t4p::FeatureViewClass::GetCodeNotebookTabText(t4p::CodeControlClass* codeCtrl) {
	wxString ret;
	std::vector<t4p::NotebookClass*> notebooks = t4p::CodeNotebooks(GetMainWindow());
	for (size_t i = 0; i < notebooks.size(); ++i) {
		t4p::NotebookClass* notebook = notebooks[i];
		int pos = notebook->GetPageIndex(codeCtrl);
		if (pos != wxNOT_FOUND) {
			ret = notebook->GetPageText(pos);
			return ret;
		}
	}
	return ret;
}

t4p::CodeControlClass* t4p::FeatureViewClass::FindCodeControlAndSelect(const wxString& fullPath) const {
	std::vector<t4p::NotebookClass*> notebooks = t4p::CodeNotebooks(GetMainWindow());
	t4p::CodeControlClass* codeCtrl = NULL;
	for (size_t i = 0; i < notebooks.size(); ++i) {
		t4p::NotebookClass* notebook = notebooks[i];
		codeCtrl = notebook->FindCodeControl(fullPath);
		if (codeCtrl) {
			int currentSelectionIndex = notebook->GetSelection();
			int pageIndex = notebook->GetPageIndex(codeCtrl);

			// the bookmark may be in a page that is not active, need to
			// swith notebook tabs if needed
			if (pageIndex != currentSelectionIndex) {
				notebook->SetSelection(pageIndex);
			}
		}
	}
	return codeCtrl;
}

void t4p::FeatureViewClass::CloseCodeControl(t4p::CodeControlClass* codeCtrl) {
	std::vector<t4p::NotebookClass*> notebooks = t4p::CodeNotebooks(GetMainWindow());
	for (size_t i = 0; i < notebooks.size(); ++i) {
		t4p::NotebookClass* notebook = notebooks[i];
		int pageIndex = notebook->GetPageIndex(codeCtrl);
		if (pageIndex != wxNOT_FOUND) {
			notebook->ClosePage(pageIndex);
			break;
		}
	}
}