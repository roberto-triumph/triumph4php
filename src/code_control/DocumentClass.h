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
#ifndef __MVCEDITOR_DOCUMENTCLASS_H__
#define __MVCEDITOR_DOCUMENTCLASS_H__

#include <globals/ProjectClass.h>
#include <pelet/LexicalAnalyzerClass.h>
#include <pelet/ParserClass.h>
#include <pelet/LanguageDiscoveryClass.h>
#include <language/SymbolTableClass.h>
#include <code_control/ResourceCacheBuilderClass.h>
#include <globals/GlobalsClass.h>
#include <wx/string.h>
#include <wx/stc/stc.h>
#include <unicode/unistr.h>

namespace mvceditor {

// forward declaration; prevent recursive dependencies
class CodeControlClass;

/**
 * this is the 'extra' functionality  that a plain text document has.
 * Plain text docouments will have auto complete, call tips, and
 * brace matching disabled.
 */
class TextDocumentClass {

public:

	TextDocumentClass();

	/**
	 * @param ctrl The 'raw' text control, used 
	 * This object will NOT own the pointer
	 */
	void SetControl(CodeControlClass* ctrl);

	virtual ~TextDocumentClass();

	/**
	 * Since auto-completion can be a time consuming task we want to make
	 * it optional so that editing documents labeled as plain text can be as
	 * smooth as possible.
	 *
	 * Sub classes can return TRUE here, but if they don then they must also
	 * implement HandleAutoCompletion method.
	 * This method will be called in response to a user keypress; speed is
	 * crucial here.
	 *
	 * For plain text documents this method returns false since we
	 * don't know what words to complete.
	 */
	virtual bool CanAutoComplete();

	/**
	 * Sub classes can implement their own logic for auto completion.
	 * This method may be called in response to a user keypress; speed is
	 * crucial here. Subclasses will need to invoke the AutoCompleteShow() method
	 * of the wxSTC control.
	 * @param completeStatus a bit of text that will help the user understand
	 *        why the complete box did not populate.
	 */
	virtual void HandleAutoCompletion(wxString& completeStatus);

	/**
	 * shows the user function definition
	 *
	 * @param wxChar the character last inserted. if '(' (or force parameter is true) call tip will be activated. else, call tip may be left
	 * activated or deactivated depending on the char
	 * @param bool if true call tip will be activated.
	 *
	 */
	virtual void HandleCallTip(wxChar ch = 0, bool force = false);

	/**
	 * Returns the tags that matched the identifier at the  current position
	 *
	 * @return tag matches
	 */
	virtual std::vector<TagClass> GetTagsAtCurrentPosition();

	/**
	 * Returns the tags that matched the identifier in the given position
	 *
	 * @param int the character position in the document to check
	 * @return tag matches
	 */
	virtual std::vector<TagClass> GetTagsAtPosition(int position);

	/**
	 * This method will get called when a new file is opened OR when a 
	 * file is reverted OR when a file is saved with a new file name (Save As...)
	 * @param fileName the file that was opened
	 */
	virtual void FileOpened(wxString fileName);

	/**
	 * If char at position (pos +1) is a brace, highlight the brace
	 * Otherwise, remove all brace highlights
	 * @param int posToCheck SCINTILLA position (byte offset) to look in
	 */
	virtual void MatchBraces(int posToCheck);

	/**
	 * subclasses should connect to Styled Text events in this method
	 */
	virtual void AttachToControl(CodeControlClass* ctrl);

	/**
	 * subclasses should disconnect to Styled Text events in this method
	 */
	virtual void DetachFromControl(CodeControlClass* ctrl);


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

protected:

	/**
	 * The 'raw' text control that this document is attached to; Ctrl will never be NULL
	 * (except in the constructor)
	 * This object will NOT own the pointer
	 */
	 CodeControlClass* Ctrl;

};

/**
 * this is a PHP specialization of a document.  It knows how to perform code
 * completion on PHP documents intelligently
 */
class PhpDocumentClass : public wxEvtHandler, public TextDocumentClass {
public:

	/**
	 * @param globals This class will NOT own this pointer. Caller must manage (delete) it.
	 *   globals helps with autocompletion
	 */
	PhpDocumentClass(mvceditor::GlobalsClass* globals);

	~PhpDocumentClass();

	/**
	 * enable auto complete
	 */
	bool CanAutoComplete();

	/**
	 * Use the project's tag finder to find auto complete suggestions
	 */
	void HandleAutoCompletion(wxString& completeStatus);

	void HandleCallTip(wxChar ch = 0, bool force = false);

	std::vector<TagClass> GetTagsAtCurrentPosition();

	std::vector<TagClass> GetTagsAtPosition(int position);

	void FileOpened(wxString fileName);

	void MatchBraces(int posToCheck);

	virtual void AttachToControl(CodeControlClass* ctrl);

	virtual void DetachFromControl(CodeControlClass* ctrl);

	/**
	 * Returns the resolved Resource[s] that is positioned in the given cursor position.
	 * Note that posToCheck must point to the END of the word to be looked up.
	 *
	 * @param int posToCheck a SCINTILLA POSITION (ie. BYTES not characters)
	 * @return list of resources that the symbol can be
	 */
	std::vector<TagClass> GetSymbolAt(int posToCheck);

	wxString GetPhpKeywords() const;
	
	wxString GetHtmlKeywords() const;
	
	wxString GetJavascriptKeywords() const;

private:

	/**
	 * handles auto completion for PHP.
	 *
	 * @param code most current source code; but only up to the current position (this helps determine
	 * the scope)
	 * @param word the word to complete
	 * @param syntax the token type that the cursor is currently on.  This helps
	 *        in determining context (ie if the cursor is inside of a string)
	 * @param completeStatus a bit of text that will help the user understand
	 *        why the complete box did not populate.
	 */
	void HandleAutoCompletionPhp(const UnicodeString& code, const UnicodeString& word, pelet::LanguageDiscoveryClass::Syntax syntax, wxString& completeStatus);

	/**
	* handles auto completion for HTML.
	* 
	* @param word the word to complete
	* @param syntax the token type that the cursor is currently on.  This helps
	*        in determining context (ie if the cursor is inside of a tag name or a tag value)
	* @param completeStatus a bit of text that will help the user understand
	*        why the complete box did not populate.
	*/
	void HandleAutoCompletionHtml(const UnicodeString& word, pelet::LanguageDiscoveryClass::Syntax syntax,
		wxString& completeStatus);
		
	/**
	 * handles auto completion of stufff inside a PHP (single quote) string 
	 * @param word the word to complete
	 * @param completeStatus a bit of text that will help the user understand
	 *        why the complete box did not populate.
	 */
	void HandleAutoCompletionString(const UnicodeString& word, wxString& completeStats);

	/**
	 * Fills completeStatus with a human-friendly version of the symbol table error
	 */
	void HandleAutoCompletionPhpStatus(const SymbolTableMatchErrorClass& error, 
		const UnicodeString& lastExpression, const pelet::VariableClass& parsedVariable,
		const pelet::ScopeClass& variableScope, wxString& completeStatus);

	/**
	 * Return a list of possible keyword matches for the given word. For example, if word="cl"
	 * then this method would return "class"
	 */
	std::vector<wxString> CollectNearMatchKeywords(wxString word);

	/**
	 * Handle the call tip up/down arrow events
	 */
	void OnCallTipClick(wxStyledTextEvent& evt);

	/**
	 * Check to see if the given position is at a PHP comment or style.
	 * This is a quick-check that doesn't do any parsing it relies on the scintiall styling only
	 * (this method will return true if the position is colored as a string or comment.)
	 * This implementation should probably change in the future.
	 *
	 * @param int posToCheck the scintilla position (byte) to check
	 * @return bool TRUE if the position is at a PHP comment or PHP string
	 */
	bool InCommentOrStringStyle(int posToCheck);

	void RegisterAutoCompletionImages();

	/**
	 * Append the '(' for method calls that are completed
	 */
	void OnAutoCompletionSelected(wxStyledTextEvent& evt);
	
	/**
	 * share code between HandleAutoCompletionString and HandleAutoCompletionPhp since
	 * we want to autocomplete sql table and column names on both single and double quoted
	 * string, but we want to keep autocompletion of PHP variables in 
	 * double-quoted strings only
	 */
	void AppendSqlTableNames(const UnicodeString& word, std::vector<wxString>& matches);

	/**
	 * In order to show the proper auto complete keywords we must know what language is
	 * being edited at any given position.  This class will help in this regard.
	 */
	pelet::LanguageDiscoveryClass LanguageDiscovery;

	/**
	* To parse a code snippet to make it suitable for code completion
	* @var pelet::ParserClass
	*/
	pelet::ParserClass Parser;

	/**
	 * To get the last expression in the source code
	 * @var pelet::LexicalAnalyzerClass
	 */
	pelet::LexicalAnalyzerClass Lexer;

	/**
	 * This will be used to find the scope of any position in a piece of PHP code
	 * @var ScopeFinderClass
	 */
	ScopeFinderClass ScopeFinder;

	/**
	 * The resources used to populate the call tips
	 */
	std::vector<TagClass> CurrentCallTipResources;

	/**
	 * The resources that are shown in the code completion list. Keeping these around
	 * so that we can append the '(' for method calls.
	 */
	std::vector<mvceditor::TagClass> AutoCompletionResourceMatches;

	/**
	 * To access any global structures: the tag cache, template variables
	 * This class will NOT own this pointer
	 */
	GlobalsClass* Globals;
	
	/**
	 * The tag signature currently being displayed in the calltip.
	 * index into CurrentCallTipResources
	 */
	size_t CurrentCallTipIndex;

	/**
	 * TRUE if the auto complete images have been registered.
	 */
	bool AreImagesRegistered;

};

/**
 * This is a SQL specialization of a document.  It knows how to code complete
 * SQL keywords
 */
class SqlDocumentClass : public TextDocumentClass {

public:

	/**
	 * @param globals This class will NOT own this pointer. Caller must manage (delete) it.
	 *   globals helps with autocompletion
	 * @param currentDbTag the connection to fetch database metadata for (auto completion)
	 */
	SqlDocumentClass(GlobalsClass* globals, const DatabaseTagClass& currentDbTag);

	/**
	 * Will enable auto complete for SQL keywords and SQL table metadata
	 */
	virtual bool CanAutoComplete();

	/**
	 * Searches the current project's SqlResourceFinder to find SQL keywords and metadata that completes the given word.
	 * Returns the keywords to be shown in the auto complete list.
	 */
	void HandleAutoCompletion(wxString& completeStatus);
	
	wxString GetMySqlKeywords() const;
	
	/**
	 * Match any parenthesis
	 */
	void MatchBraces(int posToCheck);

	/**
	 * check to see if the give pos is at a SQL comment or SQL string
	 */
	bool InCommentOrStringStyle(int posToCheck); 

private:

	std::vector<wxString> HandleAutoCompletionMySql(const UnicodeString& word);

	/**
	 * This class will NOT own this pointer
	 */
	GlobalsClass*  Globals;

	/**
	 * the connection to fetch database metadata for (auto completion)
	 */
	DatabaseTagClass CurrentDbTag;
};

class CssDocumentClass : public TextDocumentClass {

public:

	CssDocumentClass();

	bool CanAutoComplete();
	
	wxString GetCssKeywords() const;
	
	wxString GetCssPseudoClasses() const;

	/**
	 * Match any parenthesis or braces
	 */
	void MatchBraces(int posToCheck);

	/**
	 * check to see if the give pos is at a CSS comment or CSS string
	 */
	bool InCommentOrStringStyle(int posToCheck); 

};

}



#endif