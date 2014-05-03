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
 * @copyright  2014 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <features/EditorBehaviorFeatureClass.h>
#include <globals/Assets.h>
#include <Triumph.h>
#include <wx/valgen.h>

static int SciCommands[][2] = {

{ t4p::MENU_BEHAVIOR + 10, wxSTC_CMD_LINECUT },
{ t4p::MENU_BEHAVIOR + 11, wxSTC_CMD_LINEDUPLICATE },
{ t4p::MENU_BEHAVIOR + 12, wxSTC_CMD_LINEDELETE },
{ t4p::MENU_BEHAVIOR + 13, wxSTC_CMD_LINETRANSPOSE },
{ t4p::MENU_BEHAVIOR + 14, wxSTC_CMD_LINECOPY },
{ t4p::MENU_BEHAVIOR + 15, wxSTC_CMD_LOWERCASE },
{ t4p::MENU_BEHAVIOR + 16, wxSTC_CMD_UPPERCASE },
{ t4p::MENU_BEHAVIOR + 17, wxSTC_CMD_DELWORDLEFT },
{ t4p::MENU_BEHAVIOR + 18, wxSTC_CMD_DELWORDRIGHT },
{ t4p::MENU_BEHAVIOR + 19, wxSTC_CMD_DELLINELEFT },
{ t4p::MENU_BEHAVIOR + 20, wxSTC_CMD_DELLINERIGHT },
{ t4p::MENU_BEHAVIOR + 21, 2469 }, // there is no wxSTC_CMD_SELECTIONDUPLICATE
{ t4p::MENU_BEHAVIOR + 22, wxSTC_CMD_MOVESELECTEDLINESUP },
{ t4p::MENU_BEHAVIOR + 23, wxSTC_CMD_MOVESELECTEDLINESDOWN },
{ t4p::MENU_BEHAVIOR + 24, wxSTC_CMD_PARADOWNEXTEND },
{ t4p::MENU_BEHAVIOR + 25, wxSTC_CMD_PARAUPEXTEND },
{ t4p::MENU_BEHAVIOR + 26, wxSTC_CMD_WORDLEFTEXTEND },
{ t4p::MENU_BEHAVIOR + 27, wxSTC_CMD_WORDRIGHTEXTEND },
{ t4p::MENU_BEHAVIOR + 28, wxSTC_CMD_WORDLEFTENDEXTEND },
{ t4p::MENU_BEHAVIOR + 29, wxSTC_CMD_WORDRIGHTENDEXTEND },
{ t4p::MENU_BEHAVIOR + 30, wxSTC_CMD_WORDPARTLEFTEXTEND },
{ t4p::MENU_BEHAVIOR + 31, wxSTC_CMD_WORDPARTRIGHTEXTEND },
{ t4p::MENU_BEHAVIOR + 32, wxSTC_CMD_PARADOWN },
{ t4p::MENU_BEHAVIOR + 33, wxSTC_CMD_PARAUP },
{ t4p::MENU_BEHAVIOR + 34, wxSTC_CMD_WORDLEFT },
{ t4p::MENU_BEHAVIOR + 35, wxSTC_CMD_WORDRIGHT },
{ t4p::MENU_BEHAVIOR + 36, wxSTC_CMD_WORDPARTLEFT },
{ t4p::MENU_BEHAVIOR + 37, wxSTC_CMD_WORDPARTRIGHT },
{ 0, 0 }

};

// key codes > 300 have different values in wxWidgets and
// scintilla. must map them correctly
static int SciSpecialKeys[][2] = {
	{ WXK_DOWN, wxSTC_KEY_DOWN },
	{ WXK_UP, wxSTC_KEY_UP },
	{ WXK_LEFT, wxSTC_KEY_LEFT },
	{ WXK_RIGHT, wxSTC_KEY_RIGHT },
	{ WXK_HOME, wxSTC_KEY_HOME },
	{ WXK_END, wxSTC_KEY_END },
	{ WXK_PAGEUP, wxSTC_KEY_PRIOR },
	{ WXK_PAGEDOWN, wxSTC_KEY_NEXT },
	{ WXK_DELETE, wxSTC_KEY_DELETE },
	{ WXK_INSERT, wxSTC_KEY_INSERT },
	{ WXK_ESCAPE, wxSTC_KEY_ESCAPE },
	{ WXK_BACK, wxSTC_KEY_BACK },
	{ WXK_TAB, wxSTC_KEY_TAB },
	{ WXK_RETURN, wxSTC_KEY_RETURN },
	{ WXK_ADD, wxSTC_KEY_ADD },
	{ WXK_SUBTRACT, wxSTC_KEY_SUBTRACT },
	{ WXK_DIVIDE, wxSTC_KEY_DIVIDE },
	{ WXK_WINDOWS_MENU, wxSTC_KEY_WIN },
	{ WXK_WINDOWS_RIGHT, wxSTC_KEY_RWIN },
	{ WXK_MENU, wxSTC_KEY_MENU },
	{ 0, 0 }
};

static int ID_SUBMENU_TRANSFORM = wxNewId();
static int ID_SUBMENU_SELECTION = wxNewId();
static int ID_SUBMENU_CARET = wxNewId();

/**
 * adds menu items for each of the scintilla keyboard commands
 * to the given menu
 * @param menu the menu to add the items to
 * @param bool if TRUE menu items that require that text be selected
 *        will be disabled
 */
static void AddKeyboardCommands(wxMenu* menu, bool isTextSelected) {
	
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
	wxMenu* transformMenu = new wxMenu();
	transformMenu->Append(t4p::MENU_BEHAVIOR + 10, _("Cut Line\tCTRL+L"));
	transformMenu->Append(t4p::MENU_BEHAVIOR + 11, _("Duplicate Line\tCTRL+D"));
	transformMenu->Append(t4p::MENU_BEHAVIOR + 12, _("Delete Line\tCTRL+SHIFT+L"));
	transformMenu->Append(t4p::MENU_BEHAVIOR + 13, _("Transpose Line\tCTRL+T"));
	transformMenu->Append(t4p::MENU_BEHAVIOR + 14, _("Copy Line\tCTRL+SHIFT+T"));
	transformMenu->Append(t4p::MENU_BEHAVIOR + 15, _("Convert To Lower Case\tCTRL+U"));
	transformMenu->Append(t4p::MENU_BEHAVIOR + 16, _("Convert To Upper Case\tCTRL+Shift+U"));
	transformMenu->Append(t4p::MENU_BEHAVIOR + 17, _("Delete Previous Word\tCTRL+BACK"));
	transformMenu->Append(t4p::MENU_BEHAVIOR + 18, _("Delete Next Word\tCTRL+DEL"));
	transformMenu->Append(t4p::MENU_BEHAVIOR + 19, _("Delete Line up to cursor\tCTRL+SHIFT+BACK"));
	transformMenu->Append(t4p::MENU_BEHAVIOR + 20, _("Delete Line after cursor\tCTRL+SHIFT+DEL"));
	transformMenu->Append(t4p::MENU_BEHAVIOR + 21, _("Duplicate selection"));
	transformMenu->Append(t4p::MENU_BEHAVIOR + 22, _("Move selected lines up\tCTRL+SHIFT+UP"));
	transformMenu->Append(t4p::MENU_BEHAVIOR + 23, _("Move selected lines down\tCTRL+SHIFT+DOWN"));
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
	selectionMenu->Append(t4p::MENU_BEHAVIOR + 24, _("Expand selection to the end of paragraph")); //\tCTRL+SHIFT+]
	selectionMenu->Append(t4p::MENU_BEHAVIOR + 25, _("Expand selection to the beginning of paragraph")); // \tCTRL+SHIFT+[
	selectionMenu->Append(t4p::MENU_BEHAVIOR + 26, _("Expand selection to the beginning of previous word\tCTRL+SHIFT+LEFT"));
	selectionMenu->Append(t4p::MENU_BEHAVIOR + 27, _("Expand selection to the beginning of next word\tCTRL+SHIFT+RIGHT"));
	selectionMenu->Append(t4p::MENU_BEHAVIOR + 28, _("Expand selection to the end of previous word"));
	selectionMenu->Append(t4p::MENU_BEHAVIOR + 29, _("Expand selection to the end of next word"));
	selectionMenu->Append(t4p::MENU_BEHAVIOR + 30, _("Expand selection to the previous word segment")); // \tCTRL+SHIFT+/
	selectionMenu->Append(t4p::MENU_BEHAVIOR + 31, _("Expand selection to the end of next word segment")); // \tCTRL+SHIFT+backslash
	
	/*
	the caret menu exposes the following scintilla commands
	SCI_PARADOWN
	SCI_PARAUP
	SCI_WORDLEFT
	SCI_WORDRIGHT
	SCI_WORDPARTLEFT
	SCI_WORDPARTRIGHT 
	*/
		
	//?? not sure what these do
	//SCI_WORDLEFTEND
	//SCI_WORDRIGHTEND
	wxMenu* caretMenu = new wxMenu();
	caretMenu->Append(t4p::MENU_BEHAVIOR + 32, _("Move to next paragraph")); // \tCTRL+]
	caretMenu->Append(t4p::MENU_BEHAVIOR + 33, _("Move to previous paragraph")); // \tCTRL+[
	caretMenu->Append(t4p::MENU_BEHAVIOR + 34, _("Move to previous word\tCTRL+LEFT"));
	caretMenu->Append(t4p::MENU_BEHAVIOR + 35, _("Move to next word\tCTRL+RIGHT"));
	caretMenu->Append(t4p::MENU_BEHAVIOR + 36, _("Move to previous word segment")); // \tCTRL+/
	caretMenu->Append(t4p::MENU_BEHAVIOR + 37, _("Move to next word segment")); // \tCTRL+backslash
	
	menu->Append(ID_SUBMENU_TRANSFORM, _("Transform text"), transformMenu);
	menu->Append(ID_SUBMENU_SELECTION, _("Selection"), selectionMenu);
	menu->Append(ID_SUBMENU_CARET, _("Move caret"), caretMenu);
	
	transformMenu->Enable(t4p::MENU_BEHAVIOR + 15, isTextSelected);
	transformMenu->Enable(t4p::MENU_BEHAVIOR + 16, isTextSelected);

}

static void AssignKeyCommands(wxStyledTextCtrl* ctrl, const wxKeyProfile* profile) {

	for (int i = 0; SciCommands[i][0] > 0; ++i) {
		wxCmd* cmd = profile->GetCmd(SciCommands[i][0]);
		wxASSERT_MSG(cmd, _("command must be found"));
		if (cmd) {
			wxKeyBind* bind = cmd->GetShortcut(0);
			int keybinderMods = bind->GetModifiers();
			int wxkey= bind->GetKeyCode();
			int stcKey = 0;
			int stcMods = 0;
			if (keybinderMods & wxACCEL_SHIFT) {
				stcMods |= wxSTC_SCMOD_SHIFT;
			}
			if (keybinderMods & wxACCEL_CTRL) {
				stcMods |= wxSTC_SCMOD_CTRL;
			}
			if (keybinderMods & wxACCEL_ALT) {
				stcMods |= wxSTC_SCMOD_ALT;
			}
			// raw ctrl is only for mac (OSX) which we dont support right now
			
			// key codes >300 have different numbers, ie. the key code for PAGEUP is different
			// in scintilla and wxWidgets.
			bool foundKey = false;
			for (int j = 0; SciSpecialKeys[j][0] > 0; ++j) {
				if (SciSpecialKeys[j][0] == wxkey) {
					stcKey = SciSpecialKeys[j][1];
					foundKey = true;
				}
			}
			if (!foundKey) {
				stcKey = wxkey;
			}
			
			if (stcKey > 0 && stcMods >= 0) {
				ctrl->CmdKeyAssign(stcKey, stcMods, SciCommands[i][1]);
			}
		}
	}
}

t4p::EditorBehaviorFeatureClass::EditorBehaviorFeatureClass(t4p::AppClass& app)
: FeatureClass(app) {
}

void t4p::EditorBehaviorFeatureClass::AddToolBarItems(wxAuiToolBar* toolBar) {
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

void t4p::EditorBehaviorFeatureClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[t4p::MENU_BEHAVIOR + 4] = wxT("Edit-Zoom In");
	menuItemIds[t4p::MENU_BEHAVIOR + 5] = wxT("Edit-Zoom Out");
	menuItemIds[t4p::MENU_BEHAVIOR + 6] = wxT("Edit-Reset Zoom");
	
	menuItemIds[t4p::MENU_BEHAVIOR + 10] = wxT("Edit-Cut Line");
	menuItemIds[t4p::MENU_BEHAVIOR + 11] = wxT("Edit-Duplicate Line");
	menuItemIds[t4p::MENU_BEHAVIOR + 12] = wxT("Edit-Delete Line");
	menuItemIds[t4p::MENU_BEHAVIOR + 13] = wxT("Edit-Transpose Line");
	menuItemIds[t4p::MENU_BEHAVIOR + 14] = wxT("Edit-Copy Line");
	menuItemIds[t4p::MENU_BEHAVIOR + 15] = wxT("Edit-Convert To Lower Case");
	menuItemIds[t4p::MENU_BEHAVIOR + 16] = wxT("Edit-Convert To Upper Case");
	menuItemIds[t4p::MENU_BEHAVIOR + 17] = wxT("Edit-Delete Previous Word");
	menuItemIds[t4p::MENU_BEHAVIOR + 18] = wxT("Edit-Delete Next Word");
	menuItemIds[t4p::MENU_BEHAVIOR + 19] = wxT("Edit-Delete Line up to cursor");
	menuItemIds[t4p::MENU_BEHAVIOR + 20] = wxT("Edit-Delete Line after cursor");
	menuItemIds[t4p::MENU_BEHAVIOR + 21] = wxT("Edit-Duplicate selection");
	menuItemIds[t4p::MENU_BEHAVIOR + 22] = wxT("Edit-Move selected lines up");
	menuItemIds[t4p::MENU_BEHAVIOR + 23] = wxT("Edit-Move selected lines down");
	
	menuItemIds[t4p::MENU_BEHAVIOR + 24] = wxT("Edit-Expand selection to the end of paragraph");
	menuItemIds[t4p::MENU_BEHAVIOR + 25] = wxT("Edit-Expand selection to the beginning of paragraph");
	menuItemIds[t4p::MENU_BEHAVIOR + 26] = wxT("Edit-Expand selection to the beginning of previous word");
	menuItemIds[t4p::MENU_BEHAVIOR + 27] = wxT("Edit-Expand selection to the beginning of next word");
	menuItemIds[t4p::MENU_BEHAVIOR + 28] = wxT("Edit-Expand selection to the end of previous word");
	menuItemIds[t4p::MENU_BEHAVIOR + 29] = wxT("Edit-Expand selection to the end of next word");
	menuItemIds[t4p::MENU_BEHAVIOR + 30] = wxT("Edit-Expand selection to the previous word segment");
	menuItemIds[t4p::MENU_BEHAVIOR + 31] = wxT("Edit-Expand selection to the end of next word segment");
	
	menuItemIds[t4p::MENU_BEHAVIOR + 32] = wxT("Edit-Move to next paragraph");
	menuItemIds[t4p::MENU_BEHAVIOR + 33] = wxT("Edit-Move to previous paragraph");
	menuItemIds[t4p::MENU_BEHAVIOR + 34] = wxT("Edit-Move to previous word");
	menuItemIds[t4p::MENU_BEHAVIOR + 35] = wxT("Edit-Move to next word");
	menuItemIds[t4p::MENU_BEHAVIOR + 36] = wxT("Edit-Move to previous word segment");
	menuItemIds[t4p::MENU_BEHAVIOR + 37] = wxT("Edit-Move to next word segment");
	
	AddDynamicCmd(menuItemIds, shortcuts);
}

void t4p::EditorBehaviorFeatureClass::AddEditMenuItems(wxMenu* editMenu) {
	wxMenu* subMenu = new wxMenu();
	subMenu->Append(t4p::MENU_BEHAVIOR + 7, _("Unix (LF)"), _("Convert Line ending to UNIX"));
	subMenu->Append(t4p::MENU_BEHAVIOR + 8, _("Windows (CRLF)"), _("Convert Line ending to Windows"));
	subMenu->Append(t4p::MENU_BEHAVIOR + 9, _("Max (CR)"), _("Convert Line ending to MAC"));
	
	editMenu->Append(wxID_ANY, _("Convert Line Endings To"), subMenu);
	
	AddKeyboardCommands(editMenu, true);
}

void t4p::EditorBehaviorFeatureClass::AddViewMenuItems(wxMenu* menu) {
	menu->Append(t4p::MENU_BEHAVIOR + 4, _("Zoom In"), _("Zoom In"));
	menu->Append(t4p::MENU_BEHAVIOR + 5, _("Zoom Out"), _("Zoom Out"));
	menu->Append(t4p::MENU_BEHAVIOR + 6, _("Reset Zoom\tCTRL+0"), _("Reset Zoom"));
}

void t4p::EditorBehaviorFeatureClass::AddCodeControlClassContextMenuItems(wxMenu* menu) {
	
	// no need to delete moreMenu pointer, the contextMenu will delete it for us
	t4p::CodeControlClass* codeCtrl = GetCurrentCodeControl();
	bool isTextSelected = false;
	if (codeCtrl) {
		isTextSelected = !codeCtrl->GetSelectedText().IsEmpty();
	}
	AddKeyboardCommands(menu, isTextSelected);
	
	// update the menu accelerators to be the ones configured by the
	// user
	// AddKeyboardCommands sets the menu items to have the default
	// shortcuts, but the user can change them
	wxKeyBinder* binder = App.Preferences.KeyProfiles.GetSelProfile();
	wxCmdArray* cmds = binder->GetArray();
	
	for (int i = 0; SciCommands[i][0] > 0; ++i) {
		int menuId = SciCommands[i][0];
		wxMenuItem* menuItem = menu->FindItem(menuId);
		if (menuItem) {
			for (int j = 0; j < cmds->GetCount(); ++j) {
				wxCmd* cmd = cmds->Item(j);
				if (cmd->GetId() == menuId) {
					wxString label = 
						menuItem->GetItemLabelText()
						+ wxT("\t") 
						+ cmd->GetShortcut(0)->GetStr();
					menuItem->SetItemLabel(label);
				}
			}
		}
	}
}

void t4p::EditorBehaviorFeatureClass::OnToggleWordWrap(wxCommandEvent& event) {
	App.Preferences.CodeControlOptions.EnableWordWrap = !App.Preferences.CodeControlOptions.EnableWordWrap;
	
	wxConfigBase* config = wxConfigBase::Get(false);
	App.Preferences.CodeControlOptions.Save(config);
	config->Flush();
	
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorFeatureClass::OnToggleIndentationGuides(wxCommandEvent& event) {
	App.Preferences.CodeControlOptions.EnableIndentationGuides = !App.Preferences.CodeControlOptions.EnableIndentationGuides;
	
	wxConfigBase* config = wxConfigBase::Get(false);
	App.Preferences.CodeControlOptions.Save(config);
	config->Flush();
	
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorFeatureClass::OnToggleWhitespace(wxCommandEvent& event) {
	App.Preferences.CodeControlOptions.ShowWhitespace = !App.Preferences.CodeControlOptions.ShowWhitespace;
	
	wxConfigBase* config = wxConfigBase::Get(false);
	App.Preferences.CodeControlOptions.Save(config);
	config->Flush();
	
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorFeatureClass::OnZoomIn(wxCommandEvent& event) {
	App.Preferences.CodeControlOptions.Zoom++;
	if (App.Preferences.CodeControlOptions.Zoom > 50) {
		App.Preferences.CodeControlOptions.Zoom = 50;
	}
	
	wxConfigBase* config = wxConfigBase::Get(false);
	App.Preferences.CodeControlOptions.Save(config);
	config->Flush();
	
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorFeatureClass::OnZoomOut(wxCommandEvent& event) {
	App.Preferences.CodeControlOptions.Zoom--;
	if (App.Preferences.CodeControlOptions.Zoom < -35) {
		App.Preferences.CodeControlOptions.Zoom = -35;
	}
	
	wxConfigBase* config = wxConfigBase::Get(false);
	App.Preferences.CodeControlOptions.Save(config);
	config->Flush();
	
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorFeatureClass::OnZoomReset(wxCommandEvent& event) {
	App.Preferences.CodeControlOptions.Zoom = 0;
	
	wxConfigBase* config = wxConfigBase::Get(false);
	App.Preferences.CodeControlOptions.Save(config);
	config->Flush();
	
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorFeatureClass::OnEditConvertEols(wxCommandEvent& event) {
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

void t4p::EditorBehaviorFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {
	SetFeaturesOnNotebook();
}

void t4p::EditorBehaviorFeatureClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
	t4p::EditorBehaviorPanelClass* panel = new t4p::EditorBehaviorPanelClass(parent, 
		App.Preferences.CodeControlOptions);
	parent->AddPage(panel, _("Editor Behavior"));
}

void t4p::EditorBehaviorFeatureClass::SetFeaturesOnNotebook() {
	t4p::NotebookClass* notebook = GetNotebook();
	t4p::CodeControlOptionsClass options = App.Preferences.CodeControlOptions;
	for (size_t i = 0; i < notebook->GetPageCount(); ++i) {
		SetFeatures(options, notebook->GetCodeControl(i));
	}
}

void t4p::EditorBehaviorFeatureClass::SetFeatures(const t4p::CodeControlOptionsClass& options, wxStyledTextCtrl* codeCtrl) {
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
	
	const wxKeyProfile* profile = App.Preferences.KeyProfiles.GetSelProfile();
	AssignKeyCommands(codeCtrl, profile);
}

void t4p::EditorBehaviorFeatureClass::OnAppFileOpened(t4p::CodeControlEventClass& event) {
	SetFeatures(App.Preferences.CodeControlOptions, event.GetCodeControl());
}

void t4p::EditorBehaviorFeatureClass::OnAppFileNew(t4p::CodeControlEventClass& event) {
	SetFeatures(App.Preferences.CodeControlOptions, event.GetCodeControl());
}

void t4p::EditorBehaviorFeatureClass::OnEditorCommand(wxCommandEvent& event) {
	t4p::CodeControlClass* ctrl = GetCurrentCodeControl();
	if (!ctrl) {
		return;
	}
	bool handled = false;
	
	// this method handles all scintilla commands
	// use the menu item ID to lookup the corresponding scintilla command
	int itemId = event.GetId();
	for (int i = 0; SciCommands[i][0] > 0; ++i) {
		if (itemId == SciCommands[i][0]) {
			ctrl->CmdKeyExecute(SciCommands[i][1]);
			handled = true;
			break;
		}
	}
	if (!handled) {
		event.Skip();
	}
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

BEGIN_EVENT_TABLE(t4p::EditorBehaviorFeatureClass, t4p::FeatureClass)
	EVT_APP_FILE_OPEN(t4p::EditorBehaviorFeatureClass::OnAppFileOpened)
	EVT_APP_FILE_NEW(t4p::EditorBehaviorFeatureClass::OnAppFileNew)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::EditorBehaviorFeatureClass::OnPreferencesSaved)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_EXTERNALLY_UPDATED, t4p::EditorBehaviorFeatureClass::OnPreferencesSaved)
	EVT_MENU(t4p::MENU_BEHAVIOR + 1, t4p::EditorBehaviorFeatureClass::OnToggleWordWrap)
	EVT_MENU(t4p::MENU_BEHAVIOR + 2, t4p::EditorBehaviorFeatureClass::OnToggleIndentationGuides)
	EVT_MENU(t4p::MENU_BEHAVIOR + 3, t4p::EditorBehaviorFeatureClass::OnToggleWhitespace)
	EVT_MENU(t4p::MENU_BEHAVIOR + 4, t4p::EditorBehaviorFeatureClass::OnZoomIn)
	EVT_MENU(t4p::MENU_BEHAVIOR + 5, t4p::EditorBehaviorFeatureClass::OnZoomOut)
	EVT_MENU(t4p::MENU_BEHAVIOR + 6, t4p::EditorBehaviorFeatureClass::OnZoomReset)
	EVT_MENU(t4p::MENU_BEHAVIOR + 7, t4p::EditorBehaviorFeatureClass::OnEditConvertEols)
	EVT_MENU(t4p::MENU_BEHAVIOR + 8, t4p::EditorBehaviorFeatureClass::OnEditConvertEols)
	EVT_MENU(t4p::MENU_BEHAVIOR + 9, t4p::EditorBehaviorFeatureClass::OnEditConvertEols)
	
	EVT_MENU_RANGE(t4p::MENU_BEHAVIOR + 10, t4p::MENU_BEHAVIOR + 99, t4p::EditorBehaviorFeatureClass::OnEditorCommand)

END_EVENT_TABLE()


