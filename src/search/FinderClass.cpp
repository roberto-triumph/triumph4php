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
 * @date       $Date: 2011-07-04 12:25:36 -0700 (Mon, 04 Jul 2011) $
 * @version    $Rev: 568 $ 
 */
#include <search/FinderClass.h>
#include <unicode/uchar.h>
#include <unicode/ustdio.h>
#include <windows/StringHelperClass.h>
#include <assert.h>

mvceditor::FinderClass::FinderClass(UnicodeString expression, int mode)
	: Expression(expression)
	, ReplaceExpression()
	, Mode(mode)
	, CaseSensitive(false)
	, Wrap(false)
	, Pattern(NULL)
	, LastPosition(0)
	, LastLength(0)
	, IsPrepared(false)
	, IsFound(false) {
	ResetLastHit();
	PatternErrorCode = U_ZERO_ERROR;
}

mvceditor::FinderClass::~FinderClass() {
	if (Pattern != NULL) {
		delete Pattern;
	}
}

bool mvceditor::FinderClass::Prepare() {
	// delete the old pattern
	delete Pattern;
	Pattern = NULL;
	PatternErrorCode = U_ZERO_ERROR;
	
	switch(Mode) {
		case CODE:
			PrepareForCodeMode();
			break;
		case REGULAR_EXPRESSION:
			PrepareForRegularExpressionMode();
			break;
	}
	IsPrepared = !Expression.isEmpty() && (EXACT == Mode || U_SUCCESS(PatternErrorCode));
	return IsPrepared;
}

bool mvceditor::FinderClass::FindNext(const UnicodeString& text, int32_t start) {
	bool found = false;
	if (IsPrepared) {
		ResetLastHit();
		switch(Mode) {
			case EXACT:
				found = FindNextExact(text, start);
				break;
			case CODE:
			case REGULAR_EXPRESSION:
				found = FindNextRegularExpression(text, start);
			break;
		}
		if (Wrap && !found) {
			switch(Mode) {
				case EXACT:
					found = FindNextExact(text, 0);
					break;
				case CODE:
				case REGULAR_EXPRESSION:
					found = FindNextRegularExpression(text, 0);
				break;
			}
		}
	}
	return found;
}

bool mvceditor::FinderClass::FindPrevious(const UnicodeString& text, int32_t start) {
	bool found = false;
	if(EXACT == Mode) {
		ResetLastHit();
		found = FindPreviousExact(text, start);
	}
	else {
		// lazy way of backwards searching, search from the beginning until
		// we find the last hit before start
		int32_t position = 0,
			length = 0,
			nextPosition = 0,
			nextLength = 0;
		while (FindNext(text, nextPosition + nextLength)) {
			if (GetLastMatch(nextPosition, nextLength)) {
				if ((nextPosition + nextLength) >= start && start > 0) {					
					break;
				}
				found  = true;
				position = nextPosition;
				length = nextLength;
			}
		}
		IsFound = found;
		if (IsFound) {
			LastPosition = position;
			LastLength = length;
		}
	}
	return IsFound;
}

bool mvceditor::FinderClass::GetLastMatch(int32_t& position, int32_t& length) const {
	if (IsFound) {
		position = LastPosition;
		length = LastLength;
	}
	return IsFound;
}

void mvceditor::FinderClass::ResetLastHit() {
	LastLength = 0;
	LastPosition = 0;
	IsFound = false;
}

bool mvceditor::FinderClass::GetLastReplacementText(const UnicodeString& text, UnicodeString& replacementText) const {
	UBool matchFound = FALSE;
	if (IsFound && (LastPosition + LastLength) <= text.length()) {		
		UnicodeString matchedText(text, LastPosition, LastLength);
		UnicodeString replaceWith = ReplaceExpression;
		UErrorCode error = U_ZERO_ERROR;
		RegexMatcher* matcher = NULL;
		switch (Mode) {
			case EXACT:
				matchFound = Expression == matchedText;
				if (matchFound) {
					replacementText = replaceWith;
				}
				break;
			case CODE:
				matcher = Pattern->matcher(matchedText, error);
				if (U_SUCCESS(error) && matcher && matcher->matches(error) && U_SUCCESS(error)) {
					
					// dont use back references
					//replaceWith.findAndReplace(UNICODE_STRING_SIMPLE("$"), UNICODE_STRING_SIMPLE("\\$"));
					replacementText = matcher->replaceFirst(replaceWith, error);
					matchFound = TRUE;
				}
				break;
			case REGULAR_EXPRESSION:
				matcher = Pattern->matcher(matchedText, error);
				if (U_SUCCESS(error) && matcher && matcher->matches(error) && U_SUCCESS(error)) {
					replacementText = matcher->replaceFirst(replaceWith, error);
					matchFound = TRUE;
				}
				break;
		}
		if (matcher) {
			delete matcher;
		}
	}
	return matchFound == TRUE;
}

int mvceditor::FinderClass::ReplaceAllMatches(UnicodeString& text) const {
	int matches = 0;
	if (IsPrepared && !ReplaceExpression.isEmpty()) {
		UnicodeString replacement = ReplaceExpression;
		RegexMatcher* matcher = NULL;
		UErrorCode error = U_ZERO_ERROR;
		UnicodeString dest(text.length(), ' ', 0);
		int32_t pos = 0;
		switch (Mode) {
			case EXACT:
				pos = text.indexOf(Expression, 0);
				while (pos >= 0) {
					text.replaceBetween(pos, pos + Expression.length(), replacement);
					pos = text.indexOf(Expression, pos + replacement.length());
					++matches;
				}
				break;
			case CODE:
				matcher = Pattern->matcher(text, error);
				if (U_SUCCESS(error) && matcher) {
					
					// dont use back references
					replacement.findAndReplace(UNICODE_STRING_SIMPLE("$"), UNICODE_STRING_SIMPLE("\\$"));
					while (matcher->find()) {
						if (U_SUCCESS(error)) {
							matcher->appendReplacement(dest, replacement, error);
							if (U_SUCCESS(error)) {
								++matches;
							}
						}
					}
					matcher->appendTail(dest);
					text = dest;
				}
				break;
			case REGULAR_EXPRESSION:
				matcher = Pattern->matcher(text, error);
				if (U_SUCCESS(error) && matcher) {
					while (matcher->find()) {
						if (U_SUCCESS(error)) {
							matcher->appendReplacement(dest, replacement, error);
							if (U_SUCCESS(error)) {
								++matches;
							}
						}
					}
					matcher->appendTail(dest);
					text = dest;
				}
				break;
		}
		if (matcher) {
			delete matcher;
		}
	}
	return matches;
}

int mvceditor::FinderClass::RegExOfVariable(UnicodeString& code, int32_t start, UnicodeString& expressionRegEx) const {
	int length = 1;
	for (int32_t i = start + 1; i < code.length(); ++i) {
		if (!u_isalnum(code[i]) && '_' != code[i]) {
			break;
		}
		++length;
	}
	UnicodeString regex(code, start, length);
	EscapeRegEx(regex);
	expressionRegEx.append(regex);
	return length;
}

int mvceditor::FinderClass::RegExOfSingleQuotedString(UnicodeString& code, int32_t start, UnicodeString& expressionRegEx) const {
	int32_t length = 1;
	int32_t codeLength = code.length();
	UnicodeString escapedQuote = UNICODE_STRING_SIMPLE("\\'");
	for (int32_t i = start + 1; i < codeLength; ++i) {		
		if (code.compare(i, 2, escapedQuote, 0, 1) == 0) {			
			length += 2; 
			++i;
		}
		else {
			++length;
			if ('\'' == code[i]) {			
				break;
			}
		}
	}
	
	// escape regex symbols. for string literals, get matches
	// with either single or double quotes. be careful to
	// NOT add end quotes to literals that do no have ending
	// quotes
	UnicodeString regex;
	UChar lastChar = '\0';
	if (length > 1) {
		regex.append(code, start + 1, length - 1);
		lastChar = regex[regex.length() - 1];
		if ('\'' == lastChar) {
			regex.setTo(code, start + 1, length - 2);
		}
		EscapeRegEx(regex);
	}
	expressionRegEx.append(UNICODE_STRING_SIMPLE("('|\")"));
	expressionRegEx.append(regex);
	if ('\'' == lastChar) {
		expressionRegEx.append(UNICODE_STRING_SIMPLE("('|\")"));
	 }
	return length;
}

int mvceditor::FinderClass::RegExOfDoubleQuotedString(UnicodeString& code, int32_t start, UnicodeString& expressionRegEx) const {
	int length = 1;
	int32_t codeLength = code.length();
	UnicodeString escapedDoubleQuote = UNICODE_STRING_SIMPLE("\\\"");
	for (int32_t i = start + 1; i < codeLength; ++i) {		
		if (code.compare(i, 2, escapedDoubleQuote, 0, 1) == 0) {
			length += 2; 
			++i;
		}
		else {
			++length;
			if ('"' == code[i]) {			
				break;
			}
		}
	}
	
	// escape regex symbols. for string literals, get matches
	// with either single or double quotes. be careful to
	// NOT add end quotes to literals that do no have ending
	// quotes
	UnicodeString regex;
	UChar lastChar = '\0';
	if (length > 1) {
		regex.setTo(code, start + 1, length - 1);
		lastChar = regex[regex.length() - 1];
		if ('"' == lastChar) {
			regex.setTo(code, start + 1, length - 2);
		}
		EscapeRegEx(regex);
	}
	expressionRegEx.append(UNICODE_STRING_SIMPLE("('|\")"));
	expressionRegEx.append(regex);
	if ('"' == lastChar) {
		expressionRegEx.append(UNICODE_STRING_SIMPLE("('|\")"));
	}
	return length;
}

int mvceditor::FinderClass::RegExOfComment(UnicodeString& code, int32_t start, UnicodeString& expressionRegEx) const {
	int length = 2;
	if (code.compare(start, 2, UNICODE_STRING_SIMPLE("//")) == 0) {
		for (int32_t i = start + 2; i < code.length(); ++i) {
			if ('\n' == code[i]) {
				break;
			}
			++length;
		}
	}
	else {
		for (int32_t i = start + 2; i < code.length(); ++i) {		
			if (code.compare(i, 2, UNICODE_STRING_SIMPLE("*/")) == 0) {
				length += 2;
				break;
			}
			++length;
		}
	}
	
	// TODO: fancy logic of matching multi-line comments like
	//	  // line 1
	//	  // line 2
	//	  
	//	  and 
	//	  
	//	  /*
	//	   * line 1
	//	   * line 2
	//	   */
	 
	UnicodeString regex(code, start, length);
	EscapeRegEx(regex);
	expressionRegEx.append(regex);
	return length;
}

int mvceditor::FinderClass::RegExOfNumber(UnicodeString& code, int32_t start, UnicodeString& expressionRegEx) const {
	
	// TODO: logic is kinda faulty as it does not account for negative numbers
	int length = 0;
	UnicodeString firstTwoChars(code, start, 2);
	if (UNICODE_STRING_SIMPLE("0x") == firstTwoChars) {
		length = 2;
		for (int32_t i = start + 2; i < code.length(); ++i) {
			if (!u_isalnum(code[i])) {
				break;
			}
			++length;
		}
	}
	else {
		for (int32_t i = start; i < code.length(); ++i) {

			// for floats and exponents ie. 3.14 or 3.142e-49. 
			// not a really good check but good enough for now...
			if (!u_isdigit(code[i]) && '.' != code[i] && 'e' != code[i] &&
				'E' != code[i] && '-' != code[i]) {
				break;
			}
			++length;
		}
	}
	UnicodeString regex(code, start, length);
	EscapeRegEx(regex);
	expressionRegEx.append(regex);
	return length;
}

int mvceditor::FinderClass::RegExOfOperator(UnicodeString& code, int32_t start, UnicodeString& expressionRegEx) const {
	int length = 0;
	
	// check for 3-char operators
	 if (code.compare(start, 3, UNICODE_STRING_SIMPLE("===")) == 0 || 
		code.compare(start, 3, UNICODE_STRING_SIMPLE("!==")) == 0) {
		length = 3;
	} 
	
	// check for 2-char operators
	 else if (code.compare(start, 2, UNICODE_STRING_SIMPLE("&&"), 0, 2) == 0 ||
		 code.compare(start, 2, UNICODE_STRING_SIMPLE("||"), 0, 2) == 0 ||
		 code.compare(start, 2, UNICODE_STRING_SIMPLE("=="), 0, 2) == 0 ||
		 code.compare(start, 2, UNICODE_STRING_SIMPLE("<="), 0, 2) == 0 ||
		 code.compare(start, 2, UNICODE_STRING_SIMPLE(">="), 0, 2) == 0 ||
		 code.compare(start, 2, UNICODE_STRING_SIMPLE("!="), 0, 2) == 0 ||
		 code.compare(start, 2, UNICODE_STRING_SIMPLE("&="), 0, 2) == 0 ||
		 code.compare(start, 2, UNICODE_STRING_SIMPLE("|="), 0, 2) == 0 ||
		 code.compare(start, 2, UNICODE_STRING_SIMPLE("+="), 0, 2) == 0 ||
		 code.compare(start, 2, UNICODE_STRING_SIMPLE("-="), 0, 2) == 0 ||
		 code.compare(start, 2, UNICODE_STRING_SIMPLE("*="), 0, 2) == 0 ||
		 code.compare(start, 2, UNICODE_STRING_SIMPLE("/="), 0, 2) == 0 ||
		 code.compare(start, 2, UNICODE_STRING_SIMPLE("->"), 0, 2) == 0 ||
		 code.compare(start, 2, UNICODE_STRING_SIMPLE("::"), 0, 2) == 0 ||
		 code.compare(start, 2, UNICODE_STRING_SIMPLE("=>"), 0, 2) == 0) {
		length = 2;
	}
	else {
		
		// must be 1-char operators
		 length = 1;
	}
	UnicodeString regex(code, start, length);
	EscapeRegEx(regex);
	expressionRegEx.append(regex);
	return length;
}

int mvceditor::FinderClass::RegExOfOther(UnicodeString& code, int32_t start, UnicodeString& expressionRegEx) const {
	int length = 0;
	for (int32_t i = start; i < code.length(); ++i) {
		if (!u_isalnum(code[i]) && '_' != code[i]) {
			break;
		}
		++length;
	}
	UnicodeString regex(code, start, length);
	EscapeRegEx(regex);
	expressionRegEx.append(regex);
	return length;
}

void mvceditor::FinderClass::EscapeRegEx(UnicodeString& regEx) const {
	UnicodeString symbols = UNICODE_STRING_SIMPLE("!@#$%^&*()[]{}\\-+?.,\"|");
	
	// there's got to be a batter way of escaping all regex symbols
	regEx.findAndReplace(UNICODE_STRING_SIMPLE("\\"), UNICODE_STRING_SIMPLE("\\\\"));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE("!"), UNICODE_STRING_SIMPLE("\\!"));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE("@"), UNICODE_STRING_SIMPLE("\\@"));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE("#"), UNICODE_STRING_SIMPLE("\\#"));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE("$"), UNICODE_STRING_SIMPLE("\\$"));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE("%"), UNICODE_STRING_SIMPLE("\\%"));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE("^"), UNICODE_STRING_SIMPLE("\\^"));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE("&"), UNICODE_STRING_SIMPLE("\\&"));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE("*"), UNICODE_STRING_SIMPLE("\\*"));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE("("), UNICODE_STRING_SIMPLE("\\("));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE(")"), UNICODE_STRING_SIMPLE("\\)"));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE("["), UNICODE_STRING_SIMPLE("\\["));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE("]"), UNICODE_STRING_SIMPLE("\\]"));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE("{"), UNICODE_STRING_SIMPLE("\\{"));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE("}"), UNICODE_STRING_SIMPLE("\\}"));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE("-"), UNICODE_STRING_SIMPLE("\\-"));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE("+"), UNICODE_STRING_SIMPLE("\\+"));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE("?"), UNICODE_STRING_SIMPLE("\\?"));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE("."), UNICODE_STRING_SIMPLE("\\."));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE(","), UNICODE_STRING_SIMPLE("\\,"));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE("\""), UNICODE_STRING_SIMPLE("\\\""));
	regEx.findAndReplace(UNICODE_STRING_SIMPLE("|"), UNICODE_STRING_SIMPLE("\\|"));

	
}
bool mvceditor::FinderClass::FindNextExact(const UnicodeString& text, int32_t start) {
	int32_t foundIndex = 0;
	if (!CaseSensitive) {
		UnicodeString textLower(text);
		textLower.toLower();
		UnicodeString expressionLower(Expression);
		expressionLower.toLower();
		foundIndex = textLower.indexOf(expressionLower, start);
	}
	else {
		foundIndex = text.indexOf(Expression, start);
	}
	IsFound = -1 != foundIndex;
	if (IsFound) {
		LastPosition = foundIndex;
		LastLength = Expression.length();
	}
	return IsFound;
}

bool mvceditor::FinderClass::FindPreviousExact(const UnicodeString& text, int32_t start) {
	int32_t foundIndex = 0;
	if (!CaseSensitive) {
		UnicodeString textLower(text);
		textLower.toLower();
		UnicodeString expressionLower(Expression);
		expressionLower.toLower();
		foundIndex = mvceditor::StringHelperClass::FindPrevious(textLower, expressionLower, start);
	}
	else {
		foundIndex = mvceditor::StringHelperClass::FindPrevious(text, Expression, start);
	}
	IsFound = -1 != foundIndex;
	if (IsFound) {
		LastPosition = foundIndex;
		LastLength = Expression.length();
	}
	return IsFound;
}

bool mvceditor::FinderClass::FindNextRegularExpression(const UnicodeString& text, int32_t start) {
	if (U_SUCCESS(PatternErrorCode) && Pattern != NULL) {
		UnicodeString findText(text);
		if (start > 0 && start < text.length()) {
			findText.setTo(text, start);
		}
		else if (start > 0) {
			findText = UNICODE_STRING_SIMPLE("");
		}
		int32_t foundPos = 0, 
			length = 0,
			endPos = 0;
		UErrorCode error = U_ZERO_ERROR;
		RegexMatcher* matcher = Pattern->matcher(findText, error);
		if (U_SUCCESS(error) && matcher) {
			if (matcher->find()) {
				foundPos = matcher->start(error);
				endPos = matcher->end(error) ;
				if (U_SUCCESS(error) && U_SUCCESS(error)) {
					IsFound = true;

					length = endPos - foundPos; // end is the index after the match 

					// if search was started from the middle of a string,
					// need to correct the found position
					LastPosition = start > 0 ? foundPos + start : foundPos;
					LastLength = length;
				}
			}
			delete matcher;
		}
	}
	return IsFound;
}

void mvceditor::FinderClass::PrepareForCodeMode() {
	UnicodeString expressionRegEx;
	int tokenLength = 0;
	UChar nextChar;
	int32_t length = Expression.length();
	for (int32_t i = 0; i < length; ++i) {
		if (i < (length - 1)) {
			nextChar = Expression[i + 1];
		}
		tokenLength = 0;
		UChar currentChar = Expression[i];
		if ('$' == currentChar) {
			tokenLength = RegExOfVariable(Expression, i, expressionRegEx);
		}
		else if ('\'' == currentChar) {
			tokenLength = RegExOfSingleQuotedString(Expression, i, expressionRegEx);
		}
		else if ('"' == currentChar) {
			tokenLength = RegExOfDoubleQuotedString(Expression, i, expressionRegEx);
		}
		else if (('/' == currentChar && '/' == nextChar) ||
			('/' == currentChar && '*' == nextChar)) {
			tokenLength = RegExOfComment(Expression, i, expressionRegEx);
		}
		else if(u_isdigit(currentChar)) {
			tokenLength = RegExOfNumber(Expression, i, expressionRegEx);
		}
		else if (u_isspace(currentChar)) {
			//nothing
		}
		
		// underscores are not operators
		else if(!u_isalnum(currentChar) && !u_isspace(currentChar) && '_' != currentChar) {
			tokenLength = RegExOfOperator(Expression, i, expressionRegEx);
		}
		else {
			tokenLength = RegExOfOther(Expression, i, expressionRegEx);
		}
		if (tokenLength) {
			expressionRegEx.append(UNICODE_STRING_SIMPLE("\\s*"));
			i += tokenLength - 1;			
		}
	}
	if (expressionRegEx.compare(expressionRegEx.length() - 3, 3, UNICODE_STRING_SIMPLE("\\s*")) == 0) {
		expressionRegEx.setTo(expressionRegEx, 0, expressionRegEx.length() - 3);
	}
	
	// should always be valid since we have escaped properly
	int flags = UREGEX_MULTILINE;
	if (!CaseSensitive) {
		flags |= UREGEX_CASE_INSENSITIVE;
	}
	Pattern = RegexPattern::compile(expressionRegEx, flags, PatternErrorCode);
	assert(U_SUCCESS(PatternErrorCode));
	
	//UFILE *out = u_finit(stdout, NULL, NULL);
    //u_fprintf(out, "%S\n", expressionRegEx.getTerminatedBuffer());
    //u_fclose(out);
}
	
void mvceditor::FinderClass::PrepareForRegularExpressionMode() {
	int flags = 0;
	if (!CaseSensitive) {
		flags |= UREGEX_CASE_INSENSITIVE;
	}
	Pattern = RegexPattern::compile(Expression, flags, PatternErrorCode);
}