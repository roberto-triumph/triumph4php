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
#include <PluginClass.h>
#include <MvcEditor.h>
#include <wx/artprov.h>

mvceditor::PluginClass::PluginClass()
	: AuiManager(NULL)
	, StatusBarWithGauge(NULL)
	, Notebook(NULL)
	, ToolsNotebook(NULL)
	, App(NULL)
	, Project(NULL)
	, MenuBar(NULL) {
}

mvceditor::PluginClass::~PluginClass() {
}

void mvceditor::PluginClass::InitWindow(StatusBarWithGaugeClass* statusBarWithGauge, NotebookClass* notebook, wxAuiNotebook* toolsNotebook, 
	wxAuiManager* auiManager, wxMenuBar* menuBar) {
	StatusBarWithGauge = statusBarWithGauge;
	Notebook = notebook;
	ToolsNotebook = toolsNotebook;
	AuiManager = auiManager;
	MenuBar = menuBar;
}

void mvceditor::PluginClass::InitState(AppClass* app) {
	App = app;
}

void mvceditor::PluginClass::SetProject(ProjectClass* project) {
	Project = project;
	if (Project) {
		OnProjectOpened();
	}
}

mvceditor::ProjectClass* mvceditor::PluginClass::GetProject() const {
	return Project;
}

mvceditor::NotebookClass* mvceditor::PluginClass::GetNotebook() const {
	return Notebook;
}

mvceditor::EnvironmentClass* mvceditor::PluginClass::GetEnvironment() {
	return &App->Environment;
}

mvceditor::ResourceCacheClass* mvceditor::PluginClass::GetResourceCache() {
	return &App->ResourceCache;
}

mvceditor::PhpFrameworkDetectorClass& mvceditor::PluginClass::PhPFrameworks() const {
	return App->PhpFrameworks;
}

void mvceditor::PluginClass::AddToolsMenuItems(wxMenu* toolsMenu) {
}

void mvceditor::PluginClass::AddEditMenuItems(wxMenu* editMenu) {
}

void mvceditor::PluginClass::AddProjectMenuItems(wxMenu* projectMenu) {
}

void mvceditor::PluginClass::AddNewMenu(wxMenuBar*) {
}

void mvceditor::PluginClass::AddCodeControlClassContextMenuItems(wxMenu* menu) {
}

void mvceditor::PluginClass::OnProjectOpened() {
	
}

void mvceditor::PluginClass::AddKeyboardShortcuts(std::vector<mvceditor::DynamicCmdClass>& shortcuts) {

}

bool mvceditor::PluginClass::AddToolsWindow(wxWindow* window, wxString name) {
	if (ToolsNotebook->AddPage(window, name)) {
		int index = ToolsNotebook->GetPageIndex(window);
		if (index != wxNOT_FOUND) {
			ToolsNotebook->SetSelection(index);
		}
		if (NULL != AuiManager) {
			AuiManager->GetPane(ToolsNotebook).Show();
			AuiManager->Update();
		}
		return true;
	}
	return false;
}

wxWindow* mvceditor::PluginClass::FindToolsWindow(int windowId) const {
	return wxWindow::FindWindowById(windowId, GetToolsNotebook());
}

bool mvceditor::PluginClass::IsToolsWindowSelected(int windowId) const {
	wxWindow* window = wxWindow::FindWindowById(windowId, GetToolsNotebook());
	int windowIndex = ToolsNotebook->GetPageIndex(window); 
	return windowIndex != wxNOT_FOUND && windowIndex == ToolsNotebook->GetSelection();
}

bool mvceditor::PluginClass::AddContentWindow(wxWindow* window, const wxString& name) {
	return Notebook->AddPage(window, name, true, 
		wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_TOOLBAR, 
		wxSize(16, 16)));
}

mvceditor::CodeControlClass* mvceditor::PluginClass::GetCurrentCodeControl() const {
	return Notebook->GetCurrentCodeControl();
}

wxAuiNotebook* mvceditor::PluginClass::GetToolsNotebook() const {
	return ToolsNotebook;
}

mvceditor::StatusBarWithGaugeClass* mvceditor::PluginClass::GetStatusBarWithGauge() const {
	return StatusBarWithGauge;
}

void mvceditor::PluginClass::SetFocusToToolsWindow(wxWindow* window) {
	int index = ToolsNotebook->GetPageIndex(window);
	if (index != wxNOT_FOUND) {
		ToolsNotebook->SetSelection(index);
		if (NULL != AuiManager) {
			AuiManager->GetPane(ToolsNotebook).Show();
			AuiManager->Update();
		}
	}
}

wxString mvceditor::PluginClass::GetSelectedText() const {
	CodeControlClass* page = GetCurrentCodeControl();
	wxString selectedText;
	if (page) {
		selectedText = page->GetSelectedText();
	}
	return selectedText;
}

void mvceditor::PluginClass::AddWindows() {
}

void mvceditor::PluginClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
}

void mvceditor::PluginClass::LoadPreferences(wxConfigBase* config) {
}

void mvceditor::PluginClass::SavePreferences(wxConfigBase* config) {
}

void mvceditor::PluginClass::AddToolBarItems(wxAuiToolBar* toolBar) {
}

wxWindow* mvceditor::PluginClass::GetMainWindow() const {
	return ToolsNotebook->GetParent();
}

mvceditor::CodeControlClass* mvceditor::PluginClass::CreateCodeControl(const wxString& tabName) const {
	mvceditor::NotebookClass* notebook = GetNotebook();
	notebook->AddMvcEditorPage();
	if (!tabName.IsEmpty()) {
		notebook->SetPageText(notebook->GetSelection(), tabName);
	}
	mvceditor::CodeControlClass* ctrl = notebook->GetCurrentCodeControl();
	return ctrl;
}

void mvceditor::PluginClass::AppEvent(wxCommandEvent event) {
	if (App) {
		wxPostEvent(App, event);	
	}
}

void mvceditor::PluginClass::AddDynamicCmd(std::map<int, wxString> menuItemIds, std::vector<mvceditor::DynamicCmdClass>& shortcuts) {
	for (std::map<int, wxString>::iterator it = menuItemIds.begin(); it != menuItemIds.end(); ++it) {
		wxMenuItem* item = MenuBar->FindItem(it->first);
		wxASSERT_MSG(item, wxT("Menu item not found:") + it->second);
		mvceditor::DynamicCmdClass cmd(item, it->second);
		shortcuts.push_back(cmd);
	}
}

mvceditor::FileSavedEventClass::FileSavedEventClass(mvceditor::CodeControlClass* codeControl)
	: wxEvent(wxID_ANY, mvceditor::EVENT_PLUGIN_FILE_SAVED)
	, CodeControl(codeControl) {

}

mvceditor::CodeControlClass* mvceditor::FileSavedEventClass::GetCodeControl() const {
	return CodeControl;
}

wxEvent* mvceditor::FileSavedEventClass::Clone() const {
	mvceditor::FileSavedEventClass* newEvt = new mvceditor::FileSavedEventClass(CodeControl);
	return newEvt;
}

const wxEventType mvceditor::EVENT_PLUGIN_FILE_SAVED = wxNewEventType();