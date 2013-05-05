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
 * @copyright  2009-20Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <actions/ActionClass.h>
#include <algorithm>
	
mvceditor::ActionClass::ActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: RunningThreads(runningThreads)
	, EventId(eventId)
	, ActionId(0)
	, Mutex()
	, Cancelled(false) {
}

mvceditor::ActionClass::~ActionClass() {

}

void mvceditor::ActionClass::Cancel() {
	wxMutexLocker locker(Mutex);
	Cancelled = true;
}

bool mvceditor::ActionClass::IsCancelled() {
	wxMutexLocker locker(Mutex);
	return Cancelled;
}

void mvceditor::ActionClass::SetStatus(const wxString& status) {
	wxCommandEvent evt(mvceditor::EVENT_ACTION_STATUS);
	
	// make sure to copy, since wxString copy is not thread safe
	wxString cpy(status.c_str());
	evt.SetString(cpy);
	PostEvent(evt);
}

int mvceditor::ActionClass::GetEventId() const {
	return EventId;
}

void mvceditor::ActionClass::SetActionId(int actionId) {
	wxMutexLocker locker(Mutex);
	ActionId = actionId;
}

int mvceditor::ActionClass::GetActionId() {
	wxMutexLocker locker(Mutex);
	return ActionId;
}

void mvceditor::ActionClass::PostEvent(wxEvent& event) {
	event.SetId(EventId);
	RunningThreads.PostEvent(event);
}

void mvceditor::ActionClass::BackgroundCleanup() {
}


void mvceditor::ActionClass::SignalEnd() {
	wxCommandEvent evt(mvceditor::EVENT_WORK_COMPLETE, EventId);
	wxString msg = wxString::Format(wxT("Action \"%s\" stopped...\n"), (const char*)GetLabel().c_str());
	evt.SetString(msg);
	PostEvent(evt);
}

mvceditor::ThreadActionClass::ThreadActionClass(std::queue<mvceditor::ActionClass*>& actions, wxMutex& actionsMutex, 
												wxSemaphore& finishSemaphore,
												mvceditor::ThreadCleanupClass* threadCleanup)
	: wxThread(wxTHREAD_DETACHED) 
	, Actions(actions)
	, ActionsMutex(actionsMutex) 
	, FinishSemaphore(finishSemaphore) 
	, RunningActionMutex() 
	, RunningAction(NULL) 
	, ThreadCleanup(threadCleanup) {

}

void mvceditor::ThreadActionClass::CancelRunningActionIf(int actionId) {
	wxMutexLocker locker(RunningActionMutex);
	if (RunningAction && RunningAction->GetActionId() == actionId) {
		RunningAction->Cancel();
	}
}

void mvceditor::ThreadActionClass::CancelRunningAction() {
	wxMutexLocker locker(RunningActionMutex);
	if (RunningAction) {
		RunningAction->Cancel();
	}
}

int mvceditor::ThreadActionClass::GetRunningActionEventId() {
	int eventId = -1;
	wxMutexLocker locker(RunningActionMutex);
	if (RunningAction) {
		eventId = RunningAction->GetEventId();
	}
	return eventId;
}

void* mvceditor::ThreadActionClass::Entry() {
	while (!TestDestroy()) {
		mvceditor::ActionClass* action = NextAction();
		if (action && !TestDestroy()) {
			action->BackgroundWork();
			ActionComplete(action);
		}
		else if (action) {
			
			// we want to exit, don't call action->BackgroundWork 
			// as it can take a while to complete
			ActionComplete(action);
			break;
		}
		else if (!TestDestroy()) {
			// no action to work on, wait a bit 
			// 100 milliseconds = 100 microseconds * 1000
			wxMicroSleep(100 * 1000);
		}
	}

	// at this point we want to exit; cleanup any
	// actions that we did not run
	CleanupAllActions();

	// call any other logic
	if (ThreadCleanup) {
		ThreadCleanup->ThreadEnd();
		delete ThreadCleanup;
	}

	wxSemaError err = FinishSemaphore.Post();
	wxASSERT_MSG(wxSEMA_NO_ERROR == err, wxT("error posting to finish semaphore"));
	return 0;
}

mvceditor::ActionClass* mvceditor::ThreadActionClass::NextAction() {
	mvceditor::ActionClass* action = NULL;
	wxMutexLocker locker(ActionsMutex);
	if (!Actions.empty()) {
		action = Actions.front();

		// take it off the queue so that other queues don't try
		// to run it
		Actions.pop();
	}
	wxMutexLocker actionLocker(RunningActionMutex);
	RunningAction = action;
	return action;
}

void mvceditor::ThreadActionClass::ActionComplete(mvceditor::ActionClass* action) {
	action->SignalEnd();
	wxMutexLocker actionLocker(RunningActionMutex);
	delete action;
	RunningAction = NULL;	
}

void mvceditor::ThreadActionClass::CleanupAllActions() {
	wxMutexLocker locker(ActionsMutex);
	while (!Actions.empty()) {
		delete Actions.front();
		Actions.pop();
	}	
}
  
mvceditor::RunningThreadsClass::RunningThreadsClass(bool doPostEvents)
	: wxEvtHandler()
	, Actions()
	, ActionMutex()
	, ThreadActions() 
	, Handlers()
	, HandlerMutex()
	, Semaphore(NULL)
	, Timer() 
	, ThreadCleanup(NULL)
	, DoPostEvents(doPostEvents) 
	, NextActionId(0) 
	, MaxThreads(0) {
	Timer.SetOwner(this);
	MaxThreads = wxThread::GetCPUCount();
	if (MaxThreads <= 0) {
		MaxThreads = 2;
	}
	Semaphore = new wxSemaphore(0, MaxThreads);
}

mvceditor::RunningThreadsClass::~RunningThreadsClass() {
	delete Semaphore;
	if (ThreadCleanup) {
		delete ThreadCleanup;
	}
}
  
int mvceditor::RunningThreadsClass::Queue(mvceditor::ActionClass* action) {
	if (!Timer.IsRunning()) {
		Timer.Start(200, wxTIMER_CONTINUOUS);
	}
	
	// prevent multiple accesses to Actions queue
	wxMutexLocker locker(ActionMutex);
		
	Actions.push(action);

	// assign the action a unique ID 
	int actionId = -1;
	actionId = NextActionId++;
	action->SetActionId(actionId);

	// if the actual thread has not started, start it
	if (ThreadActions.empty()) {		
		for (int i = 0; i < MaxThreads; ++i) {
			ThreadCleanupClass* cleanup = NULL;
			if (ThreadCleanup) {
				cleanup = ThreadCleanup->Clone();
			}
			mvceditor::ThreadActionClass* thread = new mvceditor::ThreadActionClass(
				Actions, ActionMutex, *Semaphore, 
				
				// each thread gets its own instance, so that we dont have to worry about
				// synchronization 
				cleanup);
			wxThreadError error = wxTHREAD_NO_ERROR;
			error = thread->Create();
			wxASSERT_MSG(error == wxTHREAD_NO_ERROR, wxT("Thread could not be started"));
			if (error == wxTHREAD_NO_ERROR) {
				thread->Run();
			}
			ThreadActions.push_back(thread);
		}
	}
	return actionId;
}

void mvceditor::RunningThreadsClass::CancelAction(int actionId) {

	// this is an important lock because it ensures that
	// the front action is not deleted while we try to call Cancel on it
	wxMutexLocker locker(ActionMutex);
	wxASSERT(locker.IsOk());

	// if there is nothing in the queue nothing to cancel
	if (Actions.empty()) {
		return;
	}

	// the threads in the queue are not yet running, so we can just
	// remove them from the queue
	std::queue<mvceditor::ActionClass*> checked;
	mvceditor::ActionClass* action;
	while (!Actions.empty()) {
		action = Actions.front();
		if (action->GetActionId() != actionId) {

			// keep this action
			checked.push(action);
		}
		else {

			// this action we need to remove
			delete action;
		}		
		Actions.pop();
	}
	
	// now put back the actions we checked
	while (!checked.empty()) {
		Actions.push(checked.front());
		checked.pop();
	}

	// check all running actions to see which one matches the actionId.
	// note that if the action is being run, ThreadAction will delete the pointer once
	// the thread dies.  we cannot delete it here because the thread
	// is still running and delete it will cause crashes (invalid memory
	// accesses)	
	std::vector<mvceditor::ThreadActionClass*>::iterator thread;
	for (thread = ThreadActions.begin(); thread != ThreadActions.end(); ++thread) {
		(*thread)->CancelRunningActionIf(actionId);
	}
}

void mvceditor::RunningThreadsClass::StopAll() {

	// stop the timer, the in progress handler will want
	// to lock the action mutex
	Timer.Stop();
	if (ThreadActions.empty()) {
		return;
	}

	// if there are running actions stop then signal them to stop
	std::vector<mvceditor::ThreadActionClass*>::iterator thread;
	for (thread = ThreadActions.begin(); thread != ThreadActions.end(); ++thread) {
		(*thread)->CancelRunningAction();
	}

	// this tells the background thread to not run anymore
	// actions in the queue
	// make sure that this call is NOT in the mutex locker because
	// in windows since all threads are joinable Delete() causes a deadlock
	// ThreadAction is a wxThread it will get cleaned up automatically
	for (size_t i = 0; i < ThreadActions.size(); ++i) {
		ThreadActions[i]->Delete();

		wxSemaError err = Semaphore->WaitTimeout(4000);
		wxASSERT_MSG(wxSEMA_INVALID != err, wxT("semaphore is invalid"));
		wxASSERT_MSG(wxSEMA_TIMEOUT != err, wxT("semaphore timed out"));
		wxASSERT_MSG(wxSEMA_MISC_ERROR != err, wxT("semaphore misc error"));
	}
	ThreadActions.clear();
}

void mvceditor::RunningThreadsClass::AddEventHandler(wxEvtHandler *handler) {
	wxMutexLocker locker(HandlerMutex);
	wxASSERT(locker.IsOk());
	Handlers.push_back(handler);
}

void mvceditor::RunningThreadsClass::RemoveEventHandler(wxEvtHandler *handler) {
	wxMutexLocker locker(HandlerMutex);
	wxASSERT(locker.IsOk());
	std::vector<wxEvtHandler*>::iterator it = std::find(Handlers.begin(), Handlers.end(), handler);
	if (it != Handlers.end()) {
		Handlers.erase(it);
	}
}

void mvceditor::RunningThreadsClass::PostEvent(wxEvent& event) {
	wxMutexLocker locker(HandlerMutex);
	wxASSERT(locker.IsOk());
	std::vector<wxEvtHandler*>::iterator it;
	for (it = Handlers.begin(); it != Handlers.end(); ++it) {
		if (DoPostEvents) {
			wxPostEvent(*it, event);
		}
		else {
			(*it)->ProcessEvent(event);
		}
	}
}

void mvceditor::RunningThreadsClass::OnTimer(wxTimerEvent& event) {

	// if there is an action that is running then send an in-progress event
	// for it
	for (size_t i = 0; i < ThreadActions.size(); ++i) {
		int eventId = ThreadActions[i]->GetRunningActionEventId();
		if (eventId > 0) {
			wxCommandEvent evt(mvceditor::EVENT_WORK_IN_PROGRESS, eventId);
			PostEvent(evt);	
		}
	}

	// if there is an action queued then an in-progress event
	// for it
	wxMutexLocker locker(ActionMutex);
	std::queue<mvceditor::ActionClass*> copy;
	mvceditor::ActionClass* action;
	while (!Actions.empty()) {
		action = Actions.front();
		wxCommandEvent evt(mvceditor::EVENT_WORK_IN_PROGRESS, action->GetEventId());
		PostEvent(evt);
		copy.push(action);

		Actions.pop();
	}
	// put actions back in the queue
	while (!copy.empty()) {
		Actions.push(copy.front());
		copy.pop();
	}
}

void mvceditor::RunningThreadsClass::SetThreadCleanup(mvceditor::ThreadCleanupClass* threadCleanup) {
	ThreadCleanup = threadCleanup;
}


const wxEventType mvceditor::EVENT_ACTION_STATUS = wxNewEventType();
const wxEventType mvceditor::EVENT_WORK_COMPLETE = wxNewEventType();
const wxEventType mvceditor::EVENT_WORK_IN_PROGRESS = wxNewEventType();


BEGIN_EVENT_TABLE(mvceditor::RunningThreadsClass, wxEvtHandler)
	EVT_TIMER(wxID_ANY, mvceditor::RunningThreadsClass::OnTimer)
END_EVENT_TABLE()