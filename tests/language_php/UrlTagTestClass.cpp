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
#include <SqliteTestFixtureClass.h>
#include <language_php/UrlTagClass.h>
#include <globals/String.h>
#include <globals/Assets.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <wx/stdpaths.h>
#include <TriumphChecks.h>
#include <string>

class UrlTagFixtureClass : public SqliteTestFixtureClass {

public:

	t4p::UrlTagFinderClass Finder;
	soci::session DetectorTagSession;
	std::vector<wxFileName> SourceDirs;
	int SourceId;

	UrlTagFixtureClass()
		: SqliteTestFixtureClass(t4p::ResourceSqlSchemaAsset())
		, Finder(DetectorTagSession)
		, SourceDirs()
		, SourceId(0) {
		DetectorTagSession.open(*soci::factory_sqlite3(), ":memory:");
		CreateDatabase(DetectorTagSession, t4p::DetectorSqlSchemaAsset());

		wxFileName tmpDir;
		tmpDir.AssignDir(wxStandardPaths::Get().GetTempDir());
		SourceDirs.push_back(tmpDir);
		std::string stdDir = t4p::WxToChar(tmpDir.GetPathWithSep());

		// create the source
		soci::statement stmt = (DetectorTagSession.prepare << "INSERT INTO sources(directory) VALUES(?)",
			soci::use(stdDir));
		stmt.execute(true);
		soci::sqlite3_statement_backend* backend = static_cast<soci::sqlite3_statement_backend*>(stmt.get_backend());
		SourceId = sqlite3_last_insert_rowid(backend->session_.conn_);

		AddToDb1("http://localhost/index.php",
			"/home/user/welcome.php", "WelcomeController", "index");
		AddToDb1("http://localhost/frontend.php",
			"/home/user/frontend.php", "FrontendController", "action");

	}

	~UrlTagFixtureClass() {
		// sessions are owned by the UrlFinder
	}

	void AddToDb1(std::string url, std::string fileName, std::string className, std::string methodName) {
		soci::statement stmt = (DetectorTagSession.prepare <<
			"INSERT INTO url_tags(url, source_id, full_path, class_name, method_name) VALUES (?, ?, ?, ?, ?)",
			soci::use(url), soci::use(SourceId), soci::use(fileName),
			soci::use(className), soci::use(methodName)
		);
		stmt.execute(true);
	}

	int DatabaseRecordsNumDb1() {
		int cnt;
		DetectorTagSession.once << "SELECT COUNT(*) FROM url_tags; ",
			soci::into(cnt);
		return cnt;
	}
};

SUITE(UrlTagTestClass) {

TEST_FIXTURE(UrlTagFixtureClass, FindByUrlMatch) {
	CHECK_EQUAL(2, DatabaseRecordsNumDb1());

	wxURI toFind(wxT("http://localhost/frontend.php"));
	t4p::UrlTagClass urlTag;
	CHECK(Finder.FindByUrl(toFind, SourceDirs, urlTag));
	CHECK(toFind == urlTag.Url);
	CHECK_EQUAL(wxT("http://localhost/frontend.php"), urlTag.Url.BuildURI());

	// use filename to compare because we want this test to run on windows and on
	// linux without needing modifications
	CHECK(wxFileName(wxT("/home/user/frontend.php")) == urlTag.FileName);
	CHECK_EQUAL(wxT("FrontendController"), urlTag.ClassName);
	CHECK_EQUAL(wxT("action"), urlTag.MethodName);

	toFind.Create(wxT("http://localhost/index.php"));
	CHECK(Finder.FindByUrl(toFind, SourceDirs, urlTag));
	CHECK(toFind == urlTag.Url);
	CHECK_EQUAL(wxT("http://localhost/index.php"), urlTag.Url.BuildURI());

	// use filename to compare because we want this test to run on windows and on
	// linux without needing modifications
	CHECK(wxFileName(wxT("/home/user/welcome.php")) == urlTag.FileName);
	CHECK_EQUAL(wxT("WelcomeController"), urlTag.ClassName);
	CHECK_EQUAL(wxT("index"), urlTag.MethodName);
}

TEST_FIXTURE(UrlTagFixtureClass, FindByUrlNoMatch) {
	wxURI toFind(wxT("http://localhost/backend.php"));
	t4p::UrlTagClass urlTag;
	CHECK_EQUAL(false, Finder.FindByUrl(toFind, SourceDirs, urlTag));
	CHECK(urlTag.Url.BuildURI().IsEmpty());
}

TEST_FIXTURE(UrlTagFixtureClass, FilterUrl) {
	std::vector<t4p::UrlTagClass> urls;
	Finder.FilterUrls(wxT("front"), SourceDirs, urls);
	CHECK_VECTOR_SIZE(1, urls);
	CHECK_EQUAL(wxT("http://localhost/frontend.php"), urls[0].Url.BuildURI());
}

TEST_FIXTURE(UrlTagFixtureClass, FilterUrlNoMatches) {
	std::vector<t4p::UrlTagClass> urls;
	Finder.FilterUrls(wxT("back"), SourceDirs, urls);
	CHECK_VECTOR_SIZE(0, urls);
}

TEST_FIXTURE(UrlTagFixtureClass, DeleteUrlMatch) {
	wxURI toDelete(wxT("http://localhost/frontend.php"));
	Finder.DeleteUrl(toDelete, SourceDirs);
	CHECK_EQUAL(1, DatabaseRecordsNumDb1());
}

TEST_FIXTURE(UrlTagFixtureClass, DeleteUrlNoMatch) {
	wxURI toDelete(wxT("http://localhost/backend.php"));
	Finder.DeleteUrl(toDelete, SourceDirs);
	CHECK_EQUAL(2, DatabaseRecordsNumDb1());
}

TEST_FIXTURE(UrlTagFixtureClass, Wipe) {
	Finder.Wipe(SourceDirs);
	CHECK_EQUAL(0, DatabaseRecordsNumDb1());
}

}
