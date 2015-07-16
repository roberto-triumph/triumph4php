/*
 * The MIT License
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
#include <language/SqlLexicalAnalyzerClass.h>

#define SQL_LEXICAL_ANALYZER_BUFFER_FILL(n) { Buffer.AppendToLexeme(n); }

#define SQL_LEXICAL_ANALYZER_GET_CONDITION() CurrentCondition

#define SQL_LEXICAL_ANALYZER_SET_CONDITION(c) CurrentCondition = c

t4p::SqlLexicalAnalyzerClass::SqlLexicalAnalyzerClass() 
	: Buffer()
	, QueryStartLineNumber(0)
	, CurrentCondition(SQL_ANY) {

}

void t4p::SqlLexicalAnalyzerClass::Close() {
	Buffer.Close();
}

bool t4p::SqlLexicalAnalyzerClass::OpenString(const UnicodeString& queries) {
	QueryStartLineNumber = 1;
	return Buffer.OpenString(queries);
}

bool t4p::SqlLexicalAnalyzerClass::NextQuery(UnicodeString& query) {
	Buffer.MarkTokenStart();
	query.remove();
	QueryStartLineNumber = Buffer.GetLineNumber();
	NextToken();
	bool contents = false;
	const UChar *start = Buffer.TokenStart;
	const UChar *end =  Buffer.Current;
	if ((end - start) > 0 && Buffer.Current <= Buffer.Limit) {
	
		// if passed by the EOF need to step back otherwise we insert
		// a null character
		if (Buffer.Current >= Buffer.Limit) {
			end--;
		}
		query.append(Buffer.TokenStart, end - start);
		query.trim();
		contents = !query.isEmpty();
	}
	return contents;
}

int t4p::SqlLexicalAnalyzerClass::GetLineNumber() const {
	return QueryStartLineNumber;
}

int t4p::SqlLexicalAnalyzerClass::NextToken() {
	if (Buffer.HasReachedEnd()) {
		return t4p::SqlLexicalAnalyzerClass::SQL_EOF;
	}

sql_lexical_analyzer_next:

/*!re2c
 
re2c:define:YYCTYPE = UChar;
re2c:define:YYCURSOR = Buffer.Current;
re2c:define:YYLIMIT = Buffer.Limit;
re2c:define:YYMARKER = Buffer.Marker;
re2c:define:YYFILL = SQL_LEXICAL_ANALYZER_BUFFER_FILL;
re2c:define:YYCONDTYPE = t4p::SqlLexicalAnalyzerClass::Conditions;
re2c:define:YYGETCONDITION = SQL_LEXICAL_ANALYZER_GET_CONDITION;
re2c:define:YYSETCONDITION = SQL_LEXICAL_ANALYZER_SET_CONDITION;
re2c:indent:top = 1;
re2c:labelprefix = sql_lexical_analyzer_;
re2c:condenumprefix = SQL_;
re2c:condprefix = sql_lexical_analyzer_;

EOF = [\X0000];
ALL = [\X0000-\X00FF];
ANY = ALL\EOF;
NEWLINE = [\r][\n] | [\r] | [\n];
WHITESPACE = [ \t\v\f];

<*> EOF { return t4p::SqlLexicalAnalyzerClass::SQL_EOF; }

<ANY> ["]  { CurrentCondition = SQL_DOUBLE_QUOTE_STRING; goto sql_lexical_analyzer_next; }
<ANY> ['] { CurrentCondition = SQL_SINGLE_QUOTE_STRING; goto sql_lexical_analyzer_next; }
<ANY> [/][*] { CurrentCondition = SQL_BLOCK_COMMENT; goto sql_lexical_analyzer_next; }
<ANY> [-][-] { CurrentCondition = SQL_LINE_COMMENT; goto sql_lexical_analyzer_next; }
<ANY> [#] { CurrentCondition = SQL_LINE_COMMENT; goto sql_lexical_analyzer_next; }
<ANY> NEWLINE  { Buffer.IncrementLine(); goto sql_lexical_analyzer_next; }
<ANY> WHITESPACE {  goto sql_lexical_analyzer_next; }
<ANY> ';' {  return t4p::SqlLexicalAnalyzerClass::SEMICOLON; }
<ANY> ANY {  goto sql_lexical_analyzer_next; }

<SINGLE_QUOTE_STRING> [\\]['] { goto sql_lexical_analyzer_next; }
<SINGLE_QUOTE_STRING> ['] { CurrentCondition = SQL_ANY; goto sql_lexical_analyzer_next; }
<SINGLE_QUOTE_STRING> NEWLINE { Buffer.IncrementLine(); goto sql_lexical_analyzer_next; }
<SINGLE_QUOTE_STRING> ANY { goto sql_lexical_analyzer_next; }

<DOUBLE_QUOTE_STRING> [\\]["] { goto sql_lexical_analyzer_next; }
<DOUBLE_QUOTE_STRING> ["] { CurrentCondition = SQL_ANY; goto sql_lexical_analyzer_next; }
<DOUBLE_QUOTE_STRING> NEWLINE { Buffer.IncrementLine(); goto sql_lexical_analyzer_next; }
<DOUBLE_QUOTE_STRING> ANY { goto sql_lexical_analyzer_next; }

<LINE_COMMENT> NEWLINE { CurrentCondition = SQL_ANY; Buffer.IncrementLine(); goto sql_lexical_analyzer_next; }
<LINE_COMMENT> ANY { goto sql_lexical_analyzer_next; }

<BLOCK_COMMENT> NEWLINE { Buffer.IncrementLine(); goto sql_lexical_analyzer_next; }
<BLOCK_COMMENT> [*][/] { CurrentCondition = SQL_ANY; goto sql_lexical_analyzer_next; }
<BLOCK_COMMENT> ANY { goto sql_lexical_analyzer_next; }
*/
}