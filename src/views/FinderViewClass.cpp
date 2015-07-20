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
 * @copyright  2015 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <views/FinderViewClass.h>
#include <features/FinderFeatureClass.h>
#include <globals/String.h>
#include <code_control/CodeControlClass.h>
#include <widgets/UnicodeStringValidatorClass.h>
#include <widgets/RegularExpressionValidatorClass.h>
#include <widgets/Buttons.h>
#include <widgets/AuiManager.h>
#include <globals/Assets.h>
#include <wx/artprov.h>
#include <wx/numdlg.h>
#include <wx/valgen.h>
#include <wx/valtext.h>
#include <wx/stc/stc.h>
#include <Triumph.h>

static const int ID_FIND_PANEL = wxNewId();
static const int ID_REPLACE_PANEL = wxNewId();

// these IDs are needed so that the IDs of the Regular expression help menu
// do not collide with the menu IDs of the FindInFilesFeature
static const int ID_REGEX_MENU_START = 11000;
static const int ID_REGEX_REPLACE_FIND_MENU_START = 12000;
static const int ID_REGEX_REPLACE_MENU_START = 13000;

t4p::FinderViewClass::FinderViewClass(t4p::FinderFeatureClass& feature)
: FeatureViewClass()
, Feature(feature) {
}

void t4p::FinderViewClass::AddEditMenuItems(wxMenu* editMenu) {
	editMenu->Append(t4p::MENU_FINDER + 0, _("Find\tCTRL+F"), _("Find"));
	editMenu->Append(t4p::MENU_FINDER + 1, _("Find Next\tF3"), _("Advance to the next match"));
	editMenu->Append(t4p::MENU_FINDER + 2, _("Find Previous\tSHIFT+F3"), _("Advance to the previous match"));
	editMenu->Append(t4p::MENU_FINDER + 3, _("Replace\tCTRL+H"), _("Find and Replace in current file"));
	editMenu->Append(t4p::MENU_FINDER + 4, _("Go To Line\tCTRL+G"), _("Go To Line"));
}

void t4p::FinderViewClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[t4p::MENU_FINDER + 0] = wxT("Find-Show Find Panel");
	menuItemIds[t4p::MENU_FINDER + 1] = wxT("Find-Next");
	menuItemIds[t4p::MENU_FINDER + 2] = wxT("Find-Previous");
	menuItemIds[t4p::MENU_FINDER + 3] = wxT("Find-Replace");
	menuItemIds[t4p::MENU_FINDER + 4] = wxT("Find-Go To Line");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void t4p::FinderViewClass::OnEditFind(wxCommandEvent& event) {
	wxWindow* parent = GetMainWindow();
	parent->Freeze();
	bool transferExpression = false;

	// hide replace panel id it is shown, doesnt look good when find and the replace panel are shown
	wxWindow* replaceWindow = wxWindow::FindWindowById(ID_REPLACE_PANEL, parent);
	if (replaceWindow) {
		AuiManager->GetPane(replaceWindow).Show(false);
		replaceWindow->TransferDataFromWindow();
		transferExpression = true;
	}

	// show the find panel
	wxWindow* window = wxWindow::FindWindowById(ID_FIND_PANEL, parent);
	if (!window) {
		window = new FinderPanelClass(parent, ID_FIND_PANEL, Feature.Finder, *this, AuiManager);
		wxAuiPaneInfo info;

		// always put the finder panel at row 1
		// row=1 means that it will show up above the
		// tools notebook (row = 0) but below any code
		// notebooks (row=2+)
		int row = 1;
		info.Bottom().Row(row).Floatable(false)
			.CaptionVisible(false).CloseButton(false);
		if (!AuiManager->InsertPane(window, info, wxAUI_INSERT_ROW)) {
			window->Destroy();
			window = NULL;
		}
	}
	if (window) {
		FinderPanelClass* panel = reinterpret_cast<FinderPanelClass*>(window);
		wxString selectedText = GetSelectedText();
		if (selectedText.empty() && transferExpression) {
			selectedText = t4p::IcuToWx(Feature.FinderReplace.Expression);
		}
		if (!selectedText.empty()) {
			panel->SetExpression(selectedText);
		}
		AuiManager->GetPane(window).Show();
		panel->SetFocusOnFindText();
	}
	parent->Thaw();
	AuiManager->Update();
}

void t4p::FinderViewClass::OnEditFindNext(wxCommandEvent& event) {
	t4p::CodeControlClass* codeControl = GetCurrentCodeControl();
	if (codeControl) {
		// a couple of situations may be possible here
		// 1. the replace panel is shown
		// 2. the find panel is shown
		// 3. neither find or replace panel are shown
		//
		// solutions
		// when the replace panel is shown, use the expression in the replace panel
		// when the find panel is shown, use the expression in the find panel
		// when no panel is shown, show the find panel
		wxWindow* parent = GetMainWindow();
		wxWindow* replaceWindow = wxWindow::FindWindowById(ID_REPLACE_PANEL, parent);
		wxWindow* findWindow = wxWindow::FindWindowById(ID_FIND_PANEL, parent);
		if (replaceWindow && AuiManager->GetPane(replaceWindow).IsShown()) {
			ReplacePanelClass* panel = reinterpret_cast<ReplacePanelClass*>(replaceWindow);
			panel->FindNext();

			// give focus back to code control this is just better user experience
			codeControl->SetFocus();
		} else if (findWindow && AuiManager->GetPane(findWindow).IsShown()) {
			FinderPanelClass* panel = reinterpret_cast<FinderPanelClass*>(findWindow);
			panel->FindNext();

			// give focus back to code control this is just better user experience
			codeControl->SetFocus();
		} else {
			OnEditFind(event);
		}
	}
}

void t4p::FinderViewClass::OnEditFindPrevious(wxCommandEvent& event) {
	t4p::CodeControlClass* codeControl  = GetCurrentCodeControl();
	if (codeControl) {
		OnEditFind(event);
		wxWindow* window = wxWindow::FindWindowById(ID_FIND_PANEL,  GetMainWindow());
		FinderPanelClass* panel = reinterpret_cast<FinderPanelClass*>(window);
		panel->FindPrevious();
		// give focus back to code control this is just better user experience
		codeControl->SetFocus();
	}
}

void t4p::FinderViewClass::OnEditReplace(wxCommandEvent& event) {
	wxWindow* parent = GetMainWindow();
	parent->Freeze();
	bool transferExpression = false;

	// hide find panel id it is shown, doesnt look good when find and the replace panel are shown
	wxWindow* findWindow = wxWindow::FindWindowById(ID_FIND_PANEL, parent);
	if (findWindow) {
		// so that we can get the most up-to-date value from the textbox
		findWindow->TransferDataFromWindow();

		AuiManager->GetPane(findWindow).Show(false);
		transferExpression = true;
	}

	// show the replace panel
	wxWindow* window = wxWindow::FindWindowById(ID_REPLACE_PANEL, parent);
	if (!window) {
		window = new ReplacePanelClass(parent, ID_REPLACE_PANEL, Feature.FinderReplace, *this, AuiManager);
		wxAuiPaneInfo info;

		// always put the finder panel at row 1
		// row=1 means that it will show up above the
		// tools notebook (row = 0) but below any code
		// notebooks (row=2+)
		int row = 1;
		info.Bottom().Row(row).Floatable(false)
			.DockFixed(true).CaptionVisible(false)
			.CloseButton(false);
		if (!AuiManager->InsertPane(window, info, wxAUI_INSERT_ROW)) {
			window->Destroy();
			window = NULL;
		}
	}
	if (window) {
		ReplacePanelClass* panel = reinterpret_cast<ReplacePanelClass*>(window);
		wxString selectedText = GetSelectedText();
		if (selectedText.empty() && transferExpression) {
			selectedText = t4p::IcuToWx(Feature.Finder.Expression);
		}
		if (!selectedText.empty()) {
			panel->SetExpression(selectedText);
		}

		// row == 1 means that the finder bar goes below any code
		// notebook but above the tools notebook
		int row = 1;
		AuiManager->GetPane(window).Row(row).Show();
		panel->SetFocusOnFindText();
	}
	parent->Thaw();
	AuiManager->Update();
}

void t4p::FinderViewClass::OnEditGoToLine(wxCommandEvent& event) {
	CodeControlClass* codeControl = GetCurrentCodeControl();
	if (codeControl) {
		int maxLines = codeControl->GetLineCount();
		int goToLine = wxGetNumberFromUser(_("Enter A Line Number"), _(""), _("Go To Line Number"), 0, 1, maxLines, codeControl);
		if (goToLine >= 1) {
			codeControl->GotoLineAndEnsureVisible(goToLine);
		}
	}
}

void t4p::FinderViewClass::OnDoubleClick(wxStyledTextEvent& event) {
	t4p::CodeControlClass* ctrl = GetCurrentCodeControl();
	if (!ctrl) {
		return;
	}
	UnicodeString word = ctrl->WordAtCurrentPos();
	if (!word.isEmpty()) {
		int documentLength = ctrl->GetTextLength();

		// the action will delete it
		char* buf = new char[documentLength];

		// GET_TEXT  message
		ctrl->SendMsg(2182, documentLength, (long)buf);
		Feature.StartFinderAction(word, buf, documentLength);
	}
}

void t4p::FinderViewClass::OnFinderHit(t4p::FinderHitEventClass& event) {
	t4p::CodeControlClass* ctrl = GetCurrentCodeControl();
	if (ctrl) {
		ctrl->HighlightWord(event.Start, event.Length);
	}
}

t4p::FinderPanelClass::FinderPanelClass(wxWindow* parent, int windowId,
											  t4p::FinderClass& finder,
											  t4p::FinderViewClass& view, wxAuiManager* auiManager)
		: FinderPanelGeneratedClass(parent, windowId)
		, Finder(finder)
		, ComboBoxHistory(FindText)
		, View(view)
		, AuiManager(auiManager)
		, CurrentInsertionPointFind(0) {
	t4p::RegularExpressionValidatorClass regExValidator(&Finder.Expression, FinderMode);
	FindText->SetValidator(regExValidator);
	wxGenericValidator modeValidator(&Finder.Mode);
	FinderMode->SetValidator(modeValidator);
	wxGenericValidator wrapValidator(&Finder.Wrap);
	Wrap->SetValidator(wrapValidator);
	RESULT_MESSAGE = _("Status: Found text at line %d");
	NextButton->SetBitmapLabel(wxArtProvider::GetBitmap(wxART_GO_DOWN));
	PreviousButton->SetBitmapLabel(wxArtProvider::GetBitmap(wxART_GO_UP));
	CloseButton->SetBitmapLabel(wxArtProvider::GetBitmap(wxART_ERROR));
	HelpButtonIcon(HelpButton);
	TransferDataToWindow();
}

void t4p::FinderPanelClass::SetFocusOnFindText() {
	FindText->SetFocus();
}

void t4p::FinderPanelClass::SetExpression(const wxString& expression) {
	FindText->SetValue(expression);

	// set the expression on the underlying finder  object
	TransferDataFromWindow();
}

void t4p::FinderPanelClass::FindNext() {
	if (Validate() && TransferDataFromWindow()) {
		if (Finder.Prepare()) {
			Find();

			wxConfigBase* config = wxConfigBase::Get();
			config->Write(wxT("/Finder/Wrap"), Finder.Wrap);
			config->Write(wxT("/Finder/Mode"), Finder.Mode);
		} else {
			SetStatus(_("Status: Invalid Expression"));
		}
	}
}

void t4p::FinderPanelClass::FindPrevious() {
	if (Validate() && TransferDataFromWindow()) {
		if (Finder.Prepare()) {
			Find(false);

			wxConfigBase* config = wxConfigBase::Get();
			config->Write(wxT("/Finder/Wrap"), Finder.Wrap);
			config->Write(wxT("/Finder/Mode"), Finder.Mode);
		} else {
			SetStatus(_("Status: Invalid Expression"));
		}
	}
}

void t4p::FinderPanelClass::Find(bool findNext) {
	// only search when notebook has a current tab
	CodeControlClass* codeControl = View.GetCurrentCodeControl();
	if (codeControl) {
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
				int lineNumber = codeControl->LineFromPosition(position);

				// lineNumber seems to start at zero
				++lineNumber;
				codeControl->MarkSearchHitAndGoto(lineNumber, position, position + length, true);
				SetStatus(wxString::Format(RESULT_MESSAGE, lineNumber));
			}
		} else {
			SetStatus(_("Status: Not Found"));
		}
	}
}

void t4p::FinderPanelClass::OnNextButton(wxCommandEvent& event) {
	FindNext();
	ComboBoxHistory.Save();
}

void t4p::FinderPanelClass::OnHelpButton(wxCommandEvent& event) {
	FindHelpDialogGeneratedClass dialog(this);
	dialog.ShowModal();
}

void t4p::FinderPanelClass::OnOkButton(wxCommandEvent& event) {
	Show(false);
}

void t4p::FinderPanelClass::OnPreviousButton(wxCommandEvent& event) {
	FindPrevious();
	ComboBoxHistory.Save();
}

void t4p::FinderPanelClass::SetStatus(const wxString& message) {
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

void t4p::FinderPanelClass::OnCloseButton(wxCommandEvent& event) {
	AuiManager->GetPane(this).Hide();
	AuiManager->Update();

	// give focus back to the code control
	// better user experience
	t4p::CodeControlClass* codeControl = View.GetCurrentCodeControl();
	if (codeControl) {
		codeControl->SetFocus();
	}
}

void t4p::FinderPanelClass::OnFindEnter(wxCommandEvent& event) {
	OnNextButton(event);
}

void t4p::FinderPanelClass::OnRegExFindHelpButton(wxCommandEvent& event) {
	wxMenu regExMenu;
	t4p::PopulateRegExFindMenu(regExMenu, ID_REGEX_MENU_START);
	PopupMenu(&regExMenu);
}

void t4p::FinderPanelClass::InsertRegExSymbol(wxCommandEvent& event) {
	int id = event.GetId() - ID_REGEX_MENU_START;
	t4p::AddSymbolToRegularExpression(FindText, id, CurrentInsertionPointFind);
	FinderMode->SetSelection(FinderClass::REGULAR_EXPRESSION);
}

void t4p::FinderPanelClass::OnFindKillFocus(wxFocusEvent& event) {
	// connect to the KILL_FOCUS events so that we can capture the insertion point
	// on Win32 GetInsertionPoint() returns 0 when the combo box is no
	// in focus; we must receive the position via an outside mechanism
	CurrentInsertionPointFind = FindText->GetInsertionPoint();
	event.Skip();
}

void t4p::FinderPanelClass::OnFindKeyDown(wxKeyEvent& event) {
	if (event.GetKeyCode() == WXK_ESCAPE) {
		AuiManager->GetPane(this).Hide();
		AuiManager->Update();

		// give focus back to the code control
		// better user experience
		t4p::CodeControlClass* codeControl = View.GetCurrentCodeControl();
		if (codeControl) {
			codeControl->SetFocus();
		}
	} else {
		event.Skip();
	}
}

t4p::ReplacePanelClass::ReplacePanelClass(wxWindow* parent, int windowId, t4p::FinderClass& finder,
												t4p::FinderViewClass& view, wxAuiManager* auiManager)
		: ReplacePanelGeneratedClass(parent, windowId)
		, Finder(finder)
		, FindHistory(FindText)
		, ReplaceHistory(ReplaceWithText)
		, View(view)
		, AuiManager(auiManager)
		, CurrentInsertionPointFind(0)
		, CurrentInsertionPointReplace(0) {
	t4p::RegularExpressionValidatorClass regExValidator(&Finder.Expression, FinderMode);
	FindText->SetValidator(regExValidator);
	wxGenericValidator modeValidator(&Finder.Mode);
	FinderMode->SetValidator(modeValidator);
	wxGenericValidator wrapValidator(&Finder.Wrap);
	Wrap->SetValidator(wrapValidator);
	RESULT_MESSAGE = _("Status: Found text at line %d");
	NextButton->SetBitmapLabel(wxArtProvider::GetBitmap(wxART_GO_DOWN));
	PreviousButton->SetBitmapLabel(wxArtProvider::GetBitmap(wxART_GO_UP));
	CloseButton->SetBitmapLabel(wxArtProvider::GetBitmap(wxART_ERROR));
	HelpButtonIcon(HelpButton);

	UnicodeStringValidatorClass replaceValidator(&Finder.ReplaceExpression, true);
	ReplaceWithText->SetValidator(replaceValidator);
	ReplaceButton->SetBitmapLabel(wxArtProvider::GetBitmap(wxART_FIND_AND_REPLACE));
	ReplaceAllButton->SetBitmapLabel(wxArtProvider::GetBitmap(wxART_FIND_AND_REPLACE));
	UndoButton->SetBitmapLabel(wxArtProvider::GetBitmap(wxART_UNDO));
	ReplaceWithText->MoveAfterInTabOrder(FindText);
	RegExReplaceHelpButton->MoveAfterInTabOrder(ReplaceWithText);
	TransferDataToWindow();
}

void t4p::ReplacePanelClass::SetFocusOnFindText() {
	FindText->SetFocus();
}

void t4p::ReplacePanelClass::SetExpression(const wxString& expression) {
	FindText->SetValue(expression);

	// set the expression on the underlying finder  object
	TransferDataFromWindow();
}

void t4p::ReplacePanelClass::FindNext() {
	if (Validate() && TransferDataFromWindow()) {
		if (Finder.Prepare()) {
			Find();

			wxConfigBase* config = wxConfigBase::Get();
			config->Write(wxT("/Finder/ReplaceWrap"), Finder.Wrap);
			config->Write(wxT("/Finder/ReplaceMode"), Finder.Mode);
		} else {
			SetStatus(_("Status: Invalid Expression"));
		}
	}
}

void t4p::ReplacePanelClass::FindPrevious() {
	if (Validate() && TransferDataFromWindow()) {
		if (Finder.Prepare()) {
			Find(false);

			wxConfigBase* config = wxConfigBase::Get();
			config->Write(wxT("/Finder/ReplaceWrap"), Finder.Wrap);
			config->Write(wxT("/Finder/ReplaceMode"), Finder.Mode);
		} else {
			SetStatus(_("Status: Invalid Expression"));
		}
	}
}

void t4p::ReplacePanelClass::Find(bool findNext) {
	// only search when notebook has a current tab
	CodeControlClass* codeControl = View.GetCurrentCodeControl();
	if (codeControl) {
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
				int lineNumber = codeControl->LineFromPosition(position);

				// lineNumber seems to start at zero
				++lineNumber;
				codeControl->MarkSearchHitAndGoto(lineNumber, position, position + length, true);
				SetStatus(wxString::Format(RESULT_MESSAGE, lineNumber));
			}
		} else {
			SetStatus(_("Status: Not Found"));
		}
		EnableReplaceButtons(found);
	}
}

void t4p::ReplacePanelClass::OnNextButton(wxCommandEvent& event) {
	FindNext();
	FindHistory.Save();
}

void t4p::ReplacePanelClass::OnHelpButton(wxCommandEvent& event) {
	FindHelpDialogGeneratedClass dialog(this);
	dialog.ShowModal();
}

void t4p::ReplacePanelClass::OnOkButton(wxCommandEvent& event) {
	Show(false);
}

void t4p::ReplacePanelClass::OnPreviousButton(wxCommandEvent& event) {
	FindPrevious();
	FindHistory.Save();
}

void t4p::ReplacePanelClass::SetStatus(const wxString& message) {
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

void t4p::ReplacePanelClass::OnCloseButton(wxCommandEvent& event) {
	AuiManager->GetPane(this).Hide();
	AuiManager->Update();

	// give focus back to the code control
	// better user experience
	t4p::CodeControlClass* codeControl = View.GetCurrentCodeControl();
	if (codeControl) {
		codeControl->SetFocus();
	}
}

void t4p::ReplacePanelClass::OnReplaceButton(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow()) {
		t4p::CodeControlClass* codeControl = View.GetCurrentCodeControl();
		int32_t position = 0,
			length = 0;
		UnicodeString replaceText;

		// if user changed tabs, GetLastReplacementText will return false
		UnicodeString text = codeControl->GetSafeText();
		if (codeControl && Finder.GetLastMatch(position, length) &&
			Finder.GetLastReplacementText(text, replaceText)) {
			codeControl->SetSelectionByCharacterPosition(position, position + length, false);
			codeControl->ReplaceSelection(t4p::IcuToWx(replaceText));
			codeControl->SetSelectionByCharacterPosition(position, position + replaceText.length(), false);
			Find(true);
			ReplaceHistory.Save();
		} else {
			SetStatus(_("Status: Not Found"));
		}
	}
}

void t4p::ReplacePanelClass::OnReplaceAllButton(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow() && Finder.Prepare()) {
		// if user changed tabs, GetLastReplacementText will return false
		CodeControlClass* codeControl = View.GetCurrentCodeControl();
		 if (codeControl) {
			 UnicodeString text = codeControl->GetSafeText();
			 int matches = Finder.ReplaceAllMatches(text);
			 codeControl->SetUnicodeText(text);
			 SetStatus(wxString::Format(wxT("Status: Replaced %d matches"), matches));
			 ReplaceHistory.Save();
		} else {
			SetStatus(_("Status: Not Found"));
		}
	}
}

void t4p::ReplacePanelClass::OnUndoButton(wxCommandEvent& event) {
	t4p::CodeControlClass* codeControl = View.GetCurrentCodeControl();

	// if user changes to new tab, undo will undo changes to that file
	 if (codeControl) {
		codeControl->Undo();
		int32_t position,
			length;
		if (Finder.GetLastMatch(position, length)) {
			Finder.FindPrevious(codeControl->GetSafeText(), position);
			if (Finder.GetLastMatch(position, length)) {
				codeControl->SetSelectionByCharacterPosition(position, position + length, false);
			}
		}
	}
}

void t4p::ReplacePanelClass::OnFindEnter(wxCommandEvent& event) {
	OnNextButton(event);
}

void t4p::ReplacePanelClass::OnReplaceEnter(wxCommandEvent& event) {
	OnReplaceButton(event);
}

void t4p::ReplacePanelClass::OnFindKeyDown(wxKeyEvent& event) {
	// since this panel handles EVT_TEXT_ENTER, we need to handle the
	// tab traversal ourselves otherwise tab travesal wont work
	if (event.GetKeyCode() == WXK_TAB && event.ShiftDown()) {
		FindText->Navigate(wxNavigationKeyEvent::IsBackward);
	} else if (event.GetKeyCode() == WXK_TAB) {
		FindText->Navigate(wxNavigationKeyEvent::IsForward);
	} else if (event.GetKeyCode() == WXK_ESCAPE) {
		AuiManager->GetPane(this).Hide();
		AuiManager->Update();

		// give focus back to the code control
		// better user experience
		t4p::CodeControlClass* codeControl = View.GetCurrentCodeControl();
		if (codeControl) {
			codeControl->SetFocus();
		}
	} else {
		event.Skip();
	}
}

void t4p::ReplacePanelClass::OnReplaceKeyDown(wxKeyEvent& event) {
	// since this panel handles EVT_TEXT_ENTER, we need to handle the
	// tab traversal ourselves otherwise tab travesal wont work
	if (event.GetKeyCode() == WXK_TAB && event.ShiftDown()) {
		ReplaceWithText->Navigate(wxNavigationKeyEvent::IsBackward);
		event.Skip();
	} else if (event.GetKeyCode() == WXK_TAB) {
		ReplaceWithText->Navigate(wxNavigationKeyEvent::IsForward);
		event.Skip();
	} else if (event.GetKeyCode() == WXK_ESCAPE) {
		AuiManager->GetPane(this).Hide();
		AuiManager->Update();

		// give focus back to the code control
		// better user experience
		t4p::CodeControlClass* codeControl = View.GetCurrentCodeControl();
		if (codeControl) {
			codeControl->SetFocus();
		}
	} else {
		event.Skip();
	}
}

void t4p::ReplacePanelClass::OnRegExFindHelpButton(wxCommandEvent& event) {
	wxMenu regExMenu;
	t4p::PopulateRegExFindMenu(regExMenu, ID_REGEX_REPLACE_FIND_MENU_START);
	PopupMenu(&regExMenu);
}

void t4p::ReplacePanelClass::OnReplaceRegExFindHelpButton(wxCommandEvent& event) {
	wxMenu regExMenu;
	t4p::PopulateRegExReplaceMenu(regExMenu, ID_REGEX_REPLACE_MENU_START);
	PopupMenu(&regExMenu);
}

void t4p::ReplacePanelClass::InsertReplaceRegExSymbol(wxCommandEvent& event) {
	wxString symbols;
	int id = event.GetId() - ID_REGEX_REPLACE_MENU_START;
	t4p::AddSymbolToReplaceRegularExpression(ReplaceWithText, id, CurrentInsertionPointReplace);
	FinderMode->SetSelection(FinderClass::REGULAR_EXPRESSION);
}

void t4p::ReplacePanelClass::InsertRegExSymbol(wxCommandEvent& event) {
	int id = event.GetId() -  ID_REGEX_REPLACE_FIND_MENU_START;
	t4p::AddSymbolToRegularExpression(FindText, id, CurrentInsertionPointFind);
	FinderMode->SetSelection(FinderClass::REGULAR_EXPRESSION);
}

void t4p::ReplacePanelClass::EnableReplaceButtons(bool enable) {
	ReplaceButton->Enable(enable);
	UndoButton->Enable(enable);
}

void t4p::ReplacePanelClass::OnFindKillFocus(wxFocusEvent& event) {
	// connect to the KILL_FOCUS events so that we can capture the insertion point
	// on Win32 GetInsertionPoint() returns 0 when the combo box is no
	// in focus; we must receive the position via an outside mechanism
	CurrentInsertionPointFind = FindText->GetInsertionPoint();
	event.Skip();
}

void t4p::ReplacePanelClass::OnReplaceKillFocus(wxFocusEvent& event) {
	// connect to the KILL_FOCUS events so that we can capture the insertion point
	// on Win32 GetInsertionPoint() returns 0 when the combo box is no
	// in focus; we must receive the position via an outside mechanism
	CurrentInsertionPointReplace = ReplaceWithText->GetInsertionPoint();
	event.Skip();
}

BEGIN_EVENT_TABLE(t4p::FinderPanelClass, FinderPanelGeneratedClass)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_ZERO, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_ONE, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_ZERO_OR_ONE, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_EXACT, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_AT_LEAST, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_SEQUENCE_BETWEEN, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_BEGIN_OF_LINE, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_END_OF_LINE, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_DIGIT, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_WHITE_SPACE, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_ALPHANUMERIC, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_NOT_DECIMAL, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_NOT_WHITE_SPACE, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_NOT_ALPHANUMERIC, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_CASE_SENSITIVE, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_COMMENT, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_DOT_ALL, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_MULTI_LINE, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_UWORD, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_PHP_STRING, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_PHP_VARIABLE, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_PHP_NUMBER, t4p::FinderPanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_MENU_START + ID_MENU_REG_EX_PHP_WHITESPACE, t4p::FinderPanelClass::InsertRegExSymbol)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::ReplacePanelClass, ReplacePanelGeneratedClass)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_ONE, t4p::ReplacePanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_TWO, t4p::ReplacePanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_THREE, t4p::ReplacePanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_FOUR, t4p::ReplacePanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_REPLACE_MATCH_FIVE, t4p::ReplacePanelClass::InsertReplaceRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_MENU_START + ID_MENU_REG_EX_SEQUENCE_ONE, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_ZERO_OR_ONE, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_SEQUENCE_EXACT, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_SEQUENCE_AT_LEAST, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_SEQUENCE_BETWEEN, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_BEGIN_OF_LINE, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_END_OF_LINE, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_DIGIT, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_WHITE_SPACE, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_ALPHANUMERIC, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_NOT_DECIMAL, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_NOT_WHITE_SPACE, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_NOT_ALPHANUMERIC, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_CASE_SENSITIVE, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_COMMENT, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_DOT_ALL, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_MULTI_LINE, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_UWORD, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_PHP_STRING, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_PHP_VARIABLE, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_PHP_NUMBER, t4p::ReplacePanelClass::InsertRegExSymbol)
	EVT_MENU(ID_REGEX_REPLACE_FIND_MENU_START + ID_MENU_REG_EX_PHP_WHITESPACE, t4p::ReplacePanelClass::InsertRegExSymbol)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::FinderViewClass, t4p::FeatureViewClass)
	EVT_MENU(t4p::MENU_FINDER + 0, t4p::FinderViewClass::OnEditFind)
	EVT_MENU(t4p::MENU_FINDER + 1, t4p::FinderViewClass::OnEditFindNext)
	EVT_MENU(t4p::MENU_FINDER + 2, t4p::FinderViewClass::OnEditFindPrevious)
	EVT_MENU(t4p::MENU_FINDER + 3, t4p::FinderViewClass::OnEditReplace)
	EVT_MENU(t4p::MENU_FINDER + 4, t4p::FinderViewClass::OnEditGoToLine)
	EVT_STC_DOUBLECLICK(wxID_ANY, t4p::FinderViewClass::OnDoubleClick)
	EVT_FINDER(t4p::ID_FINDER_ACTION, t4p::FinderViewClass::OnFinderHit)
END_EVENT_TABLE()

