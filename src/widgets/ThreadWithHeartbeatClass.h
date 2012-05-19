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


namespace mvceditor {
	
/**
 * This event will be generated when the thread has completed its job
 * **successfully** A thread that has been killed will NOT generate this event.
 */
extern const wxEventType EVENT_WORK_COMPLETE;
/**
 * This event will be generated when the thread is in action. Event listeners
 * can do things like update status bars here.
 * event.GetString() may have a message describing the action being taken
 */
extern const wxEventType EVENT_WORK_IN_PROGRESS;

// defined below
class WorkerThreadClass;

/**
 * This is a thread that announces itself that is still running and when it ends.
 * This class is useful when displaying the status and percent completion to the
 * user. For now heartbeats will be at a constant rate.
 */
class ThreadWithHeartbeatClass : public wxEvtHandler {

public:
	
	/**
	 * @param handler will receive the EVENT_WORK_* events
	 * @param id if given, the generated event will have this id as its GetId() member.
	 */
	ThreadWithHeartbeatClass(wxEvtHandler& handler, int id = wxID_ANY);
	
	/**
	 * On object destruction, if this thread is running it will be stopped.
	 */
	virtual ~ThreadWithHeartbeatClass();
	
	/**
	 * This is the method to override; this method is executed in the background thread.
	 */
	virtual void Entry() = 0;

	/**
	 * A wrapper around wxThread::Create() method AND wxThread::Run() methods; the added bonus is that
	 * CreateSingleInstance() will first check for the existence of a running thread
	 * and will not Create (nor kill) a thread that is currently running. Create()
	 * will kill any running threads.
	 * This method ALSO starts thread execution, unlike wxThread::Create
	 */
	wxThreadError CreateSingleInstance();

	/**
	 * If there is a background thread running, stop it. This is NOT graceful stoppage
	 * it is a forceful action.  You should only use this as a last resort;  it
	 * is better that subclasses implement a stop flag and gracefully exit the Entry() method.
	 */
	void KillInstance();
	
	/**
	 * Will prepare to send events at regular intervals. After a call to this method, a 
	 * EVENT_WORK_IN_PROGRESS will be generated at regular intervals until SignalEnd()
	 * is called.
	 */
	void SignalStart();
	
	/**
	 * Will generate a EVENT_WORK_COMPLETE event and stop the EVENT_WORK_IN_PROGRESS events.
	 * Does not actually stop the thread.
	 */
	void SignalEnd();
	
	/**
	 * Will generate a EVENT_WORK_IN_PROGRESS event
	 */
	void OnTimer(wxTimerEvent& event);

	/**
	 * returns TRUE if there is a background thread running.
	 */
	bool IsRunning() const;
	
	/**
	 * @return true if thread should finish executing (exit gracefully from Entry() method)
	 */
	bool TestDestroy();
	
protected:

	wxEvtHandler& Handler;
	
private:
	wxTimer Timer;

	WorkerThreadClass* Worker;
	
	/**
	 * All generated events will have this ID as their EventId
	 */
	int EventId;
	
	DECLARE_EVENT_TABLE()

	friend class WorkerThreadClass;
};

/**
 * Using this class instead of wxThreadHelper because we want ThreadWithHeartbeat class
 * to be able to be re-started and we also want to guard against only 1 instance of it
 * being running WITHOUT killing the running instance. wxThreadHelper will kill
 * any previously running thread if Create() is called while a thread is still running.
 * As per the docs on wxThread: IsRunning() cannot be safely called when using
 * detached threads
 */
class WorkerThreadClass : public wxThread {

public:

	WorkerThreadClass(mvceditor::ThreadWithHeartbeatClass& owner);
	
protected:

	void* Entry();
	
private:

	mvceditor::ThreadWithHeartbeatClass& Owner;
};

}

#endif