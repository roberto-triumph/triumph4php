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
#ifndef T4P_DEBUGGERFEATURECLASS_H
#define T4P_DEBUGGERFEATURECLASS_H

#include <features/FeatureClass.h>
#include <actions/ActionClass.h>
#include <language_php/DbgpEventClass.h>
#include <globals/Events.h>
#include <wx/stc/stc.h>
#include <vector>

namespace t4p {
/**
 * event generated by the main thread when the user wants
 * to see the full, untruncated contents of a variable.
 * By default, the debugger shows at most a certain amount
 * so that a big variable won't slow the debugger down.
 * This event is of type wxCommandEvent, GetString()
 * will return a concatenation of the DbgpPropertyClass
 * FullName, newline, Key of the variable to get.
 * GetInt() will return the variable's contextId, 0
 * if its a local variable or 1 if its a global variable.
 */
extern const wxEventType EVENT_DEBUGGER_SHOW_FULL;

extern const wxEventType EVENT_DEBUGGER_BREAKPOINT_REFRESH;
extern const wxEventType EVENT_DEBUGGER_OPEN_PANEL;

/**
 * a small class that holds a debugger breakpoint along with
 * its "handle". A "handle" is an opaque ID that is assigned
 * by STC, that way we can track movements of the breakpoint
 * when code is added or deleted.
 */
class BreakpointWithHandleClass {
	public:
	/**
	 * the line number where the breakpoint is located.
	 */
	t4p::DbgpBreakpointClass Breakpoint;

	/**
	 * A "handle" is an opaque ID that is assigned
	 * by STC, that way we can track movements of the breakpoint
	 * when code is added or deleted.
	 */
	int Handle;

	/**
	 * this is the TransactionID of the command that we sent to
	 * xdebug. Transaction Id is an arbritary string that is unique
	 * to the debug engine. We need to keep track of this because
	 * the xdebug response to breakpoint_set only returns the
	 * transaction Id; we need to save the
	 * breakpoint Id that we get from Xdebug so that we can later
	 * remove or disable the breakpoint if the user wants to.
	 */
	std::string DbgpTransactionId;

	BreakpointWithHandleClass();

	BreakpointWithHandleClass(const t4p::BreakpointWithHandleClass& src);

	t4p::BreakpointWithHandleClass& operator=(const t4p::BreakpointWithHandleClass& src);

	void Copy(const t4p::BreakpointWithHandleClass& src);
};

/**
 * holds parameters that are used to configure the
 * debugger
 */
class DebuggerOptionsClass {
	public:
	/**
	 * the port that triumph will listen on for incoming xdebug
	 * connections
	 */
	int Port;

	/**
	 * The max amount of variable data to initially retrieve
	 * from xdebug. For example, if MaxChildren=10, then at most
	 * 10 array key-value pairs will be sent by xdebug during
	 * the initial load.
	 */
	int MaxChildren;

	/**
	 * The maximum depth that the debugger engine may return when
	 * sending arrays or object structures to the IDE.
	 */
	int MaxDepth;

	/**
	 * TRUE if triumph should start listening for debugs connections
	 * at program start.  By default, the server will be started
	 * on-demand.
	 */
	bool DoListenOnAppReady;

	/**
	 * TRUE if the triumph should tell the debugger to stop
	 * execution at the first line of the script
	 */
	bool DoBreakOnStart;

	/**
	 * Debugger source mappings are useful when your local
	 * copy of your source code is in a different directory
	 * in the web  server. Triumph uses the mappings when
	 * xdebug returns a file path that is not found in the
	 * local file system
	 */
	std::map<wxString, wxString> SourceCodeMappings;

	DebuggerOptionsClass();

	DebuggerOptionsClass(const t4p::DebuggerOptionsClass& opts);

	t4p::DebuggerOptionsClass& operator=(const t4p::DebuggerOptionsClass& opts);

	void Copy(const t4p::DebuggerOptionsClass& opts);
};

/**
 * This feature accepts connections from x-debug.
 */
class DebuggerFeatureClass : public t4p::FeatureClass {
	public:
	/**
	 * the breakpoints that the user has set.
	 */
	std::vector<t4p::BreakpointWithHandleClass> Breakpoints;

	/**
	 * holds the background thread that the server action is running
	 * in. This is public so that the view can register itself
	 * to listen for debugger events.
	 */
	t4p::RunningThreadsClass RunningThreads;

	/**
	 * configuration options for xdebug
	 */
	t4p::DebuggerOptionsClass Options;

	/**
	 * TRUE if the debugger session is currently being handled.  When
	 * the debug session is active, we want to immediately tell
	 * the debug engine when the user adds or removes a breakpoint.
	 */
	bool IsDebuggerSessionActive;

	/**
	 * used to build the xdebug commands. using a class-wide instance
	 * because xdebug commands require a unique "transactionId"
	 */
	t4p::DbgpCommandClass Cmd;

	DebuggerFeatureClass(t4p::AppClass& app);

	void LoadPreferences(wxConfigBase* base);

	/**
	 * issues a debugger command to get the children of the given
	 * property. The command is asynchronous; this method exits
	 * immediately, and the command is queued to be sent to
	 * the debugger over a socket on a background thread.
	 *
	 * See section 7.13 of the xdbgp protocol docs.
	 */
	void CmdPropertyGetChildren(const t4p::DbgpPropertyClass& prop, int contextId);

	/**
	 * Goes to the file and line number of the breakpoint. Opens the file
	 * if the file is not opened.
	 *
	 * @param breakpointWithHandle the breakpoint to go to.
	 */
	void BreakpointGoToSource(const t4p::BreakpointWithHandleClass& breakpointWithHandle);

	/**
	 * send a string of code to be executed by Xdebug in the current
	 * stack and context.
	 *
	 *  The command is asynchronous; this method exits
	 * immediately, and the command is queued to be sent to
	 * the debugger over a socket on a background thread.
	 *
	 * See section 8.3 of the xdbgp protocol docs.
	 *
	 * @param wxString code the code to be sent to Xdebug
	 */
	void CmdEvaluate(const wxString& code);

	/**
	 * Stops and restart the debugger server.  Note that this method
	 * handles port changes (stopping the server at port X and starting
	 * the debugger server at port Y). This is done to handle the case
	 * where the user changes the server port number in the preferences
	 * dialog.
	 *
	 * @param oldPort
	 */
	void RestartDebugger(int oldPort);

	/**
	 * Set the debugger's status as stopped. All this does is
	 * set the IsDebuggerActive flag to false
	 */
	void ResetDebugger();

	/**
	 * coverts the file paths that were returne by xdebug from
	 * remote file paths to local file paths according to the mappings
	 * that are setup by the user. Modifies the given stack in place.
	 *
	 */
	void LocalizeStackFilePaths(std::vector<t4p::DbgpStackClass>& stack);

	/**
	 * Sets the name of the current function and file location that the
	 * debugger is stopped at. We use this so that we can tell when the
	 * debugger has moved on to a new function (in which case we need to
	 * refresh the variables)
	 *
	 * The current scope is the first item in the given stack.
	 */
	void SetCurrentStack(std::vector<t4p::DbgpStackClass>& stack);

	/**
	 * Did the Scope just change?
	 *
	 * @return true if the last 2 calls to SetCurrentStack were NOT in the
	 *         same scope.
	 */
	bool HasScopeChanged() const;

	/**
	 * updates the breakpoint number, this will need to be
	 * done when the user modifies the code control text
	 * contents.
	 * @param fileName the file name used to locate the breakpoint
	 * @param handle the breakpoint's handle (ID)
	 * @param newLineNumber the updated line number of the breakpoint
	 *        after the user's modifications. Line number should be 1-based
	 */
	void UpdateBreakpointLineNumber(const wxString& fileName, int handle, int newLineNumber);

	/**
	 * removes the breakpoint at the given file / line. If
	 * a breakpoint does not exist at the given location, this
	 * method returns false. If a breakpoint already exists at the given
	 * location, it is removed (and this method returns true).
	 * If the breakpoint is removed, the remote xdebug is updated as well.
	 *
	 * @param fileName full path  of the file where the breakpoint is located
	 * @param lineNumber the line that the breakpoint is located at. Line number
	 *        should be 1-based
	 * @return TRUE if an existing breakpoint was found (and removed)
	 */
	bool RemoveBreakpointAtLine(const wxString& fileName, int lineNumber);

	/**
	 * enables a breakpoint at the given file / line. If
	 * a breakpoint does not exist at the given location, it is added (and this
	 * method returns true). If a breakpoint already exists at the given
	 * location, this method returns false. If the breakpoint is added, the
	 * remote xdebug is updated as well.
	 *
	 * @param fileName full path  of the file where the breakpoint is located
	 * @param lineNumber the line that the breakpoint is located at. Line number
	 *        should be 1-based
	 * @return TRUE if an existing breakpoint was not found at the location
	 */
	bool AddBreakpointAtLine(const wxString& fileName, int handle, int lineNumber);


	/**
	 * send a command to the debug engine.  This is an asynchronous
	 * operation; we send the command over to the background thread,
	 * and the background thread sends the command to the debug engine.
	 *
	 * @param cmd the command string to send, built by DbgpCommandClass
	 */
	void PostCmd(std::string cmd);

	/**
	 * store breakpoints to disk.
	 */
	void SaveConfig();

	// handlers for menu items
	void StepInto();
	void StepOver();
	void StepOut();
	void Continue();
	void Finish();
	void GoToExecutingLine();


	/**
	 * starts listening for incoming xdebug connections
	 * @param doOpenDebuggerPanel if TRUE the debugger panel will
	 *        be created as well. The panel will be created only
	 *        if it does not already exist.
	 */
	void StartDebugger(bool doOpenDebuggerPanel);

	/**
	 * stop listening for incoming xdebug connections.
	 * note that we stop the server by sending it a "close" message
	 * since we use synchronous server sockets
	 *
	 * @param int the port that the server is currently listening
	 *        on (to send the server the close message)
	 */
	void StopDebugger(int port);

	private:
	// handlers to begin/stop listening on server
	void OnAppReady(wxCommandEvent& event);
	void OnAppExit(wxCommandEvent& event);
	void OnAppFileOpened(t4p::CodeControlEventClass& event);
	void OnAppFileClosed(t4p::CodeControlEventClass& event);

	/**
	 * handler for saving preferences to disk
	 */
	void OnPreferencesSaved(wxCommandEvent& event);

	// below are the handlers for all responses from the debug engine
	void OnDbgpInit(t4p::DbgpInitEventClass& event);
	void OnDbgpStatus(t4p::DbgpStatusEventClass& event);
	void OnDbgpFeatureGet(t4p::DbgpFeatureGetEventClass& event);
	void OnDbgpFeatureSet(t4p::DbgpFeatureSetEventClass& event);
	void OnDbgpContinue(t4p::DbgpContinueEventClass& event);
	void OnDbgpBreakpointSet(t4p::DbgpBreakpointSetEventClass& event);

	/**
	 * this is an additional debug engine handler, we log a socket
	 * error
	 */
	void OnDebuggerSocketError(wxThreadEvent& event);

	/**
	 * event handler for the show full event, when
	 * the user wants to see the full variable
	 * contents.
	 */
	void OnDebuggerShowFull(wxCommandEvent& event);

	/**
	 * this event sink is used to "pass" debug commands such as step out,
	 * step into and step over, to the background thread.
	 */
	t4p::EventSinkLockerClass EventSinkLocker;

	/**
	 * the name of the last stack that we displayed to
	 * the user.  We use this so that we can know
	 * when we are changing scope and need to re-draw
	 * the variables tree
	 */
	wxString CurrentStackFunction;
	wxString LastStackFunction;

	/**
	 * TRUE if triumph is listening for incoming xdebug
	 * connections
	 */
	bool IsDebuggerServerActive;

	DECLARE_EVENT_TABLE()
};
}

#endif
