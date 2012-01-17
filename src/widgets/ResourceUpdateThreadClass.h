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
#include <language/SymbolTableClass.h>
#include <widgets/ThreadWithHeartbeatClass.h>
#include <wx/event.h>
#include <unicode/unistr.h>
#include <map>
 
namespace mvceditor {

/**
 * This class will update the ResourceFinder with the 
 * latest code that has been typed in.
 */
class ResourceUpdateClass {
	
public:

	ResourceUpdateClass();
	
	~ResourceUpdateClass();
	
	/**
	 * Creates a new resource finder for the given file.
	 * 
	 * @param fileName unique identifier for a file
	 * @return bool TRUE if fileName was not previously registered
	 * only a unique fileName can be registered 
	 */
	bool Register(const wxString& fileName);
	
	/**
	 * Cleans up the resource finder from the given file. This should be called whenever
	 * the user is no longer editing the file.
	 * 
	 * @param fileName unique identifier for a file
	 */
	void Unregister(const wxString& fileName);
	
	/**
	 * Will run a background thread to parse the resources of the given 
	 * text.
	 * 
	 * @param fileName unique identifier for a file
	 * @param code the file's most up-to-date source code (from the user-edited buffer)
	 * @param bool if TRUE then tileName is a new file that is not yet written to disk
	 */
	bool Update(const wxString& fileName, const UnicodeString& code, bool isNew);
	
	/**
	 * prepares all the registered resource finders plus the one given.
	 * @param resourceFinder the resource finder to prep. This class will NOT own the pointer.
	 * @param resource the resource to look for
	 * @return bool TRUE if all resource finders could be preparead
	 */
	bool PrepareAll(ResourceFinderClass* resourceFinder, const wxString& resource);
	
	/**
	 * Searches all the registered resource finders plus the one given.
	 * Will search only for full matches (it will call CollectFullyQualifiedResource
	 * on each resource finder).
	 * @param resourceFinder the resource finder to search (in addition to all of the registered ones). This class will NOT own the pointer.
	 * @return bool TRUE if at least one resource finder got a match
	 */
	bool CollectFullyQualifiedResourceFromAll(ResourceFinderClass* resourceFinder);
	
	/**
	 * Searches all the registered resource finders plus the one given
	 * Will search for near matches (it will call CollectNearMatchResources
	 * on each resource finder).
	 * 
	 * @param resourceFinder the resource finder to search (in addition to all of the registered ones). This class will NOT own the pointer.
	 * @return bool TRUE if at least one resource finder got a match
	 */
	bool CollectNearMatchResourcesFromAll(ResourceFinderClass* resourceFinder);
	
	/**
	 * @return all of the matches from all finders that were found by the Collect* call.
	 */
	std::vector<ResourceClass> Matches(ResourceFinderClass* resourceFinder);
	
	/**
	 * Collects all near matches that are possible candidates for completion of the parsed expression.
	 * Basically just a calls the ExpressionCompletionResourceMatches() of the given file's SymbolTable. See that method for more info
	 * 
	 * @param fileName the symbol table of this registered file will be searched
	 * @parm parsedExpression the parsed expression; from ParserClass::ParseExpression() 
	 * @param expressionScope the scope where parsed expression is located.  The scope let's us know which variables are
	 *        available. See ScopeFinderClass for more info.
	 * @param resourceFinder resource finder to search (in addition to all of the registered ones). This class will NOT own the pointer.
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
	void ExpressionCompletionMatches(const wxString& fileName, const SymbolClass& parsedExpression, const UnicodeString& expressionScope, 
		ResourceFinderClass* resourceFinder, std::vector<UnicodeString>& autoCompleteList,
		std::vector<ResourceClass>& autoCompleteResourceList,
		bool doDuckTyping,
		SymbolTableMatchErrorClass& error) const;

	/**
	 * This method will resolve the given parsed expression and will figure out the type of a resource.
	 * Basically just a calls the ResourceMatches() of the given file's SymbolTable. See that method for more info
	 * 
	 * @param fileName the symbol table of this registered file will be searched
	 * @parm parsedExpression the parsed expression; from ParserClass::ParseExpression() 
	 * @param expressionScope the scope where parsed expression is located.  The scope let's us know which variables are
	 *        available. See ScopeFinderClass for more info.
	 * @param resourceFinder resource finder to search (in addition to all of the registered ones). This class will NOT own the pointer.
	 * @param matches all of the resource matches will be put here
	 * @param doDuckTyping if an expression chain could not be fully resolved; then we could still
	 *        perform a search for the expression member in ALL classes. The lookups will not be
	 *        slower because ResourceFinderClass still handles them
	 * @param error any errors / explanations will be populated here. error must be set to no error (initial state of object; or use Clear())
	 */
	void ResourceMatches(const wxString& fileName, const SymbolClass& parsedExpression, const UnicodeString& expressionScope, 
		ResourceFinderClass* resourceFinder, std::vector<ResourceClass>& matches,
		bool doDuckTyping,
		SymbolTableMatchErrorClass& error) const;
	
private:

	/**
	 * Returns a list that contains all of the resource finders for the registered files plus
	 * the given resource finder.  
	 * 
	 * This clas owns the resource finder pointers, do NOT delete them
	 */
	std::vector<ResourceFinderClass*> Iterator(ResourceFinderClass* resourceFinder);
	
	/**
	 * These are the objects that will parse the source codes
	 */
	std::map<wxString, ResourceFinderClass*> Finders;
	
	/**
	 * To calculate variable information
	 */
	std::map<wxString, SymbolTableClass*> SymbolTables;
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
class ResourceUpdateThreadClass : public ThreadWithHeartbeatClass {
	
public:

	/**
	 * @param the handler will get notified to EVENT_WORK* events with the given ID
	 */
	ResourceUpdateThreadClass(wxEvtHandler& handler, int eventId = wxID_ANY);
		
	/**
	 * Creates a new resource finder for the given file.
	 * This method is thread-safe.
	 * 
	 * @param fileName unique identifier for a file
	 * @param handler will get notified with a EVENT_WORK_COMPLETE when resoruce parsing
	 * is complete. This class will NOT own the pointer.
	 * @return bool TRUE if fileName was not previously registered
	 * only a unique fileName can be registered 
	 */
	bool Register(const wxString& fileName, wxEvtHandler* handler);
	
	/**
	 * Cleans up the resource finder from the given file. This should be called whenever
	 * the user is no longer editing the file.
	 * This method is thread-safe.
	 * 
	 * @param fileName unique identifier for a file
	 */
	void Unregister(const wxString& fileName);
	
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

	/**
	 * Calls ResourceUpdateClass::ExpressionCompletionMatches in a thread-safe
	 * manner; with the caveat that if a lock could not be acquired then this 
	 * method does nothing.
	 * @see ResourceUpdatesClas::ExpressionCompletionMatches 
	 */
	void ExpressionCompletionMatches(const wxString& fileName, const SymbolClass& parsedExpression, const UnicodeString& expressionScope, 
		ResourceFinderClass* resourceFinder, std::vector<UnicodeString>& autoCompleteList,
		std::vector<ResourceClass>& autoCompleteResourceList,
		bool doDuckTyping,
		SymbolTableMatchErrorClass& error);

	/**
	 * Calls  and ResourceUpdateClass::PrepareAll and 
	 * ResourceUpdateClass::CollectNearMatchResourcesFromAll in a thread-safe
	 * manner; with the caveat that if a lock could not be acquired then this 
	 * method does nothing.
	 * @see ResourceUpdatesClass::PrepareAll
	 * @see ResourceUpdatesClass::CollectNearMatchResourcesFromAll
	 */
	std::vector<ResourceClass> PrepareAndCollectNearMatchResourcesFromAll(ResourceFinderClass* globalresourceFinder, const wxString& resource);

	/**
	 * Calls  and ResourceUpdateClass::ResourceMatches in a thread-safe
	 * manner; with the caveat that if this method will BLOCK until a lock
	 * is acquired.
	 * @see ResourceUpdatesClass::ResourceMatches 
	 */
	void ResourceMatches(const wxString& fileName, const SymbolClass& parsedExpression, const UnicodeString& expressionScope, 
		ResourceFinderClass* resourceFinder, std::vector<ResourceClass>& matches, bool doDuckTyping,
		SymbolTableMatchErrorClass& error);

protected:
	
	/**
	 * In the background thread we will parse the code for resources
	 */
	void* Entry();
	
private:

	/**
	 * This is the object that will hold all of the resource cache. It should not be accessed while
	 * the thread is running
	 */
	ResourceUpdateClass Worker;

	/**
	 * Make sure accesses are thread-safe
	 */
	wxMutex WorkerMutex;

	/**
	 * These guys will get notified when the parsing is complete. This class will NOT own
	 * the handler pointers.
	 */
	std::map<wxString, wxEvtHandler*> Handlers;

	/**
	 * the code that is being worked on by the background thread.
	 */
	UnicodeString CurrentCode;
	
	/**
	 * the name of the file that is being worked on by the background thread.
	 */
	wxString CurrentFileName;

	/**
	 * if TRUE then tileName is a new file that is not yet written to disk
	 */
	bool CurrentFileIsNew;

};
	 
};

#endif