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
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <globals/Sqlite.h>
#include <globals/String.h>
#include <wx/ffile.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <sqlite3.h>
#include <algorithm>


std::string mvceditor::SqliteSqlLikeEscape(const std::string& value, char e) {
	std::string escaped;
	for (size_t i = 0; i < value.size(); i++) {
		char c = value[i];
		if ('_' == c || '\'' == c || '%' == c) {
			escaped += e;
		}
		escaped += c;
	}
	return escaped;
}

bool mvceditor::SqliteSqlScript(const wxFileName& sqlScriptFileName, soci::session& session, wxString& error) {
	bool ret = false;

	// make sure that the sql script exists before we delete anything
	wxASSERT_MSG(sqlScriptFileName.FileExists(), wxT("SQL script file must exist"));

	// get all of the tables in the db
	std::vector<std::string> tableNames;
	ret = mvceditor::SqliteTables(session, tableNames, error);
	if (!ret) {
		return ret;
	}
	try {
		// drop all of the tables
		std::vector<std::string>::const_iterator table;
		for (table = tableNames.begin(); table != tableNames.end(); ++table) {
			session.once << "DROP TABLE " << *table;
		}

		// now get the contents of the script to be executed
		wxFFile ffile(sqlScriptFileName.GetFullPath());
		wxString sql;
		ffile.ReadAll(&sql);
		std::string stdSql = mvceditor::WxToChar(sql);

		// get the 'raw' connection because it can handle multiple statements at once
		char *errorMessage = NULL;
		soci::sqlite3_session_backend* backend = static_cast<soci::sqlite3_session_backend*>(session.get_backend());
		sqlite_api::sqlite3_exec(backend->conn_, stdSql.c_str(), NULL, NULL, &errorMessage);
		if (errorMessage) {
			error = mvceditor::CharToWx(errorMessage);
			sqlite_api::sqlite3_free(errorMessage);
		}
		else {
			ret = true;
		}
	} catch (std::exception& e) {
		error = mvceditor::CharToWx(e.what());
		wxASSERT_MSG(false, error);
	}
	return ret;
}

bool mvceditor::SqliteTables(soci::session& session, std::vector<std::string>& tableNames, wxString& error) {
	bool ret = false;
	try {	
		std::string tableName;
		soci::statement stmt = (session.prepare << 
			"SELECT tbl_name from sqlite_master WHERE type='table' AND NOT name = 'sqlite_sequence'", 
			soci::into(tableName)
		);
		if (stmt.execute(true)) {
			do {
				tableNames.push_back(tableName);
			} while (stmt.fetch());
		}
		ret = true;
	} catch (std::exception& e) {
		error = mvceditor::CharToWx(e.what());
		wxUnusedVar(e);
		wxASSERT_MSG(ret, error);
	}
	return ret;
}

int mvceditor::SqliteSchemaVersion(soci::session& session) {
	int versionNumber = 0;
	try {
		session.once << "SELECT version_number FROM schema_version", soci::into(versionNumber);
	} catch (std::exception& e) {
		wxUnusedVar(e);
		versionNumber = -1;	
	}
	return versionNumber;
}

void mvceditor::SqliteSetBusyTimeout(soci::session& session, int timeoutMs) {

	// get the 'raw' sqlite connection
	soci::sqlite3_session_backend* backend = static_cast<soci::sqlite3_session_backend*>(session.get_backend());
	sqlite_api::sqlite3_busy_timeout(backend->conn_, timeoutMs);
}


mvceditor::SqliteFinderClass::SqliteFinderClass() {
	Session = NULL;
}

mvceditor::SqliteFinderClass::~SqliteFinderClass() {
}

void mvceditor::SqliteFinderClass::InitSession(soci::session* session) {
	Session = session;
}

bool mvceditor::SqliteFinderClass::Exec(mvceditor::SqliteResultClass* result) {
	bool ret = result->Prepare(*Session, true);
	return ret;
}

bool mvceditor::SqliteFinderClass::IsInit() const {
	return NULL != Session;
}

mvceditor::SqliteResultClass::SqliteResultClass() 
: IsEmpty(true) {
	Stmt = NULL;
}

mvceditor::SqliteResultClass::~SqliteResultClass() {
	if (Stmt) {
		delete Stmt;
	}
}	

bool mvceditor::SqliteResultClass::Empty() const {
	return IsEmpty;
}

bool mvceditor::SqliteResultClass::More() const {
	return !IsEmpty && NULL != Stmt;
}

bool mvceditor::SqliteResultClass::Fetch() {
	if (!Stmt) {
		return false;
	}
	bool more = Stmt->fetch();
	if (!more) {
		delete Stmt;
		Stmt = NULL;
	}
	return more;
}

bool mvceditor::SqliteResultClass::AdoptStatement(soci::statement* stmt, wxString& error)  {
	Stmt = stmt;
	IsEmpty = true;
	try {
		Stmt->define_and_bind();
		bool hasData = Stmt->execute(true);
		if (hasData) {
			IsEmpty = false;
		}
		else {
			IsEmpty = true;
			delete Stmt;
			Stmt = NULL;
		}
	} catch (std::exception& e) {
		error = mvceditor::CharToWx(e.what()); 
		wxASSERT_MSG(false, error);
		delete Stmt;
		Stmt = NULL;
	}
	return !IsEmpty;
}
