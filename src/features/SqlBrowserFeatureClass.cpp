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
#include <MvcEditor.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <sqlite3.h>
#include <wx/artprov.h>

static const int ID_SQL_GAUGE = wxNewId();
static const int ID_SQL_EDIT_TEST = wxNewId();
static const int ID_SQL_LIST_TEST = wxNewId();
static const int ID_SQL_TABLE_DEFINITION = wxNewId();
static const int ID_SQL_TABLE_INDICES = wxNewId();
static const int ID_SQL_TABLE_CREATE = wxNewId();
static const int ID_PANEL_TABLE_DEFINITION = wxNewId();

/**
 * @param grid the grid to put the results in. any previous grid values are cleared. this function will not own the pointer
 * @param results the results to fill. this function will not own the pointer
 */
static void FillGridWithResults(wxGrid* grid, mvceditor::SqlResultClass* results) {
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
		grid->SetCellValue(0, 0, mvceditor::IcuToWx(results->Error));
		autoSizeColumns.push_back(true);
	}
	if (results->HasRows) {
		grid->AppendCols(results->ColumnNames.size());
		for (size_t i = 0; i < results->ColumnNames.size(); i++) {
			grid->SetColLabelValue(i, mvceditor::IcuToWx(results->ColumnNames[i]));
			autoSizeColumns.push_back(true);
		}
		grid->SetDefaultCellOverflow(false);
		for (size_t i = 0; i < results->StringResults.size(); i++) {
			grid->AppendRows(1);
			std::vector<UnicodeString> columnValues =  results->StringResults[i];
			for (size_t colNumber = 0; colNumber < columnValues.size(); colNumber++) {
				grid->SetCellValue(rowNumber - 1, colNumber, mvceditor::IcuToWx(columnValues[colNumber]));
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

mvceditor::SqliteConnectionDialogClass::SqliteConnectionDialogClass(wxWindow* parent, mvceditor::DatabaseTagClass& tag)
: SqliteConnectionDialogGeneratedClass(parent, wxID_ANY)
{
	mvceditor::UnicodeStringValidatorClass labelValidator(&tag.Label, false);
	Label->SetValidator(labelValidator);
	Label->SetName(wxT("label"));
	mvceditor::FilePickerValidatorClass fileValidator(&tag.FileName);
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

mvceditor::MysqlConnectionDialogClass::MysqlConnectionDialogClass(wxWindow* parent, mvceditor::DatabaseTagClass& tag,
		mvceditor::RunningThreadsClass& runningThreads)
: MysqlConnectionDialogGeneratedClass(parent, wxID_ANY)
, TestQuery()
, RunningThreads(runningThreads)
, ConnectionIdentifier()
, RunningActionId() {
	RunningThreads.AddEventHandler(this);
	
	mvceditor::UnicodeStringValidatorClass labelValidator(&tag.Label, false);
	Label->SetValidator(labelValidator);
	Label->SetName(wxT("label"));
	mvceditor::UnicodeStringValidatorClass hostValidator(&tag.Host, false);
	Host->SetValidator(hostValidator);
	Host->SetName(wxT("host"));
	wxGenericValidator portValidator(&tag.Port);
	Port->SetValidator(portValidator);
	mvceditor::UnicodeStringValidatorClass schemaValidator(&tag.Schema, true);
	Database->SetValidator(schemaValidator);
	mvceditor::UnicodeStringValidatorClass userValidator(&tag.User, true);
	User->SetValidator(userValidator);
	mvceditor::UnicodeStringValidatorClass passwordValidator(&tag.Password, true);
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

mvceditor::MysqlConnectionDialogClass::~MysqlConnectionDialogClass() {
	RunningThreads.RemoveEventHandler(this);
}

void mvceditor::MysqlConnectionDialogClass::OnTestButton(wxCommandEvent& event) {

	// get the most up-to-date values that the user has input

	TestQuery.DatabaseTag.Driver = mvceditor::DatabaseTagClass::MYSQL;
	TestQuery.DatabaseTag.Label = mvceditor::WxToIcu(Label->GetValue());
	TestQuery.DatabaseTag.Host = mvceditor::WxToIcu(Host->GetValue());
	TestQuery.DatabaseTag.Port = Port->GetValue();
	TestQuery.DatabaseTag.Schema = mvceditor::WxToIcu(Database->GetValue());
	TestQuery.DatabaseTag.User = mvceditor::WxToIcu(User->GetValue());
	TestQuery.DatabaseTag.Password = mvceditor::WxToIcu(Password->GetValue());
	
	wxWindow::FindWindowById(wxID_OK, this)->Enable(false);
	wxWindow::FindWindowById(ID_TESTBUTTON, this)->Enable(false);
	
	mvceditor::MultipleSqlExecuteClass* thread = new mvceditor::MultipleSqlExecuteClass(RunningThreads, ID_SQL_EDIT_TEST, ConnectionIdentifier);
	thread->Init(UNICODE_STRING_SIMPLE("SELECT 1"), TestQuery);
	RunningThreads.Queue(thread);
}

void mvceditor::MysqlConnectionDialogClass::OnCancelButton(wxCommandEvent& event) {
	
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

void mvceditor::MysqlConnectionDialogClass::ShowTestResults(wxCommandEvent& event) {
	mvceditor::SqlResultClass* result = (mvceditor::SqlResultClass*)event.GetClientData();
	wxString error = mvceditor::IcuToWx(result->Error);
	bool success = result->Success;
	
	wxString creds = mvceditor::IcuToWx(TestQuery.DatabaseTag.User + 
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


mvceditor::SqlConnectionListDialogClass::SqlConnectionListDialogClass(wxWindow* parent, std::vector<mvceditor::DatabaseTagClass>& dbTags, 
															  mvceditor::RunningThreadsClass& runningThreads)
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

mvceditor::SqlConnectionListDialogClass::~SqlConnectionListDialogClass() {
	RunningThreads.RemoveEventHandler(this);
}

void mvceditor::SqlConnectionListDialogClass::OnAddMysqlButton(wxCommandEvent& event) {
	mvceditor::DatabaseTagClass newTag;
	newTag.Driver = mvceditor::DatabaseTagClass::MYSQL;
	mvceditor::MysqlConnectionDialogClass mysqlDialog(this, newTag, RunningThreads);
	if (mysqlDialog.ShowModal() == wxID_OK) {
		Push(newTag);
	}
}

void mvceditor::SqlConnectionListDialogClass::OnAddSqliteButton(wxCommandEvent& event) {
	mvceditor::DatabaseTagClass newTag;
	newTag.Driver = mvceditor::DatabaseTagClass::SQLITE;
	mvceditor::SqliteConnectionDialogClass sqliteDialog(this, newTag);
	if (sqliteDialog.ShowModal() == wxID_OK) {
		Push(newTag);
	}
}

void mvceditor::SqlConnectionListDialogClass::OnCloneButton(wxCommandEvent& event) {
	wxArrayInt toCloneIndexes;
	if (List->GetSelections(toCloneIndexes)) {
		for (size_t i = 0; i < toCloneIndexes.GetCount(); ++i) {
			size_t indexToClone = toCloneIndexes[i];
			
			mvceditor::DatabaseTagClass clonedTag;
			clonedTag.Copy(EditedDatabaseTags[indexToClone]);
			
			// clone == the user created it
			clonedTag.IsDetected = false;
			
			// add a postfix to the clone label
			clonedTag.Label = clonedTag.Label + UNICODE_STRING_SIMPLE(" clone");
			Push(clonedTag);
		}
	}
}

void mvceditor::SqlConnectionListDialogClass::OnRemoveSelectedButton(wxCommandEvent& event) {
	wxArrayInt toRemoveIndexes;
	if (List->GetSelections(toRemoveIndexes)) {
		std::vector<mvceditor::DatabaseTagClass> remaining;
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

void mvceditor::SqlConnectionListDialogClass::OnRemoveAllButton(wxCommandEvent& event) {
	
	// remove from the backing list
	// but don't allow the user to delete a detected connection
	std::vector<mvceditor::DatabaseTagClass> newList;
	std::vector<mvceditor::DatabaseTagClass>::iterator tag;
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

void mvceditor::SqlConnectionListDialogClass::OnCheckToggled(wxCommandEvent& event) {
	size_t sel = event.GetInt();
	bool b = List->IsChecked(sel);
	if (sel >= 0 && sel < EditedDatabaseTags.size()) {
		EditedDatabaseTags[sel].IsEnabled = b;
	}
}

void mvceditor::SqlConnectionListDialogClass::OnListDoubleClick(wxCommandEvent& event) {
	size_t sel = (size_t)event.GetSelection();
	if (sel >= 0 && sel < EditedDatabaseTags.size()) {
		mvceditor::DatabaseTagClass editTag = EditedDatabaseTags[sel];
		int res = 0;
		if (mvceditor::DatabaseTagClass::MYSQL == editTag.Driver) {
			mvceditor::MysqlConnectionDialogClass dialog(this, editTag, RunningThreads);
			res = dialog.ShowModal();
		}
		else if (mvceditor::DatabaseTagClass::SQLITE == editTag.Driver) {
			mvceditor::SqliteConnectionDialogClass dialog(this, editTag);
			res = dialog.ShowModal();
		}
		if (wxID_OK == res) {
			EditedDatabaseTags[sel] = editTag;
			
			// update the list label
			List->SetString(sel, mvceditor::IcuToWx(editTag.Label));
		}
	}
	
}

void mvceditor::SqlConnectionListDialogClass::Push(const mvceditor::DatabaseTagClass& newTag) {
	wxString label = mvceditor::IcuToWx(newTag.Label);
	if (newTag.IsDetected) {
		label += _(" <Detected>");
	}
	EditedDatabaseTags.push_back(newTag);
		
	List->Append(label);
	List->Check(List->GetCount() - 1, newTag.IsEnabled);
}
	
void mvceditor::SqlConnectionListDialogClass::OnOkButton(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow()) {
		DatabaseTags.clear();
		DatabaseTags.resize(EditedDatabaseTags.size());
		std::copy(EditedDatabaseTags.begin(), EditedDatabaseTags.end(), DatabaseTags.begin());
		EndModal(wxOK);
	}
}

void mvceditor::SqlConnectionListDialogClass::OnCancelButton(wxCommandEvent& event) {
	soci::session session;
	UnicodeString error;
	if (TestQuery.Connect(session, error)) {
		TestQuery.KillConnection(session, ConnectionIdentifier, error);
	}
	RunningThreads.CancelAction(RunningActionId);
	RunningActionId = 0;
	event.Skip();
}

void mvceditor::SqlConnectionListDialogClass::OnTestSelectedButton(wxCommandEvent& event) {
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

		mvceditor::MultipleSqlExecuteClass* thread = new mvceditor::MultipleSqlExecuteClass(RunningThreads, ID_SQL_LIST_TEST, ConnectionIdentifier);
		thread->Init(UNICODE_STRING_SIMPLE("SELECT 1"), TestQuery);
		RunningThreads.Queue(thread);
	}
}

void mvceditor::SqlConnectionListDialogClass::ShowTestResults(wxCommandEvent& event) {
	mvceditor::SqlResultClass* result = (mvceditor::SqlResultClass*)event.GetClientData();
	wxString error = mvceditor::IcuToWx(result->Error);
	bool success = result->Success;
	
	wxString creds;
	if (TestQuery.DatabaseTag.FileName.IsOk()) {
		creds = TestQuery.DatabaseTag.FileName.GetFullPath();
	}
	else {
		creds = mvceditor::IcuToWx(TestQuery.DatabaseTag.User + 
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

mvceditor::MultipleSqlExecuteClass::MultipleSqlExecuteClass(mvceditor::RunningThreadsClass& runningThreads, int queryId,
															mvceditor::ConnectionIdentifierClass& connectionIdentifier)
	: ActionClass(runningThreads, queryId)
	, SqlLexer() 
	, Query()
	, Session()
	, ConnectionIdentifier(connectionIdentifier)
	, QueryId(queryId) {
}

wxString mvceditor::MultipleSqlExecuteClass::GetLabel() const {
	return wxT("SQL Execute");
}

void mvceditor::MultipleSqlExecuteClass::BackgroundWork() {
	UnicodeString error;
	UnicodeString query;
	bool connected = Query.Connect(Session, error);
	if (connected) {
		while (SqlLexer.NextQuery(query) && !IsCancelled()) {		
			wxLongLong start = wxGetLocalTimeMillis();

			// create a new result on the heap; the event handler must delete it
			mvceditor::SqlResultClass* results = new mvceditor::SqlResultClass;
			results->QueryTime = wxGetLocalTimeMillis() - start;
			results->LineNumber = SqlLexer.GetLineNumber();
			
			Query.ConnectionIdentifier(Session, ConnectionIdentifier);
			Query.Execute(Session, *results, query);

			// post the results even if the query has an error.
			// but dont post if the query was cancelled
			// careful: leak will happen when panel is closed since event won't be handled
			if (!IsCancelled()) {
				wxCommandEvent evt(QUERY_COMPLETE_EVENT, QueryId);
				evt.SetClientData(results);
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
		mvceditor::SqlResultClass* results = new mvceditor::SqlResultClass;
		results->LineNumber = SqlLexer.GetLineNumber();
		results->Success = false;
		results->HasRows = false;
		results->Error = error;
		wxCommandEvent evt(QUERY_COMPLETE_EVENT, QueryId);
		evt.SetClientData(results);
		PostEvent(evt);
	}
}

void mvceditor::MultipleSqlExecuteClass::DoCancel() {
	if (mvceditor::DatabaseTagClass::SQLITE == Query.DatabaseTag.Driver) {
		soci::sqlite3_session_backend* backend = static_cast<soci::sqlite3_session_backend*>(Session.get_backend());
		sqlite_api::sqlite3_interrupt(backend->conn_);
	}
	if (mvceditor::DatabaseTagClass::MYSQL == Query.DatabaseTag.Driver) {
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
				wxMessageBox(_("could not kill connection:") + mvceditor::IcuToWx(error));
			}
		}
		else {
			wxMessageBox(_("could not connect:") + mvceditor::IcuToWx(error));
		}
	}
}

bool mvceditor::MultipleSqlExecuteClass::Init(const UnicodeString& sql, const SqlQueryClass& query) {
	Query.DatabaseTag.Copy(query.DatabaseTag);
	return SqlLexer.OpenString(sql);
}

void mvceditor::MultipleSqlExecuteClass::Close() {
	Session.close();
	SqlLexer.Close();
}

mvceditor::SqlBrowserPanelClass::SqlBrowserPanelClass(wxWindow* parent, int id, 
		mvceditor::StatusBarWithGaugeClass* gauge, const mvceditor::SqlQueryClass& other,
		mvceditor::SqlBrowserFeatureClass* feature) 
	: SqlBrowserPanelGeneratedClass(parent, id)
	, Query(other)
	, ConnectionIdentifier()
	, LastError()
	, LastQuery()
	, RunningActionId(0) 
	, Results()
	, Gauge(gauge)
	, Feature(feature) {
	CodeControl = NULL;
	QueryId = id;
	ResultsGrid->DeleteCols(0, ResultsGrid->GetNumberCols());
	ResultsGrid->DeleteRows(0, ResultsGrid->GetNumberRows());
	ResultsGrid->ClearGrid();
	UpdateLabels(wxT(""));

	RefreshButton->SetBitmap(mvceditor::IconImageAsset(wxT("outline-refresh")));
	Feature->App.RunningThreads.AddEventHandler(this);
	FillConnectionList();
}

mvceditor::SqlBrowserPanelClass::~SqlBrowserPanelClass() {
	
}

bool mvceditor::SqlBrowserPanelClass::Check() {
	bool ret = CodeControl && Validate() && TransferDataFromWindow();
	if (ret) {
		LastQuery = CodeControl->GetSafeText();
		ret = !LastQuery.isEmpty();
	}
	if (ret) {
		
		// make sure a connection has been chosen
		size_t sel = (size_t)Connections->GetSelection();
		std::vector<mvceditor::DatabaseTagClass> dbTags = Feature->App.Globals.AllEnabledDatabaseTags();
		if (sel >= 0 && sel < dbTags.size()) {
			Query.DatabaseTag.Copy(dbTags[sel]);
		}
		else {
			ret = false;
		}
	}
	return ret;
}

void mvceditor::SqlBrowserPanelClass::ExecuteCodeControl() {
	if (Check() && 0 == RunningActionId) {
		mvceditor::MultipleSqlExecuteClass* thread = new mvceditor::MultipleSqlExecuteClass(
			Feature->App.RunningThreads, QueryId, ConnectionIdentifier);
		if (thread->Init(LastQuery, Query)) { 
			RunningActionId = Feature->App.RunningThreads.Queue(thread);
			Gauge->AddGauge(_("Running SQL queries"), ID_SQL_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE, wxGA_HORIZONTAL);
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


void mvceditor::SqlBrowserPanelClass::ExecuteQuery(const wxString& sql, const mvceditor::DatabaseTagClass& tag) {
	LastQuery = mvceditor::WxToIcu(sql);
	Query.DatabaseTag = tag;
	for (unsigned int i = 0; i < Connections->GetCount(); ++i) {
		if (Connections->GetString(i) == mvceditor::IcuToWx(tag.Label)) {
			Connections->SetSelection(i);
			break;
		}
	}
	RunningActionId = 0;
	mvceditor::MultipleSqlExecuteClass* thread = new mvceditor::MultipleSqlExecuteClass(
		Feature->App.RunningThreads, QueryId, ConnectionIdentifier);
	if (thread->Init(LastQuery, Query)) { 
		RunningActionId = Feature->App.RunningThreads.Queue(thread);
		Gauge->AddGauge(_("Running SQL queries"), ID_SQL_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE, wxGA_HORIZONTAL);
	}
	else {
		delete thread;
		RunningActionId = 0;
	}
}

void mvceditor::SqlBrowserPanelClass::OnRefreshButton(wxCommandEvent& event) {
	ExecuteQuery(mvceditor::IcuToWx(LastQuery), Query.DatabaseTag);
}

void mvceditor::SqlBrowserPanelClass::Stop() {
	if (RunningActionId > 0) {
		Feature->App.RunningThreads.CancelAction(RunningActionId);
		RunningActionId = 0;
		Gauge->StopGauge(ID_SQL_GAUGE);
	}
}

void mvceditor::SqlBrowserPanelClass::OnQueryComplete(wxCommandEvent& event) {
	if (event.GetId() == QueryId) {
		mvceditor::SqlResultClass* result = (mvceditor::SqlResultClass*)event.GetClientData();
		Results.push_back(result);
	}
	else {
		event.Skip();
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
			mvceditor::SqlBrowserPanelClass* newPanel = Feature->CreateResultsPanel(CodeControl);
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
				msg = mvceditor::IcuToWx(results->Error);
			}
			int rowNumber = ResultsGrid->GetNumberRows();
			wxString queryStart = mvceditor::IcuToWx(results->Query);
			ResultsGrid->AppendRows(1);
			ResultsGrid->SetCellValue(wxGridCellCoords(rowNumber , 0), wxString::Format(wxT("%d"), results->LineNumber));
			ResultsGrid->SetCellValue(wxGridCellCoords(rowNumber , 1), queryStart.Mid(0, 100)); 
			ResultsGrid->SetCellValue(wxGridCellCoords(rowNumber , 2), msg);
		}
		if (!outputSummary && !results->Error.isEmpty()) {

			// put error in the summary LABEL
			UpdateLabels(mvceditor::IcuToWx(results->Error));
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

void mvceditor::SqlBrowserPanelClass::Fill(mvceditor::SqlResultClass* results) {
	FillGridWithResults(ResultsGrid, results);

	if (!results->Success) {
		UpdateLabels(mvceditor::IcuToWx(results->Error));
	}
	else {
		UpdateLabels(wxString::Format(_("%d rows returned in %.3f sec"), 
			results->AffectedRows, (results->QueryTime.ToLong() / 1000.00)));
	}
}

void mvceditor::SqlBrowserPanelClass::UpdateLabels(const wxString& result) {
	ResultsLabel->SetLabel(result);
	ResultsLabel->GetContainingSizer()->Layout();
}

void mvceditor::SqlBrowserPanelClass::OnActionProgress(mvceditor::ActionProgressEventClass& event) {
	if (event.GetId() == QueryId) {
		Gauge->IncrementGauge(ID_SQL_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE);
	}
	else {
		event.Skip();
	}
}

void mvceditor::SqlBrowserPanelClass::OnActionComplete(mvceditor::ActionEventClass& event) {
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

void mvceditor::SqlBrowserPanelClass::SetCurrentInfo(const mvceditor::DatabaseTagClass& databaseTag) {
	Query.DatabaseTag.Copy(databaseTag);
	if (CodeControl) {
		CodeControl->SetCurrentDbTag(databaseTag);
	}
}

void mvceditor::SqlBrowserPanelClass::LinkToCodeControl(mvceditor::CodeControlClass* codeControl) {
	CodeControl = codeControl;
	
	// set the current selected db tag, so that code completion works
	size_t sel = (size_t)Connections->GetSelection();
	std::vector<mvceditor::DatabaseTagClass> dbTags = Feature->App.Globals.AllEnabledDatabaseTags();
	if (sel >= 0 && sel < dbTags.size()) {
		CodeControl->SetCurrentDbTag(dbTags[sel]);
	}
}

bool mvceditor::SqlBrowserPanelClass::IsLinkedToCodeControl(mvceditor::CodeControlClass* codeControl) {
	return CodeControl != NULL && codeControl == CodeControl;
}

void mvceditor::SqlBrowserPanelClass::UnlinkFromCodeControl() {
	CodeControl = NULL;
}

void mvceditor::SqlBrowserPanelClass::FillConnectionList() {
	std::vector<mvceditor::DatabaseTagClass>::const_iterator tag;
	Connections->Clear();
	std::vector<mvceditor::DatabaseTagClass> dbTags = Feature->App.Globals.AllEnabledDatabaseTags();
	for (tag = dbTags.begin(); tag != dbTags.end(); ++tag) {
		Connections->Append(mvceditor::IcuToWx(tag->Label));
	}
	this->Layout();
	if (!Connections->IsEmpty()) {
		Connections->SetSelection(0);
		SetCurrentInfo(dbTags[0]);
	}
}

void mvceditor::SqlBrowserPanelClass::OnConnectionChoice(wxCommandEvent& event) {
	size_t sel = (size_t)event.GetSelection();
	std::vector<mvceditor::DatabaseTagClass> dbTags = Feature->App.Globals.AllEnabledDatabaseTags();
	if (sel >= 0 && sel < dbTags.size()) {
		SetCurrentInfo(dbTags[sel]);
	}
}

mvceditor::SqlBrowserFeatureClass::SqlBrowserFeatureClass(mvceditor::AppClass& app) 
	: FeatureClass(app) {
}

mvceditor::SqlBrowserFeatureClass::~SqlBrowserFeatureClass() {
}

void mvceditor::SqlBrowserFeatureClass::DetectMetadata() {

	// thread will be owned by SequenceClass
	mvceditor::SqlMetaDataActionClass* thread = new mvceditor::SqlMetaDataActionClass(App.SqliteRunningThreads, mvceditor::ID_EVENT_ACTION_SQL_METADATA);
	std::vector<mvceditor::GlobalActionClass*> actions;
	actions.push_back(thread);
	App.Sequences.Build(actions);
}

void mvceditor::SqlBrowserFeatureClass::AddNewMenu(wxMenuBar* menuBar) {
	wxMenu* sqlMenu = new wxMenu();
	sqlMenu->Append(mvceditor::MENU_SQL + 0, _("SQL Browser\tSHIFT+F9"), _("Open a window for SQL browsing"),
		wxITEM_NORMAL);
	sqlMenu->Append(mvceditor::MENU_SQL + 1, _("SQL Connections\tCTRL+F9"), _("Show & Pick The SQL Connections that this project uses"),
		wxITEM_NORMAL);
	sqlMenu->Append(mvceditor::MENU_SQL + 2, _("Run Queries in SQL Browser\tF9"), _("Execute the query that is currently in the SQL Browser"),
		wxITEM_NORMAL);
	sqlMenu->Append(mvceditor::MENU_SQL + 3, _("Detect SQL Meta Data"), _("Detect SQL Meta data so that it is made available to code completion"),
		wxITEM_NORMAL);
	menuBar->Append(sqlMenu, _("SQL"));
}

void mvceditor::SqlBrowserFeatureClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	wxBitmap bmp = mvceditor::IconImageAsset(wxT("sql"));
	toolBar->AddTool(mvceditor::MENU_SQL + 0, _("SQL Browser"), bmp, wxT("Open the SQL Browser"), wxITEM_NORMAL);
}

void  mvceditor::SqlBrowserFeatureClass::OnSqlBrowserToolsMenu(wxCommandEvent& event) {
	int num = 1;
	mvceditor::NotebookClass* notebook = GetNotebook();
	for (size_t i = 0; i < notebook->GetPageCount(); i++) {
		wxString name = notebook->GetPageText(i);
		if (name.EndsWith(wxT(".sql")) || name.Index(_("SQL Browser")) == 0) {
			num++;
		}
	}
	
	mvceditor::CodeControlClass* ctrl = CreateCodeControl(wxString::Format(_("SQL Browser %d"), num), mvceditor::CodeControlClass::SQL);
	CreateResultsPanel(ctrl);
	ctrl->SetFocus();
}

mvceditor::SqlBrowserPanelClass* mvceditor::SqlBrowserFeatureClass::CreateResultsPanel(mvceditor::CodeControlClass* codeControl) {
	mvceditor::SqlQueryClass query;
	mvceditor::SqlBrowserPanelClass* sqlPanel = new SqlBrowserPanelClass(GetToolsNotebook(), wxNewId(), GetStatusBarWithGauge(), 
		query, this);
	mvceditor::NotebookClass* codeNotebook = GetNotebook();
	wxString tabText = codeNotebook->GetPageText(codeNotebook->GetPageIndex(codeControl));

	// name the windows, since there could be multiple windows from various features; we want to know which opened tools windows
	// are from this feature
	wxBitmap tableBitmap = mvceditor::IconImageAsset(wxT("table"));
	AddToolsWindow(sqlPanel, tabText, wxT("mvceditor::SqlBrowserPanelClass"), tableBitmap);
	sqlPanel->LinkToCodeControl(codeControl);
	return sqlPanel;
}

void mvceditor::SqlBrowserFeatureClass::OnRun(wxCommandEvent& event) {
	mvceditor::CodeControlClass* ctrl = GetNotebook()->GetCurrentCodeControl();
	if (ctrl && ctrl->GetDocumentMode() == mvceditor::CodeControlClass::SQL) {
		
		// look for results panel that corresponds to the current code control
		wxAuiNotebook* notebook = GetToolsNotebook();
		bool found = false;
		for (size_t i = 0; i < notebook->GetPageCount(); i++) {
			wxWindow* window = notebook->GetPage(i);

			// only cast when we are sure of the type of window
			// not using wxDynamicCast since SqlBrowserPanelClass does not implement the required 
			// methods
			if (window->GetName() == wxT("mvceditor::SqlBrowserPanelClass")) {
				mvceditor::SqlBrowserPanelClass* panel = (mvceditor::SqlBrowserPanelClass*)window;
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
			mvceditor::SqlBrowserPanelClass* panel = CreateResultsPanel(ctrl);
			panel->ExecuteCodeControl();
		}
	}
}

void mvceditor::SqlBrowserFeatureClass::OnSqlConnectionMenu(wxCommandEvent& event) {

	// decided to always allow the user to edit the connection info in order to
	// allow the user to create a new database from within the editor (the very 
	// first time a new project is created; its database may not exist).
	// before, a user would not be able to edit the connection info once it was detected
	// in order to make it less confusing about where the connection info comes from.
	mvceditor::SqlConnectionListDialogClass dialog(GetMainWindow(), App.Globals.DatabaseTags, App.RunningThreads);
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
			if (window->GetName() == wxT("mvceditor::SqlBrowserPanelClass")) {
				mvceditor::SqlBrowserPanelClass* panel = (mvceditor::SqlBrowserPanelClass*)window;
				panel->FillConnectionList();
			}
		}
		SavePreferences();

		// redetect the SQL meta data
		mvceditor::SqlMetaDataActionClass* thread = new mvceditor::SqlMetaDataActionClass(App.RunningThreads, mvceditor::ID_EVENT_ACTION_SQL_METADATA);
		if (thread->Init(App.Globals)) {
			App.RunningThreads.Queue(thread);
		}
		else {
			delete thread;
		}
	}
}

void mvceditor::SqlBrowserFeatureClass::OnSqlDetectMenu(wxCommandEvent& event) {
	DetectMetadata();
}

void mvceditor::SqlBrowserFeatureClass::OnContentNotebookPageChanged(wxAuiNotebookEvent& event) {
	mvceditor::CodeControlClass* contentWindow = GetNotebook()->GetCodeControl(event.GetSelection());
	if (contentWindow) {
		wxAuiNotebook* notebook = GetToolsNotebook();
		for (size_t i = 0; i < notebook->GetPageCount(); i++) {
			wxWindow* toolsWindow = notebook->GetPage(i);

			// only cast when we are sure of the type of window
			// not using wxDynamicCast since SqlBrowserPanelClass does not implement the required 
			// methods
			if (toolsWindow->GetName() == wxT("mvceditor::SqlBrowserPanelClass")) {
				mvceditor::SqlBrowserPanelClass* panel = (mvceditor::SqlBrowserPanelClass*)toolsWindow;
				if (panel->IsLinkedToCodeControl(contentWindow)) {
					
					// we found the panel bring it to the forefront and run the query
					SetFocusToToolsWindow(toolsWindow);
					break;
				}
			}
		}
	}
}

void mvceditor::SqlBrowserFeatureClass::OnContentNotebookPageClose(wxAuiNotebookEvent& event) {
	mvceditor::CodeControlClass* contentWindow = GetNotebook()->GetCodeControl(event.GetSelection());
	if (contentWindow) {
		wxAuiNotebook* notebook = GetToolsNotebook();
		for (size_t i = 0; i < notebook->GetPageCount(); i++) {
			wxWindow* toolsWindow = notebook->GetPage(i);

			// only cast when we are sure of the type of window
			// not using wxDynamicCast since SqlBrowserPanelClass does not implement the required 
			// methods
			if (toolsWindow->GetName() == wxT("mvceditor::SqlBrowserPanelClass")) {
				mvceditor::SqlBrowserPanelClass* panel = (mvceditor::SqlBrowserPanelClass*)toolsWindow;
				if (panel->IsLinkedToCodeControl(contentWindow)) {
					panel->UnlinkFromCodeControl();
				}
			}
		}
	}
}

void mvceditor::SqlBrowserFeatureClass::OnToolsNotebookPageClose(wxAuiNotebookEvent& event) {
	wxAuiNotebook* notebook = GetToolsNotebook();
	int sel = event.GetSelection();
	wxWindow* toolsWindow = notebook->GetPage(sel);

	// only cast when we are sure of the type of window
	// not using wxDynamicCast since SqlBrowserPanelClass does not implement the required 
	// methods
	if (toolsWindow->GetName() == wxT("mvceditor::SqlBrowserPanelClass")) {
		mvceditor::SqlBrowserPanelClass* panel = (mvceditor::SqlBrowserPanelClass*)toolsWindow;

		// the constructor added itself as an event handler
		App.RunningThreads.RemoveEventHandler(panel);
		panel->Stop();
	}
}

void mvceditor::SqlBrowserFeatureClass::OnAppExit(wxCommandEvent& event) {
	wxAuiNotebook* notebook = GetToolsNotebook();
	for (size_t i = 0; i < notebook->GetPageCount(); ++i) {
		wxWindow* toolsWindow = notebook->GetPage(i);

		// only cast when we are sure of the type of window
		// not using wxDynamicCast since SqlBrowserPanelClass does not implement the required 
		// methods
		if (toolsWindow->GetName() == wxT("mvceditor::SqlBrowserPanelClass")) {
			mvceditor::SqlBrowserPanelClass* panel = (mvceditor::SqlBrowserPanelClass*)toolsWindow;

			// the constructor added itself as an event handler
			App.RunningThreads.RemoveEventHandler(panel);
			panel->Stop();
		}
	}
}

void mvceditor::SqlBrowserFeatureClass::AddKeyboardShortcuts(std::vector<mvceditor::DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_SQL + 0] = wxT("SQL-Browser");
	menuItemIds[mvceditor::MENU_SQL + 1] = wxT("SQL-Connections");
	menuItemIds[mvceditor::MENU_SQL + 2] = wxT("SQL-Run Queries");
	menuItemIds[mvceditor::MENU_SQL + 3] = wxT("SQL-Detect SQL Meta Data");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::SqlBrowserFeatureClass::AuiManagerUpdate() {
	AuiManager->Update();
}

void mvceditor::SqlBrowserFeatureClass::LoadPreferences(wxConfigBase* config) {
	App.Globals.DatabaseTags.clear();
	wxString groupName;
	long index = 0;
	if (config->GetFirstGroup(groupName, index)) {
		do {
			if (groupName.Find(wxT("DatabaseInfo_")) == 0) {
				mvceditor::DatabaseTagClass info;
				info.Schema = mvceditor::WxToIcu(config->Read(groupName + wxT("/DatabaseName")));
				wxString driverString = config->Read(groupName + wxT("/Driver"));
				info.FileName.Assign(config->Read(groupName + wxT("/FileName")));
				info.Host = mvceditor::WxToIcu(config->Read(groupName + wxT("/Host")));
				info.IsDetected = false;
				info.Label = mvceditor::WxToIcu(config->Read(groupName + wxT("/Label")));
				info.Password = mvceditor::WxToIcu(config->Read(groupName + wxT("/Password")));
				config->Read(groupName + wxT("/Port"), &info.Port);
				info.User = mvceditor::WxToIcu(config->Read(groupName + wxT("/User")));
				config->Read(groupName + wxT("/IsEnabled"), &info.IsEnabled);
				
				if (driverString.CmpNoCase(wxT("MYSQL")) == 0) {
					info.Driver = mvceditor::DatabaseTagClass::MYSQL;
					App.Globals.DatabaseTags.push_back(info);
				}
				else if (driverString.CmpNoCase(wxT("SQLITE")) == 0) {
					info.Driver = mvceditor::DatabaseTagClass::SQLITE;
					App.Globals.DatabaseTags.push_back(info);
				}
			}
		} while (config->GetNextGroup(groupName, index));
	}
}

void mvceditor::SqlBrowserFeatureClass::SavePreferences() {
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
			config->Write(wxT("DatabaseName"), mvceditor::IcuToWx(App.Globals.DatabaseTags[i].Schema));
			wxString driverString;
			if (mvceditor::DatabaseTagClass::MYSQL == App.Globals.DatabaseTags[i].Driver) {
				driverString = wxT("MYSQL");
			}
			else if (mvceditor::DatabaseTagClass::SQLITE == App.Globals.DatabaseTags[i].Driver) {
				driverString = wxT("SQLITE");
			}
			config->Write(wxT("Driver"), driverString);
			config->Write(wxT("FileName"), App.Globals.DatabaseTags[i].FileName.GetFullPath());
			config->Write(wxT("Host"), mvceditor::IcuToWx(App.Globals.DatabaseTags[i].Host));
			config->Write(wxT("Label"), mvceditor::IcuToWx(App.Globals.DatabaseTags[i].Label));
			config->Write(wxT("Password"), mvceditor::IcuToWx(App.Globals.DatabaseTags[i].Password));
			config->Write(wxT("Port"), App.Globals.DatabaseTags[i].Port);
			config->Write(wxT("User"), mvceditor::IcuToWx(App.Globals.DatabaseTags[i].User));
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

void mvceditor::SqlBrowserFeatureClass::OnCmdTableDataOpen(mvceditor::OpenDbTableCommandEventClass& event) {
	
	// find the connection to use by hash, 
	mvceditor::DatabaseTagClass tag;
	bool found = App.Globals.FindDatabaseTagByHash(event.ConnectionHash, tag);
	if (found) {
		mvceditor::SqlQueryClass query;
		mvceditor::SqlBrowserPanelClass* sqlPanel = new SqlBrowserPanelClass(GetToolsNotebook(), wxNewId(), GetStatusBarWithGauge(), 
			query, this);
		
		wxString tabText = event.DbTableName;
		wxString sql = "SELECT * FROM " + event.DbTableName + " LIMIT 100";

		// name the windows, since there could be multiple windows from various features; we want to know which opened tools windows
		// are from this feature
		wxBitmap tableBitmap = mvceditor::IconImageAsset(wxT("table"));
		AddToolsWindow(sqlPanel, tabText, wxT("mvceditor::SqlBrowserPanelClass"), tableBitmap);
		sqlPanel->ExecuteQuery(sql, tag);
	}
}

void mvceditor::SqlBrowserFeatureClass::NewSqlBuffer(const wxString& sql) {
	GetNotebook()->AddMvcEditorPage(mvceditor::CodeControlClass::SQL);
	mvceditor::CodeControlClass* ctrl = GetCurrentCodeControl();
	if (ctrl) {
		ctrl->SetText(sql);
	}
}

void mvceditor::SqlBrowserFeatureClass::OnCmdTableDefinitionOpen(mvceditor::OpenDbTableCommandEventClass& event) {
	
	// find the connection to use by hash, 
	mvceditor::DatabaseTagClass tag;
	bool found = App.Globals.FindDatabaseTagByHash(event.ConnectionHash, tag);
	if (found) {
		
		// if there is an existing table definition panel use that
		wxWindow* win = wxWindow::FindWindowById(ID_PANEL_TABLE_DEFINITION, GetToolsNotebook());
		mvceditor::TableDefinitionPanelClass* sqlPanel = NULL;
		if (win) {
			sqlPanel = (mvceditor::TableDefinitionPanelClass*)win;
			SetFocusToToolsWindow(win);
		}
		else {
			sqlPanel = new TableDefinitionPanelClass(GetMainWindow(), ID_PANEL_TABLE_DEFINITION, *this);
			wxString tabText = wxT("Table Definition");
		
			// name the windows, since there could be multiple windows from various features; we want to know which opened tools windows
			// are from this feature
			wxBitmap tableBitmap = mvceditor::IconImageAsset(wxT("database-medium"));
			AddToolsWindow(sqlPanel, tabText, wxT("mvceditor::TableDefinitionPanelClass"), tableBitmap);
		}
		sqlPanel->ShowTable(tag, event.DbTableName);
	}
}

mvceditor::TableDefinitionPanelClass::TableDefinitionPanelClass(wxWindow* parent, int id, mvceditor::SqlBrowserFeatureClass& feature)
: TableDefinitionPanelGeneratedClass(parent, id)
, Feature(feature) 
, TableConnectionIdentifier() 
, IndexConnectionIdentifier() {
	ColumnsGrid->ClearGrid();
	IndicesGrid->ClearGrid();
	Connections->Clear();
	FillConnectionList();
	Feature.App.RunningThreads.AddEventHandler(this);
	RefreshButton->SetBitmap(mvceditor::IconImageAsset("outline-refresh"));
	this->Layout();
}

mvceditor::TableDefinitionPanelClass::~TableDefinitionPanelClass() {
	Feature.App.RunningThreads.RemoveEventHandler(this);
}

void mvceditor::TableDefinitionPanelClass::FillConnectionList() {
	Connections->Clear();
	std::vector<mvceditor::DatabaseTagClass> dbTags = Feature.App.Globals.AllEnabledDatabaseTags();
	for (size_t i = 0; i < dbTags.size(); ++i) {
		Connections->Append(mvceditor::IcuToWx(dbTags[i].Label));
	}
}


void mvceditor::TableDefinitionPanelClass::ShowTable(const mvceditor::DatabaseTagClass& tag, const wxString& tableName) {
	TableName->SetValue(tableName);
	
	// select the connection to the one to show
	std::vector<mvceditor::DatabaseTagClass> dbTags = Feature.App.Globals.AllEnabledDatabaseTags();
	int indexToSelect = 0;
	for (size_t i = 0; i < dbTags.size(); ++i) {
		if (dbTags[i].ConnectionHash() == tag.ConnectionHash()) {
			indexToSelect = i;
			break;
		}
	}
	if (indexToSelect) {
		Connections->Select(indexToSelect);
	}
	
	// trigger the description query
	mvceditor::SqlQueryClass query;
	query.DatabaseTag = tag;
	UnicodeString columnSql;
	if (mvceditor::DatabaseTagClass::MYSQL == tag.Driver) {
		columnSql = mvceditor::WxToIcu("DESC " + tableName);
	}
	else if (mvceditor::DatabaseTagClass::SQLITE == tag.Driver) {
		columnSql = mvceditor::WxToIcu("PRAGMA table_info('" + tableName + "')");
	}
	mvceditor::MultipleSqlExecuteClass* sqlDefExecute = 
		new mvceditor::MultipleSqlExecuteClass(Feature.App.RunningThreads, ID_SQL_TABLE_DEFINITION, TableConnectionIdentifier);
	if (sqlDefExecute->Init(columnSql, query)) {
		Feature.App.RunningThreads.Queue(sqlDefExecute);
	}
	
	UnicodeString indexSql;
	if (mvceditor::DatabaseTagClass::MYSQL == tag.Driver) {
		indexSql = mvceditor::WxToIcu("SHOW INDEX FROM " + tableName);
	}
	else if (mvceditor::DatabaseTagClass::SQLITE == tag.Driver) {
		indexSql = mvceditor::WxToIcu("PRAGMA index_list('" + tableName + "')");
	}
	mvceditor::MultipleSqlExecuteClass* sqlIndexExecute = 
		new mvceditor::MultipleSqlExecuteClass(Feature.App.RunningThreads, ID_SQL_TABLE_INDICES, IndexConnectionIdentifier);
	if (sqlIndexExecute->Init(indexSql, query)) {
		Feature.App.RunningThreads.Queue(sqlIndexExecute);
	}
}

void mvceditor::TableDefinitionPanelClass::OnColumnSqlComplete(wxCommandEvent& event) {
	ColumnsGrid->ClearGrid();
	mvceditor::SqlResultClass* result = (mvceditor::SqlResultClass*)event.GetClientData();
	if (!result) {
		return;
	}
	FillGridWithResults(ColumnsGrid, result);
	delete result;
}

void mvceditor::TableDefinitionPanelClass::OnIndexSqlComplete(wxCommandEvent& event) {
	IndicesGrid->ClearGrid();
	mvceditor::SqlResultClass* result = (mvceditor::SqlResultClass*)event.GetClientData();
	if (!result) {
		return;
	}
	FillGridWithResults(IndicesGrid, result);
	delete result;
}


void mvceditor::TableDefinitionPanelClass::OnTableNameEnter(wxCommandEvent& event) {
	
	// select the connection to the one to show
	std::vector<mvceditor::DatabaseTagClass> dbTags = Feature.App.Globals.AllEnabledDatabaseTags();
	size_t selectedIndex = Connections->GetSelection();
	if (selectedIndex >= 0 && selectedIndex < dbTags.size()) {
		mvceditor::DatabaseTagClass selectedTag = dbTags[selectedIndex];
		ShowTable(selectedTag, TableName->GetValue());
	}
}

void mvceditor::TableDefinitionPanelClass::OnSqlButton(wxCommandEvent& event) {
	std::vector<mvceditor::DatabaseTagClass> dbTags = Feature.App.Globals.AllEnabledDatabaseTags();
	size_t selectedIndex = Connections->GetSelection();
	if (selectedIndex >= dbTags.size()) {
		return;
	}
	mvceditor::DatabaseTagClass selectedTag = dbTags[selectedIndex];
	
	
	UnicodeString createSql;
	wxString tableName = TableName->GetValue();
	if (mvceditor::DatabaseTagClass::MYSQL == selectedTag.Driver) {
		createSql = mvceditor::WxToIcu("SHOW CREATE TABLE " + tableName);
	}
	else if (mvceditor::DatabaseTagClass::SQLITE == selectedTag.Driver) {
		createSql = mvceditor::WxToIcu("SELECT sql FROM sqlite_master WHERE type='table' AND name= '" + tableName + "'");
	}
	mvceditor::SqlQueryClass query;
	query.DatabaseTag = selectedTag;
	mvceditor::MultipleSqlExecuteClass* sqlCreateExecute = 
		new mvceditor::MultipleSqlExecuteClass(Feature.App.RunningThreads, ID_SQL_TABLE_CREATE, IndexConnectionIdentifier);
	if (sqlCreateExecute->Init(createSql, query)) {
		Feature.App.RunningThreads.Queue(sqlCreateExecute);
	}
}

void mvceditor::TableDefinitionPanelClass::OnCreateSqlComplete(wxCommandEvent& event) {
	mvceditor::SqlResultClass* result = (mvceditor::SqlResultClass*)event.GetClientData();
	if (!result) {
		return;
	}
	if (!result->Error.isEmpty()) {
		FillGridWithResults(ColumnsGrid, result);
	}
	else if (!result->StringResults.empty() && result->StringResults[0].size() == 2) {
		
		// mysql results
		UnicodeString table = result->StringResults[0][1];
		wxString sqlText = mvceditor::IcuToWx(table);
		Feature.NewSqlBuffer(sqlText);
	}
	else if (!result->StringResults.empty() && result->StringResults[0].size() == 1) {
		
		// sqlite results
		UnicodeString table = result->StringResults[0][0];
		wxString sqlText = mvceditor::IcuToWx(table);
		Feature.NewSqlBuffer(sqlText);
	}
	
	delete result;
}

void mvceditor::TableDefinitionPanelClass::OnRefreshButton(wxCommandEvent& event) {
	OnTableNameEnter(event);
}


BEGIN_EVENT_TABLE(mvceditor::SqlBrowserFeatureClass, wxEvtHandler)
	EVT_MENU(mvceditor::MENU_SQL + 0, mvceditor::SqlBrowserFeatureClass::OnSqlBrowserToolsMenu)	
	EVT_MENU(mvceditor::MENU_SQL + 1, mvceditor::SqlBrowserFeatureClass::OnSqlConnectionMenu)
	EVT_MENU(mvceditor::MENU_SQL + 2, mvceditor::SqlBrowserFeatureClass::OnRun)
	EVT_MENU(mvceditor::MENU_SQL + 3, mvceditor::SqlBrowserFeatureClass::OnSqlDetectMenu)
	EVT_AUINOTEBOOK_PAGE_CHANGED(mvceditor::ID_CODE_NOTEBOOK, mvceditor::SqlBrowserFeatureClass::OnContentNotebookPageChanged)
	EVT_AUINOTEBOOK_PAGE_CLOSE(mvceditor::ID_CODE_NOTEBOOK, mvceditor::SqlBrowserFeatureClass::OnContentNotebookPageClose)
	EVT_AUINOTEBOOK_PAGE_CLOSE(mvceditor::ID_TOOLS_NOTEBOOK, mvceditor::SqlBrowserFeatureClass::OnToolsNotebookPageClose)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_EXIT, mvceditor::SqlBrowserFeatureClass::OnAppExit)
	EVT_APP_DB_TABLE_DATA_OPEN(mvceditor::SqlBrowserFeatureClass::OnCmdTableDataOpen)
	EVT_APP_DB_TABLE_DEFINITION_OPEN(mvceditor::SqlBrowserFeatureClass::OnCmdTableDefinitionOpen)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::SqlBrowserPanelClass, SqlBrowserPanelGeneratedClass)
	EVT_COMMAND(wxID_ANY, QUERY_COMPLETE_EVENT, mvceditor::SqlBrowserPanelClass::OnQueryComplete)
	EVT_ACTION_PROGRESS(wxID_ANY, mvceditor::SqlBrowserPanelClass::OnActionProgress)
	EVT_ACTION_COMPLETE(wxID_ANY, mvceditor::SqlBrowserPanelClass::OnActionComplete)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::SqlConnectionListDialogClass, SqlConnectionListDialogGeneratedClass)
	EVT_COMMAND(ID_SQL_LIST_TEST, QUERY_COMPLETE_EVENT, mvceditor::SqlConnectionListDialogClass::ShowTestResults)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::MysqlConnectionDialogClass, MysqlConnectionDialogGeneratedClass)
	EVT_COMMAND(ID_SQL_EDIT_TEST, QUERY_COMPLETE_EVENT, mvceditor::MysqlConnectionDialogClass::ShowTestResults)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::TableDefinitionPanelClass, TableDefinitionPanelGeneratedClass)
	EVT_COMMAND(ID_SQL_TABLE_DEFINITION, QUERY_COMPLETE_EVENT, mvceditor::TableDefinitionPanelClass::OnColumnSqlComplete)
	EVT_COMMAND(ID_SQL_TABLE_INDICES, QUERY_COMPLETE_EVENT, mvceditor::TableDefinitionPanelClass::OnIndexSqlComplete)
	EVT_COMMAND(ID_SQL_TABLE_CREATE, QUERY_COMPLETE_EVENT, mvceditor::TableDefinitionPanelClass::OnCreateSqlComplete)
END_EVENT_TABLE()

