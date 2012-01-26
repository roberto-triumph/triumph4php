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

mvceditor::ApacheFileReaderClass::ApacheFileReaderClass(wxEvtHandler& handler)
	: BackgroundFileReaderClass(handler)
	, ApacheResults() {
}

bool mvceditor::ApacheFileReaderClass::Init(const wxString& startDirectory) {
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
	, ApacheFileReader(*this) {
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
	wxString configFile = Environment.Apache.GetHttpdPath();
	wxString results;

	// configFile is a full file name, extract the path
	wxFileName fileName(configFile);
	ApacheConfigurationDirectory->SetPath(fileName.GetPath(true));
	if (!Environment.Apache.GetDocumentRoot().IsEmpty()) {
		results += wxT("Document Root:") + Environment.Apache.GetDocumentRoot() + wxT("\n");
	}
	if (Environment.Apache.GetListenPort() > 0) {
		results += wxString::Format(wxT("Listen Port: %d\n"), Environment.Apache.GetListenPort());
	}
	std::map<wxString, wxString> virtualHosts = Environment.Apache.GetVirtualHostMappings();
	results += wxT("-----------------------------------------------------\n");
	if (0 >= virtualHosts.size()) {
		results += wxT("No virtual hosts. Will use localhost.");
	}
	else {
		for (std::map<wxString, wxString>::iterator it = virtualHosts.begin(); it != virtualHosts.end(); ++it) {
			results += wxT("Server Name: ") + it->second +
			wxT("\nDocument Root: ") + it->first +
			wxT("\n-----------------------------------------------------\n");
		}
	}
	VirtualHostResults->SetValue(results);
}

void mvceditor::ApacheEnvironmentPanelClass::OnWorkComplete(wxCommandEvent& event) {
	Gauge->SetValue(0);
	ScanButton->SetLabel(_("Scan For Configuration"));
		
	Environment.Apache = ApacheFileReader.Results();
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
	: wxPropertySheetDialog(parent, wxID_ANY, _("Environment"), wxDefaultPosition, wxDefaultSize, 
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) {
	CreateButtons(wxOK | wxCANCEL);
	wxBookCtrlBase* notebook = GetBookCtrl();
	
	// make it so that no other preference dialogs have to explictly call Transfer methods
	notebook->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);	
	wxPanel* panel = new mvceditor::PhpEnvironmentPanelClass(notebook, environment);
	notebook->AddPage(panel, _("PHP"));
	panel = new mvceditor::ApacheEnvironmentPanelClass(notebook, environment);
	notebook->AddPage(panel, _("Apache"));
	WebBrowserPanel = new mvceditor::WebBrowserEditPanelClass(notebook, environment);
	notebook->AddPage(WebBrowserPanel, _("Web Browsers"));
}

void mvceditor::EnvironmentDialogClass::Prepare() {
	GetBookCtrl()->InitDialog();
	LayoutDialog();
}

void mvceditor::EnvironmentDialogClass::OnOkButton(wxCommandEvent& event) {
	wxBookCtrlBase* book = GetBookCtrl();
	if (Validate() && book->Validate() && TransferDataFromWindow() && book->TransferDataFromWindow()) {
		WebBrowserPanel->Apply();
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
	std::map<wxString, wxFileName>::const_iterator it = EditedWebBrowsers.begin();
	int newRowNumber = 0;
	for (; it != EditedWebBrowsers.end(); ++it) {
		
		// list ctrl is tricky, for columns we must insertItem() then setItem() for the next columns
		wxListItem nameItem;
		nameItem.SetColumn(0);
		nameItem.SetId(newRowNumber);
		nameItem.SetText(it->first);
		BrowserList->InsertItem(nameItem);
		
		wxListItem webBrowserItem;
		webBrowserItem.SetId(newRowNumber);
		webBrowserItem.SetColumn(1);
		webBrowserItem.SetText(it->second.GetFullPath());
		BrowserList->SetItem(webBrowserItem);

		newRowNumber++;
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
		int newRowNumber = BrowserList->GetItemCount();
		wxListItem infoNameColumn;
		infoNameColumn.SetColumn(0);
		infoNameColumn.SetText(name);
		infoNameColumn.SetId(newRowNumber);
		BrowserList->InsertItem(infoNameColumn);
		
		wxListItem infoPathColumn;
		infoPathColumn.SetColumn(1);
		infoPathColumn.SetId(newRowNumber);
		infoPathColumn.SetText(webBrowserPath.GetFullPath());
		BrowserList->SetItem(infoPathColumn);
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
		wxListItem nameItem,
			webBrowserPathItem;
		nameItem.SetColumn(0);
		nameItem.SetId(selected);
		webBrowserPathItem.SetColumn(1);
		webBrowserPathItem.SetId(selected);
		if (BrowserList->GetItem(nameItem) && BrowserList->GetItem(webBrowserPathItem)) {
			wxString oldName = nameItem.GetText();
			wxFileName webBrowserPath(webBrowserPathItem.GetText());
			wxString newName = oldName;
			mvceditor::WebBrowserCreateDialogClass dialog(this, EditedWebBrowsers, newName, webBrowserPath);
			if (wxOK == dialog.ShowModal()) {
				BrowserList->SetItem(selected, 0, newName);
				BrowserList->SetItem(selected, 1, webBrowserPath.GetFullPath());
				
				// remove the old name since name may have been changed
				std::map<wxString, wxFileName>::iterator it = EditedWebBrowsers.find(oldName);
				if  (it != EditedWebBrowsers.end()) {
					EditedWebBrowsers.erase(it);
				}
				EditedWebBrowsers[newName] = webBrowserPath;
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

mvceditor::EnvironmentPluginClass::EnvironmentPluginClass()
	: PluginClass() {
}

void mvceditor::EnvironmentPluginClass::AddProjectMenuItems(wxMenu* projectMenu) {
	projectMenu->Append(mvceditor::MENU_ENVIRONMENT, _("Environment"));
}

void mvceditor::EnvironmentPluginClass::OnMenuEnvironment(wxCommandEvent& event) {
	EnvironmentClass* environment = GetEnvironment();
	mvceditor::EnvironmentDialogClass dialog(GetMainWindow(), *environment);
	dialog.Prepare();
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