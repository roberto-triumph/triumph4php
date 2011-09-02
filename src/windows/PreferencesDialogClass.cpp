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
#include <windows/PreferencesDialogClass.h>
#include <wx/bookctrl.h>
#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/keybinder.h>
#include <wx/stdpaths.h>
#include <wx/wfstream.h>

mvceditor::PreferencesDialogClass::PreferencesDialogClass(wxWindow* parent, mvceditor::PreferencesClass& preferences, wxMenuBar* menuBar)
		: wxPropertySheetDialog(parent, wxID_ANY, _("Preferences"))
		, Preferences(preferences) {
	CreateButtons(wxOK | wxCANCEL);
	wxBookCtrlBase* notebook = GetBookCtrl();
	
	// make it so that no other preference dialogs have to explictly call Transfer methods
	notebook->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
	EditorBehavior = new EditorBehaviorPanelClass(notebook, Preferences.CodeControlOptions);
	notebook->AddPage(EditorBehavior, _("Editor Behavior"));
	notebook->AddPage(new EditColorsPanelClass(notebook, Preferences.CodeControlOptions), _("Styles && Colors"));
	KeyConfigPanel =  new wxKeyConfigPanel(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
		wxKEYBINDER_USE_TREECTRL | wxKEYBINDER_SHOW_ADDREMOVE_PROFILE | wxKEYBINDER_ENABLE_PROFILE_EDITING);
	KeyConfigPanel->AddProfiles(Preferences.KeyProfiles);
	KeyConfigPanel->ImportMenuBarCmd(menuBar, _("MVC Editor"));
	notebook->AddPage(KeyConfigPanel, _("Keyboard Shortcuts"));
}

void mvceditor::PreferencesDialogClass::Prepare() {
	GetBookCtrl()->InitDialog();
	LayoutDialog();
}

void mvceditor::PreferencesDialogClass::OnOkButton(wxCommandEvent& event) {
	wxBookCtrlBase* book = GetBookCtrl();
	if (Validate() && book->Validate() && TransferDataFromWindow() && book->TransferDataFromWindow()) {
		KeyConfigPanel->ApplyChanges();
		Preferences.CodeControlOptions.CommitChanges();
		Preferences.KeyProfiles = KeyConfigPanel->GetProfiles();
		Preferences.Save();
		EndModal(wxOK);
	}
}

BEGIN_EVENT_TABLE(mvceditor::PreferencesDialogClass, wxDialog) 
	EVT_BUTTON(wxID_OK, mvceditor::PreferencesDialogClass::OnOkButton) 	
END_EVENT_TABLE()