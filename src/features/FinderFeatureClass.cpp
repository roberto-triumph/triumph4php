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
#include <features/FinderFeatureClass.h>
#include <globals/String.h>
#include <widgets/UnicodeStringValidatorClass.h>
#include <widgets/RegularExpressionValidatorClass.h>
#include <wx/artprov.h>
#include <wx/numdlg.h>
#include <wx/valgen.h>
#include <wx/valtext.h>

static const int ID_FIND_PANEL = wxNewId(); 
static const int ID_REPLACE_PANEL = wxNewId(); 

// these IDs are needed so that the IDs of the Regular expression help menu
// do not collide with the menu IDs of the FindInFilesFeature
static const int ID_REGEX_MENU_START = 11000;
static const int ID_REGEX_REPLACE_FIND_MENU_START = 12000;
static const int ID_REGEX_REPLACE_MENU_START = 13000;

mvceditor::FinderPanelClass::FinderPanelClass(wxWindow* parent, int windowId, 
											  mvceditor::FinderClass& finder,
											  mvceditor::NotebookClass* notebook, wxAuiManager* auiManager)
		: FinderPanelGeneratedClass(parent, windowId)
		, Finder(finder)
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
	TransferDataToWindow();
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
				
			wxConfigBase* config = wxConfigBase::Get();
			config->Write(wxT("/Finder/Wrap"), Finder.Wrap);
			config->Write(wxT("/Finder/Mode"), Finder.Mode);
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

			wxConfigBase* config = wxConfigBase::Get();
			config->Write(wxT("/Finder/Wrap"), Finder.Wrap);
			config->Write(wxT("/Finder/Mode"), Finder.Mode);
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
		// we searching backwards, start from 2 positions before because the selection
		// starts at 1 PAST the hit
		int32_t position = codeControl->GetCurrentPos(),
			length = 0;
		bool found = findNext ? Finder.FindNext(codeControl->GetSafeText(), position + 1) :
			Finder.FindPrevious(codeControl->GetSafeText(), position - 2);
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

	// give focus back to the code control
	// better user experience
	mvceditor::CodeControlClass* codeControl = Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->SetFocus();
	}
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

void mvceditor::FinderPanelClass::OnFindKillFocus(wxFocusEvent& event) {

	// connect to the KILL_FOCUS events so that we can capture the insertion point
	// on Win32 GetInsertionPoint() returns 0 when the combo box is no
	// in focus; we must receive the position via an outside mechanism
	CurrentInsertionPointFind = FindText->GetInsertionPoint();
	event.Skip();
}

void mvceditor::FinderPanelClass::OnFindKeyDown(wxKeyEvent& event) {
	if (event.GetKeyCode() == WXK_ESCAPE) {
		AuiManager->GetPane(this).Hide();
		AuiManager->Update();

		// give focus back to the code control
		// better user experience
		mvceditor::CodeControlClass* codeControl = Notebook->GetCurrentCodeControl();
		if (codeControl) {
			codeControl->SetFocus();
		}
	}
	else {
		event.Skip();
	}	
}

mvceditor::ReplacePanelClass::ReplacePanelClass(wxWindow* parent, int windowId, mvceditor::FinderClass& finder,
												mvceditor::NotebookClass* notebook, wxAuiManager* auiManager)
		: ReplacePanelGeneratedClass(parent, windowId)
		, Finder(finder)
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
	TransferDataToWindow();
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

			wxConfigBase* config = wxConfigBase::Get();
			config->Write(wxT("/Finder/ReplaceWrap"), Finder.Wrap);
			config->Write(wxT("/Finder/ReplaceMode"), Finder.Mode);
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

			wxConfigBase* config = wxConfigBase::Get();
			config->Write(wxT("/Finder/ReplaceWrap"), Finder.Wrap);
			config->Write(wxT("/Finder/ReplaceMode"), Finder.Mode);
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
		// we searching backwards, start from 2 positions before because the selection
		// starts at 1 PAST the hit
		int32_t position = codeControl->GetCurrentPos(),
			length = 0;
		bool found = findNext ? Finder.FindNext(codeControl->GetSafeText(), position + 1) :
			Finder.FindPrevious(codeControl->GetSafeText(), position - 2);
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

	// give focus back to the code control
	// better user experience
	mvceditor::CodeControlClass* codeControl = Notebook->GetCurrentCodeControl();
	if (codeControl) {
		codeControl->SetFocus();
	}
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
			codeControl->ReplaceSelection(mvceditor::IcuToWx(replaceText));
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

void mvceditor::ReplacePanelClass::OnFindKeyDown(wxKeyEvent& event) {

	// since this panel handles EVT_TEXT_ENTER, we need to handle the
	// tab traversal ourselves otherwise tab travesal wont work
	if (event.GetKeyCode() == WXK_TAB && event.ShiftDown()) {
		FindText->Navigate(wxNavigationKeyEvent::IsBackward);
	}
	else if (event.GetKeyCode() == WXK_TAB) {
		FindText->Navigate(wxNavigationKeyEvent::IsForward);
	}
	else  if (event.GetKeyCode() == WXK_ESCAPE) {
		AuiManager->GetPane(this).Hide();
		AuiManager->Update();

		// give focus back to the code control
		// better user experience
		mvceditor::CodeControlClass* codeControl = Notebook->GetCurrentCodeControl();
		if (codeControl) {
			codeControl->SetFocus();
		}
	}
	else {
		event.Skip();
	}
}

void mvceditor::ReplacePanelClass::OnReplaceKeyDown(wxKeyEvent& event) {

	// since this panel handles EVT_TEXT_ENTER, we need to handle the
	// tab traversal ourselves otherwise tab travesal wont work
	if (event.GetKeyCode() == WXK_TAB && event.ShiftDown()) {
		ReplaceWithText->Navigate(wxNavigationKeyEvent::IsBackward);
		event.Skip();
	}
	else if (event.GetKeyCode() == WXK_TAB ) {
		ReplaceWithText->Navigate(wxNavigationKeyEvent::IsForward);
		event.Skip();
	}
	else  if (event.GetKeyCode() == WXK_ESCAPE) {
		AuiManager->GetPane(this).Hide();
		AuiManager->Update();

		// give focus back to the code control
		// better user experience
		mvceditor::CodeControlClass* codeControl = Notebook->GetCurrentCodeControl();
		if (codeControl) {
			codeControl->SetFocus();
		}
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

void mvceditor::ReplacePanelClass::OnFindKillFocus(wxFocusEvent& event) {

	// connect to the KILL_FOCUS events so that we can capture the insertion point
	// on Win32 GetInsertionPoint() returns 0 when the combo box is no
	// in focus; we must receive the position via an outside mechanism
	CurrentInsertionPointFind = FindText->GetInsertionPoint();
	event.Skip();
}

void mvceditor::ReplacePanelClass::OnReplaceKillFocus(wxFocusEvent& event) {

	// connect to the KILL_FOCUS events so that we can capture the insertion point
	// on Win32 GetInsertionPoint() returns 0 when the combo box is no
	// in focus; we must receive the position via an outside mechanism
	CurrentInsertionPointReplace = ReplaceWithText->GetInsertionPoint();
	event.Skip();
}

mvceditor::FinderFeatureClass::FinderFeatureClass(mvceditor::AppClass& app)
	: FeatureClass(app) 
	, Finder()
	, FinderReplace() {
}
	
void mvceditor::FinderFeatureClass::AddEditMenuItems(wxMenu* editMenu) {
	editMenu->Append(mvceditor::MENU_FINDER + 0, _("Find\tCTRL+F"), _("Find"));
	editMenu->Append(mvceditor::MENU_FINDER + 1, _("Find Next\tF3"), _("Advance to the next match"));
	editMenu->Append(mvceditor::MENU_FINDER + 2, _("Find Previous\tSHIFT+F3"), _("Advance to the previous match"));
	editMenu->Append(mvceditor::MENU_FINDER + 3, _("Replace\tCTRL+H"), _("Find and Replace in current file"));
	editMenu->Append(mvceditor::MENU_FINDER + 4, _("Go To Line\tCTRL+G"), _("Go To Line"));
}

void mvceditor::FinderFeatureClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_FINDER + 0] = wxT("Find-Show Find Panel");
	menuItemIds[mvceditor::MENU_FINDER + 1] = wxT("Find-Next");
	menuItemIds[mvceditor::MENU_FINDER + 2] = wxT("Find-Previous");
	menuItemIds[mvceditor::MENU_FINDER + 3] = wxT("Find-Replace");
	menuItemIds[mvceditor::MENU_FINDER + 4] = wxT("Find-Go To Line");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::FinderFeatureClass::LoadPreferences(wxConfigBase* config) {
	config->Read(wxT("/Finder/Wrap"), &Finder.Wrap);
	config->Read(wxT("/Finder/Mode"), &Finder.Mode);
	config->Read(wxT("/Finder/ReplaceWrap"), &FinderReplace.Wrap);
	config->Read(wxT("/Finder/ReplaceMode"), &FinderReplace.Mode);
}


void mvceditor::FinderFeatureClass::OnEditFind(wxCommandEvent& event) {
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
		window = new FinderPanelClass(parent, ID_FIND_PANEL, Finder, GetNotebook(), AuiManager);
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
	
void mvceditor::FinderFeatureClass::OnEditFindNext(wxCommandEvent& event) {
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
	
void mvceditor::FinderFeatureClass::OnEditFindPrevious(wxCommandEvent& event) {
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

void mvceditor::FinderFeatureClass::OnEditReplace(wxCommandEvent& event) {
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
		window = new ReplacePanelClass(parent, ID_REPLACE_PANEL, FinderReplace, GetNotebook(), AuiManager);
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

void mvceditor::FinderFeatureClass::OnEditGoToLine(wxCommandEvent& event) {
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
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_PHP_STRING, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_PHP_VARIABLE, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_PHP_NUMBER, mvceditor::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_PHP_WHITESPACE, mvceditor::FinderPanelClass::InsertRegExSymbol)
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
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_PHP_STRING, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_PHP_VARIABLE, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_PHP_NUMBER, mvceditor::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_PHP_WHITESPACE, mvceditor::ReplacePanelClass::InsertRegExSymbol)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::FinderFeatureClass, wxEvtHandler) 
	EVT_MENU(mvceditor::MENU_FINDER + 0, mvceditor::FinderFeatureClass::OnEditFind)
	EVT_MENU(mvceditor::MENU_FINDER + 1, mvceditor::FinderFeatureClass::OnEditFindNext)
	EVT_MENU(mvceditor::MENU_FINDER + 2, mvceditor::FinderFeatureClass::OnEditFindPrevious)
	EVT_MENU(mvceditor::MENU_FINDER + 3, mvceditor::FinderFeatureClass::OnEditReplace)
	EVT_MENU(mvceditor::MENU_FINDER + 4, mvceditor::FinderFeatureClass::OnEditGoToLine)
END_EVENT_TABLE()

