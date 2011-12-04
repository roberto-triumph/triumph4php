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
	 */
	bool Update(const wxString& fileName, const UnicodeString& code);
	
	/**
	 * Check to see if the given resource comes from one of the registered (opened
	 * files).  If this returns true, it means that the resource may be stale.
	 */
	bool IsDirty(const ResourceClass& resource);
	
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
	 * Get the symbol that is at the given pos on the given file.
	 * 
	 * @param fileName the symbol table of this registered file will be searched
	 * @param resourceFinder resource finder to search (in addition to all of the registered ones). This class will NOT own the pointer.
	 * @param symbol
	 * @return the symbol name; can be empty if it could not figured out or pos is invalid
	 */
	UnicodeString GetSymbolAt(const wxString& fileName, int pos, ResourceFinderClass* resourceFinder, mvceditor::SymbolClass& symbol, const UnicodeString& code);
	
	/**
	 * Basically just a calls the GetVariablesInScope() of the given file's SymbolTable
	 */	
	std::vector<UnicodeString> GetVariablesInScope(const wxString& fileName, int pos, const UnicodeString& code);
	
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
 * This class will run the resource updates in a background thread
 */
class ResourceUpdateThreadClass : public ThreadWithHeartbeatClass {
	
public:

	/**
	 * @param the handler will get notified to EVENT_WORK* events with the given ID
	 */
	ResourceUpdateThreadClass(wxEvtHandler& handler, int eventId = wxID_ANY);
		
	/**
	 * Creates a new resource finder for the given file.
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
	 */
	wxThreadError StartBackgroundUpdate(const wxString& fileName, const UnicodeString& code);
	
	/**
	 * This is the object that will hold all of the resource cache. It should not be accessed while
	 * the thread is running
	 */
	ResourceUpdateClass Worker;

protected:
	
	/**
	 * In the background thread we will parse the code for resources
	 */
	void* Entry();
	
private:

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

};
	 
};

#endif