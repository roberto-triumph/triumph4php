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
#ifndef __RESOURCECACHEBUILDERTHREADCLASS_H__
#define __RESOURCECACHEBUILDERTHREADCLASS_H__

#include <widgets/ThreadWithHeartbeatClass.h>
#include <language/ResourceCacheClass.h>
#include <wx/event.h>

namespace mvceditor {

extern const wxEventType EVENT_WORKING_CACHE_COMPLETE;

extern const wxEventType EVENT_GLOBAL_CACHE_COMPLETE;

class WorkingCacheCompleteEventClass : public wxEvent {
public:

	/**
	 * This will be owned by the event handler
	 */
	mvceditor::WorkingCacheClass* WorkingCache;

	WorkingCacheCompleteEventClass(int eventId, const wxString& fileIdentifier, mvceditor::WorkingCacheClass* workingCache);

	wxEvent* Clone() const;

	/**
	 * @return the file identifier given in the constructor
	 */
	wxString GetFileIdentifier() const;

private:

	wxString FileIdentifier;
};

typedef void (wxEvtHandler::*WorkingCacheCompleteEventClassFunction)(WorkingCacheCompleteEventClass&);

#define EVT_WORKING_CACHE_COMPLETE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_WORKING_CACHE_COMPLETE, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( WorkingCacheCompleteEventClassFunction, & fn ), (wxObject *) NULL ),

class GlobalCacheCompleteEventClass : public wxEvent {
public:

	/**
	 * This will be owned by the event handler
	 */
	mvceditor::GlobalCacheClass* GlobalCache;

	GlobalCacheCompleteEventClass(int id, mvceditor::GlobalCacheClass* globalCache);

	wxEvent* Clone() const;
};

typedef void (wxEvtHandler::*GlobalCacheCompleteEventClassFunction)(GlobalCacheCompleteEventClass&);

#define EVT_GLOBAL_CACHE_COMPLETE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_GLOBAL_CACHE_COMPLETE, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( GlobalCacheCompleteEventClassFunction, & fn ), (wxObject *) NULL ),


/**
 * This class will run the resource updates in a background thread.  The caller will use
 * the Register() method when a new file is opened by the user.  Every so often, the 
 * StartBackgroundUpdate() method should be called to trigger re-parsing of the resources
 * on the background task.  The results of the resource parsing will be stored in an 
 * internal cache that's separate from the 'global' cache; that way the entire global cache
 * does not have to be re-sorted every time we want to parse new contents.
 * This class will NEVER update the 'global' resource finder; the caller must take care
 * of updating the global resource finder when the user closes the file.
 */
class WorkingCacheBuilderClass : public ThreadWithHeartbeatClass {
	
public:

	/**
	 * @param runningThreads the object will get notified to EVENT_WORK* events with the given ID
	 *        and the EVENT_WORKING_CACHE_COMPLETE.
	 */
	WorkingCacheBuilderClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);

	/**
	 * Will start the background thread 
	 * 
	 */
	wxThreadError Init(wxThreadIdType& threadId);
	
	/**
	 * this method blocks until the background thread terminates
	 * we havce to do this when the event handler has the same lifetime as 
	 * this object; we need the handler to be alive for longer than the 
	 * thread is alive because the thread will generate the EVENT_WORK_COMPLETE
	 * event after it has terminated (and the event handler needs to be valid)
	 */
	void Wait();
		
	/**
	 * Will parse the resources of the given text in a backgound thread and will
	 * post an EVENT_WORKING_CACHE_COMPLETE when the parsing is complete.
	 * 
	 * @param fileName unique identifier for a file
	 * @param code the file's most up-to-date source code (from the user-edited buffer)
	 * @param bool if TRUE then tileName is a new file that is not yet written to disk
	 * @param version The version of PHP to check against
	 */
	void Update(const wxString& fileName, const UnicodeString& code, bool isNew, pelet::Versions version);

protected:
	
	/**
	 * In the background thread we will parse the code for resources
	 */
	void BackgroundWork();
	
private:

	/**
	 * to prevent simultaneous access to the private members
	 */
	wxMutex Mutex;
	
	/**
	 * to implement the blocking wait. the main thread will call wait()
	 * on the condition right after it has deleted the background thread, once the
	 * the background thread terminates nicely the background thread will
	 * call broadcast and the main thread will unblock
	 */
	wxSemaphore Semaphore;

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

	/**
	 * The version of PHP to check against
	 */
	pelet::Versions CurrentVersion;

};

}
#endif