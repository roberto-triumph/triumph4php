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
#include <plugins/CodeIgniterPluginClass.h>
#include <MvcEditorErrors.h>
#include <MvcEditor.h>

/*
 * This is what the code igniter framework detection code calls itself.
 * see the 'getIdentifier' method of the PHP framework detection code.
 */
static const wxString FRAMEWORK_IDENTIFIER = wxT("code-igniter");

mvceditor::CodeIgniterPluginClass::CodeIgniterPluginClass()  
	: PluginClass() 
	, Process(*this)
	, ConfigFiles() {
	CodeIgniterMenu = NULL;
}

void mvceditor::CodeIgniterPluginClass::AddNewMenu(wxMenuBar *menuBar) {
	CodeIgniterMenu = new wxMenu;

	// don't insert it just yet ... we only want to add it to the menu bar
	// for the projects that use Code Igniter
}

void mvceditor::CodeIgniterPluginClass::OnProjectOpened() {
	mvceditor::ProjectClass* project = GetProject();
	if (project) {
		wxString cmd = project->DetectConfigFilesCommand(FRAMEWORK_IDENTIFIER);
		long pid;
		Process.Init(cmd, wxID_ANY, pid);
	}
}

void mvceditor::CodeIgniterPluginClass::OnProcessComplete(wxCommandEvent &event) {
	mvceditor::ProjectClass* project = GetProject();
	if (project) {
		wxString response = event.GetString();
		ConfigFiles.clear();
		mvceditor::ProjectClass::DetectError error = mvceditor::ProjectClass::NONE;
		if (!project->DetectConfigFilesResponse(response, error, ConfigFiles)) {
			mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, response);
		}
		else {
			UpdateMenu();
		}
	}
}

void mvceditor::CodeIgniterPluginClass::OnProcessFailed(wxCommandEvent& event) {
	wxString response = event.GetString();
	mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, response);
}

void mvceditor::CodeIgniterPluginClass::UpdateMenu() {
	wxMenuItemList list = CodeIgniterMenu->GetMenuItems();
	if (CodeIgniterMenu->GetMenuItemCount() == 0) {
		std::map<wxString, wxString>::const_iterator it = ConfigFiles.begin();
		for (size_t i = 0; it != ConfigFiles.end(); ++it) {
			wxString fullPath = it->second;
			wxString label = it->first;
			label.Replace(wxT("_"), wxT(" "));
			CodeIgniterMenu->Append(MENU_CODE_IGNITER + i, label, 
				_("Open ") + fullPath, wxITEM_NORMAL);
			i++;
		}
	}
	
}

void mvceditor::CodeIgniterPluginClass::OnMenuItem(wxCommandEvent& event) {
	int id = event.GetId();
	wxMenuItem* item = CodeIgniterMenu->FindItem(id);
	if (item) {
		wxString filePath = ConfigFiles[item->GetItemLabelText()];
		if (!filePath.IsEmpty()) {
			wxFileName fileName(filePath);
			if (fileName.IsOk()) {
				wxCommandEvent openEvent(mvceditor::EVENT_APP_OPEN_FILE);
				openEvent.SetString(filePath);
				AppEvent(openEvent);
			}
			else {
				mvceditor::EditorLogWarning(mvceditor::INVALID_FILE, filePath);
			}
		}
		else {
			mvceditor::EditorLogWarning(mvceditor::INVALID_FILE, item->GetItemLabelText());
		}
	}
	else {
		mvceditor::EditorLogWarning(mvceditor::INVALID_FILE, wxT("invalid event object"));
	}
}

void mvceditor::CodeIgniterPluginClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	
	// none for now since the code igniter menu is dynamic and is 
	// shown/hidden depending on the project
}

BEGIN_EVENT_TABLE(mvceditor::CodeIgniterPluginClass, wxEvtHandler) 
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_PROCESS_COMPLETE, mvceditor::CodeIgniterPluginClass::OnProcessComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_PROCESS_FAILED, mvceditor::CodeIgniterPluginClass::OnProcessFailed)
	EVT_MENU_RANGE(MENU_CODE_IGNITER, MENU_CODE_IGNITER + 10, mvceditor::CodeIgniterPluginClass::OnMenuItem)
END_EVENT_TABLE()