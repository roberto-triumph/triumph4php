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


std::string t4p::SqliteSqlLikeEscape(const std::string& value, char e) {
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

bool t4p::SqliteSqlScript(const wxFileName& sqlScriptFileName, soci::session& session, wxString& error) {
	bool ret = false;

	// make sure that the sql script exists before we delete anything
	wxASSERT_MSG(sqlScriptFileName.FileExists(), wxT("SQL script file must exist"));

	// get all of the tables in the db
	std::vector<std::string> tableNames;
	ret = t4p::SqliteTables(session, tableNames, error);
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
		std::string stdSql = t4p::WxToChar(sql);

		// get the 'raw' connection because it can handle multiple statements at once
		char *errorMessage = NULL;
		soci::sqlite3_session_backend* backend = static_cast<soci::sqlite3_session_backend*>(session.get_backend());
		
		wxASSERT_MSG(backend->conn_, "session is not connected, cannot execute sql script file");
		sqlite_api::sqlite3_exec(backend->conn_, stdSql.c_str(), NULL, NULL, &errorMessage);
		if (errorMessage) {
			error = t4p::CharToWx(errorMessage);
			sqlite_api::sqlite3_free(errorMessage);
		}
		else {
			ret = true;
		}
	} catch (std::exception& e) {
		error = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, error);
	}
	return ret;
}

bool t4p::SqliteTables(soci::session& session, std::vector<std::string>& tableNames, wxString& error) {
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
		error = t4p::CharToWx(e.what());
		wxUnusedVar(e);
		wxASSERT_MSG(ret, error);
	}
	return ret;
}

int t4p::SqliteSchemaVersion(soci::session& session) {
	int versionNumber = 0;
	try {

		// check to see if table exists, on first the very first program run 
		// there won't be any tables
		std::string table;
		session.once << "SELECT name FROM sqlite_master WHERE type='table' AND name='schema_version'",
			soci::into(table);
		if (!table.empty()) {
			session.once << "SELECT version_number FROM schema_version", soci::into(versionNumber);
		}
	} catch (std::exception& e) {
		wxUnusedVar(e);
		versionNumber = -1;	
	}
	return versionNumber;
}

void t4p::SqliteSetBusyTimeout(soci::session& session, int timeoutMs) {

	// get the 'raw' sqlite connection
	soci::sqlite3_session_backend* backend = static_cast<soci::sqlite3_session_backend*>(session.get_backend());
	sqlite_api::sqlite3_busy_timeout(backend->conn_, timeoutMs);
}

int t4p::SqliteInsertId(soci::statement& stmt) {
	soci::sqlite3_statement_backend* backend = static_cast<soci::sqlite3_statement_backend*>(stmt.get_backend());
	return sqlite3_last_insert_rowid(backend->session_.conn_);
}

bool t4p::SqliteOpen(soci::session& session, const wxString& dbName) {
	bool ret = false;
	try {
		std::string stdDbName = t4p::WxToChar(dbName);

		// we should be able to open this since it has been created by
		// the TagCacheDbVersionActionClass
		session.open(*soci::factory_sqlite3(), stdDbName);

		// set a busy handler so that if we attempt to query while the file is locked, we
		// sleep for a bit then try again
		t4p::SqliteSetBusyTimeout(session, 200);
		ret = true;
	} catch(std::exception const& e) {
		session.close();
		wxString msg = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, msg);
	}
	return ret;
}