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
#include <widgets/ThreadWithHeartbeatClass.h>
#include <wx/event.h>
#include <unicode/unistr.h>
#include <map>
 
namespace mvceditor {
	
/**
 * This class will update the ResourceFinder with the 
 * latest code that has been typed in.
 */
class ResourceUpdateThreadClass : public ThreadWithHeartbeatClass {
	
public:

	ResourceUpdateThreadClass(wxEvtHandler& handler, int eventId = wxID_ANY);
	
	virtual ~ResourceUpdateThreadClass();
	
	/**
	 * Creates a new resource finder for the given file.
	 * 
	 * @param fileName unique identifier for a file
	 * @param handler will get notified with a EVENT_WORK_COMPLETE when resoruce parsing
	 * is complete. This class will NOT own the pointer.
	 */
	void Register(const wxString& fileName, wxEvtHandler* handler);
	
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
	wxThreadError UpdateResources(const wxString& fileName, const UnicodeString& code);
	
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
	 * @param resourceFinder the resource finder to search. This class will NOT own the pointer.
	 * @return bool TRUE if at least one resource finder got a match
	 */
	bool CollectFullyQualifiedResourceFromAll(ResourceFinderClass* resourceFinder);
	
	/**
	 * Searches all the registered resource finders plus the one given
	 * Will search for near matches (it will call CollectNearMatchResources
	 * on each resource finder).
	 * 
	 * @param resourceFinder the resource finder to search. This class will NOT own the pointer.
	 * @return bool TRUE if at least one resource finder got a match
	 */
	bool CollectNearMatchResourcesFromAll(ResourceFinderClass* resourceFinder);
	
	/**
	 * Returns a list that contains all of the resource finders for the registered files plus
	 * the given resource finder.  
	 * 
	 * This clas owns the resource finder pointers, do NOT delete them
	 */
	std::vector<ResourceFinderClass*> Iterator(ResourceFinderClass* resourceFinder);
	
	
protected:

	/**
	 * In the background thread we will parse the code for resources
	 */
	void* Entry();
	
private:

	std::map<wxString, ResourceFinderClass*> Finders;
	
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
	 * The finder that is being worked on by the background thread.
	 * This is just a pointer to one of the finders that is in the map
	 * no need to delete
	 */
	ResourceFinderClass* BusyFinder;
	
	/**
	 * the handler to notify once the current parsing finishes
	 * This is just a pointer to the Handlers; no need to delete
	 */
	wxEvtHandler* CurrentHandler;
	
};
	
	 
};

#endif