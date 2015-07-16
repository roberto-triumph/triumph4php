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
#ifndef __SQLLEXICALANALYERCLASS_H__
#define __SQLLEXICALANALYERCLASS_H__

#include <pelet/UCharBufferedFileClass.h>
#include <unicode/unistr.h>

namespace t4p {
	
/**
 * Class that helps in breaking a long SQL string into tokens (and individual queries). Note that this is NOT a 
 * SQL validator / parser; it just tokenizes a SQL string.  Currently the only functionality available is
 * to break up a string that contains multiple queries into multiple strings, one for each query.
 * 
 * See the example usage.
 * @example SQLAnalyzerExample.cpp
 */
class SqlLexicalAnalyzerClass {
	
public:

	SqlLexicalAnalyzerClass();
	
	/**
	 * Prepare to tokenize given string.
	 * @return bool true if queries is not empty
	 */
	bool OpenString(const UnicodeString& queries);

	/**
	 * Clean up any resources after lexing. This should be done so that
	 * the string given in the Open() call can be released.
	 */
	void Close();
	
	/**
	 * Method to parse out an entire query out of the text. This method is usually in a 
	 * while() loop that iterates until this method returns false; like this
	 * 
	 * @code
	 *   UnicodeString query;
	 *   while (sqlLexer.NextQuery(query)) {
	 *     // query contains the query
	 *   }
	 * @endcode
	 * 
	 * @param UnicodeString the next query. query will have the
	 * same whitespace as the original text; this will help in
	 * interpreting any SQL errors. Any previous contents in this string will
	 * be discarded.
	 * @return bool TRUE if there is still more input left. (whitespace
	 * or comments do NOT count as input).
	 */
	bool NextQuery(UnicodeString& query);
	
	/**
	 * @return int the line number that the query (set by the NextQuery() method) STARTS IN.  
	 * This is a 1-based number.
	 */
	int GetLineNumber() const;
	
	private:

	/**
	 * the different conditions that the lexer can be in
	 */
	enum Conditions {
		/**
		 * any SQL keywords or identifier
		 */
		SQL_ANY,
		
		/**
		 * A string scalar
		 */
		SQL_SINGLE_QUOTE_STRING,
		
		/**
		 * A string identifier
		 */
		SQL_DOUBLE_QUOTE_STRING,
		
		/**
		 * A SQL single line comment (#, --)
		 */
		SQL_LINE_COMMENT,
		
		/**
		 * multi-line comments (/ *)
		 */
		SQL_BLOCK_COMMENT
	};
	
	/**
	 * The only tokens that are differentiated. These tokens are 
	 * enough to be able to detect the end of a query in the middle of a 
	 * string.
	 */
	enum Tokens {
		SEMICOLON,
		SQL_EOF
	};
	
	/**
	 * Get the next token from the string.
	 */
	int NextToken();
	
	/**
	 * The buffer that will hold the SQL queries.
	 */
	pelet::UCharBufferClass Buffer;
	
	/**
	 * The starting line number of the current query that was returned by NextQuery()
	 */
	int QueryStartLineNumber;
	
	/**
	 * Keeps track of the current lexer state
	 */
	Conditions CurrentCondition;
	
};
	
}

#endif