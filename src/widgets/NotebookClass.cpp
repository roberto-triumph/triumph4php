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
#include <Triumph.h>
#include <globals/Errors.h>
#include <widgets/FileTypeImageList.h>
#include <wx/artprov.h>
#include <wx/filename.h>
#include <wx/file.h>
#include <wx/wupdlock.h>

int ID_CLOSE_ALL_TABS = wxNewId();
int ID_CLOSE_TAB = wxNewId();

/**
 * convert a code control document mode to a 
 * image list number; to determine what image
 * to show in the notebook tab based on the document
 * mode.
 */
static int ImageId(t4p::CodeControlClass::Mode mode) {
	int imgId = t4p::IMGLIST_NONE;
	switch (mode) {
	case t4p::CodeControlClass::TEXT:
		imgId = t4p::IMGLIST_MISC;
		break;
	case t4p::CodeControlClass::SQL:
		imgId = t4p::IMGLIST_SQL;
		break;
	case t4p::CodeControlClass::CSS:
		imgId = t4p::IMGLIST_CSS;
		break;
	case t4p::CodeControlClass::PHP:
		imgId = t4p::IMGLIST_PHP;
		break;
	case t4p::CodeControlClass::JS:
		imgId = t4p::IMGLIST_JS;
		break;
	case t4p::CodeControlClass::CONFIG:
		imgId = t4p::IMGLIST_CONFIG;
		break;
	case t4p::CodeControlClass::CRONTAB:
		imgId = t4p::IMGLIST_CRONTAB;
		break;
	case t4p::CodeControlClass::YAML:
		imgId = t4p::IMGLIST_YAML;
		break;
	case t4p::CodeControlClass::XML:
		imgId = t4p::IMGLIST_XML;
		break;
	case t4p::CodeControlClass::RUBY:
		imgId = t4p::IMGLIST_RUBY;
		break;
	case t4p::CodeControlClass::LUA:
		imgId = t4p::IMGLIST_LUA;
		break;
	case t4p::CodeControlClass::MARKDOWN:
		imgId = t4p::IMGLIST_MARKDOWN;
		break;
	case t4p::CodeControlClass::BASH:
		imgId = t4p::IMGLIST_BASH;
		break;
	case t4p::CodeControlClass::DIFF:
		imgId = t4p::IMGLIST_DIFF;
		break;
	}
	return imgId;
}

t4p::NotebookClass::NotebookClass(wxWindow* parent, wxWindowID id, 
	const wxPoint& pos, const wxSize& size, long style)
	: wxAuiNotebook(parent, id, pos, size, style)
	, CodeControlOptions(NULL)
	, Globals(NULL)
	, EventSink(NULL)
	, ContextMenu(NULL)
	, NewPageNumber(1) 
	, TabIndexRightClickEvent(-1) {
	
	ImageList = NULL;
}

t4p::NotebookClass::~NotebookClass() {
	delete ContextMenu;
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
				!SavePage(currentPage))) {
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

bool t4p::NotebookClass::SavePage(int pageIndex) {
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

				wxCommandEvent createdEvt(t4p::EVENT_APP_FILE_CREATED);
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
void t4p::NotebookClass::AddTriumphPage(t4p::CodeControlClass::Mode mode) {
	if (NULL == ImageList) {
		ImageList = new wxImageList(16, 16);
		t4p::FillWithFileType(*ImageList);
		AssignImageList(ImageList);
	}
	wxString format;
	switch (mode) {
		case t4p::CodeControlClass::TEXT:
			format = _("Untitled %d.txt");
			break;
		case t4p::CodeControlClass::SQL:
			format = _("Untitled %d.sql");
			break;
		case t4p::CodeControlClass::CSS:
			format = _("Untitled %d.css");
			break;
		case t4p::CodeControlClass::PHP:
			format = _("Untitled %d.php");
			break;
		case t4p::CodeControlClass::JS:
			format = _("Untitled %d.js");
			break;
		case t4p::CodeControlClass::CONFIG:
			format = _("Untitled %d.conf");
			break;
		case t4p::CodeControlClass::CRONTAB:
			format = _("Untitled %d");
			break;
		case t4p::CodeControlClass::YAML:
			format = _("Untitled %d.yml");
			break;
		case t4p::CodeControlClass::XML:
			format = _("Untitled %d.xml");
			break;
		case t4p::CodeControlClass::RUBY:
			format = _("Untitled %d.rb");
			break;
		case t4p::CodeControlClass::LUA:
			format = _("Untitled %d.lua");
			break;
		case t4p::CodeControlClass::MARKDOWN:
			format = _("Untitled %d.md");
			break;
		case t4p::CodeControlClass::BASH:
			format = _("Untitled %d.sh");
			break;
		case t4p::CodeControlClass::DIFF:
			format = _("Untitled %d.diff");
			break;
	}
	
	// make sure to use a unique ID, other source code depends on this
	CodeControlClass* page = new CodeControlClass(this, *CodeControlOptions, Globals, *EventSink, wxNewId());
	page->SetDocumentMode(mode);
	
	int imgId = ImageId(mode);
	
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
		t4p::FillWithFileType(*ImageList);
		AssignImageList(ImageList);
	}
	
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
		bool hasSignature = false;
		wxString charset;
		t4p::FindInFilesClass::OpenErrors error = FindInFilesClass::FileContents(filename, fileContents, charset, hasSignature);
		if (error == t4p::FindInFilesClass::NONE) {

			// make sure to use a unique ID, other source code depends on this
			CodeControlClass* newCode = new CodeControlClass(this, *CodeControlOptions, Globals, *EventSink, wxNewId());
			newCode->TrackFile(filename, fileContents, charset, hasSignature);

			t4p::CodeControlClass::Mode mode = newCode->GetDocumentMode();
			int imgId = ImageId(mode);
			
			// if user dragged in a file on an opened file we want still want to accept dragged files
			newCode->SetDropTarget(new FileDropTargetClass(this));
			wxFileName fileName(filename);
			this->AddPage(newCode, fileName.GetFullName(), true, imgId);

			// tell the app that a file has been opened
			t4p::CodeControlEventClass openEvent(t4p::EVENT_APP_FILE_OPENED, newCode);
			EventSink->Publish(openEvent);
		}
		else if (error == t4p::FindInFilesClass::FILE_NOT_FOUND) {
			t4p::EditorLogError(t4p::ERR_INVALID_FILE, filename);
		}
		else if (t4p::FindInFilesClass::CHARSET_DETECTION == error) {
			t4p::EditorLogError(t4p::ERR_CHARSET_DETECTION, filename);
		}
		else if (t4p::FindInFilesClass::FILE_TOO_LARGE == error) {
			t4p::EditorLogError(t4p::ERR_FILE_TOO_LARGE, filename);
		}
	}
	if (info.GetOperatingSystemId() == wxOS_WINDOWS_NT && doFreeze) {
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
	return SavePage(currentPage);
}

bool t4p::NotebookClass::SaveCurrentPageAsNew() {
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

void t4p::NotebookClass::SaveAllModifiedPagesWithoutPrompting() {
	for (size_t i = 0; i < GetPageCount(); i++) {
		if (IsPageModified(i)) {
			SavePage(i);
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
	if (NULL == ContextMenu) {
		CreateContextMenu();
	}
	PopupMenu(ContextMenu);
	event.Skip();
}

void t4p::NotebookClass::CreateContextMenu() {
	ContextMenu = new wxMenu;
	ContextMenu->Append(ID_CLOSE_ALL_TABS, wxT("Close All Tabs"));
	ContextMenu->Append(ID_CLOSE_TAB, wxT("Close This Tab"));
}

void t4p::NotebookClass::OnCloseAllPages(wxCommandEvent& event) {
	if (SaveAllModifiedPages()) {
		CloseAllPages();
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
		wxAuiNotebookEvent evt(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED, this->GetId());

		// tab index
		evt.SetSelection(0);
		evt.SetEventObject(this);
		EventSink->Publish(evt);
	}
}

void t4p::NotebookClass::CloseCurrentPage() {
	int currentPage = GetSelection();
	ClosePage(currentPage);
}

void t4p::NotebookClass::ClosePage(int index) {
	bool isPageClosed = false;
	if (IsPageModified(index)) {
		wxString pageName = GetPageText(index);
		if (pageName.EndsWith(wxT("*"))) {
			pageName = pageName.SubString(0, pageName.size() -2);
		}
		wxString msg = pageName + wxT(" has not been saved. Save changes?");
		int response = wxMessageBox(msg, wxT("Save PHP File"), wxYES_NO | 
			wxCANCEL | wxICON_QUESTION, this);
		if (wxCANCEL != response) {
			if (wxYES == response && !SavePage(index)) {
				//something drastic. dont know how to handle it
			}
			DeletePage(index);
			isPageClosed = true;
		}
	}
	else {
		DeletePage(index);
		isPageClosed = true;
	}
	if (isPageClosed) {

		// notify owner that the tab has been closed
		// we must do it here; aui notebook's DeletePage does not
		// generate events
		wxAuiNotebookEvent evt(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED, this->GetId());

		// tab index
		evt.SetSelection(index);
		evt.SetEventObject(this);
		EventSink->Publish(evt);
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

int t4p::NotebookClass::WilcardIndex(t4p::CodeControlClass::Mode mode) {
	if (t4p::CodeControlClass::PHP == mode) {
		return 0;
	}
	else if (t4p::CodeControlClass::CSS == mode) {
		return 1;
	}
	else if (t4p::CodeControlClass::SQL == mode) {
		return 2;
	}
	else if (t4p::CodeControlClass::JS == mode) {
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

void t4p::NotebookClass::OnMenuClosePage(wxCommandEvent& event) {

	// get the tab that was right clicked; the tab right menu event holds
	// the index of the tab we want to close
	if (TabIndexRightClickEvent >= 0 && TabIndexRightClickEvent < (int)GetPageCount()) {
		ClosePage(TabIndexRightClickEvent);
		TabIndexRightClickEvent  = -1;
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
	EVT_AUINOTEBOOK_PAGE_CLOSE(t4p::ID_CODE_NOTEBOOK, 
		t4p::NotebookClass::SavePageIfModified)
	EVT_AUINOTEBOOK_TAB_RIGHT_UP(t4p::ID_CODE_NOTEBOOK,
		t4p::NotebookClass::ShowContextMenu)
	EVT_MENU(ID_CLOSE_ALL_TABS, t4p::NotebookClass::OnCloseAllPages)
	EVT_MENU(ID_CLOSE_TAB, t4p::NotebookClass::OnMenuClosePage)

	// using OnPageChanging instead of OnPageChanged because onPageChanged
	// generates multiple events (not quite sure why yet)
	EVT_AUINOTEBOOK_PAGE_CHANGING(t4p::ID_CODE_NOTEBOOK, t4p::NotebookClass::OnPageChanging)
END_EVENT_TABLE()