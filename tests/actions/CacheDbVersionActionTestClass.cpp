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
#include <FileTestFixtureClass.h>
#include <ActionTestFixtureClass.h>
#include <actions/CacheDbVersionActionClass.h>
#include <globals/Sqlite.h>
#include <globals/String.h>
#include <globals/Assets.h>
#include <UnitTest++.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

class CacheDbVersionActionFixtureClass : public ActionTestFixtureClass, public FileTestFixtureClass {
	public:
	t4p::TagCacheDbVersionActionClass Action;
	t4p::DetectorCacheDbVersionActionClass DetectorCacheAction;

	CacheDbVersionActionFixtureClass()
		: ActionTestFixtureClass()
		, FileTestFixtureClass(wxT("cache_db_version"))
		, Action(RunningThreads, wxID_ANY)
		, DetectorCacheAction(RunningThreads, wxID_ANY) {
		// need to make sure the directory exists
		TouchTestDir();
		InitTagCache(TestProjectDir);
	}
};

SUITE(CacheDbVersionActionTestClass) {
TEST_FIXTURE(CacheDbVersionActionFixtureClass, EmptyCacheDbFiles) {
	CreateProject(AbsoluteDir(wxT("project_1")));

	CHECK(Action.Init(Globals));
	Action.BackgroundWork();
	CHECK(Globals.TagCacheDbFileName.FileExists());

	soci::session session(*soci::factory_sqlite3(), t4p::WxToChar(Globals.TagCacheDbFileName.GetFullPath()));
	CHECK(t4p::SqliteSchemaVersion(session) > 0);
}

TEST_FIXTURE(CacheDbVersionActionFixtureClass, ExistingCacheDbFiles) {
	wxString error;
	CreateProject(AbsoluteDir(wxT("project_1")));

	// create the db files
	soci::session session(*soci::factory_sqlite3(), t4p::WxToChar(Globals.TagCacheDbFileName.GetFullPath()));
	t4p::SqliteSqlScript(t4p::ResourceSqlSchemaAsset(), session, error);

	CHECK(Action.Init(Globals));
	Action.BackgroundWork();
	CHECK(Globals.TagCacheDbFileName.FileExists());

	CHECK(t4p::SqliteSchemaVersion(session) > 0);
}

TEST_FIXTURE(CacheDbVersionActionFixtureClass, OldCacheDbFiles) {
	wxString error;
	CreateProject(AbsoluteDir(wxT("project_1")));

	// create the db files
	soci::session session(*soci::factory_sqlite3(), t4p::WxToChar(Globals.TagCacheDbFileName.GetFullPath()));
	t4p::SqliteSqlScript(t4p::ResourceSqlSchemaAsset(), session, error);

	// set the version to be an old one
	session.once << "DELETE FROM schema_version;";
	session.once << "INSERT INTO schema_version (version_number) VALUES (0);";
	CHECK(t4p::SqliteSchemaVersion(session) == 0);

	CHECK(Action.Init(Globals));
	Action.BackgroundWork();
	CHECK(Globals.TagCacheDbFileName.FileExists());

	// not sure why I have to close the connection in order for the test to work
	session.close();
	session.open(*soci::factory_sqlite3(), t4p::WxToChar(Globals.TagCacheDbFileName.GetFullPath()));
	CHECK(t4p::SqliteSchemaVersion(session) > 0);
}

TEST_FIXTURE(CacheDbVersionActionFixtureClass, DetectorEmptyCacheDbFiles) {
	CreateProject(AbsoluteDir(wxT("project_1")));

	CHECK(DetectorCacheAction.Init(Globals));
	DetectorCacheAction.BackgroundWork();
	CHECK(Globals.DetectorCacheDbFileName.FileExists());

	soci::session session(*soci::factory_sqlite3(), t4p::WxToChar(Globals.DetectorCacheDbFileName.GetFullPath()));
	CHECK(t4p::SqliteSchemaVersion(session) > 0);
}

TEST_FIXTURE(CacheDbVersionActionFixtureClass, DetectorExistingCacheDbFiles) {
	wxString error;
	CreateProject(AbsoluteDir(wxT("project_1")));

	// create the db files
	soci::session session(*soci::factory_sqlite3(), t4p::WxToChar(Globals.DetectorCacheDbFileName.GetFullPath()));
	t4p::SqliteSqlScript(t4p::DetectorSqlSchemaAsset(), session, error);

	CHECK(DetectorCacheAction.Init(Globals));
	DetectorCacheAction.BackgroundWork();
	CHECK(Globals.DetectorCacheDbFileName.FileExists());

	CHECK(t4p::SqliteSchemaVersion(session) > 0);
}

TEST_FIXTURE(CacheDbVersionActionFixtureClass, DetectorOldCacheDbFiles) {
	wxString error;
	CreateProject(AbsoluteDir(wxT("project_1")));

	// create the db files
	soci::session session(*soci::factory_sqlite3(), t4p::WxToChar(Globals.DetectorCacheDbFileName.GetFullPath()));
	t4p::SqliteSqlScript(t4p::DetectorSqlSchemaAsset(), session, error);

	// set the version to be an old one
	session.once << "DELETE FROM schema_version;";
	session.once << "INSERT INTO schema_version (version_number) VALUES (0);";
	CHECK(t4p::SqliteSchemaVersion(session) == 0);

	CHECK(DetectorCacheAction.Init(Globals));
	DetectorCacheAction.BackgroundWork();
	CHECK(Globals.DetectorCacheDbFileName.FileExists());

	// not sure why I have to close the connection in order for the test to work
	session.close();
	session.open(*soci::factory_sqlite3(), t4p::WxToChar(Globals.DetectorCacheDbFileName.GetFullPath()));
	CHECK(t4p::SqliteSchemaVersion(session) > 0);
}
}

