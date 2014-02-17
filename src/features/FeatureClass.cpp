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
#include <features/FeatureClass.h>
#include <Triumph.h>
#include <wx/artprov.h>

t4p::FeatureClass::FeatureClass(t4p::AppClass& app)
	: App(app)
	, AuiManager(NULL)	
	, StatusBarWithGauge(NULL)
	, Notebook(NULL)
	, ToolsNotebook(NULL)
	, OutlineNotebook(NULL)
	, MenuBar(NULL) {
}

t4p::FeatureClass::~FeatureClass() {
}

void t4p::FeatureClass::InitWindow(StatusBarWithGaugeClass* statusBarWithGauge, NotebookClass* notebook, wxAuiNotebook* toolsNotebook, 
	wxAuiNotebook* outlineNotebook, wxAuiManager* auiManager, wxMenuBar* menuBar) {
	StatusBarWithGauge = statusBarWithGauge;
	Notebook = notebook;
	ToolsNotebook = toolsNotebook;
	OutlineNotebook = outlineNotebook;
	AuiManager = auiManager;
	MenuBar = menuBar;
}

t4p::NotebookClass* t4p::FeatureClass::GetNotebook() const {
	return Notebook;
}

t4p::EnvironmentClass* t4p::FeatureClass::GetEnvironment() {
	return &App.Globals.Environment;
}

void t4p::FeatureClass::AddViewMenuItems(wxMenu* viewMenu) {
}

void t4p::FeatureClass::AddSearchMenuItems(wxMenu* searchMenu) {
}

void t4p::FeatureClass::AddFileMenuItems(wxMenu* fileMenu) {
}

void t4p::FeatureClass::AddEditMenuItems(wxMenu* editMenu) {
}

void t4p::FeatureClass::AddHelpMenuItems(wxMenu* editMenu) {
}

void t4p::FeatureClass::AddNewMenu(wxMenuBar* menuBar) {
}

void t4p::FeatureClass::AddCodeControlClassContextMenuItems(wxMenu* menu) {
}

void t4p::FeatureClass::AddKeyboardShortcuts(std::vector<t4p::DynamicCmdClass>& shortcuts) {

}

bool t4p::FeatureClass::AddToolsWindow(wxWindow* window, wxString tabName, wxString windowName, const wxBitmap& bitmap) {
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

bool t4p::FeatureClass::AddOutlineWindow(wxWindow* window, wxString name, const wxBitmap& bitmap) {
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

wxWindow* t4p::FeatureClass::FindToolsWindow(int windowId) const {
	return wxWindow::FindWindowById(windowId, GetToolsNotebook());
}

wxWindow* t4p::FeatureClass::FindOutlineWindow(int windowId) const {
	return wxWindow::FindWindowById(windowId, GetOutlineNotebook());
}

bool t4p::FeatureClass::IsToolsWindowSelected(int windowId) const {
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

bool t4p::FeatureClass::IsToolsWindowSelectedByName(const wxString& name) const {
	wxAuiPaneInfo info = AuiManager->GetPane(ToolsNotebook);
	if (!info.IsShown()) {
		
		// if the notebook itself is not shown, it means that the outline window
		// is hidden because its parent is hidden
		return false;
	}
	size_t selection = ToolsNotebook->GetSelection();
	if (selection >= 0 && selection < ToolsNotebook->GetPageCount()) {
		return ToolsNotebook->GetPage(selection)->GetName() == name;
	}
	return false;
}

bool t4p::FeatureClass::IsOutlineWindowSelected(int windowId) const {
	
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

t4p::CodeControlClass* t4p::FeatureClass::GetCurrentCodeControl() const {
	return Notebook->GetCurrentCodeControl();
}

wxAuiNotebook* t4p::FeatureClass::GetToolsNotebook() const {
	return ToolsNotebook;
}

wxAuiNotebook* t4p::FeatureClass::GetOutlineNotebook() const {
	return OutlineNotebook;
}

t4p::StatusBarWithGaugeClass* t4p::FeatureClass::GetStatusBarWithGauge() const {
	return StatusBarWithGauge;
}

void t4p::FeatureClass::SetFocusToToolsWindow(wxWindow* window) {
	int index = ToolsNotebook->GetPageIndex(window);
	if (index != wxNOT_FOUND) {
		ToolsNotebook->SetSelection(index);
		if (NULL != AuiManager) {
			AuiManager->GetPane(ToolsNotebook).Show();
			AuiManager->Update();
		}
	}
}

void t4p::FeatureClass::SetFocusToOutlineWindow(wxWindow* window) {
	int index = OutlineNotebook->GetPageIndex(window);
	if (index != wxNOT_FOUND) {
		OutlineNotebook->SetSelection(index);
		if (NULL != AuiManager) {
			AuiManager->GetPane(OutlineNotebook).Show();
			AuiManager->Update();
		}
	}
}

wxString t4p::FeatureClass::GetSelectedText() const {
	CodeControlClass* page = GetCurrentCodeControl();
	wxString selectedText;
	if (page) {
		selectedText = page->GetSelectedText();
	}
	return selectedText;
}

void t4p::FeatureClass::AddWindows() {
}

void t4p::FeatureClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
}

void t4p::FeatureClass::LoadPreferences(wxConfigBase* config) {
}

void t4p::FeatureClass::AddToolBarItems(wxAuiToolBar* toolBar) {
}

wxWindow* t4p::FeatureClass::GetMainWindow() const {
	return ToolsNotebook->GetParent();
}

t4p::CodeControlClass* t4p::FeatureClass::CreateCodeControl(const wxString& tabName, t4p::CodeControlClass::Mode mode) const {
	t4p::NotebookClass* notebook = GetNotebook();
	notebook->AddTriumphPage(mode);
	if (!tabName.IsEmpty()) {
		notebook->SetPageText(notebook->GetSelection(), tabName);
	}
	t4p::CodeControlClass* ctrl = notebook->GetCurrentCodeControl();
	return ctrl;
}

void t4p::FeatureClass::AddDynamicCmd(std::map<int, wxString> menuItemIds, std::vector<t4p::DynamicCmdClass>& shortcuts) {
	for (std::map<int, wxString>::iterator it = menuItemIds.begin(); it != menuItemIds.end(); ++it) {
		wxMenuItem* item = MenuBar->FindItem(it->first);
		wxASSERT_MSG(item, wxT("Menu item not found:") + it->second);
		t4p::DynamicCmdClass cmd(item, it->second);
		shortcuts.push_back(cmd);
	}
}