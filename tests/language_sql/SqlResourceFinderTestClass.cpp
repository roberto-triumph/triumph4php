/**
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
 */
#include <UnitTest++.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <unicode/ustdio.h>
#include <wx/platinfo.h>
#include <string>
#include <vector>
#include "DatabaseTestFixtureClass.h"
#include "FileTestFixtureClass.h"
#include "globals/Assets.h"
#include "globals/String.h"
#include "language_sql/SqlResourceFinderClass.h"
#include "SqliteTestFixtureClass.h"
#include "TriumphChecks.h"

class MysqlResourceFinderFixtureClass : public DatabaseTestFixtureClass,
	public SqliteTestFixtureClass {
	public:
	MysqlResourceFinderFixtureClass()
		: DatabaseTestFixtureClass("sql_resource_finder")
		, SqliteTestFixtureClass(t4p::ResourceSqlSchemaAsset())
		, DatabaseTag()
		, Fetcher(SqliteTestFixtureClass::Session)
		, Finder(SqliteTestFixtureClass::Session) {
		DatabaseTag.Driver = t4p::DatabaseTagClass::MYSQL;
		DatabaseTag.Schema = UNICODE_STRING_SIMPLE("sql_resource_finder");

		// user name, pwd are #defines come from the premake script premake_opts.lua
		DatabaseTag.Host = UNICODE_STRING_SIMPLE("127.0.0.1");
		DatabaseTag.User = t4p::CharToIcu(UserName().c_str());
		DatabaseTag.Password = t4p::CharToIcu(Password().c_str());
	}

	t4p::DatabaseTagClass DatabaseTag;

	t4p::SqlResourceFetchClass Fetcher;

	t4p::SqlResourceFinderClass Finder;
};

class SqliteResourceFinderFixtureClass : public FileTestFixtureClass, public SqliteTestFixtureClass {
	public:
	SqliteResourceFinderFixtureClass()
		: FileTestFixtureClass("sql_resource_finder")
		, SqliteTestFixtureClass(t4p::ResourceSqlSchemaAsset())
		, DatabaseTag()
		, Fetcher(Session)
		, Finder(Session)
		, SqliteFile()
		, TestSession() {
		// create a sqlite db file
		TouchTestDir();
		SqliteFile.Assign(TestProjectDir, wxT("sqlite.db"));
		TestSession.open(*soci::factory_sqlite3(),
			t4p::WxToChar(SqliteFile.GetFullPath()));

		DatabaseTag.Driver = t4p::DatabaseTagClass::SQLITE;
		DatabaseTag.FileName = SqliteFile;
	}

	bool ExecIntoTest(const std::string& sql) {
		try {
			TestSession.once << sql;
			return true;
		} catch (std::exception& e) {
			printf("exception=%s\n", e.what());
		}
		return false;
	}

	t4p::DatabaseTagClass DatabaseTag;

	t4p::SqlResourceFetchClass Fetcher;

	t4p::SqlResourceFinderClass Finder;

	wxFileName SqliteFile;

	// the db file we will inspect
	soci::session TestSession;
};

SUITE(SqlResourceFinderTestClass) {
TEST_FIXTURE(MysqlResourceFinderFixtureClass, FindTable) {
	std::string query = "CREATE TABLE web_users(idUser int);";
	CHECK(DatabaseTestFixtureClass::Exec(query));
	query = "CREATE TABLE service_names(idServiceName int);";
	CHECK(DatabaseTestFixtureClass::Exec(query));
	query = "CREATE TABLE service_locations(idServiceLocation int);";
	CHECK(DatabaseTestFixtureClass::Exec(query));
	query = "CREATE TABLE deleted_users(idUser int, deletedDate datetime);";
	CHECK(DatabaseTestFixtureClass::Exec(query));
	UnicodeString error;
	CHECK(Fetcher.Fetch(DatabaseTag, error));
	std::vector<UnicodeString> tables = Finder.FindTables(DatabaseTag, UNICODE_STRING_SIMPLE("service"));
	CHECK_VECTOR_SIZE(2, tables);
	CHECK_UNISTR_EQUALS_NO_CASE("service_locations", tables[0]);
	CHECK_UNISTR_EQUALS_NO_CASE("service_names", tables[1]);
}

TEST_FIXTURE(MysqlResourceFinderFixtureClass, FindTableCaseInsensitive) {
	std::string query = "SHOW VARIABLES WHERE Variable_name='version_compile_os' AND Value IN('Win64', 'Win32');";
	if (DatabaseTestFixtureClass::Exec(query)) {
		// skip this test on windows, MySQL always creates tables with lowercase names
		return;
	}
	query = "CREATE TABLE WebUsers(idUser int);";
	CHECK(DatabaseTestFixtureClass::Exec(query));
	query = "CREATE TABLE ServiceNames(idServiceName int);";
	CHECK(DatabaseTestFixtureClass::Exec(query));
	query = "CREATE TABLE ServiceLocations(idServiceLocation int);";
	CHECK(DatabaseTestFixtureClass::Exec(query));
	query = "CREATE TABLE Deleted_Users(idUser int, deletedDate datetime);";
	CHECK(DatabaseTestFixtureClass::Exec(query));
	UnicodeString error;
	CHECK(Fetcher.Fetch(DatabaseTag, error));
	std::vector<UnicodeString> tables = Finder.FindTables(DatabaseTag, UNICODE_STRING_SIMPLE("service"));
	CHECK_VECTOR_SIZE(2, tables);

	CHECK_UNISTR_EQUALS("ServiceLocations", tables[0]);
	CHECK_UNISTR_EQUALS("ServiceNames", tables[1]);
}

TEST_FIXTURE(MysqlResourceFinderFixtureClass, FindTableShouldLocateInformationSchema) {
	UnicodeString error;
	CHECK(Fetcher.Fetch(DatabaseTag, error));
	std::vector<UnicodeString> tables = Finder.FindTables(DatabaseTag, UNICODE_STRING_SIMPLE("information_sche"));
	CHECK_VECTOR_SIZE(1, tables);
	CHECK_UNISTR_EQUALS_NO_CASE("information_schema", tables[0]);

	tables = Finder.FindTables(DatabaseTag, UNICODE_STRING_SIMPLE("colum"));
	CHECK_VECTOR_SIZE(2, tables);
	CHECK_UNISTR_EQUALS_NO_CASE("column_privileges", tables[0]);
	CHECK_UNISTR_EQUALS_NO_CASE("columns", tables[1]);
}

TEST_FIXTURE(MysqlResourceFinderFixtureClass, FindColumns) {
	std::string query = "CREATE TABLE web_users(idIUser int);";
	CHECK(DatabaseTestFixtureClass::Exec(query));
	query = "CREATE TABLE service_names(idIServiceName int);";
	CHECK(DatabaseTestFixtureClass::Exec(query));
	query = "CREATE TABLE service_locations(idIServiceLocation int);";
	CHECK(DatabaseTestFixtureClass::Exec(query));
	query = "CREATE TABLE deleted_users(idIUser int, idIDeletedBy int);";
	CHECK(DatabaseTestFixtureClass::Exec(query));
	UnicodeString error;
	CHECK(Fetcher.Fetch(DatabaseTag, error));

	std::vector<UnicodeString> columns = Finder.FindColumns(DatabaseTag, UNICODE_STRING_SIMPLE("idIServiceL"));
	CHECK_VECTOR_SIZE(1, columns);
	CHECK_UNISTR_EQUALS_NO_CASE("idIServiceLocation", columns[0]);


	columns = Finder.FindColumns(DatabaseTag, UNICODE_STRING_SIMPLE("idI"));

	CHECK_VECTOR_SIZE(4, columns);
	CHECK_UNISTR_EQUALS_NO_CASE("idIDeletedBy", columns[0]);
	CHECK_UNISTR_EQUALS_NO_CASE("idIServiceLocation", columns[1]);
	CHECK_UNISTR_EQUALS_NO_CASE("idIServiceName", columns[2]);
	CHECK_UNISTR_EQUALS_NO_CASE("idIUser", columns[3]);
}

TEST_FIXTURE(MysqlResourceFinderFixtureClass, FindColumnsCaseInsensitive) {
	std::string query = "CREATE TABLE web_users(idIUser int);";
	CHECK(DatabaseTestFixtureClass::Exec(query));
	query = "CREATE TABLE service_names(idIServiceName int);";
	CHECK(DatabaseTestFixtureClass::Exec(query));
	query = "CREATE TABLE service_locations(idIServiceLocation int);";
	CHECK(DatabaseTestFixtureClass::Exec(query));
	query = "CREATE TABLE deleted_users(idIUser int, idIDeletedBy int);";
	CHECK(DatabaseTestFixtureClass::Exec(query));
	UnicodeString error;
	CHECK(Fetcher.Fetch(DatabaseTag, error));

	std::vector<UnicodeString> columns = Finder.FindColumns(DatabaseTag, UNICODE_STRING_SIMPLE("idiservice"));
	CHECK_VECTOR_SIZE(2, columns);
	CHECK_UNISTR_EQUALS("idIServiceLocation", columns[0]);
	CHECK_UNISTR_EQUALS("idIServiceName", columns[1]);
}

TEST_FIXTURE(MysqlResourceFinderFixtureClass, FindDatabaseTagrmationSchemaColumns) {
	UnicodeString error;
	CHECK(Fetcher.Fetch(DatabaseTag, error));
	std::vector<UnicodeString> columns = Finder.FindColumns(DatabaseTag, UNICODE_STRING_SIMPLE("table_nam"));
	CHECK_VECTOR_SIZE(1, columns);
	CHECK_UNISTR_EQUALS_NO_CASE("table_name", columns[0]);
}

TEST_FIXTURE(SqliteResourceFinderFixtureClass, FindTable) {
	std::string query = "CREATE TABLE web_users(idUser int);";
	CHECK(ExecIntoTest(query));
	query = "CREATE TABLE service_names(idServiceName int);";
	CHECK(ExecIntoTest(query));
	query = "CREATE TABLE service_locations(idServiceLocation int);";
	CHECK(ExecIntoTest(query));
	query = "CREATE TABLE deleted_users(idUser int, deletedDate datetime);";
	CHECK(ExecIntoTest(query));
	UnicodeString error;
	CHECK(Fetcher.Fetch(DatabaseTag, error));
	std::vector<UnicodeString> tables = Finder.FindTables(DatabaseTag, UNICODE_STRING_SIMPLE("service"));
	CHECK_VECTOR_SIZE(2, tables);
	CHECK_UNISTR_EQUALS_NO_CASE("service_locations", tables[0]);
	CHECK_UNISTR_EQUALS_NO_CASE("service_names", tables[1]);
}

TEST_FIXTURE(SqliteResourceFinderFixtureClass, FindColumns) {
	std::string query = "CREATE TABLE web_users(idIUser int);";
	CHECK(ExecIntoTest(query));
	query = "CREATE TABLE service_names(idIServiceName int);";
	CHECK(ExecIntoTest(query));
	query = "CREATE TABLE service_locations(idIServiceLocation int);";
	CHECK(ExecIntoTest(query));
	query = "CREATE TABLE deleted_users(idIUser int, idIDeletedBy int);";
	CHECK(ExecIntoTest(query));
	UnicodeString error;
	CHECK(Fetcher.Fetch(DatabaseTag, error));

	std::vector<UnicodeString> columns = Finder.FindColumns(DatabaseTag, UNICODE_STRING_SIMPLE("idIServiceL"));
	CHECK_VECTOR_SIZE(1, columns);
	CHECK_UNISTR_EQUALS_NO_CASE("idIServiceLocation", columns[0]);

	columns = Finder.FindColumns(DatabaseTag, UNICODE_STRING_SIMPLE("idI"));

	CHECK_VECTOR_SIZE(4, columns);
	CHECK_UNISTR_EQUALS_NO_CASE("idIDeletedBy", columns[0]);
	CHECK_UNISTR_EQUALS_NO_CASE("idIServiceLocation", columns[1]);
	CHECK_UNISTR_EQUALS_NO_CASE("idIServiceName", columns[2]);
	CHECK_UNISTR_EQUALS_NO_CASE("idIUser", columns[3]);
}
}
