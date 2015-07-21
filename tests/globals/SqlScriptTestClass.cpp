/**
 * @copyright  2013 Roberto Perpuly
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
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <string>
#include <vector>
#include "FileTestFixtureClass.h"
#include "globals/Assets.h"
#include "globals/Sqlite.h"
#include "globals/String.h"
#include "SqliteTestFixtureClass.h"
#include "TriumphChecks.h"

class SqliteFixtureClass : public FileTestFixtureClass, public SqliteTestFixtureClass {
 public:
    /**
     * The full path to a file that will contain an SQL script
     */
    wxFileName ScriptFileName;

    /**
     * connection to sqlite db
     */
    soci::session EmptySession;

    SqliteFixtureClass()
        : FileTestFixtureClass(wxT("sqlite"))
        , SqliteTestFixtureClass(t4p::ResourceSqlSchemaAsset())
        , EmptySession(*soci::factory_sqlite3(), ":memory:") {
        ScriptFileName.Assign(TestProjectDir, wxT("script.sql"));
    }
};

SUITE(SqliteTestClass) {
    TEST_FIXTURE(SqliteFixtureClass, SqlScriptWithNewFile) {
        CreateFixtureFile(wxT("script.sql"), t4p::CharToWx(
                              "CREATE TABLE my_table ( id INT, name VARCHAR(255));"
                              "CREATE TABLE your_table ( id INT, your_name VARCHAR(255));"));

        wxString error;
        bool ret = t4p::SqliteSqlScript(ScriptFileName, EmptySession, error);
        CHECK(ret);
        std::vector<std::string> tableNames;
        CHECK(t4p::SqliteTables(EmptySession, tableNames, error));
        CHECK_VECTOR_SIZE(2, tableNames);
        CHECK_EQUAL("my_table", tableNames[0]);
        CHECK_EQUAL("your_table", tableNames[1]);
    }

    TEST_FIXTURE(SqliteFixtureClass, SqlScriptWithExistingFile) {
        CreateFixtureFile(wxT("script.sql"), t4p::CharToWx(
                              "CREATE TABLE my_table ( id INT, name VARCHAR(255));"
                              "CREATE TABLE your_table ( id INT, your_name VARCHAR(255));"));
        wxString error;
        bool ret = t4p::SqliteSqlScript(ScriptFileName, EmptySession, error);
        CHECK(ret);

        CreateFixtureFile(wxT("script.sql"), t4p::CharToWx(
                              "CREATE TABLE others_table ( id INT, othername VARCHAR(255));"
                              "CREATE TABLE another_table ( id INT, another_name VARCHAR(255));"
                              "CREATE TABLE yet_another_table ( id INT, yet_another_name VARCHAR(255));"));

        ret = t4p::SqliteSqlScript(ScriptFileName, EmptySession, error);
        CHECK(ret);
        std::vector<std::string> tableNames;
        CHECK(t4p::SqliteTables(EmptySession, tableNames, error));
        CHECK_VECTOR_SIZE(3, tableNames);
        CHECK_EQUAL("others_table", tableNames[0]);
        CHECK_EQUAL("another_table", tableNames[1]);
        CHECK_EQUAL("yet_another_table", tableNames[2]);
    }

    TEST_FIXTURE(SqliteFixtureClass, SchemaVersion) {
        CreateFixtureFile(wxT("script.sql"), t4p::CharToWx(
                              "CREATE TABLE schema_version ( version_number INT);"
                              "INSERT INTO schema_version VALUES(2);"));
        wxString error;
        bool ret = t4p::SqliteSqlScript(ScriptFileName, EmptySession, error);
        CHECK(ret);
        int versionNumber = t4p::SqliteSchemaVersion(EmptySession);
        CHECK_EQUAL(2, versionNumber);
    }
}
