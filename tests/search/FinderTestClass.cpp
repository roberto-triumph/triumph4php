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
#include <UnitTest++.h>
#include <search/FinderClass.h>
#include <windows/StringHelperClass.h>
#include <unicode/unistr.h>
#include <unicode/ustream.h> //get the << overloaded operator, needed by UnitTest++

UnicodeString CODE = mvceditor::StringHelperClass::charToIcu(
	"<?php\n"
	"/**\n"
	" * function for computing area of circle\n"
	" * @copyright 2009\n"
	" * @author hacker\n"
	" */\n"
	"function computeAreaOfCircle($radius) {\n"
    "  $PI = 3.14; //constant \n"
    "  $MESSAGE = 'area of circle is: %2f';\n"
	"  if (0 === $radius || 0 >= $radius) {\n"
	"    return \"Invalid radius. \" . sprintf($MESSAGE, $PI*$radius*2);\n"
	"  }\n"
    "  return sprintf($MESSAGE, $PI * $radius * 2);\n"
    "}\n"
	"?>");

SUITE(FinderTestClass) {
	
TEST(FindNextUsingCodeModeShouldReturnValidIndexWhenSearchingCaseSensitive) {
	UnicodeString tofind(UNICODE_STRING_SIMPLE("$PI"));
	mvceditor::FinderClass finder(tofind);
	CHECK(finder.Prepare());
	int32_t expectedIndex = CODE.indexOf(tofind);
	CHECK(finder.FindNext(CODE));
	int32_t position, length;
	CHECK(finder.GetLastMatch(position, length));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL(tofind.length(), length);
}

TEST(FindNextUsingCodeModeShouldReturnValidIndexWhenSearchingCaseInsensitive) {
	UnicodeString tofind(UNICODE_STRING_SIMPLE("$pi"));
	mvceditor::FinderClass finder(tofind);
	finder.CaseSensitive = false;
	CHECK(finder.Prepare());
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("$PI"));
	CHECK(finder.FindNext(CODE));
	int32_t position, length;
	CHECK(finder.GetLastMatch(position, length));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL(tofind.length(), length);
}

TEST(FindNextUsingCodeModeShouldReturnInvalidIndexWhenExpressionIsNotFound) {
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("zzzz"));
	CHECK(finder.Prepare());
	CHECK_EQUAL(false, finder.FindNext(CODE));
	int32_t position, length;
	CHECK_EQUAL(false, finder.GetLastMatch(position, length));
}

TEST(FindNextUsingCodeModeShouldReturnValidIndexWhenSearchingFromTheMiddle) {
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("$MESSAGE"));
	CHECK(finder.Prepare());
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("$MESSAGE"), 200);
	CHECK(finder.FindNext(CODE, 200));
	int32_t position, length;
	CHECK(finder.GetLastMatch(position, length));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)8, length);	
}
TEST(FindNextUsingCodeModeShouldFindManyMatches) {
	int32_t position, length;
	int32_t expectedLength = 8;
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("$MESSAGE"));
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	CHECK(finder.GetLastMatch(position, length));
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("$MESSAGE"));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL(expectedLength, length);
	expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("$MESSAGE"), expectedIndex + 1);
	CHECK(finder.FindNext(CODE, position + length));
	CHECK(finder.GetLastMatch(position, length));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL(expectedLength, length);
	expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("$MESSAGE"), expectedIndex + 1);
	CHECK(finder.FindNext(CODE, position + length));
	CHECK(finder.GetLastMatch(position, length));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL(expectedLength, length);
	CHECK_EQUAL(false, finder.FindNext(CODE, position + length));
}

TEST(FindNextUsingCodeModeShouldReturnCorrectMatchWhenExpressionGrows) {
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("$MES"));
	CHECK(finder.Prepare());
	
	/*
	 * first find
	 */
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("$MESSAGE"));
	CHECK(finder.FindNext(CODE, 0));
	int32_t position, length;
	CHECK(finder.GetLastMatch(position, length));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)4, length);
	
	/*
	 * second find
	 */
	finder.Expression = UNICODE_STRING_SIMPLE("$MESS");
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE, 0));
	CHECK(finder.GetLastMatch(position, length));	
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)5, length);
	
	/*
	 * third find
	 */
	finder.Expression = UNICODE_STRING_SIMPLE("$MESSA");
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE, 0));
	CHECK(finder.GetLastMatch(position, length));	
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)6, length);
}

TEST(FindNextUsingCodeModeShouldFindVariableUsingCaseSensitive) {
	int32_t position, length;
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("$PI"));
	finder.CaseSensitive = true;
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	CHECK(finder.GetLastMatch(position, length));
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("$PI"));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)3, length);
}

TEST(FindNextUsingCodeModeShouldFindStatementWhenWhitespaceIsNotSignificant) {
	int32_t position, length;
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("$PI = 3.14"));
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	CHECK(finder.GetLastMatch(position, length));
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("$PI = 3.14"));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)10, length);
	finder.Expression = UNICODE_STRING_SIMPLE("$PI=3.14");
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	CHECK(finder.GetLastMatch(position, length));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)10, length);
}

TEST(FindNextUsingCodeModeShouldFindSingleQuotedLiteral) {
	int32_t position, length;
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("='area of circle is: %2f'"));
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	CHECK(finder.GetLastMatch(position, length));
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("= 'area"));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)26, length);
}

TEST(FindNextUsingCodeModeShouldFindNonEndingSingleQuotedLiteral) {
	int32_t position, length;
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("='are"));
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	CHECK(finder.GetLastMatch(position, length));
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("= 'are"));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)6, length);
}


TEST(FindNextUsingCodeModeWithOnlyASingleQuotedLiteral) {
	int32_t position, length;
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("'"));
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	CHECK(finder.GetLastMatch(position, length));
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("'"));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)1, length);
}

TEST(FindNextUsingCodeModeWithOnlyADoubleQuotedLiteral) {
	int32_t position, length;
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("\""));
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	CHECK(finder.GetLastMatch(position, length));
	
	//in code mode single quotes == double quotes
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("\'"));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)1, length);
}

TEST(FindNextUsingCodeModeShouldFindDoubleQuotedLiteral) {
	int32_t position, length;
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("\"Invalid radius. \""));
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	CHECK(finder.GetLastMatch(position, length));
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("\"Invalid radius. \""));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)18, length);
}

TEST(FindNextUsingCodeModeDoubleQuotedLiteralShouldMatchSingleQuotedLiteral) {
	int32_t position, length;
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("'Invalid radius. '"));
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	CHECK(finder.GetLastMatch(position, length));
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("\"Invalid radius. \""));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)18, length);
}

TEST(FindNextUsingCodeModeShouldFindNonEndingDoubleQuotedLiteral) {
	int32_t position, length;
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("\"Inva"));
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	CHECK(finder.GetLastMatch(position, length));
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("\"Inva"));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)5, length);
}

TEST(FindNextUsingCodeModeShouldFindCPlusPlusStyleComment) {
	int32_t position, length;
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("3.14;//constant"));
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	CHECK(finder.GetLastMatch(position, length));
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("3.14; //constant"));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)16, length);
}

TEST(FindNextUsingCodeModeShouldFindCStyleComment) {
	int32_t position, length;
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE(" /**\n * function for computing"));
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	CHECK(finder.GetLastMatch(position, length));
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("/**\n * function"));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)29, length);
}

TEST(FindNextUsingCodeModeShouldFind3CharacterOperator) {
	int32_t position, length;
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("0===$radius"));
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	CHECK(finder.GetLastMatch(position, length));
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("0 === $radius"));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)13, length);
}

TEST(FindNextUsingCodeModeShouldFind2CharacterOperator) {
	int32_t position, length;
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("0>=$radius"));
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	CHECK(finder.GetLastMatch(position, length));
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("0 >= $radius"));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)12, length);
}

TEST(FindNextUsingCodeModeShouldEscapePipe) {
	int32_t position, length;
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("0===$radius||0>=$radius"));
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	CHECK(finder.GetLastMatch(position, length));
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("0 === $radius"));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)29, length);
}

TEST(FindNextUsingCodeModeShouldFindIfIdentifier) {
	int32_t position, length;
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("if(0 === $radius"));
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	CHECK(finder.GetLastMatch(position, length));
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("if (0 === $radius"));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)17, length);
}

TEST(FindNextUsingCodeModeShouldFindCodeSnippetWhenWhitespaceIsInsignificant) {
	int32_t position, length;
	mvceditor::FinderClass finder(mvceditor::StringHelperClass::charToIcu(
		"function computeAreaOfCircle( $radius ) {\n"
		"  $PI =\n"
		"    3.14; //constant \n"
		"  $MESSAGE='area of circle is: %2f';\n"
		"  if(0===$radius||0>=$radius) {\n"
		"    return\"Invalid radius. \" . sprintf($MESSAGE, $PI*$radius*2);\n"
		"  }\n"
		"  return sprintf($MESSAGE, $PI*$radius*2);\n"
		"}\n"
	));
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	CHECK(finder.GetLastMatch(position, length));
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("function computeAreaOfCircle"));	
	int endIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("}\n?>"));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL(endIndex - expectedIndex + 1, length);
}

TEST(FindPreviousUsingCodeModeShouldReturnValidIndexWhenSearchingCaseSensitive) {
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("$PI"));
	CHECK(finder.Prepare());
	int32_t expectedIndex = CODE.lastIndexOf(UNICODE_STRING_SIMPLE("$PI"));
	CHECK(finder.FindPrevious(CODE));
	int32_t position, length;
	CHECK(finder.GetLastMatch(position, length));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)3, length);
}

TEST(FindPreviousUsingCodeModeShouldReturnInvalidIndexWhenExpressionIsNotFound) {
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("findnotme"));
	CHECK(finder.Prepare());
	CHECK_EQUAL(false, finder.FindPrevious(CODE));
	int32_t position, length;
	CHECK_EQUAL(false, finder.GetLastMatch(position, length));
}

TEST(FindPreviousUsingCodeModeShouldReturnInvalidIndexWhenExpressionIsNotFoundAfterAPreviousHit) {
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("$MESSAGE"));
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	int32_t position, length;
	CHECK(finder.GetLastMatch(position, length));
	CHECK_EQUAL(false, finder.FindPrevious(CODE, position - 1));
	CHECK_EQUAL(false,finder.GetLastMatch(position, length));
}

TEST(FindPreviousUsingCodeModeShouldReturnValidIndexWhenSearchingFromTheMiddle) {
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("$PI"));
	CHECK(finder.Prepare());
	int32_t expectedIndex = CODE.lastIndexOf(UNICODE_STRING_SIMPLE("$PI"));
	expectedIndex = CODE.lastIndexOf(UNICODE_STRING_SIMPLE("$PI"), 0, expectedIndex - 3);
	CHECK(finder.FindPrevious(CODE, CODE.lastIndexOf(UNICODE_STRING_SIMPLE("$PI")) - 3));
	int32_t position, length;
	CHECK(finder.GetLastMatch(position, length));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL((int32_t)3, length);	
}

TEST(FindNextUsingExactModeShouldReturnValidIndexWhenSearching) {
	UnicodeString tofind(UNICODE_STRING_SIMPLE("$PI"));
	mvceditor::FinderClass finder(tofind, mvceditor::FinderClass::EXACT);
	CHECK(finder.Prepare());
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("$PI"));
	CHECK(finder.FindNext(CODE));
	int32_t position, length;
	CHECK(finder.GetLastMatch(position, length));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL(tofind.length(), length);
}

TEST(FindNextUsingExactModeShouldReturnValidIndexWhenSearchingCaseInsensitive) {
	UnicodeString tofind(UNICODE_STRING_SIMPLE("$pi"));
	mvceditor::FinderClass finder(tofind, mvceditor::FinderClass::EXACT);
	finder.CaseSensitive = false;
	CHECK(finder.Prepare());
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("$PI"));
	CHECK(finder.FindNext(CODE));
	int32_t position, length;
	CHECK(finder.GetLastMatch(position, length));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL(tofind.length(), length);
}

TEST(FindNextUsingExactModeShouldReturnInvalidIndexWhenNotFound) {
	UnicodeString tofind(UNICODE_STRING_SIMPLE("$pi"));
	mvceditor::FinderClass finder(tofind, mvceditor::FinderClass::EXACT);
	finder.CaseSensitive = true;
	CHECK(finder.Prepare());
	CHECK_EQUAL(false, finder.FindNext(CODE));
	int32_t position, length;
	CHECK_EQUAL(false, finder.GetLastMatch(position, length));
}

TEST(FindNextUsingExactModeShouldReturnValidIndexWhenUsingWrap) {
	UnicodeString tofind(UNICODE_STRING_SIMPLE("$PI"));
	mvceditor::FinderClass finder(tofind, mvceditor::FinderClass::EXACT);
	finder.Wrap = true;
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("$PI"));
	int32_t start = CODE.length() - 2;
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE, start));
	int32_t position, length;
	CHECK(finder.GetLastMatch(position, length));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL(tofind.length(), length);
}

TEST(FindNextUsingRegularExpressionModeShouldReturnValidIndexWhenSearching) {
	UnicodeString tofind(UNICODE_STRING_SIMPLE("\\$PI\\s*=\\s*\\d+\\.\\d+"));
	mvceditor::FinderClass finder(tofind, mvceditor::FinderClass::REGULAR_EXPRESSION);
	CHECK(finder.Prepare());
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("$PI"));
	CHECK(finder.FindNext(CODE));
	int32_t position, length;
	CHECK(finder.GetLastMatch(position, length));
	CHECK_EQUAL(expectedIndex, position);
	UnicodeString expected = UNICODE_STRING_SIMPLE("$PI = 3.14");
	UnicodeString actual(CODE, position, length);
	CHECK(expected == actual);
}

TEST(FindNextUsingRegularExpressionModeShouldReturnValidIndexWhenSearchingCaseInsensitive) {
	UnicodeString tofind(UNICODE_STRING_SIMPLE("\\$pi\\s*=\\s*\\d+\\.\\d+"));
	mvceditor::FinderClass finder(tofind, mvceditor::FinderClass::REGULAR_EXPRESSION);
	finder.CaseSensitive = false;
	CHECK(finder.Prepare());
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("$PI"));
	CHECK(finder.FindNext(CODE));
	int32_t position, length;
	CHECK(finder.GetLastMatch(position, length));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL(0, CODE.compare(position, length, UNICODE_STRING_SIMPLE("$PI = 3.14")));
}

TEST(FindNextUsingRegularExpressionModeShouldReturnFalseWhenRegularExpressionIsInvalid) {
	UnicodeString tofind(UNICODE_STRING_SIMPLE("\\$PI=\\d+("));
	mvceditor::FinderClass finder(tofind, mvceditor::FinderClass::REGULAR_EXPRESSION);
	CHECK_EQUAL(false, finder.Prepare());
	CHECK_EQUAL(false, finder.FindNext(CODE));
}

TEST(FindNextUsingRegularExpressionModeShouldReturnInvalidIndexWhenNotFound) {
	UnicodeString tofind(UNICODE_STRING_SIMPLE("\\$PI=\\d{18}"));
	mvceditor::FinderClass finder(tofind, mvceditor::FinderClass::REGULAR_EXPRESSION);
	CHECK(finder.Prepare());
	CHECK_EQUAL(false, finder.FindNext(CODE));
}

TEST(FindNextUsingRegularExpressionModeShouldHandleBeforeAndEndOfLine) {
	UnicodeString tofind(UNICODE_STRING_SIMPLE("(?m)^  \\$PI = 3\\.14; //constant $"));
	mvceditor::FinderClass finder(tofind, mvceditor::FinderClass::REGULAR_EXPRESSION);
	CHECK(finder.Prepare());
	int32_t expectedIndex = CODE.indexOf(UNICODE_STRING_SIMPLE("  $PI"));
	CHECK(finder.FindNext(CODE));
	int32_t position, length;
	CHECK(finder.GetLastMatch(position, length));
	CHECK_EQUAL(expectedIndex, position);
	CHECK_EQUAL(0, CODE.compare(position, length, UNICODE_STRING_SIMPLE("  $PI = 3.14; //constant ")));
}

TEST(GetLastReplacementTextShouldReturnMatchedTextInCodeMode) {
	UnicodeString tofind(UNICODE_STRING_SIMPLE("$PI=3.14;"));
	mvceditor::FinderClass finder(tofind);
	finder.ReplaceExpression = UNICODE_STRING_SIMPLE("$PI = 3.1415;");
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	UnicodeString match;
	CHECK(finder.GetLastReplacementText(CODE, match));
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$PI = 3.1415;"), match);
}

TEST(GetLastReplacementTextShouldReturnMatchedTextInExactMode) {
	UnicodeString tofind(UNICODE_STRING_SIMPLE("$PI = 3.14;"));
	mvceditor::FinderClass finder(tofind, mvceditor::FinderClass::EXACT);
	finder.ReplaceExpression = UNICODE_STRING_SIMPLE("$PI = 3.1415;");
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	UnicodeString match;
	CHECK(finder.GetLastReplacementText(CODE, match));
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$PI = 3.1415;"), match);
}


TEST(GetLastReplacementTextShouldReturnMatchedTextInRegularExpressionMode) {
	UnicodeString tofind(UNICODE_STRING_SIMPLE("(?m)^\\s*\\$PI\\s*=\\s*3\\.14;\\s*//(.+)$"));
	mvceditor::FinderClass finder(tofind, mvceditor::FinderClass::REGULAR_EXPRESSION);
	finder.ReplaceExpression = UNICODE_STRING_SIMPLE("  $PI = 3.1415; /* $1 */");
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	UnicodeString match;
	CHECK(finder.GetLastReplacementText(CODE, match));
	UnicodeString expected = UNICODE_STRING_SIMPLE("  $PI = 3.1415; /* constant  */");
	CHECK_EQUAL(expected, match);
}

TEST(GetLastReplacementTextShouldReturnFalseWhenTextDoesNotHaveAMatch) {
	UnicodeString tofind(UNICODE_STRING_SIMPLE("$PI=3.14;"));
	mvceditor::FinderClass finder(tofind);
	finder.ReplaceExpression = UNICODE_STRING_SIMPLE("$PI = 3.1415;");
	CHECK(finder.Prepare());
	CHECK(finder.FindNext(CODE));
	UnicodeString match;
	UnicodeString changedCode = CODE;
	changedCode.findAndReplace(UNICODE_STRING_SIMPLE("3.14"), UNICODE_STRING_SIMPLE("3.33"));
	CHECK_EQUAL(false, finder.GetLastReplacementText(changedCode, match));
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), match);

}

TEST(ReplaceAllShouldReplaceAllMatchesUsingCodeMode) {
	UnicodeString tofind(UNICODE_STRING_SIMPLE("0 === $radius || 0 >= $radius"));
	mvceditor::FinderClass finder(tofind);
	finder.ReplaceExpression = UNICODE_STRING_SIMPLE("$radius != 0 && $radius > 0");
	CHECK(finder.Prepare());
	UnicodeString newCode(CODE);
	CHECK_EQUAL(1, finder.ReplaceAllMatches(newCode));
	UnicodeString expectedCode(CODE);
	expectedCode.findAndReplace(tofind, UNICODE_STRING_SIMPLE("$radius != 0 && $radius > 0"));
	CHECK_EQUAL(expectedCode, newCode);	
}

TEST(ReplaceAllShouldReplaceAllMatchesUsingExactMode) {
	UnicodeString tofind(UNICODE_STRING_SIMPLE("$PI"));
	mvceditor::FinderClass finder(tofind, mvceditor::FinderClass::EXACT);
	finder.ReplaceExpression = UNICODE_STRING_SIMPLE("$_PI");
	CHECK(finder.Prepare());
	UnicodeString newCode(CODE);
	CHECK_EQUAL(3, finder.ReplaceAllMatches(newCode));
	UnicodeString expectedCode(CODE);
	expectedCode.findAndReplace(tofind, UNICODE_STRING_SIMPLE("$_PI"));
	CHECK_EQUAL(expectedCode, newCode);	
}

TEST(ReplaceAllShouldReplaceAllMatchesUsingRegularExpressionMode) {
	UnicodeString tofind(UNICODE_STRING_SIMPLE("\\$(PI)"));
	mvceditor::FinderClass finder(tofind, mvceditor::FinderClass::REGULAR_EXPRESSION);
	finder.ReplaceExpression = UNICODE_STRING_SIMPLE("\\$_$1");
	CHECK(finder.Prepare());
	UnicodeString newCode(CODE);
	CHECK_EQUAL(3, finder.ReplaceAllMatches(newCode));
	UnicodeString expectedCode(CODE);
	expectedCode.findAndReplace(UNICODE_STRING_SIMPLE("$PI"), UNICODE_STRING_SIMPLE("$_PI"));
	CHECK_EQUAL(expectedCode, newCode);	
}

TEST(PrepareShouldReturnFalseWhenExpressionIsEmpty) {
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE(""),  mvceditor::FinderClass::REGULAR_EXPRESSION);
	CHECK_EQUAL(false, finder.Prepare());
}

TEST(PrepareShouldReturnTrueWhenExpressionAndReplaceExpressionAreValid) {
	mvceditor::FinderClass finder(UNICODE_STRING_SIMPLE("(a)bc"),  mvceditor::FinderClass::REGULAR_EXPRESSION);
	finder.ReplaceExpression = UNICODE_STRING_SIMPLE("$1bc");
	CHECK(finder.Prepare());
}

}