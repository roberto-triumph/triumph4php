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
#include <globals/Errors.h>
#include <MvcEditor.h>
#include <wx/artprov.h>
#include <wx/valgen.h>
#include <wx/aui/aui.h>
#include <algorithm>

static const int ID_URL_GAUGE = wxNewId();
static const size_t MAX_BROWSERS = 10;
static const size_t MAX_URLS = 40;

static void ExternalBrowser(const wxString& browserName, const wxURI& url, mvceditor::EnvironmentClass* environment) {
	wxFileName webBrowserPath;
	bool found = environment->FindBrowserByName(browserName, webBrowserPath);
	if (!found || !webBrowserPath.IsOk()) {
		mvceditor::EditorLogWarning(mvceditor::BAD_WEB_BROWSER_EXECUTABLE, webBrowserPath.GetFullPath());
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
		mvceditor::EditorLogWarning(mvceditor::BAD_WEB_BROWSER_EXECUTABLE, cmd);
	}
}

mvceditor::ChooseUrlDialogClass::ChooseUrlDialogClass(wxWindow* parent, mvceditor::UrlResourceFinderClass& urls, mvceditor::UrlResourceClass& chosenUrl)
	: ChooseUrlDialogGeneratedClass(parent, wxID_ANY)
	, UrlResourceFinder(urls)
	, EditedUrlResourceFinder(urls) 
	, ChosenUrl(chosenUrl) {
	for (size_t i = 0; i < UrlResourceFinder.Urls.size(); ++i) {
		UrlList->Append(UrlResourceFinder.Urls[i].Url.BuildURI());
	}
	if (UrlList->GetCount() > 0) {
		UrlList->Select(0);
	}
	TransferDataToWindow();
	Filter->SetFocus();
}

void mvceditor::ChooseUrlDialogClass::OnOkButton(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow()) {
		UrlResourceFinder.ReplaceAll(EditedUrlResourceFinder);
		int index = UrlList->GetSelection();
		if (index >= 0) {
			wxURI selection(UrlList->GetString(index));
			UrlResourceFinder.FindByUrl(selection, ChosenUrl);
		}
		EndModal(wxOK);
	}
}

void mvceditor::ChooseUrlDialogClass::OnListItemSelected(wxCommandEvent& event) {
	event.Skip();
}

void mvceditor::ChooseUrlDialogClass::OnText(wxCommandEvent& event) {
	wxString filter = Filter->GetValue();
	if (filter.IsEmpty()) {
		
		// empty string =  no filter show all
		UrlList->Clear();
		for (size_t i = 0; i < EditedUrlResourceFinder.Urls.size(); ++i) {
			UrlList->Append(EditedUrlResourceFinder.Urls[i].Url.BuildURI());
		}
		if (UrlList->GetCount() > 0) {
			UrlList->Select(0);
		}
	}
	else {
		std::vector<mvceditor::UrlResourceClass> filteredUrls;
		EditedUrlResourceFinder.FilterUrls(filter, filteredUrls);

		UrlList->Clear();
		for (size_t i = 0; i < filteredUrls.size(); ++i) {
			UrlList->Append(filteredUrls[i].Url.BuildURI());
		}
		if (UrlList->GetCount() > 0) {
			UrlList->Select(0);
		}
	}
}

void mvceditor::ChooseUrlDialogClass::OnDeleteButton(wxCommandEvent& event) {
	int selection = UrlList->GetSelection();
	if (selection >= 0) {

		// delete from the list control AND the data structure
		wxString selectionString = UrlList->GetString(UrlList->GetSelection());
		wxURI uri(selectionString);
		EditedUrlResourceFinder.DeleteUrl(uri);
		
		UrlList->Delete(UrlList->GetSelection());
		if (selection < (int)UrlList->GetCount()) {
			UrlList->Select(selection);
		}
		else if (UrlList->GetCount() > 0) {
			UrlList->Select(UrlList->GetCount() - 1);
		}
	}
	else {
		wxMessageBox(_("Need to select an URL to delete"));
	}
}

void mvceditor::ChooseUrlDialogClass::OnAddButton(wxCommandEvent& event) {
	wxString msg = _("Enter a URL. URL may have query string.\nExample: http://locahost/folder/test.php?val=1");
	wxString url = wxGetTextFromUser(msg, _("Add URL"), wxEmptyString, this);
	if (!url.IsEmpty()) {

		// add to the list control AND the data structure
		wxURI newUri(url);
		bool added = EditedUrlResourceFinder.AddUniqueUrl(newUri);
		if (!added) {
			wxMessageBox(_("Invalid or Duplicate URL"));
		}
		else {
			UrlList->AppendString(url);
			UrlList->Select(UrlList->GetCount() - 1);
		}
	}
}

void mvceditor::ChooseUrlDialogClass::OnCloneButton(wxCommandEvent& event) {
	int selection = UrlList->GetSelection();
	if (selection >= 0) {
		wxString selectionString = UrlList->GetString(UrlList->GetSelection());
		wxString msg = _("Enter a URL. URL may have query string.\nExample: http://locahost/folder/test.php?val=1");
		wxString url = wxGetTextFromUser(msg, _("Add URL"), selectionString, this);
		if (!url.IsEmpty()) {

			// add to the list control AND the data structure
			wxURI newUri(url);
			bool added = EditedUrlResourceFinder.AddUniqueUrl(newUri);
			if (!added) {
				wxMessageBox(_("Invalid or Duplicate URL"));
			}
			else {
				UrlList->AppendString(url);
				UrlList->Select(UrlList->GetCount() - 1);
			}
		}
	}
	else {
		wxMessageBox(_("Need to select an URL to clone"));
	}
}

void mvceditor::ChooseUrlDialogClass::OnTextEnter(wxCommandEvent& event) {
	OnOkButton(event);	
}

void mvceditor::ChooseUrlDialogClass::OnKeyDown(wxKeyEvent& event) {
	if (event.GetKeyCode() == WXK_DOWN && event.GetModifiers() == wxMOD_NONE) {
		int selection = UrlList->GetSelection();
		if (selection >= 0 && selection < (int)(UrlList->GetCount() - 1)) {
			UrlList->SetSelection(selection  + 1);
		}
		Filter->SetFocus();
	}
	else if (event.GetKeyCode() == WXK_UP && event.GetModifiers() == wxMOD_NONE) {
		int selection = UrlList->GetSelection();
		if (selection > 0 && selection < (int)UrlList->GetCount()) {
			UrlList->SetSelection(selection  - 1);
		}
		Filter->SetFocus();
	}
	else {
		event.Skip();
	}
}

mvceditor::RunBrowserFeatureClass::RunBrowserFeatureClass(mvceditor::AppClass& app)
	: FeatureClass(app) 
	, RecentUrls()
	, BrowserMenu()
	, UrlMenu()
	, RunInBrowser(NULL)
	, BrowserToolbar(NULL) {
}

void mvceditor::RunBrowserFeatureClass::AddWindows() {
	BrowserToolbar = new wxAuiToolBar(GetMainWindow(), wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE |
                                         wxAUI_TB_TEXT |
                                         wxAUI_TB_HORZ_TEXT);
    BrowserToolbar->SetToolBitmapSize(wxSize(16,16));
    wxBitmap browserBitmap = wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16));
	BrowserToolbar->AddTool(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, _("No Browsers Configured"), browserBitmap);
    BrowserToolbar->SetToolDropDown(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, true);
	BrowserToolbar->AddSeparator();
    BrowserToolbar->AddTool(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 3, _("No URLs"), browserBitmap);
    BrowserToolbar->SetToolDropDown(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 3, true);
	BrowserToolbar->AddTool(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 1, _("Run In Web Browser"), browserBitmap);
	BrowserToolbar->AddTool(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 4, _("Search for URLs..."), browserBitmap);
	BrowserToolbar->SetOverflowVisible(false);
    BrowserToolbar->Realize();

	// ATTN: not quite sure why wxAuiPaneInfo().ToolbarPane() wont work
	AuiManager->AddPane(BrowserToolbar, wxAuiPaneInfo().Top(
		).CaptionVisible(false).CloseButton(false).Gripper(
		false).DockFixed(true).PaneBorder(false).Floatable(false).Row(1).Position(0));
}	

void mvceditor::RunBrowserFeatureClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	//std::map<int, wxString> menuItemIds;
	//menuItemIds[mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 1] = wxT("Run-In Web Browser");
	//AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::RunBrowserFeatureClass::LoadPreferences(wxConfigBase* config) {
	App.Globals.UrlResourceFinder.Browsers.clear();

	// dont use the config; use the Environment that has already been seeded with 
	// the proper data
	mvceditor::EnvironmentClass* environment = GetEnvironment();
	for (std::vector<mvceditor::WebBrowserClass>::const_iterator it = environment->WebBrowsers.begin(); it != environment->WebBrowsers.end(); ++it) {
		App.Globals.UrlResourceFinder.Browsers.push_back(it->Name);
	}
	if (!App.Globals.UrlResourceFinder.Browsers.empty()) {
		App.Globals.UrlResourceFinder.ChosenBrowser = App.Globals.UrlResourceFinder.Browsers[0];
	}
	if (BrowserToolbar) {
		BrowserToolbar->SetToolLabel(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, App.Globals.UrlResourceFinder.Browsers[0]);
		BrowserToolbar->Realize();
	}
}

void mvceditor::RunBrowserFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {

	// need to update the browser toolbar if the user updates the environment
	App.Globals.UrlResourceFinder.Browsers.clear();
	mvceditor::EnvironmentClass* environment = GetEnvironment();
	for (std::vector<mvceditor::WebBrowserClass>::const_iterator it = environment->WebBrowsers.begin(); it != environment->WebBrowsers.end(); ++it) {
		App.Globals.UrlResourceFinder.Browsers.push_back(it->Name);
	}

	// for now just make the first item as selected
	if (!App.Globals.UrlResourceFinder.Browsers.empty()) {
		App.Globals.UrlResourceFinder.ChosenBrowser = App.Globals.UrlResourceFinder.Browsers[0];
	}
	if (BrowserToolbar) {
		BrowserToolbar->SetToolLabel(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, App.Globals.UrlResourceFinder.Browsers[0]);
		BrowserToolbar->Realize();
		AuiManager->Update();
	}
}

void mvceditor::RunBrowserFeatureClass::OnRunInWebBrowser(wxCommandEvent& event) {
	wxString browserName = App.Globals.UrlResourceFinder.ChosenBrowser;
	wxURI url = App.Globals.UrlResourceFinder.ChosenUrl.Url;
	if (!browserName.IsEmpty() && !url.BuildURI().IsEmpty()) {
		mvceditor::EnvironmentClass* environment = GetEnvironment();
		ExternalBrowser(browserName, url, environment);
	}
}

void mvceditor::RunBrowserFeatureClass::OnBrowserToolDropDown(wxAuiToolBarEvent& event) {
	if (event.IsDropDownClicked()) {
		BrowserToolbar->SetToolSticky(event.GetId(), true);
		mvceditor::EnvironmentClass* environment = GetEnvironment();

		// create the popup menu that contains all the available browser names
		if (!BrowserMenu.get()) {
			BrowserMenu.reset(new wxMenu);
		}
		while (BrowserMenu->GetMenuItemCount() > 0) {
			BrowserMenu->Delete(BrowserMenu->FindItemByPosition(0)->GetId());
		}
		wxBitmap bmp = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_OTHER, wxSize(16,16));
		for (size_t i = 0; i < environment->WebBrowsers.size();  ++i) {
			wxMenuItem* menuItem =  new wxMenuItem(BrowserMenu.get(), mvceditor::MENU_RUN_BROWSER + i, environment->WebBrowsers[i].Name);
			menuItem->SetBitmap(bmp);
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
	else {
		event.Skip();
	}
}

void mvceditor::RunBrowserFeatureClass::OnUrlToolDropDown(wxAuiToolBarEvent& event) {
	if (event.IsDropDownClicked()) {
		if (App.Globals.UrlResourceFinder.Urls.empty()) {
			return;
		}
		BrowserToolbar->SetToolSticky(event.GetId(), true);
		
		// create the popup menu that contains all the recent URLs
		if (!UrlMenu.get()) {
			UrlMenu.reset(new wxMenu);
		}
		while (UrlMenu->GetMenuItemCount() > 0) {
			UrlMenu->Delete(UrlMenu->FindItemByPosition(0)->GetId());
		}

		wxBitmap bmp = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_OTHER, wxSize(16,16));
		for (size_t i = 0; i < RecentUrls.size() && i < MAX_URLS; ++i) {
			wxMenuItem* menuItem =  new wxMenuItem(UrlMenu.get(), mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + i, RecentUrls[i].Url.BuildURI());
			menuItem->SetBitmap(bmp);
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
	if (!App.Globals.UrlResourceFinder.Urls.empty()) {
		ShowUrlDialog();
	}
	else {
		
		mvceditor::EditorLogWarning(mvceditor::WARNING_OTHER,
			_("Could not determine URL routes."));
	}
}

void mvceditor::RunBrowserFeatureClass::ShowUrlDialog() {
	mvceditor::ChooseUrlDialogClass dialog(GetMainWindow(), App.Globals.UrlResourceFinder, App.Globals.UrlResourceFinder.ChosenUrl);
	if (wxOK == dialog.ShowModal() && !App.Globals.UrlResourceFinder.ChosenUrl.Url.BuildURI().IsEmpty()) {
				
		// 'select' the URL (make it the current in the toolbar)
		BrowserToolbar->SetToolLabel(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 3, App.Globals.UrlResourceFinder.ChosenUrl.Url.BuildURI());
		BrowserToolbar->Realize();
		AuiManager->Update();

		// add it to the Recent list, or push up to the top of the Recent list if its already there
		bool found = false;
		for (std::vector<mvceditor::UrlResourceClass>::iterator it = RecentUrls.begin(); it  != RecentUrls.end(); ++it) {
			if (it->Url == App.Globals.UrlResourceFinder.ChosenUrl.Url) {
				found = true;
				RecentUrls.erase(it);
				RecentUrls.insert(RecentUrls.begin(), App.Globals.UrlResourceFinder.ChosenUrl);
				break;
			}
		}
		if (!found) {
			RecentUrls.insert(RecentUrls.begin(), App.Globals.UrlResourceFinder.ChosenUrl);
		}
		mvceditor::EnvironmentClass* environment = GetEnvironment();
		wxString browserName = App.Globals.UrlResourceFinder.ChosenBrowser;
		if (!browserName.IsEmpty()) {
			ExternalBrowser(browserName, App.Globals.UrlResourceFinder.ChosenUrl.Url, environment);
		}
	}
}

void mvceditor::RunBrowserFeatureClass::OnBrowserToolMenuItem(wxCommandEvent& event) {

	// detect the chosen browser based on the menu item name
	// change the current selection only if name is found
	// change both the data structure and the toolbar
	if (BrowserMenu.get()) {
		wxString name = BrowserMenu->GetLabelText(event.GetId());
		std::vector<wxString>::iterator found = std::find(App.Globals.UrlResourceFinder.Browsers.begin(), App.Globals.UrlResourceFinder.Browsers.end(), name);
		if (found != App.Globals.UrlResourceFinder.Browsers.end()) {
			App.Globals.UrlResourceFinder.ChosenBrowser = name;
			BrowserToolbar->SetToolLabel(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, name);
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
		mvceditor::UrlResourceClass urlResource;
		bool found = App.Globals.UrlResourceFinder.FindByUrl(name, urlResource);
		if (found) {
			App.Globals.UrlResourceFinder.ChosenUrl = urlResource;
			BrowserToolbar->SetToolLabel(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 3, name);
			BrowserToolbar->Realize();
			AuiManager->Update();
		}
	}
}

void mvceditor::RunBrowserFeatureClass::OnProcessInProgress(wxCommandEvent& event) {
	GetStatusBarWithGauge()->UpdateGauge(ID_URL_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE);
}

void mvceditor::RunBrowserFeatureClass::OnUrlResourceActionComplete(wxCommandEvent& event) {
	RecentUrls.clear();
	if (BrowserToolbar) {
		BrowserToolbar->SetToolLabel(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 3, _("No URLs"));
		BrowserToolbar->Realize();
		AuiManager->Update();
	}
	if (UrlMenu.get()) {
		while (UrlMenu->GetMenuItemCount() > 0) {
			UrlMenu->Delete(UrlMenu->FindItemByPosition(0)->GetId());
		}
	}
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
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_URL_RESOURCES, mvceditor::EVENT_WORK_COMPLETE, mvceditor::RunBrowserFeatureClass::OnUrlResourceActionComplete)
	
END_EVENT_TABLE()