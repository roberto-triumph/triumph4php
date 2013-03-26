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
#include <UnitTest++.h>
#include <MvcEditorChecks.h>
#include <ActionTestFixtureClass.h>
#include <FileTestFixtureClass.h>
#include <actions/ProjectTagActionClass.h>
#include <language/TagCacheClass.h>
#include <globals/Assets.h>
#include <wx/event.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

static int ID_EVENT = wxNewId();

#define MAKE_FILENAME(output, rootPath, dir1)\
	output = rootPath;\
	output += wxFileName::GetPathSeparator();\
	output += dir1;


class ProjectTagActionTestClass : public ActionTestFixtureClass, public FileTestFixtureClass {

public:

	/**
	 * the object under test. we will assert that the object generates
	 * events.
	 */
	mvceditor::ProjectTagActionClass ProjectTagAction;
	
	/**
	 * the results to check; these pointers com from the
	 * events. This class will owne the pointers.
	 * Therer should be one pointer per each scanned project.
	 */
	std::vector<mvceditor::GlobalCacheClass*> GlobalCaches;

	ProjectTagActionTestClass() 
		: ActionTestFixtureClass()
		, FileTestFixtureClass(wxT("resource_file_reader"))
		, ProjectTagAction(RunningThreads, ID_EVENT) 
		, GlobalCaches() {
		TouchTestDir();
		CreateProject(AbsoluteDir(wxT("src_project_1")), TestProjectDir);
		soci::session session(*soci::factory_sqlite3(), mvceditor::WxToChar(Globals.Projects[0].ResourceDbFileName.GetFullPath()));
		wxString error;
		mvceditor::SqliteSqlScript(mvceditor::ResourceSqlSchemaAsset(), session, error);
	}

	~ProjectTagActionTestClass() {
		std::vector<mvceditor::GlobalCacheClass*>::iterator cache;
		for (cache = GlobalCaches.begin(); cache != GlobalCaches.end(); ++cache) {
			delete (*cache);
		}
	}

	void OnGlobalCacheComplete(mvceditor::GlobalCacheCompleteEventClass& event) {
		GlobalCaches.push_back(event.GlobalCache);
	}

	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(ProjectTagActionTestClass, ActionTestFixtureClass) 
	EVT_GLOBAL_CACHE_COMPLETE(ID_EVENT, ProjectTagActionTestClass::OnGlobalCacheComplete)
END_EVENT_TABLE()

SUITE(ProjectTagActionTestClass) {

TEST_FIXTURE(ProjectTagActionTestClass, InitProject) {
	// test that when we intialize 1 project we get 1
	// global cache pointer

	wxString srcFile;
	MAKE_FILENAME(srcFile, wxT("src_project_1"), wxT("User.php"));
	CreateSubDirectory(wxT("src_project_1"));

	wxString contents = wxT("<?php class User {}");
	CreateFixtureFile(srcFile, contents);

	CHECK(ProjectTagAction.Init(Globals));

	ProjectTagAction.BackgroundWork();

	CHECK_VECTOR_SIZE(1, GlobalCaches);
	CHECK(NULL != GlobalCaches[0]);
	mvceditor::TagSearchClass search(UNICODE_STRING_SIMPLE("User"));
	CHECK_VECTOR_SIZE(1, GlobalCaches[0]->TagFinder->ExactTags(search));
}

TEST_FIXTURE(ProjectTagActionTestClass, InitMultipleProjects) {
	// test that when we intialize 2 project we get 2
	// global cache pointers

	CreateSubDirectory(wxT("src_project_1"));
	wxString srcFileProject1;
	MAKE_FILENAME(srcFileProject1, wxT("src_project_1"), wxT("User.php"));
	wxString contents = wxT("<?php class User {}");
	CreateFixtureFile(srcFileProject1, contents);

	CreateProject(AbsoluteDir(wxT("src_project_2")), TestProjectDir);
	CreateSubDirectory(wxT("src_project_2"));
	wxString srcFileProject2;
	MAKE_FILENAME(srcFileProject2, wxT("src_project_2"), wxT("Role.php"));
	contents = wxT("<?php class Role {}");
	CreateFixtureFile(srcFileProject2, contents);
	soci::session session;
	session.open(*soci::factory_sqlite3(), mvceditor::WxToChar(Globals.Projects[1].ResourceDbFileName.GetFullPath()));
	wxString error;
	mvceditor::SqliteSqlScript(mvceditor::ResourceSqlSchemaAsset(), session, error);


	CHECK(ProjectTagAction.Init(Globals));

	ProjectTagAction.BackgroundWork();

	CHECK_VECTOR_SIZE(2, GlobalCaches);
	CHECK(NULL != GlobalCaches[0]);
	CHECK(NULL != GlobalCaches[1]);

	mvceditor::TagSearchClass searchFirst(UNICODE_STRING_SIMPLE("User"));
	CHECK_VECTOR_SIZE(1, GlobalCaches[0]->TagFinder->ExactTags(searchFirst));

	mvceditor::TagSearchClass searchSecond(UNICODE_STRING_SIMPLE("Role"));
	CHECK_VECTOR_SIZE(1, GlobalCaches[1]->TagFinder->ExactTags(searchSecond));
}

}