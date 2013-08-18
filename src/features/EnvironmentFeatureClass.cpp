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
#include <features/EnvironmentFeatureClass.h>
#include <widgets/NonEmptyTextValidatorClass.h>
#include <MvcEditor.h>
#include <wx/filename.h>
#include <wx/string.h>
#include <wx/valgen.h>

static int ID_APACHE_FILE_READER = wxNewId();

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
	
	// need to set the mask flag; otherwise in MSW the text will not be set
	// this assumes the given list is set to LC_REPORT mode
	wxListItem column1,
		column2;
	column1.SetColumn(0);
	column1.SetId(rowIndex);
	column1.m_mask = wxLIST_MASK_TEXT;
	column2.SetColumn(1);
	column2.SetId(rowIndex);
	column2.m_mask = wxLIST_MASK_TEXT;

	if (list->GetItem(column1) && list->GetItem(column2)) {
		column1Value = column1.GetText();
		column2Value = column2.GetText();
	}
}

const wxEventType mvceditor::EVENT_APACHE_FILE_READ_COMPLETE = wxNewEventType();

mvceditor::ApacheFileReadCompleteEventClass::ApacheFileReadCompleteEventClass(int eventId, const mvceditor::ApacheClass &apache)
	: wxEvent(eventId, mvceditor::EVENT_APACHE_FILE_READ_COMPLETE)
	, Apache(apache) {

}

wxEvent* mvceditor::ApacheFileReadCompleteEventClass::Clone() const {
	mvceditor::ApacheFileReadCompleteEventClass* evt = new mvceditor::ApacheFileReadCompleteEventClass(GetId(), Apache);
	return evt;
}

mvceditor::ApacheFileReaderClass::ApacheFileReaderClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
: BackgroundFileReaderClass(runningThreads, eventId)
	, ApacheResults() {
}

bool mvceditor::ApacheFileReaderClass::Init(const wxString& startDirectory) {
	ApacheResults.ManualConfiguration = false;
	ApacheResults.ClearMappings();
	return BackgroundFileReaderClass::Init(startDirectory);
}

wxString mvceditor::ApacheFileReaderClass::GetLabel() const {
	return wxT("Apache File Reader");
}

bool mvceditor::ApacheFileReaderClass::BackgroundFileMatch(const wxString& file) {
	return true;
}

bool mvceditor::ApacheFileReaderClass::BackgroundFileRead(mvceditor::DirectorySearchClass& search) {
	bool ret = false;
	if (search.Walk(ApacheResults)) {
		ret = true;
	}
	if (!search.More() && !IsCancelled()) {
		
		// when we are done recursing, parse the matched files
		std::vector<wxString> possibleConfigFiles = search.GetMatchedFiles();
		
		// there may be multiple files, at this point just exist as soon as we find one file
		// that we can recognize as a config file
		bool found = false;
		for (size_t i = 0; i <  possibleConfigFiles.size(); ++i) {
			if (ApacheResults.SetHttpdPath(possibleConfigFiles[i])) {
				found = true;
				break;
			}
		}

		// send the event once we have searched all files
		if (found) {
			mvceditor::ApacheFileReadCompleteEventClass evt(ID_APACHE_FILE_READER, ApacheResults);
			PostEvent(evt);
		}
	}
	return ret;
}

mvceditor::ApacheEnvironmentPanelClass::ApacheEnvironmentPanelClass(wxWindow* parent, mvceditor::RunningThreadsClass& runningThreads, EnvironmentClass& environment)
	: ApacheEnvironmentPanelGeneratedClass(parent)
	, Environment(environment)
	, RunningThreads(runningThreads)
	, EditedApache()
	, RunningActionId(0) {
	RunningThreads.AddEventHandler(this);
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

mvceditor::ApacheEnvironmentPanelClass::~ApacheEnvironmentPanelClass() {
	if (RunningActionId > 0) {
		RunningThreads.CancelAction(RunningActionId);
	}
	RunningThreads.RemoveEventHandler(this);
}

void mvceditor::ApacheEnvironmentPanelClass::OnScanButton(wxCommandEvent& event) {
	wxString path = ApacheConfigurationDirectory->GetPath();
	if (path.IsEmpty()) {
		wxMessageBox(_("Please enter a configuration directory"));
		return;
	}
	if (0 == RunningActionId) {
		wxChar ch = wxFileName::GetPathSeparator();
		if (path[path.Length() - 1] != ch) {
			path.Append(ch);
		}
		mvceditor::ApacheFileReaderClass* reader = new mvceditor::ApacheFileReaderClass(RunningThreads, ID_APACHE_FILE_READER);
		if (reader->Init(path)) {
			RunningActionId = RunningThreads.Queue(reader);
			VirtualHostList->DeleteAllItems();
			ScanButton->SetLabel(_("Stop Scan"));
			Gauge->Show();
		}
		else {
			delete reader;
			RunningActionId = 0;
			wxMessageBox(_("Path not valid"), _("Configuration Not Found"), wxOK | wxCENTRE, this);
		}
	}
	else {
		
		// act like a stop button
		Gauge->SetValue(0);
		

		RunningThreads.CancelAction(RunningActionId);
		RunningActionId = 0;
		ScanButton->SetLabel(_("Scan For Configuration"));
	}
}

void mvceditor::ApacheEnvironmentPanelClass::Populate() {
	wxString configFile = EditedApache.GetHttpdPath();
	wxString results;

	// configFile is a full file name, extract the path
	wxFileName fileName(configFile);
	ApacheConfigurationDirectory->SetPath(fileName.GetPath());
	std::map<wxString, wxString> mappings = EditedApache.GetVirtualHostMappings();
	std::map<wxString, wxString>::const_iterator it;
	for (it = mappings.begin(); it != mappings.end(); ++it) {
		ListCtrlAdd(VirtualHostList, it->first, it->second);
	}
	if (!mappings.empty()) {
		VirtualHostList->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_MASK_STATE | wxLIST_MASK_TEXT);
		VirtualHostList->SetColumnWidth(0, wxLIST_AUTOSIZE);
		VirtualHostList->SetColumnWidth(1, wxLIST_AUTOSIZE);
	}
}

void mvceditor::ApacheEnvironmentPanelClass::OnApacheFileReadComplete(mvceditor::ApacheFileReadCompleteEventClass& event) {
		
	// copy the result apache object to overwrite what is shown in the dialog
	EditedApache.Copy(event.Apache);
	Populate();
}

void mvceditor::ApacheEnvironmentPanelClass::OnActionProgress(mvceditor::ActionProgressEventClass& event) {
	Gauge->Pulse();
}

void mvceditor::ApacheEnvironmentPanelClass::OnActionComplete(mvceditor::ActionEventClass& event) {
	Gauge->SetValue(0);
	ScanButton->SetLabel(_("Scan For Configuration"));
	RunningActionId = 0;
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
		VirtualHostList->SetItemState(VirtualHostList->GetItemCount() - 1, wxLIST_STATE_SELECTED, wxLIST_MASK_STATE | wxLIST_MASK_TEXT);
	}
}

void mvceditor::ApacheEnvironmentPanelClass::OnUpdateUi(wxUpdateUIEvent& event) {
	bool enableButtons = Manual->GetValue();
	RemoveButton->Enable(enableButtons);
	EditButton->Enable(enableButtons);
	AddButton->Enable(enableButtons);
	
	// scan button is only for automatic configuration mode
	ScanButton->Enable(!enableButtons && !ApacheConfigurationDirectory->GetPath().IsEmpty());
	ApacheConfigurationDirectory->Enable(!enableButtons);
	event.Skip();
}

void mvceditor::ApacheEnvironmentPanelClass::OnEditButton(wxCommandEvent& event) {
	int selection = VirtualHostList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selection >= 0 && selection < VirtualHostList->GetItemCount()) {
		wxString oldRootDirectory,
			oldHostName;
		ListCtrlGet(VirtualHostList, oldRootDirectory, oldHostName, selection);
		wxString newHostName = oldHostName;
		wxFileName newRootDirectory(oldRootDirectory);
		mvceditor::VirtualHostCreateDialogClass dialog(this, EditedApache.GetVirtualHostMappings(), newHostName, newRootDirectory);
		if (wxOK == dialog.ShowModal()) {
			ListCtrlEdit(VirtualHostList, newRootDirectory.GetFullPath(), newHostName, selection);
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
	if (selected >= 0 && selected < VirtualHostList->GetItemCount()) {
		wxString oldRootDirectory,
			oldHostName;
		ListCtrlGet(VirtualHostList, oldRootDirectory, oldHostName, selected);
		VirtualHostList->DeleteItem(selected);		
		EditedApache.RemoveVirtualHostMapping(oldRootDirectory);
	}
}

bool mvceditor::ApacheEnvironmentPanelClass::TransferDataFromWindow() {
	bool good = wxWindow::TransferDataFromWindow();
	if (good) {
		Environment.Apache = EditedApache;
	}
	return good;
}

void mvceditor::ApacheEnvironmentPanelClass::OnDirChanged(wxFileDirPickerEvent& event) {
	if (0 == RunningActionId) {
		wxString path = ApacheConfigurationDirectory->GetPath();
		wxChar ch = wxFileName::GetPathSeparator();
		if (path[path.Length() - 1] != ch) {
			path.Append(ch);
		}
		mvceditor::ApacheFileReaderClass* reader = new mvceditor::ApacheFileReaderClass(RunningThreads, ID_APACHE_FILE_READER);
		if (reader->Init(path)) {
			RunningActionId = RunningThreads.Queue(reader);
			VirtualHostList->DeleteAllItems();
			ScanButton->SetLabel(_("Stop Scan"));
			Gauge->Show();
		}
		else {
			delete reader;
			wxMessageBox(_("Path not valid"), _("Configuration Not Found"), wxOK | wxCENTRE, this);
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
	if (environment.Php.IsAuto) {
		Version->SetSelection(0);
	}
	else if (pelet::PHP_53 == environment.Php.Version) {
		Version->SetSelection(1);
	}
	else if (pelet::PHP_54 == environment.Php.Version) {
		Version->SetSelection(2);
	}
}

bool mvceditor::PhpEnvironmentPanelClass::TransferDataFromWindow() {
	bool good = wxWindow::TransferDataFromWindow();
	if (good) {
		int sel = Version->GetCurrentSelection();
		Environment.Php.IsAuto = false;
		if (2 == sel) {
			Environment.Php.Version = pelet::PHP_54;
		}
		else if (1 == sel) {
			Environment.Php.Version = pelet::PHP_53;
		}
		else {
			Environment.Php.IsAuto = true;
		}
		good = true;
	}
	return good;
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
	std::vector<mvceditor::WebBrowserClass>::const_iterator it = EditedWebBrowsers.begin();
	for (; it != EditedWebBrowsers.end(); ++it) {
		ListCtrlAdd(BrowserList, it->Name, it->FullPath.GetFullPath());
	}
	if (!EditedWebBrowsers.empty()) {
		BrowserList->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_MASK_STATE | wxLIST_MASK_TEXT);
		BrowserList->SetColumnWidth(0, wxLIST_AUTOSIZE);
		BrowserList->SetColumnWidth(1, wxLIST_AUTOSIZE);
	}
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
	mvceditor::WebBrowserClass newBrowser;
	mvceditor::WebBrowserCreateDialogClass dialog(this, EditedWebBrowsers, newBrowser);
	if (wxOK == dialog.ShowModal()) {
		ListCtrlAdd(BrowserList, newBrowser.Name, newBrowser.FullPath.GetFullPath());
		BrowserList->SetColumnWidth(0, wxLIST_AUTOSIZE);
		BrowserList->SetColumnWidth(1, wxLIST_AUTOSIZE);
		
		EditedWebBrowsers.push_back(newBrowser);
		BrowserList->SetItemState(BrowserList->GetItemCount() - 1, wxLIST_STATE_SELECTED, wxLIST_MASK_STATE | wxLIST_MASK_TEXT);
	}
}

void mvceditor::WebBrowserEditPanelClass::OnRemoveSelectedWebBrowser(wxCommandEvent& event) {

	// could not get the list selection function to work on windows
	// when the "edit" button is clicked
	int selection = BrowserList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selection >= 0 && selection < BrowserList->GetItemCount()) {
		int cur = 0;
		for(std::vector<mvceditor::WebBrowserClass>::iterator it = EditedWebBrowsers.begin(); it != EditedWebBrowsers.end(); ++it) {
			if (cur == selection) {
				EditedWebBrowsers.erase(it);
				break;
			}
			cur++;
		}
		BrowserList->DeleteItem(selection);
	}
	else {
		wxMessageBox(_("No browsers were selected. Please select a web browser to remove."));
	}
}

void mvceditor::WebBrowserEditPanelClass::OnEditSelectedWebBrowser(wxCommandEvent& event) {

	// could not get the list selection function to work on windows
	// when the "edit" button is clicked
	long selection = BrowserList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selection >= 0 && selection < BrowserList->GetItemCount()) {
		mvceditor::WebBrowserClass oldBrowser = EditedWebBrowsers[selection];
		mvceditor::WebBrowserCreateDialogClass dialog(this, EditedWebBrowsers, oldBrowser);
		if (wxOK == dialog.ShowModal()) {
			ListCtrlEdit(BrowserList, oldBrowser.Name, oldBrowser.FullPath.GetFullPath(), selection);
			
			// remove the old name since name may have been changed
			EditedWebBrowsers[selection] = oldBrowser;
			
			BrowserList->SetColumnWidth(0, wxLIST_AUTOSIZE);
			BrowserList->SetColumnWidth(1, wxLIST_AUTOSIZE);
		}
	}
	else {
		wxMessageBox(_("No browsers were selected. Please select a web browser to edit."));
	}
}

void mvceditor::WebBrowserEditPanelClass::OnMoveUp(wxCommandEvent& event) {
	
	// could not get the list selection function to work on windows
	// when the button is clicked
	int selection = BrowserList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selection > 0 && selection < BrowserList->GetItemCount()) {

		// the 'top' row will moved down one and the row at the selection will be moved up one
		mvceditor::WebBrowserClass top =  EditedWebBrowsers[selection - 1];
		mvceditor::WebBrowserClass selected =  EditedWebBrowsers[selection];
		EditedWebBrowsers[selection - 1] = selected;
		EditedWebBrowsers[selection] = top;
		
		ListCtrlEdit(BrowserList, selected.Name, selected.FullPath.GetFullPath(), selection - 1);
		ListCtrlEdit(BrowserList, top.Name, top.FullPath.GetFullPath(), selection);

		// toggle selection on modified rows
		BrowserList->SetItemState(selection - 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
		BrowserList->SetFocus();
	}
	else {
		wxMessageBox(_("No browsers were selected. Please select a web browser to move."));
	}
}

void mvceditor::WebBrowserEditPanelClass::OnMoveDown(wxCommandEvent& event) {

	// could not get the list selection function to work on windows
	// when the button is clicked
	int selection = BrowserList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selection >= 0 && selection < (BrowserList->GetItemCount() - 1)) {
	
		// the 'bottom' row will moved up one and the row at the selection will be moved down one
		mvceditor::WebBrowserClass bottom =  EditedWebBrowsers[selection + 1];
		mvceditor::WebBrowserClass selected =  EditedWebBrowsers[selection];
		EditedWebBrowsers[selection + 1] = selected;
		EditedWebBrowsers[selection] = bottom;

		ListCtrlEdit(BrowserList, selected.Name, selected.FullPath.GetFullPath(), selection + 1);
		ListCtrlEdit(BrowserList, bottom.Name, bottom.FullPath.GetFullPath(), selection);

		BrowserList->SetItemState(selection + 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
		BrowserList->SetFocus();
	}
	else {
		wxMessageBox(_("No browsers were selected. Please select a web browser to move."));
	}
}

bool mvceditor::WebBrowserEditPanelClass::TransferDataFromWindow() {
	bool good = wxWindow::TransferDataToWindow();
	if (good) {
		Environment.WebBrowsers = EditedWebBrowsers;
	}
	return true;
}

mvceditor::WebBrowserCreateDialogClass::WebBrowserCreateDialogClass(wxWindow* parent, 
																	std::vector<mvceditor::WebBrowserClass> existingBrowsers, 
																	mvceditor::WebBrowserClass& oldBrowser)
	: WebBrowserCreateDialogGeneratedClass(parent) 
	, ExistingBrowsers(existingBrowsers)
	, NewBrowser(oldBrowser)
	, OriginalName(oldBrowser.Name) {
	wxGenericValidator nameValidator(&NewBrowser.Name);
	WebBrowserLabel->SetValidator(nameValidator);
	WebBrowserPath->SetPath(NewBrowser.FullPath.GetFullPath());
	WebBrowserLabel->SetFocus();
}

void mvceditor::WebBrowserCreateDialogClass::OnOkButton(wxCommandEvent& event) {
	if (Validate()) {
		wxString newName = WebBrowserLabel->GetValue();
		if (newName.IsEmpty()) {
			wxMessageBox(_("Please enter a friendly name for this browser"));
			return;
		}

		// allow the user to use the same name if they did not change
		// otherwise every time the user wants to edit a browser they would need to change the name
		bool found = false;
		for(std::vector<mvceditor::WebBrowserClass>::const_iterator it = ExistingBrowsers.begin(); it != ExistingBrowsers.end(); ++it) {
			if (it->Name == newName) {
				found = true;
				break;
			}
		}
		if (OriginalName != newName && found) {
			wxMessageBox(_("Please enter a name that is unique to this browser"));
			return;
		}
		wxString path = WebBrowserPath->GetPath();
		if (!wxFileName::FileExists(path)) {
			wxMessageBox(_("Please enter a valid file path for this browser"));
			return;
		}
		TransferDataFromWindow();
		NewBrowser.FullPath.Assign(path);
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

mvceditor::EnvironmentFeatureClass::EnvironmentFeatureClass(mvceditor::AppClass& app)
	: FeatureClass(app) {
}

void mvceditor::EnvironmentFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {
	wxConfigBase* config = wxConfigBase::Get();
	mvceditor::EnvironmentClass* environment = GetEnvironment();
	environment->SaveToConfig(config);
	if (environment->Php.IsAuto) {
		environment->Php.AutoDetermine();
	}

	// signal that this app has modified the config file, that way the external
	// modification check fails and the user will not be prompted to reload the config
	App.UpdateConfigModifiedTime();
}

void mvceditor::EnvironmentFeatureClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
	parent->AddPage(new mvceditor::WebBrowserEditPanelClass(parent, *GetEnvironment()), _("Web Browsers"));	
	parent->AddPage(new mvceditor::ApacheEnvironmentPanelClass(parent, App.RunningThreads, *GetEnvironment()), _("Apache"));
	parent->AddPage(new mvceditor::PhpEnvironmentPanelClass(parent, *GetEnvironment()), _("PHP Executable"));
}

BEGIN_EVENT_TABLE(mvceditor::ApacheEnvironmentPanelClass, ApacheEnvironmentPanelGeneratedClass)
	EVT_ACTION_PROGRESS(ID_APACHE_FILE_READER, mvceditor::ApacheEnvironmentPanelClass::OnActionProgress)
	EVT_APACHE_FILE_READ_COMPLETE(ID_APACHE_FILE_READER, mvceditor::ApacheEnvironmentPanelClass::OnApacheFileReadComplete)
	EVT_ACTION_COMPLETE(ID_APACHE_FILE_READER, mvceditor::ApacheEnvironmentPanelClass::OnActionComplete)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::EnvironmentFeatureClass, wxEvtHandler) 
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_PREFERENCES_SAVED, mvceditor::EnvironmentFeatureClass::OnPreferencesSaved) 	
END_EVENT_TABLE()