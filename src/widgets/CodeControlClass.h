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
#include <language/ParserClass.h>
#include <language/LanguageDiscoveryClass.h>
#include <PreferencesClass.h>
#include <wx/config.h>
#include <wx/fdrepdlg.h>
#include <wx/stc/stc.h>
#include <unicode/unistr.h>

#include <vector>

/**
 * The source code editor.
 */
namespace mvceditor {
	
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
	 * lexer so that the file gets syntax highlight
	 * 
	 * @param wxString filename full path of the file to load
	 * @return bool true id the filename was found and readable.
	 */
	bool LoadPhpFile(const wxString& filename);
	
	/**
	 * Returns true if the this control was NOT loaded with an existing
	 * file.
	 * 
	 * @return bool true if the file was NOT loaded using LoadPhpFile method
	 */
	bool IsNew() const;
	
	/**
	 * If filename is not give, saves the contents of the code control under 
	 * the name given in LoadPhpFile().  IfLoadPhpFile was not called, then
	 * caller should supply a filename and this method will create the new 
	 * file
	 * 
	 * @param wxString filename the full path where the contents of the code 
	 * 	control will be saved to.  Existing file will be overwritten.
	 * @return bool true if the file was successfully saved.
	 */
	bool SavePhpFile(wxString filename = wxT(""));

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

	/**
	 * set the margin look of the source control
	 */
	void SetMargin();
	
	/**
	 * Set the PHP syntax highlight options
	 */
	void SetPhpOptions();
	
	/**
	 * Set the HTML syntax highlight options of the source control
	 */
	void SetHtmlOptions();

	/**
	 * Set the JavaScript highlight options of the source control
	 */
	void SetJavascriptOptions();
	
	/**
	 * Set the font, EOL, tab options of the source control
	 */
	void SetCodeControlOptions();
	
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

	/**
	 * Return a list of possible keyword matches for the given word. For example, if word="cl"
	 * then this method would return "class"
	 */
	std::vector<wxString> CollectNearMatchKeywords(wxString word);

	/**
	 * Handle the right-click event.
	 * @param wxContextMenuEvent& event the event
	 */
	void OnContextMenu(wxContextMenuEvent& event);
	
	/**
	 * Returns true if a  PHP variable is located at position pos 
	 * 
	 * @return bool
	 */
	bool PositionedAtVariable(int pos);
	
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
	 * Highlight the next matched word.
	 */
	void WordHiglightForwardSearch(wxIdleEvent& event);
	
	/**
	 * Highlight the previous matched word.
	 */
	void WordHiglightPreviousSearch(wxIdleEvent& event);

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
	 * Remove the exact matches style [that was added by the double click event] from all text. 
	 */
	void UndoHighlight();
	
	/**
	 * Use this method whenever you need to get a UnicodeString that is being calculated from Scintilla
	 * positions (GetCurrentPos(), GetWordStart(), etc...)
	 * Scintilla uses UTF-8 encoding and the positions it returns are byte offsets not character offsets.
	 * The method can be given integers where to put the resulting character indices if needed.
	 * 
	 * @param int startPos byte offset 
	 * @param int endPos byte offset, EXCLUSIVE the character at endPos will NOT be included
	 * @param int* charStartIndex character start position. This is the character position within the opened file.
	 * @param int* charEndIndex character end position. This is the character position within the opened file
	 * 
	 */
	UnicodeString GetSafeSubstring(int startPos, int endPos, int* charStartIndex = NULL, int* charEndIndex = NULL);
	
	/**
	 * handles auto completion for PHP.
	 * 
	 * @param bool force if true auto completion was triggered manually.
	 */
	void HandleAutoCompletionPhp(bool force);	
	 
	 /**
	 * handles auto completion for PHP.
	 * 
	 * @param bool force if true auto completion was triggered manually.
	 */
	void HandleAutoCompletionHtml(bool force);
	
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
	  * The timestamp of the last time a character was added to the document. Will help with auto completion by preventing
	  * resource lookup when the user is typing really fast (an indication that the user does not need help)
	  */
	 wxLongLong LastCharAddedTime;	
	 
	 /**
	  * The options to enable/disable various look & feel items
	  * 
	  * @var CodeControlOptionsClass
	  */
	 CodeControlOptionsClass& CodeControlOptions;

	 /**
	  * Store the time the file was opened / last saved. We will use this to check to see if the file was modified
	  * externally.
	  *
	  * @var wxDateTime
	  */
	 wxDateTime FileOpenedDateTime;
	 
	 /**
	  * Used by the word highlight feature to do the actual searching.
	  */
	 mvceditor::FinderClass WordHighlightFinder;
	 
	 /**
	  * Used by the word highlight feature. The word being searched.
	  */
	 UnicodeString WordHighlightWord;
	 
	 /**
	  * In order to show the proper auto complete keywords we must know what language is 
	  * being edited at any given position.  This class will help in this regard.
	  */
	 mvceditor::LanguageDiscoveryClass LanguageDiscovery;
	 
	 /**
	  * Used by the word highlight feature. The location from where to start searching (back)
	  */
	 int32_t WordHighlightPreviousIndex;
	
	 /**
	  * Used by the word highlight feature. The location from where to start searching (forward)
	  */
	 int32_t WordHighlightNextIndex;
	 
	 /**
	  * To help with autocompletion and keywords
	  * 
	  * @var ProjectClass
	  */
	 ProjectClass* Project;
	 
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
	 	
	DECLARE_EVENT_TABLE()
};

}
#endif /*MVCEDITORCODECONTROLCLASS_H_*/
