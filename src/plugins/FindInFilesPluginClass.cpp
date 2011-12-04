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
#include <plugins/FindInFilesPluginClass.h>
#include <windows/StringHelperClass.h>
#include <widgets/UnicodeStringValidatorClass.h>
#include <widgets/RegularExpressionValidatorClass.h>
#include <wx/artprov.h>
#include <wx/clipbrd.h>
#include <wx/ffile.h>
#include <wx/textfile.h>
#include <wx/valgen.h>
#include <algorithm>

static const int ID_MENU_FIND_IN_FILES = mvceditor::PluginClass::newMenuId();
static const int ID_FIND_IN_FILES_PROGRESS = wxNewId();

// these IDs are needed so that the IDs of the Regular expression help menu
// do not collide with the menu IDs of the FinderPlugin
static const int ID_REGEX_MENU_START = 9000;
static const int ID_REGEX_REPLACE_MENU_START = 10000;

mvceditor::FindInFilesBackgroundReaderClass::FindInFilesBackgroundReaderClass(wxEvtHandler& handler) 
: BackgroundFileReaderClass(handler) {

}

bool mvceditor::FindInFilesBackgroundReaderClass::InitForFind(wxEvtHandler* handler, mvceditor::FindInFilesClass findInFiles, const wxString& path) {
	Handler = handler;

	// find in files needs to be a copy; just to be sure
	// its thread safe
	// TODO: for files that are currently "dirty" (modified) we need to search the editor contents
	// and not the file contents
	// Kind of like the "SkipFiles" property
	FindInFiles = findInFiles;
	return Init(path) && FindInFiles.Prepare();
}

bool mvceditor::FindInFilesBackgroundReaderClass::InitForReplace(wxEvtHandler* handler, mvceditor::FindInFilesClass findInFiles, std::vector<wxString> files) {
	Handler = handler;
	FindInFiles = findInFiles;
	SkipFiles = files;
	return InitMatched();
}

bool mvceditor::FindInFilesBackgroundReaderClass::FileRead(DirectorySearchClass& search) {
	bool found = search.Walk(FindInFiles);
	if (found) {
		wxString fileName = search.GetMatchedFiles().back();
		bool destroy = TestDestroy();
		do {			
			if (destroy) {
				break;
			}
			int lineNumber = FindInFiles.GetCurrentLineNumber();
			wxString line = StringHelperClass::IcuToWx(FindInFiles.GetCurrentLine());
			wxCommandEvent hitEvent(EVENT_FIND_IN_FILES_FILE_HIT, ID_FIND_IN_FILES_PROGRESS);

			// in MSW the list control does not render the \t use another delimiter
			wxString hit = fileName + wxT("\t:") + 
				wxString::Format(wxT("%d"), lineNumber) + wxT("\t:") +
				line;
			hitEvent.SetString(hit);
			wxPostEvent(Handler, hitEvent);
		}
		while (!destroy && FindInFiles.FindNext());
	}
	return found;
}

bool mvceditor::FindInFilesBackgroundReaderClass::FileMatch(const wxString& file) {
	wxString fileToReplace = file;
	int matches = 0;

	// don't do replace for open files
	std::vector<wxString>::iterator it = find(SkipFiles.begin(), SkipFiles.end(), fileToReplace);
	if (it == SkipFiles.end()) {
		matches += FindInFiles.ReplaceAllMatchesInFile(fileToReplace);
	}
	return matches > 0;
}
	
mvceditor::FindInFilesResultsPanelClass::FindInFilesResultsPanelClass(wxWindow* parent, NotebookClass* notebook, StatusBarWithGaugeClass* gauge)
	: FindInFilesResultsPanelGeneratedClass(parent)
	, FindInFiles()
	, FindInFilesBackgroundFileReader(*this)
	, Notebook(notebook)
	, Gauge(gauge)
	, MatchedFiles(0) {
	FindInFilesGaugeId = wxNewId();
}

void mvceditor::FindInFilesResultsPanelClass::Find(const FindInFilesClass& findInFiles, wxString findPath) {
	FindInFiles = findInFiles;
	FindPath = findPath;
	MatchedFiles = 0;

	// for now disallow another replace when one is already active
	if (FindInFilesBackgroundFileReader.IsRunning()) {
		wxMessageBox(_("Find in files is already running. Please wait for it to finish."), _("Find In Files"));
		return;
	}
	if (FindInFilesBackgroundFileReader.InitForFind(this, FindInFiles, findPath)) {
		mvceditor::BackgroundFileReaderClass::StartError error;
		if (FindInFilesBackgroundFileReader.StartReading(error)) {
			EnableButtons(true, false);
			Gauge->AddGauge(_("Find In Files"), FindInFilesGaugeId, StatusBarWithGaugeClass::INDETERMINATE_MODE, 
				wxGA_HORIZONTAL);
		}
		else if (error == mvceditor::BackgroundFileReaderClass::ALREADY_RUNNING)  {
			wxMessageBox(_("Find in files is already running. Please wait for it to finish."), _("Find In Files"));
		}
		else if (error == mvceditor::BackgroundFileReaderClass::NO_RESOURCES)  {
			wxMessageBox(_("System is way too busy. Please try again later."), _("Find In Files"));
		}
	}
	else {
		wxMessageBox(_("Please enter a valid expression and path."));
	}
}

void mvceditor::FindInFilesResultsPanelClass::OnReplaceButton(wxCommandEvent& event) {
	CodeControlClass* codeControl = 
			Notebook->GetCodeControl(Notebook->GetSelection());	
			
	// if user changed tab, GetLastReplacementText would return false and nothing will be replaced
	UnicodeString text = codeControl->GetSafeText();
	if (codeControl) {		
		int32_t position = codeControl->GetCurrentPos(),
			length = 0;
		UnicodeString replaceText;
		FinderClass finder;
		FindInFiles.CopyFinder(finder);
		if (finder.Prepare()) {
			
			// only replace when the cursor is at the hit (which would be the case when and only when the use
			// double clicked on the result list (and focused on the hit).
			if (finder.FindNext(text, position) && finder.GetLastMatch(position, length) && 
				position == codeControl->GetCurrentPos() &&
				finder.GetLastReplacementText(text, replaceText)) {
				codeControl->SetSelectionAndEnsureVisible(position, position + length);
				codeControl->ReplaceSelection(StringHelperClass::IcuToWx(replaceText));
				codeControl->SetSelectionByCharacterPosition(position, position + replaceText.length());
			}
		}
	}
}

void mvceditor::FindInFilesResultsPanelClass::OnReplaceAllInFileButton(wxCommandEvent& event) {
	CodeControlClass* codeControl = 
			Notebook->GetCodeControl(Notebook->GetSelection());	
			
	// if user changed tab, GetLastReplacementText would return false and nothing will be replaced
	 if (codeControl) {
		UnicodeString text = codeControl->GetSafeText();
		FinderClass finder;
		FindInFiles.CopyFinder(finder);
		if (finder.Prepare()) {
			int matches = finder.ReplaceAllMatches(text);
			// TODO: must be a more efficent way than copying back and forth to/from ICU / wx string
			wxString s = StringHelperClass::IcuToWx(text);
			codeControl->SetText(s);
			SetStatus(wxString::Format(wxT("Status: Replaced %d matches"), matches));	
		}
	 }
}

void mvceditor::FindInFilesResultsPanelClass::OnReplaceInAllFilesButton(wxCommandEvent& event) {

	if (FindInFiles.Prepare()) {
		FinderClass finder;
		FindInFiles.CopyFinder(finder);
		if (finder.Prepare()) {
			// replace the open files
			for (size_t i = 0; i < Notebook->GetPageCount(); ++i) {
				CodeControlClass* codeControl = Notebook->GetCodeControl(i);
				UnicodeString text = codeControl->GetSafeText();
			
				// only update code control when there are replacements made
				if(finder.ReplaceAllMatches(text) > 0) {
					
					// TODO: must be a more efficent way than copying back and forth to/from ICU / wx string
					wxString s = StringHelperClass::IcuToWx(text);
					codeControl->SetText(s);
				}
			}
		}
		
		// we've already searched, when replacing we should iterate through matched files hence we don't call DirectorySearch,.Init().
		// for now disallow another replace when one is already active
		if (FindInFilesBackgroundFileReader.IsRunning()) {
			wxMessageBox(_("Find in files is already running. Please wait for it to finish."), _("Find In Files"));
			return;
		}
		FindInFilesBackgroundFileReader.InitForReplace(this, FindInFiles, Notebook->GetOpenedFiles());
		mvceditor::BackgroundFileReaderClass::StartError error;
		if (FindInFilesBackgroundFileReader.StartReading(error)) {
			SetStatus(_("Find In Files In Progress"));
			Gauge->AddGauge(_("Find In Files"), FindInFilesGaugeId, StatusBarWithGaugeClass::INDETERMINATE_MODE, 
				wxGA_HORIZONTAL);
			EnableButtons(true, false);
		}
		else if (error == mvceditor::BackgroundFileReaderClass::ALREADY_RUNNING)  {
			wxMessageBox(_("Find in files is already running. Please wait for it to finish."), _("Find In Files"));
		}
		else if (error == mvceditor::BackgroundFileReaderClass::NO_RESOURCES)  {
			wxMessageBox(_("System is way too busy. Please try again later."), _("Find In Files"));
		}
	}
	else {
		wxMessageBox(_("Please enter a valid expression and path."));
	}
}

void mvceditor::FindInFilesResultsPanelClass::OnFileSearched(wxCommandEvent& event) {
	int progress = event.GetInt();
	if (progress >= 0) {
		void* matchFound = event.GetClientData();
		if (matchFound) {
			++MatchedFiles;
		}
		SetStatus(wxString::Format(wxT("Searched %d files"), progress));
	}
}

void mvceditor::FindInFilesResultsPanelClass::OnFindInFilesComplete(wxCommandEvent& event) {
	int matchedFilesSize = GetNumberOfMatchedFiles();
	bool enableIterators = matchedFilesSize > 0 && !FindInFiles.ReplaceExpression.isEmpty();
	EnableButtons(false, enableIterators);
	Gauge->StopGauge(FindInFilesGaugeId);
	if (event.GetInt() == BackgroundFileReaderClass::WALK) {
		if (matchedFilesSize > 0) {
			SetStatus(wxString::Format(wxT("Found Matches in %d files"), matchedFilesSize));
		}
		else {
			SetStatus(_("Did not Find any Matches"));
		}
	}
	else if (event.GetEventType() == BackgroundFileReaderClass::MATCHED) {
		if (matchedFilesSize > 0) {
			SetStatus(wxString::Format(wxT("Replaced Matches in %d files"), matchedFilesSize));
		}
		else {
			SetStatus(_("Did not Replace. No Matches Were Found"));
		}
	}
}

void mvceditor::FindInFilesResultsPanelClass::OnFileHit(wxCommandEvent& event) {
	wxString hit = event.GetString();
	hit.Trim();
	ResultsList->Append(hit.SubString(0, 200));
}

void mvceditor::FindInFilesResultsPanelClass::OnStopButton(wxCommandEvent& event) {
	FindInFilesBackgroundFileReader.StopReading();
}

void mvceditor::FindInFilesResultsPanelClass::OnDoubleClick(wxCommandEvent& event) {
	wxString result = event.GetString();
	int index = result.Find(wxT("\t:"));
	if (index > -1) {
		wxString fileName = result.substr(0, index);
		long line = 0;
		size_t indexAfterPos = result.find(wxT("\t:"), index + 2);
		if (indexAfterPos != std::string::npos) {
			wxString posString = result.substr(index + 2, indexAfterPos - index);
			posString.ToLong(&line);
			--line; // line is 1-based but wxSTC lines start at zero
		}
		Notebook->LoadPage(fileName);
		CodeControlClass* codeControl = Notebook->GetCurrentCodeControl();
		if (codeControl) {
			
			// search for the expression and highlight it. search from the start of the line.
			int32_t pos = codeControl->PositionFromLine(line);
			int32_t length = 0;
			FinderClass finder;
			FindInFiles.CopyFinder(finder);
			if (finder.Prepare()) {
				if (finder.FindNext(codeControl->GetSafeText(), pos) && finder.GetLastMatch(pos, length)) {
					
					//we want the cursor to be in the beginning of the hit hece pos is 2nd parameter
					codeControl->SetCurrentPos(pos);
					codeControl->SetAnchor(pos);
					codeControl->SetSelectionAndEnsureVisible(pos + length, pos);
				}
			}
		}
	}
}

void mvceditor::FindInFilesResultsPanelClass::OnCopySelectedButton(wxCommandEvent& event) {
	wxArrayInt selections;
	if (ResultsList->GetSelections(selections) > 0) {
		wxString selectedItems;
		for (size_t i = 0; i < selections.Count(); i++) {
			selectedItems += ResultsList->GetString(selections[i]);
			selectedItems += wxTextFile::GetEOL();
		}
		if (wxTheClipboard->Open()) {
			wxTheClipboard->SetData(new wxTextDataObject(selectedItems));
			wxTheClipboard->Close();
		}
	}
}

void mvceditor::FindInFilesResultsPanelClass::OnCopyAllButton(wxCommandEvent& event) {
	wxArrayString selectedItemsArray = ResultsList->GetStrings();
	wxString selectedItems;
	for (size_t i = 0; i < selectedItemsArray.Count(); i++) {
		selectedItems += selectedItemsArray[i];
		selectedItems += wxTextFile::GetEOL();
	}
	if (selectedItemsArray.Count() && wxTheClipboard->Open()) {
		wxTheClipboard->SetData(new wxTextDataObject(selectedItems));
		wxTheClipboard->Close();
	}
}

void mvceditor::FindInFilesResultsPanelClass::EnableButtons(bool enableStopButton, bool enableReplaceButtons) {
	StopButton->Enable(enableStopButton);
	ReplaceButton->Enable(enableReplaceButtons);
	ReplaceAllInFileButton->Enable(enableReplaceButtons);
	ReplaceInAllFilesButton->Enable(enableReplaceButtons);
	CopySelectedButton->Enable(!enableStopButton);
	CopyAllButton->Enable(!enableStopButton);
}

void mvceditor::FindInFilesResultsPanelClass::SetStatus(const wxString& text) {
	
	// label might grow/shrink according to new text, must
	// tell the sizer to re-position the label correctly
	// we need this for the label to be right-aligned after
	// the text change
	ResultText->SetLabel(text);
	ResultText->GetContainingSizer()->Layout();
}

int mvceditor::FindInFilesResultsPanelClass::GetNumberOfMatchedFiles() {
	return MatchedFiles;
}

void mvceditor::FindInFilesResultsPanelClass::OnTimer(wxCommandEvent& event) {
	Gauge->IncrementGauge(FindInFilesGaugeId, StatusBarWithGaugeClass::INDETERMINATE_MODE);	
}

mvceditor::FindInFilesDialogClass::FindInFilesDialogClass(wxWindow* parent, mvceditor::FindInFilesPluginClass& plugin)
	: FindInFilesDialogGeneratedClass(parent, wxID_ANY)
	, Plugin(plugin)
	, CurrentInsertionPointFind(0)
	, CurrentInsertionPointReplace(0) {
	mvceditor::ProjectClass* project = Plugin.GetProject();
	Plugin.FindHistory.Attach(FindText);
	Plugin.ReplaceHistory.Attach(ReplaceWithText);
	Plugin.FilesHistory.Attach(FilesFilter);
	
	// the first time showing this dialog populate the filter to have only PHP file extensions
	if (FilesFilter->GetCount() <= 0) {
		Plugin.PreviousFindInFiles.FilesFilter = project->GetPhpFileExtensions();
	}
	if (NULL != project && !project->GetRootPath().IsEmpty()) {
		Directory->SetPath(project->GetRootPath());
	}
	mvceditor::RegularExpressionValidatorClass regExValidator(&Plugin.PreviousFindInFiles.Expression, FinderMode);
	FindText->SetValidator(regExValidator);
	UnicodeStringValidatorClass replaceExpressionValidator(&Plugin.PreviousFindInFiles.ReplaceExpression);
	wxGenericValidator filesFilterValidator(&Plugin.PreviousFindInFiles.FilesFilter);
	wxGenericValidator modeValidator(&Plugin.PreviousFindInFiles.Mode);
	wxGenericValidator caseValidator(&Plugin.PreviousFindInFiles.CaseSensitive);
	ReplaceWithText->SetValidator(replaceExpressionValidator);
	FinderMode->SetValidator(modeValidator);
	FilesFilter->SetValidator(filesFilterValidator);
	CaseSensitive->SetValidator(caseValidator);
	FindText->SetFocus();

	// since this panel handles EVT_TEXT_ENTER, we need to handle the
	// tab traversal ourselves otherwise tab travesal wont work
	FindText->GetEventHandler()->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(mvceditor::FindInFilesDialogClass::OnKeyDown));
	ReplaceWithText->GetEventHandler()->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(mvceditor::FindInFilesDialogClass::OnKeyDown));
	FilesFilter->GetEventHandler()->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(mvceditor::FindInFilesDialogClass::OnKeyDown));
	
	// connect to the KILL_FOCUS events so that we can capture the insertion point
	// on Win32 GetInsertionPoint() returns 0 when the combo box is no
	// in focus; we must receive the position via an outside mechanism
	FindText->GetEventHandler()->Connect(wxID_ANY, wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(FindInFilesDialogClass::OnKillFocusFindText), NULL, this);
	ReplaceWithText->GetEventHandler()->Connect(wxID_ANY, wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(FindInFilesDialogClass::OnKillFocusReplaceText), NULL, this);
}

mvceditor::FindInFilesDialogClass::~FindInFilesDialogClass() {
	FindText->GetEventHandler()->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(mvceditor::FindInFilesDialogClass::OnKeyDown));
	ReplaceWithText->GetEventHandler()->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(mvceditor::FindInFilesDialogClass::OnKeyDown));
	FilesFilter->GetEventHandler()->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(mvceditor::FindInFilesDialogClass::OnKeyDown));

	FindText->GetEventHandler()->Disconnect(wxID_ANY, wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(FindInFilesDialogClass::OnKillFocusFindText), NULL, this);
	ReplaceWithText->GetEventHandler()->Disconnect(wxID_ANY, wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(FindInFilesDialogClass::OnKillFocusReplaceText), NULL, this);
}

void mvceditor::FindInFilesDialogClass::OnOkButton(wxCommandEvent& event) {
	if (TransferDataFromWindow()) {
		
		if (!Plugin.PreviousFindInFiles.Prepare()) {
			wxMessageBox(_("Expression is not valid."), _("Find In Files"), wxOK | wxCENTER, this);
		}
		else if (Directory->GetPath().IsEmpty()) {
			wxMessageBox(_("Find path must not be empty."), _("Find In Files"), wxOK | wxCENTER, this);
		}
		else {
			Plugin.PreviousFindPath = Directory->GetPath();
			Plugin.FindHistory.Save();
			Plugin.ReplaceHistory.Save();
			Plugin.FilesHistory.Save();
			Plugin.FindHistory.Detach();
			Plugin.ReplaceHistory.Detach();
			Plugin.FilesHistory.Detach();
			EndModal(wxID_OK);
		}
	}
}

void mvceditor::FindInFilesDialogClass::OnCancelButton(wxCommandEvent& event) {

	// need to do this to prevent crash on app exit
	Plugin.FindHistory.Detach();
	Plugin.ReplaceHistory.Detach();
	Plugin.FilesHistory.Detach();
	EndModal(wxID_CANCEL);
}

void mvceditor::FindInFilesDialogClass::OnKeyDown(wxKeyEvent& event) {

	// warning: don't use "this"; look at the way this event has been connected 
	if (event.GetKeyCode() == WXK_TAB && event.ShiftDown()) {
		Navigate(wxNavigationKeyEvent::IsBackward);
	}
	else if (event.GetKeyCode() == WXK_TAB ) {
		Navigate(wxNavigationKeyEvent::IsForward);
	}
	else {
		event.Skip();
	}
}

void mvceditor::FindInFilesDialogClass::OnKillFocusFindText(wxFocusEvent& event) {
	CurrentInsertionPointFind = FindText->GetInsertionPoint();
	event.Skip();
}

void mvceditor::FindInFilesDialogClass::OnKillFocusReplaceText(wxFocusEvent& event) {
	CurrentInsertionPointReplace = ReplaceWithText->GetInsertionPoint();
	event.Skip();
}

mvceditor::FindInFilesPluginClass::FindInFilesPluginClass()
	: PluginClass()
	, PreviousFindInFiles()
	, PreviousFindPath() {
}

void mvceditor::FindInFilesPluginClass::AddEditMenuItems(wxMenu* editMenu) {
	editMenu->Append(ID_MENU_FIND_IN_FILES, _("Find In Files\tCTRL+SHIFT+F"), 
		_("Find an expression by searching entire directory contents"));
}

void mvceditor::FindInFilesPluginClass::OnEditFindInFiles(wxCommandEvent& event) {

	// prime finder with selected text
	wxString selectedText = GetSelectedText();
	if (!selectedText.empty()) {
		PreviousFindInFiles.Expression = StringHelperClass::wxToIcu(selectedText);
	}
	FindInFilesDialogClass dialog(NULL, *this);
	if (dialog.ShowModal() == wxID_OK) {
		mvceditor::FindInFilesResultsPanelClass* panel = new mvceditor::FindInFilesResultsPanelClass(GetToolsNotebook(), 
			GetNotebook(), GetStatusBarWithGauge());		
		if(AddToolsWindow(panel, _("Find In Files Results"))) {
			panel->Find(PreviousFindInFiles, PreviousFindPath);
		}
	}
}

void mvceditor::FindInFilesDialogClass::OnRegExFindHelpButton(wxCommandEvent& event) {
	wxMenu regExMenu;
	mvceditor::PopulateRegExFindMenu(regExMenu, ID_REGEX_MENU_START);
	PopupMenu(&regExMenu);	
}

void mvceditor::FindInFilesDialogClass::OnRegExReplaceHelpButton(wxCommandEvent& event) {
	wxMenu regExMenu;
	mvceditor::PopulateRegExReplaceMenu(regExMenu, ID_REGEX_REPLACE_MENU_START);
	PopupMenu(&regExMenu);	
}

void mvceditor::FindInFilesDialogClass::InsertRegExSymbol(wxCommandEvent& event) {
	int id = event.GetId() - ID_REGEX_MENU_START;
	mvceditor::AddSymbolToRegularExpression(FindText, id, CurrentInsertionPointFind);
	FinderMode->SetSelection(FinderClass::REGULAR_EXPRESSION);
	event.Skip();
}

void mvceditor::FindInFilesDialogClass::InsertReplaceRegExSymbol(wxCommandEvent& event) {
	int id = event.GetId() - ID_REGEX_REPLACE_MENU_START;
	mvceditor::AddSymbolToReplaceRegularExpression(ReplaceWithText, id, CurrentInsertionPointReplace);
	FinderMode->SetSelection(FinderClass::REGULAR_EXPRESSION);
}

BEGIN_EVENT_TABLE(mvceditor::FindInFilesResultsPanelClass, FindInFilesResultsPanelGeneratedClass)
	EVT_COMMAND(wxID_ANY, EVENT_FILE_READ, mvceditor::FindInFilesResultsPanelClass::OnFileSearched)
	EVT_COMMAND(ID_FIND_IN_FILES_PROGRESS, EVENT_FIND_IN_FILES_FILE_HIT, mvceditor::FindInFilesResultsPanelClass::OnFileHit)
	EVT_COMMAND(wxID_ANY, EVENT_FILE_READ_COMPLETE, mvceditor::FindInFilesResultsPanelClass::OnFindInFilesComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::FindInFilesResultsPanelClass::OnTimer)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::FindInFilesPluginClass, wxEvtHandler)
	EVT_MENU(ID_MENU_FIND_IN_FILES, mvceditor::FindInFilesPluginClass::OnEditFindInFiles)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::FindInFilesDialogClass, FindInFilesDialogGeneratedClass)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_ONE, mvceditor::FindInFilesDialogClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_TWO, mvceditor::FindInFilesDialogClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_THREE, mvceditor::FindInFilesDialogClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_FOUR, mvceditor::FindInFilesDialogClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_FIVE, mvceditor::FindInFilesDialogClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_ONE, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_ZERO_OR_ONE, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_EXACT, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_AT_LEAST, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_BETWEEN, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_BEGIN_OF_LINE, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_END_OF_LINE, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_DIGIT, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_WHITE_SPACE, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_ALPHANUMERIC, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_NOT_DECIMAL, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_NOT_WHITE_SPACE, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_NOT_ALPHANUMERIC, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_CASE_SENSITIVE, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_COMMENT, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_DOT_ALL, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_MULTI_LINE, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_UWORD, mvceditor::FindInFilesDialogClass::InsertRegExSymbol)
END_EVENT_TABLE()