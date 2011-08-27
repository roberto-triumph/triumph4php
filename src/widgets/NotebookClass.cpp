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
 * @author     $Author: robertop2004@gmail.com $
 * @date       $Date: 2011-08-13 20:31:56 -0700 (Sat, 13 Aug 2011) $
 * @version    $Rev: 596 $ 
 */
#include <widgets/NotebookClass.h>
#include <windows/StringHelperClass.h>
#include <wx/artprov.h>
#include <wx/filename.h>
#include <wx/file.h>

int ID_SAVE_MODIFIED = wxNewId();

mvceditor::NotebookClass::NotebookClass(wxWindow* parent, wxWindowID id, 
	const wxPoint& pos, const wxSize& size, long style)
	: wxAuiNotebook(parent, id, pos, size, style)
	, CodeControlOptions(NULL)
	, ContextMenu(NULL)
	, Project(NULL)
	, NewPageNumber(1) {
}

mvceditor::CodeControlClass* mvceditor::NotebookClass::GetCodeControl(size_t pageNumber) const {
	return pageNumber < GetPageCount() ? (CodeControlClass*)GetPage(pageNumber) : NULL;
}

mvceditor::CodeControlClass* mvceditor::NotebookClass::GetCurrentCodeControl() const {
	return GetCodeControl(GetSelection());
}

void mvceditor::NotebookClass::SavePageIfModified(wxAuiNotebookEvent& event) {
	int currentPage = event.GetSelection();
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
			event.Veto();
		}
	}
}

bool mvceditor::NotebookClass::SavePage(int pageIndex) {
	CodeControlClass* phpSourceCodeCtrl = GetCodeControl(
			pageIndex);
	bool saved = false;
	if (phpSourceCodeCtrl->IsNew()) {
		wxString phpFileFilter = wxT("PHP files (*.php)|*.php");
		wxFileDialog fileDialog(this, wxT("Save a PHP File"), wxT(""), wxT(""), 
				phpFileFilter, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
		if (wxID_OK == fileDialog.ShowModal()) {
			wxString newFullPath = fileDialog.GetPath();
			if (!phpSourceCodeCtrl->SavePhpFile(newFullPath)) {
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
			}
		}
	}
	else {
		if (!phpSourceCodeCtrl->SavePhpFile()) {
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
void mvceditor::NotebookClass::AddMvcEditorPage() {
	CodeControlClass* page = new CodeControlClass(this, *CodeControlOptions, Project, wxID_ANY);
	AddPage(page, wxString::Format(wxT("Untitled %d"), NewPageNumber++), true, 
		wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_TOOLBAR, 
		wxSize(16, 16)));
}

void mvceditor::NotebookClass::LoadPage() {
	wxString phpFileFilter = wxT("PHP files (*.php)|*.php");
	wxFileDialog fileDialog(this, wxT("Open a PHP File"), wxT(""), wxT(""), 
			phpFileFilter, wxFD_OPEN | wxFD_FILE_MUST_EXIST | 
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

void mvceditor::NotebookClass::LoadPage(const wxString& filename) {
	bool found = false;
	
	// if file is already opened just bring it to the forefront
	for (size_t j = 0; j < GetPageCount(); ++j) {
		CodeControlClass* control = GetCodeControl(j);
		if (control->GetFileName() == filename) {

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
		CodeControlClass* newCode = new CodeControlClass(
			this, *CodeControlOptions, Project, wxID_ANY);
		if (newCode->LoadPhpFile(filename)) {
			// if user dragged in a file on an opened file we want still want to accept dragged files
			newCode->SetDropTarget(new FileDropTargetClass(this));
			wxFileName fileName(filename);
			this->AddPage(newCode, fileName.GetFullName(), true, 
				wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_TOOLBAR, 
				wxSize(16, 16)));
		}
		else {
			
			// else probably should delete window since it's not going to 
			// get used...
			 newCode->Destroy();
		}
	}
}

void mvceditor::NotebookClass::LoadPages(const std::vector<wxString>& filenames) {
	for (size_t i = 0; i < filenames.size(); ++i) {
		LoadPage(filenames[i]);
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
	 		wxString phpFileFilter = wxT("PHP files (*.php)|*.php");
			wxFileDialog fileDialog(this, wxT("Save to a new PHP File"), wxT(""), wxT(""), 
					phpFileFilter, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
			if (wxID_OK == fileDialog.ShowModal()) {
				
				// SaveAs should have no effect on the state of the
				// code control so dont use the control's save methods
				wxString newFullPath = fileDialog.GetPath();
				wxString code = StringHelperClass::IcuToWx(codeCtrl->GetSafeText());
				wxFile saveAsFile(newFullPath, wxFile::write);
				saved = saveAsFile.IsOpened() && saveAsFile.Write(code);
				if (!saved) {
					wxMessageBox(wxT("Could Not Save File."));
				}
				saveAsFile.Close();
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

void mvceditor::NotebookClass::SetProject(ProjectClass* project) {
	Project = project;
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
	while (GetPageCount()) {
		DeletePage(0);
	}
}

void mvceditor::NotebookClass::CloseCurrentPage() {
	int currentPage = GetSelection();
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
		}
	}
	else {
		DeletePage(currentPage);
	}
}

std::vector<wxString> mvceditor::NotebookClass::GetOpenedFiles() const {
	std::vector<wxString> files;
	for (size_t j = 0; j < GetPageCount(); ++j) {
		CodeControlClass* control = GetCodeControl(j);
		files.push_back(control->GetFileName());
	}
	return files;
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
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, 
		mvceditor::NotebookClass::SavePageIfModified)
	EVT_AUINOTEBOOK_TAB_RIGHT_UP(wxID_ANY,
		mvceditor::NotebookClass::ShowContextMenu)
	EVT_MENU(ID_SAVE_MODIFIED, mvceditor::NotebookClass::OnCloseAllPages)
END_EVENT_TABLE()