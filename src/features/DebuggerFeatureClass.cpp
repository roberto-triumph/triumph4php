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
#include <istream>
#include <string>
#include <algorithm>

static int ID_PANEL_DEBUGGER = wxNewId();
static int ID_ACTION_DEBUGGER = wxNewId();

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
	t4p::RunningThreadsClass& runningThreads, int eventId)
: ActionClass(runningThreads, eventId) 
, IoService() 
, Port() {
}

void t4p::DebuggerServerActionClass::Init(int port) {
	Port = port;
}

void t4p::DebuggerServerActionClass::AddCommand(std::string cmd) {
	Commands.push(cmd);
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
			
			// xdebug xml
			ParseAndPost(response, "init");
			Log("response", response);

			SessionWork(socket);
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}
}

void t4p::DebuggerServerActionClass::SessionWork(boost::asio::ip::tcp::socket& socket) {
	boost::system::error_code readError;
	boost::system::error_code writeError;
	boost::asio::streambuf streamBuffer;

	// when starting a debugging session, set the breakpoints that the user
	// has added and tell the debugger to run the program
	t4p::DbgpCommandClass cmd;
	AddCommand(cmd.BreakpointFile(
		"C:\\Users\\Roberto\\Documents\\php_projects\\CodeIgniter_p1\\public\\index.php", 82, true
	));
	AddCommand(cmd.FeatureGet("supports_async"));
	AddCommand(cmd.FeatureGet("breakpoint_types"));
	AddCommand(cmd.FeatureGet("show_hidden"));
	AddCommand(cmd.Run());
	AddCommand(cmd.ContextNames(0));
	AddCommand(cmd.ContextGet(0, 0));

	// send the next command and read the debugger engine's resposne
	while (!Commands.empty()) {
		std::string cmd = Commands.front();
		Commands.pop();
		Log("command", cmd);

		// +1 == send the null bcoz the dbgp protocol says so
		int written = boost::asio::write(socket, boost::asio::buffer(cmd.c_str(), cmd.length() + 1), writeError);

		if (writeError != boost::system::errc::success) {
			Log("write error", writeError.message());
			break;
		}

		std::string response = ReadResponse(socket, streamBuffer, readError);
		if (readError != boost::system::errc::success) {
			Log("read error", readError.message());
			break;
		}
		
		// xdebug xml
		Log("response", response);
		ParseAndPost(response, cmd);
	}
}

void t4p::DebuggerServerActionClass::ParseAndPost(const wxString& xml, const std::string& cmd) {
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
	else if ("continue" == cmdOnly) {
		t4p::DbgpContinueEventClass continueResponse;
		if (continueResponse.FromXml(xml, xmlError)) {
			PostEvent(continueResponse);
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

wxString t4p::DebuggerServerActionClass::GetLabel() const {
	return wxT("debugger listener");
}

void t4p::DebuggerServerActionClass::Log(const wxString& title, const wxString& msg) {
	wxString toLog;
	toLog += title;
	toLog += wxT(" - ");
	toLog += msg;

	wxCommandEvent logEvt(t4p::EVENT_DEBUGGER_LOG, GetEventId());
	logEvt.SetString(toLog);
	PostEvent(logEvt);
}

t4p::DebuggerFeatureClass::DebuggerFeatureClass(t4p::AppClass& app)
: FeatureClass(app) 
, RunningThreads() {
}

void t4p::DebuggerFeatureClass::AddNewMenu(wxMenuBar* menuBar) {
	
}

void t4p::DebuggerFeatureClass::OnAppReady(wxCommandEvent& event) {
	RunningThreads.SetMaxThreads(1);
	RunningThreads.AddEventHandler(this);

	DebuggerPanel = new t4p::DebuggerPanelClass(GetToolsNotebook(), ID_PANEL_DEBUGGER);
	AddToolsWindow(DebuggerPanel, _("Debugger"));

	t4p::DebuggerServerActionClass* action = new t4p::DebuggerServerActionClass(RunningThreads, ID_ACTION_DEBUGGER);
	action->Init(9000);
	RunningThreads.Queue(action);

}

void t4p::DebuggerFeatureClass::OnDebuggerLog(wxCommandEvent& event) {
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


void t4p::DebuggerFeatureClass::OnDbgpInit(t4p::DbgpInitEventClass& event) {
	wxMessageBox("app id:" + event.AppId, "debugger init");
}

void t4p::DebuggerFeatureClass::OnDbgpError(t4p::DbgpErrorEventClass& event) {

}

void t4p::DebuggerFeatureClass::OnDbgpStatus(t4p::DbgpStatusEventClass& event) {

}

void t4p::DebuggerFeatureClass::OnDbgpFeatureGet(t4p::DbgpFeatureGetEventClass& event) {
	wxMessageBox(wxString::Format("feature=%d status=", event.Feature) + event.Data, "feature get");
}

void t4p::DebuggerFeatureClass::OnDbgpFeatureSet(t4p::DbgpFeatureSetEventClass& event) {

}

void t4p::DebuggerFeatureClass::OnDbgpContinue(t4p::DbgpContinueEventClass& event) {

}

void t4p::DebuggerFeatureClass::OnDbgpBreakpointSet(t4p::DbgpBreakpointSetEventClass& event) {

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

}

void t4p::DebuggerFeatureClass::OnDbgpContextNames(t4p::DbgpContextNamesEventClass& event) {
	wxMessageBox(wxString::Format("names size=%d", event.Names.size()));
}

void t4p::DebuggerFeatureClass::OnDbgpContextGet(t4p::DbgpContextGetEventClass& event) {
	wxMessageBox(wxString::Format("names size=%d", event.Properties.size()));	
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

t4p::DebuggerLogPanelClass::DebuggerLogPanelClass(wxWindow* parent)
: DebuggerLogPanelGeneratedClass(parent, wxID_ANY) {

}

void t4p::DebuggerLogPanelClass::Append(const wxString& text) {
	Text->AppendText(text);
	Text->AppendText(wxT("\n"));
}

t4p::DebuggerPanelClass::DebuggerPanelClass(wxWindow* parent, int id)
: DebuggerPanelGeneratedClass(parent, id) {
	Notebook->SetWindowStyle(wxAUI_NB_BOTTOM);

	Logger = new t4p::DebuggerLogPanelClass(this);

	Notebook->AddPage(Logger, _("Logger"));
}

const wxEventType t4p::EVENT_DEBUGGER_LOG = wxNewEventType();
const wxEventType t4p::EVENT_DEBUGGER_RESPONSE = wxNewEventType();

BEGIN_EVENT_TABLE(t4p::DebuggerFeatureClass, t4p::FeatureClass)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_READY, t4p::DebuggerFeatureClass::OnAppReady)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_EXIT, t4p::DebuggerFeatureClass::OnAppExit)
	
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

	EVT_COMMAND(ID_ACTION_DEBUGGER, t4p::EVENT_DEBUGGER_LOG, t4p::DebuggerFeatureClass::OnDebuggerLog)

END_EVENT_TABLE()

