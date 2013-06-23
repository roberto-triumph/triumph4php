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
 
SqliteTestFixtureClass::SqliteTestFixtureClass()
	: Session()
	, ConnectionString(":memory:") {
	Session.open(*soci::factory_sqlite3(), ConnectionString);
	CreateDatabase(Session, mvceditor::ResourceSqlSchemaAsset());
}

void SqliteTestFixtureClass::CreateDatabase(soci::session& session, const wxFileName& sqlScriptFile) {
	std::string schemaSql;
	wxASSERT_MSG(sqlScriptFile == mvceditor::ResourceSqlSchemaAsset() || sqlScriptFile == mvceditor::DetectorSqlSchemaAsset(), 
		wxT("sqlScript must be either ResourceSqlSchemaAsset() or DetectorSqlSchemaAsset() from Assets.h"));
	if (sqlScriptFile == mvceditor::ResourceSqlSchemaAsset()) {
		schemaSql = ResourceSchemaSql;
	}
	else {
		schemaSql = DetectorSchemaSql;
	}

	if (schemaSql.empty()) {
		
		// now get the contents of the script to be executed
		// keep the contents in memory so that we dont have to read the file
		// for each test
		wxFFile ffile(sqlScriptFile.GetFullPath(), wxT("rb"));
		wxString sql;
		ffile.ReadAll(&sql);
		if (sqlScriptFile == mvceditor::ResourceSqlSchemaAsset()) {
			ResourceSchemaSql = mvceditor::WxToChar(sql);
			schemaSql = ResourceSchemaSql;
		}
		else {
			DetectorSchemaSql = mvceditor::WxToChar(sql);
			schemaSql = DetectorSchemaSql;
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
			wxASSERT_MSG(strlen(errorMessage) == 0, mvceditor::CharToWx(errorMessage));
			sqlite_api::sqlite3_free(errorMessage);
		}
	} catch (const std::exception& e) {
		wxUnusedVar(e);
		wxASSERT_MSG(false, mvceditor::CharToWx(e.what()));
	}
}
	
SqliteTestFixtureClass::~SqliteTestFixtureClass() {
	Session.close();
}

bool SqliteTestFixtureClass::Exec(const std::string& query) {
	soci::statement stmt = (Session.prepare << query);
	try {
		
		// execute returns true only when there are results; we want to return
		// true on success
		stmt.execute(false);
		return true;
	} catch (std::exception& e) {
		printf("exception=%s\n", e.what());
	}
	return false;
}