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
 * @copyright  2015 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef T4P_DEBUGGERVIEWCLASS_H__
#define T4P_DEBUGGERVIEWCLASS_H__

#include <features/views/FeatureViewClass.h>
#include <features/wxformbuilder/DebuggerFeatureForms.h>
#include <features/DebuggerFeatureClass.h>
#include <language/DbgpEventClass.h>

namespace t4p {

class DebuggerViewClass : public t4p::FeatureViewClass {

public:

	DebuggerViewClass(t4p::DebuggerFeatureClass& feature);
	
	void AddNewMenu(wxMenuBar* menuBar);
	
	void AddViewMenuItems(wxMenu* menu);
	
	void AddToolBarItems(wxAuiToolBar* bar);
	
	void AddPreferenceWindow(wxBookCtrlBase* parent);
	
private:

	t4p::DebuggerFeatureClass& Feature;
	
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
	
	/**
	 * checks to see if debugger options / port was modified
	 * if so, restart the debugger
	 */
	void OnPreferencesSaved(wxCommandEvent& event);
	
	/**
	 * in this handler we will update the breakpoints list
	 */
	void OnBreakpointRefresh(wxCommandEvent& event);
	
	/**
	 * in this handler we will open the debugger panel
	 * and update the status.
	 */
	void OnDebuggerOpenPanel(wxCommandEvent& event);
	
	// menu handlers
	void OnViewDebuggerVariables(wxCommandEvent& event);
	void OnViewDebuggerLog(wxCommandEvent& event);
	void OnViewDebuggerBreakpoints(wxCommandEvent& event);
	void OnViewDebuggerEval(wxCommandEvent& event);
	void OnToggleBreakpoint(wxCommandEvent& event);

	
	// debugger event handlers; update the debugger panels
	void OnDbgpError(t4p::DbgpErrorEventClass& event);
	void OnDbgpStackGet(t4p::DbgpStackGetEventClass& event);
	void OnDbgpContextGet(t4p::DbgpContextGetEventClass& event);
	void OnDbgpPropertyGet(t4p::DbgpPropertyGetEventClass& event);
	void OnDbgpEval(t4p::DbgpEvalEventClass& event);
	void OnDbgpPropertyValue(t4p::DbgpPropertyValueEventClass& event);
	
	/**
	 * this is an additional debug engine handler, we log a listen
	 * error
	 */
	void OnDebuggerListenError(wxThreadEvent& event);
	
	/**
	 * this is an additional debug engine handler, we log the exact
	 * response we get back from the debug engine 
	 */
	void OnDebuggerLog(wxThreadEvent& event);
	
	/**
	 * this is an additional debug engine handler, we log a socket
	 * error
	 */
	void OnDebuggerSocketError(wxThreadEvent& event);
	
	/**
	 * this is an additional debug engine handler, we log a success
	 * message when Triumph successfully start listening on
	 * the port.
	 */
	void OnDebuggerListenStart(wxThreadEvent& event);
	
	/**
	 * when the user clicks on the margin of a code control, toggle
	 * a debugger breakpoint at the line that was clicked on
	 */
	void OnMarginClick(wxStyledTextEvent& event);
	
	/**
	 * we want to get notified of code changes so that
	 * if a breakpoint is now located at a new line
	 * we update the breakpoints properly
	 */
	void OnStyledTextModified(wxStyledTextEvent& event);
	
	DECLARE_EVENT_TABLE()
};

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
	
	/**
	 * update the label to say the the debugger is / is no longer running
	 * @param active bool if TRUE status will be set as active
	 */
	void ResetStatus(bool active);

private:

};

class DebuggerVariablePanelClass : public DebuggerVariablePanelGeneratedClass {

public:

	DebuggerVariablePanelClass(wxWindow* parent, int id, t4p::DebuggerFeatureClass& feature);

	void SetLocalVariables(const std::vector<t4p::DbgpPropertyClass>& variables);
	void UpdateLocalVariables(const std::vector<t4p::DbgpPropertyClass>& variables);
	void ClearLocalVariables();
	
	void SetGlobalVariables(const std::vector<t4p::DbgpPropertyClass>& variables);
	void UpdateGlobalVariables(const std::vector<t4p::DbgpPropertyClass>& variables);
	void ClearGlobalVariables();

	void VariableAddChildren(const t4p::DbgpPropertyClass& variable);
	
	/**
	 * update the label to say the the debugger is / is no longer running
	 * @param active bool if TRUE status will be set as active
	 */
	void ResetStatus(bool active);

private:

	void OnVariableExpanding(wxTreeListEvent& event);
	void OnVariableActivated(wxTreeListEvent& event);

	/**
	 * we will use the feature to get the entire variable properties when
	 * a variable is being expanded.
	 */
	t4p::DebuggerFeatureClass& Feature;
	
	wxTreeListItem LocalVariablesRoot;
	
	wxTreeListItem GlobalVariablesRoot;

	DECLARE_EVENT_TABLE()
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
 * The eval panel allows the user to enter any arbritary expression and
 * it will get sent to xdebug to be evaluated. User can only enter
 * 1 expression at a time.
 * 
 */
class DebuggerEvalPanelClass : public DebuggerEvalPanelGeneratedClass {

public:
	
	DebuggerEvalPanelClass(wxWindow* parent, int id, t4p::DebuggerFeatureClass& feature);
	
	~DebuggerEvalPanelClass();
	
	/**
	 * append the results of an eval'ed expression into the results
	 * text area
	 */
	void AppendResults(const t4p::DbgpPropertyClass& prop);
	
	/**
	 * appends an error string into the results.  This will usually
	 * happen when the user enters an invalid expression
	 */
	void AppendError(const wxString& error);
	
	/**
	 * update the label to say the the debugger is / is no longer running
	 * @param active bool if TRUE status will be set as active
	 */
	void ResetStatus(bool active);
	
private:

	void OnEvalClick(wxCommandEvent& event);
	void OnClearClick(wxCommandEvent& event);
	void OnCode(wxStyledTextEvent& event);
	void OnCmdRun(wxCommandEvent& event);
	void OnCmdComplete(wxCommandEvent& event);
	
	void PrettyPrint(const t4p::DbgpPropertyClass& prop);

	t4p::DebuggerFeatureClass& Feature;
	
	t4p::CodeControlClass* CodeCtrl;
	
	wxString InitialCode;
	
	DECLARE_EVENT_TABLE()
	
};

/**
 * shows the bulk of the debug information
 */
class DebuggerPanelClass : public DebuggerPanelGeneratedClass {

public:

	// this class will own these 3 panel pointers
	t4p::DebuggerLogPanelClass* LogPanel;
	t4p::DebuggerVariablePanelClass* VariablePanel;
	t4p::DebuggerBreakpointPanelClass* BreakpointPanel;
	t4p::DebuggerEvalPanelClass* EvalPanel;

	DebuggerPanelClass(wxWindow* parent, int id, t4p::DebuggerFeatureClass& feature);
	
	// bring the various panels to the forefront
	void SelectLoggerPanel();
	void SelectVariablePanel();
	void SelectBreakpointPanel();
	void SelectEvalPanel();
	
	/**
	 * update the label to say the the debugger is / is no longer running
	 * @param active bool if TRUE status will be set as active
	 */
	void ResetStatus(bool active);

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
