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
#ifndef __MVCEDITOR_ACTIONCLASS_H__
#define __MVCEDITOR_ACTIONCLASS_H__

#include <wx/thread.h>
#include <wx/event.h>
#include <wx/timer.h>
#include <queue>

namespace mvceditor {

// defined below
class RunningThreadsClass;

/**
 * An action is any short of long-lived logic that needs to be executed asynchronously.
 * An action is given to RunningThreadsClass to be queued; RunningThreads will then
 * call the BackgroundWork() method in a background thread and will the delete
 * the action.  Actions should always be created on the heap because RunningThreadsClass
 * will take ownership of them.
 * 
 * Actions can be cancelled by another process, like for example in response to a
 * user click button.  Actions should periodically check the IsCancelled() method and
 * should return as soon as IsCancelled returns TRUE.
 *
 * When subclassing an ActionClass, make sure that any data passed from the main thread
 * and used by the background thread are copied properly. Pay special attention to 
 * wxString variables, as the default wxString
 * assignment operator and copy constructors are NOT thread-safe (produce
 * shallow copies)
 *
 * An action will communicate with the rest of the program by posting events
 * to RunningThreadsClass, the rest of the program will attach themselves as
 * event handlers of RunningThreadsClass.  For example, an action that 
 * executes a SQL query in the background would 
 *
 * 1.) copy the connection info into member variables via the constructor
 *     or an Init() method.  the variables would be cloned.
 * 2.) in the implementation of BackgroundWork() the query would be executed,
 *     read into object. a wxEvent would be passed to the rest of the 
 *     program by calling PostEvent()
 *
 */
class ActionClass {

public:

	/**
	 * ProgressMode can be one of two modes: determinate or indeterminate.
	 * Determinate mode means that an action is able to give an accurate estimate of
	 * how much it has completed at any given time.
	 * Indeterminate mode is means that an action cannot accurately estimate how
	 * much it has completed at any given time.
	 */
	enum ProgressMode {
		DETERMINATE,
		INDETERMINATE
	};

	/**
	 * @param runningThreads used to post events. This reference must be 
	 *        alive for as long as this class is alive.
	 * @param eventId the posted events will have this ID as the EventID
	 */
	ActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);

	virtual ~ActionClass();

	/**
	 * This is the method to override; this method is executed in the background thread.
	 */
	virtual void BackgroundWork() = 0;

	/**
	 * @return wxString a short description of this action.
	 */
	virtual wxString GetLabel() const = 0;

	/**
	 * if the thread allocates any dynamic memory (using NEW) but this thread is
	 * Delete()'d before the thread gets a chance to wxPostEvent() to the main
	 * thread, then it should delete the memory in this method. This method will
	 * get called in the main thread.
	 */
	virtual void DoCancel();

	/**
	 * ask the action to stop.  the action will then stop as soon as possible but will
	 * do so while retaining a clean state.
	 *
	 * this method can be called from a different thread
	 */
	void Cancel();

	/**
	 * Will generate a EVENT_ACTION_COMPLETE event and stop the EVENT_ACTION_IN_PROGRESS events.
	 * Does not actually stop the thread.
	 */
	void SignalEnd();

	/**
	 * @return int the event ID for this action
	 */
	int GetEventId() const;

	/**
	 * a number used to identify this action; will be used to cancel the action.
	 * usually, this action ID is used in conjunction with RunningThreads class.
	 */
	void SetActionId(int actionId);

	/**
	 * get the ID of this action.
	 * usually, the action ID is used in conjunction with RunningThreads class.
	 */
	int GetActionId();

	/**
	 * @return ProgressMode the way that the action tracks its progress
	 */
	mvceditor::ActionClass::ProgressMode GetProgressMode();

	/**
	 * get the percentage complete of this action. only non-zero when the action
	 * supports determinate mode.
	 */
	int GetPercentComplete();

	/**
	 * send an event to all of the handlers that have registered via RunningThreads::AddHandler
	 * method.
	 */
	void PostEvent(wxEvent& event);

protected:

	/**
	 * Generates a EVENT_ACTION_STATUS event with the given string
	 * as the wxCommandEvent.GetString
	 */
	void SetStatus(const wxString& status);

	/**
	 * subclasses should call this method often in the BackgroundWork() method; subclasses
	 * should exit the BackgroundWork() method after IsCancelled() returns TRUE
	 */
	bool IsCancelled();

	/**
	 * @paramrProgressMode set the way that the action tracks its progress
	 */
	void SetProgressMode(mvceditor::ActionClass::ProgressMode mode);

	/**
	 * @param int percentComplete a number between 0 and 100
	 */
	void SetPercentComplete(int percentComplete);

private:

	/**
	 * Keeps a reference to this object's running thread; that way
	 * we can send events to other threads.
	 */
	mvceditor::RunningThreadsClass& RunningThreads;

	/**
	 * All events generated by this action will have this ID as their EventId
	 */
	int EventId;

	/**
	 * a number used to identify this action; will be used to cancel the action.
	 * usually, this action ID is used in conjunction with RunningThreads class.
	 */
	int ActionId;

	/**
	 * the mutext controls access to Cancelled boolean
	 */
	wxMutex Mutex;

	/**
	 * flag to signal that the action should return immediately even if it has 
	 * not completed its work.
	 */
	bool Cancelled;

	/**
	 * The way that the action tracks its progress
	 */
	mvceditor::ActionClass::ProgressMode Mode;

	/** 
	 * The progress in the current action. This is only valid if the action is in determinate progress mode
	 */
	int PercentComplete;
};

/**
 * A small class that is used to contain code that will be run when a thread 
 * is end.  The class will be used in the context of the background thread, not
 * the main thread!
 *
 * The main reason for this class is to cleanup MySQL connections; as 
 * the MySQL driver creates some data in each thread (mysql_thread_init()) 
 * and we need to clean it up by calling mysql_thread_end() in the 
 * context of the background thread.
 */
class ThreadCleanupClass {

public:

	/**
	 * override this method to put in logic to be executed just as the thread
	 * ends.
	 */
	virtual void ThreadEnd() = 0;

	/**
	 * override this method to return a cloned instance of itself.
	 * since we use more than 1 background thread, we need to
	 * create new instances of this object for each thread.
	 */
	virtual mvceditor::ThreadCleanupClass* Clone() = 0;
};

/**
 * A small class that will run in a background thread, look at a queue
 * of ActionClass instances and Run each action (call BackgroundWork()) in the 
 * background thread.
 * This class will continually poll the queue, pop actions off the
 * queue, call BackgroundWork() on them, and delete them once
 * BackgroundWork() has finished.
 */
class ThreadActionClass : public wxThread {

public:

	/**
	 *
	 * @param actions to be run; First In First Out
	 *        this class will continually poll the queue, pop actions off the
	 *        queue, call BackgroundWork() on them, and delete them once
	 *        BackgroundWork() has finished.
	 * @param actionsMutex to prevent simultaneous access to Actions
	 * @param finishSemaphore to signal when all actions have been cleaned up
	 * @param threadCleanup code to be run once the thread ends. this class will own the pointer
	 */
	ThreadActionClass(std::queue<mvceditor::ActionClass*>& actions, wxMutex& actionsMutex,
		wxSemaphore& finishSemaphore, mvceditor::ThreadCleanupClass* threadCleanup);

	void* Entry();

	/**
	 * cancel the current action that is running, but only if the action ID matches
	 * the given ID.  this method is used when a single action is to be termnated
	 * early.
	 *
	 * Do not confuse this from wxThread::Delete, this is the cancel mechanism for
	 * the action class; so that actions themselves can terminate early when signaled
	 * to do so.  When an action is cancelled, this thread will pop the next action
	 * off the queue and start woking on it.
	 *
	 * @param actionId the ID returned by mvceditor::RunningThreadsClass::Add method
	 */
	void CancelRunningActionIf(int actionId);

	/**
	 * cancel the current action that is running.  this method is used when 
	 * this thread needs to exit; we need the running action to return out of its
	 * BackgroundWork() method so that the thread can gracefully end.
	 *
	 * Do not confuse this from wxThread::Delete, this is the cancel mechanism for
	 * the action class; so that actions themselves can terminate early when signaled
	 * to do so.  
	 */
	void CancelRunningAction();

	/**
	 * posts the progress event for the action that is currently running
	 */
	void PostProgressEvent();

private:

	/**
	 * actions to be run; First In First Out
	 */
	std::queue<mvceditor::ActionClass*>& Actions;

	/**
	 * Prevent simultaneous access to Actions
	 */
	wxMutex& ActionsMutex;

	/**
	 * this is to signal when the background task has finished cleanup
	 */
	wxSemaphore& FinishSemaphore;

	/**
	 * prevent simultaneous access to the running action
	 */
	wxMutex RunningActionMutex;

	/**
	 * handle to the action that is actually running; we need to hold
	 * a pointer to it since we remove the pointer from the queue
	 * while it is working
	 */
	mvceditor::ActionClass* RunningAction;

	/**
	 * logic to be called when a thread ends
	 */
	mvceditor::ThreadCleanupClass* ThreadCleanup;

	/**
	 * @return action  next action from the queue, or NULL if queue is empty
	 */
	mvceditor::ActionClass* NextAction();

	/**
	 * cleanup the action
	 * @param action to finalize
	 */
	void ActionComplete(mvceditor::ActionClass* action);

	/**
	 * cleanup alls action that are still in the queue
	 */
	void CleanupAllActions();
  };
  
/**
 * Class to hold all of the actions that are currently running. wxWidgets
 * threads are detached by default, we cannot call the IsRunning or IsAlive
 * methods on them; this makes it necessary for us to track the threads
 * in order to stop them on demand (like say, when the user clicks a stop
 * button or closes a panel).
 *
 * Code that needs to run an action in the background will create an action
 * in the heap, initialize it as need it, then call the Add() method.  Once 
 * the Add() method is called, the action will be run in the background at
 * some point in the future.  Add() can be called many times if needed, actions
 * are queued up and run one after another in the background.
 * 
 * This class will own all given actions, and will delete them need.
 */
class RunningThreadsClass : public wxEvtHandler {
  
  	public:
  	
	RunningThreadsClass(bool doPostEvents = true);
	
	~RunningThreadsClass();
	
	/**
	 * Only call this method BEFORE any items are queued up
	 * @param int maxThreads number threads to start. This can be zero, if so
	 *        then we will start as many threads as there are CPUs in the system
	 */
	void SetMaxThreads(int maxThreads);

	/**
	 * Queues the given action to be run at some point in the near future. 
	 * This method will return an identifier that can be used to stop the 
	 * action if needed.
	 *
	 * It is very important to note that once an action is queued, it may 
	 * deleted at any time after that so the action pointer should not be accessed
	 * at all.
	 *
	 * @param action this class will own the pointer and delete it
	 * @return an action ID, which can be used to cancel the action at at 
	 * later time.
	 */
	int Queue(mvceditor::ActionClass* action);

	/**
	 * gracefully stops the given action, allowing the other added actions
	 * to keep going. If the action is running it is cancelled. if it is not
	 * running, it will be removed from the queue and deleted.
	 *
	 * @param int actionID as given by the Add() method
	 */
	void CancelAction(int actionId);

	/**
	 * stop all of the running threads. This method is guaranteed to block
	 * until all threads have terminated. If this method is hanging 
	 * indefinitely, it means that one of the running threads has not
	 * been calling Cancel() correctly.
	 */
	void StopAll();
	
	/**
	 * stops all running threads, and additionally will no longer queue up any
	 * actions given to be queued.  This method is usually called before this item
	 * goes out of scope.
	 */
	void Shutdown();

	/**
	 * This method should called before any actions are added. 
	 * 
	 * @param threadCleanup object to be called when a thread ends.
	 *        this class will own the pointer
	 *        this class will clone the object.
	 */
	void SetThreadCleanup(mvceditor::ThreadCleanupClass* threadCleanup);

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
  
	private:
  
	/**
	 * holds all actions that need to be run. This class will add 
	 * actions to the queue. An action will be removed as soon as it
	 * is actually worked on. The background threads will check to
	 * see if this queue is non-empty and then pop items from it and
	 * "work" on them (call BackgroundWork() method)
	 */
	std::queue<mvceditor::ActionClass*> Actions;
  
	/**
	 * Prevent simultaneous access to Actions
	 */
	wxMutex ActionMutex;

	/**
	 * this is the background thread that pops items from the action queue
	 * and actually runs the actions
	 */
	std::vector<mvceditor::ThreadActionClass*> ThreadActions;
	
	/**
	 * holds all event handlers to post events to. This object
	 * will not own these pointers.
	 */
	std::vector<wxEvtHandler*> Handlers;
	
	/**
	 * prevent concurrent access to the handlers
	 */
	wxMutex HandlerMutex;
	
	/**
	 * to implement blocking wait when stopping the background
	 * threads
	 */
	wxSemaphore* Semaphore;

	/**
	 *  To generate the heartbeats (EVENT_WORK_IN_PROGRESS)
	 */
	wxTimer Timer;
 
	/**
	 * logic to be called when a thread ends
	 */
	mvceditor::ThreadCleanupClass* ThreadCleanup;

	/**
	 * wheter to send events in the current event loop or the next event
	 * loop (wxPostEvent vs. ProcessEvent() ) .  this flag is used for
	 * unit tests since we don't have an event loop in the unit tests.
	 */
	bool DoPostEvents;

	/**
	 * a number that uniquely identifies an action. no two queued actions will have
	 * the same ID.
	 */
	int NextActionId;

	/**
	 * the max number of threads to create
	 */
	int MaxThreads;
	
	/**
	 * if TRUE no items will be queued.
	 */
	bool IsShutdown;
	
	/**
	 * Will generate a EVENT_ACTION_IN_PROGRESS event
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

/**
 * event identifiers for all actions. These should be used
 * as the eventIDs for the corresponding actions unless 
 * it is not possible.
 * start at 20,000 to prevent collison with feature menu IDs
 * ID_MENU_*
 */
 enum {
	ID_EVENT_ACTION_TAG_FINDER_LIST_INIT = wxID_HIGHEST + 20000,
	ID_EVENT_ACTION_TAG_FINDER_LIST,
	ID_EVENT_ACTION_SQL_METADATA_INIT,
	ID_EVENT_ACTION_SQL_METADATA,
	ID_EVENT_ACTION_URL_TAG_DETECTOR,
	ID_EVENT_ACTION_CALL_STACK,
	ID_EVENT_ACTION_TEMPLATE_FILE_TAG_DETECTOR,
	ID_EVENT_ACTION_TAG_DETECTOR_INIT,
	ID_EVENT_ACTION_TAG_DETECTOR,
	ID_EVENT_ACTION_DATABASE_TAG_DETECTOR,
	ID_EVENT_ACTION_CONFIG_TAG_DETECTOR,
	ID_EVENT_ACTION_TAG_FINDER_LIST_WIPE,
	ID_EVENT_ACTION_TAG_CACHE_VERSION_CHECK,
	ID_EVENT_ACTION_DETECTOR_CACHE_VERSION_CHECK,
	ID_EVENT_ACTION_DETECTOR_DB_INIT
 };

 

/**
 * This event will be generated when the thread has completed its job
 * **successfully** A thread that has been stopped via RunningThreads::Stop()
 * or will still generate this event.
 */
extern const wxEventType EVENT_ACTION_COMPLETE;

/**
 * This event will be generated when the thread is in action. Event listeners
 * can do things like update status bars here. The event handle receives an event
 * of type mvceditor::ActionProgressEvent.
 * event.Message may have a message describing the action being taken.
 */
extern const wxEventType EVENT_ACTION_PROGRESS;

/**
 * This is an event that actions use to announce how much of their task
 * they have completed.
 * Progress can be one of two modes: determinate or indeterminate.
 * Determinate mode means that an action is able to give an accurate estimate of
 * how much it has completed at any given time.
 * Indeterminate mode is means that an action cannot accurately estimate how
 * much it has completed at any given time.
 * When an action uses determinated mode, it assigns a percentage completed 
 * that is a number between 0 and 100, with 0 being the start of
 * the task and 100 being the end of the task.
 */
class ActionProgressEventClass : public wxEvent {

public:

	/** on of  either determinate or indeterminate */
	mvceditor::ActionClass::ProgressMode Mode;
	
	/** a number between 0 and 100 */
	int PercentComplete;

	/** a message to be displayed */
	wxString Message;

	ActionProgressEventClass(int id, mvceditor::ActionClass::ProgressMode mode, int percentComplete, const wxString& msg);

	wxEvent* Clone() const;

};

/**
 * An event with a string member that is cloned (deep copied)
 * so that it can be safely passed between threads.  Since wxString by default
 * uses "fast" assignments by sharing the same data pointer, we want to deep
 * copy any strings that we pass from one thread to another.
 */
class ActionEventClass : public wxEvent {
	
public:

	/**
	 * a string, copied between threads in a safe way
	 */
	wxString Message;

	ActionEventClass(int id, wxEventType type, const wxString& msg);
		
	wxEvent* Clone() const;
	
};

typedef void (wxEvtHandler::*ActionEventClassFunction)(mvceditor::ActionEventClass&);
typedef void (wxEvtHandler::*ActionProgressEventClassFunction)(mvceditor::ActionProgressEventClass&);


#define EVT_ACTION_PROGRESS(id, fn) \
        DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_ACTION_PROGRESS, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( ActionProgressEventClassFunction, & fn ), (wxObject *) NULL ),

#define EVT_ACTION_COMPLETE(id, fn) \
        DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_ACTION_COMPLETE, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( ActionEventClassFunction, & fn ), (wxObject *) NULL ),


}

#endif