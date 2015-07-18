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
#include <main_frame/PreferencesDialogClass.h>
#include <main_frame/SettingsDirectoryPanelClass.h>
#include <globals/GlobalsClass.h>
#include <wx/bookctrl.h>
#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/keybinder.h>
#include <wx/wfstream.h>
#include <map>
#include <vector>

t4p::PreferencesDialogClass::PreferencesDialogClass(wxWindow* parent,
														  t4p::GlobalsClass& globals,
														  t4p::PreferencesClass& preferences,
														  wxFileName& settingsDir,
														  bool& changedSettingsDir, bool& needsRetag)
		: Preferences(preferences)
		, Globals(globals)
		, OldSettingsDir(settingsDir)
		, SettingsDir(settingsDir)
		, ChangedSettingsDir(changedSettingsDir)
		, NeedsRetag(needsRetag) {
	SetSheetStyle(wxPROPSHEET_TREEBOOK);
	Create(parent, wxID_ANY, _("Preferences"));
	CreateButtons(wxOK | wxCANCEL);

	wxBookCtrlBase* notebook = GetBookCtrl();

	// make it so that no other preference dialogs have to explictly call Transfer methods
	notebook->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
	KeyboardShortcutsPanel =  new KeyboardShortcutsPanelClass(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxKEYBINDER_USE_TREECTRL | wxKEYBINDER_SHOW_ADDREMOVE_PROFILE | wxKEYBINDER_ENABLE_PROFILE_EDITING);

	KeyboardShortcutsPanel->AddProfiles(Preferences.KeyProfiles);
	KeyboardShortcutsPanel->AddDynamicCmds(Preferences.DefaultKeyboardShortcutCmds);
	notebook->AddPage(KeyboardShortcutsPanel, _("Keyboard Shortcuts"));

	t4p::SettingsDirectoryPanelClass* settingsPanel =  new t4p::SettingsDirectoryPanelClass(
		notebook, wxID_ANY, settingsDir
	);
	notebook->AddPage(settingsPanel, _("Settings Directory"));
}

void t4p::PreferencesDialogClass::Prepare() {
	GetBookCtrl()->InitDialog();
	LayoutDialog();
}

void t4p::PreferencesDialogClass::OnOkButton(wxCommandEvent& event) {
	wxBookCtrlBase* book = GetBookCtrl();
	ChangedSettingsDir = false;
	NeedsRetag = false;
	pelet::Versions oldPhpVersion = Globals.Environment.Php.Version;
	wxString oldPhpExecutable = Globals.Environment.Php.PhpExecutablePath;
	wxString oldExtensions = Globals.FileTypes.GetAllSourceFileExtensionsString();
	if (Validate() && book->Validate() && TransferDataFromWindow() && book->TransferDataFromWindow()) {
		KeyboardShortcutsPanel->ApplyChanges();
		Preferences.KeyProfiles = KeyboardShortcutsPanel->GetProfiles();

		// need to figure out if the settings directory wac changed
		if (OldSettingsDir != SettingsDir) {
			ChangedSettingsDir = true;
		}
		wxString newExtensions = Globals.FileTypes.GetAllSourceFileExtensionsString();
		if (oldExtensions != newExtensions) {
			NeedsRetag = true;
		}

		// php executable affects populating "detected" tags
		wxString newPhpExecutable = Globals.Environment.Php.PhpExecutablePath;
		if (oldPhpExecutable != newPhpExecutable && Globals.Environment.Php.Installed) {
			NeedsRetag = true;
		}
		if (oldPhpVersion != Globals.Environment.Php.Version) {
			NeedsRetag = true;
		}
		EndModal(wxOK);
	}
}

t4p::KeyboardShortcutsPanelClass::KeyboardShortcutsPanelClass(wxWindow* parent, int id, wxPoint position,
																	wxSize size, long style)
	: wxKeyConfigPanel(parent, id, position, size, style) {
}

void t4p::KeyboardShortcutsPanelClass::AddDynamicCmds(const std::vector<t4p::DynamicCmdClass>& cmds) {

	// add the dynamic commands in the tree control; this code assumes that the command
	// identifier is delimited with dashes; and the identifier will always contain
	// at least one dash;
	// the code will use the first part of  the identifier as a tree 'level'
	std::map<wxString, std::vector<wxString> > items;
	for (size_t i = 0; i < cmds.size(); ++i) {
		wxString cmdIdentifier = cmds[i].GetIdentifier();
		wxString key;
		wxString item;
		int index = cmdIdentifier.Find(wxT('-'));
		if (index != wxNOT_FOUND) {
			key = cmdIdentifier.Mid(0, index);
			item = cmdIdentifier.Mid(index + 1);
		}
		if (!key.IsEmpty() && !item.IsEmpty()) {
			items[key].push_back(item);
		}
	}
	wxTreeItemId root = m_pCommandsTree->AddRoot(_("Keyboard Shortcuts"));

	// now we build the 'leaf' nodes
	for (std::map<wxString, std::vector<wxString> >::iterator it = items.begin(); it != items.end(); ++it) {
		wxString groupName = it->first;
		wxTreeItemId group = m_pCommandsTree->AppendItem(root, groupName);
		std::vector<wxString> itemList = it->second;
		for (size_t i = 0; i < itemList.size(); ++i) {
			wxString itemLabel = itemList[i];
			wxString identifier = groupName + wxT("-") + itemLabel;

			// find the command
			for (size_t c = 0; c < cmds.size(); ++c) {
				if (cmds[c].GetIdentifier() == identifier) {
					wxExTreeItemData *treedata = new wxExTreeItemData(cmds[c].GetId());
					m_pCommandsTree->AppendItem(group, itemLabel, -1, -1, treedata);
					break;
				}
			}
		}
	}
	m_pCommandsTree->Expand(root);
}

BEGIN_EVENT_TABLE(t4p::PreferencesDialogClass, wxDialog)
	EVT_BUTTON(wxID_OK, t4p::PreferencesDialogClass::OnOkButton)
END_EVENT_TABLE()
