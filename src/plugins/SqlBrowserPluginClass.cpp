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
#include <wx/valgen.h>
#include <plugins/SqlBrowserPluginClass.h>
#include <widgets/CodeControlClass.h>
 #include <soci-mysql.h>

const int ID_SQL_EDITOR_MENU = mvceditor::PluginClass::newMenuId();
const int ID_SQL_RUN_MENU = mvceditor::PluginClass::newMenuId();
const int ID_SQL_CONNECTION_MENU = mvceditor::PluginClass::newMenuId();
const int ID_SQL_GAUGE = wxNewId();

mvceditor::SqlQueryClass::SqlQueryClass()
	: Host()
	, User()
	, Password()
	, Database()
	, Port()
	, Session()
	, Statement(NULL)
	, Row() {
}

mvceditor::SqlQueryClass::SqlQueryClass(const mvceditor::SqlQueryClass& other) 
	: Host()
	, User()
	, Password()
	, Database()
	, Port()
	, Session()
	, Statement(NULL)
	, Row()  {
	Copy(other);
}

void mvceditor::SqlQueryClass::Copy(const mvceditor::SqlQueryClass& src) {
	Host = src.Host;
	User = src.User;
	Password = src.Password;
	Database = src.Database;
	Port = src.Port;
}

mvceditor::SqlQueryClass::~SqlQueryClass() {
	Close();
}

void mvceditor::SqlQueryClass::Close() {
	Session.close();
	if (Statement) {
		Statement->clean_up();
		Statement = NULL;
	}	
}

bool mvceditor::SqlQueryClass::Query(const wxString& query, wxString& error) {
	bool success = false;
	wxString connString = wxString::Format(wxT("db=%s host=%s port=%d user=%s password='%s'"), 
		Database.c_str(), Host.c_str(), Port, 
		User.c_str(), Password.c_str());
	try {
		Session.open(*soci::factory_mysql(), (const char*)connString.ToAscii());
		Statement = new soci::statement(Session);
		Statement->alloc();
		Statement->prepare((const char*)query.ToAscii());
		Statement->define_and_bind();
		Statement->exchange_for_rowset(soci::into(Row));
		Statement->execute(true);
		
		// execute will return false if statement does not return any rows
		// but we want to return true for INSERTs and UPDATEs too
		success = true;
		
	} catch (std::exception const& e) {
		success = false;
		printf("query error=%s cs=%s\n", e.what(), (const char *)connString.ToAscii());
		error = wxString(e.what(), wxConvUTF8);
	}
	return success;
}

bool mvceditor::SqlQueryClass::More() {
	return Statement && Statement->got_data();
}

long long mvceditor::SqlQueryClass::GetAffectedRows() {
	return Statement ? Statement->get_affected_rows() : 0;
}

bool mvceditor::SqlQueryClass::ColumnNames(std::vector<wxString>& columnNames, wxString& error) {
	bool data = Statement && Statement->got_data();
	if (data) {
		try {
			for (size_t i = 0; i < Row.size(); i++) {
				soci::column_properties props = Row.get_properties(i);
				wxString col(props.get_name().c_str(), wxConvUTF8);
				columnNames.push_back(col);
			}
		}
		catch (std::exception const& e) {
			data = false;
			error = wxString(e.what(), wxConvUTF8);
		}
	}
	return data;
}

bool mvceditor::SqlQueryClass::NextRow(std::vector<wxString>& columnValues, std::vector<soci::indicator>& columnIndicators, wxString& error) {
	bool data = Statement && Statement->got_data();
	if (data) {
		try {
			for (size_t i = 0; i < Row.size(); i++) {
				soci::indicator indicator = Row.get_indicator(i);
				columnIndicators.push_back(indicator);
				soci::column_properties props = Row.get_properties(i);
				wxString col;
				if (soci::i_null != indicator) {
					switch(props.get_data_type()) {
					case soci::dt_string:
						col = wxString(Row.get<std::string>(i).c_str(), wxConvUTF8);
						break;
					case soci::dt_double:
						col = wxString::Format(wxT("%f"), Row.get<double>(i));
						break;
					case soci::dt_integer:
						col = wxString::Format(wxT("%d"), Row.get<int>(i));
						break;
					case soci::dt_unsigned_long:
					case soci::dt_unsigned_long_long:
						col = wxString::Format(wxT("%lu"), Row.get<unsigned long>(i));
						break;
					case soci::dt_long_long:
						col = wxString::Format(wxT("%ld"), Row.get<long long>(i));
						break;
					case soci::dt_date:
						wxDateTime date(Row.get<std::tm>(i));
						col = date.Format(wxT("%Y-%m-%d %H:%M:%S"));
						break;
					}
					
				}
				columnValues.push_back(col);
			}
			Statement->fetch();
		}
		catch (std::exception const& e) {
			data = false;
			error = wxString(e.what(), wxConvUTF8);
		}
	}
	return data;
}

mvceditor::SqlConnectionDialogClass::SqlConnectionDialogClass(wxWindow* parent, mvceditor::SqlQueryClass& query)
	: SqlConnectionDialogGeneratedClass(parent, wxID_ANY)
	, ModifiedQuery(query)
	, OriginalQuery(query) {
	wxGenericValidator hostValidator(&ModifiedQuery.Host);
	Host->SetValidator(hostValidator);
	wxGenericValidator userValidator(&ModifiedQuery.User);
	User->SetValidator(userValidator);
	wxGenericValidator passwordValidator(&ModifiedQuery.Password);
	Password->SetValidator(passwordValidator);
	wxGenericValidator databaseValidator(&ModifiedQuery.Database);
	Database->SetValidator(databaseValidator);
	wxGenericValidator portValidator(&ModifiedQuery.Port);
	Port->SetValidator(portValidator);
}
	
void mvceditor::SqlConnectionDialogClass::OnOkButton(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow()) {
		OriginalQuery.Copy(ModifiedQuery);
		EndModal(wxOK);
	}
}

void mvceditor::SqlConnectionDialogClass::OnCancelButton(wxCommandEvent& event) {
	wxThread* thread = GetThread();
	if (thread) { 
		thread->Kill();
	}
}

void mvceditor::SqlConnectionDialogClass::OnTestButton(wxCommandEvent& event) {
	if (TransferDataFromWindow()) {
		wxThreadError error = wxThreadHelper::Create();
		switch (error) {
		case wxTHREAD_NO_ERROR:
			GetThread()->Run();
			wxWindow::FindWindowById(wxID_OK, this)->Disable();
			wxWindow::FindWindowById(ID_TEST, this)->Disable();
			break;
		case wxTHREAD_NO_RESOURCE:
		case wxTHREAD_MISC_ERROR:
			wxMessageBox(_("Cannot run query. Your system is low on resources."));
			break;
		case wxTHREAD_RUNNING:
		case wxTHREAD_KILLED:
		case wxTHREAD_NOT_RUNNING:
			// should not be possible we are controlling this with IsRunning
			break;
		}
	}
}

void* mvceditor::SqlConnectionDialogClass::Entry() {
	wxString error;
	wxString query(wxT("SELECT 1"));
	bool success = ModifiedQuery.Query(query, error);
	wxCommandEvent evt(QUERY_COMPLETE_EVENT, wxID_ANY);
	evt.SetInt(success);
	evt.SetString(error);
	wxPostEvent(this, evt);
	return 0;
}

void mvceditor::SqlConnectionDialogClass::ShowTestResults(wxCommandEvent& event) {
	wxString msg = _("Connection successul");
	if (!event.GetInt()) {
		msg = _("Connection Failed:") + event.GetString();
	}
	ModifiedQuery.Close();
	wxMessageBox(msg);
	wxWindow::FindWindowById(wxID_OK, this)->Enable();
	wxWindow::FindWindowById(ID_TEST, this)->Enable();
}
	

mvceditor::SqlBrowserPanelClass::SqlBrowserPanelClass(wxWindow* parent, int id, mvceditor::CodeControlClass* codeControl, 
		mvceditor::StatusBarWithGaugeClass* gauge, const mvceditor::SqlQueryClass& other) 
	: SqlBrowserPanelGeneratedClass(parent, id)
	, wxThreadHelper()
	, Query(other)
	, CodeControl(codeControl) 
	, Gauge(gauge)
	, Timer()
	, LastError()
	, LastQuery()
	, IsRunning(false) {
	CodeControl->Reparent(CodeControlPanel);
	CodeControl->SetDocumentMode(mvceditor::CodeControlClass::SQL);
	ResultsGrid->DeleteCols(0, ResultsGrid->GetNumberCols());
	ResultsGrid->DeleteRows(0, ResultsGrid->GetNumberRows());
	ResultsGrid->ClearGrid();
	CodeControlPanelSizer->Add(CodeControl, 1, wxEXPAND, 0);
	CodeControlPanelSizer->Layout();
	Timer.SetOwner(this);
	CodeControl->SetText(wxT(""));
	UpdateLabels(wxT(""));
}

bool mvceditor::SqlBrowserPanelClass::Check() {
	bool ret = Validate() && TransferDataFromWindow();
	if (ret) {
		LastQuery = GetText().Trim();
		ret = !LastQuery.empty();
	}
	return ret;
}

void mvceditor::SqlBrowserPanelClass::Execute() {
	if (!IsRunning && Check()) {
		LastError = wxT("");
		wxThreadError error = wxThreadHelper::Create();
		switch (error) {
		case wxTHREAD_NO_ERROR:
			Gauge->AddGauge(_("Running SQL Query"), ID_SQL_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE, wxGA_HORIZONTAL);
			Timer.Start(200, wxTIMER_CONTINUOUS);
			QueryStart = wxGetLocalTimeMillis();
			GetThread()->Run();
			IsRunning = true;
			break;
		case wxTHREAD_NO_RESOURCE:
		case wxTHREAD_MISC_ERROR:
			wxMessageBox(_("Cannot run query. Your system is low on resources."));
			break;
		case wxTHREAD_RUNNING:
		case wxTHREAD_KILLED:
		case wxTHREAD_NOT_RUNNING:
			// should not be possible we are controlling this with IsRunning
			break;
		}
	}
	else {
		wxMessageBox(_("Please wait until the current query completes."));
	}
}

void* mvceditor::SqlBrowserPanelClass::Entry() {
	bool success = Query.Query(LastQuery, LastError);
	wxCommandEvent evt(QUERY_COMPLETE_EVENT, wxID_ANY);
	evt.SetInt(success);
	wxPostEvent(this, evt);
	return 0;
}

wxString mvceditor::SqlBrowserPanelClass::GetText() {
	return CodeControl->GetText();
}

void mvceditor::SqlBrowserPanelClass::OnRunButton(wxCommandEvent& event) {
	Execute();
}

void mvceditor::SqlBrowserPanelClass::OnQueryComplete(wxCommandEvent& event) {
	std::vector<wxString> columnNames;
	std::vector<wxString> columnValues;
	std::vector<soci::indicator> columnIndicators;
	std::vector<bool> autoSizeColumns;
	wxString error = LastError;
	ResultsGrid->BeginBatch();
	bool success = event.GetInt() != 0;
	bool nonEmpty = false;
	int rowNumber = 1;
	int affected = Query.GetAffectedRows();
	if (ResultsGrid->GetNumberCols()) {
		ResultsGrid->DeleteCols(0, ResultsGrid->GetNumberCols());
	}
	if (ResultsGrid->GetNumberRows()) {
		ResultsGrid->DeleteRows(0, ResultsGrid->GetNumberRows());
	}
	if (success) {
		if (Query.ColumnNames(columnNames, error)) {
			ResultsGrid->AppendCols(columnNames.size());
			for (size_t i = 0; i < columnNames.size(); i++) {
				ResultsGrid->SetColLabelValue(i, columnNames[i]);
				autoSizeColumns.push_back(true);
			}
			bool more = true;
			ResultsGrid->SetDefaultCellOverflow(false);
			while (more && Query.More()) {
				ResultsGrid->AppendRows(1);
				more = Query.NextRow(columnValues, columnIndicators, error);
				for (size_t colNumber = 0; colNumber < columnValues.size(); colNumber++) {
					if (columnIndicators[colNumber] == soci::i_null) {
						ResultsGrid->SetCellValue(rowNumber - 1, colNumber, wxT("<NULL>"));
					}
					else {
						ResultsGrid->SetCellValue(rowNumber - 1, colNumber, columnValues[colNumber]);
					}					
					if (columnValues[colNumber].size() > 50) {
						autoSizeColumns[colNumber] = false; 
					}
				}
				ResultsGrid->SetRowLabelValue(rowNumber - 1, wxString::Format(wxT("%d"), rowNumber));
				rowNumber++;
				columnValues.clear();
				columnIndicators.clear();								
			}
		}
	}
	if (!success) {
		UpdateLabels(error);
	}
	else {
		wxLongLong msec = wxGetLocalTimeMillis() - QueryStart;
		if (affected && nonEmpty) {
			UpdateLabels(wxString::Format(_("%d rows returned in %.3f sec"), affected, (msec.ToLong() / 1000)));
		}
		else {
			UpdateLabels(wxString::Format(_("%d rows affected in %.3f sec"), affected, (msec.ToLong() / 1000)));
		}
	}
	Query.Close();
	for (size_t i = 0; i < autoSizeColumns.size(); i++) {
		if (autoSizeColumns[i]) {
			ResultsGrid->AutoSizeColumn(i);
		}
		else {
			ResultsGrid->SetColSize(i, 50);
		}
	}
	ResultsGrid->EndBatch();
	Gauge->StopGauge(ID_SQL_GAUGE);
	Timer.Stop();
	IsRunning = false;
}

void mvceditor::SqlBrowserPanelClass::UpdateLabels(const wxString& result) {
	ConnectionLabel->SetLabel(wxString::Format(
		wxT("%s@%s:%d"),
		Query.User.c_str(),
		Query.Host.c_str(),
		Query.Port
	));
	ResultsLabel->SetLabel(result);
	ResultsLabel->GetContainingSizer()->Layout();
}

void mvceditor::SqlBrowserPanelClass::OnTimer(wxTimerEvent& event) {
	Gauge->IncrementGauge(ID_SQL_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE);
}

mvceditor::SqlBrowserPluginClass::SqlBrowserPluginClass() 
	: PluginClass() 
	, Query() {
	Query.Host = wxT("localhost");
	Query.Port = 3306;
	Query.Database = wxT("mysql");
	Query.User = wxT("root");
	Query.Password = wxT("");

}

mvceditor::SqlBrowserPluginClass::~SqlBrowserPluginClass() {
}

void mvceditor::SqlBrowserPluginClass::AddToolsMenuItems(wxMenu* toolsMenu) {
	toolsMenu->Append(ID_SQL_EDITOR_MENU, _("SQL Browser"), _("Open a window for SQL browsing"),
		wxITEM_NORMAL);
	toolsMenu->Append(ID_SQL_CONNECTION_MENU, _("SQL Connections"), _("Show & Pick The SQL Connections that this project uses"),
		wxITEM_NORMAL);
	toolsMenu->Append(ID_SQL_RUN_MENU, _("Run Queries in SQL Browser"), _("Execute the query that is currently in the SQL Browser"),
		wxITEM_NORMAL);
}

void  mvceditor::SqlBrowserPluginClass::OnSqlBrowserToolsMenu(wxCommandEvent& event) {
	mvceditor::CodeControlClass* ctrl = CreateCodeControl(GetToolsParentWindow(), 0);
	mvceditor::SqlBrowserPanelClass* sqlPanel = new SqlBrowserPanelClass(GetNotebook(), wxID_NEW, ctrl, GetStatusBarWithGauge(), Query);
	AddContentWindow(sqlPanel, _("SQL Browser"));
	SetFocusToToolsWindow(sqlPanel);
}

void mvceditor::SqlBrowserPluginClass::OnRun(wxCommandEvent& event) {
	wxWindow* window = GetCurrentContentPane();
	mvceditor::SqlBrowserPanelClass* panel = wxDynamicCast(window, mvceditor::SqlBrowserPanelClass);
	if (panel) {
		wxString queries = panel->GetText();
		panel->Execute();
	}
}

void mvceditor::SqlBrowserPluginClass::OnSqlConnectionMenu(wxCommandEvent& event) {
	mvceditor::SqlConnectionDialogClass dialog(GetMainWindow(), Query);
	if (dialog.ShowModal() == wxOK) {

		// nothing for now .. won't update the SqlBrowserPanel connection label
		// since that label shows with the data and it may confuse the user
		// TODO need to update the existing opened panels
		// but need to account for thread-safetyness
	}
}

BEGIN_EVENT_TABLE(mvceditor::SqlBrowserPluginClass, wxEvtHandler)
	EVT_MENU(ID_SQL_EDITOR_MENU, mvceditor::SqlBrowserPluginClass::OnSqlBrowserToolsMenu)
	EVT_MENU(ID_SQL_CONNECTION_MENU, mvceditor::SqlBrowserPluginClass::OnSqlConnectionMenu)
	EVT_MENU(ID_SQL_RUN_MENU, mvceditor::SqlBrowserPluginClass::OnRun)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::SqlBrowserPanelClass, SqlBrowserPanelGeneratedClass)
	EVT_COMMAND(wxID_ANY, QUERY_COMPLETE_EVENT, mvceditor::SqlBrowserPanelClass::OnQueryComplete)
	EVT_TIMER(wxID_ANY, mvceditor::SqlBrowserPanelClass::OnTimer)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::SqlConnectionDialogClass, SqlConnectionDialogGeneratedClass)
	EVT_COMMAND(wxID_ANY, QUERY_COMPLETE_EVENT, mvceditor::SqlConnectionDialogClass::ShowTestResults)
END_EVENT_TABLE()