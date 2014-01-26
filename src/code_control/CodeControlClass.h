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

#include <globals/CodeControlOptionsClass.h>
#include <search/FinderClass.h>
#include <globals/TagClass.h>
#include <globals/DatabaseTagClass.h>
#include <pelet/ParserClass.h>
#include <wx/stc/stc.h>
#include <wx/timer.h>
#include <unicode/unistr.h>

#include <vector>

/**
 * The source code editor.
 */
namespace mvceditor {

// some forward declarations to prevent re-compilation as much as possible
// Since this file is included by many features whenever a change to any included header
// files is maded most features have to be re-compiled.
class TextDocumentClass;
class TagCacheClass;
class EventSinkClass;
class GlobalsClass;

/**
 * this event is generating by a code control when the user hovers over an
 * identifier while holding the ALT key down
 * The event will be of class type wxCommandEvent, GetInt() will return
 * the character posotion where the mouse is positioned at.
 */
extern const wxEventType EVT_MOTION_ALT;

// margin 0 is taken up by line numbers, margin 1 is taken up by code folding. use
// margin 2 for lint error markers, margin 3 got search hits
extern const int CODE_CONTROL_LINT_RESULT_MARKER;
extern const int CODE_CONTROL_LINT_RESULT_MARGIN;
extern const int CODE_CONTROL_SEARCH_HIT_MARKER;
extern const int CODE_CONTROL_SEARCH_HIT_MARGIN;


// the indicator to show squiggly lines for lint errors
extern const int CODE_CONTROL_INDICATOR_PHP_LINT;

// the indicator to show boxes around found words when user double clicks
// on a word
extern const int CODE_CONTROL_INDICATOR_FIND;

// start stealing styles from "asp javascript" we will never use those styles
extern const int CODE_CONTROL_STYLE_PHP_LINT_ANNOTATION;


/**
 * source code control with the following enhancements.
 * - PHP autocompletion of structures found in the current project.
 * - Displaying of PHP call tips
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
		 * The full functionality code completion, call tips, syntax highlighting, the works
		 */
		PHP,

		/**
		 * Code completion and SQL syntax highlighting
		 */
		SQL,

		/**
		 * CSS style sheets (pure CSS files only)
		 */
		CSS,

		/**
		 * Javascript (pure JS files only)
		 */
		JS,
		
		// the rest of the document types are not slightly supported
		// syntax highlighting works but not much else
		CONFIG,
		CRONTAB,
		YAML,
		XML,
		RUBY,
		LUA,
		MARKDOWN,
		BASH,
		DIFF
	};

	/**
	 * Constructor. 
	 * @param GlobalsClass* To get items needed for autocompletion. This object
	 *        will NOT own the pointer
	  * @param eventSink to send event to the application
	 */
	CodeControlClass(wxWindow* parent, CodeControlOptionsClass& options,
		GlobalsClass* globals, mvceditor::EventSinkClass& eventSink,
	                 wxWindowID id, const wxPoint& position =
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
	 * @return wxString the full path to the file that is shown by this control.
	 */
	wxString GetFileName() const;

	/**
	 * @retrurn wxDateTime the modification time at the file was opened
	 */
	wxDateTime GetFileOpenedDateTime() const;

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
	 * Returns the tags that match the identifier located at the current cursor position.
	 * For example, if the document holds:
	 *
	 * <?php pretty_print(array(1, 2, 3)) ?>
	 *
	 * and the cursor is in position 8, this method returns the tags for the "pretty_print" function.
	 *
	 * @return tag matches
	 */
	std::vector<TagClass> GetTagsAtCurrentPosition();

	/**
	 * Returns the tags that match the identifier located at the given cursor position.
	 * For example, if the document holds:
	 *
	 * <?php pretty_print(array(1, 2, 3)) ?>
	 *
	 * and the cursor given is 8, this method returns the tags for the "pretty_print" function.
	 *
	 * @param pos character position, zero-based
	 * @return tag matches
	 */
	std::vector<TagClass> GetTagsAtPosition(int pos);

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
	 * The markers will stay forever; to remove the markers
	 * we must explicitly call MarkerDelete to remove the markers.
	 */
	void MarkLintError(const pelet::LintResultsClass& result);

	/**
	 * Marks a lint error AND set the current position to the
	 * position where the error is located
	 */
	void MarkLintErrorAndGoto(const pelet::LintResultsClass& result);

	/**
	 * Remove any and all markings caused by rendering the parse
	 * results. Markings are moved from this window only.
	 */
	void ClearLintErrors();
	
	/**
	 * Put an arrow in the margin of the line where a match ocurred
	 * The markers will stay until the user types in one character
	 * @param lineNumber 1-based
	 */
	void MarkSearchHit(int lineNumber);

	/**
	 * Marks a search hit, sets the current position to the
	 * position where the hit is located selects the hit
	 * and makes sure the hit is visible
	 * @param lineNumber 1-based
	 * @param startPos uft-8 position 
	 * @param endPos uft-8 position 
	 */
	void MarkSearchHitAndGoto(int lineNumber, int startPos, int endPos);

	/**
	 * Remove any and all markings caused by search hits.
	 * Markings are moved from this window only.
	 */
	void ClearSearchMarkers();

	/**
	 * Set the connection to use to fetch the SQL table metadata
	 */
	void SetCurrentDbTag(const DatabaseTagClass& other);

	/**
	 * Calculate the line number from the given CHARACTER position. Scintilla's
	 * LineFromPosition() works on bytes.
	 * 
	 * @param charPos character offset (no byte offset)
	 * @return int line number, zero-based
	 */
	int LineFromCharacter(int charPos);

	/**
	 * Sets this control as 'hidden'; doesn't do anything but affect call tips at the
	 * moment.
	 * The notebook should call this method as it effectively disables call tips from
	 * showing on the dwell notification
	 */
	void SetAsHidden(bool isHidden);

	/**
	 * @return the ID as a string; this is guaranteed to be a unique identifier. This string 
	 *  can be used to track code control windows; GetFileName() cannot be used in these
	 *  cases since FileName can be empty string for new files.
	 *  The string is guaranteed to be unique even across program instances (it includes
	 *  the programs PID).
	 */
	wxString GetIdString() const;

	/**
	 * this method will 'untrack' a file; meaning that the editor will assume the file is not
	 * yet in the file system and will ask the user to save the file when the file is closed.
	 */
	void TreatAsNew();

	/**
	 * update the control's opened time to the gven time.  this will make the file checking code ignore
	 * any previous external modifications
	 */
	void UpdateOpenedDateTime(wxDateTime openedDateTime);

	/**
	 * a "touched" code control is one that has been modified at since the last time
	 * SetTouched(false) was called.  touched is like modified, except that touched
	 * works on 
	 */
	bool Touched() const;
	void SetTouched(bool touched);
	
	/**
	 * Highlights the specified portion of the document.
	 *
	 * @param int byte position of start of highlighted section
	 * @param int byte position of end of highlighted section
	 */
	void HighlightWord(int utf8Start, int utf8Length);
	
	/**
	 * @return UnicodeString the word that is located at the current position
	 */
	UnicodeString WordAtCurrentPos();

	/**
	 * The options to enable/disable various look & feel items
	 *
	 * @var CodeControlOptionsClass
	 */
	CodeControlOptionsClass& CodeControlOptions;


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
	 * Handle the double clicks.
	 */
	void OnDoubleClick(wxStyledTextEvent& event);
	
	/**
	 *  on mouse motion we will turn on hotspots for identifiers 
	 */
	void OnMotion(wxMouseEvent& event);

	/**
	 * On a key press, we will undo the highlighting caused by double clicks
	 */
	void OnKeyDown(wxKeyEvent& event);

	/**
	 * On a left mouse click, we will undo the highlighting caused by double clicks
	 */
	void OnLeftDown(wxMouseEvent& event);

	/**
	 * On a left mouse click, we will trigger the hotspot click
	 */
	void OnLeftUp(wxMouseEvent& event);

	/**
	 * when the user clicks on a hotspot (matched method, class)
	 * just activate the timer, do not process the event in place.
	 * see the comment on the HotspotTimer
	 */
	void OnHotspotClick(wxStyledTextEvent& event);

	/**
	 * when the timer ends then jump to that tag
	 */
	void OnTimerComplete(wxTimerEvent& event);

	/**
	 * Removes trailing space from ALL lines in this document.
	 */
	void TrimTrailingSpaces();
	
	/**
	 * Removes any trailing empty lines.
	 * This method will only modify PHP files.
	 */
	void RemoveTrailingBlankLines();

	/*
	* The file that was loaded.
	*/
	wxString CurrentFilename;

	/**
	 * The connection to use to fetch the SQL table metadata.
	 */
	DatabaseTagClass CurrentDbTag;

	/**
	 * we will handle hotspot clicks in a timer.  This is because if we handle
	 * a hotspot that scrolls to a different place in the same file, we want to avoid
	 * selecting the text that occurs because scintilla processes the left click 
	 * event as well as the hotspot click; 
	 * Example: there is a hotspot on $this->myMethod
	 * - User clicks on myMethod
	 * - we get the tag, and scroll down to functino definition and select the function
	 * - scintilla will recognize the mouse click and will perform the default behavio
	 *   which is to set the anchor and select the text. the text we selected preivously
	 *   is no longer selectec. this is the behavior we want to avoid
	 *
	 * See https://groups.google.com/d/msg/scintilla-interest/XY1sKYBtGj0/ImtO5NRjLcsJ
	 *
	 */
	wxTimer HotspotTimer;

	/**
	* This object will be used to parse the resources of files that are currently open
	* and to use the proper version of PHP for auto completion.
	* This class will NOT own this pointer.
	*/
	GlobalsClass* Globals;

	/**
	 * to send file open commands if the user clicks on a hotspot
	 */
	EventSinkClass& EventSink;

	/**
	 * This is the current specialization (document type) that is being used. This
	 * pointer can be changed at any moment; the lifetime of the pointer may BE LESS
	 * than the code control.
	 * This object owns this pointer and will need to delete it.
	 */
	TextDocumentClass* Document;

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

	/**
	 * If TRUE, then this code control is treated as hidden and call tips 
	 * will not show when the Mouse Dwell notification is received.
	 */
	bool IsHidden;

	/**
	 * if true the user has added/removed/modified at least one character since
	 * this control was last set to touched = false
	 */
	bool IsTouched;
	
	/**
	 * if true then this control has at least one search marker visible
	 */
	bool HasSearchMarkers;

	DECLARE_EVENT_TABLE()
};

}
#endif /*MVCEDITORCODECONTROLCLASS_H_*/
