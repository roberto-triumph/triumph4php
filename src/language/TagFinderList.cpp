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
#include <language/TagFinderList.h>
#include <search/TagFinderClass.h>

std::vector<UnicodeString> mvceditor::TagFinderListClassParents(UnicodeString className, UnicodeString methodName, 
												 const std::vector<mvceditor::TagFinderClass*>& allTagFinders) {
	std::vector<UnicodeString> parents;
	bool found = false;
	UnicodeString classToLookup = className;
	do {

		// each parent class may be located in any of the finders. in practice this code is not as slow
		// as it looks; class hierarchies are usually not very deep (1-4 parents)
		found = false;
		for (size_t i = 0; i < allTagFinders.size(); ++i) {
			UnicodeString parentClass = allTagFinders[i]->ParentClassName(classToLookup);
			if (!parentClass.isEmpty()) {
				found = true;
				parents.push_back(parentClass);
				classToLookup = parentClass;

				// a class can have at most 1 parent, no need to look at other finders
				break;
			}
		}
	} while (found);
	return parents;
}

std::vector<UnicodeString> mvceditor::TagFinderListClassUsedTraits(const UnicodeString& className, 
												  const std::vector<UnicodeString>& parentClassNames, 
												  const UnicodeString& methodName, 
												  const std::vector<mvceditor::TagFinderClass*>& allTagFinders) {

	// trait support; a class can use multiple traits; hence the different logic 
	std::vector<UnicodeString> classesToLookup;
	classesToLookup.push_back(className);
	classesToLookup.insert(classesToLookup.end(), parentClassNames.begin(), parentClassNames.end());
	std::vector<UnicodeString> usedTraits;

	// TODO propagate from enabled projects
	std::vector<wxFileName> emptyVector;
	bool found = false;
	do {
		found = false;
		std::vector<UnicodeString> nextTraitsToLookup;
		for (std::vector<UnicodeString>::iterator it = classesToLookup.begin(); it != classesToLookup.end(); ++it) {
			for (size_t i = 0; i < allTagFinders.size(); ++i) {
				std::vector<UnicodeString> traits = allTagFinders[i]->GetResourceTraits(*it, methodName, emptyVector);
				if (!traits.empty()) {
					found = true;
					nextTraitsToLookup.insert(nextTraitsToLookup.end(), traits.begin(), traits.end());
					usedTraits.insert(usedTraits.end(), traits.begin(), traits.end());
				}
			}
		}

		// next, look for traits used by the traits themselves
		classesToLookup = nextTraitsToLookup;
	} while (found);

	return usedTraits;
}
