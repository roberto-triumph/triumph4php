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
#include <language/DbgpEventClass.h>
#include <features/wxformbuilder/DebuggerFeatureForms.h>
#include <wx/thread.h>
#include <wx/stc/stc.h>
#include <vector>
#include <queue>

namespace t4p {

// forward declaration, defined below
class DebuggerPanelClass;

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
 * event generated by the main thread when the user wants
 * to see the full, untruncated contents of a variable.
 * By default, the debugger shows at most a certain amount
 * so that a big variable won't slow the debugger down.
 * This event is of type wxCommandEvent, GetString() 
 * will return a concatenation of the DbgpPropertyClass 
 * FullName, newline, Key of the variable to get.
 */
extern const wxEventType EVENT_DEBUGGER_SHOW_FULL;

/**
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

	DebuggerFeatureClass(t4p::AppClass& app);

	void AddNewMenu(wxMenuBar* menuBar);
	
	void AddToolBarItems(wxAuiToolBar* bar);
	
	void AddPreferenceWindow(wxBookCtrlBase* parent);
	
	void LoadPreferences(wxConfigBase* base);
	
	/**
	 * issues a debugger command to get the children of the given
	 * property. The command is asynchronous; this method exits 
	 * immediately, and the command is queued to be sent to 
	 * the debugger over a socket on a background thread.
	 *
	 * See section 7.13 of the xdbgp protocol docs.
	 */
	void CmdPropertyGetChildren(const t4p::DbgpPropertyClass& prop);

	/**
	 * Performs all necessary actions to remove the breakpoint.
	 * Removes it from the breakpoints list
	 * Removes its marker (in the code control margin)
	 * Sends the breakpoint_remove command to the debug engine
	 *  
	 * issues a debugger command to remove the given breakpoint. 
	 * The command is asynchronous; this method exits 
	 * immediately, and the command is queued to be sent to 
	 * the debugger over a socket on a background thread.
	 *
	 * See section 7.6.4 of the xdbgp protocol docs.
	 */
	void BreakpointRemove(const t4p::BreakpointWithHandleClass& breakpointWithHandle);

	/**
	 * Performs all necessary actions to disable the breakpoint.
	 * disables it in the breakpoints list
	 * Removes its marker (in the code control margin)
	 * Sends the breakpoint_update command to the debug engine
	 *
	 * Issues a debugger command to disable the given breakpoint.
	 * The command is asynchronous; this method exits
	 * immediately, and the command is queued to be sent to 
	 * the debugger over a socket on a background thread.
	 *
	 * See section 7.6.3 of the xdbgp protocol docs.
	*/
	void BreakpointDisable(const t4p::BreakpointWithHandleClass& breakpointWithHandle);

	/**
	 * Performs all necessary actions to enable the breakpoint.
	 * Enables it in the breakpoints list
	 * Adds its marker (in the code control margin)
	 * Sends the breakpoint_update command to the debug engine
	 *
	 * Issues a debugger command to enable the given breakpoint.
	 * The command is asynchronous; this method exits
	 * immediately, and the command is queued to be sent to 
	 * the debugger over a socket on a background thread.
	 *
	 * See section 7.6.3 of the xdbgp protocol docs.
	*/
	void BreakpointEnable(const t4p::BreakpointWithHandleClass& breakpointWithHandle);

	/**
	 * Goes to the file and line number of the breakpoint. Opens the file
	 * if the file is not opened.
	 *
	 * @param breakpointWithHandle the breakpoint to go to. 
	 */
	void BreakpointGoToSource(const t4p::BreakpointWithHandleClass& breakpointWithHandle);

private:

	// handlers for menu items
	void OnStartDebugger(wxCommandEvent& event);
	void OnStopDebugger(wxCommandEvent& event);
	void OnStepInto(wxCommandEvent& event);
	void OnStepOver(wxCommandEvent& event);
	void OnStepOut(wxCommandEvent& event);
	void OnContinue(wxCommandEvent& event);
	void OnContinueToCursor(wxCommandEvent& event);
	void OnFinish(wxCommandEvent& event);
	void OnToggleBreakpoint(wxCommandEvent& event);
	void OnGoToExecutingLine(wxCommandEvent& event);

	/**
	 * turn on or off a debugger breakpoint at the currently 
	 * focused file at the given line.
	 *
	 * @param codeCtrl the code control to toggle the breakpoint from
	 * @param lineNumber 1-based line number
	 */
	void ToggleBreakpointAtLine(t4p::CodeControlClass* codeCtrl, int lineNumber);

	/**
	 * when the user clicks on the margin of a code control, toggle
	 * a debugger breakpoint at the line that was clicked on
	 */
	void OnMarginClick(wxStyledTextEvent& event);

	// handlers to begin/stop listening on server
	void OnAppReady(wxCommandEvent& event);
	void OnAppExit(wxCommandEvent& event);
	void OnAppFileOpened(t4p::CodeControlEventClass& event);
	void OnAppFileClosed(t4p::CodeControlEventClass& event);

	/**
	 * updates the GUI to alert the user that the script has finished running
	 * and that the debugger session has closed.
	 */
	void ResetDebugger();
	
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
	
	/**
	 * handler for saving preferences to disk
	 */
	void OnPreferencesSaved(wxCommandEvent& event);

	// below are the handlers for all responses from the debug engine
	void OnDbgpInit(t4p::DbgpInitEventClass& event);
	void OnDbgpError(t4p::DbgpErrorEventClass& event);
	void OnDbgpStatus(t4p::DbgpStatusEventClass& event);
	void OnDbgpFeatureGet(t4p::DbgpFeatureGetEventClass& event);
	void OnDbgpFeatureSet(t4p::DbgpFeatureSetEventClass& event);
	void OnDbgpContinue(t4p::DbgpContinueEventClass& event);
	void OnDbgpBreakpointSet(t4p::DbgpBreakpointSetEventClass& event);
	void OnDbgpBreakpointGet(t4p::DbgpBreakpointGetEventClass& event);
	void OnDbgpBreakpointUpdate(t4p::DbgpBreakpointUpdateEventClass& event);
	void OnDbgpBreakpointRemove(t4p::DbgpBreakpointRemoveEventClass& event);
	void OnDbgpBreakpointList(t4p::DbgpBreakpointListEventClass& event);
	void OnDbgpStackDepth(t4p::DbgpStackDepthEventClass& event);
	void OnDbgpStackGet(t4p::DbgpStackGetEventClass& event);
	void OnDbgpContextNames(t4p::DbgpContextNamesEventClass& event);
	void OnDbgpContextGet(t4p::DbgpContextGetEventClass& event);
	void OnDbgpPropertyGet(t4p::DbgpPropertyGetEventClass& event);
	void OnDbgpPropertyValue(t4p::DbgpPropertyValueEventClass& event);
	void OnDbgpPropertySet(t4p::DbgpPropertySetEventClass& event);
	void OnDbgpBreak(t4p::DbgpBreakEventClass& event);
	void OnDbgpEval(t4p::DbgpEvalEventClass& event);

	/**
	 * this is an additional debug engine handler, we log the exact
	 * response we get back from the debug engine 
	 */
	void OnDebuggerLog(wxThreadEvent& event);

	/**
	 * send a command to the debug engine.  This is an asynchronous
	 * operation; we send the command over to the background thread,
	 * and the background thread sends the command to the debug engine.
	 *
	 * @param cmd the command string to send, built by DbgpCommandClass
	 */
	void PostCmd(std::string cmd);
	
	/**
	 * we want to get notified of code changes so that
	 * if a brerakpoint is now located at a new line
	 * we update the breakpoints properly
	 */
	void OnStyledTextModified(wxStyledTextEvent& event);

	/**
	 * event handler for the show full event, when
	 * the user wants to see the full variable 
	 * contents.
	 */
	void OnDebuggerShowFull(wxCommandEvent& event);

	/**
	 * holds the background thread that the server action is running
	 * in
	 */
	t4p::RunningThreadsClass RunningThreads;

	/**
	 * this event sink is used to "pass" debug commands such as step out,
	 * step into and step over, to the background thread.
	 */
	t4p::EventSinkLockerClass EventSinkLocker;

	/**
	 * used to build the xdebug commands. using a class-wide instance
	 * because xdebug commands require a unique "transactionId"
	 */
	t4p::DbgpCommandClass Cmd;
	
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
	 * TRUE if triumph is listening for incoming xdebug
	 * connections
	 */
	bool IsDebuggerServerActive;
	
	/**
	 * TRUE if the user modified the port number in the preferences
	 * window. We need to know this so that we can restart the
	 * debugger server on the new port.
	 */
	bool WasDebuggerPortChanged;
	
	/**
	 * The value of the debugger port before it was modified. Used in
	 * conjunction with WasDebuggerPortChanged to stop the debugger
	 * server socket when the user changes ports.
	 */
	int WasDebuggerPort;
	
	DECLARE_EVENT_TABLE()
};

/**
 * shows a log of the data that was sent by the debugger engine xdebug
 */
class DebuggerLogPanelClass : public DebuggerLogPanelGeneratedClass {

public:

	DebuggerLogPanelClass(wxWindow* parent);

	void Append(const wxString& text);

private:

	void OnClearButton(wxCommandEvent& event);
};

/**
 * shows the current run-time stack (the function/method that is being 
 * run along with all of the functions/methods that called it).
 */
class DebuggerStackPanelClass : public DebuggerStackPanelGeneratedClass {

public:

	DebuggerStackPanelClass(wxWindow* parent, int id);

	void ShowStack(const std::vector<t4p::DbgpStackClass>& stack);

	void ClearStack();

private:

};

class DebuggerVariablePanelClass : public DebuggerVariablePanelGeneratedClass {

public:

	DebuggerVariablePanelClass(wxWindow* parent, int id, t4p::DebuggerFeatureClass& feature);

	void AddVariables(const std::vector<t4p::DbgpPropertyClass>& variables);

	void ClearVariables();

	void UpdateVariable(const t4p::DbgpPropertyClass& variable);

private:

	void OnVariableExpanding(wxDataViewEvent& event);

	/**
	 * we will use the feature to get the entire variable properties when
	 * a variable is being expanded.
	 */
	t4p::DebuggerFeatureClass& Feature;

	DECLARE_EVENT_TABLE()
};

/**
 * this class represents a single "node" in the variable data view model. a node is
 * equal to a single DbgpPropertyClass instance, along with pointers to its parent
 * property and children properties.
 * 
 * A node will own its children.
 */
class DebuggerVariableNodeClass {

public:

	t4p::DbgpPropertyClass Property;

	/**
	 * this instance will not own the parent pointer
	 */
	t4p::DebuggerVariableNodeClass* Parent;

	/**
	 * this instance WILL own the children pointers and
	 * will delete them 
	 */
	std::vector<t4p::DebuggerVariableNodeClass*> Children;

	/**
	 * this instance will not own the parent pointer
	 */
	DebuggerVariableNodeClass(t4p::DebuggerVariableNodeClass* parent);

	/**
	 * this instance will not own the parent pointer
	 */
	DebuggerVariableNodeClass(t4p::DebuggerVariableNodeClass* parent, const t4p::DbgpPropertyClass& prop);

	~DebuggerVariableNodeClass();

	/**
	 * deletes all of the items in Children, and adds newChildren to the 
	 * Children vector. This instance will now own the pointers in newChildren.
	 */
	void ReplaceChildren(const std::vector<t4p::DbgpPropertyClass>& newChildren, wxDataViewModel* model);
};

/**
 * this class is used as the "data view model" it is a requirement
 * because we are using a wxDataViewCtrl to show variables
 */
class DebuggerVariableModelClass : public wxDataViewModel {

public:

	DebuggerVariableModelClass();

	// Override this so the control can query the child items of an item.     
	virtual unsigned int GetChildren(const wxDataViewItem& item, wxDataViewItemArray& children) const;
 
	// Override this to indicate the number of columns in the model. 
	virtual unsigned int GetColumnCount() const;
 	
	// Override this to indicate what type of data is stored in the column specified by col. 
	virtual wxString GetColumnType(unsigned int col) const;
 
	// Override this to indicate which wxDataViewItem representing the parent of item or an 
	// invalid wxDataViewItem if the root item is the parent item. 
	virtual wxDataViewItem GetParent(const wxDataViewItem& item) const;

	virtual bool HasContainerColumns(const wxDataViewItem& item) const;
 	
	 // Override this to indicate the value of item. 
	virtual void GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned int col) const;
 
	// Override this to indicate of item is a container, i.e. if it can have child items. 
	virtual bool IsContainer(const wxDataViewItem& item) const;
 	
	// This gets called in order to set a value in the data model. 
	virtual bool SetValue(const wxVariant &variant, const wxDataViewItem &item, unsigned int col);
 
	void SetVariables(const std::vector<t4p::DbgpPropertyClass>& variables, wxDataViewItem& updatedItem);

	/**
	 * recurses down through the model's nodes until it finds the variable that was updated.
	 * this method will then remove the found node's children and replace them with the
	 * given variables children. 
	 *
	 * this method is typically called when the user expands a variable to see the variable's
	 * sub-properties.
	 *
	 * @param variable the new variable properties; these come from the debug engine
	 * @param updatedItem [out] will be set to the item that was updated.
	 */
	void UpdateVariable(const t4p::DbgpPropertyClass& variable, wxDataViewItem& updatedItem);

private:

	t4p::DebuggerVariableNodeClass RootVariable;
};

/**
 * shows the user the breakpoints that the user has added; the user can bulk delete / 
 * bulk disable the breakpoints by using this panel.
 */
class DebuggerBreakpointPanelClass : public DebuggerBreakpointPanelGeneratedClass {

public:

	DebuggerBreakpointPanelClass(wxWindow* parent, int id, t4p::DebuggerFeatureClass& feature);

	/**
	 * should be called when the user adds a breakpoint. the breakpoint list is redrawn.
	 */
	void RefreshList();

private:

	// button click handlers
	void OnDeleteBreakpoint(wxCommandEvent& event);
	void OnToggleAllBreakpoints(wxCommandEvent& event);

	// list event handlers
	void OnItemActivated(wxDataViewEvent& event);
	void OnItemValueChanged(wxDataViewEvent& event);


	/**
	 * to send the delete/disable breakpoint command to the debug engine
	 */
	t4p::DebuggerFeatureClass& Feature;

	/**
	 * true if all breakpoints are currently enabled. when we toggle,
	 * we disable if all breakpoints are enabled.
	 */
	bool AreAllEnabled;

	DECLARE_EVENT_TABLE()
};

/**
 * shows the bulk of the debug information
 */
class DebuggerPanelClass : public DebuggerPanelGeneratedClass {

public:

	// this class will own these 3 panel pointers
	t4p::DebuggerLogPanelClass* Logger;
	t4p::DebuggerVariablePanelClass* VariablePanel;
	t4p::DebuggerBreakpointPanelClass* BreakpointPanel;

	DebuggerPanelClass(wxWindow* parent, int id, t4p::DebuggerFeatureClass& feature);

private:
};

/**
 * allows the user to edit the options for
 * debugger settings
 */
class DebuggerOptionsPanelClass : public DebuggerOptionsPanelGeneratedClass {
	
public:
	
	DebuggerOptionsPanelClass(wxWindow* parent, t4p::DebuggerOptionsClass& options, bool& wasDebuggerPortChanged);
	
	bool TransferDataFromWindow();
	
private:

	// handlers for source code mapping CRUD
	void OnAddMapping(wxCommandEvent& event);
	void OnEditMapping(wxCommandEvent& event);
	void OnDeleteMapping(wxCommandEvent& event);
	void OnListItemActivated(wxListEvent& event);

	/**
	 * fill the mappings list according to the options
	 */
	void FillMappings();
	
	
	/**
	 * the final set of options that will be 
	 * edited only when the user hits the OK
	 * button
	 */
	t4p::DebuggerOptionsClass& Options;
	
	/**
	 * the options being edited by the user
	 */
	t4p::DebuggerOptionsClass EditedOptions;
	
	/**
	 * will be set to TRUE if the user changed port
	 */
	bool& WasDebuggerPortChanged;
};

/**
 * class that will render the variable value column of the
 * wxDataViewCtrl.
 * It is basically the same as wxDataViewTextRenderer except
 * that it will render a "..." button to enable the user to
 * see a variable's value in its entirety.
 */
class PreviewTextCustomRendererClass : public wxDataViewCustomRenderer {
	
	public:
	
	/**
	 * @param handler the handler will get an event when the
	 *        the user clicks on the show full contents
	 *        icon
	 */
	PreviewTextCustomRendererClass(wxEvtHandler& handler);
	
	/**
	 * draw the variable text on the control
	 */
	bool Render(wxRect cell, wxDC *dc, int state);
	
	/**
	 * get the text size
	 */
	wxSize GetSize() const;
	
	bool GetValue(wxVariant& variant) const;
	
	bool SetValue(const wxVariant& variant);
	
	bool ActivateCell(const wxRect& cell,
		wxDataViewModel* model,
		const wxDataViewItem& item,
		unsigned int col,
		const wxMouseEvent* mouseEvent);

	
private:

	/**
	 * the value to show in the cell
	 */
	wxString Contents;
	
	/**
	 * the image to show next to the content. when the user
	 * clicks on the bitmap, they will see a popup 
	 * dialog with the full contents of the value.
	 */
	wxBitmap MagnifierBitmap;
	
	/**
	 * will be notified of EVENT_DEBUGGER_SHOW_FULL 
	 * when the user clicks on the magnifier bitmap
	 */
	wxEvtHandler& EventHandler;
};

/**
 * This dialog shows the user contents of a variable
 * It is big to accomodate variables with lots of content
 */
class DebuggerFullViewDialogClass : public DebuggerFullViewDialogGeneratedClass {
	
public:

	DebuggerFullViewDialogClass(wxWindow* parent, const wxString& value);
};

/**
 * This class is for adding/edit a local path => remote path
 * mapping.
 */
class DebuggerMappingDialogClass : public DebuggerMappingDialogGeneratedClass {
	
public:

	DebuggerMappingDialogClass(wxWindow* parent, wxString& localPath, wxString& remotePath);
	
private:
	
	void OnCancelButton(wxCommandEvent& event);
	
	void OnOkButton(wxCommandEvent& event);
	
	wxFileName LocalDir;
	
	wxString& LocalPathString;
};

}

#endif