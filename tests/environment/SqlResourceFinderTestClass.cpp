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
 #include <environment/SqlResourceFinderClass.h>
 #include <unicode/ustdio.h>
 #include <string>
 
class MySqlFixtureClass {

public:

	MySqlFixtureClass() 
		: Info() 
		, Query() 
		, Finder()
		, Session() {
		Info.DatabaseName = UNICODE_STRING_SIMPLE("mysql");
		Info.Driver = mvceditor::DatabaseInfoClass::MYSQL;
		Info.Host = UNICODE_STRING_SIMPLE("127.0.0.1");
		Info.User = UNICODE_STRING_SIMPLE("root");
		Query.Info.Copy(Info);
		Clean();
	}
	
	~MySqlFixtureClass() {
		
	}
	
	void Clean() {
		Exec("DROP DATABASE IF EXISTS sql_resource_test");
	}
	
	bool CreateDatabase(std::string name) {
		return Exec("CREATE DATABASE " + name);
	}
	
	bool Exec(std::string query) {
		
		// in case info changes
		Query.Info.Copy(Info);
		UnicodeString error;
		bool ret = Query.Connect(Session, error);
		if (ret) {			
			soci::statement stmt = (Session.prepare << query);
			ret = Query.Execute(stmt, error);
			if (!ret) {
				UFILE* out = u_finit(stdout, NULL, NULL);
				u_fprintf(out, "%S\n", error.getTerminatedBuffer());
				u_fclose(out);
			}
			Query.Close(Session, stmt);
		}
		return ret;
	}
	
	mvceditor::DatabaseInfoClass Info;
	mvceditor::SqlQueryClass Query;
	mvceditor::SqlResourceFinderClass Finder;
	soci::session Session;
	 
 };
 
SUITE(SqlResourceFinderTestClassSuite) {
	 
TEST_FIXTURE(MySqlFixtureClass, FindTable) {
	CHECK(CreateDatabase("sql_resource_test"));
	Info.DatabaseName = UNICODE_STRING_SIMPLE("sql_resource_test");
	
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

TEST_FIXTURE(MySqlFixtureClass, FindColumns) {	
	CHECK(CreateDatabase("sql_resource_test"));
	Info.DatabaseName = UNICODE_STRING_SIMPLE("sql_resource_test");
	
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