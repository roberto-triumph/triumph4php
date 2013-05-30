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
#ifndef __MVCEDITOR_TAGPARSERCLASS_H__
#define __MVCEDITOR_TAGPARSERCLASS_H__

#include <search/DirectorySearchClass.h>
#include <globals/TagClass.h>
#include <pelet/ParserClass.h>
#include <wx/datetime.h>
#include <wx/string.h>
#include <wx/filename.h>
#include <soci/soci.h>
#include <unicode/unistr.h>
#include <vector>
#include <map>

namespace mvceditor {

/**
 * The TagParser is used to store parsed tags(classes, functions, methods, properties) into a
 * SQLite database. This class is used in conjunction with the DirectorySearchClass; the 
 * DirectorySearchClass is used to iterate files; while this class takes each file from the
 * DirectorySearchClass and parses the source code for tags.
 *
 * The TagParserClass can only handle PHP source code files; it uses the pelet library to
 * do the actual PHP parsing.
 * 
 * The TagParser has an exception-free API, no exceptions will be ever thrown, even though
 * it uses SOCI to execute queries (and SOCI uses exceptions). Instead
 * the return values for methods of this class will be false, empty, or zero. Currently this class does not expose 
 * the specific error code from SQLite.
 */
class TagParserClass : public pelet::ClassObserverClass, 
	public pelet::ClassMemberObserverClass, 
	public pelet::FunctionObserverClass, 
	public mvceditor::DirectoryWalkerClass {

public:

	/**
	 * The files to be parsed; these are php source code file 
	 * extensions
	 * 
	 * @var vector<wxString> a lst of file filters 
	 * Each item in the array will be one wildcard expression; where each
	 * expression can contain either a '*' or a '?' for use in the
	 * wxMatchWild() function.
	 */
	std::vector<wxString> PhpFileExtensions;

	/**
	 * The files to be recorded but not parsed; these are YML files, text
	 * files, any other file extensions that we want to keep track of.
	 * 
	 * @var vector<wxString> a lst of file filters 
	 * Each item in the array will be one wildcard expression; where each
	 * expression can contain either a '*' or a '?' for use in the
	 * wxMatchWild() function.
	 */
	std::vector<wxString> MiscFileExtensions;
	
	TagParserClass();
	~TagParserClass();

	/**
	 * Create the tag database that is backed by the given session. 
	 * This method can used to have the tag parser write to either a SQLite hard disk file
	 * or a SQLite in-memory database.
	 * By using an in-memory database, lookups are faster. This method would be used
	 * when parsing resources for a single file only. Also note that if a DB 
	 * file / memory DB was previously open, this method will close the existing db before the new db is opened.
	 * 
	 * Note that this method assumes that the schema has already been created (unlike
	 * the InitFile() method)
	 * 
	 * @param soci::session* the session. this class will NOT own the pointer
	 * @param fileParsingBufferSize the size of an internal buffer where parsed resources are initially 
	 *        stored. This is only a hint, the buffer will grow as necessary
	 *        Setting this value to a high value (1024) is good for large projects that have a lot
	 *        resources.
	 */
	void Init(soci::session* session, int fileParsingBufferSize = 32);
	
	/**
	 * closes any opened connection / transaction
	 */
	void Close();

	/**
	 * Implement the DirectoryWalkerClass method; will start a transaction
	 */
	void BeginSearch();
		
	/**
	 * Parses the given file for resources. Note that one of the Init() method
	 * must have been called before a call to this method is made. Also, BeginSearch() must 
	 * have been called already (if using a DirectorySearchClass, the DirectorySearchClass
	 * will take care of calling that method before this method gets called).
	 *
	 *  @param wxString  fileName the full path to the file to be parsed
	 */
	virtual bool Walk(const wxString& fileName);

	/**
	 * Implement the DirectoryWalkerClass method; will commit a transaction. If 
	 * using a DirectorySearchClass, the DirectorySearchClass
	 * will take care of calling that method before after all files have been recursed.
	 */
	void EndSearch();
	
	/**
	 * Parses the given string for resources.  This method would be used, for example, when wanting
	 * to be able to find resources from a file currently being edited by a user but the user
	 * has not yet saved the file so the new contents are not yet on disk.
	 * Note that one of the Init method
	 * must have been called before a call to this method is made.
	 * 
	 * @param const wxString&
	 * fileName the full path of the file
	 * @param const UnicodeString& code the PHP source code
	 * @param bool if TRUE then tileName is a new file that is not yet written to disk
	 */
	void BuildResourceCacheForFile(const wxString& fileName, const UnicodeString& code, bool isNew);
		
	/**
	 * Implement class observer.  When a class has been parsed, add it to the Resource Cache.
	 */
	void ClassFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& signature, 
		const UnicodeString& comment, const int lineNumber);

	/**
	 * When a define has been found, add it to the tag cache
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
	 * Print the tag cache to stdout.  Useful for debugging only.
	 */
	void Print();

	/**
	 * Removes ALL tags from this tag finder entirely. If this
	 * finder was initialized with a backing file, the backing database
	 * file will be truncated also.
	 */
	void WipeAll();


	/**
	 * Deletes tags from the given directories only. Tags from subdirectories will
	 * also be deleted. If this finder was initialized with a backing file, the backing database
	 * file will be deleted also.
	 */
	void DeleteDirectories(const std::vector<wxFileName>& dirs);

	/**
	 * Deletes tags from a single file. If this finder was initialized with a backing file, the backing database
	 * file will be deleted also.
	 */
	void DeleteFromFile(const wxString& fullPath);
	
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
	std::vector<TagClass> FileParsingCache;

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
	std::map<UnicodeString, std::vector<TraitTagClass>, UnicodeStringComparatorClass> TraitCache;
	
	/**
	 * Used to parse through code for classes & methods
	 * 
	 * @var pelet::ParserClass
	 */
	pelet::ParserClass Parser;

	/**
	 * The connection to the database that backs the tag cache
	 * The database will hold all of the files that have been looked at, as well
	 * as all of the resources that were parsed.
	 * This class will NOT own the pointer.
	 */
	soci::session* Session;

	/*
	 * Will use transaction to lock once instead of before and after every insert
	 * This class will own the pointer.
	 */
	soci::transaction* Transaction;
		
	/**
	 * The current file item being indexed.  We keep a class-wide member when parsing through many files.
	 * 
	 * @var int fileTagId the dataabse ID of the FileTag entry that corresponds to the file located at fullPath
	 */
	int CurrentFileTagId;

	/**
	 * The initial size of FileParsingCache. This is only a hint, the buffer will grow as necessary
	 * Setting this value to a high value (1024) is good for large projects that have a lot
	 * resources.
	 */
	int FileParsingBufferSize;

	/**
	 * count the number of files that have been parsed so that we commit to sqlite at regular
	 * intervals. 
	 */
	int FilesParsed;

	/**
	 * Flag to make sure we initialize the tag database.
	 */
	bool IsCacheInitialized;
	
	/**
	 * Goes through the given file and parses out resources.
	 * 
	 * @param wxString fullPath full path to the file to look at
	 * @param bool parseClasses if TRUE, file will be opened and TagCache will be populated.  Otherwise, only FileCache
	 *        will be populated.
	 */
	void BuildResourceCache(const wxString& fullPath, bool parseClasses);
	
	/**
	 * remove all resources for the given file_item_ids.
	 * 
	 * @param fileTagids the list of file_item_id that will be deleted from the SQLite database.
	 */
	void RemovePersistedResources(const std::vector<int>& fileTagIds);
	
	/**
	 * Write the file item into the database. The item's FileId member will be set as well.
	 */
	void PersistFileTag(mvceditor::FileTagClass& fileTag);

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
	 * add all of the given resources into the database.
	 * @param resources the list of resources that were parsed out
	 * @param int the file that the resources are located in
	 */
	void PersistResources(const std::vector<mvceditor::TagClass>& resources, int fileTagId);

	/**
	 * add all of the given trait resources into the database.
	 * @param traits the map of resources that were parsed out
	 * @param int the file that the resources are located in
	 */
	void PersistTraits(
		const std::map<UnicodeString, std::vector<mvceditor::TraitTagClass>, UnicodeStringComparatorClass>& traitMap,
		int fileTagId);

	/**
	 * @return all of the traits that any of the given classes use.
	 */
	std::vector<mvceditor::TraitTagClass> FindTraitsByClassName(const std::vector<std::string>& keyStarts);

	/**
	 * check the database AND the current file's parsed cache to see if the namespace has been seen
	 * before.
	 * @return bool TRUE if the namespace is NOT in the database and its NOT in the current file
	 *  parsed cache
	 */
	bool IsNewNamespace(const UnicodeString& namespaceName);
};

}
#endif 
