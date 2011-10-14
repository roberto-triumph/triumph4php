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
	std::map<long, wxProcess*>::iterator it = RunningProcesses.begin();
	while (it != RunningProcesses.end()) {
		it->second->Detach();
		long pid = it->first;
		wxProcess::Kill(pid);
		delete it->second;
		it++;
	}
	Timer.Stop();
}

bool mvceditor::ProcessWithHeartbeatClass::Init(const wxString& command, int commandId) {
	wxProcess* newProcess = new wxProcess(this, commandId);
	newProcess->Redirect();
	long pid = wxExecute(command, wxEXEC_ASYNC, newProcess);
	if (pid != 0) {
		RunningProcesses[pid] = newProcess;
		Timer.Start(200, wxTIMER_CONTINUOUS);
	}
	else {
		delete newProcess;
	}
	return 0 != pid;
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
		delete it->second;
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
		RunningProcesses.erase(it);
	}
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

BEGIN_EVENT_TABLE(mvceditor::ProcessWithHeartbeatClass, wxEvtHandler)
	EVT_END_PROCESS(wxID_ANY, mvceditor::ProcessWithHeartbeatClass::OnProcessEnded)
END_EVENT_TABLE()