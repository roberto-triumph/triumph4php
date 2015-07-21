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
#include "views/EditorMessagesViewClass.h"
#include <map>
#include <vector>
#include "features/EditorMessagesFeatureClass.h"
#include "globals/Assets.h"
#include "globals/Errors.h"

static const int ID_DEBUG_WINDOW = wxNewId();

t4p::EditorMessagesPanelClass::EditorMessagesPanelClass(wxWindow* parent, int id)
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

void t4p::EditorMessagesPanelClass::OnClearButton(wxCommandEvent& event) {
	int rowCount = Grid->GetNumberRows();
	if (rowCount > 0) {
		Grid->DeleteRows(0, rowCount);
	}
}

void t4p::EditorMessagesPanelClass::AddMessage(wxLogLevel level, const wxChar* msg, time_t timestamp) {
	// put a bound on the number of messages to display
	int maxRows = 1000;
	if (Grid->GetNumberRows() > maxRows) {
		Grid->DeleteRows(0, 1);
	}
	wxString error,
		fix,
		levelString,
		dateString;
	t4p::EditorErrorFix(msg, error, fix);
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
		Grid->SetCellOverflow(Grid->GetNumberRows() - 1, 0, false);
		Grid->SetCellValue(Grid->GetNumberRows() - 1, 0, error);
		Grid->SetCellValue(Grid->GetNumberRows() - 1, 1, fix);
		Grid->SetCellValue(Grid->GetNumberRows() - 1, 2, levelString);
		Grid->SetCellValue(Grid->GetNumberRows() - 1, 3, dateString);

		// ATTN: make the grid expand to the entire panel
		// not sure if this will render nicely when many messages are shown
		Grid->AutoSize();
		Layout();
	}
}
t4p::EditorMessagesViewClass::EditorMessagesViewClass()
: FeatureViewClass() {
}

void t4p::EditorMessagesViewClass::AddViewMenuItems(wxMenu *viewMenu) {
	viewMenu->Append(t4p::MENU_EDITOR_MESSAGES, _("Editor Messages"),
		_("Editor Messages"));
}

void t4p::EditorMessagesViewClass::OnMenu(wxCommandEvent& event) {
	wxWindow* window = FindToolsWindow(ID_DEBUG_WINDOW);
	if (window) {
		SetFocusToToolsWindow(window);
	} else {
		wxWindow* panel = new t4p::EditorMessagesPanelClass(GetToolsNotebook(), ID_DEBUG_WINDOW);
		wxBitmap msgBitmap = t4p::BitmapImageAsset(wxT("editor-messages"));
		AddToolsWindow(panel, _("Editor Messages"), wxEmptyString, msgBitmap);
	}
}

void t4p::EditorMessagesViewClass::OnAppLog(t4p::EditorLogEventClass& evt) {
	AddMessage(evt.Level, evt.Message, evt.Timestamp);
}

void t4p::EditorMessagesViewClass::AddMessage(wxLogLevel level, const wxChar* msg, time_t timestamp) {
	// in MSW ignore log messages dealting with SetFocus()
	// If we don't ignore this, the app will stack overflow when the app is minized
	// and later restored.
	// Best guess at explaining why this is happening:
	// When the app is restored after being minized, the app is given focus but this
	// results in a debug message ("SetFocus failed"....) When the debug message
	// is triggered, this method (AddMessage) is called. Now, we try
	// to add the warning to the panel and set the focus to the messages panel. This
	// new call to focus on the panel in turn triggers another ("SetFocus failed"....)
	// debug message; which in turn calls this method again. And the cycle never
	// stops.
	//
	// The setFocus warning seems to be an issue inside windows API and the sash, but it
	// seems to not be harmful; we can ignore it.
	// see http://forums.wxwidgets.org/viewtopic.php?f=1&t=30907
	wxString ignoringMsg = wxT("window.cpp(643): 'SetFocus' failed with error 0x00000057 (the parameter is incorrect.).");
	wxString logMsg(msg);
	if (logMsg.Find(ignoringMsg) != wxNOT_FOUND) {
		return;
	}
	wxWindow* window = FindToolsWindow(ID_DEBUG_WINDOW);
	t4p::EditorMessagesPanelClass* panel = NULL;
	if (window) {
		panel = (t4p::EditorMessagesPanelClass*)window;
		SetFocusToToolsWindow(window);
	} else {
		panel = new t4p::EditorMessagesPanelClass(GetToolsNotebook(), ID_DEBUG_WINDOW);
		wxBitmap msgBitmap = t4p::BitmapImageAsset(wxT("editor-messages"));
		AddToolsWindow(panel, _("Editor Messages"), wxEmptyString, msgBitmap);
	}
	panel->AddMessage(level, msg, timestamp);
}

void t4p::EditorMessagesViewClass::AddKeyboardShortcuts(std::vector<DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[t4p::MENU_EDITOR_MESSAGES] = wxT("Editor-Messages");
	AddDynamicCmd(menuItemIds, shortcuts);
}

BEGIN_EVENT_TABLE(t4p::EditorMessagesViewClass, t4p::FeatureViewClass)
	EVT_MENU(t4p::MENU_EDITOR_MESSAGES, t4p::EditorMessagesViewClass::OnMenu)
	EVT_APP_LOG(t4p::EditorMessagesViewClass::OnAppLog)
END_EVENT_TABLE()
