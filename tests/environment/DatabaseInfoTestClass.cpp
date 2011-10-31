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
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <UnitTest++.h>
#include <DatabaseTestFixtureClass.h>
#include <environment/DatabaseInfoClass.h>
#include <soci.h>

class DatabaseInfoTestFixtureClass : public DatabaseTestFixtureClass {
public:
	DatabaseInfoTestFixtureClass()
		: DatabaseTestFixtureClass("database_info")
		, Info() {
		Info.DatabaseName = UNICODE_STRING_SIMPLE("database_info");
		Info.Host = UNICODE_STRING_SIMPLE("127.0.0.1");
		Info.User = UNICODE_STRING_SIMPLE("root");
	}
	
	mvceditor::DatabaseInfoClass Info;
};

SUITE(DatabaseInfoClassTest) {
	
TEST_FIXTURE(DatabaseInfoTestFixtureClass, ConnectQueryAndResults) {
	Exec("CREATE TABLE names (id INT, name VARCHAR(255));");
	Exec("INSERT INTO names(id, name) VALUES(1, 'one')");
	Exec("INSERT INTO names(id, name) VALUES(2, 'two')");
	Exec("INSERT INTO names(id, name) VALUES(3, 'three')");
	
	soci::session session;
	bool hasError = false;
	mvceditor::SqlQueryClass query;
	UnicodeString error;
	
	query.Info.Copy(Info);
	CHECK(query.Connect(session, error));
	CHECK_EQUAL(0, error.length());
	mvceditor::SqlResultClass results(session);
	CHECK(query.Execute(session, results, UNICODE_STRING_SIMPLE("SELECT * FROM names ORDER BY id;")));
	CHECK_EQUAL(0, results.Error.length());
	CHECK(results.Success);
	CHECK_EQUAL(3, query.GetAffectedRows(*results.Stmt));
	CHECK(query.More(*results.Stmt, hasError, error));
	CHECK_EQUAL(0, error.length());
	CHECK_EQUAL(1, results.Row.get<int>(0));
	CHECK_EQUAL("one", results.Row.get<std::string>(1));
	
	CHECK(query.More(*results.Stmt, hasError, error));
	CHECK_EQUAL(2, results.Row.get<int>(0));
	CHECK_EQUAL("two", results.Row.get<std::string>(1));
	
	CHECK(query.More(*results.Stmt, hasError, error));
	CHECK_EQUAL(3, results.Row.get<int>(0));
	CHECK_EQUAL("three", results.Row.get<std::string>(1));
	
	CHECK_EQUAL(false, query.More(*results.Stmt, hasError, error));
}

TEST_FIXTURE(DatabaseInfoTestFixtureClass, MultipleQueries) {
	Exec("CREATE TABLE names (id INT, name VARCHAR(255));");
	Exec("CREATE TABLE places (id INT, name VARCHAR(255));");
	Exec("INSERT INTO names(id, name) VALUES(1, 'one')");
	Exec("INSERT INTO names(id, name) VALUES(2, 'two')");
	Exec("INSERT INTO names(id, name) VALUES(3, 'three')");
	
	soci::session session;
	bool hasError = false;
	mvceditor::SqlQueryClass query;
	UnicodeString error;
	
	query.Info.Copy(Info);
	CHECK(query.Connect(session, error));
	CHECK_EQUAL(0, error.length());
	mvceditor::SqlResultClass results(session);
	
	CHECK(query.Execute(session, results, UNICODE_STRING_SIMPLE("DELETE FROM places;")));
	CHECK_EQUAL(0, results.Error.length());
	CHECK(results.Success);
	CHECK_EQUAL(false, query.More(*results.Stmt, hasError, error));
	results.Close();
	
	mvceditor::SqlResultClass results2(session);
	CHECK(query.Execute(session, results2, UNICODE_STRING_SIMPLE("SELECT * FROM names ORDER BY id;")));
	CHECK_EQUAL(0, results2.Error.length());
	CHECK(results2.Success);
	int rowCount = 0;
	while (query.More(*results2.Stmt, hasError, error)) {
		CHECK_EQUAL(false, hasError);
		CHECK_EQUAL(0, error.length());
		rowCount++;
	}
	CHECK_EQUAL(3, rowCount);
	results2.Close();
	
	mvceditor::SqlResultClass results3(session);
	CHECK(query.Execute(session, results3, UNICODE_STRING_SIMPLE("DELETE FROM names;")));
	CHECK_EQUAL(0, results3.Error.length());
	CHECK(results3.Success);
	CHECK_EQUAL(false, query.More(*results3.Stmt, hasError, error));
	CHECK_EQUAL(false, query.More(*results3.Stmt, hasError, error));
	results3.Close();
}

}