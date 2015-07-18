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
#include <wx/intl.h>
#include <algorithm>

t4p::ThreadCleanupClass::ThreadCleanupClass() {

}

t4p::ThreadCleanupClass::~ThreadCleanupClass() {

}

t4p::ActionClass::ActionClass(t4p::RunningThreadsClass& runningThreads, int eventId)
	: RunningThreads(runningThreads)
	, EventId(eventId)
	, ActionId(0)
	, Mutex()
	, Cancelled(false)
	, Mode(INDETERMINATE)
	, PercentComplete(0) {
}

t4p::ActionClass::~ActionClass() {

}

void t4p::ActionClass::Cancel() {
	{   // NOLINT(whitespace/braces) we want a lock to only last in this block
		wxMutexLocker locker(Mutex);
		Cancelled = true;
	}
	DoCancel();
}

bool t4p::ActionClass::IsCancelled() {
	wxMutexLocker locker(Mutex);
	return Cancelled;
}

void t4p::ActionClass::SetStatus(const wxString& status) {
	t4p::ActionProgressEventClass evt(wxID_ANY, GetProgressMode(), GetPercentComplete(), status);
	PostEvent(evt);
}

int t4p::ActionClass::GetEventId() const {
	return EventId;
}

void t4p::ActionClass::SetActionId(int actionId) {
	wxMutexLocker locker(Mutex);
	ActionId = actionId;
}

int t4p::ActionClass::GetActionId() {
	wxMutexLocker locker(Mutex);
	return ActionId;
}

void t4p::ActionClass::PostEvent(wxEvent& event) {
	event.SetId(EventId);
	RunningThreads.PostEvent(event);
}

void t4p::ActionClass::DoCancel() {
}


void t4p::ActionClass::SetProgressMode(t4p::ActionClass::ProgressMode mode) {
	wxMutexLocker locker(Mutex);
	Mode = mode;
}

t4p::ActionClass::ProgressMode t4p::ActionClass::GetProgressMode() {
	wxMutexLocker locker(Mutex);
	return Mode;
}

void t4p::ActionClass::SetPercentComplete(int percentComplete) {
	wxMutexLocker locker(Mutex);
	PercentComplete = percentComplete;
}

int t4p::ActionClass::GetPercentComplete() {
	wxMutexLocker locker(Mutex);
	return PercentComplete;
}


void t4p::ActionClass::SignalEnd() {
	wxString msg = wxString::Format(wxT("Action \"%s\" stopped...\n"), (const char*)GetLabel().c_str());
	t4p::ActionEventClass evt(GetEventId(), t4p::EVENT_ACTION_COMPLETE, msg);
	PostEvent(evt);
}

t4p::ThreadActionClass::ThreadActionClass(std::queue<t4p::ActionClass*>& actions, wxMutex& actionsMutex,
												wxSemaphore& finishSemaphore,
												t4p::ThreadCleanupClass* threadCleanup)
	: wxThread(wxTHREAD_DETACHED)
	, Actions(actions)
	, ActionsMutex(actionsMutex)
	, FinishSemaphore(finishSemaphore)
	, RunningActionMutex()
	, RunningAction(NULL)
	, ThreadCleanup(threadCleanup) {

}

void t4p::ThreadActionClass::CancelRunningActionIf(int actionId) {
	wxMutexLocker locker(RunningActionMutex);
	if (RunningAction && RunningAction->GetActionId() == actionId) {
		RunningAction->Cancel();
	}
}

void t4p::ThreadActionClass::CancelRunningAction() {
	wxMutexLocker locker(RunningActionMutex);
	if (RunningAction) {
		RunningAction->Cancel();
	}
}

void t4p::ThreadActionClass::PostProgressEvent() {
	wxMutexLocker locker(RunningActionMutex);
	if (RunningAction) {
		int eventId = RunningAction->GetEventId();
		t4p::ActionProgressEventClass evt(eventId, RunningAction->GetProgressMode(), RunningAction->GetPercentComplete(), wxT(""));
		RunningAction->PostEvent(evt);
	}
}

void* t4p::ThreadActionClass::Entry() {
	while (!TestDestroy()) {
		t4p::ActionClass* action = NextAction();
		if (action && !TestDestroy()) {

			// signal the start of this action
			t4p::ActionProgressEventClass evt(action->GetEventId(), action->GetProgressMode(), 0, wxT(""));
			action->PostEvent(evt);

			try {
				action->BackgroundWork();
			} catch (std::exception& e) {
				wxASSERT_MSG(true, e.what());
			}
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
	wxUnusedVar(err);
	wxASSERT_MSG(wxSEMA_NO_ERROR == err, wxT("error posting to finish semaphore"));
	return 0;
}

t4p::ActionClass* t4p::ThreadActionClass::NextAction() {
	t4p::ActionClass* action = NULL;
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

void t4p::ThreadActionClass::ActionComplete(t4p::ActionClass* action) {
	action->SignalEnd();
	wxMutexLocker actionLocker(RunningActionMutex);
	delete action;
	RunningAction = NULL;
}

void t4p::ThreadActionClass::CleanupAllActions() {
	wxMutexLocker locker(ActionsMutex);
	while (!Actions.empty()) {
		delete Actions.front();
		Actions.pop();
	}
}

t4p::RunningThreadsClass::RunningThreadsClass(bool doPostEvents)
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
	, MaxThreads(0)
	, IsShutdown(false) {
	Timer.SetOwner(this);
	SetMaxThreads(wxThread::GetCPUCount());
}

t4p::RunningThreadsClass::~RunningThreadsClass() {
	Shutdown();
	delete Semaphore;
	if (ThreadCleanup) {
		delete ThreadCleanup;
	}
}

void t4p::RunningThreadsClass::SetMaxThreads(int maxThreads) {
	if (maxThreads <= 0) {
		maxThreads = wxThread::GetCPUCount();
	}
	if (maxThreads <= 0) {
		maxThreads = 2;
	}
	MaxThreads = maxThreads;
	if (Semaphore) {
		delete Semaphore;
	}
	Semaphore = new wxSemaphore(0, MaxThreads);
}

int t4p::RunningThreadsClass::Queue(t4p::ActionClass* action) {
	if (IsShutdown) {
		delete action;
		wxASSERT_MSG(IsShutdown, _("Cannot queue items when the running threads has been shutdown"));
		return -1;
	}
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
			t4p::ThreadActionClass* thread = new t4p::ThreadActionClass(
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

void t4p::RunningThreadsClass::CancelAction(int actionId) {

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
	std::queue<t4p::ActionClass*> checked;
	t4p::ActionClass* action;
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
	std::vector<t4p::ThreadActionClass*>::iterator thread;
	for (thread = ThreadActions.begin(); thread != ThreadActions.end(); ++thread) {
		(*thread)->CancelRunningActionIf(actionId);
	}
}

void t4p::RunningThreadsClass::StopAll() {

	// stop the timer, the in progress handler will want
	// to lock the action mutex
	Timer.Stop();
	if (ThreadActions.empty()) {
		return;
	}

	{  //  NOLINT(whitespace/braces) we want a lock to only last in this block
		// delete all queued actions. we do this first so that when we stop
		// the running action the thread does not start working on the next action
		// in the queue
		wxMutexLocker locker(ActionMutex);
		while (!Actions.empty()) {
			delete Actions.front();
			Actions.pop();
		}
	}

	// if there are running actions stop then signal them to stop
	std::vector<t4p::ThreadActionClass*>::iterator thread;
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
		wxUnusedVar(err);
		wxASSERT_MSG(wxSEMA_INVALID != err, wxT("semaphore is invalid"));
		wxASSERT_MSG(wxSEMA_TIMEOUT != err, wxT("semaphore timed out"));
		wxASSERT_MSG(wxSEMA_MISC_ERROR != err, wxT("semaphore misc error"));
	}
	ThreadActions.clear();
}

void t4p::RunningThreadsClass::Shutdown() {
	IsShutdown = true;
	StopAll();
}

void t4p::RunningThreadsClass::AddEventHandler(wxEvtHandler *handler) {
	wxMutexLocker locker(HandlerMutex);
	wxASSERT(locker.IsOk());
	Handlers.push_back(handler);
}

void t4p::RunningThreadsClass::RemoveEventHandler(wxEvtHandler *handler) {
	wxMutexLocker locker(HandlerMutex);
	wxASSERT(locker.IsOk());
	std::vector<wxEvtHandler*>::iterator it = std::find(Handlers.begin(), Handlers.end(), handler);
	if (it != Handlers.end()) {
		Handlers.erase(it);
	}
}

void t4p::RunningThreadsClass::PostEvent(wxEvent& event) {
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

void t4p::RunningThreadsClass::OnTimer(wxTimerEvent& event) {

	// if there is an action that is running then send an in-progress event
	// for it
	for (size_t i = 0; i < ThreadActions.size(); ++i) {
		ThreadActions[i]->PostProgressEvent();
	}
}

void t4p::RunningThreadsClass::SetThreadCleanup(t4p::ThreadCleanupClass* threadCleanup) {
	ThreadCleanup = threadCleanup;
}

t4p::ActionEventClass::ActionEventClass(int id, wxEventType type, const wxString& msg)
: wxEvent(id, type)
, Message() {
	Message.append(msg);
}

wxEvent* t4p::ActionEventClass::Clone() const {
	t4p::ActionEventClass* clone = new t4p::ActionEventClass(GetId(), GetEventType(), Message);
	return clone;
}

t4p::ActionProgressEventClass::ActionProgressEventClass(int id, t4p::ActionClass::ProgressMode mode, int percentComplete, const wxString& msg)
: wxEvent(id, t4p::EVENT_ACTION_PROGRESS)
, Mode(mode)
, PercentComplete(percentComplete)

// thread-safe clone of the string
, Message(msg.c_str()) {

}

wxEvent* t4p::ActionProgressEventClass::Clone() const {
	return new t4p::ActionProgressEventClass(GetId(), Mode, PercentComplete, Message);
}

const wxEventType t4p::EVENT_ACTION_PROGRESS = wxNewEventType();
const wxEventType t4p::EVENT_ACTION_COMPLETE = wxNewEventType();


BEGIN_EVENT_TABLE(t4p::RunningThreadsClass, wxEvtHandler)
	EVT_TIMER(wxID_ANY, t4p::RunningThreadsClass::OnTimer)
END_EVENT_TABLE()
