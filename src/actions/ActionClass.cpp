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

mvceditor::ActionClass::ActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: RunningThreads(runningThreads)
	, EventId(eventId)
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

void mvceditor::ActionClass::PostEvent(wxEvent& event) {
	event.SetId(EventId);
	RunningThreads.PostEvent(event);
}

void mvceditor::ActionClass::BackgroundCleanup() {
}

void mvceditor::ActionClass::SignalEnd() {
	wxCommandEvent evt(mvceditor::EVENT_WORK_COMPLETE, EventId);
	wxString msg = wxString::Format(wxT("Thread %s stopped...\n"), GetLabel());
	evt.SetString(msg);
	PostEvent(evt);
}

mvceditor::ThreadActionClass::ThreadActionClass(std::queue<mvceditor::ActionClass*>& actions, wxMutex& actionsMutex, wxSemaphore& finishSemaphore)
	: Actions(actions)
	, ActionsMutex(actionsMutex) 
	, FinishSemaphore(finishSemaphore) {

}

void* mvceditor::ThreadActionClass::Entry() {
	while (!TestDestroy()) {
		mvceditor::ActionClass* action = NextAction();
		if (action && !TestDestroy()) {
			action->BackgroundWork();
			ActionComplete();
		}
		else if (action) {
			
			// we want to exit, don't call action->BackgroundWork 
			// as it can take a while to complete
			ActionComplete();
			break;
		}
		else {
			// no action to work on, wait a bit 
			wxMicroSleep(300);
		}
	}
	wxSemaError err = FinishSemaphore.Post();
	wxASSERT_MSG(wxSEMA_NO_ERROR == err, wxT("error posting to finish semaphore"));

	// at this point we want to exit; cleanup any
	// actions that we did not run
	wxMutexLocker locker(ActionsMutex);
	while (!Actions.empty()) {
		delete Actions.front();
		Actions.pop();
	}	
	return 0;
}

mvceditor::ActionClass* mvceditor::ThreadActionClass::NextAction() {
	mvceditor::ActionClass* action = NULL;
	wxMutexLocker locker(ActionsMutex);
	if (!Actions.empty()) {
		action = Actions.front();
	}
	return action;
}

void mvceditor::ThreadActionClass::ActionComplete() {
	wxMutexLocker locker(ActionsMutex);
	if (!Actions.empty()) {
		mvceditor::ActionClass* action = Actions.front();
		action->SignalEnd();
		delete action;
		Actions.pop();
	}
}
  
mvceditor::RunningThreadsClass::RunningThreadsClass(bool doPostEvents)
	: Actions()
	, ActionMutex()
	, Handlers()
	, HandlerMutex()
	, Semaphore(0, 1)
	, DoPostEvents(doPostEvents) {
	ThreadAction = NULL;
  }
  
void mvceditor::RunningThreadsClass::Add(mvceditor::ActionClass* action) {
	wxMutexLocker locker(ActionMutex);
	Actions.push(action);
	if (NULL == ThreadAction) {
		
		ThreadAction = new mvceditor::ThreadActionClass(Actions, ActionMutex, Semaphore);
		wxThreadError error = wxTHREAD_NO_ERROR;
		error = ThreadAction->Create();
		wxASSERT_MSG(error == wxTHREAD_NO_ERROR, wxT("Thread could not be started"));
		if (error == wxTHREAD_NO_ERROR) {
			ThreadAction->Run();
		}
	}
}
/*
void mvceditor::RunningThreadsClass::Remove(mvceditor::ThreadWithHeartbeatClass* thread) {
	{
		wxMutexLocker locker(Mutex);
		wxASSERT(locker.IsOk());
		std::vector<mvceditor::ThreadWithHeartbeatClass*>::iterator it = Workers.begin();
		while (it != Workers.end()) {
			if (*it == thread) {
  				
				// untrack only 
				it = Workers.erase(it);
  			}
			else {
				it++;
			}
		}
	}
	if (Semaphore) {
		Semaphore->Post();
	}
}
*/

void mvceditor::RunningThreadsClass::StopAll() {

	{
		// we tell the thread to stop running but we can also
		// release the mutex while we wait for the finish semaphore
		// to be set
		// this is an important lock because it ensures that
		// the front action is not deleted while we try to call Cancel on it
		wxMutexLocker locker(ActionMutex);
		wxASSERT(locker.IsOk());

		// if there is nothing in the queue nothing is running
		if (!Actions.empty()) {

			// if there is an action that is running then stop it
			mvceditor::ActionClass* action = Actions.front();
			action->Cancel();
		}
	}
	if (ThreadAction) {

		// this tells the background thread to not run anymore
		// actions in the queue
		// make sure that this call is NOT in the mutex locker because
		// in windows since all threads are joinable Delete() causes a deadlock
		// THreadAction is a wxThread it will get cleaned up automatically
		ThreadAction->Delete();
		wxSemaError err = Semaphore.WaitTimeout(4000);
		wxASSERT_MSG(wxSEMA_INVALID != err, wxT("semaphore is invalid"));
		wxASSERT_MSG(wxSEMA_TIMEOUT != err, wxT("semaphore timed out"));
		wxASSERT_MSG(wxSEMA_MISC_ERROR != err, wxT("semaphore misc error"));
		ThreadAction = NULL;
	}
}

void mvceditor::RunningThreadsClass::StopCurrent() {
	// this is an important lock because it ensures that
	// the front action is not deleted while we try to call Cancel on it
	wxMutexLocker locker(ActionMutex);
	wxASSERT(locker.IsOk());

	// if there is nothing in the queue nothing to cancel
	if (!Actions.empty()) {

		// if there is an action that is running then stop it
		// note that ThreadAction will delete the pointer once
		// the thread dies.  we cannot delete it here because the thread
		// is still running and delete it will cause crashes (invalid memory
		// accesses)
		mvceditor::ActionClass* action = Actions.front();
		action->Cancel();
	}
}

/*
void mvceditor::RunningThreadsClass::Stop(wxThreadIdType threadId) {
	mvceditor::ThreadWithHeartbeatClass* thread = NULL;
	{
		wxMutexLocker locker(Mutex);
		wxASSERT(locker.IsOk());
		std::vector<mvceditor::ThreadWithHeartbeatClass*>::iterator it;
		for (it = Workers.begin(); it != Workers.end(); ++it) {
			if ((*it)->GetId() == threadId) {
				thread = *it;
				it = Workers.erase(it);
				break;
			}
		}
		if (thread) {
			Semaphore = new wxSemaphore(0, 1);
		}
	}
	if (thread && Semaphore) {
		// Delete will gracefully delete, which will endup calling
		// the Remove() method
		// also, thread pointer will delete itself
		// we must do this outside of the mutex because in windows
		// detached threads are really just joinable threads and the wxThread::Delete()
		// call blocks until the thread terminates; but if we call Delete
		// inside the mutex we produce a deadlock since Remove() tries to lock
		// the mutex too
		thread->Cancel();
		thread->Delete();

		Semaphore->Wait();
		delete Semaphore;
		Semaphore = NULL;
	}
}
*/

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

/*
bool mvceditor::RunningThreadsClass::IsRunning(mvceditor::ThreadWithHeartbeatClass* thread) {
	bool found = false;
	wxMutexLocker locker(Mutex);
	wxASSERT(locker.IsOk());
	std::vector<mvceditor::ThreadWithHeartbeatClass*>::iterator it = Workers.begin();
	while (it != Workers.end()) {
		if (*it == thread) {
			found = true;
			break;
		}
		++it;
	}
	return found;
} */

const wxEventType mvceditor::EVENT_ACTION_STATUS = wxNewEventType();
const wxEventType mvceditor::EVENT_WORK_COMPLETE = wxNewEventType();
const wxEventType mvceditor::EVENT_WORK_IN_PROGRESS = wxNewEventType();