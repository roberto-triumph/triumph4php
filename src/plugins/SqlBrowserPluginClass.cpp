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
#include <plugins/SqlBrowserPluginClass.h>
#include <widgets/CodeControlClass.h>

const int ID_SQL_EDITOR_MENU = mvceditor::PluginClass::newMenuId();
const int ID_SQL_RUN_MENU = mvceditor::PluginClass::newMenuId();

mvceditor::SqlBrowserPanelClass::SqlBrowserPanelClass(wxWindow* parent, int id, mvceditor::CodeControlClass* codeControl) 
	: SqlBrowserPanelGeneratedClass(parent, id)
	, CodeControl(codeControl) {
	CodeControl->Reparent(this);
	CodeControl->SetDocumentMode(mvceditor::CodeControlClass::SQL);
	TopPanelSizer->Add(CodeControl, 1, wxALL | wxEXPAND, 5);
	ResultsGrid->DeleteCols(0, ResultsGrid->GetNumberCols());
	ResultsGrid->DeleteRows(0, ResultsGrid->GetNumberRows());
}

wxString mvceditor::SqlBrowserPanelClass::GetText() {
	return CodeControl->GetText();
}

void mvceditor::SqlBrowserPanelClass::OnRunButton(wxCommandEvent& event) {
	wxString queries = GetText();
	wxMessageBox(wxT("button clicked. going to query\n\n") + queries);
}

mvceditor::SqlBrowserPluginClass::SqlBrowserPluginClass() 
: PluginClass() {

}

void mvceditor::SqlBrowserPluginClass::AddToolsMenuItems(wxMenu* toolsMenu) {
	toolsMenu->Append(ID_SQL_EDITOR_MENU, _("SQL Browser"), _("Open a window for SQL browsing"),
		wxITEM_NORMAL);
	toolsMenu->Append(ID_SQL_RUN_MENU, _("Run Queries in SQL Browser"), _("Execute the query that is currently in the SQL Browser"),
		wxITEM_NORMAL);
}

void  mvceditor::SqlBrowserPluginClass::OnSqlBrowserToolsMenu(wxCommandEvent& event) {
	mvceditor::CodeControlClass* ctrl = CreateCodeControl(GetToolsParentWindow());
	mvceditor::SqlBrowserPanelClass* sqlPanel = new SqlBrowserPanelClass(GetNotebook(), wxID_NEW, ctrl);
	AddContentWindow(sqlPanel, _("SQL Browser"));
	SetFocusToToolsWindow(sqlPanel);
}

void mvceditor::SqlBrowserPluginClass::OnRun(wxCommandEvent& event) {
	wxWindow* window = GetCurrentContentPane();
	mvceditor::SqlBrowserPanelClass* panel = wxDynamicCast(window, mvceditor::SqlBrowserPanelClass);
	if (panel) {
		wxString queries = panel->GetText();
		wxMessageBox(wxT("going to query\n\n") + queries);
	}
}

BEGIN_EVENT_TABLE(mvceditor::SqlBrowserPluginClass, wxEvtHandler)
	EVT_MENU(ID_SQL_EDITOR_MENU, mvceditor::SqlBrowserPluginClass::OnSqlBrowserToolsMenu)
	EVT_MENU(ID_SQL_RUN_MENU, mvceditor::SqlBrowserPluginClass::OnRun)
END_EVENT_TABLE()