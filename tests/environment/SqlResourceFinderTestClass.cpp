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
 #include <environment/SqlResourceFinderClass.h>
 #include <unicode/ustdio.h>
 #include <string>
 
class SqlResourceFinderFixtureClass : public DatabaseTestFixtureClass {

public:

	SqlResourceFinderFixtureClass() 
		: DatabaseTestFixtureClass("sql_resource_finder") 
		, Info()
		, Finder() {
		Info.DatabaseName = UNICODE_STRING_SIMPLE("sql_resource_finder");
		Info.Host = UNICODE_STRING_SIMPLE("127.0.0.1");
		Info.User = UNICODE_STRING_SIMPLE("root");
	}
	
	mvceditor::DatabaseInfoClass Info;
	
	mvceditor::SqlResourceFinderClass Finder;
 };
 
SUITE(SqlResourceFinderTestClass) {
	 
TEST_FIXTURE(SqlResourceFinderFixtureClass, FindTable) {
	Info.DatabaseName = UNICODE_STRING_SIMPLE("sql_resource_finder");
	
	std::string query = "CREATE TABLE web_users(idUser int);";
	CHECK(Exec(query));
	query = "CREATE TABLE service_names(idServiceName int);";
	CHECK(Exec(query));
	query = "CREATE TABLE service_locations(idServiceLocation int);";
	CHECK(Exec(query));
	query = "CREATE TABLE deleted_users(idUser int, deletedDate datetime);";
	CHECK(Exec(query));
	UnicodeString error;
	CHECK(Finder.Fetch(Info, error));
	std::vector<UnicodeString> tables = Finder.FindTables(Info, UNICODE_STRING_SIMPLE("service"));
	CHECK_EQUAL((size_t)2, tables.size());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("service_locations").compare(tables.at(0)), (int8_t)0);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("service_names").compare(tables.at(1)), (int8_t)0);
}

TEST_FIXTURE(SqlResourceFinderFixtureClass, FindColumns) {	
	Info.DatabaseName = UNICODE_STRING_SIMPLE("sql_resource_finder");
	
	std::string query = "CREATE TABLE web_users(idUser int);";
	CHECK(Exec(query));
	query = "CREATE TABLE service_names(idServiceName int);";
	CHECK(Exec(query));
	query = "CREATE TABLE service_locations(idServiceLocation int);";
	CHECK(Exec(query));
	query = "CREATE TABLE deleted_users(idUser int, idDeletedBy int);";
	CHECK(Exec(query));
	UnicodeString error;
	CHECK(Finder.Fetch(Info, error));
	
	std::vector<UnicodeString> columns = Finder.FindColumns(Info, UNICODE_STRING_SIMPLE("idServiceL"));
	CHECK_EQUAL((size_t)1, columns.size());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("idServiceLocation").compare(columns.at(0)), (int8_t)0);
	
	columns = Finder.FindColumns(Info, UNICODE_STRING_SIMPLE("id"));
	CHECK_EQUAL((size_t)4, columns.size());	
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("idDeletedBy").compare(columns.at(0)), (int8_t)0);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("idServiceLocation").compare(columns.at(1)), (int8_t)0);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("idServiceName").compare(columns.at(2)), (int8_t)0);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("idUser").compare(columns.at(3)), (int8_t)0);
	
}
	 
}