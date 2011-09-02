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
#include <wx/artprov.h>
#include <wx/numdlg.h>
#include <wx/valgen.h>
#include <wx/valtext.h>

const int ID_MENU_FIND = mvceditor::PluginClass::newMenuId();
const int ID_MENU_FIND_NEXT = mvceditor::PluginClass::newMenuId();
const int ID_MENU_FIND_PREVIOUS = mvceditor::PluginClass::newMenuId();
const int ID_MENU_GOTO_LINE = mvceditor::PluginClass::newMenuId();
const int ID_MENU_REPLACE = mvceditor::PluginClass::newMenuId();
const int ID_FIND_PANEL = wxNewId(); 
const int ID_REPLACE_PANEL = wxNewId(); 
const int ID_MENU_REG_EX_SEQUENCE_ZERO = wxNewId();
const int ID_MENU_REG_EX_SEQUENCE_ONE = wxNewId();
const int ID_MENU_REG_EX_ZERO_OR_ONE = wxNewId();
const int ID_MENU_REG_EX_SEQUENCE_EXACT = wxNewId();
const int ID_MENU_REG_EX_SEQUENCE_AT_LEAST = wxNewId();
const int ID_MENU_REG_EX_SEQUENCE_BETWEEN = wxNewId();
const int ID_MENU_REG_EX_BEGIN_OF_LINE = wxNewId();
const int ID_MENU_REG_EX_END_OF_LINE = wxNewId();
const int ID_MENU_REG_EX_DIGIT = wxNewId();
const int ID_MENU_REG_EX_WHITE_SPACE = wxNewId();
const int ID_MENU_REG_EX_ALPHANUMERIC = wxNewId();
const int ID_MENU_REG_EX_NOT_DECIMAL = wxNewId();
const int ID_MENU_REG_EX_NOT_WHITE_SPACE = wxNewId();
const int ID_MENU_REG_EX_NOT_ALPHANUMERIC = wxNewId();
const int ID_MENU_REG_EX_CASE_SENSITIVE = wxNewId();
const int ID_MENU_REG_EX_COMMENT = wxNewId();
const int ID_MENU_REG_EX_DOT_ALL = wxNewId();
const int ID_MENU_REG_EX_MULTI_LINE = wxNewId();
const int ID_MENU_REG_EX_UWORD = wxNewId();

const int ID_MENU_REG_EX_REPLACE_MATCH_ONE = wxNewId();
const int ID_MENU_REG_EX_REPLACE_MATCH_TWO = wxNewId();
const int ID_MENU_REG_EX_REPLACE_MATCH_THREE = wxNewId();
const int ID_MENU_REG_EX_REPLACE_MATCH_FOUR = wxNewId();
const int ID_MENU_REG_EX_REPLACE_MATCH_FIVE = wxNewId();


/**
 * Add all of the regular expression flags to the given menu
 */
void PopulateRegExMenu(wxMenu& regExMenu);

/**
 * Alter the given textbox (that contains a regular expression) depending on the user choice. 
 * Note that the caret will be left where the text was inserted
 * 
 * @param wxComboBox& the textbox containing the regular expression (will be modified in place)
 * @param int menuId the menu ID that the user chose.
 */
void AddSymbolToRegularExpression(wxComboBox* text, const int menuId);

/**
 * Add a flag to the given regular expression string, taking care not to clobber existing flags
 * 
 * @param wxComboBox& the textbox containing the regular expression (will be modified in place)
 * @param wxString flag the flag to add to the regular expression
 */
void AddFlagToRegEx(wxComboBox* text, wxString flag);


mvceditor::FinderPanelClass::FinderPanelClass(wxWindow* parent, mvceditor::NotebookClass* notebook, wxAuiManager* auiManager, int windowId)
		: FinderPanelGeneratedClass(parent, windowId)
		, Finder()
		, ComboBoxHistory(FindText)
		, Notebook(notebook)
		, AuiManager(auiManager) {
	UnicodeStringValidatorClass expressionValidator(&Finder.Expression);
	FindText->SetValidator(expressionValidator);
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
  //TODO: get the strings from translation "_()" macro
  wxString help = wxT("Find modes:\n")
	  wxT("Code:\n" )
	  wxT("Type in PHP source code.  The editor will ignore whitespace ")
	  wxT("where it does not matter and will also match single and double quoted literals ")
	  wxT("if their contents match. For example, searching for\n\tstr_pos($text, 'needle')\n")
	  wxT("will match\n\tstr_pos( $text,\"needle\" )\n\n")
	  wxT("Exact:\n")
	  wxT("Searching will be done using exact, case sensitive matching\n\n")
	  wxT("Regular Expression:\n")
	  wxT("Searching will be done using the entered regular expression. Regular ")
	  wxT("expression syntax is that of an \"Advanced Regular Expression\" (ARE) as ")
	  wxT("described at http://docs.wxwidgets.org/stable/wx_wxresyn.html\n");
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
	PopulateRegExMenu(regExMenu);
	PopupMenu(&regExMenu);	
}

void mvceditor::FinderPanelClass::InsertRegExSymbol(wxCommandEvent& event) {
	int id = event.GetId();
	AddSymbolToRegularExpression(FindText, id);
	FinderMode->SetSelection(FinderClass::REGULAR_EXPRESSION);
}

mvceditor::ReplacePanelClass::ReplacePanelClass(wxWindow* parent, mvceditor::NotebookClass* notebook, wxAuiManager* auiManager, int windowId)
		: ReplacePanelGeneratedClass(parent, windowId)
		, Finder()
		, FindHistory(FindText)
		, ReplaceHistory(ReplaceWithText)
		, Notebook(notebook)
		, AuiManager(auiManager) {
	UnicodeStringValidatorClass expressionValidator(&Finder.Expression);
	FindText->SetValidator(expressionValidator);
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
  //TODO: get the strings from translation "_()" macro
  wxString help = wxT("Find modes:\n")
	  wxT("Code:\n" )
	  wxT("Type in PHP source code.  The editor will ignore whitespace ")
	  wxT("where it does not matter and will also match single and double quoted literals ")
	  wxT("if their contents match. For example, searching for\n\tstr_pos($text, 'needle')\n")
	  wxT("will match\n\tstr_pos( $text,\"needle\" )\n\n")
	  wxT("Exact:\n")
	  wxT("Searching will be done using exact, case sensitive matching\n\n")
	  wxT("Regular Expression:\n")
	  wxT("Searching will be done using the entered regular expression. Regular ")
	  wxT("expression syntax is that of an \"Advanced Regular Expression\" (ARE) as ")
	  wxT("described at http://docs.wxwidgets.org/stable/wx_wxresyn.html\n");
	wxMessageBox(help, _("Find Help"), wxOK, this);
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
			 
			 // TODO find a more efficient way. Is this taking 3x the memory?
			 UnicodeString text = codeControl->GetSafeText();
			 int matches = Finder.ReplaceAllMatches(text);
			 codeControl->SetText(StringHelperClass::IcuToWx(text));
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

void mvceditor::ReplacePanelClass::OnRegExFindHelpButton(wxCommandEvent& event) {
	wxMenu regExMenu;
	PopulateRegExMenu(regExMenu);
	PopupMenu(&regExMenu);	
}

void mvceditor::ReplacePanelClass::OnReplaceRegExFindHelpButton(wxCommandEvent& event) {
	wxMenu regExMenu;
	regExMenu.Append(ID_MENU_REG_EX_REPLACE_MATCH_ONE,		_("$1      Match Group 1"));
	regExMenu.Append(ID_MENU_REG_EX_REPLACE_MATCH_TWO,		_("$2      Match Group 2"));
	regExMenu.Append(ID_MENU_REG_EX_REPLACE_MATCH_THREE,	_("$3      Match Group 3"));
	regExMenu.Append(ID_MENU_REG_EX_REPLACE_MATCH_FOUR,		_("$4      Match Group 4"));
	regExMenu.Append(ID_MENU_REG_EX_REPLACE_MATCH_FIVE,		_("$5      Match Group 5"));
	PopupMenu(&regExMenu);	
}

void mvceditor::ReplacePanelClass::InsertReplaceRegExSymbol(wxCommandEvent& event) {
	wxString symbols;
	int id = event.GetId();
	if (id == ID_MENU_REG_EX_REPLACE_MATCH_ONE) {
		symbols = wxT("$1");
	}
	if (id == ID_MENU_REG_EX_REPLACE_MATCH_TWO) {
		symbols = wxT("$2");
	}
	if (id == ID_MENU_REG_EX_REPLACE_MATCH_THREE) {
		symbols = wxT("$3");
	}
	if (id == ID_MENU_REG_EX_REPLACE_MATCH_FOUR) {
		symbols = wxT("$4");
	}
	if (id == ID_MENU_REG_EX_REPLACE_MATCH_FIVE) {
		symbols = wxT("$5");
	}
	if (!symbols.IsEmpty()) {
		ReplaceWithText->Replace(ReplaceWithText->GetInsertionPoint(), ReplaceWithText->GetInsertionPoint(), symbols);
	}
	FinderMode->SetSelection(FinderClass::REGULAR_EXPRESSION);
}

void mvceditor::ReplacePanelClass::InsertRegExSymbol(wxCommandEvent& event) {
	int id = event.GetId();
	AddSymbolToRegularExpression(FindText, id);
	FinderMode->SetSelection(FinderClass::REGULAR_EXPRESSION);
}

void mvceditor::ReplacePanelClass::EnableReplaceButtons(bool enable) {
	ReplaceButton->Enable(enable);
	UndoButton->Enable(enable);
}

mvceditor::FinderPluginClass::FinderPluginClass()
	: PluginClass() {
}
	
void mvceditor::FinderPluginClass::AddEditMenuItems(wxMenu* editMenu) {
	editMenu->Append(ID_MENU_FIND, _("Find\tCTRL+F"), _("Find"));
	editMenu->Append(ID_MENU_FIND_NEXT, _("Find Next\tF3"), _("Advance to the next match"));
	editMenu->Append(ID_MENU_FIND_PREVIOUS, _("Find Previous\tSHIFT+F3"), _("Advance to the previous match"));
	editMenu->Append(ID_MENU_REPLACE, _("Replace\tCTRL+H"), _("Find and Replace in current file"));
	editMenu->Append(ID_MENU_GOTO_LINE, _("Go To Line\tCTRL+G"), _("Go To Line"));
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
	if (GetCurrentCodeControl()) {
		OnEditFind(event);
		wxWindow* window = wxWindow::FindWindowById(ID_FIND_PANEL,  GetMainWindow());
		FinderPanelClass* panel = (FinderPanelClass*) window;
		panel->FindNext();
		
		// give focus back to code control this is just better user experience	
		GetCurrentCodeControl()->SetFocus();
	}
}
	
void mvceditor::FinderPluginClass::OnEditFindPrevious(wxCommandEvent& event) {
	if (GetCurrentCodeControl()) {
		OnEditFind(event);
		wxWindow* window = wxWindow::FindWindowById(ID_FIND_PANEL,  GetMainWindow());
		FinderPanelClass* panel = (FinderPanelClass*) window;	
		panel->FindPrevious();	
		// give focus back to code control this is just better user experience
		GetCurrentCodeControl()->SetFocus();
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

void PopulateRegExMenu(wxMenu& regExMenu) {
	regExMenu.Append(ID_MENU_REG_EX_SEQUENCE_ZERO, _("*      Sequence of 0 or more"));
	regExMenu.Append(ID_MENU_REG_EX_SEQUENCE_ONE, _("+      Sequence of 1 or more"));
	regExMenu.Append(ID_MENU_REG_EX_ZERO_OR_ONE, _("?      Sequence of 0 or 1"));
	regExMenu.Append(ID_MENU_REG_EX_SEQUENCE_EXACT, _("{m}    Sequence of m"));
	regExMenu.Append(ID_MENU_REG_EX_SEQUENCE_AT_LEAST, _("{m,}   Sequence of at least m"));
	regExMenu.Append(ID_MENU_REG_EX_SEQUENCE_BETWEEN, _("{m,n}  Sequence of m through n matches (inclusive)"));
	regExMenu.Append(ID_MENU_REG_EX_BEGIN_OF_LINE, _("^      Beginning of line"));
	regExMenu.Append(ID_MENU_REG_EX_END_OF_LINE, _("$      End of line"));
	regExMenu.AppendSeparator();
	regExMenu.Append(ID_MENU_REG_EX_DIGIT, _("\\d    Decimal digit"));
	regExMenu.Append(ID_MENU_REG_EX_WHITE_SPACE, _("\\s    White space character"));
	regExMenu.Append(ID_MENU_REG_EX_ALPHANUMERIC, _("\\w    Alphanumeric (letter or digit)"));
	regExMenu.Append(ID_MENU_REG_EX_NOT_DECIMAL, _("\\D    Not a decimal digit"));
	regExMenu.Append(ID_MENU_REG_EX_NOT_WHITE_SPACE, _("\\S    Not a white space character"));
	regExMenu.Append(ID_MENU_REG_EX_NOT_ALPHANUMERIC, _("\\W    Not an Alphanumeric (letter or digit)"));
	regExMenu.AppendSeparator();
	regExMenu.Append(ID_MENU_REG_EX_CASE_SENSITIVE, _("(?i)    Case insensitive matching"));
	regExMenu.Append(ID_MENU_REG_EX_COMMENT, _("(?x)    allow use of white space and #comments within patterns"));
	regExMenu.Append(ID_MENU_REG_EX_DOT_ALL, _("(?s)    A dot (.) will match line terminator. \r\n will be treated as one character."));
	regExMenu.Append(ID_MENU_REG_EX_MULTI_LINE, _("(?m)    (^) and ($) will also match at the start and end of each line"));
	regExMenu.Append(ID_MENU_REG_EX_UWORD, _("(?w)    word boundaries are found according to the definitions of word found in Unicode UAX 29"));
}

void AddSymbolToRegularExpression(wxComboBox* text, int id) {
	wxString symbols;
	if (id == ID_MENU_REG_EX_SEQUENCE_ZERO) {
		symbols = wxT("*");
	}
	if (id == ID_MENU_REG_EX_SEQUENCE_ONE) {
		symbols = wxT("+");
	}
	if (id == ID_MENU_REG_EX_ZERO_OR_ONE) {
		symbols = wxT("?");
	}
	if (id == ID_MENU_REG_EX_SEQUENCE_EXACT) {
		symbols = wxT("{m}");
	}
	if (id == ID_MENU_REG_EX_SEQUENCE_AT_LEAST) {
		symbols = wxT("{m,}");
	}
	if (id == ID_MENU_REG_EX_SEQUENCE_BETWEEN) {
		symbols = wxT("{m,n}");
	}
	if (id == ID_MENU_REG_EX_BEGIN_OF_LINE) {
		symbols = wxT("^");
	}
	if (id == ID_MENU_REG_EX_END_OF_LINE) {
		symbols = wxT("$");
	}
	if (id == ID_MENU_REG_EX_DIGIT) {
		symbols = wxT("\\d");
	}
	if (id == ID_MENU_REG_EX_WHITE_SPACE) {
		symbols = wxT("\\s");
	}
	if (id == ID_MENU_REG_EX_ALPHANUMERIC) {
		symbols = wxT("\\w");
	}
	if (id == ID_MENU_REG_EX_NOT_DECIMAL) {
		symbols = wxT("\\D");
	}
	if (id == ID_MENU_REG_EX_NOT_WHITE_SPACE) {
		symbols = wxT("\\S");
	}
	if (id == ID_MENU_REG_EX_NOT_ALPHANUMERIC) {
		symbols = wxT("\\W");
	}
	if (!symbols.IsEmpty()) {
		text->Replace(text->GetInsertionPoint(), text->GetInsertionPoint(), symbols);
	}
	
	// reg ex flags always go at the beginning
	else if (id == ID_MENU_REG_EX_CASE_SENSITIVE) {
		AddFlagToRegEx(text, wxT("i"));
	}
	else if (id == ID_MENU_REG_EX_COMMENT) {
		AddFlagToRegEx(text, wxT("x"));
	}
	else if (id == ID_MENU_REG_EX_DOT_ALL) {
		AddFlagToRegEx(text, wxT("s"));
	}
	else if (id == ID_MENU_REG_EX_MULTI_LINE) {
		AddFlagToRegEx(text, wxT("m"));
	}
	else if (id == ID_MENU_REG_EX_UWORD) {
		AddFlagToRegEx(text, wxT("w"));
	}
}

void AddFlagToRegEx(wxComboBox* text, wxString flag) {
	wxString value = text->GetValue();
	
	//meta syntax (?i) at the start of the regex. we need to put the new flag, but only if it is not already there
	wxString startFlag(wxT("(?"));
	if (0 == value.Find(startFlag)) {
		int afterMetasPos = value.Find(wxT(")"));
		if (afterMetasPos > 0) {
			wxString flags = value.SubString(0, afterMetasPos);
			if (wxNOT_FOUND == flags.Find(flag)) {
				text->Replace(afterMetasPos - 1, afterMetasPos - 1, flag);
			}
		} 
	}
	else {
		value = startFlag + flag + wxT(")") + value;
		text->SetValue(value);
	}
}

BEGIN_EVENT_TABLE(mvceditor::FinderPanelClass, FinderPanelGeneratedClass)
	EVT_MENU(ID_MENU_REG_EX_SEQUENCE_ZERO, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_SEQUENCE_ONE, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_ZERO_OR_ONE, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_SEQUENCE_EXACT, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_SEQUENCE_AT_LEAST, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_SEQUENCE_BETWEEN, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_BEGIN_OF_LINE, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_END_OF_LINE, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_DIGIT, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_WHITE_SPACE, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_ALPHANUMERIC, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_NOT_DECIMAL, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_NOT_WHITE_SPACE, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_NOT_ALPHANUMERIC, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_CASE_SENSITIVE, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_COMMENT, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_DOT_ALL, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_MULTI_LINE, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_UWORD, mvceditor::FinderPanelClass::InsertRegExSymbol)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::ReplacePanelClass, ReplacePanelGeneratedClass)
	EVT_MENU(ID_MENU_REG_EX_REPLACE_MATCH_ONE, mvceditor::ReplacePanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_REPLACE_MATCH_TWO, mvceditor::ReplacePanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_REPLACE_MATCH_THREE, mvceditor::ReplacePanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_REPLACE_MATCH_FOUR, mvceditor::ReplacePanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_REPLACE_MATCH_FIVE, mvceditor::ReplacePanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_SEQUENCE_ONE, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_ZERO_OR_ONE, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_SEQUENCE_EXACT, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_SEQUENCE_AT_LEAST, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_SEQUENCE_BETWEEN, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_BEGIN_OF_LINE, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_END_OF_LINE, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_DIGIT, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_WHITE_SPACE, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_ALPHANUMERIC, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_NOT_DECIMAL, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_NOT_WHITE_SPACE, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_NOT_ALPHANUMERIC, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_CASE_SENSITIVE, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_COMMENT, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_DOT_ALL, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_MULTI_LINE, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_MENU_REG_EX_UWORD, mvceditor::ReplacePanelClass::InsertRegExSymbol)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::FinderPluginClass, wxEvtHandler) 
	EVT_MENU(ID_MENU_FIND, mvceditor::FinderPluginClass::OnEditFind)
	EVT_MENU(ID_MENU_FIND_NEXT, mvceditor::FinderPluginClass::OnEditFindNext)
	EVT_MENU(ID_MENU_FIND_PREVIOUS, mvceditor::FinderPluginClass::OnEditFindPrevious)
	EVT_MENU(ID_MENU_REPLACE, mvceditor::FinderPluginClass::OnEditReplace)
	EVT_MENU(ID_MENU_GOTO_LINE, mvceditor::FinderPluginClass::OnEditGoToLine)
END_EVENT_TABLE()

