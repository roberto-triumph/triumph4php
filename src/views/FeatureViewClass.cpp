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

t4p::FeatureViewClass::FeatureViewClass() 
: wxEvtHandler() {}void t4p::FeatureViewClass::InitWindow(		StatusBarWithGaugeClass* statusBarWithGauge, 		NotebookClass* notebook, wxAuiNotebook* toolsNotebook, 		wxAuiNotebook* outlineNotebook, wxAuiManager* auiManager, 		wxMenuBar* menuBar) {	AuiManager = auiManager;	StatusBarWithGauge = statusBarWithGauge;	Notebook = notebook;	ToolsNotebook = toolsNotebook;	OutlineNotebook = outlineNotebook;	MenuBar = menuBar;
}void t4p::FeatureViewClass::AddViewMenuItems(wxMenu* viewMenu) {}void t4p::FeatureViewClass::AddSearchMenuItems(wxMenu* searchMenu) {}void t4p::FeatureViewClass::AddFileMenuItems(wxMenu* fileMenu) {}void t4p::FeatureViewClass::AddEditMenuItems(wxMenu* editMenu) {}void t4p::FeatureViewClass::AddHelpMenuItems(wxMenu* editMenu) {}void t4p::FeatureViewClass::AddNewMenu(wxMenuBar* menuBar) {}void t4p::FeatureViewClass::AddCodeControlClassContextMenuItems(wxMenu* menu) {}void t4p::FeatureViewClass::AddKeyboardShortcuts(std::vector<t4p::DynamicCmdClass>& shortcuts) {}void t4p::FeatureViewClass::AddWindows() {}void t4p::FeatureViewClass::AddPreferenceWindow(wxBookCtrlBase* parent) {}void t4p::FeatureViewClass::AddToolBarItems(wxAuiToolBar* toolBar) {}void t4p::FeatureViewClass::AddDynamicCmd(std::map<int, wxString> menuItemIds, std::vector<t4p::DynamicCmdClass>& shortcuts) {	for (std::map<int, wxString>::iterator it = menuItemIds.begin(); it != menuItemIds.end(); ++it) {		wxMenuItem* item = MenuBar->FindItem(it->first);		wxASSERT_MSG(item, wxT("Menu item not found:") + it->second);		t4p::DynamicCmdClass cmd(item, it->second);		shortcuts.push_back(cmd);	}}

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

t4p::NotebookClass* t4p::FeatureViewClass::GetNotebook() const {
	return Notebook;
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
	return Notebook->GetCurrentCodeControl();
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
	t4p::NotebookClass* notebook = GetNotebook();
	notebook->AddTriumphPage(type);
	if (!tabName.IsEmpty()) {
		notebook->SetPageText(notebook->GetSelection(), tabName);
	}
	t4p::CodeControlClass* ctrl = notebook->GetCurrentCodeControl();
	return ctrl;
}

void t4p::FeatureViewClass::LoadCodeControl(const wxString& fileName) {
	t4p::NotebookClass* notebook = GetNotebook();
	notebook->LoadPage(fileName);
}

std::vector<t4p::CodeControlClass*> t4p::FeatureViewClass::AllCodeControls() const {
	std::vector<t4p::CodeControlClass*> ctrls;
	t4p::NotebookClass* notebook = GetNotebook();
	for (size_t i = 0; i < notebook->GetPageCount(); ++i) {
		t4p::CodeControlClass* ctrl = notebook->GetCodeControl(i);
		ctrls.push_back(ctrl);
	}
	return ctrls;
}