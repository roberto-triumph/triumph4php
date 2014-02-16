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

static void ExternalBrowser(const wxString& browserName, const wxURI& url, mvceditor::EnvironmentClass* environment) {
	wxFileName webBrowserPath;
	bool found = environment->FindBrowserByName(browserName, webBrowserPath);
	if (!found || !webBrowserPath.IsOk()) {
		mvceditor::EditorLogWarning(mvceditor::ERR_BAD_WEB_BROWSER_EXECUTABLE, webBrowserPath.GetFullPath());
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
		mvceditor::EditorLogWarning(mvceditor::ERR_BAD_WEB_BROWSER_EXECUTABLE, cmd);
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
	wxBitmap browserOperaBitmap = mvceditor::IconImageAsset(wxT("browser-opera"));
	wxBitmap browserChromeBitmap = mvceditor::IconImageAsset(wxT("browser-chrome"));
	wxBitmap browserInternetExplorerBitmap = mvceditor::IconImageAsset(wxT("browser-internet-explorer"));
	wxBitmap browserFirefoxBitmap = mvceditor::IconImageAsset(wxT("browser-firefox"));
	wxBitmap browserSafariBitmap = mvceditor::IconImageAsset(wxT("browser-safari"));
	wxBitmap browserGenericBitmap = mvceditor::IconImageAsset(wxT("browser-generic"));

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

mvceditor::RunBrowserFeatureClass::RunBrowserFeatureClass(mvceditor::AppClass& app)
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

void mvceditor::RunBrowserFeatureClass::AddWindows() {
	BrowserToolbar = new wxAuiToolBar(GetMainWindow(), wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE |
                                         wxAUI_TB_TEXT |
                                         wxAUI_TB_HORZ_TEXT);
	BrowserToolbar->SetFont(App.Preferences.ApplicationFont);
    BrowserToolbar->SetToolBitmapSize(wxSize(16,16));
    wxBitmap browserBitmap = mvceditor::IconImageAsset(wxT("browser-generic"));
	wxBitmap runBitmap = mvceditor::IconImageAsset(wxT("run-browser"));
	wxBitmap searchUrlBitmap = mvceditor::IconImageAsset(wxT("search-url-tags"));
	wxBitmap recentUrlBitmap = mvceditor::IconImageAsset(wxT("recent-urls"));

	BrowserToolbar->AddTool(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, _("No Browsers Configured"), browserBitmap);
    BrowserToolbar->SetToolDropDown(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, true);
	BrowserToolbar->AddSeparator();
    BrowserToolbar->AddTool(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 3, _("No URLs"), recentUrlBitmap);
    BrowserToolbar->SetToolDropDown(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 3, true);
	
	
	BrowserToolbar->AddTool(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 1, _("Run In Web Browser"), runBitmap);
	BrowserToolbar->AddTool(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 4, _("Search for URLs..."), searchUrlBitmap);
	BrowserToolbar->SetOverflowVisible(false);
    BrowserToolbar->Realize();

	AuiManager->AddPane(BrowserToolbar, wxAuiPaneInfo()
		.ToolbarPane().Top().Row(2).Position(1)
		.LeftDockable(false).RightDockable(false)
		.Gripper(false).CaptionVisible(false).CloseButton(false).DockFixed(true)
		.PaneBorder(true).Floatable(false)
	);
}

void mvceditor::RunBrowserFeatureClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	//std::map<int, wxString> menuItemIds;
	//menuItemIds[mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 1] = wxT("Run-In Web Browser");
	//AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::RunBrowserFeatureClass::LoadPreferences(wxConfigBase* config) {

	// dont use the config; use the Environment that has already been seeded with 
	// the proper data
	App.Globals.ChosenBrowser = wxT("");
	std::vector<wxString> browserNames = App.Globals.Environment.BrowserNames();
	if (!browserNames.empty()) {
		App.Globals.ChosenBrowser = browserNames[0];
	}
	if (BrowserToolbar) {
		BrowserToolbar->SetToolLabel(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, App.Globals.ChosenBrowser);
		std::vector<wxBitmap> browserIcons = BrowserIconsLoad();
		int index = BrowserIconsIndex(browserIcons, App.Globals.ChosenBrowser);
		if (index >= 0 && index < (int)browserIcons.size()) {		
			BrowserToolbar->SetToolBitmap(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, browserIcons[index]);
		}
		BrowserToolbar->Realize();
	}
}

void mvceditor::RunBrowserFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {

	// need to update the browser toolbar if the user updates the environment
	std::vector<wxString> browserNames = App.Globals.Environment.BrowserNames();
	
	// for now just make the first item as selected
	App.Globals.ChosenBrowser = wxT("");
	if (!browserNames.empty()) {
		App.Globals.ChosenBrowser = browserNames[0];
	}
	if (BrowserToolbar) {
		BrowserToolbar->SetToolLabel(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, App.Globals.ChosenBrowser);
		std::vector<wxBitmap> browserIcons = BrowserIconsLoad();
		int index = BrowserIconsIndex(browserIcons, App.Globals.ChosenBrowser);
		if (index >= 0 && index < (int)browserIcons.size()) {		
			BrowserToolbar->SetToolBitmap(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, browserIcons[index]);
		}
		BrowserToolbar->Realize();
		AuiManager->Update();
	}
}

void mvceditor::RunBrowserFeatureClass::OnRunInWebBrowser(wxCommandEvent& event) {
	wxString browserName = App.Globals.ChosenBrowser;
	wxURI url = App.Globals.CurrentUrl.Url;
	if (!browserName.IsEmpty() && !url.BuildURI().IsEmpty()) {
		mvceditor::EnvironmentClass* environment = GetEnvironment();
		ExternalBrowser(browserName, url, environment);
	}
}

void mvceditor::RunBrowserFeatureClass::OnBrowserToolDropDown(wxAuiToolBarEvent& event) {
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
		wxMenuItem* menuItem =  new wxMenuItem(BrowserMenu.get(), mvceditor::MENU_RUN_BROWSER + i, browserName);
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

void mvceditor::RunBrowserFeatureClass::OnUrlToolDropDown(wxAuiToolBarEvent& event) {
	if (event.IsDropDownClicked()) {
		if (RecentUrls.empty()) {
			return;
		}
		wxBitmap recentUrlBitmap = mvceditor::IconImageAsset(wxT("recent-urls"));

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
			wxMenuItem* menuItem =  new wxMenuItem(UrlMenu.get(), mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + i, url);
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

void mvceditor::RunBrowserFeatureClass::OnUrlSearchTool(wxCommandEvent& event) {
	std::vector<wxFileName> sourceDirs = App.Globals.AllEnabledSourceDirectories();
	if (IsUrlCacheStale) {
		IsWaitingForUrlDetection = true;

		mvceditor::UrlTagDetectorActionClass* action = new mvceditor::UrlTagDetectorActionClass(App.SqliteRunningThreads, mvceditor::ID_EVENT_ACTION_URL_TAG_DETECTOR);
		std::vector<mvceditor::GlobalActionClass*> actions;
		actions.push_back(action);
		App.Sequences.Build(actions);

		if (GaugeDialog) {
			GaugeDialog->Destroy();
			GaugeDialog = NULL;
		}
		GaugeDialog = new mvceditor::GaugeDialogClass(GetMainWindow(), _("URL Detection"), _("Running URL Detection"));
	}
	else if (App.Globals.UrlTagFinder.Count(sourceDirs) > 0) {
		ShowUrlDialog();
	}
	else {
		mvceditor::EditorLogWarning(mvceditor::WARNING_OTHER,
			_("Could not determine URL routes."));
	}
}

void mvceditor::RunBrowserFeatureClass::ShowUrlDialog() {
	mvceditor::ChooseUrlDialogClass dialog(GetMainWindow(), App.Globals.UrlTagFinder, App.Globals.Projects, App.Globals.CurrentUrl);
	if (wxOK == dialog.ShowModal() && !App.Globals.CurrentUrl.Url.BuildURI().IsEmpty()) {
				
		// 'select' the URL (make it the current in the toolbar)
		BrowserToolbar->SetToolLabel(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 3, App.Globals.CurrentUrl.Url.BuildURI());
		BrowserToolbar->Realize();
		AuiManager->Update();

		// add it to the Recent list, or push up to the top of the Recent list if its already there
		bool found = false;
		for (std::vector<mvceditor::UrlTagClass>::iterator it = RecentUrls.begin(); it  != RecentUrls.end(); ++it) {
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
		mvceditor::EnvironmentClass* environment = GetEnvironment();
		wxString browserName = App.Globals.ChosenBrowser;
		if (!browserName.IsEmpty()) {
			ExternalBrowser(browserName, App.Globals.CurrentUrl.Url, environment);
		}
	}
}

void mvceditor::RunBrowserFeatureClass::OnBrowserToolMenuItem(wxCommandEvent& event) {

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
			BrowserToolbar->SetToolLabel(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, name);
			if (browserIndex >= 0 && browserIndex < (int)browserIcons.size()) {
				BrowserToolbar->SetToolBitmap(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, browserIcons[browserIndex]);
			}
			BrowserToolbar->Realize();
			AuiManager->Update();
		}
	}
}

void mvceditor::RunBrowserFeatureClass::OnUrlToolMenuItem(wxCommandEvent& event) {
	
	// detect the chosen url based on the menu item name
	// change the current selection only if name is found
	// change both the data structure and the toolbar 
	if (UrlMenu.get()) {
		wxString name = UrlMenu->GetLabelText(event.GetId());
		mvceditor::UrlTagClass urlTag;
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
			BrowserToolbar->SetToolLabel(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 3, name);
			BrowserToolbar->Realize();
			AuiManager->Update();
		}
	}
}

void mvceditor::RunBrowserFeatureClass::OnFileSaved(mvceditor::CodeControlEventClass& event) {
	if (IsUrlCacheStale) {

		// already know that cache is stale, then no need to check for dirty again
		return;
	}
	mvceditor::CodeControlClass* control = event.GetCodeControl();
	if (!control) {
		return;
	}
	wxString fileName = control->GetFileName();
	std::vector<mvceditor::ProjectClass>::const_iterator project;
	for (project = App.Globals.Projects.begin(); project != App.Globals.Projects.end(); ++project) {
		if (project->IsEnabled && project->IsAPhpSourceFile(fileName)) {
			IsUrlCacheStale = true;
			break;
		}
	}
}

void mvceditor::RunBrowserFeatureClass::OnUrlDetectionComplete(mvceditor::ActionEventClass& event) {
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

BEGIN_EVENT_TABLE(mvceditor::RunBrowserFeatureClass, wxEvtHandler) 
	
	// if the end values of the ranges need to be modified, need to modify mvceditor::FeatureClass::MenuIds as well
	EVT_MENU_RANGE(mvceditor::MENU_RUN_BROWSER + 0, mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS - 1, mvceditor::RunBrowserFeatureClass::OnBrowserToolMenuItem)
	EVT_MENU_RANGE(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS, mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS - 1, mvceditor::RunBrowserFeatureClass::OnUrlToolMenuItem)
	EVT_TOOL(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 1, mvceditor::RunBrowserFeatureClass::OnRunInWebBrowser)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, mvceditor::RunBrowserFeatureClass::OnBrowserToolDropDown)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 3, mvceditor::RunBrowserFeatureClass::OnUrlToolDropDown)
	EVT_TOOL(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 4, mvceditor::RunBrowserFeatureClass::OnUrlSearchTool)

	// application events
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_PREFERENCES_SAVED, mvceditor::RunBrowserFeatureClass::OnPreferencesSaved)
	EVT_APP_FILE_SAVED(mvceditor::RunBrowserFeatureClass::OnFileSaved)
	EVT_ACTION_COMPLETE(mvceditor::ID_EVENT_ACTION_URL_TAG_DETECTOR, mvceditor::RunBrowserFeatureClass::OnUrlDetectionComplete)
END_EVENT_TABLE()
