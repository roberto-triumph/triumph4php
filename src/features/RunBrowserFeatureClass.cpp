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
#include <actions/UrlDetectorActionClass.h>
#include <globals/Errors.h>
#include <globals/Assets.h>
#include <MvcEditor.h>
#include <wx/artprov.h>
#include <wx/valgen.h>
#include <wx/aui/aui.h>
#include <algorithm>

static const int ID_URL_GAUGE = wxNewId();
static const size_t MAX_BROWSERS = 10;
static const size_t MAX_URLS = 40;
static const int ID_URL_DETECTOR_PANEL = wxNewId();

static const int ID_URL_DETECTOR_TREE_OPEN = wxNewId();
static const int ID_URL_DETECTOR_TREE_RENAME = wxNewId();
static const int ID_URL_DETECTOR_TREE_DELETE = wxNewId();

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

mvceditor::ChooseUrlDialogClass::ChooseUrlDialogClass(wxWindow* parent, mvceditor::UrlResourceFinderClass* urls, mvceditor::UrlResourceClass& chosenUrl)
	: ChooseUrlDialogGeneratedClass(parent, wxID_ANY)
	, UrlResourceFinder(urls)
	, ChosenUrl(chosenUrl) {

	// get some urls to prepopulate the list
	std::vector<mvceditor::UrlResourceClass> allUrlResources;
	urls->FilterUrls(wxT("http://"), allUrlResources);
	for (size_t i = 0; i < allUrlResources.size(); ++i) {
		UrlList->Append(allUrlResources[i].Url.BuildURI());
	}
	if (UrlList->GetCount() > 0) {
		UrlList->Select(0);
	}
	TransferDataToWindow();
	Filter->SetFocus();
}

void mvceditor::ChooseUrlDialogClass::OnOkButton(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow()) {
		int index = UrlList->GetSelection();
		if (index >= 0) {
			wxURI selection(UrlList->GetString(index));
			UrlResourceFinder->FindByUrl(selection, ChosenUrl);
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
		std::vector<mvceditor::UrlResourceClass> allUrlResources;
		UrlResourceFinder->FilterUrls(wxT("http://"), allUrlResources);
		for (size_t i = 0; i < allUrlResources.size(); ++i) {
			UrlList->Append(allUrlResources[i].Url.BuildURI());
		}
		if (UrlList->GetCount() > 0) {
			UrlList->Select(0);
		}
	}
	else {
		std::vector<mvceditor::UrlResourceClass> filteredUrls;
		UrlResourceFinder->FilterUrls(filter, filteredUrls);

		UrlList->Clear();
		for (size_t i = 0; i < filteredUrls.size(); ++i) {
			UrlList->Append(filteredUrls[i].Url.BuildURI());
		}
		if (UrlList->GetCount() > 0) {
			UrlList->Select(0);
		}
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

/**
 * small class to hold the full path to the detector script on each
 * tree item
 */
class TreeItemDataStringClass : public wxTreeItemData {

public:

	wxString FullPath;

	TreeItemDataStringClass(wxString fullPath);

};

TreeItemDataStringClass::TreeItemDataStringClass(wxString fullPath) 
	: wxTreeItemData() 
	, FullPath(fullPath) {
	
}

mvceditor::UrlDetectorPanelClass::UrlDetectorPanelClass(wxWindow* parent, int id, mvceditor::GlobalsClass& globals,
														mvceditor::EventSinkClass& eventSink)
	: UrlDetectorPanelGeneratedClass(parent, id) 
	, Globals(globals) 
	, EventSink(eventSink) {
	HelpButton->SetBitmapLabel((wxArtProvider::GetBitmap(wxART_HELP, 
		wxART_TOOLBAR, wxSize(16, 16))));
}

void mvceditor::UrlDetectorPanelClass::Init() {
	wxString globalRootDir = mvceditor::UrlDetectorsGlobalAsset().GetPath();
	wxString localRootDir = mvceditor::UrlDetectorsLocalAsset().GetPath();
	
	UrlDetectorTree->Freeze();
	UrlDetectorTree->DeleteAllItems();
	UrlDetectorTree->AddRoot(_("URL Detectors"));
	wxTreeItemId globalItemId = UrlDetectorTree->AppendItem(UrlDetectorTree->GetRootItem(), _("Global"));
	wxTreeItemId localItemId = UrlDetectorTree->AppendItem(UrlDetectorTree->GetRootItem(), _("Local"));
	
	FillSubTree(globalRootDir, globalItemId);
	FillSubTree(localRootDir, localItemId);

	UrlDetectorTree->ExpandAllChildren(UrlDetectorTree->GetRootItem());
	UrlDetectorTree->Thaw();
}

void mvceditor::UrlDetectorPanelClass::UpdateProjects() {
	wxArrayString projectLabels;
	std::vector<mvceditor::ProjectClass>::const_iterator project;
	for (project = Globals.Projects.begin(); project != Globals.Projects.end(); ++project) {
		projectLabels.Add(project->Label);
	}
	ProjectChoice->Clear();
	if (!projectLabels.IsEmpty()) {
		ProjectChoice->Append(projectLabels);
		ProjectChoice->Select(0);
	}
}

void mvceditor::UrlDetectorPanelClass::FillSubTree(const wxString& detectorRootDir, wxTreeItemId treeItemDir) {
	wxDir dir;
	if (dir.Open(detectorRootDir)) {
		wxString fileName;
		if (dir.GetFirst(&fileName, wxEmptyString, wxDIR_DIRS)) {
			do {
				wxTreeItemId subRoot = UrlDetectorTree->AppendItem(treeItemDir, fileName);
				FillSubTree(detectorRootDir + wxFileName::GetPathSeparator() + fileName, subRoot);
			} while (dir.GetNext(&fileName));
		}
		if (dir.GetFirst(&fileName, wxEmptyString, wxDIR_FILES)) {
			do {
				wxFileName fullPath(detectorRootDir, fileName);
				TreeItemDataStringClass* treeItemData = new TreeItemDataStringClass(fullPath.GetFullPath());
				UrlDetectorTree->AppendItem(treeItemDir, fileName, -1, -1, treeItemData);
			} while (dir.GetNext(&fileName));

		}
	}
}

void mvceditor::UrlDetectorPanelClass::OnTreeItemActivated(wxTreeEvent& event) {
	wxTreeItemId id = event.GetItem();
	TreeItemDataStringClass* treeItemData = (TreeItemDataStringClass*)UrlDetectorTree->GetItemData(id);
	if (treeItemData) {
		wxCommandEvent openEvent(mvceditor::EVENT_CMD_FILE_OPEN);
		openEvent.SetString(treeItemData->FullPath);
		EventSink.Publish(openEvent);
	}
}

void mvceditor::UrlDetectorPanelClass::OnHelpButton(wxCommandEvent& event) {
	wxString help = wxString::FromAscii(
		"URL Detectors are PHP scripts that MVC Editor uses to find out "
		"all of the valid URL routes for your projects.  \n"
		"MVC Editor can detect routes for CodeIgniter projects.\n"
	);
	help = wxGetTranslation(help);
	wxMessageBox(help, _("URL Detectors Help"), wxOK, this);
}

void mvceditor::UrlDetectorPanelClass::OnAddButton(wxCommandEvent& event) {
	wxString name = ::wxGetTextFromUser(_("Please enter a file name (no extension)"), _("Create a URL Detector"), wxEmptyString, 
		// grandParent == outline notebook's parent (the frame)
		this->GetGrandParent());
	wxString forbidden = wxFileName::GetForbiddenChars();
	if (name.find_first_of(forbidden, 0) != std::string::npos) {
		wxMessageBox(_("Please enter valid a file name"));
		return;
	}
	if (!name.EndsWith(wxT(".php"))) {
		name += wxT(".php");
	}
	wxFileName localDetectorScript(mvceditor::UrlDetectorsLocalAsset().GetPath(), name);
	if (localDetectorScript.FileExists()) {
		wxMessageBox(_("File name already exists. Please enter another name."));
		return;
	}

	wxFile file;
	if (file.Create(localDetectorScript.GetFullPath())) {
		
		// populate the file with some starter code to guide the user
		wxFileName urlDetectorSkeleton = mvceditor::SkeletonsBaseAsset();
		urlDetectorSkeleton.Assign(urlDetectorSkeleton.GetPath(), wxT("UrlDetector.skeleton.php"));
		wxFFile skeletonFile(urlDetectorSkeleton.GetFullPath().fn_str(), wxT("r"));
		wxASSERT_MSG(skeletonFile.IsOpened(), _("UrlDetector.skeleton.php is not found."));
		wxString contents;
		
		skeletonFile.ReadAll(&contents);
		file.Write(contents);

		// close file so that it can be opened later on by the app
		file.Close();
		wxCommandEvent openEvent(mvceditor::EVENT_CMD_FILE_OPEN);
		openEvent.SetString(localDetectorScript.GetFullPath());
		EventSink.Publish(openEvent);

		// add the tree node also
		TreeItemDataStringClass* treeItemData = new TreeItemDataStringClass(localDetectorScript.GetFullPath());
		wxTreeItemId localLabelTreeItemId = UrlDetectorTree->GetLastChild(UrlDetectorTree->GetRootItem());
		UrlDetectorTree->AppendItem(localLabelTreeItemId, name, -1,-1, treeItemData);
		UrlDetectorTree->SortChildren(localLabelTreeItemId);
		UrlDetectorTree->ExpandAllChildren(localLabelTreeItemId);
	}
	else {
		wxMessageBox(_("File could not be created:") + localDetectorScript.GetFullPath());
	}
}

void mvceditor::UrlDetectorPanelClass::OnTestButton(wxCommandEvent& event) {
	
	// create the command to test the selected detector on the selected
	// project
	int projectChoiceIndex = ProjectChoice->GetSelection();
	if (ProjectChoice->IsEmpty() || projectChoiceIndex >= (int)Globals.Projects.size()) {
		wxMessageBox(_("Please choose a project to test the URL detector on."));
		return;
	}

	// make sure that a child item in the tree is selected
	wxTreeItemId itemId = UrlDetectorTree->GetSelection();
	if (!itemId.IsOk() || UrlDetectorTree->GetRootItem() == itemId) {
		wxMessageBox(_("Please choose a URL detector to test."));
		return;
	}
	mvceditor::ProjectClass project = Globals.Projects[projectChoiceIndex];
	if (project.Sources.empty()) {
		wxMessageBox(_("Selected project does not have any source directories. Please choose another project"));
		return;
	}	
	mvceditor::SourceClass source = project.Sources[0];
	wxString rootUrl = Globals.Environment.Apache.GetUrl(source.RootDirectory.GetPath());
	if (rootUrl.IsEmpty()) {
		wxMessageBox(_("Cannot determine the root URL selected project. Please add a virtual host mapping for ") + source.RootDirectory.GetPath() +
			_(" under Edit -> Preferences -> Apache"));
		return;
	}
	TreeItemDataStringClass* treeItemData = (TreeItemDataStringClass*)UrlDetectorTree->GetItemData(itemId);
	if (!treeItemData) {
		wxMessageBox(_("Please choose a URL detector to test."));
		return;
	}
	
	wxString detectorScriptFullPath = treeItemData->FullPath;

	// leave OutputDbFileName empty; that way the results are output to
	// STDOUT
	// that way the user can easily see what URLs would be detected
	mvceditor::UrlDetectorParamsClass params;
	params.PhpExecutablePath = Globals.Environment.Php.PhpExecutablePath;
	params.PhpIncludePath = mvceditor::PhpDetectorsBaseAsset();
	params.ScriptName = detectorScriptFullPath;
	params.SourceDir = source.RootDirectory;
	params.ResourceDbFileName = project.ResourceDbFileName;
	params.RootUrl = rootUrl;

	wxString cmdLine = params.BuildCmdLine();

	// send the command line to a new app command event to start a process
	wxCommandEvent runEvent(mvceditor::EVENT_CMD_RUN_COMMAND);
	runEvent.SetString(cmdLine);
	EventSink.Publish(runEvent);
}

void mvceditor::UrlDetectorPanelClass::OnTreeItemRightClick(wxTreeEvent& event) {
	wxTreeItemId treeItemId = event.GetItem();
	TreeItemDataStringClass* data = (TreeItemDataStringClass*)UrlDetectorTree->GetItemData(treeItemId);
	if (data) {
		UrlDetectorTree->SelectItem(treeItemId, true);

		// only show the menu on the filenames not the "local" or "global" labels
		wxTreeItemId i = treeItemId;
		bool isLocalDetector = false;
		while (i != UrlDetectorTree->GetRootItem()) {
			wxString label = UrlDetectorTree->GetItemText(i);
			if (_("Local") == label) {
				isLocalDetector = true;
				break;
			}
			i = UrlDetectorTree->GetItemParent(i);
		}

		wxMenu menu;
		menu.Append(ID_URL_DETECTOR_TREE_OPEN, _("Open"), _("Open the selected URL Detector"));	
		menu.Append(ID_URL_DETECTOR_TREE_RENAME, _("Rename"), _("Rename the selected URL Detector"));
		menu.Append(ID_URL_DETECTOR_TREE_DELETE, _("Delete"), _("Delete the selected URL Detector"));
	
		// only show the destructive menu items on the "local" detectors
		menu.Enable(ID_URL_DETECTOR_TREE_RENAME, isLocalDetector);
		menu.Enable(ID_URL_DETECTOR_TREE_DELETE, isLocalDetector);

		wxPoint pos = event.GetPoint();
		PopupMenu(&menu, pos);
	}
	event.Skip();
}

void mvceditor::UrlDetectorPanelClass::OnTreeItemDelete(wxTreeEvent& event) {
	wxTreeItemId treeItemId = event.GetItem();
	TreeItemDataStringClass* data = (TreeItemDataStringClass*)UrlDetectorTree->GetItemData(treeItemId);
	if (data) {
		wxRemoveFile(data->FullPath);
	}
	event.Skip();
}

void mvceditor::UrlDetectorPanelClass::OnMenuOpenDetector(wxCommandEvent& event) {
	wxTreeItemId id = UrlDetectorTree->GetSelection();
	TreeItemDataStringClass* treeItemData = (TreeItemDataStringClass*)UrlDetectorTree->GetItemData(id);
	if (treeItemData) {
		wxCommandEvent openEvent(mvceditor::EVENT_CMD_FILE_OPEN);
		openEvent.SetString(treeItemData->FullPath);
		EventSink.Publish(openEvent);
	}
}

void mvceditor::UrlDetectorPanelClass::OnMenuRenameDetector(wxCommandEvent& event) {
	wxTreeItemId id = UrlDetectorTree->GetSelection();
	TreeItemDataStringClass* treeItemData = (TreeItemDataStringClass*)UrlDetectorTree->GetItemData(id);
	if (treeItemData) {
		UrlDetectorTree->EditLabel(id);	
	}
}

void mvceditor::UrlDetectorPanelClass::OnMenuDeleteDetector(wxCommandEvent& event) {
	wxTreeItemId treeItemId = UrlDetectorTree->GetSelection();
	TreeItemDataStringClass* data = (TreeItemDataStringClass*)UrlDetectorTree->GetItemData(treeItemId);
	if (data) {
		int res = wxMessageBox(_("Delete this detector? The operation cannot be undone.\n") + data->FullPath, _("Delete URL detector"), wxYES_NO, 
			this->GetGrandParent());
		if (wxYES == res) {
			UrlDetectorTree->Delete(treeItemId);
		}
	}
}

void mvceditor::UrlDetectorPanelClass::OnTreeItemEndLabelEdit(wxTreeEvent& event) {
	wxTreeItemId treeItemId = event.GetItem();
	wxString newName = event.GetLabel();
	if (newName.find_first_of(wxFileName::GetForbiddenChars(), 0) != std::string::npos) {
		wxMessageBox(_("Filename contains invalid characters."));
		event.Veto();
	}
	else {

		// rename the file and set the tree item data; the label itself will be set 
		// by the next event handler
		TreeItemDataStringClass* data = (TreeItemDataStringClass*)UrlDetectorTree->GetItemData(treeItemId);
		wxASSERT(data);
		wxFileName oldFileName(data->FullPath);
		wxFileName newFileName(oldFileName.GetPath(), newName);
		if (!newFileName.FileExists()) {
			data->FullPath = newFileName.GetFullPath();
			wxRenameFile(oldFileName.GetFullPath(), newFileName.GetFullPath());
			event.Skip();
		}
		else {
			wxMessageBox(_("File name already exists. Please choose a different name."));
			event.Veto();
		}
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

void mvceditor::RunBrowserFeatureClass::AddNewMenu(wxMenuBar* menuBar) {
	wxMenu* menu = new wxMenu(0);
	menu->Append(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 5, _("View URL Detectors"), _("View the URL Detectors"), wxITEM_NORMAL);
	menu->Append(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 6, _("Run URL Detection"), _("Run the URL Detectors against the current projects"), wxITEM_NORMAL);
	menuBar->Append(menu, _("Detectors"));
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
		std::vector<wxString> browserNames = App.Globals.Environment.BrowserNames();
		for (size_t i = 0; i < browserNames.size();  ++i) {
			wxMenuItem* menuItem =  new wxMenuItem(BrowserMenu.get(), mvceditor::MENU_RUN_BROWSER + i, browserNames[i]);
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
		if (RecentUrls.empty()) {
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
	if (App.Globals.UrlResourceFinder.Count() > 0) {
		ShowUrlDialog();
	}
	else {
		mvceditor::EditorLogWarning(mvceditor::WARNING_OTHER,
			_("Could not determine URL routes."));
	}
}

void mvceditor::RunBrowserFeatureClass::ShowUrlDialog() {
	mvceditor::ChooseUrlDialogClass dialog(GetMainWindow(), &App.Globals.UrlResourceFinder, App.Globals.CurrentUrl);
	if (wxOK == dialog.ShowModal() && !App.Globals.CurrentUrl.Url.BuildURI().IsEmpty()) {
				
		// 'select' the URL (make it the current in the toolbar)
		BrowserToolbar->SetToolLabel(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 3, App.Globals.CurrentUrl.Url.BuildURI());
		BrowserToolbar->Realize();
		AuiManager->Update();

		// add it to the Recent list, or push up to the top of the Recent list if its already there
		bool found = false;
		for (std::vector<mvceditor::UrlResourceClass>::iterator it = RecentUrls.begin(); it  != RecentUrls.end(); ++it) {
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
			BrowserToolbar->SetToolLabel(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 2, name);
			BrowserToolbar->Realize();
			AuiManager->Update();
		}
	}
}

void mvceditor::RunBrowserFeatureClass::OnViewUrlDetectors(wxCommandEvent& event) {
	wxWindow* window = FindOutlineWindow(ID_URL_DETECTOR_PANEL);
	if (window) {
		SetFocusToOutlineWindow(window);
	}
	else {
		mvceditor::UrlDetectorPanelClass* panel = new mvceditor::UrlDetectorPanelClass(GetOutlineNotebook(), ID_URL_DETECTOR_PANEL, 
			App.Globals, App.EventSink);
		if (AddOutlineWindow(panel, _("URL Detectors"))) {
			panel->Init();
			panel->UpdateProjects();
		}
	}
}

void mvceditor::RunBrowserFeatureClass::OnRunUrlDetectors(wxCommandEvent& event) {
	if (App.Sequences.Running()) {
		wxMessageBox(_("Please wait for the current background task to finish"));
		return;
	}
	std::vector<mvceditor::ActionClass*> actions;

	// the sequence class will own this pointer
	actions.push_back(
		new mvceditor::UrlDetectorActionClass(App.RunningThreads, mvceditor::ID_EVENT_ACTION_URL_DETECTOR)	
	);
	App.Sequences.Build(actions);
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
			App.Globals.CurrentUrl = urlResource;
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
	EVT_COMMAND(mvceditor::ID_EVENT_ACTION_URL_DETECTOR, mvceditor::EVENT_WORK_COMPLETE, mvceditor::RunBrowserFeatureClass::OnUrlResourceActionComplete)

	EVT_MENU(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 5, mvceditor::RunBrowserFeatureClass::OnViewUrlDetectors)
	EVT_MENU(mvceditor::MENU_RUN_BROWSER + MAX_BROWSERS + MAX_URLS + 6, mvceditor::RunBrowserFeatureClass::OnRunUrlDetectors)
	
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::UrlDetectorPanelClass, UrlDetectorPanelGeneratedClass)
	EVT_TREE_ITEM_ACTIVATED(wxID_ANY, mvceditor::UrlDetectorPanelClass::OnTreeItemActivated)
	EVT_MENU(ID_URL_DETECTOR_TREE_OPEN, mvceditor::UrlDetectorPanelClass::OnMenuOpenDetector)
	EVT_MENU(ID_URL_DETECTOR_TREE_RENAME, mvceditor::UrlDetectorPanelClass::OnMenuRenameDetector)
	EVT_MENU(ID_URL_DETECTOR_TREE_DELETE, mvceditor::UrlDetectorPanelClass::OnMenuDeleteDetector)
END_EVENT_TABLE()