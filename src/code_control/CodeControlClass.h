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
#ifndef SRC_CODE_CONTROL_CODECONTROLCLASS_H_
#define SRC_CODE_CONTROL_CODECONTROLCLASS_H_

#include <pelet/ParserClass.h>
#include <unicode/unistr.h>
#include <wx/stc/stc.h>
#include <wx/timer.h>
#include <vector>
#include "globals/CodeControlOptionsClass.h"
#include "globals/FileTypeClass.h"
#include "language_php/PhpTagClass.h"
#include "language_sql/DatabaseTagClass.h"
#include "search/FinderClass.h"

namespace t4p {
// some forward declarations to prevent re-compilation as much as possible
// Since this file is included by many features whenever a change to any included header
// files is maded most features have to be re-compiled.
class TagCacheClass;
class EventSinkClass;
class GlobalsClass;


// forward declaration, defined below
class CodeControlClass;

/**
 * this event is generating by a code control when the user hovers over an
 * identifier while holding the ALT key down
 * The event will be of class type wxCommandEvent, GetInt() will return
 * the character posotion where the mouse is positioned at.
 */
extern const wxEventType EVT_MOTION_ALT;

// margin 0 is taken up by line numbers
// margin 1 is taken up by code folding.
// margin 2 for lint error markers
// margin 3 for search hits
extern const int CODE_CONTROL_LINT_RESULT_MARKER;
extern const int CODE_CONTROL_SEARCH_HIT_GOOD_MARKER;
extern const int CODE_CONTROL_SEARCH_HIT_BAD_MARKER;
extern const int CODE_CONTROL_BOOKMARK_MARKER;
extern const int CODE_CONTROL_EXECUTION_MARKER;
extern const int CODE_CONTROL_BREAKPOINT_MARKER;

// the indicator to show squiggly lines for lint errors
extern const int CODE_CONTROL_INDICATOR_PHP_LINT;

// the indicator to show boxes around found words when user double clicks
// on a word
extern const int CODE_CONTROL_INDICATOR_FIND;

// start stealing styles from "asp javascript" we will never use those styles
extern const int CODE_CONTROL_STYLE_PHP_LINT_ANNOTATION;

/**
 * A single item that the CodeCompletionProvider has suggested
 * based on the user input
 */
class CodeCompletionItemClass {
 public:
    /**
     * the string that will be shown to the user
     */
    wxString Label;

    /**
     * the string that will be added to the document
     */
    wxString Code;

    CodeCompletionItemClass();

    CodeCompletionItemClass(const t4p::CodeCompletionItemClass& src);

    t4p::CodeCompletionItemClass& operator=(const t4p::CodeCompletionItemClass& src);

    void Copy(const t4p::CodeCompletionItemClass& src);
};

/**
 * CodeControl instances will query CodeCompletionProviders to
 * get the code completion suggestions to show the user.
 *
 * Whenever the user triggers code completion, the code control
 * will loop through all of its providers, calling the provide()
 * method.  Then, the code control will show the collected suggestions
 * to the user.
 */
class CodeCompletionProviderClass {
 public:
    CodeCompletionProviderClass();

    virtual ~CodeCompletionProviderClass();

    /**
     * Sub-classes will implmement this method so that
     * the code control can avoid querying the provider
     * in case they do not support the file's language.
     * We do this because we want to be quick during code
     * completion and we want to cut un-necessary calls
     *
     * @return bool TRUE if this completion provider can
     *         add suggestions for the given file type
     */
    virtual bool DoesSupport(t4p::FileType type) = 0;

    /**
     * The code control will call this method when the user triggers
     * code completion.  This call happens in the main (GUI) thread,
     * so it needs to be fast to avoid "white-screens".
     *
     * Sub classes can implement their own logic for auto completion.
     * This method may be called in response to a user keypress; speed is
     * crucial here. Subclasses will need to invoke the AutoCompleteShow() method
     * of the wxSTC control.
     *
     * @param ctrl the control that contains the code. used to get the current position
     *        as well as the source code text
     * @param suggestions sub-classes will implement this method to add suggestions
     *        to show the user. Implementations of this method will call
     *        suggestions.push_back() (or equivalent methods)
     * @param completeStatus any errors to show the user; these are useful so that
     *        the user knows why completion did not suggest any items
     */
    virtual void Provide(t4p::CodeControlClass* ctrl, std::vector<t4p::CodeCompletionItemClass>& suggestions,
                         wxString& completeStatus) = 0;
};

/**
 * Code control instances will query call tip providers when
 * the user triggers a call tip
 */
class CallTipProviderClass {
 public:
    CallTipProviderClass();
    virtual ~CallTipProviderClass();

    /**
     * Sub-classes will implmement this method so that
     * the code control can avoid querying the provider
     * in case they do not support the file's language.
     * We do this because we want to be quick during code
     * completion and we want to cut un-necessary calls
     *
     * @return bool TRUE if this completion provider can
     *         add suggestions for the given file type
     */
    virtual bool DoesSupport(t4p::FileType type) = 0;

    /**
     *
     * @param ch the last character that the user typed in
     *        providers can use this to automatically show/hide
     *        the tip.
     * @param force TRUE if the user forced the tip
     * @param status any errors to show the user; these are useful so that
     *        the user knows why completion did not suggest any items
     */
    virtual void ProvideTip(t4p::CodeControlClass* ctrl, wxChar ch, bool force, wxString& status) = 0;
};

/**
 * code control instances will call brace match providers
 * to decorate matching braces.
 */
class BraceMatchStylerClass {
 public:
    BraceMatchStylerClass();
    virtual ~BraceMatchStylerClass();

    /**
     * Sub-classes will implmement this method so that
     * the code control can avoid querying the provider
     * in case they do not support the file's language.
     * We do this because we want to be quick during code
     * completion and we want to cut un-necessary calls
     *
     * @return bool TRUE if this completion provider can
     *         add suggestions for the given file type
     */
    virtual bool DoesSupport(t4p::FileType type) = 0;

    /**
     * If char at position (pos +1) is a brace, highlight the brace
     * Otherwise, remove all brace highlights
     *
     * @param ctrl the control to style
     * @param int posToCheck SCINTILLA position (byte offset) to look in
     */
    virtual void Style(t4p::CodeControlClass* ctrl, int postToCheck) = 0;
};

/**
 * source code control with the following enhancements.
 * - PHP autocompletion of structures found in the current project.
 * - Displaying of PHP call tips
 * - Automatic indentation
 */
class CodeControlClass : public wxStyledTextCtrl {
 public:
    /**
     * Constructor.
     * @param GlobalsClass* To get items needed for autocompletion. This object
     *        will NOT own the pointer
      * @param eventSink to send event to the application
     */
    CodeControlClass(wxWindow* parent, CodeControlOptionsClass& options,
                     GlobalsClass* globals, t4p::EventSinkClass& eventSink,
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
     * @param charset the character set the file will be written as when saved
     * @param bool TRUE if the file had a file signature BOM marker
     */
    void TrackFile(const wxString& filename, UnicodeString& contents, const wxString& charset,
                   bool hasSignature);

    /**
     * lets avoid calls like this
     *
     * UnicodeString contents  //....
     * wxStyledTextCtrl::SetText(t4p::IcuToWx(contents))
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
     * 	      control will be saved to. If empty, the existing name is used.
     *        The existing file will be overwritten if it exists on the disk.
     * @param willDestroy TRUE if this code control will be deleted after the file
     *        is save (ie. the user will close the file right after the save)
     *        we need to know this in case a file is saved asynchronously, we
     *        don't need to track the file in this case
     * @return bool true if the file was successfully saved.
     */
    bool SaveAndTrackFile(wxString filename, bool willDestroy);

    /**
     * mark a code control as having been saved to disk. this will
     * read the saved time from the file system, so that we can track
     * if the file has had changes while its open
     */
    void MarkAsSaved();

    /**
     * Set the options for this file.  This is auto-detected when using the LoadAndTrackFile() method
     * and will most likely not need to be called.
     *
     * @param type a file type; changes will take place immediately (causing a repaint)
     */
    void SetFileType(t4p::FileType type);

    /**
     * @return the current file type that this code control is using.  it could be the one
     * set by SetFileType() or it could have been detected.
     */
    t4p::FileType GetFileType();

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
     * Puts the caret at the start of the given line number
     * and expands the line if it is currently folded. Also,
     * the caret policy is changed so that the caret always
     * ends up in the middle of the screen.
     *
     * @param int lineNumber 1-based line number
     */
    void GotoLineAndEnsureVisible(int lineNumber);

    /**
     * Selects the given position of text but does not expand the line if it is currently folded.
     * Use this instead of SetSelection() method  when start, end are character positions not byte positions
     * (useful when selecting results of a search by FinderClass, FindInFilesClass since they return character positions)
     * This only matters during editing multi-byte (UTF-8) documents
     *
     * @param int character position of start of selection
     * @param int character position of end of selection
     * @param bool setPos if TRUE, then the cursor will be set at the start position
     */
    void SetSelectionByCharacterPosition(int start, int end, bool setPos);

    /**
     * @param provider the provider to register with this code control. After
     *        a provider is added, the code control will query the provider
     *        to get suggestions whenever the user triggers code completion.
     *        The given pointer will not be owned by this class; the provider
     *        should have at least the same lifetime as this code control.
     */
    void RegisterCompletionProvider(t4p::CodeCompletionProviderClass* provider);

    /**
     * @param provider the provider to register with this code control. After
     *        a provider is added, the code control will query the provider
     *        to get contents whenever the user triggers a call tip.
     *        The given pointer will not be owned by this class; the provider
     *        should have at least the same lifetime as this code control.
     */
    void RegisterCallTipProvider(t4p::CallTipProviderClass* provider);

    /**
     * @param styler the styler to register with this code control. After
     *        a styler is added, the code control will query the styler
     *        to highlight matching braces during update UI events.
     *        The given pointer will not be owned by this class; the styler
     *        should have at least the same lifetime as this code control.
     */
    void RegisterBraceMatchStyler(t4p::BraceMatchStylerClass* styler);

    /**
     * handles auto completion. This will differ based upon which file type is loaded.
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
     * Use this method whenever you need to get a UnicodeString that is being calculated from Scintilla
     * positions (GetCurrentPos(), GetWordStart(), etc...)
     * Scintilla uses UTF-8 encoding and the positions it returns are byte offsets not character offsets.
     * The method can be given integers where to put the resulting character indices if needed.
     *
     * @param int startPos byte offset
     * @param int endPos byte offset, EXCLUSIVE the character at endPos will NOT be included
     *
     */
    UnicodeString GetSafeSubstring(int startPos, int endPos);

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
     * @param goodHit if TRUE then we show a different marker than
     *        when hit is no longer at the expected pos
     */
    void MarkSearchHit(int lineNumber, bool goodHit);

    /**
     * Marks a search hit, sets the current position to the
     * position where the hit is located selects the hit
     * and makes sure the hit is visible
     * @param lineNumber 1-based
     * @param startPos character position
     * @param endPos character position
     * @param goodHit if TRUE then we show a different marker than
     *        when hit is no longer at the expected pos
     */
    void MarkSearchHitAndGoto(int lineNumber, int startPos, int endPos, bool goodHit);

    /**
     * Remove any and all markings caused by search hits.
     * Markings are moved from this window only.
     */
    void ClearSearchMarkers();

    /**
     * Marks a bookmark at the current line.
     * @param [out] line number that was bookmarked , 1-based
     * @param [out] int the bookmark "handle", used to query the
     *        the line of the bookmark when text has been added/removed
     *        from the document
     * @return bool TRUE if bookmark was added, false otherwise.
     *         will be false on out-of-memory error, invalid line number.
     */
    bool BookmarkMarkCurrent(int& lineNumber, int& handle);

    /**
     * Marks a bookmark at the given line.
     * @param  int line number to be bookmarked , 1-based
     * @param [out] int the bookmark "handle", used to query the
     *        the line of the bookmark when text has been added/removed
     *        from the document
     * @return bool TRUE if bookmark was added, false otherwise.
     *         will be false on out-of-memory error, invalid line number.
     */
    bool BookmarkMarkAt(int lineNumber, int& handle);

    /**
     * Remove a single marking caused by BookmarkMark() at the
     * given line
     * Markings are moved from this window only.
     *
     * @param  bookmark at line number will be removed, 1-based
     */
    void BookmarkClearAt(int lineNumber);

    /**
     * @param int the bookmark "handle", used to query the
     *        the line of the bookmark when text has been added/removed
     *        from the document
     * @return int line number where the bookmark is now located. 1-based
     */
    int BookmarkGetLine(int handle);

    /**
     * Remove all markings caused by BookmarkMark().
     * Markings are moved from this window only.
     */
    void BookmarkClearAll();

    /**
     * Marks the given line as the current line being executed.
     * All other marks of this type are removed from this control.
     * @param  int line number to be marked, 1-based
     * @return bool TRUE if mark was added, false otherwise.
     *         will be false on out-of-memory error, invalid line number.
     */
    bool ExecutionMarkAt(int lineNumber);

    /**
     * Removes the exection mark from this control.
     */
    void ExecutionMarkRemove();

    /**
     * Put a breakpoint marker at the given line
     * @param  int line number to be marked, 1-based
     * @param [out] int the bookmark "handle", used to query the
     *        the line of the bookmark when text has been added/removed
     *        from the document
     * @return bool TRUE if mark was added, false otherwise.
     *         will be false on out-of-memory error, invalid line number.
     */
    bool BreakpointMarkAt(int lineNumber, int& handle);

    /**
     * Clears the breakpoint marker at the given line
     * @param  int line number to be marked, 1-based
     * @return bool TRUE if mark was added, false otherwise.
     *         will be false on out-of-memory error, invalid line number.
     */
    void BreakpointRemove(int lineNumber);

    /**
     * Remove all markings caused by BreakpointMarkAt().
     * Markings are moved from this window only.
     */
    void BreakpointRemoveAll();

    /**
     * @param int the bookmark "handle", used to query the
     *        the line of the breakpoint when text has been added/removed
     *        from the document
     * @return int line number where the breakpoint is now located. 1-based
     */
    int BreakpointGetLine(int handle);

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
     * Determine the correct file type (based on extension) and sets it (causing a repaint)
     */
    void AutoDetectFileType();

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
     * capture the SCN_MODIFIED event, to propagate it
     * to the rest of the app
     */
    void OnModified(wxStyledTextEvent& event);

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

    /**
     * this method will publish events to the EventSink
     * its easier to just publish to the event sink
     * since features are automatically connected to the
     * event sink; otherwise each feature would have to
     * connect to the wxStyledTextCtrl events manually
     */
    void PropagateToEventSink(wxStyledTextEvent& event);

    /**
     * When a new code control has been focused on propagate it
     * to the event sink.
     */
    void OnSetFocus(wxFocusEvent& event);

    /*
    * The file that was loaded.
    */
    wxString CurrentFilename;

    /**
     *  used to get auto completion suggestions.
     *  this class will not own these pointers.
     */
    std::vector<t4p::CodeCompletionProviderClass*> CompletionProviders;

    /**
     *  used to get call tip contents.
     *  this class will not own these pointers.
     */
    std::vector<t4p::CallTipProviderClass*> CallTipProviders;

    /**
     *  used to style brace matching.
     *  this class will not own these pointers.
     */
    std::vector<t4p::BraceMatchStylerClass*> BraceMatchStylers;

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
     * @var Type
     */
    t4p::FileType Type;

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

    /**
     * if true, then the file will be written with a file signature
     * BOM marker
     */
    bool HasFileSignature;

    /**
     *  thecharacter set that the file will be written as
     * when saved
     */
    wxString Charset;

    DECLARE_EVENT_TABLE()
};
}  // namespace t4p

#endif  // SRC_CODE_CONTROL_CODECONTROLCLASS_H_
