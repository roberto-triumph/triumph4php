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
#include <features/SqlBrowserFeatureClass.h>
#include <globals/String.h>
#include <globals/Number.h>
#include <globals/Errors.h>
#include <globals/Assets.h>
#include <Triumph.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <sqlite3.h>
#include <wx/sstream.h>
#include <algorithm>


t4p::QueryCompleteEventClass::QueryCompleteEventClass(t4p::SqlResultClass* results, int eventId)
: wxEvent(eventId, t4p::QUERY_COMPLETE_EVENT)
, Results(results) {
	
}

wxEvent* t4p::QueryCompleteEventClass::Clone() const {
	return new t4p::QueryCompleteEventClass(Results, GetId());
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
		
		if (i < (size - 1)) {
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

t4p::RowToSqlInsertClass::RowToSqlInsertClass()
: Values()
, Columns()
, TableName()
, CheckedColumns()
, CheckedValues()
, LineMode(MULTI_LINE) {
}

t4p::RowToSqlInsertClass::RowToSqlInsertClass(const t4p::RowToSqlInsertClass& src)
: Values()
, Columns()
, TableName()
, CheckedColumns()
, CheckedValues()
, LineMode(MULTI_LINE) {
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

const wxEventType t4p::QUERY_COMPLETE_EVENT = wxNewEventType();
