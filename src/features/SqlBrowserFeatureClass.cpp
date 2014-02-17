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
#include <features/SqlBrowserFeatureClass.h>
#include <code_control/CodeControlClass.h>
#include <globals/String.h>
#include <widgets/UnicodeStringValidatorClass.h>
#include <widgets/FilePickerValidatorClass.h>
#include <globals/Errors.h>
#include <globals/Assets.h>
#include <Triumph.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <sqlite3.h>
#include <wx/artprov.h>
#include <wx/wupdlock.h>
#include <wx/clipbrd.h>
#include <wx/sstream.h>

static const int ID_SQL_GAUGE = wxNewId();
static const int ID_SQL_EDIT_TEST = wxNewId();
static const int ID_SQL_LIST_TEST = wxNewId();
static const int ID_SQL_TABLE_DEFINITION = wxNewId();
static const int ID_SQL_TABLE_INDICES = wxNewId();
static const int ID_SQL_TABLE_CREATE = wxNewId();
static const int ID_PANEL_TABLE_DEFINITION = wxNewId();

static const int ID_GRID_COPY_CELL = wxNewId();
static const int ID_GRID_COPY_ROW = wxNewId();
static const int ID_GRID_COPY_ROW_SQL = wxNewId();
static const int ID_GRID_COPY_ROW_PHP = wxNewId();
static const int ID_GRID_COPY_ALL = wxNewId();
static const int ID_GRID_OPEN_IN_EDITOR = wxNewId();


/**
 * @param grid the grid to put the results in. any previous grid values are cleared. this function will not own the pointer
 * @param results the results to fill. this function will not own the pointer
 */
static void FillGridWithResults(wxGrid* grid, t4p::SqlResultClass* results) {
	std::vector<bool> autoSizeColumns;

	// there may be many results; freeze the drawing until we fill in the grid
	grid->BeginBatch();
	int rowNumber = 1;
	if (grid->GetNumberCols()) {
		grid->DeleteCols(0, grid->GetNumberCols());
	}
	if (grid->GetNumberRows()) {
		grid->DeleteRows(0, grid->GetNumberRows());
	}
	if (!results->Error.isEmpty()) {
		grid->AppendCols(1);
		grid->SetColLabelValue(1, _("Error Message"));
		grid->AppendRows(1);
		grid->SetCellValue(0, 0, t4p::IcuToWx(results->Error));
		autoSizeColumns.push_back(true);
	}
	if (results->HasRows) {
		grid->AppendCols(results->ColumnNames.size());
		for (size_t i = 0; i < results->ColumnNames.size(); i++) {
			grid->SetColLabelValue(i, t4p::IcuToWx(results->ColumnNames[i]));
			autoSizeColumns.push_back(true);
		}
		grid->SetDefaultCellOverflow(false);
		for (size_t i = 0; i < results->StringResults.size(); i++) {
			grid->AppendRows(1);
			std::vector<UnicodeString> columnValues =  results->StringResults[i];
			for (size_t colNumber = 0; colNumber < columnValues.size(); colNumber++) {
				grid->SetCellValue(rowNumber - 1, colNumber, t4p::IcuToWx(columnValues[colNumber]));
				if (columnValues[colNumber].length() > 50) {
					autoSizeColumns[colNumber] = false; 
				}
			}
			grid->SetRowLabelValue(rowNumber - 1, wxString::Format(wxT("%d"), rowNumber));
			rowNumber++;
		}
	}

	// resize columns to fit the content, unless the content of the columns is too big (ie blobs)
	for (size_t i = 0; i < autoSizeColumns.size(); i++) {
		if (autoSizeColumns[i]) {
			grid->AutoSizeColumn(i);
			grid->SetColMinimalWidth(i, grid->GetColumnWidth(i));
		}
		else {
			grid->SetColSize(i, 50);
			grid->SetColMinimalWidth(i, 50);
		}
	}

	// unfreeze the grid
	grid->EndBatch();
}

t4p::QueryCompleteEventClass::QueryCompleteEventClass(t4p::SqlResultClass* results, int eventId)
: wxEvent(eventId, t4p::QUERY_COMPLETE_EVENT)
, Results(results) {
	
}

wxEvent* t4p::QueryCompleteEventClass::Clone() const {
	return new t4p::QueryCompleteEventClass(Results, GetId());
}

t4p::SqliteConnectionDialogClass::SqliteConnectionDialogClass(wxWindow* parent, t4p::DatabaseTagClass& tag)
: SqliteConnectionDialogGeneratedClass(parent, wxID_ANY)
{
	t4p::UnicodeStringValidatorClass labelValidator(&tag.Label, false);
	Label->SetValidator(labelValidator);
	Label->SetName(wxT("label"));
	t4p::FilePickerValidatorClass fileValidator(&tag.FileName);
	File->SetValidator(fileValidator);
	File->SetName(wxT("file"));
	TransferDataToWindow();
	
	// dont allow editing of detected connections
	if (tag.IsDetected) {
		Label->Enable(false);
		File->Enable(false);
		wxWindow::FindWindowById(wxID_OK, this)->Enable(false);
	}
}

t4p::MysqlConnectionDialogClass::MysqlConnectionDialogClass(wxWindow* parent, t4p::DatabaseTagClass& tag,
		t4p::RunningThreadsClass& runningThreads)
: MysqlConnectionDialogGeneratedClass(parent, wxID_ANY)
, TestQuery()
, RunningThreads(runningThreads)
, ConnectionIdentifier()
, RunningActionId() {
	RunningThreads.AddEventHandler(this);
	
	t4p::UnicodeStringValidatorClass labelValidator(&tag.Label, false);
	Label->SetValidator(labelValidator);
	Label->SetName(wxT("label"));
	t4p::UnicodeStringValidatorClass hostValidator(&tag.Host, false);
	Host->SetValidator(hostValidator);
	Host->SetName(wxT("host"));
	wxGenericValidator portValidator(&tag.Port);
	Port->SetValidator(portValidator);
	t4p::UnicodeStringValidatorClass schemaValidator(&tag.Schema, true);
	Database->SetValidator(schemaValidator);
	t4p::UnicodeStringValidatorClass userValidator(&tag.User, true);
	User->SetValidator(userValidator);
	t4p::UnicodeStringValidatorClass passwordValidator(&tag.Password, true);
	Password->SetValidator(passwordValidator);
	
	TransferDataToWindow();
	
	// dont allow editing of detected connections
	if (tag.IsDetected) {
		Label->Enable(false);
		Host->Enable(false);
		Port->Enable(false);
		Database->Enable(false);
		User->Enable(false);
		Password->Enable(false);
		
		wxWindow::FindWindowById(wxID_OK, this)->Enable(false);
	}
}

t4p::MysqlConnectionDialogClass::~MysqlConnectionDialogClass() {
	RunningThreads.RemoveEventHandler(this);
}

void t4p::MysqlConnectionDialogClass::OnTestButton(wxCommandEvent& event) {

	// get the most up-to-date values that the user has input

	TestQuery.DatabaseTag.Driver = t4p::DatabaseTagClass::MYSQL;
	TestQuery.DatabaseTag.Label = t4p::WxToIcu(Label->GetValue());
	TestQuery.DatabaseTag.Host = t4p::WxToIcu(Host->GetValue());
	TestQuery.DatabaseTag.Port = Port->GetValue();
	TestQuery.DatabaseTag.Schema = t4p::WxToIcu(Database->GetValue());
	TestQuery.DatabaseTag.User = t4p::WxToIcu(User->GetValue());
	TestQuery.DatabaseTag.Password = t4p::WxToIcu(Password->GetValue());
	
	wxWindow::FindWindowById(wxID_OK, this)->Enable(false);
	wxWindow::FindWindowById(ID_TESTBUTTON, this)->Enable(false);
	
	t4p::MultipleSqlExecuteClass* thread = new t4p::MultipleSqlExecuteClass(RunningThreads, ID_SQL_EDIT_TEST, ConnectionIdentifier);
	thread->Init(UNICODE_STRING_SIMPLE("SELECT 1"), TestQuery);
	RunningThreads.Queue(thread);
}

void t4p::MysqlConnectionDialogClass::OnCancelButton(wxCommandEvent& event) {
	
	// in case the test query is stuck, send a kill command to mysql
	soci::session session;
	UnicodeString error;
	if (TestQuery.Connect(session, error)) {
		TestQuery.KillConnection(session, ConnectionIdentifier, error);
	}
	RunningThreads.CancelAction(RunningActionId);
	RunningActionId = 0;
	event.Skip();
}

void t4p::MysqlConnectionDialogClass::ShowTestResults(t4p::QueryCompleteEventClass& event) {
	t4p::SqlResultClass* result = event.Results;
	wxString error = t4p::IcuToWx(result->Error);
	bool success = result->Success;
	
	wxString creds = t4p::IcuToWx(TestQuery.DatabaseTag.User + 
			UNICODE_STRING_SIMPLE("@") +
			TestQuery.DatabaseTag.Host);
	
	wxString msg = _("Connection to %s was successful");
	msg = wxString::Format(msg, creds.c_str());
	if (!success) {
		msg = _("Connection to %s failed: %s");
		msg = wxString::Format(msg, 
		creds, 
		error.c_str());
	}
	wxMessageBox(msg);
	wxWindow::FindWindowById(wxID_OK, this)->Enable();
	wxWindow::FindWindowById(ID_TESTBUTTON, this)->Enable();
	delete result;
}


t4p::SqlConnectionListDialogClass::SqlConnectionListDialogClass(wxWindow* parent, std::vector<t4p::DatabaseTagClass>& dbTags, 
															  t4p::RunningThreadsClass& runningThreads)
	: SqlConnectionListDialogGeneratedClass(parent, wxID_ANY) 
	, DatabaseTags(dbTags)
	, EditedDatabaseTags()
	, TestQuery()
	, RunningThreads(runningThreads)
	, ConnectionIdentifier()
	, RunningActionId() {
	RunningThreads.AddEventHandler(this);
	for (size_t i = 0; i < dbTags.size(); i++) {
		Push(dbTags[i]);
	}
	TransferDataToWindow();
}

t4p::SqlConnectionListDialogClass::~SqlConnectionListDialogClass() {
	RunningThreads.RemoveEventHandler(this);
}

void t4p::SqlConnectionListDialogClass::OnAddMysqlButton(wxCommandEvent& event) {
	t4p::DatabaseTagClass newTag;
	newTag.Driver = t4p::DatabaseTagClass::MYSQL;
	t4p::MysqlConnectionDialogClass mysqlDialog(this, newTag, RunningThreads);
	if (mysqlDialog.ShowModal() == wxID_OK) {
		Push(newTag);
	}
}

void t4p::SqlConnectionListDialogClass::OnAddSqliteButton(wxCommandEvent& event) {
	t4p::DatabaseTagClass newTag;
	newTag.Driver = t4p::DatabaseTagClass::SQLITE;
	t4p::SqliteConnectionDialogClass sqliteDialog(this, newTag);
	if (sqliteDialog.ShowModal() == wxID_OK) {
		Push(newTag);
	}
}

void t4p::SqlConnectionListDialogClass::OnCloneButton(wxCommandEvent& event) {
	wxArrayInt toCloneIndexes;
	if (List->GetSelections(toCloneIndexes)) {
		for (size_t i = 0; i < toCloneIndexes.GetCount(); ++i) {
			size_t indexToClone = toCloneIndexes[i];
			
			t4p::DatabaseTagClass clonedTag;
			clonedTag.Copy(EditedDatabaseTags[indexToClone]);
			
			// clone == the user created it
			clonedTag.IsDetected = false;
			
			// add a postfix to the clone label
			clonedTag.Label = clonedTag.Label + UNICODE_STRING_SIMPLE(" clone");
			Push(clonedTag);
		}
	}
}

void t4p::SqlConnectionListDialogClass::OnRemoveSelectedButton(wxCommandEvent& event) {
	wxArrayInt toRemoveIndexes;
	if (List->GetSelections(toRemoveIndexes)) {
		std::vector<t4p::DatabaseTagClass> remaining;
		for (size_t i = 0; i < EditedDatabaseTags.size(); ++i) {
			
			// dont allow the user to delete a detected connection
			if (toRemoveIndexes.Index(i) == wxNOT_FOUND || EditedDatabaseTags[i].IsDetected) {
				remaining.push_back(EditedDatabaseTags[i]);
			}
		}
		
		EditedDatabaseTags.clear();
		List->Clear();
		
		for (size_t i = 0; i < remaining.size(); ++i) {
			Push(remaining[i]);
		}
	}
}

void t4p::SqlConnectionListDialogClass::OnRemoveAllButton(wxCommandEvent& event) {
	
	// remove from the backing list
	// but don't allow the user to delete a detected connection
	std::vector<t4p::DatabaseTagClass> newList;
	std::vector<t4p::DatabaseTagClass>::iterator tag;
	for (tag = EditedDatabaseTags.begin(); tag != EditedDatabaseTags.end(); ++tag) {
		if (tag->IsDetected) {
			newList.push_back(*tag);
		}
	}
	
	// delete all tags from the list
	EditedDatabaseTags.clear();
	List->Clear();
	
	// add the tags that we didn't delete
	for (tag = newList.begin(); tag != newList.end(); ++tag) {
		Push(*tag);
	}
}

void t4p::SqlConnectionListDialogClass::OnCheckToggled(wxCommandEvent& event) {
	size_t sel = event.GetInt();
	bool b = List->IsChecked(sel);
	if (sel >= 0 && sel < EditedDatabaseTags.size()) {
		EditedDatabaseTags[sel].IsEnabled = b;
	}
}

void t4p::SqlConnectionListDialogClass::OnListDoubleClick(wxCommandEvent& event) {
	size_t sel = (size_t)event.GetSelection();
	if (sel >= 0 && sel < EditedDatabaseTags.size()) {
		t4p::DatabaseTagClass editTag = EditedDatabaseTags[sel];
		int res = 0;
		if (t4p::DatabaseTagClass::MYSQL == editTag.Driver) {
			t4p::MysqlConnectionDialogClass dialog(this, editTag, RunningThreads);
			res = dialog.ShowModal();
		}
		else if (t4p::DatabaseTagClass::SQLITE == editTag.Driver) {
			t4p::SqliteConnectionDialogClass dialog(this, editTag);
			res = dialog.ShowModal();
		}
		if (wxID_OK == res) {
			EditedDatabaseTags[sel] = editTag;
			
			// update the list label
			List->SetString(sel, t4p::IcuToWx(editTag.Label));
		}
	}
	
}

void t4p::SqlConnectionListDialogClass::Push(const t4p::DatabaseTagClass& newTag) {
	wxString label = t4p::IcuToWx(newTag.Label);
	if (newTag.IsDetected) {
		label += _(" <Detected>");
	}
	EditedDatabaseTags.push_back(newTag);
		
	List->Append(label);
	List->Check(List->GetCount() - 1, newTag.IsEnabled);
}
	
void t4p::SqlConnectionListDialogClass::OnOkButton(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow()) {
		DatabaseTags.clear();
		DatabaseTags.resize(EditedDatabaseTags.size());
		std::copy(EditedDatabaseTags.begin(), EditedDatabaseTags.end(), DatabaseTags.begin());
		EndModal(wxOK);
	}
}

void t4p::SqlConnectionListDialogClass::OnCancelButton(wxCommandEvent& event) {
	soci::session session;
	UnicodeString error;
	if (TestQuery.Connect(session, error)) {
		TestQuery.KillConnection(session, ConnectionIdentifier, error);
	}
	RunningThreads.CancelAction(RunningActionId);
	RunningActionId = 0;
	event.Skip();
}

void t4p::SqlConnectionListDialogClass::OnTestSelectedButton(wxCommandEvent& event) {
	wxArrayInt toTestIndexes;
	if (List->GetSelections(toTestIndexes) > 1) {
		wxMessageBox(_("Select a single connection to test"), _("Error"), wxOK | wxCENTRE, this);
		return;
	}
	if (toTestIndexes.IsEmpty()) {
		return;
	}
	size_t index = (size_t)toTestIndexes[0];
	if (index < EditedDatabaseTags.size()) {
		wxWindow::FindWindowById(wxID_OK, this)->Enable(false);
		wxWindow::FindWindowById(ID_TESTBUTTON, this)->Enable(false);
	

		// get the most up-to-date values that the user has input
		TestQuery.DatabaseTag.Copy(EditedDatabaseTags[index]);

		t4p::MultipleSqlExecuteClass* thread = new t4p::MultipleSqlExecuteClass(RunningThreads, ID_SQL_LIST_TEST, ConnectionIdentifier);
		thread->Init(UNICODE_STRING_SIMPLE("SELECT 1"), TestQuery);
		RunningThreads.Queue(thread);
	}
}

void t4p::SqlConnectionListDialogClass::ShowTestResults(t4p::QueryCompleteEventClass& event) {
	t4p::SqlResultClass* result = event.Results;
	wxString error = t4p::IcuToWx(result->Error);
	bool success = result->Success;
	
	wxString creds;
	if (TestQuery.DatabaseTag.FileName.IsOk()) {
		creds = TestQuery.DatabaseTag.FileName.GetFullPath();
	}
	else {
		creds = t4p::IcuToWx(TestQuery.DatabaseTag.User + 
			UNICODE_STRING_SIMPLE("@") +
			TestQuery.DatabaseTag.Host);
	}
	wxString msg = _("Connection to %s was successful");
	msg = wxString::Format(msg, 
		creds.c_str());
	if (!success) {
		msg = _("Connection to %s failed: %s");
		msg = wxString::Format(msg, 
		creds, 
		error.c_str());
	}
	wxMessageBox(msg, _("Test"), wxOK | wxCENTRE, this);
	wxWindow::FindWindowById(wxID_OK, this)->Enable();
	wxWindow::FindWindowById(ID_TESTBUTTON, this)->Enable();
	delete result;
}

t4p::MultipleSqlExecuteClass::MultipleSqlExecuteClass(t4p::RunningThreadsClass& runningThreads, int queryId,
															t4p::ConnectionIdentifierClass& connectionIdentifier)
	: ActionClass(runningThreads, queryId)
	, SqlLexer() 
	, Query()
	, Session()
	, ConnectionIdentifier(connectionIdentifier)
	, QueryId(queryId) {
}

wxString t4p::MultipleSqlExecuteClass::GetLabel() const {
	return wxT("SQL Execute");
}

void t4p::MultipleSqlExecuteClass::BackgroundWork() {
	UnicodeString error;
	UnicodeString query;
	bool connected = Query.Connect(Session, error);
	if (connected) {
		while (SqlLexer.NextQuery(query) && !IsCancelled()) {		
			wxLongLong start = wxGetLocalTimeMillis();

			// create a new result on the heap; the event handler must delete it
			t4p::SqlResultClass* results = new t4p::SqlResultClass;
			results->QueryTime = wxGetLocalTimeMillis() - start;
			results->LineNumber = SqlLexer.GetLineNumber();
			
			Query.ConnectionIdentifier(Session, ConnectionIdentifier);
			Query.Execute(Session, *results, query);

			// post the results even if the query has an error.
			// but dont post if the query was cancelled
			// careful: leak will happen when panel is closed since event won't be handled
			if (!IsCancelled()) {
				t4p::QueryCompleteEventClass evt(results, QueryId);
				PostEvent(evt);
			}
			else {
				delete results;
			}
		}
		SqlLexer.Close();
	}
	else {

		// signal a failed connection
		t4p::SqlResultClass* results = new t4p::SqlResultClass;
		results->LineNumber = SqlLexer.GetLineNumber();
		results->Success = false;
		results->HasRows = false;
		results->Error = error;
		t4p::QueryCompleteEventClass evt(results, QueryId);
		PostEvent(evt);
	}
}

void t4p::MultipleSqlExecuteClass::DoCancel() {
	if (t4p::DatabaseTagClass::SQLITE == Query.DatabaseTag.Driver) {
		soci::sqlite3_session_backend* backend = static_cast<soci::sqlite3_session_backend*>(Session.get_backend());
		sqlite_api::sqlite3_interrupt(backend->conn_);
	}
	if (t4p::DatabaseTagClass::MYSQL == Query.DatabaseTag.Driver) {
		// for stopping mysql queries, we
		// send a KILL sql command instead of killing the
		// thread; that way the thread can gracefully exit
		// we need to do both: kill the currently running 
		// query (via a SQL KILL) and stop the thread
		// so that the rest of the SQL statements are not
		// sent to the server and the thread terminates
		soci::session session;
		UnicodeString error;
		bool good = Query.Connect(session, error);
		if (good) {
			good = Query.KillConnection(session, ConnectionIdentifier, error);
			if (!good) {
				wxMessageBox(_("could not kill connection:") + t4p::IcuToWx(error));
			}
		}
		else {
			wxMessageBox(_("could not connect:") + t4p::IcuToWx(error));
		}
	}
}

bool t4p::MultipleSqlExecuteClass::Init(const UnicodeString& sql, const SqlQueryClass& query) {
	Query.DatabaseTag.Copy(query.DatabaseTag);
	return SqlLexer.OpenString(sql);
}

void t4p::MultipleSqlExecuteClass::Close() {
	Session.close();
	SqlLexer.Close();
}

t4p::SqlBrowserPanelClass::SqlBrowserPanelClass(wxWindow* parent, int id, 
		t4p::StatusBarWithGaugeClass* gauge, const t4p::SqlQueryClass& other,
		t4p::SqlBrowserFeatureClass* feature) 
	: SqlBrowserPanelGeneratedClass(parent, id)
	, SelectedCol(-1)
	, SelectedRow(-1)
	, Query(other)
	, ConnectionIdentifier()
	, LastError()
	, LastQuery()
	, RunningActionId(0) 
	, Results()
	, RowToSqlInsert()
	, RowToPhp()
	, Gauge(gauge)
	, Feature(feature) 
	, CopyOptions() {
	CodeControl = NULL;
	QueryId = id;
	ResultsGrid->DeleteCols(0, ResultsGrid->GetNumberCols());
	ResultsGrid->DeleteRows(0, ResultsGrid->GetNumberRows());
	ResultsGrid->ClearGrid();
	UpdateLabels(wxT(""));

	RefreshButton->SetBitmap(t4p::IconImageAsset(wxT("outline-refresh")));
	Feature->App.RunningThreads.AddEventHandler(this);
	FillConnectionList();
}

t4p::SqlBrowserPanelClass::~SqlBrowserPanelClass() {
	
}

bool t4p::SqlBrowserPanelClass::Check() {
	bool ret = CodeControl && Validate() && TransferDataFromWindow();
	if (ret) {
		LastQuery = CodeControl->GetSafeText();
		ret = !LastQuery.isEmpty();
	}
	if (ret) {
		
		// make sure a connection has been chosen
		size_t sel = (size_t)Connections->GetSelection();
		std::vector<t4p::DatabaseTagClass> dbTags = Feature->App.Globals.AllEnabledDatabaseTags();
		if (sel >= 0 && sel < dbTags.size()) {
			Query.DatabaseTag.Copy(dbTags[sel]);
		}
		else {
			ret = false;
		}
	}
	return ret;
}

void t4p::SqlBrowserPanelClass::ExecuteCodeControl() {
	if (Check() && 0 == RunningActionId) {
		t4p::MultipleSqlExecuteClass* thread = new t4p::MultipleSqlExecuteClass(
			Feature->App.RunningThreads, QueryId, ConnectionIdentifier);
		if (thread->Init(LastQuery, Query)) { 
			RunningActionId = Feature->App.RunningThreads.Queue(thread);
			Gauge->AddGauge(_("Running SQL queries"), ID_SQL_GAUGE, t4p::StatusBarWithGaugeClass::INDETERMINATE_MODE, wxGA_HORIZONTAL);
		}
		else {
			delete thread;
			RunningActionId = 0;
		}
	}
	else if (LastQuery.isEmpty()) {
		wxMessageBox(_("Please enter a query into the code control."), _("Error"), wxOK | wxCENTRE, this);
	}
	else if (Connections->IsEmpty()) {
		wxMessageBox(_("Please create a database connection."), _("Error"), wxOK | wxCENTRE, this);
	}
	else {
		wxMessageBox(_("Please wait until the current queries completes."), _("Error"), wxOK | wxCENTRE, this);
	}
}


void t4p::SqlBrowserPanelClass::ExecuteQuery(const wxString& sql, const t4p::DatabaseTagClass& tag) {
	LastQuery = t4p::WxToIcu(sql);
	Query.DatabaseTag = tag;
	for (unsigned int i = 0; i < Connections->GetCount(); ++i) {
		if (Connections->GetString(i) == t4p::IcuToWx(tag.Label)) {
			Connections->SetSelection(i);
			break;
		}
	}
	RunningActionId = 0;
	t4p::MultipleSqlExecuteClass* thread = new t4p::MultipleSqlExecuteClass(
		Feature->App.RunningThreads, QueryId, ConnectionIdentifier);
	if (thread->Init(LastQuery, Query)) { 
		RunningActionId = Feature->App.RunningThreads.Queue(thread);
		Gauge->AddGauge(_("Running SQL queries"), ID_SQL_GAUGE, t4p::StatusBarWithGaugeClass::INDETERMINATE_MODE, wxGA_HORIZONTAL);
	}
	else {
		delete thread;
		RunningActionId = 0;
	}
}

void t4p::SqlBrowserPanelClass::OnRefreshButton(wxCommandEvent& event) {
	ExecuteQuery(t4p::IcuToWx(LastQuery), Query.DatabaseTag);
}

void t4p::SqlBrowserPanelClass::Stop() {
	if (RunningActionId > 0) {
		Feature->App.RunningThreads.CancelAction(RunningActionId);
		RunningActionId = 0;
		Gauge->StopGauge(ID_SQL_GAUGE);
	}
}

void t4p::SqlBrowserPanelClass::OnQueryComplete(t4p::QueryCompleteEventClass& event) {
	if (event.GetId() == QueryId) {
		t4p::SqlResultClass* result = event.Results;
		Results.push_back(result);
		
		RowToSqlInsert.Columns = result->ColumnNames;
		RowToSqlInsert.CheckedColumns = result->ColumnNames;
		RowToSqlInsert.TableName = UNICODE_STRING_SIMPLE("");
		if (result->TableNames.size() == 1) {
			RowToSqlInsert.TableName = result->TableNames[0];
		}
		
		RowToPhp.Columns = result->ColumnNames;
		RowToPhp.CheckedColumns = result->ColumnNames;
	}
	else {
		event.Skip();
	}
}

void t4p::SqlBrowserPanelClass::RenderAllResults() {
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
		t4p::SqlResultClass* results = Results[i];
		UnicodeString error;
		
		// if only one query was executed: render the results in this panel
		// if more than one query was executed: render any messages for 
		//	in this panel, each new resultset in a new panel
		if (results->Success && results->HasRows && !outputSummary) {
			Fill(results);
		}
		else if (results->Success && results->HasRows && outputSummary) {
			t4p::SqlBrowserPanelClass* newPanel = Feature->CreateResultsPanel(CodeControl);
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
				msg = t4p::IcuToWx(results->Error);
			}
			int rowNumber = ResultsGrid->GetNumberRows();
			wxString queryStart = t4p::IcuToWx(results->Query);
			ResultsGrid->AppendRows(1);
			ResultsGrid->SetCellValue(wxGridCellCoords(rowNumber , 0), wxString::Format(wxT("%d"), results->LineNumber));
			ResultsGrid->SetCellValue(wxGridCellCoords(rowNumber , 1), queryStart.Mid(0, 100)); 
			ResultsGrid->SetCellValue(wxGridCellCoords(rowNumber , 2), msg);
		}
		if (!outputSummary && !results->Error.isEmpty()) {

			// put error in the summary LABEL
			UpdateLabels(t4p::IcuToWx(results->Error));
		}
		else if (!outputSummary) {

			// put summary in the summary LABEL
			wxDateTime now = wxDateTime::Now();
			wxString msg = wxString::Format(_("%d rows affected in %.3f sec [%s]"), results->AffectedRows, 
				(results->QueryTime.ToLong() / 1000.00), now.FormatTime().c_str());
			UpdateLabels(msg);
		}
	}
	if (outputSummary) {
		ResultsGrid->AutoSizeColumn(0);
		ResultsGrid->AutoSizeColumn(1);
		ResultsGrid->AutoSizeColumn(2);
	}
}

void t4p::SqlBrowserPanelClass::Fill(t4p::SqlResultClass* results) {
	FillGridWithResults(ResultsGrid, results);

	if (!results->Success) {
		UpdateLabels(t4p::IcuToWx(results->Error));
	}
	else {
		UpdateLabels(wxString::Format(_("%d rows returned in %.3f sec"), 
			results->AffectedRows, (results->QueryTime.ToLong() / 1000.00)));
	}
}

void t4p::SqlBrowserPanelClass::UpdateLabels(const wxString& result) {
	ResultsLabel->SetLabel(result);
	ResultsLabel->GetContainingSizer()->Layout();
}

void t4p::SqlBrowserPanelClass::OnActionProgress(t4p::ActionProgressEventClass& event) {
	if (event.GetId() == QueryId) {
		Gauge->IncrementGauge(ID_SQL_GAUGE, t4p::StatusBarWithGaugeClass::INDETERMINATE_MODE);
	}
	else {
		event.Skip();
	}
}

void t4p::SqlBrowserPanelClass::OnActionComplete(t4p::ActionEventClass& event) {
	if (event.GetId() == QueryId) {
		RenderAllResults();

		// make sure to cleanup all results
		for (size_t i = 0; i < Results.size(); i++) {
			delete Results[i];
		}
		Results.clear();
		RunningActionId = 0;
		Gauge->StopGauge(ID_SQL_GAUGE);
		Feature->AuiManagerUpdate();
	}
	else {
		event.Skip();
	}
}

void t4p::SqlBrowserPanelClass::SetCurrentInfo(const t4p::DatabaseTagClass& databaseTag) {
	Query.DatabaseTag.Copy(databaseTag);
	if (CodeControl) {
		CodeControl->SetCurrentDbTag(databaseTag);
	}
}

void t4p::SqlBrowserPanelClass::LinkToCodeControl(t4p::CodeControlClass* codeControl) {
	CodeControl = codeControl;
	
	// set the current selected db tag, so that code completion works
	size_t sel = (size_t)Connections->GetSelection();
	std::vector<t4p::DatabaseTagClass> dbTags = Feature->App.Globals.AllEnabledDatabaseTags();
	if (sel >= 0 && sel < dbTags.size()) {
		CodeControl->SetCurrentDbTag(dbTags[sel]);
	}
}

bool t4p::SqlBrowserPanelClass::IsLinkedToCodeControl(t4p::CodeControlClass* codeControl) {
	return CodeControl != NULL && codeControl == CodeControl;
}

void t4p::SqlBrowserPanelClass::UnlinkFromCodeControl() {
	CodeControl = NULL;
}

void t4p::SqlBrowserPanelClass::FillConnectionList() {
	std::vector<t4p::DatabaseTagClass>::const_iterator tag;
	Connections->Clear();
	std::vector<t4p::DatabaseTagClass> dbTags = Feature->App.Globals.AllEnabledDatabaseTags();
	for (tag = dbTags.begin(); tag != dbTags.end(); ++tag) {
		Connections->Append(t4p::IcuToWx(tag->Label));
	}
	this->Layout();
	if (!Connections->IsEmpty()) {
		Connections->SetSelection(0);
		SetCurrentInfo(dbTags[0]);
	}
}

void t4p::SqlBrowserPanelClass::OnConnectionChoice(wxCommandEvent& event) {
	size_t sel = (size_t)event.GetSelection();
	std::vector<t4p::DatabaseTagClass> dbTags = Feature->App.Globals.AllEnabledDatabaseTags();
	if (sel >= 0 && sel < dbTags.size()) {
		SetCurrentInfo(dbTags[sel]);
	}
}

void t4p::SqlBrowserPanelClass::OnGridRightClick(wxGridEvent& event) {
	wxMenu menu;
	wxMenuItem* item;
	
	item = menu.Append(ID_GRID_COPY_ALL, _("Copy All Rows"), _("Copies all rows to the clipboard"));
	item = menu.Append(ID_GRID_COPY_CELL, _("Copy Cell Data"), _("Copies cell data to the clipboard"));
	item = menu.Append(ID_GRID_COPY_ROW, _("Copy Row"), _("Copies the entire row to the clipboard"));
	item = menu.Append(ID_GRID_COPY_ROW_SQL, _("Copy Row As SQL"), _("Copies the entire row As SQL to the clipboard"));
	item->Enable(!RowToSqlInsert.TableName.isEmpty());
		
	item = menu.Append(ID_GRID_COPY_ROW_PHP, _("Copy Row As PHP"), _("Copies the entire row As a PHP Array to the clipboard"));
	item = menu.Append(ID_GRID_OPEN_IN_EDITOR, _("Open Cell in New Buffer"), _("Opens the cell data into an untitled doc"));
	
	SelectedCol = event.GetCol();
	SelectedRow = event.GetRow();
	this->PopupMenu(&menu, event.GetPosition());
}

void t4p::SqlBrowserPanelClass::OnCopyAllRows(wxCommandEvent& event) {
	if (SelectedCol < 0 || SelectedRow < 0) {
		return;
	}
	if (ResultsGrid->GetRows() > 2000) {
		wxMessageBox(wxT("There are too many rows to copy to the clipboard"), wxT("Copy All Rows"));
		return;
	}
	t4p::SqlCopyDialogClass copyDialog(this, wxID_ANY, CopyOptions);
	if (wxOK == copyDialog.ShowModal()) {
		std::vector<wxString> values;
		wxStringOutputStream ostream;
		wxTextOutputStream tstream(ostream, wxEOL_UNIX);
		int cols = ResultsGrid->GetCols();
		int rows = ResultsGrid->GetRows();
		for (int r = 0; r < rows; ++r) {
			for (int c = 0; c < cols; ++c) {
				wxString val = ResultsGrid->GetCellValue(r, c);
				values.push_back(val);
			}
			CopyOptions.Export(values, tstream);
			CopyOptions.EndRow(tstream);
			values.clear();
		}
		wxString toExport = ostream.GetString();
		if (!toExport.empty()) {
			if (wxTheClipboard->Open()) {
				wxTheClipboard->SetData(new wxTextDataObject(toExport));
				wxTheClipboard->Close();
			}
		}
	}
}

void t4p::SqlBrowserPanelClass::OnCopyRow(wxCommandEvent& event) {
	if (SelectedCol < 0 || SelectedRow < 0) {
		return;
	}
	t4p::SqlCopyDialogClass copyDialog(this, wxID_ANY, CopyOptions);
	if (wxOK == copyDialog.ShowModal()) {
		std::vector<wxString> values;
		wxStringOutputStream ostream;
		wxTextOutputStream tstream(ostream, wxEOL_UNIX);
		for (int i = 0; i < ResultsGrid->GetCols(); ++i) {
			wxString val = ResultsGrid->GetCellValue(SelectedRow, i);
			values.push_back(val);
		}
		CopyOptions.Export(values, tstream);
		
		wxString toExport = ostream.GetString();
		if (!toExport.empty()) {
			if (wxTheClipboard->Open()) {
				wxTheClipboard->SetData(new wxTextDataObject(toExport));
				wxTheClipboard->Close();
			}
		}
	}
}

void t4p::SqlBrowserPanelClass::OnCopyRowAsSql(wxCommandEvent& event) {
	if (SelectedCol < 0 || SelectedRow < 0) {
		return;
	}
	if (RowToSqlInsert.TableName.isEmpty()) {
		return;
	}
	RowToSqlInsert.Values.clear();
	RowToSqlInsert.CheckedValues.clear();
	for (int i = 0; i < ResultsGrid->GetCols(); ++i) {
		wxString val = ResultsGrid->GetCellValue(SelectedRow, i);
		RowToSqlInsert.Values.push_back(t4p::WxToIcu(val));
	}
	RowToSqlInsert.CheckedValues = RowToSqlInsert.Values;
		
	t4p::SqlCopyAsInsertDialogClass copyDialog(this, wxID_ANY, RowToSqlInsert);
	if (wxOK == copyDialog.ShowModal() && !RowToSqlInsert.CheckedColumns.empty() 
			&& !RowToSqlInsert.CheckedValues.empty()) {
		UnicodeString sql = RowToSqlInsert.CreateStatement(Query.DatabaseTag.Driver);
		
		wxString wxSql = t4p::IcuToWx(sql);
		if (wxTheClipboard->Open()) {
			wxTheClipboard->SetData(new wxTextDataObject(wxSql));
			wxTheClipboard->Close();
		}
	}
}

void t4p::SqlBrowserPanelClass::OnCopyRowAsPhp(wxCommandEvent& event) {
	if (SelectedCol < 0 || SelectedRow < 0) {
		return;
	}
	RowToPhp.Values.clear();
	RowToPhp.CheckedValues.clear();
	for (int i = 0; i < ResultsGrid->GetCols(); ++i) {
		wxString val = ResultsGrid->GetCellValue(SelectedRow, i);
		RowToPhp.Values.push_back(t4p::WxToIcu(val));
	}
		
	t4p::SqlCopyAsPhpDialogClass copyDialog(this, wxID_ANY, RowToPhp);
	if (wxOK == copyDialog.ShowModal() && !RowToPhp.CheckedColumns.empty() 
			&& !RowToPhp.CheckedValues.empty()) {
		UnicodeString code = RowToPhp.CreatePhpArray();
		
		wxString wxCode = t4p::IcuToWx(code);
		if (wxTheClipboard->Open()) {
			wxTheClipboard->SetData(new wxTextDataObject(wxCode));
			wxTheClipboard->Close();
		}
	}
}

void t4p::SqlBrowserPanelClass::OnCopyCellData(wxCommandEvent& event) {
	if (SelectedCol < 0 || SelectedRow < 0) {
		return;
	}
	wxString val = ResultsGrid->GetCellValue(SelectedRow, SelectedCol);
	if (!val.empty()) {
		if (wxTheClipboard->Open()) {
			wxTheClipboard->SetData(new wxTextDataObject(val));
			wxTheClipboard->Close();
		}
	}
}

void t4p::SqlBrowserPanelClass::OnOpenInEditor(wxCommandEvent& event) {
	if (SelectedCol < 0 || SelectedRow < 0) {
		return;
	}
	wxString val = ResultsGrid->GetCellValue(SelectedRow, SelectedCol);
	if (!val.empty()) {
		Feature->NewTextBuffer(val);
	}
}

t4p::SqlBrowserFeatureClass::SqlBrowserFeatureClass(t4p::AppClass& app) 
	: FeatureClass(app) {
}

t4p::SqlBrowserFeatureClass::~SqlBrowserFeatureClass() {
}

void t4p::SqlBrowserFeatureClass::DetectMetadata() {

	// thread will be owned by SequenceClass
	t4p::SqlMetaDataActionClass* thread = new t4p::SqlMetaDataActionClass(App.SqliteRunningThreads, t4p::ID_EVENT_ACTION_SQL_METADATA);
	std::vector<t4p::GlobalActionClass*> actions;
	actions.push_back(thread);
	App.Sequences.Build(actions);
}

void t4p::SqlBrowserFeatureClass::AddNewMenu(wxMenuBar* menuBar) {
	wxMenu* sqlMenu = new wxMenu();
	sqlMenu->Append(t4p::MENU_SQL + 0, _("SQL Browser\tSHIFT+F9"), _("Open a window for SQL browsing"),
		wxITEM_NORMAL);
	sqlMenu->Append(t4p::MENU_SQL + 1, _("SQL Connections\tCTRL+F9"), _("Show & Pick The SQL Connections that this project uses"),
		wxITEM_NORMAL);
	sqlMenu->Append(t4p::MENU_SQL + 2, _("Run Queries in SQL Browser\tF9"), _("Execute the query that is currently in the SQL Browser"),
		wxITEM_NORMAL);
	sqlMenu->Append(t4p::MENU_SQL + 3, _("Detect SQL Meta Data"), _("Detect SQL Meta data so that it is made available to code completion"),
		wxITEM_NORMAL);
	menuBar->Append(sqlMenu, _("SQL"));
}

void t4p::SqlBrowserFeatureClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	wxBitmap bmp = t4p::IconImageAsset(wxT("sql"));
	toolBar->AddTool(t4p::MENU_SQL + 0, _("SQL Browser"), bmp, wxT("Open the SQL Browser"), wxITEM_NORMAL);
}

void  t4p::SqlBrowserFeatureClass::OnSqlBrowserToolsMenu(wxCommandEvent& event) {
	int num = 1;
	t4p::NotebookClass* notebook = GetNotebook();
	for (size_t i = 0; i < notebook->GetPageCount(); i++) {
		wxString name = notebook->GetPageText(i);
		if (name.EndsWith(wxT(".sql")) || name.Index(_("SQL Browser")) == 0) {
			num++;
		}
	}
	
	t4p::CodeControlClass* ctrl = CreateCodeControl(wxString::Format(_("SQL Browser %d"), num), t4p::CodeControlClass::SQL);
	CreateResultsPanel(ctrl);
	ctrl->SetFocus();
}

t4p::SqlBrowserPanelClass* t4p::SqlBrowserFeatureClass::CreateResultsPanel(t4p::CodeControlClass* codeControl) {
	t4p::SqlQueryClass query;
	t4p::SqlBrowserPanelClass* sqlPanel = new SqlBrowserPanelClass(GetToolsNotebook(), wxNewId(), GetStatusBarWithGauge(), 
		query, this);
	t4p::NotebookClass* codeNotebook = GetNotebook();
	wxString tabText = codeNotebook->GetPageText(codeNotebook->GetPageIndex(codeControl));

	// name the windows, since there could be multiple windows from various features; we want to know which opened tools windows
	// are from this feature
	wxBitmap tableBitmap = t4p::IconImageAsset(wxT("table"));
	AddToolsWindow(sqlPanel, tabText, wxT("t4p::SqlBrowserPanelClass"), tableBitmap);
	sqlPanel->LinkToCodeControl(codeControl);
	return sqlPanel;
}

void t4p::SqlBrowserFeatureClass::OnRun(wxCommandEvent& event) {
	t4p::CodeControlClass* ctrl = GetNotebook()->GetCurrentCodeControl();
	if (ctrl && ctrl->GetDocumentMode() == t4p::CodeControlClass::SQL) {
		
		// look for results panel that corresponds to the current code control
		wxAuiNotebook* notebook = GetToolsNotebook();
		bool found = false;
		for (size_t i = 0; i < notebook->GetPageCount(); i++) {
			wxWindow* window = notebook->GetPage(i);

			// only cast when we are sure of the type of window
			// not using wxDynamicCast since SqlBrowserPanelClass does not implement the required 
			// methods
			if (window->GetName() == wxT("t4p::SqlBrowserPanelClass")) {
				t4p::SqlBrowserPanelClass* panel = (t4p::SqlBrowserPanelClass*)window;
				if (panel->IsLinkedToCodeControl(ctrl)) {
				
					// we found the panel bring it to the forefront and run the query
					found = true;
					SetFocusToToolsWindow(window);
					panel->ExecuteCodeControl();
					break;
				}
			}
		}
		if (!found) {
			t4p::SqlBrowserPanelClass* panel = CreateResultsPanel(ctrl);
			panel->ExecuteCodeControl();
		}
	}
}

void t4p::SqlBrowserFeatureClass::OnSqlConnectionMenu(wxCommandEvent& event) {

	// decided to always allow the user to edit the connection info in order to
	// allow the user to create a new database from within the editor (the very 
	// first time a new project is created; its database may not exist).
	// before, a user would not be able to edit the connection info once it was detected
	// in order to make it less confusing about where the connection info comes from.
	t4p::SqlConnectionListDialogClass dialog(GetMainWindow(), App.Globals.DatabaseTags, App.RunningThreads);
	if (dialog.ShowModal() == wxOK) {
		
		// if chosen connection changed need to update the code control so that it knows to use the new
		// connection for auto completion purposes
		// all SQL panels are updated.
		wxAuiNotebook* notebook = GetToolsNotebook();
		for (size_t i = 0; i < notebook->GetPageCount(); ++i) {
			wxWindow* window = notebook->GetPage(i);
			
			// only cast when we are sure of the type of window
			// not using wxDynamicCast since SqlBrowserPanelClass does not implement the required 
			// methods
			if (window->GetName() == wxT("t4p::SqlBrowserPanelClass")) {
				t4p::SqlBrowserPanelClass* panel = (t4p::SqlBrowserPanelClass*)window;
				panel->FillConnectionList();
			}
		}
		SavePreferences();

		// redetect the SQL meta data
		t4p::SqlMetaDataActionClass* thread = new t4p::SqlMetaDataActionClass(App.RunningThreads, t4p::ID_EVENT_ACTION_SQL_METADATA);
		if (thread->Init(App.Globals)) {
			App.RunningThreads.Queue(thread);
		}
		else {
			delete thread;
		}
	}
}

void t4p::SqlBrowserFeatureClass::OnSqlDetectMenu(wxCommandEvent& event) {
	DetectMetadata();
}

void t4p::SqlBrowserFeatureClass::OnContentNotebookPageChanged(wxAuiNotebookEvent& event) {
	t4p::CodeControlClass* contentWindow = GetNotebook()->GetCodeControl(event.GetSelection());
	if (contentWindow) {
		wxAuiNotebook* notebook = GetToolsNotebook();
		for (size_t i = 0; i < notebook->GetPageCount(); i++) {
			wxWindow* toolsWindow = notebook->GetPage(i);

			// only cast when we are sure of the type of window
			// not using wxDynamicCast since SqlBrowserPanelClass does not implement the required 
			// methods
			if (toolsWindow->GetName() == wxT("t4p::SqlBrowserPanelClass")) {
				t4p::SqlBrowserPanelClass* panel = (t4p::SqlBrowserPanelClass*)toolsWindow;
				if (panel->IsLinkedToCodeControl(contentWindow)) {
					
					// we found the panel bring it to the forefront and run the query
					SetFocusToToolsWindow(toolsWindow);
					break;
				}
			}
		}
	}
}

void t4p::SqlBrowserFeatureClass::OnContentNotebookPageClose(wxAuiNotebookEvent& event) {
	t4p::CodeControlClass* contentWindow = GetNotebook()->GetCodeControl(event.GetSelection());
	if (contentWindow) {
		wxAuiNotebook* notebook = GetToolsNotebook();
		for (size_t i = 0; i < notebook->GetPageCount(); i++) {
			wxWindow* toolsWindow = notebook->GetPage(i);

			// only cast when we are sure of the type of window
			// not using wxDynamicCast since SqlBrowserPanelClass does not implement the required 
			// methods
			if (toolsWindow->GetName() == wxT("t4p::SqlBrowserPanelClass")) {
				t4p::SqlBrowserPanelClass* panel = (t4p::SqlBrowserPanelClass*)toolsWindow;
				if (panel->IsLinkedToCodeControl(contentWindow)) {
					panel->UnlinkFromCodeControl();
				}
			}
		}
	}
}

void t4p::SqlBrowserFeatureClass::OnToolsNotebookPageClose(wxAuiNotebookEvent& event) {
	wxAuiNotebook* notebook = GetToolsNotebook();
	int sel = event.GetSelection();
	wxWindow* toolsWindow = notebook->GetPage(sel);

	// only cast when we are sure of the type of window
	// not using wxDynamicCast since SqlBrowserPanelClass does not implement the required 
	// methods
	if (toolsWindow->GetName() == wxT("t4p::SqlBrowserPanelClass")) {
		t4p::SqlBrowserPanelClass* panel = (t4p::SqlBrowserPanelClass*)toolsWindow;

		// the constructor added itself as an event handler
		App.RunningThreads.RemoveEventHandler(panel);
		panel->Stop();
	}
}

void t4p::SqlBrowserFeatureClass::OnAppExit(wxCommandEvent& event) {
	wxAuiNotebook* notebook = GetToolsNotebook();
	for (size_t i = 0; i < notebook->GetPageCount(); ++i) {
		wxWindow* toolsWindow = notebook->GetPage(i);

		// only cast when we are sure of the type of window
		// not using wxDynamicCast since SqlBrowserPanelClass does not implement the required 
		// methods
		if (toolsWindow->GetName() == wxT("t4p::SqlBrowserPanelClass")) {
			t4p::SqlBrowserPanelClass* panel = (t4p::SqlBrowserPanelClass*)toolsWindow;

			// the constructor added itself as an event handler
			App.RunningThreads.RemoveEventHandler(panel);
			panel->Stop();
		}
	}
}

void t4p::SqlBrowserFeatureClass::AddKeyboardShortcuts(std::vector<t4p::DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[t4p::MENU_SQL + 0] = wxT("SQL-Browser");
	menuItemIds[t4p::MENU_SQL + 1] = wxT("SQL-Connections");
	menuItemIds[t4p::MENU_SQL + 2] = wxT("SQL-Run Queries");
	menuItemIds[t4p::MENU_SQL + 3] = wxT("SQL-Detect SQL Meta Data");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void t4p::SqlBrowserFeatureClass::AuiManagerUpdate() {
	AuiManager->Update();
}

void t4p::SqlBrowserFeatureClass::LoadPreferences(wxConfigBase* config) {
	App.Globals.DatabaseTags.clear();
	wxString groupName;
	long index = 0;
	if (config->GetFirstGroup(groupName, index)) {
		do {
			if (groupName.Find(wxT("DatabaseInfo_")) == 0) {
				t4p::DatabaseTagClass info;
				info.Schema = t4p::WxToIcu(config->Read(groupName + wxT("/DatabaseName")));
				wxString driverString = config->Read(groupName + wxT("/Driver"));
				info.FileName.Assign(config->Read(groupName + wxT("/FileName")));
				info.Host = t4p::WxToIcu(config->Read(groupName + wxT("/Host")));
				info.IsDetected = false;
				info.Label = t4p::WxToIcu(config->Read(groupName + wxT("/Label")));
				info.Password = t4p::WxToIcu(config->Read(groupName + wxT("/Password")));
				config->Read(groupName + wxT("/Port"), &info.Port);
				info.User = t4p::WxToIcu(config->Read(groupName + wxT("/User")));
				config->Read(groupName + wxT("/IsEnabled"), &info.IsEnabled);
				
				if (driverString.CmpNoCase(wxT("MYSQL")) == 0) {
					info.Driver = t4p::DatabaseTagClass::MYSQL;
					App.Globals.DatabaseTags.push_back(info);
				}
				else if (driverString.CmpNoCase(wxT("SQLITE")) == 0) {
					info.Driver = t4p::DatabaseTagClass::SQLITE;
					App.Globals.DatabaseTags.push_back(info);
				}
			}
		} while (config->GetNextGroup(groupName, index));
	}
}

void t4p::SqlBrowserFeatureClass::SavePreferences() {
	wxConfigBase* config = wxConfig::Get();

	// delete any previous connections that are in the config
	wxString key;
	long index = 0;
	bool next = config->GetFirstGroup(key, index);
	std::vector<wxString> keysToDelete;
	while (next) {
		if (key.Find(wxT("DatabaseInfo_")) == 0) {
			keysToDelete.push_back(key);
		}
		next = config->GetNextGroup(key, index);
	}
	for (size_t i = 0; i < keysToDelete.size(); ++i) {
		config->DeleteGroup(keysToDelete[i]);
	}
	
	// now save all of the new ones
	int saveIndex = 0;
	for (size_t i = 0; i < App.Globals.DatabaseTags.size(); ++i) {
		if (!App.Globals.DatabaseTags[i].IsDetected) {
			config->SetPath(wxString::Format(wxT("/DatabaseInfo_%d"), saveIndex));
			config->Write(wxT("DatabaseName"), t4p::IcuToWx(App.Globals.DatabaseTags[i].Schema));
			wxString driverString;
			if (t4p::DatabaseTagClass::MYSQL == App.Globals.DatabaseTags[i].Driver) {
				driverString = wxT("MYSQL");
			}
			else if (t4p::DatabaseTagClass::SQLITE == App.Globals.DatabaseTags[i].Driver) {
				driverString = wxT("SQLITE");
			}
			config->Write(wxT("Driver"), driverString);
			config->Write(wxT("FileName"), App.Globals.DatabaseTags[i].FileName.GetFullPath());
			config->Write(wxT("Host"), t4p::IcuToWx(App.Globals.DatabaseTags[i].Host));
			config->Write(wxT("Label"), t4p::IcuToWx(App.Globals.DatabaseTags[i].Label));
			config->Write(wxT("Password"), t4p::IcuToWx(App.Globals.DatabaseTags[i].Password));
			config->Write(wxT("Port"), App.Globals.DatabaseTags[i].Port);
			config->Write(wxT("User"), t4p::IcuToWx(App.Globals.DatabaseTags[i].User));
			config->Write(wxT("IsEnabled"), App.Globals.DatabaseTags[i].IsEnabled);
			saveIndex++;
		}
	}
	config->SetPath(wxT("/"));
	config->Flush();

	// signal that this app has modified the config file, that way the external
	// modification check fails and the user will not be prompted to reload the config
	App.UpdateConfigModifiedTime();
}

void t4p::SqlBrowserFeatureClass::OnCmdTableDataOpen(t4p::OpenDbTableCommandEventClass& event) {
	
	// find the connection to use by hash, 
	t4p::DatabaseTagClass tag;
	bool found = App.Globals.FindDatabaseTagByHash(event.ConnectionHash, tag);
	if (found) {
		t4p::SqlQueryClass query;
		t4p::SqlBrowserPanelClass* sqlPanel = new SqlBrowserPanelClass(GetToolsNotebook(), wxNewId(), GetStatusBarWithGauge(), 
			query, this);
		
		wxString tabText = event.DbTableName;
		wxString sql = "SELECT * FROM " + event.DbTableName + " LIMIT 100";

		// name the windows, since there could be multiple windows from various features; we want to know which opened tools windows
		// are from this feature
		wxBitmap tableBitmap = t4p::IconImageAsset(wxT("table"));
		AddToolsWindow(sqlPanel, tabText, wxT("t4p::SqlBrowserPanelClass"), tableBitmap);
		sqlPanel->ExecuteQuery(sql, tag);
	}
}

void t4p::SqlBrowserFeatureClass::NewSqlBuffer(const wxString& sql) {
	GetNotebook()->AddTriumphPage(t4p::CodeControlClass::SQL);
	t4p::CodeControlClass* ctrl = GetCurrentCodeControl();
	if (ctrl) {
		ctrl->SetText(sql);
	}
}

void t4p::SqlBrowserFeatureClass::NewTextBuffer(const wxString& text) {
	GetNotebook()->AddTriumphPage(t4p::CodeControlClass::TEXT);
	t4p::CodeControlClass* ctrl = GetCurrentCodeControl();
	if (ctrl) {
		ctrl->SetText(text);
	}
}

void t4p::SqlBrowserFeatureClass::OnCmdTableDefinitionOpen(t4p::OpenDbTableCommandEventClass& event) {
	
	// find the connection to use by hash, 
	t4p::DatabaseTagClass tag;
	bool found = App.Globals.FindDatabaseTagByHash(event.ConnectionHash, tag);
	if (found) {
		
		// if there is an existing table definition panel use that
		wxWindow* win = wxWindow::FindWindowById(ID_PANEL_TABLE_DEFINITION, GetToolsNotebook());
		t4p::TableDefinitionPanelClass* sqlPanel = NULL;
		if (win) {
			sqlPanel = (t4p::TableDefinitionPanelClass*)win;
			SetFocusToToolsWindow(win);
		}
		else {
			sqlPanel = new TableDefinitionPanelClass(GetMainWindow(), ID_PANEL_TABLE_DEFINITION, *this);
			wxString tabText = wxT("Table Definition");
		
			// name the windows, since there could be multiple windows from various features; we want to know which opened tools windows
			// are from this feature
			wxBitmap tableBitmap = t4p::IconImageAsset(wxT("database-medium"));
			AddToolsWindow(sqlPanel, tabText, wxT("t4p::TableDefinitionPanelClass"), tableBitmap);
		}
		sqlPanel->ShowTable(tag, event.DbTableName);
	}
}

t4p::TableDefinitionPanelClass::TableDefinitionPanelClass(wxWindow* parent, int id, t4p::SqlBrowserFeatureClass& feature)
: TableDefinitionPanelGeneratedClass(parent, id)
, Feature(feature) 
, RunningThreads()
, TableConnectionIdentifier() 
, IndexConnectionIdentifier() {
	Connections->Clear();
	FillConnectionList();
	RunningThreads.AddEventHandler(this);
	RefreshButton->SetBitmap(t4p::IconImageAsset("outline-refresh"));

	DefinitionIndicesPanel = new t4p::DefinitionIndicesPanelClass(Notebook);
	DefinitionColumnsPanel = new t4p::DefinitionColumnsPanelClass(Notebook);

	Notebook->SetWindowStyle(wxAUI_NB_BOTTOM);
	Notebook->AddPage(DefinitionColumnsPanel, _("Columns"));
	Notebook->AddPage(DefinitionIndicesPanel, _("Indices"));
	this->Layout();
}

t4p::TableDefinitionPanelClass::~TableDefinitionPanelClass() {
	RunningThreads.RemoveEventHandler(this);
}

void t4p::TableDefinitionPanelClass::FillConnectionList() {
	Connections->Clear();
	std::vector<t4p::DatabaseTagClass> dbTags = Feature.App.Globals.AllEnabledDatabaseTags();
	for (size_t i = 0; i < dbTags.size(); ++i) {
		Connections->Append(t4p::IcuToWx(dbTags[i].Label));
	}
}


void t4p::TableDefinitionPanelClass::ShowTable(const t4p::DatabaseTagClass& tag, const wxString& tableName) {
	TableName->SetValue(tableName);
	
	// select the connection to the one to show
	std::vector<t4p::DatabaseTagClass> dbTags = Feature.App.Globals.AllEnabledDatabaseTags();
	int indexToSelect = 0;
	for (size_t i = 0; i < dbTags.size(); ++i) {
		if (dbTags[i].ConnectionHash() == tag.ConnectionHash()) {
			indexToSelect = i;
			Connections->Select(indexToSelect);
			break;
		}
	}
	
	// trigger the description query
	t4p::SqlQueryClass query;
	query.DatabaseTag = tag;
	UnicodeString columnSql;
	if (t4p::DatabaseTagClass::MYSQL == tag.Driver) {
		columnSql = t4p::WxToIcu("DESC " + tableName);
	}
	else if (t4p::DatabaseTagClass::SQLITE == tag.Driver) {
		columnSql = t4p::WxToIcu("PRAGMA table_info('" + tableName + "')");
	}
	t4p::MultipleSqlExecuteClass* sqlDefExecute = 
		new t4p::MultipleSqlExecuteClass(RunningThreads, ID_SQL_TABLE_DEFINITION, TableConnectionIdentifier);
	if (sqlDefExecute->Init(columnSql, query)) {
		RunningThreads.Queue(sqlDefExecute);
	}
	
	UnicodeString indexSql;
	if (t4p::DatabaseTagClass::MYSQL == tag.Driver) {
		indexSql = t4p::WxToIcu("SHOW INDEX FROM " + tableName);
	}
	else if (t4p::DatabaseTagClass::SQLITE == tag.Driver) {
		indexSql = t4p::WxToIcu("PRAGMA index_list('" + tableName + "')");
	}
	t4p::MultipleSqlExecuteClass* sqlIndexExecute = 
		new t4p::MultipleSqlExecuteClass(RunningThreads, ID_SQL_TABLE_INDICES, IndexConnectionIdentifier);
	if (sqlIndexExecute->Init(indexSql, query)) {
		RunningThreads.Queue(sqlIndexExecute);
	}
}

void t4p::TableDefinitionPanelClass::OnColumnSqlComplete(t4p::QueryCompleteEventClass& event) {
	wxWindowUpdateLocker locker(this);
	t4p::SqlResultClass* result = event.Results;
	if (result) {
		DefinitionColumnsPanel->Fill(result);
		delete result;
	}
	this->Layout();
}

void t4p::TableDefinitionPanelClass::OnIndexSqlComplete(t4p::QueryCompleteEventClass& event) {
	wxWindowUpdateLocker locker(this);
	t4p::SqlResultClass* result = event.Results;
	if (result) {
		DefinitionIndicesPanel->Fill(result);
		delete result;
	}
	this->Layout();
}


void t4p::TableDefinitionPanelClass::OnTableNameEnter(wxCommandEvent& event) {
	
	// select the connection to the one to show
	std::vector<t4p::DatabaseTagClass> dbTags = Feature.App.Globals.AllEnabledDatabaseTags();
	size_t selectedIndex = Connections->GetSelection();
	if (selectedIndex >= 0 && selectedIndex < dbTags.size()) {
		t4p::DatabaseTagClass selectedTag = dbTags[selectedIndex];
		ShowTable(selectedTag, TableName->GetValue());
	}
}

void t4p::TableDefinitionPanelClass::OnSqlButton(wxCommandEvent& event) {
	std::vector<t4p::DatabaseTagClass> dbTags = Feature.App.Globals.AllEnabledDatabaseTags();
	size_t selectedIndex = Connections->GetSelection();
	if (selectedIndex >= dbTags.size()) {
		return;
	}
	t4p::DatabaseTagClass selectedTag = dbTags[selectedIndex];
	
	
	UnicodeString createSql;
	wxString tableName = TableName->GetValue();
	if (t4p::DatabaseTagClass::MYSQL == selectedTag.Driver) {
		createSql = t4p::WxToIcu("SHOW CREATE TABLE " + tableName);
	}
	else if (t4p::DatabaseTagClass::SQLITE == selectedTag.Driver) {
		createSql = t4p::WxToIcu("SELECT sql FROM sqlite_master WHERE type='table' AND name= '" + tableName + "'");
	}
	t4p::SqlQueryClass query;
	query.DatabaseTag = selectedTag;
	t4p::MultipleSqlExecuteClass* sqlCreateExecute = 
		new t4p::MultipleSqlExecuteClass(RunningThreads, ID_SQL_TABLE_CREATE, IndexConnectionIdentifier);
	if (sqlCreateExecute->Init(createSql, query)) {
		RunningThreads.Queue(sqlCreateExecute);
	}
}

void t4p::TableDefinitionPanelClass::OnCreateSqlComplete(t4p::QueryCompleteEventClass& event) {
	t4p::SqlResultClass* result = event.Results;
	if (!result) {
		return;
	}
	if (!result->Error.isEmpty()) {
		DefinitionColumnsPanel->Fill(result);
	}
	else if (!result->StringResults.empty() && result->StringResults[0].size() == 2) {
		
		// mysql results
		UnicodeString table = result->StringResults[0][1];
		wxString sqlText = t4p::IcuToWx(table);
		Feature.NewSqlBuffer(sqlText);
	}
	else if (!result->StringResults.empty() && result->StringResults[0].size() == 1) {
		
		// sqlite results
		UnicodeString table = result->StringResults[0][0];
		wxString sqlText = t4p::IcuToWx(table);
		Feature.NewSqlBuffer(sqlText);
	}
	
	delete result;
}

void t4p::TableDefinitionPanelClass::OnRefreshButton(wxCommandEvent& event) {
	OnTableNameEnter(event);
}

t4p::DefinitionIndicesPanelClass::DefinitionIndicesPanelClass(wxWindow* parent)
: DefinitionIndicesPanelGeneratedClass(parent, wxID_ANY) {
	IndicesGrid->ClearGrid();

}

void t4p::DefinitionIndicesPanelClass::Fill(t4p::SqlResultClass* result) {
	IndicesGrid->ClearGrid();
	FillGridWithResults(IndicesGrid, result);
}

t4p::DefinitionColumnsPanelClass::DefinitionColumnsPanelClass(wxWindow* parent)
: DefinitionColumnsPanelGeneratedClass(parent, wxID_ANY) {
	ColumnsGrid->ClearGrid();
}

void t4p::DefinitionColumnsPanelClass::Fill(t4p::SqlResultClass* result) {
	ColumnsGrid->ClearGrid();
	FillGridWithResults(ColumnsGrid, result);
}


t4p::SqlCopyOptionsClass::SqlCopyOptionsClass()
: ColumnDelim(wxT(","))
, ColumnEnclosure(wxT("\""))
, RowDelim(wxT("\\n"))
, NullFiller(wxT("")) {
}

t4p::SqlCopyOptionsClass::SqlCopyOptionsClass(const t4p::SqlCopyOptionsClass& src)
: ColumnDelim(wxT(","))
, ColumnEnclosure(wxT("\""))
, RowDelim(wxT("\n"))
, NullFiller(wxT("")) {
	Copy(src);
}

t4p::SqlCopyOptionsClass& t4p::SqlCopyOptionsClass::operator=(const t4p::SqlCopyOptionsClass& src) {
	Copy(src);
	return *this;
}

void t4p::SqlCopyOptionsClass::Copy(const t4p::SqlCopyOptionsClass& src) {
	ColumnDelim = src.ColumnDelim;
	ColumnEnclosure = src.ColumnEnclosure;
	RowDelim = src.RowDelim;
	NullFiller = src.NullFiller;
}

void t4p::SqlCopyOptionsClass::Export(std::vector<wxString> values, wxTextOutputStream& stream) {
	wxString trueColumnDelim;
	if (ColumnDelim == wxT("\\t")) {
		trueColumnDelim = wxT("\t");
	}
	else if (ColumnDelim == wxT("\\n")) {
		trueColumnDelim = wxT("\n");
	}
	else if (ColumnDelim == wxT("\\f")) {
		trueColumnDelim = wxT("\f");
	}
	else {
		trueColumnDelim = ColumnDelim;
	}
	
	
	size_t size = values.size();
	for (size_t i = 0; i < size; ++i) {
		wxString val = values[i];
		
		stream.WriteString(ColumnEnclosure);
		
		// write the cell value
		if (val == wxT("<NULL>")) {
			stream.WriteString(NullFiller);
		}
		else {
			for (size_t j = 0; j < val.length(); ++j) {
				if (val[j] == ColumnEnclosure) {
					stream.PutChar(wxT('\\'));
					stream.WriteString(ColumnEnclosure);
				}
				else {
					stream.PutChar(val[j]);
				}
			}
		}
		stream.WriteString(ColumnEnclosure);
		
		if (i >= 0 && i < (size - 1)) {
			stream.WriteString(trueColumnDelim);
		}
	}
}

void t4p::SqlCopyOptionsClass::EndRow(wxTextOutputStream& stream) {
	wxString trueRowDelim;
	if (RowDelim == wxT("\\t")) {
		trueRowDelim = wxT("\t");
	}
	else if (RowDelim == wxT("\\n")) {
		trueRowDelim = wxT("\n");
	}
	else if (RowDelim == wxT("\\f")) {
		trueRowDelim = wxT("\f");
	}
	else {
		trueRowDelim = RowDelim;
	}
	stream.WriteString(trueRowDelim);
}

t4p::SqlCopyDialogClass::SqlCopyDialogClass(wxWindow* parent, int id, t4p::SqlCopyOptionsClass& options)
: SqlCopyDialogGeneratedClass(parent, id)
, EditedOptions(options) 
, OriginalOptions(options) {
	
	wxTextValidator columnDelimValidator(wxFILTER_NONE, &EditedOptions.ColumnDelim);
	ColumnDelim->SetValidator(columnDelimValidator);
	
	wxTextValidator columnEnclosureValidator(wxFILTER_NONE, &EditedOptions.ColumnEnclosure);
	ColumnEnclosure->SetValidator(columnEnclosureValidator);
	
	wxTextValidator rowDelimValidator(wxFILTER_NONE, &EditedOptions.RowDelim);
	RowDelim->SetValidator(rowDelimValidator);
	
	wxTextValidator nullFillerValidator(wxFILTER_NONE, &EditedOptions.NullFiller);
	NullFiller->SetValidator(nullFillerValidator);
}


void t4p::SqlCopyDialogClass::OnCancelButton(wxCommandEvent& event) {
	EndModal(wxCANCEL);
}

void t4p::SqlCopyDialogClass::OnOkButton(wxCommandEvent& event) {
	OriginalOptions = EditedOptions;
	
	EndModal(wxOK);
}


t4p::SqlCopyAsInsertDialogClass::SqlCopyAsInsertDialogClass(wxWindow* parent, int id, 
	t4p::RowToSqlInsertClass& rowToSql)
: SqlCopyAsInsertDialogGeneratedClass(parent, id)
, EditedRowToSql(rowToSql)
, RowToSql(rowToSql)
, HasCheckedAll(false) {
	
	for (size_t i = 0; i < RowToSql.Columns.size(); ++i) {
		Columns->Append(t4p::IcuToWx(RowToSql.Columns[i]));
		Columns->Check(i);
	}
	
	if (EditedRowToSql.LineMode == t4p::RowToSqlInsertClass::SINGLE_LINE) {
		LineModeRadio->SetSelection(0);
	}
	else {
		LineModeRadio->SetSelection(1);
	}
}

void t4p::SqlCopyAsInsertDialogClass::OnCheckAll(wxCommandEvent& event) {
	HasCheckedAll = !HasCheckedAll;
	for (size_t i = 0; i < Columns->GetCount(); ++i) {
		Columns->Check(i, HasCheckedAll);
	}
}

void t4p::SqlCopyAsInsertDialogClass::OnCancelButton(wxCommandEvent& event) {
	return EndModal(wxCANCEL);
}

void t4p::SqlCopyAsInsertDialogClass::OnOkButton(wxCommandEvent& event) {
	TransferDataFromWindow();
	
	EditedRowToSql.CheckedColumns.clear();
	EditedRowToSql.CheckedValues.clear();
	for (size_t i = 0; i < Columns->GetCount(); ++i) {
		if (Columns->IsChecked(i)) {
			EditedRowToSql.CheckedColumns.push_back(EditedRowToSql.Columns[i]);
			if (i >= 0 && i < EditedRowToSql.Values.size()) {
				EditedRowToSql.CheckedValues.push_back(EditedRowToSql.Values[i]);
			}
		}
	}
	
	if (LineModeRadio->GetSelection() == 0) {
		EditedRowToSql.LineMode = t4p::RowToSqlInsertClass::SINGLE_LINE;
	}
	else {
		EditedRowToSql.LineMode = t4p::RowToSqlInsertClass::MULTI_LINE;
	}
	
	RowToSql = EditedRowToSql;
	
	return EndModal(wxOK);
}

t4p::RowToSqlInsertClass::RowToSqlInsertClass()
: Values()
, Columns()
, TableName()
, CheckedColumns()
, CheckedValues()
, LineMode(MULTI_LINE)
{
}

t4p::RowToSqlInsertClass::RowToSqlInsertClass(const t4p::RowToSqlInsertClass& src)
: Values()
, Columns()
, TableName()
, CheckedColumns()
, CheckedValues()
, LineMode(MULTI_LINE)
{
	Copy(src);
}

t4p::RowToSqlInsertClass& t4p::RowToSqlInsertClass::operator=(const t4p::RowToSqlInsertClass& src) {
	Copy(src);
	return *this;
}

void t4p::RowToSqlInsertClass::Copy(const t4p::RowToSqlInsertClass& src) {
	Values = src.Values;
	Columns = src.Columns;
	TableName = src.TableName;
	CheckedColumns = src.CheckedColumns;
	CheckedValues = src.CheckedValues;
	LineMode = src.LineMode;
}

UnicodeString t4p::RowToSqlInsertClass::CreateStatement(t4p::DatabaseTagClass::Drivers driver) const {
	UnicodeString query;
	
	UnicodeString columnDelim;
	switch (driver) {
	case t4p::DatabaseTagClass::MYSQL:
		columnDelim = '`';
		break;
	case t4p::DatabaseTagClass::SQLITE:
		columnDelim = '"';
		break;
	}
	bool doMultiLine = MULTI_LINE == LineMode;
	
	query = UNICODE_STRING_SIMPLE("INSERT INTO ");
	query += TableName;
	query += UNICODE_STRING_SIMPLE(" (");
	for (size_t i = 0; i < CheckedColumns.size(); ++i) {
		if (doMultiLine && (i % 5) == 0) {
			query += UNICODE_STRING_SIMPLE("\n");
		}
		query += columnDelim + CheckedColumns[i] + columnDelim;
		if (i < (CheckedColumns.size() - 1)) {
			query += UNICODE_STRING_SIMPLE(",");
		}
		
	}
	if (doMultiLine) {
		query += UNICODE_STRING_SIMPLE("\n");
	}
	query += UNICODE_STRING_SIMPLE(" ) VALUES (");
	
	for (size_t i = 0; i < CheckedValues.size(); ++i) {
		if (doMultiLine && (i % 5) == 0) {
			query += UNICODE_STRING_SIMPLE("\n");
		}
		UnicodeString val = CheckedValues[i];
		if (val.compare(UNICODE_STRING_SIMPLE("<NULL>")) == 0) {
			val = UNICODE_STRING_SIMPLE("NULL");
		}
		else {
			val = UNICODE_STRING_SIMPLE("'") + val + UNICODE_STRING_SIMPLE("'");
		}
		
		query += val;
		if (i < (CheckedValues.size() - 1)) {
			query += UNICODE_STRING_SIMPLE(", ");
		}
		
	}
	query += UNICODE_STRING_SIMPLE(");");
	return query;
}


t4p::RowToPhpClass::RowToPhpClass()
: Columns()
, Values()
, CheckedColumns()
, CheckedValues()
, CopyValues(VALUES_ROW)
, ArraySyntax(SYNTAX_KEYWORD) {
}

t4p::RowToPhpClass::RowToPhpClass(const t4p::RowToPhpClass& src)
: Columns()
, Values()
, CheckedColumns()
, CheckedValues()
, CopyValues(VALUES_ROW)
, ArraySyntax(SYNTAX_KEYWORD) {
	Copy(src);
}

t4p::RowToPhpClass& t4p::RowToPhpClass::operator=(const t4p::RowToPhpClass& src) {
	Copy(src);
	return *this;
}

void t4p::RowToPhpClass::Copy(const t4p::RowToPhpClass& src) {
	Columns = src.Columns;
	Values = src.Values;
	CheckedColumns = src.CheckedColumns;
	CheckedValues = src.CheckedValues;
	CopyValues = src.CopyValues;
	ArraySyntax = src.ArraySyntax;
}

UnicodeString t4p::RowToPhpClass::CreatePhpArray() {
	UnicodeString code;
	UnicodeString endCode;
	if (SYNTAX_KEYWORD == ArraySyntax) {
		code += UNICODE_STRING_SIMPLE("array (\n");
		endCode =  UNICODE_STRING_SIMPLE(")\n");
	}
	else {
		code += UNICODE_STRING_SIMPLE("[\n");
		endCode =  UNICODE_STRING_SIMPLE("]\n");
	}
	for (size_t i = 0; i < CheckedColumns.size(); ++i) {
		code += UNICODE_STRING_SIMPLE("  ");
		code += UNICODE_STRING_SIMPLE("'");
		code += CheckedColumns[i];
		code += UNICODE_STRING_SIMPLE("'");
		code += UNICODE_STRING_SIMPLE(" => ");
		
		UnicodeString val;
		if (VALUES_ROW == CopyValues && i < CheckedValues.size()) {
			val = CheckedValues[i];
			if (val.compare(UNICODE_STRING_SIMPLE("<NULL>")) == 0) {
				code += UNICODE_STRING_SIMPLE("NULL");
			}
			else {
				code += UNICODE_STRING_SIMPLE("'");
				code += val;
				code += UNICODE_STRING_SIMPLE("'");
			}
			
		}
		else {
			code += UNICODE_STRING_SIMPLE("''");
		}
			
		if (i < (CheckedColumns.size() - 1)) {
			code += UNICODE_STRING_SIMPLE(",");
		}
		code += UNICODE_STRING_SIMPLE("\n");
	}
	code += endCode;
	code += UNICODE_STRING_SIMPLE("\n");
	return code;
}

t4p::SqlCopyAsPhpDialogClass::SqlCopyAsPhpDialogClass(wxWindow* parent, int id, t4p::RowToPhpClass& rowToPhp)
: SqlCopyAsPhpDialogGeneratedClass(parent, id)
, EditedRowToPhp(rowToPhp)
, RowToPhp(rowToPhp) 
, HasCheckedAll(false) {
	
	if (t4p::RowToPhpClass::SYNTAX_KEYWORD == rowToPhp.ArraySyntax) {
		ArraySyntaxRadio->SetSelection(0);
	}
	else {
		ArraySyntaxRadio->SetSelection(1);
	}
	if (t4p::RowToPhpClass::VALUES_ROW == rowToPhp.CopyValues) {
		CopyValues->SetSelection(0);
	}
	else {
		CopyValues->SetSelection(1);
	}
	for(size_t i = 0; i < rowToPhp.Columns.size(); ++i) {
		Columns->AppendString(t4p::IcuToWx(rowToPhp.Columns[i]));
		Columns->Check(i);
	}
}

void t4p::SqlCopyAsPhpDialogClass::OnCheckAll(wxCommandEvent& event) {
	HasCheckedAll = !HasCheckedAll;
	for (size_t i = 0; i < Columns->GetCount(); ++i) {
		Columns->Check(i, HasCheckedAll);
	}
}

void t4p::SqlCopyAsPhpDialogClass::OnCancelButton(wxCommandEvent& event) {
	EndModal(wxCANCEL);
}

void t4p::SqlCopyAsPhpDialogClass::OnOkButton(wxCommandEvent& event) {
	if (t4p::RowToPhpClass::SYNTAX_KEYWORD == ArraySyntaxRadio->GetSelection()) {
		EditedRowToPhp.ArraySyntax = t4p::RowToPhpClass::SYNTAX_KEYWORD;
	}
	else {
		EditedRowToPhp.ArraySyntax = t4p::RowToPhpClass::SYNTAX_OPERATOR;
	}
	if (t4p::RowToPhpClass::VALUES_ROW == CopyValues->GetSelection()) {
		EditedRowToPhp.CopyValues = t4p::RowToPhpClass::VALUES_ROW;
	}
	else {
		EditedRowToPhp.CopyValues = t4p::RowToPhpClass::VALUES_EMPTY;
	}
	
	EditedRowToPhp.CheckedColumns.clear();
	EditedRowToPhp.CheckedValues.clear();
	
	for (size_t i = 0; i < Columns->GetCount(); ++i) {
		if (Columns->IsChecked(i)) {
			EditedRowToPhp.CheckedColumns.push_back(EditedRowToPhp.Columns[i]);
			if (i < EditedRowToPhp.Values.size()) {
				EditedRowToPhp.CheckedValues.push_back(EditedRowToPhp.Values[i]);
			}
		}
	}
	
	RowToPhp = EditedRowToPhp;
	
	EndModal(wxOK);
}

BEGIN_EVENT_TABLE(t4p::SqlBrowserFeatureClass, wxEvtHandler)
	EVT_MENU(t4p::MENU_SQL + 0, t4p::SqlBrowserFeatureClass::OnSqlBrowserToolsMenu)	
	EVT_MENU(t4p::MENU_SQL + 1, t4p::SqlBrowserFeatureClass::OnSqlConnectionMenu)
	EVT_MENU(t4p::MENU_SQL + 2, t4p::SqlBrowserFeatureClass::OnRun)
	EVT_MENU(t4p::MENU_SQL + 3, t4p::SqlBrowserFeatureClass::OnSqlDetectMenu)
	EVT_AUINOTEBOOK_PAGE_CHANGED(t4p::ID_CODE_NOTEBOOK, t4p::SqlBrowserFeatureClass::OnContentNotebookPageChanged)
	EVT_AUINOTEBOOK_PAGE_CLOSE(t4p::ID_CODE_NOTEBOOK, t4p::SqlBrowserFeatureClass::OnContentNotebookPageClose)
	EVT_AUINOTEBOOK_PAGE_CLOSE(t4p::ID_TOOLS_NOTEBOOK, t4p::SqlBrowserFeatureClass::OnToolsNotebookPageClose)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_EXIT, t4p::SqlBrowserFeatureClass::OnAppExit)
	EVT_APP_DB_TABLE_DATA_OPEN(t4p::SqlBrowserFeatureClass::OnCmdTableDataOpen)
	EVT_APP_DB_TABLE_DEFINITION_OPEN(t4p::SqlBrowserFeatureClass::OnCmdTableDefinitionOpen)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::SqlBrowserPanelClass, SqlBrowserPanelGeneratedClass)
	EVT_QUERY_COMPLETE(wxID_ANY, t4p::SqlBrowserPanelClass::OnQueryComplete)
	EVT_ACTION_PROGRESS(wxID_ANY, t4p::SqlBrowserPanelClass::OnActionProgress)
	EVT_ACTION_COMPLETE(wxID_ANY, t4p::SqlBrowserPanelClass::OnActionComplete)
	EVT_MENU(ID_GRID_COPY_ALL, t4p::SqlBrowserPanelClass::OnCopyAllRows)
	EVT_MENU(ID_GRID_COPY_ROW, t4p::SqlBrowserPanelClass::OnCopyRow)
	EVT_MENU(ID_GRID_COPY_ROW_SQL, t4p::SqlBrowserPanelClass::OnCopyRowAsSql)
	EVT_MENU(ID_GRID_COPY_ROW_PHP, t4p::SqlBrowserPanelClass::OnCopyRowAsPhp)
	EVT_MENU(ID_GRID_COPY_CELL, t4p::SqlBrowserPanelClass::OnCopyCellData)
	EVT_MENU(ID_GRID_OPEN_IN_EDITOR, t4p::SqlBrowserPanelClass::OnOpenInEditor)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::SqlConnectionListDialogClass, SqlConnectionListDialogGeneratedClass)
	EVT_QUERY_COMPLETE(ID_SQL_LIST_TEST, t4p::SqlConnectionListDialogClass::ShowTestResults)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::MysqlConnectionDialogClass, MysqlConnectionDialogGeneratedClass)
	EVT_QUERY_COMPLETE(ID_SQL_EDIT_TEST, t4p::MysqlConnectionDialogClass::ShowTestResults)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(t4p::TableDefinitionPanelClass, TableDefinitionPanelGeneratedClass)
	EVT_QUERY_COMPLETE(ID_SQL_TABLE_DEFINITION, t4p::TableDefinitionPanelClass::OnColumnSqlComplete)
	EVT_QUERY_COMPLETE(ID_SQL_TABLE_INDICES, t4p::TableDefinitionPanelClass::OnIndexSqlComplete)
	EVT_QUERY_COMPLETE(ID_SQL_TABLE_CREATE, t4p::TableDefinitionPanelClass::OnCreateSqlComplete)
END_EVENT_TABLE()


