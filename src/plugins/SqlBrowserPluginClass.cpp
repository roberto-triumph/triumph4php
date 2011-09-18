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
#include <windows/StringHelperClass.h>
#include <widgets/UnicodeStringValidatorClass.h>

const int ID_SQL_EDITOR_MENU = mvceditor::PluginClass::newMenuId();
const int ID_SQL_RUN_MENU = mvceditor::PluginClass::newMenuId();
const int ID_SQL_CONNECTION_MENU = mvceditor::PluginClass::newMenuId();
const int ID_SQL_GAUGE = wxNewId();

mvceditor::SqlConnectionDialogClass::SqlConnectionDialogClass(wxWindow* parent, std::vector<mvceditor::DatabaseInfoClass>& infos, 
		size_t& chosenIndex, bool allowEdit)
	: SqlConnectionDialogGeneratedClass(parent, wxID_ANY) 
	, Infos(infos)
	, TestQuery()
	, ChosenIndex(chosenIndex) {
	for (size_t i = 0; i < infos.size(); i++) {
		wxString name = mvceditor::StringHelperClass::IcuToWx(infos[i].Name);
		List->Append(name);
	}
	if (chosenIndex < infos.size()) {
		Host->SetValue(mvceditor::StringHelperClass::IcuToWx(infos[chosenIndex].Host));
		User->SetValue(mvceditor::StringHelperClass::IcuToWx(infos[chosenIndex].User));
		Password->SetValue(mvceditor::StringHelperClass::IcuToWx(infos[chosenIndex].Password));
		Database->SetValue(mvceditor::StringHelperClass::IcuToWx(infos[chosenIndex].DatabaseName));
		Port->SetValue(infos[chosenIndex].Port);
		List->Select(chosenIndex);
	}
	if (!allowEdit) {
		Host->Enable(false);
		User->Enable(false);
		Password->Enable(false);
		Database->Enable(false);
		Port->Enable(false);
	}
	else if (!infos.empty()) {
		UnicodeStringValidatorClass hostValidator(&Infos[0].Host);
		Host->SetValidator(hostValidator);
		UnicodeStringValidatorClass userValidator(&Infos[0].User);
		User->SetValidator(userValidator);
		UnicodeStringValidatorClass passwordValidator(&Infos[0].Password);
		Password->SetValidator(passwordValidator);
		UnicodeStringValidatorClass databaseValidator(&Infos[0].DatabaseName);
		Database->SetValidator(databaseValidator);
		wxGenericValidator portValidator(&Infos[0].Port);
		Port->SetValidator(portValidator);
		TransferDataToWindow();
	}
}
	
void mvceditor::SqlConnectionDialogClass::OnOkButton(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow()) {
		ChosenIndex = List->GetSelection();
		EndModal(wxOK);
	}
}

void mvceditor::SqlConnectionDialogClass::OnCancelButton(wxCommandEvent& event) {
	wxThread* thread = GetThread();
	if (thread) { 
		thread->Kill();
	}
	event.Skip();
}

void mvceditor::SqlConnectionDialogClass::OnTestButton(wxCommandEvent& event) {
	if (TransferDataFromWindow()) {
		size_t index = List->GetSelection();
		if (index < Infos.size()) {
			TestQuery.Info.Copy(Infos[index]);
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
}

void mvceditor::SqlConnectionDialogClass::OnHelpButton(wxCommandEvent& event) {
	wxString help = wxString::FromAscii(
		"The SQL Connections dialog shows you the list of database connections that were detected.\n"
		"This detection works only for supported frameworks (MVC Editor can support multiple frameworks).\n"
		"If this list is empty it means that the editor the current project is not using one of the \n"
		"supported frameworks. If you modify the file containing the database credentials you will need \n"
		"to reopen the project"		
	);
	help = wxGetTranslation(help);
	wxMessageBox(help, _("Help"));
}

void* mvceditor::SqlConnectionDialogClass::Entry() {
	UnicodeString error;
	soci::session session;
	bool success = false;
	if (TestQuery.Connect(session, error)) {
		soci::statement stmt = (session.prepare << "SELECT 1");
		success = TestQuery.Execute(stmt, error);
		TestQuery.Close(session, stmt);
	}
	wxCommandEvent evt(QUERY_COMPLETE_EVENT, wxID_ANY);
	evt.SetInt(success ? 1 : 0);
	evt.SetString(mvceditor::StringHelperClass::IcuToWx(error));
	wxPostEvent(this, evt);
	return 0;
}

void mvceditor::SqlConnectionDialogClass::ShowTestResults(wxCommandEvent& event) {
	wxString msg = _("Connection to %s@%s was successful");
	msg = wxString::Format(msg, 
		mvceditor::StringHelperClass::IcuToWx(TestQuery.Info.User).c_str(), 
		mvceditor::StringHelperClass::IcuToWx(TestQuery.Info.Host).c_str());
	if (event.GetInt() == 0) {
		msg = _("Connection to %s@%s failed: %s");
		msg = wxString::Format(msg, 
		mvceditor::StringHelperClass::IcuToWx(TestQuery.Info.User).c_str(), 
		mvceditor::StringHelperClass::IcuToWx(TestQuery.Info.Host).c_str(), 
		event.GetString().c_str());
	}
	wxMessageBox(msg);
	wxWindow::FindWindowById(wxID_OK, this)->Enable();
	wxWindow::FindWindowById(ID_TEST, this)->Enable();
}

void mvceditor::SqlConnectionDialogClass::OnListboxSelected(wxCommandEvent& event) {
	ChosenIndex = event.GetInt();
	if (ChosenIndex <Infos.size()) {
		Host->SetValue(mvceditor::StringHelperClass::IcuToWx(Infos[ChosenIndex].Host));
		User->SetValue(mvceditor::StringHelperClass::IcuToWx(Infos[ChosenIndex].User));
		Password->SetValue(mvceditor::StringHelperClass::IcuToWx(Infos[ChosenIndex].Password));
		Database->SetValue(mvceditor::StringHelperClass::IcuToWx(Infos[ChosenIndex].DatabaseName));
		Port->SetValue(Infos[ChosenIndex].Port);
	}
}
	

mvceditor::SqlBrowserPanelClass::SqlBrowserPanelClass(wxWindow* parent, int id, mvceditor::CodeControlClass* codeControl, 
		mvceditor::StatusBarWithGaugeClass* gauge, const mvceditor::SqlQueryClass& other) 
	: SqlBrowserPanelGeneratedClass(parent, id)
	, wxThreadHelper()
	, Query(other)
	, Session()
	, Stmt(NULL)
	, Row()
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

void mvceditor::SqlBrowserPanelClass::Close() {
	if (Stmt) {
		Query.Close(Session, *Stmt);
		delete Stmt;
		Stmt = NULL;
	}
	else {
		Session.close();
	}
}

bool mvceditor::SqlBrowserPanelClass::Check() {
	bool ret = Validate() && TransferDataFromWindow();
	if (ret) {
		LastQuery = mvceditor::StringHelperClass::wxToIcu(GetText().Trim());
		ret = !LastQuery.isEmpty();
	}
	return ret;
}

void mvceditor::SqlBrowserPanelClass::Execute() {
	if (!IsRunning && Check()) {
		LastError = UNICODE_STRING_SIMPLE("");
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
	bool success = Query.Connect(Session, LastError);
	if (success) {
		Stmt = new soci::statement(Session);
		success = Query.Execute(Session, *Stmt, Row, LastQuery, LastError);
	}
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
	std::vector<UnicodeString> columnNames;
	std::vector<UnicodeString> columnValues;
	std::vector<soci::indicator> columnIndicators;
	std::vector<bool> autoSizeColumns;
	UnicodeString error = LastError;
	ResultsGrid->BeginBatch();
	bool success = event.GetInt() != 0;
	bool nonEmpty = false;
	int rowNumber = 1;
	int affected = Query.GetAffectedRows(*Stmt);
	if (ResultsGrid->GetNumberCols()) {
		ResultsGrid->DeleteCols(0, ResultsGrid->GetNumberCols());
	}
	if (ResultsGrid->GetNumberRows()) {
		ResultsGrid->DeleteRows(0, ResultsGrid->GetNumberRows());
	}
	if (success) {
		if (Query.ColumnNames(Row, columnNames, error)) {
			ResultsGrid->AppendCols(columnNames.size());
			for (size_t i = 0; i < columnNames.size(); i++) {
				ResultsGrid->SetColLabelValue(i, mvceditor::StringHelperClass::IcuToWx(columnNames[i]));
				autoSizeColumns.push_back(true);
			}
			bool more = true;
			ResultsGrid->SetDefaultCellOverflow(false);
				while (more && Query.More(*Stmt, success, error)) {
				ResultsGrid->AppendRows(1);
				more = Query.NextRow(Row, columnValues, columnIndicators, error);
				for (size_t colNumber = 0; colNumber < columnValues.size(); colNumber++) {
					if (columnIndicators[colNumber] == soci::i_null) {
						ResultsGrid->SetCellValue(rowNumber - 1, colNumber, wxT("<NULL>"));
					}
					else {
						ResultsGrid->SetCellValue(rowNumber - 1, colNumber, mvceditor::StringHelperClass::IcuToWx(columnValues[colNumber]));
					}					
					if (columnValues[colNumber].length() > 50) {
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
	Close();

	// time for query only; not time to render grid
	wxLongLong msec = wxGetLocalTimeMillis() - QueryStart;
	for (size_t i = 0; i < autoSizeColumns.size(); i++) {
		if (autoSizeColumns[i]) {
			ResultsGrid->AutoSizeColumn(i);
			ResultsGrid->SetColMinimalWidth(i, ResultsGrid->GetColumnWidth(i));
		}
		else {
			ResultsGrid->SetColSize(i, 50);
			ResultsGrid->SetColMinimalWidth(i, 50);
		}
	}
	ResultsGrid->EndBatch();
	Gauge->StopGauge(ID_SQL_GAUGE);
	Timer.Stop();
	if (!success) {
		UpdateLabels(mvceditor::StringHelperClass::IcuToWx(error));
	}
	else {
		
		if (affected && nonEmpty) {
			UpdateLabels(wxString::Format(_("%d rows returned in %.3f sec"), affected, (msec.ToLong() / 1000.00)));
		}
		else {
			UpdateLabels(wxString::Format(_("%d rows affected in %.3f sec"), affected, (msec.ToLong() / 1000.00)));
		}
	}
	IsRunning = false;
}

void mvceditor::SqlBrowserPanelClass::UpdateLabels(const wxString& result) {
	ConnectionLabel->SetLabel(wxString::Format(
		wxT("%s@%s:%d"),
		mvceditor::StringHelperClass::IcuToWx(Query.Info.User).c_str(),
		mvceditor::StringHelperClass::IcuToWx(Query.Info.Host).c_str(),
		Query.Info.Port
	));
	ResultsLabel->SetLabel(result);
	ResultsLabel->GetContainingSizer()->Layout();
}

void mvceditor::SqlBrowserPanelClass::OnTimer(wxTimerEvent& event) {
	Gauge->IncrementGauge(ID_SQL_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE);
}

mvceditor::SqlBrowserPluginClass::SqlBrowserPluginClass() 
	: PluginClass() 
	, Infos()
	, ChosenIndex(0)
	, WasEmptyDetectedInfo(false) {
}

mvceditor::SqlBrowserPluginClass::~SqlBrowserPluginClass() {
}

void mvceditor::SqlBrowserPluginClass::OnProjectOpened() {
	GetProject()->Detect();
	ChosenIndex = 0;
	
	// TODO: how and when to refresh ??
	// a "redetect" button??
	Infos = GetProject()->DatabaseInfo();
	WasEmptyDetectedInfo = Infos.empty();
	if (WasEmptyDetectedInfo) {
		mvceditor::DatabaseInfoClass info;
		info.Name = UNICODE_STRING_SIMPLE("Custom info");
		Infos.push_back(info);
	}
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
	mvceditor::SqlQueryClass query;
	if (ChosenIndex < Infos.size()) {
		query.Info.Copy(Infos[ChosenIndex]);
	}
	mvceditor::SqlBrowserPanelClass* sqlPanel = new SqlBrowserPanelClass(GetNotebook(), wxID_NEW, ctrl, GetStatusBarWithGauge(), query);
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
	mvceditor::SqlConnectionDialogClass dialog(GetMainWindow(), Infos, ChosenIndex, WasEmptyDetectedInfo);
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