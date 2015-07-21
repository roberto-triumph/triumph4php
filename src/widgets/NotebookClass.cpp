/**
 * @copyright  2009-2011 Roberto Perpuly
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
#include "widgets/NotebookClass.h"
#include <wx/artprov.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/wupdlock.h>
#include <vector>
#include "code_control/CodeControlClass.h"
#include "globals/Assets.h"
#include "globals/Errors.h"
#include "globals/Events.h"
#include "globals/GlobalsClass.h"
#include "globals/Number.h"
#include "globals/String.h"
#include "main_frame/PreferencesClass.h"
#include "search/FindInFilesClass.h"
#include "Triumph.h"
#include "widgets/AuiManager.h"
#include "widgets/FileTypeImageList.h"

static const int ID_CLOSE_ALL_TABS = wxNewId();
static const int ID_CLOSE_TAB = wxNewId();
static const int ID_SPLIT_HORIZONTALLY = wxNewId();
static const int ID_SPLIT_VERTICALLY = wxNewId();

// we only have 5 notebooks at most right now
// see widgets/AuiManager.h for more details
static const int ID_MOVE_TAB_TO_NOTEBOOK_1 = wxNewId();
static const int ID_MOVE_TAB_TO_NOTEBOOK_2 = wxNewId();
static const int ID_MOVE_TAB_TO_NOTEBOOK_3 = wxNewId();
static const int ID_MOVE_TAB_TO_NOTEBOOK_4 = wxNewId();
static const int ID_MOVE_TAB_TO_NOTEBOOK_5 = wxNewId();

int t4p::NotebookClass::NewPageNumber = 1;

t4p::NotebookClass::NotebookClass(wxWindow* parent, wxWindowID id,
	const wxPoint& pos, const wxSize& size, long style, const wxString& name)
	: wxAuiNotebook(parent, id, pos, size, style)
	, CodeControlOptions(NULL)
	, Globals(NULL)
	, Preferences(NULL)
	, EventSink(NULL)
	, AuiManager(NULL)
	, TabIndexRightClickEvent(-1) {
	ImageList = NULL;
	SetName(name);

	// when the notebook is empty we want to accept dragged files
	SetDropTarget(new FileDropTargetClass(this));

	Connect(wxEVT_AUINOTEBOOK_PAGE_CLOSE, wxAuiNotebookEventHandler(t4p::NotebookClass::SavePageIfModified), NULL, this);
	Connect(wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, wxAuiNotebookEventHandler(t4p::NotebookClass::ShowContextMenu), NULL, this);

	// using OnPageChanging instead of OnPageChanged because onPageChanged
	// generates multiple events (not quite sure why yet)
	Connect(wxEVT_AUINOTEBOOK_PAGE_CHANGING, wxAuiNotebookEventHandler(t4p::NotebookClass::OnPageChanging), NULL, this);
	Connect(wxEVT_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler(t4p::NotebookClass::OnPageChanged), NULL, this);
}

t4p::NotebookClass::~NotebookClass() {
	// delete the DropTarget that was created in the constructor
	SetDropTarget(NULL);

	Disconnect(wxEVT_AUINOTEBOOK_PAGE_CLOSE, wxAuiNotebookEventHandler(t4p::NotebookClass::SavePageIfModified), NULL, this);
	Disconnect(wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, wxAuiNotebookEventHandler(t4p::NotebookClass::ShowContextMenu), NULL, this);
	Disconnect(wxEVT_AUINOTEBOOK_PAGE_CHANGING, wxAuiNotebookEventHandler(t4p::NotebookClass::OnPageChanging), NULL, this);
	Disconnect(wxEVT_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler(t4p::NotebookClass::OnPageChanged), NULL, this);
}

void t4p::NotebookClass::InitApp(t4p::CodeControlOptionsClass* options,
		t4p::PreferencesClass* preferences,
		t4p::GlobalsClass* globals,
		t4p::EventSinkClass* eventSink,
		wxAuiManager* auiManager) {
	CodeControlOptions = options;
	Globals = globals;
	Preferences = preferences;
	EventSink = eventSink;
	AuiManager = auiManager;

	// so that all dialogs / panels use the same font
	// ATTN: on linux, default fonts are too big
	//       this code makes them smaller
	wxPlatformInfo info;
	if (info.GetOperatingSystemId() == wxOS_UNIX_LINUX) {
		// so that the tabs use the same font
		SetFont(preferences->ApplicationFont);
		SetNormalFont(preferences->ApplicationFont);
	}
}

t4p::CodeControlClass* t4p::NotebookClass::GetCodeControl(size_t pageNumber) const {
	t4p::CodeControlClass* codeControl = NULL;
	if (pageNumber < GetPageCount()) {
		wxWindow* window = GetPage(pageNumber);
		codeControl = (t4p::CodeControlClass*)window;
	}
	return codeControl;
}

t4p::CodeControlClass* t4p::NotebookClass::FindCodeControl(const wxString& fullPath) const {
	for (size_t i = 0; i < GetPageCount(); ++i) {
		t4p::CodeControlClass* ctrl = GetCodeControl(i);
		if (ctrl->GetFileName() == fullPath) {
			return ctrl;
		}
	}
	return NULL;
}

t4p::CodeControlClass* t4p::NotebookClass::GetCurrentCodeControl() const {
	return GetCodeControl(GetSelection());
}

void t4p::NotebookClass::Adopt(t4p::CodeControlClass* codeCtrl, t4p::NotebookClass* src) {
	int pageIndex = src->GetPageIndex(codeCtrl);
	if (pageIndex == wxNOT_FOUND) {
		return;
	}
	wxString tabName = src->GetPageText(pageIndex);
	src->RemovePage(pageIndex);
	AdoptOrphan(codeCtrl, tabName);
}

void t4p::NotebookClass::AdoptOrphan(t4p::CodeControlClass* codeCtrl, wxString tabName) {
	if (NULL == ImageList) {
		ImageList = new wxImageList(16, 16);
		t4p::FileTypeImageList(*ImageList);
		AssignImageList(ImageList);
	}

	// GetPageImage is not implemented in wxAuiNotebook
	int tabImageId = t4p::FileTypeImageIdFromType(codeCtrl->GetFileType());
	AddPage(codeCtrl, tabName, false, tabImageId);

	// notify the app that a page was moved
	t4p::CodeControlEventClass moveEvt(t4p::EVENT_APP_FILE_NOTEBOOK_CHANGED, codeCtrl);
	moveEvt.SetEventObject(this);
	EventSink->Publish(moveEvt);
}

void t4p::NotebookClass::SavePageIfModified(wxAuiNotebookEvent& event) {
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
				!SavePage(currentPage, true))) {
			vetoed = false;
			event.Veto();
		}
	}
	if (!vetoed && codeCtrl) {
		// tell the app that a file has been closed
		t4p::CodeControlEventClass codeControlEvent(t4p::EVENT_APP_FILE_CLOSED, codeCtrl);
		EventSink->Publish(codeControlEvent);
	}
	if (!vetoed) {
		event.Skip();
	}
}

bool t4p::NotebookClass::SavePage(int pageIndex, bool willDestroy) {
	CodeControlClass* phpSourceCodeCtrl = GetCodeControl(
			pageIndex);
	bool saved = false;
	if (phpSourceCodeCtrl && phpSourceCodeCtrl->IsNew()) {
		wxString fileFilter = CreateWildcardString();
		int filterIndex = WilcardIndex(phpSourceCodeCtrl->GetFileType());
		wxFileDialog fileDialog(this, wxT("Save a File"), wxT(""), wxT(""),
				fileFilter, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
		fileDialog.SetFilterIndex(filterIndex);
		if (wxID_OK == fileDialog.ShowModal()) {
			wxString newFullPath = fileDialog.GetPath();
			if (!phpSourceCodeCtrl->SaveAndTrackFile(newFullPath, willDestroy)) {
				wxMessageBox(wxT("Could Not Save File."));
			} else {
				wxString newFilename, extension;
				bool hasExtension = false;
				wxFileName::SplitPath(newFullPath, NULL, NULL, &newFilename,
					&extension, &hasExtension);
				newFilename = hasExtension ? newFilename + wxT(".") + extension
					: newFilename;
				SetPageText(pageIndex, newFilename);
				saved = true;

				wxCommandEvent createdEvt(t4p::EVENT_APP_FILE_CREATED);
				createdEvt.SetString(newFullPath);
				EventSink->Publish(createdEvt);
			}
		}
	} else {
		if (phpSourceCodeCtrl && !phpSourceCodeCtrl->SaveAndTrackFile(wxT(""), willDestroy)) {
			wxMessageBox(wxT("Could Not Save File."));
		} else {
			saved = true;
		}
	}
	return saved;
}

void t4p::NotebookClass::MarkPageAsModified(int windowId) {
	int pageNumber = GetPageIndex(FindWindow(windowId));
	wxString filename = GetPageText(pageNumber);
	if (!filename.EndsWith(wxT("*"))) {
		filename = filename + wxT("*");
		SetPageText(pageNumber, filename);
	}
}

void t4p::NotebookClass::MarkPageAsNotModified(int windowId) {
	int pageNumber = GetPageIndex(FindWindow(windowId));
	wxString filename = GetPageText(pageNumber);
	if (filename.EndsWith(wxT("*"))) {
		filename = filename.SubString(0, filename.size() - 2);
		SetPageText(pageNumber, filename);
	}
}
void t4p::NotebookClass::AddTriumphPage(t4p::FileType type) {
	if (NULL == ImageList) {
		ImageList = new wxImageList(16, 16);
		t4p::FileTypeImageList(*ImageList);
		AssignImageList(ImageList);
	}
	wxString format;
	switch (type) {
		case t4p::FILE_TYPE_TEXT:
			format = _("Untitled %d.txt");
			break;
		case t4p::FILE_TYPE_SQL:
			format = _("Untitled %d.sql");
			break;
		case t4p::FILE_TYPE_CSS:
			format = _("Untitled %d.css");
			break;
		case t4p::FILE_TYPE_PHP:
			format = _("Untitled %d.php");
			break;
		case t4p::FILE_TYPE_JS:
			format = _("Untitled %d.js");
			break;
		case t4p::FILE_TYPE_CONFIG:
			format = _("Untitled %d.conf");
			break;
		case t4p::FILE_TYPE_CRONTAB:
			format = _("Untitled %d");
			break;
		case t4p::FILE_TYPE_YAML:
			format = _("Untitled %d.yml");
			break;
		case t4p::FILE_TYPE_XML:
			format = _("Untitled %d.xml");
			break;
		case t4p::FILE_TYPE_RUBY:
			format = _("Untitled %d.rb");
			break;
		case t4p::FILE_TYPE_LUA:
			format = _("Untitled %d.lua");
			break;
		case t4p::FILE_TYPE_MARKDOWN:
			format = _("Untitled %d.md");
			break;
		case t4p::FILE_TYPE_BASH:
			format = _("Untitled %d.sh");
			break;
		case t4p::FILE_TYPE_DIFF:
			format = _("Untitled %d.diff");
			break;
	}

	// make sure to use a unique ID, other source code depends on this
	CodeControlClass* page = new CodeControlClass(this, *CodeControlOptions, Globals, *EventSink, wxNewId());
	page->SetFileType(type);

	int imgId = t4p::FileTypeImageId(Globals->FileTypes, wxFileName(format));

	AddPage(page, wxString::Format(format, NewPageNumber++), true, imgId);

	t4p::CodeControlEventClass newEvent(t4p::EVENT_APP_FILE_NEW, page);
	EventSink->Publish(newEvent);
}

void t4p::NotebookClass::LoadPage() {
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

void t4p::NotebookClass::LoadPage(const wxString& filename, bool doFreeze) {
	if (NULL == ImageList) {
		ImageList = new wxImageList(16, 16);
		t4p::FileTypeImageList(*ImageList);
		AssignImageList(ImageList);
	}

	// when we used wxWidgets 2.8 we would freeze the notebook, add all pages,
	// then thaw the notebook
	// when upgrading to wxWidgets 2.9, the notebook get frozen/thawed once for
	// each new page; we can no longer safely  freeze/thaw here because then thaw
	// would be called n + 1 times and GTK does not like that.
	wxPlatformInfo info;
	if (info.GetOperatingSystemId() != wxOS_UNIX_LINUX && doFreeze) {
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
			if (!t4p::NumberEqualTo(GetSelection(), j)) {
				SetSelection(j);
			}
			found = true;
			break;
		}
	}
	if (!found) {
		UnicodeString fileContents;

		// not using wxStyledTextCtrl::LoadFile() because it does not correctly handle files with high ascii characters
		bool hasSignature = false;
		wxString charset;
		t4p::FindInFilesClass::OpenErrors error = FindInFilesClass::FileContents(filename, fileContents, charset, hasSignature);
		if (error == t4p::FindInFilesClass::NONE) {
			// make sure to use a unique ID, other source code depends on this
			CodeControlClass* newCode = new CodeControlClass(this, *CodeControlOptions, Globals, *EventSink, wxNewId());
			newCode->TrackFile(filename, fileContents, charset, hasSignature);

			int imgId = t4p::FileTypeImageId(Globals->FileTypes, wxFileName(filename));

			// if user dragged in a file on an opened file we want still want to accept dragged files
			newCode->SetDropTarget(new FileDropTargetClass(this));
			wxFileName fileName(filename);
			this->AddPage(newCode, fileName.GetFullName(), true, imgId);

			// tell the app that a file has been opened
			t4p::CodeControlEventClass openEvent(t4p::EVENT_APP_FILE_OPENED, newCode);
			EventSink->Publish(openEvent);
		} else if (error == t4p::FindInFilesClass::FILE_NOT_FOUND) {
			t4p::EditorLogError(t4p::ERR_INVALID_FILE, filename);
		} else if (t4p::FindInFilesClass::CHARSET_DETECTION == error) {
			t4p::EditorLogError(t4p::ERR_CHARSET_DETECTION, filename);
		} else if (t4p::FindInFilesClass::FILE_TOO_LARGE == error) {
			t4p::EditorLogError(t4p::ERR_FILE_TOO_LARGE, filename);
		}
	}
	if (info.GetOperatingSystemId() != wxOS_UNIX_LINUX && doFreeze) {
		this->Thaw();
	}
}

void t4p::NotebookClass::LoadPages(const std::vector<wxString>& filenames) {
	// when we used wxWidgets 2.8 we would freeze the notebook, add all pages,
	// then thaw the notebook
	// when upgrading to wxWidgets 2.9, the notebook get frozen/thawed once for
	// each new page; we can no longer safely  freeze/thaw here because then thaw
	// would be called n + 1 times and GTK does not like that.
	wxPlatformInfo info;
	if (info.GetOperatingSystemId() != wxOS_UNIX_LINUX) {
		this->Freeze();
	}
	for (size_t i = 0; i < filenames.size(); ++i) {
		LoadPage(filenames[i], false);
	}
	if (info.GetOperatingSystemId() != wxOS_UNIX_LINUX) {
		this->Thaw();
	}
}

void t4p::NotebookClass::RefreshCodeControlOptions() {
	for (size_t i = 0; i < GetPageCount(); ++i) {
		CodeControlClass* control = GetCodeControl(i);
		if (control) {
			control->ApplyPreferences();
		}
	}
}

bool t4p::NotebookClass::SaveCurrentPage() {
	int currentPage = GetSelection();
	return SavePage(currentPage, false);
}

bool t4p::NotebookClass::SaveCurrentPageAsNew() {
	int currentPage = GetSelection();
	bool saved = false;
	CodeControlClass* codeCtrl = GetCodeControl(currentPage);
	if (codeCtrl) {
		if (codeCtrl->IsNew()) {
			SaveCurrentPage();
		} else {
	 		wxString fileFilter = CreateWildcardString();
			int filterIndex = WilcardIndex(codeCtrl->GetFileType());
			wxFileDialog fileDialog(this, wxT("Save to a new PHP File"), wxT(""), wxT(""),
					fileFilter, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
			fileDialog.SetFilterIndex(filterIndex);
			if (wxID_OK == fileDialog.ShowModal()) {
				// SaveAs should have no effect on the state of the
				// code control so dont use the control's save methods
				wxString newFullPath = fileDialog.GetPath();
				wxString code = t4p::IcuToWx(codeCtrl->GetSafeText());
				wxFile saveAsFile(newFullPath, wxFile::write);
				saved = saveAsFile.IsOpened() && saveAsFile.Write(code);
				if (!saved) {
					wxMessageBox(wxT("Could Not Save File."));
				}
				saveAsFile.Close();

				wxCommandEvent createdEvt(t4p::EVENT_APP_FILE_CREATED);
				createdEvt.SetString(newFullPath);
				EventSink->Publish(createdEvt);
			}
		}
	}
	return saved;
}

bool t4p::NotebookClass::SaveAllModifiedPages() {
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
		dialog.Center();
		if (wxID_CANCEL == dialog.ShowModal()) {
			changed = false;
		} else {
			wxArrayInt selections = dialog.GetSelections();
			for (size_t i = 0; i < selections.size(); ++i) {
				SavePage(modifiedPageIndexes[selections[i]], false);
			}
		}
	}
	return changed;
}

void t4p::NotebookClass::SaveAllModifiedPagesWithoutPrompting() {
	for (size_t i = 0; i < GetPageCount(); i++) {
		if (IsPageModified(i)) {
			SavePage(i, false);
		}
	}
}

bool t4p::NotebookClass::IsPageModified(int pageNumber) const {
	CodeControlClass* codeCtrl = GetCodeControl(pageNumber);
	return codeCtrl && codeCtrl->GetModify();
}

bool t4p::NotebookClass::GetModifiedPageNames(std::vector<wxString>& modifiedPageNames, std::vector<int>& modifiedPageIndexes) const {
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

void t4p::NotebookClass::ShowContextMenu(wxAuiNotebookEvent& event) {
	TabIndexRightClickEvent = event.GetSelection();
	std::vector<t4p::NotebookClass*> notebooks = t4p::AuiVisibleCodeNotebooks(*AuiManager);
	t4p::NotebookClass* hiddenNotebook = t4p::AuiNextHiddenCodeNotebook(*AuiManager);

	wxMenu menu;
	menu.Append(ID_CLOSE_ALL_TABS, wxT("Close All Tabs"));
	menu.Append(ID_CLOSE_TAB, wxT("Close This Tab"));
	if (hiddenNotebook) {
		menu.Append(ID_SPLIT_VERTICALLY, wxT("Split Vertically"));
		menu.Append(ID_SPLIT_HORIZONTALLY, wxT("Split Horizontally"));
	}
	if (notebooks.size() == 2) {
		menu.Append(ID_MOVE_TAB_TO_NOTEBOOK_1, wxT("Move This Tab To Other Notebook"));
	} else if (notebooks.size() > 2) {
		for (size_t i = 0; i < notebooks.size(); i++) {
			if (notebooks[i] != this) {
				wxAuiPaneInfo info = AuiManager->GetPane(notebooks[i]);
				wxString menuItem = wxT("Move This Tab To ") + info.caption;
				menu.Append(ID_MOVE_TAB_TO_NOTEBOOK_1 + i, menuItem);
			}
		}
	}

	PopupMenu(&menu);
}

void t4p::NotebookClass::OnCloseAllPages(wxCommandEvent& event) {
	if (SaveAllModifiedPages()) {
		CloseAllPages();
	}
}

void t4p::NotebookClass::OnMovePage(wxCommandEvent& event) {
	t4p::CodeControlClass* ctrl = GetCurrentCodeControl();
	if (!ctrl) {
		return;
	}

	std::vector<t4p::NotebookClass*> notebooks = t4p::AuiVisibleCodeNotebooks(*AuiManager);

	// when there are 2  notebooks, then just move from one to the other
	if (notebooks.size() == 2) {
		if (notebooks[0] == this) {
			notebooks[1]->Adopt(ctrl, this);
		} else if (notebooks[1] == this) {
			notebooks[0]->Adopt(ctrl, this);
		}
	} else {
		int index = event.GetId() - ID_MOVE_TAB_TO_NOTEBOOK_1;
		if (index >= 0 && t4p::NumberLessThan(index, notebooks.size())) {
			t4p::NotebookClass* destNotebook = notebooks[index];
			destNotebook->Adopt(ctrl, this);
		}
	}
}

void t4p::NotebookClass::CloseAllPages() {
	while (GetPageCount() > 0) {
		t4p::CodeControlClass* codeCtrl = GetCodeControl(0);
		t4p::CodeControlEventClass codeControlEvent(t4p::EVENT_APP_FILE_CLOSED, codeCtrl);
		EventSink->Publish(codeControlEvent);

		DeletePage(0);

		// notify owner that the tab has been closed
		// we must do it here; aui notebook's DeletePage does not
		// generate events
		wxAuiNotebookEvent evt(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, this->GetId());

		// tab index
		evt.SetSelection(0);
		evt.SetEventObject(this);
		ProcessEvent(evt);
	}
}

void t4p::NotebookClass::CloseCurrentPage() {
	int currentPage = GetSelection();
	ClosePage(currentPage);
}

void t4p::NotebookClass::ClosePage(int index) {
	bool doDeletePage = false;
	if (IsPageModified(index)) {
		wxString pageName = GetPageText(index);
		if (pageName.EndsWith(wxT("*"))) {
			pageName = pageName.SubString(0, pageName.size() -2);
		}
		wxString msg = pageName + wxT(" has not been saved. Save changes?");
			int response = wxMessageBox(msg, wxT("Save PHP File"), wxYES_NO |
			wxCANCEL | wxICON_QUESTION, this);
		if (wxCANCEL != response) {
			if (wxYES == response && !SavePage(index, true)) {
				// something drastic. dont know how to handle it
			}
			doDeletePage = true;
		}
	} else {
		doDeletePage = true;
	}
	if (doDeletePage) {
		DeletePage(index);

		// notify owner that the tab has been closed
		// we must do it here; aui notebook's DeletePage does not
		// generate events
		wxAuiNotebookEvent evt(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED, this->GetId());

		// tab index
		evt.SetSelection(index);
		evt.SetEventObject(this);
		ProcessEvent(evt);
	}
}

std::vector<wxString> t4p::NotebookClass::GetOpenedFiles() const {
	std::vector<wxString> files;
	for (size_t j = 0; j < GetPageCount(); ++j) {
		CodeControlClass* control = GetCodeControl(j);
		if (control) {
			files.push_back(control->GetFileName());
		}
	}
	return files;
}

wxString t4p::NotebookClass::CreateWildcardString() const {
	wxString phpLabel = Globals->FileTypes.PhpFileExtensionsString,
		cssLabel = Globals->FileTypes.CssFileExtensionsString,
		sqlLabel = Globals->FileTypes.SqlFileExtensionsString,
		jsLabel = Globals->FileTypes.JsFileExtensionsString,
		configLabel = Globals->FileTypes.ConfigFileExtensionsString,
		crontabLabel = Globals->FileTypes.CrontabFileExtensionsString,
		yamlLabel = Globals->FileTypes.YamlFileExtensionsString,
		xmlLabel = Globals->FileTypes.XmlFileExtensionsString,
		rubyLabel = Globals->FileTypes.RubyFileExtensionsString,
		luaLabel = Globals->FileTypes.LuaFileExtensionsString,
		markdownLabel = Globals->FileTypes.MarkdownFileExtensionsString,
		bashLabel = Globals->FileTypes.BashFileExtensionsString,
		diffLabel = Globals->FileTypes.DiffFileExtensionsString;

	/*phpLabel.Replace(wxT(";"), wxT(" "));
	cssLabel.Replace(wxT(";"), wxT(" "));
	sqlLabel.Replace(wxT(";"), wxT(" "));
	jsLabel.Replace(wxT(";"), wxT(" "));
	configLabel.Replace(wxT(";"), wxT(" "));
	crontabLabel.Replace(wxT(";"), wxT(" "));
	yamlLabel.Replace(wxT(";"), wxT(" "));
	xmlLabel.Replace(wxT(";"), wxT(" "));
	rubyLabel.Replace(wxT(";"), wxT(" "));
	luaLabel.Replace(wxT(";"), wxT(" "));
	markdownLabel.Replace(wxT(";"), wxT(" "));
	bashLabel.Replace(wxT(";"), wxT(" "));
	diffLabel.Replace(wxT(";"), wxT(" "));
	*/
	wxString php = Globals->FileTypes.PhpFileExtensionsString,
		css = Globals->FileTypes.CssFileExtensionsString,
		sql = Globals->FileTypes.SqlFileExtensionsString,
		js = Globals->FileTypes.JsFileExtensionsString,
		config = Globals->FileTypes.ConfigFileExtensionsString,
		crontab = Globals->FileTypes.CrontabFileExtensionsString,
		yaml = Globals->FileTypes.YamlFileExtensionsString,
		xml = Globals->FileTypes.XmlFileExtensionsString,
		ruby = Globals->FileTypes.RubyFileExtensionsString,
		lua = Globals->FileTypes.LuaFileExtensionsString,
		markdown = Globals->FileTypes.MarkdownFileExtensionsString,
		bash = Globals->FileTypes.BashFileExtensionsString,
		diff = Globals->FileTypes.DiffFileExtensionsString;
	wxString allSourceCode =
			Globals->FileTypes.GetAllSourceFileExtensionsString();

	wxString fileFilter =
		wxString::Format(wxT("All Source Code Files (%s)|%s|"), allSourceCode.c_str(), allSourceCode.c_str()) +
		wxString::Format(wxT("PHP Files (%s)|%s|"), phpLabel.c_str(), php.c_str()) +
		wxString::Format(wxT("CSS Files (%s)|%s|"), cssLabel.c_str(), css.c_str()) +
		wxString::Format(wxT("SQL Files (%s)|%s|"), sqlLabel.c_str(), sql.c_str()) +
		wxString::Format(wxT("Javascript Files (%s)|%s|"), jsLabel.c_str(), js.c_str()) +
		wxString::Format(wxT("Config Files (%s)|%s|"), configLabel.c_str(), config.c_str()) +
		wxString::Format(wxT("Crontab Files (%s)|%s|"), crontabLabel.c_str(), crontab.c_str()) +
		wxString::Format(wxT("YAML Files (%s)|%s|"), yamlLabel.c_str(), yaml.c_str()) +
		wxString::Format(wxT("XML Files (%s)|%s|"), xmlLabel.c_str(), xml.c_str()) +
		wxString::Format(wxT("Ruby Files (%s)|%s|"), rubyLabel.c_str(), ruby.c_str()) +
		wxString::Format(wxT("Lua Files (%s)|%s|"), luaLabel.c_str(), lua.c_str()) +
		wxString::Format(wxT("Markdown Files (%s)|%s|"), markdownLabel.c_str(), markdown.c_str()) +
		wxString::Format(wxT("Bash Files (%s)|%s|"), bashLabel.c_str(), bash.c_str()) +
		wxString::Format(wxT("Diff Files (%s)|%s|"), diffLabel.c_str(), diff.c_str()) +
		wxT("All Files (*.*)|*.*");
	return fileFilter;
}

int t4p::NotebookClass::WilcardIndex(t4p::FileType type) {
	if (t4p::FILE_TYPE_PHP == type) {
		return 0;
	} else if (t4p::FILE_TYPE_CSS == type) {
		return 1;
	} else if (t4p::FILE_TYPE_SQL == type) {
		return 2;
	} else if (t4p::FILE_TYPE_JS == type) {
		return 3;
	}
	return 4;
}

void t4p::NotebookClass::OnPageChanging(wxAuiNotebookEvent& event) {
	int selected = event.GetSelection();
	int num = GetPageCount();
	for (int i = 0; i < num; ++i) {
		t4p::CodeControlClass* ctrl = GetCodeControl(i);
		ctrl->SetAsHidden(i != selected);
	}
	event.Skip();
}

void t4p::NotebookClass::OnPageChanged(wxAuiNotebookEvent& event) {
	int selected = event.GetSelection();
	t4p::CodeControlClass* codeCtrl = GetCodeControl(selected);
	if (codeCtrl) {
		t4p::CodeControlEventClass ctrlEvt(t4p::EVENT_APP_FILE_PAGE_CHANGED, codeCtrl);
		EventSink->Publish(ctrlEvt);
	}
	event.Skip();
}


void t4p::NotebookClass::OnMenuClosePage(wxCommandEvent& event) {
	// get the tab that was right clicked; the tab right menu event holds
	// the index of the tab we want to close
	if (t4p::NumberLessThan(TabIndexRightClickEvent, GetPageCount())) {
		ClosePage(TabIndexRightClickEvent);
		TabIndexRightClickEvent  = -1;
	}
}


void t4p::NotebookClass::SplitHorizontally() {
	t4p::NotebookClass* newNotebook = t4p::AuiNextHiddenCodeNotebook(*AuiManager);
	if (!newNotebook) {
		return;
	}

	wxSize newNotebookSize = GetSize();
	wxAuiPaneInfo currentNotebookInfo = AuiManager->GetPane(this);
	int row = currentNotebookInfo.dock_row;
	int position = currentNotebookInfo.dock_pos;
	int layer = currentNotebookInfo.dock_layer;
	int insertLevel = wxAUI_INSERT_ROW;
	int direction = currentNotebookInfo.dock_direction;

	// splitting horizontally
	// if we are splitting the center notebook, then
	// we split horizontally by putting the new notebook
	// in a new dock position (bottom)
	// see widgets/AuiManager.h for more info about AUI docks,
	// row, and position.
	if (currentNotebookInfo.dock_direction == wxAUI_DOCK_CENTER) {
		direction = wxAUI_DOCK_BOTTOM;

		// 2 because we always want the finder panel and the tools
		// notebook to be closest to the bottom
		row = 2;
		insertLevel = wxAUI_INSERT_ROW;
	} else if (currentNotebookInfo.dock_direction == wxAUI_DOCK_RIGHT) {
		position++;
		insertLevel = wxAUI_INSERT_PANE;
	} else if (currentNotebookInfo.dock_direction == wxAUI_DOCK_BOTTOM) {
		row++;
		insertLevel = wxAUI_INSERT_ROW;
	}
	newNotebookSize.Scale(1, 0.5);

	if (GetPageCount() > 1) {
		newNotebook->Adopt(GetCurrentCodeControl(), this);
	} else {
		newNotebook->AddTriumphPage(t4p::FILE_TYPE_PHP);
	}
	AuiManager->InsertPane(newNotebook,
		wxAuiPaneInfo()
			.Layer(layer).Row(row).Position(position)
			.Direction(direction)
			.Gripper(false).Resizable(true).Floatable(false)
			.Resizable(true).PaneBorder(false).CaptionVisible(false)
			.CloseButton(false),
		insertLevel
	);
	AuiManager->GetPane(newNotebook).Show();

	// when there are more than 2 notebooks, given them names so
	// that the user can tell them apart (for "moving tabs" purposes)
	std::vector<t4p::NotebookClass*> notebooks = t4p::AuiVisibleCodeNotebooks(*AuiManager);
	if (notebooks.size() > 2) {
		for (size_t i = 0; i < notebooks.size(); i++) {
			wxAuiPaneInfo& info = AuiManager->GetPane(notebooks[i]);
			info.CaptionVisible(true);
		}
	}

	AuiManager->Update();
}

void t4p::NotebookClass::SplitVertically() {
	t4p::NotebookClass* newNotebook = t4p::AuiNextHiddenCodeNotebook(*AuiManager);
	if (!newNotebook) {
		return;
	}
	wxSize newNotebookSize = GetSize();
	wxAuiPaneInfo currentNotebookInfo = AuiManager->GetPane(this);
	int row = currentNotebookInfo.dock_row;
	int position = currentNotebookInfo.dock_pos;
	int layer = currentNotebookInfo.dock_layer;
	int insertLevel = wxAUI_INSERT_PANE;
	int direction = currentNotebookInfo.dock_direction;

	// splitting vertically
	// if we are splitting the center notebook, then
	// we split vertically by putting the new notebook
	// in a new dock row (right)
	// see widgets/AuiManager.h for more info about AUI docks,
	// row, and position.
	if (currentNotebookInfo.dock_direction == wxAUI_DOCK_CENTER) {
		direction = wxAUI_DOCK_RIGHT;
		row = t4p::AuiRowCount(*AuiManager, wxAUI_DOCK_RIGHT);
		insertLevel = wxAUI_INSERT_ROW;
	} else if (currentNotebookInfo.dock_direction == wxAUI_DOCK_RIGHT) {
		row--;
		insertLevel = wxAUI_INSERT_ROW;
	} else if (currentNotebookInfo.dock_direction == wxAUI_DOCK_BOTTOM) {
		position++;
		insertLevel = wxAUI_INSERT_PANE;
	}
	newNotebookSize.Scale(0.5, 1);
	if (GetPageCount() > 1) {
		newNotebook->Adopt(GetCurrentCodeControl(), this);
	} else {
		newNotebook->AddTriumphPage(t4p::FILE_TYPE_PHP);
	}
	AuiManager->InsertPane(newNotebook,
		wxAuiPaneInfo()
			.Layer(layer).Row(row).Position(position)
			.Direction(direction)
			.Gripper(false).Resizable(true).Floatable(false)
			.Resizable(true).PaneBorder(false).CaptionVisible(false)
			.CloseButton(false),
		insertLevel
	);
	AuiManager->GetPane(newNotebook).Show();

	// when there are more than 2 notebooks, given them names so
	// that the user can tell them apart (for "moving tabs" purposes)
	std::vector<t4p::NotebookClass*> notebooks = t4p::AuiVisibleCodeNotebooks(*AuiManager);
	if (notebooks.size() > 2) {
		for (size_t i = 0; i < notebooks.size(); i++) {
			wxAuiPaneInfo& info = AuiManager->GetPane(notebooks[i]);
			info.Name(wxString::Format("Notebook %ld", i + 1));
			info.Caption(wxString::Format("Notebook %ld", i + 1));
			info.CaptionVisible(true);
		}
	}

	AuiManager->Update();
}

void t4p::NotebookClass::OnMenuSplit(wxCommandEvent& event) {
	if (event.GetId() == ID_SPLIT_HORIZONTALLY) {
		SplitHorizontally();
	} else if (event.GetId() == ID_SPLIT_VERTICALLY) {
		SplitVertically();
	}
}

t4p::FileDropTargetClass::FileDropTargetClass(t4p::NotebookClass* notebook) :
	Notebook(notebook) {
}

bool t4p::FileDropTargetClass::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& files) {
	std::vector<wxString> fileVector;
	for (size_t i = 0; i < files.GetCount(); ++i) {
		fileVector.push_back(files[i]);
	}
	Notebook->LoadPages(fileVector);
	return true;
}

BEGIN_EVENT_TABLE(t4p::NotebookClass, wxAuiNotebook)
	EVT_MENU(ID_CLOSE_ALL_TABS, t4p::NotebookClass::OnCloseAllPages)
	EVT_MENU(ID_CLOSE_TAB, t4p::NotebookClass::OnMenuClosePage)
	EVT_MENU(ID_SPLIT_HORIZONTALLY, t4p::NotebookClass::OnMenuSplit)
	EVT_MENU(ID_SPLIT_VERTICALLY, t4p::NotebookClass::OnMenuSplit)
	EVT_MENU(ID_MOVE_TAB_TO_NOTEBOOK_1, t4p::NotebookClass::OnMovePage)
	EVT_MENU(ID_MOVE_TAB_TO_NOTEBOOK_2, t4p::NotebookClass::OnMovePage)
	EVT_MENU(ID_MOVE_TAB_TO_NOTEBOOK_3, t4p::NotebookClass::OnMovePage)
	EVT_MENU(ID_MOVE_TAB_TO_NOTEBOOK_4, t4p::NotebookClass::OnMovePage)
	EVT_MENU(ID_MOVE_TAB_TO_NOTEBOOK_5, t4p::NotebookClass::OnMovePage)
END_EVENT_TABLE()
