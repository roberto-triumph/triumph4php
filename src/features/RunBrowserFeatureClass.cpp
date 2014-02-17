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
#include <features/RunBrowserFeatureClass.h>
#include <actions/UrlTagDetectorActionClass.h>
#include <globals/Errors.h>
#include <globals/Assets.h>
#include <widgets/ChooseUrlDialogClass.h>
#include <Triumph.h>
#include <wx/artprov.h>
#include <algorithm>

static const size_t MAX_BROWSERS = 10;
static const size_t MAX_URLS = 40;

static void ExternalBrowser(const wxString& browserName, const wxURI& url, t4p::EnvironmentClass* environment) {
	wxFileName webBrowserPath;
	bool found = environment->FindBrowserByName(browserName, webBrowserPath);
	if (!found || !webBrowserPath.IsOk()) {
		t4p::EditorLogWarning(t4p::ERR_BAD_WEB_BROWSER_EXECUTABLE, webBrowserPath.GetFullPath());
		return;
	}
	wxString cmd = wxT("\"") + webBrowserPath.GetFullPath() + wxT("\""); 
	cmd += wxT(" \"");
	cmd += url.BuildURI();
	cmd += wxT("\"");
			
	// dont track this PID, let the browser stay open if the editor is closed.
	// if we dont do pass the make group leader flag the browser thinks it crashed and will tell the user 
	// that the browser crashed. 
	long pid = wxExecute(cmd, wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER);
	if (pid <= 0) {
		t4p::EditorLogWarning(t4p::ERR_BAD_WEB_BROWSER_EXECUTABLE, cmd);
	}
}

/**
 * get all of the browser icons and load them into memory.
 * @return vector the returned bitmaps 
 *         the returned bitmaps will alaways have a guaranteed order
 *         0 = chrome icon
 *         1 = firefox icon
 *         2 = internet explorer icon
 *         3 = opera icon
 *         4 = safari icon
 *         5 = unknown 'generic' web browser icon
 *         
 */
static std::vector<wxBitmap> BrowserIconsLoad() {
	wxBitmap browserOperaBitmap = t4p::IconImageAsset(wxT("browser-opera"));
	wxBitmap browserChromeBitmap = t4p::IconImageAsset(wxT("browser-chrome"));
	wxBitmap browserInternetExplorerBitmap = t4p::IconImageAsset(wxT("browser-internet-explorer"));
	wxBitmap browserFirefoxBitmap = t4p::IconImageAsset(wxT("browser-firefox"));
	wxBitmap browserSafariBitmap = t4p::IconImageAsset(wxT("browser-safari"));
	wxBitmap browserGenericBitmap = t4p::IconImageAsset(wxT("browser-generic"));

	std::vector<wxBitmap> icons;
	icons.push_back(browserChromeBitmap);
	icons.push_back(browserFirefoxBitmap);
	icons.push_back(browserInternetExplorerBitmap);
	icons.push_back(browserOperaBitmap);
	icons.push_back(browserSafariBitmap);
	icons.push_back(browserGenericBitmap);
	return icons;
}

/**
 * @param browserIcons vector of the loaded browser bitmaps
 * @param name string to compare
 * @return int index into browserIcons of the icon that should be displayed with the given name
 */
static int BrowserIconsIndex(std::vector<wxBitmap>&  browserIcons, const wxString& name) {
	wxString lowerCaseBrowserName(name);
	lowerCaseBrowserName.LowerCase();
	if (lowerCaseBrowserName.Contains(wxT("chrome"))) {
		return 0;
	}
	else if (lowerCaseBrowserName.Contains(wxT("firefox"))) {
		return 1;
	}
	else if (lowerCaseBrowserName.Contains(wxT("explorer"))) {
		return 2;
	}
	else if (lowerCaseBrowserName.Contains(wxT("opera"))) {
		return 3;
	}
	else if (lowerCaseBrowserName.Contains(wxT("safari"))) {
		return 4;
	}
	return 5;
}

/**
 * set the menu item icon to the corresponding web browser icon.  the menu item name
 * will determine what browser icon we use
 */
static void BrowserIconsMenuSet(std::vector<wxBitmap>&  browserIcons, wxMenuItem* menuItem) {
	int index = BrowserIconsIndex(browserIcons, menuItem->GetItemLabelText());
	if (index >= 0 && index < (int)browserIcons.size()) {
		menuItem->SetBitmap(browserIcons[index]);
	}
}

t4p::RunBrowserFeatureClass::RunBrowserFeatureClass(t4p::AppClass& app)
	: FeatureClass(app) 
	, RecentUrls()
	, BrowserMenu()
	, UrlMenu()
	, GaugeDialog(NULL)
	, RunInBrowser(NULL)
	, BrowserToolbar(NULL)
	, IsUrlCacheStale(false)
	, IsWaitingForUrlDetection(false) {
}

void t4p::RunBrowserFeatureClass::AddWindows() {
	BrowserToolbar = new wxAuiToolBar(GetMainWindow(), wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE |
                                         wxAUI_TB_TEXT |
                                         wxAUI_TB_HORZ_TEXT);
	BrowserToolbar->SetFont(App.Preferences.ApplicationFont);
    BrowserToolbar->SetToolBitmapSize(wxSize(16,16));
    wxBitmap browserBitmap = t4p::IconImageAsset(wxT("browser-generic"));
	wxBitmap runBitmap = t4p::IconImageAsset(wxT("run-browser"));
	wxBitmap searchUrlBitmap = t4p::IconImageAsset(wxT("search-url-tags"));
	wxBitmap recentUrlBitmap = t4p::IconImageAsset(wxT("recent-urls"));

	BrowserToolbar->AddTool(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, _("No Browsers Configured"), browserBitmap);
    BrowserToolbar->SetToolDropDown(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, true);
	BrowserToolbar->AddSeparator();
    BrowserToolbar->AddTool(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 3, _("No URLs"), recentUrlBitmap);
    BrowserToolbar->SetToolDropDown(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 3, true);
	
	
	BrowserToolbar->AddTool(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 1, _("Run In Web Browser"), runBitmap);
	BrowserToolbar->AddTool(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 4, _("Search for URLs..."), searchUrlBitmap);
	BrowserToolbar->SetOverflowVisible(false);
    BrowserToolbar->Realize();

	AuiManager->AddPane(BrowserToolbar, wxAuiPaneInfo()
		.ToolbarPane().Top().Row(2).Position(1)
		.LeftDockable(false).RightDockable(false)
		.Gripper(false).CaptionVisible(false).CloseButton(false).DockFixed(true)
		.PaneBorder(true).Floatable(false)
	);
}

void t4p::RunBrowserFeatureClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	//std::map<int, wxString> menuItemIds;
	//menuItemIds[t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 1] = wxT("Run-In Web Browser");
	//AddDynamicCmd(menuItemIds, shortcuts);
}

void t4p::RunBrowserFeatureClass::LoadPreferences(wxConfigBase* config) {

	// dont use the config; use the Environment that has already been seeded with 
	// the proper data
	App.Globals.ChosenBrowser = wxT("");
	std::vector<wxString> browserNames = App.Globals.Environment.BrowserNames();
	if (!browserNames.empty()) {
		App.Globals.ChosenBrowser = browserNames[0];
	}
	if (BrowserToolbar) {
		BrowserToolbar->SetToolLabel(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, App.Globals.ChosenBrowser);
		std::vector<wxBitmap> browserIcons = BrowserIconsLoad();
		int index = BrowserIconsIndex(browserIcons, App.Globals.ChosenBrowser);
		if (index >= 0 && index < (int)browserIcons.size()) {		
			BrowserToolbar->SetToolBitmap(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, browserIcons[index]);
		}
		BrowserToolbar->Realize();
	}
}

void t4p::RunBrowserFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {

	// need to update the browser toolbar if the user updates the environment
	std::vector<wxString> browserNames = App.Globals.Environment.BrowserNames();
	
	// for now just make the first item as selected
	App.Globals.ChosenBrowser = wxT("");
	if (!browserNames.empty()) {
		App.Globals.ChosenBrowser = browserNames[0];
	}
	if (BrowserToolbar) {
		BrowserToolbar->SetToolLabel(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, App.Globals.ChosenBrowser);
		std::vector<wxBitmap> browserIcons = BrowserIconsLoad();
		int index = BrowserIconsIndex(browserIcons, App.Globals.ChosenBrowser);
		if (index >= 0 && index < (int)browserIcons.size()) {		
			BrowserToolbar->SetToolBitmap(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, browserIcons[index]);
		}
		BrowserToolbar->Realize();
		AuiManager->Update();
	}
}

void t4p::RunBrowserFeatureClass::OnRunInWebBrowser(wxCommandEvent& event) {
	wxString browserName = App.Globals.ChosenBrowser;
	wxURI url = App.Globals.CurrentUrl.Url;
	if (!browserName.IsEmpty() && !url.BuildURI().IsEmpty()) {
		t4p::EnvironmentClass* environment = GetEnvironment();
		ExternalBrowser(browserName, url, environment);
	}
}

void t4p::RunBrowserFeatureClass::OnBrowserToolDropDown(wxAuiToolBarEvent& event) {
	if (!event.IsDropDownClicked()) {
		event.Skip();
		return;
	}
	std::vector<wxBitmap> browserIcons = BrowserIconsLoad();
	BrowserToolbar->SetToolSticky(event.GetId(), true);

	// create the popup menu that contains all the available browser names
	if (!BrowserMenu.get()) {
		BrowserMenu.reset(new wxMenu);
	}
	while (BrowserMenu->GetMenuItemCount() > 0) {
		BrowserMenu->Delete(BrowserMenu->FindItemByPosition(0)->GetId());
	}
	std::vector<wxString> browserNames = App.Globals.Environment.BrowserNames();
	for (size_t i = 0; i < browserNames.size();  ++i) {
		wxString browserName = browserNames[i];
		wxMenuItem* menuItem =  new wxMenuItem(BrowserMenu.get(), t4p::MENU_RUN_BROWSER + i, browserName);
		BrowserIconsMenuSet(browserIcons, menuItem);
		BrowserMenu->Append(menuItem);
	}
	
	// line up our menu with the button
	wxRect rect = BrowserToolbar->GetToolRect(event.GetId());
	wxPoint pt = BrowserToolbar->ClientToScreen(rect.GetBottomLeft());
	pt = GetMainWindow()->ScreenToClient(pt);
	GetMainWindow()->PopupMenu(BrowserMenu.get(), pt);

	// make sure the button is "un-stuck"
	BrowserToolbar->SetToolSticky(event.GetId(), false);
}

void t4p::RunBrowserFeatureClass::OnUrlToolDropDown(wxAuiToolBarEvent& event) {
	if (event.IsDropDownClicked()) {
		if (RecentUrls.empty()) {
			return;
		}
		wxBitmap recentUrlBitmap = t4p::IconImageAsset(wxT("recent-urls"));

		BrowserToolbar->SetToolSticky(event.GetId(), true);
		
		// create the popup menu that contains all the recent URLs
		if (!UrlMenu.get()) {
			UrlMenu.reset(new wxMenu);
		}
		while (UrlMenu->GetMenuItemCount() > 0) {
			UrlMenu->Delete(UrlMenu->FindItemByPosition(0)->GetId());
		}
		for (size_t i = 0; i < RecentUrls.size() && i < MAX_URLS; ++i) {
			wxString url = RecentUrls[i].Url.BuildURI();

			// make sure to watch out for ampersans in the URL, so that the menu does not think 
			// they are menu accelerators
			url.Replace(wxT("&"), wxT("&&"));
			wxMenuItem* menuItem =  new wxMenuItem(UrlMenu.get(), t4p::MENU_RUN_BROWSER + MAX_BROWSERS + i, url);
			menuItem->SetBitmap(recentUrlBitmap);
			UrlMenu->Append(menuItem);
		}
		
		// line up our menu with the button
		wxRect rect = BrowserToolbar->GetToolRect(event.GetId());
		wxPoint pt = BrowserToolbar->ClientToScreen(rect.GetBottomLeft());
		pt = GetMainWindow()->ScreenToClient(pt);
		GetMainWindow()->PopupMenu(UrlMenu.get(), pt);

		// make sure the button is "un-stuck"
		BrowserToolbar->SetToolSticky(event.GetId(), false);
	}
	else {
		event.Skip();
	}
}

void t4p::RunBrowserFeatureClass::OnUrlSearchTool(wxCommandEvent& event) {
	std::vector<wxFileName> sourceDirs = App.Globals.AllEnabledSourceDirectories();
	if (IsUrlCacheStale) {
		IsWaitingForUrlDetection = true;

		t4p::UrlTagDetectorActionClass* action = new t4p::UrlTagDetectorActionClass(App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_URL_TAG_DETECTOR);
		std::vector<t4p::GlobalActionClass*> actions;
		actions.push_back(action);
		App.Sequences.Build(actions);

		if (GaugeDialog) {
			GaugeDialog->Destroy();
			GaugeDialog = NULL;
		}
		GaugeDialog = new t4p::GaugeDialogClass(GetMainWindow(), _("URL Detection"), _("Running URL Detection"));
	}
	else if (App.Globals.UrlTagFinder.Count(sourceDirs) > 0) {
		ShowUrlDialog();
	}
	else {
		t4p::EditorLogWarning(t4p::WARNING_OTHER,
			_("Could not determine URL routes."));
	}
}

void t4p::RunBrowserFeatureClass::ShowUrlDialog() {
	t4p::ChooseUrlDialogClass dialog(GetMainWindow(), App.Globals.UrlTagFinder, App.Globals.Projects, App.Globals.CurrentUrl);
	if (wxOK == dialog.ShowModal() && !App.Globals.CurrentUrl.Url.BuildURI().IsEmpty()) {
				
		// 'select' the URL (make it the current in the toolbar)
		BrowserToolbar->SetToolLabel(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 3, App.Globals.CurrentUrl.Url.BuildURI());
		BrowserToolbar->Realize();
		AuiManager->Update();

		// add it to the Recent list, or push up to the top of the Recent list if its already there
		bool found = false;
		for (std::vector<t4p::UrlTagClass>::iterator it = RecentUrls.begin(); it  != RecentUrls.end(); ++it) {
			if (it->Url == App.Globals.CurrentUrl.Url) {
				found = true;
				RecentUrls.erase(it);
				RecentUrls.insert(RecentUrls.begin(), App.Globals.CurrentUrl);
				break;
			}
		}
		if (!found) {
			RecentUrls.insert(RecentUrls.begin(), App.Globals.CurrentUrl);
		}
		t4p::EnvironmentClass* environment = GetEnvironment();
		wxString browserName = App.Globals.ChosenBrowser;
		if (!browserName.IsEmpty()) {
			ExternalBrowser(browserName, App.Globals.CurrentUrl.Url, environment);
		}
	}
}

void t4p::RunBrowserFeatureClass::OnBrowserToolMenuItem(wxCommandEvent& event) {

	// detect the chosen browser based on the menu item name
	// change the current selection only if name is found
	// change both the data structure and the toolbar
	if (BrowserMenu.get()) {
		wxString name = BrowserMenu->GetLabelText(event.GetId());
		std::vector<wxString> browserNames = App.Globals.Environment.BrowserNames();
		std::vector<wxString>::iterator found = std::find(browserNames.begin(), browserNames.end(), name);
		if (found != browserNames.end()) {
			App.Globals.ChosenBrowser = name;

			std::vector<wxBitmap> browserIcons = BrowserIconsLoad();
			int browserIndex = BrowserIconsIndex(browserIcons, name);
			BrowserToolbar->SetToolLabel(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, name);
			if (browserIndex >= 0 && browserIndex < (int)browserIcons.size()) {
				BrowserToolbar->SetToolBitmap(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, browserIcons[browserIndex]);
			}
			BrowserToolbar->Realize();
			AuiManager->Update();
		}
	}
}

void t4p::RunBrowserFeatureClass::OnUrlToolMenuItem(wxCommandEvent& event) {
	
	// detect the chosen url based on the menu item name
	// change the current selection only if name is found
	// change both the data structure and the toolbar 
	if (UrlMenu.get()) {
		wxString name = UrlMenu->GetLabelText(event.GetId());
		t4p::UrlTagClass urlTag;
		bool found = false;
		for (size_t i = 0; i < RecentUrls.size(); i++) {
			if (RecentUrls[i].Url.BuildURI() == name) {
				urlTag = RecentUrls[i]; 
				found = true;
				break;
			}
		}
		if (found) {
			App.Globals.CurrentUrl = urlTag;
			BrowserToolbar->SetToolLabel(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 3, name);
			BrowserToolbar->Realize();
			AuiManager->Update();
		}
	}
}

void t4p::RunBrowserFeatureClass::OnFileSaved(t4p::CodeControlEventClass& event) {
	if (IsUrlCacheStale) {

		// already know that cache is stale, then no need to check for dirty again
		return;
	}
	t4p::CodeControlClass* control = event.GetCodeControl();
	if (!control) {
		return;
	}
	wxString fileName = control->GetFileName();
	std::vector<t4p::ProjectClass>::const_iterator project;
	for (project = App.Globals.Projects.begin(); project != App.Globals.Projects.end(); ++project) {
		if (project->IsEnabled && project->IsAPhpSourceFile(fileName)) {
			IsUrlCacheStale = true;
			break;
		}
	}
}

void t4p::RunBrowserFeatureClass::OnUrlDetectionComplete(t4p::ActionEventClass& event) {
	IsUrlCacheStale = false;
	if (!IsWaitingForUrlDetection) {

		// we did not trigger the url detection
		return;
	}
	if (GaugeDialog) {
		GaugeDialog->Destroy();
		GaugeDialog = NULL;
	}
	IsWaitingForUrlDetection = false;

	// we triggered the url detection, show the user the url dialog
	ShowUrlDialog();
}

BEGIN_EVENT_TABLE(t4p::RunBrowserFeatureClass, wxEvtHandler) 
	
	// if the end values of the ranges need to be modified, need to modify t4p::FeatureClass::MenuIds as well
	EVT_MENU_RANGE(t4p::MENU_RUN_BROWSER + 0, t4p::MENU_RUN_BROWSER + MAX_BROWSERS - 1, t4p::RunBrowserFeatureClass::OnBrowserToolMenuItem)
	EVT_MENU_RANGE(t4p::MENU_RUN_BROWSER + MAX_BROWSERS, t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS - 1, t4p::RunBrowserFeatureClass::OnUrlToolMenuItem)
	EVT_TOOL(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 1, t4p::RunBrowserFeatureClass::OnRunInWebBrowser)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, t4p::RunBrowserFeatureClass::OnBrowserToolDropDown)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 3, t4p::RunBrowserFeatureClass::OnUrlToolDropDown)
	EVT_TOOL(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 4, t4p::RunBrowserFeatureClass::OnUrlSearchTool)

	// application events
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::RunBrowserFeatureClass::OnPreferencesSaved)
	EVT_APP_FILE_SAVED(t4p::RunBrowserFeatureClass::OnFileSaved)
	EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_URL_TAG_DETECTOR, t4p::RunBrowserFeatureClass::OnUrlDetectionComplete)
END_EVENT_TABLE()
