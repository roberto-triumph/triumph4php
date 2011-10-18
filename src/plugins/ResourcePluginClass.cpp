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

mvceditor::ResourceFileReaderClass::ResourceFileReaderClass(wxEvtHandler& handler) 
	: BackgroundFileReaderClass(handler)
	, NewResources() {
}

bool mvceditor::ResourceFileReaderClass::InitForNativeFunctionsFile(const mvceditor::ResourceFinderClass& finder) {
	wxFileName nativeFunctionsFilePath = NewResources.NativeFunctionsFilePath();
	if (Init(nativeFunctionsFilePath.GetPath())) {
		NewResources.CopyResourcesFrom(finder);
		NewResources.FilesFilter = nativeFunctionsFilePath.GetFullName();

		// need to do this so that the resource finder attempts to parse the files
		NewResources.Prepare(wxT("FakeClass"));
		return true;
	}
	return false;
}

bool mvceditor::ResourceFileReaderClass::InitForProject(const mvceditor::ResourceFinderClass& finder, const wxString& projectRootPath, const wxString& phpFileExtensions) {
	if (Init(projectRootPath)) {
		NewResources.CopyResourcesFrom(finder);
		NewResources.FilesFilter = phpFileExtensions;
		return true;
	}
	return false;
}

void mvceditor::ResourceFileReaderClass::GetNewResources(mvceditor::ResourceFinderClass& dest) {
	dest.CopyResourcesFrom(NewResources);
}

bool mvceditor::ResourceFileReaderClass::FileRead(mvceditor::DirectorySearchClass& search) {
	return search.Walk(NewResources);
}

bool mvceditor::ResourceFileReaderClass::FileMatch(const wxString& file) {
	return wxMatchWild(NewResources.FilesFilter, file);
}

mvceditor::ResourcePluginClass::ResourcePluginClass()
	: PluginClass()
	, JumpToText()
	, ResourceFileReader(*this)
	, ProjectIndexMenu(NULL)
	, State(FREE) 
	, HasCodeLookups(false)
	, HasFileLookups(false) {

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
	ResourcePluginPanelClass* panel = new ResourcePluginPanelClass(GetMainWindow(), *this, GetNotebook());
	AuiManager->AddPane(panel, wxAuiPaneInfo().Top(
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
	if (ResourceFileReader.IsRunning()) {
		ResourceFileReader.StopReading();
	}
	if (ResourceFileReader.InitForNativeFunctionsFile(*(GetProject()->GetResourceFinder()))) {
		mvceditor::BackgroundFileReaderClass::StartError error = mvceditor::BackgroundFileReaderClass::NONE;
		if (ResourceFileReader.StartReading(error)) {
			State = INDEXING_NATIVE_FUNCTIONS;
			GetStatusBarWithGauge()->AddGauge(_("Preparing Resource Index"), ID_COUNT_FILES_GAUGE, 
				StatusBarWithGaugeClass::INDETERMINATE_MODE, wxGA_HORIZONTAL);
		}
		else if (mvceditor::BackgroundFileReaderClass::ALREADY_RUNNING == error) {
			wxMessageBox(_("Indexing is already taking place. Please wait."), wxT("Warning"), wxICON_EXCLAMATION);
		}
		else if (mvceditor::BackgroundFileReaderClass::NO_RESOURCES == error) {
			wxMessageBox(_("Your system is low on resources. Try again later."), wxT("Warning"), wxICON_EXCLAMATION);
		}
	}
}

void mvceditor::ResourcePluginClass::SearchForResources() {
	mvceditor::ProjectClass* project = GetProject();
	mvceditor::ResourceFinderClass* resourceFinder = project->GetResourceFinder();
	
	// don't bother searching when path or expression is not valid
	if (!resourceFinder->Prepare(JumpToText)) {
		wxMessageBox(_("Invalid Expression"), wxT("Warning"), wxICON_EXCLAMATION);
		return;
	}
	
	// if we know the indexing has already taken place lets just do the lookup; it will be quick.
	if (!NeedToIndex() && project) {
		resourceFinder->CollectNearMatchResources();
		ShowJumpToResults();
		return;
	}

	// need to do indexing; start the background process
	
	if (project && !project->GetRootPath().IsEmpty()) {

		//prevent two finds at a time
		if (FREE == State) { 
			ResourceFinderClass* resourceFinder = project->GetResourceFinder();

			// don't bother searching when path is not valid
			if (ResourceFileReader.InitForProject(*resourceFinder, project->GetRootPath(), 
				project->GetPhpFileExtensions())) {
					mvceditor::BackgroundFileReaderClass::StartError error = mvceditor::BackgroundFileReaderClass::NONE;
					if (ResourceFileReader.StartReading(error)) {
						State = GOTO;
						GetStatusBarWithGauge()->AddGauge(_("Searching For Resources"),
							ID_COUNT_FILES_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE,
							wxGA_HORIZONTAL);
					}
					else if (mvceditor::BackgroundFileReaderClass::ALREADY_RUNNING == error) {
						wxMessageBox(_("Indexing is already taking place. Please wait."), wxT("Warning"), wxICON_EXCLAMATION);
					}
					else if (mvceditor::BackgroundFileReaderClass::NO_RESOURCES == error) {
						wxMessageBox(_("Your system is low on resources. Try again later."), wxT("Warning"), wxICON_EXCLAMATION);
					}
			}
			else {
				wxMessageBox(_("Invalid Project Path"), wxT("Warning"), wxICON_EXCLAMATION);
			}
		}
		else {
			wxMessageBox(_("Indexing is already taking place. Please wait."), wxT("Warning"), wxICON_EXCLAMATION);
		}
	}
	else {
		wxMessageBox(_("This feature can only be used when you open project"), wxT("Warning"), wxICON_EXCLAMATION);
	}
}

void mvceditor::ResourcePluginClass::OnWorkInProgress(wxCommandEvent& event) {
	GetStatusBarWithGauge()->IncrementGauge(ID_COUNT_FILES_GAUGE, StatusBarWithGaugeClass::INDETERMINATE_MODE);
}

void mvceditor::ResourcePluginClass::OnWorkComplete(wxCommandEvent& event) {
	GetStatusBarWithGauge()->StopGauge(ID_COUNT_FILES_GAUGE);

	mvceditor::ResourceFinderClass* resourceFinder = GetResourceFinder();
	ResourceFileReader.GetNewResources(*resourceFinder);

	// figure out what resources have been cached, so that next time we can jump
	// to the results without creating a new background thread
	if (INDEXING_PROJECT == State || GOTO == State) {
		if (ResourceFinderClass::CLASS_NAME == resourceFinder->GetResourceType() ||
			ResourceFinderClass::CLASS_NAME_METHOD_NAME == resourceFinder->GetResourceType()) {
			HasCodeLookups = true;
			HasFileLookups = true;
		}
		else if (ResourceFinderClass::FILE_NAME == resourceFinder->GetResourceType() ||
			ResourceFinderClass::FILE_NAME_LINE_NUMBER == resourceFinder->GetResourceType()) {
			HasFileLookups = true;
		}
	}
	
	// if we indexed because of a user query; need to show the user the results.
	States previousState = State;
	State = FREE;
	if (GOTO == previousState) {
		resourceFinder->CollectNearMatchResources();
		ShowJumpToResults();
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
	mvceditor::ProjectClass* project = GetProject();
	if (project && !project->GetRootPath().IsEmpty()) {

		//prevent two finds at a time
		if (FREE == State) { 
			ResourceFinderClass* resourceFinder = project->GetResourceFinder();

			// don't bother searching when path or expression is not valid
			// need to do this so that the resource finder attempts to parse the files
			resourceFinder->Prepare(wxT("FakeClass"));			
			if (ResourceFileReader.InitForProject(*resourceFinder, project->GetRootPath(), 
				project->GetPhpFileExtensions())) {
					mvceditor::BackgroundFileReaderClass::StartError error = mvceditor::BackgroundFileReaderClass::NONE;
					if (ResourceFileReader.StartReading(error)) {
						State = INDEXING_PROJECT;
						GetStatusBarWithGauge()->AddGauge(_("Indexing Project"),
							ID_COUNT_FILES_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE,
							wxGA_HORIZONTAL);
					}
					else if (mvceditor::BackgroundFileReaderClass::ALREADY_RUNNING == error) {
						wxMessageBox(_("Indexing is already taking place. Please wait."), wxT("Warning"), wxICON_EXCLAMATION);
					}
					else if (mvceditor::BackgroundFileReaderClass::NO_RESOURCES == error) {
						wxMessageBox(_("Your system is low on resources. Try again later."), wxT("Warning"), wxICON_EXCLAMATION);
					}
			}
			else {
				wxMessageBox(_("Invalid Project Path"), wxT("Warning"), wxICON_EXCLAMATION);
			}
		}
		else {
			wxMessageBox(_("Indexing is already taking place. Please wait."), wxT("Warning"), wxICON_EXCLAMATION);
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


mvceditor::ResourceFinderClass* mvceditor::ResourcePluginClass::GetResourceFinder() const {
	return GetProject()->GetResourceFinder();
}

mvceditor::ResourcePluginPanelClass::ResourcePluginPanelClass(wxWindow* parent, ResourcePluginClass& resource, NotebookClass* notebook)
	: ResourcePluginGeneratedPanelClass(parent, ID_RESOURCE_PLUGIN_PANEL)
	, ResourcePlugin(resource)
	, Notebook(notebook) {
	wxGenericValidator filesComboValidator(&ResourcePlugin.JumpToText);
	FilesCombo->SetValidator(filesComboValidator);
	
	HelpButton->SetBitmapLabel((wxArtProvider::GetBitmap(wxART_HELP, 
		wxART_TOOLBAR, wxSize(16, 16))));
		
	// lets capture the notebook events so we can update the combo box
	Notebook->Connect(Notebook->GetId(), wxID_ANY, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, 
		wxAuiNotebookEventHandler(ResourcePluginPanelClass::OnPageChanged), NULL, this);
	Notebook->Connect(Notebook->GetId(), wxID_ANY, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED, 
		wxAuiNotebookEventHandler(ResourcePluginPanelClass::OnPageClosed), NULL, this);
}

void mvceditor::ResourcePluginPanelClass::OnClose(wxCloseEvent& event) {
	Notebook->Disconnect(Notebook->GetId(), wxID_ANY, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED,
		wxAuiNotebookEventHandler(ResourcePluginPanelClass::OnPageChanged), NULL, this);
	Notebook->Disconnect(Notebook->GetId(), wxID_ANY, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED, 
		wxAuiNotebookEventHandler(ResourcePluginPanelClass::OnPageClosed), NULL, this);
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
			Notebook->LoadPage(fileName.GetFullPath());
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
			Notebook->LoadPage(fileName.GetFullPath());
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

void mvceditor::ResourcePluginPanelClass::OnPageChanged(wxAuiNotebookEvent& event) {
	size_t selection =  event.GetSelection();
	CodeControlClass* code = Notebook->GetCodeControl(selection);
	if (code) {
		wxString fileName = code->GetFileName();
		
		// for now let's ignore the new (untitled) files
		if (!fileName.IsEmpty()) {
			FilesCombo->SetValue(fileName);
			wxArrayString values = FilesCombo->GetStrings();
			int foundIndex = values.Index(fileName);
			if (foundIndex == wxNOT_FOUND) {		
				FilesCombo->Append(fileName);	
			}
		}
	}
	
}

void mvceditor::ResourcePluginPanelClass::OnPageClosed(wxAuiNotebookEvent& event) {
	int MAX_ITEMS = 18;
	int pageCount = Notebook->GetPageCount();
	if (pageCount <= 0) {
		FilesCombo->SetValue(wxEmptyString);
	}

	// over a certain size, remove file names that are no longer open
	// we want to remove as little as possible so that it is easier for the 
	// user to get back to a file.
	for (size_t i = 0; i < FilesCombo->GetCount() && FilesCombo->GetCount() > (size_t)MAX_ITEMS; ++i) {
		wxString fileName = FilesCombo->GetString(i);
		bool found = false;
		for(size_t j = 0; j < Notebook->GetPageCount(); ++j) {
			CodeControlClass* code = Notebook->GetCodeControl(j);
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
	event.Skip();
}

BEGIN_EVENT_TABLE(mvceditor::ResourcePluginClass, wxEvtHandler)
	EVT_MENU(ID_TOOLBAR_INDEX, mvceditor::ResourcePluginClass::OnProjectIndex)
	EVT_MENU(ID_MENU_JUMP, mvceditor::ResourcePluginClass::OnJump)
	EVT_MENU(ID_MENU_INDEX, mvceditor::ResourcePluginClass::OnProjectIndex)
	EVT_UPDATE_UI(wxID_ANY, mvceditor::ResourcePluginClass::OnUpdateUi)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_FILE_READ_COMPLETE, mvceditor::ResourcePluginClass::OnWorkComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::ResourcePluginClass::OnWorkInProgress)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::ResourcePluginPanelClass, ResourcePluginGeneratedPanelClass)
	EVT_CLOSE(mvceditor::ResourcePluginPanelClass::OnClose)
END_EVENT_TABLE()