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
#ifndef MVCEDITORFINDERFEATURECLASS_H_
#define MVCEDITORFINDERFEATURECLASS_H_

#include <features/wxformbuilder/FinderFeatureForms.h>
#include <widgets/ComboBoxHistoryClass.h>
#include <widgets/NotebookClass.h>
#include <widgets/StatusBarWithGaugeClass.h>
#include <search/FinderClass.h>
#include <features/FeatureClass.h>
#include <actions/ActionClass.h>

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
	void OnFindKeyDown(wxKeyEvent& event);

	/**
	 * Need to save the insertion point of the Find and replace combo boxes; in Win32
	 * GetInsertionPoint() of combobox fails when it does not have focus.
	 * The insertion is needed to have the cursor show up properly when
	 * the user clicks on the regex help buttons
	 */
	void OnFindKillFocus(wxFocusEvent& event);
		
public:
	
	/**
	 * Construct a new finder dialog
	 * 
	 * @param wxWindow* parent the parent window
	 * @param int windowId the window ID
	 * @param finder the finder instance to use for searching
	 * @param NotebookClass notebook the object that holds the text. The pointer will NOT be managed (deleted) by this class. 
	 * @param wxAuiManager* auiManager used to hide this panel.  This class will NOT
	 *        delete the pointer, it is up to the caller.
	 
	 */
	FinderPanelClass(wxWindow* parent, int windowId, mvceditor::FinderClass& finder, NotebookClass* notebook, wxAuiManager* auiManager);

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
	 * The Finder object to be displayed.
	 * 
	 * @var FinderClass
	 */
	FinderClass& Finder;
	
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

	/** 
	 * since this panel handles EVT_TEXT_ENTER, we need to handle the
	 * tab traversal ourselves otherwise tab travesal wont work
	 */
	void OnFindKeyDown(wxKeyEvent& event);
	void OnReplaceKeyDown(wxKeyEvent& event);

	/**
	 * Need to save the insertion point of the Find and replace combo boxes; in Win32
	 * GetInsertionPoint() of combobox fails when it does not have focus.
	 * The insertion is needed to have the cursor show up properly when
	 * the user clicks on the regex help buttons
	 */
	void OnFindKillFocus(wxFocusEvent& event);
	void OnReplaceKillFocus(wxFocusEvent& event);
	
public:

	/**
	 * Construct a new finder dialog
	 * 
	 * @param wxWindow* parent the parent window
	 * @param int windowId the window ID
	 * @param finder the object that does the searching
	 * @param NotebookClass notebook the object that holds the text. The pointer will NOT be managed (deleted) by this class. 
	 * @param wxAuiManager* auiManager used to hide this panel.  This class will NOT
	 *        delete the pointer, it is up to the caller.
	 */
	ReplacePanelClass(wxWindow* parent, int windowId, mvceditor::FinderClass& finder, NotebookClass* notebook, wxAuiManager* auiManager);

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
	 * The Finder object to be displayed.
	 * 
	 * @var FinderClass
	 */
	FinderClass &Finder;
	
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

/**
 * An action that will perform a find on an entire set of text
 * and POST FinderHitEventClas for each found hit. 
 */
class FinderActionClass : public mvceditor::ActionClass {
	
public:

	/**
	 * @param utf8buf this class will take ownership of this pointer
	 */
	FinderActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId,
		const UnicodeString& search, char* utf8Buf, int bufLength);
	
protected:
	
	void BackgroundWork();
	
	wxString GetLabel() const;
	
	private:
	
	/**
	 * to perform the search
	 */
	mvceditor::FinderClass Finder;

	/**
	 *  unicode representation of ut8buf
	 */
	UnicodeString Code;
	
	/**
	 * this class will own the pointer
	 */
	char* Utf8Buf;
	
	/**
	 * the number of characters in Utf8Buf
	 */
	int BufferLength;
};

/**
 * the event that is generated when a FinderActionClass finds an 
 * instance of the search text in the text being searched.
 * Note that positions are given as byte offsets and not character
 * counts so that the character-to-byte offsets are calculated in the
 * background thread.
 */
class FinderHitEventClass : public wxEvent {
	
public:

	/**
	 * this is the index into the utf8 buffer, not character pos
	 */
	int Start;
	
	/**
	 * this is the number of utf8 bytes, not number of characters
	 */
	int Length;
	
	FinderHitEventClass(int id, int start, int length);
	
	wxEvent* Clone() const;
	
};

extern const wxEventType EVENT_FINDER_ACTION;

typedef void (wxEvtHandler::*FinderHitEventClassFunction)(mvceditor::FinderHitEventClass&);

#define EVT_FINDER(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_FINDER_ACTION, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( FinderHitEventClassFunction, & fn ), (wxObject *) NULL ),


class FinderFeatureClass : public FeatureClass {

public:

	/**
	 * Constructor
 	 */
	FinderFeatureClass(mvceditor::AppClass& app);
	
	/**
	 * Add menu items to the edit menu for this feature.
	 * 
	 * @param wxMenu* menu the tools menu to add items to.
	 */
	void AddEditMenuItems(wxMenu* editMenu);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);

	void LoadPreferences(wxConfigBase* config);

private:

	/**
	 * will perform the searching
	 */
	mvceditor::FinderClass Finder;
	mvceditor::FinderClass FinderReplace;

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
	
	/**
	 * when a background search finds a hit we will highlight it
	 */
	void OnFinderHit(mvceditor::FinderHitEventClass& event);
	
	/**
	 *  when the user double clicks on a word we will start a search
	 * for that word 
	 */
	void OnDoubleClick(wxStyledTextEvent& event);
	
	DECLARE_EVENT_TABLE()
};

}
#endif /*MVCEDITORFINDERFEATURECLASS_H_*/
