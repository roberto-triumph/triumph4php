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
#ifndef MVCEDITORFINDERCLASS_H_
#define MVCEDITORFINDERCLASS_H_

#include <unicode/unistr.h>
#include <unicode/regex.h>

namespace mvceditor {
	
/**
 * This class can be used to search for a subset of text within a given 
 * text. This class searches for code snippets within given text, using 
 * PHP syntax rules to determine when case sensitivity and whitespace applies. The FindNext & FindPrevious
 * methods perform the search, while iterating through the results is done by using the GetLastMatch() method.
 * Example:
 * 
 * <code>
 *   UnicodeString code = UNICODE_STRING_SIMPLE("this is a string FINDME this is a string FINDME again");
 *   FinderClass finder("FINDME");
 *   finder.Prepare();
 *   int position = 0,
 *       length;
 *   while(finder.FindNext(code, position)) {
 *     if (finder.GetLastMatch(&position, &length) {
 *       printf("Found target at position: %d, length of match: %d\n", position, length);
 *     }
 *   }
 * 
 *   // result of above code
 *   // Found target at position: 17, length of match: 6
 *   // Found target at position: 41, length of match: 6
 * </code>
 * 
 * Finder Rules:  The finder makes searching PHP code easy by using PHP syntax rules to
 * determine when case sensitivity and whitespace significance applies. This relieves the burden
 * of having a precise expression to find code.
 * 
 * Example:  Let's say that we have a string variable named code with the following PHP code:
 * <code>
 *   UnicodeString code = UNICODE_STRING_SIMPLE("
 *     function computeAreaOfCircle($radius) {
 *       $PI = 3.14;
 *       $MESSAGE = 'area of circle is: %2f';
 *       return sprintf($MESSAGE, $PI * $radius * 2);
 *     }");
 * </code>
 * 
 * variable names: Variable names will always be case-sensitive, since in PHP variables are
 * case sensitive:
 * 
 * <code>
 *   FinderClass finder(UNICODE_STRING_SIMPLE("$PI"));
 *   printf("%d", finder.findNext(code)); // prints 1
 *   finder.Expression = UNICODE_STRING_SIMPLE("$pi");
 *   finder.Prepare();
 *   printf("%d", finder.FindNext(code)); // prints 0
 * </code>
 * 
 * whitespace significance: white space around operators and parentheses are insignificant
 * 
 * <code>
 *   FinderClass finder(UNICODE_STRING_SIMPLE("$PI = 3.14"));
 *   printf("%d", finder.findNext(code)); // prints 1
 *   finder.Expression = UNICODE_STRING_SIMPLE("$PI=3.14");
 *   finder.Prepare();
 *   printf("%d", finder.FindNext(code)); // prints 1
 *   finder.Expression = UNICODE_STRING_SIMPLE("$PI*$radius*2");
 *   finder.Prepare();
 *   printf("%d", finder.FindNext(code)); // prints 1
 *   finder.Expression = UNICODE_STRING_SIMPLE("computeAreaOfCircle( $radius )");
 *   finder.Prepare();
 *   printf("%d", finder.FindNext(code)); // prints 1
 *   finder.Prepare();
 *   finder.Expression = UNICODE_STRING_SIMPLE("sprintf (");
 *   finder.Prepare();
 *   printf("%d", finder.FindNext(code)); // prints 1
 * </code>
 * 
 * note that case sensitivity and whitespace rules will not apply to string literals.  However, literals
 * are searched using both single quotes and double quotes (since in PHP they are the same).
 * 
 * <code>
 *   FinderClass finder(UNICODE_STRING_SIMPLE("'area of circle is: %2f'"));
 *   printf("%d", finder.FindNext(code)); // prints 1
 *   finder.Expression = UNICODE_STRING_SIMPLE("'area of circle is:%2f'");
 *   finder.Prepare();
 *   printf("%d", finder.FindNext(code)); // prints 0
 *   finder.Expression = UNICODE_STRING_SIMPLE("\"area of circle is: %2f\"");
 *   finder.Prepare();
 *   printf("%d", finder.FindNext(code)); // prints 1
 * </code>
 */
class FinderClass {
	
public:
	
	/**
	 * Default constructor.
	 * 
	 * @param UnicodeString expression the expression to search for
	 */
	FinderClass(UnicodeString expression = UNICODE_STRING_SIMPLE(""), int mode = CODE);
	
	/**
	 * cleanup pattern
	 */
	~FinderClass();
	
	/**
	 * Prepare  Expression 
	 * @return bool if Expression is valid, Expression is not empty.
	 */
	bool Prepare();
	
	/**
	 * Find the next instance this expression in the given text.
	 * User FinderClass::GetLastMatch() method to get the hit 
	 * position. 
	 * 
	 * @param UnicodeString text the text to search in
	 * @param int start the index to start searching from
	 * @return bool true if expression is found
	 */
	bool FindNext(const UnicodeString& text, int32_t start = 0);
	
	
	/**
	 * Find the previous instance this expression in the given text.
	 * User FinderClass::GetLastMatch() method to get the hit 
	 * position.
	 * 
	 * @param UnicodeString text the text to search in
	 * @param int start the index to start searching from, if zero the searching will start from the end of the string
	 * @return bool true if expression is found
	 */	
	bool FindPrevious(const UnicodeString& text, int32_t start = 0);
	
	/**
	 * Return the position and length of the last hit found by 
	 * FinderClass::FindNext() method
	 * 
	 * @param int32_t position gets set with the position found 
	 * @param int32_t length gets set with the length of the hit
	 * @return bool false if FinderClass::FindNext() has not been 
	 * called or it did not find a match 
	 */
	bool GetLastMatch(int32_t& position, int32_t& length) const;
	
	/**
	 * Returns true if this Expression is valid. For exact & code modes, this method
	 * will always return true. For regular expression mode, this method will return
	 * true if this expression is of proper syntax.
	 * 
	 * @return bool
	 */
	bool IsValid() const;
	
	/**
	 * Returns the replacement string, with any back references already replaced with
	 * the correct text. text MUST be the same as the text given to the FindNext() or 
	 * FindPrevious methods.  If text is not, then this method will return false.
	 * 
	 * @param const UnicodeString& text the original text.
	 * @param UnicodeString& replacementText text that matched this expression
	 * @return bool true if this expression was found in the last call to FindPrevious()
	 * or FindNext() methods
	 */
	bool GetLastReplacementText(const UnicodeString& text, UnicodeString& replacementText) const;
	
	/**
	 * Replace all occurrences of this Expression in text with ReplaceExpression.
	 * 
	 * @var UnicodeString text the text to modify
	 * @return int the number of replacements made, 0 if ReplaceExpression is
	 * an empty string
	 */
	int ReplaceAllMatches(UnicodeString& text) const;
	
	/**
	 * reset the last position & length to signal not found.
	 */
	void ResetLastHit();	

	/**
	 * The string to search for. If Expression is modified, Prepare() method MUST be called in order to
	 * set the internal state machine.
	 * @var UnicodeString
	 */
	UnicodeString Expression;
	
	/**
	 * The string to replace the expression. If mode is REGULAR_EXPRESSION, this
	 * string can contain references (\1,\2)
	 * 
	 * @var UnicodeString
	 */
	 UnicodeString ReplaceExpression;
	
	/**
	 * Search mode.  If Mode is modified, Prepare() method MUST be called in order to
	 * set the internal state machine.
	 * @var int one of the search modes
	 */
	int Mode;
	
	/**
	 * Flag for case sensitive matching.
	 * 
	 * @var bool
	 */
	bool CaseSensitive;
	
	/**
	 * Flag for wrapping the search; ie if no hit then search from the beginning 
	 */
	bool Wrap;
	
	/**
	 * All possible search modes
	 * 
	 * @var enum
	 */
	enum {
		CODE = 0,
		EXACT = 1,
		REGULAR_EXPRESSION = 2
	};
	
private:
	
	/**
	 * The compiled regular expression
	 * @var RegExPattern*
	 */
	RegexPattern* Pattern;
	
	/**
	 * The matcher (instance of the the pattern)
	 * @var RegExMatcher
	 */
	RegexMatcher* Matcher;
	
	/**
	 * the position of the last found hit
	 * @var int32_t
	 */
	int32_t LastPosition;

	/**
	 * the length of the last found hit
	 * @var int32_t
	 */	
	int32_t LastLength;
	
	/**
	 * Error code when creating a RegexPattern
	 */
	UErrorCode PatternErrorCode;
	
	/**
	 * true if the Expression has been prepared.
	 * @var bool
	 */
	bool IsPrepared;
	
	/**
	 * Flag that tells us whether we found something
	 * @var 
	 */
	bool IsFound;
	
	/**
	 * Compiles the internal regular expression object from the Expression
	 */
	void PrepareForCodeMode();	
	
	/**
	 * Compiles the internal regular expression object from the Expression
	 */
	void PrepareForRegularExpressionMode();	
	
	/**
	 * Appends the regular expression for variable at the given code, starting from the given
	 * start index.  Will set the length parameter to the length of the token. This method
	 * assumes that code[start] is the beginning of a PHP variable ('$')
	 * 
	 * @param UnicodeString code the string to search
	 * @param int32_t start the index from where to begin
	 * @param UnicodeString the regular expression to append to
	 * @return int length the number of characters of the token
	 */
	int RegExOfVariable(UnicodeString& code, int32_t start, UnicodeString& expressionRegEx) const;
	
	/**
	 * Appends the regular expression for a string literal at the given code, starting 
	 * from the given start index.  Will set the length parameter to the length of the token.
	 * This method assumes that code[start] is the beginning of a PHP string literal (').
	 * The appended regular expression will match either single or double quoted
	 * string since they have the same meaning.
	 * 
	 * @param UnicodeString code the string to search
	 * @param int32_t start the index from where to begin
	 * @param UnicodeString the regular expression to append to
	 * @return int length the number of characters of the token
	 */
	int RegExOfSingleQuotedString(UnicodeString& code, int32_t start, UnicodeString& expressionRegEx) const;

	/**
	 * Appends the regular expression for a double quoted string literal at the given code, starting 
	 * from the given start index.  Will set the length parameter to the length of the token. 
	 * This method assumes that code[start] is the beginning of a PHP string literal (").
	 * The appended regular expression will match either single or double quoted
	 * string since they have the same meaning.
	 * 
	 * @param UnicodeString code the string to search
	 * @param int32_t start the index from where to begin
	 * @param UnicodeString the regular expression to append to
	 * @return int length the number of characters of the token including the quotes
	 */
	int RegExOfDoubleQuotedString(UnicodeString& code, int32_t start, UnicodeString& expressionRegEx) const;
	
	/**
	 * Appends the regular expression for a comment at the given code, starting from the given
	 * start index.  Will set the length parameter to the length of the token. This method
	 * assumes that code[start ... start + 1] is the beginning of a PHP comment  ("//" or "/ *")
	 * 
	 * @param UnicodeString code the string to search
	 * @param int32_t start the index from where to begin
	 * @param UnicodeString the regular expression to append to
	 * @return int length the number of characters of the token
	 */	
	int RegExOfComment(UnicodeString& code, int32_t start, UnicodeString& expressionRegEx) const;
	
	/**
	 * Appends the regular expression for a number literal at the given code, starting from the given
	 * start index.  Will set the length parameter to the length of the token. This method
	 * assumes that code[start] is the beginning of a PHP number literal
	 * 
	 * @param UnicodeString code the string to search
	 * @param int32_t start the index from where to begin
	 * @param UnicodeString the regular expression to append to
	 * @return int length the number of characters of the token
	 */	
	int RegExOfNumber(UnicodeString& code, int32_t start, UnicodeString& expressionRegEx) const;
	
	/**
	 * Appends the regular expression for an operator at the given code, starting from the given
	 * start index.  Will set the length parameter to the length of the token. This method
	 * assumes that code[start] is the beginning of a PHP operator.
	 * 
	 * @param UnicodeString code the string to search
	 * @param int32_t start the index from where to begin
	 * @param UnicodeString the regular expression to append to
	 * @return int length the number of characters of the token
	 */		
	int RegExOfOperator(UnicodeString& code, int32_t start, UnicodeString& expressionRegEx) const;
	
	/**
	 * Returns the regular expression for an identifier at the given code, starting from the given
	 * start index.  Will set the length parameter to the length of the token. This method
	 * assumes that code[start] is the beginning of a PHP identifier
	 * 
	 * @param UnicodeString code the string to search
	 * @param int32_t start the index from where to begin
     * @param UnicodeString the regular expression to append to
	 * @return int length the number of characters of the token
	 */			
	int RegExOfOther(UnicodeString& code, int32_t start, UnicodeString& expressionRegEx) const;
	
	/**
	 * replace all regular expression symbols so that they are treated as normal
	 * characters
	 * 
	 * @param UnicodeString& regEx the string to modify
	 */
	void EscapeRegEx(UnicodeString& regEx) const;
	
	/**
	 * Finds the next instance of this expression in the given text using exact matching.
	 * 
	 * @var const UnicodeString& text the text to search in
	 * @var int start the index to start searching from
	 * @return bool true if expression is found. This method will also set the
	 *         LastPosition, LastLength private variables.
	 */
	bool FindNextExact(const UnicodeString& text, int32_t start = 0);
	
	/**
	 * Finds the last instance of this expression in the given text using exact matching.
	 * 
	 * @var const UnicodeString& text the text to search in
	 * @var int start the index to start searching from (search will be done backwards)
	 * @return bool true if expression is found. This method will also set the
	 *         LastPosition, LastLength private variables.
	 */
	bool FindPreviousExact(const UnicodeString&text, int32_t start = 0);
	
	/**
	 * Finds this expression in the given text using regular expression matching.
	 * 
	 * @var const UnicodeString& text the text to search in
	 * @var int32_t start the index to start searching from
	 * @return bool true if expression is found. This method will also set the
	 *         LastPosition, LastLength private variables.
	 */
	bool FindNextRegularExpression(const UnicodeString& text, int32_t start = 0);
};

}
#endif /*MVCEDITORFINDERCLASS_H_*/
