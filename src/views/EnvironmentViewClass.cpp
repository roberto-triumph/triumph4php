/**
 * @copyright  2015 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
 */
#include <views/EnvironmentViewClass.h>
#include <features/EnvironmentFeatureClass.h>
#include <widgets/NonEmptyTextValidatorClass.h>
#include <widgets/ListWidget.h>
#include <Triumph.h>
#include <wx/valgen.h>
#include <wx/msgdlg.h>

static int ID_APACHE_FILE_READER = wxNewId();

t4p::ApacheEnvironmentPanelClass::ApacheEnvironmentPanelClass(wxWindow* parent, t4p::RunningThreadsClass& runningThreads, EnvironmentClass& environment)
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

t4p::ApacheEnvironmentPanelClass::~ApacheEnvironmentPanelClass() {
	if (RunningActionId > 0) {
		RunningThreads.CancelAction(RunningActionId);
	}
	RunningThreads.RemoveEventHandler(this);
}

void t4p::ApacheEnvironmentPanelClass::OnScanButton(wxCommandEvent& event) {
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
		t4p::ApacheFileReaderClass* reader = new t4p::ApacheFileReaderClass(RunningThreads, ID_APACHE_FILE_READER);
		if (reader->Init(path)) {
			RunningActionId = RunningThreads.Queue(reader);
			VirtualHostList->DeleteAllItems();
			ScanButton->SetLabel(_("Stop Scan"));
			Gauge->Show();
		} else {
			delete reader;
			RunningActionId = 0;
			wxMessageBox(_("Path not valid"), _("Configuration Not Found"), wxOK | wxCENTRE, this);
		}
	} else {
		// act like a stop button
		Gauge->SetValue(0);


		RunningThreads.CancelAction(RunningActionId);
		RunningActionId = 0;
		ScanButton->SetLabel(_("Scan For Configuration"));
	}
}

void t4p::ApacheEnvironmentPanelClass::Populate() {
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

void t4p::ApacheEnvironmentPanelClass::OnApacheFileReadComplete(t4p::ApacheFileReadCompleteEventClass& event) {
	// copy the result apache object to overwrite what is shown in the dialog
	EditedApache.Copy(event.Apache);
	Populate();
}

void t4p::ApacheEnvironmentPanelClass::OnActionProgress(t4p::ActionProgressEventClass& event) {
	Gauge->Pulse();
}

void t4p::ApacheEnvironmentPanelClass::OnActionComplete(t4p::ActionEventClass& event) {
	Gauge->SetValue(0);
	ScanButton->SetLabel(_("Scan For Configuration"));
	RunningActionId = 0;
}

void t4p::ApacheEnvironmentPanelClass::OnResize(wxSizeEvent& event) {
	if (GetContainingSizer() && HelpText->GetContainingSizer()) {
		HelpText->Wrap(event.GetSize().GetX());
		HelpText->GetContainingSizer()->Layout();
		GetContainingSizer()->Layout();
		Refresh();
	}
	event.Skip();
}

void t4p::ApacheEnvironmentPanelClass::OnAddButton(wxCommandEvent& event) {
	wxFileName rootDirectory;
	wxString hostName;
	t4p::VirtualHostCreateDialogClass dialog(this, EditedApache.GetVirtualHostMappings(), hostName, rootDirectory);
	if (wxOK == dialog.ShowModal()) {
		ListCtrlAdd(VirtualHostList, rootDirectory.GetFullPath(), hostName);
		VirtualHostList->SetColumnWidth(0, wxLIST_AUTOSIZE);
		VirtualHostList->SetColumnWidth(1, wxLIST_AUTOSIZE);

		EditedApache.SetVirtualHostMapping(rootDirectory.GetFullPath(), hostName);
		VirtualHostList->SetItemState(VirtualHostList->GetItemCount() - 1, wxLIST_STATE_SELECTED, wxLIST_MASK_STATE | wxLIST_MASK_TEXT);
	}
}

void t4p::ApacheEnvironmentPanelClass::OnUpdateUi(wxUpdateUIEvent& event) {
	bool enableButtons = Manual->GetValue();
	RemoveButton->Enable(enableButtons);
	EditButton->Enable(enableButtons);
	AddButton->Enable(enableButtons);

	// scan button is only for automatic configuration mode
	ScanButton->Enable(!enableButtons && !ApacheConfigurationDirectory->GetPath().IsEmpty());
	ApacheConfigurationDirectory->Enable(!enableButtons);
	event.Skip();
}

void t4p::ApacheEnvironmentPanelClass::OnEditButton(wxCommandEvent& event) {
	int selection = VirtualHostList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selection >= 0 && selection < VirtualHostList->GetItemCount()) {
		wxString oldRootDirectory,
			oldHostName;
		ListCtrlGet(VirtualHostList, oldRootDirectory, oldHostName, selection);
		wxString newHostName = oldHostName;
		wxFileName newRootDirectory(oldRootDirectory);
		t4p::VirtualHostCreateDialogClass dialog(this, EditedApache.GetVirtualHostMappings(), newHostName, newRootDirectory);
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

void t4p::ApacheEnvironmentPanelClass::OnRemoveButton(wxCommandEvent& event) {
	int selected = VirtualHostList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selected >= 0 && selected < VirtualHostList->GetItemCount()) {
		wxString oldRootDirectory,
			oldHostName;
		ListCtrlGet(VirtualHostList, oldRootDirectory, oldHostName, selected);
		VirtualHostList->DeleteItem(selected);
		EditedApache.RemoveVirtualHostMapping(oldRootDirectory);
	}
}

bool t4p::ApacheEnvironmentPanelClass::TransferDataFromWindow() {
	bool good = wxWindow::TransferDataFromWindow();
	if (good) {
		Environment.Apache = EditedApache;
	}
	return good;
}

void t4p::ApacheEnvironmentPanelClass::OnDirChanged(wxFileDirPickerEvent& event) {
	if (0 == RunningActionId) {
		wxString path = ApacheConfigurationDirectory->GetPath();
		wxChar ch = wxFileName::GetPathSeparator();
		if (path[path.Length() - 1] != ch) {
			path.Append(ch);
		}
		t4p::ApacheFileReaderClass* reader = new t4p::ApacheFileReaderClass(RunningThreads, ID_APACHE_FILE_READER);
		if (reader->Init(path)) {
			RunningActionId = RunningThreads.Queue(reader);
			VirtualHostList->DeleteAllItems();
			ScanButton->SetLabel(_("Stop Scan"));
			Gauge->Show();
		} else {
			delete reader;
			wxMessageBox(_("Path not valid"), _("Configuration Not Found"), wxOK | wxCENTRE, this);
		}
	} else {
		wxMessageBox(_("Scan is already running. Stop and restart the scan so that the new directory can be scanned."));
	}
}

t4p::PhpEnvironmentPanelClass::PhpEnvironmentPanelClass(wxWindow* parent, t4p::EnvironmentClass& environment)
	: PhpEnvironmentPanelGeneratedClass(parent)
	, Environment(environment) {
	PhpExecutable->SetValue(environment.Php.PhpExecutablePath);
	wxGenericValidator installedValidator(&environment.Php.Installed);
	Installed->SetValidator(installedValidator);
	if (environment.Php.IsAuto) {
		Version->SetSelection(0);
	} else if (pelet::PHP_53 == environment.Php.Version) {
		Version->SetSelection(1);
	} else if (pelet::PHP_54 == environment.Php.Version) {
		Version->SetSelection(2);
	}
	PhpExecutable->Enable(environment.Php.Installed);
	PhpExecutableFile->Enable(environment.Php.Installed);
}

bool t4p::PhpEnvironmentPanelClass::TransferDataFromWindow() {
	bool good = true;

	// php executable can be empty when user does not have php installed
	if (Installed->GetValue()) {
		wxString path = PhpExecutable->GetValue();
		if (!wxFileName::FileExists(path)) {
			wxMessageBox(_("PHP executable cannot be empty."), _("Error: PHP Executable"));
			good = false;
		}
	}
	if (!Installed->GetValue()) {
		int sel = Version->GetCurrentSelection();
		if (0 == sel) {
			wxMessageBox(_("If PHP executable is not installed you need to choose a PHP version."), _("Error: PHP Executable"));
			good = false;
		}
	}
	if (good) {
		good = wxWindow::TransferDataFromWindow();
	}
	if (good) {
		int sel = Version->GetCurrentSelection();
		Environment.Php.IsAuto = false;
		if (2 == sel) {
			Environment.Php.Version = pelet::PHP_54;
		} else if (1 == sel) {
			Environment.Php.Version = pelet::PHP_53;
		} else {
			Environment.Php.IsAuto = true;
		}
		if (Installed->GetValue()) {
			Environment.Php.PhpExecutablePath = PhpExecutable->GetValue();
		}
	}
	return good;
}


void t4p::PhpEnvironmentPanelClass::OnPhpFileChanged(wxFileDirPickerEvent& event) {
	PhpExecutable->SetValue(event.GetPath());
}

void t4p::PhpEnvironmentPanelClass::OnInstalledCheck(wxCommandEvent& event) {
	PhpExecutable->Enable(event.IsChecked());
	PhpExecutableFile->Enable(event.IsChecked());
}

void t4p::PhpEnvironmentPanelClass::OnResize(wxSizeEvent& event) {
	if (GetContainingSizer() && HelpText->GetContainingSizer()) {
		HelpText->Wrap(event.GetSize().GetX());
		HelpText->GetContainingSizer()->Layout();
		GetContainingSizer()->Layout();
		Refresh();
	}
	event.Skip();
}

t4p::WebBrowserEditPanelClass::WebBrowserEditPanelClass(wxWindow* parent, t4p::EnvironmentClass& environment)
	: WebBrowserEditPanelGeneratedClass(parent)
	, Environment(environment)
	, EditedWebBrowsers(environment.WebBrowsers) {
	BrowserList->ClearAll();
	BrowserList->InsertColumn(0, _("Web Browser Label"));
	BrowserList->InsertColumn(1, _("Web Browser Path"));

	// in case there are no configured browsers still properly show the columns
	BrowserList->SetColumnWidth(0, 250);
	BrowserList->SetColumnWidth(1, 250);
	std::vector<t4p::WebBrowserClass>::const_iterator it = EditedWebBrowsers.begin();
	for (; it != EditedWebBrowsers.end(); ++it) {
		ListCtrlAdd(BrowserList, it->Name, it->FullPath.GetFullPath());
	}
	if (!EditedWebBrowsers.empty()) {
		BrowserList->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_MASK_STATE | wxLIST_MASK_TEXT);
		BrowserList->SetColumnWidth(0, wxLIST_AUTOSIZE);
		BrowserList->SetColumnWidth(1, wxLIST_AUTOSIZE);
	}
}

void t4p::WebBrowserEditPanelClass::OnResize(wxSizeEvent& event) {
	if (GetContainingSizer() && HelpText->GetContainingSizer()) {
		HelpText->Wrap(event.GetSize().GetX());
		HelpText->GetContainingSizer()->Layout();
		GetContainingSizer()->Layout();
		Refresh();
	}
	event.Skip();
}

void t4p::WebBrowserEditPanelClass::OnAddWebBrowser(wxCommandEvent& event) {
	t4p::WebBrowserClass newBrowser;
	t4p::WebBrowserCreateDialogClass dialog(this, EditedWebBrowsers, newBrowser);
	if (wxOK == dialog.ShowModal()) {
		ListCtrlAdd(BrowserList, newBrowser.Name, newBrowser.FullPath.GetFullPath());
		BrowserList->SetColumnWidth(0, wxLIST_AUTOSIZE);
		BrowserList->SetColumnWidth(1, wxLIST_AUTOSIZE);

		EditedWebBrowsers.push_back(newBrowser);
		BrowserList->SetItemState(BrowserList->GetItemCount() - 1, wxLIST_STATE_SELECTED, wxLIST_MASK_STATE | wxLIST_MASK_TEXT);
	}
}

void t4p::WebBrowserEditPanelClass::OnRemoveSelectedWebBrowser(wxCommandEvent& event) {
	// could not get the list selection function to work on windows
	// when the "edit" button is clicked
	int selection = BrowserList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selection >= 0 && selection < BrowserList->GetItemCount()) {
		int cur = 0;
		for(std::vector<t4p::WebBrowserClass>::iterator it = EditedWebBrowsers.begin(); it != EditedWebBrowsers.end(); ++it) {
			if (cur == selection) {
				EditedWebBrowsers.erase(it);
				break;
			}
			cur++;
		}
		BrowserList->DeleteItem(selection);
	} else {
		wxMessageBox(_("No browsers were selected. Please select a web browser to remove."));
	}
}

void t4p::WebBrowserEditPanelClass::OnEditSelectedWebBrowser(wxCommandEvent& event) {
	// could not get the list selection function to work on windows
	// when the "edit" button is clicked
	long selection = BrowserList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selection >= 0 && selection < BrowserList->GetItemCount()) {
		t4p::WebBrowserClass oldBrowser = EditedWebBrowsers[selection];
		t4p::WebBrowserCreateDialogClass dialog(this, EditedWebBrowsers, oldBrowser);
		if (wxOK == dialog.ShowModal()) {
			ListCtrlEdit(BrowserList, oldBrowser.Name, oldBrowser.FullPath.GetFullPath(), selection);

			// remove the old name since name may have been changed
			EditedWebBrowsers[selection] = oldBrowser;

			BrowserList->SetColumnWidth(0, wxLIST_AUTOSIZE);
			BrowserList->SetColumnWidth(1, wxLIST_AUTOSIZE);
		}
	} else {
		wxMessageBox(_("No browsers were selected. Please select a web browser to edit."));
	}
}

void t4p::WebBrowserEditPanelClass::OnMoveUp(wxCommandEvent& event) {
	// could not get the list selection function to work on windows
	// when the button is clicked
	int selection = BrowserList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selection > 0 && selection < BrowserList->GetItemCount()) {
		// the 'top' row will moved down one and the row at the selection will be moved up one
		t4p::WebBrowserClass top =  EditedWebBrowsers[selection - 1];
		t4p::WebBrowserClass selected =  EditedWebBrowsers[selection];
		EditedWebBrowsers[selection - 1] = selected;
		EditedWebBrowsers[selection] = top;

		ListCtrlEdit(BrowserList, selected.Name, selected.FullPath.GetFullPath(), selection - 1);
		ListCtrlEdit(BrowserList, top.Name, top.FullPath.GetFullPath(), selection);

		// toggle selection on modified rows
		BrowserList->SetItemState(selection - 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
		BrowserList->SetFocus();
	} else {
		wxMessageBox(_("No browsers were selected. Please select a web browser to move."));
	}
}

void t4p::WebBrowserEditPanelClass::OnMoveDown(wxCommandEvent& event) {
	// could not get the list selection function to work on windows
	// when the button is clicked
	int selection = BrowserList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selection >= 0 && selection < (BrowserList->GetItemCount() - 1)) {
		// the 'bottom' row will moved up one and the row at the selection will be moved down one
		t4p::WebBrowserClass bottom =  EditedWebBrowsers[selection + 1];
		t4p::WebBrowserClass selected =  EditedWebBrowsers[selection];
		EditedWebBrowsers[selection + 1] = selected;
		EditedWebBrowsers[selection] = bottom;

		ListCtrlEdit(BrowserList, selected.Name, selected.FullPath.GetFullPath(), selection + 1);
		ListCtrlEdit(BrowserList, bottom.Name, bottom.FullPath.GetFullPath(), selection);

		BrowserList->SetItemState(selection + 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
		BrowserList->SetFocus();
	} else {
		wxMessageBox(_("No browsers were selected. Please select a web browser to move."));
	}
}

bool t4p::WebBrowserEditPanelClass::TransferDataFromWindow() {
	bool good = wxWindow::TransferDataToWindow();
	if (good) {
		Environment.WebBrowsers = EditedWebBrowsers;
	}
	return true;
}

t4p::WebBrowserCreateDialogClass::WebBrowserCreateDialogClass(wxWindow* parent,
																	std::vector<t4p::WebBrowserClass> existingBrowsers,
																	t4p::WebBrowserClass& oldBrowser)
	: WebBrowserCreateDialogGeneratedClass(parent)
	, ExistingBrowsers(existingBrowsers)
	, NewBrowser(oldBrowser)
	, OriginalName(oldBrowser.Name) {
	wxGenericValidator nameValidator(&NewBrowser.Name);
	WebBrowserLabel->SetValidator(nameValidator);
	WebBrowserPath->SetPath(NewBrowser.FullPath.GetFullPath());
	WebBrowserLabel->SetFocus();
}

void t4p::WebBrowserCreateDialogClass::OnOkButton(wxCommandEvent& event) {
	if (Validate()) {
		wxString newName = WebBrowserLabel->GetValue();
		if (newName.IsEmpty()) {
			wxMessageBox(_("Please enter a friendly name for this browser"));
			return;
		}

		// allow the user to use the same name if they did not change
		// otherwise every time the user wants to edit a browser they would need to change the name
		bool found = false;
		for(std::vector<t4p::WebBrowserClass>::const_iterator it = ExistingBrowsers.begin(); it != ExistingBrowsers.end(); ++it) {
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

t4p::VirtualHostCreateDialogClass::VirtualHostCreateDialogClass(wxWindow* parent,
		std::map<wxString, wxString> existingVirtualHosts, wxString& hostName, wxFileName& rootDirectory)
	: VirtualHostCreateDialogGeneratedClass(parent)
	, ExistingVirtualHosts(existingVirtualHosts)
	, RootDirectoryFileName(rootDirectory) {
	wxGenericValidator nameValidator(&hostName);
	Hostname->SetValidator(nameValidator);
	RootDirectory->SetPath(RootDirectoryFileName.GetFullPath());
	RootDirectory->SetFocus();
}

void t4p::VirtualHostCreateDialogClass::OnOkButton(wxCommandEvent& event) {
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
t4p::EnvironmentViewClass::EnvironmentViewClass(t4p::EnvironmentFeatureClass& feature)
: FeatureViewClass()
, Feature(feature) {
}

void t4p::EnvironmentViewClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
	parent->AddPage(new t4p::WebBrowserEditPanelClass(parent, Feature.App.Globals.Environment), _("Web Browsers"));
	parent->AddPage(new t4p::ApacheEnvironmentPanelClass(parent, Feature.App.RunningThreads, Feature.App.Globals.Environment), _("Apache"));
	parent->AddPage(new t4p::PhpEnvironmentPanelClass(parent, Feature.App.Globals.Environment), _("PHP Executable"));
}

BEGIN_EVENT_TABLE(t4p::ApacheEnvironmentPanelClass, ApacheEnvironmentPanelGeneratedClass)
	EVT_ACTION_PROGRESS(ID_APACHE_FILE_READER, t4p::ApacheEnvironmentPanelClass::OnActionProgress)
	EVT_APACHE_FILE_READ_COMPLETE(wxID_ANY, t4p::ApacheEnvironmentPanelClass::OnApacheFileReadComplete)
	EVT_ACTION_COMPLETE(ID_APACHE_FILE_READER, t4p::ApacheEnvironmentPanelClass::OnActionComplete)
END_EVENT_TABLE()
