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
#include <wx/txtstrm.h>

extern const wxEventType t4p::EVENT_PROCESS_COMPLETE = wxNewEventType();

extern const wxEventType t4p::EVENT_PROCESS_FAILED = wxNewEventType();

extern const wxEventType t4p::EVENT_PROCESS_IN_PROGRESS = wxNewEventType();

t4p::ProcessWithHeartbeatClass::ProcessWithHeartbeatClass(wxEvtHandler& handler)
	: wxEvtHandler()
	, RunningProcesses()
	, Timer()
	, Handler(handler) 
	, EventId(0) {
	Timer.SetOwner(this);
}

t4p::ProcessWithHeartbeatClass::~ProcessWithHeartbeatClass() {
	Timer.Stop();
	std::map<long, wxProcess*>::iterator it = RunningProcesses.begin();
	while (it != RunningProcesses.end()) {
		it->second->Detach();
		long pid = it->first;
		wxProcess::Kill(pid, wxSIGILL);
		delete it->second;
		it++;
	}
}

bool t4p::ProcessWithHeartbeatClass::Init(wxString command, const wxFileName& workingDir, 
		int eventId, long& pid) {
	wxProcess* newProcess = new wxProcess(this, eventId);
	EventId = eventId;
	newProcess->Redirect();
	wxExecuteEnv* env = new wxExecuteEnv;
	if (workingDir.IsOk()) {
		env->cwd = workingDir.GetPath();
	}
	
	// dont use newProcess::GetPid(), it seems to always return zero.
	// http://forums.wxwidgets.org/viewtopic.php?t=13559
	pid = wxExecute(command, wxEXEC_ASYNC, newProcess, env);
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

bool t4p::ProcessWithHeartbeatClass::Stop(long pid) {
	Timer.Stop();
	bool stopped = false;	
	std::map<long, wxProcess*>::iterator it = RunningProcesses.find(pid);
	if (it != RunningProcesses.end()) {
		it->second->Detach();

		// in Windows, wxSIGTERM is too nice and the process does not die
		wxKillError killError = wxProcess::Kill(pid, wxSIGKILL); 
		stopped = wxKILL_OK == killError;
		
		/* 
		 * do not delete the process pointer, as the wxWidgets docs for wxProcess states:
		 *
		 *  the object will delete itself upon reception of the process termination notification
		 *
		 * deleting it here would result in double delete (crash)
		 */
		/*delete it->second;*/
		RunningProcesses.erase(it);
	}
	return stopped;
}

void t4p::ProcessWithHeartbeatClass::OnProcessEnded(wxProcessEvent& event) {
	Timer.Stop();
	long pid = event.GetPid();
	std::map<long, wxProcess*>::iterator it = RunningProcesses.find(pid);
	if (event.GetExitCode() == 0 && it != RunningProcesses.end() && it->second) {
		wxProcess* proc = it->second;
		wxString output = GetProcessOutput(proc);
		wxCommandEvent completeEvent(t4p::EVENT_PROCESS_COMPLETE);
		completeEvent.SetId(event.GetId());
		completeEvent.SetString(output);
		wxPostEvent(&Handler, completeEvent);
	}
	else if (it != RunningProcesses.end() && it->second) {
		wxProcess* proc = it->second;
		wxString output = GetProcessOutput(proc);
		wxCommandEvent completeEvent(t4p::EVENT_PROCESS_FAILED);
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

wxString t4p::ProcessWithHeartbeatClass::GetProcessOutput(long pid) const {
	std::map<long, wxProcess*>::const_iterator it = RunningProcesses.find(pid);
	wxString output;
	if (it != RunningProcesses.end()) {
		output = GetProcessOutput(it->second);
	}
	return output;
}

wxString t4p::ProcessWithHeartbeatClass::GetProcessOutput(wxProcess* proc) const {
	wxString allOutput;		
	while (proc->IsInputAvailable()) {
		wxInputStream* stream = proc->GetInputStream();
		wxTextInputStream tis(*stream);
		allOutput << tis.ReadLine();
		allOutput << wxT("\n");
	}
	while (proc->IsErrorAvailable()) {
		wxInputStream* stream = proc->GetErrorStream();
		wxTextInputStream tis(*stream);
		allOutput << tis.ReadLine();
		allOutput << wxT("\n");
	}
	
	return allOutput;
}

void t4p::ProcessWithHeartbeatClass::OnTimer(wxTimerEvent& event) {
	wxCommandEvent intProgressEvent(t4p::EVENT_PROCESS_IN_PROGRESS, EventId);
	wxPostEvent(&Handler, intProgressEvent);
}

BEGIN_EVENT_TABLE(t4p::ProcessWithHeartbeatClass, wxEvtHandler)
	EVT_END_PROCESS(wxID_ANY, t4p::ProcessWithHeartbeatClass::OnProcessEnded)	
	EVT_TIMER(wxID_ANY, t4p::ProcessWithHeartbeatClass::OnTimer)
END_EVENT_TABLE()