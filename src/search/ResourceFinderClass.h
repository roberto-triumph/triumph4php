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
#include <pelet/LexicalAnalyzerClass.h>
#include <pelet/ParserClass.h>
#include <MvcEditorString.h>
#include <wx/datetime.h>
#include <wx/string.h>
#include <wx/filename.h>
#include <unicode/unistr.h>
#include <vector>
#include <map>

namespace mvceditor {

// defined at the bottom of the file
class ResourceClass; 
class TraitResourceClass;

/**
 * The ResourceFinderClass is used to locate source code artifacts (classes, functions, methods, and files). The 
 * general flow of a search is as follows:
 * 
 * 1) ResourceFinderClass object is instantiated
 * 2) The resource cache must be built.  The reason that this needs to be done explicitly is because the caller 
 *    may want to control that process (i.e. manually start, stop it, show a progess bar). 
 * 3) Caller will invoke the Prepare() method, giving it the item to search for.
 * 4) The search is performed by calling the CollectFullyQualifiedResource or CollectNearMatchResources() methods.
 *    Fully qualified search does exact matching while the near match search performs special logic (see method for
 *    details on search logic).
 * 5) Iteration of the search results is done through the GetResourceMatch(), GetResourceMatchCount() methods. The
 *    caller can get the artifact as well as the location of the artifact and its signature.
 * 
 * Note that the ResourceFinder cache will need to be refreshed if files are modified after the cache is built. Below
 * is some sample code of steps 1-5:
 * 
 * <code>
 * ResourceFinderClass resourceFinder;
 * DirectorySearchClass search;
 * if (search.Init(wxT("/home/user/workspace/project/"))) {
 *   resourceFinder.FilesFilter.push_back(wxT("*.php"));
 *   if (resourceFinder.Prepare(wxT("UserClass"))) {
 *     while (search.More()) {
 *       search.Walk(resourceFinder);
 *     }
 *     //now that the resources have been cached, we can query the cache
 *     if (resourceFinder.CollectNearMatchResources()) {
 *       for (int i = 0; i < resourceFinder.GetResourceMatchCount(); i++)  {
 *         mvceditor::ResourceClass resource = resourceFinder.GetResourceMatch(i);
 *         // do something with the resource 
 *         // print the comment resource.Comment
 *         //  print the signature   resource.Signature
 *         // do something with the matched file
 *         UnicodeString className = resourceFinder.GetClassName(); //the class name parsed from resource (given in the Prepare method)
 *         UnicodeString methodName = resourceFinder.GetMethodName(); // the method name parsed from resource (given in the Prepare method)
 *         int lineNumber = resourceFinder.GetLineNumber(); // the line number parsed from resource (given in the Prepare method)
 *         
 *       }
 *     }
 *     else {
 *       puts("Resource not found\n");
 *     }
 *   }
 *   else {
 *     puts("Filter or Expression are not valid.\n");
 *   }
 * }
 * else  {
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
	
	ResourceFinderClass();
	
	/**
	 * The files to look in
	 * 
	 * @var vector<wxString> a lst of file filters 
	 * Each item in the array will be one wildcard expression; where each
	 * expression can contain either a '*' or a '?' for use in the
	 * wxMatchWild() function.
	 */
	std::vector<wxString> FileFilters;
	
	/**
	 * This is the entry point into the resource lookups.  Callers will call the Prepare method giving it a 'query'
	 * ie. the item to search for. After calling this method, the caller can perform the actual lookup by calling the 
	 * CollectNearMatchResources or CollectFullyQualifiedResource methods. See method descriptions for more info on the
	 * search logic.
	 * 
	 * A resource string can be one of: class name, file name, method name, file name with line number
	 * Examples:
 	 * 
 	 * user.php //looks for the file named user.php
	 * user.php:891 //looks for the file named user.php that has line 891
	 * User //looks for a class named User
	 * User::login //looks for a class named User that has a method called login
	 * User:: //looks for all methods and properties for the user class
	 * 
	 * @param wxString resource
	 * @return boolean false if resource string is bad or empty
	 */ 
	 // TODO: make the API simpler and just remove this method altogether
	 // currently Walk() wont work if Prepare() is not called; but many times when we want to 
	 //	index a file we don't have a query string
	 // make the CollectXXX() methods take in a resource query string and return a vector of results
	 // that way we wont need the GetResourceMatchXXX() methods either
	bool Prepare(const wxString& resource);
		
	/**
	 * Parses the given file for resources
	 *
	 *  @param wxString  fileName the full path to the file to be parsed
	 */
	virtual bool Walk(const wxString& fileName);
	
	/**
	 * Builds cache for PHP native functions. After a call to this method, CollectNearMatchResources, 
	 * GetResourceSignature methods will work for PHP native functions (array, string, file functions ...).
	 * @return bool TRUE if native functions file exists and was able to be read
	 */
	bool BuildResourceCacheForNativeFunctions();
	
	/**
	 * Parses the given string for resources.  This method would be used, for example, when wanting
	 * to be able to find resources from a file currently being edited by a user but the user
	 * has not yet saved the file so the new contents are not yet on disk.
	 * 
	 * @param const wxString&
	 * fileName the full path of the file
	 * @param const UnicodeString& code the PHP source code
	 * @param bool if TRUE then tileName is a new file that is not yet written to disk
	 */
	void BuildResourceCacheForFile(const wxString& fileName, const UnicodeString& code, bool isNew);
	
	/**
	 * Looks for the resource, using exact, case insensitive matching. Will collect the fully qualified resource name 
	 * itself. Collected resource names can be accessed through GetResourceMatch() method
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
	 * This method is also tolerant of class hierarchy; meaning that any inherited
	 * if a property name does not match then the parent classes are searched. For example; the following code
	 *
	 *   class AdminClass extends UserClass {
	 *   }
	 * 
	 * then the folowing resource queries will result in a match
	 *
	 * AdminClass
	 * AdminClass::name
	 * AdminClass::getName
	 * 
	 * @return bool returns true if resource was found
	 */
	bool CollectFullyQualifiedResource();
	
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
	 * Collected resources can be accessed through GetResourceMatch() method.
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
	 * @return bool returns true if resource was found
	 */
	bool CollectNearMatchResources();
	
	/**
	 * Get the parent class of a given resource. For example, let's say source code contained two classes: AdminClass and 
	 * UserClass, AdminClass inherited from UserClass.  When this method is called in this manner
	 * 
	 * resourceFinder.GetResourceParentClass(UNICODE_STRING_SIMPLE("AdminClass"))
	 * 
	 * then this method will return "UserClass"
	 * 
	 * @param UnicodeString className the class to search
	 * @param UnicodeString methodName the method to search.  IF and only IF given, then returned parent class will contain the given method.
	 * @param return UnicodeString the class' most immediate parent that contains method
	 */
	UnicodeString GetResourceParentClassName(const UnicodeString& className, const UnicodeString& methodName) const;

	/**
	 * @param className the class name to check
	 * @return all of the classes that the given class inherits from (parent, grandparent, and all the way up).
	 * Note that only "extended" classes will be returned; interfaces will NOT be returned.
	 */
	std::vector<UnicodeString> ClassHierarchy(const UnicodeString& className) const;
	
	/**
	 * Calculates the number of files that had matches. 
	 * 
	 * @return int the number of files under FindPath that had at least one match for this resource
	 */
	size_t GetResourceMatchCount() const;

	/**
	 * Returns the full resource name that matched this expression.
	 * 
	 * @param int the resource match index  0 < i < GetResourceMatchCount().  
	 * @return ResourceClass returns empty reource on invalid index
	 */
	ResourceClass GetResourceMatch(size_t index) const;

	/**
	 * Returns the full path to  the resource that matched this expression.
	 * 
	 * @param size_t the resource match index  0 < i < GetResourceMatchCount().  
	 * @return wxString returns empty string on invalid index
	 */
	wxString GetResourceMatchFullPath(size_t index) const;
	
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
	 * Returns the parsed class name
	 * 
	 * @var UnicodeString
	 */
	UnicodeString GetClassName() const;
	
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
	ResourceFinderClass::ResourceTypes GetResourceType() const;
	
	/**
	 * Implement class observer.  When a class has been parsed, add it to the Resource Cache.
	 */
	void ClassFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& signature, 
		const UnicodeString& comment, const int lineNumber);

	/**
	 * When a define has been found, add it to the resource cache
	 */
	void DefineDeclarationFound(const UnicodeString& variableName, const UnicodeString& variableValue, 
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
	void Print() const;
	
	/**
	 * @return bool true if this resource finder has not parsed any files (or those files did not have
	 * any resources). Will also return true if the ONLY file that has been cached is the native functions
	 * file.
	 */
	bool IsFileCacheEmpty() const;

	/**
	 * @return bool true if this resource finder has not parsed any resources. Will also return true if the 
	 * ONLY resources that have been cached are those for the the native functions
	 * file. Note that this could return TRUE even though the file cache is not empty.
	 */
	bool IsResourceCacheEmpty() const;

	/**
	 * This method copies the internal resource lists from the given object to this. This method can
	 * be used to create a new instance of ResourceFinder without needing to re-scan the entire
	 * file system.  After a call to this method both src and this will have the same resources
	 * (but as DIFFERENT COPIES).
	 *
	 * Note that the matches are NOT copied.
	 */
	void CopyResourcesFrom(const ResourceFinderClass& src);

	/**
	 * This method will remove all resources that were found in fileName, the add
	 * all of the resources from the given finder.
	 */
	void UpdateResourcesFrom(const wxString& fullPath, const ResourceFinderClass& src);

	/**
	 * Adds any arbritary resource to the cache. This is tolerate of duplicates; in case a duplicate
	 * of an existing resource is found, then the resource's attributes will be updated.
	 * Equivalence between two resources is determined by comparing the resource's Resource member.
	 * @param dyamicResources the list of resources to add
	 */
	void AddDynamicResources(const std::vector<ResourceClass>& dynamicResources);
	
	/**
	 * Sorts the resources if they are not already sorted. This is a relatively expensive operation.
	 * After sorting, we calls to Collect* methods will be fast.
	 * This method should be called once all files have been parsed. It is not necessary to call this;
	 * but it may make sense to call it explicitly after parsing an entire project while the background thread
	 * is still running (that way the caller can "control" that the sorting be done in the background thread).
	 *
	 * Also clears out all of the previous matches.
	 */
	void EnsureSorted();

	/**
	 * @return the list of file names  (full paths) of all of the files that have been parsed by this
	 * object.
	 */
	std::vector<wxString> GetCachedFiles() const;

	/**
	 * Writes the parsed resources into the given file. Format is this:
	 *
	 * TYPE,File name,Class name or Function name,Method / property name 
	 *
	 * Where TYPE is either CLASS, METHOD, MEMBER, FUNCTION. Method / property name 
	 * may be empty for CLASS or FUNCTION types. File name is the full path
	 *
	 * Examples:
	 * CLASS,/home/users/public_html/controllers/my_controller.php,MyController,
	 * FUNCTION,/home/users/public_html/lib/calc.php,calculate,
	 * METHOD,/home/users/public_html/controllers/my_controller.php,MyController,index
	 *
	 * @return bool TRUE if file was successfully written to.
	 */
	bool Persist(const wxFileName& outputFile) const;

	/**
	 * @return vector of ALL parsed Resources. Be careful as this method may return
	 * many items (10000+). Try to use the CollectXXX() methods as much as possible.
	 * An example use of this method is when wanting to find all functions in a single file.
	 */
	std::vector<ResourceClass> All() const;

	/**
	 * @return vector of ALL parsed class Resources. Be careful as this method may return
	 * many items (10000+). Try to use the CollectXXX() methods as much as possible.
	 * An example use of this method is when wanting to find all classes in a project.
	 * This method will NOT return native PHP classes (ie. PDO, DateTime).
	 */
	std::vector<ResourceClass> AllNonNativeClasses() const;

	/**
	 * Removes all resources from the cache entirely.
	 */
	void Clear();
	
	/**
	 * set the PHP version to handle
	 */
	void SetVersion(pelet::Versions version);

	/**
	 * Parses a resource string into its components: class name, file name, method name, line number
	 * resource string is a string that contain a file name and line number or
	 * class name or method name. Examples:
	 * 
	 * user.php //looks for the file named user.php
	 * user.php:891 //looks for the file named user.php that has line 891
	 * User //looks for a class named User
	 * User::login //looks for a class named User that has a method called login
	 * 
	 * @param const wxString& resource the resource to look for
	 * @param fileName if resource is a denifitely a file (name+ extension) this variable will be filled
	 * @param className if resoruce is a class this variable will be filled
	 * @param methodName if resource is a method name this variable will be filled
	 * @param lineNumber if resource has a line number this variable will be filled
	 */
	static ResourceTypes ParseGoToResource(const wxString& resource, UnicodeString& fileName, UnicodeString& className, 
		UnicodeString& methodName, int& lineNumber);
	
private:
	
	/**
	 * This struct will be used to keep track of which files we have already cached.  The last modified timestamp
	 * will be used so that we dont look at files that have not been modified since we last parsed them
	 */
	struct FileItem {
		
		/**
		 * The full path to the file where this resource was found
		 */
		wxString FullPath;
		
		/**
		 * The time that this resource was looked at.
		 */
		wxDateTime DateTime;
		
		/**
		 * whether or not file has been parsed, could be false if we only looked for files
		 */
		bool Parsed;

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
	};
	
	/**
	 * All of the classes / functions resources found.
	 */
	std::vector<ResourceClass> ResourceCache;
	
	/**
	 * All of the methods / properties / constants found.
	 */
	std::vector<ResourceClass> MembersCache;
	
	/**
	 * All of the namespaces found
	 */
	std::vector<ResourceClass> NamespaceCache;
	
	/**
	 * trait info for each class that uses a trait. The trait cache will contain the
	 * aliases and naming resolutions (insteadof). Since a single class can use multiple traits
	 * the value is a vector, each item in the result vector represents one trait being used.
	 */
	std::map<UnicodeString, std::vector<TraitResourceClass>, UnicodeStringComparatorClass> TraitCache;
	
	/**
	 * All the files that have been looked at.
	 */
	std::vector<FileItem> FileCache;
	
	/**
	 * Full paths to all the resource matches (the actual structs)
	 */
	std::vector<ResourceClass>  Matches;
	
	/**
	 * Used to parse through code for classes & methods
	 * 
	 * @var pelet::LexicalAnalyzerClass 
	 */
	pelet::LexicalAnalyzerClass Lexer;
	
	/**
	 * Used to parse through code for classes & methods
	 * 
	 * @var pelet::ParserClass
	 */
	pelet::ParserClass Parser;
	
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
	 * The resource type that was parsed
	 */
	ResourceTypes ResourceType;
	
	/**
	 * the line number parsed from resource string 
	 * 
	 * @var int
	 */
	int LineNumber;
	
	/**
	 * The current file item being indexed.  We keep a class-wide member when parsing through many files.
	 * 
	 * @var int fileItemIndex the index of the FileCache entry that corresponds to the file located at fullPath
	 */
	int CurrentFileItemIndex;
	
	/**
	 * Flag that will signal when the cache has been sorted.  The cache will be sorted only until
	 * a CollectNearMatches or CollectFullyQualifiedResource methods are called, and it will
	 * be unsorted when a call to BuildResourceCache is called
	 * 
	 */
	bool IsCacheSorted;
	
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
	 * remove all resources for the file.
	 * 
	 * @param int fileItemIndex the index of the FileCache entry that corresponds to the file located at fullPath
	 */
	void RemoveCachedResources(int fileItemIndex);
	
	/**
	 * Collects all resources that are files and match the parsed Resource [given to Prepare()]. 
	 * Any hits will be accumulated in Matches vector.
	 */
	void CollectNearMatchFiles();

	/**
	 * Collects all resources that are classes / functions / defines and match the parsed Resource [given to Prepare()]. 
	 * Any hits will be accumulated in Matches vector.
	 * 
	 */
	void CollectNearMatchNonMembers();
	
	/**
	 * Collects all resources that are class methods / properties and match the parsed Resource [given to Prepare()]. 
	 * Any hits will be accumulated in Matches vector.
	 * 
	 */
	void CollectNearMatchMembers();
	
	/**
	 * Collects all resources that are namespaces and match the parsed Resource [given to Prepare()]
	 * Any hits will be accumulated in Matches vector.
	 */
	void CollectNearMatchNamespaces();
	
	/**
	 * Collect all of the resources that are methods / properties of the given classes.
	 */
	void CollectAllMembers(const std::vector<UnicodeString>& classNames);
	
	/**
	 * collect all of the resources from all of the traits used by the given class
	 */
	void CollectAllTraitMembers(const UnicodeString& className);
	
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
	void EnsureMatchesExist();

	/**
	 * Returns the full path to the given resource
	 * 
	 * @param resource a resource returned by one of the CollectXXX() methods OF THIS OBJECT.
	 * @return wxString returns empty string on invalid resource
	 */
	wxString GetResourceMatchFullPathFromResource(const ResourceClass& resource) const;

	/**
	 * create a new entry in the file cache
	 * @return the file item index of the new entry
	 */
	int PushIntoFileCache(const wxString& fullPath, bool isParsed, bool isNew);

	/**
	 * find the FileItem entry that has the given FullPath.
	 * @param fullPath the full path to search for
	 * @param fileItemIndex the index of the found item will be set here (if found)
	 * @param fileItem the FileItem itself will be copied here (if found)
	 * @return bool if TRUE it means that this ResourceFinder has encountered the given
	 * file before.
	 */
	bool FindInFileCache(const wxString& fullPath, int& fileItemIndex, FileItem& fileItem) const;

	/**
	 * Read all resources from the given tag file. After a call to this method,
	 * all resources from fileName will be added the the internal resource 
	 * cache.
	 *
	 * @param fileName full path to the tag file. fileName is a file that is
	 *        in ctags format.
	 * @param bool TRUE if the given file is the tags file for the built-in PHP functions.
	 * @return bool TRUE if file exists and ALL of the containing tags are valid.
	 */
	bool LoadTagFile(const wxFileName& fileName, bool isNativeTags);
	
	/**
	 * Checks to see if the given method resource is inherited by the given class. Checking is 
	 * done by looking at the trait use, trait alias, and trait insteadof statements.
	 * 
	 * @param memberResource the member resource to check
	 * @param fullyQualifiedClassName fully qualified class name of class to check in.  This class 
	 *        and any trait classes that this class uses, will be checked.
	 * @return bool TRUE if memberResource belongs to className or any traits that 
	 *         className uses.
	 */
	bool IsTraitInherited(const ResourceClass& memberResource, const UnicodeString& fullyQualifiedClassName);
};

/**
 * This class represents each resource we have found in the project.  
 */
class ResourceClass {

public:

	/**
	 * All the resources we collect
	 */
	enum Type {
		CLASS,
		METHOD,
		FUNCTION,
		MEMBER,
		DEFINE,
		CLASS_CONSTANT,
		NAMESPACE
	};
	
	/**
	 * The resource fully qualified name; members will have a class name with it: ie. User::Name
	 * @var UnicodeString
	 */
	UnicodeString Resource;
	
	/**
	 * The identifer name of this resource. Members will not have a class name with it; ie. a Name method's Identifier will be Name
	 * @var UnicodeString
	 */
	UnicodeString Identifier;
	
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
	ResourceClass::Type Type;

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
	 * @return the full path where this resource is located.
	 */
	wxString GetFullPath() const;

	/**
	 * set all properties to empty string
	 */
	void Clear();

private:

	/**
	 * This will only be filled after a match occurs; in the cache we will just 
	 * hold the file item index.
	 */
	wxString FullPath;

	/**
	 * The index to the file where this resource was found. 
	 */
	int FileItemIndex;
	
	/**
	 * The resource finder class will populate FileItemIndex and FullPath
	 */
	friend class ResourceFinderClass;
};

class TraitResourceClass {
	
public:

	/** 
	 * the fully qualified name of the trait being used
	 */
	UnicodeString TraitClassName;
	
	/**
	 * The names of any aliases
	 */
	std::vector<UnicodeString> Aliased;
	
	/**
	 * the names of any class names excluded from being used by the
	 * 'insteadof' operator
	 */
	std::vector<UnicodeString> Excluded;
	
	TraitResourceClass();
};

}
#endif // __MVCEDITORRESOURCEFINDER_H__
