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
#include <views/EditorBehaviorViewClass.h>
#include <features/EditorBehaviorFeatureClass.h>
#include <globals/Assets.h>
#include <code_control/CodeControlClass.h>
#include <widgets/NotebookClass.h>
#include <main_frame/PreferencesClass.h>
#include <widgets/AuiManager.h>
#include <Triumph.h>
#include <wx/valgen.h>
#include <wx/stc/stc.h>

static int ID_SUBMENU_TRANSFORM = wxNewId();
static int ID_SUBMENU_SELECTION = wxNewId();
static int ID_SUBMENU_CARET = wxNewId();

static void AssignKeyCommands(wxStyledTextCtrl* ctrl, const std::vector<t4p::EditorKeyboardCommandClass>& keyboardCommands) {
	for (size_t i = 0; i < keyboardCommands.size(); ++i) {
		t4p::EditorKeyboardCommandClass cmd = keyboardCommands[i];
		if (cmd.CmdId > 0 && cmd.IsOk()) {
			ctrl->CmdKeyAssign(cmd.KeyCode, cmd.Modifiers, cmd.CmdId);
		}
	}
}

t4p::EditorBehaviorViewClass::EditorBehaviorViewClass(t4p::EditorBehaviorFeatureClass& feature)
: FeatureViewClass()
, Feature(feature) {

}

void t4p::EditorBehaviorViewClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	toolBar->AddSeparator();
	toolBar->AddTool(t4p::MENU_BEHAVIOR + 1, _("Word Wrap"),
		t4p::BitmapImageAsset(wxT("wrap")), _("Enable / disable word wrap"), wxITEM_NORMAL
	);
	toolBar->AddTool(t4p::MENU_BEHAVIOR + 2, _("Indentation"),
		t4p::BitmapImageAsset(wxT("indentation")), _("Enable / disable identation guides"), wxITEM_NORMAL
	);
	toolBar->AddTool(t4p::MENU_BEHAVIOR + 3, _("Whitespace"),
		t4p::BitmapImageAsset(wxT("whitespace")), _("Enable / disable showing whitespace"), wxITEM_NORMAL
	);
}

void t4p::EditorBehaviorViewClass::AddEditMenuItems(wxMenu* editMenu) {
	editMenu->Append(wxID_UNDO, _("Undo"), _("Reverts the most recent change"), wxITEM_NORMAL);
	editMenu->Append(wxID_REDO, _("Redo"), _("Un-reverts the most recent undo"), wxITEM_NORMAL);

	editMenu->Append(wxID_CUT, _("Cut"), _("Cut the selected text and place in the clipboard"), wxITEM_NORMAL);
	editMenu->Append(wxID_COPY, _("Copy"), _("Copy the selected text to the clipboard"), wxITEM_NORMAL);
	editMenu->Append(wxID_PASTE, _("Paste"), _("Paste the current contents of the clipboard"), wxITEM_NORMAL);
	editMenu->Append(wxID_SELECTALL, _("Select All"), _("Select the entire contents of the control"), wxITEM_NORMAL);
	editMenu->AppendSeparator();
	editMenu->Append(t4p::MENU_BEHAVIOR + 10, _("Complete Symbol"), _("Show an autocompletion box of possible matches of the current symbol"), wxITEM_NORMAL);
	editMenu->Append(t4p::MENU_BEHAVIOR + 11, _("Show Call Tip"), _("Display the function signature of the symbol at the current caret position"), wxITEM_NORMAL);

	wxMenu* subMenu = new wxMenu();
	subMenu->Append(t4p::MENU_BEHAVIOR + 7, _("Unix (LF)"), _("Convert Line ending to UNIX"));
	subMenu->Append(t4p::MENU_BEHAVIOR + 8, _("Windows (CRLF)"), _("Convert Line ending to Windows"));
	subMenu->Append(t4p::MENU_BEHAVIOR + 9, _("Max (CR)"), _("Convert Line ending to MAC"));

	editMenu->Append(wxID_ANY, _("Convert Line Endings To"), subMenu);
}

void t4p::EditorBehaviorViewClass::AddViewMenuItems(wxMenu* menu) {
	menu->Append(t4p::MENU_BEHAVIOR + 4, _("Zoom In"), _("Zoom In"));
	menu->Append(t4p::MENU_BEHAVIOR + 5, _("Zoom Out"), _("Zoom Out"));
	menu->Append(t4p::MENU_BEHAVIOR + 6, _("Reset Zoom\tCTRL+0"), _("Reset Zoom"));
}

void t4p::EditorBehaviorViewClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> cmds;
	cmds[wxID_UNDO] = wxT("Edit-Undo");
	cmds[wxID_REDO] = wxT("Edit-Redo");
	cmds[wxID_CUT] = wxT("Edit-Cut");
	cmds[wxID_COPY] = wxT("Edit-Copy");
	cmds[wxID_PASTE] = wxT("Edit-Paste");
	cmds[wxID_SELECTALL] = wxT("Edit-Select All");
	cmds[t4p::MENU_BEHAVIOR + 10] = wxT("Edit-Complete Symbol");
	cmds[t4p::MENU_BEHAVIOR + 11] = wxT("Edit-Call Tip");

	AddDynamicCmd(cmds, shortcuts);
}

void t4p::EditorBehaviorViewClass::AddCodeControlClassContextMenuItems(wxMenu* menu) {

	// no need to delete moreMenu pointer, the contextMenu will delete it for us
	t4p::CodeControlClass* codeCtrl = GetCurrentCodeControl();
	bool isTextSelected = false;
	bool canPaste = false;
	bool canUndo = false;
	bool canRedo = false;
	if (codeCtrl) {
		isTextSelected = !codeCtrl->GetSelectedText().IsEmpty();
		canPaste = codeCtrl->CanPaste();
		canUndo = codeCtrl->CanUndo();
		canRedo = codeCtrl->CanRedo();
	}

	menu->Append(wxID_CUT, _("Cut"));
	menu->Append(wxID_COPY, _("Copy"));
	menu->Append(wxID_PASTE, _("Paste"));
	menu->Append(wxID_UNDO, _("Undo"));
	menu->Append(wxID_REDO, _("Redo"));
	menu->Append(wxID_SELECTALL, _("Select All"));
	menu->AppendSeparator();
	menu->Append(wxID_FIND, _("Find"));
	menu->AppendSeparator();

	menu->Enable(wxID_CUT, isTextSelected);
	menu->Enable(wxID_COPY, isTextSelected);
	menu->Enable(wxID_PASTE, canPaste);
	menu->Enable(wxID_UNDO, canUndo);
	menu->Enable(wxID_REDO, canRedo);

	AddKeyboardCommands(menu, isTextSelected);

	// update the menu accelerators to be the ones configured by the
	// user
	for (size_t i = 0; i < Feature.KeyboardCommands.size(); ++i) {
		int cmdId = Feature.KeyboardCommands[i].CmdId;
		int menuId = Feature.SciCommandToMenuId(cmdId);
		wxMenuItem* menuItem = menu->FindItem(menuId);
		if (menuItem) {
			wxString label =
				menuItem->GetItemLabelText()
				+ wxT("\t")
				+ Feature.KeyboardCommands[i].ToString();
			menuItem->SetItemLabel(label);
		}
	}
}

void t4p::EditorBehaviorViewClass::OnToggleWordWrap(wxCommandEvent& event) {
	Feature.ToggleWordWrap();
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorViewClass::OnToggleIndentationGuides(wxCommandEvent& event) {
	Feature.ToggleIndentationGuides();
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorViewClass::OnToggleWhitespace(wxCommandEvent& event) {
	Feature.ToggleWhitespace();
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorViewClass::OnZoomIn(wxCommandEvent& event) {
	Feature.ZoomIn();
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorViewClass::OnZoomOut(wxCommandEvent& event) {
	Feature.ZoomOut();
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorViewClass::OnZoomReset(wxCommandEvent& event) {
	Feature.ZoomReset();
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorViewClass::OnEditConvertEols(wxCommandEvent& event) {
	t4p::CodeControlClass* codeCtrl = GetCurrentCodeControl();
	if (!codeCtrl) {
		return;
	}
	if (event.GetId() == (t4p::MENU_BEHAVIOR + 7)) {
		codeCtrl->ConvertEOLs(wxSTC_EOL_LF);
	}
	else if (event.GetId() == (t4p::MENU_BEHAVIOR + 8)) {
		codeCtrl->ConvertEOLs(wxSTC_EOL_CRLF);
	}
	else if (event.GetId() == (t4p::MENU_BEHAVIOR + 9)) {
		codeCtrl->ConvertEOLs(wxSTC_EOL_CR);
	}
}

void t4p::EditorBehaviorViewClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
	wxKeyBinder* keyBinder = Feature.App.Preferences.KeyProfiles.GetSelProfile();

	t4p::EditorBehaviorPanelClass* panel = new t4p::EditorBehaviorPanelClass(parent,
		Feature.App.Preferences.CodeControlOptions);
	t4p::EditorCommandPanelClass* cmdPanel = new t4p::EditorCommandPanelClass(parent,
		wxID_ANY, Feature.KeyboardCommands, keyBinder);
	parent->AddPage(panel, _("Editor Behavior"));
	parent->AddPage(cmdPanel, _("Editor Shortcuts"));
}

void t4p::EditorBehaviorViewClass::AddKeyboardCommands(wxMenu* menu, bool isTextSelected) {

	/*
	 the transform menu exposes the following scintilla commands

	SCI_LINECUT
	SCI_LINEDUPLICATE
	SCI_LINEDELETE
	SCI_LINETRANSPOSE
	SCI_LINECOPY
	SCI_LOWERCASE
	SCI_UPPERCASE
	SCI_DELWORDLEFT
	SCI_DELWORDRIGHT
	SCI_DELLINELEFT
	SCI_DELLINERIGHT
	SCI_SELECTIONDUPLICATE
	SCI_MOVESELECTEDLINESUP
	SCI_MOVESELECTEDLINESDOWN
	*/
	// not sure what these message do
	// SCI_DELETEBACKNOTLINE
	// SCI_DELWORDRIGHTEND
	// 2469 == there is no wxSTC_CMD_SELECTIONDUPLICATE
	wxMenu* transformMenu = new wxMenu();
	transformMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_LINECUT), _("Cut Line"));
	transformMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_LINEDUPLICATE), _("Duplicate Line"));
	transformMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_LINEDELETE), _("Delete Line"));
	transformMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_LINETRANSPOSE), _("Transpose Line"));
	transformMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_LINECOPY), _("Copy Line"));
	transformMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_LOWERCASE), _("Convert To Lower Case"));
	transformMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_UPPERCASE), _("Convert To Upper Case"));
	transformMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_DELWORDLEFT), _("Delete to start of Word"));
	transformMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_DELWORDRIGHT), _("Delete to end of Word"));
	transformMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_DELLINELEFT), _("Delete Line up to cursor"));
	transformMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_DELLINERIGHT), _("Delete Line after cursor"));
	transformMenu->Append(Feature.SciCommandToMenuId(2469), _("Duplicate selection"));
	transformMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_MOVESELECTEDLINESUP), _("Move selected lines up"));
	transformMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_MOVESELECTEDLINESDOWN), _("Move selected lines down"));

	/*
	the selection menu exposes the following scintilla commands
	SCI_PARADOWNEXTEND
	SCI_PARAUPEXTEND
	SCI_WORDLEFTEXTEND
	SCI_WORDRIGHTEXTEND
	SCI_WORDLEFTENDEXTEND
	SCI_WORDRIGHTENDEXTEND
	SCI_WORDPARTLEFTEXTEND
	SCI_WORDPARTRIGHTEXTEND
	*/
	wxMenu* selectionMenu = new wxMenu();


	/*
	the caret menu exposes the following scintilla commands
	SCI_PARADOWN
	SCI_PARAUP
	SCI_WORDLEFT
	SCI_WORDRIGHT
	SCI_WORDPARTLEFT
	SCI_WORDPARTRIGHT
	*/
	selectionMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_PARADOWNEXTEND), _("Expand selection to the end of paragraph")); //\tCTRL+SHIFT+]
	selectionMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_PARAUPEXTEND), _("Expand selection to the beginning of paragraph")); // \tCTRL+SHIFT+[
	selectionMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_WORDLEFTEXTEND), _("Expand selection to the beginning of previous word\tCTRL+SHIFT+LEFT"));
	selectionMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_WORDRIGHTEXTEND), _("Expand selection to the beginning of next word\tCTRL+SHIFT+RIGHT"));
	selectionMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_WORDLEFTENDEXTEND), _("Expand selection to the end of previous word"));
	selectionMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_WORDRIGHTENDEXTEND), _("Expand selection to the end of next word"));
	selectionMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_WORDPARTLEFTEXTEND), _("Expand selection to the previous word segment")); // \tCTRL+SHIFT+/
	selectionMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_WORDPARTRIGHTEXTEND), _("Expand selection to the end of next word segment")); // \tCTRL+SHIFT+backslash

	//?? not sure what these do
	//SCI_WORDLEFTEND
	//SCI_WORDRIGHTEND
	wxMenu* caretMenu = new wxMenu();
	caretMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_PARADOWN), _("Move to next paragraph")); // \tCTRL+]
	caretMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_PARAUP), _("Move to previous paragraph")); // \tCTRL+[
	caretMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_WORDLEFT), _("Move to previous word\tCTRL+LEFT"));
	caretMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_WORDRIGHT), _("Move to next word\tCTRL+RIGHT"));
	caretMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_WORDPARTLEFT), _("Move to previous word segment")); // \tCTRL+/
	caretMenu->Append(Feature.SciCommandToMenuId(wxSTC_CMD_WORDPARTRIGHT), _("Move to next word segment")); // \tCTRL+backslash

	menu->Append(ID_SUBMENU_TRANSFORM, _("Transform text"), transformMenu);
	menu->Append(ID_SUBMENU_SELECTION, _("Selection"), selectionMenu);
	menu->Append(ID_SUBMENU_CARET, _("Move caret"), caretMenu);

	transformMenu->Enable(Feature.SciCommandToMenuId(wxSTC_CMD_LOWERCASE), isTextSelected);
	transformMenu->Enable(Feature.SciCommandToMenuId(wxSTC_CMD_UPPERCASE), isTextSelected);
}


void t4p::EditorBehaviorViewClass::SetFeaturesOnNotebook() {
	t4p::CodeControlOptionsClass options = Feature.App.Preferences.CodeControlOptions;
	std::vector<t4p::CodeControlClass*> ctrls = AllCodeControls();
	for (size_t i = 0; i < ctrls.size(); ++i) {
		SetFeatures(options, ctrls[i]);
	}
}

void t4p::EditorBehaviorViewClass::SetFeatures(const t4p::CodeControlOptionsClass& options, wxStyledTextCtrl* codeCtrl) {
	if (options.IndentUsingTabs) {
		codeCtrl->SetUseTabs(true);
		codeCtrl->SetTabWidth(options.TabWidth);
		codeCtrl->SetIndent(0);
		codeCtrl->SetTabIndents(true);
		codeCtrl->SetBackSpaceUnIndents(true);
	}
	else {
		codeCtrl->SetUseTabs(false);
		codeCtrl->SetTabWidth(options.SpacesPerIndent);
		codeCtrl->SetIndent(options.SpacesPerIndent);
		codeCtrl->SetTabIndents(false);
		codeCtrl->SetBackSpaceUnIndents(false);
	}
	if (options.RightMargin > 0) {
		codeCtrl->SetEdgeMode(wxSTC_EDGE_LINE);
		codeCtrl->SetEdgeColumn(options.RightMargin);
	}
	else {
		codeCtrl->SetEdgeMode(wxSTC_EDGE_NONE);
	}
	codeCtrl->SetIndentationGuides(options.EnableIndentationGuides);
	codeCtrl->SetEOLMode(options.LineEndingMode);
	codeCtrl->SetViewEOL(options.EnableLineEndings);

	if (options.EnableWordWrap) {
		codeCtrl->SetWrapMode(wxSTC_WRAP_WORD);
		codeCtrl->SetWrapVisualFlags(wxSTC_WRAPVISUALFLAG_START);
	}
	else {
		codeCtrl->SetWrapMode(wxSTC_WRAP_NONE);
	}

	codeCtrl->SetMultipleSelection(options.EnableMultipleSelection);
	codeCtrl->SetAdditionalSelectionTyping(options.EnableMultipleSelection);
	if (options.EnableMultipleSelection) {
		codeCtrl->SetMultiPaste(wxSTC_MULTIPASTE_EACH);

	}
	else {
		codeCtrl->SetMultiPaste(wxSTC_MULTIPASTE_ONCE);
	}

	int virtualSpaceOpts = wxSTC_SCVS_NONE;
	if (options.EnableVirtualSpace) {
		virtualSpaceOpts |= wxSTC_SCVS_USERACCESSIBLE;
	}
	if (options.EnableRectangularSelection) {
		virtualSpaceOpts |= wxSTC_SCVS_RECTANGULARSELECTION;
	}
	codeCtrl->SetVirtualSpaceOptions(virtualSpaceOpts);

	int selectionMode = wxSTC_SEL_STREAM;
	if (options.EnableRectangularSelection) {
		selectionMode = wxSTC_SEL_RECTANGLE;
	}
	codeCtrl->SetSelectionMode(selectionMode);
	int whitespaceMode = wxSTC_WS_INVISIBLE;
	if (options.ShowWhitespace) {
		whitespaceMode = wxSTC_WS_VISIBLEALWAYS;
	}
	codeCtrl->SetViewWhiteSpace(whitespaceMode);
	codeCtrl->SetZoom(options.Zoom);

	AssignKeyCommands(codeCtrl, Feature.KeyboardCommands);
}

void t4p::EditorBehaviorViewClass::OnAppFileOpened(t4p::CodeControlEventClass& event) {
	SetFeatures(Feature.App.Preferences.CodeControlOptions, event.GetCodeControl());
	MenuUpdate();
}

void t4p::EditorBehaviorViewClass::OnAppFileNew(t4p::CodeControlEventClass& event) {
	SetFeatures(Feature.App.Preferences.CodeControlOptions, event.GetCodeControl());
	MenuUpdate();
}

void t4p::EditorBehaviorViewClass::OnAppFileClosed(t4p::CodeControlEventClass& event) {

	// since this gets called when the code control is ABOUT to be closed
	// the count of code controls is 1 but it wil soon be zero
	bool hasEditors = AllCodeControls().size() > 1;
	wxMenuItem* menuItem = MenuBar->FindItem(t4p::MENU_BEHAVIOR + 10);
	menuItem->Enable(hasEditors);
	menuItem = MenuBar->FindItem(t4p::MENU_BEHAVIOR + 11);
	menuItem->Enable(hasEditors);
}

void t4p::EditorBehaviorViewClass::MenuUpdate() {
	bool hasEditors = !AllCodeControls().empty();
	wxMenuItem* menuItem = MenuBar->FindItem(t4p::MENU_BEHAVIOR + 10);
	menuItem->Enable(hasEditors);
	menuItem = MenuBar->FindItem(t4p::MENU_BEHAVIOR + 11);
	menuItem->Enable(hasEditors);
}

void t4p::EditorBehaviorViewClass::OnEditorCommand(wxCommandEvent& event) {
	t4p::CodeControlClass* ctrl = GetCurrentCodeControl();
	if (!ctrl) {
		return;
	}

	// this method handles all scintilla commands
	// use the menu item ID is scintilla command to execute
	int menuId = event.GetId();
	int cmdId = Feature.MenuIdToSciCommand(menuId);
	ctrl->CmdKeyExecute(cmdId);
}

void t4p::EditorBehaviorViewClass::OnEditContentAssist(wxCommandEvent& event) {
	CodeControlClass* page = GetCurrentCodeControl();
	if (page) {
		page->HandleAutoCompletion();
	}
}

void t4p::EditorBehaviorViewClass::OnEditCallTip(wxCommandEvent& event) {
	CodeControlClass* page = GetCurrentCodeControl();
	if (page) {
		page->HandleCallTip(0, true);
	}
}

void t4p::EditorBehaviorViewClass::OnEditCut(wxCommandEvent& event) {

	// need to handle cut in all text controls
	wxWindow* obj = wxWindow::FindFocus();
	wxTextCtrl* t = wxDynamicCast(obj, wxTextCtrl);
	wxComboBox* combo = wxDynamicCast(obj, wxComboBox);
	wxStyledTextCtrl* stc = wxDynamicCast(obj, wxStyledTextCtrl);
	CodeControlClass* code = GetCurrentCodeControl();
	if (t != NULL) {
		t->Cut();
	}
	else if (combo != NULL) {
		combo->Cut();
	}
	else if (stc != NULL) {
		stc->Cut();
	}
	else if (code != NULL) {
		code->Cut();
	}
	else {
		event.Skip();
	}
}

void t4p::EditorBehaviorViewClass::OnEditCopy(wxCommandEvent& event) {

	// need to handle copy in all text controls
	wxWindow* obj = wxWindow::FindFocus();
	wxTextCtrl* t = wxDynamicCast(obj, wxTextCtrl);
	wxComboBox* combo = wxDynamicCast(obj, wxComboBox);
	wxStyledTextCtrl* stc = wxDynamicCast(obj, wxStyledTextCtrl);
	CodeControlClass* code = GetCurrentCodeControl();
	if (t != NULL) {
		t->Copy();
	}
	else if (combo != NULL) {
		combo->Copy();
	}
	else if (stc != NULL) {
		stc->Copy();
	}
	else if (code != NULL) {
		code->Copy();
	}
	else {
		event.Skip();
	}
}

void t4p::EditorBehaviorViewClass::OnEditPaste(wxCommandEvent& event) {

	// need to handle paste in all text controls
	wxWindow* obj = wxWindow::FindFocus();
	wxTextCtrl* t = wxDynamicCast(obj, wxTextCtrl);
	wxStyledTextCtrl* stc = wxDynamicCast(obj, wxStyledTextCtrl);
	wxComboBox* combo = wxDynamicCast(obj, wxComboBox);
	CodeControlClass* code = GetCurrentCodeControl();
	if (t != NULL) {
		t->Paste();
	}
	else if (combo != NULL) {
		combo->Paste();
	}
	else if (stc != NULL) {
		stc->Paste();
	}
	else if (code != NULL) {
		code->Paste();
	}
	else {
		event.Skip();
	}
}

void t4p::EditorBehaviorViewClass::OnEditSelectAll(wxCommandEvent& event) {

	// need to handle select All in all text controls
	wxWindow* obj = wxWindow::FindFocus();
	wxTextCtrl* t = wxDynamicCast(obj, wxTextCtrl);
	wxStyledTextCtrl* stc = wxDynamicCast(obj, wxStyledTextCtrl);
	wxComboBox* combo = wxDynamicCast(obj, wxComboBox);
	CodeControlClass* code = GetCurrentCodeControl();
	if (t != NULL) {
		t->SelectAll();
	}
	else if (combo != NULL) {
		combo->SelectAll();
	}
	else if (stc != NULL) {
		stc->SelectAll();
	}
	else if (code != NULL) {
		code->SelectAll();
	}
	else {
		event.Skip();
	}
}

void t4p::EditorBehaviorViewClass::OnUndo(wxCommandEvent& event) {
	t4p::CodeControlClass* codeControl =  GetCurrentCodeControl();
	if (codeControl) {
		codeControl->Undo();
	}
}

void t4p::EditorBehaviorViewClass::OnRedo(wxCommandEvent& event) {
	t4p::CodeControlClass* codeControl =  GetCurrentCodeControl();
	if (codeControl) {
		codeControl->Redo();
	}
}

void t4p::EditorBehaviorViewClass::OnPreferencesSaved(wxCommandEvent& event) {
	std::vector<t4p::NotebookClass*> notebooks = t4p::AuiAllCodeNotebooks(*AuiManager);
	for (size_t i = 0; i < notebooks.size(); i++) {
		t4p::NotebookClass* notebook = notebooks[i];
		notebook->RefreshCodeControlOptions();
	}

	SetFeaturesOnNotebook();
}

t4p::EditorBehaviorPanelClass::EditorBehaviorPanelClass(wxWindow* parent, t4p::CodeControlOptionsClass& options)
	: EditorBehaviorPanelGeneratedClass(parent) {
	wxGenericValidator enableCodeFoldingValidator(&options.EnableCodeFolding);
	EnableCodeFolding->SetValidator(enableCodeFoldingValidator);
	wxGenericValidator enableAutomaticIndentationValidator(&options.EnableAutomaticLineIndentation);
	EnableAutomaticLineIndentation->SetValidator(enableAutomaticIndentationValidator);
	wxGenericValidator enableLineNumbersValidator(&options.EnableLineNumbers);
	EnableLineNumbers->SetValidator(enableLineNumbersValidator);
	wxGenericValidator indentUsingTabsValidator(&options.IndentUsingTabs);
	IndentUsingTabs->SetValidator(indentUsingTabsValidator);
	wxGenericValidator tabWidthValidator(&options.TabWidth);
	TabWidth->SetValidator(tabWidthValidator);
	wxGenericValidator spacesPerIndentValidator(&options.SpacesPerIndent);
	SpacesPerIndent->SetValidator(spacesPerIndentValidator);
	TabWidth->Enable(options.IndentUsingTabs);
	SpacesPerIndent->Enable(!options.IndentUsingTabs);
	wxGenericValidator enableIndentationGuidesValidator(&options.EnableIndentationGuides);
	EnableIndentationGuides->SetValidator(enableIndentationGuidesValidator);
	wxGenericValidator rightMarginValidator(&options.RightMargin);
	RightMargin->SetValidator(rightMarginValidator);
	wxGenericValidator enableLineEndingsValidator(&options.EnableLineEndings);
	EnableLineEndings->SetValidator(enableLineEndingsValidator);
	wxGenericValidator lineEndingModeValidator(&options.LineEndingMode);
	LineEndingMode->SetValidator(lineEndingModeValidator);
	wxGenericValidator autoCompletionValidator(&options.EnableAutoCompletion);
	EnableAutoCompletion->SetValidator(autoCompletionValidator);
	wxGenericValidator dynamicAutoCompletionValidator(&options.EnableDynamicAutoCompletion);
	EnableDynamicAutoCompletion->SetValidator(dynamicAutoCompletionValidator);
	wxGenericValidator wordWrapValidator(&options.EnableWordWrap);
	EnableWordWrap->SetValidator(wordWrapValidator);
	wxGenericValidator trimTrailingSpaceValidator(&options.TrimTrailingSpaceBeforeSave);
	TrimTrailingSpaceBeforeSave->SetValidator(trimTrailingSpaceValidator);
	wxGenericValidator removeTrailingBlankLinesValidator(&options.RemoveTrailingBlankLinesBeforeSave);
	RemoveTrailingBlankLines->SetValidator(removeTrailingBlankLinesValidator);
	wxGenericValidator callTipOnMouseHoverValidator(&options.EnableCallTipsOnMouseHover);
	EnableCallTipOnMouseHover->SetValidator(callTipOnMouseHoverValidator);
	wxGenericValidator multipleSelectionValidator(&options.EnableMultipleSelection);
	EnableMultipleSelection->SetValidator(multipleSelectionValidator);
	wxGenericValidator virtualSpaceValidator(&options.EnableVirtualSpace);
	EnableVirtualSpace->SetValidator(virtualSpaceValidator);
	wxGenericValidator rectangularSelectionValidator(&options.EnableRectangularSelection);
	EnableRectangularSelection->SetValidator(rectangularSelectionValidator);
	wxGenericValidator whitespaceValidator(&options.ShowWhitespace);
	ShowWhitespace->SetValidator(whitespaceValidator);
	wxGenericValidator zoomValidator(&options.Zoom);
	Zoom->SetValidator(zoomValidator);
}

void t4p::EditorBehaviorPanelClass::OnIndentUsingSpaces(wxCommandEvent& event) {
	TabWidth->Enable(event.IsChecked());
	SpacesPerIndent->Enable(!event.IsChecked());
}

bool t4p::EditorBehaviorPanelClass::TransferDataToWindow() {
	if (wxWindow::TransferDataToWindow()) {

		// CodeControlOptionsClass disables right margin when RightMargin = 0
		EnableRightMargin->SetValue(RightMargin->GetValue() > 0);
		RightMargin->Enable(RightMargin->GetValue() > 0);
		return true;
	}
	return false;
}

bool t4p::EditorBehaviorPanelClass::TransferDataFromWindow() {

	// CodeControlOptionsClass disables right margin when RightMargin = 0
	if (!EnableRightMargin->IsChecked()) {
		RightMargin->SetValue(0);
	}
	return wxWindow::TransferDataFromWindow();
}

void t4p::EditorBehaviorPanelClass::OnCheckRightMargin(wxCommandEvent& event) {
	RightMargin->Enable(event.IsChecked());
}

t4p::EditorCommandPanelClass::EditorCommandPanelClass(wxWindow* parent, int id,
													  std::vector<t4p::EditorKeyboardCommandClass>& commands,
													  wxKeyBinder* keyBinder)
: EditorCommandPanelGeneratedClass(parent, id)
, Commands(commands)
, EditedCommands(commands)
, KeyBinder(keyBinder) {
	List->ClearAll();
	List->InsertColumn(0, _("Command"));
	List->InsertColumn(1, _("Shortcut"));
	List->SetColumnWidth(0, 250);
	List->SetColumnWidth(1, 250);
	FillCommands();
}

bool t4p::EditorCommandPanelClass::TransferDataFromWindow() {
	Commands = EditedCommands;
	return true;
}

void t4p::EditorCommandPanelClass::FillCommands() {
	for (size_t i = 0; i < EditedCommands.size(); ++i) {
		wxListItem itemName;
		itemName.SetColumn(0);
		itemName.SetId(i);
		itemName.SetText(EditedCommands[i].Name);
		List->InsertItem(itemName);

		wxListItem itemShortcut;
		itemShortcut.SetId(i);
		itemShortcut.SetColumn(1);
		itemShortcut.SetText(EditedCommands[i].ToString());
		List->SetItem(itemShortcut);
	}
	List->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_MASK_STATE | wxLIST_MASK_TEXT);
	List->SetColumnWidth(0, wxLIST_AUTOSIZE);
	List->SetColumnWidth(1, wxLIST_AUTOSIZE);
}

void t4p::EditorCommandPanelClass::OnItemActivated(wxListEvent& event) {
	int row = event.GetIndex();

	wxString name = List->GetItemText(row, 0);
	wxString shortcut = List->GetItemText(row, 1);

	t4p::KeyboardCommandEditDialogClass dialog(this, name, shortcut, EditedCommands, KeyBinder);
	if (dialog.ShowModal() == wxOK) {
		EditedCommands[row].InitFromString(shortcut);
		List->SetItem(row, 1, shortcut);
	}
}

t4p::KeyboardCommandEditDialogClass::KeyboardCommandEditDialogClass(wxWindow* parent, const wxString& commandName,
																	wxString& shortcut,
																	std::vector<t4p::EditorKeyboardCommandClass>& commands,
																	wxKeyBinder* keyBinder)
: KeyboardCommandEditDialogGeneratedClass(parent, wxID_ANY)
, Commands(commands)
, KeyBinder(keyBinder)
, OriginalShortcut(shortcut) {
	Help->SetLabel(Help->GetLabel() + wxT(" ") + commandName);

	wxTextValidator val(wxFILTER_NONE, &shortcut);
	Edit->SetValidator(val);
	TransferDataToWindow();
}

void t4p::KeyboardCommandEditDialogClass::OnOkButton(wxCommandEvent& event) {
	wxString newShortcut = Edit->GetValue();
	if (newShortcut == OriginalShortcut || newShortcut.empty()) {

		// allow the user to "save" the same shortcut
		// also, allow the user to remove shortcuts (empty
		// string == no shortcut)
		TransferDataFromWindow();
		EndModal(wxOK);
		return;
	}
	wxString existingCmd;

	// for a shortcut to be valid, it must not already be assigned to
	// either the scintilla commands NOR the menu shortcuts
	bool alreadyExists = false;
	for (size_t i = 0; i < Commands.size(); ++i) {
		if (Commands[i].ToString() == newShortcut) {
			alreadyExists = true;
			existingCmd = Commands[i].Name;
			break;
		}
	}
	if (!alreadyExists && !newShortcut.empty()) {
		wxCmd* cmd = KeyBinder->GetCmdBindTo(newShortcut);
		if (cmd) {
			alreadyExists = true;
			existingCmd = cmd->GetName();
		}
	}
	if (alreadyExists) {
		wxMessageBox(
			wxString::Format(_("shortcut %s is already defined to %s"), newShortcut.c_str(), existingCmd),
			_("Edit Keyboard Command"),
			wxCENTRE, this
		);
		return;
	}
	TransferDataFromWindow();
	EndModal(wxOK);
}

void t4p::KeyboardCommandEditDialogClass::OnEnter(wxCommandEvent& event) {
	OnOkButton(event);
}

void t4p::KeyboardCommandEditDialogClass::OnKey(wxKeyEvent& event) {
	int keyCode = event.GetKeyCode();
	bool hasModifiers = event.HasAnyModifiers();
	if (hasModifiers && keyCode > 0) {
		Edit->SetValue(t4p::KeyCodeToShortcutString(event.GetModifiers(), keyCode));
		Edit->SetInsertionPointEnd();
	}
	else if (keyCode > WXK_START && keyCode <=  WXK_SPECIAL20) {
		Edit->SetValue(t4p::KeyCodeToShortcutString(event.GetModifiers(), keyCode));
		Edit->SetInsertionPointEnd();
	}
	else if (hasModifiers && (
		WXK_BACK == keyCode ||
		WXK_TAB == keyCode ||
		WXK_RETURN == keyCode ||
		WXK_ESCAPE == keyCode ||
		WXK_SPACE == keyCode ||
		WXK_DELETE  == keyCode)) {

		// shortcuts with special chars ie CTRL+TAB
		Edit->SetValue(t4p::KeyCodeToShortcutString(event.GetModifiers(), keyCode));
		Edit->SetInsertionPointEnd();
	}
	else {
		Edit->Clear();
		event.Skip();

	}
}

BEGIN_EVENT_TABLE(t4p::EditorBehaviorViewClass, t4p::FeatureViewClass)
	EVT_APP_FILE_OPEN(t4p::EditorBehaviorViewClass::OnAppFileOpened)
	EVT_APP_FILE_NEW(t4p::EditorBehaviorViewClass::OnAppFileNew)
	EVT_APP_FILE_CLOSED(t4p::EditorBehaviorViewClass::OnAppFileClosed)
	EVT_MENU(t4p::MENU_BEHAVIOR + 1, t4p::EditorBehaviorViewClass::OnToggleWordWrap)
	EVT_MENU(t4p::MENU_BEHAVIOR + 2, t4p::EditorBehaviorViewClass::OnToggleIndentationGuides)
	EVT_MENU(t4p::MENU_BEHAVIOR + 3, t4p::EditorBehaviorViewClass::OnToggleWhitespace)
	EVT_MENU(t4p::MENU_BEHAVIOR + 4, t4p::EditorBehaviorViewClass::OnZoomIn)
	EVT_MENU(t4p::MENU_BEHAVIOR + 5, t4p::EditorBehaviorViewClass::OnZoomOut)
	EVT_MENU(t4p::MENU_BEHAVIOR + 6, t4p::EditorBehaviorViewClass::OnZoomReset)
	EVT_MENU(t4p::MENU_BEHAVIOR + 7, t4p::EditorBehaviorViewClass::OnEditConvertEols)
	EVT_MENU(t4p::MENU_BEHAVIOR + 8, t4p::EditorBehaviorViewClass::OnEditConvertEols)
	EVT_MENU(t4p::MENU_BEHAVIOR + 9, t4p::EditorBehaviorViewClass::OnEditConvertEols)

	EVT_MENU(wxID_CUT, t4p::EditorBehaviorViewClass::OnEditCut)
	EVT_MENU(wxID_COPY, t4p::EditorBehaviorViewClass::OnEditCopy)
	EVT_MENU(wxID_PASTE, t4p::EditorBehaviorViewClass::OnEditPaste)
	EVT_MENU(wxID_SELECTALL, t4p::EditorBehaviorViewClass::OnEditSelectAll)
	EVT_MENU(wxID_UNDO, t4p::EditorBehaviorViewClass::OnUndo)
	EVT_MENU(wxID_REDO, t4p::EditorBehaviorViewClass::OnRedo)
	EVT_MENU(t4p::MENU_BEHAVIOR + 10, t4p::EditorBehaviorViewClass::OnEditContentAssist)
	EVT_MENU(t4p::MENU_BEHAVIOR + 11, t4p::EditorBehaviorViewClass::OnEditCallTip)
	EVT_MENU_RANGE(t4p::MENU_BEHAVIOR + 20, t4p::MENU_BEHAVIOR + 99, t4p::EditorBehaviorViewClass::OnEditorCommand)

	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::EditorBehaviorViewClass::OnPreferencesSaved)
END_EVENT_TABLE()

