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
#ifndef T4P_STRING_H
#define T4P_STRING_H

#include <unicode/unistr.h>
#include <wx/string.h>
#include <vector>
#include <map>

namespace t4p {
/**
 * Case-sensitive string comparator for use as STL Predicate
 */
class UnicodeStringComparatorClass {
	public:
	bool operator()(const UnicodeString& str1, const UnicodeString& str2) const;
};

/**
 * Case-insensitive version of UnicodeString::beginsWith(); will return true if haystack
 * begins with needle performing case-insensitive searching. For example
 *
 * haystack = "thisIsaString"
 * needle  =  "thisisa"
 *
 * These inputs will make this function return TRUE.
 *
 *
 * @param haystack the string to search in
 * @param needle the string to search in
 * @return the index to haystack where needle begins; -1 if needle is not in haystack
 */
bool CaseStartsWith(UnicodeString haystack, UnicodeString needle);

/**
 * This is a collection of functions that are necessary due to the many C++ string
 * objects used by various libraries.  Be careful when using any of these; using
 * any methods of this class should be done as a last resort because these methods
 * copy data. In a nutshell, the bulk of the time UnicodeString should
 * be used, and only when something is going to be displayed to the user then
 * at that point convert it to a wx String.
 */

/**
 * conversion from wxString to ICU string
 */
wxString IcuToWx(const UnicodeString& icu);

/**
 * conversion from ICU String to wxString
 */
UnicodeString WxToIcu(wxString icu);

/**
 * conversion from a C-string to a ICU string.  assuming that
 * C-string is A PLAIN ASCII STRING!
 */
UnicodeString CharToIcu(const char* s);

/**
 * conversion from ICU to C++ string
 */
std::string IcuToChar(const UnicodeString& source);

/**
 * conversion from wxString to C++ string
 */
std::string WxToChar(const wxString& source);

/**
 * conversion from wxString to C++ string
 */
wxString CharToWx(const char* source);

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
int Utf8PosToChar(const char* bytes, int bytesLength, int bytePos);

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
int CharToUtf8Pos(const char* bytes, int bytesLength, int charPos);

/**
 * Looks backwards for expression in text.  Searching is case sensitive.
 * @param UnicodeString text the string to search in
 * @param expression the string to search for
 * @param start the index to start searching in. By default (-1) searching will start from the end.
 * @return int32_t the index of the last occurrence of expression in text; -1 if not found
 */
int32_t FindPrevious(const UnicodeString& text, const UnicodeString& expression, int start = -1);

/**
 * Deep Copy a vector of wxStrings. This is not trivial since wxString uses CopyOnWrite.
 * This will be used when vectors of wxStrings are passed between threads via posted events.
 *
 *
 * @param dest vector to insert/delete strings from
 * @param src vector to copy from
 */
void DeepCopy(std::vector<wxString>& dest, const std::vector<wxString>& src);

/**
 * Deep Copy a vector of wxStrings. This is not trivial since wxString uses CopyOnWrite.
 * This will be used when vectors of wxStrings are passed between threads via posted events.
 *
 *
 * @param dest map to insert/delete strings from
 * @param src map to copy from
 */
void DeepCopy(std::map<wxString, wxString>& dest, const std::map<wxString, wxString>& src);

}

#endif
