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
#include <features/DebuggerFeatureClass.h>
#include <globals/Errors.h>
#include <globals/Assets.h>
#include <Triumph.h>
#include <istream>
#include <string>
#include <algorithm>

static int ID_PANEL_DEBUGGER = wxNewId();
static int ID_ACTION_DEBUGGER = wxNewId();
static int ID_PANEL_DEBUGGER_STACK = wxNewId();

/**
 * reads a response from the debug engine. engine responses are
 * in the following format:
 *
 * data_length
 * [NULL]
 * <?xml version="1.0" encoding="UTF-8"?>
 * <response command="command_name"
 *         transaction_id="transaction_id"/>
 * [NULL]
 *
 * where data_length is the numeric string of the size of the xml response
 * 
 * @param socket the socket to read
 * @param streamBuffer buffer to use
 * @param [out] error any error code will be set here if there is an error
 * @return wxString the XML response only 
 */
static std::string ReadResponse(boost::asio::ip::tcp::socket& socket, boost::asio::streambuf& streamBuffer, boost::system::error_code& error) {
	std::string contents;
	int read = boost::asio::read_until(socket, streamBuffer, '\0', error);
	if (read <= 0) {
		return contents;
	}	

	// extract the xml size
	std::istream is(&streamBuffer);
	std::getline(is, contents, '\0');

	// this is our (triumph's) message that we should no longer accept
	// connections. we do it this way because we use synchronous sockets
	// which we cannot stop from another thread.
	if (contents == "close") {
		return contents;
	}

	// now read the xml payload
	read = boost::asio::read_until(socket, streamBuffer, '\0', error);
	if (read <= 0) {
		return contents;
	}	
	std::getline(is, contents, '\0');
	return contents;
}

// xdebug returns files in form
// file://{system name}/c:/wamp/www/index.php
// 
// we remove the file:/// from the name
static wxFileName ToLocalFilename(wxString xdebugFile) {
	wxFileName name(xdebugFile.Mid(8)); // 8  = size of "file:///"
	name.Normalize();
	return name;
}

/**
t4p::DebuggerCmdBreakpointSetClass::DebuggerCmdBreakpointSetClass(int id,
																  wxEventType type, 
																  const std::vector<t4p::DbgpBreakpointClass>& breakpoints,
																  t4p::DbgpBreakpointClass currentBreakpoint) 
: wxEvent(id, type)
, AllBreakpoints(breakpoints) 
, CurrentBreakpoint(currentBreakpoint) {
	
}

wxEvent* t4p::DebuggerCmdBreakpointSetClass::Clone() const {
	return new t4p::DebuggerCmdBreakpointSetClass(GetId(), GetEventType(), AllBreakpoints, CurrentBreakpoint);
}
*/

t4p::DebuggerServerActionClass::DebuggerServerActionClass(
	t4p::RunningThreadsClass& runningThreads, int eventId, t4p::EventSinkLockerClass& eventSinkLocker)
: wxEvtHandler()
, ActionClass(runningThreads, eventId) 
, Commands()
, CommandMutex()
, IoService()
, EventSinkLocker(eventSinkLocker)
, Port() {
	EventSinkLocker.PushHandler(this);
	Connect(t4p::EVENT_DEBUGGER_CMD, wxThreadEventHandler(t4p::DebuggerServerActionClass::OnCmd));
}

t4p::DebuggerServerActionClass::~DebuggerServerActionClass() {
	Disconnect(t4p::EVENT_DEBUGGER_CMD, wxThreadEventHandler(t4p::DebuggerServerActionClass::OnCmd));
}

void t4p::DebuggerServerActionClass::Init(int port) {
	Port = port;
}

void t4p::DebuggerServerActionClass::AddCommand(std::string cmd) {
	wxMutexLocker locker(CommandMutex);
	Commands.push(cmd);
}

std::string t4p::DebuggerServerActionClass::NextCommand() {
	std::string cmd;
	wxMutexLocker locker(CommandMutex);
	if (Commands.empty()) {
		return cmd;
	}
	cmd = Commands.front();
	Commands.pop();
	return cmd;
}

void t4p::DebuggerServerActionClass::BackgroundWork() {
	boost::asio::ip::tcp::acceptor acceptor(IoService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), Port));	
	while (!IsCancelled()) {
		try {
			boost::asio::ip::tcp::socket socket(IoService);
			acceptor.accept(socket);

			boost::system::error_code readError;
			boost::asio::streambuf streamBuffer;
			std::string response = ReadResponse(socket, streamBuffer, readError);
			if (readError != boost::system::errc::success) {
				Log("read error", readError.message());
				break;
			}

			// this is our (triumph's) message that we should no longer accept
			// connections. we do it this way because we use synchrnous sockets
			// which we cannot stop from another thread.
			if (response == "close") {
				Log("close", "shutting down io service");
				break;
			}
			
			// xdebug responses
			bool isDebuggerStopped = false;
			ParseAndPost(response, "init ", isDebuggerStopped);
			Log("response", response);
			if (!isDebuggerStopped) {
				SessionWork(socket);
			}
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	// no longer need to listen for commands
	EventSinkLocker.RemoveHandler(this);
}

void t4p::DebuggerServerActionClass::SessionWork(boost::asio::ip::tcp::socket& socket) {
	boost::system::error_code readError;
	boost::system::error_code writeError;
	boost::asio::streambuf streamBuffer;

	// send the next command and read the debugger engine's response
	std::string next;
	bool done = false;
	bool isDebuggerStopped = false;
	while (!done && !isDebuggerStopped && !IsCancelled()) {
		next = NextCommand();
		if (!next.empty()) {
			Log("command", next);

			// +1 == send the null bcoz the dbgp protocol says so
			int written = boost::asio::write(socket, boost::asio::buffer(next.c_str(), next.length() + 1), writeError);

			if (writeError != boost::system::errc::success) {
				Log("write error", writeError.message());
				done = true;
				break;
			}

			std::string response = ReadResponse(socket, streamBuffer, readError);
			if (readError != boost::system::errc::success) {
				Log("read error", readError.message());
				done = true;
				break;
			}
			
			// xdebug xml
			Log("response", response);
			ParseAndPost(response, next, isDebuggerStopped);
		}
		else {
			wxThread::Sleep(150);
		}
		if (isDebuggerStopped) {
			
			// remove all commands as they will no longer be run
			// so that commands don't carry over script runs
			next = NextCommand();
			while (!next.empty()) {
				next = NextCommand();
			}
		}
	}
}

void t4p::DebuggerServerActionClass::ParseAndPost(const wxString& xml, const std::string& cmd, bool& isDebuggerStopped) {
	size_t spacePos = cmd.find(" ");
	std::string cmdOnly = std::string::npos == spacePos ? cmd : cmd.substr(0, spacePos);
	std::transform(cmdOnly.begin(), cmdOnly.end(), cmdOnly.begin(), std::tolower);
	bool parseError = false;
	t4p::DbgpXmlErrors xmlError = t4p::DBGP_XML_ERROR_NONE;
	if ("init" == cmdOnly) {
		t4p::DbgpInitEventClass initResponse;
		if (initResponse.FromXml(xml, xmlError)) {
			PostEvent(initResponse);
		}
	}
	else if ("status" == cmdOnly) {
		t4p::DbgpStatusEventClass statusResponse;
		if (statusResponse.FromXml(xml, xmlError)) {
			PostEvent(statusResponse);
		}
	}
	else if ("feature_get" == cmdOnly) {
		t4p::DbgpFeatureGetEventClass featureGetResponse;
		if (featureGetResponse.FromXml(xml, xmlError)) {
			PostEvent(featureGetResponse);
		}
	}
	else if ("feature_set" == cmdOnly) {
		t4p::DbgpFeatureSetEventClass featureSetResponse;
		if (featureSetResponse.FromXml(xml, xmlError)) {
			PostEvent(featureSetResponse);
		}
	}
	else if ("run" == cmdOnly || "step_into" == cmdOnly || 
			"step_over" == cmdOnly || "step_out" == cmdOnly || "stop" == cmdOnly) {
		t4p::DbgpContinueEventClass continueResponse;
		if (continueResponse.FromXml(xml, xmlError)) {
			PostEvent(continueResponse);
			isDebuggerStopped = t4p::DBGP_STATUS_STOPPING == continueResponse.Status;
		}
	}
	else if ("breakpoint_set" == cmdOnly) {
		t4p::DbgpBreakpointSetEventClass breakpointSetResponse;
		if (breakpointSetResponse.FromXml(xml, xmlError)) {
			PostEvent(breakpointSetResponse);
		}
	}
	else if ("breakpoint_get" == cmdOnly) {
		t4p::DbgpBreakpointGetEventClass breakpointGetResponse;
		if (breakpointGetResponse.FromXml(xml, xmlError)) {
			PostEvent(breakpointGetResponse);
		}
	}
	else if ("breakpoint_update" == cmdOnly) {
		t4p::DbgpBreakpointUpdateEventClass breakpointUpdateResponse;
		if (breakpointUpdateResponse.FromXml(xml, xmlError)) {
			PostEvent(breakpointUpdateResponse);
		}
	}
	else if ("breakpoint_remove" == cmdOnly) {
		t4p::DbgpBreakpointRemoveEventClass breakpointRemoveResponse;
		if (breakpointRemoveResponse.FromXml(xml, xmlError)) {
			PostEvent(breakpointRemoveResponse);
		}
	}
	else if ("breakpoint_list" == cmdOnly) {
		t4p::DbgpBreakpointListEventClass breakpointListResponse;
		if (breakpointListResponse.FromXml(xml, xmlError)) {
			PostEvent(breakpointListResponse);
		}
	}
	else if ("stack_depth" == cmdOnly) {
		t4p::DbgpStackDepthEventClass stackDepthResponse;
		if (stackDepthResponse.FromXml(xml, xmlError)) {
			PostEvent(stackDepthResponse);
		}
	}
	else if ("stack_get" == cmdOnly) {
		t4p::DbgpStackGetEventClass stackGetResponse;
		if (stackGetResponse.FromXml(xml, xmlError)) {
			PostEvent(stackGetResponse);
		}
	}
	else if ("context_names" == cmdOnly) {
		t4p::DbgpContextNamesEventClass contextNamesResponse;
		if (contextNamesResponse.FromXml(xml, xmlError)) {
			PostEvent(contextNamesResponse);
		}
	}
	else if ("context_get" == cmdOnly) {
		t4p::DbgpContextGetEventClass contextGetResponse;
		if (contextGetResponse.FromXml(xml, xmlError)) {
			PostEvent(contextGetResponse);
		}
	}
	else if ("property_get" == cmdOnly) {
		t4p::DbgpPropertyGetEventClass propertyGetResponse;
		if (propertyGetResponse.FromXml(xml, xmlError)) {
			PostEvent(propertyGetResponse);
		}
	}
	else if ("property_value" == cmdOnly) {

		// property_value is exactly the same as property_get except
		// that it is not bounded in length
		t4p::DbgpPropertyGetEventClass propertyValueResponse;
		if (propertyValueResponse.FromXml(xml, xmlError)) {
			PostEvent(propertyValueResponse);
		}
	}
	else if ("property_set" == cmdOnly) {
		t4p::DbgpPropertySetEventClass propertySetResponse;
		if (propertySetResponse.FromXml(xml, xmlError)) {
			PostEvent(propertySetResponse);
		}
	}
	else if ("break" == cmdOnly) {
		t4p::DbgpBreakEventClass breakResponse;
		if (breakResponse.FromXml(xml, xmlError)) {
			PostEvent(breakResponse);
		}
	}
	else if ("eval" == cmdOnly) {
		t4p::DbgpEvalEventClass evalResponse;
		if (evalResponse.FromXml(xml, xmlError)) {
			PostEvent(evalResponse);
		}
	}

	// if no matches, then maybe the debugger returned an error
	// response
	if (t4p::DBGP_XML_ERROR_NONE != xmlError) {
		t4p::DbgpErrorEventClass errorResponse;
		if (errorResponse.FromXml(xml, xmlError)) {
			PostEvent(errorResponse);
		}
	}
}

void t4p::DebuggerServerActionClass::OnCmd(wxThreadEvent& event) {
	wxString cmd = event.GetString();
	AddCommand(t4p::WxToChar(cmd));
}

wxString t4p::DebuggerServerActionClass::GetLabel() const {
	return wxT("debugger listener");
}

void t4p::DebuggerServerActionClass::Log(const wxString& title, const wxString& msg) {
	wxString toLog;  
	toLog += title;
	toLog += wxT(" - ");
	toLog += msg;

	wxThreadEvent logEvt(t4p::EVENT_DEBUGGER_LOG, GetEventId());
	logEvt.SetString(toLog);
	PostEvent(logEvt);
}

t4p::BreakpointWithHandleClass::BreakpointWithHandleClass()
: Breakpoint()
, Handle(0) {

}

t4p::BreakpointWithHandleClass::BreakpointWithHandleClass(const t4p::BreakpointWithHandleClass& src)
: Breakpoint()
, Handle(0) {
	Copy(src);
}

t4p::BreakpointWithHandleClass& t4p::BreakpointWithHandleClass::operator=(const t4p::BreakpointWithHandleClass& src) {
	Copy(src);
	return *this;
}

void t4p::BreakpointWithHandleClass::Copy(const t4p::BreakpointWithHandleClass& src) {
	Breakpoint = src.Breakpoint;
	Handle = src.Handle;
}

t4p::DebuggerFeatureClass::DebuggerFeatureClass(t4p::AppClass& app)
: FeatureClass(app) 
, RunningThreads() 
, EventSinkLocker() 
, Cmd()
, Breakpoints() 
, IsDebuggerSessionActive(false) {
}

void t4p::DebuggerFeatureClass::AddNewMenu(wxMenuBar* menuBar) {
	wxMenu* menu = new wxMenu();
	menu->AppendCheckItem(t4p::MENU_DEBUGGER + 0, _("Start Listening for Debugger"), 
			_("Opens a server socket to listen for incoming xdebug connections"));
	menu->AppendCheckItem(t4p::MENU_DEBUGGER + 1, _("Break at start"),
		_("When checked, program will halt at the first line"));
	menu->AppendSeparator();
	menu->Append(t4p::MENU_DEBUGGER + 2, _("Step Into\tF11"), 
		_("Run the next command, recursing inside function calls"));
	menu->Append(t4p::MENU_DEBUGGER + 3, _("Step Over\tF10"),
		_("Run the next command, without recursing inside function calls"));
	menu->Append(t4p::MENU_DEBUGGER + 4, _("Step Out\tShift+F11"),
		_("Run until the end of the current function"));
	menu->Append(t4p::MENU_DEBUGGER + 5, _("Continue\tShift+F10"),
		_("Run until the next breakpoint"));
	menu->Append(t4p::MENU_DEBUGGER + 6, _("Continue To Cursor"),
		_("Run until the code reaches the cursor"));

	menuBar->Append(menu, _("Debug"));	
}

void t4p::DebuggerFeatureClass::OnAppReady(wxCommandEvent& event) {
	RunningThreads.SetMaxThreads(1);
	RunningThreads.AddEventHandler(this);

	DebuggerPanel = new t4p::DebuggerPanelClass(GetToolsNotebook(), ID_PANEL_DEBUGGER);
	AddToolsWindow(DebuggerPanel, _("Debugger"));

	t4p::DebuggerServerActionClass* action = new t4p::DebuggerServerActionClass(RunningThreads, ID_ACTION_DEBUGGER, EventSinkLocker);
	action->Init(9000);
	RunningThreads.Queue(action);
}

void t4p::DebuggerFeatureClass::OnDebuggerLog(wxThreadEvent& event) {
	DebuggerPanel->Logger->Append(event.GetString());
}

void t4p::DebuggerFeatureClass::OnAppExit(wxCommandEvent& event) {
	RunningThreads.RemoveEventHandler(this);

	// this is our (triumph's) message telling the listener that we should no longer accept
	// connections. we do it this way because we use synchronous sockets
	// which we cannot stop from another thread.
	boost::asio::io_service service;
	boost::asio::ip::tcp::resolver resolver(service);
	boost::asio::ip::tcp::resolver::query query("localhost", "9000");
	boost::asio::ip::tcp::resolver::iterator endpointIterator = resolver.resolve(query);

	boost::asio::ip::tcp::socket socket(service);
	boost::asio::connect(socket, endpointIterator);
	std::string closeMsg = "close";
	boost::system::error_code writeError;
	boost::asio::write(socket, boost::asio::buffer(closeMsg), writeError);
	socket.close();

	RunningThreads.StopAll();
}

void t4p::DebuggerFeatureClass::OnMarginClick(wxStyledTextEvent& event) {
	wxObject * object = event.GetEventObject();
	wxStyledTextCtrl* ctrl = wxDynamicCast(object, wxStyledTextCtrl);
	if (!ctrl) {
		return;
	}
	if (t4p::CodeControlOptionsClass::MARGIN_MARKERS != event.GetMargin()) {

		// user did not click on the breakpoint margin
		return;
	}
	t4p::CodeControlClass* codeCtrl = (t4p::CodeControlClass*) ctrl;
	
	// insert/remove the breakpoint to the line that was clicked on and not
	// the line where the cursor is
	// +1 because our breakpoint line numbers start at 1 but scintilla
	// line number start at zero
	int lineNumber = ctrl->LineFromPosition(event.GetPosition()) + 1;

	// if the user clicked on a line that already has a breakpoint, then
	// the user wants to remove the breakpoint
	bool found = false;
	std::vector<t4p::BreakpointWithHandleClass>::iterator it = Breakpoints.begin();
	while (it != Breakpoints.end()) {		
		if (it->Breakpoint.LineNumber == lineNumber
			&& it->Breakpoint.Filename == codeCtrl->GetFileName()) {
			found = true;

			// now if the debugger session is active, we want to remove the breakpoint
			// from the debugger immediately.
			if (IsDebuggerSessionActive) {
				std::vector<t4p::DbgpBreakpointClass> allBreakpoints;
				t4p::DbgpBreakpointClass breakpointToRemove = it->Breakpoint;
				PostCmd(
					Cmd.BreakpointRemove(breakpointToRemove.BreakpointId)
				);
			}
			it = Breakpoints.erase(it);

			codeCtrl->BreakpointRemove(lineNumber);
		}
		else {
			++it;
		}
	}
	
	// if user clicked on a line that does not have a breakpoint, add the 
	// breakpoint 
	if (!found) {
		t4p::BreakpointWithHandleClass breakpointWithHandle;
		breakpointWithHandle.Breakpoint.Filename = codeCtrl->GetFileName();	
		breakpointWithHandle.Breakpoint.LineNumber = lineNumber;
		breakpointWithHandle.Breakpoint.IsEnabled = true;
		bool added = codeCtrl->BreakpointMarkAt(breakpointWithHandle.Breakpoint.LineNumber, breakpointWithHandle.Handle);
		if (added) {
			
			// now if the debugger session is active, we want to tell the
			// debugger of the new breakpoint immediately.
			if (IsDebuggerSessionActive) {
				breakpointWithHandle.DbgpTransactionId = Cmd.CurrentTransactionId();
				PostCmd(
					Cmd.BreakpointFile(
						breakpointWithHandle.Breakpoint.Filename, 
						breakpointWithHandle.Breakpoint.LineNumber, 
						breakpointWithHandle.Breakpoint.IsEnabled
					)
				);
			}

			Breakpoints.push_back(breakpointWithHandle);
		}
	}
}

void t4p::DebuggerFeatureClass::OnStartDebugger(wxCommandEvent& event) {
	// TODO
}

void t4p::DebuggerFeatureClass::OnBreakAtStart(wxCommandEvent& event) {
	// TODO
}

void t4p::DebuggerFeatureClass::OnStepInto(wxCommandEvent& event) {
	PostCmd(
		Cmd.StepInto()	
	);

	// post the stack get command so that the debugger tells us which
	// line is being executed next
	PostCmd(
		Cmd.StackGet(0)	
	);
}

void t4p::DebuggerFeatureClass::OnStepOver(wxCommandEvent& event) {
	PostCmd(
		Cmd.StepOver()	
	);

	// post the stack get command so that the debugger tells us which
	// line is being executed next
	PostCmd(
		Cmd.StackGet(0)	
	);
}

void t4p::DebuggerFeatureClass::OnStepOut(wxCommandEvent& event) {
	PostCmd(
		Cmd.StepOut()	
	);

	// post the stack get command so that the debugger tells us which
	// line is being executed next
	PostCmd(
		Cmd.StackGet(0)	
	);
}

void t4p::DebuggerFeatureClass::OnContinue(wxCommandEvent& event) {
	PostCmd(
		Cmd.Run()	
	);

	// post the stack get command so that the debugger tells us which
	// line is being executed next
	PostCmd(
		Cmd.StackGet(0)	
	);
}

void t4p::DebuggerFeatureClass::OnContinueToCursor(wxCommandEvent& event) {
	t4p::CodeControlClass* codeCtrl = GetCurrentCodeControl();
	if (!codeCtrl) {
		return;
	}
	if (codeCtrl->IsNew()) {
		return;
	}
	wxString filename = codeCtrl->GetFileName();

	// scintilla lines are 0-based, xdebug lines are 1-based
	int lineNumber = codeCtrl->LineFromPosition(codeCtrl->GetCurrentPos()) + 1;

	PostCmd(
		Cmd.BreakpointRunToCursor(filename, lineNumber)
	);

	// post the stack get command so that the debugger tells us which
	// line is being executed next
	PostCmd(
		Cmd.StackGet(0)	
	);
}

void t4p::DebuggerFeatureClass::OnDbgpInit(t4p::DbgpInitEventClass& event) {

	// when starting a debugging session, set the breakpoints that the user
	// has added and tell the debugger to run the program
	// there is no "current" breakppoint at the start of the debug session
	t4p::DbgpBreakpointClass breakpoint;
	std::vector<t4p::BreakpointWithHandleClass>::iterator it = Breakpoints.begin();
	for (; it != Breakpoints.end(); ++it) {
		
		// save the transaction Id, we will need it so that we can match up
		// the breakpoint to the breakpoint Id that xdebug returns in the
		// breakpoint_set response
		it->DbgpTransactionId = Cmd.CurrentTransactionId();
		PostCmd(
			Cmd.BreakpointFile(
				it->Breakpoint.Filename, 
				it->Breakpoint.LineNumber, 
				it->Breakpoint.IsEnabled
			)
		);
	}

	PostCmd(Cmd.Run());
	PostCmd(Cmd.ContextNames(0));
	PostCmd(Cmd.ContextGet(0, 0));
	PostCmd(Cmd.StackGet(0));

	IsDebuggerSessionActive = true;
}

void t4p::DebuggerFeatureClass::OnDbgpError(t4p::DbgpErrorEventClass& event) {
	IsDebuggerSessionActive = false;
}

void t4p::DebuggerFeatureClass::OnDbgpStatus(t4p::DbgpStatusEventClass& event) {

}

void t4p::DebuggerFeatureClass::OnDbgpFeatureGet(t4p::DbgpFeatureGetEventClass& event) {

}

void t4p::DebuggerFeatureClass::OnDbgpFeatureSet(t4p::DbgpFeatureSetEventClass& event) {

}

void t4p::DebuggerFeatureClass::OnDbgpContinue(t4p::DbgpContinueEventClass& event) {
	IsDebuggerSessionActive = t4p::DBGP_STATUS_STARTING == event.Status 
		|| t4p::DBGP_STATUS_RUNNING == event.Status
		|| t4p::DBGP_STATUS_BREAK == event.Status;
}

void t4p::DebuggerFeatureClass::OnDbgpBreakpointSet(t4p::DbgpBreakpointSetEventClass& event) {
	
	// when the debug engine signals that a breakpoint was set, we need to store
	// the debug engine's breakpoint ID because the ID is needed in case the user
	// wants to remove the breakpoint.
	for (size_t i = 0; i < Breakpoints.size(); ++i) {
		if (Breakpoints[i].DbgpTransactionId == event.TransactionId) {
			Breakpoints[i].Breakpoint.BreakpointId = event.BreakpointId;
		}
	}
}

void t4p::DebuggerFeatureClass::OnDbgpBreakpointGet(t4p::DbgpBreakpointGetEventClass& event) {

}

void t4p::DebuggerFeatureClass::OnDbgpBreakpointUpdate(t4p::DbgpBreakpointUpdateEventClass& event) {

}
void t4p::DebuggerFeatureClass::OnDbgpBreakpointRemove(t4p::DbgpBreakpointRemoveEventClass& event) {

}

void t4p::DebuggerFeatureClass::OnDbgpBreakpointList(t4p::DbgpBreakpointListEventClass& event) {

}

void t4p::DebuggerFeatureClass::OnDbgpStackDepth(t4p::DbgpStackDepthEventClass& event) {

}

void t4p::DebuggerFeatureClass::OnDbgpStackGet(t4p::DbgpStackGetEventClass& event) {
	if (event.Stack.empty()) {
		return;
	}

	// in this method we will open the file at which execution has 
	// stopped

	// turn the filename that Xdebug returns into a local filesystem filename
	wxFileName currentFilename = ToLocalFilename(event.Stack[0].Filename);
	t4p::OpenFileCommandEventClass openEvt(
		currentFilename.GetFullPath(), -1, -1,
		event.Stack[0].LineNumber
	);
	App.EventSink.Publish(openEvt);
	t4p::CodeControlClass* ctrl = GetCurrentCodeControl();
	if (ctrl) {

		// if the file was successfully opened then mark
		// the current line where execution stopped.
		ctrl->ExecutionMarkAt(event.Stack[0].LineNumber);
	}

	wxWindow* window = FindOutlineWindow(ID_PANEL_DEBUGGER_STACK);
	t4p::DebuggerStackPanelClass* stackPanel = NULL;
	if (window) {
		stackPanel = (t4p::DebuggerStackPanelClass*) window;
	}
	else {
		stackPanel = new t4p::DebuggerStackPanelClass(GetOutlineNotebook(), ID_PANEL_DEBUGGER_STACK);
		AddOutlineWindow(stackPanel, _("Stack"));
	}
	stackPanel->ShowStack(event.Stack);
}

void t4p::DebuggerFeatureClass::OnDbgpContextNames(t4p::DbgpContextNamesEventClass& event) {
	
}

void t4p::DebuggerFeatureClass::OnDbgpContextGet(t4p::DbgpContextGetEventClass& event) {
	
}

void t4p::DebuggerFeatureClass::OnDbgpPropertyGet(t4p::DbgpPropertyGetEventClass& event) {

}

void t4p::DebuggerFeatureClass::OnDbgpPropertyValue(t4p::DbgpPropertyValueEventClass& event) {

}

void t4p::DebuggerFeatureClass::OnDbgpPropertySet(t4p::DbgpPropertySetEventClass& event) {

}

void t4p::DebuggerFeatureClass::OnDbgpBreak(t4p::DbgpBreakEventClass& event) {

}

void t4p::DebuggerFeatureClass::OnDbgpEval(t4p::DbgpEvalEventClass& event) {

}

void t4p::DebuggerFeatureClass::PostCmd(std::string cmd) {
	wxThreadEvent evt(t4p::EVENT_DEBUGGER_CMD, wxID_ANY);
	evt.SetString(cmd);
	EventSinkLocker.Post(evt);
}

t4p::DebuggerLogPanelClass::DebuggerLogPanelClass(wxWindow* parent)
: DebuggerLogPanelGeneratedClass(parent, wxID_ANY) {
	ClearButton->SetBitmapLabel(t4p::BitmapImageAsset(wxT("eraser")));
}

void t4p::DebuggerLogPanelClass::Append(const wxString& text) {
	Text->AppendText(text);
	Text->AppendText(wxT("\n"));
}

void t4p::DebuggerLogPanelClass::OnClearButton(wxCommandEvent& event) {
	Text->Clear();
}

t4p::DebuggerPanelClass::DebuggerPanelClass(wxWindow* parent, int id)
: DebuggerPanelGeneratedClass(parent, id) {
	Notebook->SetWindowStyle(wxAUI_NB_BOTTOM);

	Logger = new t4p::DebuggerLogPanelClass(this);

	Notebook->AddPage(Logger, _("Logger"));
}

t4p::DebuggerStackPanelClass::DebuggerStackPanelClass(wxWindow* parent, int id)
: DebuggerStackPanelGeneratedClass(parent, id) {
	StackList->DeleteAllItems();
	while (StackList->GetColumnCount() > 0) {
		StackList->DeleteColumn(0);
	}
	StackList->AppendColumn(_("Function"));
	StackList->AppendColumn(_("Line Number"));
	StackList->AppendColumn(_("Filename"));
}

void t4p::DebuggerStackPanelClass::ShowStack(const std::vector<t4p::DbgpStackClass>& stack) {
	StackList->DeleteAllItems();

	std::vector<t4p::DbgpStackClass>::const_iterator it;
	for (it = stack.begin(); it != stack.end(); ++it) {
		int newRowNumber = StackList->GetItemCount();
		wxListItem column1;
		column1.SetColumn(0);
		column1.SetId(newRowNumber);
		column1.SetMask(wxLIST_MASK_TEXT);
		column1.SetText(it->Where);
		StackList->InsertItem(column1);

		wxListItem column2;
		column2.SetColumn(1);
		column2.SetId(newRowNumber);
		column2.SetMask(wxLIST_MASK_TEXT);
		column2.SetText(wxString::Format("%d", it->LineNumber));
		StackList->SetItem(column2);

		wxFileName fileName = ToLocalFilename(it->Filename);
		wxListItem column3;
		column3.SetColumn(2);
		column3.SetId(newRowNumber);
		column3.SetMask(wxLIST_MASK_TEXT);
		column3.SetText(fileName.GetFullPath());
		StackList->SetItem(column3);
	}

	StackList->SetColumnWidth(0, wxLIST_AUTOSIZE);
	StackList->SetColumnWidth(1, wxLIST_AUTOSIZE);
	StackList->SetColumnWidth(2, wxLIST_AUTOSIZE);
}

const wxEventType t4p::EVENT_DEBUGGER_LOG = wxNewEventType();
const wxEventType t4p::EVENT_DEBUGGER_RESPONSE = wxNewEventType();
const wxEventType t4p::EVENT_DEBUGGER_CMD = wxNewEventType();

BEGIN_EVENT_TABLE(t4p::DebuggerFeatureClass, t4p::FeatureClass)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_READY, t4p::DebuggerFeatureClass::OnAppReady)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_EXIT, t4p::DebuggerFeatureClass::OnAppExit)

	EVT_STC_MARGINCLICK(wxID_ANY, t4p::DebuggerFeatureClass::OnMarginClick)
	
	EVT_MENU(t4p::MENU_DEBUGGER + 0, t4p::DebuggerFeatureClass::OnStartDebugger)
	EVT_MENU(t4p::MENU_DEBUGGER + 1, t4p::DebuggerFeatureClass::OnBreakAtStart)
	EVT_MENU(t4p::MENU_DEBUGGER + 2, t4p::DebuggerFeatureClass::OnStepInto)
	EVT_MENU(t4p::MENU_DEBUGGER + 3, t4p::DebuggerFeatureClass::OnStepOver)
	EVT_MENU(t4p::MENU_DEBUGGER + 4, t4p::DebuggerFeatureClass::OnStepOut)
	EVT_MENU(t4p::MENU_DEBUGGER + 5, t4p::DebuggerFeatureClass::OnContinue)
	EVT_MENU(t4p::MENU_DEBUGGER + 6, t4p::DebuggerFeatureClass::OnContinueToCursor)

	EVT_DBGP_INIT(t4p::DebuggerFeatureClass::OnDbgpInit)
	EVT_DBGP_ERROR(t4p::DebuggerFeatureClass::OnDbgpError)
	EVT_DBGP_STATUS(t4p::DebuggerFeatureClass::OnDbgpStatus)
	EVT_DBGP_FEATUREGET(t4p::DebuggerFeatureClass::OnDbgpFeatureGet)
	EVT_DBGP_FEATURESET(t4p::DebuggerFeatureClass::OnDbgpFeatureSet)
	EVT_DBGP_CONTINUE(t4p::DebuggerFeatureClass::OnDbgpContinue)
	EVT_DBGP_BREAKPOINTSET(t4p::DebuggerFeatureClass::OnDbgpBreakpointSet)
	EVT_DBGP_BREAKPOINTGET(t4p::DebuggerFeatureClass::OnDbgpBreakpointGet)
	EVT_DBGP_BREAKPOINTUPDATE(t4p::DebuggerFeatureClass::OnDbgpBreakpointUpdate)
	EVT_DBGP_BREAKPOINTREMOVE(t4p::DebuggerFeatureClass::OnDbgpBreakpointRemove)
	EVT_DBGP_BREAKPOINTLIST(t4p::DebuggerFeatureClass::OnDbgpBreakpointList)
	EVT_DBGP_STACKDEPTH(t4p::DebuggerFeatureClass::OnDbgpStackDepth)
	EVT_DBGP_STACKGET(t4p::DebuggerFeatureClass::OnDbgpStackGet)
	EVT_DBGP_CONTEXTNAMES(t4p::DebuggerFeatureClass::OnDbgpContextNames)
	EVT_DBGP_CONTEXTGET(t4p::DebuggerFeatureClass::OnDbgpContextGet)
	EVT_DBGP_PROPERTYGET(t4p::DebuggerFeatureClass::OnDbgpPropertyGet)
	EVT_DBGP_PROPERTYVALUE(t4p::DebuggerFeatureClass::OnDbgpPropertyValue)
	EVT_DBGP_PROPERTYSET(t4p::DebuggerFeatureClass::OnDbgpPropertySet)
	EVT_DBGP_BREAK(t4p::DebuggerFeatureClass::OnDbgpBreak)
	EVT_DBGP_EVAL(t4p::DebuggerFeatureClass::OnDbgpEval)

	EVT_DEBUGGER_LOG(ID_ACTION_DEBUGGER, t4p::DebuggerFeatureClass::OnDebuggerLog)

END_EVENT_TABLE()

