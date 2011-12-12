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
	UnicodeString tofind(UNICODE_STRING_SIMPLE("$PI = 3.14;"));
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