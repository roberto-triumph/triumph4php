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
#include <globals/UrlTagClass.h>
#include <globals/String.h>
#include <globals/Assets.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <MvcEditorChecks.h>
#include <string>

class UrlTagFixtureClass : public SqliteTestFixtureClass {

public:

	// using these session as opposed to the inherited one because
	// the UrlTagFinder owns the session pointers
	soci::session* Session1;
	soci::session* Session2;

	mvceditor::UrlTagFinderClass Finder;

	UrlTagFixtureClass()
		: SqliteTestFixtureClass()
		, Session1(NULL)
		, Session2(NULL) 
		, Finder() {
	
		Session1 = new soci::session(*soci::factory_sqlite3(), ":memory:");
		CreateDatabase(*Session1, mvceditor::DetectorSqlSchemaAsset());
		Finder.AdoptSession(Session1);
		
		AddToDb1("http://localhost/index.php", 
			"/home/user/welcome.php", "WelcomeController", "index");
		AddToDb1("http://localhost/frontend.php",
			"/home/user/frontend.php", "FrontendController", "action");

	}

	~UrlTagFixtureClass() {
		// sessions are owned by the UrlFinder
	}

	void AddToDb1(std::string url, std::string fileName, std::string className, std::string methodName) {
		soci::statement stmt = (Session1->prepare <<
			"INSERT INTO url_tags(url, full_path, class_name, method_name) VALUES (?, ?, ?, ?)",
			soci::use(url), soci::use(fileName),
			soci::use(className), soci::use(methodName)
		);
		stmt.execute(true);
	}

	void InitDb2() {
		Session2 = new soci::session(*soci::factory_sqlite3(), ":memory:");
		CreateDatabase(*Session2, mvceditor::DetectorSqlSchemaAsset());
		Finder.AdoptSession(Session2);
	}

	void AddToDb2(std::string url, std::string fileName, std::string className, std::string methodName) {
		soci::statement stmt = (Session2->prepare <<
			"INSERT INTO url_tags(url, full_path, class_name, method_name) VALUES (?, ?, ?, ?)",
			soci::use(url), soci::use(fileName),
			soci::use(className), soci::use(methodName)
		);
		stmt.execute(true);
	}

	int DatabaseRecordsNumDb1() {
		int cnt;
		Session1->once << "SELECT COUNT(*) FROM url_tags; ",
			soci::into(cnt);
		return cnt;
	}

	int DatabaseRecordsNumDb2() {
		int cnt;
		Session2->once << "SELECT COUNT(*) FROM url_tags; ",
			soci::into(cnt);
		return cnt;
	}

};

SUITE(UrlTagTestClass) {

TEST_FIXTURE(UrlTagFixtureClass, FindByUrlMatch) {
	CHECK_EQUAL(2, DatabaseRecordsNumDb1());

	wxURI toFind(wxT("http://localhost/frontend.php"));
	mvceditor::UrlTagClass urlTag;
	CHECK(Finder.FindByUrl(toFind, urlTag));
	CHECK(toFind == urlTag.Url);
	CHECK_EQUAL(wxT("http://localhost/frontend.php"), urlTag.Url.BuildURI());
	
	// use filename to compare because we want this test to run on windows and on
	// linux without needing modifications
	CHECK(wxFileName(wxT("/home/user/frontend.php")) == urlTag.FileName);
	CHECK_EQUAL(wxT("FrontendController"), urlTag.ClassName);
	CHECK_EQUAL(wxT("action"), urlTag.MethodName);

	toFind.Create(wxT("http://localhost/index.php"));
	CHECK(Finder.FindByUrl(toFind, urlTag));
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
	mvceditor::UrlTagClass urlTag;
	CHECK_EQUAL(false, Finder.FindByUrl(toFind, urlTag));
	CHECK(urlTag.Url.BuildURI().IsEmpty());
}

TEST_FIXTURE(UrlTagFixtureClass, FindUrlWithMultipleDbs) {
	InitDb2();
	AddToDb2("http://localhost/frontend_dev.php",
			"/home/user/project2/frontend.php", "FrontendController", "action");

	// test the find across multiple dbs
	wxURI toFind;
	toFind.Create(wxT("http://localhost/frontend_dev.php"));
	mvceditor::UrlTagClass urlTag;

	CHECK(Finder.FindByUrl(toFind, urlTag));
	
	CHECK(toFind == urlTag.Url);

	// use filename to compare because we want this test to run on windows and on
	// linux without needing modifications
	CHECK(wxFileName(wxT("/home/user/project2/frontend.php")) == urlTag.FileName);
	CHECK_EQUAL(wxT("FrontendController"), urlTag.ClassName);
	CHECK_EQUAL(wxT("action"), urlTag.MethodName);
}

TEST_FIXTURE(UrlTagFixtureClass, FilterUrl) {
	std::vector<mvceditor::UrlTagClass> urls;
	Finder.FilterUrls(wxT("front"), urls);
	CHECK_VECTOR_SIZE(1, urls);
	CHECK_EQUAL(wxT("http://localhost/frontend.php"), urls[0].Url.BuildURI());
}

TEST_FIXTURE(UrlTagFixtureClass, FilterUrlWithMultipleDbs) {
	InitDb2();
	AddToDb2("http://localhost/frontend_dev.php",
			"/home/user/project2/frontend.php", "FrontendController", "action");

	// test the filter across multiple dbs
	std::vector<mvceditor::UrlTagClass> urls;
	Finder.FilterUrls(wxT("front"), urls);
	CHECK_VECTOR_SIZE(2, urls);
	CHECK_EQUAL(wxT("http://localhost/frontend.php"), urls[0].Url.BuildURI());
	CHECK_EQUAL(wxT("http://localhost/frontend_dev.php"), urls[1].Url.BuildURI());
}

TEST_FIXTURE(UrlTagFixtureClass, FilterUrlNoMatches) {
	std::vector<mvceditor::UrlTagClass> urls;
	Finder.FilterUrls(wxT("back"), urls);
	CHECK_VECTOR_SIZE(0, urls);
}

TEST_FIXTURE(UrlTagFixtureClass, DeleteUrlMatch) {
	wxURI toDelete(wxT("http://localhost/frontend.php"));
	Finder.DeleteUrl(toDelete);
	CHECK_EQUAL(1, DatabaseRecordsNumDb1());
}

TEST_FIXTURE(UrlTagFixtureClass, DeleteUrlNoMatch) {
	wxURI toDelete(wxT("http://localhost/backend.php"));
	Finder.DeleteUrl(toDelete);
	CHECK_EQUAL(2, DatabaseRecordsNumDb1());
}

TEST_FIXTURE(UrlTagFixtureClass, WipeAcrossMultipleDbs) {
	InitDb2();
	AddToDb2("http://localhost/frontend_dev.php",
			"/home/user/project2/frontend.php", "FrontendController", "action");

	Finder.Wipe();
	CHECK_EQUAL(0, DatabaseRecordsNumDb1());
	CHECK_EQUAL(0, DatabaseRecordsNumDb2());
}

}