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
#ifndef T4P_TAGFINDERLIST_H
#define T4P_TAGFINDERLIST_H

#include <language_php/TagParserClass.h>
#include <language_php/ParsedTagFinderClass.h>
#include <wx/filename.h>
#include <unicode/unistr.h>
#include <vector>


namespace t4p {

/**
 * A tag list contains all 3 tags db files used by Triumph.  All projects' tags
 * are stored in a SQLite file that persisted and then loaded when Triumph starts; this way
 * the user can jump to files & classes without needing to re-index the 
 * entire project. The tag list also contains tags db files for the 
 * native functions (str_*, array_*) and any detected tags from the 
 * TagDetectors database. This class is given to the TagCacheClass.
 */
class TagFinderListClass {

	public:

	/**
	 * The connections to all sqlite db files
	 * These need to be declared first because the
	 * tagparsers depend on them
	 */
	soci::session TagDbSession;
	soci::session NativeDbSession;
	soci::session DetectedTagDbSession;

	/**
	 * The object that will parse and persist tags
	 */
	t4p::TagParserClass TagParser;

	/**
	 * The object that will be used to lookup project tags
	 * This class will own this pointer
	 */
	t4p::ParsedTagFinderClass TagFinder;

	/**
	 * The object that will be used to lookup php native function tags
	 */
	t4p::ParsedTagFinderClass NativeTagFinder;

	/**
	 * The object that will be used to lookup tags
	 */
	t4p::SqliteFinderClass DetectedTagFinder;

	/**
	 * TRUE if NativeTagFinder has an opened and valid connection
	 */
	bool IsNativeTagFinderInit;

	/**
	 * TRUE if TagFinder has an opened and valid connection
	 */
	bool IsTagFinderInit;

	/**
	 * TRUE if DetectedTagFinder has an opened and valid connection
	 */
	bool IsDetectedTagFinderInit;

public:

	TagFinderListClass();

	~TagFinderListClass();

	/**
	 * Opens the SQLite resource db file, or creates it if it does not exist.
	 * This method clones data structures (extensions string vectors) where necessary, so that this
	 * TagFinderList instance can be used from a separate thread than where globals
	 * resides
	 *
	 * @param resourceDbFileName the full path to the SQLite resources database.
	 *        If this full path does not exist it will be created.
	 * @param phpFileExtensions the wildcards that hold which files to parse
	 * @param miscFileExtensions the wildcards that hold which files to to record but not parse
	 * @param version the PHP version that the parser will check against
	*/
	void InitGlobalTag(const wxFileName& tagDbFileName, const std::vector<wxString>& phpFileExtensions, 
		const std::vector<wxString>& miscFileExtensions, pelet::Versions version);

	/**
	 * same as InitGlobalTag() but it will open an in-memory database
	 * This method clones data structures (extensions string vectors) where necessary, so that this
	 * TagFinderList instance can be used from a separate thread than where globals
	 * resides
	 * @param session this object will own the pointer and delete it 
	 */
	void CreateGlobalTag(const std::vector<wxString>& phpFileExtensions,
		const std::vector<wxString>& miscFileExtensions, pelet::Versions version);

	/**
	 * Opens the detector db SQLite file, or creates it if it does not exist.
	 *
	 * @param detectorDbFileName the full path to the SQLite detectors database.
	 *        If this full path does not exist it will be created.
	*/
	void InitDetectorTag(const wxFileName& detectorDbFileName);

	/**
	 * same as InitDetectorTag() but will create an in-memory db
	 */
	void CreateDetectorTag();

	/**
	 * Opens the native functions SQLite file.  
	 * @param nativeFunctionsDbFileName the full path to the SQLite native functions database.
	 *        This full path MUST exist; it will never be created.
	 */
	void InitNativeTag(const wxFileName& nativeFunctionsDbFileName);

	/**
	 * Will update the tag finder by calling Walk(); meaning that the next file
	 * given by the directorySearch will be parsed and its resources will be stored
	 * in the database.
	 *
	 * @see t4p::TagParserClass::Walk
	 * @param directorySearch keeps track of the file to parse
	 */
	void  Walk(DirectorySearchClass& directorySearch);

	/**
	 * @param version the PHP version that the parser will check against
	 */
	void SetVersion(pelet::Versions version);

	/**
	 *
	 * 
	 * @return vector of all of the classes that are parent classes of the given
	 *         class. this method will search across all tag finders
	 */
	std::vector<UnicodeString> ClassParents(UnicodeString className, UnicodeString methodName);


	/**
	 * @return vector of all of the traits that are used by any of the given class or parent classes.
	 *         This method will search across all tag finders
	 */
	std::vector<UnicodeString> ClassUsedTraits(const UnicodeString& className, 
		const std::vector<UnicodeString>& parentClassNames, 
		const UnicodeString& methodName,
		const std::vector<wxFileName>& sourceDirs);

	/**
	 * Figure out a tag's type by looking at all of the initalized finders.
	 * @param resourceToLookup MUST BE fully qualified (class name  + method name,  or function name).  string can have the
	 *        object operator "::" that separates the class and method name.
	 * @param sourceDirs will look only for tags in the given sources
	 * @return the tag's type; (for methods / functions, it's the return type of the method) could be empty string if type could 
	 *         not be determined 
	 */
	UnicodeString ResolveResourceType(UnicodeString resourceToLookup, const std::vector<wxFileName>& sourceDirs);

	/**
	 * searches all tag finders for the parent class of the given class
	 */
	UnicodeString ParentClassName(UnicodeString className, int fileTagId);

	/**
	 * queries all tag finders for resources that match tagSearch exactly. Any matched tags are
	 * appended to the matches vector.
	 */
	void ExactMatchesFromAll(t4p::TagSearchClass& tagSearch, std::vector<t4p::PhpTagClass>& matches, const std::vector<wxFileName>& sourceDirs);
	
	/**
	 * queries all tag finders for resources that nearly match tagSearch (begin with). Any matched tags are
	 * appended to the matches vector.
	 */
	void NearMatchesFromAll(t4p::TagSearchClass& tagSearch, std::vector<t4p::PhpTagClass>& matches, const std::vector<wxFileName>& sourceDirs);

	/**
	 * queries all tag finders for trait aliases of that match the given tagSearch extactly. Any matched tag are
	 * appended to the matches vector.
	 */
	void ExactTraitAliasesFromAll(t4p::TagSearchClass& tagSearch, std::vector<t4p::PhpTagClass>& matches);

	/**
	 * queries all tag finders for trait aliases of that match the given tagSearch extactly. Any matched tag are
	 * appended to the matches vector.
	 */
	void NearMatchTraitAliasesFromAll(t4p::TagSearchClass& tagSearch, std::vector<t4p::PhpTagClass>& matches);

private:

};

}

#endif
