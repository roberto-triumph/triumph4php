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
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <plugins/EditorMessagesPluginClass.h>
#include <MvcEditorErrors.h>

static const int ID_DEBUG_WINDOW = wxNewId();

mvceditor::EditorMessagesPanelClass::EditorMessagesPanelClass(wxWindow* parent, int id)
	: EditorMessagesGeneratedPanelClass(parent, id) {
	int rowCount = Grid->GetNumberRows();
	Grid->DeleteRows(0, rowCount);
	int colCount = Grid->GetNumberCols();
	Grid->DeleteCols(0, colCount);
	
	Grid->AppendCols(4);
	Grid->SetColLabelValue(0, _("Message"));
	Grid->SetColLabelValue(1, _("Fix"));
	Grid->SetColLabelValue(2, _("Severity"));
	Grid->SetColLabelValue(3, _("Time"));
}

void mvceditor::EditorMessagesPanelClass::OnClearButton(wxCommandEvent& event) {
	Grid->ClearGrid();
	int rowCount = Grid->GetNumberRows();
	Grid->DeleteRows(0, rowCount);
}

void mvceditor::EditorMessagesPanelClass::AddMessage(wxLogLevel level, const wxChar* msg, time_t timestamp) {
	
	// put a bound on the number of messages to display
	int maxRows = 1000;
	if (Grid->GetNumberRows() > maxRows) {
		Grid->DeleteRows(0, 1);
	}
	wxString error, 
		fix,
		levelString,
		dateString;
	mvceditor::EditorErrorFix(msg, error, fix);
	switch (level) {
	case wxLOG_Debug:
		levelString = _("Debug");
		break;
	case wxLOG_Error:
		levelString = _("Error");
		break;
	case wxLOG_FatalError:
		levelString = _("Fatal Error");
		break;
	case wxLOG_Info:
		levelString = _("Info");
		break;
	case wxLOG_Message:
		levelString = _("Message");
		break;
	case wxLOG_Trace:
		levelString = _("Trace");
		break;
	case wxLOG_Warning:
		levelString = _("Warning");
		break;
	}
	wxDateTime dateTime(timestamp);
	dateString = dateTime.FormatISOTime();
	if (Grid->AppendRows(1)) {
		Grid->SetCellOverflow(Grid->GetRows() - 1, 0, false);
		Grid->SetCellValue(Grid->GetRows() - 1, 0, error);
		Grid->SetCellValue(Grid->GetRows() - 1, 1, fix);
		Grid->SetCellValue(Grid->GetRows() - 1, 2, levelString);
		Grid->SetCellValue(Grid->GetRows() - 1, 3, dateString);

		// ATTN: make the grid expand to the entire panel
		// not sure if this will render nicely when many messages are shown
		Grid->AutoSize();
		Layout();
	}
}


mvceditor::EditorMessagesPluginClass::EditorMessagesPluginClass()
	: PluginClass() {
	
	// this line is needed so that we get all the wxLogXXX messages
	// pointer will be managed by wxWidgets
	wxLog::SetActiveTarget(new mvceditor::EditorMessagesLoggerClass(*this));
}

void mvceditor::EditorMessagesPluginClass::AddToolsMenuItems(wxMenu *toolsMenu) {
	toolsMenu->Append(mvceditor::MENU_EDITOR_MESSAGES, _("Editor Messages"), 
		_("Editor Messages"));
}

void mvceditor::EditorMessagesPluginClass::OnMenu(wxCommandEvent& event) {
	wxWindow* window = FindToolsWindow(ID_DEBUG_WINDOW);
	mvceditor::EditorMessagesPanelClass* panel = wxDynamicCast(window, mvceditor::EditorMessagesPanelClass);
	if (panel) {
		SetFocusToToolsWindow(window);
	}
	else {
		panel = new mvceditor::EditorMessagesPanelClass(GetToolsNotebook(), ID_DEBUG_WINDOW);
		AddToolsWindow(panel, _("Editor Messages"));
	}
}

void mvceditor::EditorMessagesPluginClass::AddMessage(wxLogLevel level, const wxChar* msg, time_t timestamp) {
	wxWindow* window = FindToolsWindow(ID_DEBUG_WINDOW);
	mvceditor::EditorMessagesPanelClass* panel = wxDynamicCast(window, mvceditor::EditorMessagesPanelClass);
	if (panel) {
		SetFocusToToolsWindow(window);
	}
	else {
		panel = new mvceditor::EditorMessagesPanelClass(GetToolsNotebook(), ID_DEBUG_WINDOW);
		AddToolsWindow(panel, _("Editor Messages"));
	}
	panel->AddMessage(level, msg, timestamp);
}

void mvceditor::EditorMessagesPluginClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_EDITOR_MESSAGES] = wxT("Editor-Messages");
	AddDynamicCmd(menuItemIds, shortcuts);
}

mvceditor::EditorMessagesLoggerClass::EditorMessagesLoggerClass(mvceditor::EditorMessagesPluginClass& plugin)
	: wxLog()
	, Plugin(plugin) {

}

void mvceditor::EditorMessagesLoggerClass::DoLog(wxLogLevel level, const wxChar *msg, time_t timestamp) {
	Plugin.AddMessage(level, msg, timestamp);
}

BEGIN_EVENT_TABLE(mvceditor::EditorMessagesPluginClass, mvceditor::PluginClass)
	EVT_MENU(mvceditor::MENU_EDITOR_MESSAGES, mvceditor::EditorMessagesPluginClass::OnMenu)
END_EVENT_TABLE()