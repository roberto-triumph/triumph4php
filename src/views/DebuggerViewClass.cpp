/**
 * @copyright  2015 Roberto Perpuly
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
#include "views/DebuggerViewClass.h"
#include <wx/valgen.h>
#include <map>
#include <vector>
#include "actions/DebuggerServerActionClass.h"
#include "code_control/CodeControlClass.h"
#include "globals/Assets.h"
#include "globals/Errors.h"
#include "globals/GlobalsClass.h"
#include "globals/Number.h"
#include "language_php/DbgpEventClass.h"
#include "Triumph.h"
#include "widgets/DirPickerValidatorClass.h"
#include "widgets/ListWidget.h"
#include "widgets/TreeList.h"

static const int ID_PANEL_DEBUGGER = wxNewId();
static const int ID_PANEL_DEBUGGER_STACK = wxNewId();
static const int ID_EVAL_PANEL_RUN = wxNewId();
static const int ID_EVAL_PANEL_CODE_COMPLETE = wxNewId();

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
    } else if (prop.DataType == wxT("object")) {
        isArrayOrObject = true;
        enclosingStart = wxT("{");
        enclosingEnd = wxT("}");
    }

    if (isArrayOrObject) {
        if (prop.DataType == wxT("array")) {
            preview += wxString::Format("array(%d) ", prop.NumChildren);
        } else if (prop.DataType == wxT("object")) {
            preview += prop.ClassName + wxT(" ");
        }
        preview += enclosingStart;
        for (size_t i = 0; i < prop.ChildProperties.size(); ++i) {
            // a child could be a key-value pairs, key => objects.
            // or key => arrays
            if (prop.ChildProperties[i].DataType == wxT("array")) {
                preview += prop.ChildProperties[i].Name + wxT(" => array[ ... ]");
            } else if (prop.ChildProperties[i].DataType == wxT("object")) {
                preview += prop.ChildProperties[i].Name + wxT(" => { ... }");
            } else if (prop.ChildProperties[i].Name.empty()) {
                // special case for the nodes that have not been retrieved from
                // the debug engine yet
                preview += wxT(" ... ");
            } else {
                preview += prop.ChildProperties[i].Name;
                preview += wxT(" => ");
                preview += prop.ChildProperties[i].Value;
            }
            if (i < (prop.ChildProperties.size() - 1)) {
                preview += wxT(", ");
            }
        }
        preview += enclosingEnd;
    } else {
        preview = prop.Value;
    }

    // truncate to desired length
    bool isTruncated = !t4p::NumberLessThan(maxPreviewLength, preview.length())
                       && !t4p::NumberEqualTo(maxPreviewLength, preview.length());
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
    } else if (prop.HasChildren) {
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
    } else if (prop.HasChildren) {
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
        wxStringClientData* data = reinterpret_cast<wxStringClientData*>(ctrl->GetItemData(item));
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

t4p::DebuggerViewClass::DebuggerViewClass(t4p::DebuggerFeatureClass& feature)
    : FeatureViewClass()
    , Feature(feature)
    , WasDebuggerPortChanged(false)
    , WasDebuggerPort(0) {
    feature.RunningThreads.AddEventHandler(this);
}

t4p::DebuggerViewClass::~DebuggerViewClass() {
    Feature.RunningThreads.RemoveEventHandler(this);
}

void t4p::DebuggerViewClass::AddNewMenu(wxMenuBar* menuBar) {
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

void t4p::DebuggerViewClass::AddViewMenuItems(wxMenu* menu) {
    wxMenu* subMenu = new wxMenu();

    subMenu->Append(t4p::MENU_DEBUGGER + 11, _("Variables"));
    subMenu->Append(t4p::MENU_DEBUGGER + 12, _("Breakpoints"));
    subMenu->Append(t4p::MENU_DEBUGGER + 13, _("Eval"));
    subMenu->Append(t4p::MENU_DEBUGGER + 14, _("Log"));

    menu->AppendSubMenu(subMenu, _("Debugger"));
}

void t4p::DebuggerViewClass::AddToolBarItems(wxAuiToolBar* bar) {
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

void t4p::DebuggerViewClass::AddPreferenceWindow(wxBookCtrlBase* parent) {
    WasDebuggerPortChanged = false;
    WasDebuggerPort = Feature.Options.Port;
    t4p::DebuggerOptionsPanelClass* panel =  new t4p::DebuggerOptionsPanelClass(
        parent, Feature.Options, WasDebuggerPortChanged);

    parent->AddPage(panel, _("Debugger"));
}

void t4p::DebuggerViewClass::OnDebuggerLog(wxThreadEvent& event) {
    wxWindow* window = FindToolsWindow(ID_PANEL_DEBUGGER);
    if (!window) {
        return;
    }
    t4p::DebuggerPanelClass* panel = (t4p::DebuggerPanelClass*) window;
    panel->LogPanel->Append(event.GetString());
}


void t4p::DebuggerViewClass::OnPreferencesSaved(wxCommandEvent& event) {
    if (WasDebuggerPortChanged) {
        // need to shutdown the server and listen on the new port
        Feature.RestartDebugger(WasDebuggerPort);
    }
    WasDebuggerPortChanged = false;
    WasDebuggerPort = Feature.Options.Port;
}

void t4p::DebuggerViewClass::OnBreakpointRefresh(wxCommandEvent& event) {
    wxWindow* window = FindToolsWindow(ID_PANEL_DEBUGGER);
    if (window) {
        t4p::DebuggerPanelClass* panel = (t4p::DebuggerPanelClass*) window;
        panel->BreakpointPanel->RefreshList();
    }
}

void t4p::DebuggerViewClass::OnDebuggerOpenPanel(wxCommandEvent& event) {
    wxWindow* window = FindToolsWindow(ID_PANEL_DEBUGGER);
    t4p::DebuggerPanelClass* panel = NULL;
    if (!window) {
        panel = new t4p::DebuggerPanelClass(
            GetToolsNotebook(), ID_PANEL_DEBUGGER, Feature, *this);
        AddToolsWindow(panel, _("Debugger"));
    } else {
        panel = (t4p::DebuggerPanelClass*)window;
    }
    panel->ResetStatus(Feature.IsDebuggerSessionActive);
}

void t4p::DebuggerViewClass::OnDbgpError(t4p::DbgpErrorEventClass& event) {
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
    } else {
        Feature.ResetDebugger();
        std::vector<t4p::CodeControlClass*> codeCtrls = AllCodeControls();
        for (size_t i = 0; i < codeCtrls.size(); ++i) {
            t4p::CodeControlClass* codeCtrl = codeCtrls[i];
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
    }
}

void t4p::DebuggerViewClass::OnDbgpStackGet(t4p::DbgpStackGetEventClass& event) {
    if (event.Stack.empty()) {
        return;
    }

    // lets convert remote paths to local paths
    Feature.LocalizeStackFilePaths(event.Stack);
    Feature.SetCurrentStack(event.Stack);

    // open the file
    t4p::OpenFileCommandEventClass openEvt(
        event.Stack[0].Filename, -1, -1,
        event.Stack[0].LineNumber);
    Feature.App.EventSink.Publish(openEvt);

    t4p::CodeControlClass* codeCtrl = GetCurrentCodeControl();
    if (codeCtrl) {
        codeCtrl->ExecutionMarkAt(event.Stack[0].LineNumber);
    }
    wxWindow* window = FindOutlineWindow(ID_PANEL_DEBUGGER_STACK);
    t4p::DebuggerStackPanelClass* stackPanel = NULL;
    if (window) {
        stackPanel = (t4p::DebuggerStackPanelClass*) window;
    } else {
        stackPanel = new t4p::DebuggerStackPanelClass(GetOutlineNotebook(), ID_PANEL_DEBUGGER_STACK);
        AddOutlineWindow(stackPanel, _("Stack"));
    }
    stackPanel->ShowStack(event.Stack);
}


void t4p::DebuggerViewClass::OnDbgpContextGet(t4p::DbgpContextGetEventClass& event) {
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
    if (Feature.HasScopeChanged() && event.ContextId == 0) {
        panel->VariablePanel->ClearLocalVariables();
        panel->VariablePanel->SetLocalVariables(event.Properties);
    } else if (Feature.HasScopeChanged() && event.ContextId == 1) {
        panel->VariablePanel->ClearGlobalVariables();
        panel->VariablePanel->SetGlobalVariables(event.Properties);
    } else if (event.ContextId == 0) {
        panel->VariablePanel->UpdateLocalVariables(event.Properties);
    } else if (event.ContextId == 1) {
        panel->VariablePanel->UpdateGlobalVariables(event.Properties);
    }
}

void t4p::DebuggerViewClass::OnDbgpPropertyGet(t4p::DbgpPropertyGetEventClass& event) {
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

void t4p::DebuggerViewClass::OnDbgpPropertyValue(t4p::DbgpPropertyValueEventClass& event) {
    t4p::DebuggerFullViewDialogClass dialog(GetMainWindow(), event.Value);
    dialog.ShowModal();
}

void t4p::DebuggerViewClass::OnDebuggerListenError(wxThreadEvent& event) {
    Feature.ResetDebugger();

    t4p::EditorLogErrorFix(wxString::Format(_("Debugger Error: Could not start listening on port %d"), Feature.Options.Port),
                           wxString::Format(_("Use netstat to find out the process listening on port %d and stop it\n"), Feature.Options.Port)
                           + _("OR Go to Edit ... Preferences ... Debugger and choose a different port to listen on.\n")
                           + _("In this case you will need to change your php.ini setting xdebug.remote_port."));
}

void t4p::DebuggerViewClass::OnDbgpEval(t4p::DbgpEvalEventClass& event) {
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

void t4p::DebuggerViewClass::OnViewDebuggerVariables(wxCommandEvent& event) {
    wxWindow* panelWindow = FindToolsWindow(ID_PANEL_DEBUGGER);
    t4p::DebuggerPanelClass* panel;
    if (panelWindow) {
        panel = (t4p::DebuggerPanelClass*) panelWindow;
    } else {
        panel = new t4p::DebuggerPanelClass(
            GetToolsNotebook(), ID_PANEL_DEBUGGER, Feature, *this);
        AddToolsWindow(panel, _("Debugger"));
    }
    panel->ResetStatus(Feature.IsDebuggerSessionActive);
    panel->SelectVariablePanel();
}

void t4p::DebuggerViewClass::OnViewDebuggerLog(wxCommandEvent& event) {
    wxWindow* panelWindow = FindToolsWindow(ID_PANEL_DEBUGGER);
    t4p::DebuggerPanelClass* panel;
    if (panelWindow) {
        panel = (t4p::DebuggerPanelClass*) panelWindow;
    } else {
        panel = new t4p::DebuggerPanelClass(
            GetToolsNotebook(), ID_PANEL_DEBUGGER, Feature, *this);
        AddToolsWindow(panel, _("Debugger"));
    }
    panel->ResetStatus(Feature.IsDebuggerSessionActive);
    panel->SelectLoggerPanel();
}

void t4p::DebuggerViewClass::OnViewDebuggerBreakpoints(wxCommandEvent& event) {
    wxWindow* panelWindow = FindToolsWindow(ID_PANEL_DEBUGGER);
    t4p::DebuggerPanelClass* panel;
    if (panelWindow) {
        panel = (t4p::DebuggerPanelClass*) panelWindow;
    } else {
        panel = new t4p::DebuggerPanelClass(
            GetToolsNotebook(), ID_PANEL_DEBUGGER, Feature, *this);
        AddToolsWindow(panel, _("Debugger"));
    }
    panel->ResetStatus(Feature.IsDebuggerSessionActive);
    panel->SelectBreakpointPanel();
}

void t4p::DebuggerViewClass::OnViewDebuggerEval(wxCommandEvent& event) {
    wxWindow* panelWindow = FindToolsWindow(ID_PANEL_DEBUGGER);
    t4p::DebuggerPanelClass* panel;
    if (panelWindow) {
        panel = (t4p::DebuggerPanelClass*) panelWindow;
    } else {
        panel = new t4p::DebuggerPanelClass(
            GetToolsNotebook(), ID_PANEL_DEBUGGER, Feature, *this);
        AddToolsWindow(panel, _("Debugger"));
    }
    panel->ResetStatus(Feature.IsDebuggerSessionActive);
    panel->SelectEvalPanel();
}

void t4p::DebuggerViewClass::OnDebuggerSocketError(wxThreadEvent& event) {
    Feature.ResetDebugger();
    wxWindow* window = FindToolsWindow(ID_PANEL_DEBUGGER);
    if (!window) {
        return;
    }
    t4p::DebuggerPanelClass* panel = (t4p::DebuggerPanelClass*) window;
    panel->LogPanel->Append(event.GetString());
}

void t4p::DebuggerViewClass::OnDebuggerListenStart(wxThreadEvent& event) {
    wxWindow* window = FindToolsWindow(ID_PANEL_DEBUGGER);
    if (!window) {
        return;
    }
    t4p::DebuggerPanelClass* panel = (t4p::DebuggerPanelClass*) window;
    panel->LogPanel->Append(wxString::Format("Triumph has started listening on port %d ...", event.GetInt()));
}

void t4p::DebuggerViewClass::OnContinueToCursor(wxCommandEvent& event) {
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

    Feature.PostCmd(
        Feature.Cmd.BreakpointRunToCursor(filename, lineNumber));
    Feature.PostCmd(
        Feature.Cmd.Run());
}

void t4p::DebuggerViewClass::BreakpointRemove(const t4p::BreakpointWithHandleClass& breakpointWithHandle) {
    // if the breakpoint is located in an opened file and has a marker
    // lets remove the marker.  note that the file may not be open and that's
    // not unexpected.
    t4p::CodeControlClass* codeCtrl = FindCodeControl(breakpointWithHandle.Breakpoint.Filename);
    if (codeCtrl) {
        codeCtrl->BreakpointRemove(breakpointWithHandle.Breakpoint.LineNumber);
    }

    // remove the breakpoint from this list
    std::vector<t4p::BreakpointWithHandleClass>::iterator it = Feature.Breakpoints.begin();
    while (it != Feature.Breakpoints.end()) {
        if (it->Breakpoint.Filename == breakpointWithHandle.Breakpoint.Filename &&
                it->Breakpoint.LineNumber == breakpointWithHandle.Breakpoint.LineNumber) {
            it = Feature.Breakpoints.erase(it);
            break;
        } else {
            ++it;
        }
    }

    Feature.PostCmd(
        Feature.Cmd.BreakpointRemove(breakpointWithHandle.Breakpoint.BreakpointId));

    // store the breakpoints to disk
    Feature.SaveConfig();
}

void t4p::DebuggerViewClass::BreakpointDisable(const t4p::BreakpointWithHandleClass& breakpointWithHandle) {
    // if the breakpoint is located in an opened file and has a marker
    // lets remove the marker.  note that the file may not be open and that's
    // not unexpected.
    t4p::CodeControlClass* codeCtrl = FindCodeControl(breakpointWithHandle.Breakpoint.Filename);
    if (codeCtrl) {
        codeCtrl->BreakpointRemove(breakpointWithHandle.Breakpoint.LineNumber);
    }

    // set the breakpoint as disabled
    std::vector<t4p::BreakpointWithHandleClass>::iterator it = Feature.Breakpoints.begin();
    while (it != Feature.Breakpoints.end()) {
        if (it->Breakpoint.Filename == breakpointWithHandle.Breakpoint.Filename &&
                it->Breakpoint.LineNumber == breakpointWithHandle.Breakpoint.LineNumber) {
            it->Breakpoint.IsEnabled = false;
            break;
        } else {
            ++it;
        }
    }

    Feature.PostCmd(
        Feature.Cmd.BreakpointDisable(breakpointWithHandle.Breakpoint.BreakpointId));

    Feature.SaveConfig();
}

void t4p::DebuggerViewClass::BreakpointEnable(const t4p::BreakpointWithHandleClass& breakpointWithHandle) {
    // if the breakpoint is located in an opened file and does not have a marker
    // lets add the marker.  note that the file may not be open and that's
    // not unexpected.
    int newHandle = -1;
    t4p::CodeControlClass* codeCtrl = FindCodeControl(breakpointWithHandle.Breakpoint.Filename);
    if (codeCtrl) {
        codeCtrl->BreakpointMarkAt(breakpointWithHandle.Breakpoint.LineNumber, newHandle);
    }

    // set the breakpoint as enabled
    std::vector<t4p::BreakpointWithHandleClass>::iterator it = Feature.Breakpoints.begin();
    while (it != Feature.Breakpoints.end()) {
        if (it->Breakpoint.Filename == breakpointWithHandle.Breakpoint.Filename &&
                it->Breakpoint.LineNumber == breakpointWithHandle.Breakpoint.LineNumber) {
            it->Breakpoint.IsEnabled = true;
            it->Handle = newHandle;
            break;
        } else {
            ++it;
        }
    }

    Feature.PostCmd(
        Feature.Cmd.BreakpointEnable(breakpointWithHandle.Breakpoint.BreakpointId));

    Feature.SaveConfig();
}

void t4p::DebuggerViewClass::OnMarginClick(wxStyledTextEvent& event) {
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
    int lineNumber = codeCtrl->LineFromPosition(event.GetPosition()) + 1;
    bool removed = Feature.RemoveBreakpointAtLine(codeCtrl->GetFileName(), lineNumber);
    if (removed) {
        codeCtrl->BreakpointRemove(lineNumber);
    } else {
        int handle = 0;
        codeCtrl->BreakpointMarkAt(lineNumber, handle);
        Feature.AddBreakpointAtLine(codeCtrl->GetFileName(), handle, lineNumber);
    }

    wxWindow* window = FindToolsWindow(ID_PANEL_DEBUGGER);
    if (window) {
        t4p::DebuggerPanelClass* panel = (t4p::DebuggerPanelClass*) window;
        panel->BreakpointPanel->RefreshList();
    }
}

void t4p::DebuggerViewClass::OnStyledTextModified(wxStyledTextEvent& event) {
    int mask = wxSTC_MOD_INSERTTEXT | wxSTC_MOD_DELETETEXT;
    if (event.GetModificationType() & mask) {
        // lets update the location of all of the breakpoints in this file
        t4p::CodeControlClass* ctrl = (t4p::CodeControlClass*)event.GetEventObject();
        if (!ctrl->IsNew()) {
            wxString ctrlFileName = ctrl->GetFileName();
            std::vector<t4p::BreakpointWithHandleClass>::iterator it;
            bool changed = false;
            for (it = Feature.Breakpoints.begin(); it != Feature.Breakpoints.end(); ++it) {
                if (it->Breakpoint.Filename == ctrlFileName) {
                    int newLineNumber = ctrl->BreakpointGetLine(it->Handle);
                    Feature.UpdateBreakpointLineNumber(ctrlFileName, it->Handle, newLineNumber);
                    changed = true;
                }
            }
            if (changed) {
                wxWindow* window = FindToolsWindow(ID_PANEL_DEBUGGER);
                if (window) {
                    t4p::DebuggerPanelClass* panel = (t4p::DebuggerPanelClass*) window;
                    panel->BreakpointPanel->RefreshList();
                }
            }
        }
    }
}

void t4p::DebuggerViewClass::OnToggleBreakpoint(wxCommandEvent& event) {
    t4p::CodeControlClass* codeCtrl = GetCurrentCodeControl();
    if (!codeCtrl) {
        return;
    }
    int lineNumber = codeCtrl->LineFromPosition(codeCtrl->GetCurrentPos()) + 1;
    bool removed = Feature.RemoveBreakpointAtLine(codeCtrl->GetFileName(), lineNumber);
    if (removed) {
        codeCtrl->BreakpointRemove(lineNumber);
    } else {
        int handle = 0;
        codeCtrl->BreakpointMarkAt(lineNumber, handle);
        Feature.AddBreakpointAtLine(codeCtrl->GetFileName(), handle, lineNumber);
    }

    wxWindow* window = FindToolsWindow(ID_PANEL_DEBUGGER);
    if (window) {
        t4p::DebuggerPanelClass* panel = (t4p::DebuggerPanelClass*) window;
        panel->BreakpointPanel->RefreshList();
    }
}

void t4p::DebuggerViewClass::OnStartDebugger(wxCommandEvent& event) {
    Feature.StartDebugger(true);
}

void t4p::DebuggerViewClass::OnStepInto(wxCommandEvent& event) {
    Feature.StepInto();
}

void t4p::DebuggerViewClass::OnStepOver(wxCommandEvent& event) {
    Feature.StepOver();
}
void t4p::DebuggerViewClass::OnStepOut(wxCommandEvent& event) {
    Feature.StepOut();
}
void t4p::DebuggerViewClass::OnContinue(wxCommandEvent& event) {
    Feature.Continue();
}
void t4p::DebuggerViewClass::OnStopDebugger(wxCommandEvent& event) {
    Feature.StopDebugger(Feature.Options.Port);
}
void t4p::DebuggerViewClass::OnFinish(wxCommandEvent& event) {
    Feature.Finish();
}

void t4p::DebuggerViewClass::OnGoToExecutingLine(wxCommandEvent& event) {
    Feature.GoToExecutingLine();
}

t4p::DebuggerLogPanelClass::DebuggerLogPanelClass(wxWindow* parent)
    : DebuggerLogPanelGeneratedClass(parent, wxID_ANY) {
    ClearButton->SetBitmapLabel(t4p::BitmapImageButtonPrepAsset(wxT("eraser")));
}

void t4p::DebuggerLogPanelClass::Append(const wxString& text) {
    Text->AppendText(text);
    Text->AppendText(wxT("\n"));
}

void t4p::DebuggerLogPanelClass::OnClearButton(wxCommandEvent& event) {
    Text->Clear();
}

t4p::DebuggerPanelClass::DebuggerPanelClass(wxWindow* parent, int id, t4p::DebuggerFeatureClass& feature,
        t4p::DebuggerViewClass& view)
    : DebuggerPanelGeneratedClass(parent, id) {
    Notebook->SetWindowStyle(wxAUI_NB_BOTTOM);

    LogPanel = new t4p::DebuggerLogPanelClass(this);
    VariablePanel = new t4p::DebuggerVariablePanelClass(this, wxID_ANY, feature);
    BreakpointPanel = new t4p::DebuggerBreakpointPanelClass(this, wxID_ANY, feature, view);
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
    } else {
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
    } else {
        this->StatusLabel->SetLabel(_("Status: Debugging Session inactive"));
    }
    this->Layout();
}

void t4p::DebuggerVariablePanelClass::SetLocalVariables(const std::vector<t4p::DbgpPropertyClass>& variables) {
    std::vector<t4p::DbgpPropertyClass>::const_iterator it;
    for (it = variables.begin(); it != variables.end(); ++it) {
        AppendTreeListItem(VariablesList, LocalVariablesRoot, *it);
    }
    VariablesList->Expand(LocalVariablesRoot);

    // auto size all columns, so that the user can see the variables clearly
    t4p::TreeListAutoSizeAllColumns(VariablesList);
}

void t4p::DebuggerVariablePanelClass::UpdateLocalVariables(const std::vector<t4p::DbgpPropertyClass>& variables) {
    std::vector<t4p::DbgpPropertyClass>::const_iterator it;
    for (it = variables.begin(); it != variables.end(); ++it) {
        wxTreeListItem item = FindTreeListItemByFullName(VariablesList, it->FullName);
        if (item.IsOk()) {
            DeleteChildrenTreeListItems(VariablesList, item);
            ReplaceTreeListItem(VariablesList, item, *it);
        } else {
            AppendTreeListItem(VariablesList, LocalVariablesRoot, *it);
        }
    }

    // auto size all columns, so that the user can see the variables clearly
    t4p::TreeListAutoSizeAllColumns(VariablesList);
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

    // auto size all columns, so that the user can see the variables clearly
    t4p::TreeListAutoSizeAllColumns(VariablesList);
}

void t4p::DebuggerVariablePanelClass::UpdateGlobalVariables(const std::vector<t4p::DbgpPropertyClass>& variables) {
    std::vector<t4p::DbgpPropertyClass>::const_iterator it;
    for (it = variables.begin(); it != variables.end(); ++it) {
        wxTreeListItem item = FindTreeListItemByFullName(VariablesList, it->FullName);
        if (item.IsOk()) {
            DeleteChildrenTreeListItems(VariablesList, item);
            ReplaceTreeListItem(VariablesList, item, *it);
        } else {
            AppendTreeListItem(VariablesList, GlobalVariablesRoot, *it);
        }
    }

    // auto size all columns, so that the user can see the variables clearly
    t4p::TreeListAutoSizeAllColumns(VariablesList);
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
            wxStringClientData* clientData = reinterpret_cast<wxStringClientData*>(VariablesList->GetItemData(item));
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
    wxStringClientData* data = reinterpret_cast<wxStringClientData*>(VariablesList->GetItemData(item));
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
        } else {
            AppendTreeListItem(VariablesList, itemToReplace, *child);
        }
    }

    // update the parent property preview, since we now know sub-properties
    VariablesList->SetItemText(itemToReplace, 2, VariablePreview(variable, 80));
}

t4p::DebuggerBreakpointPanelClass::DebuggerBreakpointPanelClass(wxWindow* parent, int id,
        t4p::DebuggerFeatureClass& feature,
        t4p::DebuggerViewClass& view)
    : DebuggerBreakpointPanelGeneratedClass(parent, id)
    , Feature(feature)
    , View(view)
    , AreAllEnabled(false) {
    DeleteBreakpointButton->SetBitmap(t4p::BitmapImageButtonPrepAsset(wxT("breakpoint-delete")));
    ToggleAllBreakpointsButton->SetBitmap(t4p::BitmapImageButtonPrepAsset(wxT("breakpoint-toggle")));

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
    if (!t4p::NumberLessThan(index, BreakpointsList->GetItemCount())) {
        return;
    }
    t4p::BreakpointWithHandleClass toRemove = Feature.Breakpoints[index];
    View.BreakpointRemove(toRemove);

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
            View.BreakpointEnable(*it);
        } else {
            View.BreakpointDisable(*it);
        }

        // uncheck the enabled flag in the widget
        BreakpointsList->SetToggleValue(newValue, row, 0);
        row++;
    }

    AreAllEnabled = newValue;
}

void t4p::DebuggerBreakpointPanelClass::OnItemActivated(wxDataViewEvent& event) {
    int row = BreakpointsList->ItemToRow(event.GetItem());
    if (!t4p::NumberLessThan(row, BreakpointsList->GetItemCount())) {
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
    if (!t4p::NumberLessThan(row, BreakpointsList->GetItemCount())) {
        return;
    }
    bool isEnabled = BreakpointsList->GetToggleValue(row, 0);
    if (isEnabled) {
        View.BreakpointEnable(Feature.Breakpoints[row]);
    } else {
        View.BreakpointDisable(Feature.Breakpoints[row]);
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
    , Feature(feature) {
    CodeCtrl = new t4p::CodeControlClass(
        ExprCodePanel,
        Feature.App.Preferences.CodeControlOptions,
        &Feature.App.Globals,
        Feature.App.EventSink,
        wxID_ANY);
    CodeCtrl->SetFileType(t4p::FILE_TYPE_PHP);
    ExprSizer->Add(CodeCtrl, 1, wxALL | wxEXPAND, 5);

    EvalButton->SetBitmap(t4p::BitmapImageButtonPrepAsset(wxT("debugger-eval")));
    ClearButton->SetBitmap(t4p::BitmapImageButtonPrepAsset(wxT("eraser")));
    Splitter->SetSashPosition(0);
    ResetStatus(false);

    InitialCode = t4p::CharToWx(
                      "<?php \n"
                      "// enter code here to send to Xdebug to be executed.\n"
                      "// Hitting CTRL+ENTER will execute the code.");
    CodeCtrl->AppendText(InitialCode);

    t4p::CodeControlEventClass evt(t4p::EVENT_APP_FILE_NEW, CodeCtrl);
    Feature.App.EventSink.Publish(evt);

    // we want to capture CTRL+ENTER, CTRL+SPACE
    // shortcuts
    // use an accelerator table so that the code completion works
    // inside of this panel and won't get captured by the global menu
    // shortcuts
    wxAcceleratorEntry entries[5];
    entries[0].Set(wxACCEL_CMD, WXK_RETURN, ID_EVAL_PANEL_RUN);
    entries[1].Set(wxACCEL_CMD, WXK_SPACE, ID_EVAL_PANEL_CODE_COMPLETE);
    wxAcceleratorTable table(2, entries);
    CodeCtrl->SetAcceleratorTable(table);

    wxPlatformInfo platform;
    int os = platform.GetOperatingSystemId();

    // ATTN: different OSs have different fonts
    wxString fontName;
    int fontSize = 10;
    if (os == wxOS_WINDOWS_NT) {
        fontName = wxT("Courier New");
        fontSize = 10;
    } else if (os == wxOS_UNIX_LINUX) {
        // default font: some websites say Monospace is a good programming font
        fontName = wxT("Monospace");
        fontSize = 10;
    } else if (os == wxOS_MAC_OSX_DARWIN) {
        fontName = wxT("Monaco");
        fontSize = 12;
    }
    wxFont font(wxFontInfo(fontSize).AntiAliased(true).FaceName(fontName));
    ExprResult->SetFont(font);
}

t4p::DebuggerEvalPanelClass::~DebuggerEvalPanelClass() {
}

void t4p::DebuggerEvalPanelClass::ResetStatus(bool active) {
    if (active) {
        this->StatusLabel->SetLabel(_("Status: Debugging Session active"));
    } else {
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
    } else {
        ExprResult->AppendText(wxT("   => (") + prop.ClassName + wxT(") {"));
    }
    for (size_t i = 0; i < prop.ChildProperties.size(); ++i) {
        wxString childValue = prop.ChildProperties[i].Value;
        if (prop.ChildProperties[i].DataType == "object") {
            childValue = prop.ChildProperties[i].ClassName + wxT(" { ... }");
        } else if (prop.ChildProperties[i].DataType == "array") {
            childValue = wxString::Format(wxT("(array[%d])"), prop.ChildProperties[i].NumChildren) + wxT(" [ ... ]");
        }
        if (i == 0) {
            ExprResult->AppendText(wxT("\n        ") + prop.ChildProperties[i].Name + wxT(" => ") + childValue);
        } else {
            ExprResult->AppendText(wxT("\n      , ") + prop.ChildProperties[i].Name + wxT(" => ") + childValue);
        }
    }

    if (isArray) {
        ExprResult->AppendText(wxT("\n   ]\n\n"));
    } else {
        ExprResult->AppendText(wxT("\n   }\n\n"));
    }
}

void t4p::DebuggerEvalPanelClass::OnClearClick(wxCommandEvent& event) {
    ExprResult->Clear();
}

void t4p::DebuggerEvalPanelClass::OnEvalClick(wxCommandEvent& event) {
    wxString code = CodeCtrl->GetText();
    if (code.Find(InitialCode) == 0) {
        code = code.Mid(InitialCode.length());
    }
    code.Trim().Trim(true);
    ExprResult->AppendText(code);
    ExprResult->AppendText(wxT("\n"));

    Feature.CmdEvaluate(code);
}

void t4p::DebuggerEvalPanelClass::OnCmdRun(wxCommandEvent& event) {
    wxString code = CodeCtrl->GetText();
    if (code.Find(InitialCode) == 0) {
        code = code.Mid(InitialCode.length());
    }
    code.Trim().Trim(true);
    ExprResult->AppendText(code);
    ExprResult->AppendText(wxT("\n"));

    Feature.CmdEvaluate(code);
}

void t4p::DebuggerEvalPanelClass::OnCmdComplete(wxCommandEvent& event) {
    CodeCtrl->HandleAutoCompletion();
}

BEGIN_EVENT_TABLE(t4p::DebuggerViewClass, t4p::FeatureViewClass)
    EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_PREFERENCES_SAVED, t4p::DebuggerViewClass::OnPreferencesSaved)
    EVT_COMMAND(wxID_ANY, t4p::EVENT_DEBUGGER_BREAKPOINT_REFRESH, t4p::DebuggerViewClass::OnBreakpointRefresh)
    EVT_COMMAND(wxID_ANY, t4p::EVENT_DEBUGGER_OPEN_PANEL, t4p::DebuggerViewClass::OnDebuggerOpenPanel)

    EVT_STC_MODIFIED(wxID_ANY, t4p::DebuggerViewClass::OnStyledTextModified)
    EVT_STC_MARGINCLICK(wxID_ANY, t4p::DebuggerViewClass::OnMarginClick)

    EVT_MENU(t4p::MENU_DEBUGGER + 6, t4p::DebuggerViewClass::OnContinueToCursor)
    EVT_MENU(t4p::MENU_DEBUGGER + 7, t4p::DebuggerViewClass::OnToggleBreakpoint)
    EVT_MENU(t4p::MENU_DEBUGGER + 11, t4p::DebuggerViewClass::OnViewDebuggerVariables)
    EVT_MENU(t4p::MENU_DEBUGGER + 12, t4p::DebuggerViewClass::OnViewDebuggerBreakpoints)
    EVT_MENU(t4p::MENU_DEBUGGER + 13, t4p::DebuggerViewClass::OnViewDebuggerEval)
    EVT_MENU(t4p::MENU_DEBUGGER + 14, t4p::DebuggerViewClass::OnViewDebuggerLog)

    EVT_MENU(t4p::MENU_DEBUGGER + 0, t4p::DebuggerViewClass::OnStartDebugger)
    EVT_MENU(t4p::MENU_DEBUGGER + 2, t4p::DebuggerViewClass::OnStepInto)
    EVT_MENU(t4p::MENU_DEBUGGER + 3, t4p::DebuggerViewClass::OnStepOver)
    EVT_MENU(t4p::MENU_DEBUGGER + 4, t4p::DebuggerViewClass::OnStepOut)
    EVT_MENU(t4p::MENU_DEBUGGER + 5, t4p::DebuggerViewClass::OnContinue)
    EVT_MENU(t4p::MENU_DEBUGGER + 8, t4p::DebuggerViewClass::OnStopDebugger)
    EVT_MENU(t4p::MENU_DEBUGGER + 9, t4p::DebuggerViewClass::OnFinish)
    EVT_MENU(t4p::MENU_DEBUGGER + 10, t4p::DebuggerViewClass::OnGoToExecutingLine)

    EVT_TOOL(t4p::MENU_DEBUGGER + 0, t4p::DebuggerViewClass::OnStartDebugger)
    EVT_TOOL(t4p::MENU_DEBUGGER + 2, t4p::DebuggerViewClass::OnStepInto)
    EVT_TOOL(t4p::MENU_DEBUGGER + 3, t4p::DebuggerViewClass::OnStepOver)
    EVT_TOOL(t4p::MENU_DEBUGGER + 4, t4p::DebuggerViewClass::OnStepOut)
    EVT_TOOL(t4p::MENU_DEBUGGER + 9, t4p::DebuggerViewClass::OnFinish)
    EVT_TOOL(t4p::MENU_DEBUGGER + 10, t4p::DebuggerViewClass::OnGoToExecutingLine)


    EVT_DBGP_ERROR(t4p::DebuggerViewClass::OnDbgpError)
    EVT_DBGP_STACKGET(t4p::DebuggerViewClass::OnDbgpStackGet)
    EVT_DBGP_CONTEXTGET(t4p::DebuggerViewClass::OnDbgpContextGet)
    EVT_DBGP_PROPERTYGET(t4p::DebuggerViewClass::OnDbgpPropertyGet)
    EVT_DBGP_PROPERTYVALUE(t4p::DebuggerViewClass::OnDbgpPropertyValue)
    EVT_DBGP_EVAL(t4p::DebuggerViewClass::OnDbgpEval)

    EVT_DEBUGGER_LOG(wxID_ANY, t4p::DebuggerViewClass::OnDebuggerLog)
    EVT_DEBUGGER_SOCKET_ERROR(wxID_ANY, t4p::DebuggerViewClass::OnDebuggerSocketError)
    EVT_DEBUGGER_LISTEN_ERROR(wxID_ANY, t4p::DebuggerViewClass::OnDebuggerListenError)
    EVT_DEBUGGER_LISTEN(wxID_ANY, t4p::DebuggerViewClass::OnDebuggerListenStart)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::DebuggerVariablePanelClass, DebuggerVariablePanelGeneratedClass)
    EVT_TREELIST_ITEM_EXPANDING(wxID_ANY, t4p::DebuggerVariablePanelClass::OnVariableExpanding)
    EVT_TREELIST_ITEM_ACTIVATED(wxID_ANY, t4p::DebuggerVariablePanelClass::OnVariableActivated)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::DebuggerBreakpointPanelClass, DebuggerBreakpointPanelGeneratedClass)
    EVT_DATAVIEW_ITEM_ACTIVATED(wxID_ANY, t4p::DebuggerBreakpointPanelClass::OnItemActivated)
    EVT_DATAVIEW_ITEM_VALUE_CHANGED(wxID_ANY, t4p::DebuggerBreakpointPanelClass::OnItemValueChanged)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::DebuggerEvalPanelClass, DebuggerEvalPanelGeneratedClass)
    EVT_MENU(ID_EVAL_PANEL_CODE_COMPLETE , t4p::DebuggerEvalPanelClass::OnCmdComplete)
    EVT_MENU(ID_EVAL_PANEL_RUN, t4p::DebuggerEvalPanelClass::OnCmdRun)
END_EVENT_TABLE()
