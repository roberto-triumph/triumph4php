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
 * @author     $Author: robertop2004@gmail.com $
 * @date       $Date: 2011-08-13 20:31:56 -0700 (Sat, 13 Aug 2011) $
 * @version    $Rev: 596 $
 */

#ifndef __LINTPLUGINCLASS_H__
#define __LINTPLUGINCLASS_H__

#include <vector>
#include <PluginClass.h>
#include <search/DirectorySearchClass.h>
#include <plugins/BackgroundFileReaderClass.h>
#include <plugins/wxformbuilder/LintPluginGeneratedClass.h>

BEGIN_DECLARE_EVENT_TYPES()

	/**
	 * This event will get dispatched when a lint (syntax) error is encountered.  It will
	 * be filled with the following info:
	 * event.getClientData(): this is a pointer to a mvceditor::LintResultsClass instance.  
	 * the event handler MUST delete the pointer!
	 */
	DECLARE_EVENT_TYPE(EVENT_LINT_ERROR, wxNewId())
END_DECLARE_EVENT_TYPES()

namespace mvceditor {
	
/** 
 * This class will help in parsing the large project. It will enable access
 * to DirectorySearch and easily parse many files.
 */
class ParserDirectoryWalkerClass : public DirectoryWalkerClass {
public:

	ParserDirectoryWalkerClass();
	
	/**
	 * This is the method where the parsing will take place. Will return true
	 * if and only if there is a parse error.  
	 */
	virtual bool Walk(const wxString& file);
	
	/**
	 * The last parsing results.
	 * 
	 */
	LintResultsClass LastResults;
	
	/**
	 * The file extensions we want to attempt to parse.
	 */
	wxString PhpFileExtensions;
	
	/**
	 * Running count of files that had parse errors.
	 */
	int WithErrors;
	
	
	/**
	 * Running count of files that had zero parse errors.
	 */
	int WithNoErrors;
	
private:

	mvceditor::ParserClass Parser;
};

/**
 * This class will allow us to perform parsing in a background thread.
 * 
 */
class LintBackgroundFileReaderClass : public BackgroundFileReaderClass {

public:	

	/**
	 * @param handler the object that will receive LINT_ERROR events
	 *        this pointer will NOT be managed by this object (will not
	 *        DELETE the pointer).
	 */
	LintBackgroundFileReaderClass(wxEvtHandler* handler);
	
	/**
	 * Start the background thread.  Lint errors will be propagated as events.
	 * @param wxString directory the location of files that need to be parsed.  Parsing will
	 *        be recursive (sub directories will be parsed also).
	 * @param wxString phpFileExtension wildcard filter of files to parse. See wxIsWild()
	 *       for description of valid wildcards.
	 * @param StartError& error the reason for a failure to start will be set here.
	 * return bool TRUE if and only if the thread was started.  If false, either thread could
	 * not be started or directory is not valid. 
	 */
	bool BeginDirectoryLint(const wxString& directory, const wxString& phpFileExtensions, StartError& error);

	/**
	 * Lint checks the given file in the current thread.  This is a thread-safe method;
	 * it is safe to call this even when the thread started by BeginDirectoryLint() has not
	 * finished.
	 *
	 * Lint errors will be propagated as events.
	 * Returns TRUE if the file contains a lint error.
	 */
	bool LintSingleFile(const wxString& fileName);
	
protected:

	/**
	 * Will parse the current file. 
	 * @return TRUE if file had ZERO parse errors
	 */
	bool FileRead(DirectorySearchClass& search);
	
	/**
	 * 
	 * will do nothing for now
	 */
	bool FileMatch(const wxString& file);
	

private:

	ParserDirectoryWalkerClass ParserDirectoryWalker;
		
	wxEvtHandler* Handler;
	
	wxString PhpFileExtensions;
};

/**
 * This panel will show the list of lint errors.
 */
class LintResultsPanelClass : public LintResultsGeneratedPanelClass {
	
public:

	LintResultsPanelClass(wxWindow *parent, int id, NotebookClass* notebook, std::vector<LintResultsClass*>& lintErrors);

	~LintResultsPanelClass();
	
	/**
	 * adds to the list box widget AND the parseResults data structure
	 * This object will own the given pointer (will delete it).
	 */
	void AddError(LintResultsClass* lintError);
	
	/**
	 * deletes from the list box widget AND the parseResults data structure
	 */
	void ClearErrors();

	/**
	 * deletes from the list box widget AND the parseResults data structure all of
	 * the results for the given file.
	 */
	void RemoveErrorsFor(const wxString& fileName);

	/**
	 * Marks up the source code control window with the error that is located
	 * at the given index.  For example; if given index is 0 then the first lint 
	 * result file (added via AddError()) will be opened, scrolled to the lint error line, and the 
	 * editor will be marked up.
	 */
	void DisplayLintError(int index);

	virtual void OnListDoubleClick(wxCommandEvent& event);

private:

	NotebookClass* Notebook;

	std::vector<LintResultsClass*>& LintErrors;
};

/**
 * This is the class that will manage all of the UI buttons and menus
 * for the plugin
 */
class LintPluginClass : public PluginClass {
	
public:

	LintPluginClass();

protected: 

	void AddProjectMenuItems(wxMenu* projectMenu);
	
	void AddToolBarItems(wxAuiToolBar* toolBar);
	
private:

	void OnLintMenu(wxCommandEvent& event);
	
	void OnLintError(wxCommandEvent& event);

	void OnLintFileComplete(wxCommandEvent& event);

	void OnLintComplete(wxCommandEvent& event);
	
	void OnTimer(wxTimerEvent& event);
	
	void OnFileSaved(wxCommandEvent& event);

	LintBackgroundFileReaderClass LintBackgroundFileReader;
	
	/**
	 * To increment the gauge smoothly.
	 * 
	 * @var wxTimer
	 */
	wxTimer Timer;

	/**
	 * This will hold all info about parse errors. This class will own 
	 * the pointers themselves as well and will delete them when they
	 * are no longer needed.
	 */
	std::vector<mvceditor::LintResultsClass*>LintErrors;
	
	DECLARE_EVENT_TABLE()
};


}

#endif
