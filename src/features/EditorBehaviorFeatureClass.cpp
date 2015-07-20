/**
 * @copyright  2014 Roberto Perpuly
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
#include <features/EditorBehaviorFeatureClass.h>
#include <globals/Assets.h>
#include <main_frame/PreferencesClass.h>
#include <Triumph.h>
#include <wx/valgen.h>
#include <wx/stc/stc.h>

static int SciCommands[][2] = {
{ t4p::MENU_BEHAVIOR + 20, wxSTC_CMD_LINECUT },
{ t4p::MENU_BEHAVIOR + 21, wxSTC_CMD_LINEDUPLICATE },
{ t4p::MENU_BEHAVIOR + 22, wxSTC_CMD_LINEDELETE },
{ t4p::MENU_BEHAVIOR + 23, wxSTC_CMD_LINETRANSPOSE },
{ t4p::MENU_BEHAVIOR + 24, wxSTC_CMD_LINECOPY },
{ t4p::MENU_BEHAVIOR + 25, wxSTC_CMD_LOWERCASE },
{ t4p::MENU_BEHAVIOR + 26, wxSTC_CMD_UPPERCASE },
{ t4p::MENU_BEHAVIOR + 27, wxSTC_CMD_DELWORDLEFT },
{ t4p::MENU_BEHAVIOR + 28, wxSTC_CMD_DELWORDRIGHT },
{ t4p::MENU_BEHAVIOR + 29, wxSTC_CMD_DELLINELEFT },
{ t4p::MENU_BEHAVIOR + 30, wxSTC_CMD_DELLINERIGHT },
{ t4p::MENU_BEHAVIOR + 31, 2469 },  // there is no wxSTC_CMD_SELECTIONDUPLICATE
{ t4p::MENU_BEHAVIOR + 32, wxSTC_CMD_MOVESELECTEDLINESUP },
{ t4p::MENU_BEHAVIOR + 33, wxSTC_CMD_MOVESELECTEDLINESDOWN },
{ t4p::MENU_BEHAVIOR + 34, wxSTC_CMD_PARADOWNEXTEND },
{ t4p::MENU_BEHAVIOR + 35, wxSTC_CMD_PARAUPEXTEND },
{ t4p::MENU_BEHAVIOR + 36, wxSTC_CMD_WORDLEFTEXTEND },
{ t4p::MENU_BEHAVIOR + 37, wxSTC_CMD_WORDRIGHTEXTEND },
{ t4p::MENU_BEHAVIOR + 38, wxSTC_CMD_WORDLEFTENDEXTEND },
{ t4p::MENU_BEHAVIOR + 39, wxSTC_CMD_WORDRIGHTENDEXTEND },
{ t4p::MENU_BEHAVIOR + 40, wxSTC_CMD_WORDPARTLEFTEXTEND },
{ t4p::MENU_BEHAVIOR + 41, wxSTC_CMD_WORDPARTRIGHTEXTEND },
{ t4p::MENU_BEHAVIOR + 42, wxSTC_CMD_PARADOWN },
{ t4p::MENU_BEHAVIOR + 43, wxSTC_CMD_PARAUP },
{ t4p::MENU_BEHAVIOR + 44, wxSTC_CMD_WORDLEFT },
{ t4p::MENU_BEHAVIOR + 45, wxSTC_CMD_WORDRIGHT },
{ t4p::MENU_BEHAVIOR + 46, wxSTC_CMD_WORDPARTLEFT },
{ t4p::MENU_BEHAVIOR + 47, wxSTC_CMD_WORDPARTRIGHT },
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

void t4p::EditorKeyboardCommandClass::InitFromString(const wxString& shortcut) {
	int wxKeyCode = 0;
	int wxModifiers = 0;
	if (!shortcut.empty()) {
		t4p::ShortcutStringToKeyCode(shortcut, wxModifiers, wxKeyCode);

		// turn wx keys, modifiers into stc counterparts
		KeyCode = WxKeyCodeToSciKeyCode(wxKeyCode);
		Modifiers = WxModifiersToSciModifiers(wxModifiers);
	} else {
		// empty string == no shortcut
		KeyCode = 0;
		Modifiers = 0;
	}
}

wxString t4p::EditorKeyboardCommandClass::ToString() const {
	// turn stc keys, modifiers into wx counterparts
	// to feed into the keybinder function
	int wxKeyCode = SciKeyCodeToWxKeyCode(KeyCode);
	int wxModifiers = SciModifiersToWxModifiers(Modifiers);
	if (wxKeyCode > 0 && wxModifiers > 0) {
		return t4p::KeyCodeToShortcutString(wxModifiers, wxKeyCode);
	}
	return wxT("");
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
			KeyboardCommands[i].InitFromString(storedShortcut);
		}
	}
}

void t4p::EditorBehaviorFeatureClass::ToggleWordWrap() {
	App.Preferences.CodeControlOptions.EnableWordWrap = !App.Preferences.CodeControlOptions.EnableWordWrap;

	wxConfigBase* config = wxConfigBase::Get(false);
	App.Preferences.CodeControlOptions.Save(config);
	config->Flush();
}

void t4p::EditorBehaviorFeatureClass::ToggleIndentationGuides() {
	App.Preferences.CodeControlOptions.EnableIndentationGuides = !App.Preferences.CodeControlOptions.EnableIndentationGuides;

	wxConfigBase* config = wxConfigBase::Get(false);
	App.Preferences.CodeControlOptions.Save(config);
	config->Flush();
}

void t4p::EditorBehaviorFeatureClass::ToggleWhitespace() {
	App.Preferences.CodeControlOptions.ShowWhitespace = !App.Preferences.CodeControlOptions.ShowWhitespace;

	wxConfigBase* config = wxConfigBase::Get(false);
	App.Preferences.CodeControlOptions.Save(config);
	config->Flush();
}

void t4p::EditorBehaviorFeatureClass::ZoomIn() {
	App.Preferences.CodeControlOptions.Zoom++;
	if (App.Preferences.CodeControlOptions.Zoom > 50) {
		App.Preferences.CodeControlOptions.Zoom = 50;
	}

	wxConfigBase* config = wxConfigBase::Get(false);
	App.Preferences.CodeControlOptions.Save(config);
	config->Flush();
}

void t4p::EditorBehaviorFeatureClass::ZoomOut() {
	App.Preferences.CodeControlOptions.Zoom--;
	if (App.Preferences.CodeControlOptions.Zoom < -35) {
		App.Preferences.CodeControlOptions.Zoom = -35;
	}

	wxConfigBase* config = wxConfigBase::Get(false);
	App.Preferences.CodeControlOptions.Save(config);
	config->Flush();
}

void t4p::EditorBehaviorFeatureClass::ZoomReset() {
	App.Preferences.CodeControlOptions.Zoom = 0;

	wxConfigBase* config = wxConfigBase::Get(false);
	App.Preferences.CodeControlOptions.Save(config);
	config->Flush();
}

void t4p::EditorBehaviorFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {
	// write the shortcuts to the config
	wxConfigBase* config = wxConfig::Get();
	for (size_t i = 0; i < KeyboardCommands.size(); ++i) {
		wxString key = wxString::Format(wxT("KeyboardCommands/Cmd-%d"), KeyboardCommands[i].CmdId);
		wxString storedShortcut = KeyboardCommands[i].ToString();
		config->Write(key, storedShortcut);
	}
}

int t4p::EditorBehaviorFeatureClass::SciCommandToMenuId(int cmdId) {
	for (int i = 0; SciCommands[i][0] > 0; ++i) {
		if (SciCommands[i][1] == cmdId) {
			return SciCommands[i][0];
		}
	}
	return 0;
}

int t4p::EditorBehaviorFeatureClass::MenuIdToSciCommand(int menuId) {
	for (int i = 0; SciCommands[i][0] > 0; ++i) {
		if (SciCommands[i][0] == menuId) {
			return SciCommands[i][1];
		}
	}
	return 0;
}

BEGIN_EVENT_TABLE(t4p::EditorBehaviorFeatureClass, t4p::FeatureClass)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::EditorBehaviorFeatureClass::OnPreferencesSaved)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_EXTERNALLY_UPDATED, t4p::EditorBehaviorFeatureClass::OnPreferencesSaved)
END_EVENT_TABLE()


