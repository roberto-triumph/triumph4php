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

#include <language/TagParserClass.h>
#include <language/ParsedTagFinderClass.h>
#include <wx/filename.h>
#include <unicode/unistr.h>
#include <vector>


namespace mvceditor {

/**
 * A tag list contains all 3 tags db files used by MVC Editor.  All projects' tags
 * are stored in a SQLite file that persisted and then loaded when MVC Editor starts; this way
 * the user can jump to files & classes without needing to re-index the 
 * entire project. The tag list also contains tags db files for the 
 * native functions (str_*, array_*) and any detected tags from the 
 * TagDetectors database. This class is given to the TagCacheClass.
 */
class TagFinderListClass {

public:

	/**
	 * The object that will parse and persist tags
	 */
	mvceditor::TagParserClass TagParser;

	/**
	 * The object that will be used to lookup project tags
	 * This class will own this pointer
	 */
	mvceditor::ParsedTagFinderClass TagFinder;

	/**
	 * The object that will be used to lookup php native function tags
	 */
	mvceditor::ParsedTagFinderClass NativeTagFinder;

	/**
	 * The object that will be used to lookup tags
	 */
	mvceditor::SqliteFinderClass DetectedTagFinder;

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

private:

	/**
	 * The connections to all sqlite db files
	 * These need to be declared last because the
	 * tagparsers use them; and we want the sessions to be cleaned up last
	 */
	soci::session* TagDbSession;
	soci::session* NativeDbSession;
	soci::session* DetectedTagDbSession;

public:

	TagFinderListClass();

	~TagFinderListClass();

	/**
	 * Opens the SQLite resource db file, or creates it if it does not exist.
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
	 * same as InitGlobalTag() but it takes ownership of an existing session
	 * @param session this object will own the pointer and delete it 
	 */
	void AdoptGlobalTag(soci::session* session, const std::vector<wxString>& phpFileExtensions, 
		const std::vector<wxString>& miscFileExtensions, pelet::Versions version);

	/**
	 * Opens the detector db SQLite file, or creates it if it does not exist.
	 *
	 * @param detectorDbFileName the full path to the SQLite detectors database.
	 *        If this full path does not exist it will be created.
	*/
	void InitDetectorTag(const wxFileName& detectorDbFileName);

	/**
	 * same as InitDetectorTag() but it takes ownership of an existing session
	 * @param session this object will own the pointer and delete it 
	 */
	void AdoptDetectorTag(soci::session* session);

	/**
	 * Opens the native functions SQLite file.  
	 * @param nativeFunctionsDbFileName the full path to the SQLite native functions database.
	 *        This full path MUST exist; it will never be created.
	 */
	void InitNativeTag(const wxFileName& nativeFunctionsDbFileName);

	/**
	 * same as InitNativeTag() but it takes ownership of an existing session
	 * @param session this object will own the pointer and delete it 
	 */
	void AdoptNativeTag(soci::session* session);

	/**
	 * Will update the tag finder by calling Walk(); meaning that the next file
	 * given by the directorySearch will be parsed and its resources will be stored
	 * in the database.
	 *
	 * @see mvceditor::TagParserClass::Walk
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
		const UnicodeString& methodName);

	/**
	 * Figure out a tag's type by looking at all of the initalized finders.
	 * @param resourceToLookup MUST BE fully qualified (class name  + method name,  or function name).  string can have the
	 *        object operator "::" that separates the class and method name.
	 * @return the tag's type; (for methods / functions, it's the return type of the method) could be empty string if type could 
	 *         not be determined 
	 */
	UnicodeString ResolveResourceType(UnicodeString resourceToLookup);

	/**
	 * searches all tag finders for the parent class of the given class
	 */
	UnicodeString ParentClassName(UnicodeString className);

	/**
	 * queries all tag finders for resources that match tagSearch exactly. Any matched tags are
	 * appended to the matches vector.
	 */
	void ExactMatchesFromAll(mvceditor::TagSearchClass& tagSearch, std::vector<mvceditor::TagClass>& matches);
	
	/**
	 * queries all tag finders for resources that nearly match tagSearch (begin with). Any matched tags are
	 * appended to the matches vector.
	 */
	void NearMatchesFromAll(mvceditor::TagSearchClass& tagSearch, std::vector<mvceditor::TagClass>& matches);

	/**
	 * queries all tag finders for trait aliases of that match the given tagSearch extactly. Any matched tag are
	 * appended to the matches vector.
	 */
	void ExactTraitAliasesFromAll(mvceditor::TagSearchClass& tagSearch, std::vector<mvceditor::TagClass>& matches);

	/**
	 * queries all tag finders for trait aliases of that match the given tagSearch extactly. Any matched tag are
	 * appended to the matches vector.
	 */
	void NearMatchTraitAliasesFromAll(mvceditor::TagSearchClass& tagSearch, std::vector<mvceditor::TagClass>& matches);

private:

	/**
	 * create the database connection to the given db
	 *
	 * @param session the db connection to open. this class will not own this pointer.
	 * @param wxString dbName, given to SQLite.  db can be a full path to a file  The
	 *        file does not needs to exist and have been initialized with the schema
	 */
	bool Open(soci::session* session, const wxString& dbName);
};

}

#endif