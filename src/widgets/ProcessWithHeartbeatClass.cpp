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
#include <widgets/ProcessWithHeartbeatClass.h>

extern const wxEventType mvceditor::EVENT_PROCESS_COMPLETE = wxNewEventType();

extern const wxEventType mvceditor::EVENT_PROCESS_FAILED = wxNewEventType();

extern const wxEventType mvceditor::EVENT_PROCESS_IN_PROGRESS = wxNewEventType();

mvceditor::ProcessWithHeartbeatClass::ProcessWithHeartbeatClass(wxEvtHandler& handler)
	: wxEvtHandler()
	, RunningProcesses()
	, Timer()
	, Handler(handler) {
	Timer.SetOwner(this);
}

mvceditor::ProcessWithHeartbeatClass::~ProcessWithHeartbeatClass() {
	Timer.Stop();
	std::map<long, wxProcess*>::iterator it = RunningProcesses.begin();
	while (it != RunningProcesses.end()) {
		it->second->Detach();
		long pid = it->first;
		wxProcess::Kill(pid);
		delete it->second;
		it++;
	}
}

bool mvceditor::ProcessWithHeartbeatClass::Init(wxString command, int commandId, long& pid) {
	wxProcess* newProcess = new wxProcess(this, commandId);
	newProcess->Redirect();

	// dont use newProcess::GetPid(), it seems to always return zero.
	// http://forums.wxwidgets.org/viewtopic.php?t=13559
	pid = wxExecute(command, wxEXEC_ASYNC, newProcess);
	if (pid != 0) {
		std::map<long, wxProcess*>::iterator it = RunningProcesses.find(pid);
		if (it != RunningProcesses.end()) {
			
			// not sure the new process will get the same PID if another one is running
			// let's cleanup just to be sure
			Stop(pid);
		}
		RunningProcesses[pid] = newProcess;
		Timer.Start(POLL_INTERVAL, wxTIMER_CONTINUOUS);
	}
	else {
		delete newProcess;
	}
	return 0 != pid;
}

bool mvceditor::ProcessWithHeartbeatClass::Stop(long pid) {
	Timer.Stop();
	bool stopped = false;	
	std::map<long, wxProcess*>::iterator it = RunningProcesses.find(pid);
	if (it != RunningProcesses.end()) {
		it->second->Detach();

		// in Windows, wxSIGTERM is too nice and the process does not die
		wxKillError killError = wxProcess::Kill(pid, wxSIGKILL); 
		stopped = wxKILL_OK == killError;
		delete it->second;
		RunningProcesses.erase(it);
	}
	return stopped;
}

void mvceditor::ProcessWithHeartbeatClass::OnProcessEnded(wxProcessEvent& event) {
	Timer.Stop();
	long pid = event.GetPid();
	std::map<long, wxProcess*>::iterator it = RunningProcesses.find(pid);
	if (event.GetExitCode() == 0 && it != RunningProcesses.end() && it->second) {
		wxProcess* proc = it->second;
		wxString output = GetProcessOutput(proc);
		wxCommandEvent completeEvent(mvceditor::EVENT_PROCESS_COMPLETE);
		completeEvent.SetId(event.GetId());
		completeEvent.SetString(output);
		wxPostEvent(&Handler, completeEvent);
	}
	else if (it != RunningProcesses.end() && it->second) {
		wxProcess* proc = it->second;
		wxString output = GetProcessOutput(proc);
		wxCommandEvent completeEvent(mvceditor::EVENT_PROCESS_FAILED);
		completeEvent.SetId(event.GetId());
		completeEvent.SetString(output);
		wxPostEvent(&Handler, completeEvent);
	}
	if (it != RunningProcesses.end()) {

		// since we don't call event.Skip() we are the "parent" and we must delete the process pointer
		// ourselves. See the wxProcess documentation
		delete it->second;
		RunningProcesses.erase(it);
	}
}

wxString mvceditor::ProcessWithHeartbeatClass::GetProcessOutput(long pid) const {
	std::map<long, wxProcess*>::const_iterator it = RunningProcesses.find(pid);
	wxString output;
	if (it != RunningProcesses.end()) {
		output = GetProcessOutput(it->second);
	}
	return output;
}

wxString mvceditor::ProcessWithHeartbeatClass::GetProcessOutput(wxProcess* proc) const {
	wxInputStream* stream = proc->GetInputStream();
	wxString allOutput;
	while (proc->IsInputAvailable()) {
		char ch = stream->GetC();
		if (isprint(ch) || isspace(ch)) {
			allOutput.Append(ch);
		}
	}
	stream = proc->GetErrorStream();
	while (proc->IsErrorAvailable()) {
		char ch = stream->GetC();
		if (isprint(ch) || isspace(ch)) {
			allOutput.Append(ch);
		}
	}
	return allOutput;
}

void mvceditor::ProcessWithHeartbeatClass::OnTimer(wxTimerEvent& event) {
	wxCommandEvent intProgressEvent(mvceditor::EVENT_PROCESS_IN_PROGRESS, wxID_ANY);
	wxPostEvent(&Handler, intProgressEvent);
}

BEGIN_EVENT_TABLE(mvceditor::ProcessWithHeartbeatClass, wxEvtHandler)
	EVT_END_PROCESS(wxID_ANY, mvceditor::ProcessWithHeartbeatClass::OnProcessEnded)	
	EVT_TIMER(wxID_ANY, mvceditor::ProcessWithHeartbeatClass::OnTimer)
END_EVENT_TABLE()