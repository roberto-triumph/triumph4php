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
#include <SqliteTestFixtureClass.h>
#include <language_php/DetectorDbClass.h>
#include <globals/Assets.h>
#include <globals/Sqlite.h>
#include <globals/String.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <wx/stdpaths.h>

class DetectorDbTestFixtureClass : public SqliteTestFixtureClass {

	public:

	soci::session DetectorTagSession;
	t4p::DetectorDbClass DetectorDb;
	wxFileName Source1,
		Source2;

	DetectorDbTestFixtureClass()
		: SqliteTestFixtureClass(t4p::ResourceSqlSchemaAsset())
		, DetectorTagSession()
		, DetectorDb()
		, Source1()
		, Source2() {

		DetectorTagSession.open(*soci::factory_sqlite3(), ":memory:");
		CreateDatabase(DetectorTagSession, t4p::DetectorSqlSchemaAsset());

		DetectorDb.Init(&DetectorTagSession);

		wxStandardPaths paths = wxStandardPaths::Get();
		wxString tmpPath = paths.GetTempDir();
		wxFileName tmpDir;
		tmpDir.AssignDir(tmpPath);

		Source1.AssignDir(tmpDir.GetPathWithSep() + wxT("source_one"));
		Source2.AssignDir(tmpDir.GetPathWithSep() + wxT("source_two"));

		int sourceId1 = AddSource(Source1);
		AddCallStack(sourceId1, 1);
		AddDatabaseTag(sourceId1);
		AddTemplateFileTag(sourceId1);
		AddConfigTag(sourceId1);
		AddUrlTag(sourceId1);

		int sourceId2 = AddSource(Source2);
		AddCallStack(sourceId2, 2);
		AddDatabaseTag(sourceId2);
		AddTemplateFileTag(sourceId2);
		AddConfigTag(sourceId2);
		AddUrlTag(sourceId2);
	}


	void AddCallStack(int sourceId, int stepNumber) {
		DetectorTagSession.once
			<< "INSERT INTO call_stacks "
			<< "(source_id, step_number, step_type, expression) "
			<<	"VALUES (?, ?, 'BEGIN_FUNCTION', 'work')"
			, soci::use(sourceId)
			, soci::use(stepNumber);
	}

	void AddDatabaseTag(int sourceId) {
		DetectorTagSession.once
			<< "INSERT INTO database_tags "
			<< "(source_id, label, schema, driver, host, port, \"user\", \"password\") "
			<< "VALUES(?, 'label', 'schema', 'MYSQL', 'host', 3306, 'user', 'password') "
			, soci::use(sourceId);
	}

	void AddTemplateFileTag(int sourceId) {
		DetectorTagSession.once
			<< "INSERT INTO template_file_tags "
			<< "(source_id, full_path, variables) "
			<< "VALUES(?, 'full_path', 'variables')"
			, soci::use(sourceId);
	}

	void AddConfigTag(int sourceId) {
		DetectorTagSession.once
			<< "INSERT INTO config_tags "
			<< "(source_id, label, full_path) "
			<< "VALUES(?, 'label', 'full_path')"
			, soci::use(sourceId);
	}

	void AddUrlTag(int sourceId) {
		std::ostringstream stream;
		stream << "http://localhost/k" << sourceId;
		std::string url = stream.str();
		DetectorTagSession.once
			<< "INSERT INTO url_tags"
			<< "(source_id, url, full_path, class_name, method_name) "
			<< "VALUES(?, ?, 'full_path', 'class_name', 'method_name')"
			, soci::use(sourceId), soci::use(url);
	}

	int AddSource(const wxFileName& fileName) {
		int sourceId = 0;
		std::string stdDir = t4p::WxToChar(fileName.GetPathWithSep());
		soci::statement stmt = (DetectorTagSession.prepare << "INSERT INTO sources(directory) VALUES(?)",
			soci::use(stdDir));
		stmt.execute(true);
		soci::sqlite3_statement_backend* backend = static_cast<soci::sqlite3_statement_backend*>(stmt.get_backend());
		sourceId = sqlite3_last_insert_rowid(backend->session_.conn_);
		return sourceId;
	}


	int RowCount(const std::string& tableName) {
		int count = 0;
		DetectorTagSession << ("SELECT COUNT(*) FROM " + tableName), soci::into(count);
		return count;
	}
};

SUITE(DetectorDbTestClass) {

TEST_FIXTURE(DetectorDbTestFixtureClass, WipeAll) {
	int count;
	count = RowCount("sources");
	CHECK_EQUAL(2, count);
	count = RowCount("call_stacks");
	CHECK_EQUAL(2, count);
	count = RowCount("database_tags");
	CHECK_EQUAL(2, count);
	count = RowCount("template_file_tags");
	CHECK_EQUAL(2, count);
	count = RowCount("config_tags");
	CHECK_EQUAL(2, count);
	count = RowCount("url_tags");
	CHECK_EQUAL(2, count);

	DetectorDb.Wipe();

	count = RowCount("call_stacks");
	CHECK_EQUAL(0, count);
	count = RowCount("database_tags");
	CHECK_EQUAL(0, count);
	count = RowCount("template_file_tags");
	CHECK_EQUAL(0, count);
	count = RowCount("config_tags");
	CHECK_EQUAL(0, count);
	count = RowCount("url_tags");
	CHECK_EQUAL(0, count);
}

TEST_FIXTURE(DetectorDbTestFixtureClass, DeleteSource) {
	int count;

	DetectorDb.DeleteSource(Source2);

	count = RowCount("sources");
	CHECK_EQUAL(1, count);
	count = RowCount("call_stacks");
	CHECK_EQUAL(1, count);
	count = RowCount("database_tags");
	CHECK_EQUAL(1, count);
	count = RowCount("template_file_tags");
	CHECK_EQUAL(1, count);
	count = RowCount("config_tags");
	CHECK_EQUAL(1, count);
	count = RowCount("url_tags");
	CHECK_EQUAL(1, count);
}

}
