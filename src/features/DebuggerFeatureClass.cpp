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
#include <globals/Assets.h>
#include <widgets/DirPickerValidatorClass.h>
#include <widgets/ListWidget.h>
#include <wx/valgen.h>
#include <Triumph.h>
#include <istream>
#include <string>
#include <algorithm>

// TODO: not sure how breakpoints react when
//       file is edited (and breakpoints moves lines) 
//       but file is then reloaded/discarded

static int ID_PANEL_DEBUGGER = wxNewId();
static int ID_ACTION_DEBUGGER = wxNewId();
static int ID_PANEL_DEBUGGER_STACK = wxNewId();

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
static wxString VariablePreview(const t4p::DbgpPropertyClass& prop, int maxPreviewLength) {
	wxString preview;
	bool isArrayOrObject = false;
	wxString enclosingStart;
	wxString enclosingEnd;
	if (prop.DataType == wxT("array")) {
		isArrayOrObject = true;
		enclosingStart = wxT("[");
		enclosingEnd = wxT("]");
		
	}
	else if (prop.DataType == wxT("object")) {
		isArrayOrObject = true;
		enclosingStart = wxT("{");
		enclosingEnd = wxT("}");
	}
	
	if (isArrayOrObject) {
		if (prop.DataType == wxT("array")) {
			preview += wxString::Format("array(%d) ", prop.NumChildren);
		}
		else if (prop.DataType == wxT("object")) {
			preview += prop.ClassName + wxT(" ");
		}
		preview += enclosingStart;
		for (size_t i = 0; i < prop.ChildProperties.size(); ++i) {
			
			// a child could be a key-value pairs, key => objects.
			// or key => arrays
			if (prop.ChildProperties[i].DataType == wxT("array")) {
				preview += prop.ChildProperties[i].Name + wxT(" => array[ ... ]");
			}
			else if (prop.ChildProperties[i].DataType == wxT("object")) {
				preview += prop.ChildProperties[i].Name + wxT(" => { ... }");
			}
			else if (prop.ChildProperties[i].Name.empty()) {
				
				// special case for the nodes that have not been retrieved from
				// the debug engine yet
				preview += wxT(" ... ");
			}
			else {
				preview += prop.ChildProperties[i].Name;
				preview += wxT(" => ");
				preview += prop.ChildProperties[i].Value;
			}
			if (i < (prop.ChildProperties.size() - 1)) {
				preview += wxT(", ");
			}
		}
		preview += enclosingEnd;
	}
	else {
		preview = prop.Value;
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

/**
 * Returns the DBGP context ID of the given node. context ID is
 * 0 == local variable 1 == global variable
 * @return int the context id
 */ 
static int ContextIdFromItem(wxTreeListCtrl* ctrl, wxTreeListItem item, wxTreeListItem& localRoot, wxTreeListItem& globalRoot) {
	
	// recurse up the parent chain and look for local
	// or global root
	if (!item.IsOk()) {
		return -1;
	}
	wxTreeListItem parent = ctrl->GetItemParent(item);
	if (parent == localRoot) {
		return 0;
	}
	if (parent == globalRoot) {
		return 1;
	}
	return ContextIdFromItem(ctrl, parent, localRoot, globalRoot);
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
, CurrentStackFunction()
, LastStackFunction()
, IsDebuggerSessionActive(false)
, IsDebuggerServerActive(false) 
, WasDebuggerPortChanged(false) 
, WasDebuggerPort(0) {
}

void t4p::DebuggerFeatureClass::AddNewMenu(wxMenuBar* menuBar) {
	wxMenu* menu = new wxMenu();
	menu->Append(t4p::MENU_DEBUGGER + 0, _("Start Listening for Debugger"), 
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
	menu->Append(t4p::MENU_DEBUGGER + 7, _("Toggle Breakpoint\tCTRL+K"), 
		_("Turn on or off a breakpoint at the current line of source code."));

	menuBar->Append(menu, _("Debug"));
}

void t4p::DebuggerFeatureClass::AddViewMenuItems(wxMenu* menu) {
	wxMenu* subMenu = new wxMenu();
	
	subMenu->Append(t4p::MENU_DEBUGGER + 11, _("Variables"));
	subMenu->Append(t4p::MENU_DEBUGGER + 12, _("Breakpoints"));
	subMenu->Append(t4p::MENU_DEBUGGER + 13, _("Eval"));
	subMenu->Append(t4p::MENU_DEBUGGER + 14, _("Log"));
	
	menu->AppendSubMenu(subMenu, _("Debugger"));
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
	panel->LogPanel->Append(event.GetString());
}

void t4p::DebuggerFeatureClass::OnDebuggerSocketError(wxThreadEvent& event) {
	IsDebuggerSessionActive = false;
	IsDebuggerServerActive = false;
	ResetDebugger();
	
	wxWindow* window = FindToolsWindow(ID_PANEL_DEBUGGER);
	if (!window) {
		return;
	}
	t4p::DebuggerPanelClass* panel = (t4p::DebuggerPanelClass*) window;
	panel->LogPanel->Append(event.GetString());
}

void t4p::DebuggerFeatureClass::OnDebuggerListenError(wxThreadEvent& event) {
	IsDebuggerSessionActive = false;
	IsDebuggerServerActive = false;
	ResetDebugger();

	t4p::EditorLogErrorFix(wxString::Format(_("Debugger Error: Could not start listening on port %d"), Options.Port),
		wxString::Format(_("Use netstat to find out the process listening on port %d and stop it\n"), Options.Port)
			+ _("OR Go to Edit ... Preferences ... Debugger and choose a different port to listen on.\n")
			+ _("In this case you will need to change your php.ini setting xdebug.remote_port.")
		);
}

void t4p::DebuggerFeatureClass::OnDebuggerListenStart(wxThreadEvent& event) {
	wxWindow* window = FindToolsWindow(ID_PANEL_DEBUGGER);
	if (!window) {
		return;
	}
	t4p::DebuggerPanelClass* panel = (t4p::DebuggerPanelClass*) window;
	panel->LogPanel->Append(wxString::Format("Triumph has started listening on port %d ...", event.GetInt()));
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
			panel->ResetStatus(IsDebuggerSessionActive);
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

void t4p::DebuggerFeatureClass::CmdPropertyGetChildren(const t4p::DbgpPropertyClass& prop, int contextId) {
	PostCmd(
		Cmd.PropertyGet(0, contextId, prop.FullName, prop.Key)
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
	wxWindow* window = FindToolsWindow(ID_PANEL_DEBUGGER);
	if (!window) {
		t4p::DebuggerPanelClass* panel = new t4p::DebuggerPanelClass(GetToolsNotebook(), ID_PANEL_DEBUGGER, *this);
		panel->ResetStatus(IsDebuggerSessionActive);
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

void t4p::DebuggerFeatureClass::OnDbgpError(t4p::DbgpErrorEventClass& event) {
	if (event.Command == "eval") {
			
		// errors in eval do not kill the entire debug session
		wxWindow* window = FindToolsWindow(ID_PANEL_DEBUGGER);
		if (!window) {
		
			// not sure if we should create the panel again.
			// if the panel is not here it means that the user closed it
			// and wants it closed?
			return;
		}
	
		t4p::DebuggerPanelClass* panel = (t4p::DebuggerPanelClass*) window;
		panel->EvalPanel->AppendError(event.Message);
	}
	else {
		IsDebuggerSessionActive = false;
		ResetDebugger();
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
	
	LastStackFunction = CurrentStackFunction;
	CurrentStackFunction = event.Stack[0].Where;
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
	panel->ResetStatus(true);
	
	// if we changed scope, need to clear out the old variables
	if (LastStackFunction != CurrentStackFunction && event.ContextId == 0) {
		panel->VariablePanel->ClearLocalVariables();
		panel->VariablePanel->SetLocalVariables(event.Properties);
	}
	else if (LastStackFunction != CurrentStackFunction && event.ContextId == 1) {
		panel->VariablePanel->ClearGlobalVariables();
		panel->VariablePanel->SetGlobalVariables(event.Properties);
	}
	else if (event.ContextId == 0) {
		panel->VariablePanel->UpdateLocalVariables(event.Properties);
	}
	else if (event.ContextId == 1) {
		panel->VariablePanel->UpdateGlobalVariables(event.Properties);
	}
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
	panel->VariablePanel->VariableAddChildren(event.Property);
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
	wxWindow* window = FindToolsWindow(ID_PANEL_DEBUGGER);
	if (!window) {
		
		// not sure if we should create the panel again.
		// if the panel is not here it means that the user closed it
		// and wants it closed?
		return;
	}
	t4p::DebuggerPanelClass* panel = (t4p::DebuggerPanelClass*) window;
	panel->EvalPanel->AppendResults(event.Property);
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
		stackPanel->ResetStatus(false);
	}
	
	wxWindow* panelWindow = FindToolsWindow(ID_PANEL_DEBUGGER);
	if (panelWindow) {
		t4p::DebuggerPanelClass* panel = (t4p::DebuggerPanelClass*) panelWindow;
		panel->VariablePanel->ClearLocalVariables();
		panel->VariablePanel->ClearGlobalVariables();
		panel->ResetStatus(false);
	}
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

void t4p::DebuggerFeatureClass::OnViewDebuggerVariables(wxCommandEvent& event) {
	wxWindow* panelWindow = FindToolsWindow(ID_PANEL_DEBUGGER);
	t4p::DebuggerPanelClass* panel;
	if (panelWindow) {
		panel = (t4p::DebuggerPanelClass*) panelWindow;
	}
	else {
		panel = new t4p::DebuggerPanelClass(GetToolsNotebook(), ID_PANEL_DEBUGGER, *this);
		AddToolsWindow(panel, _("Debugger"));
	}
	panel->ResetStatus(IsDebuggerSessionActive);
	panel->SelectVariablePanel();
}

void t4p::DebuggerFeatureClass::OnViewDebuggerLog(wxCommandEvent& event) {
	wxWindow* panelWindow = FindToolsWindow(ID_PANEL_DEBUGGER);
	t4p::DebuggerPanelClass* panel;
	if (panelWindow) {
		panel = (t4p::DebuggerPanelClass*) panelWindow;
	}
	else {
		panel = new t4p::DebuggerPanelClass(GetToolsNotebook(), ID_PANEL_DEBUGGER, *this);
		AddToolsWindow(panel, _("Debugger"));
	}
	panel->ResetStatus(IsDebuggerSessionActive);
	panel->SelectLoggerPanel();
}

void t4p::DebuggerFeatureClass::OnViewDebuggerBreakpoints(wxCommandEvent& event) {
	wxWindow* panelWindow = FindToolsWindow(ID_PANEL_DEBUGGER);
	t4p::DebuggerPanelClass* panel;
	if (panelWindow) {
		panel = (t4p::DebuggerPanelClass*) panelWindow;
	}
	else {
		panel = new t4p::DebuggerPanelClass(GetToolsNotebook(), ID_PANEL_DEBUGGER, *this);
		AddToolsWindow(panel, _("Debugger"));
	}
	panel->ResetStatus(IsDebuggerSessionActive);
	panel->SelectBreakpointPanel();
}

void t4p::DebuggerFeatureClass::OnViewDebuggerEval(wxCommandEvent& event) {
	wxWindow* panelWindow = FindToolsWindow(ID_PANEL_DEBUGGER);
	t4p::DebuggerPanelClass* panel;
	if (panelWindow) {
		panel = (t4p::DebuggerPanelClass*) panelWindow;
	}
	else {
		panel = new t4p::DebuggerPanelClass(GetToolsNotebook(), ID_PANEL_DEBUGGER, *this);
		AddToolsWindow(panel, _("Debugger"));
	}
	panel->ResetStatus(IsDebuggerSessionActive);
	panel->SelectEvalPanel();
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

	LogPanel = new t4p::DebuggerLogPanelClass(this);
	VariablePanel = new t4p::DebuggerVariablePanelClass(this, wxID_ANY, feature);
	BreakpointPanel = new t4p::DebuggerBreakpointPanelClass(this, wxID_ANY, feature);
	EvalPanel = new t4p::DebuggerEvalPanelClass(this, wxID_ANY, feature);

	Notebook->AddPage(VariablePanel, _("Variables"));
	Notebook->AddPage(BreakpointPanel, _("Breakpoints"));
	Notebook->AddPage(EvalPanel, _("Eval"));
	Notebook->AddPage(LogPanel, _("Logger"));
	
	ResetStatus(false);
}

void t4p::DebuggerPanelClass::ResetStatus(bool active) {
	VariablePanel->ResetStatus(active);
	EvalPanel->ResetStatus(active);
}

void t4p::DebuggerPanelClass::SelectLoggerPanel() {
	Notebook->SetSelection(Notebook->GetPageIndex(LogPanel));
}

void t4p::DebuggerPanelClass::SelectVariablePanel() {
	Notebook->SetSelection(Notebook->GetPageIndex(VariablePanel));
}

void t4p::DebuggerPanelClass::SelectBreakpointPanel() {
	Notebook->SetSelection(Notebook->GetPageIndex(BreakpointPanel));
}

void t4p::DebuggerPanelClass::SelectEvalPanel() {
	Notebook->SetSelection(Notebook->GetPageIndex(EvalPanel));
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
	this->ResetStatus(false);
}

void t4p::DebuggerStackPanelClass::ResetStatus(bool active) {
	if (active) {
		this->StatusLabel->SetLabel(_("Status: Debugging Session active"));
	}
	else { 
		this->StatusLabel->SetLabel(_("Status: Debugging Session inactive"));
	}
	this->Layout();
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

	ResetStatus(true);
}

void t4p::DebuggerStackPanelClass::ClearStack() {
	StackList->DeleteAllItems();
	ResetStatus(false);
}

t4p::DebuggerVariablePanelClass::DebuggerVariablePanelClass(wxWindow* parent, int id, t4p::DebuggerFeatureClass& feature)
: DebuggerVariablePanelGeneratedClass(parent, id) 
, Feature(feature) 
, LocalVariablesRoot()
, GlobalVariablesRoot() {
	VariablesList->AppendColumn(_("Variable Name"), wxCOL_WIDTH_DEFAULT, wxALIGN_LEFT, wxCOL_RESIZABLE);
	VariablesList->AppendColumn(_("Variable Type"), wxCOL_WIDTH_DEFAULT, wxALIGN_LEFT, wxCOL_RESIZABLE);
	VariablesList->AppendColumn(_("Variable Value"), wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxCOL_RESIZABLE);
	
	LocalVariablesRoot = VariablesList->AppendItem(VariablesList->GetRootItem(), _("Local Variables"));
	GlobalVariablesRoot = VariablesList->AppendItem(VariablesList->GetRootItem(), _("Global Variables"));
	
	ResetStatus(false);
}

void t4p::DebuggerVariablePanelClass::ResetStatus(bool active) {
	if (active) {
		this->StatusLabel->SetLabel(_("Status: Debugging Session active"));
	}
	else { 
		this->StatusLabel->SetLabel(_("Status: Debugging Session inactive"));
	}
	this->Layout();
}

static void AppendTreeListItem(wxTreeListCtrl* ctrl, wxTreeListItem& parent, const t4p::DbgpPropertyClass& prop) {
	wxString type = prop.DataType == "object" ? prop.ClassName : prop.DataType;
	wxString preview = VariablePreview(prop, 80);
	
	wxTreeListItem newItem = ctrl->AppendItem(parent, prop.Name);
	ctrl->SetItemText(newItem, 1, type);
	ctrl->SetItemText(newItem, 2, preview);
	
	wxStringClientData* data = new wxStringClientData(prop.FullName);
	ctrl->SetItemData(newItem, data);
	
	if (prop.ChildProperties.size()) {
		std::vector<t4p::DbgpPropertyClass>::const_iterator child;
		for (child = prop.ChildProperties.begin(); child != prop.ChildProperties.end(); ++child) {
			AppendTreeListItem(ctrl, newItem, *child);
		}
	}
	else if (prop.HasChildren) {
		
		// insert a temporary child so that the item has an expand icon
		// in the tree.
		// we add an empty node because we know that the property that has children 
		// its just that the debug engine has not returned it
		// due to hitting the depth limit 
		t4p::DbgpPropertyClass tempProp;
		tempProp.Name = wxT("Loading");
		AppendTreeListItem(ctrl, newItem, tempProp);
	}
}

static void ReplaceTreeListItem(wxTreeListCtrl* ctrl, wxTreeListItem& item, const t4p::DbgpPropertyClass& prop) {
	wxString type = prop.DataType == "object" ? prop.ClassName : prop.DataType;
	wxString preview = VariablePreview(prop, 80);
	ctrl->SetItemText(item, 0, prop.Name);
	ctrl->SetItemText(item, 1, type);
	ctrl->SetItemText(item, 2, preview);
	
	wxStringClientData* data = new wxStringClientData(prop.FullName);
	ctrl->SetItemData(item, data);
	
	if (prop.ChildProperties.size()) {
		std::vector<t4p::DbgpPropertyClass>::const_iterator child;
		for (child = prop.ChildProperties.begin(); child != prop.ChildProperties.end(); ++child) {
			AppendTreeListItem(ctrl, item, *child);
		}
	}
	else if (prop.HasChildren) {
		
		// insert a temporary child so that the item has an expand icon
		// in the tree.
		// we add an empty node because we know that the property that has children 
		// its just that the debug engine has not returned it
		// due to hitting the depth limit 
		t4p::DbgpPropertyClass tempProp;
		tempProp.Name = wxT("Loading");
		AppendTreeListItem(ctrl, item, tempProp);
	}
}

static wxTreeListItem FindTreeListItemByFullName(wxTreeListCtrl* ctrl, const wxString& fullName) {
	for (wxTreeListItem item = ctrl->GetFirstItem(); item.IsOk(); item = ctrl->GetNextItem(item)) {
		wxStringClientData* data = (wxStringClientData*) ctrl->GetItemData(item);
		if (data && data->GetData() == fullName) {
			return item;
		}
	}
	return wxTreeListItem();
}

static void DeleteChildrenTreeListItems(wxTreeListCtrl* ctrl, wxTreeListItem& parent) {
	wxTreeListItem item = ctrl->GetFirstChild(parent);
	while (item.IsOk()) {
		wxTreeListItem toDel(item);
		item = ctrl->GetNextSibling(item);
		ctrl->DeleteItem(toDel);
	}
}

void t4p::DebuggerVariablePanelClass::SetLocalVariables(const std::vector<t4p::DbgpPropertyClass>& variables) {
	std::vector<t4p::DbgpPropertyClass>::const_iterator it;
	for (it = variables.begin(); it != variables.end(); ++it) {
		AppendTreeListItem(VariablesList, LocalVariablesRoot, *it);
	}
	VariablesList->Expand(LocalVariablesRoot);;
}

void t4p::DebuggerVariablePanelClass::UpdateLocalVariables(const std::vector<t4p::DbgpPropertyClass>& variables) {
	std::vector<t4p::DbgpPropertyClass>::const_iterator it;
	for (it = variables.begin(); it != variables.end(); ++it) {
		wxTreeListItem item = FindTreeListItemByFullName(VariablesList, it->FullName);
		if (item.IsOk()) {
			DeleteChildrenTreeListItems(VariablesList, item);
			ReplaceTreeListItem(VariablesList, item, *it);
		}
		else {
			AppendTreeListItem(VariablesList, LocalVariablesRoot, *it);
		}
	}
}

void t4p::DebuggerVariablePanelClass::ClearLocalVariables() {
	DeleteChildrenTreeListItems(VariablesList, LocalVariablesRoot);
}

void t4p::DebuggerVariablePanelClass::SetGlobalVariables(const std::vector<t4p::DbgpPropertyClass>& variables) {
	std::vector<t4p::DbgpPropertyClass>::const_iterator it;
	for (it = variables.begin(); it != variables.end(); ++it) {
		AppendTreeListItem(VariablesList, GlobalVariablesRoot, *it);
	}
	VariablesList->Expand(GlobalVariablesRoot);
	
	ResetStatus(true);
	
	VariablesList->SetColumnWidth(0, wxCOL_WIDTH_DEFAULT);
	VariablesList->SetColumnWidth(1, wxCOL_WIDTH_DEFAULT);
	VariablesList->SetColumnWidth(2, wxCOL_WIDTH_AUTOSIZE);
}

void t4p::DebuggerVariablePanelClass::UpdateGlobalVariables(const std::vector<t4p::DbgpPropertyClass>& variables) {
	std::vector<t4p::DbgpPropertyClass>::const_iterator it;
	for (it = variables.begin(); it != variables.end(); ++it) {
		wxTreeListItem item = FindTreeListItemByFullName(VariablesList, it->FullName);
		if (item.IsOk()) {
			DeleteChildrenTreeListItems(VariablesList, item);
			ReplaceTreeListItem(VariablesList, item, *it);
		}
		else {
			AppendTreeListItem(VariablesList, GlobalVariablesRoot, *it);
		}
	}
}

void t4p::DebuggerVariablePanelClass::ClearGlobalVariables() {
	DeleteChildrenTreeListItems(VariablesList, GlobalVariablesRoot);
	
	StatusLabel->SetLabel(wxT("Status: Debugging session not active"));
	this->Layout();
}

void t4p::DebuggerVariablePanelClass::OnVariableExpanding(wxTreeListEvent& event) {
	wxTreeListItem item = event.GetItem();
	if (!item.GetID()) {
		return;
	}

	// "object" type nodes have an extra child that holds the classname. xdebug does not 
	// count that as part of the NumChildren but it does return the extra property
	// when we loaded this property we did not get all of it.
	// fetch all of it now
	bool doFetch = false;
	wxTreeListItem child = VariablesList->GetFirstChild(item);
	if (child.IsOk()) {
		wxString text = VariablesList->GetItemText(child, 0);
		if (text == wxT("Loading")) {
			doFetch = true;
		}
	}
	if (doFetch) {
		
		// when we fill the data view ctrl, we add an empty node when we get
		// a property that has children but the debug engine has not returned it
		// due to hitting the depth limit
		int contextId = ContextIdFromItem(VariablesList, item, LocalVariablesRoot, GlobalVariablesRoot);
		if (contextId >= 0) {
			wxStringClientData* clientData = (wxStringClientData*)VariablesList->GetItemData(item);
			if (clientData) {
				t4p::DbgpPropertyClass prop;
				prop.FullName = clientData->GetData();
				Feature.CmdPropertyGetChildren(prop, contextId);
			}
		}
	}
}

void t4p::DebuggerVariablePanelClass::OnVariableActivated(wxTreeListEvent& event) {
	wxTreeListItem item = event.GetItem();
	if (!item.IsOk()) {
		return;
	}
	wxStringClientData* data = (wxStringClientData*)VariablesList->GetItemData(item);
	if (data) {
		wxString var = data->GetData();
		wxCommandEvent evt(t4p::EVENT_DEBUGGER_SHOW_FULL, wxID_ANY);
		evt.SetString(var);
		evt.SetInt(ContextIdFromItem(VariablesList, item, LocalVariablesRoot, GlobalVariablesRoot));
		
		wxPostEvent(&Feature, evt);
	}
}

void t4p::DebuggerVariablePanelClass::VariableAddChildren(const t4p::DbgpPropertyClass& variable) {
	
	// iterate through all variables, lookup by full name
	wxTreeListItem itemToReplace = FindTreeListItemByFullName(VariablesList, variable.FullName);
	
	wxASSERT_MSG(itemToReplace.IsOk(), wxT("item was not found: ") + variable.FullName);
	
	wxTreeListItem firstChildItem = VariablesList->GetFirstChild(itemToReplace);
	
	wxASSERT_MSG(firstChildItem.IsOk(), wxT("item must have a child ") + variable.FullName);
	
	// we found the item to add children to. note that the item
	// already has 1 child (the temp "loading")
	std::vector<t4p::DbgpPropertyClass>::const_iterator child;
	for (child = variable.ChildProperties.begin(); child != variable.ChildProperties.end(); ++child) {
		if (child == variable.ChildProperties.begin()) {
			ReplaceTreeListItem(VariablesList, firstChildItem, *child);
		}
		else {
			AppendTreeListItem(VariablesList, itemToReplace, *child);
		}
	}
	
	// update the parent property preview, since we now know sub-properties
	VariablesList->SetItemText(itemToReplace, 2, VariablePreview(variable, 80));
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

t4p::DebuggerEvalPanelClass::DebuggerEvalPanelClass(wxWindow* parent, int id, t4p::DebuggerFeatureClass& feature)
: DebuggerEvalPanelGeneratedClass(parent, id)
, Feature(feature)
{
	CodeCtrl = new t4p::CodeControlClass(
		ExprCodePanel, 
		Feature.App.Preferences.CodeControlOptions, 
		&Feature.App.Globals,
		Feature.App.EventSink,
		wxID_ANY
	);
	CodeCtrl->SetDocumentMode(t4p::CodeControlClass::PHP);
	ExprSizer->Add(CodeCtrl, 1, wxALL | wxEXPAND, 5);
	
	EvalButton->SetBitmap(t4p::BitmapImageAsset(wxT("debugger-eval")));
	ClearButton->SetBitmap(t4p::BitmapImageAsset(wxT("eraser")));
	Splitter->SetSashPosition(0);
	ResetStatus(false);
	
	InitialCode = t4p::CharToWx(
		"<?php \n"
		"// enter code here to send to Xdebug to be executed.\n"
		"// Hitting CTRL+ENTER will execute the code.\n\n"
	);
	CodeCtrl->AppendText(InitialCode);
	
	t4p::CodeControlEventClass evt(t4p::EVENT_APP_FILE_NEW, CodeCtrl);
	Feature.App.EventSink.Publish(evt);
	
	// we want to capture CTRL+ENTER
	// so we bind on KeyDown
	CodeCtrl->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(t4p::DebuggerEvalPanelClass::OnCodeKeyDown), NULL, this);
	
	wxPlatformInfo platform;
	int os = platform.GetOperatingSystemId();
	
	//ATTN: different OSs have different fonts
	wxString fontName;
	if (os == wxOS_WINDOWS_NT) {
		fontName = wxT("Courier New");
	}
	else {
		
		// default font: some websites say Monospace is a good programming font
		fontName = wxT("Monospace");
	}
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL | wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_NORMAL, false,
				fontName);
	ExprResult->SetFont(font);
}

t4p::DebuggerEvalPanelClass::~DebuggerEvalPanelClass() {
	CodeCtrl->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(t4p::DebuggerEvalPanelClass::OnCodeKeyDown), NULL, this);
}

void t4p::DebuggerEvalPanelClass::ResetStatus(bool active) {
	if (active) {
		this->StatusLabel->SetLabel(_("Status: Debugging Session active"));
	}
	else { 
		this->StatusLabel->SetLabel(_("Status: Debugging Session inactive"));
	}
	this->Layout();
}

void t4p::DebuggerEvalPanelClass::AppendResults(const t4p::DbgpPropertyClass& prop) {
	this->StatusLabel->SetLabel(_("Status: Debugging Session active"));
	this->Layout();
	
	PrettyPrint(prop);
	ExprResult->AppendText(wxT("\n"));
}

void t4p::DebuggerEvalPanelClass::AppendError(const wxString& error) {
	this->StatusLabel->SetLabel(_("Debugger Status: Active"));
	this->Layout();
	
	ExprResult->AppendText(wxT("Error: ") + error);
	ExprResult->AppendText(wxT("\n"));
}

void t4p::DebuggerEvalPanelClass::PrettyPrint(const t4p::DbgpPropertyClass& prop) {
	if (prop.DataType != wxT("object") && prop.DataType != wxT("array")) {
		ExprResult->AppendText(wxT("   => (") + prop.DataType +wxT(") ") + prop.Value);
		return;
	}
	bool isArray = prop.DataType == wxT("array");
	if (isArray) {
		ExprResult->AppendText(wxString::Format(wxT("   => (array[%d]) ["), prop.NumChildren));
	}
	else {
		ExprResult->AppendText(wxT("   => (") + prop.ClassName + wxT(") {"));
	}
	for (size_t i = 0; i < prop.ChildProperties.size(); ++i) {
		wxString childValue = prop.ChildProperties[i].Value;
		if (prop.ChildProperties[i].DataType == "object") {
			childValue = prop.ChildProperties[i].ClassName + wxT(" { ... }");
		}
		else if (prop.ChildProperties[i].DataType == "array") {
			childValue = wxString::Format(wxT("(array[%d])"), prop.ChildProperties[i].NumChildren) + wxT(" [ ... ]");
		}
		if (i == 0) {
			ExprResult->AppendText(wxT("\n        ") + prop.ChildProperties[i].Name + wxT(" => ") + childValue);
		}
		else {
			ExprResult->AppendText(wxT("\n      , ") + prop.ChildProperties[i].Name + wxT(" => ") + childValue);
		}
	}
	
	if (isArray) {
		ExprResult->AppendText(wxT("\n   ]\n\n"));
	}
	else {
		ExprResult->AppendText(wxT("\n   }\n\n"));
	}
}

void t4p::DebuggerEvalPanelClass::OnClearClick(wxCommandEvent& event)
{
	ExprResult->Clear();
}

void t4p::DebuggerEvalPanelClass::OnEvalClick(wxCommandEvent& event)
{
	wxString code = CodeCtrl->GetText();
	if (code.Find(InitialCode) == 0) {
		code = code.Mid(InitialCode.length());
	}
	ExprResult->AppendText(code);
	ExprResult->AppendText(wxT("\n"));
	
	Feature.CmdEvaluate(code);
}

void t4p::DebuggerEvalPanelClass::OnCodeKeyDown(wxKeyEvent& event) {
	if (event.GetModifiers() & WXK_ALT && event.GetKeyCode() == WXK_RETURN) {
		wxString code = CodeCtrl->GetText();
		if (code.Find(InitialCode) == 0) {
			code = code.Mid(InitialCode.length());
		}
		ExprResult->AppendText(code);
		ExprResult->AppendText(wxT("\n"));
		
		Feature.CmdEvaluate(code);
	}
	event.Skip();
}


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
	EVT_MENU(t4p::MENU_DEBUGGER + 11, t4p::DebuggerFeatureClass::OnViewDebuggerVariables)
	EVT_MENU(t4p::MENU_DEBUGGER + 12, t4p::DebuggerFeatureClass::OnViewDebuggerBreakpoints)
	EVT_MENU(t4p::MENU_DEBUGGER + 13, t4p::DebuggerFeatureClass::OnViewDebuggerEval)
	EVT_MENU(t4p::MENU_DEBUGGER + 14, t4p::DebuggerFeatureClass::OnViewDebuggerLog)

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
	EVT_DEBUGGER_SOCKET_ERROR(ID_ACTION_DEBUGGER, t4p::DebuggerFeatureClass::OnDebuggerSocketError)
	EVT_DEBUGGER_LISTEN_ERROR(ID_ACTION_DEBUGGER, t4p::DebuggerFeatureClass::OnDebuggerListenError)
	EVT_DEBUGGER_LISTEN(ID_ACTION_DEBUGGER, t4p::DebuggerFeatureClass::OnDebuggerListenStart)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_DEBUGGER_SHOW_FULL, t4p::DebuggerFeatureClass::OnDebuggerShowFull)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::DebuggerVariablePanelClass, DebuggerVariablePanelGeneratedClass)
	EVT_TREELIST_ITEM_EXPANDING(wxID_ANY, t4p::DebuggerVariablePanelClass::OnVariableExpanding)
	EVT_TREELIST_ITEM_ACTIVATED(wxID_ANY, t4p::DebuggerVariablePanelClass::OnVariableActivated)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::DebuggerBreakpointPanelClass, DebuggerBreakpointPanelGeneratedClass)
	EVT_DATAVIEW_ITEM_ACTIVATED(wxID_ANY, t4p::DebuggerBreakpointPanelClass::OnItemActivated)
	EVT_DATAVIEW_ITEM_VALUE_CHANGED(wxID_ANY, t4p::DebuggerBreakpointPanelClass::OnItemValueChanged)
END_EVENT_TABLE()
