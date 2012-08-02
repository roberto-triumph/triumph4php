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
#include <Events.h>
#include <windows/StringHelperClass.h>
#include <wx/choicdlg.h>

mvceditor::CodeIgniterPluginClass::CodeIgniterPluginClass(mvceditor::AppClass& app)  
	: PluginClass(app)
	, ConfigFiles() 
	, CodeIgniterMenu(NULL) {
	MenuBar = NULL;
}

void mvceditor::CodeIgniterPluginClass::AddNewMenu(wxMenuBar *menuBar) {
	MenuBar = menuBar;

	// don't insert it just yet ... we only want to add it to the menu bar
	// for the projects that use Code Igniter
}

void mvceditor::CodeIgniterPluginClass::OnProjectsUpdated(wxCommandEvent& event) {
	ConfigFiles.clear();
	for (size_t i = 0; i < App.Structs.Frameworks.size(); ++i) {
		ConfigFiles.insert(App.Structs.Frameworks[i].ConfigFiles.begin(), App.Structs.Frameworks[i].ConfigFiles.end());
	}
	if (!ConfigFiles.empty()) {

		// remove the files from any previously opened project
		if (CodeIgniterMenu) {
			while (CodeIgniterMenu->GetMenuItemCount() > 0) {
				CodeIgniterMenu->Delete(CodeIgniterMenu->FindItemByPosition(0)->GetId());
			}
		}
		else {

			// on the first project, menu wont exist
			CodeIgniterMenu = new wxMenu;
		}
		UpdateMenu();
	}
	else {
		
		// the new project is not a code igniter project, wont need the menu
		int index = MenuBar->FindMenu(_("Code Igniter"));
		if (index != wxNOT_FOUND) {
			MenuBar->Remove(index);
			delete CodeIgniterMenu;
			CodeIgniterMenu = NULL;
		}
	}
	mvceditor::ResourceCacheClass* resourceCache = GetResourceCache();
	for (size_t i = 0; i < App.Structs.Frameworks.size(); ++i) {
		resourceCache->GlobalAddDynamicResources(App.Structs.Frameworks[i].Resources);
	}
}

void mvceditor::CodeIgniterPluginClass::UpdateMenu() {
	wxMenuItemList list = CodeIgniterMenu->GetMenuItems();
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
	CodeIgniterMenu->Append(MENU_CODE_IGNITER + ConfigFiles.size() + 2, 
		_("Navigate To View"), 
		_("Go to the current controller's view."));
	CodeIgniterMenu->Append(MENU_CODE_IGNITER + ConfigFiles.size() + 3, 
		_("Navigate To Controller"), 
		_("Go to the current view's controller."));
	if (MenuBar->FindMenu(_("Code Igniter")) == wxNOT_FOUND) {
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
					wxCommandEvent openEvent(mvceditor::EVENT_CMD_FILE_OPEN);
					openEvent.SetString(filePath);
					App.EventSink.Publish(openEvent);
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
		else if (id == (int)(MENU_CODE_IGNITER + ConfigFiles.size() + 2)) {
			GoToView();
		}
		else if (id == (int)(MENU_CODE_IGNITER + ConfigFiles.size() + 3)) {
			GoToController();
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

void mvceditor::CodeIgniterPluginClass::GoToView() {
	mvceditor::CodeControlClass* codeCtrl = GetCurrentCodeControl();
	if (codeCtrl) {
		std::vector<mvceditor::ViewInfoClass> viewInfos = App.Structs.CurrentViewInfos;

		// go through the chosen url, and get the templates for that controller
		wxArrayString controllerViews;
		for (size_t i = 0; i < viewInfos.size(); ++i) {
			mvceditor::ViewInfoClass viewInfo = viewInfos[i];
			wxFileName viewFileName(viewInfo.FileName);
			if (viewFileName.IsOk()) {
				controllerViews.Add(viewInfo.FileName);
			}			
		}
		if (controllerViews.size() == 1) {
			GetNotebook()->LoadPage(controllerViews[0]);
		}
		else {
			wxMultiChoiceDialog dialog(GetMainWindow(), _("Select View(s) to open"),
				_("Go to a view"), controllerViews);
			dialog.SetSize(600, 200);
			if (dialog.ShowModal() == wxID_OK) {
				wxArrayInt selections = dialog.GetSelections();
				std::vector<wxString> chosen;
				for (size_t i = 0; i < selections.size(); ++i) {
					chosen.push_back(controllerViews[selections[i]]);
				}
				if (!chosen.empty()) {
					GetNotebook()->LoadPages(chosen);
				}
			}
		}
	}
}

void mvceditor::CodeIgniterPluginClass::GoToController() {
	mvceditor::CodeControlClass* codeCtrl = GetCurrentCodeControl();
	if (codeCtrl) {
		wxFileName currentFileName(codeCtrl->GetFileName());
		mvceditor::UrlResourceClass urlResource = App.Structs.CurrentUrl;
		if (urlResource.FileName.IsOk()) {
			GetNotebook()->LoadPage(urlResource.FileName.GetFullPath());
		}
	}
}

BEGIN_EVENT_TABLE(mvceditor::CodeIgniterPluginClass, wxEvtHandler) 
	EVT_MENU_RANGE(MENU_CODE_IGNITER, MENU_CODE_IGNITER + 15, mvceditor::CodeIgniterPluginClass::OnMenuItem)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_PROJECTS_UPDATED, mvceditor::CodeIgniterPluginClass::OnProjectsUpdated)
END_EVENT_TABLE()