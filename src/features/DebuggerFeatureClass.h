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
#ifndef __T4P_DEBUGGERFEATURECLASS_H__
#define __T4P_DEBUGGERFEATURECLASS_H__

#include <boost/asio.hpp>
#include <features/FeatureClass.h>
#include <actions/ActionClass.h>
#include <features/wxformbuilder/DebuggerFeatureForms.h>
#include <vector>
#include <queue>

namespace t4p {

// forward declaration, defined below
class DebuggerPanelClass;

extern const wxEventType EVENT_DEBUGGER_LOG;
extern const wxEventType EVENT_DEBUGGER_RESPONSE;

class DebuggerServerActionClass : public t4p::ActionClass {

public:

	DebuggerServerActionClass(t4p::RunningThreadsClass& runningThreads, int eventId);
	
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

	wxString GetLabel() const;

	/**
	 * the loop for each debugger seesion (script)
	 */
	void SessionWork(boost::asio::ip::tcp::socket& socket);

	/**
	 * adds a command to be sent over to the debug engine.
	 */
	void AddCommand(std::string cmd);

	/**
	 * send a log event; this is usually the command we send to xdebug or
	 * the response we get back
	 */
	void Log(const wxString& title, const wxString& msg);

	/**
	 * commands to be sent by triumph to the debugger engine. These strings
	 * are built with DbgpCommandClass. Examples of commands: set a 
	 * breakpoint, get a variable value, get the runtime stack
	 */
	std::queue<std::string> Commands;
	
	/**
	 * the io service listens on the socket for
	 * information from the debugger engine
	 */
	boost::asio::io_service IoService;

	/**
	 * the port number to listen on.  This is the same port
	 * that the user will set on their php.ini file for
	 * "xdebug.remote_port"
	 */
	int Port;
};

/**
 * This feature accepts connections from x-debug.
 */
class DebuggerFeatureClass : public t4p::FeatureClass {

public:

	DebuggerFeatureClass(t4p::AppClass& app);

	void AddNewMenu(wxMenuBar* menuBar);

private:

	void OnAppReady(wxCommandEvent& event);

	void OnAppExit(wxCommandEvent& event);

	void OnDebuggerLog(wxCommandEvent& event);

	t4p::RunningThreadsClass RunningThreads;

	t4p::DebuggerPanelClass* DebuggerPanel;

	DECLARE_EVENT_TABLE()
};

/**
 * shows a log of the data that was sent by the debugger engine xdebug
 */
class DebuggerLogPanelClass : public DebuggerLogPanelGeneratedClass {

public:

	DebuggerLogPanelClass(wxWindow* parent);

	void Append(const wxString& text);

};

/**
 * shows the bulk of the debug information
 */
class DebuggerPanelClass : public DebuggerPanelGeneratedClass {

public:

	t4p::DebuggerLogPanelClass* Logger;

	DebuggerPanelClass(wxWindow* parent, int id);

private:

	
};

}

#endif