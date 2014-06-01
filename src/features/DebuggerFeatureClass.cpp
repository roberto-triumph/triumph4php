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
#include <language/DbgpEventClass.h>
#include <istream>
#include <string>

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
 * @param char* buffer . this function will NOT own the pointer
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
	AddCommand(cmd.StepOver());
	AddCommand(cmd.StepOver());
	AddCommand(cmd.StepOver());

	// send the next command and read the debugger engine's resposne
	while (!Commands.empty()) {
		std::string cmd = Commands.front();
		Commands.pop();
		Log("command", cmd);

		// +1 == send the null bcoz the protocol says so
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

	EVT_COMMAND(ID_ACTION_DEBUGGER, t4p::EVENT_DEBUGGER_LOG, t4p::DebuggerFeatureClass::OnDebuggerLog)
END_EVENT_TABLE()

