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
#ifndef MVCEDITORCODECONTROLCLASS_H_
#define MVCEDITORCODECONTROLCLASS_H_

#include <php_frameworks/ProjectClass.h>
#include <search/ResourceFinderClass.h>
#include <code_control/CodeControlOptionsClass.h>
#include <wx/stc/stc.h>
#include <wx/timer.h>
#include <unicode/unistr.h>

#include <vector>

/**
 * The source code editor.
 */
namespace mvceditor {

// some forward declarations to prevent re-compilation as much as possible
// Since this file is included by many plugins whenever a change to any included header
// files is maded most plugins have to be re-compiled.
class TextDocumentClass;
class ProjectClass;
class ResourceUpdateThreadClass;


/**
 * source code control with the following enhancements.
 * - Outside modification of a file: This code control will alert the user when the
 *   loaded file has been modified outside of the editor.
 * - PHP autocompletion of structures found in the current project.
 * - Displaying of PHP call tips
 * - "Word highlight" functionality; when user double clicks on a word then all
 *   instances of that word are highlighted.
 * - Automatic indentation
 */
class CodeControlClass : public wxStyledTextCtrl {

public:

	/**
	 * This mode flag controls what settings are used for syntax highlighting, margins,
	 * and code folding.  It also controls how Auto code completion should be handled
	 * if at all.
	 * The CodeControlClass will auto-detect the correct mode based on file name, but it
	 * can be changed via the SetDocumentMode() method.
	 */
	enum Mode {

		/**
		 * No code completion, ever. No syntax highlight, ever.  This is the default mode
		 * for anything that's not PHP or SQL.
		 */
		TEXT,

		/**
		 * The full functionalityL code completion, call tips, syntax highlighting, the works
		 */
		PHP,

		/**
		 * Code completion and SQL syntax highlighting
		 */
		SQL,

		/**
		 * CSS style sheets (pure CSS files only)
		 */
		CSS
	};

	/**
	 * Constructor. 
	 * @param ProjectClass* Memory management of project is left to the caller of this method. project
	 * CANNOT be NULL.
	 * @param ResourceUpdateThreadClass* resourceUpdates can be null. if null, then code completion will not be
	 *        functional.
	 * 
	 */
	CodeControlClass(wxWindow* parent, CodeControlOptionsClass& options, ProjectClass* project,
					ResourceUpdateThreadClass* resourceUpdates,
	                 int id, const wxPoint& position =
	                     wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
	                 const wxString& name = wxT("code"));

	~CodeControlClass();
	
	/**
	 * Loads the given contents of the given file, and initializes the
	 * proper lexer so that the file gets syntax highlight according to the language
	 * it contains.  This load method is better than the wxStyledTextCtrl load
	 * method because it will keep track of the file modification times and
	 * prompt the user when a file has been modified outside of the editor.
	 *
	 * @param wxString filename full path of the file to load
	 * @param contents the file contents
	 */
	void TrackFile(const wxString& filename, UnicodeString& contents);

	/**
	 * lets avoid calls like this
	 *
	 * UnicodeString contents //....
	 * wxStyledTextCtrl::SetText(mvceditor::IcuToWx(contents))
	 *
	 * This method prevents going from 
	 * UnicodeString -> UTF8 -> wxString  -> UTF8 -> Scintilla
	 * cost of translation could be big for big sized files
	 * because of the double encoding due to all three libraries using
	 * different internal encodings for their strings
	 * 
	 * @param contents the UnicodeString of contents that the code control will render 
	 */
	void SetUnicodeText(UnicodeString& contents);

	/**
	 * Returns true if the this control was NOT loaded with an existing
	 * file.
	 *
	 * @return bool true if the file was NOT loaded using LoadPhpFile method
	 */
	bool IsNew() const;

	/**
	 * If filename is not given, saves the contents of the code control under
	 * the name given in LoadAndTrackFile().  IfLoadPhpFile was not called, then
	 * caller should supply a filename and this method will create the new
	 * file
	 *
	 * If LoadAndTrackFile method was called then this method should be called otherwise
	 * the tracking feature will be broken.
	 *
	 * @param wxString filename the full path where the contents of the code
	 * 	control will be saved to.  Existing file will be overwritten.
	 * @return bool true if the file was successfully saved.
	 */
	bool SaveAndTrackFile(wxString filename = wxT(""));

	/**
	 * Set the options for this document.  This is auto-detected when using the LoadAndTrackFile() method
	 * and will most likely not need to be called.
	 *
	 * @param Mode a document mode; changes will take place immediately (causing a repaint)
	 */
	void SetDocumentMode(Mode mode);

	/**
	 * @return the current document mode that this code control is using.  it could be the one
	 * set by SetDocumentMode() or it could have been detected.
	 */
	Mode GetDocumentMode();

	/**
	 * Reload the file from this.  This method does nothing if this control was NOT loaded
	 * using LoadPhpFile method.
	 */
	void Revert();

	/**
	 * The filename of the file that is opened in this code control.
	 *
	 * @retrurn wxString the full path to the file that is shown by this control.
	 */
	wxString GetFileName() const;

	/**
	 * Look for a new line character in order to auto indent
	 *
	 * @param wxStyledTextCtrl &event the event
	 */
	void OnCharAdded(wxStyledTextEvent &event);

	/**
	 * highlight matching / unmatching braces if the current pos is a brace
	 *
	 * @param wxStyledTextEvent& event
	 */
	void OnUpdateUi(wxStyledTextEvent &event);

	/**
	 * Handle the margin clicks to toggle code folds
	 * @param wxStyledTextEvent event the event
	 */
	void OnMarginClick(wxStyledTextEvent& event);

	/**
	 * Selects the given position of text and expands the line if it is currently folded.
	 *
	 * @param int character position of start of selection
	 * @param int character position of end of selection
	 */
	void SetSelectionAndEnsureVisible(int start, int end);

	/**
	 * Selects the given position of text but does not expand the line if it is currently folded.
	 * Use this instead of SetSelection() method  when start, end are character positions not byte positions
	 * (useful when selecting results of a search by FinderClass, FindInFilesClass since they return character positions)
	 * This only matters during editing multi-byte (UTF-8) documents
	 *
	 * @param int character position of start of selection
	 * @param int character position of end of selection
	 */
	void SetSelectionByCharacterPosition(int start, int end);

	/**
	 * handles auto completion. This will differ based upon which document mode is loaded.
	 */
	void HandleAutoCompletion();

	/**
	 * shows the user function definition
	 *
	 * @param wxChar the character last inserted. if '(' (or force parameter is true) call tip will be activated. else, call tip may be left
	 * activated or deactivated depending on the char
	 * @param bool if true call tip will be activated.
	 *
	 */
	void HandleCallTip(wxChar ch = 0, bool force = false);

	/**
	 * Returns the resources that match the the current cursor position.
	 *
	 * @return resource matches
	 */
	std::vector<ResourceClass> GetCurrentSymbolResource();

	/**
	 * Applies the current prefernces to this window. This method should be called when the CodeControlOptions class
	 * has been modified by some outside code.
	 */
	void ApplyPreferences();

	/**
	 * Returns a string containing all of the contents of the code control.  The difference with GetText() is that this
	 * method accounts for high ascii characters correctly.
	 *
	 * ALWAYS USE THIS METHOD INSTEAD OF GetText()
	 * @return UnicodeString
	 */
	UnicodeString GetSafeText();

	/**
	 * Put an arrow in the margin of the line where the parse error ocurred.
	 * The markes will stay forever; to remove the markers
	 * we must explicitly call MarkerDelete to remove the markers.
	 */
	void MarkLintError(const LintResultsClass& result);

	/**
	 * Remove any and all markings caused by rendering the parse
	 * results. Markings are moved from this window only.
	 */
	void ClearLintErrors();

	/**
	 * Set the connection to use to fetch the SQL table metadata
	 */
	void SetCurrentInfo(const DatabaseInfoClass& other);

	/**
	 * Calculate the line number from the given CHARACTER position. Scintilla's
	 * LineFromPosition() works on bytes.
	 * 
	 * @param charPos character offset (no byte offset)
	 * @return int line number, zero-based
	 */
	int LineFromCharacter(int charPos);

private:

	/**
	 * Opens the file, loads its contents of the given file, and initializes the
	 * proper lexer so that the file gets syntax highlight according to the language
	 * it contains.  This load method is better than the wxStyledTextCtrl load
	 * method because it will keep track of the file modification times and
	 * prompt the user when a file has been modified outside of the editor.
	 *
	 * @param wxString filename full path of the file to load
	 * @param contents the file contents
	 */
	void LoadAndTrackFile(const wxString& filename);

//------------------------------------------------------------------------
// setting the various wxStyledTextCtrl options
// wxStyledTextCtrl is super-configurable.  These methods will turn on
// some sensible defaults for plain-text, PHP, HTML, and SQL editing.
//------------------------------------------------------------------------

	/**
	 * set the margin look of the source control
	 */
	void SetMargin();

	/**
	 * Set the font, EOL, tab options of the source control
	 * Set generic defaults for plain text editing.
	 */
	void SetCodeControlOptions(const std::vector<mvceditor::StylePreferenceClass>& styles);

	/**
	 * Set the PHP syntax highlight options. Note that since PHP is embedded the PHP options will be suitable for
	 * HTML and Javascript editing as well.
	 */
	void SetPhpOptions();

	/**
	 * Set the SQL highlight options of the source control
	 */
	void SetSqlOptions();

	/**
	 * Set the CSS highlight options of the source control
	 */
	void SetCssOptions();

	/**
	 * Set the font settings for plain text documents.
	 */
	void SetPlainTextOptions();

	/**
	 * Determine the correct document Mode and sets it (causing a repaint)
	 */
	void AutoDetectDocumentMode();

	/**
	 * indents if the given char is a new line character.  Handles windows and unix line endings.
	 *
	 * @param char ch the character that was added to the document
	 */
	void HandleAutomaticIndentation(char ch);

//------------------------------------------------------------------------
// word highlight feature
//------------------------------------------------------------------------
	/**
	 * Highlight the next matched word.
	 */
	void WordHiglightForwardSearch(wxIdleEvent& event);

	/**
	 * Highlight the previous matched word.
	 */
	void WordHiglightPreviousSearch(wxIdleEvent& event);

	/**
	 * Remove the exact matches style [that was added by the double click event] from all text.
	 */
	void UndoHighlight();


//------------------------------------------------------------------------
// Event handlers
//------------------------------------------------------------------------

	/**
	 * Handle the right-click event.
	 * @param wxContextMenuEvent& event the event
	 */
	void OnContextMenu(wxContextMenuEvent& event);

	/**
	 * Check to see if file has been modified outside of the editor.
	 */
	void OnIdle(wxIdleEvent& event);

	/**
	 * Handle the double clicks.  Highlight any exact matches of the double-clicked word.
	 * This is the trigger for the "word highlight" feature.
	 */
	void OnDoubleClick(wxStyledTextEvent& event);

	/**
	 * On a key press, we will undo the highlighting caused by double clicks
	 */
	void OnKeyDown(wxKeyEvent& event);

	/**
	 * On a left mouse click, we will undo the highlighting caused by double clicks
	 */
	void OnLeftDown(wxMouseEvent& event);

	/**
	 * show the symbol comment popup to the user
	 */
	void OnDwellStart(wxStyledTextEvent& event);

	/**
	 * Hide the symbool comment popup
	 */
	void OnDwellEnd(wxStyledTextEvent& event);

	/*
	* The file that was loaded.
	*/
	wxString CurrentFilename;

	/**
	 * The options to enable/disable various look & feel items
	 *
	 * @var CodeControlOptionsClass
	 */
	CodeControlOptionsClass& CodeControlOptions;

	/**
	  * Used by the word highlight feature to do the actual searching.
	  */
	FinderClass WordHighlightFinder;

	/**
	 * Used by the word highlight feature. The word being searched.
	 */
	UnicodeString WordHighlightWord;

	/**
	 * The connection to use to fetch the SQL table metadata.
	 */
	DatabaseInfoClass CurrentInfo;

	/**
	* This object will be used to parse the resources of files that are currently open.
	* This class will NOT own this pointer.
	*/
	ResourceUpdateThreadClass* ResourceUpdates;

	/**
	  * To help with autocompletion and keywords. This object will NOT own this pointer
	  *
	  * @var ProjectClass
	  */
	ProjectClass* Project;

	/**
	 * This is the current specialization (document type) that is being used. This
	 * pointer can be changed at any moment; the lifetime of the pointer may BE LESS
	 * than the code control.
	 * This object owns this pointer and will need to delete it.
	 */
	TextDocumentClass* Document;

	/**
	* Used by the word highlight feature. The location from where to start searching (back)
	*/
	int32_t WordHighlightPreviousIndex;

	/**
	 * Used by the word highlight feature. The location from where to start searching (forward)
	 */
	int32_t WordHighlightNextIndex;

	/**
	 * This is the style bit for the WordHighlight.  Since this code control handles multiple parsers
	 * and each parser can have more style bits, the indicator offsets will be different for different
	 * lexers.
	 *
	 * For example, the HTML lexer has 7 bits, so the first indicator style will be 128 (2 ^ 7)
	 * The SQL lexer has 5 bits, so the first indicator style will be 32 (2 ^ 5)
	 */
	int WordHighlightStyle;

	/**
	 * Detect when the 'Externally Modified' dialog is opened. In linux, the externally modified
	 * dialog kept popping up indefinitely.
	 *
	 * @var bool
	 */
	bool ModifiedDialogOpen;

	/**
	 * TRUE if the user double clicked a word and that word is not highlighted.
	 * @var bool
	 */
	bool WordHighlightIsWordHighlighted;

	/**
	 * Store the time the file was opened / last saved. We will use this to check to see if the file was modified
	 * externally.
	 *
	 * @var wxDateTime
	 */
	wxDateTime FileOpenedDateTime;

	/**
	 * The current rendering options being used.
	 * Each language will have its own options.
	 * @var Mode
	 */
	Mode DocumentMode;

	DECLARE_EVENT_TABLE()
};

}
#endif /*MVCEDITORCODECONTROLCLASS_H_*/
