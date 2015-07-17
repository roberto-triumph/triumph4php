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
#include <UnitTest++.h>
#include <globals/String.h>
#include <TriumphChecks.h>
#include <ActionTestFixtureClass.h>
#include <DatabaseTestFixtureClass.h>
#include <SqliteTestFixtureClass.h>
#include <FileTestFixtureClass.h>
#include <actions/SqlMetaDataActionClass.h>
#include <globals/Assets.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

static int ID_SQL_METADATA_FETCH = wxNewId();

class SqlMetaDataActionTestFixtureClass : public ActionTestFixtureClass, 
	public DatabaseTestFixtureClass, public SqliteTestFixtureClass, public FileTestFixtureClass {

public:

	t4p::SqlMetaDataActionClass SqlMetaDataAction;
	t4p::SqlResourceFinderClass Results;

	SqlMetaDataActionTestFixtureClass()
		: ActionTestFixtureClass()
		, DatabaseTestFixtureClass("metadata_fetch") 
		, SqliteTestFixtureClass(t4p::ResourceSqlSchemaAsset())
		, FileTestFixtureClass(wxT("metadata_fetch"))
		, SqlMetaDataAction(RunningThreads, ID_SQL_METADATA_FETCH) 
		, Results(Globals.ResourceCacheSession) {
		t4p::DatabaseTagClass dbTag;
		dbTag.Schema = UNICODE_STRING_SIMPLE("metadata_fetch");

		// user name, pwd are #defines come from the premake script premake_opts.lua
		dbTag.Host = UNICODE_STRING_SIMPLE("127.0.0.1");
		dbTag.User = t4p::CharToIcu(UserName().c_str());
		dbTag.Password = t4p::CharToIcu(Password().c_str());
		Globals.DatabaseTags.push_back(dbTag);

		TouchTestDir();
		InitTagCache(TestProjectDir);
		Globals.ResourceCacheSession.open(*soci::factory_sqlite3(), t4p::WxToChar(Globals.TagCacheDbFileName.GetFullPath()));
		SqliteTestFixtureClass::CreateDatabase(Globals.ResourceCacheSession, t4p::ResourceSqlSchemaAsset());
		CreateTable();
	}

	void CreateTable() {
		CHECK(DatabaseTestFixtureClass::Exec("CREATE TABLE my_users (id INT, name VARCHAR(255) NOT NULL);"));
	}

	void OnSqlMetaDataComplete(t4p::SqlMetaDataEventClass& event) {
	}

	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(SqlMetaDataActionTestFixtureClass, ActionTestFixtureClass)
	EVT_SQL_META_DATA_COMPLETE(ID_SQL_METADATA_FETCH, SqlMetaDataActionTestFixtureClass::OnSqlMetaDataComplete)
END_EVENT_TABLE()

SUITE(SqlMetaDataActionTestClass) {

TEST_FIXTURE(SqlMetaDataActionTestFixtureClass, Fetch) {
	CHECK(SqlMetaDataAction.Init(Globals));

	SqlMetaDataAction.BackgroundWork();

	std::vector<UnicodeString> tables = Results.FindTables(Globals.DatabaseTags[0], UNICODE_STRING_SIMPLE("my_users"));
	CHECK_VECTOR_SIZE(1, tables);
	CHECK_UNISTR_EQUALS("my_users", tables[0]); 
}

}