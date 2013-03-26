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
#ifndef __MVCEDITORRESOURCECACHECLASS_H__
#define __MVCEDITORRESOURCECACHECLASS_H__

#include <search/TagFinderClass.h>
#include <search/DirectorySearchClass.h>
#include <language/TagParserClass.h>
#include <language/SymbolTableClass.h>
#include <unicode/unistr.h>
#include <map>
#include <wx/thread.h>
#include <wx/event.h>
 
namespace mvceditor {

/**
 * A global cache is a cache of an entire project.  All of a project's resources
 * are stored in a SQLite file that is loaded when MVC Editor starts; this way
 * the user can jump to files & classes without needing to re-index the 
 * entire project.
 */
class GlobalCacheClass {

private:

	/**
	 * The connection to the tagcache sqlite db file
	 * This needs to be declared first because the
	 * tagparser uses it; and we want it to be cleaned up last
	 */
	soci::session Session;

public:

	/**
	 * The object that will parse and persist tags
	 */
	mvceditor::TagParserClass TagParser;

	/**
	 * The object that will be used to lookup tags
	 * This class will own this pointer
	 */
	mvceditor::TagFinderClass* TagFinder;

	/**
	 * The location of the SQLite file where the resources will be
	 * stored.
	 */
	wxFileName ResourceDbFileName;

	GlobalCacheClass();

	~GlobalCacheClass();

	/**
	 * Opens the SQLite resource db file, or creates it if it does not exist.
	 *
	 * @param resourceDbFileName the full path to the SQLite resources database.
	 *        If this full path does not exist it will be created.
	 * @param phpFileExtensions the wildcards that hold which files to parse
	 * @param miscFileExtensions the wildcards that hold which files to to record but not parse
	 * @param version the PHP version that the parser will check against
	 * @param fileParsingBufferSize the size of an internal buffer where parsed resources are initially 
	 *        stored. This is only a hint, the buffer will grow as necessary
	 *        Setting this value to a high value (1024) is good for large projects that have a lot
	 *        resources.
	*/
	void InitGlobalTag(const wxFileName& tagDbFileName, const std::vector<wxString>& phpFileExtensions, 
		const std::vector<wxString>& miscFileExtensions, pelet::Versions version, int fileParsingBufferSize = 32);

	/**
	 * Opens the detector db SQLite file, or creates it if it does not exist.
	 *
	 * @param detectorDbFileName the full path to the SQLite detectors database.
	 *        If this full path does not exist it will be created.
	*/
	void InitDetectorTag(const wxFileName& detectorDbFileName);

	/**
	 * Will update the tag finder by calling Walk(); meaning that the next file
	 * given by the directorySearch will be parsed and its resources will be stored
	 * in the database.
	 *
	 * @see mvceditor::TagFinderClass::Walk
	 * @param resourceDbFileName the location of the SQLite cache for the tag finder. this 
	 *        is the file where the resources will be persisted to
	 * @param directorySearch keeps track of the file to parse
	 */
	void  Walk(DirectorySearchClass& directorySearch);

	/**
	 * @param version the PHP version that the parser will check against
	 */
	void SetVersion(pelet::Versions version);

private:
	/**
	 * create the database connection to the given db, and create tables to store the parsed resources
	 * If the file does not exist; it will be created and the schema will be initialized as
	 * well.
	 *
	 * @param wxString dbName, given to SQLite.  db can be a full path to a file  The
	 *        file does not need to exist; if it does not exist it will be created.
	 * @param schemaFileName the filename of the SQL script that contains the schema.
	 *        The schema file will be run if the db has to be created.
	 */
	void OpenAndCreateTables(const wxString& dbName, const wxFileName& schemaFileName);
};

/**
 * The working cache is an in-memory cache of source code that is being edited
 * by the user. It will be treated separately because it changes very often, and
 * we don't want to disturb the global cache (which is much bigger).
 */
class WorkingCacheClass {

public:

	/**
	 * The object that will parse and persist tags
	 */
	mvceditor::TagParserClass TagParser;

	/**
	 * The object that will be used to lookup tags
	 */
	mvceditor::ParsedTagFinderClass TagFinder;

	/**
	 * The object that keeps track of variable types; we only need
	 * to keep track of variable types for opened files.
	 */
	mvceditor::SymbolTableClass SymbolTable;

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
	 * @param fileName the full path to the file being parsed
	 * @param fileIdentifier  A unique, *non-empty* identifier string. 
	 * @param isNew TRUE if the user is editing a new file
	 * @param version the PHP version that the parser will check against
	 * @param createSymbols if TRUE then the symbol table is built from the file
	 *        if this is true then fileName must be a valid full path
	 */
	void Init(const wxString& fileName, const wxString& fileIdentifier, bool isNew, pelet::Versions version, bool createSymbols);

	/**
	 * Will parse the resources and determine type information for the given text 
	 * and store the results into the this cache.
	 * 
	 * @param code the file's most up-to-date source code (from the user-edited buffer)
	 * @return bool TRUE if code did not contain a syntax error
	 */
	bool Update(const UnicodeString& code);

private:

	/**
	 * will run code through a lint check and choose not to update the cache if 
	 * the code is not valid.
	 */
	pelet::ParserClass Parser;

	/**
	 * this will hold a memory-based sqlite db for use by this working cache
	 */
	soci::session Session;

	/**
	 * create the database connection to a SQLite memory db will be created and the schema will be initialized as
	 * well.
	 *
	 */
	void OpenAndCreateTables();

};

/**
 * This class represents all the most up-to-date resources of the currently opened project
 * in MVC Editor.  It consists of a series of "global" caches together with a series of 
 * separate caches for each file that is being edited.  The reason for putting opened files
 * in their own cache is for speed and accuracy; while a file is being edited (but the new
 * contents have not yet been flushed to disk) we can parse the code control content for resources without 
 * disturbing the global-wide cache; lookups are faster because the cache won't need to re-sort
 * the entire global list of resources since the resources from files that are not openeed are not affected.
 * Lookups are also accurate because we can parse contents that have not yet been parsed, allowing code 
 * completion to be helpful on code not yet saved.
 *
 * Usage:
 * The tag cache can handle multiple global caches; this can be used to separate the
 * tag cache into multiple files; one file per project. Something like this:
 *
 * @code
 
 * mvceditor::TagCacheClass cache;
 *
 * // setup the parsed resources to be saved into the user's home directory
 * // note that file need not exist
 * wxFileName cache1("/home/user/.mvceditorcache");
 * cache.InitGlobal(cache1);
 *
 * // now parse a project
 * mvceditor::DirectorySearchClass search;
 * search.Init(wxT("/home/user/project1/src"));
 * std::vector<wxString> fileFilters;
 * fileFilters.push_back(wxT("*.php"));
 * while (search.More()) {
 *   cache.WalkGlobal(search, fileFilters);
 * }
 *
 * @endcode
 *
 * Since TagCacheClass is backed by ParsedTagFinderClass, the global tag finders 
 * are stored on disk and are preserved across application restarts. 
 *
 * Note that NONE of the methods of this class can be safely accessed by multiple
 * threads. Concurrent access can be achieved by creating separate instances of 
 * TagCacheClas with both pointing to the same DB files.
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
	bool RegisterWorking(const wxString& fileName, mvceditor::WorkingCacheClass* cache);

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
	bool ReplaceWorking(const wxString& fileName, mvceditor::WorkingCacheClass* cache);
	
	/**
	 * Cleans up the tag finder from the given file. This should be called whenever
	 * the user is no longer editing the file.
	 * 
	 * @param fileName unique identifier for a file
	 */
	void RemoveWorking(const wxString& fileName);

	/**
	 * Push a new cache into the global cache list. After a call
	 * to this method, the cache is available for use by 
	 * the ExpressionCompletionMatches and ResourceMatches methods
	 *
	 * @return bool FALSE if the tag file is already initialized
	 */
	bool RegisterGlobal(mvceditor::GlobalCacheClass* globalCache);

	/**
	 * Removes a db file from the global tag finders.
	 * @param resourceDbFileName the location of the SQLite cache for the 
	 *        tag finder.
	 */
	void RemoveGlobal(const wxFileName& resourceDbFileName);

	/**
	 * check to see if a tag DB file is already loaded
	 *
	 * @return bool TRUE if db file is already loaded.
	 */
	bool IsInitGlobal(const wxFileName& resourceDbFileName) const;
	
	/**
	 * Searches all the registered caches (working AND global caches)
	 * Will return only for full exact matches (it will call ExactTags
	 * on each tag finder).
	 * @see mvceditor::TagFinderClass::ExactTags
	 * @param search string to search for
	 * @return std::vector<mvceditor::TagClass> matched resources 
	 */
	std::vector<mvceditor::TagClass> ExactTags(const UnicodeString& search);
	
	/**
	 * Searches all the registered caches (working AND global caches)
	 * Will return near matches (it will call NearMatchTags
	 * on each tag finder).
	 *
	 * @see mvceditor::TagFinderClass::NearMatchTags
	 * @param string to search for
	 * @return std::vector<mvceditor::TagClass> matched resources
	 */
	std::vector<mvceditor::TagClass> NearMatchTags(const UnicodeString& search);

	/**
	 * Searches all the registered caches (working AND global caches)
	 * Will return only for full exact matches (it will call ExactClassOrFile
	 * on each tag finder).
	 * @see mvceditor::TagFinderClass::ExactClassOrFile
	 * @param search string to search for
	 * @return std::vector<mvceditor::TagClass> matched resources. will be either files or classes 
	 */
	std::vector<mvceditor::TagClass> ExactClassOrFile(const UnicodeString& search);
	
	/**
	 * Searches all the registered caches (working AND global caches)
	 * Will return near matches (it will call NearMatchClassesOrFiles
	 * on each tag finder).
	 *
	 * @see mvceditor::TagFinderClass::NearMatchClassesOrFiles
	 * @param string to search for
	 * @return std::vector<mvceditor::TagClass> matched resources. will be either files or classes
	 */
	std::vector<mvceditor::TagClass> NearMatchClassesOrFiles(const UnicodeString& search);

	/**
	 * gets all tags for a single class
	 * @param className the class to search.  not fully qualified (no namespace)
	 * @return vector of tags; 
	 *         all methods and properties that are defined in the class PLUS
	 *         all methods and properties that are defined in any of its base classes PLUS
	 *         all methods and properties that are defined in any of the traits used by any of the base classes
	 */
	std::vector<mvceditor::TagClass> AllMemberTags(const UnicodeString& className);

	/**
	 * gets all tags that were found in a single file. for classes, all of the class' members (including
	 * inherited members) are returned as well.
	 *
	 * @param fullPath full path to a file to lookup
	 * @return vector of tags; 
	 *         all functions or define's in the give file PLUS
	 *         all classes in the given file PLUS
	 *         all methods and properties that are defined in any classes PLUS
	 *         all methods and properties that are defined in any classes' base classes PLUS
	 *         all methods and properties that are defined in any of the traits used any classes PLUS
	 *         all methods and properties that are defined in any of the traits used by any of the base classes
	 */
	std::vector<mvceditor::TagClass> AllTagsInFile(const wxString& fullPath); 
	
	/**
	 * Collects all near matches that are possible candidates for completion of the parsed expression.
	 * Basically just a calls the ExpressionCompletionResourceMatches() of the given file's SymbolTable. See that method for more info
	 * 
	 * @param fileName the symbol table of this registered file will be searched
	 * @parm parsedExpression the parsed expression; from ParserClass::ParseExpression() 
	 * @param expressionScope the scope where parsed expression is located.  The scope let's us know which variables are
	 *        available. See ScopeFinderClass for more info.
	 * @param autoCompleteVariableList the results of the matches; these are the names of the variables that
	 *        are "near matches" to the parsed expression. This will be filled only when parsedExpression is a variable. 
	 * @param autoCompleteResourceList the results of the matches; these are the names of the items that
	 *        are "near matches" to the parsed expression. This will be filled only when parsedExpression is a variable "chain" or
	 *        a function / static class call.
	 * @param doDuckTyping if an expression chain could not be fully resolved; then we could still
	 *        perform a search for the expression member in ALL classes. The lookups will not be
	 *        slower because TagFinderClass still handles them
	 * @param error any errors / explanations will be populated here. error must be set to no error (initial state of object; or use Clear())
	 */
	void ExpressionCompletionMatches(const wxString& fileName, const pelet::ExpressionClass& parsedExpression, const pelet::ScopeClass& expressionScope, 
		std::vector<UnicodeString>& autoCompleteList,
		std::vector<TagClass>& autoCompleteResourceList,
		bool doDuckTyping,
		SymbolTableMatchErrorClass& error);

	/**
	 * This method will resolve the given parsed expression and will figure out the type of a tag.
	 * Basically just a calls the ResourceMatches() of the given file's SymbolTable. See that method for more info
	 * 
	 * @param fileName the symbol table of this registered file will be searched
	 * @parm parsedExpression the parsed expression; from ParserClass::ParseExpression() 
	 * @param expressionScope the scope where parsed expression is located.  The scope let's us know which variables are
	 *        available. See ScopeFinderClass for more info.
	 * @param matches all of the tag matches will be put here
	 * @param doDuckTyping if an expression chain could not be fully resolved; then we could still
	 *        perform a search for the expression member in ALL classes. The lookups will not be
	 *        slower because TagFinderClass still handles them
	 * @param doFullyQualifiedMatchOnly if TRUE the only resources that match fully qualified resources will be
	 *        returned
	 * @param error any errors / explanations will be populated here. error must be set to no error (initial state of object; or use Clear())
	 */
	void ResourceMatches(const wxString& fileName, const pelet::ExpressionClass& parsedExpression, const pelet::ScopeClass& expressionScope, 
		std::vector<TagClass>& matches,
		bool doDuckTyping, bool doFullyQualifiedMatchOnly,
		SymbolTableMatchErrorClass& error);
	
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
	std::vector<UnicodeString> ParentClassesAndTraits(const UnicodeString& className);
	 
private:
		
	/**
	 * Returns a list that contains all of the tag finders for the registered files plus
	 * the global tag finders.
	 * 
	 * This clas owns the tag finder pointers, do NOT delete them
	 */
	std::vector<TagFinderClass*> AllFinders();
	
	/**
	 * These are the tag finders from the ENTIRE projects; it may include stale resources
	 * This class will own these pointers and will delete them when appropriate.
	 */
	std::vector<mvceditor::GlobalCacheClass*> GlobalCaches;
	
	/**
	 * To calculate variable type information
	 * This class will own these pointers and will delete them when appropriate.
	 */
	std::map<wxString, mvceditor::WorkingCacheClass*> WorkingCaches;
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
extern const wxEventType EVENT_GLOBAL_CACHE_COMPLETE;


class WorkingCacheCompleteEventClass : public wxEvent {
public:

	/**
	 * This will be owned by the event handler
	 */
	mvceditor::WorkingCacheClass* WorkingCache;

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
		const wxString& fileName, const wxString& fileIdentifier, mvceditor::WorkingCacheClass* workingCache);

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

class GlobalCacheCompleteEventClass : public wxEvent {
public:

	/**
	 * This will be owned by the event handler
	 */
	mvceditor::GlobalCacheClass* GlobalCache;

	GlobalCacheCompleteEventClass(int id, mvceditor::GlobalCacheClass* globalCache);

	wxEvent* Clone() const;
};

}

typedef void (wxEvtHandler::*WorkingCacheCompleteEventClassFunction)(mvceditor::WorkingCacheCompleteEventClass&);

#define EVT_WORKING_CACHE_COMPLETE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_WORKING_CACHE_COMPLETE, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( WorkingCacheCompleteEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*GlobalCacheCompleteEventClassFunction)(mvceditor::GlobalCacheCompleteEventClass&);

#define EVT_GLOBAL_CACHE_COMPLETE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_GLOBAL_CACHE_COMPLETE, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( GlobalCacheCompleteEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*GlobalCacheCompleteEventClassFunction)(mvceditor::GlobalCacheCompleteEventClass&);

#define EVT_GLOBAL_CACHE_COMPLETE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_GLOBAL_CACHE_COMPLETE, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( GlobalCacheCompleteEventClassFunction, & fn ), (wxObject *) NULL ),

#endif