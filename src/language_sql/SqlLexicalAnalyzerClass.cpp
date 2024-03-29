/* Generated by re2c 0.13.5 */
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
#include "language_sql/SqlLexicalAnalyzerClass.h"

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


    {
        UChar yych;
        switch (SQL_LEXICAL_ANALYZER_GET_CONDITION()) {
        case SQL_ANY:
            goto sql_lexical_analyzer_ANY;
        case SQL_BLOCK_COMMENT:
            goto sql_lexical_analyzer_BLOCK_COMMENT;
        case SQL_DOUBLE_QUOTE_STRING:
            goto sql_lexical_analyzer_DOUBLE_QUOTE_STRING;
        case SQL_LINE_COMMENT:
            goto sql_lexical_analyzer_LINE_COMMENT;
        case SQL_SINGLE_QUOTE_STRING:
            goto sql_lexical_analyzer_SINGLE_QUOTE_STRING;
        }
        /* *********************************** */
sql_lexical_analyzer_ANY:

        if ((Buffer.Limit - Buffer.Current) < 2) SQL_LEXICAL_ANALYZER_BUFFER_FILL(2);
        yych = *Buffer.Current;
        switch (yych) {
        case 0x00:
            goto sql_lexical_analyzer_2;
        case '\t':
        case '\v':
        case '\f':
        case ' ':
            goto sql_lexical_analyzer_16;
        case '\n':
            goto sql_lexical_analyzer_13;
        case '\r':
            goto sql_lexical_analyzer_15;
        case '"':
            goto sql_lexical_analyzer_4;
        case '#':
            goto sql_lexical_analyzer_11;
        case '\'':
            goto sql_lexical_analyzer_6;
        case '-':
            goto sql_lexical_analyzer_10;
        case '/':
            goto sql_lexical_analyzer_8;
        case ';':
            goto sql_lexical_analyzer_18;
        default:
            goto sql_lexical_analyzer_20;
        }
sql_lexical_analyzer_2:
        ++Buffer.Current;
        {
            return t4p::SqlLexicalAnalyzerClass::SQL_EOF;
        }
sql_lexical_analyzer_4:
        ++Buffer.Current;
        {
            CurrentCondition = SQL_DOUBLE_QUOTE_STRING;
            goto sql_lexical_analyzer_next;
        }
sql_lexical_analyzer_6:
        ++Buffer.Current;
        {
            CurrentCondition = SQL_SINGLE_QUOTE_STRING;
            goto sql_lexical_analyzer_next;
        }
sql_lexical_analyzer_8:
        ++Buffer.Current;
        switch ((yych = *Buffer.Current)) {
        case '*':
            goto sql_lexical_analyzer_24;
        default:
            goto sql_lexical_analyzer_9;
        }
sql_lexical_analyzer_9:
        {
            goto sql_lexical_analyzer_next;
        }
sql_lexical_analyzer_10:
        yych = *++Buffer.Current;
        switch (yych) {
        case '-':
            goto sql_lexical_analyzer_22;
        default:
            goto sql_lexical_analyzer_9;
        }
sql_lexical_analyzer_11:
        ++Buffer.Current;
        {
            CurrentCondition = SQL_LINE_COMMENT;
            goto sql_lexical_analyzer_next;
        }
sql_lexical_analyzer_13:
        ++Buffer.Current;
sql_lexical_analyzer_14:
        {
            Buffer.IncrementLine();
            goto sql_lexical_analyzer_next;
        }
sql_lexical_analyzer_15:
        yych = *++Buffer.Current;
        switch (yych) {
        case '\n':
            goto sql_lexical_analyzer_21;
        default:
            goto sql_lexical_analyzer_14;
        }
sql_lexical_analyzer_16:
        ++Buffer.Current;
        {
            goto sql_lexical_analyzer_next;
        }
sql_lexical_analyzer_18:
        ++Buffer.Current;
        {
            return t4p::SqlLexicalAnalyzerClass::SEMICOLON;
        }
sql_lexical_analyzer_20:
        yych = *++Buffer.Current;
        goto sql_lexical_analyzer_9;
sql_lexical_analyzer_21:
        yych = *++Buffer.Current;
        goto sql_lexical_analyzer_14;
sql_lexical_analyzer_22:
        ++Buffer.Current;
        {
            CurrentCondition = SQL_LINE_COMMENT;
            goto sql_lexical_analyzer_next;
        }
sql_lexical_analyzer_24:
        ++Buffer.Current;
        {
            CurrentCondition = SQL_BLOCK_COMMENT;
            goto sql_lexical_analyzer_next;
        }
        /* *********************************** */
sql_lexical_analyzer_BLOCK_COMMENT:
        if ((Buffer.Limit - Buffer.Current) < 2) SQL_LEXICAL_ANALYZER_BUFFER_FILL(2);
        yych = *Buffer.Current;
        switch (yych) {
        case 0x00:
            goto sql_lexical_analyzer_28;
        case '\n':
            goto sql_lexical_analyzer_30;
        case '\r':
            goto sql_lexical_analyzer_32;
        case '*':
            goto sql_lexical_analyzer_33;
        default:
            goto sql_lexical_analyzer_35;
        }
sql_lexical_analyzer_28:
        ++Buffer.Current;
        {
            return t4p::SqlLexicalAnalyzerClass::SQL_EOF;
        }
sql_lexical_analyzer_30:
        ++Buffer.Current;
sql_lexical_analyzer_31:
        {
            Buffer.IncrementLine();
            goto sql_lexical_analyzer_next;
        }
sql_lexical_analyzer_32:
        yych = *++Buffer.Current;
        switch (yych) {
        case '\n':
            goto sql_lexical_analyzer_38;
        default:
            goto sql_lexical_analyzer_31;
        }
sql_lexical_analyzer_33:
        ++Buffer.Current;
        switch ((yych = *Buffer.Current)) {
        case '/':
            goto sql_lexical_analyzer_36;
        default:
            goto sql_lexical_analyzer_34;
        }
sql_lexical_analyzer_34:
        {
            goto sql_lexical_analyzer_next;
        }
sql_lexical_analyzer_35:
        yych = *++Buffer.Current;
        goto sql_lexical_analyzer_34;
sql_lexical_analyzer_36:
        ++Buffer.Current;
        {
            CurrentCondition = SQL_ANY;
            goto sql_lexical_analyzer_next;
        }
sql_lexical_analyzer_38:
        ++Buffer.Current;
        yych = *Buffer.Current;
        goto sql_lexical_analyzer_31;
        /* *********************************** */
sql_lexical_analyzer_DOUBLE_QUOTE_STRING:
        if ((Buffer.Limit - Buffer.Current) < 2) SQL_LEXICAL_ANALYZER_BUFFER_FILL(2);
        yych = *Buffer.Current;
        switch (yych) {
        case 0x00:
            goto sql_lexical_analyzer_41;
        case '\n':
            goto sql_lexical_analyzer_47;
        case '\r':
            goto sql_lexical_analyzer_49;
        case '"':
            goto sql_lexical_analyzer_45;
        case '\\':
            goto sql_lexical_analyzer_43;
        default:
            goto sql_lexical_analyzer_50;
        }
sql_lexical_analyzer_41:
        ++Buffer.Current;
        {
            return t4p::SqlLexicalAnalyzerClass::SQL_EOF;
        }
sql_lexical_analyzer_43:
        ++Buffer.Current;
        switch ((yych = *Buffer.Current)) {
        case '"':
            goto sql_lexical_analyzer_52;
        default:
            goto sql_lexical_analyzer_44;
        }
sql_lexical_analyzer_44:
        {
            goto sql_lexical_analyzer_next;
        }
sql_lexical_analyzer_45:
        ++Buffer.Current;
        {
            CurrentCondition = SQL_ANY;
            goto sql_lexical_analyzer_next;
        }
sql_lexical_analyzer_47:
        ++Buffer.Current;
sql_lexical_analyzer_48:
        {
            Buffer.IncrementLine();
            goto sql_lexical_analyzer_next;
        }
sql_lexical_analyzer_49:
        yych = *++Buffer.Current;
        switch (yych) {
        case '\n':
            goto sql_lexical_analyzer_51;
        default:
            goto sql_lexical_analyzer_48;
        }
sql_lexical_analyzer_50:
        yych = *++Buffer.Current;
        goto sql_lexical_analyzer_44;
sql_lexical_analyzer_51:
        yych = *++Buffer.Current;
        goto sql_lexical_analyzer_48;
sql_lexical_analyzer_52:
        ++Buffer.Current;
        {
            goto sql_lexical_analyzer_next;
        }
        /* *********************************** */
sql_lexical_analyzer_LINE_COMMENT:
        if ((Buffer.Limit - Buffer.Current) < 2) SQL_LEXICAL_ANALYZER_BUFFER_FILL(2);
        yych = *Buffer.Current;
        switch (yych) {
        case 0x00:
            goto sql_lexical_analyzer_56;
        case '\n':
            goto sql_lexical_analyzer_58;
        case '\r':
            goto sql_lexical_analyzer_60;
        default:
            goto sql_lexical_analyzer_61;
        }
sql_lexical_analyzer_56:
        ++Buffer.Current;
        {
            return t4p::SqlLexicalAnalyzerClass::SQL_EOF;
        }
sql_lexical_analyzer_58:
        ++Buffer.Current;
sql_lexical_analyzer_59:
        {
            CurrentCondition = SQL_ANY;
            Buffer.IncrementLine();
            goto sql_lexical_analyzer_next;
        }
sql_lexical_analyzer_60:
        yych = *++Buffer.Current;
        switch (yych) {
        case '\n':
            goto sql_lexical_analyzer_63;
        default:
            goto sql_lexical_analyzer_59;
        }
sql_lexical_analyzer_61:
        ++Buffer.Current;
        {
            goto sql_lexical_analyzer_next;
        }
sql_lexical_analyzer_63:
        ++Buffer.Current;
        yych = *Buffer.Current;
        goto sql_lexical_analyzer_59;
        /* *********************************** */
sql_lexical_analyzer_SINGLE_QUOTE_STRING:
        if ((Buffer.Limit - Buffer.Current) < 2) SQL_LEXICAL_ANALYZER_BUFFER_FILL(2);
        yych = *Buffer.Current;
        switch (yych) {
        case 0x00:
            goto sql_lexical_analyzer_66;
        case '\n':
            goto sql_lexical_analyzer_72;
        case '\r':
            goto sql_lexical_analyzer_74;
        case '\'':
            goto sql_lexical_analyzer_70;
        case '\\':
            goto sql_lexical_analyzer_68;
        default:
            goto sql_lexical_analyzer_75;
        }
sql_lexical_analyzer_66:
        ++Buffer.Current;
        {
            return t4p::SqlLexicalAnalyzerClass::SQL_EOF;
        }
sql_lexical_analyzer_68:
        ++Buffer.Current;
        switch ((yych = *Buffer.Current)) {
        case '\'':
            goto sql_lexical_analyzer_77;
        default:
            goto sql_lexical_analyzer_69;
        }
sql_lexical_analyzer_69:
        {
            goto sql_lexical_analyzer_next;
        }
sql_lexical_analyzer_70:
        ++Buffer.Current;
        {
            CurrentCondition = SQL_ANY;
            goto sql_lexical_analyzer_next;
        }
sql_lexical_analyzer_72:
        ++Buffer.Current;
sql_lexical_analyzer_73:
        {
            Buffer.IncrementLine();
            goto sql_lexical_analyzer_next;
        }
sql_lexical_analyzer_74:
        yych = *++Buffer.Current;
        switch (yych) {
        case '\n':
            goto sql_lexical_analyzer_76;
        default:
            goto sql_lexical_analyzer_73;
        }
sql_lexical_analyzer_75:
        yych = *++Buffer.Current;
        goto sql_lexical_analyzer_69;
sql_lexical_analyzer_76:
        yych = *++Buffer.Current;
        goto sql_lexical_analyzer_73;
sql_lexical_analyzer_77:
        ++Buffer.Current;
        {
            goto sql_lexical_analyzer_next;
        }
    }
}
