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
#include <globals/Sqlite.h>
#include <wx/string.h>
#include <soci/soci.h>
#include <unicode/unistr.h>
#include <vector>

namespace mvceditor {

// forward declaration, defined below
class TagResultClass;
class FileTagResultClass;

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
	
	/**
	 * get the classes to search in. this list is used to restrict the
	 * matching to tags to tags from certain classes. this is the list of classes given to
	 * SetClasses() method.  this is usually the base classes 
	 */
	std::vector<UnicodeString> GetParentClasses() const;
	void SetParentClasses(const std::vector<UnicodeString>& parents);

	/**
	 * get the traits to search in. this list is used to restrict the
	 * matching to tags to tags from certain traits. this is the list of traits given to
	 * SetTraits() method.  this is usually the used traits for a class.
	 */
	std::vector<UnicodeString> GetTraits() const;
	void SetTraits(const std::vector<UnicodeString>& traits);

	/**
	 * @return the class name plus all ParentClasses that were set plus all Traits that were set
	 */
	std::vector<UnicodeString> GetClassHierarchy() const;

	/**
	 * get the directories to search in. this list is used to restrict the
	 * matching to tags to tags from certain directories. this is the list of sourceDirs given to
	 * SetSourceDirs() method.  this is usually the directories for the enabled projects.
	 */
	std::vector<wxFileName> GetSourceDirs() const;
	void SetSourceDirs(const std::vector<wxFileName>& sourceDirs);
	
	/**
	 * if a file id is set then matches will be restricted to the file only
	 */
	int GetFileItemId() const;
	void SetFileItemId(int fileItemId);

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
	 * This method can tolerant of class hierarchy; meaning that any inherited
	 * if a property name will match then the parent classes are searched. For example; the following code
	 *
	 *   class AdminClass extends UserClass {
	 *   }
	 * 
	 * then the folowing tag queries will result in a match
	 *
	 * AdminClass
	 * AdminClass::name
	 * AdminClass::getName
	 *
	 * But this depends on the TagSearch::setParentClassNames() be used.
	 * To search the hierarchy, the ParentClassName() and GetResourceTraits() methods can be useful
	 * 
	 *
	 * @return TagResultClass to iterate through the results of the query. The
	 *          returned pointer must be deleted by the caller.
	 */
	mvceditor::TagResultClass* CreateExactResults() const;

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
	 * @return TagResultClass to iterate through the results of the query. The
	 *          returned pointer must be deleted by the caller.
	 */
	mvceditor::TagResultClass* CreateNearMatchResults() const;

	/**
	 * Create a query that will exact match on file names (name or full path)
	 *
	 * @return FileTagResultClass to iterate through the results of the query. The
	 *          returned pointer must be deleted by the caller.
	 */
	mvceditor::FileTagResultClass* CreateExactFileResults() const;

	/**
	 * Create a query that will match near files of the given input.
	 *
	 * @return FileTagResultClass to iterate through the results of the query. The
	 *          returned pointer must be deleted by the caller.
	 */
	mvceditor::FileTagResultClass* CreateNearMatchFileResults() const;

	/**
	 * Returns the parsed class name
	 * 
	 * @return UnicodeString
	 */
	UnicodeString GetClassName() const;

	/**
	 * Returns the parsed file name
	 * 
	 * @return UnicodeString
	 */
	UnicodeString GetFileName() const;
	
	/**
	 * Returns the parsed method name
	 * 
	 * @return UnicodeString
	 */
	UnicodeString GetMethodName() const;

	/**
	 * The parsed namespace name.
	 * @return UnicodeString
	 */
	UnicodeString GetNamespaceName() const;
	
	/**
	 * Returns the parsed line number
	 * 
	 * @return int
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
	 * The namespace name parsed from the tag string
	 * @var UnicodeString
	 */
	UnicodeString NamespaceName;

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
	 * get the directories to search in. this list is used to restrict the
	 * matching to tags to tags from certain directories. this is the list of sourceDirs given to
	 * SetSourceDirs() method.  this is usually the directories for the enabled projects.
	 */
	std::vector<wxFileName> SourceDirs;
	
	/**
	 * If given, searches will be restricted to this file
	 */
	int FileItemId;
	
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
 * The TagResult is used to loop through database rows of the tag table
 */
class TagResultClass : public mvceditor::SqliteResultClass {

public:

	mvceditor::TagClass Tag;

	TagResultClass();

	/**
	 * @param stmt this object will own the statement pointer
	 * @return bool TRUE if there is at least one result
	 */
	bool Init(soci::statement* stmt);

	// TODO: remove this method
	std::vector<mvceditor::TagClass> Matches();

	/**
	 * advance to the next row. after a call to this method, the Tag member variable will contain the 
	 * resulting row.
	 */
	void Next();

protected:

	// variables to bind to the statement
	int Id;
	int FileTagId;
	int SourceId;
	std::string Key;
	std::string Identifier;
	std::string ClassName;
	int Type;
	std::string NamespaceName;
	std::string Signature;
	std::string ReturnType;
	std::string Comment;
	std::string FullPath;
	int IsProtected;
	int IsPrivate;
	int IsStatic;
	int IsDynamic;
	int IsNative;
	int FileIsNew;
	soci::indicator FileTagIdIndicator,
		FullPathIndicator,
		FileIsNewIndicator;
};

class FileTagResultClass : public mvceditor::SqliteResultClass {

public:

	mvceditor::FileTagClass FileTag;

	FileTagResultClass();

	/**
	 * in this method subclasses will build the SQL and execute it.
	 *
	 * @param session the connection
	 * @param doLimit boolean if TRUE there should be a limit on the query
	 */
	bool Prepare(soci::session& session, bool doLimit);

	/**
	 * @param filePart the file name to search for
	 */
	void Set(const UnicodeString& filePart, int lineNumber, bool exactMatch, const std::vector<wxFileName>& sourceDirs);

	// TODO: remove this method
	std::vector<mvceditor::FileTagClass> Matches();
	std::vector<mvceditor::TagClass> MatchesAsTags();

	/**
	 * advance to the next row. after a call to this method, the Tag member variable will contain the 
	 * resulting row.
	 */
	void Next();

private:

	/**
	 * Get the line count from the given file.
	 * 
	 * @param const wxString& fullPath 
	 * @return int line count
	 */
	int GetLineCountFromFile(const wxString& fullPath) const;

	/**
	 * only tags that were found in files located in the given directories will match.
	 * search is recursive, sourceDirs and all of their subdirs are searched
	 */
	std::vector<std::string> SourceDirs;

	/**
	 * the part of the filename to search for
	 */
	std::string FilePart;

	/**
	 * if non-zero, files will be restricted to having at least this many lines
	 */
	int LineNumber;

	/**
	 * if TRUE exact match will be performed
	 */
	bool ExactMatch;

	// variables to bind to the statement
	int FileTagId;
	std::string FullPath;
	int IsNew;
};

class TraitTagResultClass : public mvceditor::SqliteResultClass {

public:

	mvceditor::TraitTagClass TraitTag;

	TraitTagResultClass();

	/**
	 * in this method subclasses will build the SQL and execute it.
	 *
	 * @param session the connection
	 * @param doLimit boolean if TRUE there should be a limit on the query
	 */
	bool Prepare(soci::session& session, bool doLimit);

	/**
	 * @param classNames the class names that we want to search for, can be either class names
	 *                   or fully qualified class names
	 * @param memberName part of the member name to restrict
	 */
	void Set(const std::vector<UnicodeString>& classNames, const UnicodeString& memberName, bool exactMatch, const std::vector<wxFileName>& sourceDirs);

	// TODO: remove this method
	std::vector<mvceditor::TagClass> MatchesAsTags();

	/**
	 * advance to the next row. after a call to this method, the Tag member variable will contain the 
	 * resulting row.
	 */
	void Next();

private:

	/**
	 * only tags that were found in files located in the given directories will match.
	 * search is recursive, sourceDirs and all of their subdirs are searched
	 */
	std::vector<std::string> SourceDirs;

	/**
	 * the class names to search for
	 */
	std::vector<std::string> Keys;

	/**
	 * the part of the member name to search for
	 */
	UnicodeString MemberName;

	/**
	 * if TRUE exact match will be performed
	 */
	bool ExactMatch;

	// variables to bind to the statement
	std::string Key;
	int FileTagId;
	std::string ClassName;
	std::string NamespaceName;
	std::string TraitClassName;
	std::string TraitNamespaceName;
	std::string Aliases;
	std::string InsteadOfs;
};

class ExactMemberTagResultClass : public mvceditor::TagResultClass {

public:

	ExactMemberTagResultClass();

	bool Prepare(soci::session& session, bool doLimit);
	
	virtual void Set(const std::vector<UnicodeString>& classNames, const UnicodeString& memberName, const std::vector<wxFileName>& sourceDirs);

protected:

	std::vector<std::string> Keys;

	std::vector<int> TagTypes;

	std::vector<std::string> SourceDirs;
};

class NearMatchMemberTagResultClass : public mvceditor::ExactMemberTagResultClass {

public:

	NearMatchMemberTagResultClass();

	void Set(const std::vector<UnicodeString>& classNames, const UnicodeString& memberName, int fileItemId, const std::vector<wxFileName>& sourceDirs);

	bool Prepare(soci::session& session, bool doLimit);

private:

	// number of classes we want to search for
	int ClassCount;
	
	// if set, matches will be restricted to this file
	int FileItemId;
};

/**
 * The ParsedTagFinderClass is used to locate source code artifacts (classes, functions, methods, and files). The 
 * general flow of a search is as follows:
 * 
 * 1) The tag cache must be built using TagParserClass. 
 * 2) An object of type ParsedTagFinderClass is instantiated and initialized using the same connection
 *    as the TagParserClass; that way the ParsedTagFinderClass reads whatever tags the TagParserClass found.
 * 3) The search is performed by calling the Exec method.
 *    Fully qualified search does exact matching while the near match search performs special logic (see method for
 *    details on search logic).
 * 4) Iteration of the search results is done by using TagResultClass::Next() and TagResultClass::More() methods. Note 
 *    that because this search is done on a database, the returned matches may contain matches from 
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
class ParsedTagFinderClass : public mvceditor::SqliteFinderClass {

public:
	
	ParsedTagFinderClass();

	/**
	 * Gets all classes, functions, and constants (defines) that were parsed from
	 * the given file.
	 *
	 * @param fullPath the full path of the file to lookup
	 * @return vector of tags all tags that were parsed from the given file that 
	 *         are either a class, function, or define 
	 */
	std::vector<mvceditor::TagClass> ClassesFunctionsDefines(const wxString& fullPath);

	/**
	 * Looks for a class or file tag, using exact, case insensitive matching. 
	 * 
	 * @param tagSearch the resources to look for
	 * @return std::vector<TagClass> the matched resources
	 *         Because this search is done on a database,
	 *         the returned list may contain matches from files that are no longer in 
	 *         the file system.
	 */
	std::vector<mvceditor::TagClass> ExactClassOrFile(const mvceditor::TagSearchClass& tagSearch);

	/**
	 * Looks for a class name using exact, case insensitive matching. Note that namespace
	 * name must also match.
	 * 
	 * @param tagSearch the resources to look for
	 * @return std::vector<TagClass> the matched resources
	 *         Because this search is done on a database,
	 *         the returned list may contain matches from files that are no longer in 
	 *         the file system.
	 */
	std::vector<mvceditor::TagClass> ExactClass(const mvceditor::TagSearchClass& tagSearch);

	/**
	 * Looks for a function name using exact, case insensitive matching. Note that namespace
	 * name must also match. 
	 * 
	 * @param tagSearch the resources to look for
	 * @return std::vector<TagClass> the matched resources
	 *         Because this search is done on a database,
	 *         the returned list may contain matches from files that are no longer in 
	 *         the file system.
	 */
	std::vector<mvceditor::TagClass> ExactFunction(const mvceditor::TagSearchClass& tagSearch);

	/**
	 * Looks for a method name using exact, case insensitive matching.  The method name
	 * is searched from all classes. Note that a limit of at most 10 items is returned
	 * 
	 * @param tagSearch the resources to look for
	 * @param onlyStatic if TRUE, then only static methods will be returned
	 * @return std::vector<TagClass> the matched resources
	 *         Because this search is done on a database,
	 *         the returned list may contain matches from files that are no longer in 
	 *         the file system.
	 */
	std::vector<mvceditor::TagClass> ExactMethod(const mvceditor::TagSearchClass& tagSearch, bool onlyStatic);

	/**
	 * Looks for a property name using exact, case insensitive matching.  The property name
	 * is searched from all classes. Note that a limit of at most 10 items is returned	 
	 * 
	 * @param tagSearch the resources to look for
	 * @param onlyStatic if TRUE, then only static properties (and constants) will be returned
	 * @return std::vector<TagClass> the matched resources
	 *         Because this search is done on a database,
	 *         the returned list may contain matches from files that are no longer in 
	 *         the file system.
	 */
	std::vector<mvceditor::TagClass> ExactProperty(const mvceditor::TagSearchClass& tagSearch, bool onlyStatic);
	
	/**
	 * Looks for the class or file tag, using a near-match logic. Logic is as follows:
	 * 
	 *  1) A class name is given:
	 *    a class name will match if the class starts with the query.  If the query is 'User', 
	 *    the  classes like 'UserAdmin', 'UserGuest' will match, Note that if a class name is not found, 
	 *    then file name search (item 2 below) is performed.  This logic makes it easier for the user to 
	 *    search for something without having to type in entire file names.
	 * 
	 * 2) A file name is given:
	 *    A file name  matches will be done based on the file name only, not the full path. A file name will match if
	 *    it contains the query.  For example, if the query is 'user.php', then file names like 'guest_user.php',
	 *   'admin_user.php' will match.
	 * 
	 * Note that if any exact matches are found, then no near-matches will be collected.
	 * 
	 * @param tagSearch the partial name of resources to look for
	 * @return matches the list of matched resources (max of 50)
	 *         Because this search is done on a database,
	 *         the returned list may contain matches from files that are no longer in 
	 *         the file system.
	 */
	std::vector<mvceditor::TagClass> NearMatchClassesOrFiles(const mvceditor::TagSearchClass& tagSearch);
	
	/**
	 * Get the parent class of a given tag. For example, let's say source code contained two classes: AdminClass and 
	 * UserClass, AdminClass inherited from UserClass.  When this method is called in this manner
	 * 
	 * tagFinder.ParentClassName(UNICODE_STRING_SIMPLE("AdminClass"))
	 * 
	 * then this method will return "UserClass"
	 * 
	 * @param UnicodeString fullyQualifiedClassName the class to search
	 * @param int the file to restrict matches to. can be zero, in which case all files are searched
	 * @param return UnicodeString fully qualified class name of the class' most immediate parent
	 */
	UnicodeString ParentClassName(const UnicodeString& fullyQualifiedClassName, int fileTagId);

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
	 * @param sourceDirs the directories to search in. this list is used to restrict the
	          matching to tags to tags from certain directories. this is usually the directories for the enabled projects.
	 * @param return vector UnicodeString the class' most immediate used traits (ie won't return the traits' traits). 
	 *        returned vector is not guaranteed to be in any order
	 */
	std::vector<UnicodeString> GetResourceTraits(const UnicodeString& className, const UnicodeString& methodName,
		const std::vector<wxFileName>& sourceDirs);
	
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
	static bool GetResourceMatchPosition(const mvceditor::TagClass& tag, const UnicodeString& text, int32_t& pos, int32_t& length);

	/**
	 * retrieves a tag by its ID
	 * 
	 * @param id the ID to query for
	 * @param tag out parameter, will be filled in with the tag data
	 * @return bool TRUE if the ID was found
	 */
	bool FindById(int id, mvceditor::TagClass& tag);
	
	/**
	 * @param int fileTagId file to search for
	 * @return wxString full path to the source directory that the given file tag id is located in
	 */
	wxString SourceDirFromFile(int fileTagId);
	
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
	std::vector<mvceditor::TagClass> All();

	/**
	 * check to see if this tag finder has the given file 
	 *
	 * @param fullPath the path to check. check is done case-insensitive
	 * @return bool TRUE if the full path is in this db 
	 */
	bool HasFullPath(const wxString& fullPath);

	/**
	 * check to see if this tag finder has the given directory
	 *
	 * @param dir the path to check. check is done case-insensitive. note that dir is not a source directory,
	 *        it could be a subdirectory of a source directory
	 * @return bool TRUE if the dir is in this db 
	 */
	bool HasDir(const wxString& dir);

protected:

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

private:
		
	/**
	 * Collects all resources that are classes / functions / defines and match the the given Resource search.
	 * Any hits will be returned. Search is done for all tags that start with the tagSearch string; unless 
	 * the tagSearch string is an exact match in which case only the exact match will be returned
	 *
	 * @param tagSearch the name of resources to look for
	 * @param doClass if TRUE class tags will be collected
	 * @param doDefines if TRUE define tags will be collected
	 * @param doFunctions if TRUE function tags will be collected
	 */
	std::vector<mvceditor::TagClass> NearMatchNonMembers(const mvceditor::TagSearchClass& tagSearch, bool doClasses, bool doDefines, bool doFunctions);
	
	/**
	 * Collects all resources that are class methods / properties and match the given Resource search.
	 * Any hits will be returned
	 * 
	 * @param tagSearch the name of resources to look for
	 */
	std::vector<mvceditor::TagClass> NearMatchMembers(const mvceditor::TagSearchClass& tagSearch);
			
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
	void EnsureMatchesExist(std::vector<mvceditor::TagClass>& matches);
	
	/**
	 * Get all of the traits that a given class uses. Checking is 
	 * done by looking at the trait use, trait alias, and trait insteadof statements.
	 * 
	 * @param fullyQualifiedClassName fully qualified class name of class to query
	 * @param inheritedTraits the list of traits will be appended to this vector
	 */
	void InheritedTraits(const UnicodeString& fullyQualifiedClassName, std::vector<UnicodeString>& inheritedTraits);
	
	/**
	 * check the database AND the current file's parsed cache to see if the namespace has been seen
	 * before.
	 * @return bool TRUE if the namespace is NOT in the database and its NOT in the current file
	 *  parsed cache
	 */
	bool IsNewNamespace(const UnicodeString& namespaceName);
};

}
#endif // __MVCEDITORRESOURCEFINDER_H__
