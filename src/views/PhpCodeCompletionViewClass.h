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
 * @copyright  2014 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef T4P_PHPCODECOMPLETIONVIEWCLASS_H
#define T4P_PHPCODECOMPLETIONVIEWCLASS_H

#include <views/FeatureViewClass.h>
#include <features/PhpCodeCompletionFeatureClass.h>
#include <code_control/CodeControlClass.h>
#include <pelet/LanguageDiscoveryClass.h>
#include <pelet/TokenClass.h>
#include <language_php/SymbolTableClass.h>
#include <globals/Events.h>

namespace t4p {

class PhpCodeCompletionProviderClass : public t4p::CodeCompletionProviderClass {

	public:

	PhpCodeCompletionProviderClass(t4p::GlobalsClass& globals);

	/**
	 * @return bool TRUE if file type is php or html
	 */
	bool DoesSupport(t4p::FileType type);

	void Provide(t4p::CodeControlClass* ctrl, std::vector<t4p::CodeCompletionItemClass>& suggestions, wxString& completeStatus);

	void RegisterAutoCompletionImages(wxStyledTextCtrl* ctrl);

	/**
	 * Append the '(' for method calls that are completed
	 */
	void OnAutoCompletionSelected(wxStyledTextEvent& evt);

	private:

	/**
	 * handles auto completion for PHP.
	 *
	 * @param code most current source code; but only up to the current position (this helps determine
	 * the scope)
	 * @param word the word to complete
	 * @param syntax the token type that the cursor is currently on.  This helps
	 *        in determining context (ie if the cursor is inside of a string)
	 * @param ctrl the code control that contains the source
	 * @param completeStatus a bit of text that will help the user understand
	 *        why the complete box did not populate.
	 */
	void HandleAutoCompletionPhp(const UnicodeString& code, const UnicodeString& word, pelet::LanguageDiscoveryClass::Syntax syntax,
		t4p::CodeControlClass* ctrl, wxString& completeStatus);

	/**
	* handles auto completion for HTML.
	*
	* @param word the word to complete
	* @param syntax the token type that the cursor is currently on.  This helps
	*        in determining context (ie if the cursor is inside of a tag name or a tag value)
	* @param ctrl the code control that contains the source
	* @param completeStatus a bit of text that will help the user understand
	*        why the complete box did not populate.
	*/
	void HandleAutoCompletionHtml(const UnicodeString& word, pelet::LanguageDiscoveryClass::Syntax syntax,
		wxStyledTextCtrl* ctrl, wxString& completeStatus);

	/**
	 * handles auto completion of stufff inside a PHP (single quote) string
	 * @param word the word to complete
	 * @param ctrl the code control that contains the source
	 * @param completeStatus a bit of text that will help the user understand
	 *        why the complete box did not populate.
	 */
	void HandleAutoCompletionString(const UnicodeString& word, wxStyledTextCtrl* ctrl, wxString& completeStats);

	/**
	 * Fills completeStatus with a human-friendly version of the symbol table error
	 */
	void HandleAutoCompletionPhpStatus(const t4p::SymbolTableMatchErrorClass& error,
		const UnicodeString& lastExpression, const pelet::VariableClass& parsedVariable,
		const pelet::ScopeClass& variableScope, wxString& completeStatus);

	/**
	 * share code between HandleAutoCompletionString and HandleAutoCompletionPhp since
	 * we want to autocomplete sql table and column names on both single and double quoted
	 * string, but we want to keep autocompletion of PHP variables in
	 * double-quoted strings only
	 */
	void AppendSqlTableNames(const UnicodeString& word, std::vector<wxString>& matches);

	/**
	 * Return a list of possible keyword matches for the given word. For example, if word="cl"
	 * then this method would return "class"
	 */
	std::vector<wxString> CollectNearMatchKeywords(wxString word);

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
	t4p::ScopeFinderClass ScopeFinder;

	/**
	 * To access any global structures: the tag cache, template variables
	 */
	t4p::GlobalsClass& Globals;

	/**
	 * The resources that are shown in the code completion list. Keeping these around
	 * so that we can append the '(' for method calls.
	 */
	std::vector<t4p::PhpTagClass> AutoCompletionResourceMatches;
};

/**
 * the php call tip provider shows the user a window that contains
 * the function signature of the function/method that the cursor
 * is currently on.
 */
class PhpCallTipProviderClass : public t4p::CallTipProviderClass {

	public:

	PhpCallTipProviderClass(t4p::GlobalsClass& globals);

	/**
	 * @return bool TRUE if file type is php or html
	 */
	bool DoesSupport(t4p::FileType type);

	/**
	 *
	 * @param ch the last character that the user typed in
	 *        providers can use this to automatically show/hide
	 *        the tip.
	 * @param force TRUE if the user forced the tip
	 * @param status any errors to show the user; these are useful so that
	 *        the user knows why completion did not suggest any items
	 */
	void ProvideTip(t4p::CodeControlClass* ctrl, wxChar ch, bool force, wxString& status);

	/**
	 * Handle the call tip up/down arrow events
	 */
	void OnCallTipClick(wxStyledTextEvent& evt);

	private:

	/**
	 * To access any global structures: the tag cache, template variables
	 */
	t4p::GlobalsClass& Globals;

	/**
	 * The resources used to populate the call tips
	 */
	std::vector<t4p::PhpTagClass> CurrentCallTipResources;

	/**
	 * The tag signature currently being displayed in the calltip.
	 * index into CurrentCallTipResources
	 */
	size_t CurrentCallTipIndex;

};

/**
 * this class highlights matching braces {}, [], and ()
 */
class PhpBraceMatchStylerClass : public t4p::BraceMatchStylerClass {

	public:

	PhpBraceMatchStylerClass();

	/**
	 * @return bool TRUE if file type is php or html
	 */
	bool DoesSupport(t4p::FileType type);

	void Style(t4p::CodeControlClass* ctrl, int postToCheck);
};

/**
 * The PhpCodeCompletion feature provides the user with
 * code suggestions. This feature only handles code
 * completion suggestions for PHP code ie. source inside
 * <?php tags.
 */
class PhpCodeCompletionViewClass : public t4p::FeatureViewClass {

	public:
	PhpCodeCompletionViewClass(t4p::PhpCodeCompletionFeatureClass& feature);

	private:

	void OnAppFileOpened(t4p::CodeControlEventClass& event);
	void OnAutoCompletionSelected(wxStyledTextEvent& event);
	void OnCallTipClick(wxStyledTextEvent& event);

	t4p::PhpCodeCompletionProviderClass CodeCompletionProvider;
	t4p::PhpCallTipProviderClass CallTipProvider;
	t4p::PhpBraceMatchStylerClass BraceStyler;

	DECLARE_EVENT_TABLE()
};

}

#endif // T4P_PHPCODECOMPLETIONVIEWCLASS_H
