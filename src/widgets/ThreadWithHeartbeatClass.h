/**
 * The MIT License
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
#ifndef __THREADWITHHEARTBEATCLASS_H__
#define __THREADWITHHEARTBEATCLASS_H__

#include <wx/thread.h>
#include <wx/timer.h>
#include <vector>

namespace mvceditor {
	
/**
 * This is a thread that announces itself that is still running and when it ends.
 * This class is useful when displaying the status and percent completion to the
 * user. For now heartbeats will be at a constant rate.
 * 
 * This class is also useful for correctly implementing background threads.
 * Threads in wxWidgets by default are detached; this means that IsAlive() and
 * IsRunning() methods cannot be called on them. This class forces the use
 * of RunningThreads class, which implements thread deletion correctly. The only
 * restriction is that the given instance of RunningThreads must be valid
 * until the threads have terminated (otherwise threads will attempt to post events
 * to an invalid object). To make sure of this, the method RunningThreads::StopAll
 * must be used to stop all running threads.
 *
 * Threads created with this class are ALWAYS gracefully terminated.  This means
 * that the implementing classes MUST check TestDestroy() many times and exit promptly.
 * 
 */
#if 0
class ThreadWithHeartbeatClass : public wxEvtHandler, public wxThread {

public:
	
	/**
	 * @param runningThreads to stop the thread gracefully if need be. Will also receive the EVENT_WORK_* events
	 * @param id if given, the generated event will have this id as its GetId() member.
	 */
	ThreadWithHeartbeatClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);
	
	virtual ~ThreadWithHeartbeatClass();

	/**
	 * A wrapper around wxThread::Create() method AND wxThread::Run() methods; the added bonus is that
	 * CreateSingleInstance() will first check for the existence of a running thread
	 * and will not Create (nor kill) a thread that is currently running. Create()
	 * will kill any running threads.
	 * This method ALSO starts thread execution, unlike wxThread::Create
	 * Because of this, you need to take care and make sure any member variables
	 * are accessed safely as soon as this method exits. Example of what NOT to do:
	 *
	 *  class MyThread : public ThreadWithHeartbeatClass {
	 *
	 *    wxString Name;
	 *
	 *    bool Init(wxString& name) {
	 *      CreateSingleInstance();
	 *      // WRONG! at this point Entry could already be working, the variable Name 
	 *      // may be written to and read from at the same time.
	 *      Name = name;
	 *    }
	 *
	 *    void Entry() {
	 *      Name.ToLower();
	 *    }
	 *  }
	 *
	 */
	wxThreadError CreateSingleInstance(wxThreadIdType& threadId);
	
	
protected:

	/**
	 * This method is executed in the background thread.
	 */
	void* Entry();

private:

	/**
	 *  To generate the heartbeats (EVENT_WORK_IN_PROGRESS)
	 */
	wxTimer Timer;
	
	/**
	 * Will generate a EVENT_WORK_IN_PROGRESS event
	 */
	void OnTimer(wxTimerEvent& event);
	
	/**
	 * Will prepare to send events at regular intervals. After a call to this method, a 
	 * EVENT_WORK_IN_PROGRESS will be generated at regular intervals until SignalEnd()
	 * is called.
	 */
	void SignalStart();
	
	DECLARE_EVENT_TABLE() 
};

#endif
}

#endif