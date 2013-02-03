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
#ifndef __MVCEDITORPARSEDTAGFINDERCLASS_H__
#define __MVCEDITORPARSEDTAGFINDERCLASS_H__

#include <globals/TagClass.h>
#include <wx/string.h>
#include <soci/soci.h>
#include <unicode/unistr.h>
#include <vector>

namespace mvceditor {

/**
 * This represents a single 'query' for a tag; ie. this is how the tell the tag finder
 * what to look for.
 * A tag string can be one of: class name, file name, method name, file name with line number
 * Examples:
 * 
 * user.php //looks for the file named user.php
 * user.php:891 //looks for the file named user.php that has line 891
 * User //looks for a class named User
 * User::login //looks for a class named User that has a method called login
 * User:: //looks for all methods and properties for the user class
 * 
 */
class TagSearchClass {

public:

	/**
	 * These are the different near match scenarios that can occur. 
	 * 
	 * CLASS_NAME:  caller asks to search class names or function names.
	 * CLASS_NAME_METHOD_NAME:  caller asks to search method names (class functions). Class name is optional, in which
	 *                          all classes are searched
	 * FILE_NAME:  caller asks to search file names
	 * FILE_NAME_LINE_NUMBER:  caller asks to search file names that have at least a certain amount of lines
	 * NAMESPACE_NAME: caller asks to search for fully qualified namespaces
	 */
	enum ResourceTypes {
		CLASS_NAME,
		CLASS_NAME_METHOD_NAME,
		FILE_NAME,
		FILE_NAME_LINE_NUMBER,
		NAMESPACE_NAME
	};

	TagSearchClass(UnicodeString query);

	std::vector<UnicodeString> GetParentClasses() const;
	void SetParentClasses(const std::vector<UnicodeString>& parents);

	std::vector<UnicodeString> GetTraits() const;
	void SetTraits(const std::vector<UnicodeString>& traits);

	/**
	 * Returns the parsed class name
	 * 
	 * @var UnicodeString
	 */
	UnicodeString GetClassName() const;

	/**
	 * Returns the parsed file name
	 * 
	 * @var UnicodeString
	 */
	UnicodeString GetFileName() const;
	
	/**
	 * Returns the parsed method name
	 * 
	 * @var UnicodeString
	 */
	UnicodeString GetMethodName() const;
	
	/**
	 * Returns the parsed line number
	 * 
	 * @var int
	 */
	int GetLineNumber() const;
	
	/**
	 * Returns the tag type that was given in the Prepare() methods. 
	 * @return ResourceTypes
	 */
	TagSearchClass::ResourceTypes GetResourceType() const;

private:

	/**
	 * the file name parsed from tag string 
	 * 
	 * @var UnicodeString
	 */
	UnicodeString FileName;

	/**
	 * the class name parsed from tag string 
	 * 
	 * @var UnicodeString
	 */
	UnicodeString ClassName;
	
	/**
	 * the method name parsed from tag string
	 * 
	 * @var UnicodeString
	 */
	UnicodeString MethodName;

	/**
	 * The list of classes to look in. When this is given, the method name will be searched
	 * across all of these classes
	 *
	 * @var std::vector<UnicodeString>
	 */
	std::vector<UnicodeString> ParentClasses;

	/**
	 * The list of classes to look in. When this is given, the method name will be searched
	 * across all of these traits
	 *
	 * @var std::vector<UnicodeString>
	 */
	std::vector<UnicodeString> Traits;
	
	/**
	 * The tag type that was parsed
	 */
	ResourceTypes ResourceType;
	
	/**
	 * the line number parsed from tag string 
	 * 
	 * @var int
	 */
	int LineNumber;

};

/**
 * The ParsedTagFinderClass is used to locate source code artifacts (classes, functions, methods, and files). The 
 * general flow of a search is as follows:
 * 
 * 1) The tag cache must be built using TagParserClass. 
 * 2) An object of type ParsedTagFinderClass is instantiated and initialized using the same connection
 *    as the TagParserClass; that way the ParsedTagFinderClass reads whatever tags the TagParserClass found.
 * 3) The search is performed by calling the CollectFullyQualifiedResource or CollectNearMatchResources() methods.
 *    Fully qualified search does exact matching while the near match search performs special logic (see method for
 *    details on search logic).
 * 4) Iteration of the search results is done through the results vector that each of the Collect methods
 *    returns. Note that because this search is done on a database, the returned matches may contain matches from 
 *    files that are no longer in the file system.
 * 
 * The parsed resources are persisted in a SQLite database; the database may be a file backed database or
 * a memory-backed SQLite database.
 *
 * The ParsedTagFinderClass has an exception-free API, no exceptions will be ever thrown, even though
 * it uses SOCI to execute queries (and SOCI uses exceptions). Instead
 * the return values for methods of this class will be false, empty, or zero. Currently this class does not expose 
 * the specific error code from SQLite.
 */
class TagFinderClass {

public:
	
	TagFinderClass();
	virtual ~TagFinderClass();

	/**
	 * this is a virtual method so that each tag finder can use to perform a query to
	 * get all of its class tags. This is a virtual method so that each specific tag finder
	 * can define what table names and columns to query.
	 *
	 * @return vector of ALL parsed class Resources. Be careful as this method may return
	 * many items (10000+). Try to use the CollectXXX() methods as much as possible.
	 * An example use of this method is when wanting to find all classes in a project.
	 * This method will NOT return native PHP classes (ie. PDO, DateTime).
	 */
	virtual std::vector<TagClass> AllNonNativeClasses() = 0;


protected:

	/**
	 * The connection to the database that backs the tag cache
	 * The database will hold all of the files that have been looked at, as well
	 * as all of the resources that were parsed.
	 * This class will NOT own the pointer.
	 */
	soci::session* Session;

	/**
	 * Flag to make sure we initialize the tag database.
	 */
	bool IsCacheInitialized;

	/**
	 * this is a virtual method so that each tag finder can use to perform a query to
	 * get a certain portion of its tags. This is a virtual method so that each specific tag finder
	 * can define what table names and columns to query.
	 *
	 * @param whereCond the WHERE clause of the query to execute (query will be into the resources table)
	 *        this does NOT have the "where" keyword.  examples:  
	 *        "Key = 'ClassName'"
	 *        "Key = 'ClassName::Method' AND Type IN(3)"
	 * @param doLimit if TRUE a max amount of results will be returned, if FALSE then ALL results will be returned
	 *        most of the time you want to set this to TRUE
	 * @return the vector of resources pulled from the statement's results
	 */
	virtual std::vector<mvceditor::TagClass> ResourceStatementMatches(std::string whereCond, bool doLimit) = 0;

	/**
	 * Collects all resources that are files and match the given name. 
	 * Any hits will be returned
	 *
	 * @param search the name of file to look for. 
	 * @param lineNumber if this is greater than zero, then only files that contain this many lines will be returned
	 */
	virtual std::vector<TagClass> CollectNearMatchFiles(const UnicodeString& search, int lineNumber) = 0;

	/**
	 * collect all of the methods that are aliased from all of the traits used by the given classes
	 * @param classNames the names of the classes to search  in. these are the classes that use the
	 *        traits
	 * @param methodName if non-empty then only aliases that begin with this name will be returned
	 */
	virtual std::vector<TagClass> CollectAllTraitAliases(const std::vector<UnicodeString>& classNames, const UnicodeString& methodName) = 0;

	/**
	 * @return all of the traits that any of the given classes use.
	 */
	virtual std::vector<mvceditor::TraitTagClass> FindTraitsByClassName(const std::vector<std::string>& keyStarts) = 0;

	/**
	 * @return all resources whose identifier begins with the given identifier(case insensitive)
	 */
	virtual std::vector<mvceditor::TagClass> FindByIdentifierExactAndTypes(const std::string& identifier, const std::vector<int>& types, bool doLimit) = 0;
	
	/**
	 * @return all resources whose identifier begins with the given identifierStart (case insensitive) AND are of the given type 
	 */
	virtual std::vector<mvceditor::TagClass> FindByIdentifierStartAndTypes(const std::string& identifierStart, const std::vector<int>& types, bool doLimit) = 0;

public:

	/**
	 * Create the tag database that is backed by the given session. 
	 * This method can used to have the tag parser write to either  a file-backed db or a memory db.
	 * By using an in-memory database, lookups are faster.
	 * Note that this method assumes that the schema has already been created.
	 * 
	 * @param soci::session* the session. this class will NOT own the pointer
	 */
	void Init(soci::session* session);
		
	/**
	 * Looks for the tag, using exact, case insensitive matching. Will collect the fully qualified tag name 
	 * itself.
	 * For example for the following class:
	 * 
	 *  class UserClass {
	 *   private $name;
	 *  
	 *  function getName() {
	 *   // ...
	 *  } 
	 * }
	 * 
	 * ONLY the following tag queries will result in a match:
	 * 
	 * UserClass
	 * UserClass::name
	 * UserClass::getName
	 *
	 * This method is NOT tolerant of class hierarchy; meaning that any inherited
	 * if a property name will NOT match then the parent classes are searched. For example; the following code
	 *
	 *   class AdminClass extends UserClass {
	 *   }
	 * 
	 * then the folowing tag queries will NOT result in a match
	 *
	 * AdminClass
	 * AdminClass::name
	 * AdminClass::getName
	 *
	 * To search the hierarchy, the GetResourceParentClassName() and GetResourceTraits() methods can be useful
	 * 
	 * @param tagSearch the resources to look for
	 * @return std::vector<TagClass> the matched resources
	 *         Because this search is done on a database,
	 *         the returned list may contain matches from files that are no longer in 
	 *         the file system.
	 */
	std::vector<TagClass> CollectFullyQualifiedResource(const mvceditor::TagSearchClass& tagSearch);
	
	/**
	 * Looks for the tag, using a near-match logic. Logic is as follows:
	 * 
	 *  1) A class name or function is given:
	 *    a class name or function will match if the class/function starts with the query.  If the query is 'User', 
	 *    the  classes like 'UserAdmin', 'UserGuest' will match, Functions like 'userPrint', 'userIsLoggedIn' 
	 *    will match as well. Note that if a class name or function is not found, then file name search (item 3 below)
	 *    is performed.  This logic makes it easier for the user to search for something without having to type
	 *    in entire file names.
	 * 
	 *  2) A method name is given:
	 *     When looking for a method / property, only match methods or properties.  For example, if the query is
	 *     'User::getN' then methods like 'UserClass::getName', 'UserClass::getNumber' will match. Also, any inherited
	 *     methods are searched; if UserClass inherits a method called 'getNumericId' (but the code itself is in a
	 *     base class) it will match as well. Note that queries need not include a class name; a query can be made
	 *     for '::load' which will match all methods that start with 'load'.
	 * 
	 * 3) A file name is given:
	 *    A file name  matches will be done based on the file name only, not the full path. A file name will match if
	 *    it contains the query.  For example, if the query is 'user.php', then file names like 'guest_user.php',
	 *   'admin_user.php' will match.
	 * 
	 * 4) A file name and line number is given:
	 *    A file name  matches will be done based on the file name only, not the full path. A file name will match if
	 *    it contains the query.  For example, if the query is 'user.php:900', then file names like 'guest_user.php',
	 *   'admin_user.php' will match. Additionally, the file will only match if it has at least as many lines as the
	 *    query.  For example, if the query is 'user.php:900' then all files that would match 'user.php' AND are 
	 *    at least 900 lines long will match.
	 * 
	 * For example the following class:
	 * 
	 * class UserClass {
	 *   private $name;
	 *  
	 *  function getName() {
	 *   // ...
	 *  } 
	 * }
	 * 
	 * the following tag queries will result in a match:
	 * 
	 * UserClass
	 * name
	 * getName
	 * UserC
	 * 
	 * Note that if any exact matches are found, then no near-matches will be collected.
	 * 
	 * @param tagSearch the partial name of resources to look for
	 * @param doCollectFileNames if TRUE, then file name matches will be returned if no
	 *        class / function names are found
	 * @return matches the list of matched resources (max of 50)
	 *         Because this search is done on a database,
	 *         the returned list may contain matches from files that are no longer in 
	 *         the file system.
	 */
	std::vector<TagClass> CollectNearMatchResources(const mvceditor::TagSearchClass& tagSearch,
		bool doCollectFileNames = false);
	
	/**
	 * Get the parent class of a given tag. For example, let's say source code contained two classes: AdminClass and 
	 * UserClass, AdminClass inherited from UserClass.  When this method is called in this manner
	 * 
	 * tagFinder.GetResourceParentClassName(UNICODE_STRING_SIMPLE("AdminClass"))
	 * 
	 * then this method will return "UserClass"
	 * 
	 * @param UnicodeString className the class to search
	 * @param return UnicodeString the class' most immediate parent
	 */
	UnicodeString GetResourceParentClassName(const UnicodeString& className);

	/**
	 * Get the traits used by a given tag. For example, let's say source code contained a class and two traits: UserClass,
	 * a Save trait and a Load trait; AdminClass uses the Save and Load traits.  When this method is called in this manner
	 * 
	 * tagFinder.GetResourceTraits(UNICODE_STRING_SIMPLE("AdminClass"))
	 * 
	 * then this method will return ["Save", "Load"]
	 * 
	 * @param UnicodeString className the class to search for
	 * @param UnicodeString methodName the method to search.  IF and only IF given, then returned traits will be further constraint by 
	 *        looking at the trait conflict resolution (insteadof). In this case, returned traits will have been checked and
	 *        passed the insteadof operator.
	 * @param return vector UnicodeString the class' most immediate used traits (ie won't return the traits' traits). 
	 *        returned vector is not guaranteed to be in any order
	 */
	std::vector<UnicodeString> GetResourceTraits(const UnicodeString& className, const UnicodeString& methodName);
	
	/**
	 * Searches the given text for the position of the given tag.  For example, if the tag matched 3 items
	 * and this method is called with index=2, then text will be searched for tag 2 and will return the 
	 * position of tag 2 in text
	 * 
	 * @param tag the tag match to look for
	 * @param UnicodeString text the text to look in
	 * @param int32_t pos the position where tag starts [in the text]
	 * @param int32_t length the length of the tag [in the text]
	 * @return bool true if match was found in text
	 */
	static bool GetResourceMatchPosition(const TagClass& tag, const UnicodeString& text, int32_t& pos, int32_t& length);
	
	/**
	 * Print the tag cache to stdout.  Useful for debugging only.
	 */
	void Print();
	
	/**
	 * @return bool true if this tag finder has not parsed any files (or those files did not have
	 * any resources). Will also return true if the ONLY file that has been cached is the native functions
	 * file.
	 */
	bool IsFileCacheEmpty();

	/**
	 * @return bool true if this tag finder has not parsed any resources. Will also return true if the 
	 * ONLY resources that have been cached are those for the the native functions
	 * file. Note that this could return TRUE even though the file cache is not empty.
	 */
	bool IsResourceCacheEmpty();

	/**
	 * @return vector of ALL parsed Resources. Be careful as this method may return
	 * many items (10000+). Try to use the CollectXXX() methods as much as possible.
	 * An example use of this method is when wanting to find all functions in a single file.
	 */
	std::vector<TagClass> All();

protected:

	/**
	 * Get the line count from the given file.
	 * 
	 * @param const wxString& fullPath 
	 * @return int line count
	 */
	int GetLineCountFromFile(const wxString& fullPath) const;

private:
		
	/**
	 * Collects all resources that are classes / functions / defines and match the the given Resource search.
	 * Any hits will be returned
	 *
	 * @param tagSearch the name of resources to look for
	 */
	std::vector<TagClass> CollectNearMatchNonMembers(const mvceditor::TagSearchClass& tagSearch);
	
	/**
	 * Collects all resources that are class methods / properties and match the given Resource search.
	 * Any hits will be returned
	 * 
	 * @param tagSearch the name of resources to look for
	 */
	std::vector<TagClass> CollectNearMatchMembers(const mvceditor::TagSearchClass& tagSearch);
	
	/**
	 * Collects all resources that are namespaces and match the given Resource search.
	 * Any hits will be returned
	 */
	std::vector<TagClass> CollectNearMatchNamespaces(const mvceditor::TagSearchClass& tagSearch);
	
	/**
	 * Collect all of the resources that are methods / properties of the given classes.
	 */
	std::vector<TagClass> CollectAllMembers(const std::vector<UnicodeString>& classNames);
		
	/**
	 * Extracts the parent class from a class signature.  The class signature, as parsed by the parser contains a string
	 * "extends ZZZZ ", then this method will return "ZZZZ"
	 * 
	 * @param UnicodeString signature the class signature
	 * @return UnicodeString the parent class name
	 */
	UnicodeString ExtractParentClassFromSignature(const UnicodeString& signature) const;
	
	/**
	 * Look through all of the matches and verifies that the file still actually exists (file has not been deleted).
	 * If the file was deleted, then the match is removed from the matches vector.
	 */
	void EnsureMatchesExist(std::vector<TagClass>& matches);
	
	/**
	 * Get all of the traits that a given class uses. Checking is 
	 * done by looking at the trait use, trait alias, and trait insteadof statements.
	 * 
	 * @param fullyQualifiedClassName fully qualified class name of class to query
	 * @param inheritedTraits the list of traits will be appended to this vector
	 */
	void InheritedTraits(const UnicodeString& fullyQualifiedClassName, std::vector<UnicodeString>& inheritedTraits);

	/**
	 * @return all resources that match the key exact (case insensitive)
	 */
	std::vector<mvceditor::TagClass> FindByKeyExact(const std::string& key);
	
	/**
	 * @return all resources that match the key exact (case insensitive) AND are of the given types
	 */
	std::vector<mvceditor::TagClass> FindByKeyExactAndTypes(const std::string& key, const std::vector<int>& types, bool doLimit);

	/**
	 * @return all resources whose key begins with the given keyStart (case insensitive)
	 */
	std::vector<mvceditor::TagClass> FindByKeyStart(const std::string& keyStart, bool doLimit);
	
	/**
	 * @return all resources whose key begins with the given keyStart (case insensitive) AND are of the given types 
	 */
	std::vector<mvceditor::TagClass> FindByKeyStartAndTypes(const std::string& keyStart, const std::vector<int>& types, bool doLimit);
	
	/**
	 * @return all resources whose key begins with the given at least one of the given keyStart (case insensitive)
	 */
	std::vector<mvceditor::TagClass> FindByKeyStartMany(const std::vector<std::string>& keyStarts, bool doLimit);

	/**
	 * Find the FileTag entry that has the given full path (exact, case insensitive search into
	 * the database).
	 *
	 * @param fullPath the full path to search for
	 * @param fileTag the FileTag itself will be copied here (if found)
	 * @return bool if TRUE it means that this ResourceFinder has encountered the given
	 * file before.
	 */
	bool FindFileTagByFullPathExact(const wxString& fullPath, mvceditor::FileTagClass& fileTag);

	/**
	 * check the database AND the current file's parsed cache to see if the namespace has been seen
	 * before.
	 * @return bool TRUE if the namespace is NOT in the database and its NOT in the current file
	 *  parsed cache
	 */
	bool IsNewNamespace(const UnicodeString& namespaceName);
};

/**
 * the ParsedTagFinderClass will query from the resources db. The resources db
 * has different columns than the detectors db.
 */
class ParsedTagFinderClass : public mvceditor::TagFinderClass {

public:
	
	ParsedTagFinderClass();

	/**
	 * @return vector of ALL parsed class Resources. Be careful as this method may return
	 * many items (10000+). Try to use the CollectXXX() methods as much as possible.
	 * An example use of this method is when wanting to find all classes in a project.
	 * This method will NOT return native PHP classes (ie. PDO, DateTime).
	 */
	virtual std::vector<TagClass> AllNonNativeClasses();

protected:

	/**
	 * @param whereCond the WHERE clause of the query to execute (query will be into the resources table)
	 *        this does NOT have the "where" keyword.  examples:  
	 *        "Key = 'ClassName'"
	 *        "Key = 'ClassName::Method' AND Type IN(3)"
	 * @param doLimit if TRUE a max amount of results will be returned, if FALSE then ALL results will be returned
	 *        most of the time you want to set this to TRUE
	 * @return the vector of resources pulled from the statement's results
	 */
	std::vector<mvceditor::TagClass> ResourceStatementMatches(std::string whereCond, bool doLimit);

	/**
	 * Collects all resources that are files and match the given name. 
	 * Any hits will be returned
	 *
	 * @param search the name of file to look for. 
	 * @param lineNumber if this is greater than zero, then only files that contain this many lines will be returned
	 */
	std::vector<TagClass> CollectNearMatchFiles(const UnicodeString& search, int lineNumber);

	/**
	 * collect all of the methods that are aliased from all of the traits used by the given classes
	 * @param classNames the names of the classes to search  in. these are the classes that use the
	 *        traits
	 * @param methodName if non-empty then only aliases that begin with this name will be returned
	 */
	std::vector<TagClass> CollectAllTraitAliases(const std::vector<UnicodeString>& classNames, const UnicodeString& methodName);

	/**
	 * @return all of the traits that any of the given classes use.
	 */
	std::vector<mvceditor::TraitTagClass> FindTraitsByClassName(const std::vector<std::string>& keyStarts);

	/**
	 * @return all resources whose identifier begins with the given identifier(case insensitive)
	 */
	std::vector<mvceditor::TagClass> FindByIdentifierExactAndTypes(const std::string& identifier, const std::vector<int>& types, bool doLimit);
	
	/**
	 * @return all resources whose identifier begins with the given identifierStart (case insensitive) AND are of the given type 
	 */
	std::vector<mvceditor::TagClass> FindByIdentifierStartAndTypes(const std::string& identifierStart, const std::vector<int>& types, bool doLimit);
};


/**
 * the DetectedTagFinderClass will query from the detectors db. The detectors db
 * has different columns than the resources db.
 */
class DetectedTagFinderClass : public mvceditor::TagFinderClass {

public:
	
	DetectedTagFinderClass();

	/**
	 * @return vector of ALL parsed class Resources. Be careful as this method may return
	 * many items (10000+). Try to use the CollectXXX() methods as much as possible.
	 * An example use of this method is when wanting to find all classes in a project.
	 * This method will NOT return native PHP classes (ie. PDO, DateTime).
	 */
	std::vector<TagClass> AllNonNativeClasses();

protected:

	/**
	 * @param whereCond the WHERE clause of the query to execute (query will be into the resources table)
	 *        this does NOT have the "where" keyword.  examples:  
	 *        "Key = 'ClassName'"
	 *        "Key = 'ClassName::Method' AND Type IN(3)"
	 * @param doLimit if TRUE a max amount of results will be returned, if FALSE then ALL results will be returned
	 *        most of the time you want to set this to TRUE
	 * @return the vector of resources pulled from the statement's results
	 */
	std::vector<mvceditor::TagClass> ResourceStatementMatches(std::string whereCond, bool doLimit);

	/**
	 * Collects all resources that are files and match the given name. 
	 * Any hits will be returned
	 *
	 * @param search the name of file to look for. 
	 * @param lineNumber if this is greater than zero, then only files that contain this many lines will be returned
	 */
	std::vector<TagClass> CollectNearMatchFiles(const UnicodeString& search, int lineNumber);

	/**
	 * collect all of the methods that are aliased from all of the traits used by the given classes
	 * @param classNames the names of the classes to search  in. these are the classes that use the
	 *        traits
	 * @param methodName if non-empty then only aliases that begin with this name will be returned
	 */
	std::vector<TagClass> CollectAllTraitAliases(const std::vector<UnicodeString>& classNames, const UnicodeString& methodName);

	/**
	 * @return all of the traits that any of the given classes use.
	 */
	std::vector<mvceditor::TraitTagClass> FindTraitsByClassName(const std::vector<std::string>& keyStarts);
	
	/**
	 * @return all resources whose identifier begins with the given identifier(case insensitive)
	 */
	std::vector<mvceditor::TagClass> FindByIdentifierExactAndTypes(const std::string& identifier, const std::vector<int>& types, bool doLimit);
	
	/**
	 * @return all resources whose identifier begins with the given identifierStart (case insensitive) AND are of the given type 
	 */
	std::vector<mvceditor::TagClass> FindByIdentifierStartAndTypes(const std::string& identifierStart, const std::vector<int>& types, bool doLimit);
};

}
#endif // __MVCEDITORRESOURCEFINDER_H__
