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
#ifndef SRC_SEARCH_FINDERCLASS_H_
#define SRC_SEARCH_FINDERCLASS_H_

#include <unicode/unistr.h>
#include <unicode/regex.h>

namespace t4p {
/**
 * This class can be used to search for a subset of text within a given
 * text. The FindNext & FindPrevious methods perform the search, while iterating through the
 * results is done by using the GetLastMatch() method.
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
 *
 */
class FinderClass {
	public:
	/**
	 * All possible search modes
	 * EXACT - exact case sensitive matching
	 * CASE_INSENSITIVE - exact but case insensitive matching
	 * REGULAR_EXPRESSION - a regular expression
	 * @var enum
	 */
	enum Modes {
		EXACT = 0,
		CASE_INSENSITIVE = 1,
		REGULAR_EXPRESSION = 2
	};

	/**
	 * Default constructor.
	 *
	 * @param UnicodeString expression the expression to search for
	 */
	FinderClass(UnicodeString expression = UNICODE_STRING_SIMPLE(""), Modes mode = EXACT);

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
	 * replace all regular expression symbols so that they are treated as normal
	 * characters
	 *
	 * @param UnicodeString& regEx the string to modify
	 */
	static void EscapeRegEx(UnicodeString& regEx);

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
	 * Flag for wrapping the search; ie if no hit then search from the beginning
	 */
	bool Wrap;

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
	 * Finds the next instance of this expression in the given text using exact matching.
	 *
	 * @param const UnicodeString& text the text to search in
	 * @param int start the index to start searching from
	 * @param bool caseSensitive if TRUE then search will be case sensitive
	 * @return bool true if expression is found. This method will also set the
	 *         LastPosition, LastLength private variables.
	 */
	bool FindNextExact(const UnicodeString& text, int32_t start = 0, bool caseSensitive = false);

	/**
	 * Finds the last instance of this expression in the given text using exact matching.
	 *
	 * @param const UnicodeString& text the text to search in
	 * @param int start the index to start searching from (search will be done backwards)
	 * @param bool caseSensitive if TRUE then search will be case sensitive
	 * @return bool true if expression is found. This method will also set the
	 *         LastPosition, LastLength private variables.
	 */
	bool FindPreviousExact(const UnicodeString&text, int32_t start = 0, bool caseSensitive = false);

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
#endif  // SRC_SEARCH_FINDERCLASS_H_
