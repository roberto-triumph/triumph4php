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

	WorkingCacheCompleteEventClass(mvceditor::WorkingCacheClass* workingCache);

	wxEvent* Clone() const;
};

typedef void (wxEvtHandler::*WorkingCacheCompleteEventClassFunction)(WorkingCacheCompleteEventClass&);

#define EVT_WORKING_CACHE_COMPLETE(fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_WORKING_CACHE_COMPLETE, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( WorkingCacheCompleteEventClassFunction, & fn ), (wxObject *) NULL ),

class GlobalCacheCompleteEventClass : public wxEvent {
public:

	/**
	 * This will be owned by the event handler
	 */
	mvceditor::GlobalCacheClass* GlobalCache;

	GlobalCacheCompleteEventClass(mvceditor::GlobalCacheClass* globalCache);

	wxEvent* Clone() const;
};

typedef void (wxEvtHandler::*GlobalCacheCompleteEventClassFunction)(GlobalCacheCompleteEventClass&);

#define EVT_GLOBAL_CACHE_COMPLETE(fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_GLOBAL_CACHE_COMPLETE, wxID_ANY, -1, \
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
	 * @param the handler will get notified to EVENT_WORK* events with the given ID
	 *        and the EVENT_WORKING_CACHE_COMPLETE.
	 *        pointer will NOT be owned by this object
	 */
	WorkingCacheBuilderClass(wxEvtHandler& handler,
		mvceditor::RunningThreadsClass& runningThreads, int eventId = wxID_ANY);

	/**
	 * Will start the background thread 
	 * 
	 */
	wxThreadError Init();
		
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