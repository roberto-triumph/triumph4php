/**
 * @copyright  2014 Roberto Perpuly
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
#include <actions/DebuggerServerActionClass.h>
#include <globals/String.h>
#include <string>
#include <algorithm>

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
	boost::asio::ip::tcp::acceptor acceptor(IoService);
	try {
		acceptor.open(boost::asio::ip::tcp::v4());
		acceptor.bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), Port));
		boost::asio::ip::tcp::acceptor::reuse_address option(true);
		acceptor.set_option(option);
		acceptor.listen();
	}
	catch (std::exception& e) {
		wxThreadEvent errEvt(t4p::EVENT_DEBUGGER_LISTEN_ERROR, GetEventId());
		errEvt.SetString(e.what());
		PostEvent(errEvt);

		// cannot bind to port, just let the thread die.
		// no longer need to listen for commands
		// we need to this since this object will be deleted after
		// we return from this method.
		EventSinkLocker.RemoveHandler(this);
		return;
	}

	wxThreadEvent startEvt(t4p::EVENT_DEBUGGER_LISTEN, GetEventId());
	startEvt.SetInt(Port);
	PostEvent(startEvt);

	while (!IsCancelled()) {
		try {
			boost::asio::ip::tcp::socket socket(IoService);
			acceptor.accept(socket);

			boost::system::error_code readError;
			boost::asio::streambuf streamBuffer;
			std::string response = ReadResponse(socket, streamBuffer, readError);
			if (readError != boost::system::errc::success) {
				Log("read error", readError.message());
				socket.close();
				break;
			}

			// this is our (triumph's) message that we should no longer accept
			// connections. we do it this way because we use synchrnous sockets
			// which we cannot stop from another thread.
			if (response == "close") {
				Log("close", "shutting down io service");
				socket.close();
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
			wxThreadEvent errEvt(t4p::EVENT_DEBUGGER_SOCKET_ERROR, GetEventId());
			errEvt.SetString(e.what());
			PostEvent(errEvt);
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
			boost::asio::write(socket, boost::asio::buffer(next.c_str(), next.length() + 1), boost::asio::transfer_all(), writeError);

			if (writeError != boost::system::errc::success) {
				Log("write error", writeError.message());
				done = true;
				socket.close();
				break;
			}

			std::string response = ReadResponse(socket, streamBuffer, readError);
			if (readError != boost::system::errc::success) {
				Log("read error", readError.message());
				done = true;
				socket.close();
				break;
			}

			// xdebug xml
			Log("response", response);
			ParseAndPost(response, next, isDebuggerStopped);
		} else {
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
	std::transform(cmdOnly.begin(), cmdOnly.end(), cmdOnly.begin(), ::tolower);
	t4p::DbgpXmlErrors xmlError = t4p::DBGP_XML_ERROR_NONE;
	if ("init" == cmdOnly) {
		t4p::DbgpInitEventClass initResponse;
		if (initResponse.FromXml(xml, xmlError)) {
			PostEvent(initResponse);
		}
	} else if ("status" == cmdOnly) {
		t4p::DbgpStatusEventClass statusResponse;
		if (statusResponse.FromXml(xml, xmlError)) {
			PostEvent(statusResponse);
		}
	} else if ("feature_get" == cmdOnly) {
		t4p::DbgpFeatureGetEventClass featureGetResponse;
		if (featureGetResponse.FromXml(xml, xmlError)) {
			PostEvent(featureGetResponse);
		}
	} else if ("feature_set" == cmdOnly) {
		t4p::DbgpFeatureSetEventClass featureSetResponse;
		if (featureSetResponse.FromXml(xml, xmlError)) {
			PostEvent(featureSetResponse);
		}
	} else if ("run" == cmdOnly || "step_into" == cmdOnly ||
			"step_over" == cmdOnly || "step_out" == cmdOnly || "stop" == cmdOnly) {
		t4p::DbgpContinueEventClass continueResponse;
		if (continueResponse.FromXml(xml, xmlError)) {
			PostEvent(continueResponse);
			isDebuggerStopped = t4p::DBGP_STATUS_STOPPING == continueResponse.Status;
		}
	} else if ("breakpoint_set" == cmdOnly) {
		t4p::DbgpBreakpointSetEventClass breakpointSetResponse;
		if (breakpointSetResponse.FromXml(xml, xmlError)) {
			PostEvent(breakpointSetResponse);
		}
	} else if ("breakpoint_get" == cmdOnly) {
		t4p::DbgpBreakpointGetEventClass breakpointGetResponse;
		if (breakpointGetResponse.FromXml(xml, xmlError)) {
			PostEvent(breakpointGetResponse);
		}
	} else if ("breakpoint_update" == cmdOnly) {
		t4p::DbgpBreakpointUpdateEventClass breakpointUpdateResponse;
		if (breakpointUpdateResponse.FromXml(xml, xmlError)) {
			PostEvent(breakpointUpdateResponse);
		}
	} else if ("breakpoint_remove" == cmdOnly) {
		t4p::DbgpBreakpointRemoveEventClass breakpointRemoveResponse;
		if (breakpointRemoveResponse.FromXml(xml, xmlError)) {
			PostEvent(breakpointRemoveResponse);
		}
	} else if ("breakpoint_list" == cmdOnly) {
		t4p::DbgpBreakpointListEventClass breakpointListResponse;
		if (breakpointListResponse.FromXml(xml, xmlError)) {
			PostEvent(breakpointListResponse);
		}
	} else if ("stack_depth" == cmdOnly) {
		t4p::DbgpStackDepthEventClass stackDepthResponse;
		if (stackDepthResponse.FromXml(xml, xmlError)) {
			PostEvent(stackDepthResponse);
		}
	} else if ("stack_get" == cmdOnly) {
		t4p::DbgpStackGetEventClass stackGetResponse;
		if (stackGetResponse.FromXml(xml, xmlError)) {
			PostEvent(stackGetResponse);
		}
	} else if ("context_names" == cmdOnly) {
		t4p::DbgpContextNamesEventClass contextNamesResponse;
		if (contextNamesResponse.FromXml(xml, xmlError)) {
			PostEvent(contextNamesResponse);
		}
	} else if ("context_get" == cmdOnly) {
		t4p::DbgpContextGetEventClass contextGetResponse;
		if (contextGetResponse.FromXml(xml, xmlError)) {
			PostEvent(contextGetResponse);
		}
	} else if ("property_get" == cmdOnly) {
		t4p::DbgpPropertyGetEventClass propertyGetResponse;
		if (propertyGetResponse.FromXml(xml, xmlError)) {
			PostEvent(propertyGetResponse);
		}
	} else if ("property_value" == cmdOnly) {
		t4p::DbgpPropertyValueEventClass propertyValueResponse;
		if (propertyValueResponse.FromXml(xml, xmlError)) {
			PostEvent(propertyValueResponse);
		}
	} else if ("property_set" == cmdOnly) {
		t4p::DbgpPropertySetEventClass propertySetResponse;
		if (propertySetResponse.FromXml(xml, xmlError)) {
			PostEvent(propertySetResponse);
		}
	} else if ("break" == cmdOnly) {
		t4p::DbgpBreakEventClass breakResponse;
		if (breakResponse.FromXml(xml, xmlError)) {
			PostEvent(breakResponse);
		}
	} else if ("eval" == cmdOnly) {
		t4p::DbgpEvalEventClass evalResponse;
		if (evalResponse.FromXml(xml, xmlError)

			// determine if we actually had an error
			// we look for properties that either
			// dont have a name, value, data type, or children
			// added the children test for windows
			// xdebug version 2.1.0
			&& (!evalResponse.Property.Name.empty() || !evalResponse.Property.Value.empty()
				|| !evalResponse.Property.DataType.empty()
				|| evalResponse.Property.NumChildren)) {
			PostEvent(evalResponse);
		} else {
			// most likely an error response
			t4p::DbgpErrorEventClass errorResponse;
			if (errorResponse.FromXml(xml, xmlError)) {
				PostEvent(errorResponse);
			}
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

const wxEventType t4p::EVENT_DEBUGGER_LOG = wxNewEventType();
const wxEventType t4p::EVENT_DEBUGGER_SOCKET_ERROR = wxNewEventType();
const wxEventType t4p::EVENT_DEBUGGER_LISTEN_ERROR = wxNewEventType();
const wxEventType t4p::EVENT_DEBUGGER_LISTEN = wxNewEventType();
const wxEventType t4p::EVENT_DEBUGGER_RESPONSE = wxNewEventType();
const wxEventType t4p::EVENT_DEBUGGER_CMD = wxNewEventType();
