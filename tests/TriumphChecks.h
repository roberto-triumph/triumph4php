/*
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

#ifndef TESTS_TRIUMPHCHECKS_H_
#define TESTS_TRIUMPHCHECKS_H_

#include <unicode/ustream.h>  // get the << overloaded operator, needed by UnitTest++
#include <wx/string.h>  // so that UnitTest++ can compare wxStrings by their content

/**
 * This is a "hard" check for vector size. Will check that the vetor is of the given
 * size, if false, then the macro will exit from the current function
 */
#define CHECK_VECTOR_SIZE(expectedSize, list) \
    CHECK_EQUAL((size_t)expectedSize, list.size());\
    if ((size_t)expectedSize != list.size()) return;


/**
 * A macro to reduce the code needed to check unicode strings against a known ascii-only string
 * in a  case sensitive manner. expected is assumed to be an ASCII string.
 */
#define CHECK_UNISTR_EQUALS(expected, actual) \
    CHECK_EQUAL(UNICODE_STRING_SIMPLE(expected), actual);


/**
 * A macro to reduce the code needed to check unicode strings against a known ascii-only string in
 * a case-insensitive manner.
 * expected is assumed to be an ASCII string.
 */
#define CHECK_UNISTR_EQUALS_NO_CASE(expected, actual) \
    CHECK_EQUAL((int8_t)0, actual.caseCompare(UNICODE_STRING_SIMPLE(expected), 0));
#endif  // TESTS_TRIUMPHCHECKS_H_
