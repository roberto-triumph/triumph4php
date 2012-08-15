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
#include <widgets/ThreadWithHeartbeatClass.h>


mvceditor::ThreadWithHeartbeatClass::ThreadWithHeartbeatClass(wxEvtHandler& handler, mvceditor::RunningThreadsClass& runningThreads, 
		int id)
	: wxEvtHandler()
	, wxThread(wxTHREAD_DETACHED)
	, Handler(handler)
	, Timer()
	, RunningThreads(runningThreads)
	, EventId(id) {
	Timer.SetOwner(this);
}

mvceditor::ThreadWithHeartbeatClass::~ThreadWithHeartbeatClass() {
	
}

wxThreadError mvceditor::ThreadWithHeartbeatClass::CreateSingleInstance() {
	wxThreadError error = wxTHREAD_NO_ERROR;
	error = Create();
	if (error == wxTHREAD_NO_ERROR) {
		SignalStart();
		Run();
	}
	return error;
}

void mvceditor::ThreadWithHeartbeatClass::SignalStart() {
	Timer.Start(200, wxTIMER_CONTINUOUS);
}
	
void mvceditor::ThreadWithHeartbeatClass::SignalEnd() {
	Timer.Stop();
	wxCommandEvent evt(mvceditor::EVENT_WORK_COMPLETE, EventId);
	wxPostEvent(&Handler, evt);
}

void mvceditor::ThreadWithHeartbeatClass::OnTimer(wxTimerEvent& event) {
	wxCommandEvent evt(mvceditor::EVENT_WORK_IN_PROGRESS, EventId);
	wxPostEvent(&Handler, evt);
}

void* mvceditor::ThreadWithHeartbeatClass::Entry() {
	RunningThreads.Add(this);
	BackgroundWork();
	
	// by doing this the owner can know when the work has been done
	// since we are using detached threads, the thread delete themselves
	BackgroundCleanup();
	RunningThreads.Remove(this);
	///if (!TestDestroy()) {
		SignalEnd();
	///}
	return 0;
}

void mvceditor::ThreadWithHeartbeatClass::BackgroundCleanup() {
	// nothing here; subclasses can fill this is if they
	// use dynamically allocated memory
}


mvceditor::RunningThreadsClass::RunningThreadsClass()
	: Workers()
	, Mutex() {
		
}

void mvceditor::RunningThreadsClass::Add(wxThread* thread) {
	wxMutexLocker locker(Mutex);
	wxASSERT(locker.IsOk());
	Workers.push_back(thread);
}

void mvceditor::RunningThreadsClass::Remove(wxThread* thread) {
	wxMutexLocker locker(Mutex);
	wxASSERT(locker.IsOk());
	std::vector<wxThread*>::iterator it = Workers.begin();
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

void mvceditor::RunningThreadsClass::RemoveAndStop(wxThread* thread) {

	// no need to synchronize here as we are not touching the 
	// internal vector

	// wxThread::Delete will result in graceful thread termination 
	// will call Remove() which will untrack the thread
	Remove(thread);
	thread->Delete();
}

void mvceditor::RunningThreadsClass::StopAll() {
	std::vector<wxThread*> copy;
	{

		// need to copy the threads so that we can Delete() them
		// Delete() will call Remove() which also needs
		// the mutex and we don't want a deadlock
		wxMutexLocker locker(Mutex);
		wxASSERT(locker.IsOk());
		copy = Workers;
	}
	std::vector<wxThread*>::iterator it;
	for (it = copy.begin(); it != copy.end(); ++it) {
		(*it)->Delete();
	}
}

void mvceditor::RunningThreadsClass::Stop(unsigned long threadId) {
	wxThread* thread = NULL;
	{
		wxMutexLocker locker(Mutex);
		wxASSERT(locker.IsOk());
		std::vector<wxThread*>::iterator it;
		for (it = Workers.begin(); it != Workers.end(); ++it) {
			if ((*it)->GetId() == threadId) {
				thread = *it;
				break;
			}
		}
	}
	if (thread) {

		// Delete will gracefully delete, which will endup calling
		// the Remove() method
		// also, thread pointer will delete itself
		thread->Delete();
	}
}

void mvceditor::RunningThreadsClass::Kill(unsigned long threadId) {
	wxThread* thread = NULL;
	{
		wxMutexLocker locker(Mutex);
		wxASSERT(locker.IsOk());
		std::vector<wxThread*>::iterator it;
		for (it = Workers.begin(); it != Workers.end(); ++it) {
			if ((*it)->GetId() == threadId) {
				thread = *it;

				// different than Stop() method above; since
				// wxThread::Kill won't gracefully terminate the thread
				// Remove() never gets called. we must remove this pointer
				// ourselves
				it = Workers.erase(it);
				break;
			}
		}
	}
	if (thread) {
		thread->Kill();
	}
}

const wxEventType mvceditor::EVENT_WORK_COMPLETE = wxNewEventType();
const wxEventType mvceditor::EVENT_WORK_IN_PROGRESS = wxNewEventType();

BEGIN_EVENT_TABLE(mvceditor::ThreadWithHeartbeatClass, wxEvtHandler)
	EVT_TIMER(wxID_ANY, mvceditor::ThreadWithHeartbeatClass::OnTimer)
END_EVENT_TABLE()