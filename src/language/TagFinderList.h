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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef __MVCEDITOR_TAGFINDERLIST_H__
#define __MVCEDITOR_TAGFINDERLIST_H__

#include <unicode/unistr.h>
#include <vector>

namespace mvceditor {

// forward declaration
class ParsedTagFinderClass;


/**
 * These are functions that work with multiple instances of TagFinderClass instances
 * at the same time. We want to query many tag finders at once because classes in one
 * source directory may use classes in another source directory; for example the 
 * base class for a particular class is located in a totally separate directory.
 */

/**
 *
 * 
 * @return vector of all of the classes that are parent classes of the given
 *         class. this method will search across all tag finders
 */
std::vector<UnicodeString> TagFinderListClassParents(UnicodeString className, UnicodeString methodName, 
												 const std::vector<mvceditor::ParsedTagFinderClass*>& allTagFinders);


/**
 * @return vector of all of the traits that are used by any of the given class or parent classes.
 *         This method will search across all tag finders
 */
std::vector<UnicodeString> TagFinderListClassUsedTraits(const UnicodeString& className, 
												  const std::vector<UnicodeString>& parentClassNames, 
												  const UnicodeString& methodName, 
												  const std::vector<mvceditor::ParsedTagFinderClass*>& allTagFinders);

}

#endif