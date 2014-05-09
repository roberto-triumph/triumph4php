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
#include <main_frame/PreferencesClass.h>
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


static int WxKeyCodeToSciKeyCode(int wxKeyCode) {
	int sciKeyCode = 0;
	bool foundKey = false;
	for (int j = 0; SciSpecialKeys[j][0] > 0; ++j) {
		if (SciSpecialKeys[j][0] == wxKeyCode) {
			sciKeyCode = SciSpecialKeys[j][1];
			foundKey = true;
		}
	}
	if (!foundKey) {
		sciKeyCode = wxKeyCode;
	}
	return sciKeyCode;
}

static int SciKeyCodeToWxKeyCode(int sciKeyCode) {
	int wxKeyCode = 0;
	bool foundKey = false;
	for (int j = 0; SciSpecialKeys[j][0] > 0; ++j) {
		if (SciSpecialKeys[j][1] == sciKeyCode) {
			wxKeyCode = SciSpecialKeys[j][0];
			foundKey = true;
		}
	}
	if (!foundKey) {
		wxKeyCode = sciKeyCode;
	}
	return wxKeyCode;
}

static int WxModifiersToSciModifiers(int wxModifiers) {
	int stcMods = 0;
	if (wxModifiers & wxACCEL_SHIFT) {
		stcMods |= wxSTC_SCMOD_SHIFT;
	}
	if (wxModifiers & wxACCEL_CTRL) {
		stcMods |= wxSTC_SCMOD_CTRL;
	}
	if (wxModifiers & wxACCEL_ALT) {
		stcMods |= wxSTC_SCMOD_ALT;
	}
	return stcMods;
}

static int SciModifiersToWxModifiers(int sciModifiers) {
	int wxMods = 0;
	if (sciModifiers & wxSTC_SCMOD_SHIFT) {
		wxMods |= wxACCEL_SHIFT;
	}	
	if (sciModifiers & wxSTC_SCMOD_CTRL) {
		wxMods |= wxACCEL_CTRL;
	}
	if (sciModifiers & wxSTC_SCMOD_ALT) {
		wxMods |= wxACCEL_ALT;
	}
	return wxMods;
}

static  int SciCommandToMenuId(int cmdId) {
	for (int i = 0; SciCommands[i][0] > 0; ++i) {
		if (SciCommands[i][1] == cmdId) {
			return SciCommands[i][0];
		}
	}
	return 0;
}

static int MenuIdToSciCommand(int menuId) {
	for (int i = 0; SciCommands[i][0] > 0; ++i) {
		if (SciCommands[i][0] == menuId) {
			return SciCommands[i][1];
		}
	}
	return 0;
}

static void KeyboardCommandFromString(t4p::EditorKeyboardCommandClass& cmd, const wxString& shortcut) {
	int wxKeyCode = 0;
	int wxModifiers = 0;
	if (!shortcut.empty()) {
		t4p::ShortcutStringToKeyCode(shortcut, wxModifiers, wxKeyCode);
	
		// turn wx keys, modifiers into stc counterparts
		cmd.KeyCode = WxKeyCodeToSciKeyCode(wxKeyCode);
		cmd.Modifiers = WxModifiersToSciModifiers(wxModifiers);
	}
	else {
		// empty string == no shortcut
		cmd.KeyCode = 0;
		cmd.Modifiers = 0;
	}
}

static wxString KeyboardCommandToString(t4p::EditorKeyboardCommandClass& cmd) {

	// turn stc keys, modifiers into wx counterparts
	// to feed into the keybinder function
	int wxKeyCode = SciKeyCodeToWxKeyCode(cmd.KeyCode);
	int wxModifiers = SciModifiersToWxModifiers(cmd.Modifiers);
	if (wxKeyCode > 0 && wxModifiers > 0) {
		return t4p::KeyCodeToShortcutString(wxModifiers, wxKeyCode);
	}
	return wxT("");
}

static void PC(std::vector<t4p::EditorKeyboardCommandClass>& commands, const wxString& name, int cmdId, int modifiers, int keyCode) {
	commands.push_back(t4p::EditorKeyboardCommandClass(name, cmdId, modifiers, keyCode));
}

static void SetupCommands(std::vector<t4p::EditorKeyboardCommandClass>& cmds) {
	PC(cmds, _("Cut Line"), wxSTC_CMD_LINECUT, wxSTC_SCMOD_CTRL, 'L');
	PC(cmds, _("Duplicate Line"), wxSTC_CMD_LINEDUPLICATE,  wxSTC_SCMOD_CTRL, 'D');
	PC(cmds, _("Delete Line"), wxSTC_CMD_LINEDELETE, wxSTC_SCMOD_CTRL | wxSTC_SCMOD_SHIFT, 'L');
	PC(cmds, _("Transpose Line"), wxSTC_CMD_LINETRANSPOSE, wxSTC_SCMOD_CTRL, 'T');
	PC(cmds, _("Copy Line"), wxSTC_CMD_LINECOPY , wxSTC_SCMOD_CTRL | wxSTC_SCMOD_SHIFT, 'T');
	PC(cmds, _("Convert To Lower Case"), wxSTC_CMD_LOWERCASE, wxSTC_SCMOD_CTRL, 'U');
	PC(cmds, _("Convert To Upper Case"), wxSTC_CMD_UPPERCASE, wxSTC_SCMOD_CTRL | wxSTC_SCMOD_SHIFT, 'U');
	PC(cmds, _("Delete to start of Word"), wxSTC_CMD_DELWORDLEFT, wxSTC_SCMOD_CTRL, wxSTC_KEY_BACK);
	PC(cmds, _("Delete to end of Word"), wxSTC_CMD_DELWORDRIGHT, wxSTC_SCMOD_CTRL, wxSTC_KEY_DELETE);
	PC(cmds, _("Delete Line up to cursor"), wxSTC_CMD_DELLINELEFT, wxSTC_SCMOD_CTRL | wxSTC_SCMOD_SHIFT, wxSTC_KEY_BACK);
	PC(cmds, _("Delete Line after cursor"), wxSTC_CMD_DELLINERIGHT, wxSTC_SCMOD_CTRL | wxSTC_SCMOD_SHIFT, wxSTC_KEY_DELETE);
	PC(cmds, _("Duplicate selection"), 2469, /* there is no wxSTC_CMD_SELECTIONDUPLICATE*/ 0, 0);
	PC(cmds, _("Move selected lines up"), wxSTC_CMD_MOVESELECTEDLINESUP, wxSTC_SCMOD_CTRL | wxSTC_SCMOD_SHIFT, wxSTC_KEY_UP);
	PC(cmds, _("Move selected lines down"), wxSTC_CMD_MOVESELECTEDLINESDOWN, wxSTC_SCMOD_CTRL | wxSTC_SCMOD_SHIFT, wxSTC_KEY_DOWN);
	PC(cmds, _("Expand selection to the end of paragraph"), wxSTC_CMD_PARADOWNEXTEND, wxSTC_SCMOD_CTRL | wxSTC_SCMOD_SHIFT, ']');
	PC(cmds, _("Expand selection to the beginning of paragraph"), wxSTC_CMD_PARAUPEXTEND, wxSTC_SCMOD_CTRL | wxSTC_SCMOD_SHIFT, '[');
	PC(cmds, _("Expand selection to the beginning of previous word"), wxSTC_CMD_WORDLEFTEXTEND, wxSTC_SCMOD_CTRL | wxSTC_SCMOD_SHIFT, wxSTC_KEY_LEFT);
	PC(cmds, _("Expand selection to the beginning of next word"), wxSTC_CMD_WORDRIGHTEXTEND, wxSTC_SCMOD_CTRL | wxSTC_SCMOD_SHIFT, wxSTC_KEY_RIGHT);
	PC(cmds, _("Expand selection to the end of previous word"), wxSTC_CMD_WORDLEFTENDEXTEND, 0, 0);
	PC(cmds, _("Expand selection to the end of next word") , wxSTC_CMD_WORDRIGHTENDEXTEND, 0, 0);
	PC(cmds, _("Expand selection to the previous word segment"), wxSTC_CMD_WORDPARTLEFTEXTEND, wxSTC_SCMOD_CTRL | wxSTC_SCMOD_SHIFT, '/');
	PC(cmds, _("Expand selection to the end of next word segment"), wxSTC_CMD_WORDPARTRIGHTEXTEND, wxSTC_SCMOD_CTRL | wxSTC_SCMOD_SHIFT, '\\');
	PC(cmds, _("Move to next paragraph"), wxSTC_CMD_PARADOWN, wxSTC_SCMOD_CTRL, ']');
	PC(cmds, _("Move to previous paragraph"), wxSTC_CMD_PARAUP, wxSTC_SCMOD_CTRL, '[');
	PC(cmds, _("Move to previous word"), wxSTC_CMD_WORDLEFT, wxSTC_SCMOD_CTRL, wxSTC_KEY_LEFT);
	PC(cmds, _("Move to next word"), wxSTC_CMD_WORDRIGHT, wxSTC_SCMOD_CTRL, wxSTC_KEY_RIGHT);
	PC(cmds, _("Move to previous word segment"), wxSTC_CMD_WORDPARTLEFT, wxSTC_SCMOD_CTRL, '/');
	PC(cmds, _("Move to next word segment"), wxSTC_CMD_WORDPARTRIGHT, wxSTC_SCMOD_CTRL, '\\');
}

t4p::EditorKeyboardCommandClass::EditorKeyboardCommandClass() 
: Name()
, CmdId(0)
, Modifiers(0)
, KeyCode(0) {
}

t4p::EditorKeyboardCommandClass::EditorKeyboardCommandClass(const wxString& name,
															int cmdId, int modifiers,
															int keyCode) 
: Name(name)
, CmdId(cmdId)
, Modifiers(modifiers)
, KeyCode(keyCode) {
}

t4p::EditorKeyboardCommandClass::EditorKeyboardCommandClass(const t4p::EditorKeyboardCommandClass& src)
: Name()
, CmdId(0)
, Modifiers(0)
, KeyCode(0) {
	Copy(src);
}

t4p::EditorKeyboardCommandClass& t4p::EditorKeyboardCommandClass::operator=(const t4p::EditorKeyboardCommandClass& src) {
	Copy(src);
	return *this;
}

void t4p::EditorKeyboardCommandClass::Copy(const t4p::EditorKeyboardCommandClass& src) {
	Name = src.Name;
	CmdId = src.CmdId;
	Modifiers = src.Modifiers;
	KeyCode = src.KeyCode;
}

bool t4p::EditorKeyboardCommandClass::IsOk() const {
	return Modifiers > 0 && KeyCode > 0;
}

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
	// 2469 == there is no wxSTC_CMD_SELECTIONDUPLICATE
	wxMenu* transformMenu = new wxMenu();
	transformMenu->Append(SciCommandToMenuId(wxSTC_CMD_LINECUT), _("Cut Line"));
	transformMenu->Append(SciCommandToMenuId(wxSTC_CMD_LINEDUPLICATE), _("Duplicate Line"));
	transformMenu->Append(SciCommandToMenuId(wxSTC_CMD_LINEDELETE), _("Delete Line"));
	transformMenu->Append(SciCommandToMenuId(wxSTC_CMD_LINETRANSPOSE), _("Transpose Line"));
	transformMenu->Append(SciCommandToMenuId(wxSTC_CMD_LINECOPY), _("Copy Line"));
	transformMenu->Append(SciCommandToMenuId(wxSTC_CMD_LOWERCASE), _("Convert To Lower Case"));
	transformMenu->Append(SciCommandToMenuId(wxSTC_CMD_UPPERCASE), _("Convert To Upper Case"));
	transformMenu->Append(SciCommandToMenuId(wxSTC_CMD_DELWORDLEFT), _("Delete to start of Word"));
	transformMenu->Append(SciCommandToMenuId(wxSTC_CMD_DELWORDRIGHT), _("Delete to end of Word"));
	transformMenu->Append(SciCommandToMenuId(wxSTC_CMD_DELLINELEFT), _("Delete Line up to cursor"));
	transformMenu->Append(SciCommandToMenuId(wxSTC_CMD_DELLINERIGHT), _("Delete Line after cursor"));
	transformMenu->Append(SciCommandToMenuId(2469), _("Duplicate selection"));
	transformMenu->Append(SciCommandToMenuId(wxSTC_CMD_MOVESELECTEDLINESUP), _("Move selected lines up"));
	transformMenu->Append(SciCommandToMenuId(wxSTC_CMD_MOVESELECTEDLINESDOWN), _("Move selected lines down"));

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
	selectionMenu->Append(SciCommandToMenuId(wxSTC_CMD_PARADOWNEXTEND), _("Expand selection to the end of paragraph")); //\tCTRL+SHIFT+]
	selectionMenu->Append(SciCommandToMenuId(wxSTC_CMD_PARAUPEXTEND), _("Expand selection to the beginning of paragraph")); // \tCTRL+SHIFT+[
	selectionMenu->Append(SciCommandToMenuId(wxSTC_CMD_WORDLEFTEXTEND), _("Expand selection to the beginning of previous word\tCTRL+SHIFT+LEFT"));
	selectionMenu->Append(SciCommandToMenuId(wxSTC_CMD_WORDRIGHTEXTEND), _("Expand selection to the beginning of next word\tCTRL+SHIFT+RIGHT"));
	selectionMenu->Append(SciCommandToMenuId(wxSTC_CMD_WORDLEFTENDEXTEND), _("Expand selection to the end of previous word"));
	selectionMenu->Append(SciCommandToMenuId(wxSTC_CMD_WORDRIGHTENDEXTEND), _("Expand selection to the end of next word"));
	selectionMenu->Append(SciCommandToMenuId(wxSTC_CMD_WORDPARTLEFTEXTEND), _("Expand selection to the previous word segment")); // \tCTRL+SHIFT+/
	selectionMenu->Append(SciCommandToMenuId(wxSTC_CMD_WORDPARTRIGHTEXTEND), _("Expand selection to the end of next word segment")); // \tCTRL+SHIFT+backslash
	
	//?? not sure what these do
	//SCI_WORDLEFTEND
	//SCI_WORDRIGHTEND
	wxMenu* caretMenu = new wxMenu();
	caretMenu->Append(SciCommandToMenuId(wxSTC_CMD_PARADOWN), _("Move to next paragraph")); // \tCTRL+]
	caretMenu->Append(SciCommandToMenuId(wxSTC_CMD_PARAUP), _("Move to previous paragraph")); // \tCTRL+[
	caretMenu->Append(SciCommandToMenuId(wxSTC_CMD_WORDLEFT), _("Move to previous word\tCTRL+LEFT"));
	caretMenu->Append(SciCommandToMenuId(wxSTC_CMD_WORDRIGHT), _("Move to next word\tCTRL+RIGHT"));
	caretMenu->Append(SciCommandToMenuId(wxSTC_CMD_WORDPARTLEFT), _("Move to previous word segment")); // \tCTRL+/
	caretMenu->Append(SciCommandToMenuId(wxSTC_CMD_WORDPARTRIGHT), _("Move to next word segment")); // \tCTRL+backslash
	
	menu->Append(ID_SUBMENU_TRANSFORM, _("Transform text"), transformMenu);
	menu->Append(ID_SUBMENU_SELECTION, _("Selection"), selectionMenu);
	menu->Append(ID_SUBMENU_CARET, _("Move caret"), caretMenu);
	
	transformMenu->Enable(SciCommandToMenuId(wxSTC_CMD_LOWERCASE), isTextSelected);
	transformMenu->Enable(SciCommandToMenuId(wxSTC_CMD_UPPERCASE), isTextSelected);
}

static void AssignKeyCommands(wxStyledTextCtrl* ctrl, const std::vector<t4p::EditorKeyboardCommandClass>& keyboardCommands) {
	for (size_t i = 0; i < keyboardCommands.size(); ++i) {
		t4p::EditorKeyboardCommandClass cmd = keyboardCommands[i];
		if (cmd.CmdId > 0 && cmd.IsOk()) {
			ctrl->CmdKeyAssign(cmd.KeyCode, cmd.Modifiers, cmd.CmdId);
		}
	}
}

t4p::EditorBehaviorFeatureClass::EditorBehaviorFeatureClass(t4p::AppClass& app)
: FeatureClass(app)
, KeyboardCommands() {
	
}

void t4p::EditorBehaviorFeatureClass::LoadPreferences(wxConfigBase* config) {
	SetupCommands(KeyboardCommands);

	// now read them from the config
	for (size_t i = 0; i < KeyboardCommands.size(); ++i) {
		wxString key = wxString::Format(wxT("KeyboardCommands/Cmd-%d"), KeyboardCommands[i].CmdId);
		wxString storedShortcut;
		if (config->Read(key, &storedShortcut) && !storedShortcut.empty()) {
			KeyboardCommandFromString(KeyboardCommands[i], storedShortcut);
		}
	}
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

void t4p::EditorBehaviorFeatureClass::AddEditMenuItems(wxMenu* editMenu) {
	wxMenu* subMenu = new wxMenu();
	subMenu->Append(t4p::MENU_BEHAVIOR + 7, _("Unix (LF)"), _("Convert Line ending to UNIX"));
	subMenu->Append(t4p::MENU_BEHAVIOR + 8, _("Windows (CRLF)"), _("Convert Line ending to Windows"));
	subMenu->Append(t4p::MENU_BEHAVIOR + 9, _("Max (CR)"), _("Convert Line ending to MAC"));
	
	editMenu->Append(wxID_ANY, _("Convert Line Endings To"), subMenu);
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
	for (size_t i = 0; i < KeyboardCommands.size(); ++i) {
		int cmdId = KeyboardCommands[i].CmdId;
		int menuId = SciCommandToMenuId(cmdId);
		wxMenuItem* menuItem = menu->FindItem(menuId);
		if (menuItem) {
			wxString label = 
				menuItem->GetItemLabelText()
				+ wxT("\t") 
				+ KeyboardCommandToString(KeyboardCommands[i]);
			menuItem->SetItemLabel(label);
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

	// write the shortcuts to the config
	wxConfigBase* config = wxConfig::Get();
	for (size_t i = 0; i < KeyboardCommands.size(); ++i) {
		wxString key = wxString::Format(wxT("KeyboardCommands/Cmd-%d"), KeyboardCommands[i].CmdId);
		wxString storedShortcut = KeyboardCommandToString(KeyboardCommands[i]);
		config->Write(key, storedShortcut);
	}
}

void t4p::EditorBehaviorFeatureClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
	wxKeyBinder* keyBinder = App.Preferences.KeyProfiles.GetSelProfile();

	t4p::EditorBehaviorPanelClass* panel = new t4p::EditorBehaviorPanelClass(parent, 
		App.Preferences.CodeControlOptions);
	t4p::EditorCommandPanelClass* cmdPanel = new t4p::EditorCommandPanelClass(parent,
		wxID_ANY, KeyboardCommands, keyBinder);
	parent->AddPage(panel, _("Editor Behavior"));
	parent->AddPage(cmdPanel, _("Editor Shortcuts"));
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
	
	AssignKeyCommands(codeCtrl, KeyboardCommands);
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

	// this method handles all scintilla commands
	// use the menu item ID is scintilla command to execute
	int menuId = event.GetId();
	int cmdId = MenuIdToSciCommand(menuId);
	ctrl->CmdKeyExecute(cmdId);
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
		itemShortcut.SetText(KeyboardCommandToString(EditedCommands[i]));
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
		KeyboardCommandFromString(EditedCommands[row], shortcut);
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
		if (KeyboardCommandToString(Commands[i]) == newShortcut) {
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


