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
#include <plugins/ResourcePluginClass.h>
#include <windows/StringHelperClass.h>
#include <wx/artprov.h>
#include <wx/filename.h>
#include <wx/valgen.h>

int ID_JUMP_TO_GAUGE = wxNewId();
int ID_COUNT_FILES_GAUGE = wxNewId();
int ID_MENU_INDEX = mvceditor::PluginClass::newMenuId();
int ID_MENU_JUMP = mvceditor::PluginClass::newMenuId();
int ID_TOOLBAR_INDEX = wxNewId();
int ID_RESOURCE_PLUGIN_PANEL = wxNewId();

mvceditor::ResourcePluginClass::ResourcePluginClass()
	: PluginClass()
	, JumpToText()
	, DirectorySearch()
	, ProjectIndexMenu(NULL)
	, Timer()
	, State(FREE)
	, HasCodeLookups(false)
	, HasFileLookups(false) {
	Timer.SetOwner(this);
	ResourcePluginPanel = NULL;
}

void mvceditor::ResourcePluginClass::AddProjectMenuItems(wxMenu* projectMenu) {
	ProjectIndexMenu = projectMenu->Append(ID_MENU_INDEX, _("Index"), _("Index the project"));
	projectMenu->Append(ID_MENU_JUMP, _("Jump To Resource"), _("Jump To Current Resource"));
	ProjectIndexMenu->Enable(GetProject() && FREE == State);
}

void mvceditor::ResourcePluginClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	toolBar->AddTool(ID_TOOLBAR_INDEX, wxT("Index"), wxArtProvider::GetBitmap(
		wxART_EXECUTABLE_FILE, wxART_TOOLBAR, wxSize(16, 16)), wxT("Index"), wxITEM_NORMAL);
}

void mvceditor::ResourcePluginClass::AddWindows() {
	ResourcePluginPanel = new ResourcePluginPanelClass(GetMainWindow(), *this);
	AuiManager->AddPane(ResourcePluginPanel, wxAuiPaneInfo().Top(
		).CaptionVisible(false).CloseButton(false).Gripper(
		false).DockFixed(true).PaneBorder(false).Floatable(
		false).Row(2));
}

void mvceditor::ResourcePluginClass::AddCodeControlClassContextMenuItems(wxMenu* menu) {
	menu->Append(ID_MENU_JUMP, _("Jump To Source"));
}

void mvceditor::ResourcePluginClass::OnProjectOpened() {
	HasCodeLookups = false;
	HasFileLookups = false;
}

void mvceditor::ResourcePluginClass::SearchForResources() {
	// ATTN: resource parsing is done in the main thread.
	// there is no mechanism for thread safe access to resource finder.
	// at some point, use BackgroundFileReaderClass
	if (GetProject() && !GetProject()->GetRootPath().IsEmpty()) {
		if (FREE == State) { //prevent two finds at a time
			ResourceFinderClass* resourceFinder = GetResourceFinder();

			// don't bother searching when path or expression is not valid
			if (DirectorySearch.Init(GetProject()->GetRootPath()) && resourceFinder->Prepare(JumpToText)) {
				GetStatusBarWithGauge()->AddGauge(_("Counting Files"), ID_COUNT_FILES_GAUGE, StatusBarWithGaugeClass::INDETERMINATE_MODE, 
					wxGA_HORIZONTAL);
				FileCount = 0;
				State = GOTO_COUNT_FILES;
				Timer.Start(200, wxTIMER_CONTINUOUS);
			}
			else {
				wxMessageBox(_("Invalid Expression or Project Path"), wxT("Warning"), wxICON_EXCLAMATION);
			}
		}
	}
	else {
		wxMessageBox(_("This feature can only be used when you open project"), wxT("Warning"), wxICON_EXCLAMATION);
	}
}

void mvceditor::ResourcePluginClass::OnIdleEvent(wxIdleEvent& event) {
	if (GOTO_COUNT_FILES == State || INDEX_COUNT_FILES == State) {
		
		// we may or may not index depending on whether we have built the appropriate cache
		if (!NeedToIndex()) {
			Timer.Stop();
			GetStatusBarWithGauge()->StopGauge(ID_COUNT_FILES_GAUGE);
			State = State == GOTO_COUNT_FILES ? GOTO : INDEX;
		}
		
		// for each idle event do at most 50 files.  this will prevent freezes on big projects.
		int i = 0;
		int MAX_DIR = 50;
		while (i < MAX_DIR && DirectorySearch.More()) {
			if (DirectorySearch.Walk(*this)) {
				FileCount++;
			}
			i++;
		}
		if (!DirectorySearch.More()) {
			// end of file count. build the status bar and commence searching
			Timer.Stop();
			GetStatusBarWithGauge()->StopGauge(ID_COUNT_FILES_GAUGE);			
			if (DirectorySearch.Init(GetProject()->GetRootPath())) {
				// not quite sure why we are one-off here, if I dont add the one I get an failed assertion in the gauge class
				GetStatusBarWithGauge()->AddGauge(_("Resource Lookup "), ID_JUMP_TO_GAUGE, FileCount + 1, 
						wxGA_HORIZONTAL);
				State = State == GOTO_COUNT_FILES ? GOTO : INDEX;
			}
		}
		else {
			event.RequestMore();
		}
	}
	else if (GOTO == State || INDEX == State) {
		ResourceFinderClass* resourceFinder = GetResourceFinder();

		// we may or may not index depending on whether we have built the appropriate cache
		// only walk through at most 10 files per idle event
		if (NeedToIndex()) {
			for (int i = 0; i < 10 && DirectorySearch.More(); i++) {
				GetStatusBarWithGauge()->IncrementGauge(ID_JUMP_TO_GAUGE);
				DirectorySearch.Walk(*resourceFinder);				
			}
		}
		if (!NeedToIndex() || !DirectorySearch.More()) {
			// THE END of the resource caching
			if (ResourceFinderClass::CLASS_NAME == resourceFinder->GetResourceType() || 
				ResourceFinderClass::CLASS_NAME_METHOD_NAME == resourceFinder->GetResourceType()) {
				HasCodeLookups = true;
				HasFileLookups = true;
			}
			else if (ResourceFinderClass::FILE_NAME == resourceFinder->GetResourceType() || 
				ResourceFinderClass::FILE_NAME_LINE_NUMBER == resourceFinder->GetResourceType()) {
				HasFileLookups = true;
			}			
			if (GOTO == State) {
				resourceFinder->CollectNearMatchResources();
			}
			GetStatusBarWithGauge()->StopGauge(ID_JUMP_TO_GAUGE);
			
			// switch state before showing results, so that when more idle events get triggered we skip them
			States previousState = State;
			State = FREE;
			if (GOTO == previousState) {
				ShowJumpToResults();
			}			
		}
		else {
			event.RequestMore();
		}
	}
}

void mvceditor::ResourcePluginClass::ShowJumpToResults() {
	ResourceFinderClass* resourceFinder = GetResourceFinder();
	wxArrayString files;
	switch (resourceFinder->GetResourceMatchCount()) {
		case 1:
			LoadPageFromResourceFinder(resourceFinder, 0);
			break;
		case 0:
			if (ResourceFinderClass::CLASS_NAME_METHOD_NAME == resourceFinder->GetResourceType()) {
				wxMessageBox(_("Method Not Found"));
			}
			else if (ResourceFinderClass::CLASS_NAME == resourceFinder->GetResourceType()) {
				wxMessageBox(_("Class Not Found"));
			}
			else {
				wxMessageBox(_("File Not Found"));
			}
			break;
		default:
			for (size_t i = 0; i < resourceFinder->GetResourceMatchCount(); ++i) {
				files.Add(resourceFinder->GetResourceMatchFullPath(i));
			}
			
			// dont show the project path to the user
			for (size_t i = 0; i < files.GetCount(); ++i) {
				files[i].Replace(GetProject()->GetRootPath(), wxT(""));
				if (ResourceFinderClass::FILE_NAME != resourceFinder->GetResourceType() &&
					ResourceFinderClass::FILE_NAME_LINE_NUMBER != resourceFinder->GetResourceType()) {
					files[i] += wxT("  (") + mvceditor::StringHelperClass::IcuToWx(resourceFinder->GetResourceMatch(i).Resource) + wxT(")");
				}
			}
			wxSingleChoiceDialog dialog(NULL,
				wxString::Format(_("Found %d files. Please choose file to open."), resourceFinder->GetResourceMatchCount()),
				_("Resource Finder"), files, NULL, wxDEFAULT_DIALOG_STYLE| wxRESIZE_BORDER | wxOK | wxCANCEL);
			dialog.SetSize(wxSize(640, 120));
			if (wxID_OK == dialog.ShowModal()) {
				int selection = dialog.GetSelection();
				LoadPageFromResourceFinder(resourceFinder, selection);	
			}
			break;
	}
}

void mvceditor::ResourcePluginClass::OnProjectIndex(wxCommandEvent& event) {
	if (GetProject() && !GetProject()->GetRootPath().IsEmpty()) {
		
		// prevent indexing from being done if already being performed
		if (FREE == State) {
			ResourceFinderClass* resourceFinder = GetResourceFinder();
			
			// index for now is just searching for something that will never be found
			if (DirectorySearch.Init(GetProject()->GetRootPath()) && resourceFinder->Prepare(wxT("FakeClass"))) {				
				GetStatusBarWithGauge()->AddGauge(_("Counting Files"), ID_COUNT_FILES_GAUGE, StatusBarWithGaugeClass::INDETERMINATE_MODE, 
					wxGA_HORIZONTAL);
				FileCount = 0;
				Timer.Start(200, wxTIMER_CONTINUOUS);
				State = INDEX_COUNT_FILES;
				
				// INDEX state is user-initiated index, force a refresh of the cache
				HasCodeLookups = false;
				HasFileLookups = false;
				
			}
		}
	}
	else {
		wxMessageBox(_("This feature can only be used when you open project"), wxT("Warning"), wxICON_EXCLAMATION);
	}
}

void mvceditor::ResourcePluginClass::OnJump(wxCommandEvent& event) {
	CodeControlClass* codeControl = GetCurrentCodeControl();
	wxWindow* mainWindow = GetMainWindow();
	if (codeControl) {
		ResourceFinderClass* resourceFinder = GetResourceFinder();
		wxString text = codeControl->GetSelectedText();
		if (!text.IsEmpty()) {
			// the user specifically marked a string to search
			// lets see if we can determine the class. if we can determine the class, we will have a higher
			// chance of getting fewer matches from the resource finder
			wxString symbol = codeControl->GetCurrentSymbol();
			if (symbol.Contains(wxT("::"))) {
				text = symbol;
			}
			bool found = resourceFinder->Prepare(text) && resourceFinder->CollectNearMatchResources();
			if (found) {
				JumpToText = text;
				ShowJumpToResults();	
			}
			else {
				
				// maybe cache has not been created or user has not indexed the project, lets index the project and try again
				JumpToText = text;
				
				ResourcePluginPanelClass* window = (ResourcePluginPanelClass*)wxWindow::FindWindowById(ID_RESOURCE_PLUGIN_PANEL, mainWindow);
				window->FocusOnSearchControl();
				SearchForResources();
			}
		}
		else {
			ResourcePluginPanelClass* window = (ResourcePluginPanelClass*)wxWindow::FindWindowById(ID_RESOURCE_PLUGIN_PANEL, mainWindow);
			window->FocusOnSearchControl();
			SearchForResources();
		}
	}
	else {
		ResourcePluginPanelClass* window = (ResourcePluginPanelClass*)wxWindow::FindWindowById(ID_RESOURCE_PLUGIN_PANEL, mainWindow);
		window->FocusOnSearchControl();
	}
}

void mvceditor::ResourcePluginClass::LoadPageFromResourceFinder(ResourceFinderClass* resourceFinder, int resourceMatchIndex) {
	GetNotebook()->LoadPage(resourceFinder->GetResourceMatchFullPath(resourceMatchIndex));
	CodeControlClass* codeControl = GetCurrentCodeControl();
	if (codeControl) {
		int lineNumber = resourceFinder->GetLineNumber();
		int32_t position, 
			length;
		bool found = resourceFinder->GetResourceMatchPosition(resourceMatchIndex, codeControl->GetSafeText(), position, length);
		if (ResourceFinderClass::FILE_NAME_LINE_NUMBER == resourceFinder->GetResourceType()) {
				
			// scintilla line numbers start at zero. use the ensure method so that the line is shown in the 
			// center of the screen
			int pos = codeControl->PositionFromLine(lineNumber - 1);
			codeControl->SetSelectionAndEnsureVisible(pos, pos);
			codeControl->GotoLine(lineNumber - 1);
		}
		else if (found) {
			codeControl->SetSelectionAndEnsureVisible(position, position + length);
		}
	}
}

void mvceditor::ResourcePluginClass::OnUpdateUi(wxUpdateUIEvent& event) {
	ProjectIndexMenu->Enable(GetProject() && FREE == State);
	event.Skip();
}

mvceditor::ResourceFinderClass* mvceditor::ResourcePluginClass::GetResourceFinder() const {
	return GetProject()->GetResourceFinder();
}

bool mvceditor::ResourcePluginClass::NeedToIndex() const {
	ResourceFinderClass* resourceFinder = GetResourceFinder();
	if ((ResourceFinderClass::CLASS_NAME == resourceFinder->GetResourceType() || 
		ResourceFinderClass::CLASS_NAME_METHOD_NAME == resourceFinder->GetResourceType()) && !HasCodeLookups)  {
		return true;
	}
	else if ((ResourceFinderClass::FILE_NAME == resourceFinder->GetResourceType() || 
		ResourceFinderClass::FILE_NAME_LINE_NUMBER == resourceFinder->GetResourceType()) && !HasFileLookups) {
		return true;
	}			
	return false;
}

bool mvceditor::ResourcePluginClass::Walk(const wxString& file) {
	// always count all files.
	return true;
}

void mvceditor::ResourcePluginClass::OnTimer(wxTimerEvent& event) {
	GetStatusBarWithGauge()->IncrementGauge(ID_COUNT_FILES_GAUGE, StatusBarWithGaugeClass::INDETERMINATE_MODE);
}

void mvceditor::ResourcePluginClass::OnPageChanged(wxAuiNotebookEvent& event) {
	size_t selection =  event.GetSelection();
	CodeControlClass* code = GetNotebook()->GetCodeControl(selection);
	if (code) {
		wxString fileName = code->GetFileName();
		ResourcePluginPanel->ChangeToFileName(fileName);
	}
	event.Skip();
}

void mvceditor::ResourcePluginClass::OnPageClosed(wxAuiNotebookEvent& event) {
	ResourcePluginPanel->RemoveClosedFiles(GetNotebook());
	event.Skip();
}

void mvceditor::ResourcePluginClass::OpenFile(wxString fileName) {
	GetNotebook()->LoadPage(fileName);
}

mvceditor::ResourcePluginPanelClass::ResourcePluginPanelClass(wxWindow* parent, ResourcePluginClass& resource)
	: ResourcePluginGeneratedPanelClass(parent, ID_RESOURCE_PLUGIN_PANEL)
	, ResourcePlugin(resource) {
	wxGenericValidator filesComboValidator(&ResourcePlugin.JumpToText);
	FilesCombo->SetValidator(filesComboValidator);
	
	HelpButton->SetBitmapLabel((wxArtProvider::GetBitmap(wxART_HELP, 
		wxART_TOOLBAR, wxSize(16, 16))));
}

void mvceditor::ResourcePluginPanelClass::FocusOnSearchControl() {
	TransferDataToWindow();
	FilesCombo->SetFocus();
}
	
/**
 * When a file is chosen make the notebook show the file
 * @param wxCommandEvent& the event
 */
void mvceditor::ResourcePluginPanelClass::OnFilesComboCombobox(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow()) {
		wxString fullPath = FilesCombo->GetValue();
		wxFileName fileName(fullPath);
		if (fileName.Normalize() && wxFileName::IsFileReadable(fileName.GetFullPath())) {
			ResourcePlugin.OpenFile(fileName.GetFullPath());
		}
		else {
			ResourcePlugin.SearchForResources();
		}
	}
}

/**
 * When the user presses enter iniate a file search for the entered text.
 * @param wxCommandEvent& the event
 */
void mvceditor::ResourcePluginPanelClass::OnFilesComboTextEnter(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow()) {
		wxString fullPath = FilesCombo->GetValue();
		wxFileName fileName(fullPath);
		if (fileName.Normalize() && wxFileName::IsFileReadable(fileName.GetFullPath())) {
			ResourcePlugin.OpenFile(fileName.GetFullPath());
		}
		else {
			ResourcePlugin.SearchForResources();
		}
	}
}


/**
 * When the user clicks the help button help text will be shown
 * @param wxCommandEvent& the event
 */
void mvceditor::ResourcePluginPanelClass::OnHelpButtonClick(wxCommandEvent& event) {
  wxString help = wxString::FromAscii("Type in a file name, file name:page number, "
		"class name,  or class name::method name. The resulting page will then ben opened.\n\nExamples:\n\n"
		"user.php\n"
		"user.php:129\n"
		"User\n"
		"User::login\n\n"
		"You may even enter part of files names\n\n"
		"ser.php (would match user.php)\n"
		"You may even do a fuzzy string search (edit distance) for parts of class names.\n\n"
		"ser, usr, USER (would all match a 'user' class)\n\n"
		"Note that an exact match is done first; if an exact match is found then that file is opened; only if " 
		"an exact match is not found will a fuzzy search be done.\n"
		"The fuzzy string search (edit distance) threshold is configurable in under Edit ... Preferences in case " 
		"searches are returning too many matches.\n\n"
	);
	help = wxGetTranslation(help);
	wxMessageBox(help, _("Help"), wxOK);	
}

void mvceditor::ResourcePluginPanelClass::ChangeToFileName(wxString fileName) {

	// for now let's ignore the new (untitled) files
	if (!fileName.IsEmpty()) {
		FilesCombo->SetValue(fileName);
		wxArrayString values = FilesCombo->GetStrings();
		int foundIndex = values.Index(fileName);
		if (foundIndex == wxNOT_FOUND) {		
			FilesCombo->Append(fileName);	
		}
	}
	else {
		FilesCombo->SetValue(wxEmptyString);
	}
}

void mvceditor::ResourcePluginPanelClass::RemoveClosedFiles(mvceditor::NotebookClass* notebook) {
	int MAX_ITEMS = 18;
	int pageCount = notebook->GetPageCount();
	if (pageCount <= 0) {
		FilesCombo->SetValue(wxEmptyString);
	}

	// over a certain size, remove file names that are no longer open
	// we want to remove as little as possible so that it is easier for the 
	// user to get back to a file.
	for (size_t i = 0; i < FilesCombo->GetCount() && FilesCombo->GetCount() > (size_t)MAX_ITEMS; ++i) {
		wxString fileName = FilesCombo->GetString(i);
		bool found = false;
		for(size_t j = 0; j < notebook->GetPageCount(); ++j) {
			CodeControlClass* code = notebook->GetCodeControl(j);
			if (NULL != code) {
				if (fileName == code->GetFileName()) {
					found = true;
					
					// we are guaranteed to only have one instance of filename (file can be in at most 1 tab only).
					break;
				}
			}
		}
		if (!found) {
			FilesCombo->Delete(i);
			--i;
		}
	}
}

BEGIN_EVENT_TABLE(mvceditor::ResourcePluginClass, wxEvtHandler)
	EVT_MENU(ID_TOOLBAR_INDEX, mvceditor::ResourcePluginClass::OnProjectIndex)
	EVT_MENU(ID_MENU_JUMP, mvceditor::ResourcePluginClass::OnJump)
	EVT_IDLE(mvceditor::ResourcePluginClass::OnIdleEvent)
	EVT_MENU(ID_MENU_INDEX, mvceditor::ResourcePluginClass::OnProjectIndex)
	EVT_UPDATE_UI(wxID_ANY, mvceditor::ResourcePluginClass::OnUpdateUi)
	EVT_TIMER(wxID_ANY, mvceditor::ResourcePluginClass::OnTimer)
	EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, mvceditor::ResourcePluginClass::OnPageChanged)
	EVT_AUINOTEBOOK_PAGE_CLOSED(wxID_ANY, mvceditor::ResourcePluginClass::OnPageClosed)
END_EVENT_TABLE()