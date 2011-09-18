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
#ifndef STRING_HELPER_CLASS_H
#define STRING_HELPER_CLASS_H

#include <wx/string.h>
#include <unicode/ustdio.h>
#include <unicode/unistr.h>
#include <string>

namespace mvceditor {

/** 
 * This is a collection of methods that are necessary due to the many C++ string
 * objects used by various libraries.  Be careful when using any of these; using
 * any methods of this class should be done as a last resort because these methods
 * copy data. In a nutshell, you the bulk of the time UnicodeString should
 * be used, and only when something is going to be displayed to the user then 
 * at that point convert it to a wx String.
 */
class StringHelperClass {
public:

	/**
	 * conversion from wxString to ICU string
	 */
	static wxString IcuToWx(const UnicodeString& icu);

	/**
	 * conversion from ICU String to wxString
	 */
	static UnicodeString wxToIcu(wxString icu);

	/**
	 * conversion from a C-string to a ICU string.  assuming that
	 * C-string is A PLAIN ASCII STRING!
	 */
	static UnicodeString charToIcu(const char* s);
	
	/**
	 * conversion from ICU to C++ string
	 */
	static std::string IcuToChar(const UnicodeString& source);
	
	/**
	 * Calculate the number of characters in a UTF-8 string.  Since UTF-8 is a multi-byte variable
	 * encoding, we cannot easily tell how many characters (1 character may be 1, 2 ,3, or 4 bytes). 
	 * This method answers the question: What is the character number that is located at byte N of a UTF-8 string?
	 * 
	 * @param const char* bytes the UTF-8 character array (the string to look in)
	 * @param int bytesLength the size of bytes array (in bytes)
	 * @param int bytePos the position number to convert from Returns a negative number when bytesPos >= bytesLength
	 * @return int the number of codepoints (ie. characters). Returns a negative number on invalid UTF-8 strings.
	 */
	static int Utf8PosToChar(const char* bytes, int bytesLength, int bytePos);
	
	/**
	 * Calculate the byte offset of a specific character in a UTF-8 string.  Since UTF-8 is a multi-byte variable
	 * encoding (1 character may be 1, 2 ,3, or 4 bytes), we cannot easily tell how many characters. This method 
	 * answers the question: What is the byte number of character N of a UTF-8 string?
	 * 
	 * @param const char* bytes the UTF-8 character array (the string to look in)
	 * @param int bytesLength the size of bytes array (in bytes)
	 * @param int charPos the character number to convert from.
	 * @return int the byte offset where the character is located. Returns a negative number on invalid UTF-8 strings.
	 */
	static int CharToUtf8Pos(const char* bytes, int bytesLength, int charPos);
	
	/**
	 * Looks backwards for expression in text.  Searching is case sensitive.
	 * @param UnicodeString text the string to search in
	 * @param expression the string to search for
	 * @param start the index to start searching in. By default (-1) searching will start from the end.
	 * @return int32_t the index of the last occurrence of expression in text; -1 if not found
	 */
	static int32_t FindPrevious(const UnicodeString& text, const UnicodeString& expression, int start = -1);
};

}
#endif