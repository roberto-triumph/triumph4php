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
#ifndef __THREADWITHHEARTBEATCLASS_H__
#define __THREADWITHHEARTBEATCLASS_H__

#include <wx/thread.h>
#include <wx/timer.h>


namespace mvceditor {
	
/**
 * This event will be generated when the thread has completed its job
 * **successfully** A thread that has been killed will NOT generate this event.
 */
extern const wxEventType EVENT_WORK_COMPLETE;
/**
 * This event will be generated when the thread is in action. Event listeners
 * can do things like update status bars here.
 * event.GetString() may have a message describing the action being taken
 */
extern const wxEventType EVENT_WORK_IN_PROGRESS;

/**
 * This is a thread that announces itself that is still running and when it ends.
 * This class is useful when displaying the status and percent completion to the
 * user. For now heartbeats will be at a constant rate.
 */
class ThreadWithHeartbeatClass : public wxEvtHandler, public wxThreadHelper {

public:
	
	/**
	 * @param handler will receive the EVENT_WORK_* events
	 * @param id if given, the generated event will have this id as its GetId() member.
	 */
	ThreadWithHeartbeatClass(wxEvtHandler& handler, int id = wxID_ANY);
	
	/**
	 * Will prepare to send events at regular intervals. After a call to this method, a 
	 * EVENT_WORK_IN_PROGRESS will be generated at regular intervals until SignalEnd()
	 * is called.
	 */
	void SignalStart();
	
	/**
	 * Will generate a EVENT_WORK_COMPLETE event and stop the EVENT_WORK_IN_PROGRESS events.
	 */
	void SignalEnd();
	
	/**
	 * Will generate a EVENT_WORK_IN_PROGRESS event
	 */
	void OnTimer(wxTimerEvent& event);
	
protected:

	wxEvtHandler& Handler;
	
private:

	wxTimer Timer;

	/**
	 * All generated events will have this ID as their EventId
	 */
	int EventId;
	
	DECLARE_EVENT_TABLE()

};

}

#endif