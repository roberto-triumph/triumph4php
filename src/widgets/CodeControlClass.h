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
#ifndef MVCEDITORCODECONTROLCLASS_H_
#define MVCEDITORCODECONTROLCLASS_H_

#include <php_frameworks/ProjectClass.h>
#include <language/SymbolTableClass.h>
#include <language/LanguageDiscoveryClass.h>
#include <widgets/CodeControlOptionsClass.h>
#include <wx/stc/stc.h>
#include <unicode/unistr.h>

#include <vector>

/**
 * The source code editor.
 */
namespace mvceditor {

/**
 * this is the 'extra' functionality  that a plain text document has.
 * Plain text docouments will have auto complete, call tips, and
 * brace matching disabled.
 */ 	
class TextDocumentClass {
	
	public:

	TextDocumentClass();
	
	virtual ~TextDocumentClass();
	
	/**
	 * Since auto-completion can be a time consuming task we want to make
	 * it optional so that editing documents labeled as plain text can be as
	 * smooth as possible.
	 * 
	 * Sub classes can return TRUE here, but if they don then they must also
	 * implement HandleAutoComplete method.
	 * This method will be called in response to a user keypress; speed is
	 * crucial here.
	 * 
	 * For plain text documents this method returns false since we
	 * don't know what words to complete.
	 */
	virtual bool CanAutoComplete();
	
	/**
	 * Sub classes can implement their own logic for auto completion.
	 * This method will be called in response to a user keypress; speed is
	 * crucial here.
	 * @return a vector of strings, one item for each keyword to be 
	 * shown to the user.
	 */
	virtual std::vector<wxString> HandleAutoComplete(const wxString& fileName, const UnicodeString& code, const UnicodeString& word, bool force);

};

/**
 * this is a PHP specialization of a document.  It knows how to perform code
 * completion on PHP documents intelligently 
 */
class PhpDocumentClass : public TextDocumentClass {
public:

	/**
	 * This class will NOT own this pointer. Caller must manage (delete) it.
	 */
	PhpDocumentClass(ProjectClass* project);
	
	/**
	 * enable auto complete
	 */
	virtual bool CanAutoComplete();
	
	/**
	 * Use the project's resource finder to find auto complete suggestions
	 */
	virtual std::vector<wxString> HandleAutoComplete(const wxString& fileName, const UnicodeString& code, const UnicodeString& word, bool force);
	
private:

	/**
	 * handles auto completion for PHP.
	 * 
	 * @param bool force if true auto completion was triggered manually.
	 * @param syntax the token type that the cursor is currently on.  This helps
	 * int determining context (ie if the cursor is inside of a comment or string literal)
	 */
	std::vector<wxString> HandleAutoCompletionPhp(const wxString& fileName, const UnicodeString& code, const UnicodeString& word, bool force, mvceditor::LanguageDiscoveryClass::Syntax syntax);	
	 
	 /**
	 * handles auto completion for PHP.
	 * 
	 * @param bool force if true auto completion was triggered manually.
	 * @param syntax the token type that the cursor is currently on.  This helps
	 * int determining context (ie if the cursor is inside of a comment or string literal)
	 */
	std::vector<wxString> HandleAutoCompletionHtml(const UnicodeString& code, const UnicodeString& word, bool force, mvceditor::LanguageDiscoveryClass::Syntax syntax);
	
	/**
	 * Return a list of possible keyword matches for the given word. For example, if word="cl"
	 * then this method would return "class"
	 */
	std::vector<wxString> CollectNearMatchKeywords(wxString word);


	 /**
	  * In order to show the proper auto complete keywords we must know what language is 
	  * being edited at any given position.  This class will help in this regard.
	  */
	 mvceditor::LanguageDiscoveryClass LanguageDiscovery;
	 
	 /**
	 * To calculate variable information
	 * @var SymbolTableClass
	 */
	SymbolTableClass SymbolTable;

	/**
	 * This class will NOT own this pointer
	 */
	ProjectClass* Project;
	
};

/**
 * This is a SQL specialization of a document.  It knows how to code complete 
 * SQL keywords
 */
class SqlDocumentClass : public TextDocumentClass {
	
public:

	SqlDocumentClass();
	

	/**
	 * Will enable auto complete for SQL keywords
	 */
	virtual bool CanAutoComplete();
	
	/**
	 * Returns the keywords to be shown in the auto complete list.
	 */
	virtual std::vector<wxString> HandleAutoComplete(const wxString& fileName, const UnicodeString& code, const UnicodeString& word, bool force);
};

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
	 * Constructor. Memory management of project is left to the caller of this method. project
	 * CANNOT be NULL.
	 */
	CodeControlClass(wxWindow* parent, CodeControlOptionsClass& options, ProjectClass* project, 
		int id, const wxPoint& position = 
		wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
		const wxString& name = wxT("code"));
	
	/**
	 * Loads the contents of the given file, and initializes the
	 * proper lexer so that the file gets syntax highlight according to the language
	 * it containts.  This load method is better than the wxStyledTextCtrl load
	 * method because it will keep track of the file modification times and
	 * prompt the user when a file has been modified outside of the editor.
	 * 
	 * @param wxString filename full path of the file to load
	 * @return bool true id the filename was found and readable.
	 */
	bool LoadAndTrackFile(const wxString& filename);
	
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
	 * handles auto completion.
	 * 
	 * @param bool force if true auto completion is always triggered. By default, it is only triggered
	 * 	when the current word is greater than a certain length.
	 */
	void HandleAutoCompletion(bool force);	
	
	/**
	 * shows the user function definition
	 * 
	 * @param wxChar the character last inserted. if '(' call tip will be activated. else, call tip may be left
	 * activated or deactivated depending on the char
	 * 
	 */
	void HandleCallTip(wxChar ch = 0);
	
	/**
	 * Returns the symbol that is positioned in the current cursos position.
	 * 
	 * @return wxString a class name, or class name/method name, function name, or keyword.  The string is suitable
	 *         for passing to a ResourceFinderClass instance.
	 */
	wxString GetCurrentSymbol();
	
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
	 * @return wxString
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
	
private:

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
	void SetCodeControlOptions();
	
	/**
	 * Set the PHP syntax highlight options. Note that since PHP is embedded the PHP options will be suitable for
	 * HTML and Javascript editing as well.
	 */
	void SetPhpOptions();

	/**
	 * Set the SQL highlight options of the source control
	 */
	void SetSqlOptions();
	
	
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
	
	
//------------------------------------------------------------------------
// Brace matching and automatic indentation features
//------------------------------------------------------------------------
	/**
	 * If char at poistion pos ot (pos +1) is a brace, highlight the brace
	 * Otherwise, remove all brace highlights
	 * @param int posToCheck text position to look in
	 */
	void MatchBraces(int posToCheck);
	
	/**
	 * indents if the given char is a new line character.  Handles windows and unix line endings.
	 * 
	 * @param char ch the character that was added to the document
	 */
	void HandleAutomaticIndentation(char ch);

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
	 * cleanup of internal pointers
	 */
	void OnClose(wxCloseEvent& event);
	
//------------------------------------------------------------------------
// Auto complete feature
//------------------------------------------------------------------------
	/**
	 * Returns true if a  PHP variable is located at position pos 
	 * 
	 * @return bool
	 */
	bool PositionedAtVariable(int pos);

//------------------------------------------------------------------------
// Character conversion (UTF-8 <--> ICU) needed for proper string offsets
//------------------------------------------------------------------------	

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
	 * Returns the offset (byte) . This method is needed because Scintilla's "position" is byte-based not character
	 * based, while all other code (find, find in files etc...) return characters based positions.  This makes all the
	 * difference in the world for multi-byte documents, since internally Scintilla stores contents as UTF-8.
	 * 
	 * @param chracter position if this is more than the number of characters in the document then this method returns the last byte position.
	 * @return int byte position
	 */
	int CharacterToPos(int character);
	
	 /**
	 * To calculate variable information
	 * @var SymbolTableClass
	 */
	SymbolTableClass SymbolTable;
	 
	 /*
	 * The file that was loaded.
	 */
	wxString CurrentFilename;
	
	/**
	 * The resource signature currently being displayed in the calltip.
	 * 
	 * @var wxString
	 */
	wxString CurrentSignature;
	
	 /**
	  * The options to enable/disable various look & feel items
	  * 
	  * @var CodeControlOptionsClass
	  */
	 CodeControlOptionsClass& CodeControlOptions;
	 
	/**
	  * Used by the word highlight feature to do the actual searching.
	  */
	 mvceditor::FinderClass WordHighlightFinder;
	 
	 /**
	  * Used by the word highlight feature. The word being searched.
	  */
	 UnicodeString WordHighlightWord;

	/**
	  * To help with autocompletion and keywords
	  * 
	  * @var ProjectClass
	  */
	 ProjectClass* Project;
	 
	 /**
	  * This is the current specialization (document type) that is being used. This
	  * pointer can be changed at any moment; the lifetime of the pointer may BE LESS
	  * than the code control.
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
	  * The timestamp of the last time a character was added to the document. Will help with auto completion by preventing
	  * resource lookup when the user is typing really fast (an indication that the user does not need help)
	  */
	 wxLongLong LastCharAddedTime;	
	 
	 /**
	  * Store the time the file was opened / last saved. We will use this to check to see if the file was modified
	  * externally.
	  *
	  * @var wxDateTime
	  */
	 wxDateTime FileOpenedDateTime;
	 	
	DECLARE_EVENT_TABLE()
};

}
#endif /*MVCEDITORCODECONTROLCLASS_H_*/
