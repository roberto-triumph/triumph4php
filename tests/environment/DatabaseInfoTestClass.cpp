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
	mvceditor::SqlQueryClass query;
	UnicodeString error;
	
	query.Info.Copy(Info);
	CHECK(query.Connect(session, error));
	CHECK_EQUAL(0, error.length());
	mvceditor::SqlResultClass results;
	CHECK(query.Execute(session, results, UNICODE_STRING_SIMPLE("SELECT * FROM names ORDER BY id;")));
	CHECK_EQUAL(0, results.Error.length());
	CHECK(results.Success);
	CHECK_EQUAL(3, results.AffectedRows);
	CHECK_EQUAL((size_t)3, results.StringResults.size());
	CHECK_EQUAL(0, error.length());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("1"), results.StringResults[0][0]);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("one"), results.StringResults[0][1]);
	
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("2"), results.StringResults[1][0]);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("two"), results.StringResults[1][1]);

	CHECK_EQUAL(UNICODE_STRING_SIMPLE("3"), results.StringResults[2][0]);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("three"), results.StringResults[2][1]);
}

TEST_FIXTURE(DatabaseInfoTestFixtureClass, MultipleQueries) {
	Exec("CREATE TABLE names (id INT, name VARCHAR(255));");
	Exec("CREATE TABLE places (id INT, name VARCHAR(255));");
	Exec("INSERT INTO names(id, name) VALUES(1, 'one')");
	Exec("INSERT INTO names(id, name) VALUES(2, 'two')");
	Exec("INSERT INTO names(id, name) VALUES(3, 'three')");
	
	soci::session session;
	mvceditor::SqlQueryClass query;
	UnicodeString error;
	
	query.Info.Copy(Info);
	CHECK(query.Connect(session, error));
	CHECK_EQUAL(0, error.length());
	mvceditor::SqlResultClass results;
	
	CHECK(query.Execute(session, results, UNICODE_STRING_SIMPLE("DELETE FROM places;")));
	CHECK_EQUAL(0, results.Error.length());
	CHECK(results.Success);
	CHECK_EQUAL(false, results.HasRows);
	results.Close();
	
	mvceditor::SqlResultClass results2;
	CHECK(query.Execute(session, results2, UNICODE_STRING_SIMPLE("SELECT * FROM names ORDER BY id;")));
	CHECK_EQUAL(0, results2.Error.length());
	CHECK(results2.Success);
	CHECK_EQUAL((size_t)3, results2.StringResults.size());
	CHECK_EQUAL(3, results2.AffectedRows);
	results2.Close();
	
	mvceditor::SqlResultClass results3;
	CHECK(query.Execute(session, results3, UNICODE_STRING_SIMPLE("DELETE FROM names;")));
	CHECK_EQUAL(0, results3.Error.length());
	CHECK(results3.Success);
	CHECK_EQUAL(false, results3.HasRows);
	results3.Close();
}

}