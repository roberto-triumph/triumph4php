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
#include <plugins/FinderPluginClass.h>
#include <windows/StringHelperClass.h>
#include <widgets/UnicodeStringValidatorClass.h>
#include <widgets/RegularExpressionValidatorClass.h>
#include <wx/artprov.h>
#include <wx/numdlg.h>
#include <wx/valgen.h>
#include <wx/valtext.h>

static const int ID_FIND_PANEL = wxNewId(); 
static const int ID_REPLACE_PANEL = wxNewId(); 

// these IDs are needed so that the IDs of the Regular expression help menu
// do not collide with the menu IDs of the FindInFilesPlugin
static const int ID_REGEX_MENU_START = 11000;
static const int ID_REGEX_REPLACE_FIND_MENU_START = 12000;
static const int ID_REGEX_REPLACE_MENU_START = 13000;

mvceditor::FinderPanelClass::FinderPanelClass(wxWindow* parent, mvceditor::NotebookClass* notebook, wxAuiManager* auiManager, int windowId)
		: FinderPanelGeneratedClass(parent, windowId)
		, Finder()
		, ComboBoxHistory(FindText)
		, Notebook(notebook)
		, AuiManager(auiManager) 
		, CurrentInsertionPointFind(0) {
	mvceditor::RegularExpressionValidatorClass regExValidator(&Finder.Expression, FinderMode);
	FindText->SetValidator(regExValidator);
	wxGenericValidator modeValidator(&Finder.Mode);
	FinderMode->SetValidator(modeValidator);
	wxGenericValidator wrapValidator(&Finder.Wrap);
	Wrap->SetValidator(wrapValidator);
	RESULT_MESSAGE = _("Status: Found text at line %d");
	NextButton->SetBitmapLabel(wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_TOOLBAR,
		wxSize(16, 16)));
	PreviousButton->SetBitmapLabel(wxArtProvider::GetBitmap(wxART_GO_UP, wxART_TOOLBAR,
		wxSize(16, 16)));
	HelpButton->SetBitmapLabel((wxArtProvider::GetBitmap(wxART_HELP, 
		wxART_TOOLBAR, wxSize(16, 16))));
	CloseButton->SetBitmapLabel((wxArtProvider::GetBitmap(wxART_ERROR, 
		wxART_FRAME_ICON, wxSize(16, 16))));

	// connect to the KILL_FOCUS events so that we can capture the insertion point
	// on Win32 GetInsertionPoint() returns 0 when the combo box is no
	// in focus; we must receive the position via an outside mechanism
	FindText->GetEventHandler()->Connect(wxID_ANY, wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(FinderPanelClass::OnKillFocusFindText), NULL, this);
}

mvceditor::FinderPanelClass::~FinderPanelClass() {
	FindText->GetEventHandler()->Disconnect(wxID_ANY, wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(FinderPanelClass::OnKillFocusFindText), NULL, this);
}

void mvceditor::FinderPanelClass::SetFocusOnFindText() {
	FindText->SetFocus();
}

void mvceditor::FinderPanelClass::SetExpression(const wxString& expression) {
	FindText->SetValue(expression);
	
	// set the expression on the underlying finder  object
	TransferDataFromWindow();
}

void mvceditor::FinderPanelClass::FindNext() {
	if (Validate() && TransferDataFromWindow()) {
		if (Finder.Prepare()) {
			Find();
		}
		else {
			SetStatus(_("Status: Invalid Expression"));	
		}
	}	
}

void mvceditor::FinderPanelClass::FindPrevious() {
	if (Validate() && TransferDataFromWindow()) {
		if (Finder.Prepare()) {
			Find(false);
		}
		else {
			SetStatus(_("Status: Invalid Expression"));	
		}
	}	
}

void mvceditor::FinderPanelClass::Find(bool findNext) {
	
	// only search when notebook has a current tab
	CodeControlClass* codeControl = 
			Notebook->GetCodeControl(Notebook->GetSelection());	
	if (Notebook->GetPageCount() && codeControl) {
		
		// pick up from last found spot if possible. increment/decrement so that
		// we dont find the same hit again
		int32_t position = codeControl->GetCurrentPos(),
			length = 0;
		bool found = findNext ? Finder.FindNext(codeControl->GetSafeText(), position + 1) :
			Finder.FindPrevious(codeControl->GetSafeText(), position - 1);
		if (found) {
			if (Finder.GetLastMatch(position, length)) {
				codeControl->SetSelectionAndEnsureVisible(position, position + length);
				int lineNumber = codeControl->LineFromPosition(position);
				
				// lineNumber seems to start at zero
				++lineNumber;
				SetStatus(wxString::Format(RESULT_MESSAGE, lineNumber));
			}
		}
		else {			
			SetStatus(_("Status: Not Found"));
		}
	}
}

void mvceditor::FinderPanelClass::OnNextButton(wxCommandEvent& event) {
	FindNext();
	ComboBoxHistory.Save();
}

void mvceditor::FinderPanelClass::OnHelpButton(wxCommandEvent& event) {
	wxString help = wxString::FromAscii("Find modes:\n"
	  "Code:\n" 
	  "Type in PHP source code.  The editor will ignore whitespace "
	  "where it does not matter and will also match single and double quoted literals "
	  "if their contents match. For example, searching for\n\tstr_pos($text, 'needle')\n"
	  "will match\n\tstr_pos( $text,\"needle\" )\n\n"
	  "Exact:\n"
	  "Searching will be done using exact, case sensitive matching\n\n"
	  "Regular Expression:\n"
	  "Searching will be done using the entered regular expression. You "
	  "can use the button on the right to see all regular expression symbols."
	  "The full regular expression syntax is described at "
	  "http://www.unicode.org/reports/tr18/\n");
	help = wxGetTranslation(help);
	wxMessageBox(help, _("Find Help"), wxOK, this);
}

void mvceditor::FinderPanelClass::OnOkButton(wxCommandEvent& event) {
	Show(false);
}

void mvceditor::FinderPanelClass::OnPreviousButton(wxCommandEvent& event) {
	FindPrevious();
	ComboBoxHistory.Save();
}

void mvceditor::FinderPanelClass::SetStatus(const wxString& message) {
	ResultText->SetLabel(message);
	
	// label might grow/shrink according to new text, must
	// tell the sizer to re-position the label correctly
	// we need this for the label to be right-aligned after
	// the text change
	ResultText->GetContainingSizer()->Layout();

	// NOTE: had to add these calls otherwise the find panel would not 
	// totally repaint itself and it looks garbled when the status
	// message changes
	GetContainingSizer()->Layout();
	Refresh();
}

void mvceditor::FinderPanelClass::OnCloseButton(wxCommandEvent& event) {
	AuiManager->GetPane(this).Hide();
	AuiManager->Update();
}

void mvceditor::FinderPanelClass::OnFindEnter(wxCommandEvent& event) {
	OnNextButton(event);
}

void mvceditor::FinderPanelClass::OnRegExFindHelpButton(wxCommandEvent& event) {
	wxMenu regExMenu;
	mvceditor::PopulateRegExFindMenu(regExMenu, ID_REGEX_MENU_START);
	PopupMenu(&regExMenu);	
}

void mvceditor::FinderPanelClass::InsertRegExSymbol(wxCommandEvent& event) {
	int id = event.GetId() - ID_REGEX_MENU_START;
	mvceditor::AddSymbolToRegularExpression(FindText, id, CurrentInsertionPointFind);
	FinderMode->SetSelection(FinderClass::REGULAR_EXPRESSION);
}

void mvceditor::FinderPanelClass::OnKillFocusFindText(wxFocusEvent& event) {
	CurrentInsertionPointFind = FindText->GetInsertionPoint();
	event.Skip();
}

mvceditor::ReplacePanelClass::ReplacePanelClass(wxWindow* parent, mvceditor::NotebookClass* notebook, wxAuiManager* auiManager, int windowId)
		: ReplacePanelGeneratedClass(parent, windowId)
		, Finder()
		, FindHistory(FindText)
		, ReplaceHistory(ReplaceWithText)
		, Notebook(notebook)
		, AuiManager(auiManager)
		, CurrentInsertionPointFind(0) 
		, CurrentInsertionPointReplace(0) {
	mvceditor::RegularExpressionValidatorClass regExValidator(&Finder.Expression, FinderMode);
	FindText->SetValidator(regExValidator);
	wxGenericValidator modeValidator(&Finder.Mode);
	FinderMode->SetValidator(modeValidator);
	wxGenericValidator wrapValidator(&Finder.Wrap);
	Wrap->SetValidator(wrapValidator);
	RESULT_MESSAGE = _("Status: Found text at line %d");
	NextButton->SetBitmapLabel(wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_TOOLBAR,
		wxSize(16, 16)));
	PreviousButton->SetBitmapLabel(wxArtProvider::GetBitmap(wxART_GO_UP, wxART_TOOLBAR,
		wxSize(16, 16)));
	HelpButton->SetBitmapLabel((wxArtProvider::GetBitmap(wxART_HELP, 
		wxART_TOOLBAR, wxSize(16, 16))));
	CloseButton->SetBitmapLabel((wxArtProvider::GetBitmap(wxART_ERROR, 
		wxART_FRAME_ICON, wxSize(16, 16))));
	UnicodeStringValidatorClass replaceValidator(&Finder.ReplaceExpression);
	ReplaceWithText->SetValidator(replaceValidator);
	ReplaceButton->SetBitmapLabel((wxArtProvider::GetBitmap(wxART_FIND_AND_REPLACE, 
		wxART_TOOLBAR, wxSize(16, 16))));
	ReplaceAllButton->SetBitmapLabel((wxArtProvider::GetBitmap(wxART_FIND_AND_REPLACE, 
		wxART_TOOLBAR, wxSize(16, 16))));
	UndoButton->SetBitmapLabel((wxArtProvider::GetBitmap(wxART_UNDO, 
		wxART_TOOLBAR, wxSize(16, 16))));
	ReplaceWithText->MoveAfterInTabOrder(FindText);
	RegExReplaceHelpButton->MoveAfterInTabOrder(ReplaceWithText);

	// since this panel handles EVT_TEXT_ENTER, we need to handle th
	// tab traversal ourselves otherwise tab travesal wont work
	FindText->GetEventHandler()->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(mvceditor::ReplacePanelClass::OnKeyDown));
	ReplaceWithText->GetEventHandler()->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(mvceditor::ReplacePanelClass::OnKeyDown));
	
	// connect to the KILL_FOCUS events so that we can capture the insertion point
	// on Win32 GetInsertionPoint() returns 0 when the combo box is no
	// in focus; we must receive the position via an outside mechanism
	FindText->GetEventHandler()->Connect(wxID_ANY, wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(ReplacePanelClass::OnKillFocusFindText), NULL, this);
	ReplaceWithText->GetEventHandler()->Connect(wxID_ANY, wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(ReplacePanelClass::OnKillFocusReplaceText), NULL, this);
}

mvceditor::ReplacePanelClass::~ReplacePanelClass() {
	FindText->GetEventHandler()->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(mvceditor::ReplacePanelClass::OnKeyDown));
	ReplaceWithText->GetEventHandler()->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(mvceditor::ReplacePanelClass::OnKeyDown));
	
	FindText->GetEventHandler()->Disconnect(wxID_ANY, wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(ReplacePanelClass::OnKillFocusFindText), NULL, this);
	ReplaceWithText->GetEventHandler()->Disconnect(wxID_ANY, wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(ReplacePanelClass::OnKillFocusReplaceText), NULL, this);
}

void mvceditor::ReplacePanelClass::SetFocusOnFindText() {
	FindText->SetFocus();
}

void mvceditor::ReplacePanelClass::SetExpression(const wxString& expression) {
	FindText->SetValue(expression);
	
	// set the expression on the underlying finder  object
	TransferDataFromWindow();
}

void mvceditor::ReplacePanelClass::FindNext() {
	if (Validate() && TransferDataFromWindow()) {
		if (Finder.Prepare()) {
			Find();
		}
		else {
			SetStatus(_("Status: Invalid Expression"));	
		}
	}	
}

void mvceditor::ReplacePanelClass::FindPrevious() {
	if (Validate() && TransferDataFromWindow()) {
		if (Finder.Prepare()) {
			Find(false);
		}
		else {
			SetStatus(_("Status: Invalid Expression"));	
		}
	}	
}

void mvceditor::ReplacePanelClass::Find(bool findNext) {
	
	// only search when notebook has a current tab
	CodeControlClass* codeControl = 
			Notebook->GetCodeControl(Notebook->GetSelection());	
	if (Notebook->GetPageCount() && codeControl) {
		
		// pick up from last found spot if possible. increment/decrement so that
		// we dont find the same hit again
		int32_t position = codeControl->GetCurrentPos(),
			length = 0;
		bool found = findNext ? Finder.FindNext(codeControl->GetSafeText(), position + 1) :
			Finder.FindPrevious(codeControl->GetSafeText(), position - 1);
		if (found) {
			if (Finder.GetLastMatch(position, length)) {
				codeControl->SetSelectionAndEnsureVisible(position, position + length);
				int lineNumber = codeControl->LineFromPosition(position);
				
				// lineNumber seems to start at zero
				++lineNumber;
				SetStatus(wxString::Format(RESULT_MESSAGE, lineNumber));
			}
		}
		else {			
			SetStatus(_("Status: Not Found"));
		}
		EnableReplaceButtons(found);
	}
}

void mvceditor::ReplacePanelClass::OnNextButton(wxCommandEvent& event) {
	FindNext();
	FindHistory.Save();
}

void mvceditor::ReplacePanelClass::OnHelpButton(wxCommandEvent& event) {
	wxString help = wxString::FromAscii("Find modes:\n"
	  "Code:\n" 
	  "Type in PHP source code.  The editor will ignore whitespace "
	  "where it does not matter and will also match single and double quoted literals "
	  "if their contents match. For example, searching for\n\tstr_pos($text, 'needle')\n"
	  "will match\n\tstr_pos( $text,\"needle\" )\n\n"
	  "Exact:\n"
	  "Searching will be done using exact, case sensitive matching\n\n"
	  "Regular Expression:\n"
	  "Searching will be done using the entered regular expression. You "
	  "can use the button on the right to see all regular expression symbols."
	  "The full regular expression syntax is described at "
	  "http://www.unicode.org/reports/tr18/\n");
	help = wxGetTranslation(help);
	wxMessageBox(help, _("Replace Help"), wxOK, this);
}

void mvceditor::ReplacePanelClass::OnOkButton(wxCommandEvent& event) {
	Show(false);
}

void mvceditor::ReplacePanelClass::OnPreviousButton(wxCommandEvent& event) {
	FindPrevious();
	FindHistory.Save();
}

void mvceditor::ReplacePanelClass::SetStatus(const wxString& message) {
	ResultText->SetLabel(message);
	
	// label might grow/shrink according to new text, must
	// tell the sizer to re-position the label correctly
	// we need this for the label to be right-aligned after
	// the text change
	ResultText->GetContainingSizer()->Layout();

	// NOTE: had to add these calls otherwise the find panel would not 
	// totally repaint itself and it looks garbled when the status
	// message changes
	GetContainingSizer()->Layout();
	Refresh();
}

void mvceditor::ReplacePanelClass::OnCloseButton(wxCommandEvent& event) {
	AuiManager->GetPane(this).Hide();
	AuiManager->Update();
}

void mvceditor::ReplacePanelClass::OnReplaceButton(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow()) {
		mvceditor::CodeControlClass* codeControl = 
				Notebook->GetCodeControl(Notebook->GetSelection());	
		int32_t position = 0,
			length = 0;
		UnicodeString replaceText;
		
		// if user changed tabs, GetLastReplacementText will return false
		UnicodeString text = codeControl->GetSafeText();
		if (codeControl && Finder.GetLastMatch(position, length) &&
			Finder.GetLastReplacementText(text, replaceText)) {
			codeControl->SetSelectionByCharacterPosition(position, position + length);
			codeControl->ReplaceSelection(StringHelperClass::IcuToWx(replaceText));
			codeControl->SetSelectionByCharacterPosition(position, position + replaceText.length());
			Find(true);
			ReplaceHistory.Save();
		}
		else {			
			SetStatus(_("Status: Not Found"));
		}
	}
}

void mvceditor::ReplacePanelClass::OnReplaceAllButton(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow() && Finder.Prepare()) {
		
		// if user changed tabs, GetLastReplacementText will return false
		CodeControlClass* codeControl = 
				Notebook->GetCodeControl(Notebook->GetSelection());
		 if (codeControl) {
			 UnicodeString text = codeControl->GetSafeText();
			 int matches = Finder.ReplaceAllMatches(text);
			 codeControl->SetUnicodeText(text);
			 SetStatus(wxString::Format(wxT("Status: Replaced %d matches"), matches));	
			 ReplaceHistory.Save();
		}
		else {			
			SetStatus(_("Status: Not Found"));
		}
	}
}

void mvceditor::ReplacePanelClass::OnUndoButton(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl = 
			Notebook->GetCodeControl(Notebook->GetSelection());	
			
	// if user changes to new tab, undo will undo changes to that file
	 if (codeControl) {
		codeControl->Undo();
		int32_t position,
			length;
		if (Finder.GetLastMatch(position, length)) {
			Finder.FindPrevious(codeControl->GetSafeText(), position);
			if (Finder.GetLastMatch(position, length)) {	
				codeControl->SetSelectionByCharacterPosition(position, position + length);
			}
		}
	}
}

void mvceditor::ReplacePanelClass::OnFindEnter(wxCommandEvent& event) {
	OnNextButton(event);
}

void mvceditor::ReplacePanelClass::OnReplaceEnter(wxCommandEvent& event) {
	OnReplaceButton(event);
}

void mvceditor::ReplacePanelClass::OnKeyDown(wxKeyEvent& event) {

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

void mvceditor::ReplacePanelClass::OnRegExFindHelpButton(wxCommandEvent& event) {
	wxMenu regExMenu;
	mvceditor::PopulateRegExFindMenu(regExMenu, ID_REGEX_REPLACE_FIND_MENU_START);
	PopupMenu(&regExMenu);	
}

void mvceditor::ReplacePanelClass::OnReplaceRegExFindHelpButton(wxCommandEvent& event) {
	wxMenu regExMenu;
	mvceditor::PopulateRegExReplaceMenu(regExMenu, ID_REGEX_REPLACE_MENU_START);
	PopupMenu(&regExMenu);
}

void mvceditor::ReplacePanelClass::InsertReplaceRegExSymbol(wxCommandEvent& event) {
	wxString symbols;
	int id = event.GetId() - ID_REGEX_REPLACE_MENU_START;
	mvceditor::AddSymbolToReplaceRegularExpression(ReplaceWithText, id,CurrentInsertionPointReplace);
	FinderMode->SetSelection(FinderClass::REGULAR_EXPRESSION);
}

void mvceditor::ReplacePanelClass::InsertRegExSymbol(wxCommandEvent& event) {
	int id = event.GetId() -  ID_REGEX_REPLACE_FIND_MENU_START;
	mvceditor::AddSymbolToRegularExpression(FindText, id, CurrentInsertionPointFind);
	FinderMode->SetSelection(FinderClass::REGULAR_EXPRESSION);
}

void mvceditor::ReplacePanelClass::EnableReplaceButtons(bool enable) {
	ReplaceButton->Enable(enable);
	UndoButton->Enable(enable);
}

void mvceditor::ReplacePanelClass::OnKillFocusFindText(wxFocusEvent& event) {
	CurrentInsertionPointFind = FindText->GetInsertionPoint();
	event.Skip();
}

void mvceditor::ReplacePanelClass::OnKillFocusReplaceText(wxFocusEvent& event) {
	CurrentInsertionPointReplace = ReplaceWithText->GetInsertionPoint();
	event.Skip();
}

mvceditor::FinderPluginClass::FinderPluginClass()
	: PluginClass() {
}
	
void mvceditor::FinderPluginClass::AddEditMenuItems(wxMenu* editMenu) {
	editMenu->Append(mvceditor::MENU_FINDER + 0, _("Find\tCTRL+F"), _("Find"));
	editMenu->Append(mvceditor::MENU_FINDER + 1, _("Find Next\tF3"), _("Advance to the next match"));
	editMenu->Append(mvceditor::MENU_FINDER + 2, _("Find Previous\tSHIFT+F3"), _("Advance to the previous match"));
	editMenu->Append(mvceditor::MENU_FINDER + 3, _("Replace\tCTRL+H"), _("Find and Replace in current file"));
	editMenu->Append(mvceditor::MENU_FINDER + 4, _("Go To Line\tCTRL+G"), _("Go To Line"));
}

void mvceditor::FinderPluginClass::OnEditFind(wxCommandEvent& event) {
	wxWindow* parent = GetMainWindow();
	
	// hide replace panel id it is shown, doesnt look good when find and the replace panel are shown
	wxWindow* replaceWindow = wxWindow::FindWindowById(ID_REPLACE_PANEL, parent);
	if (replaceWindow) {
		AuiManager->GetPane(replaceWindow).Show(false);
		AuiManager->Update();
	}
	
	// show the find panel 
	wxWindow* window = wxWindow::FindWindowById(ID_FIND_PANEL, parent);
	if (!window) {
		window = new FinderPanelClass(parent, GetNotebook(), AuiManager, ID_FIND_PANEL);
		if (!AuiManager->AddPane(window, 
			wxAuiPaneInfo().Bottom().Row(1).Floatable(false).DockFixed(true).CaptionVisible(false).CloseButton(false))) {
			window->Destroy();
			window = NULL;
		}
	}
	if (window) {
		FinderPanelClass* panel = (FinderPanelClass*) window;
		wxString selectedText = GetSelectedText();
		if (!selectedText.empty()) {
			panel->SetExpression(selectedText);
		}
		AuiManager->GetPane(window).Show();
		AuiManager->Update();
		panel->SetFocusOnFindText();
	}
}
	
void mvceditor::FinderPluginClass::OnEditFindNext(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl = GetCurrentCodeControl();
	if (codeControl) {
		OnEditFind(event);
		wxWindow* window = wxWindow::FindWindowById(ID_FIND_PANEL,  GetMainWindow());
		FinderPanelClass* panel = (FinderPanelClass*) window;
		panel->FindNext();
		
		// give focus back to code control this is just better user experience	
		codeControl->SetFocus();
	}
}
	
void mvceditor::FinderPluginClass::OnEditFindPrevious(wxCommandEvent& event) {
	mvceditor::CodeControlClass* codeControl  = GetCurrentCodeControl();
	if (codeControl) {
		OnEditFind(event);
		wxWindow* window = wxWindow::FindWindowById(ID_FIND_PANEL,  GetMainWindow());
		FinderPanelClass* panel = (FinderPanelClass*) window;	
		panel->FindPrevious();	
		// give focus back to code control this is just better user experience
		codeControl->SetFocus();
	}
}

void mvceditor::FinderPluginClass::OnEditReplace(wxCommandEvent& event) {
	wxWindow* parent = GetMainWindow();
	
	// hide find panel id it is shown, doesnt look good when find and the replace panel are shown
	wxWindow* findWindow = wxWindow::FindWindowById(ID_FIND_PANEL, parent);
	if (findWindow) {
		AuiManager->GetPane(findWindow).Show(false);
		AuiManager->Update();
	}
	
	// show the replace panel
	wxWindow* window = wxWindow::FindWindowById(ID_REPLACE_PANEL, parent);
	if (!window) {
		window = new ReplacePanelClass(parent, GetNotebook(), AuiManager, ID_REPLACE_PANEL);
		if (!AuiManager->AddPane(window, 
			wxAuiPaneInfo().Bottom().Row(1).Floatable(false).DockFixed(true).CaptionVisible(false).CloseButton(false))) {
			window->Destroy();
			window = NULL;
		}
	}
	if (window) {
		ReplacePanelClass* panel = (ReplacePanelClass*) window;
		wxString selectedText = GetSelectedText();
		if (!selectedText.empty()) {
			panel->SetExpression(selectedText);
		}
		AuiManager->GetPane(window).Show();
		AuiManager->Update();
		panel->SetFocusOnFindText();
	}
}

void mvceditor::FinderPluginClass::OnEditGoToLine(wxCommandEvent& event) {
	CodeControlClass* codeControl = GetCurrentCodeControl();
	if (codeControl) {
		int maxLines = codeControl->GetLineCount();
		int goToLine = wxGetNumberFromUser(_("Enter A Line Number"), _(""), _("Go To Line Number"), 0, 1, maxLines, codeControl);
		if (goToLine >= 1) {

			// line is zero-based in scintilla
			goToLine--;
			codeControl->GotoLine(goToLine);
			codeControl->EnsureVisible(goToLine);
		}
	}
}

BEGIN_EVENT_TABLE(mvceditor::FinderPanelClass, FinderPanelGeneratedClass)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_ZERO, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_ONE, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_ZERO_OR_ONE, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_EXACT, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_AT_LEAST, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_BETWEEN, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_BEGIN_OF_LINE, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_END_OF_LINE, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_DIGIT, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_WHITE_SPACE, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_ALPHANUMERIC, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_NOT_DECIMAL, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_NOT_WHITE_SPACE, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_NOT_ALPHANUMERIC, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_CASE_SENSITIVE, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_COMMENT, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_DOT_ALL, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_MULTI_LINE, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_UWORD, mvceditor::FinderPanelClass::InsertRegExSymbol)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::ReplacePanelClass, ReplacePanelGeneratedClass)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_ONE, mvceditor::ReplacePanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_TWO, mvceditor::ReplacePanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_THREE, mvceditor::ReplacePanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_FOUR, mvceditor::ReplacePanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_FIVE, mvceditor::ReplacePanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_SEQUENCE_ONE, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_ZERO_OR_ONE, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_SEQUENCE_EXACT, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_SEQUENCE_AT_LEAST, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_SEQUENCE_BETWEEN, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_BEGIN_OF_LINE, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_END_OF_LINE, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_DIGIT, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_WHITE_SPACE, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_ALPHANUMERIC, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_NOT_DECIMAL, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_NOT_WHITE_SPACE, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_NOT_ALPHANUMERIC, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_CASE_SENSITIVE, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_COMMENT, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_DOT_ALL, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_MULTI_LINE, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_UWORD, mvceditor::ReplacePanelClass::InsertRegExSymbol)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::FinderPluginClass, wxEvtHandler) 
	EVT_MENU(mvceditor::MENU_FINDER + 0, mvceditor::FinderPluginClass::OnEditFind)
	EVT_MENU(mvceditor::MENU_FINDER + 1, mvceditor::FinderPluginClass::OnEditFindNext)
	EVT_MENU(mvceditor::MENU_FINDER + 2, mvceditor::FinderPluginClass::OnEditFindPrevious)
	EVT_MENU(mvceditor::MENU_FINDER + 3, mvceditor::FinderPluginClass::OnEditReplace)
	EVT_MENU(mvceditor::MENU_FINDER + 4, mvceditor::FinderPluginClass::OnEditGoToLine)
END_EVENT_TABLE()

