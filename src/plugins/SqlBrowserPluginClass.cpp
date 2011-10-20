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
	

mvceditor::SqlBrowserPanelClass::SqlBrowserPanelClass(wxWindow* parent, int id, 
		mvceditor::StatusBarWithGaugeClass* gauge, const mvceditor::SqlQueryClass& other) 
	: SqlBrowserPanelGeneratedClass(parent, id)
	, wxThreadHelper()
	, Query(other)
	, Session()
	, Stmt(NULL)
	, Row()
	, Gauge(gauge)
	, Timer()
	, LastError()
	, LastQuery()
	, IsRunning(false) {
	CodeControl = NULL;
	ResultsGrid->DeleteCols(0, ResultsGrid->GetNumberCols());
	ResultsGrid->DeleteRows(0, ResultsGrid->GetNumberRows());
	ResultsGrid->ClearGrid();
	Timer.SetOwner(this);
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
	bool ret = 	CodeControl && Validate() && TransferDataFromWindow();
	if (ret) {
		LastQuery = CodeControl->GetSafeText();
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
	int affected = Stmt ? Query.GetAffectedRows(*Stmt) : 0;
	if (ResultsGrid->GetNumberCols()) {
		ResultsGrid->DeleteCols(0, ResultsGrid->GetNumberCols());
	}
	if (ResultsGrid->GetNumberRows()) {
		ResultsGrid->DeleteRows(0, ResultsGrid->GetNumberRows());
	}
	if (success && Stmt) {
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
	if (!Query.Info.Host.isEmpty()) {
		ConnectionLabel->SetLabel(wxString::Format(
			wxT("%s@%s:%d"),
			mvceditor::StringHelperClass::IcuToWx(Query.Info.User).c_str(),
			mvceditor::StringHelperClass::IcuToWx(Query.Info.Host).c_str(),
			Query.Info.Port
		));
	}
	else {
		ConnectionLabel->SetLabel(_("No Connection is not configured."));
	}
	ResultsLabel->SetLabel(result);
	ResultsLabel->GetContainingSizer()->Layout();
}

void mvceditor::SqlBrowserPanelClass::OnTimer(wxTimerEvent& event) {
	Gauge->IncrementGauge(ID_SQL_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE);
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
	: ThreadWithHeartbeatClass(handler) {
		
}

bool mvceditor::SqlMetaDataFetchClass::Read(std::vector<mvceditor::DatabaseInfoClass>* infos, mvceditor::ProjectClass* project) {
	bool ret = false;
	Infos = infos;
	wxThreadError err = Create();
	if (wxTHREAD_NO_RESOURCE == err) {
		wxMessageBox(_("System is way too busy. Please try again later."), _("SQL MetaData fetch"));
	}
	else if (wxTHREAD_RUNNING == err) {
		wxMessageBox(_("There is already another SQL MetaData fetch that is active. Please wait for it to finish."), _("SQL MetaData fetch"));
	}
	else if (wxTHREAD_NO_ERROR == err) {
		Infos = infos;
		Project = project;
		GetThread()->Run();
		SignalStart();
		ret = true;
	}
	return ret;
}

void* mvceditor::SqlMetaDataFetchClass::Entry() {
	wxMutexLocker locker(Project->SqlResourceFinderMutex);
	if (locker.IsOk()) {
		mvceditor::SqlResourceFinderClass* finder = Project->GetSqlResourceFinder();
		
		std::vector<UnicodeString> errors;
		for (std::vector<mvceditor::DatabaseInfoClass>::iterator it = Infos->begin(); it != Infos->end(); ++it) {
			UnicodeString error;
			if (!finder->Fetch(*it, error)) {
				errors.push_back(error);
			}
		}
		// TODO do something with the error strings
	}
	SignalEnd();
	return 0;
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
	else {
		if (SqlMetaDataFetch.Read(&Infos, GetProject())) {
			GetStatusBarWithGauge()->AddGauge(_("Fetching SQL meta data"), ID_SQL_METADATA_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE, 0);
		}
	}
}

void mvceditor::SqlBrowserPluginClass::AddToolsMenuItems(wxMenu* toolsMenu) {
	toolsMenu->Append(ID_SQL_EDITOR_MENU, _("SQL Browser\tSHIFT+F9"), _("Open a window for SQL browsing"),
		wxITEM_NORMAL);
	toolsMenu->Append(ID_SQL_CONNECTION_MENU, _("SQL Connections\tCTRL+F9"), _("Show & Pick The SQL Connections that this project uses"),
		wxITEM_NORMAL);
	toolsMenu->Append(ID_SQL_RUN_MENU, _("Run Queries in SQL Browser\tF9"), _("Execute the query that is currently in the SQL Browser"),
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
	
	mvceditor::SqlBrowserPanelClass* sqlPanel = new SqlBrowserPanelClass(GetToolsNotebook(), wxID_NEW, GetStatusBarWithGauge(), query);
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
	mvceditor::SqlConnectionDialogClass dialog(GetMainWindow(), Infos, ChosenIndex, WasEmptyDetectedInfo);
	if (dialog.ShowModal() == wxOK) {
		SqlMetaDataFetch.Read(&Infos, GetProject());
		
		// if connection changed need to update the code control so that it knows to use the new
		// connection for auto completion purposes
		// TODO update the currrent panel only ?
		size_t selection = GetToolsNotebook()->GetSelection();
		if (selection >= 0 && GetToolsNotebook()->GetPageCount() > 0) {
			wxWindow* window = GetToolsNotebook()->GetPage(selection);
			mvceditor::SqlBrowserPanelClass* panel = wxDynamicCast(window, mvceditor::SqlBrowserPanelClass);
			if (panel) {
				if (ChosenIndex < Infos.size()) {
					panel->SetCurrentInfo(Infos[ChosenIndex]);
				}
				panel->UpdateLabels(wxT(""));			
			}
		}
	}
}

void mvceditor::SqlBrowserPluginClass::OnContentNotebookPageChanged(wxAuiNotebookEvent& event) {
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
	event.Skip();
}

void mvceditor::SqlBrowserPluginClass::OnContentNotebookPageClose(wxAuiNotebookEvent& event) {
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
	event.Skip();
}

void mvceditor::SqlBrowserPluginClass::OnWorkInProgress(wxCommandEvent& event) {
	GetStatusBarWithGauge()->IncrementGauge(ID_SQL_METADATA_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE);
}

void mvceditor::SqlBrowserPluginClass::OnWorkComplete(wxCommandEvent& event) {
	GetStatusBarWithGauge()->StopGauge(ID_SQL_METADATA_GAUGE);
}

BEGIN_EVENT_TABLE(mvceditor::SqlBrowserPluginClass, wxEvtHandler)
	EVT_MENU(ID_SQL_EDITOR_MENU, mvceditor::SqlBrowserPluginClass::OnSqlBrowserToolsMenu)
	EVT_MENU(ID_SQL_CONNECTION_MENU, mvceditor::SqlBrowserPluginClass::OnSqlConnectionMenu)
	EVT_MENU(ID_SQL_RUN_MENU, mvceditor::SqlBrowserPluginClass::OnRun)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_IN_PROGRESS, mvceditor::SqlBrowserPluginClass::OnWorkInProgress)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_WORK_COMPLETE, mvceditor::SqlBrowserPluginClass::OnWorkComplete)
	EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, mvceditor::SqlBrowserPluginClass::OnContentNotebookPageChanged)
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, mvceditor::SqlBrowserPluginClass::OnContentNotebookPageClose)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::SqlBrowserPanelClass, SqlBrowserPanelGeneratedClass)
	EVT_COMMAND(wxID_ANY, QUERY_COMPLETE_EVENT, mvceditor::SqlBrowserPanelClass::OnQueryComplete)
	EVT_TIMER(wxID_ANY, mvceditor::SqlBrowserPanelClass::OnTimer)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::SqlConnectionDialogClass, SqlConnectionDialogGeneratedClass)
	EVT_COMMAND(wxID_ANY, QUERY_COMPLETE_EVENT, mvceditor::SqlConnectionDialogClass::ShowTestResults)
END_EVENT_TABLE()