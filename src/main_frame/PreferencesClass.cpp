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
#include <main_frame/PreferencesClass.h>
#include <code_control/CodeControlStyles.h>
#include <globals/Errors.h>
#include <globals/Assets.h>
#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/menuutils.h>
#include <wx/stdpaths.h>
#include <wx/wfstream.h>
#include <wx/frame.h>
#include <wx/tokenzr.h>
#include <wx/wx.h>

// use our own function to convert string to key codes
// because the wxKeyBinder function does not handle
// converting  LEFT, RIGHT arrow keys to the correct key code
static int StringToKeyCode(const wxString& str) {
	wxString normalized(str);
	normalized.MakeUpper();

	// function keys
	if (normalized.StartsWith(wxT("F")) && normalized.Len() > 1) {
		long n;
		normalized.Right(normalized.Len() - 1).ToLong(&n);
		return WXK_F1 + (n - 1);
	}

	// special keys
	if (normalized == wxT("BACK")) {
		return WXK_BACK;
	}
	if (normalized == wxT("ENTER")) {
		return WXK_RETURN;
	}
	if (normalized == wxT("RETURN")) {
		return WXK_RETURN;
	}
	if (normalized == wxT("TAB")) {
		return WXK_TAB;
	}
	if (normalized == wxT("ESCAPE")) {
		return WXK_ESCAPE;
	}
	if (normalized == wxT("SPACE")) {
		return WXK_SPACE;
	}
	if (normalized == wxT("DELETE")) {
		return WXK_DELETE;
	}
	if (normalized == wxT("*")) {
		return WXK_MULTIPLY;
	}
	if (normalized == wxT("+")) {
		return WXK_ADD;
	}
	if (normalized == wxT("SEPARATOR")) {
		return WXK_SEPARATOR;
	}
	if (normalized == wxT("-")) {
		return WXK_SUBTRACT;
	}
	if (normalized == wxT(".")) {
		return WXK_DECIMAL;
	}
	if (normalized == wxT("/")) {
		return WXK_DIVIDE;
	}
	if (normalized == wxT("PAGEUP")) {
		return WXK_PAGEUP;
	}
	if (normalized == wxT("PAGEDOWN")) {
		return WXK_PAGEDOWN;
	}
	if (normalized == wxT("LEFT")) {
		return WXK_LEFT;
	}
	if (normalized == wxT("UP")) {
		return WXK_UP;
	}
	if (normalized == wxT("RIGHT")) {
		return WXK_RIGHT;
	}
	if (normalized == wxT("DOWN")) {
		return WXK_DOWN;
	}
	if (normalized == wxT("SELECT")) {
		return WXK_SELECT;
	}
	if (normalized == wxT("PRINT")) {
		return WXK_PRINT;
	}
	if (normalized == wxT("EXECUTE")) {
		return WXK_EXECUTE;
	}
	if (normalized == wxT("SNAPSHOT")) {
		return WXK_SNAPSHOT;
	}
	if (normalized == wxT("INSERT")) {
		return WXK_INSERT;
	}
	if (normalized == wxT("HELP")) {
		return WXK_HELP;
	}
	if (normalized == wxT("CANCEL")) {
		return WXK_CANCEL;
	}
	if (normalized == wxT("MENU")) {
		return WXK_MENU;
	}
	if (normalized == wxT("CAPITAL")) {
		return WXK_CAPITAL;
	}
	if (normalized == wxT("END")) {
		return WXK_END;
	}
	if (normalized == wxT("HOME")) {
		return WXK_HOME;
	}

	// it should be an ASCII key...
	if (!normalized.empty()) {
		return static_cast<int>(normalized.GetChar(0));
	}
	return 0;
}

static int StringToModifiers(const wxString& str) {
	int mod = 0;

    // case-insensitive, ctrl == CTRL
    wxString keyModifier(str);
	keyModifier.MakeUpper();

    if (keyModifier.Contains(wxT("ALT"))) {
        mod |= wxACCEL_ALT;
	}
    if (keyModifier.Contains(wxT("CTRL"))) {
        mod |= wxACCEL_CTRL;
	}
    if (keyModifier.Contains(wxT("SHIFT"))) {
        mod |= wxACCEL_SHIFT;
	}
    return mod;
}

void t4p::ShortcutStringToKeyCode(const wxString& str, int& modifiers, int& keyCode) {
	wxString strMods = str;
	wxString strKey = str.AfterLast(wxT('+'));
	modifiers  = StringToModifiers(strMods);
	keyCode = StringToKeyCode(strKey);
}

wxString t4p::KeyCodeToShortcutString(int modifiers, int keyCode) {
	// there is a bug with wxKeyBind::KeyCodeToString
	// it does not handle non-alphanumerics like [ or /
	// we check here
	wxString keyStr = wxKeyBind::KeyCodeToString(keyCode);
	if (!wxIsalnum(keyCode) && keyCode >= 32 && keyCode < 127) {
		keyStr << (wxChar)keyCode;
	}
	return wxKeyBind::KeyModifierToString(modifiers) + keyStr;
}

/* this function gracefully handles non-existant menus (bindings that are stored in the config but are no longer
 * commands in the app).  This would happen, for example, when functionality is removed.
 (
 * @param dynamicCmds the commands to modify.  This vector should not be empty; it should contain the default bindings
 *        after this function is called, the bindings will have been updated based on the config file. This function
 *        will only update the bindings, it wont add new commands.
 * @param config the stored config settings
 * @param configKey the name of the config group to load.
 */


static int LoadKeyProfileBindings(std::vector<t4p::DynamicCmdClass>& dynamicCmds, wxConfigBase* config, const wxString& configKey) {
	int total = 0;
	config->SetPath(configKey);
	wxString configName;
	long configIterator;

	// shortcuts will be stored with a prefix of "shortcut-XXX"
	// where XXX is the command identifer, which should be unique.
	// loop through all stored shortcuts, remove the prefix,
	// then update the binding by locating the command in the dynamicCmds
	// [by looping through the already created commands]
	//
	// Example: config file will look like this
	//
	//  ... other top-level confg items ...
	//  ProfileSelected = 1
	//  [keyprof_0]
	//  ProfileName = "Triumph shortcuts"
	//  ProfileDescription = "Triumph shortcuts"
	//  Shortcut-File-Open = "CTRL+O"
	//  Shortcut-File-New = "CTRL+N"
	//  ... other commands ...
	//  [keyprof_1]
	//  ProfileName = "Triumph shortcuts"
	//  ProfileDescription = "Triumph shortcuts"
	//  Shortcut-File-Open = "CTRL+ALT+O"
	//  Shortcut-File-New = "CTRL+SHIFT+N"
	//  ... other commands ...
	//
	// A shortcut can have multiple bindings; separated by '|'
	//  Shortcut-File-New = "CTRL+SHIFT+N|CTRL+ALT_N"
	//
	bool next = config->GetFirstEntry(configName, configIterator);
	while (next) {
		if (configName.StartsWith(wxT("Shortcut-"))) {
			wxString shortcutIdentifier = configName.Mid(9); // 9 = length of prefix
			for (size_t i = 0; i < dynamicCmds.size(); ++i) {
				if (shortcutIdentifier == dynamicCmds[i].GetIdentifier()) {
					wxString value = config->Read(configName);
					if (!value.IsEmpty()) {
						dynamicCmds[i].ClearShortcuts();
						wxStringTokenizer tok(value, wxT("|"));
						while (tok.HasMoreTokens()) {
							dynamicCmds[i].AddShortcut(tok.GetNextToken());
						}
						total++;
					}
					break;
				}
			}
		}
		next = config->GetNextEntry(configName, configIterator);
	}
	return total;
}

/* this function was ripped off from wxKeyProfile::Load() but this function
 * gracefully handles non-existant menus
 * @param dynamicCmds the commands to modify.  This vector should not be empty; it should contain the default bindings
 *        after this function is called, the bindings will have been updated based on the config file. This function
 *        will only update the bindings, it wont add new commands. *
 * @param profile will update the profile name and description only. The bindings will be handled by the vector dynamicCmds
 * @param config the stored config settings
 * @param configKey the name of the config group to load.
 */
static bool LoadKeyProfile(std::vector<t4p::DynamicCmdClass>& dynamicCmds, wxKeyProfile& profile, wxConfigBase* config, const wxString& configKey) {
	bool ret = false;
	config->SetPath(configKey);        // enter into this group
    wxString name;
    wxString desc;

    // do we have valid DESC & NAME entries ?
    if (config->HasEntry(wxT("ProfileDescription")) && config->HasEntry(wxT("ProfileName"))) {
        if (!config->Read(wxT("ProfileDescription"), &desc))
            return FALSE;
        if (!config->Read(wxT("ProfileName"), &name))
            return FALSE;

        // check for valid name & desc
        if (name.IsEmpty())
            return FALSE;

        profile.SetName(name);
        profile.SetDesc(desc);

        // load from current path (we cannot use '.')
        ret = LoadKeyProfileBindings(dynamicCmds, config, wxT("../") + configKey) > 0;
	}
	return ret;
}

static bool LoadKeyProfileArray(std::vector<t4p::DynamicCmdClass>& defaultShortcuts, wxKeyProfileArray& profiles, wxConfigBase* config, const wxString& configKey) {
    config->SetPath(configKey);
	int profileSelected = 0;

	// status will hold FALSE when there is a problem reading the config
    bool status = config->Read(wxT("ProfileSelected"), &profileSelected, 0);
	long configIterator;
	wxString configName;

	// multiple shorcut profiles can be found in the config. Each saved profile is
	// stored with a prefix
    bool next = config->GetFirstGroup(configName, configIterator);
    while (next) {
        if (configName.StartsWith(wxKEYPROFILE_CONFIG_PREFIX)) {
			// wxKeyProfileArray will cleanup this pointer in wxKeyProfileArray::Cleanup()
			wxKeyProfile* profile = new wxKeyProfile();
			std::vector<t4p::DynamicCmdClass> cmds(defaultShortcuts);
			status = LoadKeyProfile(cmds, *profile, config, configName);
			if (status) {
				for (size_t i = 0; i < cmds.size(); ++i) {
					profile->AddCmd(cmds[i].CloneCommand());
				}
				profiles.Add(profile);
			} else {
				delete profile;
				break;
			}
        }

        // set the path again (it could have been changed...)
        config->SetPath(configKey);
        next = config->GetNextGroup(configName, configIterator);
	}
	if (profileSelected < profiles.GetCount()) {
		profiles.SetSelProfile(profileSelected);
	} else if (!profiles.IsEmpty()) {
		profiles.SetSelProfile(0);
	} else if (profiles.IsEmpty()) {
		// no profiles were stored; use the defaults
		wxKeyProfile* profile = new wxKeyProfile(wxT("Triumph keyboard shortcuts"), wxT("Triumph keyboard shortcuts"));
		for (size_t i = 0; i < defaultShortcuts.size(); ++i) {
			profile->AddCmd(defaultShortcuts[i].CloneCommand());
		}
		profiles.Add(profile);
		profiles.SetSelProfile(0);
	}
    return status;
}

/*
 * @param defaultShortcuts when saving commands, we will use the Identifier of the command and NOT the ID, since ID
 *        can change between application runs
 * @param profiles contains the actual bindings.  Since we use the default wxKeyConfigPanel; when the user updates shortcuts
 *        wxKeyProfileArray is actually the data stucture that is modified
 * @param config the shortcuts will be stored to the given config
 * @param configKey the shortcuts will be stored to the given config group
 */
static bool SaveKeyProfileArray(std::vector<t4p::DynamicCmdClass>& defaultShortcuts, wxKeyProfileArray& profiles, wxConfigBase* config, const wxString& configKey) {
	// Example config file will look like this
	//
	//  ... other top-level confg items ...
	//  ProfileSelected = 1
	//  [keyprof_0]
	//  ProfileName = "Triumph shortcuts"
	//  ProfileDescription = "Triumph shortcuts"
	//  Shortcut-File-Open = "CTRL+O"
	//  Shortcut-File-New = "CTRL+N"
	//  ... other commands ...
	//  [keyprof_1]
	//  ProfileName = "Triumph shortcuts"
	//  ProfileDescription = "Triumph shortcuts"
	//  Shortcut-File-Open = "CTRL+ALT+O"
	//  Shortcut-File-New = "CTRL+SHIFT+N"
	//  ... other commands ...
	//
	// A shortcut can have multiple bindings; separated by '|'
	//  Shortcut-File-New = "CTRL+SHIFT+N|CTRL+ALT_N"
	//
	config->SetPath(configKey);
	bool success = false;
	success = config->Write(wxT("ProfileSelected"), profiles.GetSelProfileIdx());
	for (int i = 0; i < profiles.GetCount() && success; i++) {
		const wxKeyProfile* profile = profiles.Item(i);
		wxString groupName = wxKEYPROFILE_CONFIG_PREFIX + wxString::Format(wxT("%d"), i);
		config->SetPath(groupName);
		success = config->Write(wxT("ProfileName"), profile->GetName())
			&& config->Write(wxT("ProfileDescription"), profile->GetDesc());

		// this loop writes all of the shortcuts for a single profile to the configs
		// use the defaultShortcuts to get the unique identifier, but the actual bindings
		// are stored in the wxKeyProfile
		for (size_t j = 0; j < defaultShortcuts.size() && success; j++) {
			wxArrayString shortcutList = profile->GetShortcutsList(defaultShortcuts[j].GetId());
			wxString shortcutConfigValue;
			for (size_t k = 0; k < shortcutList.Count(); k++) {
				shortcutConfigValue += shortcutList[k];
				if (k < (shortcutList.Count() - 1)) {
					shortcutConfigValue += wxT("|");
				}
			}
			wxString shortcutConfigKey = wxT("Shortcut-") + defaultShortcuts[j].GetIdentifier();
			success = config->Write(shortcutConfigKey, shortcutConfigValue);
		}

		// so that on the next iteration the next profile will be a sibling
		config->SetPath(wxT("../"));
	}
	return success;
}

t4p::DynamicCmdClass::DynamicCmdClass(wxMenuItem* item, const wxString& identifier)
	: MenuCmd(item, identifier, item->GetHelp())
	, Identifier(identifier) {
	// check for the default shortcuts
	wxAcceleratorEntry *acc = item->GetAccel();
	if (acc) {
		// this menuitem has an associated accelerator... add an entry
		// to the array of bindings for the relative command...
		MenuCmd.AddShortcut(acc->GetFlags(), acc->GetKeyCode());
		delete acc;
	}
}

void t4p::DynamicCmdClass::AddShortcut(const wxString& key) {
	int mods = 0;
	int keyCode = 0;
	t4p::ShortcutStringToKeyCode(key, mods, keyCode);
	MenuCmd.AddShortcut(mods, keyCode);
}

wxCmd* t4p::DynamicCmdClass::CloneCommand() const {
	return MenuCmd.Clone();
}

void t4p::DynamicCmdClass::ClearShortcuts() {
	MenuCmd.RemoveAllShortcuts();
}

int t4p::DynamicCmdClass::GetId() const {
	return MenuCmd.GetId();
}

wxString t4p::DynamicCmdClass::GetIdentifier() const {
	return Identifier;
}

t4p::PreferencesClass::PreferencesClass()
	: CodeControlOptions()
	, KeyProfiles()
	, ApplicationFont()
	, CheckForUpdates(true) {
}

void t4p::PreferencesClass::Init() {
	wxPlatformInfo info;

	ApplicationFont.SetFamily(wxFONTFAMILY_DEFAULT);

	// ATTN: on linux, default fonts are too big
	if (info.GetOperatingSystemId() == wxOS_UNIX_LINUX) {
		ApplicationFont.SetPointSize(8);
	} else if (info.GetOperatingSystemId() == wxOS_WINDOWS_NT) {
		ApplicationFont.SetPointSize(8);
	} else if (info.GetOperatingSystemId() == wxOS_MAC_OSX_DARWIN) {
		ApplicationFont.SetPointSize(12);
	}
	t4p::CodeControlStylesInit(CodeControlOptions);
}

t4p::PreferencesClass::~PreferencesClass() {
	//KeyProfileArray destructor deletes the pointers
}

void t4p::PreferencesClass::ClearAllShortcuts() {
	KeyProfiles.Cleanup();
	DefaultKeyboardShortcutCmds.clear();
}

void t4p::PreferencesClass::Load(wxConfigBase* config, wxFrame* frame) {
	KeyProfiles.Cleanup();
	CodeControlOptions.Load(config);

	wxString applicationFontInfo = config->Read(wxT("ApplicationFont"));
	if (!applicationFontInfo.IsEmpty()) {
		ApplicationFont.SetNativeFontInfo(applicationFontInfo);
	}
	config->Read(wxT("CheckForUpdates"), &CheckForUpdates);

	int totalCmdCount = 0;

	// call out own key profile load function
	// out function will gracefully handle deleted menus ; the normal keybinder code
	// throws asserts
	wxMenuCmd::Register(frame->GetMenuBar());
	LoadKeyProfileArray(DefaultKeyboardShortcutCmds, KeyProfiles, config, wxT(""));
	for (int i = 0; i < KeyProfiles.GetCount(); ++i) {
		totalCmdCount += KeyProfiles.Item(i)->GetCmdCount();
	}

	// keybinder sets the config path, must reset it back to normal
	config->SetPath(wxT("/"));
	EnableSelectedProfile(frame);
}

void t4p::PreferencesClass::EnableSelectedProfile(wxWindow* window) {
	KeyProfiles.DetachAll();
	KeyProfiles.GetSelProfile()->Enable(true);

	// not attaching recursively for now; since the child windows (the code editor widgets)
	// are temporary we cannot attach them to the profile otherwise when a code editor
	// is deleted the profile would have a dangling pointer.
	// however this means that the given window must be the frame
	KeyProfiles.GetSelProfile()->Attach(window);
}

void t4p::PreferencesClass::Save() {
	wxConfigBase* config = wxConfigBase::Get();
	CodeControlOptions.Save(config);
	config->Write(wxT("ApplicationFont"), ApplicationFont.GetNativeFontInfoDesc());
	config->Write(wxT("CheckForUpdates"), CheckForUpdates);
	SaveKeyProfileArray(DefaultKeyboardShortcutCmds, KeyProfiles, config, wxT(""));

	// keybinder sets the config path, must reset it back to normal
	config->SetPath(wxT("/"));
	config->Flush();
}

bool t4p::PreferencesClass::InitConfig() {
	wxStandardPaths paths = wxStandardPaths::Get();
	wxFileName configDir = t4p::ConfigDirAsset();
	if (configDir.DirExists()) {
		wxFileName configFileName(configDir.GetPath(), wxT("triumph4php.ini"));

		// this config will be automatically deleted by wxWidgets at the end
		wxFileConfig* config = new wxFileConfig(wxT("triumph4php"), wxEmptyString, configFileName.GetFullPath(), wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
		wxConfigBase::Set(config);
		return true;
	}

	// will not be able to write config file but the app still assumes that
	// wxConfigBase::Get() will be valid, so lets initialize a throw-away file
	wxFileName tempConfigFileName(t4p::TempDirAsset().GetPath(), wxT("triumph4php-temp.ini"));
	wxFileConfig* config = new wxFileConfig(wxT("triumph4php-temp"), wxEmptyString, tempConfigFileName.GetFullPath(), wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
	wxConfigBase::Set(config);
	return false;
}

void t4p::PreferencesClass::SetSettingsDir(const wxFileName& settingsDir) {
	// save the settings dir in the bootstrap file
	t4p::SetSettingsDirLocation(settingsDir);

	// delete the old config and set the global config object
	wxConfigBase* config = wxConfig::Get();
	delete config;

	// use the new config dir
	PreferencesClass::InitConfig();
}
