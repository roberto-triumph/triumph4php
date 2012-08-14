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
#ifndef __FindInFilesPanelClass__
#define __FindInFilesPanelClass__

/**
@file
Subclass of FindInFilesPanelGeneratedClass, which is generated by wxFormBuilder.
*/

#include <plugins/wxformbuilder/FindInFilesPluginDialogGeneratedClass.h>
#include <plugins/BackgroundFileReaderClass.h>
#include <search/DirectorySearchClass.h>
#include <search/FindInFilesClass.h>
#include <environment/ProjectClass.h>
#include <widgets/ComboBoxHistoryClass.h>
#include <widgets/NotebookClass.h>
#include <widgets/StatusBarWithGaugeClass.h>
#include <PluginClass.h>
#include <wx/thread.h>
#include <vector>

namespace mvceditor {

/**
 * A single hit that resulted from a find in files action.
 */
class FindInFilesHitClass {

public:

	/**
	 * the full path of the file searched
	 */
	wxString FileName;
	
	/**
	 * preview is the entire line where the hit occurred.
	 */
	wxString Preview;
	
	/**
	 * line where the hit was found (1- based)
	 */
	int LineNumber;

	FindInFilesHitClass();

	FindInFilesHitClass(const wxString& fileName, const wxString& preview, int lineNumber);
};

/**
 * One EVENT_FIND_IN_FILES_FILE_HIT event will be generated once an entire file has been searched.
 */
class FindInFilesHitEventClass : public wxEvent {

public:

	/**
	 * All of the hits for a single file.
	 */
	std::vector<mvceditor::FindInFilesHitClass> Hits;

	FindInFilesHitEventClass(const std::vector<mvceditor::FindInFilesHitClass>& hits);

	wxEvent* Clone() const;
};


const wxEventType EVENT_FIND_IN_FILES_FILE_HIT = wxNewEventType();

typedef void (wxEvtHandler::*FindInFilesHitEventClassFunction)(FindInFilesHitEventClass&);

#define EVT_FIND_IN_FILES_HITS(fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_FIND_IN_FILES_FILE_HIT, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( FindInFilesHitEventClassFunction, & fn ), (wxObject *) NULL ),


/**
 * This class is the background thread where all finding and replacing will be done.
 */
class FindInFilesBackgroundReaderClass: public BackgroundFileReaderClass {
public:

	FindInFilesBackgroundReaderClass(wxEvtHandler& handler, mvceditor::RunningThreadsClass& runningThreads);

	/**
	 * Prepare to iterate through all files in the given directory.
	 * 
	 * @param wxEvtHandler* This object will receive the EVENT_FIND_IN_FILES_FILE_HIT events. The pointer will NOT be managed 
	 *        (deleted) by this class. 
	 * @param FindInFilesClass findInFiles the expression to search for
	 * @param doHiddenFiles if TRUE then hidden files are searched
	 * @param skipFiles full paths of files to not search. We want to NOT perform searches 
	 *        in files that are already opened; those would result in incorrect hits.
	 * @return True if directory is valid and the find expression is valid.
	 */
	bool InitForFind(wxEvtHandler* handler, FindInFilesClass findInFiles, bool doHiddenFiles, std::vector<wxString> skipFiles);

	/**
	 * When replacing, the thread will replace all matched files. In case files are opened, we don't want to
	 * replace them in the background since the user may have modified them but not saved them yet.
	 * 
	 * @param wxEvtHandler* This object will receive the various FIND events. The pointer will NOT be managed 
	 *        (deleted) by this class. 
	 * @param FindInFilesClass findInFiles the expression to search and replace with
	 * @param skipFiles full paths of files to not replace. We want to NOT perform replacements
	 * in files that are already opened.
	 * @return true if there are matching files from the previous find
	 * operation.
	 */
	bool InitForReplace(wxEvtHandler* handler, FindInFilesClass findInFiles, std::vector<wxString> skipFiles);

	/**
	 * Creates a Hit event for the current FindInFiles match. (the event will NOT be posted).
	 * @param lineNumber the line that where the hit occurred
	 * @param the line text itself
	 * @param fileName the name of the file that was searched.
	 */
	static wxCommandEvent MakeHitEvent(int lineNumber, const wxString& lineText, const wxString& fileName);

protected:

	/**
	 * Finds the expression in all files.
	 */
	bool BackgroundFileRead(DirectorySearchClass& search);

	/**
	 * Replaces this expression in all files.
	 */
	bool BackgroundFileMatch(const wxString& file);
	
private:

	/**
	 * To find matches in files.
	 * 
	 * @var FindInFilesClass
	 */
	FindInFilesClass FindInFiles;
	
	/**
	 * Matched files that will NOT be replaced / searched
	 * The plugin will make the background thread skip the files that are currently opened; this way the result do not
	 * show stale (and possibly wrong) hits
	 */
	std::vector<wxString> SkipFiles;
	
	/**
	 * This object will receive the various FIND events. The pointer will NOT be managed 
	 * deleted) by this class. 
	 * 
	 * @var wxEvtHandler*
	 */
	wxEvtHandler* Handler;
};

/** Implementing FindInFilesPanelGeneratedClass */
class FindInFilesResultsPanelClass : public FindInFilesResultsPanelGeneratedClass {
protected:
	// Handlers for FindInFilesResultsPanelClass events.
	void OnReplaceButton(wxCommandEvent& event);
	void OnReplaceAllInFileButton(wxCommandEvent& event);
	void OnReplaceInAllFilesButton(wxCommandEvent& event);
	void OnFileHit(mvceditor::FindInFilesHitEventClass& event);
	void OnStopButton(wxCommandEvent& event);
	void OnDoubleClick(wxCommandEvent& event);
	void OnCopySelectedButton(wxCommandEvent& event);
	void OnCopyAllButton(wxCommandEvent& event);
	void OnFindInFilesComplete(wxCommandEvent& event);
		
public:

	/**
	 * Construct a new FindInFilesResultsPanelClass 
	 * 
	 * @param wxWindow* parent the parent window
	 * @param NotebookClass* notebook the object that holds the text. The pointer will NOT be managed (deleted) by this class. 
	 * @param StatusBarWithGaugeClass* gauge the object used to create gauge for showing progress. The pointer will NOT be managed 
	 *        (deleted) by this class. 
	 * to search in
	 */
	FindInFilesResultsPanelClass(wxWindow* parent, NotebookClass* notebook, StatusBarWithGaugeClass* gauge,
		mvceditor::RunningThreadsClass& runningThreads);
	
	~FindInFilesResultsPanelClass();
	
	/**
	 * Start a file search.
	 * 
	 * @param FindInFilesClass findInFiles the search expression
	 * @param wxString path the directory to search in
	 * @param bool if TRUE then hidden files will be searched
	 */
	void Find(const FindInFilesClass& findInFiles, bool doHiddenFiles);
	
	/**
	 * Stops a currently running search. It will clean up 
	 * the gauge also, this means that it must be called while the gauge is valid (ie NOT
	 * at program end)
	 */
	void Stop();

	/**
	 * Move the cursor to the next find in files match.
	 * Will set the cursor to the position of the match, opening the file
	 * it if it's not already open.
	 */
	void ShowNextMatch();
	
	/**
	 * Move the cursor to the previous find in files match.
	 * Will set the cursor to the position of the match, opening the file
	 * it if it's not already open.
	 */
	void ShowPreviousMatch();

private:

	/**
	 * To find matches in files.
	 * 
	 * @var FindInFilesClass
	 */
	FindInFilesClass FindInFiles;
	
	/**
	 * Used to iterate through directories
	 * @var DirectorySearchClass
	 */
	FindInFilesBackgroundReaderClass* FindInFilesBackgroundFileReader;
	
	/**
	 * To open the files 
	 * @var NotebookClass
	 */
	NotebookClass* Notebook;
	
	/**
	 * To display status bar to the user
	 * @var StatusBarWithGaugeClass
	 */
	StatusBarWithGaugeClass* Gauge;
	
	/**
	 * The number of files that contained at least one match.
	 * 
	 * @var int
	 */
	int MatchedFiles;

	/**
	 * The unique identifier for the gauge.
	 */
	int FindInFilesGaugeId;
	
	/**
	 * Enable the controls that allow the user to replace hits or stop searches.
	 * 
	 * @param bool enableStopButton if true, the stop button is enabled, else it is disabled
	 * @param bool enableReplaceButtons if true, the replace buttons are enabled, else they are disabled
	 * @param bool enableCopyButtons if true the copy to clipboard buttos are enabled, else they are disabled
	 */
	void EnableButtons(bool enableStopButton, bool enableReplaceButtons, bool enableCopyButtons);

	/**
	 * Place the given string in the results status label
	 * 
	 * @param wxString message to show to the user
	 */
	void SetStatus(const wxString& status);
			
	/**
	 * Retutns the number of files that had matches. 
	 * 
	 * @return int the number of files under FindPath that had at least one match for this Expression
	 */
	int GetNumberOfMatchedFiles();
	
	/**
	 * Timer handler.
	 * 
	 * @param wxCommandEvent& event
	 */
	 void OnTimer(wxCommandEvent& event);

	 /**
	  * Search in the files that are opened. We want to do this so that we only show the most up-to-date
	  * hits
	  */
	 void FindInOpenedFiles();

	 /**
	  * Shows the i'th match. Will set the cursor to the position of the match, opening the file
	  * it if it's not already open.
	  *
	  * @param i index of hit to show. i is 0-based
	  */
	 void ShowMatch(int i);
	
	DECLARE_EVENT_TABLE()
};

class FindInFilesPluginClass : public PluginClass {

public:
	
	/**
	 * Holds previously entered searches. We need these to persist longer than the dialog so that
	 * the user's inputs are saved.
	 */
	ComboBoxHistoryClass FindHistory;
	
	/**
	 * Holds previously entered replacements. We need these to persist longer than the dialog so that
	 * the user's inputs are saved.
	 */
	ComboBoxHistoryClass ReplaceHistory;

	/**
	 * Holds previously entered directories to search in. We need these to persist longer than the dialog so that
	 * the user's inputs are saved.
	 */
	ComboBoxHistoryClass DirectoriesHistory;
	
	/**
	 * Holds previously entered file filters. We need these to persist longer than the dialog so that
	 * the user's inputs are saved.
	 */
	ComboBoxHistoryClass FilesHistory;
	
	/**
	 * We will NOT use this object to actually search, we will just use this to keep track of the last
	 * inputs by the user so we can show them
	 *
	 */
	FindInFilesClass PreviousFindInFiles;

	/**
	 * If TRUE, hidden files are searched
	 */
	bool DoHiddenFiles;

	/**
	 * Constructor
	 */
	FindInFilesPluginClass(mvceditor::AppClass& app);
	
	/**
	 * Add menu items to the search menu for this plugin.
	 * 
	 * @param wxMenu* menu the tools menu to add items to.
	 */
	void AddSearchMenuItems(wxMenu* searchMenu);

	void AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts);

private:
	
	/**
	 * show the find dialog 
	 */
	void OnEditFindInFiles(wxCommandEvent& event);

	void OnEditFindInFilesNext(wxCommandEvent& event);

	void OnEditFindInFilesPrevious(wxCommandEvent& event);
	
	/**
	 * When a tools notebook tab is closed, if its a find in files results tab
	 * the the search will be stopped AND the gauge must be cleaned up.
	 */
	void OnToolsNotebookPageClosed(wxAuiNotebookEvent& event);
	
	/**
	 * Safer to keep a pointer to these, rather than attempt to type cast.
	 * Because the tools notebook can contain many types of panels.
	 * When a tools notebook tab is closed, if its a find in files results tab
	 * the the search will be stopped AND the gauge must be cleaned up.
	 * Note that because these are window pointers, we wont own the pointers
	 * wxWidgets will clean them up.
	 */
	std::vector<mvceditor::FindInFilesResultsPanelClass*> ResultsPanels;
	
	DECLARE_EVENT_TABLE()
};

class FindInFilesDialogClass: public FindInFilesDialogGeneratedClass {
	
public:

	FindInFilesDialogClass(wxWindow* parent, FindInFilesPluginClass& plugin);
	
	~FindInFilesDialogClass();

protected:

	virtual void OnOkButton(wxCommandEvent& event);
	virtual void OnCancelButton(wxCommandEvent& event);
	void OnRegExFindHelpButton(wxCommandEvent& event);
	void OnRegExReplaceHelpButton(wxCommandEvent& event);
	void OnDirChanged(wxFileDirPickerEvent& event);

private:

	FindInFilesPluginClass& Plugin;

	int CurrentInsertionPointFind;

	int CurrentInsertionPointReplace;
	
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
	 * handle the regular expression popup menu. Will add the symbol
	 * the the expression textbox value.
	 */
	void InsertRegExSymbol(wxCommandEvent& event);

	/**
	 * handle the regular expression replace popup menu. Will add the symbol
	 * the the replace textbox value.
	 */
	void InsertReplaceRegExSymbol(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

}
#endif // __FindInFilesPanelClass__
