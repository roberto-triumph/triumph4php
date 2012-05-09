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
#ifndef __MVCEDITORSTRING_H__
#define __MVCEDITORSTRING_H__

#include <unicode/unistr.h>

namespace mvceditor {
	
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

}

#endif