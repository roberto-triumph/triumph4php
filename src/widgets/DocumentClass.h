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
#include <language/LexicalAnalyzerClass.h>
#include <language/ParserClass.h>
#include <language/LanguageDiscoveryClass.h>
#include <widgets/ResourceUpdateThreadClass.h>
#include <wx/string.h>
#include <unicode/unistr.h>

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
	virtual std::vector<wxString> HandleAutoComplete(const wxString& fileName, const UnicodeString& code, const UnicodeString& word);

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
	PhpDocumentClass(ProjectClass* project, ResourceUpdateThreadClass* resourceUpdates);

	/**
	 * enable auto complete
	 */
	virtual bool CanAutoComplete();

	/**
	 * Use the project's resource finder to find auto complete suggestions
	 */
	virtual std::vector<wxString> HandleAutoComplete(const wxString& fileName, const UnicodeString& code, const UnicodeString& word);
	
	wxString GetPhpKeywords() const;
	
	wxString GetHtmlKeywords() const;
	
	wxString GetJavascriptKeywords() const;

private:

	/**
	 * handles auto completion for PHP.
	 *
	 * @param fileName the current file which autocomplete is working on
	 * @param code the entire, most current, source code
	 * @param word the current word where the cursor lies (this determines what the current symbol)
	 * @param syntax the token type that the cursor is currently on.  This helps
	 * in determining context (ie if the cursor is inside of a comment or string literal)
	 */
	std::vector<wxString> HandleAutoCompletionPhp(const wxString& fileName, const UnicodeString& code, const UnicodeString& word, mvceditor::LanguageDiscoveryClass::Syntax syntax);

	/**
	* handles auto completion for PHP.
	*
	* @param syntax the token type that the cursor is currently on.  This helps
	* int determining context (ie if the cursor is inside of a comment or string literal)
	*/
	std::vector<wxString> HandleAutoCompletionHtml(const UnicodeString& code, const UnicodeString& word, mvceditor::LanguageDiscoveryClass::Syntax syntax);

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
	* To parse a code snippet to make it suitable for code completion
	* @var ParserClass
	*/
	ParserClass Parser;

	/**
	 * To get the last expression in the source code
	 * @var LexicalAnalyzerClass
	 */
	LexicalAnalyzerClass Lexer;

	/**
	 * This class will NOT own this pointer
	 */
	ProjectClass* Project;
	
	/**
	 * This object will be used to parse the resources of files that are currently open.
	 * This class will NOT own this pointer
	 */
	ResourceUpdateThreadClass* ResourceUpdates;

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
	virtual std::vector<wxString> HandleAutoComplete(const wxString& fileName, const UnicodeString& code, const UnicodeString& word);
	
	wxString GetMySqlKeywords() const;

private:

	/**
	 * This class will NOT own this pointer
	 */
	ProjectClass*  Project;

	DatabaseInfoClass CurrentInfo;
};

class CssDocumentClass : public TextDocumentClass {

public:

	CssDocumentClass();

	virtual bool CanAutoComplete();

	virtual std::vector<wxString> HandleAutoComplete(const wxString& fileName, const UnicodeString& code, const UnicodeString& word);
	
	wxString GetCssKeywords() const;
	
	wxString GetCssPseudoClasses() const;

};

}



#endif