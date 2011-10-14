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
#ifndef __PROCESSWITHHEARTBEATCLASS_H__
#define __PROCESSWITHHEARTBEATCLASS_H__

#include <wx/event.h>
#include <wx/timer.h>
#include <wx/process.h>
#include <map>

namespace mvceditor {

	/**
	 * This event will be generated when the external process has completed its job
	 * **successfully** A process that has been killed will NOT generate this event.
	 */
	extern const wxEventType EVENT_PROCESS_COMPLETE;

	/**
	 * This event will be generated when the external process was submitted (started)
	 * **successfully** but failed while executing (returned a non-zero exit code). A 
	 * process that has been killed will NOT generate this event.
	 */
	extern const wxEventType EVENT_PROCESS_FAILED;

	/**
	 * This event will be generated when the process is in action. Event listeners
	 * can do things like update status bars here.
	 * event.GetString() may have a message describing the action being taken
	 */
	extern const wxEventType EVENT_PROCESS_IN_PROGRESS;

/**
 * This class helps with running one external process 
 * asynchronously.  It will take care to delete process
 * data structures when it completes. It will signal proces
 * completion by generating an EVENT_PROCESS_COMPLETE event.
 */
class ProcessWithHeartbeatClass : public wxEvtHandler {

public:

	/**
	 * @param handler will receive the EVENT_PROCESS_* events
	 */
	ProcessWithHeartbeatClass(wxEvtHandler& handler);

	/**
	 * on destruction, clean up all running processes.
	 */
	~ProcessWithHeartbeatClass();

	/**
	 * Start a process ASYNCHRONOUSLY
	 *
	 * @param command the command to start (with arguments as well)
	 * @param int commandId command ID will be used when an EVENT_PROCESS_* is genereated
	 *        this way the caller can correlate a command to an event.
	 * @return bool TRUE if the command was started successfully. if FALSE then 
	 *         command is invalid or command is not found.
	 */
	bool Init(const wxString& command, int commandId);

private:

	/**
	 * On process termination clean up any process data structures.
	 */
	void OnProcessEnded(wxProcessEvent& event);

	/**
	 * Get the output from the process' output stream.
	 */
	wxString GetProcessOutput(wxProcess* proc) const;

	/**
	 * the processes that are alive.
	 * key is the PID, value is the wxProcess instance for that PID.
	 */
	std::map<long, wxProcess*> RunningProcesses;
	
	wxTimer Timer;
	
	wxEvtHandler& Handler;

	DECLARE_EVENT_TABLE()
};

}

#endif