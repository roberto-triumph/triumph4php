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
#ifndef MVCEDITORFINDERPANELCLASS_H_
#define MVCEDITORFINDERPANELCLASS_H_

#include <plugins/wxformbuilder/FinderPluginGeneratedClass.h>
#include <widgets/ComboBoxHistoryClass.h>
#include <widgets/NotebookClass.h>
#include <widgets/StatusBarWithGaugeClass.h>
#include <search/FinderClass.h>
#include <PluginClass.h>

namespace mvceditor {

/**
 * This dialog allows the user to input text and search a file
 * for desired text.
 */
class FinderPanelClass : public FinderPanelGeneratedClass {
	
protected:
	// Handlers for FindDialogGeneratedClass events.
	void OnHelpButton(wxCommandEvent& event);
	void OnOkButton(wxCommandEvent& event);
	void OnPreviousButton(wxCommandEvent& event);
	void OnNextButton(wxCommandEvent& event);
	void OnCloseButton(wxCommandEvent& event);
	void OnRegExFindHelpButton(wxCommandEvent& event);
	void OnFindEnter(wxCommandEvent& event);
		
public:
	
	/**
	 * Construct a new finder dialog
	 * 
	 * @param wxWindow* parent the parent window
	 * @param NotebookClass notebook the object that holds the text. The pointer will NOT be managed (deleted) by this class. 
	 * @param wxAuiManager* auiManager used to hide this panel.  This class will NOT
	 *        delete the pointer, it is up to the caller.
	 * @param int windowId the window ID
	 */
	FinderPanelClass(wxWindow* parent, NotebookClass* notebook, wxAuiManager* auiManager, int windowId);
	
	~FinderPanelClass();

	/**
	 * The focus will  be set on the find text box.
	 */
	void SetFocusOnFindText();
	
	/**
	 * Set the new expression.
	 * 
	 * @param const wxString& expression the expression to set
	 */
	void SetExpression(const wxString& expression);

	/**
	 * Searches for the next instance of this expression in the current active code control. Searching starts at the current
	 * cursor position.
	 */
	void FindNext();
	
	/**
	 * Searches for the previous instance of this expression in the current active code control. Searching starts at the current
	 * cursor position.
	 */
	void FindPrevious();
	
private:

	/**
	 * Execute a find on the currently active notebook page
	 */
	void Find(bool findNext = true);
	
	/**
	 * Place the given string in the results status label
	 * 
	 * @param wxString message to show to the user
	 */
	void SetStatus(const wxString& message);
	
	/**
	 * When called show the user a picked a item in the menu of reg ex symbols
	 * @param wxCommandEvent& event
	 */
	void InsertRegExSymbol(wxCommandEvent& event);

	/**
	 * When called show the user a picked a item in the menu of replace reg ex symbols
	 * @param wxCommandEvent& event
	 */
	void InsertReplaceRegExSymbol(wxCommandEvent& event);

	/**
	 * Need to save the insertion point of the Find and replace combo boxes; in Win32
	 * GetInsertionPoint() of combobox fails when it does not have focus.
	 * The insertion is needed to have the cursor show up properly when
	 * the user clicks on the regex help buttons
	 */
	void OnKillFocusFindText(wxFocusEvent& event);
	
	/**
	 * The Finder object to be displayed.
	 * 
	 * @var FinderClass
	 */
	FinderClass Finder;
	
	/**
	 * Holds previously entered searches
	 */
	ComboBoxHistoryClass ComboBoxHistory;
	
	/**
	 * The source of text to search in
	 * @var MvcEditorNotebookClass
	 */
	NotebookClass* Notebook;
	
	/**
	 * The AUI Manager is needed in order to hide this panel based on keyboard shortcuts. This class will NOT
	 * delete this pointer.
	 * 
	 * @var wxAuiManager* 
	 */
	wxAuiManager* AuiManager;
	
	/**
	 * The message shown to the user (suitable to input into printf() function)
	 * @var wxString
	 */
	wxString RESULT_MESSAGE;

	int CurrentInsertionPointFind;
	
	DECLARE_EVENT_TABLE()

};

class ReplacePanelClass : public ReplacePanelGeneratedClass {
	
protected:
	void OnHelpButton(wxCommandEvent& event);
	void OnOkButton(wxCommandEvent& event);
	void OnPreviousButton(wxCommandEvent& event);
	void OnNextButton(wxCommandEvent& event);
	void OnReplaceButton(wxCommandEvent& event);
	void OnReplaceAllButton(wxCommandEvent& event);
	void OnUndoButton(wxCommandEvent& event);
	void OnCloseButton(wxCommandEvent& event);
	void OnRegExFindHelpButton(wxCommandEvent& event);
	void OnReplaceRegExFindHelpButton(wxCommandEvent& event);
	void OnFindEnter(wxCommandEvent& event);
	void OnReplaceEnter(wxCommandEvent& event);
	
public:

	/**
	 * Construct a new finder dialog
	 * 
	 * @param wxWindow* parent the parent window
	 * @param NotebookClass notebook the object that holds the text. The pointer will NOT be managed (deleted) by this class. 
	 * @param wxAuiManager* auiManager used to hide this panel.  This class will NOT
	 *        delete the pointer, it is up to the caller.
	 * @param int windowId the window ID
	 */
	ReplacePanelClass(wxWindow* parent, NotebookClass* notebook, wxAuiManager* auiManager, int windowId);
	
	~ReplacePanelClass();

	/**
	 * Enables/disables replace buttons
	 * @var bool enable if true, buttons will be enabled. else, buttons
	 *     will be disabled.
	 */
	void EnableReplaceButtons(bool enable);

	/**
	 * The focus will  be set on the find text box.
	 */
	void SetFocusOnFindText();
	
	/**
	 * Set the new expression.
	 * 
	 * @param const wxString& expression the expression to set
	 */
	void SetExpression(const wxString& expression);

	/**
	 * Searches for the next instance of this expression in the current active code control. Searching starts at the current
	 * cursor position.
	 */
	void FindNext();
	
	/**
	 * Searches for the previous instance of this expression in the current active code control. Searching starts at the current
	 * cursor position.
	 */
	void FindPrevious();
	
private:

	/**
	 * Execute a find on the currently active notebook page
	 */
	void Find(bool findNext = true);
	
	/**
	 * Place the given string in the results status label
	 * 
	 * @param wxString message to show to the user
	 */
	void SetStatus(const wxString& message);
	
	/**
	 * Called whenever the user hits the ENTER key. Will perform a find when in Multiline mode.
	 * @param wxCommandEvent& event
	 */
	void OnTextEnter(wxCommandEvent& event);
	
	/**
	 * When called show the user a picked a item in the menu of reg ex symbols
	 * @param wxCommandEvnt& event
	 */
	void InsertRegExSymbol(wxCommandEvent& event);
	
	/**
	 * When called show the user a picked a item in the menu of replace reg ex symbols
	 * @param wxCommandEvnt& event
	 */
	void InsertReplaceRegExSymbol(wxCommandEvent& event);

	/** 
	 * since this panel handles EVT_TEXT_ENTER, we need to handle the
	 * tab traversal ourselves otherwise tab travesal wont work
	 */
	void OnKeyDown(wxKeyEvent& event);

	/**
	 * Need to save the insertion point of the Find and replace combo boxes; in Win32
	 * GetInsertionPoint() of combobox fails when it does not have focus.
	 * The insertion is needed to have the cursor show up properly when
	 * the user clicks on the regex help buttons
	 */
	void OnKillFocusFindText(wxFocusEvent& event);
	void OnKillFocusReplaceText(wxFocusEvent& event);

	/**
	 * The Finder object to be displayed.
	 * 
	 * @var FinderClass
	 */
	FinderClass Finder;
	
	/**
	 * Holds previously entered searches
	 */
	ComboBoxHistoryClass FindHistory;
	
	/**
	 * Holds previously entered replacements
	 */
	ComboBoxHistoryClass ReplaceHistory;
	
	/**
	 * The source of text to search in
	 * @var MvcEditorNotebookClass
	 */
	NotebookClass* Notebook;
	
	/**
	 * The AUI Manager is needed in order to hide this panel based on keyboard shortcuts. This class will NOT
	 * delete this pointer.
	 * 
	 * @var wxAuiManager* 
	 */
	wxAuiManager* AuiManager;
	
	/**
	 * The message shown to the user (suitable to input into printf() function)
	 * @var wxString
	 */
	wxString RESULT_MESSAGE;

	int CurrentInsertionPointFind;

	int CurrentInsertionPointReplace;
	
	DECLARE_EVENT_TABLE()
};

class FinderPluginClass : public PluginClass {

public:

	/**
	 * Constructor
 	 */
	FinderPluginClass();
	
	/**
	 * Add menu items to the edit menu for this plugin.
	 * 
	 * @param wxMenu* menu the tools menu to add items to.
	 */
	void AddEditMenuItems(wxMenu* editMenu);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);

private:

	/**
	 * show the find dialog 
	 * 
	 * @param wxCommandEvent& event the event
	 */
	void OnEditFind(wxCommandEvent& event);
	
	/**
	 * go to the next hit
	 * 
	 * @param wxCommandEvent& event
	 */
	void OnEditFindNext(wxCommandEvent& event);
	
	/**
	 * go to the previous hit
	 * 
	 * @param wxCommandEvent& event
	 */
	void OnEditFindPrevious(wxCommandEvent& event);

	/**
	 * show the replace dialog
	 * 
	 * @param wxCommandEvent& event
	 */
	void OnEditReplace(wxCommandEvent& event);

	/**
	 * show the Go To Line dialog
	 * 
	 * @param wxCommandEvent& event
	 */
	void OnEditGoToLine(wxCommandEvent& event);
	
	DECLARE_EVENT_TABLE()
};

}
#endif /*MVCEDITORFINDERPANELCLASS_H_*/
