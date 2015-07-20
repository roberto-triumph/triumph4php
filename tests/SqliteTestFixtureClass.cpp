/*
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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <SqliteTestFixtureClass.h>
#include <globals/Assets.h>
#include <globals/String.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <wx/ffile.h>
#include <stdio.h>

static std::string ResourceSchemaSql = "";
static std::string DetectorSchemaSql = "";
static std::string JsTagsSchemaSql = "";

SqliteTestFixtureClass::SqliteTestFixtureClass(const wxFileName& sqlScriptFile)
	: Session()
	, ConnectionString(":memory:") {
	Session.open(*soci::factory_sqlite3(), ConnectionString);
	CreateDatabase(Session, sqlScriptFile);
}

void SqliteTestFixtureClass::CreateDatabase(soci::session& session, const wxFileName& sqlScriptFile) {
	std::string schemaSql;
	wxASSERT_MSG(sqlScriptFile == t4p::ResourceSqlSchemaAsset() ||
		sqlScriptFile == t4p::DetectorSqlSchemaAsset() ||
		sqlScriptFile == t4p::JsTagsSqlSchemaAsset(),
		wxT("sqlScript must be either ResourceSqlSchemaAsset() or DetectorSqlSchemaAsset() t4p::JsTagsSqlSchemaAsset() from Assets.h"));
	if (sqlScriptFile == t4p::ResourceSqlSchemaAsset()) {
		schemaSql = ResourceSchemaSql;
	} else if (sqlScriptFile == t4p::DetectorSqlSchemaAsset()) {
		schemaSql = DetectorSchemaSql;
	} else if (sqlScriptFile == t4p::JsTagsSqlSchemaAsset()) {
		schemaSql = JsTagsSchemaSql;
	}

	if (schemaSql.empty()) {
		// now get the contents of the script to be executed
		// keep the contents in memory so that we dont have to read the file
		// for each test
		wxFFile ffile(sqlScriptFile.GetFullPath(), wxT("rb"));
		wxString sql;
		ffile.ReadAll(&sql);
		if (sqlScriptFile == t4p::ResourceSqlSchemaAsset()) {
			ResourceSchemaSql = t4p::WxToChar(sql);
			schemaSql = ResourceSchemaSql;
		} else if (sqlScriptFile == t4p::DetectorSqlSchemaAsset()) {
			DetectorSchemaSql = t4p::WxToChar(sql);
			schemaSql = DetectorSchemaSql;
		} else if (sqlScriptFile == t4p::JsTagsSqlSchemaAsset()) {
			JsTagsSchemaSql = t4p::WxToChar(sql);
			schemaSql = JsTagsSchemaSql;
		}
	}
	try {
		// get the 'raw' connection because it can handle multiple statements at once
		char *errorMessage = NULL;
		soci::sqlite3_session_backend* backend = static_cast<soci::sqlite3_session_backend*>(session.get_backend());
		wxASSERT(backend != 0);
		wxASSERT(backend->conn_ != 0);
		wxASSERT(schemaSql.c_str() != 0);

		sqlite_api::sqlite3_exec(backend->conn_, schemaSql.c_str(), NULL, NULL, &errorMessage);
		if (errorMessage) {
			wxASSERT_MSG(strlen(errorMessage) == 0, t4p::CharToWx(errorMessage));
			sqlite_api::sqlite3_free(errorMessage);
		}
	} catch (const std::exception& e) {
		wxUnusedVar(e);
		wxASSERT_MSG(false, t4p::CharToWx(e.what()));
	}
}

SqliteTestFixtureClass::~SqliteTestFixtureClass() {
	Session.close();
}

bool SqliteTestFixtureClass::Exec(const std::string& query) {
	try {
		Session.once << query;
		return true;
	} catch (std::exception& e) {
		printf("exception=%s\n", e.what());
	}
	return false;
}
