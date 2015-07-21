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
#include <UnitTest++.h>
#include <unicode/ustream.h>  // get the << overloaded operator, needed by UnitTest++
#include <string>
#include "globals/String.h"

SUITE(StringHelperTestClass) {
    TEST(FindPreviousShouldReturnLastIndex) {
        UnicodeString text = UNICODE_STRING_SIMPLE("this is a test of the backwards find");
        UnicodeString expression = UNICODE_STRING_SIMPLE("test");
        int32_t expected = text.indexOf(expression);
        CHECK_EQUAL(expected, t4p::FindPrevious(text, expression, -1));
    }

    TEST(FindPreviousShouldReturnLastIndexWhenMatchIsAtBeginning) {
        UnicodeString text = UNICODE_STRING_SIMPLE("this is a test of the backwards find");
        UnicodeString expression = UNICODE_STRING_SIMPLE("this");
        CHECK_EQUAL(0, t4p::FindPrevious(text, expression, -1));
    }

    TEST(FindPreviousShouldReturnLastIndexWhenMatchIsAtEnd) {
        UnicodeString text = UNICODE_STRING_SIMPLE("this is a test of the backwards find");
        UnicodeString expression = UNICODE_STRING_SIMPLE("find");
        int32_t expected = text.indexOf(expression);
        CHECK_EQUAL(expected, t4p::FindPrevious(text, expression, -1));
    }

    TEST(FindPreviousShouldReturnLastIndexWhenMatchIsAtStart) {
        UnicodeString text = UNICODE_STRING_SIMPLE("this is a test of the backwards find");
        UnicodeString expression = UNICODE_STRING_SIMPLE("backwards");
        int32_t expected = text.indexOf(expression);
        CHECK_EQUAL(expected, t4p::FindPrevious(text, expression, expected + expression.length() - 1));
    }

    TEST(FindPreviousShouldReturnNotFoundWhenMatchIsNotFound) {
        UnicodeString text = UNICODE_STRING_SIMPLE("this is a test of the backwards find");
        UnicodeString expression = UNICODE_STRING_SIMPLE("dackwards");  // 1 char off d vs. b
        CHECK_EQUAL(-1, t4p::FindPrevious(text, expression, -1));
    }


    TEST(FindPreviousShouldReturnNotFoundWhenMatchIsNotBeforeStart) {
        UnicodeString text = UNICODE_STRING_SIMPLE("this is a test of the backwards find");
        UnicodeString expression = UNICODE_STRING_SIMPLE("backwards");
        CHECK_EQUAL(-1, t4p::FindPrevious(text, expression, 29));  // 29 = index of 'd' of "backward"
    }

    TEST(WxToIcuConversions) {
        UnicodeString uniStr = UNICODE_STRING_SIMPLE("this is a test of the conversions");
        wxString wxStr = wxT("this is a test of the conversions");

        CHECK_EQUAL(wxStr, t4p::IcuToWx(uniStr));
        CHECK_EQUAL(uniStr, t4p::WxToIcu(wxStr));

        // convert twice
        UnicodeString test = t4p::WxToIcu(
                                 t4p::IcuToWx(uniStr));
        CHECK_EQUAL(uniStr, test);
    }

    TEST(IcuToCharConversions) {
        UnicodeString uniStr = UNICODE_STRING_SIMPLE("this is a test of the conversions");
        std::string str = "this is a test of the conversions";

        CHECK_EQUAL(str, t4p::IcuToChar(uniStr));
        CHECK_EQUAL(uniStr, t4p::CharToIcu(str.c_str()));

        // convert twice
        UnicodeString test = t4p::CharToIcu(
                                 t4p::IcuToChar(uniStr).c_str());
        CHECK_EQUAL(uniStr, test);
    }
}
