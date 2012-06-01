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

#include <php_frameworks/ProjectClass.h>
#include <pelet/LexicalAnalyzerClass.h>
#include <pelet/ParserClass.h>
#include <pelet/LanguageDiscoveryClass.h>
#include <language/SymbolTableClass.h>
#include <widgets/ResourceCacheClass.h>
#include <environment/EnvironmentClass.h>
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
	 * Returns the resources that match the the current cursor position.
	 *
	 * @return resource matches
	 */
	virtual std::vector<ResourceClass> GetCurrentSymbolResource();

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
	 * This class will NOT own any of these pointer. Caller must manage (delete) it.
	 */
	PhpDocumentClass(ProjectClass* project, ResourceCacheClass* resourceCache, EnvironmentClass* environment);

	~PhpDocumentClass();

	/**
	 * enable auto complete
	 */
	bool CanAutoComplete();

	/**
	 * Use the project's resource finder to find auto complete suggestions
	 */
	void HandleAutoCompletion(wxString& completeStatus);

	void HandleCallTip(wxChar ch = 0, bool force = false);

	std::vector<ResourceClass> GetCurrentSymbolResource();

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
	std::vector<ResourceClass> GetSymbolAt(int posToCheck);

	wxString GetPhpKeywords() const;
	
	wxString GetHtmlKeywords() const;
	
	wxString GetJavascriptKeywords() const;

private:

	/**
	 * handles auto completion for PHP.
	 *
	 * @param code most current source code; but only up to the current position (this helps determine
	 * the scope)
	 * @param completeStatus a bit of text that will help the user understand
	 *        why the complete box did not populate.
	 */
	void HandleAutoCompletionPhp(const UnicodeString& code, wxString& completeStatus);

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
	 * Fills completeStatus with a human-friendly version of the symbol table error
	 */
	void HandleAutoCompletionPhpStatus(const SymbolTableMatchErrorClass& error, 
		const UnicodeString& lastExpression, const pelet::ExpressionClass& parsedExpression,
		const ScopeResultClass& expressionScope, wxString& completeStatus);

	/**
	 * Return a list of possible keyword matches for the given word. For example, if word="cl"
	 * then this method would return "class"
	 */
	std::vector<wxString> CollectNearMatchKeywords(wxString word);

	/**
	 * This method will get called by the ResourceCache object when parsing of the
	 * code in this control has been completed.
	 */
	void OnResourceUpdateComplete(wxCommandEvent& event);
	
	/**
	 * This method will check to see if document is "dirty" and if so it will
	 * start re-parsing in the background
	 */
	void OnTimer(wxTimerEvent& event);

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

	/**
	 * When text is changed, we will update the resource cache if need be (retrigger the 
	 * buulding of the symbol table)
	 */
	void OnModification(wxStyledTextEvent& event);

	void RegisterAutoCompletionImages();

	/**
	 * Append the '(' for method calls that are completed
	 */
	void OnAutoCompletionSelected(wxStyledTextEvent& evt);

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
	std::vector<ResourceClass> CurrentCallTipResources;

	/**
	 * The resources that are shown in the code completion list. Keeping these around
	 * so that we can append the '(' for method calls.
	 */
	std::vector<mvceditor::ResourceClass> AutoCompletionResourceMatches;

	/**
	 * Used to control how often to check for resource re-parsing
	 */
	wxTimer Timer;

	/**
	 * A unique string used to identify this code control. This string is used in conjunction with 
	 * the ResourceCache object.
	 */
	wxString FileIdentifier;

	/**
	 * This class will NOT own this pointer
	 */
	ProjectClass* Project;
	
	/**
	 * This object will be used to parse the resources of files that are currently open.
	 * This class will NOT own this pointer
	 */
	ResourceCacheClass* ResourceCache;
	ResourceCacheUpdateThreadClass ResourceCacheUpdateThread;
	

	/**
	 * This class will NOT own this pointer
	 */
	EnvironmentClass* Environment;

	/**
	 * The resource signature currently being displayed in the calltip.
	 * index into CurrentCallTipResources
	 */
	size_t CurrentCallTipIndex;

		/**
	 * This flag will control whether the document is "dirty" and needs to be re-parsed
	 * This is NOT the same as GetModify() from scintilla; scintilla's Modify will be
	 * set to false if the user undoes changes; but if a user undoes changes we still
	 * want to trigger a re-parsing
	 */
	bool NeedToUpdateResources;

	/**
	 * TRUE if the auto complete images have been registered.
	 */
	bool AreImagesRegistered;


	DECLARE_EVENT_TABLE()
};

/**
 * This is a SQL specialization of a document.  It knows how to code complete
 * SQL keywords
 */
class SqlDocumentClass : public TextDocumentClass {

public:

	/**
	 * This class will NOT own this pointer. Caller must manage (delete) it.
	 * @param project the current project
	 * @param currentInfo the connection to fetch database metadata for (auto completion)
	 */
	SqlDocumentClass(ProjectClass* project, const DatabaseInfoClass& currentInfo);

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
	ProjectClass*  Project;

	DatabaseInfoClass CurrentInfo;
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