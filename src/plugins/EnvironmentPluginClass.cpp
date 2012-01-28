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
#include <wx/valgen.h>

/**
 * A helper function to add a row into a list control. list control must have 2 columns
 * 
 * @param list the list to add to
 * @param column1Value the value for column 1 
 * @param column2Value the value for column 2
 */
static void ListCtrlAdd(wxListCtrl* list, const wxString& column1Value, const wxString& column2Value) {
	int newRowNumber = list->GetItemCount();
	
	// list ctrl is tricky, for columns we must insertItem() then setItem() for the next columns
	wxListItem column1;
	column1.SetColumn(0);
	column1.SetId(newRowNumber);
	column1.SetText(column1Value);
	list->InsertItem(column1);
	
	wxListItem column2;
	column2.SetId(newRowNumber);
	column2.SetColumn(1);
	column2.SetText(column2Value);
	list->SetItem(column2);
}

/**
 * A helper function to edit a list control row's contents
 * 
 * @param list the list to add to
 * @param column1Value the new value for column 1 
 * @param column2Value the new value for column 2
 * @param rowIndex 0-based into of row to change
 */
static void ListCtrlEdit(wxListCtrl* list, const wxString& column1Value, const wxString& column2Value, int rowIndex) {
	wxListItem column1;
	column1.SetColumn(0);
	column1.SetId(rowIndex);
	column1.SetText(column1Value);
	list->SetItem(column1);
	
	wxListItem column2;
	column2.SetId(rowIndex);
	column2.SetColumn(1);
	column2.SetText(column2Value);
	list->SetItem(column2);
}

/**
 * extract the column values from the given list control
 * @param list the list to extract from
 * @param column1Value will be filled with the contents of (rowIndex, 0)
 * @param column2Value will be filled with the contents of (rowIndex, 1)
 * @param rowIndex the row to get
 */
static void ListCtrlGet(wxListCtrl* list, wxString& column1Value, wxString& column2Value, int rowIndex) {
	wxListItem column1,
		column2;
	column1.SetColumn(0);
	column1.SetId(rowIndex);
	column2.SetColumn(1);
	column2.SetId(rowIndex);
	if (list->GetItem(column1) && list->GetItem(column2)) {
		column1Value = column1.GetText();
		column2Value = column2.GetText();
	}
}


mvceditor::ApacheFileReaderClass::ApacheFileReaderClass(wxEvtHandler& handler)
	: BackgroundFileReaderClass(handler)
	, ApacheResults() {
}

bool mvceditor::ApacheFileReaderClass::Init(const wxString& startDirectory) {
	ApacheResults.ManualConfiguration = false;
	ApacheResults.ClearMappings();
	return BackgroundFileReaderClass::Init(startDirectory);
}

mvceditor::ApacheClass mvceditor::ApacheFileReaderClass::Results() const {
	return ApacheResults;
}

bool mvceditor::ApacheFileReaderClass::FileMatch(const wxString& file) {
	return true;
}

bool mvceditor::ApacheFileReaderClass::FileRead(mvceditor::DirectorySearchClass& search) {
	bool ret = false;
	if (search.Walk(ApacheResults)) {
		ret = true;
	}
	if (!search.More()) {
		
		// when we are done recursing, parse the matched files
		std::vector<wxString> possibleConfigFiles = search.GetMatchedFiles();
		
		// there may be multiple files, at this point just print out the number of defined virtual hosts
		for (size_t i = 0; i <  possibleConfigFiles.size(); ++i) {
			
			// as soon as we find one exit
			if (ApacheResults.SetHttpdPath(possibleConfigFiles[i])) {
				break;
			}
		}
	}
	return ret;
}

mvceditor::ApacheEnvironmentPanelClass::ApacheEnvironmentPanelClass(wxWindow* parent, EnvironmentClass& environment)
	: ApacheEnvironmentPanelGeneratedClass(parent)
	, Environment(environment)
	, ApacheFileReader(*this)
	, EditedApache() {
	EditedApache = environment.Apache;
	wxGenericValidator manualValidator(&EditedApache.ManualConfiguration);	
	Manual->SetValidator(manualValidator);
	
	VirtualHostList->ClearAll();
	VirtualHostList->InsertColumn(0, _("Root Directory"));
	VirtualHostList->InsertColumn(1, _("Host Name"));
	
	// in case there are no virtual hosts still properly show the columns 
	VirtualHostList->SetColumnWidth(0, 250);
	VirtualHostList->SetColumnWidth(1, 250);
	Populate();
}

void mvceditor::ApacheEnvironmentPanelClass::OnScanButton(wxCommandEvent& event) {
	if (!ApacheFileReader.IsRunning()) {
		wxString path = ApacheConfigurationDirectory->GetPath();
		wxChar ch = wxFileName::GetPathSeparator();
		if (path[path.Length() - 1] != ch) {
			path.Append(ch);
		}
		mvceditor::BackgroundFileReaderClass::StartError error = mvceditor::BackgroundFileReaderClass::NONE;
		if (ApacheFileReader.Init(path) && ApacheFileReader.StartReading(error)) {
			VirtualHostList->DeleteAllItems();
			ScanButton->SetLabel(_("Stop Scan"));
			Gauge->Show();
		}
		else {
			switch (error) {
			case mvceditor::BackgroundFileReaderClass::ALREADY_RUNNING:
				wxMessageBox(_("Scanner is already running"), _("Configuration Not Found"), wxOK | wxCENTRE, this);
				break;
			case mvceditor::BackgroundFileReaderClass::NO_RESOURCES:
				wxMessageBox(_("System is low on resources.  Try again later."), _("Configuration Not Found"), wxOK | wxCENTRE, this);
				break;
			default:
				wxMessageBox(_("Path not valid"), _("Configuration Not Found"), wxOK | wxCENTRE, this);
				break;
			}
		}
	}
	else {
		
		// act like a stop button
		Gauge->SetValue(0);
		ApacheFileReader.StopReading();
		ScanButton->SetLabel(_("Scan For Configuration"));
	}
}

void mvceditor::ApacheEnvironmentPanelClass::Populate() {
	wxString configFile = EditedApache.GetHttpdPath();
	wxString results;

	// configFile is a full file name, extract the path
	wxFileName fileName(configFile);
	ApacheConfigurationDirectory->SetPath(fileName.GetPath(true));
	std::map<wxString, wxString> mappings = EditedApache.GetVirtualHostMappings();
	std::map<wxString, wxString>::const_iterator it;
	for (it = mappings.begin(); it != mappings.end(); ++it) {
		ListCtrlAdd(VirtualHostList, it->first, it->second);
	}
	VirtualHostList->SetColumnWidth(0, wxLIST_AUTOSIZE);
	VirtualHostList->SetColumnWidth(1, wxLIST_AUTOSIZE);
}

void mvceditor::ApacheEnvironmentPanelClass::OnWorkComplete(wxCommandEvent& event) {
	Gauge->SetValue(0);
	ScanButton->SetLabel(_("Scan For Configuration"));
		
	mvceditor::ApacheClass newSettings = ApacheFileReader.Results();
	
	// setting the HTTPD path will reparse the file ... for now
	// this is OK
	EditedApache.SetHttpdPath(newSettings.GetHttpdPath());
	Populate();
}

void mvceditor::ApacheEnvironmentPanelClass::OnWorkInProgress(wxCommandEvent& event) {
	Gauge->Pulse();
}

void mvceditor::ApacheEnvironmentPanelClass::OnResize(wxSizeEvent& event) {
	if (GetContainingSizer() && HelpText->GetContainingSizer()) {
		HelpText->Wrap(event.GetSize().GetX());
		HelpText->GetContainingSizer()->Layout();
		GetContainingSizer()->Layout();
		Refresh();
	}
	event.Skip();
}

void mvceditor::ApacheEnvironmentPanelClass::OnAddButton(wxCommandEvent& event) {
	wxFileName rootDirectory;
	wxString hostName; 
	mvceditor::VirtualHostCreateDialogClass dialog(this, EditedApache.GetVirtualHostMappings(), hostName, rootDirectory);
	if (wxOK == dialog.ShowModal()) {
		ListCtrlAdd(VirtualHostList, rootDirectory.GetFullPath(), hostName);
		VirtualHostList->SetColumnWidth(0, wxLIST_AUTOSIZE);
		VirtualHostList->SetColumnWidth(1, wxLIST_AUTOSIZE);
		
		EditedApache.SetVirtualHostMapping(rootDirectory.GetFullPath(), hostName);
	}
}

void mvceditor::ApacheEnvironmentPanelClass::OnUpdateUi(wxUpdateUIEvent& event) {
	bool enableButtons = Manual->GetValue();
	RemoveButton->Enable(enableButtons);
	EditButton->Enable(enableButtons);
	AddButton->Enable(enableButtons);
	
	// scan button is only for automatic configuration mode
	ScanButton->Enable(!enableButtons);
	ApacheConfigurationDirectory->Enable(!enableButtons);
	event.Skip();
}

void mvceditor::ApacheEnvironmentPanelClass::OnEditButton(wxCommandEvent& event) {
	int selected = VirtualHostList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selected >= 0) {
		wxString oldRootDirectory,
			oldHostName;
		ListCtrlGet(VirtualHostList, oldRootDirectory, oldHostName, selected);
		wxString newHostName = oldHostName;
		wxFileName newRootDirectory(oldRootDirectory);
		mvceditor::VirtualHostCreateDialogClass dialog(this, EditedApache.GetVirtualHostMappings(), newHostName, newRootDirectory);
		if (wxOK == dialog.ShowModal()) {
			ListCtrlEdit(VirtualHostList, newRootDirectory.GetFullPath(), newHostName, selected);
			VirtualHostList->SetColumnWidth(0, wxLIST_AUTOSIZE);
			VirtualHostList->SetColumnWidth(1, wxLIST_AUTOSIZE);
		
			// in case the root directory changes
			EditedApache.RemoveVirtualHostMapping(oldRootDirectory);
			EditedApache.SetVirtualHostMapping(newRootDirectory.GetFullPath(), newHostName);
		}
	}
}

void mvceditor::ApacheEnvironmentPanelClass::OnRemoveButton(wxCommandEvent& event) {
	int selected = VirtualHostList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selected >= 0) {
		wxString oldRootDirectory,
			oldHostName;
		ListCtrlGet(VirtualHostList, oldRootDirectory, oldHostName, selected);
		VirtualHostList->DeleteItem(selected);		
		EditedApache.RemoveVirtualHostMapping(oldRootDirectory);
	}
}

void mvceditor::ApacheEnvironmentPanelClass::Apply() {
	Environment.Apache = EditedApache;
}

void mvceditor::ApacheEnvironmentPanelClass::OnDirChanged(wxFileDirPickerEvent& event) {
	if (!ApacheFileReader.IsRunning()) {
		wxString path = ApacheConfigurationDirectory->GetPath();
		wxChar ch = wxFileName::GetPathSeparator();
		if (path[path.Length() - 1] != ch) {
			path.Append(ch);
		}
		mvceditor::BackgroundFileReaderClass::StartError error = mvceditor::BackgroundFileReaderClass::NONE;
		if (ApacheFileReader.Init(path) && ApacheFileReader.StartReading(error)) {
			VirtualHostList->DeleteAllItems();
			ScanButton->SetLabel(_("Stop Scan"));
			Gauge->Show();
		}
		else {
			switch (error) {
			case mvceditor::BackgroundFileReaderClass::ALREADY_RUNNING:
				wxMessageBox(_("Scanner is already running"), _("Configuration Not Found"), wxOK | wxCENTRE, this);
				break;
			case mvceditor::BackgroundFileReaderClass::NO_RESOURCES:
				wxMessageBox(_("System is low on resources.  Try again later."), _("Configuration Not Found"), wxOK | wxCENTRE, this);
				break;
			default:
				wxMessageBox(_("Path not valid"), _("Configuration Not Found"), wxOK | wxCENTRE, this);
				break;
			}
		}
	}
	else {
		wxMessageBox(_("Scan is already running. Stop and restart the scan so that the new directory can be scanned."));
	}	
}

mvceditor::PhpEnvironmentPanelClass::PhpEnvironmentPanelClass(wxWindow* parent, mvceditor::EnvironmentClass& environment)
	: PhpEnvironmentPanelGeneratedClass(parent)
	, Environment(environment) {
	NonEmptyTextValidatorClass phpExecutableValidator(&environment.Php.PhpExecutablePath, PhpLabel);
	PhpExecutable->SetValidator(phpExecutableValidator);
}


void mvceditor::PhpEnvironmentPanelClass::OnPhpFileChanged(wxFileDirPickerEvent& event) {
	PhpExecutable->SetValue(event.GetPath());
}

void mvceditor::PhpEnvironmentPanelClass::OnResize(wxSizeEvent& event) {
	if (GetContainingSizer() && HelpText->GetContainingSizer()) {
		HelpText->Wrap(event.GetSize().GetX());
		HelpText->GetContainingSizer()->Layout();
		GetContainingSizer()->Layout();
		Refresh();
	}
	event.Skip();
}

mvceditor::EnvironmentDialogClass::EnvironmentDialogClass(wxWindow* parent, mvceditor::EnvironmentClass& environment) 
	: Environment(environment) {	
	
	// make it so that no other preference dialogs have to explictly call Transfer methods
	SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
	
	// the following calls must be executed in the following order
	// we need to set the style BEFORE calling Create(); that's why parent class constructor
	// is not in the initializer list
	SetSheetStyle(wxPROPSHEET_DEFAULT);
	Create(parent, wxID_ANY, _("Environment"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	CreateButtons(wxOK | wxCANCEL);
	
	wxBookCtrlBase* notebook = GetBookCtrl();
	wxPanel* panel = new mvceditor::PhpEnvironmentPanelClass(notebook, Environment);
	notebook->AddPage(panel, _("PHP"));
	ApacheEnvironmentPanel = new mvceditor::ApacheEnvironmentPanelClass(notebook, Environment);
	notebook->AddPage(ApacheEnvironmentPanel, _("Apache"));
	WebBrowserPanel = new mvceditor::WebBrowserEditPanelClass(notebook, Environment);
	notebook->AddPage(WebBrowserPanel, _("Web Browsers"));
	LayoutDialog();
}

void mvceditor::EnvironmentDialogClass::OnOkButton(wxCommandEvent& event) {
	wxBookCtrlBase* book = GetBookCtrl();
	if (Validate() && book->Validate() && TransferDataFromWindow() && book->TransferDataFromWindow()) {
		WebBrowserPanel->Apply();
		ApacheEnvironmentPanel->Apply();
		EndModal(wxOK);
	}
}

mvceditor::WebBrowserEditPanelClass::WebBrowserEditPanelClass(wxWindow* parent, mvceditor::EnvironmentClass& environment)
	: WebBrowserEditPanelGeneratedClass(parent)
	, Environment(environment) 
	, EditedWebBrowsers(environment.WebBrowsers) {
	BrowserList->ClearAll();
	BrowserList->InsertColumn(0, _("Web Browser Label"));
	BrowserList->InsertColumn(1, _("Web Browser Path"));
	
	// in case there are no configured browsers still properly show the columns 
	BrowserList->SetColumnWidth(0, 250);
	BrowserList->SetColumnWidth(1, 250);
	std::map<wxString, wxFileName>::const_iterator it = EditedWebBrowsers.begin();
	for (; it != EditedWebBrowsers.end(); ++it) {
		ListCtrlAdd(BrowserList, it->first, it->second.GetFullPath());
	}
	BrowserList->SetColumnWidth(0, wxLIST_AUTOSIZE);
	BrowserList->SetColumnWidth(1, wxLIST_AUTOSIZE);
}

void mvceditor::WebBrowserEditPanelClass::OnResize(wxSizeEvent& event) {
	if (GetContainingSizer() && HelpText->GetContainingSizer()) {
		HelpText->Wrap(event.GetSize().GetX());
		HelpText->GetContainingSizer()->Layout();
		GetContainingSizer()->Layout();
		Refresh();
	}
	event.Skip();
}

void mvceditor::WebBrowserEditPanelClass::OnAddWebBrowser(wxCommandEvent& event) {
	wxString name;
	wxFileName webBrowserPath; 
	mvceditor::WebBrowserCreateDialogClass dialog(this, EditedWebBrowsers, name, webBrowserPath);
	if (wxOK == dialog.ShowModal()) {
		ListCtrlAdd(BrowserList, name, webBrowserPath.GetFullPath());
		BrowserList->SetColumnWidth(0, wxLIST_AUTOSIZE);
		BrowserList->SetColumnWidth(1, wxLIST_AUTOSIZE);
		
		EditedWebBrowsers[name] = webBrowserPath;
	}
}

void mvceditor::WebBrowserEditPanelClass::OnRemoveSelectedWebBrowser(wxCommandEvent& event) {
	int selected = BrowserList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selected >= 0) {
		wxListItem item;
		item.SetColumn(0);
		item.SetId(selected);
		if (BrowserList->GetItem(item)) {
			wxString name = item.GetText();
			std::map<wxString, wxFileName>::iterator it = EditedWebBrowsers.find(name);
			if  (it != EditedWebBrowsers.end()) {
				EditedWebBrowsers.erase(it);
			}
			BrowserList->DeleteItem(selected);
		}
	}
	else {
		wxMessageBox(_("No browsers were selected. Please select a web browser to remove."));
	}
}

void mvceditor::WebBrowserEditPanelClass::OnEditSelectedWebBrowser(wxCommandEvent& event) {
	int selected = BrowserList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selected >= 0) {
		wxString oldName,
			oldWebBrowserPath;
		ListCtrlGet(BrowserList, oldName, oldWebBrowserPath, selected);
		if (!oldName.IsEmpty() && !oldWebBrowserPath.IsEmpty()) {
			wxString newName = oldName;
			wxFileName newWebBrowserPath(oldWebBrowserPath);
			mvceditor::WebBrowserCreateDialogClass dialog(this, EditedWebBrowsers, newName, newWebBrowserPath);
			if (wxOK == dialog.ShowModal()) {
				ListCtrlEdit(BrowserList, newName, newWebBrowserPath.GetFullPath(), selected);
				
				// remove the old name since name may have been changed
				std::map<wxString, wxFileName>::iterator it = EditedWebBrowsers.find(oldName);
				if  (it != EditedWebBrowsers.end()) {
					EditedWebBrowsers.erase(it);
				}
				EditedWebBrowsers[newName] = newWebBrowserPath;
			}
		}
	}
	else {
		wxMessageBox(_("No browsers were selected. Please select a web browser to edit."));
	}
}

void mvceditor::WebBrowserEditPanelClass::Apply() {
	Environment.WebBrowsers = EditedWebBrowsers;
}

mvceditor::WebBrowserCreateDialogClass::WebBrowserCreateDialogClass(wxWindow* parent, 
		std::map<wxString, wxFileName> existingBrowsers, wxString& name, wxFileName& webBrowserFileName)
	: WebBrowserCreateDialogGeneratedClass(parent) 
	, ExistingBrowsers(existingBrowsers)
	, WebBrowserFileName(webBrowserFileName) {
	wxGenericValidator nameValidator(&name);
	WebBrowserLabel->SetValidator(nameValidator);
	WebBrowserPath->SetPath(webBrowserFileName.GetFullPath());
	WebBrowserLabel->SetFocus();
}

void mvceditor::WebBrowserCreateDialogClass::OnOkButton(wxCommandEvent& event) {
	if (Validate()) {
		wxString newName = WebBrowserLabel->GetValue();
		if (newName.IsEmpty()) {
			wxMessageBox(_("Please enter a friendly name for this browser"));
			return;
		}
		if (ExistingBrowsers.find(newName) != ExistingBrowsers.end()) {
			wxMessageBox(_("Please enter a name that is unique to this browser"));
			return;
		}
		wxString path = WebBrowserPath->GetPath();
		if (!wxFileName::FileExists(path)) {
			wxMessageBox(_("Please enter a valid file name for this browser"));
			return;
		}
		TransferDataFromWindow();
		WebBrowserFileName.Assign(path);
		EndModal(wxOK);
	}	
}

mvceditor::VirtualHostCreateDialogClass::VirtualHostCreateDialogClass(wxWindow* parent, 
		std::map<wxString, wxString> existingVirtualHosts, wxString& hostName, wxFileName& rootDirectory)
	: VirtualHostCreateDialogGeneratedClass(parent)
	, ExistingVirtualHosts(existingVirtualHosts)
	, RootDirectoryFileName(rootDirectory) {
	wxGenericValidator nameValidator(&hostName);
	Hostname->SetValidator(nameValidator);
	RootDirectory->SetPath(RootDirectoryFileName.GetFullPath());
	RootDirectory->SetFocus();
}

void mvceditor::VirtualHostCreateDialogClass::OnOkButton(wxCommandEvent& event) {
	if (Validate()) {
		wxString newRootPath = RootDirectory->GetPath();
		if (newRootPath.IsEmpty()) {
			wxMessageBox(_("Please enter a root directory"));
			return;
		}
		if (ExistingVirtualHosts.find(newRootPath) != ExistingVirtualHosts.end()) {
			wxMessageBox(_("Please enter a root directory that is unique."));
			return;
		}
		if  (!wxFileName::DirExists(newRootPath)) {
			wxMessageBox(_("Root directory must be a directory and must be acccessible."));
			return;
		}
		TransferDataFromWindow();
		RootDirectoryFileName.Assign(newRootPath);
		EndModal(wxOK);
	}
}

mvceditor::EnvironmentPluginClass::EnvironmentPluginClass()
	: PluginClass() {
}

void mvceditor::EnvironmentPluginClass::AddProjectMenuItems(wxMenu* projectMenu) {
	projectMenu->Append(mvceditor::MENU_ENVIRONMENT, _("Environment"));
}

void mvceditor::EnvironmentPluginClass::OnMenuEnvironment(wxCommandEvent& event) {
	EnvironmentClass* environment = GetEnvironment();
	mvceditor::EnvironmentDialogClass dialog(GetMainWindow(), *environment);
	if (wxOK == dialog.ShowModal()) {
		environment->SaveToConfig();
	}
}

void mvceditor::EnvironmentPluginClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_ENVIRONMENT] = wxT("Environment-Configure Environment");
	AddDynamicCmd(menuItemIds, shortcuts);
}

BEGIN_EVENT_TABLE(mvceditor::ApacheEnvironmentPanelClass, ApacheEnvironmentPanelGeneratedClass)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_COMPLETE, mvceditor::ApacheEnvironmentPanelClass::OnWorkComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::ApacheEnvironmentPanelClass::OnWorkInProgress)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::EnvironmentPluginClass, wxEvtHandler) 
	EVT_MENU(mvceditor::MENU_ENVIRONMENT, mvceditor::EnvironmentPluginClass::OnMenuEnvironment)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::EnvironmentDialogClass, wxDialog) 
	EVT_BUTTON(wxID_OK, mvceditor::EnvironmentDialogClass::OnOkButton) 	
END_EVENT_TABLE()