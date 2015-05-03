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
#include <actions/DebuggerServerActionClass.h>
#include <globals/Errors.h>
#include <globals/Number.h>
#include <Triumph.h>
#include <istream>
#include <string>
#include <algorithm>

// TODO: not sure how breakpoints react when
//       file is edited (and breakpoints moves lines) 
//       but file is then reloaded/discarded
static int ID_ACTION_DEBUGGER = wxNewId();

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
, Options()
, IsDebuggerSessionActive(false)
, Cmd()
, RunningThreads() 
, EventSinkLocker() 
, CurrentStackFunction()
, LastStackFunction()
, IsDebuggerServerActive(false) {
}

void t4p::DebuggerFeatureClass::OnAppReady(wxCommandEvent& event) {
	RunningThreads.SetMaxThreads(1);
	RunningThreads.AddEventHandler(this);
	if (Options.DoListenOnAppReady) {
		StartDebugger(false);
	}
}

void t4p::DebuggerFeatureClass::OnAppExit(wxCommandEvent& event) {
	RunningThreads.RemoveEventHandler(this);
	StopDebugger(Options.Port);
}

void t4p::DebuggerFeatureClass::SaveConfig() {
	ConfigStore(wxConfigBase::Get(), Options, Breakpoints);
}

void t4p::DebuggerFeatureClass::LoadPreferences(wxConfigBase* base) {
	ConfigLoad(base, Options, Breakpoints);
}

void t4p::DebuggerFeatureClass::OnPreferencesSaved(wxCommandEvent& event) {
	wxConfigBase* base = wxConfig::Get();
	ConfigStore(base, Options, Breakpoints);
}

void t4p::DebuggerFeatureClass::RestartDebugger(int oldPort) {
		
	// need to shutdown the server and listen on the new port
	StopDebugger(oldPort);
	if (Options.DoListenOnAppReady) {
		StartDebugger(false);
	}
}
void t4p::DebuggerFeatureClass::UpdateBreakpointLineNumber(const wxString& ctrlFileName, int handle, 
	int newLineNumber) {
	std::vector<t4p::BreakpointWithHandleClass>::iterator it;
	for (it = Breakpoints.begin(); it != Breakpoints.end(); ++it) {
		if (it->Breakpoint.Filename == ctrlFileName && it->Handle) {
			
			// only update when line >= 1.  if line == 0 then it means
			// that the entire text has been deleted. this is most likely
			// scenario when the user reloads the file.
			it->Breakpoint.LineNumber = newLineNumber;
		}
	}
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

bool t4p::DebuggerFeatureClass::RemoveBreakpointAtLine(const wxString& fileName, int lineNumber) {

	// if the user clicked on a line that already has a breakpoint, then
	// the user wants to remove the breakpoint
	bool found = false;
	std::vector<t4p::BreakpointWithHandleClass>::iterator it = Breakpoints.begin();
	while (it != Breakpoints.end()) {
		if (it->Breakpoint.LineNumber == lineNumber
			&& it->Breakpoint.Filename == fileName) {
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
		}
		else {
			++it;
		}
	}
	return found;
}

bool t4p::DebuggerFeatureClass::AddBreakpointAtLine(const wxString& fileName, int handle, int lineNumber) {
		bool found = false;
	std::vector<t4p::BreakpointWithHandleClass>::iterator it = Breakpoints.begin();
	while (it != Breakpoints.end()) {
		if (it->Breakpoint.LineNumber == lineNumber
			&& it->Breakpoint.Filename == fileName) {
			found = true;
		}
		++it;
	}
	if (!found) {
		t4p::BreakpointWithHandleClass breakpointWithHandle;
		breakpointWithHandle.Breakpoint.Filename = fileName;	
		breakpointWithHandle.Breakpoint.LineNumber = lineNumber;
		breakpointWithHandle.Breakpoint.IsEnabled = true;
		breakpointWithHandle.Handle = handle;
			
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

		// tell the view that the breakpoints have updated
		wxCommandEvent refreshEvt(t4p::EVENT_DEBUGGER_BREAKPOINT_REFRESH);
		App.EventSink.Post(refreshEvt);
		
		// store the breakpoints to disk
		wxConfigBase* base = wxConfig::Get();
		ConfigStore(base, Options, Breakpoints);
	}	
	return !found;
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
		wxCommandEvent refreshEvt(t4p::EVENT_DEBUGGER_OPEN_PANEL);
		App.EventSink.Post(refreshEvt);
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

void t4p::DebuggerFeatureClass::CmdPropertyGetChildren(const t4p::DbgpPropertyClass& prop, int contextId) {
	PostCmd(
		Cmd.PropertyGet(0, contextId, prop.FullName, prop.Key)
	);
}

void t4p::DebuggerFeatureClass::CmdEvaluate(const wxString& code) {
	PostCmd(
		Cmd.Eval(code)
	);
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
	wxCommandEvent openEvt(t4p::EVENT_DEBUGGER_OPEN_PANEL);
	App.EventSink.Post(openEvt);

	// set the max children of variables to get at once
	// we can handle more than the default
	PostCmd(Cmd.FeatureSet("max_depth", wxString::Format("%d", Options.MaxDepth)));
	PostCmd(Cmd.FeatureSet("max_children", wxString::Format("%d", Options.MaxChildren)));
	
	// now we send commands to the debug engine
	// if the user wants to break at the first line, we
	// want to step into the first line.
	if (Options.DoBreakOnStart) {

		// run stack command first, ContextGet handler needs 
		// to differentiate when scope function has changed
		PostCmd(Cmd.StepInto());
		PostCmd(Cmd.StackGet(0));
		PostCmd(Cmd.ContextNames(0));
		PostCmd(Cmd.ContextGet(0, 0));
		PostCmd(Cmd.ContextGet(0, 1));
		
	}
	else {
		
		// we want the script to run until a breakpoint is
		// hit
		PostCmd(Cmd.Run());
		
		// run stack command first, ContextGet handler needs 
		// to differentiate when scope function has changed
		PostCmd(Cmd.StackGet(0));
		PostCmd(Cmd.ContextNames(0));
		PostCmd(Cmd.ContextGet(0, 0));
		PostCmd(Cmd.ContextGet(0, 1));
	}
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
		PostCmd(Cmd.ContextGet(0, 0));
		PostCmd(Cmd.ContextGet(0, 1));
		
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

void t4p::DebuggerFeatureClass::LocalizeStackFilePaths(std::vector<t4p::DbgpStackClass>& stack) {
	
	// turn the filename that Xdebug returns into a local filesystem filename
	// lets convert remote paths to local paths
	for (size_t i = 0; i < stack.size(); ++i) {
		stack[i].Filename = ToLocalFilename(stack[i].Filename, Options.SourceCodeMappings).GetFullPath();
	}
}

void t4p::DebuggerFeatureClass::SetCurrentStack(std::vector<t4p::DbgpStackClass>& stack) {
	if (stack.empty()) {
		return;
	}
	LastStackFunction = CurrentStackFunction;
	CurrentStackFunction = stack[0].Where;
}

bool t4p::DebuggerFeatureClass::HasScopeChanged() const {
	return LastStackFunction != CurrentStackFunction;
}

void t4p::DebuggerFeatureClass::ResetDebugger() {
	IsDebuggerSessionActive = false;
	IsDebuggerServerActive = false;
	LastStackFunction = wxT("");
	CurrentStackFunction = wxT("");
}

void t4p::DebuggerFeatureClass::PostCmd(std::string cmd) {
	wxThreadEvent evt(t4p::EVENT_DEBUGGER_CMD, wxID_ANY);
	evt.SetString(cmd);
	EventSinkLocker.Post(evt);
}

void t4p::DebuggerFeatureClass::OnDebuggerShowFull(wxCommandEvent& event) {
	
	// the command string contains the property full name 
	// the int contains the context ID
	wxString fullName = event.GetString();
	int contextId = event.GetInt();
	PostCmd(
		Cmd.PropertyValue(0, contextId, fullName, wxT("")) // xdebug does not return key property 
	);
}

const wxEventType t4p::EVENT_DEBUGGER_SHOW_FULL = wxNewEventType();
const wxEventType t4p::EVENT_DEBUGGER_BREAKPOINT_REFRESH = wxNewEventType();
const wxEventType t4p::EVENT_DEBUGGER_OPEN_PANEL = wxNewEventType();

BEGIN_EVENT_TABLE(t4p::DebuggerFeatureClass, t4p::FeatureClass)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_READY, t4p::DebuggerFeatureClass::OnAppReady)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_EXIT, t4p::DebuggerFeatureClass::OnAppExit)
	EVT_APP_FILE_OPEN(t4p::DebuggerFeatureClass::OnAppFileOpened)
	EVT_APP_FILE_CLOSED(t4p::DebuggerFeatureClass::OnAppFileClosed)
	
	EVT_MENU(t4p::MENU_DEBUGGER + 0, t4p::DebuggerFeatureClass::OnStartDebugger)
	EVT_MENU(t4p::MENU_DEBUGGER + 2, t4p::DebuggerFeatureClass::OnStepInto)
	EVT_MENU(t4p::MENU_DEBUGGER + 3, t4p::DebuggerFeatureClass::OnStepOver)
	EVT_MENU(t4p::MENU_DEBUGGER + 4, t4p::DebuggerFeatureClass::OnStepOut)
	EVT_MENU(t4p::MENU_DEBUGGER + 5, t4p::DebuggerFeatureClass::OnContinue)
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
	EVT_DBGP_STATUS(t4p::DebuggerFeatureClass::OnDbgpStatus)
	EVT_DBGP_FEATUREGET(t4p::DebuggerFeatureClass::OnDbgpFeatureGet)
	EVT_DBGP_FEATURESET(t4p::DebuggerFeatureClass::OnDbgpFeatureSet)
	EVT_DBGP_CONTINUE(t4p::DebuggerFeatureClass::OnDbgpContinue)
	EVT_DBGP_BREAKPOINTSET(t4p::DebuggerFeatureClass::OnDbgpBreakpointSet)

	EVT_COMMAND(wxID_ANY, t4p::EVENT_DEBUGGER_SHOW_FULL, t4p::DebuggerFeatureClass::OnDebuggerShowFull)
END_EVENT_TABLE()
