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
 * This event will be generated when the thread has completed its job
 * **successfully** A thread that has been stopped via RunningThreads::Stop()
 * or will still generate this event.
 */
extern const wxEventType EVENT_WORK_COMPLETE;
/**
 * This event will be generated when the thread is in action. Event listeners
 * can do things like update status bars here.
 * event.GetString() may have a message describing the action being taken
 */
extern const wxEventType EVENT_WORK_IN_PROGRESS;

// defined below
class RunningThreadsClass;

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
	
	/**
	 * This is the method to override; this method is executed in the background thread.
	 */
	virtual void BackgroundWork() = 0;

	/**
	 * if the thread allocates any dynamic memory (using NEW) but this thread is
	 * Delete()'d before the thread gets a chance to wxPostEvent() to the main
	 * thread, then it should delete the memory in this method. This method will
	 * get called in the background thread.
	 */
	virtual void BackgroundCleanup();

	/**
	 * send an event to all of the handlers that have registered via RunningThreads::AddHandler
	 * method.
	 */
	void PostEvent(wxEvent& event);
	
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
	 * Keeps a reference to this object's running thread; that way
	 * we can send events to other threads.
	 */
	mvceditor::RunningThreadsClass& RunningThreads;

	/**
	 * All generated events will have this ID as their EventId
	 */
	int EventId;
	
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
	
	/**
	 * Will generate a EVENT_WORK_COMPLETE event and stop the EVENT_WORK_IN_PROGRESS events.
	 * Does not actually stop the thread.
	 */
	void SignalEnd();
	
	DECLARE_EVENT_TABLE() 
};

/**
 * Class to hold all of the threads that are currently running.  wxWidgets
 * threads are detached by default, we cannot call the IsRunning or IsAlive
 * methods on them; this makes it necessary for us to track the threads
 * in order to stop them on demand (like say, when the user clicks a stop
 * button or closes a panel).
 * 
 * This class will own the given threads, and will delete them if need be.
 */
 class RunningThreadsClass {

	public:
	
	RunningThreadsClass();
	
	/**
	 * Keeps track of the given worker.  Most of the times we will
	 * do nothing with this pointer; wxWidgets detached threads delete themselves.
	 * The only time we will need this pointer is when we want to stop
	 * the thread while it is running. Remember that it is not safe to
	 * call IsAlive, IsRunning methods on detached threads.
	 * 
	 * @param worker must be a DETACHED thread
	 */
	void Add(wxThread* thread);
	
	/**
	 * Method to "forget" the given worker.
	 * This method should be called by the worker itself right before it
	 * exits the Entry() method (in the background thread).
	 * @param thread the thread to be untracked
	 */
	void Remove(wxThread* thread);

	/**
	 * stop all of the running threads. This method is guaranteed to block
	 * until all threads have terminated. If this method is hanging 
	 * indefinitely, it means that one of the running threads has not
	 * been calling TestDestroy() correctly.
	 */
	void StopAll();

	/**
	 * gracefully stops the thread with the given ID.
	 * see wxThread::Delete
	 *
	 * @param thread ID of the thread to stop; see wxThread::GetId()
	 */
	void Stop(wxThreadIdType threadId);

	/**
	 * adds an event handler to this instance.  Running threads will
	 * post events to all registered handlers.  The handlers pointer
	 * is NOT owned by this object; the caller must ensure to call
	 * RemoveEventHandler before the event handler goes out of scope.
	 */
	void AddEventHandler(wxEvtHandler* handler);

	/**
	 * removes event handler to this instance, if handler has
	 * not been registered then this method does nothing.
	 */
	void RemoveEventHandler(wxEvtHandler* handler);

	/**
	 * post an event (using wxPostEvent) to all registered handlers.
	 * this means that the event will be received in the next event loop.
	 */
	void PostEvent(wxEvent& event);

	/**
	 * Check to see if the given thread is being tracked; if it was previously
	 * added with the Add() method but not yet removed via the Remove() or Stop()
	 * methods.  If this returns true, then it means that the thread is definitely alive.
	 * This is safe to call on detached or joinable threads, as it does not depend
	 * on wxThread::IsAlive or  wxThread::IsRunning methods.
	 */
	bool IsRunning(wxThread* thread);

	private:

	/**
	 * holds all threads that are alive and running. These are 'self-destructing'
	 * threads that will tell us when they need to be removed.
	 */
	std::vector<wxThread*> Workers;
	
	/**
	 * holds all event handlers to post events to. This object
	 * will not own these pointers.
	 */
	std::vector<wxEvtHandler*> Handlers;
	
	/**
	 * prevent concurrent access to the internal vectors
	 */
	wxMutex Mutex;	
	
	/**
	 * to implement blocking wait when stopping all
	 * threads
	 */
	wxSemaphore* Semaphore;
};

}

#endif