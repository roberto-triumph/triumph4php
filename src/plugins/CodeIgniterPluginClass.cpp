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
#include <windows/StringHelperClass.h>

/*
 * This is what the code igniter framework detection code calls itself.
 * see the 'getIdentifier' method of the PHP framework detection code.
 */
static const wxString FRAMEWORK_IDENTIFIER = wxT("code-igniter");
static const int ID_CONFIG_DETECTION = wxNewId();
static const int ID_RESOURCE_DETECTION = wxNewId();
static const int ID_CODE_IGNITER_GAUGE = wxNewId();

mvceditor::CodeIgniterPluginClass::CodeIgniterPluginClass()  
	: PluginClass() 
	, ResourcesDetector(*this)
	, ConfigFilesDetector(*this)
	, ConfigFiles() {
	CodeIgniterMenu = NULL;
	MenuBar = NULL;
}

void mvceditor::CodeIgniterPluginClass::AddNewMenu(wxMenuBar *menuBar) {
	CodeIgniterMenu = new wxMenu;
	MenuBar = menuBar;

	// don't insert it just yet ... we only want to add it to the menu bar
	// for the projects that use Code Igniter
}

void mvceditor::CodeIgniterPluginClass::OnProjectOpened() {
	int menuIndex = MenuBar->FindMenu(_("Code Igniter"));
	if (MenuBar && menuIndex != wxNOT_FOUND) {
		MenuBar->Remove(menuIndex);
		while (CodeIgniterMenu->GetMenuItemCount() > 0) {
			CodeIgniterMenu->Delete(CodeIgniterMenu->FindItemByPosition(0)->GetId());
		}
	}
	mvceditor::ProjectClass* project = GetProject();
	mvceditor::EnvironmentClass* environment = GetEnvironment();

	// on startup the editor is in "non-project" mode (no root path)
	if (project && !project->GetRootPath().IsEmpty()) {
		if (!ConfigFilesDetector.Init(ID_CONFIG_DETECTION, *environment, project->GetRootPath(), FRAMEWORK_IDENTIFIER)) {
			mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, wxT("Config File Detection process could not be started"));
		}
		else {
			GetStatusBarWithGauge()->StopGauge(ID_CODE_IGNITER_GAUGE);
			GetStatusBarWithGauge()->AddGauge(
				_("Code Igniter Resource Detection"), ID_CODE_IGNITER_GAUGE, 
				mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE, 0);
		}
	}
}

void mvceditor::CodeIgniterPluginClass::OnProcessComplete(wxCommandEvent &event) {
	GetStatusBarWithGauge()->StopGauge(ID_CODE_IGNITER_GAUGE);
	mvceditor::ProjectClass* project = GetProject();
	if (project && event.GetId() == ID_CONFIG_DETECTION) {
		wxString response = event.GetString();
		ConfigFiles.clear();
		if (mvceditor::DetectorClass::NONE != ConfigFilesDetector.Error) {
			mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, response);
		}
		else {
			ConfigFiles = ConfigFilesDetector.ConfigFiles;
			UpdateMenu();
		}
		mvceditor::EnvironmentClass* environment = GetEnvironment();
		if (!ResourcesDetector.Init(ID_RESOURCE_DETECTION, *environment, project->GetRootPath(), FRAMEWORK_IDENTIFIER)) {
			mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, wxT("Resource Detection process could not be started"));
		}
		else {
			GetStatusBarWithGauge()->AddGauge(
				_("Code Igniter Resource Detection"), ID_CODE_IGNITER_GAUGE, 
				mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE, 0);
		}
	}
	else if (project && event.GetId() == ID_RESOURCE_DETECTION) {
		if (mvceditor::DetectorClass::NONE != ResourcesDetector.Error) {
			mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, wxT(""));
		}
		else {
			mvceditor::ResourceFinderClass* finder = project->GetResourceFinder();
			finder->AddDynamicResources(ResourcesDetector.Resources);
		}
	}
}

void mvceditor::CodeIgniterPluginClass::OnProcessFailed(wxCommandEvent& event) {
	wxString response = event.GetString();
	mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, response);
	event.Skip();
}

void mvceditor::CodeIgniterPluginClass::UpdateMenu() {
	wxMenuItemList list = CodeIgniterMenu->GetMenuItems();
	if (CodeIgniterMenu->GetMenuItemCount() == 0 && !ConfigFiles.empty()) {
		std::map<wxString, wxString>::const_iterator it = ConfigFiles.begin();
		for (size_t i = 0; it != ConfigFiles.end(); ++it) {
			wxString fullPath = it->second;
			wxString label = it->first;

			// make label a bit friendlier for humans
			label.Replace(wxT("_"), wxT(" "));
			CodeIgniterMenu->Append(MENU_CODE_IGNITER + i, label, 
				_("Open ") + fullPath, wxITEM_NORMAL);
			i++;
		}
		CodeIgniterMenu->AppendSeparator();
		CodeIgniterMenu->Append(MENU_CODE_IGNITER + ConfigFiles.size() + 0, 
			_("New Code Igniter Model"), 
			_("Create a new PHP File That Will Contain a Code Igniter model"));
		CodeIgniterMenu->Append(MENU_CODE_IGNITER + ConfigFiles.size() + 1, 
			_("New Code Igniter Controller"), 
			_("Create a new PHP File That Will Contain a Code Igniter controller"));
		MenuBar->Insert(MenuBar->GetMenuCount() - 1, CodeIgniterMenu, _("Code Igniter"));
	}
}

void mvceditor::CodeIgniterPluginClass::OnMenuItem(wxCommandEvent& event) {
	int id = event.GetId();
	wxMenuItem* item = CodeIgniterMenu->FindItem(id);
	if (item) {
		if (id < (int)(MENU_CODE_IGNITER + ConfigFiles.size())) {
			wxString menuLabel = item->GetItemLabelText();

			// label was made friendlier for humans; undo it
			menuLabel.Replace(wxT(" "), wxT("_"));
			wxString filePath = ConfigFiles[menuLabel];
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
		else if (id == (int)(MENU_CODE_IGNITER + ConfigFiles.size() + 0)) {
			mvceditor::CodeControlClass* codeControl = CreateCodeControl(wxT(""));
			codeControl->SetDocumentMode(mvceditor::CodeControlClass::PHP);
			UnicodeString contents = mvceditor::StringHelperClass::charToIcu(
				"<?php\n"
				"\n"
				"class Model_name extends CI_Model {\n"
				"\n"
				"\tfunction __construct() {\n"
				"\t\tparent::__construct();\n"
				"\t}\n"
				"}\n"
			);
			codeControl->SetUnicodeText(contents);

			// select the "Model_name" so that user can easily change it first
			int32_t index = contents.indexOf(UNICODE_STRING_SIMPLE("Model_name"));
			codeControl->SetSelectionByCharacterPosition(index, index + 10); // 10 => length of Model_name
		}
		else if (id == (int)(MENU_CODE_IGNITER + ConfigFiles.size() + 1)) {
			mvceditor::CodeControlClass* codeControl = CreateCodeControl(wxT(""));
			codeControl->SetDocumentMode(mvceditor::CodeControlClass::PHP);
			UnicodeString contents = mvceditor::StringHelperClass::charToIcu(
				"<?php\n"
				"\n"
				"class Controller extends CI_Controller {\n"
				"\n"
				"\tfunction __construct() {\n"
				"\t\tparent::__construct();\n"
				"\t}\n"
				"}\n"
			);
			codeControl->SetUnicodeText(contents);

			// select the "Controller" so that user can easily change it first
			int32_t index = contents.indexOf(UNICODE_STRING_SIMPLE("Controller"));
			codeControl->SetSelectionByCharacterPosition(index, index + 10); // 10 => length of Controller
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
	EVT_COMMAND(ID_CONFIG_DETECTION, mvceditor::EVENT_PROCESS_COMPLETE, mvceditor::CodeIgniterPluginClass::OnProcessComplete)
	EVT_COMMAND(ID_CONFIG_DETECTION, mvceditor::EVENT_PROCESS_FAILED, mvceditor::CodeIgniterPluginClass::OnProcessFailed)
	EVT_COMMAND(ID_RESOURCE_DETECTION, mvceditor::EVENT_PROCESS_COMPLETE, mvceditor::CodeIgniterPluginClass::OnProcessComplete)
	EVT_COMMAND(ID_RESOURCE_DETECTION , mvceditor::EVENT_PROCESS_FAILED, mvceditor::CodeIgniterPluginClass::OnProcessFailed)
	EVT_MENU_RANGE(MENU_CODE_IGNITER, MENU_CODE_IGNITER + 13, mvceditor::CodeIgniterPluginClass::OnMenuItem)
END_EVENT_TABLE()