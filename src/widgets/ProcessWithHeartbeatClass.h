/**
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
 */
#ifndef SRC_WIDGETS_PROCESSWITHHEARTBEATCLASS_H_
#define SRC_WIDGETS_PROCESSWITHHEARTBEATCLASS_H_

#include <wx/event.h>
#include <wx/filename.h>
#include <wx/process.h>
#include <wx/timer.h>
#include <map>

namespace t4p {
/**
 * This event will be generated when the external process has completed its job
 * **successfully** A process that has been killed will NOT generate this event.
 * event.GetString() will contain the entire process STDOUT and STDERR output; with
 * some caveats. see  t4p::ProcessWithHeartbeatClass::GetProcessOutput(long pid)
 */
extern const wxEventType EVENT_PROCESS_COMPLETE;

/**
 * This event will be generated when the external process was submitted (started)
 * **successfully** but failed while executing (returned a non-zero exit code). A
 * process that has been killed will NOT generate this event.
 * event.GetString() will contain the entire process STDOUT and STDERR output; with
 * some caveats. see  t4p::ProcessWithHeartbeatClass::GetProcessOutput(long pid)
 */
extern const wxEventType EVENT_PROCESS_FAILED;

/**
 * This event will be generated when the process is in action. Event listeners
 * can do things like update status bars here.
 * event.GetString() may have a message describing the action being taken (NOT
 * the process STDOUT / STDERR  output).
 * These events will NOT have an event ID, connect to them with wxID_ANY
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
     * @param workingDirectory the CWD of the command. This param is optional
     *        an empty filename can be passed in.
     * @param int eventId event ID will be used when an EVENT_PROCESS_* is genereated
     *        this way the caller can correlate a command to an event.
     * @param pid the PID of the new process will be set here
     * @return bool TRUE if the command was started successfully. if FALSE then
     *         command is invalid or command is not found.
     */
    bool Init(wxString command, const wxFileName& workingDirectory, int eventId, long& pid);

    /**
     * stop a running process.
     *
     * @param pid the PID to stop.
     */
    bool Stop(long pid);

    /**
     * Get the output from the process' output stream. Note that if you call this while
     * the process is running then the PROCESS_COMPLETE event will NOT have the entire
     * process output.
     *
      *@param long pid the running process ID
     * @return wxString the process STDOUT and STDERR output , so far
     */
    wxString GetProcessOutput(long pid) const;

 private:
    /**
     * On process termination clean up any process data structures.
     */
    void OnProcessEnded(wxProcessEvent& event);

    /**
     * Get the output from the process' STDOUT and STDERR output.
     */
    wxString GetProcessOutput(wxProcess* proc) const;

    /**
     * Timer callback. In this method, we will get the process output and append it to the textbox.
     */
    void OnTimer(wxTimerEvent& event);

    /**
     * the processes that are alive.
     * key is the PID, value is the wxProcess instance for that PID.
     */
    std::map<long, wxProcess*> RunningProcesses;

    /**
     * used to give out constant feedback.
     */
    wxTimer Timer;

    /**
     * Any EVENT_PROCESS_* events are sent to this handler.
     */
    wxEvtHandler& Handler;

    /**
     * All generated events will have this ID
     */
    int EventId;

    /**
     * how often we check the process for new output
     */
    static const int POLL_INTERVAL = 300;


    DECLARE_EVENT_TABLE()
};
}  // namespace t4p

#endif  // SRC_WIDGETS_PROCESSWITHHEARTBEATCLASS_H_
