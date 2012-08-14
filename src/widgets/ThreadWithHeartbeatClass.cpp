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
	, wxThread()
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
	else {
		delete this;
	}
	return error;
}

void mvceditor::ThreadWithHeartbeatClass::KillInstance() {
	Timer.Stop();
	
	// RemoveAndStop() will call Delete which will gracefully end
	// the thread and then SignalEnd() will be called; no need
	// to call it here
	RunningThreads.RemoveAndStop(this);
}

void mvceditor::ThreadWithHeartbeatClass::ForceKillInstance() {
	
	// since killing does not clean up, we must clean up
	// ourselves. we need to trigger the complete event so 
	// that the other code can remove their pointers to this
	// object too
	SignalEnd();
	RunningThreads.Remove(this);
	Kill();
	delete this;
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
	RunningThreads.Remove(this);
	if (!TestDestroy()) {
		SignalEnd();
	}
	return 0;
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

const wxEventType mvceditor::EVENT_WORK_COMPLETE = wxNewEventType();
const wxEventType mvceditor::EVENT_WORK_IN_PROGRESS = wxNewEventType();

BEGIN_EVENT_TABLE(mvceditor::ThreadWithHeartbeatClass, wxEvtHandler)
	EVT_TIMER(wxID_ANY, mvceditor::ThreadWithHeartbeatClass::OnTimer)
END_EVENT_TABLE()