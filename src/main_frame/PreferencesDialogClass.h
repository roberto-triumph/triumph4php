/**
 * @copyright  2009-2011 Roberto Perpuly
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
#ifndef SRC_MAIN_FRAME_PREFERENCESDIALOGCLASS_H_
#define SRC_MAIN_FRAME_PREFERENCESDIALOGCLASS_H_

/**
@file
Subclass of PreferencesDialogGeneratedClass, which is generated by wxFormBuilder.
*/
#include <main_frame/PreferencesClass.h>
#include <wx/propdlg.h>

namespace t4p {
// forward declaration
class GlobalsClass;

class KeyboardShortcutsPanelClass : public wxKeyConfigPanel {
	public:
	KeyboardShortcutsPanelClass(wxWindow* parent, int id, wxPoint position = wxDefaultPosition,
		wxSize size = wxDefaultSize, long style = 0);

	void AddDynamicCmds(const std::vector<t4p::DynamicCmdClass>& cmds);
};

/** Implementing PreferencesDialogGeneratedClass */
class PreferencesDialogClass : public wxPropertySheetDialog {
	public:
	/**
	 * @param parent the parent window
	 * @param globals the GlobalsClass, to check to see if the user modified any globals settings
	 * @param preferences the Preferences object to edit
	 * @param settingsDir wxFileName the location where settings are stored
	 * @param changedSettingsDir bool [out] if TRUE the user changed the location of the settings directory
	 *        the caller should save the settings to the new directory
	 * @param needsRetags bool [out] if TRUE the active projects should be retagged because
	 *        a settings that affects PHP parsing was changed
	 */
	PreferencesDialogClass(wxWindow* parent,
		t4p::GlobalsClass& globals,
		t4p::PreferencesClass& preferences,
		wxFileName& settingsDir, bool& changedSettingsDir, bool& needsRetag);

	/**
	 * Call this after all property sheets have been added but before they are shown. This method
	 * will transfer all variables to the windows.
	 */
	void Prepare();

	/**
	 * Saves the user's changes.
	 */
	void OnOkButton(wxCommandEvent& event);

	private:
	/**
	 * The dialog to let the user create shortcut keys
	 *
	 * @var KeyboardShortcutsPanelClass
	 */
	KeyboardShortcutsPanelClass* KeyboardShortcutsPanel;

	/**
	 * The preferences object to show & manipulate
	 *
	 * @var PreferencesClass
	 */
	PreferencesClass& Preferences;

	/**
	 * The Globals object to check for modifications
	 *
	 * @var GlobalsClass
	 */
	GlobalsClass& Globals;

	/**
	 * The initial value of SettingsDir
	 */
	wxFileName OldSettingsDir;

	/**
	 * The [possibly[ updated value of SettingsDir
	 */
	wxFileName& SettingsDir;

	/**
	 * if TRUE the user changed the location of the settings directory
	 * the caller should save the settings to the new directory
	 */
	bool& ChangedSettingsDir;

	/**
	 * if TRUE the active projects should be retagged because
	 * a settings that affects PHP parsing was changed
	 */
	bool& NeedsRetag;

	DECLARE_EVENT_TABLE()
};
}
#endif  // SRC_MAIN_FRAME_PREFERENCESDIALOGCLASS_H_
