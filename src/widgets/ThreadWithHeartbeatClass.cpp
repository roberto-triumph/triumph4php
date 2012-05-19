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

mvceditor::WorkerThreadClass::WorkerThreadClass(mvceditor::ThreadWithHeartbeatClass& owner)
	: wxThread()
	, Owner(owner) {
			
}
void* mvceditor::WorkerThreadClass::Entry() {
	Owner.Entry();
	
	// by doing this the owner can know when the work has been done
	// since we are using detached threads, the thread delete themselves
	Owner.Worker = NULL;
	return 0;
}

mvceditor::ThreadWithHeartbeatClass::ThreadWithHeartbeatClass(wxEvtHandler& handler, int id)
	: wxEvtHandler()
	, Handler(handler)
	, Timer()
	, Worker(NULL)
	, EventId(id) {
	Timer.SetOwner(this);
}

mvceditor::ThreadWithHeartbeatClass::~ThreadWithHeartbeatClass() {
	KillInstance();
}

wxThreadError mvceditor::ThreadWithHeartbeatClass::CreateSingleInstance() {
	wxThreadError error = wxTHREAD_NO_ERROR;
	if (IsRunning()) {
		error = wxTHREAD_RUNNING;
	}
	else {
		Worker = new mvceditor::WorkerThreadClass(*this);
		error = Worker->Create();
		if (error == wxTHREAD_NO_ERROR) {
			Worker->Run();
		}
	}
	return error;
}

void mvceditor::ThreadWithHeartbeatClass::KillInstance() {
	if (IsRunning()) {
		Worker->Delete();
		Worker = NULL;
	}
	Timer.Stop();
}

void mvceditor::ThreadWithHeartbeatClass::SignalStart() {
	Timer.Start(200, wxTIMER_CONTINUOUS);
}
	
void mvceditor::ThreadWithHeartbeatClass::SignalEnd() {
	Timer.Stop();
	wxCommandEvent evt(mvceditor::EVENT_WORK_COMPLETE, EventId);
	wxPostEvent(&Handler, evt);
}
	
bool mvceditor::ThreadWithHeartbeatClass::TestDestroy() {
	bool ret = true;
	if (Worker) {
		ret = Worker->TestDestroy();
	}
	return ret;
}

void mvceditor::ThreadWithHeartbeatClass::OnTimer(wxTimerEvent& event) {
	wxCommandEvent evt(mvceditor::EVENT_WORK_IN_PROGRESS, EventId);
	wxPostEvent(&Handler, evt);
}

bool mvceditor::ThreadWithHeartbeatClass::IsRunning() const {
	return Worker != NULL;
}

const wxEventType mvceditor::EVENT_WORK_COMPLETE = wxNewEventType();
const wxEventType mvceditor::EVENT_WORK_IN_PROGRESS = wxNewEventType();

BEGIN_EVENT_TABLE(mvceditor::ThreadWithHeartbeatClass, wxEvtHandler)
	EVT_TIMER(wxID_ANY, mvceditor::ThreadWithHeartbeatClass::OnTimer)
END_EVENT_TABLE()