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
#include <MvcEditorChecks.h>
#include <ActionTestFixtureClass.h>
#include <DatabaseTestFixtureClass.h>
#include <actions/SqlMetaDataActionClass.h>

static int ID_SQL_METADATA_FETCH = wxNewId();

class SqlMetaDataActionTestFixtureClass : public ActionTestFixtureClass, public DatabaseTestFixtureClass {

public:

	mvceditor::SqlMetaDataActionClass SqlMetaDataAction;
	mvceditor::SqlResourceFinderClass Results;
	mvceditor::GlobalsClass Globals;

	SqlMetaDataActionTestFixtureClass()
		: ActionTestFixtureClass()
		, DatabaseTestFixtureClass("metadata_fetch") 
		, SqlMetaDataAction(RunningThreads, ID_SQL_METADATA_FETCH) 
		, Results() 
		, Globals() {
		mvceditor::DatabaseInfoClass info;
		info.DatabaseName = UNICODE_STRING_SIMPLE("metadata_fetch");

		// user name, pwd are #defines come from the premake script premake_opts.lua
		info.Host = UNICODE_STRING_SIMPLE("127.0.0.1");
		info.User = mvceditor::CharToIcu(UserName().c_str());
		info.Password = mvceditor::CharToIcu(Password().c_str());

		Globals.Infos.push_back(info);

		CreateTable();
	}

	void CreateTable() {
		CHECK(Exec("CREATE TABLE my_users (id INT, name VARCHAR(255) NOT NULL);"));
	}

	void OnSqlMetaDataComplete(mvceditor::SqlMetaDataEventClass& event) {
		Results.Copy(event.NewResources);
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

	std::vector<UnicodeString> tables = Results.FindTables(Globals.Infos[0], UNICODE_STRING_SIMPLE("my_users"));
	CHECK_VECTOR_SIZE(1, tables);
	CHECK_UNISTR_EQUALS("my_users", tables[0]); 
}

}