/**
 * $Date: 2011-08-09 20:24:12 -0700 (Tue, 09 Aug 2011) $
 * $Rev: 592 $
 * $Author: robertop2004@gmail.com $
 * 
 * @copyright 2010 Roberto Perpuly
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
#include <php_frameworks/ProjectClass.h>
#include <widgets/ComboBoxHistoryClass.h>
#include <widgets/NotebookClass.h>
#include <widgets/StatusBarWithGaugeClass.h>
#include <PluginClass.h>
#include <wx/thread.h>
#include <vector>

namespace mvceditor {

/*
 * States of the background thread. The background thread will be where all finding and replacing 
 * will be done.  Callers will give the thread the FindInFiles object and directory to search in. This thread will generate
 * events whenever (1) a file is searched, (2) when a hit is found and (3) when a hit is replaced.  The following events
 * will be generated:
 *   EVENT_FIND_IN_FILES_FILE_HIT - when hit is found. The event String will be a string with the format 
 *     "filename\t\tLine Number\t\tLinePreview" where filename is the full path of the file searched, line number is the 
 *     line where the hit was found, and line preview is the entire line where the hit occurred.
 */
const wxEventType EVENT_FIND_IN_FILES_FILE_HIT = wxNewEventType();
	
/**
 * This class is the background thread where all finding and replacing will be done.
 */
class FindInFilesBackgroundReaderClass: public BackgroundFileReaderClass {
public:

	FindInFilesBackgroundReaderClass(wxEvtHandler& handler);

	/**
	 * Prepare to iterate through all files in the given directory.
	 * 
	 * @param wxEvtHandler* This object will receive the EVENT_FIND_IN_FILES_FILE_HIT events. The pointer will NOT be managed 
	 *        (deleted) by this class. 
	 * @param FindInFilesClass findInFiles the expression to search for
	 * @param wxString the directory search. 
	 * @return True if directory is valid and the find expression is valid.
	 */
	bool InitForFind(wxEvtHandler* handler, FindInFilesClass findInFiles, const wxString& path);

	/**
	 * When replacing, the thread will replace all matched files. In case files are opened, we don't want to
	 * replace them in the background since the user may have modified them but not saved them yet.
	 * 
	 * @param wxEvtHandler* This object will receive the various FIND events. The pointer will NOT be managed 
	 *        (deleted) by this class. 
	 * @param FindInFilesClass findInFiles the expression to search and replace with
	 * @param files full paths of files to not replace. We want to NOT perform replacements
	 * in files that are already opened.
	 * @return true if there are matching files from the previous find
	 * operation.
	 */
	bool InitForReplace(wxEvtHandler* handler, FindInFilesClass findInFiles, std::vector<wxString> files);

protected:

	/**
	 * Finds the expression in all files.
	 */
	bool FileRead(DirectorySearchClass& search);

	/**
	 * Replaces this expression in all files.
	 */
	bool FileMatch(const wxString& file);
	
private:

	/**
	 * To find matches in files.
	 * 
	 * @var FindInFilesClass
	 */
	FindInFilesClass FindInFiles;
	
	/**
	 * Matched files that will NOT be replaced
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
	void OnFileSearched(wxCommandEvent& event);
	void OnFileHit(wxCommandEvent& event);
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
	FindInFilesResultsPanelClass(wxWindow* parent, NotebookClass* notebook, StatusBarWithGaugeClass* gauge);
	
	/**
	 * Start a file search.
	 * 
	 * @param FindInFilesClass findInFiles the search expression
	 * @param wxString path the directory to search in
	 */
	void Find(const FindInFilesClass& findInFiles, wxString findPath);
	
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
	FindInFilesBackgroundReaderClass FindInFilesBackgroundFileReader;
	
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
	 * The path to search. 
	 */
	wxString FindPath;
	
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
	 */
	void EnableButtons(bool enableStopButton, bool enableReplaceButtons);

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
	 * Previous find path
	 * @var wxString
	 */
	wxString PreviousFindPath;	

	/**
	 * Constructor
	 */
	FindInFilesPluginClass();
	
	/**
	 * Add menu items to the edit menu for this plugin.
	 * 
	 * @param wxMenu* menu the tools menu to add items to.
	 */
	virtual void AddEditMenuItems(wxMenu* editMenu);

private:
	
	/**
	 * show the find dialog 
	 */
	void OnEditFindInFiles(wxCommandEvent& event);
	
	DECLARE_EVENT_TABLE()
};

class FindInFilesDialogClass: public FindInFilesDialogGeneratedClass {
	
public:

	FindInFilesDialogClass(wxWindow* parent, FindInFilesPluginClass& plugin);

protected:

	virtual void OnOkButton(wxCommandEvent& event);

	virtual void OnCancelButton(wxCommandEvent& event);

private:

	FindInFilesPluginClass& Plugin;
	
	/** 
	 * since this panel handles EVT_TEXT_ENTER, we need to handle the
	 * tab traversal ourselves otherwise tab travesal wont work
	 */
	void OnKeyDown(wxKeyEvent& event);
};

}
#endif // __FindInFilesPanelClass__
