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
#ifndef T4P_EDITORBEHAVIORVIEWCLASS_H__
#define T4P_EDITORBEHAVIORVIEWCLASS_H__

#include <views/FeatureViewClass.h>
#include <views/wxformbuilder/EditorBehaviorFeatureForms.h>
#include <features/EditorBehaviorFeatureClass.h>
#include <globals/Events.h>
#include <wx/stc/stc.h>

namespace t4p {

class EditorBehaviorViewClass : public t4p::FeatureViewClass {

public:

	EditorBehaviorViewClass(t4p::EditorBehaviorFeatureClass& feature);
	
	void AddToolBarItems(wxAuiToolBar* toolBar);
	
	void AddEditMenuItems(wxMenu* editMenu);
	
	void AddViewMenuItems(wxMenu* viewMenu);
		
	void AddCodeControlClassContextMenuItems(wxMenu* menu);
	
	void AddPreferenceWindow(wxBookCtrlBase* parent);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);

private:

	/**
	 * adds menu items for each of the scintilla keyboard commands
	 * to the given menu
	 * @param menu the menu to add the items to
	 * @param bool if TRUE menu items that require that text be selected
	 *        will be disabled
	 */
	void AddKeyboardCommands(wxMenu* menu, bool isTextSelected);
	void SetFeaturesOnNotebook();
	void SetFeatures(const t4p::CodeControlOptionsClass& options, wxStyledTextCtrl* codeCtrl);
	
	void OnAppFileOpened(t4p::CodeControlEventClass& event);
	void OnAppFileNew(t4p::CodeControlEventClass& event);
	
	void OnToggleWordWrap(wxCommandEvent& event);
	void OnToggleIndentationGuides(wxCommandEvent& event);
	void OnToggleWhitespace(wxCommandEvent& event);
	void OnZoomIn(wxCommandEvent& event);
	void OnZoomOut(wxCommandEvent& event);
	void OnZoomReset(wxCommandEvent& event);
	void OnEditConvertEols(wxCommandEvent& event);
	void OnEditorCommand(wxCommandEvent& event);

	// Top Menu Bar handlers
	void OnEditCut(wxCommandEvent& event);
	void OnEditCopy(wxCommandEvent& event);
	void OnEditPaste(wxCommandEvent& event);
	void OnEditSelectAll(wxCommandEvent& event);
	void OnEditContentAssist(wxCommandEvent& event);
	void OnEditCallTip(wxCommandEvent& event);

 	/**
 	 * Handle the Undo popup menu event
	 */
	void OnUndo(wxCommandEvent& event);

	/**
	 * Handle the Redo popup menu event
	 */
	void OnRedo(wxCommandEvent& event);

	/**
	 * Toggle various widgets on or off based on the application state.
	 */
	void OnUpdateUi(wxUpdateUIEvent& event);

	t4p::EditorBehaviorFeatureClass& Feature;

	DECLARE_EVENT_TABLE()
};

/**
 * panel that shows all options regarding editor behavior
 */
class EditorBehaviorPanelClass : public EditorBehaviorPanelGeneratedClass {

public:
	
	EditorBehaviorPanelClass(wxWindow* parent, CodeControlOptionsClass& options);
	bool TransferDataToWindow();
	bool TransferDataFromWindow();

protected:

	// Handlers for EditorBehaviorPanelGeneratedClass events.
	void OnIndentUsingSpaces(wxCommandEvent& event);
	void OnCheckRightMargin(wxCommandEvent& event);

};

/**
 * The editor command panel allows the user to edit the keyboard commands
 * that scintilla exposes.
 * They are separate from menu accelators because menu accelators 
 * respond to their keystrokes at the application level; meaning that
 * if we had the "move to next word" command (CTRL+RIGHT)in the menu, then
 * hitting CTRL+RIGHT in ANY textbox will move the code control, not the
 * textbox that has focus.
 */
class EditorCommandPanelClass : public EditorCommandPanelGeneratedClass {

public:

	/**
	 * @param parent the window parent
	 * @param int id the window ID
	 * @param commands the scintilla keyboard commands being edited
	 * @param keyBinder holds the menu shortcuts. will check for duplicates when the user enters a shortcut.
	 *        this class will NOT own the keyBinder pointer
	 */
	EditorCommandPanelClass(wxWindow* parent, int id, std::vector<t4p::EditorKeyboardCommandClass>& commands,
		wxKeyBinder* keyBinder);

	bool TransferDataFromWindow();

private:

	void FillCommands();

	void OnItemActivated(wxListEvent& event);

	/**
	 * the commands to be updated after the user clicks the OK button
	 */
	std::vector<t4p::EditorKeyboardCommandClass>& Commands;

	/**
	 * the commands to being edited by the user.
	 */
	std::vector<t4p::EditorKeyboardCommandClass> EditedCommands;

	/**
	 * this class will NOT own the keyBinder pointer
	 */
	wxKeyBinder* KeyBinder;

};

/**
 * the edit dialog will check for (and reject) duplicate menu shortcuts
 * this class will NOT own the keyBinder pointer
 */
class KeyboardCommandEditDialogClass : public KeyboardCommandEditDialogGeneratedClass {

public:

	/**
	 * @param parent the window parent
	 * @param commandName to display to the user
	 * @param shortcut [out] the new shortcut from the user will be set here
	 * @param commands the scintilla keyboard commands
	 * @param keyBinder holds the menu shortcuts this class will NOT own the keyBinder pointer
	 */
	KeyboardCommandEditDialogClass(wxWindow* parent, const wxString& commandName, wxString& shortcut,
		std::vector<t4p::EditorKeyboardCommandClass>& commands,
		wxKeyBinder* keyBinder);

private:

	void OnOkButton(wxCommandEvent& event);
	void OnKey(wxKeyEvent& event);
	void OnEnter(wxCommandEvent& event);

	/**
	 * to check for (and reject) duplicate shortcuts
	 */
	std::vector<t4p::EditorKeyboardCommandClass>& Commands;

	/**
	 * to check for (and reject) duplicate menu shortcuts
	 * this class will NOT own this pointer
	 */
	wxKeyBinder* KeyBinder;

	/**
	 * store a copy of the original shortcut, so that the user
	 * can "assign" the same shortcut without the app telling
	 * them that it is a dup
	 */
	wxString OriginalShortcut;
};

}

#endif

