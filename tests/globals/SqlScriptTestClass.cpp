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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <UnitTest++.h>
#include <FileTestFixtureClass.h>
#include <globals/Sqlite.h>
#include <globals/String.h>
#include <globals/Assets.h>
#include <MvcEditorChecks.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

class SqliteFixtureClass : public FileTestFixtureClass {

public:

	/**
	 * The full path to the sqlite db
	 */
	wxFileName DbFileName;

	/**
	 * The full path to a file that will contain an SQL script
	 */
	wxFileName ScriptFileName;

	/**
	 * connection to sqlite db
	 */
	soci::session Session;

	SqliteFixtureClass()
		: FileTestFixtureClass(wxT("sqlite"))
		, Session() {
		DbFileName.Assign(TestProjectDir, wxT("sqlite.db"));
		ScriptFileName.Assign(TestProjectDir, wxT("script.sql"));
	}
};

SUITE(SqliteTestClass) {

TEST_FIXTURE(SqliteFixtureClass, SqlScriptWithNewFile) {
	CreateFixtureFile(wxT("script.sql"), mvceditor::CharToWx(
		"CREATE TABLE my_table ( id INT, name VARCHAR(255));"	
		"CREATE TABLE your_table ( id INT, your_name VARCHAR(255));"	
	));
	Session.open(*soci::factory_sqlite3(), mvceditor::WxToChar(DbFileName.GetFullPath()));
	wxString error;
	bool ret = mvceditor::SqliteSqlScript(ScriptFileName, Session, error);
	CHECK(ret);
	std::vector<std::string> tableNames;
	CHECK(mvceditor::SqliteTables(Session, tableNames, error));
	CHECK_VECTOR_SIZE(2, tableNames);
	CHECK_EQUAL("my_table", tableNames[0]);
	CHECK_EQUAL("your_table", tableNames[1]);
}

TEST_FIXTURE(SqliteFixtureClass, SqlScriptWithExistingFile) {
	CreateFixtureFile(wxT("script.sql"), mvceditor::CharToWx(
		"CREATE TABLE my_table ( id INT, name VARCHAR(255));"	
		"CREATE TABLE your_table ( id INT, your_name VARCHAR(255));"	
	));
	Session.open(*soci::factory_sqlite3(), mvceditor::WxToChar(DbFileName.GetFullPath()));
	wxString error;
	bool ret = mvceditor::SqliteSqlScript(ScriptFileName, Session, error);
	CHECK(ret);

	CreateFixtureFile(wxT("script.sql"), mvceditor::CharToWx(
		"CREATE TABLE others_table ( id INT, othername VARCHAR(255));"	
		"CREATE TABLE another_table ( id INT, another_name VARCHAR(255));"	
		"CREATE TABLE yet_another_table ( id INT, yet_another_name VARCHAR(255));"	
	));
	Session.close();
	Session.open(*soci::factory_sqlite3(), mvceditor::WxToChar(DbFileName.GetFullPath()));
	ret = mvceditor::SqliteSqlScript(ScriptFileName, Session, error);
	CHECK(ret);
	std::vector<std::string> tableNames;
	CHECK(mvceditor::SqliteTables(Session, tableNames, error));
	CHECK_VECTOR_SIZE(3, tableNames);
	CHECK_EQUAL("others_table", tableNames[0]);
	CHECK_EQUAL("another_table", tableNames[1]);
	CHECK_EQUAL("yet_another_table", tableNames[2]);

}

TEST_FIXTURE(SqliteFixtureClass, SchemaVersion) {
	CreateFixtureFile(wxT("script.sql"), mvceditor::CharToWx(
		"CREATE TABLE schema_version ( version_number INT);"	
		"INSERT INTO schema_version VALUES(2);"	
	));
	Session.open(*soci::factory_sqlite3(), mvceditor::WxToChar(DbFileName.GetFullPath()));
	wxString error;
	bool ret = mvceditor::SqliteSqlScript(ScriptFileName, Session, error);
	CHECK(ret);
	int versionNumber = mvceditor::SqliteSchemaVersion(Session);
	CHECK_EQUAL(2, versionNumber);
}

}