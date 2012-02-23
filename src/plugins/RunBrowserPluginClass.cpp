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
#include <MvcEditor.h>
#include <wx/artprov.h>
#include <wx/valgen.h>
#include <wx/aui/aui.h>
#include <algorithm>

static const int ID_TOOLBAR_BROWSER = wxNewId();
static const int ID_BROWSER_AUI_TOOLBAR = wxNewId();
static const int ID_URL_AUI_TOOLBAR = wxNewId();
static const int ID_URL_SEARCH_AUI_TOOLBAR = wxNewId();

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


mvceditor::UrlChoiceClass::UrlChoiceClass(const std::vector<mvceditor::UrlResourceClass>& urls, const wxString& fileName, mvceditor::EnvironmentClass* environment)
	: UrlList(urls)
	, Extra() 
	, ChosenIndex(0) {
}

mvceditor::UrlResourceClass mvceditor::UrlChoiceClass::ChosenUrl() const {
	mvceditor::UrlResourceClass chosenUrl;
	if ((size_t)ChosenIndex < UrlList.size()) {
		chosenUrl = UrlList[ChosenIndex];
		chosenUrl.Url += Extra;
	}
	return chosenUrl;
}

mvceditor::ChooseUrlDialogClass::ChooseUrlDialogClass(wxWindow* parent, mvceditor::UrlChoiceClass& urlChoice)
	: ChooseUrlDialogGeneratedClass(parent, wxID_ANY)
	, UrlChoice(urlChoice) {
	for (size_t i = 0; i < UrlChoice.UrlList.size(); ++i) {
		UrlList->Append(UrlChoice.UrlList[i].Url);
	}
	UrlList->SetSelection(UrlChoice.ChosenIndex);
	
	wxGenericValidator extraValidator(&UrlChoice.Extra);
	Extra->SetValidator(extraValidator);
	TransferDataToWindow();
	
	if (!urlChoice.UrlList.empty()) {
		wxString label= _("Complete URL: ") + urlChoice.UrlList[0].Url;
		CompleteLabel->SetLabel(label);
	}
}

void mvceditor::ChooseUrlDialogClass::OnOkButton(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow()) {
		UrlChoice.ChosenIndex = UrlList->GetSelection();
		EndModal(wxOK);
	}
}

void mvceditor::ChooseUrlDialogClass::OnListItemSelected(wxCommandEvent& event) {
	wxString url = UrlList->GetStringSelection() + Extra->GetValue();
	wxString label= _("Complete URL: ") + url;
	CompleteLabel->SetLabel(label);
	event.Skip();
}

void mvceditor::ChooseUrlDialogClass::OnText(wxCommandEvent& event) {
	wxString url = UrlList->GetStringSelection() + Extra->GetValue();
	wxString label= _("Complete URL: ") + url;
	CompleteLabel->SetLabel(label);
	event.Skip();
}

mvceditor::RunBrowserPluginClass::RunBrowserPluginClass()
	: PluginClass() 
	, PhpFrameworks(NULL)
	, ResourceCacheThread(NULL)
	, ResourceCacheFileName()
	, RunInBrowser(NULL)
	, BrowserToolbar(NULL) {
		
}

void mvceditor::RunBrowserPluginClass::AddToolsMenuItems(wxMenu* toolsMenu) {
	RunInBrowser = new wxMenuItem(toolsMenu, mvceditor::MENU_RUN_BROWSER + 0, 
		_("Run In Web Browser"),
		_("Run the script in the chosen Web Browser"), wxITEM_NORMAL);
	toolsMenu->Append(RunInBrowser);
}

void mvceditor::RunBrowserPluginClass::AddWindows() {
	BrowserToolbar = new wxAuiToolBar(GetMainWindow(), wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE |
                                         wxAUI_TB_OVERFLOW |
                                         wxAUI_TB_TEXT |
                                         wxAUI_TB_HORZ_TEXT);
    BrowserToolbar->SetToolBitmapSize(wxSize(16,16));
    wxBitmap browserBitmap = wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16));
	BrowserToolbar->AddTool(ID_BROWSER_AUI_TOOLBAR, wxT("No Browsers Configured"), browserBitmap);
    BrowserToolbar->SetToolDropDown(ID_BROWSER_AUI_TOOLBAR, true);
	BrowserToolbar->AddSeparator();
    BrowserToolbar->AddTool(ID_URL_AUI_TOOLBAR, wxT("No URLs"), browserBitmap);
    BrowserToolbar->SetToolDropDown(ID_URL_AUI_TOOLBAR, true);
	BrowserToolbar->AddTool(ID_URL_SEARCH_AUI_TOOLBAR, _("Search for URLs..."), browserBitmap);
	BrowserToolbar->SetOverflowVisible(false);
    BrowserToolbar->Realize();

	// ATTN: not quite sure why wxAuiPaneInfo().ToolbarPane() wont work
	AuiManager->AddPane(BrowserToolbar, wxAuiPaneInfo().Top(
		).CaptionVisible(false).CloseButton(false).Gripper(
		false).DockFixed(true).PaneBorder(false).Floatable(false).Row(1).Position(0));
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
		App->UrlResourceFinder.Browsers.push_back(it->first);
	}
	if (!App->UrlResourceFinder.Browsers.empty()) {
		App->UrlResourceFinder.ChosenBrowser = App->UrlResourceFinder.Browsers[0];
	}
	if (BrowserToolbar) {
		BrowserToolbar->SetToolLabel(ID_BROWSER_AUI_TOOLBAR, App->UrlResourceFinder.Browsers[0]);
		BrowserToolbar->Realize();
	}
}

void mvceditor::RunBrowserPluginClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	wxBitmap bitmap = wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE, wxART_TOOLBAR, wxSize(16, 16));
	toolBar->AddTool(ID_TOOLBAR_BROWSER, _("Web Browser"), bitmap, _("Run On a Web Browser"));
}

void mvceditor::RunBrowserPluginClass::OnRunInWebBrowser(wxCommandEvent& event) {
	wxString browserName = App->UrlResourceFinder.ChosenBrowser;
	wxString url = App->UrlResourceFinder.ChosenUrl.Url;
	if (!browserName.IsEmpty() && !url.IsEmpty()) {
		mvceditor::EnvironmentClass* environment = GetEnvironment();
		ExternalBrowser(browserName, url, environment);
	}
}

void mvceditor::RunBrowserPluginClass::OnBrowserToolDropDown(wxAuiToolBarEvent& event) {
	if (event.IsDropDownClicked()) {
		BrowserToolbar->SetToolSticky(event.GetId(), true);
		mvceditor::EnvironmentClass* environment = GetEnvironment();
		
		// create the popup menu that contains all the available browser names
		wxMenu menuPopup;
		wxBitmap bmp = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_OTHER, wxSize(16,16));
		std::map<wxString, wxFileName> webBrowsers = environment->WebBrowsers;
		int i = 0;
		for (std::map<wxString, wxFileName>::const_iterator it = webBrowsers.begin(); it != webBrowsers.end(); ++it) {
			wxMenuItem* menuItem =  new wxMenuItem(&menuPopup, mvceditor::MENU_RUN_BROWSER + i, it->first);
			menuItem->SetBitmap(bmp);
			menuPopup.Append(menuItem);
			i++;
		}
		
		// line up our menu with the button
		wxRect rect = BrowserToolbar->GetToolRect(event.GetId());
		wxPoint pt = BrowserToolbar->ClientToScreen(rect.GetBottomLeft());
		pt = GetMainWindow()->ScreenToClient(pt);
		GetMainWindow()->PopupMenu(&menuPopup, pt);

		// make sure the button is "un-stuck"
		BrowserToolbar->SetToolSticky(event.GetId(), false);
	}
	else {
		event.Skip();
	}
}

void mvceditor::RunBrowserPluginClass::OnUrlToolDropDown(wxAuiToolBarEvent& event) {
	if (event.IsDropDownClicked()) {
		if (App->UrlResourceFinder.Urls.empty()) {
			return;
		}
		BrowserToolbar->SetToolSticky(event.GetId(), true);
		
		// create the popup menu that contains all the available browser names
		wxMenu menuPopup;
		wxBitmap bmp = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_OTHER, wxSize(16,16));
		std::vector<mvceditor::UrlResourceClass> urls = App->UrlResourceFinder.Urls;
		for (size_t i = 0; i < urls.size() && i < 40; ++i) {
			wxMenuItem* menuItem =  new wxMenuItem(&menuPopup, mvceditor::MENU_RUN_BROWSER_URLS + i, urls[i].Url);
			menuItem->SetBitmap(bmp);
			menuPopup.Append(menuItem);
		}
		
		// line up our menu with the button
		wxRect rect = BrowserToolbar->GetToolRect(event.GetId());
		wxPoint pt = BrowserToolbar->ClientToScreen(rect.GetBottomLeft());
		pt = GetMainWindow()->ScreenToClient(pt);
		GetMainWindow()->PopupMenu(&menuPopup, pt);

		// make sure the button is "un-stuck"
		BrowserToolbar->SetToolSticky(event.GetId(), false);
	}
	else {
		event.Skip();
	}
}

void mvceditor::RunBrowserPluginClass::OnUrlSearchTool(wxCommandEvent& event) {
wxString fileName;
	if (GetCurrentCodeControl()) {
		fileName = GetCurrentCodeControl()->GetFileName();
	}
	mvceditor::UrlResourceClass chosenUrl;
	mvceditor::EnvironmentClass* environment = GetEnvironment();
	if (PhPFrameworks().Identifiers.empty()) {
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
	}
	if (!App->UrlResourceFinder.Urls.empty()) {
		mvceditor::UrlChoiceClass urlChoice(App->UrlResourceFinder.Urls, fileName, environment);
		mvceditor::ChooseUrlDialogClass dialog(GetMainWindow(), urlChoice);
		if (wxOK == dialog.ShowModal()) {
			chosenUrl = urlChoice.ChosenUrl();
		}
	}
	else {
		
		// no URLs means that the file can be accessed normally
		// turn file name into a url in the default manner (by calculating from the vhost document root)
		chosenUrl.Url = environment->Apache.GetUrl(fileName);
		chosenUrl.FileName.Assign(fileName);
		if (chosenUrl.Url.IsEmpty()) {
			mvceditor::EditorLogWarning(mvceditor::INVALID_FILE, _("File is not under web root"));	
			return;
		}
	}
	if (!chosenUrl.Url.IsEmpty()) {
					
		// add to URL to the internal recent list so that the user can easily access it
		// multiple times.
		if (App->UrlResourceFinder.Urls.size() > 50) {
			App->UrlResourceFinder.Urls.pop_back();
		}
		mvceditor::UrlResourceClass urlResource;
		bool found = App->UrlResourceFinder.FindByUrl(chosenUrl.Url, urlResource);
		if (!found) {
			App->UrlResourceFinder.Urls.push_back(chosenUrl);
			App->UrlResourceFinder.ChosenUrl = chosenUrl;
		}
		else {
			App->UrlResourceFinder.ChosenUrl = urlResource;
		}
	}
}

void mvceditor::RunBrowserPluginClass::OnUrlDetectionComplete(mvceditor::UrlDetectedEventClass& event) {
	for (size_t i = 0; i < event.Urls.size(); ++i) {
		App->UrlResourceFinder.Urls.push_back(event.Urls[i]);
	}
	wxRemoveFile(ResourceCacheFileName.GetFullPath());
}

void mvceditor::RunBrowserPluginClass::OnUrlDetectionFailed(wxCommandEvent& event) {
	mvceditor::EditorLogWarning(mvceditor::PROJECT_DETECTION, event.GetString());
	wxRemoveFile(ResourceCacheFileName.GetFullPath());
}

void mvceditor::RunBrowserPluginClass::OnBrowserToolMenuItem(wxCommandEvent& event) {
	
	// detect the chosen browser based on the menu item name
	// change the current selection only if name is found
	// change both the data structure and the toolbar 
	wxMenu* menu = wxDynamicCast(event.GetEventObject(), wxMenu);
	if (menu) {
		wxString name = menu->GetLabelText(event.GetId());
		std::vector<wxString>::iterator found = std::find(App->UrlResourceFinder.Browsers.begin(), App->UrlResourceFinder.Browsers.end(), name);
		if (found != App->UrlResourceFinder.Browsers.end()) {
			App->UrlResourceFinder.ChosenBrowser = name;
			BrowserToolbar->SetToolLabel(ID_BROWSER_AUI_TOOLBAR, name);
			BrowserToolbar->Realize();
			AuiManager->Update();
		}
	}
}

void mvceditor::RunBrowserPluginClass::OnUrlToolMenuItem(wxCommandEvent& event) {
	
	// detect the chosen url based on the menu item name
	// change the current selection only if name is found
	// change both the data structure and the toolbar 
	wxMenu* menu = wxDynamicCast(event.GetEventObject(), wxMenu);
	if (menu) {
		wxString name = menu->GetLabelText(event.GetId());
		mvceditor::UrlResourceClass urlResource;
		bool found = App->UrlResourceFinder.FindByUrl(name, urlResource);
		if (found) {
			App->UrlResourceFinder.ChosenUrl = urlResource;
			BrowserToolbar->SetToolLabel(ID_URL_AUI_TOOLBAR, name);
			BrowserToolbar->Realize();
			AuiManager->Update();
		}
	}
}

void mvceditor::RunBrowserPluginClass::OnProjectIndexed() {
	if (!ResourceCacheThread.get()) {
		ResourceCacheThread.reset(new mvceditor::ResourceCacheUpdateThreadClass(GetResourceCache(), *this));
	}
	ResourceCacheFileName.Assign(wxFileName::CreateTempFileName(wxT("resource_cache")));
	ResourceCacheThread->StartPersist(ResourceCacheFileName);
}

void mvceditor::RunBrowserPluginClass::OnWorkComplete(wxCommandEvent& event) {
	mvceditor::EnvironmentClass* environment = GetEnvironment();
	mvceditor::ProjectClass* project = GetProject();
	wxString projectRootUrl =  environment->Apache.GetUrl(project->GetRootPath());
	if (!PhpFrameworks.get()) {
		PhpFrameworks.reset(new PhpFrameworkDetectorClass(*this, *GetEnvironment()));
		PhpFrameworks->Identifiers = PhPFrameworks().Identifiers;
		PhpFrameworks->InitUrlDetector(GetProject()->GetRootPath(), ResourceCacheFileName.GetFullPath(), projectRootUrl);
	}
	else {
		PhpFrameworks->InitUrlDetector(GetProject()->GetRootPath(), ResourceCacheFileName.GetFullPath(), projectRootUrl);
	}
}

BEGIN_EVENT_TABLE(mvceditor::RunBrowserPluginClass, wxEvtHandler) 
	EVT_MENU(mvceditor::MENU_RUN_BROWSER + 0, mvceditor::RunBrowserPluginClass::OnRunInWebBrowser)
	
	// if the end values of the ranges need to be modified, need to modify mvceditor::PluginClass::MenuIds as well
	EVT_MENU_RANGE(mvceditor::MENU_RUN_BROWSER + 0, mvceditor::MENU_RUN_BROWSER + 19, mvceditor::RunBrowserPluginClass::OnBrowserToolMenuItem)
	EVT_MENU_RANGE(mvceditor::MENU_RUN_BROWSER_URLS + 0, mvceditor::MENU_RUN_BROWSER_URLS + 39, mvceditor::RunBrowserPluginClass::OnUrlToolMenuItem)
	EVT_TOOL(ID_TOOLBAR_BROWSER, mvceditor::RunBrowserPluginClass::OnRunInWebBrowser)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(ID_BROWSER_AUI_TOOLBAR, mvceditor::RunBrowserPluginClass::OnBrowserToolDropDown)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(ID_URL_AUI_TOOLBAR, mvceditor::RunBrowserPluginClass::OnUrlToolDropDown)
	EVT_TOOL(ID_URL_SEARCH_AUI_TOOLBAR, mvceditor::RunBrowserPluginClass::OnUrlSearchTool)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_FRAMEWORK_URL_FAILED, mvceditor::RunBrowserPluginClass::OnUrlDetectionFailed)
	EVT_FRAMEWORK_URL_COMPLETE(mvceditor::RunBrowserPluginClass::OnUrlDetectionComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_COMPLETE, mvceditor::RunBrowserPluginClass::OnWorkComplete)
END_EVENT_TABLE()