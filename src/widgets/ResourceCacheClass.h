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
#ifndef __RESOURCEUPDATETHREADCLASS_H__
#define __RESOURCEUPDATETHREADCLASS_H__

#include <search/ResourceFinderClass.h>
#include <search/DirectorySearchClass.h>
#include <language/SymbolTableClass.h>
#include <widgets/ThreadWithHeartbeatClass.h>
#include <wx/event.h>
#include <unicode/unistr.h>
#include <map>
 
namespace mvceditor {

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
 * The resource cache can handle multiple global caches; this can be used to separate the
 * resource cache into multiple files; one file per project. Something like this:
 *
 * @code
 
 * mvceditor::ResourceCacheClass cache;
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
 * Since ResourceCacheClass is backed by ResourceFinderClass, the global resource finders 
 * are stored on disk and are preserved across application restarts. This means that 
 * if the cache file already exists, then WalkGlobal() may return immediately if the file
 * to be parsed has not been modified since the last time we parsed it.
 *
 * Note that all of the methods of this class can be safely accessed by multiple
 * threads.
 */
class ResourceCacheClass {
	
public:

	/**
	 * Make sure accesses are thread-safe
	 */
	wxMutex Mutex;

	ResourceCacheClass();
	
	~ResourceCacheClass();
	
	/**
	 * Creates a new resource finder for the given file.
	 * 
	 * @param fileName unique identifier for a file
	 * @param createSymbols if TRUE then the symbol table from the file will be created immediately; otherwise
	 * caller will need to call Update() method to build the symbol table.
	 * @return bool TRUE if fileName was not previously registered
	 * only a unique fileName can be registered 
	 */
	bool Register(const wxString& fileName, bool createSymbols);
	
	/**
	 * Cleans up the resource finder from the given file. This should be called whenever
	 * the user is no longer editing the file.
	 * 
	 * @param fileName unique identifier for a file
	 */
	void Unregister(const wxString& fileName);
	
	/**
	 * Will parse the resources of the given text into the file's LOCAL cache.
	 * The fileName must have been previously Register() 'ed
	 * 
	 * @param fileName unique identifier for a file that was given to Register()
	 * @param code the file's most up-to-date source code (from the user-edited buffer)
	 * @param bool if TRUE then fileName is a new file that is not yet written to disk
	 * @return false if lock could not be acquired or code contains a parse error
	 */
	bool Update(const wxString& fileName, const UnicodeString& code, bool isNew);

	/**
	 * Create a new global resource finder that is backed by the given 
	 * file.  File may or may not exist; if file does not exist then it will
	 * be created.
	 * A resource DB file can only be initialized once.
	 *
	 * @param resourceDbFileName the location of the SQLite cache for the resource finder
	 * @param fileParsingBufferSize the size of an internal buffer where parsed resources are initially 
	 *        stored. This is only a hint, the buffer will grow as necessary
	 *        Setting this value to a high value (1024) is good for large projects that have a lot
	 *        resources.
	 * @return bool FALSE if the resource file is already initialized
	 */
	bool InitGlobal(const wxFileName& resourceDbFileName, int fileParsingBufferSize = 32);

	/**
	 * check to see if a resource DB file is already loaded
	 *
	 * @return bool TRUE if db file is already loaded.
	 */
	bool IsInitGlobal(const wxFileName& resourceDbFileName);

	/**
	 * Will update the GLOBAL cache by calling Walk() on the global resource cache (in a thread safe manner)
	 * The last finder created by InitGlobal() will be modified.
	 *
	 * @see mvceditor::ResourceFinderClass::Walk
	 * @param resourceDbFileName the location of the SQLite cache for the resource finder. this 
	 *        is the file where the resources will be persisted to
	 * @param directorySearch keeps track of the file to parse
	 * @param phpFileFilters the wildcards that hold which files to parse
	 * @return false if lock could not be acquired
	 */
	bool WalkGlobal(const wxFileName& resourceDbFileName, DirectorySearchClass& directorySearch, const std::vector<wxString>& phpFileFilters);

	/**
	 * calls AllNonNativeClasses() method on the GLOBAL resource. This is usually done after all files have been indexed.
	 * @see mvceditor::ResourceFinderClass::AllNonNativeClasses
	 */
	std::vector<ResourceClass> AllNonNativeClassesGlobal();
	
	/**
	 * Searches all the registered resource finders plus the global given.
	 * Will search only for full matches (it will call CollectFullyQualifiedResource
	 * on each resource finder).
	 * @param search string to search for
	 * @return std::vector<mvceditor::ResourceClass> matched resources 
	 */
	std::vector<mvceditor::ResourceClass> CollectFullyQualifiedResourceFromAll(const UnicodeString& search);
	
	/**
	 * Searches all the registered resource finders plus the global given
	 * Will search for near matches (it will call CollectNearMatchResources
	 * on each resource finder).
	 *
	 * @param string to search for
	 * @return std::vector<mvceditor::ResourceClass> matched resources
	 */
	std::vector<mvceditor::ResourceClass> CollectNearMatchResourcesFromAll(const UnicodeString& search);

	/**
	 * thread-safe wrapper for ResourceFinderClass::AddDynamicResouces on the GLOBAL cache
	 * @see ResourceFinderClass::AddDynamicResources
	 */
	void GlobalAddDynamicResources(const std::vector<ResourceClass>& resources);
	
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
	 *        slower because ResourceFinderClass still handles them
	 * @param error any errors / explanations will be populated here. error must be set to no error (initial state of object; or use Clear())
	 */
	void ExpressionCompletionMatches(const wxString& fileName, const pelet::ExpressionClass& parsedExpression, const pelet::ScopeClass& expressionScope, 
		std::vector<UnicodeString>& autoCompleteList,
		std::vector<ResourceClass>& autoCompleteResourceList,
		bool doDuckTyping,
		SymbolTableMatchErrorClass& error);

	/**
	 * This method will resolve the given parsed expression and will figure out the type of a resource.
	 * Basically just a calls the ResourceMatches() of the given file's SymbolTable. See that method for more info
	 * 
	 * @param fileName the symbol table of this registered file will be searched
	 * @parm parsedExpression the parsed expression; from ParserClass::ParseExpression() 
	 * @param expressionScope the scope where parsed expression is located.  The scope let's us know which variables are
	 *        available. See ScopeFinderClass for more info.
	 * @param matches all of the resource matches will be put here
	 * @param doDuckTyping if an expression chain could not be fully resolved; then we could still
	 *        perform a search for the expression member in ALL classes. The lookups will not be
	 *        slower because ResourceFinderClass still handles them
	 * @param doFullyQualifiedMatchOnly if TRUE the only resources that match fully qualified resources will be
	 *        returned
	 * @param error any errors / explanations will be populated here. error must be set to no error (initial state of object; or use Clear())
	 */
	void ResourceMatches(const wxString& fileName, const pelet::ExpressionClass& parsedExpression, const pelet::ScopeClass& expressionScope, 
		std::vector<ResourceClass>& matches,
		bool doDuckTyping, bool doFullyQualifiedMatchOnly,
		SymbolTableMatchErrorClass& error);
	
	/**
	 * print the resource cache to stdout. Really only useful for debugging and not much else
	 */
	void Print();
	
	/**
	 * @return TRUE if the cache has not been initialized with either a call to Register() or a call to WalkGlobal()
	 */
	bool IsFileCacheEmpty();

	/**
	 * @return TRUE if the cache has not been initialized with either a call to Register() or a call to WalkGlobal()
	 */
	bool IsResourceCacheEmpty();
	
	/**
	 * Remove all items from all caches and also unregisters any and all files.
	 */
	void Clear();
	
	/**
	 * Set the version that the PHP parser should use.
	 */
	void SetVersion(pelet::Versions version);
	 
private:
		
	/**
	 * Returns a list that contains all of the resource finders for the registered files plus
	 * the global resource finders.
	 * 
	 * This clas owns the resource finder pointers, do NOT delete them
	 */
	std::vector<ResourceFinderClass*> AllFinders();
	
	/**
	 * These are the objects that will parse the source codes
	 */
	std::map<wxString, ResourceFinderClass*> Finders;
	
	/**
	 * To calculate variable type information
	 */
	std::map<wxString, SymbolTableClass*> SymbolTables;

	/**
	 * These are the resource finders from the ENTIRE project; it may include stale resources
	 * The key is the full path to the resource finder DB file, the value is the
	 * resource finder itself.
	 */
	std::map<wxString, mvceditor::ResourceFinderClass*> GlobalResourceFinders;
	
	/**
	 * the version of PHP to use when parsing source code
	 */
	pelet::Versions Version;
};

/**
 * This class will run the resource updates in a background thread.  The caller will use
 * the Register() method when a new file is opened by the user.  Every so ofte, the 
 * StartBackgroundUpdate() method should be called to trigger re-parsing of the resources
 * on the background task.  The results of the resource parsing will be stored in an 
 * internal cache that's separate from the 'global' cache; that way the entire global cache
 * does not have to be re-sorted every time we want to parse new contents.
 * This class will NEVER update the 'global' resource finder; the caller must take care
 * of updating the global resource finder when the user closes the file.
 */
class ResourceCacheUpdateThreadClass : public ThreadWithHeartbeatClass {
	
public:

	/**
	 * @param the handler will get notified to EVENT_WORK* events with the given ID
	 * pointer will NOT be owned by this object
	 */
	ResourceCacheUpdateThreadClass(ResourceCacheClass* resourceCache, wxEvtHandler& handler,
		mvceditor::RunningThreadsClass& runningThreads, int eventId = wxID_ANY);
		
	/**
	 * Will run a background thread to parse the resources of the given 
	 * text.
	 * This method is thread-safe.
	 * 
	 * @param fileName unique identifier for a file
	 * @param code the file's most up-to-date source code (from the user-edited buffer)
	 * @param bool if TRUE then tileName is a new file that is not yet written to disk
	 */
	wxThreadError StartBackgroundUpdate(const wxString& fileName, const UnicodeString& code, bool isNew);

protected:
	
	/**
	 * In the background thread we will parse the code for resources
	 */
	void Entry();
	
private:

	/**
	 * the type of work that will happen in the background thread
	 */
	enum Modes {
		UPDATE
	} Mode;

	/**
	 * This is the object that will hold all of the resource cache. It should not be accessed while
	 * the thread is running. Pointer will NOT be owned by this object.
	 */
	ResourceCacheClass* ResourceCache;

	/**
	 * the code that is being worked on by the background thread.
	 */
	UnicodeString CurrentCode;
	
	/**
	 * the name of the file that is being worked on by the background thread.
	 */
	wxString CurrentFileName;

	/**
	 * if TRUE then tileName is a new file that does not yet exist on disk
	 */
	bool CurrentFileIsNew;

};
	 
};

#endif