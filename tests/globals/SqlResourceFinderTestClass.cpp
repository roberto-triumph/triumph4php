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
#include <UnitTest++.h>
#include <DatabaseTestFixtureClass.h>
#include <globals/SqlResourceFinderClass.h>
#include <globals/String.h>
#include <MvcEditorChecks.h>
#include <unicode/ustdio.h>
#include <wx/platinfo.h>
#include <string>

class SqlResourceFinderFixtureClass : public DatabaseTestFixtureClass {

public:

	SqlResourceFinderFixtureClass() 
		: DatabaseTestFixtureClass("sql_resource_finder") 
		, DatabaseTag()
		, Finder() {
		DatabaseTag.Schema = UNICODE_STRING_SIMPLE("sql_resource_finder");

		// user name, pwd are #defines come from the premake script premake_opts.lua
		DatabaseTag.Host = UNICODE_STRING_SIMPLE("127.0.0.1");
		DatabaseTag.User = mvceditor::CharToIcu(UserName().c_str());
		DatabaseTag.Password = mvceditor::CharToIcu(Password().c_str());
	}
	
	mvceditor::DatabaseTagClass DatabaseTag;
	
	mvceditor::SqlResourceFinderClass Finder;
 };
 
SUITE(SqlResourceFinderTestClass) {
	 
TEST_FIXTURE(SqlResourceFinderFixtureClass, FindTable) {
	DatabaseTag.Schema = UNICODE_STRING_SIMPLE("sql_resource_finder");
	
	std::string query = "CREATE TABLE web_users(idUser int);";
	CHECK(Exec(query));
	query = "CREATE TABLE service_names(idServiceName int);";
	CHECK(Exec(query));
	query = "CREATE TABLE service_locations(idServiceLocation int);";
	CHECK(Exec(query));
	query = "CREATE TABLE deleted_users(idUser int, deletedDate datetime);";
	CHECK(Exec(query));
	UnicodeString error;
	CHECK(Finder.Fetch(DatabaseTag, error));
	std::vector<UnicodeString> tables = Finder.FindTables(DatabaseTag, UNICODE_STRING_SIMPLE("service"));
	CHECK_VECTOR_SIZE(2, tables);
	CHECK_UNISTR_EQUALS_NO_CASE("service_locations", tables[0]);
	CHECK_UNISTR_EQUALS_NO_CASE("service_names", tables[1]);
}

TEST_FIXTURE(SqlResourceFinderFixtureClass, FindTableCaseInsensitive) {
	std::string query = "SHOW VARIABLES WHERE Variable_name='version_compile_os' AND Value IN('Win64', 'Win32');";
	if (Exec(query)) {
		//skip this test on windows, MySQL always creates tables with lowercase names
		return;
	}

	DatabaseTag.Schema = UNICODE_STRING_SIMPLE("sql_resource_finder");
	
	query = "CREATE TABLE WebUsers(idUser int);";
	CHECK(Exec(query));
	query = "CREATE TABLE ServiceNames(idServiceName int);";
	CHECK(Exec(query));
	query = "CREATE TABLE ServiceLocations(idServiceLocation int);";
	CHECK(Exec(query));
	query = "CREATE TABLE Deleted_Users(idUser int, deletedDate datetime);";
	CHECK(Exec(query));
	UnicodeString error;
	CHECK(Finder.Fetch(DatabaseTag, error));
	std::vector<UnicodeString> tables = Finder.FindTables(DatabaseTag, UNICODE_STRING_SIMPLE("service"));
	CHECK_VECTOR_SIZE(2, tables);

	CHECK_UNISTR_EQUALS("ServiceLocations", tables[0]);
	CHECK_UNISTR_EQUALS("ServiceNames", tables[1]);
}

TEST_FIXTURE(SqlResourceFinderFixtureClass, FindTableShouldLocateDatabaseTagrmationSchema) {
	DatabaseTag.Schema = UNICODE_STRING_SIMPLE("sql_resource_finder");
	UnicodeString error;
	CHECK(Finder.Fetch(DatabaseTag, error));
	std::vector<UnicodeString> tables = Finder.FindTables(DatabaseTag, UNICODE_STRING_SIMPLE("information_sche"));
	CHECK_VECTOR_SIZE(1, tables);
	CHECK_UNISTR_EQUALS_NO_CASE("information_schema", tables[0]);

	tables = Finder.FindTables(DatabaseTag, UNICODE_STRING_SIMPLE("colum"));
	CHECK_VECTOR_SIZE(2, tables);
	CHECK_UNISTR_EQUALS_NO_CASE("column_privileges", tables[0]);
	CHECK_UNISTR_EQUALS_NO_CASE("columns", tables[1]);
}

TEST_FIXTURE(SqlResourceFinderFixtureClass, FindColumns) {	
	DatabaseTag.Schema = UNICODE_STRING_SIMPLE("sql_resource_finder");
	
	std::string query = "CREATE TABLE web_users(idIUser int);";
	CHECK(Exec(query));
	query = "CREATE TABLE service_names(idIServiceName int);";
	CHECK(Exec(query));
	query = "CREATE TABLE service_locations(idIServiceLocation int);";
	CHECK(Exec(query));
	query = "CREATE TABLE deleted_users(idIUser int, idIDeletedBy int);";
	CHECK(Exec(query));
	UnicodeString error;
	CHECK(Finder.Fetch(DatabaseTag, error));
	
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

TEST_FIXTURE(SqlResourceFinderFixtureClass, FindColumnsCaseInsensitive) {	
	DatabaseTag.Schema = UNICODE_STRING_SIMPLE("sql_resource_finder");
	
	std::string query = "CREATE TABLE web_users(idIUser int);";
	CHECK(Exec(query));
	query = "CREATE TABLE service_names(idIServiceName int);";
	CHECK(Exec(query));
	query = "CREATE TABLE service_locations(idIServiceLocation int);";
	CHECK(Exec(query));
	query = "CREATE TABLE deleted_users(idIUser int, idIDeletedBy int);";
	CHECK(Exec(query));
	UnicodeString error;
	CHECK(Finder.Fetch(DatabaseTag, error));
	
	std::vector<UnicodeString> columns = Finder.FindColumns(DatabaseTag, UNICODE_STRING_SIMPLE("idiservice"));
	CHECK_VECTOR_SIZE(2, columns);
	CHECK_UNISTR_EQUALS("idIServiceLocation", columns[0]);
	CHECK_UNISTR_EQUALS("idIServiceName", columns[1]);

}

TEST_FIXTURE(SqlResourceFinderFixtureClass, FindDatabaseTagrmationSchemaColumns) {
	DatabaseTag.Schema = UNICODE_STRING_SIMPLE("sql_resource_finder");
	UnicodeString error;
	CHECK(Finder.Fetch(DatabaseTag, error));
	std::vector<UnicodeString> columns = Finder.FindColumns(DatabaseTag, UNICODE_STRING_SIMPLE("table_nam"));
	CHECK_VECTOR_SIZE(1, columns);
	CHECK_UNISTR_EQUALS_NO_CASE("table_name", columns[0]);
}

}
