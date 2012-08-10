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
#ifndef __MVCEDITORRESOURCEFINDER_H__
#define __MVCEDITORRESOURCEFINDER_H__

#include <search/DirectorySearchClass.h>
#include <search/FindInFilesClass.h>
#include <pelet/ParserClass.h>
#include <MvcEditorString.h>
#include <wx/datetime.h>
#include <wx/string.h>
#include <wx/filename.h>
#include <soci/soci.h>
#include <unicode/unistr.h>
#include <vector>
#include <map>

namespace mvceditor {

// these are defined at the bottom of the file
class ResourceClass; 
class TraitResourceClass;
class FileItemClass;

/**
 * This represents a single 'query' for a resource; ie. this is how the tell the resource finder
 * what to look for.
 * A resource string can be one of: class name, file name, method name, file name with line number
 * Examples:
 * 
 * user.php //looks for the file named user.php
 * user.php:891 //looks for the file named user.php that has line 891
 * User //looks for a class named User
 * User::login //looks for a class named User that has a method called login
 * User:: //looks for all methods and properties for the user class
 * 
 */
class ResourceSearchClass {

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

	ResourceSearchClass(UnicodeString query);

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
	 * Returns the resource type that was given in the Prepare() methods. 
	 * @return ResourceTypes
	 */
	ResourceSearchClass::ResourceTypes GetResourceType() const;

private:

	/**
	 * the file name parsed from resource string 
	 * 
	 * @var UnicodeString
	 */
	UnicodeString FileName;

	/**
	 * the class name parsed from resource string 
	 * 
	 * @var UnicodeString
	 */
	UnicodeString ClassName;
	
	/**
	 * the method name parsed from resource string
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
	 * The resource type that was parsed
	 */
	ResourceTypes ResourceType;
	
	/**
	 * the line number parsed from resource string 
	 * 
	 * @var int
	 */
	int LineNumber;

};

/**
 * The ResourceFinderClass is used to locate source code artifacts (classes, functions, methods, and files). The 
 * general flow of a search is as follows:
 * 
 * 1) ResourceFinderClass object is instantiated
 * 2) The resource cache must be built.  The reason that this needs to be done explicitly is because the caller 
 *    may want to control that process (i.e. manually start, stop it, show a progess bar). 
 * 3) The search is performed by calling the CollectFullyQualifiedResource or CollectNearMatchResources() methods.
 *    Fully qualified search does exact matching while the near match search performs special logic (see method for
 *    details on search logic).
 * 4) Iteration of the search results is done through the results vector that each of the Collect methods
 *    returns.  Because this search is done on a database, the returned matches may contain matches from 
 *    files that are no longer in the file system.
 * 
 * Note that the ResourceFinder cache will need to be refreshed if files are modified after the cache is built. Below
 * is some sample code of steps 1-5:
 * 
 * <code>
 * ResourceFinderClass resourceFinder;
 * resourceFinder.InitMemory();
 * DirectorySearchClass search;
 * if (search.Init(wxT("/home/user/workspace/project/"))) {
 *   resourceFinder.FilesFilter.push_back(wxT("*.php"));
 *   while (search.More()) {
 *     search.Walk(resourceFinder);
 *   }
 *   //now that the resources have been cached, we can query the cache
 *   mvceditor::ResourceSearchClass search(UNICODE_STRING_SIMPLE("UserClass"));
 *   std::vector<mvceditor::ResourceClass> matches = resourceFinder.CollectNearMatchResources(search);
 *   if (!matches.empty()) {
 *     for (size_t i = 0; i < matches.size(); i++)  {
 *       mvceditor::ResourceClass resource = matches[i];
 *       // do something with the resource 
 *       // print the comment resource.Comment
 *       //  print the signature   resource.Signature
 *       // do something with the matched file
 *       UnicodeString className = search.GetClassName(); //the class name parsed from resource (given in to ResourceSearchClass)
 *       UnicodeString methodName = search.GetMethodName(); // the method name parsed from resource (given to ResourceSearchClass)
 *       int lineNumber = search.GetLineNumber(); // the line number parsed from resource (given to ResourceSearchClass)    
 *     }
 *   }
 *   else {
 *     puts("Resource not found\n");
 *   }
 * }
 * else {
 *   puts("Directory not readable.\n");
 * }
 * </code>
 * 
 */
class ResourceFinderClass : public pelet::ClassObserverClass, 
	public pelet::ClassMemberObserverClass, 
	public pelet::FunctionObserverClass, 
	public DirectoryWalkerClass {

public:

	/**
	 * The files to look in
	 * 
	 * @var vector<wxString> a lst of file filters 
	 * Each item in the array will be one wildcard expression; where each
	 * expression can contain either a '*' or a '?' for use in the
	 * wxMatchWild() function.
	 */
	std::vector<wxString> FileFilters;
	
	ResourceFinderClass();
	~ResourceFinderClass();

	/**
	 * Create the resource database that is backed by a SQLite database file.  By 
	 * using a file, we can hold many resources while keeping memory usage in check.
	 * This will also allow us to save parsed resources across program restarts.
	 *
	 * Unless you will be using the resource finder for a single file, then 
	 * use this method. Also note that if a DB file / memory DB was previously open, this method
	 * will close the existing db before the new db is opened.
	 * @param fileName the location where the database file is, or where it 
	 *        will be created if it does not exist.
	 * @param fileParsingBufferSize the size of an internal buffer where parsed resources are initially 
	 *        stored. This is only a hint, the buffer will grow as necessary
	 *        Setting this value to a high value (1024) is good for large projects that have a lot
	 *        resources.
	 */
	void InitFile(const wxFileName& fileName, int fileParsingBufferSize = 32);

	/**
	 * Create the resource database that is backed by a SQLite in-memory database.
	 * By using an in-memory database, lookups are faster. This method would be used
	 * when parsing resources for a single file only. Also note that if a DB 
	 * file / memory DB was previously open, this method will close the existing db before the new db is opened.
	 */
	void InitMemory();
	
	/**
	 * Implement the DirectoryWalkerClass method; will start a transaction
	 */
	void BeginSearch();
		
	/**
	 * Parses the given file for resources. Note that one of the InitXXX() methods
	 * must have been called before a call to this method is made. Also, BeginSearch() must 
	 * have been called already (if using a DirectorySearchClass, the DirectorySearchClass
	 * will take care of calling that method before this method gets called).
	 *
	 *  @param wxString  fileName the full path to the file to be parsed
	 */
	virtual bool Walk(const wxString& fileName);

	/**
	 * Implement the DirectoryWalkerClass method; will commit a transaction
	 */
	void EndSearch();
	
	/**
	 * Parses the given string for resources.  This method would be used, for example, when wanting
	 * to be able to find resources from a file currently being edited by a user but the user
	 * has not yet saved the file so the new contents are not yet on disk.
	 * Note that one of the InitXXX() methods
	 * must have been called before a call to this method is made.
	 * 
	 * @param const wxString&
	 * fileName the full path of the file
	 * @param const UnicodeString& code the PHP source code
	 * @param bool if TRUE then tileName is a new file that is not yet written to disk
	 */
	void BuildResourceCacheForFile(const wxString& fileName, const UnicodeString& code, bool isNew);
	
	/**
	 * Looks for the resource, using exact, case insensitive matching. Will collect the fully qualified resource name 
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
	 * ONLY the following resource queries will result in a match:
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
	 * then the folowing resource queries will NOT result in a match
	 *
	 * AdminClass
	 * AdminClass::name
	 * AdminClass::getName
	 *
	 * To search the hierarchy, the GetResourceParentClassName() and GetResourceTraits() methods can be useful
	 * 
	 * @param resourceSearch the resources to look for
	 * @return std::vector<ResourceClass> the matched resources
	 *         Because this search is done on a database,
	 *         the returned list may contain matches from files that are no longer in 
	 *         the file system.
	 */
	std::vector<ResourceClass> CollectFullyQualifiedResource(const mvceditor::ResourceSearchClass& resourceSearch);
	
	/**
	 * Looks for the resource, using a near-match logic. Logic is as follows:
	 * 
	 *  1) A class name or function is given:
	 *    a class name or function will match if the class/function starts with the query.  If the query is 'User', 
	 *    the  classes like 'UserAdmin', 'UserGuest' will match, Functions like 'userPrint', 'userIsLoggedIn' 
	 *    will match as well.
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
	 * the following resource queries will result in a match:
	 * 
	 * UserClass
	 * name
	 * getName
	 * UserC
	 * 
	 * Note that if any exact matches are found, then no near-matches will be collected.
	 * 
	 * @param resourceSearch the partial name of resources to look for
	 * @return matches the list of matched resources (max of 50)
	 *         Because this search is done on a database,
	 *         the returned list may contain matches from files that are no longer in 
	 *         the file system.
	 */
	std::vector<ResourceClass> CollectNearMatchResources(const mvceditor::ResourceSearchClass& resourceSearch);
	
	/**
	 * Get the parent class of a given resource. For example, let's say source code contained two classes: AdminClass and 
	 * UserClass, AdminClass inherited from UserClass.  When this method is called in this manner
	 * 
	 * resourceFinder.GetResourceParentClassName(UNICODE_STRING_SIMPLE("AdminClass"))
	 * 
	 * then this method will return "UserClass"
	 * 
	 * @param UnicodeString className the class to search
	 * @param return UnicodeString the class' most immediate parent
	 */
	UnicodeString GetResourceParentClassName(const UnicodeString& className);

	/**
	 * Get the traits used by a given resource. For example, let's say source code contained a class and two traits: UserClass,
	 * a Save trait and a Load trait; AdminClass uses the Save and Load traits.  When this method is called in this manner
	 * 
	 * resourceFinder.GetResourceTraits(UNICODE_STRING_SIMPLE("AdminClass"))
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
	 * Searches the given text for the position of the given resource.  For example, if the resource matched 3 items
	 * and this method is called with index=2, then text will be searched for resource 2 and will return the 
	 * position of resource 2 in text
	 * 
	 * @param resource the resource match to look for
	 * @param UnicodeString text the text to look in
	 * @param int32_t pos the position where resource starts [in the text]
	 * @param int32_t length the length of the resource [in the text]
	 * @return bool true if match was found in text
	 */
	static bool GetResourceMatchPosition(const ResourceClass& resource, const UnicodeString& text, int32_t& pos, int32_t& length);
		
	/**
	 * Implement class observer.  When a class has been parsed, add it to the Resource Cache.
	 */
	void ClassFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& signature, 
		const UnicodeString& comment, const int lineNumber);

	/**
	 * When a define has been found, add it to the resource cache
	 */
	void DefineDeclarationFound(const UnicodeString& namespaceName, const UnicodeString& variableName, const UnicodeString& variableValue, 
			const UnicodeString& comment, const int lineNumber);
			
	void TraitAliasFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& traitUsedClassName,
		const UnicodeString& traitMethodName, const UnicodeString& alias, pelet::TokenClass::TokenIds visibility);

	void TraitInsteadOfFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& traitUsedClassName,
		const UnicodeString& traitMethodName, const std::vector<UnicodeString>& insteadOfList);

	void TraitUseFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& fullyQualifiedTraitName);
	
	/**
	 * Implement class member observer.  When a class method has been parsed, add it to the Resource Cache.
	 */
	void MethodFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName, 
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment,
		pelet::TokenClass::TokenIds visibility, bool isStatic, const int lineNumber);
 
	/**
	 * Implement class member observer.  When a class property has been parsed, add it to the Resource Cache.
	 */
	void PropertyFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& propertyName, 
		const UnicodeString& propertyType, const UnicodeString& comment, 
		pelet::TokenClass::TokenIds visibility, bool isConst, bool isStatic, const int lineNumber);
		
	/**
	 * Implement function observer.  When a function has been parsed, add it to the Resource Cache.
	 */
	void FunctionFound(const UnicodeString& namespaceName, const UnicodeString& methodName, 
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment, const int lineNumber);

	/**
	 * Print the resource cache to stdout.  Useful for debugging only.
	 */
	void Print();
	
	/**
	 * @return bool true if this resource finder has not parsed any files (or those files did not have
	 * any resources). Will also return true if the ONLY file that has been cached is the native functions
	 * file.
	 */
	bool IsFileCacheEmpty();

	/**
	 * @return bool true if this resource finder has not parsed any resources. Will also return true if the 
	 * ONLY resources that have been cached are those for the the native functions
	 * file. Note that this could return TRUE even though the file cache is not empty.
	 */
	bool IsResourceCacheEmpty();

	/**
	 * Adds any arbritary resource to the cache. This is tolerate of duplicates; in case a duplicate
	 * of an existing resource is found, then the resource's attributes will be updated.
	 * Equivalence between two resources is determined by comparing the resource's Resource member.
	 * @param dyamicResources the list of resources to add
	 */
	void AddDynamicResources(const std::vector<ResourceClass>& dynamicResources);

	/**
	 * @return vector of ALL parsed Resources. Be careful as this method may return
	 * many items (10000+). Try to use the CollectXXX() methods as much as possible.
	 * An example use of this method is when wanting to find all functions in a single file.
	 */
	std::vector<ResourceClass> All();

	/**
	 * @return vector of ALL parsed class Resources. Be careful as this method may return
	 * many items (10000+). Try to use the CollectXXX() methods as much as possible.
	 * An example use of this method is when wanting to find all classes in a project.
	 * This method will NOT return native PHP classes (ie. PDO, DateTime).
	 */
	std::vector<ResourceClass> AllNonNativeClasses() ;

	/**
	 * Removes all resources from this resource finder entirely. If this
	 * finder was initialized with a backing file, the backing database
	 * file will be truncated also.
	 */
	void Wipe();
	
	/**
	 * set the PHP version to handle
	 */
	void SetVersion(pelet::Versions version);
	
private:
	
	/**
	 * All of the resources that have been parsed during the current walk. This is only a temporary cache that is pushed into while the 
	 * files are being parsed. After all files have been parsed, these resources will be
	 * added to the database.
	 */
	std::vector<ResourceClass> FileParsingCache;

	/**
	 * cache of namespaces, used because the same namespace may be declared in multiple 
	 * files and we don't want to insert multiple rows of the same namespace name. Since
	 * our transaction is commited once ALL files have been parsed, the DB will
	 * not able to help us in determining duplicates.
	 */
	std::map<UnicodeString, int, UnicodeStringComparatorClass> NamespaceCache;
		
	/**
	 * trait info for each class that uses a trait. The trait cache will contain the
	 * aliases and naming resolutions (insteadof). Since a single class can use multiple traits
	 * the value is a vector, each item in the result vector represents one trait being used.
	 * the key to this map is the current file item ID + class name + trait name being parsed
	 * key is a concatenation of file item id, fully qualified class and fully qualified trait
	 * this will make the alias and instead easier to update.
	 * the value will always be a 2 item vector: item 0 is the fully qualified key and 
	 * item 1 is the class only key
	 */
	std::map<UnicodeString, std::vector<TraitResourceClass>, UnicodeStringComparatorClass> TraitCache;
	
	/**
	 * Used to parse through code for classes & methods
	 * 
	 * @var pelet::ParserClass
	 */
	pelet::ParserClass Parser;

	/**
	 * The connection to the database that backs the resource cache
	 * The database will hold all of the files that have been looked at, as well
	 * as all of the resources that were parsed.
	 */
	soci::session Session;

	/*
	 * Will use transaction to lock once instead of before and after every insert
	 */
	soci::transaction* Transaction;

	/**
	 * The full path to the backing SQLite file.  This may be invalid if this 
	 * finder is a memory backed finder (was created with InitMemory() instead of
	 * InitFile() ).
	 */
	wxFileName DbFileName;
		
	/**
	 * The current file item being indexed.  We keep a class-wide member when parsing through many files.
	 * 
	 * @var int fileItemIndex the index of the FileCache entry that corresponds to the file located at fullPath
	 */
	int CurrentFileItemId;

	/**
	 * The initial size of FileParsingCache. This is only a hint, the buffer will grow as necessary
	 * Setting this value to a high value (1024) is good for large projects that have a lot
	 * resources.
	 */
	int FileParsingBufferSize;

	/**
	 * Flag to make sure we initialize the resource database.
	 */
	bool IsCacheInitialized;

	/**
	 * create the database connection to the given db, and create tables to store the parsed resources
	 * Also note that if a DB file / memory DB was previously open, this method
	 * will close the existing db before the new db is opened.
	 * @param wxString dbName, given to SQLite.  db can be a full path to a file or
	 *        the special ":memory:" to create an in-memory db.  The
	 *        file does not need to exist; if it does not exist it will be created.
	 */
	void OpenAndCreateTables(const wxString& dbName);
	
	/**
	 * Goes through the given file and parses out resources.
	 * 
	 * @param wxString fullPath full path to the file to look at
	 * @param bool parseClasses if TRUE, file will be opened and ResourceCache will be populated.  Otherwise, only FileCache
	 *        will be populated.
	 */
	void BuildResourceCache(const wxString& fullPath, bool parseClasses);
	
	/**
	 * Get the line count from the given file.
	 * 
	 * @param const wxString& fullPath 
	 * @return int line count
	 */
	int GetLineCountFromFile(const wxString& fullPath) const;
	
	/**
	 * remove all resources for the given file_item_ids.
	 * 
	 * @param fileItemids the list of file_item_id that will be deleted from the SQLite database.
	 */
	void RemovePersistedResources(const std::vector<int>& fileItemIds);
	
	/**
	 * Collects all resources that are files and match the parsed Resource [given to Prepare()]. 
	 * Any hits will be returned
	 *
	 * @param resourceSearch the name of resources to look for
	 */
	std::vector<ResourceClass> CollectNearMatchFiles(const mvceditor::ResourceSearchClass& resourceSearch);

	/**
	 * Collects all resources that are classes / functions / defines and match the the given Resource search.
	 * Any hits will be returned
	 *
	 * @param resourceSearch the name of resources to look for
	 */
	std::vector<ResourceClass> CollectNearMatchNonMembers(const mvceditor::ResourceSearchClass& resourceSearch);
	
	/**
	 * Collects all resources that are class methods / properties and match the given Resource search.
	 * Any hits will be returned
	 * 
	 * @param resourceSearch the name of resources to look for
	 */
	std::vector<ResourceClass> CollectNearMatchMembers(const mvceditor::ResourceSearchClass& resourceSearch);
	
	/**
	 * Collects all resources that are namespaces and match the given Resource search.
	 * Any hits will be returned
	 */
	std::vector<ResourceClass> CollectNearMatchNamespaces(const mvceditor::ResourceSearchClass& resourceSearch);
	
	/**
	 * Collect all of the resources that are methods / properties of the given classes.
	 */
	std::vector<ResourceClass> CollectAllMembers(const std::vector<UnicodeString>& classNames);
	
	/**
	 * collect all of the methods that are aliased from all of the traits used by the given classes
	 * @param classNames the names of the classes to search  in. these are the classes that use the
	 *        traits
	 * @param methodName if non-empty then only aliases that begin with this name will be returned
	 */
	std::vector<ResourceClass> CollectAllTraitAliases(const std::vector<UnicodeString>& classNames, const UnicodeString& methodName);
	
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
	 * If the file was deleted, then the match is invalidated and the cache for that file removed.
	 */
	void EnsureMatchesExist(std::vector<ResourceClass>& matches);
	
	/**
	 * Get all of the traits that a given class uses. Checking is 
	 * done by looking at the trait use, trait alias, and trait insteadof statements.
	 * 
	 * @param fullyQualifiedClassName fully qualified class name of class to query
	 * @param inheritedTraits the list of traits will be appended to this vector
	 */
	void InheritedTraits(const UnicodeString& fullyQualifiedClassName, std::vector<UnicodeString>& inheritedTraits);

	/**
	 * @param whereCond the WHERE clause of the query to execute (query will be into the resources table)
	 *        this does NOT have the "where" keyword.  examples:  
	 *        "Key = 'ClassName'"
	 *        "Key = 'ClassName::Method' AND Type IN(3)"
	 * @param doLimit if TRUE a max amount of results will be returned, if FALSE then ALL results will be returned
	 *        most of the time you want to set this to TRUE
	 * @return the vector of resources pulled from the statement's results
	 */
	std::vector<mvceditor::ResourceClass> ResourceStatementMatches(std::string whereCond, bool doLimit);

	/**
	 * @return all resources that match the key exact (case insensitive)
	 */
	std::vector<mvceditor::ResourceClass> FindByKeyExact(const std::string& key);
	
	/**
	 * @return all resources that match the key exact (case insensitive) AND are of the given types
	 */
	std::vector<mvceditor::ResourceClass> FindByKeyExactAndTypes(const std::string& key, const std::vector<int>& types, bool doLimit);

	/**
	 * @return all resources whose key begins with the given keyStart (case insensitive)
	 */
	std::vector<mvceditor::ResourceClass> FindByKeyStart(const std::string& keyStart, bool doLimit);
	
	/**
	 * @return all resources whose key begins with the given keyStart (case insensitive) AND are of the given types 
	 */
	std::vector<mvceditor::ResourceClass> FindByKeyStartAndTypes(const std::string& keyStart, const std::vector<int>& types, bool doLimit);
	
	/**
	 * @return all resources whose key begins with the given at least one of the given keyStart (case insensitive)
	 */
	std::vector<mvceditor::ResourceClass> FindByKeyStartMany(const std::vector<std::string>& keyStarts, bool doLimit);

	/**
	 * @return all resources whose identifier begins with the given identifier(case insensitive)
	 */
	std::vector<mvceditor::ResourceClass> FindByIdentifierExactAndTypes(const std::string& identifier, const std::vector<int>& types, bool doLimit);
	
	/**
	 * @return all resources whose identifier begins with the given identifierStart (case insensitive) AND are of the given type 
	 */
	std::vector<mvceditor::ResourceClass> FindByIdentifierStartAndTypes(const std::string& identifierStart, const std::vector<int>& types, bool doLimit);

	/**
	 * Write the file item into the database. The item's FileId member will be set as well.
	 */
	void PersistFileItem(mvceditor::FileItemClass& fileItem);

	/**
	 * Find the FileItem entry that has the given full path (exact, case insensitive search into
	 * the database).
	 *
	 * @param fullPath the full path to search for
	 * @param fileItem the FileItem itself will be copied here (if found)
	 * @return bool if TRUE it means that this ResourceFinder has encountered the given
	 * file before.
	 */
	bool FindFileItemByFullPathExact(const wxString& fullPath, mvceditor::FileItemClass& fileItem);

	/**
	 * add all of the given resources into the database.
	 * @param resources the list of resources that were parsed out
	 * @param int the file that the resources are located in
	 */
	void PersistResources(const std::vector<mvceditor::ResourceClass>& resources, int fileitemId);

	/**
	 * add all of the given trait resources into the database.
	 * @param traits the map of resources that were parsed out
	 */
	void PersistTraits(
		const std::map<UnicodeString, std::vector<mvceditor::TraitResourceClass>, UnicodeStringComparatorClass>& traitMap);


	/**
	 * @return all of the traits that any of the given classes use.
	 */
	std::vector<mvceditor::TraitResourceClass> FindTraitsByClassName(const std::vector<std::string>& keyStarts);

	/**
	 * check the database AND the current file's parsed cache to see if the namespace has been seen
	 * before.
	 * @return bool TRUE if the namespace is NOT in the database and its NOT in the current file
	 *  parsed cache
	 */
	bool IsNewNamespace(const UnicodeString& namespaceName);
};

/**
 * This class represents each resource we have found in the project.  
 */
class ResourceClass {

public:

	/**
	 * All the resources we collect
	 */
	enum Types {
		CLASS,
		METHOD,
		FUNCTION,
		MEMBER,
		DEFINE,
		CLASS_CONSTANT,
		NAMESPACE
	};
	
	/**
	 * The identifer name of this resource. Members will not have a class name with it; ie. a Name method's Identifier will be Name
	 * @var UnicodeString
	 */
	UnicodeString Identifier;
	
	/**
	 * The name of the class that this resource belongs to; only members will have a class name with it: ie. User::Name
	 * The class name will NOT have the namespace
	 * @var UnicodeString
	 */
	UnicodeString ClassName;
	
	/**
	 * The namespace that this function / class is in.
	 */
	UnicodeString NamespaceName;
	
	/**
	 * The resource signature. For methods / functions; it is the entire argument list
	 * For classes; it is the class declaration ("class User extends Object implements ISerializable")
	 * @var UnicodeString
	 */
	UnicodeString Signature;
	
	/**
	 * If this resource item is a method / function / member, ReturnType is the function's return type
	 * @var UnicodeString
	 */
	UnicodeString ReturnType;
	
	/**
	 * The PHPDoc comment attached to the resource.
	 * @var UnicodeString
	 */
	UnicodeString Comment;
	
	/**
	 * The resource item type
	 * @var ReourceClass::Type
	 */
	ResourceClass::Types Type;

	/**
	 * TRUE if this is a protected member
	 */
	bool IsProtected;

	/**
	 * TRUE if this is a private member
	 */
	bool IsPrivate;

	/**
	 * TRUE if this is a static member
	 */
	bool IsStatic;

	/**
	 * TRUE if this is a resource is a 'dynamic' resource; it means that the resource
	 * is not actually in the source; it was either generated via a PHPDoc comment (@property, @method)
	 * or a a Plugin object.
	 */
	bool IsDynamic;

	/**
	 * TRUE if this is a 'native' resource; one of the standard PHP functions / classes (str_*, array_*,
	 * DateTime).  This also includes any extensions (PDO, memcache, etc..) basically anything function
	 * that is documented in php.net.
	 */
	bool IsNative;
	
	ResourceClass();

	static mvceditor::ResourceClass MakeNamespace(const UnicodeString& namespaceName);
	
	/**
	 * Clones a ResourceClass
	 */
	void operator=(const ResourceClass& src);
	
	/**
	 * Defined a comparison function so that sorting algorithms work for resource containers. A resource is "less"
	 * than  another if Resource property is less than the other. (essentially containers are sorted by 
	 * Resource).
	 */
	bool operator<(const ResourceClass& a) const;

	/**
	 * Defined a comparison function so for find function. This will compare resource names in an 
	 * exact, case sensitive manner.
	 */	
	bool operator==(const ResourceClass& a) const;

	/**
	 * set all properties to empty string
	 */
	void Clear();
	
	/**
	 * @return TRUE if given key is the same as this resource's key (case insensitive)
	 */
	bool IsKeyEqualTo(const UnicodeString& key) const;

	/**
	 * @return the FileName that this resource is located in. This may be an
	 *         invalid FileName if this resource is a native or dynamic resource.
	 *         Note that this creates a new wxFileName, which may affect performance
	 */
	wxFileName FileName() const;

	/**
	 * @return the full path that this resource is located in.
	 *         if a resource is a native or dynamic resource.
	 */
	wxString GetFullPath() const;

	/**
	 * @param fullPath the full path where this resource is located
	 */
	void SetFullPath(const wxString& fullPath);

private:
	
	/**
	 * This is the "key" that we will use for lookups. This is the string that will be used to index resources
	 * by so that we can use binary search.
	 * The key can be one of:
	 * - A single identifier (class name, function name, property / method name)
	 * - A full member name (Class::Method)
	 * - A fully namespaced name (\First\Sec\Class)
	 */
	UnicodeString Key;

	/**
	 * Full path to the file where this resource was found. Note that this may not be a valid file
	 * if a resource is a native or dynamic resource.
	 */
	wxString FullPath;

	/**
	 * The index to the file where this resource was found. 
	 */
	int FileItemId;

	/**
	 * Same as FileItemClass::IsNew ie TRUE if this resource was parsed from contents
	 * not yet written to disk
	 * @see FileItemClass::IsNew
	 */
	bool FileIsNew;
	
	/**
	 * The resource finder class will populate FileItemId and FullPath
	 */
	friend class ResourceFinderClass;
};

class TraitResourceClass {
	
public:

	/**
	 * the key is used to perform lookups into this table. The key will be either
	 * 1. The name of the class that uses a trait (same as ClassName property)
	 * 2. The fully qualified name of the class that uses the trait (concatenation of NamespaceName and ClassName)
	 */
	UnicodeString Key;

	/**
	 * the name of the class that uses a trait. This is the name of the class only
	 * (no namespace)
	 */
	UnicodeString ClassName;

	/** 
	 * the namespace of the class that uses the trait. This will be "\" if the class is
	 * in the root namespace
	 */
	UnicodeString NamespaceName;

	/** 
	 * the name of the class of the trait that is being used. This is the name of the class only
	 * (no namespace)
	 */
	UnicodeString TraitClassName;

	/**
	 * the namespace of the trait being used. This will be "\" if the trait is
	 * in the root namespace
	 */
	UnicodeString TraitNamespaceName;
	
	/**
	 * The names of any aliases
	 */
	std::vector<UnicodeString> Aliased;
	
	/**
	 * the names of any class names excluded from being used by the
	 * 'insteadof' operator
	 */
	std::vector<UnicodeString> InsteadOfs;
	
	TraitResourceClass();
};

/**
 * This struct will be used to keep track of which files we have already cached.  The last modified timestamp
 * will be used so that we dont look at files that have not been modified since we last parsed them
 */
class FileItemClass {

public:
	
	/**
	 * The full path to the file where this resource was found
	 */
	wxString FullPath;
	
	/**
	 * The time that this resource was looked at.
	 */
	wxDateTime DateTime;

	/**
	 * unique identifier for this file. Guaranteed to be unique once this itemsd
	 * has been saved to the database.
	 */
	int FileId;
	
	/**
	 * whether or not file has been parsed, could be false if we only looked for files
	 */
	bool IsParsed;

	/**
	 * If TRUE, then this file is not yet written to disk (ie the resource only exists in memory
	 * ( but not yet in the filesystem). This is needed because the finder will do
	 * a sanity check to ensure that the file that contained a match still exists. Iif a file is deleted 
	 * after a file was cached then we want to eliminate that match. But, this sanity checks would kill
	 * matches that were a result of a manual call to BuildResourceCacheForFile. This flag ensures
	 * proper operation (resources that were parsed from code that the user has typed in but no yet
	 * saved are NOT removed).
	 */
	bool IsNew;

	FileItemClass();

	/**
	 * Check to see if this file needs to be parsed. A file needs to be parsed when
	 * 1. it is seen for the first time
	 * 2. has not been parsed yet (IsParsed is FALSE)
	 * 3. it has been modified since the last time we parsed it
	 */
	bool NeedsToBeParsed(const wxDateTime& fileLastModifiedTime) const;

	/**
	 * initialize the members of this file item for insertion into the database.
	 */
	void MakeNew(const wxFileName& fileName, bool isParsed);
};

}
#endif // __MVCEDITORRESOURCEFINDER_H__
