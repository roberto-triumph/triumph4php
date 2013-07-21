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
#include <widgets/NotebookClass.h>
#include <globals/String.h>
#include <globals/Assets.h>
#include <search/FindInFilesClass.h>
#include <MvcEditor.h>
#include <globals/Errors.h>
#include <wx/artprov.h>
#include <wx/filename.h>
#include <wx/file.h>
#include <wx/wupdlock.h>

int ID_SAVE_MODIFIED = wxNewId();

mvceditor::NotebookClass::NotebookClass(wxWindow* parent, wxWindowID id, 
	const wxPoint& pos, const wxSize& size, long style)
	: wxAuiNotebook(parent, id, pos, size, style)
	, CodeControlOptions(NULL)
	, Globals(NULL)
	, EventSink(NULL)
	, ContextMenu(NULL)
	, NewPageNumber(1) {
}

mvceditor::NotebookClass::~NotebookClass() {
	delete ContextMenu;
}

mvceditor::CodeControlClass* mvceditor::NotebookClass::GetCodeControl(size_t pageNumber) const {
	mvceditor::CodeControlClass* codeControl = NULL;
	if (pageNumber < GetPageCount()) {
		wxWindow* window = GetPage(pageNumber);
		codeControl = (mvceditor::CodeControlClass*)window;
	}
	return codeControl;
}

mvceditor::CodeControlClass* mvceditor::NotebookClass::GetCurrentCodeControl() const {
	return GetCodeControl(GetSelection());
}

void mvceditor::NotebookClass::SavePageIfModified(wxAuiNotebookEvent& event) {
	int currentPage = event.GetSelection();
	bool vetoed = false;
	CodeControlClass* codeCtrl = GetCodeControl(currentPage);
	if (codeCtrl && codeCtrl->GetModify()) {
		wxString pageName = GetPageText(currentPage);
		if (pageName.EndsWith(wxT("*"))) {
			pageName = pageName.SubString(0, pageName.size() -2);
		}
		wxString msg = pageName + wxT(" has not been saved. Save changes?");
		int response = wxMessageBox(msg, wxT("Save PHP File"), wxYES_NO | 
			wxCANCEL | wxICON_QUESTION, this);
		if (wxCANCEL == response || (wxYES == response && 
				!SavePage(currentPage))) {
			vetoed = false;
			event.Veto();
		}
	}
	if (!vetoed && codeCtrl) {

		// tell the app that a file has been closed
		mvceditor::CodeControlEventClass codeControlEvent(mvceditor::EVENT_APP_FILE_CLOSED, codeCtrl);
		EventSink->Publish(codeControlEvent);
	}
	if (!vetoed) {
		event.Skip();
	}
}

bool mvceditor::NotebookClass::SavePage(int pageIndex) {
	CodeControlClass* phpSourceCodeCtrl = GetCodeControl(
			pageIndex);
	bool saved = false;
	if (phpSourceCodeCtrl && phpSourceCodeCtrl->IsNew()) {
		wxString fileFilter = CreateWildcardString();
		int filterIndex = WilcardIndex(phpSourceCodeCtrl->GetDocumentMode());
		wxFileDialog fileDialog(this, wxT("Save a File"), wxT(""), wxT(""), 
				fileFilter, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
		fileDialog.SetFilterIndex(filterIndex);
		if (wxID_OK == fileDialog.ShowModal()) {
			wxString newFullPath = fileDialog.GetPath();
			if (!phpSourceCodeCtrl->SaveAndTrackFile(newFullPath)) {
				wxMessageBox(wxT("Could Not Save File."));
			}
			else {
				wxString newFilename, extension;
				bool hasExtension = false;
				wxFileName::SplitPath(newFullPath, NULL, NULL, &newFilename, 
					&extension, &hasExtension);
				newFilename = hasExtension ? newFilename + wxT(".") + extension
					: newFilename;
				SetPageText(pageIndex, newFilename);
				saved = true;

				wxCommandEvent createdEvt(mvceditor::EVENT_APP_FILE_CREATED);
				createdEvt.SetString(newFullPath);
				EventSink->Publish(createdEvt);
			}
		}
	}
	else {
		if (phpSourceCodeCtrl && !phpSourceCodeCtrl->SaveAndTrackFile()) {
			wxMessageBox(wxT("Could Not Save File."));
		}
		else {
			saved = true;
		}
	}
	return saved;
}

void mvceditor::NotebookClass::MarkPageAsModified(int windowId) {
	int pageNumber = GetPageIndex(FindWindow(windowId));
	wxString filename = GetPageText(pageNumber);
	if (!filename.EndsWith(wxT("*"))) {
		filename = filename + wxT("*");
		SetPageText(pageNumber, filename);
	}
}

void mvceditor::NotebookClass::MarkPageAsNotModified(int windowId) {
	int pageNumber = GetPageIndex(FindWindow(windowId));
	wxString filename = GetPageText(pageNumber);
	if (filename.EndsWith(wxT("*"))) {
		filename = filename.SubString(0, filename.size() - 2);
		SetPageText(pageNumber, filename);
	}
}
void mvceditor::NotebookClass::AddMvcEditorPage(mvceditor::CodeControlClass::Mode mode) {
	wxString format;
	switch (mode) {
		case mvceditor::CodeControlClass::TEXT:
			format = _("Untitled %d.txt");
			break;
		case mvceditor::CodeControlClass::SQL:
			format = _("Untitled %d.sql");
			break;
		case mvceditor::CodeControlClass::CSS:
			format = _("Untitled %d.css");
			break;
		case mvceditor::CodeControlClass::PHP:
			format = _("Untitled %d.php");
			break;
	}
	
	// make sure to use a unique ID, other source code depends on this
	CodeControlClass* page = new CodeControlClass(this, *CodeControlOptions, Globals, *EventSink, wxNewId());
	page->SetDocumentMode(mode);
	wxBitmap docBitmap = mvceditor::IconImageAsset(wxT("document-text"));
	if (mvceditor::CodeControlClass::PHP == mode) {
		docBitmap = mvceditor::IconImageAsset(wxT("document-php"));
	}
	else if (mvceditor::CodeControlClass::CSS == mode) {
		docBitmap = mvceditor::IconImageAsset(wxT("document-css"));
	}
	else if (mvceditor::CodeControlClass::SQL == mode) {
		docBitmap = mvceditor::IconImageAsset(wxT("document-sql"));
	}

	AddPage(page, wxString::Format(format, NewPageNumber++), true, docBitmap);

	wxCommandEvent newEvent(mvceditor::EVENT_APP_FILE_NEW);
	EventSink->Publish(newEvent);
	
}

void mvceditor::NotebookClass::LoadPage() {
	wxString fileFilter = CreateWildcardString();
	wxFileDialog fileDialog(this, wxT("Open a File"), wxT(""), wxT(""), 
			fileFilter, wxFD_OPEN | wxFD_FILE_MUST_EXIST | 
			wxFD_MULTIPLE);
	if (wxID_OK == fileDialog.ShowModal()) {
		wxArrayString filenames;
		fileDialog.GetPaths(filenames);
		std::vector<wxString> fileVector;
		for (size_t i = 0; i < filenames.GetCount(); ++i) {
			fileVector.push_back(filenames[i]);
		}
		LoadPages(fileVector);
	}
}

void mvceditor::NotebookClass::LoadPage(const wxString& filename, bool doFreeze) {
	// when we used wxWidgets 2.8 we would freeze the notebook, add all pages,
	// then thaw the notebook
	// when upgrading to wxWidgets 2.9, the notebook get frozen/thawed once for
	// each new page; we can no longer safely  freeze/thaw here because then thaw
	// would be called n + 1 times and GTK does not like that.
	wxPlatformInfo info;
	if (info.GetOperatingSystemId() == wxOS_WINDOWS_NT && doFreeze) {
		this->Freeze();
	}
	bool found = false;
	
	// if file is already opened just bring it to the forefront
	for (size_t j = 0; j < GetPageCount(); ++j) {
		CodeControlClass* control = GetCodeControl(j);
		if (control && control->GetFileName() == filename) {

			// only set the selection when it's different
			// if it gets called for the window that's already opened
			// focus goes to the notebook. Meaning that if LoadPage gets
			// called on a file that is already at the forefront the 
			// code control will lose focus.
			if (GetSelection() != (int)j) {
				SetSelection(j);
			}
			found = true;
			break;
		}
	}
	if (!found) {
		UnicodeString fileContents;	

		// not using wxStyledTextCtrl::LoadFile() because it does not correctly handle files with high ascii characters
		mvceditor::FindInFilesClass::OpenErrors error = FindInFilesClass::FileContents(filename, fileContents);
		if (error == mvceditor::FindInFilesClass::NONE) {

			// make sure to use a unique ID, other source code depends on this
			CodeControlClass* newCode = new CodeControlClass(this, *CodeControlOptions, Globals, *EventSink, wxNewId());
			newCode->TrackFile(filename, fileContents);

			int mode = newCode->GetDocumentMode();
			wxBitmap docBitmap = mvceditor::IconImageAsset(wxT("document-text"));
			if (mvceditor::CodeControlClass::PHP == mode) {
				docBitmap = mvceditor::IconImageAsset(wxT("document-php"));
			}
			else if (mvceditor::CodeControlClass::CSS == mode) {
				docBitmap = mvceditor::IconImageAsset(wxT("document-css"));
			}
			else if (mvceditor::CodeControlClass::SQL == mode) {
				docBitmap = mvceditor::IconImageAsset(wxT("document-sql"));
			}

			// if user dragged in a file on an opened file we want still want to accept dragged files
			newCode->SetDropTarget(new FileDropTargetClass(this));
			wxFileName fileName(filename);
			this->AddPage(newCode, fileName.GetFullName(), true, docBitmap);

			// tell the app that a file has been opened
			wxCommandEvent openEvent(mvceditor::EVENT_APP_FILE_OPENED);
			openEvent.SetString(fileName.GetFullPath());
			EventSink->Publish(openEvent);
		}
		else if (error == mvceditor::FindInFilesClass::FILE_NOT_FOUND) {
			mvceditor::EditorLogError(mvceditor::ERR_INVALID_FILE, filename);
		}
		else if (mvceditor::FindInFilesClass::CHARSET_DETECTION == error) {
			mvceditor::EditorLogError(mvceditor::ERR_CHARSET_DETECTION, filename);
		}
	}
	if (info.GetOperatingSystemId() == wxOS_WINDOWS_NT && doFreeze) {
		this->Thaw();
	}
}

void mvceditor::NotebookClass::LoadPages(const std::vector<wxString>& filenames) {
	
	// when we used wxWidgets 2.8 we would freeze the notebook, add all pages,
	// then thaw the notebook
	// when upgrading to wxWidgets 2.9, the notebook get frozen/thawed once for
	// each new page; we can no longer safely  freeze/thaw here because then thaw
	// would be called n + 1 times and GTK does not like that.
	wxPlatformInfo info;
	if (info.GetOperatingSystemId() == wxOS_WINDOWS_NT) {
		this->Freeze();
	}
	for (size_t i = 0; i < filenames.size(); ++i) {
		LoadPage(filenames[i], false);
	}
	if (info.GetOperatingSystemId() == wxOS_WINDOWS_NT) {
		this->Thaw();
	}
}

void mvceditor::NotebookClass::RefreshCodeControlOptions() {
	for (size_t i = 0; i < GetPageCount(); ++i) {
		CodeControlClass* control = GetCodeControl(i);
		if (control) {
			control->ApplyPreferences();
		}
	}	
}

bool mvceditor::NotebookClass::SaveCurrentPage() {
	int currentPage = GetSelection();
	return SavePage(currentPage);
}

bool mvceditor::NotebookClass::SaveCurrentPageAsNew() {
	int currentPage = GetSelection();
	bool saved = false;
	CodeControlClass* codeCtrl = GetCodeControl(currentPage);
	if (codeCtrl) {
		if (codeCtrl->IsNew()) {
			SaveCurrentPage();
		}
		else {
	 		wxString fileFilter = CreateWildcardString();
			int filterIndex = WilcardIndex(codeCtrl->GetDocumentMode());
			wxFileDialog fileDialog(this, wxT("Save to a new PHP File"), wxT(""), wxT(""), 
					fileFilter, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
			fileDialog.SetFilterIndex(filterIndex);
			if (wxID_OK == fileDialog.ShowModal()) {
				
				// SaveAs should have no effect on the state of the
				// code control so dont use the control's save methods
				wxString newFullPath = fileDialog.GetPath();
				wxString code = mvceditor::IcuToWx(codeCtrl->GetSafeText());
				wxFile saveAsFile(newFullPath, wxFile::write);
				saved = saveAsFile.IsOpened() && saveAsFile.Write(code);
				if (!saved) {
					wxMessageBox(wxT("Could Not Save File."));
				}
				saveAsFile.Close();

				wxCommandEvent createdEvt(mvceditor::EVENT_APP_FILE_CREATED);
				createdEvt.SetString(newFullPath);
				EventSink->Publish(createdEvt);
			}
		}
	}
	return saved;
}

bool mvceditor::NotebookClass::SaveAllModifiedPages() {
	std::vector<int> modifiedPageIndexes;
	std::vector<wxString> modifiedPageNames;
	bool changed = true;
	GetModifiedPageNames(modifiedPageNames, modifiedPageIndexes);
	if (modifiedPageIndexes.size()) {
		wxArrayString modifiedPageNamesArray;
		for (size_t i = 0; i < modifiedPageNames.size(); ++i) {
			modifiedPageNamesArray.Add(modifiedPageNames[i]);
		}
		wxMultiChoiceDialog dialog(this, wxT(
			"Do you wish to save any files before exiting?"),
			wxT("Save Files"), modifiedPageNamesArray, 
			wxDEFAULT_DIALOG_STYLE | wxOK | wxCANCEL);
		if (wxID_CANCEL == dialog.ShowModal()) {
			changed = false;				
		}
		else {
			wxArrayInt selections = dialog.GetSelections();
			for (size_t i = 0; i < selections.size(); ++i) {
				SavePage(modifiedPageIndexes[selections[i]]);
			}
		}
	}
	return changed;
}

void mvceditor::NotebookClass::SaveAllModifiedPagesWithoutPrompting() {
	for (size_t i = 0; i < GetPageCount(); i++) {
		if (IsPageModified(i)) {
			SavePage(i);
		}
	}
}

bool mvceditor::NotebookClass::IsPageModified(int pageNumber) const {
	CodeControlClass* codeCtrl = GetCodeControl(pageNumber);
	return codeCtrl && codeCtrl->GetModify();
}

bool mvceditor::NotebookClass::GetModifiedPageNames(std::vector<wxString>& modifiedPageNames, std::vector<int>& modifiedPageIndexes) const {
	bool modified = false;
	for (size_t i = 0; i < GetPageCount(); ++i) {
		if (IsPageModified(i)) {
			modified = true;
			modifiedPageIndexes.push_back(i);
			wxString pageName = GetPageText(i);
			if (pageName.EndsWith(wxT("*"))) {
				pageName = pageName.SubString(0, pageName.size() - 2);
			}
			modifiedPageNames.push_back(pageName);
		}
	}
	return modified;
}

void mvceditor::NotebookClass::ShowContextMenu(wxAuiNotebookEvent& event) {
	if (NULL == ContextMenu) {
		CreateContextMenu();
	}
	PopupMenu(ContextMenu);
	event.Skip();
}

void mvceditor::NotebookClass::CreateContextMenu() {
	ContextMenu = new wxMenu;
	ContextMenu->Append(ID_SAVE_MODIFIED, wxT("Close All Tabs"));
}

void mvceditor::NotebookClass::OnCloseAllPages(wxCommandEvent& event) {
	if (SaveAllModifiedPages()) {
		CloseAllPages();
	}
}

void mvceditor::NotebookClass::CloseAllPages() {
	while (GetPageCount() > 0) {
		mvceditor::CodeControlClass* codeCtrl = GetCodeControl(0);
		mvceditor::CodeControlEventClass codeControlEvent(mvceditor::EVENT_APP_FILE_CLOSED, codeCtrl);
		EventSink->Publish(codeControlEvent);

		DeletePage(0);

		// notify owner that the tab has been closed
		// we must do it here; aui notebook's DeletePage does not
		// generate events
		wxAuiNotebookEvent evt(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED, this->GetId());

		// tab index
		evt.SetSelection(0);
		evt.SetEventObject(this);
		EventSink->Publish(evt);
	}
}

void mvceditor::NotebookClass::CloseCurrentPage() {
	int currentPage = GetSelection();
	bool isPageClosed = false;
	if (IsPageModified(currentPage)) {
		wxString pageName = GetPageText(currentPage);
		if (pageName.EndsWith(wxT("*"))) {
			pageName = pageName.SubString(0, pageName.size() -2);
		}
		wxString msg = pageName + wxT(" has not been saved. Save changes?");
		int response = wxMessageBox(msg, wxT("Save PHP File"), wxYES_NO | 
			wxCANCEL | wxICON_QUESTION, this);
		if (wxCANCEL != response) {
			if (wxYES == response && !SavePage(currentPage)) {
				//something drastic. dont know how to handle it
			}
			DeletePage(currentPage);
			isPageClosed = true;
		}
	}
	else {
		DeletePage(currentPage);
		isPageClosed = true;
	}
	if (isPageClosed) {

		// notify owner that the tab has been closed
		// we must do it here; aui notebook's DeletePage does not
		// generate events
		wxAuiNotebookEvent evt(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED, this->GetId());

		// tab index
		evt.SetSelection(currentPage);
		evt.SetEventObject(this);
		EventSink->Publish(evt);
	}
}

std::vector<wxString> mvceditor::NotebookClass::GetOpenedFiles() const {
	std::vector<wxString> files;
	for (size_t j = 0; j < GetPageCount(); ++j) {
		CodeControlClass* control = GetCodeControl(j);
		if (control) {
			files.push_back(control->GetFileName());
		}
	}
	return files;
}

wxString mvceditor::NotebookClass::CreateWildcardString() const {
	wxString phpLabel = Globals->PhpFileExtensionsString,
		cssLabel = Globals->CssFileExtensionsString,
		sqlLabel = Globals->SqlFileExtensionsString;
	phpLabel.Replace(wxT(";"), wxT(" "));
	cssLabel.Replace(wxT(";"), wxT(" "));
	sqlLabel.Replace(wxT(";"), wxT(" "));
	wxString php = Globals->PhpFileExtensionsString,
		css = Globals->CssFileExtensionsString,
		sql = Globals->SqlFileExtensionsString;

	wxString fileFilter = 
		wxString::Format(wxT("PHP Files (%s)|%s|"), phpLabel.c_str(), php.c_str()) +
		wxString::Format(wxT("CSS Files (%s)|%s|"), cssLabel.c_str(), css.c_str()) +
		wxString::Format(wxT("SQL Files (%s)|%s|"), sqlLabel.c_str(), sql.c_str()) +
		wxT("All Files (*.*)|*.*");
	return fileFilter;
}

int mvceditor::NotebookClass::WilcardIndex(mvceditor::CodeControlClass::Mode mode) {
	if (mvceditor::CodeControlClass::PHP == mode) {
		return 0;
	}
	else if (mvceditor::CodeControlClass::CSS == mode) {
		return 1;
	}
	else if (mvceditor::CodeControlClass::SQL == mode) {
		return 2;
	}
	return 3;
}

void mvceditor::NotebookClass::OnPageChanging(wxAuiNotebookEvent& event) {
	int selected = event.GetSelection();
	int num = GetPageCount();
	for (int i = 0; i < num; ++i) {
		mvceditor::CodeControlClass* ctrl = GetCodeControl(i);
		ctrl->SetAsHidden(i != selected);
	}
	event.Skip();
}

mvceditor::FileDropTargetClass::FileDropTargetClass(mvceditor::NotebookClass* notebook) :
	Notebook(notebook) {

}

bool mvceditor::FileDropTargetClass::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& files) {
	std::vector<wxString> fileVector;
	for (size_t i = 0; i < files.GetCount(); ++i) {
		fileVector.push_back(files[i]);
	}	
	Notebook->LoadPages(fileVector);
	return true;
}

BEGIN_EVENT_TABLE(mvceditor::NotebookClass, wxAuiNotebook)
	EVT_AUINOTEBOOK_PAGE_CLOSE(mvceditor::ID_CODE_NOTEBOOK, 
		mvceditor::NotebookClass::SavePageIfModified)
	EVT_AUINOTEBOOK_TAB_RIGHT_UP(mvceditor::ID_CODE_NOTEBOOK,
		mvceditor::NotebookClass::ShowContextMenu)
	EVT_MENU(ID_SAVE_MODIFIED, mvceditor::NotebookClass::OnCloseAllPages)

	// using OnPageChanging instead of OnPageChanged because onPageChanged
	// generates multiple events (not quite sure why yet)
	EVT_AUINOTEBOOK_PAGE_CHANGING(mvceditor::ID_CODE_NOTEBOOK, mvceditor::NotebookClass::OnPageChanging)
END_EVENT_TABLE()