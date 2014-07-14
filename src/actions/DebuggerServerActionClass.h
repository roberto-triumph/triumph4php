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
 * @copyright  2014 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef __T4P_DEBUGGERSERVERACTIONCLASS_H__
#define __T4P_DEBUGGERSERVERACTIONCLASS_H__

#include <actions/ActionClass.h>
#include <language/DbgpEventClass.h>
#include <globals/Events.h>
#include <wx/thread.h>
#include <boost/asio.hpp>
#include <queue>

namespace t4p {

/**
 * event generated by the background thread when it receives
 * a response from the debug engine.  The event will contain
 * the unparsed xml string exactly as the debug engine sent it.
 * The event is of type wxThreadEvent.
 */
extern const wxEventType EVENT_DEBUGGER_LOG;
#define EVT_DEBUGGER_LOG(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DEBUGGER_LOG, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( wxThreadEventFunction, & fn ), (wxObject *) NULL ),

/**
 * event generated by the background thread when it encounters
 * a socket read or write error.  The event will contain
 * an error message.
 * The event is of type wxThreadEvent.
 */
extern const wxEventType EVENT_DEBUGGER_SOCKET_ERROR;
#define EVT_DEBUGGER_SOCKET_ERROR(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DEBUGGER_SOCKET_ERROR, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( wxThreadEventFunction, & fn ), (wxObject *) NULL ),

/**
 * event generated by the background thread when
 * the server socket could not start listening on the specified port.  The event will contain
 * an error message.
 * The event is of type wxThreadEvent.
 */
extern const wxEventType EVENT_DEBUGGER_LISTEN_ERROR;
#define EVT_DEBUGGER_LISTEN_ERROR(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DEBUGGER_LISTEN_ERROR, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( wxThreadEventFunction, & fn ), (wxObject *) NULL ),

/**
 * event generated by the background thread when
 * the server socket has sucessfully started listening on the specified port.  The event will contain
 * the port number that was connected to in GetInt().
 * The event is of type wxThreadEvent.
 */
extern const wxEventType EVENT_DEBUGGER_LISTEN;
#define EVT_DEBUGGER_LISTEN(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_DEBUGGER_LISTEN, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( wxThreadEventFunction, & fn ), (wxObject *) NULL ),

/**
 * event generated by the background thread when it receives
 * a response from the debug engine.  The event will contain
 * the PARSED object. The event is one of the Dbgp*Event
 * classes.
 */
extern const wxEventType EVENT_DEBUGGER_RESPONSE;

/**
 * event generated by the main thread when it receives
 * input from the user. For example, the main thread will
 * send a step-over command when the user wants to step
 * to the next line of code. The event is of type wxThreadEvent.
 */
extern const wxEventType EVENT_DEBUGGER_CMD;

/**
 * The general flow of data goes like this
 *
 * ------------   command line                   command line
 * | Debugger |    =========>       Debugger       ======>   Xdebug
 * | Feature  |    <========       ServerAction    <======  Xdebug
 * ------------   Dbgp*EventClass                  xml
 *
 * The feature sends commands to the action, the action actually
 * contains the opened socket and will send the commands to the
 * Xdebug debugger. The action will also parse the response
 * into the appropriate Dbgp*EventClass instance and Post
 * the results to the feature.
 *
 * the debugger server action listens for connections from the 
 * debug engine (xdebug) in a background thread.  The debugger
 * server action parses the xdebug responses and publishes
 * t4p::DBGP_* events.
 *
 * This action is a bit different from other actions in that the action
 * will also listen for events from the main thread. the main thread
 * will send commands that should be sent over to the debug engine, like
 * step in, step over, step out. A command is nothing more than a string
 * that was put together with DbgpCommandClass.
 *
 * This action is NOT a fire and forget type like most other actions. This
 * action will stay alive for the entire debug sessions (1 script). Cancelling
 * the action is a bit tricky since the action uses a synchronous socket listener
 * that blocks until we get a request.  To cancel this action you will
 * need to connect to the port and send a payload of "close"
 */
class DebuggerServerActionClass : public wxEvtHandler, public t4p::ActionClass {

public:

	DebuggerServerActionClass(t4p::RunningThreadsClass& runningThreads, int eventId, t4p::EventSinkLockerClass& eventSinkLocker);

	~DebuggerServerActionClass();
	
	/**
	 * set the port that will be listened on.  this should be the same as the "xdebug.remote_port"
	 * setting in the user's php.ini.
	 */
	void Init(int port);

protected:

	/**
	 * this method contains the "main loop" of the socket service. we will
	 * continously listen for new responses and send new commands.
	 * we will also listen for a triumph-only special "close" message to
	 * be able to close the socket properly.
	 */
	void BackgroundWork();

	/**
	 * parses the xdebug xml response into the approprivate DbgpEvent class
	 * and posts the event.
	 *
	 * @param xml the xml response from xdebug
	 * @param cmd the command that we sent to xdebug 
	 * @param [out] isDebuggerStopped will be set to TRUE when the debugger responds
	 *        that the script has finished running (ie. "step over" the last line of the script)
	 */
	void ParseAndPost(const wxString& xml, const std::string& cmd, bool& isDebuggerStopped);

	wxString GetLabel() const;

	/**
	 * the loop for each debugger seesion (script)
	 */
	void SessionWork(boost::asio::ip::tcp::socket& socket);

	/**
	 * adds a command to be sent over to the debug engine. adding is
	 * done safely by using the mutex.
	 */
	void AddCommand(std::string cmd);

	/**
	 * @return the next command to send, empty string if there are no more commands
	 *         to send. the command is removed from the queue.
	 *         this method safely removes from the queue by using the mutex.
	 */
	std::string NextCommand();

	/**
	 * send a log event; this is usually the command we send to xdebug or
	 * the response we get back
	 */
	void Log(const wxString& title, const wxString& msg);

	/**
	 * handler of the EVENT_DEBUGGER_CMD event. adds the command
	 * to be sent over to the debug engine after all previously queues
	 * commands have been sent.
	 */
	void OnCmd(wxThreadEvent& event);

	/**
	 * commands to be sent by triumph to the debugger engine. These strings
	 * are built with DbgpCommandClass. Examples of commands: set a 
	 * breakpoint, get a variable value, get the runtime stack
	 */
	std::queue<std::string> Commands;
	
	/**
	 * prevent simulatenous access to commands list
	 */
	wxMutex CommandMutex;

	/**
	 * the io service listens on the socket for
	 * information from the debugger engine
	 */
	boost::asio::io_service IoService;

	/**
	 * we use the event sink to get commands from the foreground thread.
	 * for example, the user clicks the "step out" command the
	 * foreground thread will post a command to this event sink.
	 * the server action will listen for the commands an send them to
	 * the debug engine.
	 */
	t4p::EventSinkLockerClass& EventSinkLocker;

	/**
	 * used to build the xdebug commands. using a class-wide instance
	 * because xdebug commands require a unique "transactionId"
	 */
	t4p::DbgpCommandClass Cmd;

	/**
	 * the port number to listen on.  This is the same port
	 * that the user will set on their php.ini file for
	 * "xdebug.remote_port"
	 */
	int Port;
};

}

#endif