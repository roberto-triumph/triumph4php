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
#include <views/RunBrowserViewClass.h>
#include <features/RunBrowserFeatureClass.h>
#include <actions/UrlTagDetectorActionClass.h>
#include <globals/Errors.h>
#include <globals/Assets.h>
#include <globals/Number.h>
#include <widgets/ChooseUrlDialogClass.h>
#include <Triumph.h>
#include <wx/artprov.h>
#include <algorithm>

static const size_t MAX_BROWSERS = 10;
static const size_t MAX_URLS = 40;

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
	wxBitmap browserOperaBitmap = t4p::BitmapImageAsset(wxT("browser-opera"));
	wxBitmap browserChromeBitmap = t4p::BitmapImageAsset(wxT("browser-chrome"));
	wxBitmap browserInternetExplorerBitmap = t4p::BitmapImageAsset(wxT("browser-internet-explorer"));
	wxBitmap browserFirefoxBitmap = t4p::BitmapImageAsset(wxT("browser-firefox"));
	wxBitmap browserSafariBitmap = t4p::BitmapImageAsset(wxT("browser-safari"));
	wxBitmap browserGenericBitmap = t4p::BitmapImageAsset(wxT("browser-generic"));

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
	if (t4p::NumberLessThan(index, browserIcons.size())) {
		menuItem->SetBitmap(browserIcons[index]);
	}
}

t4p::RunBrowserViewClass::RunBrowserViewClass(t4p::RunBrowserFeatureClass& feature)
	: FeatureViewClass()
	, Feature(feature)
	, BrowserMenu()
	, UrlMenu()
	, GaugeDialog(NULL)
	, RunInBrowser(NULL)
	, BrowserToolbar(NULL)
	, IsUrlCacheStale(false)
	, IsWaitingForUrlDetection(false) {
}

void t4p::RunBrowserViewClass::SetCurrentBrowser() {
	// dont use the config; use the Environment that has already been seeded with
	// the proper data
	Feature.App.Globals.ChosenBrowser = wxT("");
	std::vector<wxString> browserNames = Feature.App.Globals.Environment.BrowserNames();
	if (!browserNames.empty()) {
		Feature.App.Globals.ChosenBrowser = browserNames[0];
	}
	if (BrowserToolbar) {
		BrowserToolbar->SetToolLabel(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, Feature.App.Globals.ChosenBrowser);
		std::vector<wxBitmap> browserIcons = BrowserIconsLoad();
		int index = BrowserIconsIndex(browserIcons, Feature.App.Globals.ChosenBrowser);
		if (t4p::NumberLessThan(index, browserIcons.size())) {
			BrowserToolbar->SetToolBitmap(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, browserIcons[index]);
		}
		BrowserToolbar->Realize();
	}
}

void t4p::RunBrowserViewClass::AddWindows() {
	BrowserToolbar = new wxAuiToolBar(GetMainWindow(), wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE |
                                         wxAUI_TB_TEXT |
                                         wxAUI_TB_HORZ_TEXT);
	BrowserToolbar->SetFont(Feature.App.Preferences.ApplicationFont);
	BrowserToolbar->SetToolBitmapSize(wxSize(16, 16));
	wxBitmap browserBitmap = t4p::BitmapImageAsset(wxT("browser-generic"));
	wxBitmap runBitmap = t4p::BitmapImageAsset(wxT("run-browser"));
	wxBitmap searchUrlBitmap = t4p::BitmapImageAsset(wxT("search-url-tags"));
	wxBitmap recentUrlBitmap = t4p::BitmapImageAsset(wxT("recent-urls"));

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
		.ToolbarPane().Top().Row(2).Position(2)
		.LeftDockable(false).RightDockable(false)
		.Gripper(false).CaptionVisible(false).CloseButton(false).DockFixed(true)
		.PaneBorder(true).Floatable(false)
		.MinSize(860, BrowserToolbar->GetSize().GetHeight())
	);
	SetCurrentBrowser();
}

void t4p::RunBrowserViewClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	//std::map<int, wxString> menuItemIds;
	//menuItemIds[t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 1] = wxT("Run-In Web Browser");
	//AddDynamicCmd(menuItemIds, shortcuts);
}

void t4p::RunBrowserViewClass::OnRunInWebBrowser(wxCommandEvent& event) {
	wxString browserName = Feature.App.Globals.ChosenBrowser;
	wxURI url = Feature.App.Globals.CurrentUrl.Url;
	if (!browserName.IsEmpty() && !url.BuildURI().IsEmpty()) {
		Feature.ExternalBrowser(browserName, url);
	}
}

void t4p::RunBrowserViewClass::OnPreferencesSaved(wxCommandEvent& event) {

	// need to update the browser toolbar if the user updates the environment
	std::vector<wxString> browserNames = Feature.App.Globals.Environment.BrowserNames();

	// for now just make the first item as selected
	Feature.App.Globals.ChosenBrowser = wxT("");
	if (!browserNames.empty()) {
		Feature.App.Globals.ChosenBrowser = browserNames[0];
	}
	if (BrowserToolbar) {
		BrowserToolbar->SetToolLabel(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, Feature.App.Globals.ChosenBrowser);
		std::vector<wxBitmap> browserIcons = BrowserIconsLoad();
		int index = BrowserIconsIndex(browserIcons, Feature.App.Globals.ChosenBrowser);
		if (t4p::NumberLessThan(index, browserIcons.size())) {
			BrowserToolbar->SetToolBitmap(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, browserIcons[index]);
		}
		BrowserToolbar->Realize();
		AuiManager->Update();
	}
}

void t4p::RunBrowserViewClass::OnBrowserToolDropDown(wxAuiToolBarEvent& event) {
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
	std::vector<wxString> browserNames = Feature.App.Globals.Environment.BrowserNames();
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

void t4p::RunBrowserViewClass::OnUrlToolDropDown(wxAuiToolBarEvent& event) {
	if (event.IsDropDownClicked()) {
		if (Feature.RecentUrls.empty()) {
			return;
		}
		wxBitmap recentUrlBitmap = t4p::BitmapImageAsset(wxT("recent-urls"));

		BrowserToolbar->SetToolSticky(event.GetId(), true);

		// create the popup menu that contains all the recent URLs
		if (!UrlMenu.get()) {
			UrlMenu.reset(new wxMenu);
		}
		while (UrlMenu->GetMenuItemCount() > 0) {
			UrlMenu->Delete(UrlMenu->FindItemByPosition(0)->GetId());
		}
		for (size_t i = 0; i < Feature.RecentUrls.size() && i < MAX_URLS; ++i) {
			wxString url = Feature.RecentUrls[i].Url.BuildURI();

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

void t4p::RunBrowserViewClass::OnUrlSearchTool(wxCommandEvent& event) {
	std::vector<wxFileName> sourceDirs = Feature.App.Globals.AllEnabledSourceDirectories();
	if (IsUrlCacheStale) {
		IsWaitingForUrlDetection = true;

		t4p::UrlTagDetectorActionClass* action = new t4p::UrlTagDetectorActionClass(Feature.App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_URL_TAG_DETECTOR);
		std::vector<t4p::GlobalActionClass*> actions;
		actions.push_back(action);
		Feature.App.Sequences.Build(actions);

		if (GaugeDialog) {
			GaugeDialog->Destroy();
			GaugeDialog = NULL;
		}
		GaugeDialog = new t4p::GaugeDialogClass(GetMainWindow(), _("URL Detection"), _("Running URL Detection"));
	}
	else if (Feature.App.Globals.UrlTagFinder.Count(sourceDirs) > 0) {
		ShowUrlDialog();
	}
	else {
		t4p::EditorLogWarningFix(
			_("Could not determine URL routes."),
			_("URL detector is not working. Are you using an unsupported framework?"));
	}
}

void t4p::RunBrowserViewClass::ShowUrlDialog() {
	t4p::ChooseUrlDialogClass dialog(GetMainWindow(), Feature.App.Globals.UrlTagFinder,
		Feature.App.Globals.Projects, Feature.App.Globals.FileTypes, Feature.App.Globals.CurrentUrl);
	if (wxOK == dialog.ShowModal() && !Feature.App.Globals.CurrentUrl.Url.BuildURI().IsEmpty()) {

		// 'select' the URL (make it the current in the toolbar)
		BrowserToolbar->SetToolLabel(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 3, Feature.App.Globals.CurrentUrl.Url.BuildURI());
		BrowserToolbar->Realize();
		AuiManager->Update();

		// add it to the Recent list, or push up to the top of the Recent list if its already there
		bool found = false;
		for (std::vector<t4p::UrlTagClass>::iterator it = Feature.RecentUrls.begin(); it  != Feature.RecentUrls.end(); ++it) {
			if (it->Url == Feature.App.Globals.CurrentUrl.Url) {
				found = true;
				Feature.RecentUrls.erase(it);
				Feature.RecentUrls.insert(Feature.RecentUrls.begin(), Feature.App.Globals.CurrentUrl);
				break;
			}
		}
		if (!found) {
			Feature.RecentUrls.insert(Feature.RecentUrls.begin(), Feature.App.Globals.CurrentUrl);
		}
		wxString browserName = Feature.App.Globals.ChosenBrowser;
		if (!browserName.IsEmpty()) {
			Feature.ExternalBrowser(browserName, Feature.App.Globals.CurrentUrl.Url);
		}
	}
}

void t4p::RunBrowserViewClass::OnBrowserToolMenuItem(wxCommandEvent& event) {

	// detect the chosen browser based on the menu item name
	// change the current selection only if name is found
	// change both the data structure and the toolbar
	if (BrowserMenu.get()) {
		wxString name = BrowserMenu->GetLabelText(event.GetId());
		std::vector<wxString> browserNames = Feature.App.Globals.Environment.BrowserNames();
		std::vector<wxString>::iterator found = std::find(browserNames.begin(), browserNames.end(), name);
		if (found != browserNames.end()) {
			Feature.App.Globals.ChosenBrowser = name;

			std::vector<wxBitmap> browserIcons = BrowserIconsLoad();
			int browserIndex = BrowserIconsIndex(browserIcons, name);
			BrowserToolbar->SetToolLabel(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, name);
			if (t4p::NumberLessThan(browserIndex, browserIcons.size())) {
				BrowserToolbar->SetToolBitmap(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, browserIcons[browserIndex]);
			}
			BrowserToolbar->Realize();
			AuiManager->Update();
		}
	}
}

void t4p::RunBrowserViewClass::OnUrlToolMenuItem(wxCommandEvent& event) {

	// detect the chosen url based on the menu item name
	// change the current selection only if name is found
	// change both the data structure and the toolbar
	if (UrlMenu.get()) {
		wxString name = UrlMenu->GetLabelText(event.GetId());
		t4p::UrlTagClass urlTag;
		bool found = false;
		for (size_t i = 0; i < Feature.RecentUrls.size(); i++) {
			if (Feature.RecentUrls[i].Url.BuildURI() == name) {
				urlTag = Feature.RecentUrls[i];
				found = true;
				break;
			}
		}
		if (found) {
			Feature.App.Globals.CurrentUrl = urlTag;
			BrowserToolbar->SetToolLabel(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 3, name);
			BrowserToolbar->Realize();
			AuiManager->Update();
		}
	}
}

void t4p::RunBrowserViewClass::OnFileSaved(t4p::CodeControlEventClass& event) {
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
	for (project = Feature.App.Globals.Projects.begin(); project != Feature.App.Globals.Projects.end(); ++project) {
		if (project->IsEnabled && project->IsAPhpSourceFile(fileName, Feature.App.Globals.FileTypes)) {
			IsUrlCacheStale = true;
			break;
		}
	}
}

void t4p::RunBrowserViewClass::OnUrlDetectionComplete(t4p::ActionEventClass& event) {
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

BEGIN_EVENT_TABLE(t4p::RunBrowserViewClass, t4p::FeatureViewClass)

	// if the end values of the ranges need to be modified, need to modify t4p::FeatureClass::MenuIds as well
	EVT_MENU_RANGE(t4p::MENU_RUN_BROWSER + 0, t4p::MENU_RUN_BROWSER + MAX_BROWSERS - 1, t4p::RunBrowserViewClass::OnBrowserToolMenuItem)
	EVT_MENU_RANGE(t4p::MENU_RUN_BROWSER + MAX_BROWSERS, t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS - 1, t4p::RunBrowserViewClass::OnUrlToolMenuItem)
	EVT_TOOL(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 1, t4p::RunBrowserViewClass::OnRunInWebBrowser)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, t4p::RunBrowserViewClass::OnBrowserToolDropDown)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 3, t4p::RunBrowserViewClass::OnUrlToolDropDown)
	EVT_TOOL(t4p::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 4, t4p::RunBrowserViewClass::OnUrlSearchTool)

	// application events
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::RunBrowserViewClass::OnPreferencesSaved)
	EVT_APP_FILE_SAVED(t4p::RunBrowserViewClass::OnFileSaved)
	EVT_ACTION_COMPLETE(t4p::ID_EVENT_ACTION_URL_TAG_DETECTOR, t4p::RunBrowserViewClass::OnUrlDetectionComplete)
END_EVENT_TABLE()
