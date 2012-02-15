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
#include <plugins/RunBrowserPluginClass.h>
#include <MvcEditorErrors.h>
#include <wx/artprov.h>
#include <wx/valgen.h>

const int ID_TOOLBAR_BROWSER = wxNewId();

static void ExternalBrowser(const wxString& browserName, const wxString& url, mvceditor::EnvironmentClass* environment) {
	wxFileName webBrowserPath  = environment->WebBrowsers[browserName];
	if (!webBrowserPath.IsOk()) {
		mvceditor::EditorLogWarning(mvceditor::BAD_WEB_BROWSER_EXECUTABLE, webBrowserPath.GetFullPath());
		return;
	}
	wxString cmd = wxT("\"") + webBrowserPath.GetFullPath() + wxT("\""); 
	cmd += wxT(" \"");
	cmd += url;
	cmd += wxT("\"");
			
	// TODO track this PID ... ?
	// what about when user closes the external browser ?
	// make the browser its own process so that it stays alive if the editor program is exited
	// if we dont do this the browser thinks it crashed and will tell the user so
	long pid = wxExecute(cmd, wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER);
	if (pid <= 0) {
		mvceditor::EditorLogWarning(mvceditor::BAD_WEB_BROWSER_EXECUTABLE, cmd);
	}
}

mvceditor::UrlChoiceClass::UrlChoiceClass(const std::vector<wxString>& urls, const wxString& fileName, mvceditor::EnvironmentClass* environment)
	: UrlList()
	, Extra() 
	, ChosenIndex(0) {
	wxString url = environment->Apache.GetUrl(fileName);
		
	// we only want the virtual host name, as the url we get from the framework's routing url scheme
	wxString host = url.Mid(7); // 7 = length of "http://"
	host = host.Mid(0, host.Find(wxT("/")));
	host = wxT("http://") + host + wxT("/");
	for (size_t i = 0; i < urls.size(); ++i) {
		wxString item = host + urls[i];
		UrlList.Add(item);
	}
}

wxString mvceditor::UrlChoiceClass::ChosenUrl() const {
	wxString chosenUrl;
	if ((size_t)ChosenIndex < UrlList.size()) {
		chosenUrl = UrlList[ChosenIndex] + Extra;
	}
	return chosenUrl;
}

mvceditor::ChooseUrlDialogClass::ChooseUrlDialogClass(wxWindow* parent, mvceditor::UrlChoiceClass& urlChoice)
	: ChooseUrlDialogGeneratedClass(parent, wxID_ANY)
	, UrlChoice(urlChoice) {
	UrlList->Append(UrlChoice.UrlList);
	UrlList->SetSelection(UrlChoice.ChosenIndex);
	
	wxGenericValidator extraValidator(&UrlChoice.Extra);
	Extra->SetValidator(extraValidator);
	TransferDataToWindow();
	
	if (!urlChoice.UrlList.IsEmpty()) {
		wxString label= _("Complete URL: ") + urlChoice.UrlList[0];
		CompleteLabel->SetLabel(label);
	}
}

void mvceditor::ChooseUrlDialogClass::OnOkButton(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow()) {
		UrlChoice.ChosenIndex = UrlList->GetSelection();
		EndModal(wxOK);
	}
}

void mvceditor::ChooseUrlDialogClass::OnUpdateUi(wxUpdateUIEvent& event) {
	wxString url = UrlList->GetStringSelection() + Extra->GetValue();
	wxString label= _("Complete URL: ") + url;
	CompleteLabel->SetLabel(label);
	event.Skip();
}

mvceditor::RunBrowserPluginClass::RunBrowserPluginClass()
	: PluginClass() 
	, RunInBrowser(NULL)
	, BrowserComboBox(NULL) 
	, PhpFrameworks(NULL) {
		
}

mvceditor::RunBrowserPluginClass::~RunBrowserPluginClass() {
	if (PhpFrameworks) {
		delete PhpFrameworks;
	}
}

void mvceditor::RunBrowserPluginClass::AddToolsMenuItems(wxMenu* toolsMenu) {
	RunInBrowser = new wxMenuItem(toolsMenu, mvceditor::MENU_RUN_BROWSER + 0, 
		_("Run In Web Browser"),
		_("Run the script in the chosen Web Browser"), wxITEM_NORMAL);
	toolsMenu->Append(RunInBrowser);
}

void mvceditor::RunBrowserPluginClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_RUN_BROWSER + 0] = wxT("Run-In Web Browser");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::RunBrowserPluginClass::LoadPreferences(wxConfigBase* config) {
	
	// dont use the config; use the Environment that has already been seeded with 
	// the proper data
	// TODO: need to update the combo box if the user updates the environment
	mvceditor::EnvironmentClass* environment = GetEnvironment();
	std::map<wxString, wxFileName> webBrowsers = environment->WebBrowsers;
	for (std::map<wxString, wxFileName>::const_iterator it = webBrowsers.begin(); it != webBrowsers.end(); ++it) {
		BrowserComboBox->AppendString(it->first);
	}
	if (!webBrowsers.empty()) {
		BrowserComboBox->Select(0);
	}
}


void mvceditor::RunBrowserPluginClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	wxBitmap bitmap = wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE, wxART_TOOLBAR, wxSize(16, 16));
	toolBar->AddTool(ID_TOOLBAR_BROWSER, _("Web Browser"), bitmap, _("Run On a Web Browser"));
	
	// to be filled in after the config is read
	wxArrayString choices;
	BrowserComboBox = new wxComboBox(toolBar, wxID_NEW, wxT(""), wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	toolBar->AddControl(BrowserComboBox);
}

void mvceditor::RunBrowserPluginClass::OnRunInWebBrowser(wxCommandEvent& event) {
	mvceditor::CodeControlClass* currentCodeControl = GetCurrentCodeControl();
	if (!currentCodeControl) {
		return;
	}
	wxString fileName = currentCodeControl->GetFileName();
	mvceditor::EnvironmentClass* environment = GetEnvironment();
	if (PhPFrameworks().Identifiers.empty()) {
		wxString browserName = BrowserComboBox->GetValue();
		if (!wxFileName::FileExists(fileName)) {
			mvceditor::EditorLogWarning(mvceditor::INVALID_FILE, fileName);
			return;
		}

		// turn file name into a url in the default manner (by calculating from the vhost document root)
		wxString url = environment->Apache.GetUrl(fileName);
		if (url.IsEmpty()) {
			mvceditor::EditorLogWarning(mvceditor::INVALID_FILE, _("File is not under web root"));	
			return;
		}	
		ExternalBrowser(browserName, url, environment);
	}
	else if (!PhpFrameworks) {
		PhpFrameworks = new PhpFrameworkDetectorClass(*this, *environment);
		PhpFrameworks->Identifiers = PhPFrameworks().Identifiers;
		PhpFrameworks->InitUrlDetector(GetProject()->GetRootPath(), fileName);
	}
	else {
		PhpFrameworks->InitUrlDetector(GetProject()->GetRootPath(), fileName);
	}
}


void mvceditor::RunBrowserPluginClass::OnUrlDetectionComplete(mvceditor::UrlDetectedEventClass& event) {
	mvceditor::CodeControlClass* currentCodeControl = GetCurrentCodeControl();
	if (!currentCodeControl) {
		return;
	}
	wxString fileName = currentCodeControl->GetFileName();
	wxString browserName = BrowserComboBox->GetValue();
	mvceditor::EnvironmentClass* environment = GetEnvironment();
	if (!event.Urls.empty()) {
		mvceditor::UrlChoiceClass urlChoice(event.Urls, fileName, environment);
		mvceditor::ChooseUrlDialogClass dialog(GetMainWindow(), urlChoice);
		if (wxOK == dialog.ShowModal()) {
			wxString chosenUrl = urlChoice.ChosenUrl();
			ExternalBrowser(browserName, chosenUrl, environment);
			
			// TODO: save this choice into a toolbar dropdown so that the user can easily access it
			// multiple times.
		}
	}
	else {
		
		// no URLs means that the file can be accessed normally
		// turn file name into a url in the default manner (by calculating from the vhost document root)
		wxString url = environment->Apache.GetUrl(fileName);
		if (url.IsEmpty()) {
			mvceditor::EditorLogWarning(mvceditor::INVALID_FILE, _("File is not under web root"));	
			return;
		}	
		ExternalBrowser(browserName, url, environment);
	}
}

void mvceditor::RunBrowserPluginClass::OnUrlDetectionFailed(wxCommandEvent& event) {
	mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, event.GetString());
}

BEGIN_EVENT_TABLE(mvceditor::RunBrowserPluginClass, wxEvtHandler) 
	EVT_MENU(mvceditor::MENU_RUN_BROWSER + 0, mvceditor::RunBrowserPluginClass::OnRunInWebBrowser)
	EVT_TOOL(ID_TOOLBAR_BROWSER, mvceditor::RunBrowserPluginClass::OnRunInWebBrowser)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_FRAMEWORK_URL_FAILED, mvceditor::RunBrowserPluginClass::OnUrlDetectionFailed)
	EVT_FRAMEWORK_URL_COMPLETE(mvceditor::RunBrowserPluginClass::OnUrlDetectionComplete)
END_EVENT_TABLE()