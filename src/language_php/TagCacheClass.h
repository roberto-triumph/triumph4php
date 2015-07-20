/**
 * @copyright  2009-2011 Roberto Perpuly
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
#ifndef T4P_RESOURCECACHECLASS_H
#define T4P_RESOURCECACHECLASS_H

#include <search/DirectorySearchClass.h>
#include <language_php/TagParserClass.h>
#include <language_php/SymbolTableClass.h>
#include <globals/Sqlite.h>
#include <globals/SqliteResultClass.h>
#include <unicode/unistr.h>
#include <map>
#include <wx/thread.h>
#include <wx/event.h>

namespace t4p {
// forward declarations
class DetectedTagNearMatchMemberResultClass;
class DetectedTagExactMemberResultClass;
class TagFinderListClass;
class TagFinderClass;
class TagResultClass;
class FileTagResultClass;
class ParsedTagFinderClass;
class GlobalsClass;

/**
 * The working cache is an in-memory cache of source code that is being edited
 * by the user. It will be treated separately because it changes very often, and
 * we don't want to disturb the global cache (which is much bigger).
 */
class WorkingCacheClass {
	public:
	/**
	 * The object that keeps track of variable types; we only need
	 * to keep track of variable types for opened files.
	 */
	t4p::SymbolTableClass SymbolTable;

	/**
	 * The full path to the file being parsed. This may be the empty string
	 * if the file resides completely in memory
	 */
	wxString FileName;

	/**
	 * A unique, *non-empty* identifier string.
	 */
	wxString FileIdentifier;

	/**
	 * TRUE if the user is editing a new file
	 */
	bool IsNew;

	WorkingCacheClass();

	/**
	 * This method accepts the previously created symbol table, so that in case the given code
	 * has a syntax error, we can use the previously built table's variables; that way code completion
	 * can work even on files that contain syntax errors.
	 * In the case the give code has a syntax error this symbol table is built by using the
	 * previous table's variables, then tokenizing the given code to add any additional
	 * variables not found in the previous table.
	 *
	 * @param fileName the full path to the file being parsed
	 * @param fileIdentifier  A unique, *non-empty* identifier string.
	 * @param isNew TRUE if the user is editing a new file
	 * @param version the PHP version that the parser will check against
	 * @param createSymbols if TRUE then the symbol table is built from the file
	 *        if this is true then fileName must be a valid full path
	 * @param previousSymbolTable used when code has a syntax error.
	 */
	void Init(const wxString& fileName, const wxString& fileIdentifier, bool isNew, pelet::Versions version,
		bool createSymbols, const t4p::SymbolTableClass& previousSymbolTable);

	/**
	 * Will parse the resources and determine type information for the given text
	 * and store the results into the this cache.
	 *
	 * This method accepts the previously created symbol table, so that in case the given code
	 * has a syntax error, we can use the previously built table's variables; that way code completion
	 * can work even on files that contain syntax errors.
	 * In the case the give code has a syntax error this symbol table is built by using the
	 * previous table's variables, then tokenizing the given code to add any additional
	 * variables not found in the previous table.
	 *
	 * @param code the file's most up-to-date source code (from the user-edited buffer)
	 * @param previousSymbolTable used when code has a syntax error.
	 * @return bool TRUE if code did not contain a syntax error
	 */
	bool Update(const UnicodeString& code, const t4p::SymbolTableClass& previousSymbolTable);

	private:
};

/**
 * This class represents all the most up-to-date resources of the currently opened project
 * in Triumph.  It consists of a series of "global" caches together with a
 * separate cache for all files that are being edited.  The reason for putting opened files
 * in their own cache is for speed and accuracy; while a file is being edited (but the new
 * contents have not yet been flushed to disk) we can parse the code control content for resources without
 * disturbing the global-wide cache; lookups are faster because the cache won't need to re-sort
 * the entire global list of resources since the resources from files that are not openeed are not affected.
 * Lookups are also accurate because we can parse contents that have not yet been parsed, allowing code
 * completion to be helpful on code not yet saved.
 *
 * Since TagCacheClass is backed by ParsedTagFinderClass, the global tag finders
 * are stored on disk and are preserved across application restarts.
 *
 * Note that NONE of the methods of this class can be safely accessed by multiple
 * threads. Concurrency is achieved by creating separate instances of
 * TagCacheClass with both pointing to the same DB files.
 */
class TagCacheClass {
	public:
	TagCacheClass();

	~TagCacheClass();

	/**
	 * Creates a new tag finder for the given file. This should be called
	 * when the user opens a file.
	 *
	 * @param fileName unique identifier for a file
	 * @param cache the working cache. Must have been initialized.
	 *         This object will own the pointer and will delete
	 *         when the file is unregistered or when this object is destroyed. If this method
	 *         returns FALSE, then this object will NOT take ownership of the pointer and
	 *         the caller must delete it.
	 * @return bool TRUE if fileName was not previously registered
	 * only a unique fileName can be registered
	 */
	bool RegisterWorking(const wxString& fileName, t4p::WorkingCacheClass* cache);

	/**
	 * Updates the working cache for a given file. Cache should be updated often
	 * so that the ExpressionCompletionMatches and ResourceMatches will return up-to-date info.
	 * This method assumes that the cache is ready, ie. Update() method has been called
	 * on it.
	 *
	 * @param fileName the file to update. This is the name given to the RegisterWorking() method
	 * @param cache the new cache for the file. In this method, this object will ALWAYS take
	 *        ownership of the cache, unlike RegisterWorking() method
	 */
	bool ReplaceWorking(const wxString& fileName, t4p::WorkingCacheClass* cache);

	/**
	 * Cleans up the tag finder from the given file. This should be called whenever
	 * the user is no longer editing the file.
	 *
	 * @param fileName unique identifier for a file
	 */
	void RemoveWorking(const wxString& fileName);

	/**
	 * Get the working cache for a file. This should only be used as a last-resort;
	 * it is preferred to use the other accessor methods instead ie. ExactTags, et al.
	 *
	 * @param fileName the file to update. This is the name given to the RegisterWorking() method
	 * @return cache of the file; NULL if the cache for fileName does not exist. This class
	 *         retains ownership of the pointer; the returned pointer must NOT be deleted.
	 */
	t4p::WorkingCacheClass* GetWorking(const wxString& fileName);

	/**
	 * Set the global cache. After a call
	 * to this method, the cache is available for use by
	 * the ExpressionCompletionMatches and ResourceMatches methods
	 *
	 * @param tagFinderList this class will own the pointer
	 */
	void RegisterGlobal(t4p::TagFinderListClass* tagFinderList);

	/**
	 * Set the global cache using the default settings (from Asset). After a call
	 * to this method, the cache is available for use by
	 * the ExpressionCompletionMatches and ResourceMatches methods
	 * This method clones data structures where necessary, so that this
	 * TagCache can be used from a separate thread than where globals
	 * resides
	 */
	void RegisterDefault(t4p::GlobalsClass& globals);

	/**
	 * Searches the parsed tag finder
	 * Will return only for full exact matches (it will call ExactTags).
	 * @param search string to search for
	 * @param sourceDirs directories to restrict matching tags in. If empty, then the entire cache will be searched.
	 * @return TagResultClass to iterate through the results of the query. The
	 *          returned pointer must be deleted by the caller.
	 */
	t4p::TagResultClass* ExactTags(const UnicodeString& search, const std::vector<wxFileName>& searchDirs);

	/**
	 * Searches the native tag finder
	 * Will return only for full exact matches (it will call ExactTags).
	 * @param search string to search for
	 * @return TagResultClass to iterate through the results of the query. The
	 *          returned pointer must be deleted by the caller.
	 */
	t4p::TagResultClass* ExactNativeTags(const UnicodeString& search);

	/**
	 * Searches the detected tag finder
	 * Will return only for full exact matches (it will call ExactTags).
	 * @param search string to search for
	 * @param sourceDirs directories to restrict matching tags in. If empty, then the entire cache will be searched.
	 * @return TagResultClass to iterate through the results of the query. The
	 *          returned pointer must be deleted by the caller.
	 */
	t4p::DetectedTagExactMemberResultClass* ExactDetectedTags(const UnicodeString& search, const std::vector<wxFileName>& searchDirs);

	/**
	 * Searches the tag cache using near-match logic
	 *
	 * @param string to search for
	 * @param sourceDirs source directories to restrict matching tags in. If empty, then the entire cache will be searched.
	 * @return TagResultClass to iterate through the results of the query. The
	 *          returned pointer must be deleted by the caller.
	 */
	t4p::TagResultClass* NearMatchTags(const UnicodeString& search, const std::vector<wxFileName>& sourceDirs);

	/**
	 * Searches the detected tag cache using near-match logic
	 *
	 * @param string to search for
	 * @param sourceDirs source directories to restrict matching tags in. If empty, then the entire cache will be searched.
	 * @return TagResultClass to iterate through the results of the query. The
	 *          returned pointer must be deleted by the caller.
	 */
	t4p::DetectedTagNearMatchMemberResultClass* NearMatchDetectedTags(const UnicodeString& search, const std::vector<wxFileName>& sourceDirs);

	/**
	 * Searches the native tag cache using near-match logic
	 *
	 * @param string to search for
	 * @return TagResultClass to iterate through the results of the query. The
	 *          returned pointer must be deleted by the caller.
	 */
	t4p::TagResultClass* NearMatchNativeTags(const UnicodeString& search);

	/**
	 * Searches the tag cache for filenames using exact-match logic on file names or full paths
	 *
	 * @param string part of filename to search for
	 * @param sourceDirs source directories to restrict matching tags in. If empty, then the entire cache will be searched.
	 * @return FiileTagResultClass to iterate through the results of the query. The
	 *          returned pointer must be deleted by the caller.
	 */
	t4p::FileTagResultClass* ExactFileTags(const UnicodeString& search, const std::vector<wxFileName>& sourceDirs);

	/**
	 * Searches the tag cache for filenames using near-match logic
	 *
	 * @param string part of filename to search for
	 * @param sourceDirs source directories to restrict matching tags in. If empty, then the entire cache will be searched.
	 * @return FiileTagResultClass to iterate through the results of the query. The
	 *          returned pointer must be deleted by the caller.
	 */
	t4p::FileTagResultClass* NearMatchFileTags(const UnicodeString& search, const std::vector<wxFileName>& sourceDirs);

	/**
	 * Searches all the registered caches (working AND global caches)
	 * Will return only for full exact matches (it will call ExactClassOrFile
	 * on each tag finder).
	 * @param search string to search for
	 * @return std::vector<t4p::PhpTagClass> matched resources. will be either files or classes
	 */
	std::vector<t4p::PhpTagClass> ExactClassOrFile(const UnicodeString& search);

	/**
	 * Searches all the registered caches (working AND global caches)
	 * Will return only for full exact matches (it will call ExactClass
	 * on each tag finder).
	 * @param search string to search for
	 * @return std::vector<t4p::PhpTagClass> matched resources. will be class tags only
	 */
	std::vector<t4p::PhpTagClass> ExactClass(const UnicodeString& search);

	/**
	 * Searches all the registered caches (working AND global caches)
	 * Will return only for full exact matches (it will call ExactFunction
	 * on each tag finder).
	 * @param search string to search for
	 * @return std::vector<t4p::PhpTagClass> matched resources. will be function tags only
	 */
	std::vector<t4p::PhpTagClass> ExactFunction(const UnicodeString& search);

	/**
	 * Searches all the registered caches (working AND global caches)
	 * Will return only for full exact matches on methods (it will call ExactMethod
	 * on each tag finder). Note that matching methods from all classes are returned
	 * @param search string to search for
	 * @param onlyStatic if TRUE, then only static methods will be returned
	 * @return std::vector<t4p::PhpTagClass> matched resources. will be method tags only
	 */
	std::vector<t4p::PhpTagClass> ExactMethod(const UnicodeString& search, bool onlyStatic);

	/**
	 * Searches all the registered caches (working AND global caches)
	 * Will return only for full exact matches on properties or constants (it will call ExactProperty
	 * on each tag finder). Note that matching properties/constants from all classes are returned.
	 * @param search string to search for
	 * @param onlyStatic if TRUE, then only static properties and constants will be returned
	 * @return std::vector<t4p::PhpTagClass> matched resources. will be function tags only
	 */
	std::vector<t4p::PhpTagClass> ExactProperty(const UnicodeString& search, bool onlyStatic);

	/**
	 * Searches all the registered caches (working AND global caches)
	 * Will return near matches (it will call NearMatchClassesOrFiles
	 * on each tag finder).
	 *
	 * @param string to search for
	 * @return std::vector<t4p::PhpTagClass> matched resources. will be either files or classes
	 */
	std::vector<t4p::PhpTagClass> NearMatchClassesOrFiles(const UnicodeString& search);

	/**
	 * prepares the given result against the global tag cache
	 *
	 * @param result the statement to prepare
	 * @param doLimit if TRUE the query will have a limit added to it
	 * @return bool TRUE if the result was successfully prepared
	 */
	bool GlobalPrepare(t4p::SqliteResultClass& result, bool doLimit);

	/**
	 * prepares the given result against the native tag cache
	 *
	 * @param result the statement to execute
	 * @param doLimit if TRUE the query will have a limit added to it
	 * @return bool TRUE if the result was successfully prepared
	 */
	bool NativePrepare(t4p::SqliteResultClass& result, bool doLimit);

	/**
	 * gets all tags for a single class
	 * @param fullyQualifiedClassName the class to search.  fully qualified (with namespace)
	 * @param fileTagId the file in which the class is found in. this is to filter out
	 *        classes if the same class name is found in multiple files
	 * @param sourceDirs the source directories to look in
	 * @return vector of tags;
	 *         all methods and properties that are defined in the class PLUS
	 *         all methods and properties that are defined in any of its base classes PLUS
	 *         all methods and properties that are defined in any of the traits used by any of the base classes
	 */
	std::vector<t4p::PhpTagClass> AllMemberTags(const UnicodeString& fullyQualifiedClassName, int fileTagId, std::vector<wxFileName>& sourceDirs);

	/**
	 * gets all tags that were found in a single file. for classes, all of the class' members (including
	 * inherited members) are returned as well.
	 *
	 * @param fullPath full path to a file to lookup
	 * @return vector of tags;
	 *         all classes, functions or define's in the give file
	 */
	std::vector<t4p::PhpTagClass> AllClassesFunctionsDefines(const wxString& fullPath);

	/**
	 * Collects all near matches that are possible candidates for completion of the parsed variable.
	 * Basically just a calls the ExpressionCompletionResourceMatches() of the given file's SymbolTable. See that method for more info
	 *
	 * @param fileName the symbol table of this registered file will be searched
	 * @param parsedVariable the parsed variable to be completed; from ParserClass::ParseExpression()
	 * @param variableScope the scope where parsed variable is located.  The scope let's us know which variables are
	 *        available. See ScopeFinderClass for more info.
	 * @param sourceDirs the list of enabled source directories, only tags whose source_id matches source directories will be returned
	 * @param autoCompleteVariableList the results of the matches; these are the names of the variables that
	 *        are "near matches" to the parsed expression. This will be filled only when parsedExpression is a variable.
	 * @param autoCompleteResourceList the results of the matches; these are the names of the items that
	 *        are "near matches" to the parsed expression. This will be filled only when parsedExpression is a variable "chain" or
	 *        a function / static class call.
	 * @param doDuckTyping if an expression chain could not be fully resolved; then we could still
	 *        perform a search for the expression member in ALL classes. The lookups will not be
	 *        slower because ParsedTagFinderClass still handles them
	 * @param error any errors / explanations will be populated here. error must be set to no error (initial state of object; or use Clear())
	 */
	void ExpressionCompletionMatches(const wxString& fileName,
		const pelet::VariableClass& parsedVariable,
		const pelet::ScopeClass& variableScope,
		const std::vector<wxFileName>& sourceDirs,
		std::vector<UnicodeString>& autoCompleteList,
		std::vector<t4p::PhpTagClass>& autoCompleteResourceList,
		bool doDuckTyping,
		SymbolTableMatchErrorClass& error);

	/**
	 * This method will resolve the given parsed variable and will figure out the type of a tag.
	 * Basically just a calls the ResourceMatches() of the given file's SymbolTable. See that method for more info
	 *
	 * @param fileName the symbol table of this registered file will be searched
	 * @param parsedVariable the parsed variable to lookup; from ParserClass::ParseExpression()
	 * @param variableScope the scope where parsed variable is located.  The scope let's us know which variables are
	 *        available. See ScopeFinderClass for more info.
	 * @param sourceDirs the list of enabled source directories, only tags whose source_id matches source directories will be returned
	 * @param matches all of the tag matches will be put here
	 * @param doDuckTyping if an expression chain could not be fully resolved; then we could still
	 *        perform a search for the expression member in ALL classes. The lookups will not be
	 *        slower because ParsedTagFinderClass still handles them
	 * @param doFullyQualifiedMatchOnly if TRUE the only resources that match fully qualified resources will be
	 *        returned
	 * @param error any errors / explanations will be populated here. error must be set to no error (initial state of object; or use Clear())
	 */
	void ResourceMatches(const wxString& fileName,
		const pelet::VariableClass& parsedVariable,
		const pelet::ScopeClass& variableScope,
		const std::vector<wxFileName>& sourceDirs,
		std::vector<t4p::PhpTagClass>& matches,
		bool doDuckTyping, bool doFullyQualifiedMatchOnly,
		SymbolTableMatchErrorClass& error);

	/**
	 * Returns the tags that matched the identifier in the given position
	 *
	 * @param fileName the symbol table of this registered file will be searched
	 * @param code the code of the document being checked
	 * @param int the character position in the document to check
	 * @param sourceDirs the list of enabled source directories, only tags whose source_id matches source directories will be returned
	 * @param globals to get the PHP version (to parse the code)
	 * @param [out] status if there is an error querying for tags, error will be set. this is a human-friendly
	 *        error message
	 * @return tag matches
	 */
	std::vector<t4p::PhpTagClass> GetTagsAtPosition(
		const wxString& fileName,
		const UnicodeString& code, int posToCheck,
		const std::vector<wxFileName>& sourceDirs, t4p::GlobalsClass& globals,
		wxString& status);

	/**
	 * print the tag cache to stdout. Really only useful for debugging and not much else
	 */
	void Print();

	/**
	 * @return TRUE if the cache has not been initialized with either a call to RegisterWorking(), RegisterGlobal() or a call to WalkGlobal()
	 */
	bool IsFileCacheEmpty();

	/**
	 * @return TRUE if the cache has not been initialized with either a call to RegisterWorking(), RegisterGlobal() or a call to WalkGlobal()
	 */
	bool IsResourceCacheEmpty();

	/**
	 * Remove all items from all caches and also unregisters any and all files.
	 */
	void Clear();

	/**
	 * @param className the PHP class to check
	 * @return list of class names that are base classes for the given class; plus traits
	 *         used by the given class or any of its base classes
	 */
	std::vector<UnicodeString> ParentClassesAndTraits(const UnicodeString& className, const std::vector<wxFileName>& sourceDirs);

	/**
	 * retrieves a tag by its ID
	 *
	 * @param id the ID to query for
	 * @param tag out parameter, will be filled in with the tag data
	 * @return bool TRUE if the ID was found
	 */
	bool FindById(int id, t4p::PhpTagClass& tag);

	/**
	 * @param fullPath filename to delete tags that were found in filename.
	 */
	void DeleteFromFile(const wxString& fullPath);

	/**
	 * check to see if a file was cached
	 *
	 * @param fullPath the path to check. check is done case-insensitive
	 * @return bool TRUE if the file was seen by the tag parser
	 */
	bool HasFullPath(const wxString& fullPath);

	/**
	 * check to see if a directory was cached
	 *
	 * @param dir the path to check. check is done case-insensitive. note that dir is not a source directory,
	 *        it could be a subdirectory of a source directory
	 * @return bool TRUE if the directory was seen by the tag parser
	 */
	bool HasDir(const wxString& dir);

	private:
	/**
	 * Returns a list that contains all of the tag finders for the registered files plus
	 * the global tag finders.
	 *
	 * This clas owns the tag finder pointers, do NOT delete them
	 */
	std::vector<t4p::ParsedTagFinderClass*> AllFinders();

	/**
	 * These are the tag finders from the ALL projects and native functions; it may include stale resources
	 * This class will own the pointer and will delete them when appropriate.
	 */
	t4p::TagFinderListClass* TagFinderList;

	/**
	 * To calculate variable type information
	 * This class will own these pointers and will delete them when appropriate.
	 */
	std::map<wxString, t4p::WorkingCacheClass*> WorkingCaches;
};

/**
 * Event that will hold the results of a tag finder
 * parsing an entire directory.
 */
extern const wxEventType EVENT_WORKING_CACHE_COMPLETE;

/**
 * Event that will hold the results of tag finder & symbol table
 * on a single file.
 */
extern const wxEventType EVENT_TAG_FINDER_LIST_COMPLETE;


class WorkingCacheCompleteEventClass : public wxEvent {
	public:
	/**
	 * This will be owned by the event handler
	 */
	t4p::WorkingCacheClass* WorkingCache;

	/**
	 * @param evetId the event ID
	 * @param fileName full path to the file that was parsed. note that this may
	 *        not be unique; it can be empty if a "new" code control is being
	 *        parsed
	 * @param fileIdentifier a unique string
	 * @param workingCache the parsed tag data for the file.
	 *        this pointer will be owned by the event handler.
	 */
	WorkingCacheCompleteEventClass(int eventId,
		const wxString& fileName, const wxString& fileIdentifier, t4p::WorkingCacheClass* workingCache);

	wxEvent* Clone() const;

	/**
	 * @return the file identifier given in the constructor
	 */
	wxString GetFileIdentifier() const;

	/**
	 * @return the file name given in the constructor
	 */
	wxString GetFileName() const;

	private:
	wxString FileName;

	wxString FileIdentifier;
};

class TagFinderListCompleteEventClass : public wxEvent {
	public:
	TagFinderListCompleteEventClass(int id);

	wxEvent* Clone() const;
};
}

typedef void (wxEvtHandler::*WorkingCacheCompleteEventClassFunction)(t4p::WorkingCacheCompleteEventClass&);

#define EVT_WORKING_CACHE_COMPLETE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_WORKING_CACHE_COMPLETE, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent(WorkingCacheCompleteEventClassFunction, & fn), (wxObject *) NULL),

typedef void (wxEvtHandler::*TagFinderListCompleteEventClassFunction)(t4p::TagFinderListCompleteEventClass&);

#define EVT_TAG_FINDER_LIST_COMPLETE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_TAG_FINDER_LIST_COMPLETE, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent(TagFinderListCompleteEventClassFunction, & fn), (wxObject *) NULL),

typedef void (wxEvtHandler::*TagFinderListCompleteEventClassFunction)(t4p::TagFinderListCompleteEventClass&);

#define EVT_TAG_FINDER_LIST_COMPLETE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_TAG_FINDER_LIST_COMPLETE, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent(TagFinderListCompleteEventClassFunction, & fn), (wxObject *) NULL),

#endif
