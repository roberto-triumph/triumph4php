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
#include <globals/TagList.h>
#include <globals/ProjectClass.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <vector>

void t4p::TagListRemoveNativeMatches(std::vector<t4p::PhpTagClass>& matches) {
	std::vector<t4p::PhpTagClass>::iterator it = matches.begin();
	while (it != matches.end()) {
		if (it->IsNative) {
			it = matches.erase(it);
		} else {
			it++;
		}
	}
}

void t4p::TagListKeepMatchesFromProjects(std::vector<t4p::PhpTagClass>& matches,
		std::vector<t4p::ProjectClass*> projects, const t4p::FileTypeClass& fileType) {
	std::vector<t4p::PhpTagClass>::iterator tag = matches.begin();
	std::vector<t4p::ProjectClass*>::const_iterator project;
	while (tag != matches.end()) {
		bool isInProjects = false;
		for (project = projects.begin(); project != projects.end(); ++project) {
			isInProjects = (*project)->IsASourceFile(tag->GetFullPath(), fileType);
			if (isInProjects) {
				break;
			}
		}
		if (!isInProjects) {
			tag = matches.erase(tag);
		} else {
			tag++;
		}
	}
}
