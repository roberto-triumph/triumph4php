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
#include <widgets/DirPickerValidatorClass.h>
#include <widgets/ListWidget.h>
#include <wx/valgen.h>
#include <Triumph.h>
#include <istream>
#include <string>
#include <algorithm>

// TODO: fix variables tree expansion
// TODO: global variables / all contexts
// TODO: expression eval
// TODO: not sure how breakpoints react when
//       file is edited (and breakpoints moves lines) 
//       but file is then reloaded/discarded

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

/**
 * xdebug returns files in form
 * file://{system name}/c:/wamp/www/index.php
 *
 * we remove the file:/// from the name. 
 * 
 * Also, we want to map the name that xdebug returns to a local
 * path.  this is in case the program being debugged remotely,
 * for instance the user is using a virtual machine for their
 * LAMP setup and Triumph is running on a windows machine; 
 * in this case xdebug will return unix paths (/var/www/index.php)
 * but we want to translate that to a local path (C:\users\user\projects\index.php)
 * 
 */
static wxFileName ToLocalFilename(const wxString& xdebugFile, const std::map<wxString, wxString>& localToRemoteMappings) {
	wxString remoteFile = xdebugFile.Mid(8); // 8  = size of "file:///"
	if (!remoteFile.Contains(":")) {
		
		// in linux, filename comes back as "file:///"
		// we want to keep the last slash
		remoteFile = xdebugFile.Mid(7); 
	}
	
	// check to see if the remote path is inside one of the mappings
	std::map<wxString, wxString>::const_iterator mapping;
	wxString localFile;
	wxString remoteLower(remoteFile);
	remoteLower.MakeLower();
	bool foundMapping = false;
	for (mapping = localToRemoteMappings.begin(); mapping != localToRemoteMappings.end(); ++mapping) {
		
		// lets make comparison case-insensitive
		wxString remoteMappingLower(mapping->second);
		remoteMappingLower.MakeLower();
		
		// convert windows backslashes to forward slashes as xdebug does as
		// well
		remoteMappingLower.Replace(wxT("\\"), wxT("/"));
		if (remoteLower.Find(remoteMappingLower) == 0) {
			
			// lets perform the mapping. replace the remote 
			// path with the local path
			localFile = remoteFile;
			localFile.Replace(mapping->second, mapping->first);
			foundMapping = true;
			break;
		}
		
	}
	
	if (!foundMapping) {
		localFile = remoteFile;
	}
	wxFileName name(localFile);
	name.Normalize();
	return name;
}

/**
 * This is the inverse of ToLocalFilename
 *
 * when we send file breakpoints to xdebug, we want the paths
 * to be paths on the remote system.
 * 
 * for instance the user is using a virtual machine for their
 * LAMP setup and Triumph is running on a windows machine; 
 * the user add a breakpoint to C:\users\user\projects\index.php
 * then we want to add a file breakpoint as a 
 * unix path (/var/www/index.php)
 * 
 */
static wxString ToRemoteFilename(const wxString& localFile, const std::map<wxString, wxString>& localToRemoteMappings) {
		
	// check to see if the local path is inside one of the mappings
	std::map<wxString, wxString>::const_iterator mapping;
	wxString remoteFile;
	wxString localLower(localFile);
	localLower.MakeLower();
	bool foundMapping = false;
	for (mapping = localToRemoteMappings.begin(); mapping != localToRemoteMappings.end(); ++mapping) {
		
		// lets make comparison case-insensitive
		wxString localMappingLower(mapping->first);
		localMappingLower.MakeLower();		
		if (localLower.Find(localMappingLower) == 0) {
			
			// lets perform the mapping. replace the local
			// path with the remote path
			remoteFile = localFile;
			remoteFile.Replace(mapping->first, mapping->second);

			// convert windows backslashes to forward slashes as xdebug does as
			// well
			remoteFile.Replace(wxT("\\"), wxT("/"));
			foundMapping = true;
			break;
		}
		
	}
	return foundMapping ? remoteFile : localFile;
}

/**
 * @param config the config to delete the groups from.
 * @param groupNameStart groups that start with this string will be
 *        deleted from the config. Searches are case-sensitive
 */
static void ConfigDeleteGroups(wxConfigBase* config, const wxString& groupNameStart) {
	long index;
	wxString groupName;
	std::vector<wxString> keysToDelete;
	if (config->GetFirstGroup(groupName, index)) {
		do {
			if (groupName.Find(groupNameStart) == 0) {
				keysToDelete.push_back(groupName);
			}
		} while (config->GetNextGroup(groupName, index));
	}
	for (size_t j = 0; j < keysToDelete.size(); ++j) {
		config->DeleteGroup(keysToDelete[j]);
	}
}

/**
 * reads debugger options from the given config.
 */
static void ConfigLoad(wxConfigBase* config,
	t4p::DebuggerOptionsClass& options, 
	std::vector<t4p::BreakpointWithHandleClass>& breakpoints) {
	config->Read(wxT("Debugger/Port"), &options.Port, options.Port);
	config->Read(wxT("Debugger/MaxChildren"), &options.MaxChildren, options.MaxChildren);
	config->Read(wxT("Debugger/MaxDepth"), &options.MaxDepth, options.MaxDepth);
	config->Read(wxT("Debugger/DoListenOnAppReady"), &options.DoListenOnAppReady, options.DoListenOnAppReady);
	config->Read(wxT("Debugger/DoBreakOnStart"), &options.DoBreakOnStart, options.DoBreakOnStart);
	
	wxString groupName;
	long index;
	if (config->GetFirstGroup(groupName, index)) {
		do {
			if (groupName.Find(wxT("DebuggerBreakpoint_")) == 0) {
				t4p::BreakpointWithHandleClass breakpoint;
				
				config->Read(groupName + wxT("/Filename"), &breakpoint.Breakpoint.Filename);
				config->Read(groupName + wxT("/LineNumber"), &breakpoint.Breakpoint.LineNumber);
				config->Read(groupName + wxT("/IsEnabled"), &breakpoint.Breakpoint.IsEnabled );
				config->Read(groupName + wxT("/BreakpointType"), &breakpoint.Breakpoint.BreakpointType);
				config->Read(groupName + wxT("/Exception"), &breakpoint.Breakpoint.Exception);
				config->Read(groupName + wxT("/Expression"), &breakpoint.Breakpoint.Expression);
				config->Read(groupName + wxT("/Function"), &breakpoint.Breakpoint.Function);
				config->Read(groupName + wxT("/HitCondition"), &breakpoint.Breakpoint.HitCondition);
				config->Read(groupName + wxT("/HitValue"), &breakpoint.Breakpoint.HitValue);
				
				breakpoints.push_back(breakpoint);
			}
			if (groupName.Find(wxT("DebuggerMapping_")) == 0) {
				wxString localPath;
				wxString remotePath;
				
				config->Read(groupName + wxT("/LocalPath"), &localPath);
				config->Read(groupName + wxT("/RemotePath"), &remotePath);
				
				if (!localPath.IsEmpty() && !remotePath.IsEmpty()) {
					options.SourceCodeMappings[localPath] = remotePath;
				}
			}
		} while (config->GetNextGroup(groupName, index));
	}
}

/**
 * writes debugger options to the given config.
 * the config is flushed, meaning that it is persisted to
 * disk immediately.
 */
static void ConfigStore(wxConfigBase* config,
	const t4p::DebuggerOptionsClass& options, 
	const std::vector<t4p::BreakpointWithHandleClass>& breakpoints) {
	
	config->Write(wxT("Debugger/Port"),options.Port);
	config->Write(wxT("Debugger/MaxChildren"), options.MaxChildren);
	config->Write(wxT("Debugger/MaxDepth"), options.MaxDepth);
	config->Write(wxT("Debugger/DoListenOnAppReady"), options.DoListenOnAppReady);
	config->Write(wxT("Debugger/DoBreakOnStart"), options.DoBreakOnStart);
	
	// delete all previous breakpoints and mappings
	ConfigDeleteGroups(config, wxT("DebuggerBreakpoint_"));
	ConfigDeleteGroups(config, wxT("DebuggerMapping_"));
	
	std::vector<t4p::BreakpointWithHandleClass>::const_iterator it;
	int i = 0;
	for (it = breakpoints.begin(); it != breakpoints.end(); ++it) {
		config->Write(wxString::Format("DebuggerBreakpoint_%d/Filename", i), it->Breakpoint.Filename);
		config->Write(wxString::Format("DebuggerBreakpoint_%d/LineNumber", i), it->Breakpoint.LineNumber);
		config->Write(wxString::Format("DebuggerBreakpoint_%d/IsEnabled", i), it->Breakpoint.IsEnabled);
		config->Write(wxString::Format("DebuggerBreakpoint_%d/BreakpointType", i), it->Breakpoint.BreakpointType);
		config->Write(wxString::Format("DebuggerBreakpoint_%d/Exception", i), it->Breakpoint.Exception);
		config->Write(wxString::Format("DebuggerBreakpoint_%d/Expression", i), it->Breakpoint.Expression);
		config->Write(wxString::Format("DebuggerBreakpoint_%d/Function", i), it->Breakpoint.Function);
		config->Write(wxString::Format("DebuggerBreakpoint_%d/HitCondition", i), it->Breakpoint.HitCondition);
		config->Write(wxString::Format("DebuggerBreakpoint_%d/HitValue", i), it->Breakpoint.HitValue);
		
		++i;
	}
	
	i = 0;
	std::map<wxString, wxString>::const_iterator mapping;
	for (mapping = options.SourceCodeMappings.begin(); mapping != options.SourceCodeMappings.end(); ++mapping) {
		config->Write(wxString::Format("DebuggerMapping_%d/LocalPath", i), mapping->first);
		config->Write(wxString::Format("DebuggerMapping_%d/RemotePath", i), mapping->second);
		
		i++;
	}
	config->Flush();
}

/**
 * @param node the node to read and turn into a string. 
 * @return string representation of a variable
 */
static wxString VariablePreview(t4p::DebuggerVariableNodeClass* node, int maxPreviewLength) {
	wxString preview;
	bool isArrayOrObject = false;
	wxString enclosingStart;
	wxString enclosingEnd;
	if (node->Property.DataType == wxT("array")) {
		isArrayOrObject = true;
		enclosingStart = wxT("[");
		enclosingEnd = wxT("]");
		
	}
	else if (node->Property.DataType == wxT("object")) {
		isArrayOrObject = true;
		enclosingStart = wxT("{");
		enclosingEnd = wxT("}");
	}
	
	if (isArrayOrObject) {
		if (node->Property.DataType == wxT("array")) {
			preview += wxString::Format("array(%d) ", node->Children.size());
		}
		else if (node->Property.DataType == wxT("object")) {
			preview += node->Property.ClassName + wxT(" ");
		}
		preview += enclosingStart;
		for (size_t i = 0; i < node->Children.size(); ++i) {
			
			// a child could be a key-value pairs, key => objects.
			// or key => arrays
			if (node->Children[i]->Property.DataType == wxT("array")) {
				preview += node->Children[i]->Property.Name + wxT(" => array[ ... ]");
			}
			else if (node->Children[i]->Property.DataType == wxT("object")) {
				preview += node->Children[i]->Property.Name + wxT(" => { ... }");
			}
			else if (node->Children[i]->Property.Name.empty()) {
				
				// special case for the nodes that have not been retrieved from
				// the debug engine yet
				preview += wxT(" ... ");
			}
			else {
				preview += node->Children[i]->Property.Name;
				preview += wxT(" => ");
				preview += node->Children[i]->Property.Value;
			}
			if (i < (node->Children.size() - 1)) {
				preview += wxT(", ");
			}
		}
		preview += enclosingEnd;
	}
	else {
		preview = node->Property.Value;
	}
	
	// truncate to desired length
	bool isTruncated = ((int)preview.length()) > maxPreviewLength;
	preview = preview.Mid(0, maxPreviewLength);
	if (isTruncated) {
		preview += wxT(" ...");
	}
	
	// replace common non-alpha characters
	preview.Replace(wxT("\t"), wxT("\\t"));
	preview.Replace(wxT("\r"), wxT("\\r"));
	preview.Replace(wxT("\n"), wxT("\\n"));
	preview.Replace(wxT("\v"), wxT("\\v"));
	return preview;
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
			boost::asio::write(socket, boost::asio::buffer(next.c_str(), next.length() + 1), boost::asio::transfer_all(), writeError);

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
	std::transform(cmdOnly.begin(), cmdOnly.end(), cmdOnly.begin(), ::tolower);
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
		t4p::DbgpPropertyValueEventClass propertyValueResponse;
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
, Handle(0) 
, DbgpTransactionId() {

}

t4p::BreakpointWithHandleClass::BreakpointWithHandleClass(const t4p::BreakpointWithHandleClass& src)
: Breakpoint()
, Handle(0) 
, DbgpTransactionId() {
	Copy(src);
}

t4p::BreakpointWithHandleClass& t4p::BreakpointWithHandleClass::operator=(const t4p::BreakpointWithHandleClass& src) {
	Copy(src);
	return *this;
}

void t4p::BreakpointWithHandleClass::Copy(const t4p::BreakpointWithHandleClass& src) {
	Breakpoint = src.Breakpoint;
	Handle = src.Handle;
	DbgpTransactionId = src.DbgpTransactionId;
}

t4p::DebuggerOptionsClass::DebuggerOptionsClass()
: Port(9000)
, MaxChildren(100)
, MaxDepth(1) 
, DoListenOnAppReady(false)
, DoBreakOnStart(false)
, SourceCodeMappings() {
	
}

t4p::DebuggerOptionsClass::DebuggerOptionsClass(const t4p::DebuggerOptionsClass& src)
: Port(9000)
, MaxChildren(100)
, MaxDepth(1) 
, DoListenOnAppReady(false)
, DoBreakOnStart(false)
, SourceCodeMappings() {
	Copy(src);
}

t4p::DebuggerOptionsClass& t4p::DebuggerOptionsClass::operator=(const t4p::DebuggerOptionsClass& src) {
	Copy(src);
	return *this;
}

void t4p::DebuggerOptionsClass::Copy(const t4p::DebuggerOptionsClass& src) {
	Port = src.Port;
	MaxChildren = src.MaxChildren;
	MaxDepth = src.MaxDepth;
	DoListenOnAppReady = src.DoListenOnAppReady;
	DoBreakOnStart = src.DoBreakOnStart;
	
	t4p::DeepCopy(SourceCodeMappings, src.SourceCodeMappings);
}

t4p::DebuggerFeatureClass::DebuggerFeatureClass(t4p::AppClass& app)
: FeatureClass(app) 
, Breakpoints() 
, RunningThreads() 
, EventSinkLocker() 
, Cmd()
, Options()
, IsDebuggerSessionActive(false)
, IsDebuggerServerActive(false) 
, WasDebuggerPortChanged(false) 
, WasDebuggerPort(0) {
}

void t4p::DebuggerFeatureClass::AddNewMenu(wxMenuBar* menuBar) {
	wxMenu* menu = new wxMenu();
	menu->AppendCheckItem(t4p::MENU_DEBUGGER + 0, _("Start Listening for Debugger"), 
			_("Opens a server socket to listen for incoming xdebug connections"));
	menu->Append(t4p::MENU_DEBUGGER + 8, _("Stop Listening for Debugger"), 
		_("Closes the server socket that listens for incoming xdebug connections"));
	menu->AppendSeparator();
	
	menu->Append(t4p::MENU_DEBUGGER + 2, _("Step Into\tF11"), 
		_("Run the next command, recursing inside function calls"));
	menu->Append(t4p::MENU_DEBUGGER + 3, _("Step Over\tF10"),
		_("Run the next command, without recursing inside function calls"));
	menu->Append(t4p::MENU_DEBUGGER + 4, _("Step Out\tCTRL+F11"),
		_("Run until the end of the current function"));
	menu->Append(t4p::MENU_DEBUGGER + 5, _("Continue\tF5"),
		_("Run until the next breakpoint"));
	menu->Append(t4p::MENU_DEBUGGER + 6, _("Continue To Cursor"),
		_("Run until the code reaches the cursor"));
	menu->Append(t4p::MENU_DEBUGGER + 9, _("Finish Session"),
		_("Run until the end of the session, ignoring any breakpoints"));
	menu->Append(t4p::MENU_DEBUGGER + 10, _("Go To Current Line"),
		_("Places the cursor in the line where the debugger has stopped at."));	
	menu->AppendSeparator();
	menu->Append(t4p::MENU_DEBUGGER + 7, _("Toggle Breakpoint\tAlt+F10"), 
		_("Turn on or off a breakpoint at the current line of source code."));

	menuBar->Append(menu, _("Debug"));
}

void t4p::DebuggerFeatureClass::AddToolBarItems(wxAuiToolBar* bar) {
	bar->AddSeparator();
	
	bar->AddTool(t4p::MENU_DEBUGGER + 0, _("Start Debugger"), t4p::BitmapImageAsset(wxT("debugger-start")), 
		_("Opens a server socket to listen for incoming xdebug connections"), wxITEM_NORMAL);
	bar->AddTool(t4p::MENU_DEBUGGER + 9, _("Finish Session"), t4p::BitmapImageAsset(wxT("debugger-finish")),
		_("Run until the end of the session, ignoring any breakpoints"), wxITEM_NORMAL);
		
	bar->AddTool(t4p::MENU_DEBUGGER + 2, _("Step Into"), t4p::BitmapImageAsset(wxT("arrow-step-in")), 
		_("Run the next command, recursing inside function calls"), wxITEM_NORMAL);
	bar->AddTool(t4p::MENU_DEBUGGER + 3, _("Step Over"), t4p::BitmapImageAsset(wxT("arrow-step-over")), 
		_("Run the next command, without recursing inside function calls"), wxITEM_NORMAL);
	bar->AddTool(t4p::MENU_DEBUGGER + 4, _("Step Out"), t4p::BitmapImageAsset(wxT("arrow-step-out")), 
		_("Run until the end of the current function"), wxITEM_NORMAL);
	bar->AddTool(t4p::MENU_DEBUGGER + 10, _("Go To Current Line"), t4p::BitmapImageAsset(wxT("arrow-right")), 
		_("Places the cursor in the line where the debugger has stopped at."));	
}


void t4p::DebuggerFeatureClass::OnAppReady(wxCommandEvent& event) {
	RunningThreads.SetMaxThreads(1);
	RunningThreads.AddEventHandler(this);
	if (Options.DoListenOnAppReady) {
		StartDebugger(false);
	}
}

void t4p::DebuggerFeatureClass::OnDebuggerLog(wxThreadEvent& event) {
	wxWindow* window = FindToolsWindow(ID_PANEL_DEBUGGER);
	if (!window) {
		return;
	}
	t4p::DebuggerPanelClass* panel = (t4p::DebuggerPanelClass*) window;
	panel->Logger->Append(event.GetString());
}

void t4p::DebuggerFeatureClass::OnAppExit(wxCommandEvent& event) {
	RunningThreads.RemoveEventHandler(this);
	StopDebugger(Options.Port);
}

void t4p::DebuggerFeatureClass::LoadPreferences(wxConfigBase* base) {
	ConfigLoad(base, Options, Breakpoints);
}

void t4p::DebuggerFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {
	wxConfigBase* base = wxConfig::Get();
	ConfigStore(base, Options, Breakpoints);
	
	if (WasDebuggerPortChanged) {
		
		// need to shutdown the server and listen on the new port
		StopDebugger(WasDebuggerPort);
		if (Options.DoListenOnAppReady) {
			StartDebugger(false);
		}
	}
	WasDebuggerPortChanged = false;
	WasDebuggerPort = Options.Port;
}

void t4p::DebuggerFeatureClass::OnStyledTextModified(wxStyledTextEvent& event) {
	int mask = wxSTC_MOD_INSERTTEXT | wxSTC_MOD_DELETETEXT;
	if (event.GetModificationType() & mask) {
		
		// lets update the location of the breakpoints in this file
		t4p::CodeControlClass* ctrl = (t4p::CodeControlClass*)event.GetEventObject();
		if (!ctrl->IsNew()) {
			wxString ctrlFileName = ctrl->GetFileName();
			std::vector<t4p::BreakpointWithHandleClass>::iterator it;
			for (it = Breakpoints.begin(); it != Breakpoints.end(); ++it) {
				if (it->Breakpoint.Filename == ctrlFileName) {
					
					// only update when line >= 1.  if line == 0 then it means
					// that the entire text has been deleted. this is most likely
					// scenario when the user reloads the file.
					it->Breakpoint.LineNumber = ctrl->BreakpointGetLine(it->Handle);
				}
			}
		}
	}
}


void t4p::DebuggerFeatureClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
	WasDebuggerPortChanged = false;
	WasDebuggerPort = Options.Port;
	t4p::DebuggerOptionsPanelClass* panel =  new t4p::DebuggerOptionsPanelClass(parent, Options, WasDebuggerPortChanged);
	
	parent->AddPage(panel, _("Debugger"));
}

void t4p::DebuggerFeatureClass::OnStopDebugger(wxCommandEvent& event) {
	StopDebugger(Options.Port);
}

void t4p::DebuggerFeatureClass::StopDebugger(int port) {
	
	// this is our (triumph's) message telling the listener that we should no longer accept
	// connections. we do it this way because we use synchronous sockets
	// which we cannot stop from another thread.
	boost::asio::io_service service;
	boost::asio::ip::tcp::resolver resolver(service);
	
	wxString wxsPort = wxString::Format("%d", port);
	std::string strPort = t4p::WxToChar(wxsPort);
	
	boost::asio::ip::tcp::resolver::query query("localhost", strPort);
	boost::asio::ip::tcp::resolver::iterator endpointIterator = resolver.resolve(query);

	boost::asio::ip::tcp::socket socket(service);
	boost::system::error_code error = boost::asio::error::host_not_found;
	boost::asio::ip::tcp::resolver::iterator end;
    while (error && endpointIterator != end) {
      socket.close();
      socket.connect(*endpointIterator++, error);
	  if (!error) {
		std::string closeMsg = "close";
		boost::system::error_code writeError;
		boost::asio::write(socket, boost::asio::buffer(closeMsg), boost::asio::transfer_all(), writeError);
		socket.close();
		break;
	  }
	}
	RunningThreads.StopAll();
	IsDebuggerServerActive = false;
}

void t4p::DebuggerFeatureClass::OnToggleBreakpoint(wxCommandEvent& event) {
	t4p::CodeControlClass* codeCtrl = GetCurrentCodeControl();
	if (!codeCtrl) {
		return;
	}
	int lineNumber = codeCtrl->LineFromPosition(codeCtrl->GetCurrentPos()) + 1;
	ToggleBreakpointAtLine(codeCtrl, lineNumber);
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
	ToggleBreakpointAtLine(codeCtrl, lineNumber);
}

void t4p::DebuggerFeatureClass::ToggleBreakpointAtLine(t4p::CodeControlClass* codeCtrl, int lineNumber) {

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
						ToRemoteFilename(breakpointWithHandle.Breakpoint.Filename, Options.SourceCodeMappings), 
						breakpointWithHandle.Breakpoint.LineNumber, 
						breakpointWithHandle.Breakpoint.IsEnabled
					)
				);
			}

			Breakpoints.push_back(breakpointWithHandle);
		}
	}
	
	wxWindow* window = FindToolsWindow(ID_PANEL_DEBUGGER);
	if (window) {
		t4p::DebuggerPanelClass* panel = (t4p::DebuggerPanelClass*) window;
		panel->BreakpointPanel->RefreshList();
	}
	
	// store the breakpoints to disk
	wxConfigBase* base = wxConfig::Get();
	ConfigStore(base, Options, Breakpoints);
}

void t4p::DebuggerFeatureClass::OnStartDebugger(wxCommandEvent& event) {
	StartDebugger(true);
}

void t4p::DebuggerFeatureClass::StartDebugger(bool doOpenDebuggerPanel) {
	if (IsDebuggerSessionActive) {
		return;
	}
	if (IsDebuggerServerActive) {
		return;
	}
	
	if (doOpenDebuggerPanel) {
		wxWindow* window = FindToolsWindow(ID_PANEL_DEBUGGER);
		if (!window) {
			t4p::DebuggerPanelClass* panel = new t4p::DebuggerPanelClass(GetToolsNotebook(), ID_PANEL_DEBUGGER, *this);
			AddToolsWindow(panel, _("Debugger"));
		}
	}

	t4p::DebuggerServerActionClass* action = new t4p::DebuggerServerActionClass(RunningThreads, ID_ACTION_DEBUGGER, EventSinkLocker);
	action->Init(Options.Port);
	RunningThreads.Queue(action);
	IsDebuggerServerActive = true;
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

void t4p::DebuggerFeatureClass::OnFinish(wxCommandEvent& event) {
	
	// finish is a special command
	// 1. disable all breakpoints
	// 2. run to completion
	// this way, the script ends normally, as opposed to using
	// the stop command which will stop the script and not let
	// it finish
	std::vector<t4p::BreakpointWithHandleClass>::iterator it;
	for (it = Breakpoints.begin(); it != Breakpoints.end(); ++it) {
		if (it->Breakpoint.IsEnabled) {
			PostCmd(
				Cmd.BreakpointDisable(it->Breakpoint.BreakpointId)
			);
		}
	}
	PostCmd(
		Cmd.Run()
	);

	// post the stack get command so that the debugger tells us which
	// line is being executed next
	PostCmd(
		Cmd.StackGet(0)
	);
}

void t4p::DebuggerFeatureClass::OnGoToExecutingLine(wxCommandEvent& event) {

	// post the stack get command so that the debugger tells us which
	// line is being executed next
	PostCmd(
		Cmd.StackGet(0)
	);	
}

void t4p::DebuggerFeatureClass::CmdPropertyGetChildren(const t4p::DbgpPropertyClass& prop) {
	PostCmd(
		Cmd.PropertyGet(0, 0, prop.FullName, prop.Key)
	);
}

void t4p::DebuggerFeatureClass::BreakpointRemove(const t4p::BreakpointWithHandleClass& breakpointWithHandle) {
	
	// if the breakpoint is located in an opened file and has a marker
	// lets remove the marker.  note that the file may not be open and that's
	// not unexpected.
	t4p::CodeControlClass* codeCtrl = GetNotebook()->FindCodeControl(breakpointWithHandle.Breakpoint.Filename);
	if (codeCtrl) {
		codeCtrl->BreakpointRemove(breakpointWithHandle.Breakpoint.LineNumber);
	}
	
	// remove the breakpoint from this list
	std::vector<t4p::BreakpointWithHandleClass>::iterator it = Breakpoints.begin();
	while (it != Breakpoints.end()) {
		if (it->Breakpoint.Filename == breakpointWithHandle.Breakpoint.Filename &&
			it->Breakpoint.LineNumber == breakpointWithHandle.Breakpoint.LineNumber) {
			it = Breakpoints.erase(it);
			break;
		}
		else {
			++it;
		}
	}

	PostCmd(
		Cmd.BreakpointRemove(breakpointWithHandle.Breakpoint.BreakpointId)
	);
	
	// store the breakpoints to disk
	wxConfigBase* base = wxConfig::Get();
	ConfigStore(base, Options, Breakpoints);
}

void t4p::DebuggerFeatureClass::BreakpointDisable(const t4p::BreakpointWithHandleClass& breakpointWithHandle) {
	
	// if the breakpoint is located in an opened file and has a marker
	// lets remove the marker.  note that the file may not be open and that's
	// not unexpected.
	t4p::CodeControlClass* codeCtrl = GetNotebook()->FindCodeControl(breakpointWithHandle.Breakpoint.Filename);
	if (codeCtrl) {
		codeCtrl->BreakpointRemove(breakpointWithHandle.Breakpoint.LineNumber);
	}
	
	// set the breakpoint as disabled
	std::vector<t4p::BreakpointWithHandleClass>::iterator it = Breakpoints.begin();
	while (it != Breakpoints.end()) {
		if (it->Breakpoint.Filename == breakpointWithHandle.Breakpoint.Filename &&
			it->Breakpoint.LineNumber == breakpointWithHandle.Breakpoint.LineNumber) {
			it->Breakpoint.IsEnabled = false;
			break;
		}
		else {
			++it;
		}
	}

	PostCmd(
		Cmd.BreakpointDisable(breakpointWithHandle.Breakpoint.BreakpointId)
	);
	
	// store the breakpoints to disk
	wxConfigBase* base = wxConfig::Get();
	ConfigStore(base, Options, Breakpoints);
}

void t4p::DebuggerFeatureClass::BreakpointEnable(const t4p::BreakpointWithHandleClass& breakpointWithHandle) {
	
	// if the breakpoint is located in an opened file and does not have a marker
	// lets add the marker.  note that the file may not be open and that's
	// not unexpected.
	int newHandle = -1;
	t4p::CodeControlClass* codeCtrl = GetNotebook()->FindCodeControl(breakpointWithHandle.Breakpoint.Filename);
	if (codeCtrl) {
		codeCtrl->BreakpointMarkAt(breakpointWithHandle.Breakpoint.LineNumber, newHandle);
	}
	
	// set the breakpoint as enabled
	std::vector<t4p::BreakpointWithHandleClass>::iterator it = Breakpoints.begin();
	while (it != Breakpoints.end()) {
		if (it->Breakpoint.Filename == breakpointWithHandle.Breakpoint.Filename &&
			it->Breakpoint.LineNumber == breakpointWithHandle.Breakpoint.LineNumber) {
			it->Breakpoint.IsEnabled = true;
			it->Handle = newHandle;
			break;
		}
		else {
			++it;
		}
	}

	PostCmd(
		Cmd.BreakpointEnable(breakpointWithHandle.Breakpoint.BreakpointId)
	);
	
	// store the breakpoints to disk
	wxConfigBase* base = wxConfig::Get();
	ConfigStore(base, Options, Breakpoints);
}

void t4p::DebuggerFeatureClass::BreakpointGoToSource(const t4p::BreakpointWithHandleClass& breakpointWithHandle) {
	t4p::OpenFileCommandEventClass openEvt(
		breakpointWithHandle.Breakpoint.Filename, -1, -1,
		breakpointWithHandle.Breakpoint.LineNumber
	);
	App.EventSink.Publish(openEvt);
}

void t4p::DebuggerFeatureClass::OnAppFileOpened(t4p::CodeControlEventClass& event) {
	
	// here we want to add the breakpoint marker if there is a breakpoint
	// in the opened file
	std::vector<t4p::BreakpointWithHandleClass>::iterator br;
	t4p::CodeControlClass* codeCtrl = event.GetCodeControl();

	for (br = Breakpoints.begin(); br != Breakpoints.end(); ++br) {
		if (br->Breakpoint.Filename == codeCtrl->GetFileName()) {
			codeCtrl->BreakpointMarkAt(br->Breakpoint.LineNumber, br->Handle);
		}
	}
}

void t4p::DebuggerFeatureClass::OnAppFileClosed(t4p::CodeControlEventClass& event) {
	
	// here we want to remove the handles to the breakpoints that
	// belong to the file being closed.
	std::vector<t4p::BreakpointWithHandleClass>::iterator br;
	t4p::CodeControlClass* codeCtrl = event.GetCodeControl();

	for (br = Breakpoints.begin(); br != Breakpoints.end(); ++br) {
		if (br->Breakpoint.Filename == codeCtrl->GetFileName()) {
			br->Handle = -1;
		}
	}
	
	// we want to save the breakpoints because the user may  have
	// added or removed code and now they are located in a different
	// line number
	wxConfigBase* config = wxConfig::Get();
	ConfigStore(config, Options, Breakpoints);
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
				ToRemoteFilename(it->Breakpoint.Filename, Options.SourceCodeMappings), 
				it->Breakpoint.LineNumber, 
				it->Breakpoint.IsEnabled
			)
		);
	}

	// mark the session as having been started
	IsDebuggerSessionActive = true;
	
	// open the debugger panel if its not yet open
	wxWindow* window = FindToolsWindow(ID_PANEL_DEBUGGER);
	if (!window) {
		t4p::DebuggerPanelClass* panel = new t4p::DebuggerPanelClass(GetToolsNotebook(), ID_PANEL_DEBUGGER, *this);
		AddToolsWindow(panel, _("Debugger"));
	}

	// set the max children of variables to get at once
	// we can handle more than the default
	PostCmd(Cmd.FeatureSet("max_depth", wxString::Format("%d", Options.MaxDepth)));
	PostCmd(Cmd.FeatureSet("max_children", wxString::Format("%d", Options.MaxChildren)));
	
	// now we send commands to the debug engine
	// if the user wants to break at the first line, we
	// want to step into the first line.
	if (Options.DoBreakOnStart) {
		PostCmd(Cmd.StepInto());
		PostCmd(Cmd.ContextNames(0));
		PostCmd(Cmd.ContextGet(0, 0));
		PostCmd(Cmd.StackGet(0));
	}
	else {
		
		// we want the script to run until a breakpoint is
		// hit
		PostCmd(Cmd.Run());
		PostCmd(Cmd.ContextNames(0));
		PostCmd(Cmd.ContextGet(0, 0));
		PostCmd(Cmd.StackGet(0));
	}
}

void t4p::DebuggerFeatureClass::OnDbgpError(t4p::DbgpErrorEventClass& event) {
	IsDebuggerSessionActive = false;
	ResetDebugger();
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
	if (!IsDebuggerSessionActive) {
		ResetDebugger();
	}
	else {
		PostCmd(
			Cmd.ContextGet(0, 0)
		);

		// the script has paused execution, alert the user that the debugger windows
		// are being updated with new script stack trace, variables.
		if (!App.IsActive()) {
			wxCommandEvent evt(t4p::EVENT_CMD_APP_USER_ATTENTION);
			App.EventSink.Publish(evt);
		}
	}
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
	wxFileName currentFilename = ToLocalFilename(event.Stack[0].Filename, Options.SourceCodeMappings);
	t4p::OpenFileCommandEventClass openEvt(
		currentFilename.GetFullPath(), -1, -1,
		event.Stack[0].LineNumber
	);
	App.EventSink.Publish(openEvt);
	t4p::CodeControlClass* codeCtrl = GetCurrentCodeControl();
	codeCtrl->ExecutionMarkAt(event.Stack[0].LineNumber);

	wxWindow* window = FindOutlineWindow(ID_PANEL_DEBUGGER_STACK);
	t4p::DebuggerStackPanelClass* stackPanel = NULL;
	if (window) {
		stackPanel = (t4p::DebuggerStackPanelClass*) window;
	}
	else {
		stackPanel = new t4p::DebuggerStackPanelClass(GetOutlineNotebook(), ID_PANEL_DEBUGGER_STACK);
		AddOutlineWindow(stackPanel, _("Stack"));
	}
	
	// lets convert remote paths to local paths
	for (size_t i = 0; i < event.Stack.size(); ++i) {
		event.Stack[i].Filename = ToLocalFilename(event.Stack[i].Filename, Options.SourceCodeMappings).GetFullPath();
	}
	stackPanel->ShowStack(event.Stack);
}

void t4p::DebuggerFeatureClass::OnDbgpContextNames(t4p::DbgpContextNamesEventClass& event) {
	
}

void t4p::DebuggerFeatureClass::OnDbgpContextGet(t4p::DbgpContextGetEventClass& event) {
	wxWindow* window = FindToolsWindow(ID_PANEL_DEBUGGER);
	if (!window) {
		
		// not sure if we should create the panel again.
		// if the panel is not here it means that the user closed it
		// and wants it closed?
		return;
	}
	t4p::DebuggerPanelClass* panel = (t4p::DebuggerPanelClass*) window;
	panel->VariablePanel->AddVariables(event.Properties);
}

void t4p::DebuggerFeatureClass::OnDbgpPropertyGet(t4p::DbgpPropertyGetEventClass& event) {
	wxWindow* window = FindToolsWindow(ID_PANEL_DEBUGGER);
	if (!window) {
		
		// not sure if we should create the panel again.
		// if the panel is not here it means that the user closed it
		// and wants it closed?
		return;
	}
	t4p::DebuggerPanelClass* panel = (t4p::DebuggerPanelClass*) window;
	panel->VariablePanel->UpdateVariable(event.Property);
}

void t4p::DebuggerFeatureClass::OnDbgpPropertyValue(t4p::DbgpPropertyValueEventClass& event) {
	t4p::DebuggerFullViewDialogClass dialog(GetMainWindow(), event.Value);
	dialog.ShowModal();
}

void t4p::DebuggerFeatureClass::OnDbgpPropertySet(t4p::DbgpPropertySetEventClass& event) {

}

void t4p::DebuggerFeatureClass::OnDbgpBreak(t4p::DbgpBreakEventClass& event) {

}

void t4p::DebuggerFeatureClass::OnDbgpEval(t4p::DbgpEvalEventClass& event) {

}

void t4p::DebuggerFeatureClass::ResetDebugger() {
	t4p::NotebookClass* notebook = GetNotebook();
	for (size_t i = 0; i < notebook->GetPageCount(); ++i) {
		t4p::CodeControlClass* codeCtrl = notebook->GetCodeControl(i);
		codeCtrl->ExecutionMarkRemove();
	}

	wxWindow* window = FindOutlineWindow(ID_PANEL_DEBUGGER_STACK);
	if (window) {
		t4p::DebuggerStackPanelClass* stackPanel = (t4p::DebuggerStackPanelClass*) window;
		stackPanel->ClearStack();
	}
	
	wxWindow* panelWindow = FindToolsWindow(ID_PANEL_DEBUGGER);
	if (panelWindow) {
		t4p::DebuggerPanelClass* panel = (t4p::DebuggerPanelClass*) panelWindow;
		panel->VariablePanel->ClearVariables();
	}
}

void t4p::DebuggerFeatureClass::PostCmd(std::string cmd) {
	wxThreadEvent evt(t4p::EVENT_DEBUGGER_CMD, wxID_ANY);
	evt.SetString(cmd);
	EventSinkLocker.Post(evt);
}

void t4p::DebuggerFeatureClass::OnDebuggerShowFull(wxCommandEvent& event) {
	
	// the command string contains the property full name and 
	//. key, separated by newline
	wxString all = event.GetString();
	wxString key = all.After(wxT('\n'));
	wxString fullName = all.Before(wxT('\n'));
	PostCmd(
		Cmd.PropertyValue(0, 0, fullName, key)
	);
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

t4p::DebuggerPanelClass::DebuggerPanelClass(wxWindow* parent, int id, t4p::DebuggerFeatureClass& feature)
: DebuggerPanelGeneratedClass(parent, id) {
	Notebook->SetWindowStyle(wxAUI_NB_BOTTOM);

	Logger = new t4p::DebuggerLogPanelClass(this);
	VariablePanel = new t4p::DebuggerVariablePanelClass(this, wxID_ANY, feature);
	BreakpointPanel = new t4p::DebuggerBreakpointPanelClass(this, wxID_ANY, feature);

	Notebook->AddPage(VariablePanel, _("Variables"));
	Notebook->AddPage(BreakpointPanel, _("Breakpoints"));
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

		// file has already been converted to local path
		wxFileName fileName = it->Filename;
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

	StatusLabel->SetLabel(wxT("Status: Debugging session active"));
	this->Layout();
}

void t4p::DebuggerStackPanelClass::ClearStack() {
	StackList->DeleteAllItems();
	StatusLabel->SetLabel(wxT("Status: Debugging session not active"));
	this->Layout();
}

t4p::DebuggerVariablePanelClass::DebuggerVariablePanelClass(wxWindow* parent, int id, t4p::DebuggerFeatureClass& feature)
: DebuggerVariablePanelGeneratedClass(parent, id) 
, Feature(feature) {
	wxObjectDataPtr<t4p::DebuggerVariableModelClass> variableModel;
	
	// variableModel is ref counted; wxObjectDataPtr decreases
	// the ref count once it goes out of scope
	variableModel = new t4p::DebuggerVariableModelClass();
	VariablesList->AssociateModel(variableModel.get());

	VariablesList->AppendTextColumn(_("Variable Name"), 0, wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE);
	VariablesList->AppendTextColumn(_("Variable Type"), 1, wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE);
	
	
	// renderer is owned by col and col is owned by VariablesList
	t4p::PreviewTextCustomRendererClass* renderer = new t4p::PreviewTextCustomRendererClass(feature);
	wxDataViewColumn* col = new wxDataViewColumn(_("Variable Value"), renderer, 2, wxCOL_WIDTH_AUTOSIZE,
		wxALIGN_LEFT);
	VariablesList->AppendColumn(col);
}

void t4p::DebuggerVariablePanelClass::AddVariables(const std::vector<t4p::DbgpPropertyClass>& variables) {
	t4p::DebuggerVariableModelClass* variableModel = (t4p::DebuggerVariableModelClass*)VariablesList->GetModel();
	variableModel->SetVariables(variables);

	VariablesList->Expand(variableModel->GetParent(wxDataViewItem()));
}

void t4p::DebuggerVariablePanelClass::ClearVariables() {
	t4p::DebuggerVariableModelClass* variableModel = (t4p::DebuggerVariableModelClass*)VariablesList->GetModel();
	std::vector<t4p::DbgpPropertyClass> emptyVariables;
	variableModel->SetVariables(emptyVariables);
	StatusLabel->SetLabel(wxT("Status: Debugging session not active"));
	this->Layout();
}

void t4p::DebuggerVariablePanelClass::OnVariableExpanding(wxDataViewEvent& event) {
	wxDataViewItem item = event.GetItem();
	if (!item.GetID()) {
		return;
	}
 	t4p::DebuggerVariableNodeClass* node = (t4p::DebuggerVariableNodeClass*) item.GetID();
	if (node->Property.HasChildren) {
		
		// "object" type nodes have an extra child that holds the classname. xdebug does not 
		// count that as part of the NumChildren but it does return the extra property
		// when we loaded this property we did not get all of it.
			// fetch all of it now
		int fetchedSize = (int)node->Children.size();
		if (node->Property.DataType == "object" 
			&& fetchedSize < node->Property.NumChildren) {
			
			Feature.CmdPropertyGetChildren(node->Property);
		}
		else if (fetchedSize < node->Property.NumChildren) {
			Feature.CmdPropertyGetChildren(node->Property);
		}
		else if (fetchedSize == node->Property.NumChildren && fetchedSize == 1 
			&& node->Children[0]->Property.Name.empty()) {
			
			// when we fill the data view ctrl, we add an empty node when we get
			// a property that has children but the debug engine has not returned it
			// due to hitting the depth limit
			Feature.CmdPropertyGetChildren(node->Property);
		}
	}
}

void t4p::DebuggerVariablePanelClass::UpdateVariable(const t4p::DbgpPropertyClass& variable) {
	t4p::DebuggerVariableModelClass* variableModel = (t4p::DebuggerVariableModelClass*)VariablesList->GetModel();
	wxDataViewItem updatedItem;
	variableModel->UpdateVariable(variable, updatedItem);
	
	VariablesList->Expand(updatedItem);
}


static wxDataViewItem MakeItem(t4p::DebuggerVariableNodeClass* node) {
	return wxDataViewItem((void*)node);
}

t4p::DebuggerVariableNodeClass::DebuggerVariableNodeClass(t4p::DebuggerVariableNodeClass* parent)
: Property()
, Parent(parent)
, Children() {

}

t4p::DebuggerVariableNodeClass::DebuggerVariableNodeClass(t4p::DebuggerVariableNodeClass* parent, 
														  const t4p::DbgpPropertyClass& prop)
: Property(prop)
, Parent(parent)
, Children() {

}

t4p::DebuggerVariableNodeClass::~DebuggerVariableNodeClass() {
	for (size_t i = 0; i < Children.size(); ++i) {
		delete Children[i];
	}
}

static void RecursiveAddNode(t4p::DebuggerVariableNodeClass* parent, const t4p::DbgpPropertyClass& prop, wxDataViewModel* model) {
	wxDataViewItem rootItem = MakeItem(parent);
	
	t4p::DebuggerVariableNodeClass* childNode = new t4p::DebuggerVariableNodeClass(parent, prop);
	parent->Children.push_back(childNode);
	
	model->ItemAdded(rootItem, MakeItem(childNode));

	for (size_t i = 0; i < prop.ChildProperties.size(); ++i) {
		RecursiveAddNode(childNode, prop.ChildProperties[i], model);
	}
}

/**
 * deletes all of the children of node, but not the node itself.
 * Will also delete descendants of the children.
 * will also notify the model of the items that were deleted.
 */
static void RecursiveDeleteChildren(t4p::DebuggerVariableNodeClass* node, wxDataViewModel* model) {
	wxDataViewItemArray deletedItems;
	for (size_t i = 0; i < node->Children.size(); ++i) {
		if (!node->Children[i]->Children.empty()) {
			RecursiveDeleteChildren(node->Children[i], model);
		}		
		wxDataViewItem item = MakeItem(node->Children[i]);
		deletedItems.Add(item);

		// delete the item itself
		delete node->Children[i];
	}
	node->Children.clear();

	// tell the control that items were removed
	wxDataViewItem parent = MakeItem(node);
	model->ItemsDeleted(parent, deletedItems);
}

void t4p::DebuggerVariableNodeClass::ReplaceChildren(const std::vector<t4p::DbgpPropertyClass>& newChildren, wxDataViewModel* model) {
	wxDataViewItem rootItem = MakeItem(this);
	wxDataViewItemArray toRemove;
	for (size_t i = 0; i < Children.size(); ++i) {
		toRemove.Add(MakeItem(Children[i]));
		RecursiveDeleteChildren(Children[i], model);
		delete Children[i];
	}
	Children.clear();
	model->ItemsDeleted(rootItem, toRemove);

	for (size_t i = 0; i < newChildren.size(); ++i) {
		RecursiveAddNode(this, newChildren[i], model);
	}
}

t4p::DebuggerVariableModelClass::DebuggerVariableModelClass()
: RootVariable(NULL) {
	t4p::DebuggerVariableNodeClass* root = new t4p::DebuggerVariableNodeClass(NULL);
	root->Property.Name = wxT("Local Variables");
	RootVariable.Children.push_back(root);
}

unsigned int t4p::DebuggerVariableModelClass::GetChildren(const wxDataViewItem& item, wxDataViewItemArray& children) const {
	t4p::DebuggerVariableNodeClass* node = (t4p::DebuggerVariableNodeClass*) item.GetID();
	if (!node) {
		for (size_t i = 0; i < RootVariable.Children.size(); ++i) {
			children.Add(MakeItem(RootVariable.Children[i]));
		}
		return RootVariable.Children.size();
	}
	for (size_t i = 0; i < node->Children.size(); ++i) {
		children.Add(MakeItem(node->Children[i]));
	}
	if (node->Children.empty() && node->Property.HasChildren) {
		
		// the property has children, but the debugger did not return it
		// because of the max depth limit.  put an empty node here, so
		// that the  dataviewctrl shows the expand icon.
		t4p::DbgpPropertyClass emptyProp;
		t4p::DebuggerVariableNodeClass* emptyNode = new t4p::DebuggerVariableNodeClass(node, emptyProp);
		node->Children.push_back(emptyNode);
		
		children.Add(MakeItem(node->Children[0]));
	}
	return node->Children.size();
}

unsigned int t4p::DebuggerVariableModelClass::GetColumnCount() const {
	return 3;
}

wxString t4p::DebuggerVariableModelClass::GetColumnType(unsigned int col) const {
	return wxT("string");
}

wxDataViewItem t4p::DebuggerVariableModelClass::GetParent(const wxDataViewItem& item) const {
	t4p::DebuggerVariableNodeClass* node = (t4p::DebuggerVariableNodeClass*) item.GetID();
	if (!node) {
		return wxDataViewItem(0);
	}
	return wxDataViewItem(node->Parent);
}

bool t4p::DebuggerVariableModelClass::HasContainerColumns(const wxDataViewItem& item) const {
	return true;
}

void t4p::DebuggerVariableModelClass::GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned int col) const {
	wxASSERT(item.IsOk());
	t4p::DebuggerVariableNodeClass* node = (t4p::DebuggerVariableNodeClass*) item.GetID();
	switch (col) {
	case 0:
		variant = node->Property.Name;
		break;
	case 1:
		variant = node->Property.DataType == "object" ? node->Property.ClassName : node->Property.DataType;
		break;
	case 2:
		variant = VariablePreview(node, 80);
		break;
	}
}

bool t4p::DebuggerVariableModelClass::IsContainer(const wxDataViewItem& item) const {
	if (!item.IsOk()) {
		return true;
	}
	t4p::DebuggerVariableNodeClass* node = (t4p::DebuggerVariableNodeClass*) item.GetID();

	// use the HasChildren property not the chidlren vector because the debug engine may not
	// have returned the children due to the max depth limit
	return node->Property.HasChildren || NULL == node->Parent;
}

bool t4p::DebuggerVariableModelClass::SetValue(const wxVariant& variant, const wxDataViewItem& item, unsigned int col) {
	
	// for now dont allow modifications by the user as the
	// user would expect the changes to be reflected in the
	// debug engine (running script variables change)
	return false;
}

void t4p::DebuggerVariableModelClass::SetVariables(const std::vector<t4p::DbgpPropertyClass>& variables) {
	RootVariable.Children[0]->ReplaceChildren(variables, this);
}

static bool UpdateAndNotifyVariable(t4p::DebuggerVariableNodeClass* node, wxDataViewModel* model, const t4p::DbgpPropertyClass& updatedVariable,
									wxDataViewItem& updatedItem) {

	// first check to see if the udpated variable is a child of this code
	bool found = false;
	std::vector<t4p::DebuggerVariableNodeClass*>::const_iterator nodeIt;
	for (nodeIt = node->Children.begin(); !found && nodeIt != node->Children.end(); ++nodeIt) {

		// was the child of this node updated
		if ((*nodeIt)->Property.FullName == updatedVariable.FullName) {
			RecursiveDeleteChildren((*nodeIt), model);
			(*nodeIt)->ReplaceChildren(updatedVariable.ChildProperties, model);
			found = true;

			updatedItem = MakeItem(*nodeIt);
			break;
		}
	}
	if (!found) {
		
		// is this the node that was updated
		if (node->Property.FullName == updatedVariable.FullName) {
			RecursiveDeleteChildren(node, model);
			node->ReplaceChildren(updatedVariable.ChildProperties, model);
			found = true;

			updatedItem = MakeItem(node);
		}
	}
	if (!found) {
		
		// recursively attempt to find the variable that was updated.
		for (size_t i = 0; !found && i < node->Children.size(); ++i) {
			if (UpdateAndNotifyVariable(node->Children[i], model, updatedVariable, updatedItem)) {
				found = true;
				break;
			}
		}
	}
	return found;
}

void t4p::DebuggerVariableModelClass::UpdateVariable(const t4p::DbgpPropertyClass& updatedVariable, wxDataViewItem& updatedItem) {
	bool found = false;
	for (size_t i = 0; !found && i < RootVariable.Children.size(); ++i) {
		found = UpdateAndNotifyVariable(RootVariable.Children[i], this, updatedVariable, updatedItem);
	}
}


t4p::DebuggerBreakpointPanelClass::DebuggerBreakpointPanelClass(wxWindow* parent, int id, t4p::DebuggerFeatureClass& feature)
: DebuggerBreakpointPanelGeneratedClass(parent, id)
, Feature(feature) 
, AreAllEnabled(false) {

	DeleteBreakpointButton->SetBitmap(t4p::BitmapImageAsset(wxT("breakpoint-delete")));
	ToggleAllBreakpointsButton->SetBitmap(t4p::BitmapImageAsset(wxT("breakpoint-toggle")));

	BreakpointsList->AppendToggleColumn("Enabled");
	BreakpointsList->AppendTextColumn("File");
	BreakpointsList->AppendTextColumn("Line Number");

	RefreshList();
}

void t4p::DebuggerBreakpointPanelClass::RefreshList() {
	BreakpointsList->DeleteAllItems();

	std::vector<t4p::BreakpointWithHandleClass>::const_iterator br;
	AreAllEnabled = true;
	for (br = Feature.Breakpoints.begin(); br != Feature.Breakpoints.end(); ++br) {
		wxFileName brFile(br->Breakpoint.Filename);
		
		wxVector<wxVariant> row;
		row.push_back(wxVariant(br->Breakpoint.IsEnabled));	
		row.push_back(wxVariant(brFile.GetFullName()));
		row.push_back(wxVariant(wxString::Format("%d", br->Breakpoint.LineNumber)));

		BreakpointsList->AppendItem(row);

		if (!br->Breakpoint.IsEnabled) {
			AreAllEnabled = false;
		}
	}

	if (!Feature.Breakpoints.empty()) {
		BreakpointsList->SelectRow(0);
	}
}

void t4p::DebuggerBreakpointPanelClass::OnDeleteBreakpoint(wxCommandEvent& event) {
	int index = BreakpointsList->GetSelectedRow();
	if (wxNOT_FOUND == index || index >= (int)Feature.Breakpoints.size()) {
		return;
	}
	t4p::BreakpointWithHandleClass toRemove = Feature.Breakpoints[index];
	Feature.BreakpointRemove(toRemove);

	// remove from the widget immediately. we can't really acknowledge that
	// this exact breakpoint was removed by the debug engine since the 
	// debug engine's response to the breakpoint_remove command does not
	// return the breakpointId.
	BreakpointsList->DeleteItem(index);
}

void t4p::DebuggerBreakpointPanelClass::OnToggleAllBreakpoints(wxCommandEvent& event) {
	bool newValue = !AreAllEnabled;
	
	int row = 0;
	std::vector<t4p::BreakpointWithHandleClass>::const_iterator it;
	for (it = Feature.Breakpoints.begin(); it != Feature.Breakpoints.end(); ++it) {
		if (newValue) {
			Feature.BreakpointEnable(*it);
		}
		else {
			Feature.BreakpointDisable(*it);
		}

		// uncheck the enabled flag in the widget
		BreakpointsList->SetToggleValue(newValue, row, 0);
		row++;
	}

	AreAllEnabled = newValue;
}

void t4p::DebuggerBreakpointPanelClass::OnItemActivated(wxDataViewEvent& event) {
	int row = BreakpointsList->ItemToRow(event.GetItem());
	if (wxNOT_FOUND == row) {
		return;
	}
	if (row >= (int)Feature.Breakpoints.size()) {
		return;
	}
	Feature.BreakpointGoToSource(Feature.Breakpoints[row]);

}

void t4p::DebuggerBreakpointPanelClass::OnItemValueChanged(wxDataViewEvent& event) {
	
	// only want to see changes to enabled flag
	if (event.GetColumn() != 0) {
		return;
	}
	int row = BreakpointsList->ItemToRow(event.GetItem());
	if (wxNOT_FOUND == row) {
		return;
	}
	if (row >= (int)Feature.Breakpoints.size()) {
		return;
	}
	bool isEnabled = BreakpointsList->GetToggleValue(row, 0);
	if (isEnabled) {
		Feature.BreakpointEnable(Feature.Breakpoints[row]);
	}
	else {
		Feature.BreakpointDisable(Feature.Breakpoints[row]);
	}
}

t4p::DebuggerOptionsPanelClass::DebuggerOptionsPanelClass(wxWindow* parent, t4p::DebuggerOptionsClass& options, 
	bool& wasDebuggerPortChanged)
: DebuggerOptionsPanelGeneratedClass(parent, wxID_ANY) 
, Options(options)
, EditedOptions(options) 
, WasDebuggerPortChanged(wasDebuggerPortChanged) {
	
	wxGenericValidator portValidator(&EditedOptions.Port);
	Port->SetValidator(portValidator);
	wxGenericValidator maxChildrenValidator(&EditedOptions.MaxChildren);
	MaxChildren->SetValidator(maxChildrenValidator);
	wxGenericValidator maxDepthValidator(&EditedOptions.MaxDepth);
	MaxDepth->SetValidator(maxDepthValidator);
	wxGenericValidator doListenOnAppReadyValidator(&EditedOptions.DoListenOnAppReady);
	DoListenOnAppReady->SetValidator(doListenOnAppReadyValidator);
	wxGenericValidator doBreakonStartValidator(&EditedOptions.DoBreakOnStart);
	DoBreakOnStart->SetValidator(doBreakonStartValidator);
	
	
	SourceCodeMappings->DeleteAllColumns();
	SourceCodeMappings->AppendColumn(_("Local Path"));
	SourceCodeMappings->AppendColumn(_("Remote Path"));
	
	FillMappings();
}

void t4p::DebuggerOptionsPanelClass::FillMappings() {
	SourceCodeMappings->DeleteAllItems();
	
	std::map<wxString, wxString>::iterator it;
	int row = 0;
	for (it = EditedOptions.SourceCodeMappings.begin(); it != EditedOptions.SourceCodeMappings.end(); ++it) {
		t4p::ListCtrlAdd(SourceCodeMappings, it->first, it->second);
		++row;
	}
	
	if (SourceCodeMappings->GetItemCount() > 0) {
		SourceCodeMappings->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_MASK_STATE | wxLIST_MASK_TEXT);
		SourceCodeMappings->SetColumnWidth(0, wxLIST_AUTOSIZE);
		SourceCodeMappings->SetColumnWidth(1, wxLIST_AUTOSIZE);
	}
}

void t4p::DebuggerOptionsPanelClass::OnAddMapping(wxCommandEvent& event) {
	wxString local;
	wxString remote;
	
	t4p::DebuggerMappingDialogClass dialog(this, local, remote);
	if (dialog.ShowModal() == wxOK) {
		EditedOptions.SourceCodeMappings[local] = remote;
		
		t4p::ListCtrlAdd(SourceCodeMappings, local, remote);
		if (SourceCodeMappings->GetItemCount() == 1) {
			SourceCodeMappings->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_MASK_STATE | wxLIST_MASK_TEXT);
		}
		SourceCodeMappings->SetColumnWidth(0, wxLIST_AUTOSIZE);
		SourceCodeMappings->SetColumnWidth(1, wxLIST_AUTOSIZE);
	}
}

void t4p::DebuggerOptionsPanelClass::OnDeleteMapping(wxCommandEvent& event) {
	int selected = t4p::ListCtrlSelected(SourceCodeMappings);
	if (selected == wxNOT_FOUND) {
		return;
	}
	SourceCodeMappings->DeleteItem(selected);
	
	std::map<wxString, wxString>::iterator it;
	int i = 0;
	for (it = EditedOptions.SourceCodeMappings.begin(); it != EditedOptions.SourceCodeMappings.end(); ++it) {
		if (i == selected) {
			EditedOptions.SourceCodeMappings.erase(it);
			break;
		}
		i++;
	}
}

void t4p::DebuggerOptionsPanelClass::OnEditMapping(wxCommandEvent& event) {
	int selected = t4p::ListCtrlSelected(SourceCodeMappings);
	if (selected == wxNOT_FOUND) {
		return;
	}
	std::map<wxString, wxString>::iterator it;
	int i = 0;
	for (it = EditedOptions.SourceCodeMappings.begin(); it != EditedOptions.SourceCodeMappings.end(); ++it) {
		if (i == selected) {
			wxString localPath = it->first;
			wxString remotePath = it->second;
			t4p::DebuggerMappingDialogClass dialog(this, localPath, remotePath);
			if (dialog.ShowModal() == wxOK) {
				EditedOptions.SourceCodeMappings[localPath] = remotePath;
				
				t4p::ListCtrlEdit(SourceCodeMappings, localPath, remotePath, i);
				
				SourceCodeMappings->SetColumnWidth(0, wxLIST_AUTOSIZE);
				SourceCodeMappings->SetColumnWidth(1, wxLIST_AUTOSIZE);
			}
			break;
		}
		i++;
	}
}

void t4p::DebuggerOptionsPanelClass::OnListItemActivated(wxListEvent& event) {
	wxCommandEvent evt;
	OnEditMapping(evt);
}

bool t4p::DebuggerOptionsPanelClass::TransferDataFromWindow() {
	if (!wxWindow::TransferDataFromWindow()) {
		return false;
	}
	WasDebuggerPortChanged = Options.Port != EditedOptions.Port;
	Options = EditedOptions;
	return true;
}

t4p::PreviewTextCustomRendererClass::PreviewTextCustomRendererClass(wxEvtHandler& handler)
: wxDataViewCustomRenderer(wxT("string"), wxDATAVIEW_CELL_ACTIVATABLE) 
, Contents() 
, EventHandler(handler) {
	
	MagnifierBitmap = t4p::BitmapImageAsset(wxT("magnifier"));
}

bool t4p::PreviewTextCustomRendererClass::GetValue(wxVariant& variant) const {
	variant = Contents;
	return true;
}

bool t4p::PreviewTextCustomRendererClass::SetValue(const wxVariant& variant) {
	Contents = variant.GetString();
	return true;
}

wxSize t4p::PreviewTextCustomRendererClass::GetSize() const {
	if (Contents.empty()) {
		return wxSize(wxDVC_DEFAULT_RENDERER_SIZE, wxDVC_DEFAULT_RENDERER_SIZE);
	}
	wxSize size = GetTextExtent(Contents);
	size.SetWidth(size.GetWidth() + MagnifierBitmap.GetWidth() + 4);
	return size;
}

bool t4p::PreviewTextCustomRendererClass::Render(wxRect cell, wxDC* dc, int state) {	
	if (!Contents.empty()) {
		dc->DrawBitmap(MagnifierBitmap, cell.GetPosition());
	}
	RenderText(Contents, MagnifierBitmap.GetWidth() + 4, cell, dc, state);
	return true;
}

bool t4p::PreviewTextCustomRendererClass::ActivateCell(const wxRect& cell,
		wxDataViewModel* model,
		const wxDataViewItem& item,
		unsigned int col,
		const wxMouseEvent* mouseEvent 
	) {
		
		// need to get the variable's full name and key because that's 
		// what we need to call xdebug's property_value command. Contents
		// will have the value.
		wxCommandEvent evt(t4p::EVENT_DEBUGGER_SHOW_FULL, wxID_ANY);
		if (item.IsOk()) {
			t4p::DebuggerVariableNodeClass* node = (t4p::DebuggerVariableNodeClass*) item.GetID();
			wxString var = node->Property.FullName + wxT("\n") + node->Property.Key;
			evt.SetString(var);
		}
		
		// make sure user clicked inside the bitmap
		if (mouseEvent && mouseEvent->GetX() < (MagnifierBitmap.GetWidth() + 4)) {
			wxPostEvent(&EventHandler, evt);
			return true;
		} 
		else if (!mouseEvent) {
			wxPostEvent(&EventHandler, evt);
			return true;
		}
		return true;
}

t4p::DebuggerFullViewDialogClass::DebuggerFullViewDialogClass(wxWindow* parent, const wxString& value)
: DebuggerFullViewDialogGeneratedClass(parent, wxID_ANY) {
	Text->SetValue(value);
}


t4p::DebuggerMappingDialogClass::DebuggerMappingDialogClass(wxWindow* parent, wxString& localPath, wxString& remotePath)
: DebuggerMappingDialogGeneratedClass(parent, wxID_ANY) 
, LocalDir() 
, LocalPathString(localPath) {
	LocalDir.AssignDir(localPath);
	t4p::DirPickerValidatorClass localValidator(&LocalDir);
	LocalPath->SetValidator(localValidator);
	
	wxTextValidator remoteValidator(wxFILTER_NONE, &remotePath);
	RemotePath->SetValidator(remoteValidator);
	
	TransferDataToWindow();
}

void t4p::DebuggerMappingDialogClass::OnCancelButton(wxCommandEvent& event) {
	EndModal(wxCANCEL);
}

void t4p::DebuggerMappingDialogClass::OnOkButton(wxCommandEvent& event) {
	if (!TransferDataFromWindow()) {
		return;
	}
	if (RemotePath->GetValue().IsEmpty()) {
		wxMessageBox(_("Remote path cannot be empty"), _("Error"));
		return;
	}
	if (LocalPath->GetPath().IsEmpty()) {
		wxMessageBox(_("Local path cannot be empty"), _("Error"));
		return;
	}
	if (!wxFileName::DirExists(LocalPath->GetPath())) {
		wxMessageBox(_("Local path must exist and must be a directory"), _("Error"));
		return;
	}
	if (!RemotePath->GetValue().EndsWith(wxT("/"))) {
		wxMessageBox(_("Remote path must end with a forward slash directory separator '/' "), _("Error"));
		return;
	}
	if (RemotePath->GetValue().Contains(wxT("\\"))) {
		wxMessageBox(_("Remote path must use forward slash '/'  as directory separators"), _("Error"));
		return;
	}
	LocalPathString = LocalDir.GetPathWithSep();
	EndModal(wxOK);
}


const wxEventType t4p::EVENT_DEBUGGER_LOG = wxNewEventType();
const wxEventType t4p::EVENT_DEBUGGER_RESPONSE = wxNewEventType();
const wxEventType t4p::EVENT_DEBUGGER_CMD = wxNewEventType();
const wxEventType t4p::EVENT_DEBUGGER_SHOW_FULL = wxNewEventType();

BEGIN_EVENT_TABLE(t4p::DebuggerFeatureClass, t4p::FeatureClass)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_READY, t4p::DebuggerFeatureClass::OnAppReady)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_EXIT, t4p::DebuggerFeatureClass::OnAppExit)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::DebuggerFeatureClass::OnPreferencesSaved)
	EVT_APP_FILE_OPEN(t4p::DebuggerFeatureClass::OnAppFileOpened)
	EVT_APP_FILE_CLOSED(t4p::DebuggerFeatureClass::OnAppFileClosed)
	EVT_STC_MODIFIED(wxID_ANY, t4p::DebuggerFeatureClass::OnStyledTextModified)
	EVT_STC_MARGINCLICK(wxID_ANY, t4p::DebuggerFeatureClass::OnMarginClick)
	
	EVT_MENU(t4p::MENU_DEBUGGER + 0, t4p::DebuggerFeatureClass::OnStartDebugger)
	EVT_MENU(t4p::MENU_DEBUGGER + 2, t4p::DebuggerFeatureClass::OnStepInto)
	EVT_MENU(t4p::MENU_DEBUGGER + 3, t4p::DebuggerFeatureClass::OnStepOver)
	EVT_MENU(t4p::MENU_DEBUGGER + 4, t4p::DebuggerFeatureClass::OnStepOut)
	EVT_MENU(t4p::MENU_DEBUGGER + 5, t4p::DebuggerFeatureClass::OnContinue)
	EVT_MENU(t4p::MENU_DEBUGGER + 6, t4p::DebuggerFeatureClass::OnContinueToCursor)
	EVT_MENU(t4p::MENU_DEBUGGER + 7, t4p::DebuggerFeatureClass::OnToggleBreakpoint)
	EVT_MENU(t4p::MENU_DEBUGGER + 8, t4p::DebuggerFeatureClass::OnStopDebugger)
	EVT_MENU(t4p::MENU_DEBUGGER + 9, t4p::DebuggerFeatureClass::OnFinish)
	EVT_MENU(t4p::MENU_DEBUGGER + 10, t4p::DebuggerFeatureClass::OnGoToExecutingLine)

	EVT_TOOL(t4p::MENU_DEBUGGER + 0, t4p::DebuggerFeatureClass::OnStartDebugger)
	EVT_TOOL(t4p::MENU_DEBUGGER + 2, t4p::DebuggerFeatureClass::OnStepInto)
	EVT_TOOL(t4p::MENU_DEBUGGER + 3, t4p::DebuggerFeatureClass::OnStepOver)
	EVT_TOOL(t4p::MENU_DEBUGGER + 4, t4p::DebuggerFeatureClass::OnStepOut)
	EVT_TOOL(t4p::MENU_DEBUGGER + 9, t4p::DebuggerFeatureClass::OnFinish)
	EVT_TOOL(t4p::MENU_DEBUGGER + 10, t4p::DebuggerFeatureClass::OnGoToExecutingLine)
	

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
	EVT_COMMAND(wxID_ANY, t4p::EVENT_DEBUGGER_SHOW_FULL, t4p::DebuggerFeatureClass::OnDebuggerShowFull)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::DebuggerVariablePanelClass, DebuggerVariablePanelGeneratedClass)
	EVT_DATAVIEW_ITEM_EXPANDING(wxID_ANY, t4p::DebuggerVariablePanelClass::OnVariableExpanding)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::DebuggerBreakpointPanelClass, DebuggerBreakpointPanelGeneratedClass)
	EVT_DATAVIEW_ITEM_ACTIVATED(wxID_ANY, t4p::DebuggerBreakpointPanelClass::OnItemActivated)
	EVT_DATAVIEW_ITEM_VALUE_CHANGED(wxID_ANY, t4p::DebuggerBreakpointPanelClass::OnItemValueChanged)
END_EVENT_TABLE()
