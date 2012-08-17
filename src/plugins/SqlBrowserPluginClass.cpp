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
#include <code_control/CodeControlClass.h>
#include <MvcEditorString.h>
#include <widgets/UnicodeStringValidatorClass.h>
#include <MvcEditorErrors.h>
#include <MvcEditor.h>
#include <wx/artprov.h>

static const int ID_SQL_GAUGE = wxNewId();
static const int ID_SQL_METADATA_GAUGE = wxNewId();
static const int ID_SQL_METADATA_FETCH = wxNewId();
static const int ID_SQL_TEST = wxNewId();

mvceditor::SqlConnectionDialogClass::SqlConnectionDialogClass(wxWindow* parent, std::vector<mvceditor::DatabaseInfoClass>& infos, 
															  size_t& chosenIndex,
															  mvceditor::RunningThreadsClass& runningThreads)
	: SqlConnectionDialogGeneratedClass(parent, wxID_ANY) 
	, Infos(infos)
	, EditedInfos(infos)
	, TestQuery()
	, RunningThreads(runningThreads)
	, ConnectionIdentifier()
	, RunningThreadId()
	, ChosenIndex(chosenIndex) {
	RunningThreads.AddEventHandler(this);
	Label->SetValue(wxT(""));
	Host->SetValue(wxT(""));
	User->SetValue(wxT(""));
	Password->SetValue(wxT(""));
	Database->SetValue(wxT(""));
	Port->SetValue(0);
	for (size_t i = 0; i < EditedInfos.size(); i++) {
		wxString label = mvceditor::IcuToWx(EditedInfos[i].Label);
		if (EditedInfos[i].IsDetected) {
			label += _(" <Detected>");
		}
		List->Append(label);
	}
	UpdateTextInputs();
	TransferDataToWindow();
}

mvceditor::SqlConnectionDialogClass::~SqlConnectionDialogClass() {
	RunningThreads.RemoveEventHandler(this);
}

void mvceditor::SqlConnectionDialogClass::UpdateTextInputs() {
	bool allowEdit = true;
	if (ChosenIndex < EditedInfos.size()) {
		wxString label = mvceditor::IcuToWx(EditedInfos[ChosenIndex].Label);
		if (EditedInfos[ChosenIndex].IsDetected) {
			label += _(" <Detected>");
		}
		Label->SetValue(label);
		Host->SetValue(mvceditor::IcuToWx(EditedInfos[ChosenIndex].Host));
		User->SetValue(mvceditor::IcuToWx(EditedInfos[ChosenIndex].User));
		Password->SetValue(mvceditor::IcuToWx(EditedInfos[ChosenIndex].Password));
		Database->SetValue(mvceditor::IcuToWx(EditedInfos[ChosenIndex].DatabaseName));
		Port->SetValue(EditedInfos[ChosenIndex].Port);
		List->Select(ChosenIndex);
		allowEdit = !EditedInfos[ChosenIndex].IsDetected;
	}
	Label->Enable(allowEdit);
	Host->Enable(allowEdit);
	User->Enable(allowEdit);
	Password->Enable(allowEdit);
	Database->Enable(allowEdit);
	Port->Enable(allowEdit);
	DeleteButton->Enable(allowEdit);
}
	
void mvceditor::SqlConnectionDialogClass::OnOkButton(wxCommandEvent& event) {
	if (Validate() && TransferDataFromWindow()) {

		// save the old values
		if (ChosenIndex < EditedInfos.size()) {
			EditedInfos[ChosenIndex].Label = mvceditor::WxToIcu(Label->GetValue());
			EditedInfos[ChosenIndex].Host = mvceditor::WxToIcu(Host->GetValue());
			EditedInfos[ChosenIndex].User = mvceditor::WxToIcu(User->GetValue());
			EditedInfos[ChosenIndex].Password = mvceditor::WxToIcu(Password->GetValue());
			EditedInfos[ChosenIndex].DatabaseName = mvceditor::WxToIcu(Database->GetValue());
			EditedInfos[ChosenIndex].Port = Port->GetValue();
		}

		ChosenIndex = List->GetSelection();
		Infos.resize(EditedInfos.size());
		std::copy(EditedInfos.begin(), EditedInfos.end(), Infos.begin());
		EndModal(wxOK);
	}
}

void mvceditor::SqlConnectionDialogClass::OnCancelButton(wxCommandEvent& event) {
	soci::session session;
	UnicodeString error;
	if (TestQuery.Connect(session, error)) {
		TestQuery.KillConnection(session, ConnectionIdentifier, error);
	}
	RunningThreads.Stop(RunningThreadId);
	RunningThreadId = 0;
	event.Skip();
}

void mvceditor::SqlConnectionDialogClass::OnTestButton(wxCommandEvent& event) {
	if (TransferDataFromWindow()) {
		size_t index = List->GetSelection();
		if (index < EditedInfos.size()) {

			// get the most up-to-date values that the user has input
			EditedInfos[index].DatabaseName = mvceditor::WxToIcu(Database->GetValue());
			EditedInfos[index].Host = mvceditor::WxToIcu(Host->GetValue());
			EditedInfos[index].Port = Port->GetValue();
			EditedInfos[index].User = mvceditor::WxToIcu(User->GetValue());
			EditedInfos[index].Password = mvceditor::WxToIcu(Password->GetValue());
			
			TestQuery.Info.Copy(EditedInfos[index]);
			wxThreadError error = wxTHREAD_NO_ERROR;
			
			mvceditor::MultipleSqlExecuteClass* thread = new mvceditor::MultipleSqlExecuteClass(RunningThreads, ID_SQL_TEST, ConnectionIdentifier);
			thread->Init(UNICODE_STRING_SIMPLE("SELECT 1"), TestQuery);
			error = thread->CreateSingleInstance();
			switch (error) {
			case wxTHREAD_NO_ERROR:
				RunningThreadId = thread->GetId();
				wxWindow::FindWindowById(wxID_OK, this)->Disable();
				wxWindow::FindWindowById(ID_TESTBUTTON, this)->Disable();
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

void mvceditor::SqlConnectionDialogClass::OnAddButton(wxCommandEvent& event) {

	// save the old values
	if (ChosenIndex < EditedInfos.size()) {
		EditedInfos[ChosenIndex].Label = mvceditor::WxToIcu(Label->GetValue());
		EditedInfos[ChosenIndex].Host = mvceditor::WxToIcu(Host->GetValue());
		EditedInfos[ChosenIndex].User = mvceditor::WxToIcu(User->GetValue());
		EditedInfos[ChosenIndex].Password = mvceditor::WxToIcu(Password->GetValue());
		EditedInfos[ChosenIndex].DatabaseName = mvceditor::WxToIcu(Database->GetValue());
		EditedInfos[ChosenIndex].Port = Port->GetValue();
	}

	// add the new item to the info list and the list control
	mvceditor::DatabaseInfoClass newInfo;
	newInfo.Label = UNICODE_STRING_SIMPLE("Untitled");
	newInfo.IsDetected = false;
	EditedInfos.push_back(newInfo);
	List->Append(wxT("Untitled"));
	ChosenIndex = List->GetCount() - 1;
	List->SetSelection(List->GetCount() - 1);

	// put the newly "blank" values in the textboxes
	UpdateTextInputs();
	Label->SetFocus();
}

void mvceditor::SqlConnectionDialogClass::OnDeleteButton(wxCommandEvent& event) {
	int selection = List->GetSelection();

	// dont allow the user to delete a detected connection
	if (selection >= 0 && (size_t)selection < EditedInfos.size() && !EditedInfos[selection].IsDetected) {
		List->Delete(selection);
		if ((size_t)selection < EditedInfos.size()) {
			EditedInfos.erase(EditedInfos.begin() + selection);
		}
		if ((size_t)selection < List->GetCount()) {
			List->SetSelection(selection);
			ChosenIndex = selection;
		}
		else {
			List->SetSelection(List->GetCount() - 1);
			ChosenIndex = List->GetCount() - 1;
		}
		UpdateTextInputs();
	}
}

void mvceditor::SqlConnectionDialogClass::OnLabelText(wxCommandEvent& event) {
	
	// update list box with the label that is being typed in
	int selection = List->GetSelection();
	if (selection >= 0 && (size_t)selection < List->GetCount()) {
		List->SetString(selection, Label->GetValue());
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

void mvceditor::SqlConnectionDialogClass::ShowTestResults(wxCommandEvent& event) {
	mvceditor::SqlResultClass* result = (mvceditor::SqlResultClass*)event.GetClientData();
	wxString error = mvceditor::IcuToWx(result->Error);
	bool success = result->Success;
	
	wxString msg = _("Connection to %s@%s was successful");
	msg = wxString::Format(msg, 
		mvceditor::IcuToWx(TestQuery.Info.User).c_str(), 
		mvceditor::IcuToWx(TestQuery.Info.Host).c_str());
	if (!success) {
		msg = _("Connection to %s@%s failed: %s");
		msg = wxString::Format(msg, 
		mvceditor::IcuToWx(TestQuery.Info.User).c_str(), 
		mvceditor::IcuToWx(TestQuery.Info.Host).c_str(), 
		error.c_str());
	}
	wxMessageBox(msg);
	wxWindow::FindWindowById(wxID_OK, this)->Enable();
	wxWindow::FindWindowById(ID_TESTBUTTON, this)->Enable();
	delete result;
}

void mvceditor::SqlConnectionDialogClass::OnListboxSelected(wxCommandEvent& event) {
	
	// save the old values
	if (ChosenIndex < EditedInfos.size() && !EditedInfos[ChosenIndex].IsDetected) {
		EditedInfos[ChosenIndex].Label = mvceditor::WxToIcu(Label->GetValue());
		EditedInfos[ChosenIndex].Host = mvceditor::WxToIcu(Host->GetValue());
		EditedInfos[ChosenIndex].User = mvceditor::WxToIcu(User->GetValue());
		EditedInfos[ChosenIndex].Password = mvceditor::WxToIcu(Password->GetValue());
		EditedInfos[ChosenIndex].DatabaseName = mvceditor::WxToIcu(Database->GetValue());
		EditedInfos[ChosenIndex].Port = Port->GetValue();
	}

	// put the newly chosen values in the textboxes
	ChosenIndex = event.GetInt();
	UpdateTextInputs();
}

mvceditor::MultipleSqlExecuteClass::MultipleSqlExecuteClass(mvceditor::RunningThreadsClass& runningThreads, int queryId,
															mvceditor::ConnectionIdentifierClass& connectionIdentifier)
	: ThreadWithHeartbeatClass(runningThreads, queryId)
	, SqlLexer() 
	, Query()
	, Session()
	, ConnectionIdentifier(connectionIdentifier)
	, QueryId(queryId) {
}

bool mvceditor::MultipleSqlExecuteClass::Execute() {
	bool ret = false;
	
	wxThreadError error = CreateSingleInstance();
	switch (error) {
	case wxTHREAD_NO_ERROR:
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

void mvceditor::MultipleSqlExecuteClass::BackgroundWork() {
	UnicodeString error;
	UnicodeString query;
	bool connected = Query.Connect(Session, error);
	if (connected) {
		while (SqlLexer.NextQuery(query) && !TestDestroy()) {		
			wxLongLong start = wxGetLocalTimeMillis();

			// create a new result on the heap; the event handler must delete it
			mvceditor::SqlResultClass* results = new mvceditor::SqlResultClass;
			results->QueryTime = wxGetLocalTimeMillis() - start;
			results->LineNumber = SqlLexer.GetLineNumber();
			
			Query.ConnectionIdentifier(Session, ConnectionIdentifier);

			// always post the results even if the query has an error.
			Query.Execute(Session, *results, query);
			wxCommandEvent evt(QUERY_COMPLETE_EVENT, QueryId);
			evt.SetClientData(results);
			PostEvent(evt);			
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

bool mvceditor::MultipleSqlExecuteClass::Init(const UnicodeString& sql, const SqlQueryClass& query) {
	Query.Info.Copy(query.Info);
	return SqlLexer.OpenString(sql);
}

void mvceditor::MultipleSqlExecuteClass::Close() {
	Session.close();
	SqlLexer.Close();
}

mvceditor::SqlBrowserPanelClass::SqlBrowserPanelClass(wxWindow* parent, int id, 
		mvceditor::StatusBarWithGaugeClass* gauge, const mvceditor::SqlQueryClass& other,
		mvceditor::SqlBrowserPluginClass* plugin) 
	: SqlBrowserPanelGeneratedClass(parent, id)
	, Query(other)
	, ConnectionIdentifier()
	, LastError()
	, LastQuery()
	, RunningThreadId(0) 
	, Results()
	, Gauge(gauge)
	, Plugin(plugin) {
	CodeControl = NULL;
	QueryId = id;
	ResultsGrid->DeleteCols(0, ResultsGrid->GetNumberCols());
	ResultsGrid->DeleteRows(0, ResultsGrid->GetNumberRows());
	ResultsGrid->ClearGrid();
	UpdateLabels(wxT(""));
	Plugin->App.RunningThreads.AddEventHandler(this);
}

mvceditor::SqlBrowserPanelClass::~SqlBrowserPanelClass() {
	if (RunningThreadId > 0) {
		Stop();
	}
	Plugin->App.RunningThreads.RemoveEventHandler(this);
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
	if (Check() && 0 == RunningThreadId) {
		mvceditor::MultipleSqlExecuteClass* thread = new mvceditor::MultipleSqlExecuteClass(
			Plugin->App.RunningThreads, QueryId, ConnectionIdentifier);
		if (thread->Init(LastQuery, Query) && thread->Execute()) {
			RunningThreadId = thread->GetId();
			Gauge->AddGauge(_("Running SQL queries"), ID_SQL_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE, wxGA_HORIZONTAL);
		}
		else {
			delete thread;
			RunningThreadId = 0;			
		}
	}
	else if (LastQuery.isEmpty()) {
		wxMessageBox(_("Please enter a query into the code control."));
	}
	else {
		wxMessageBox(_("Please wait until the current queries completes."));
	}
}

void mvceditor::SqlBrowserPanelClass::Stop() {
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
			wxMessageBox(_("could not kill:") + mvceditor::IcuToWx(error));
		}
	}
	else {
		wxMessageBox(_("could not connect:") + mvceditor::IcuToWx(error));
	}
	Plugin->App.RunningThreads.Stop(RunningThreadId);
	RunningThreadId = 0;
	Gauge->StopGauge(ID_SQL_GAUGE);
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
			ResultsGrid->SetColLabelValue(i, mvceditor::IcuToWx(results->ColumnNames[i]));
			autoSizeColumns.push_back(true);
		}
		ResultsGrid->SetDefaultCellOverflow(false);
		for (size_t i = 0; i < results->StringResults.size(); i++) {
			ResultsGrid->AppendRows(1);
			std::vector<UnicodeString> columnValues =  results->StringResults[i];
			for (size_t colNumber = 0; colNumber < columnValues.size(); colNumber++) {
				ResultsGrid->SetCellValue(rowNumber - 1, colNumber, mvceditor::IcuToWx(columnValues[colNumber]));
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
		UpdateLabels(mvceditor::IcuToWx(results->Error));
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
				mvceditor::IcuToWx(Query.Info.Host).c_str(),
				Query.Info.Port,
				mvceditor::IcuToWx(Query.Info.User).c_str(),
				mvceditor::IcuToWx(Query.Info.DatabaseName).c_str()
			));
		}
		else {
			ConnectionLabel->SetLabel(wxString::Format(
				wxT("%s:host=%s user=%s dbname=%s"),
				driver.c_str(),
				mvceditor::IcuToWx(Query.Info.Host).c_str(),
				mvceditor::IcuToWx(Query.Info.User).c_str(),
				mvceditor::IcuToWx(Query.Info.DatabaseName).c_str()
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
	else {
		event.Skip();
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
		RunningThreadId = 0;
		Gauge->StopGauge(ID_SQL_GAUGE);
		Plugin->AuiManagerUpdate();
	}
	else {
		event.Skip();
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

const wxEventType mvceditor::EVENT_SQL_META_DATA_COMPLETE = wxNewEventType();

mvceditor::SqlMetaDataEventClass::SqlMetaDataEventClass(int eventId, const mvceditor::SqlResourceFinderClass& newResources,
														const std::vector<UnicodeString>& errors) 
	: wxEvent(eventId, mvceditor::EVENT_SQL_META_DATA_COMPLETE)
	, NewResources(newResources)
	, Errors(errors) {
}

wxEvent* mvceditor::SqlMetaDataEventClass::Clone() const {
	mvceditor::SqlMetaDataEventClass* evt = new 
		mvceditor::SqlMetaDataEventClass(GetId(), NewResources, Errors);
	return evt;
}

mvceditor::SqlMetaDataFetchClass::SqlMetaDataFetchClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: ThreadWithHeartbeatClass(runningThreads, eventId)
	, Infos() {
		
}

bool mvceditor::SqlMetaDataFetchClass::Read(std::vector<mvceditor::DatabaseInfoClass> infos) {
	bool ret = false;

	// make sure to set these BEFORE calling CreateSingleInstance
	// in order to prevent Entry from reading them while we write to them
	Infos = infos;
	wxThreadError err = CreateSingleInstance();
	if (wxTHREAD_NO_RESOURCE == err) {
		mvceditor::EditorLogError(mvceditor::LOW_RESOURCES);
	}
	else if (wxTHREAD_RUNNING == err) {
		wxMessageBox(_("There is already another SQL MetaData fetch that is active. Please wait for it to finish."), _("SQL MetaData fetch"));
	}
	else if (wxTHREAD_NO_ERROR == err) {
		ret = true;
	}
	return ret;
}

void mvceditor::SqlMetaDataFetchClass::BackgroundWork() {
	std::vector<UnicodeString> errors;
	mvceditor::SqlResourceFinderClass newResources;
	for (std::vector<mvceditor::DatabaseInfoClass>::iterator it = Infos.begin(); it != Infos.end(); ++it) {
		if (!TestDestroy()) {
			UnicodeString error;
			if (!newResources.Fetch(*it, error)) {
				errors.push_back(error);
			}
		}
		else {
			break;
		}
	}
	if (!TestDestroy()) {

		// PostEvent() will set the correct event Id
		mvceditor::SqlMetaDataEventClass evt(wxID_ANY, newResources, errors);
		PostEvent(evt);
	}
}

mvceditor::SqlBrowserPluginClass::SqlBrowserPluginClass(mvceditor::AppClass& app) 
	: PluginClass(app)
	, ChosenIndex(0) {

	// will get removed when the app terminates
	App.RunningThreads.AddEventHandler(this);
}

mvceditor::SqlBrowserPluginClass::~SqlBrowserPluginClass() {
}

void mvceditor::SqlBrowserPluginClass::OnProjectsUpdated(wxCommandEvent& event) {


	// remove any connections previously detected
	std::vector<mvceditor::DatabaseInfoClass>::iterator it = App.Structs.Infos.begin();
	while (it != App.Structs.Infos.end()) {
		if (it->IsDetected) {
			it = App.Structs.Infos.erase(it);
		}
		else {
			++it;
		}
	}
	
	// add the detected connections to the infos list
	// this makes it easier; that way we always work with one list only

	// TODO: make a AllEnabledDatabaseConnections method in StructsClass. that will clean
	// some stuff up
	for (size_t i = 0; i < App.Structs.Frameworks.size(); ++i) {
		App.Structs.Infos.insert(App.Structs.Infos.end(), App.Structs.Frameworks[i].Databases.begin(), App.Structs.Frameworks[i].Databases.end());
	}
	DetectMetadata();
}

void mvceditor::SqlBrowserPluginClass::DetectMetadata() {
	ChosenIndex = 0;
	if (!App.Structs.Infos.empty()) {
		mvceditor::SqlMetaDataFetchClass* thread = new mvceditor::SqlMetaDataFetchClass(App.RunningThreads, ID_SQL_METADATA_FETCH);
		if (thread->Read(App.Structs.Infos)) {
			GetStatusBarWithGauge()->AddGauge(_("Fetching SQL meta data"), ID_SQL_METADATA_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE, 0);
		}
		else {
			delete thread;
		}
	}
}

void mvceditor::SqlBrowserPluginClass::AddNewMenu(wxMenuBar* menuBar) {
	wxMenu* sqlMenu = new wxMenu(0);
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

void mvceditor::SqlBrowserPluginClass::AddToolBarItems(wxAuiToolBar* toolBar) {
	toolBar->AddTool(mvceditor::MENU_SQL + 0, _("SQL Browser"), wxArtProvider::GetBitmap(
		wxART_EXECUTABLE_FILE, wxART_TOOLBAR, wxSize(16, 16)), wxT("Open the SQL Browser"), wxITEM_NORMAL);
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
	if (ChosenIndex < App.Structs.Infos.size()) {
		query.Info.Copy(App.Structs.Infos[ChosenIndex]);
		codeControl->SetCurrentInfo(App.Structs.Infos[ChosenIndex]);
	}
	
	mvceditor::SqlBrowserPanelClass* sqlPanel = new SqlBrowserPanelClass(GetToolsNotebook(), wxNewId(), GetStatusBarWithGauge(), 
		query, this);
	mvceditor::NotebookClass* codeNotebook = GetNotebook();
	wxString tabText = codeNotebook->GetPageText(codeNotebook->GetPageIndex(codeControl));

	// name the windows, since there could be multiple windows from various plugins; we want to know which opened tools windows
	// are from this plugin
	AddToolsWindow(sqlPanel, tabText, wxT("mvceditor::SqlBrowserPanelClass"));
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

			// only cast when we are sure of the type of window
			// not using wxDynamicCast since SqlBrowserPanelClass does not implement the required 
			// methods
			if (window->GetName() == wxT("mvceditor::SqlBrowserPanelClass")) {
				mvceditor::SqlBrowserPanelClass* panel = (mvceditor::SqlBrowserPanelClass*)window;
				if (panel->IsLinkedToCodeControl(ctrl)) {
				
					// we found the panel bring it to the forefront and run the query
					found = true;
					SetFocusToToolsWindow(window);
					panel->Execute();
					break;
				}
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
	mvceditor::SqlConnectionDialogClass dialog(GetMainWindow(), App.Structs.Infos, ChosenIndex, App.RunningThreads);
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
				if (ChosenIndex < App.Structs.Infos.size()) {
					panel->SetCurrentInfo(App.Structs.Infos[ChosenIndex]);
				}
				panel->UpdateLabels(wxT(""));
			}
		}
		SavePreferences();

		// redetect the SQL meta data
		mvceditor::SqlMetaDataFetchClass* thread = new mvceditor::SqlMetaDataFetchClass(App.RunningThreads, ID_SQL_METADATA_FETCH);
		if (!thread->Read(App.Structs.Infos)) {
			delete thread;
		}
	}
}

void mvceditor::SqlBrowserPluginClass::OnSqlDetectMenu(wxCommandEvent& event) {
	DetectMetadata();
}

void mvceditor::SqlBrowserPluginClass::OnContentNotebookPageChanged(wxAuiNotebookEvent& event) {
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

void mvceditor::SqlBrowserPluginClass::OnContentNotebookPageClose(wxAuiNotebookEvent& event) {
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

void mvceditor::SqlBrowserPluginClass::OnToolsNotebookPageClose(wxAuiNotebookEvent& event) {
	wxAuiNotebook* notebook = GetToolsNotebook();
	int sel = event.GetSelection();
	wxWindow* toolsWindow = notebook->GetPage(sel);

	// only cast when we are sure of the type of window
	// not using wxDynamicCast since SqlBrowserPanelClass does not implement the required 
	// methods
	if (toolsWindow->GetName() == wxT("mvceditor::SqlBrowserPanelClass")) {
		mvceditor::SqlBrowserPanelClass* panel = (mvceditor::SqlBrowserPanelClass*)toolsWindow;
		panel->Stop();
	}
}

void mvceditor::SqlBrowserPluginClass::OnAppExit(wxCommandEvent& event) {
	wxAuiNotebook* notebook = GetToolsNotebook();
	for (size_t i = 0; i < notebook->GetPageCount(); ++i) {
		wxWindow* toolsWindow = notebook->GetPage(i);

		// only cast when we are sure of the type of window
		// not using wxDynamicCast since SqlBrowserPanelClass does not implement the required 
		// methods
		if (toolsWindow->GetName() == wxT("mvceditor::SqlBrowserPanelClass")) {
			mvceditor::SqlBrowserPanelClass* panel = (mvceditor::SqlBrowserPanelClass*)toolsWindow;
			panel->Stop();
		}
	}
}

void mvceditor::SqlBrowserPluginClass::OnWorkInProgress(wxCommandEvent& event) {
	GetStatusBarWithGauge()->IncrementGauge(ID_SQL_METADATA_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE);
}

void mvceditor::SqlBrowserPluginClass::OnWorkComplete(wxCommandEvent& event) {
	GetStatusBarWithGauge()->StopGauge(ID_SQL_METADATA_GAUGE);
}

void mvceditor::SqlBrowserPluginClass::OnSqlMetaDataComplete(mvceditor::SqlMetaDataEventClass& event) {
	App.Structs.SqlResourceFinder.Copy(event.NewResources);
	std::vector<UnicodeString> errors = event.Errors;
	for (size_t i = 0; i < errors.size(); ++i) {
		wxString wxError = mvceditor::IcuToWx(errors[i]);
		mvceditor::EditorLogError(mvceditor::BAD_SQL, wxError);
	}
}

void mvceditor::SqlBrowserPluginClass::AddKeyboardShortcuts(std::vector<mvceditor::DynamicCmdClass>& shortcuts) {
	std::map<int, wxString> menuItemIds;
	menuItemIds[mvceditor::MENU_SQL + 0] = wxT("SQL-Browser");
	menuItemIds[mvceditor::MENU_SQL + 1] = wxT("SQL-Connections");
	menuItemIds[mvceditor::MENU_SQL + 2] = wxT("SQL-Run Queries");
	menuItemIds[mvceditor::MENU_SQL + 3] = wxT("SQL-Detect SQL Meta Data");
	AddDynamicCmd(menuItemIds, shortcuts);
}

void mvceditor::SqlBrowserPluginClass::AuiManagerUpdate() {
	AuiManager->Update();
}

void mvceditor::SqlBrowserPluginClass::LoadPreferences(wxConfigBase* config) {
	wxString groupName;
	long index = 0;
	if (config->GetFirstGroup(groupName, index)) {
		do {
			if (groupName.Find(wxT("DatabaseInfo_")) == 0) {
				mvceditor::DatabaseInfoClass info;
				info.DatabaseName = mvceditor::WxToIcu(config->Read(groupName + wxT("/DatabaseName")));
				wxString driverString = config->Read(groupName + wxT("/Driver"));
				info.FileName = mvceditor::WxToIcu(config->Read(groupName + wxT("/FileName")));
				info.Host = mvceditor::WxToIcu(config->Read(groupName + wxT("/Host")));
				info.IsDetected = false;
				info.Label = mvceditor::WxToIcu(config->Read(groupName + wxT("/Label")));
				info.Password = mvceditor::WxToIcu(config->Read(groupName + wxT("/Password")));
				config->Read(groupName + wxT("/Port"), &info.Port);
				info.User = mvceditor::WxToIcu(config->Read(groupName + wxT("/User")));
				
				if (driverString.CmpNoCase(wxT("MYSQL")) == 0) {
					info.Driver = mvceditor::DatabaseInfoClass::MYSQL;
					App.Structs.Infos.push_back(info);
				}
			}
		} while (config->GetNextGroup(groupName, index));
	}
}

void mvceditor::SqlBrowserPluginClass::SavePreferences() {
	wxConfigBase* config = wxConfig::Get();
	wxString groupName;
	long index = 0;

	// delete any previous connections that are in the config
	if (config->GetFirstGroup(groupName, index)) {
		do {
			if (groupName.Find(wxT("DatabaseInfo_")) == 0) {
				config->DeleteGroup(groupName);
			}
		} while (config->GetNextGroup(groupName, index));
	}

	// now save all of the new ones
	int saveIndex = 0;
	for (size_t i = 0; i < App.Structs.Infos.size(); ++i) {
		if (!App.Structs.Infos[i].IsDetected) {
			config->SetPath(wxString::Format(wxT("/DatabaseInfo_%d"), saveIndex));
			config->Write(wxT("DatabaseName"), mvceditor::IcuToWx(App.Structs.Infos[i].DatabaseName));
			wxString driverString;
			if (mvceditor::DatabaseInfoClass::MYSQL == App.Structs.Infos[i].Driver) {
				driverString = wxT("MYSQL");
			}
			config->Write(wxT("Driver"), driverString);
			config->Write(wxT("FileName"), mvceditor::IcuToWx(App.Structs.Infos[i].FileName));
			config->Write(wxT("Host"), mvceditor::IcuToWx(App.Structs.Infos[i].Host));
			config->Write(wxT("Label"), mvceditor::IcuToWx(App.Structs.Infos[i].Label));
			config->Write(wxT("Password"), mvceditor::IcuToWx(App.Structs.Infos[i].Password));
			config->Write(wxT("Port"), App.Structs.Infos[i].Port);
			config->Write(wxT("User"), mvceditor::IcuToWx(App.Structs.Infos[i].User));
			saveIndex++;
		}
	}
	config->SetPath(wxT("/"));
	config->Flush();
}

BEGIN_EVENT_TABLE(mvceditor::SqlBrowserPluginClass, wxEvtHandler)
	EVT_MENU(mvceditor::MENU_SQL + 0, mvceditor::SqlBrowserPluginClass::OnSqlBrowserToolsMenu)	
	EVT_MENU(mvceditor::MENU_SQL + 1, mvceditor::SqlBrowserPluginClass::OnSqlConnectionMenu)
	EVT_MENU(mvceditor::MENU_SQL + 2, mvceditor::SqlBrowserPluginClass::OnRun)
	EVT_MENU(mvceditor::MENU_SQL + 3, mvceditor::SqlBrowserPluginClass::OnSqlDetectMenu)
	EVT_COMMAND(ID_SQL_METADATA_FETCH, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SqlBrowserPluginClass::OnWorkInProgress)
	EVT_COMMAND(ID_SQL_METADATA_FETCH, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SqlBrowserPluginClass::OnWorkComplete)
	EVT_AUINOTEBOOK_PAGE_CHANGED(mvceditor::ID_CODE_NOTEBOOK, mvceditor::SqlBrowserPluginClass::OnContentNotebookPageChanged)
	EVT_AUINOTEBOOK_PAGE_CLOSE(mvceditor::ID_CODE_NOTEBOOK, mvceditor::SqlBrowserPluginClass::OnContentNotebookPageClose)
	EVT_AUINOTEBOOK_PAGE_CLOSE(mvceditor::ID_TOOLS_NOTEBOOK, mvceditor::SqlBrowserPluginClass::OnToolsNotebookPageClose)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_PROJECTS_UPDATED, mvceditor::SqlBrowserPluginClass::OnProjectsUpdated)
	EVT_SQL_META_DATA_COMPLETE(ID_SQL_METADATA_FETCH, mvceditor::SqlBrowserPluginClass::OnSqlMetaDataComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_APP_EXIT, mvceditor::SqlBrowserPluginClass::OnAppExit)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::SqlBrowserPanelClass, SqlBrowserPanelGeneratedClass)
	EVT_COMMAND(wxID_ANY, QUERY_COMPLETE_EVENT, mvceditor::SqlBrowserPanelClass::OnQueryComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SqlBrowserPanelClass::OnWorkInProgress)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SqlBrowserPanelClass::OnWorkComplete)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::SqlConnectionDialogClass, SqlConnectionDialogGeneratedClass)
	EVT_COMMAND(ID_SQL_TEST, QUERY_COMPLETE_EVENT, mvceditor::SqlConnectionDialogClass::ShowTestResults)
END_EVENT_TABLE()