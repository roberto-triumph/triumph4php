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
#include <plugins/EnvironmentPluginClass.h>
#include <widgets/NonEmptyTextValidatorClass.h>
#include <wx/filename.h>
#include <wx/string.h>


BEGIN_EVENT_TABLE(mvceditor::EnvironmentDialogClass, EnvironmentGeneratedDialogClass)
	EVT_IDLE(mvceditor::EnvironmentDialogClass::OnIdle)
	EVT_FILEPICKER_CHANGED(ID_PHP_FILE, mvceditor::EnvironmentDialogClass::OnPhpFileChanged)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::EnvironmentPluginClass, wxEvtHandler) 
	EVT_MENU(mvceditor::MENU_ENVIRONMENT, mvceditor::EnvironmentPluginClass::OnMenuEnvironment)
END_EVENT_TABLE()

mvceditor::EnvironmentDialogClass::EnvironmentDialogClass(wxWindow* parent, EnvironmentClass& environment)
	: EnvironmentGeneratedDialogClass(parent)
	, Environment(environment)
	, DirectorySearch()
	, State(FREE) {
	NonEmptyTextValidatorClass phpExecutableValidator(&environment.Php.PhpExecutablePath, PhpLabel);
	PhpExecutable->SetValidator(phpExecutableValidator);
	Populate();
}

void mvceditor::EnvironmentDialogClass::OnScanButton(wxCommandEvent& event) {
	if (FREE == State) {
		Gauge->Show();
		wxString path = ApacheConfigurationDirectory->GetPath();
		wxChar ch = wxFileName::GetPathSeparator();
		if (!path.EndsWith(&ch)) {
			path.Append(ch);
		}
		if (DirectorySearch.Init(path)) {
			State = SEARCHING;
			wxWakeUpIdle();
		}
		else {
			wxMessageBox(_("Path not valid"), _("Configuration Not Found"), wxOK | wxCENTRE, this);
		}
		ScanButton->SetLabel(_("Stop Scan"));
	}
	else {
		
		// act like a stop button
		State = FREE;
		Gauge->SetValue(0);
		ScanButton->SetLabel(_("Scan For Configuration"));
	}
}

void mvceditor::EnvironmentDialogClass::OnOkButton(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow()) {
		EndModal(wxOK);
	}
}

void mvceditor::EnvironmentDialogClass::Populate() {
	wxString configFile = Environment.Apache.GetHttpdPath();
	wxString results;

	// configFile is a full file name, extract the path
	wxFileName fileName(configFile);
	ApacheConfigurationDirectory->SetPath(fileName.GetPath(true));
	if (!Environment.Apache.GetDocumentRoot().IsEmpty()) {
		results += wxT("Document Root:") + Environment.Apache.GetDocumentRoot() + wxT("\n");
	}
	if (Environment.Apache.GetListenPort() > 0) {
		results += wxString::Format(wxT("Listen Port: %d\n"), Environment.Apache.GetListenPort());
	}
	std::map<wxString, wxString> virtualHosts = Environment.Apache.GetVirtualHostMappings();
	results += wxT("-----------------------------------------------------\n");
	if (0 >= virtualHosts.size()) {
		results += wxT("No virtual hosts. Will use localhost.");
	}
	else {
		for (std::map<wxString, wxString>::iterator it = virtualHosts.begin(); it != virtualHosts.end(); ++it) {
			results += wxT("Server Name: ") + it->second +
			wxT("\nDocument Root: ") + it->first +
			wxT("\n-----------------------------------------------------\n");
		}
	}
	VirtualHostResults->SetValue(results);
}

void mvceditor::EnvironmentDialogClass::OnIdle(wxIdleEvent& event) {
	switch (State) {
		case FREE:
			break;
		case SEARCHING:
			for (int i = 0; i < 100; ++i) {
				if (DirectorySearch.More()) {
					DirectorySearch.Walk(Environment.Apache);
				}
				if (!DirectorySearch.More()) {
					State = OPENING_FILE;
					break;
				}
			}
			event.RequestMore();
			Gauge->Pulse();
			break;
		case OPENING_FILE:
			std::vector<wxString> matchedFiles = DirectorySearch.GetMatchedFiles();
			bool found = false;
			for (size_t i = 0; i < matchedFiles.size(); ++i) {
				if (Environment.Apache.SetHttpdPath(matchedFiles[i])) {
					found = true;
					break;
				}
			}
			State = FREE;
			Gauge->SetValue(0);
			ScanButton->SetLabel(_("Scan For Configuration"));
			Populate();
			break;
	}
}

void mvceditor::EnvironmentDialogClass::OnPhpFileChanged(wxFileDirPickerEvent& event) {
	PhpExecutable->SetValue(event.GetPath());
}

mvceditor::EnvironmentPluginClass::EnvironmentPluginClass()
	: PluginClass() {
}

void mvceditor::EnvironmentPluginClass::AddProjectMenuItems(wxMenu* projectMenu) {
	projectMenu->Append(mvceditor::MENU_ENVIRONMENT, _("Environment"));
}

void mvceditor::EnvironmentPluginClass::OnMenuEnvironment(wxCommandEvent& event) {
	EnvironmentClass* environment = GetEnvironment();
	EnvironmentDialogClass dialog(GetMainWindow(), *environment);
	if (wxOK == dialog.ShowModal()) {
		environment->SaveToConfig();
	}
}

void mvceditor::EnvironmentPluginClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_ENVIRONMENT] = wxT("Environment-Configure Environment");
	AddDynamicCmd(menuItemIds, shortcuts);
}