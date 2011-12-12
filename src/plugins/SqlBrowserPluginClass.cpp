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
#include <windows/StringHelperClass.h>
#include <widgets/UnicodeStringValidatorClass.h>
#include <MvcEditorErrors.h>

const int ID_SQL_GAUGE = wxNewId();
const int ID_SQL_METADATA_GAUGE = wxNewId();

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
			wxThreadError error = wxTHREAD_NO_ERROR;
			if (GetThread() && GetThread()->IsRunning()) {
				error = wxTHREAD_RUNNING;
			}
			else {
				error = wxThreadHelper::Create();
			}
			switch (error) {
			case wxTHREAD_NO_ERROR:
				GetThread()->Run();
				wxWindow::FindWindowById(wxID_OK, this)->Disable();
				wxWindow::FindWindowById(ID_TEST, this)->Disable();
				break;
			case wxTHREAD_NO_RESOURCE:
			case wxTHREAD_MISC_ERROR:
				mvceditor::EditorLogError(mvceditor::LOW_RESOURCES);
				break;
			case wxTHREAD_RUNNING:
			case wxTHREAD_KILLED:
			case wxTHREAD_NOT_RUNNING:

				// should not be possible we are controlling this with IsRunning
				// by disabling the buttons
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

mvceditor::MultipleSqlExecuteClass::MultipleSqlExecuteClass(wxEvtHandler& handler, int queryId)
	: ThreadWithHeartbeatClass(handler, queryId)
	, SqlLexer() 
	, Query()
	, Session()
	, QueryId(queryId)
	, IsRunning(false) {
}

bool mvceditor::MultipleSqlExecuteClass::Execute() {
	bool ret = false;
	
	wxThreadError error = CreateSingleInstance();
	switch (error) {
	case wxTHREAD_NO_ERROR:
		GetThread()->Run();
		SignalStart();
		IsRunning = true;
		ret = true;
		break;
	case wxTHREAD_NO_RESOURCE:
	case wxTHREAD_MISC_ERROR:
		mvceditor::EditorLogError(mvceditor::LOW_RESOURCES);
		break;
	case wxTHREAD_RUNNING:
	case wxTHREAD_KILLED:
	case wxTHREAD_NOT_RUNNING:

		wxMessageBox(_("Please wait while current queries finish executing."));
		break;
	}
	return ret;
}

void* mvceditor::MultipleSqlExecuteClass::Entry() {
	UnicodeString error;
	UnicodeString query;
	bool connected = Query.Connect(Session, error);
	if (connected) {
		while (SqlLexer.NextQuery(query)) {		
			wxLongLong start = wxGetLocalTimeMillis();

			// create a new result on the heap; the event handler must delete it
			mvceditor::SqlResultClass* results = new mvceditor::SqlResultClass;
			results->QueryTime = wxGetLocalTimeMillis() - start;
			results->LineNumber = SqlLexer.GetLineNumber();
			Query.Execute(Session, *results, query);
			wxCommandEvent evt(QUERY_COMPLETE_EVENT, QueryId);
			evt.SetClientData(results);
			wxPostEvent(&Handler, evt);
			if (!results->Success) {
				break;
			}
		}
		SqlLexer.Close();
	}
	else {

		// signal a failed connection
		mvceditor::SqlResultClass* results = new mvceditor::SqlResultClass;
		results->LineNumber = SqlLexer.GetLineNumber();
		results->Success = false;
		results->HasRows = false;
		results->Error = error;
		wxCommandEvent evt(QUERY_COMPLETE_EVENT, QueryId);
		evt.SetClientData(results);
		wxPostEvent(&Handler, evt);
	}
	SignalEnd();
	return 0;
}

bool mvceditor::MultipleSqlExecuteClass::Init(const UnicodeString& sql, const SqlQueryClass& query) {
	Query.Info.Copy(query.Info);
	return !IsRunning && SqlLexer.OpenString(sql);
}

void mvceditor::MultipleSqlExecuteClass::Close() {
	Session.close();
	SqlLexer.Close();
	IsRunning = false;
}

mvceditor::SqlBrowserPanelClass::SqlBrowserPanelClass(wxWindow* parent, int id, 
		mvceditor::StatusBarWithGaugeClass* gauge, const mvceditor::SqlQueryClass& other,
		mvceditor::SqlBrowserPluginClass* plugin) 
	: SqlBrowserPanelGeneratedClass(parent, id)
	, Query(other)
	, LastError()
	, LastQuery()
	, MultipleSqlExecute(*this, id) 
	, Results()
	, Gauge(gauge)
	, Plugin(plugin) {
	CodeControl = NULL;
	QueryId = id;
	ResultsGrid->DeleteCols(0, ResultsGrid->GetNumberCols());
	ResultsGrid->DeleteRows(0, ResultsGrid->GetNumberRows());
	ResultsGrid->ClearGrid();
	UpdateLabels(wxT(""));
}

bool mvceditor::SqlBrowserPanelClass::Check() {
	bool ret = 	CodeControl && Validate() && TransferDataFromWindow();
	if (ret) {
		LastQuery = CodeControl->GetSafeText();
		ret = !LastQuery.isEmpty();
	}
	return ret;
}

void mvceditor::SqlBrowserPanelClass::Execute() {
	if (Check() && MultipleSqlExecute.Init(LastQuery, Query) && MultipleSqlExecute.Execute()) {
		Gauge->AddGauge(_("Running SQL queries"), ID_SQL_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE, wxGA_HORIZONTAL);
	}
	else if (LastQuery.isEmpty()) {
		wxMessageBox(_("Please enter a query into the code control."));
	}
	else {
		wxMessageBox(_("Please wait until the current queries completes."));
	}
}

void mvceditor::SqlBrowserPanelClass::OnQueryComplete(wxCommandEvent& event) {
	if (event.GetId() == QueryId) {
		mvceditor::SqlResultClass* result = (mvceditor::SqlResultClass*)event.GetClientData();
		Results.push_back(result);
	}
}

void mvceditor::SqlBrowserPanelClass::RenderAllResults() {
	if (ResultsGrid->GetNumberCols()) {
		ResultsGrid->DeleteCols(0, ResultsGrid->GetNumberCols());
	}
	if (ResultsGrid->GetNumberRows()) {
		ResultsGrid->DeleteRows(0, ResultsGrid->GetNumberRows());
	}
	bool outputSummary = false;
	if (Results.size() > 1) {

		// this grid will show a summary of all query results
		outputSummary = true;
		ResultsGrid->SetDefaultCellOverflow(false);
		ResultsGrid->AppendCols(3);
		ResultsGrid->SetColLabelValue(0, _("Query Line Number"));
		ResultsGrid->SetColLabelValue(1, _("Query"));
		ResultsGrid->SetColLabelValue(2, _("Result"));
	}
	
	for (size_t i = 0; i < Results.size(); i++) {
		mvceditor::SqlResultClass* results = Results[i];
		UnicodeString error;
		
		// if only one query was executed: render the results in this panel
		// if more than one query was executed: render any messages for 
		//	in this panel, each new resultset in a new panel
		if (results->Success && results->HasRows && !outputSummary) {
			Fill(results);
		}
		else if (results->Success && results->HasRows && outputSummary) {
			mvceditor::SqlBrowserPanelClass* newPanel = Plugin->CreateResultsPanel(CodeControl);
			newPanel->Fill(results);
		}
		if (outputSummary) {

			// put summary in the summary GRID
			wxString msg;
			if (results->Success) {
				msg = wxString::Format(_("%d rows affected in %.3f sec"), results->AffectedRows, 
					(results->QueryTime.ToLong() / 1000.00));
			}
			else {
				msg = mvceditor::StringHelperClass::IcuToWx(results->Error);
			}
			int rowNumber = ResultsGrid->GetNumberRows();
			wxString queryStart = mvceditor::StringHelperClass::IcuToWx(results->Query);
			ResultsGrid->AppendRows(1);
			ResultsGrid->SetCellValue(wxGridCellCoords(rowNumber , 0), wxString::Format(wxT("%d"), results->LineNumber));
			ResultsGrid->SetCellValue(wxGridCellCoords(rowNumber , 1), queryStart.Mid(0, 100)); 
			ResultsGrid->SetCellValue(wxGridCellCoords(rowNumber , 2), msg);
		}
		if (!outputSummary && !results->Error.isEmpty()) {

			// put error in the summary LABEL
			UpdateLabels(mvceditor::StringHelperClass::IcuToWx(results->Error));
		}
		else if (!outputSummary) {

			// put summary in the summary LABEL
			wxString msg = wxString::Format(_("%d rows affected in %.3f sec"), results->AffectedRows, 
					(results->QueryTime.ToLong() / 1000.00));
			UpdateLabels(msg);
		}
	}
	if (outputSummary) {
		ResultsGrid->AutoSizeColumn(0);
		ResultsGrid->AutoSizeColumn(1);
		ResultsGrid->AutoSizeColumn(2);
	}
}

void mvceditor::SqlBrowserPanelClass::Fill(mvceditor::SqlResultClass* results) {
	std::vector<bool> autoSizeColumns;

	// there may be many results; freeze the drawing until we fill in the grid
	ResultsGrid->BeginBatch();
	int rowNumber = 1;
	if (ResultsGrid->GetNumberCols()) {
		ResultsGrid->DeleteCols(0, ResultsGrid->GetNumberCols());
	}
	if (ResultsGrid->GetNumberRows()) {
		ResultsGrid->DeleteRows(0, ResultsGrid->GetNumberRows());
	}
	if (results->HasRows) {
		ResultsGrid->AppendCols(results->ColumnNames.size());
		for (size_t i = 0; i < results->ColumnNames.size(); i++) {
			ResultsGrid->SetColLabelValue(i, mvceditor::StringHelperClass::IcuToWx(results->ColumnNames[i]));
			autoSizeColumns.push_back(true);
		}
		ResultsGrid->SetDefaultCellOverflow(false);
		for (size_t i = 0; i < results->StringResults.size(); i++) {
			ResultsGrid->AppendRows(1);
			std::vector<UnicodeString> columnValues =  results->StringResults[i];
			for (size_t colNumber = 0; colNumber < columnValues.size(); colNumber++) {
				ResultsGrid->SetCellValue(rowNumber - 1, colNumber, mvceditor::StringHelperClass::IcuToWx(columnValues[colNumber]));
				if (columnValues[colNumber].length() > 50) {
					autoSizeColumns[colNumber] = false; 
				}
			}
			ResultsGrid->SetRowLabelValue(rowNumber - 1, wxString::Format(wxT("%d"), rowNumber));
			rowNumber++;
		}
	}

	// resize columns to fit the content, unless the content of the columns is too big (ie blobs)
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

	// unfreeze the grid
	ResultsGrid->EndBatch();
	if (!results->Success) {
		UpdateLabels(mvceditor::StringHelperClass::IcuToWx(results->Error));
	}
	else {
		UpdateLabels(wxString::Format(_("%d rows returned in %.3f sec"), 
			results->AffectedRows, (results->QueryTime.ToLong() / 1000.00)));		
	}
}

void mvceditor::SqlBrowserPanelClass::UpdateLabels(const wxString& result) {
	if (!Query.Info.Host.isEmpty()) {
		wxString driver;
		if (Query.Info.Driver == mvceditor::DatabaseInfoClass::MYSQL) {
			driver = _("mysql");
		}
		if (Query.Info.Port) {
			ConnectionLabel->SetLabel(wxString::Format(
				wxT("%s:host=%s:%d user=%s dbname=%s"),
				driver.c_str(),
				mvceditor::StringHelperClass::IcuToWx(Query.Info.Host).c_str(),
				Query.Info.Port,
				mvceditor::StringHelperClass::IcuToWx(Query.Info.User).c_str(),
				mvceditor::StringHelperClass::IcuToWx(Query.Info.DatabaseName).c_str()
			));
		}
		else {
			ConnectionLabel->SetLabel(wxString::Format(
				wxT("%s:host=%s user=%s dbname=%s"),
				driver.c_str(),
				mvceditor::StringHelperClass::IcuToWx(Query.Info.Host).c_str(),
				mvceditor::StringHelperClass::IcuToWx(Query.Info.User).c_str(),
				mvceditor::StringHelperClass::IcuToWx(Query.Info.DatabaseName).c_str()
			));
		}
	}
	else {
		ConnectionLabel->SetLabel(_("No Connection is not configured."));
	}
	ResultsLabel->SetLabel(result);
	ResultsLabel->GetContainingSizer()->Layout();
}

void mvceditor::SqlBrowserPanelClass::OnWorkInProgress(wxCommandEvent& event) {
	if (event.GetId() == QueryId) {
		Gauge->IncrementGauge(ID_SQL_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE);
	}
}

void mvceditor::SqlBrowserPanelClass::OnWorkComplete(wxCommandEvent& event) {
	if (event.GetId() == QueryId) {
		RenderAllResults();

		// make sure to cleanup all results
		for (size_t i = 0; i < Results.size(); i++) {
			delete Results[i];
		}
		Results.clear();
		MultipleSqlExecute.Close();
		Gauge->StopGauge(ID_SQL_GAUGE);
		Plugin->AuiManagerUpdate();
	}
}

void mvceditor::SqlBrowserPanelClass::SetCurrentInfo(const mvceditor::DatabaseInfoClass& info) {
	Query.Info.Copy(info);
	if (CodeControl) {
		CodeControl->SetCurrentInfo(info);
	}
}

void mvceditor::SqlBrowserPanelClass::LinkToCodeControl(mvceditor::CodeControlClass* codeControl) {
	CodeControl = codeControl;
}

bool mvceditor::SqlBrowserPanelClass::IsLinkedToCodeControl(mvceditor::CodeControlClass* codeControl) {
	return CodeControl != NULL && codeControl == CodeControl;
}

void mvceditor::SqlBrowserPanelClass::UnlinkFromCodeControl() {
	CodeControl = NULL;
}

mvceditor::SqlMetaDataFetchClass::SqlMetaDataFetchClass(wxEvtHandler& handler)
	: ThreadWithHeartbeatClass(handler)
	, Infos() 
	, Errors()
	, NewResources() {
		
}

bool mvceditor::SqlMetaDataFetchClass::Read(std::vector<mvceditor::DatabaseInfoClass> infos) {
	bool ret = false;
	wxThreadError err = CreateSingleInstance();
	if (wxTHREAD_NO_RESOURCE == err) {
		mvceditor::EditorLogError(mvceditor::LOW_RESOURCES);
	}
	else if (wxTHREAD_RUNNING == err) {
		wxMessageBox(_("There is already another SQL MetaData fetch that is active. Please wait for it to finish."), _("SQL MetaData fetch"));
	}
	else if (wxTHREAD_NO_ERROR == err) {
		Infos = infos;
		Errors.clear();
		GetThread()->Run();
		SignalStart();
		ret = true;
	}
	return ret;
}

void* mvceditor::SqlMetaDataFetchClass::Entry() {
	for (std::vector<mvceditor::DatabaseInfoClass>::iterator it = Infos.begin(); it != Infos.end(); ++it) {
		UnicodeString error;
		if (!NewResources.Fetch(*it, error)) {
			Errors.push_back(error);
		}
	}
	SignalEnd();
	return 0;
}

void mvceditor::SqlMetaDataFetchClass::WriteResultsInto(mvceditor::SqlResourceFinderClass& dest) {
	dest.Copy(NewResources);
}

std::vector<UnicodeString> mvceditor::SqlMetaDataFetchClass::GetErrors() {
	return Errors;
}

mvceditor::SqlBrowserPluginClass::SqlBrowserPluginClass() 
	: PluginClass()
	, Infos()
	, SqlMetaDataFetch(*this)
	, ChosenIndex(0)
	, WasEmptyDetectedInfo(false) {
}

mvceditor::SqlBrowserPluginClass::~SqlBrowserPluginClass() {
}

void mvceditor::SqlBrowserPluginClass::OnProjectOpened() {
	DetectMetadata();
}

void mvceditor::SqlBrowserPluginClass::DetectMetadata() {
	ChosenIndex = 0;
	Infos = GetProject()->DatabaseInfo();
	WasEmptyDetectedInfo = Infos.empty();
	if (WasEmptyDetectedInfo) {
		mvceditor::DatabaseInfoClass info;
		info.Name = UNICODE_STRING_SIMPLE("Custom info");
		Infos.push_back(info);
	}
	else {
		if (SqlMetaDataFetch.Read(Infos)) {
			GetStatusBarWithGauge()->AddGauge(_("Fetching SQL meta data"), ID_SQL_METADATA_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE, 0);
		}
	}
}

void mvceditor::SqlBrowserPluginClass::AddToolsMenuItems(wxMenu* toolsMenu) {
	toolsMenu->Append(mvceditor::MENU_SQL + 0, _("SQL Browser\tSHIFT+F9"), _("Open a window for SQL browsing"),
		wxITEM_NORMAL);
	toolsMenu->Append(mvceditor::MENU_SQL + 1, _("SQL Connections\tCTRL+F9"), _("Show & Pick The SQL Connections that this project uses"),
		wxITEM_NORMAL);
	toolsMenu->Append(mvceditor::MENU_SQL + 2, _("Run Queries in SQL Browser\tF9"), _("Execute the query that is currently in the SQL Browser"),
		wxITEM_NORMAL);
	toolsMenu->Append(mvceditor::MENU_SQL + 3, _("Detect SQL Meta Data"), _("Detect SQL Meta data so that it is made available to code completion"),
		wxITEM_NORMAL);
}

void  mvceditor::SqlBrowserPluginClass::OnSqlBrowserToolsMenu(wxCommandEvent& event) {
	int num = 1;
	mvceditor::NotebookClass* notebook = GetNotebook();
	for (size_t i = 0; i < notebook->GetPageCount(); i++) {
		wxString name = notebook->GetPageText(i);
		if (name.EndsWith(wxT(".sql")) || name.Index(_("SQL Browser")) == 0) {
			num++;
		}
	}
	
	mvceditor::CodeControlClass* ctrl = CreateCodeControl(wxString::Format(_("SQL Browser %d"), num));
	ctrl->SetDocumentMode(mvceditor::CodeControlClass::SQL);
	CreateResultsPanel(ctrl);
	ctrl->SetFocus();
}

mvceditor::SqlBrowserPanelClass* mvceditor::SqlBrowserPluginClass::CreateResultsPanel(mvceditor::CodeControlClass* codeControl) {
	mvceditor::SqlQueryClass query;
	if (ChosenIndex < Infos.size()) {
		query.Info.Copy(Infos[ChosenIndex]);
		codeControl->SetCurrentInfo(Infos[ChosenIndex]);
	}
	
	mvceditor::SqlBrowserPanelClass* sqlPanel = new SqlBrowserPanelClass(GetToolsNotebook(), wxNewId(), GetStatusBarWithGauge(), query, this);
	mvceditor::NotebookClass* codeNotebook = GetNotebook();
	wxString tabText = codeNotebook->GetPageText(codeNotebook->GetPageIndex(codeControl));
	AddToolsWindow(sqlPanel, tabText);
	sqlPanel->LinkToCodeControl(codeControl);
	return sqlPanel;
}

void mvceditor::SqlBrowserPluginClass::OnRun(wxCommandEvent& event) {
	mvceditor::CodeControlClass* ctrl = GetNotebook()->GetCurrentCodeControl();
	if (ctrl && ctrl->GetDocumentMode() == mvceditor::CodeControlClass::SQL) {
		
		// look for results panel that corresponds to the current code control
		wxAuiNotebook* notebook = GetToolsNotebook();
		bool found = false;
		for (size_t i = 0; i < notebook->GetPageCount(); i++) {
			wxWindow* window = notebook->GetPage(i);
			mvceditor::SqlBrowserPanelClass* panel = wxDynamicCast(window, mvceditor::SqlBrowserPanelClass);
			if (panel && panel->IsLinkedToCodeControl(ctrl)) {
				
				// we found the panel bring it to the forefront and run the query
				found = true;
				SetFocusToToolsWindow(window);
				panel->Execute();
				break;
			}
		}
		if (!found) {
			mvceditor::SqlBrowserPanelClass* panel = CreateResultsPanel(ctrl);
			panel->Execute();
		}
	}
}

void mvceditor::SqlBrowserPluginClass::OnSqlConnectionMenu(wxCommandEvent& event) {

	// decided to always allow the user to edit the connection info in order to
	// allow the user to create a new database from within the editor (the very 
	// first time a new project is created; its database may not exist).
	// before, a user would not be able to edit the connection info once it was detected
	// in order to make it less confusing about where the connection info comes from.
	mvceditor::SqlConnectionDialogClass dialog(GetMainWindow(), Infos, ChosenIndex, true);
	if (dialog.ShowModal() == wxOK) {
		
		// if connection changed need to update the code control so that it knows to use the new
		// connection for auto completion purposes
		// all SQL panels are updated.
		wxAuiNotebook* notebook = GetToolsNotebook();
		for (size_t i = 0; i < notebook->GetPageCount(); ++i) {
			wxWindow* window = notebook->GetPage(i);
			mvceditor::SqlBrowserPanelClass* panel = wxDynamicCast(window, mvceditor::SqlBrowserPanelClass);
			if (panel) {
				if (ChosenIndex < Infos.size()) {
					panel->SetCurrentInfo(Infos[ChosenIndex]);
				}
				panel->UpdateLabels(wxT(""));			
			}
		}

		// redetect the SQL meta data
		SqlMetaDataFetch.Read(Infos);
	}
}

void mvceditor::SqlBrowserPluginClass::OnSqlDetectMenu(wxCommandEvent& event) {
	DetectMetadata();
}

void mvceditor::SqlBrowserPluginClass::OnContentNotebookPageChanged(wxAuiNotebookEvent& event) {
	if (event.GetEventObject() == GetNotebook()) {
		mvceditor::CodeControlClass* contentWindow = GetNotebook()->GetCodeControl(event.GetSelection());
		if (contentWindow) {
			wxAuiNotebook* notebook = GetToolsNotebook();
			for (size_t i = 0; i < notebook->GetPageCount(); i++) {
				wxWindow* toolsWindow = notebook->GetPage(i);
				mvceditor::SqlBrowserPanelClass* panel = wxDynamicCast(toolsWindow, mvceditor::SqlBrowserPanelClass);
				if (panel && panel->IsLinkedToCodeControl(contentWindow)) {
					
					// we found the panel bring it to the forefront and run the query
					SetFocusToToolsWindow(toolsWindow);
					break;
				}
			}
		}
	}
	event.Skip();
}

void mvceditor::SqlBrowserPluginClass::OnContentNotebookPageClose(wxAuiNotebookEvent& event) {
	if (event.GetEventObject() == GetNotebook()) {	
		mvceditor::CodeControlClass* contentWindow = GetNotebook()->GetCodeControl(event.GetSelection());
		if (contentWindow) {
			wxAuiNotebook* notebook = GetToolsNotebook();
			for (size_t i = 0; i < notebook->GetPageCount(); i++) {
				wxWindow* toolsWindow = notebook->GetPage(i);
				mvceditor::SqlBrowserPanelClass* panel = wxDynamicCast(toolsWindow, mvceditor::SqlBrowserPanelClass);
				if (panel && panel->IsLinkedToCodeControl(contentWindow)) {
					panel->UnlinkFromCodeControl();
				}
			}
		}
	}
	event.Skip();
}

void mvceditor::SqlBrowserPluginClass::OnWorkInProgress(wxCommandEvent& event) {
	GetStatusBarWithGauge()->IncrementGauge(ID_SQL_METADATA_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE);
}

void mvceditor::SqlBrowserPluginClass::OnWorkComplete(wxCommandEvent& event) {
	GetStatusBarWithGauge()->StopGauge(ID_SQL_METADATA_GAUGE);
	SqlMetaDataFetch.WriteResultsInto(*GetProject()->GetSqlResourceFinder());

	std::vector<UnicodeString> errors = SqlMetaDataFetch.GetErrors();
	for (size_t i = 0; i < errors.size(); ++i) {
		wxString wxError = mvceditor::StringHelperClass::IcuToWx(errors[i]);
		mvceditor::EditorLogError(mvceditor::BAD_SQL, wxError);
	}
}

void mvceditor::SqlBrowserPluginClass::AuiManagerUpdate() {
	AuiManager->Update();
}

BEGIN_EVENT_TABLE(mvceditor::SqlBrowserPluginClass, wxEvtHandler)
	EVT_MENU(mvceditor::MENU_SQL + 0, mvceditor::SqlBrowserPluginClass::OnSqlBrowserToolsMenu)
	EVT_MENU(mvceditor::MENU_SQL + 1, mvceditor::SqlBrowserPluginClass::OnSqlConnectionMenu)
	EVT_MENU(mvceditor::MENU_SQL + 2, mvceditor::SqlBrowserPluginClass::OnRun)
	EVT_MENU(mvceditor::MENU_SQL + 3, mvceditor::SqlBrowserPluginClass::OnSqlDetectMenu)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SqlBrowserPluginClass::OnWorkInProgress)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SqlBrowserPluginClass::OnWorkComplete)
	EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, mvceditor::SqlBrowserPluginClass::OnContentNotebookPageChanged)
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, mvceditor::SqlBrowserPluginClass::OnContentNotebookPageClose)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::SqlBrowserPanelClass, SqlBrowserPanelGeneratedClass)
	EVT_COMMAND(wxID_ANY, QUERY_COMPLETE_EVENT, mvceditor::SqlBrowserPanelClass::OnQueryComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SqlBrowserPanelClass::OnWorkInProgress)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SqlBrowserPanelClass::OnWorkComplete)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::SqlConnectionDialogClass, SqlConnectionDialogGeneratedClass)
	EVT_COMMAND(wxID_ANY, QUERY_COMPLETE_EVENT, mvceditor::SqlConnectionDialogClass::ShowTestResults)
END_EVENT_TABLE()