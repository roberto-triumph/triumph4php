/**
 * @copyright  2013 Roberto Perpuly
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
#ifndef SRC_GLOBALS_TAGLIST_H_
#define SRC_GLOBALS_TAGLIST_H_

#include <language_php/PhpTagClass.h>
#include <language_php/ParsedTagFinderClass.h>
#include <globals/FileTypeClass.h>
#include <vector>

namespace t4p {
// forward declaration
class ProjectClass;

/**
 * remove matches from the given list that are for php built-in functions. PHP files do not have a source
 * file we can show the user.
 *
 * @param matches any native matches from this given vector will be removed
 */
void TagListRemoveNativeMatches(std::vector<t4p::PhpTagClass>& matches);

/**
 * Filter the given matches by only the matches that belong to the given projects.
 *
 * @param matches the tags list to modify
 * @param projects matches from these projects will be kept; all others will be erased.
 *        this method will not own the project pointers
 * @param fileTypes the configured file types
 */
void TagListKeepMatchesFromProjects(std::vector<t4p::PhpTagClass>& matches,
	std::vector<t4p::ProjectClass*> projects, const t4p::FileTypeClass& fileType);

}  // namespace t4p
#endif  // SRC_GLOBALS_TAGLIST_H_
