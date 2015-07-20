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
#include <language_sql/SqlLexicalAnalyzerClass.h>
#include <UnitTest++.h>
#include "unicode/ustream.h"  // get the << overloaded operator, needed by UnitTest++

SUITE(SqlLexicalAnalyzerTestClass) {
TEST(SingleQuery) {
	t4p::SqlLexicalAnalyzerClass lexer;
	UnicodeString query = UNICODE_STRING_SIMPLE("SELECT * FROM users;");
	CHECK(lexer.OpenString(query));
	UnicodeString extracted;
	bool ret = lexer.NextQuery(extracted);
	CHECK(ret);
	CHECK_EQUAL(query, extracted);
	ret = lexer.NextQuery(extracted);
	CHECK_EQUAL(false, ret);
}

TEST(MultipleQueries) {
	t4p::SqlLexicalAnalyzerClass lexer;
	UnicodeString expectedQuery1 =
		UNICODE_STRING_SIMPLE("DELETE FROM users WHERE name='my user';\n\nDELETE FROM users WHERE name='another'");
	UnicodeString queries = expectedQuery1;
	CHECK(lexer.OpenString(queries));
	UnicodeString extracted;
	bool ret = lexer.NextQuery(extracted);
	CHECK(ret);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("DELETE FROM users WHERE name='my user';"), extracted);
	extracted.remove();
	ret = lexer.NextQuery(extracted);
	CHECK(ret);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("DELETE FROM users WHERE name='another'"), extracted);
	ret = lexer.NextQuery(extracted);
	CHECK_EQUAL(false, ret);
}

TEST(Comments) {
	t4p::SqlLexicalAnalyzerClass lexer;
	UnicodeString expectedQuery1 =
		UNICODE_STRING_SIMPLE("/* a ; comment */ -- another; \n #yet another; \n DELETE FROM users WHERE name='my user'");
	UnicodeString queries = expectedQuery1;
	CHECK(lexer.OpenString(queries));
	UnicodeString extracted;
	bool ret = lexer.NextQuery(extracted);
	CHECK(ret);
	CHECK_EQUAL(expectedQuery1, extracted);
}

TEST(StringWithSemicolons) {
	t4p::SqlLexicalAnalyzerClass lexer;
	UnicodeString expectedQuery1 =
		UNICODE_STRING_SIMPLE("DELETE FROM users WHERE name=\"my;user\" or name = 'my;user'");
	UnicodeString queries = expectedQuery1;
	CHECK(lexer.OpenString(queries));
	UnicodeString extracted;
	bool ret = lexer.NextQuery(extracted);
	CHECK(ret);
	CHECK_EQUAL(expectedQuery1, extracted);
}
}
