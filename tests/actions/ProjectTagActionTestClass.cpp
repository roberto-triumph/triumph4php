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
	 * connection to the tag db
	 */
	soci::session Session;

	/**
	 * will use this to lookup tags once parsing is complete
	 */
	mvceditor::ParsedTagFinderClass Finder;

	ProjectTagActionTestClass() 
		: ActionTestFixtureClass()
		, FileTestFixtureClass(wxT("resource_file_reader"))
		, ProjectTagAction(RunningThreads, ID_EVENT)
		, Session()
		, Finder() {
		TouchTestDir();
		InitTagCache(TestProjectDir);
		CreateProject(AbsoluteDir(wxT("src_project_1")));
		Session.open(*soci::factory_sqlite3(), mvceditor::WxToChar(Globals.TagCacheDbFileName.GetFullPath()));
		wxString error;
		bool init = mvceditor::SqliteSqlScript(mvceditor::ResourceSqlSchemaAsset(), Session, error);
		wxASSERT_MSG(init, error);
		Finder.Init(&Session);
	}
};


SUITE(ProjectTagActionTestClass) {

TEST_FIXTURE(ProjectTagActionTestClass, InitProject) {
	// test that when we intialize 1 project it gets parsed

	wxString srcFile;
	MAKE_FILENAME(srcFile, wxT("src_project_1"), wxT("User.php"));
	CreateSubDirectory(wxT("src_project_1"));

	wxString contents = wxT("<?php class User {}");
	CreateFixtureFile(srcFile, contents);

	CHECK(ProjectTagAction.Init(Globals));

	ProjectTagAction.BackgroundWork();

	mvceditor::TagSearchClass search(UNICODE_STRING_SIMPLE("User"));
	CHECK_VECTOR_SIZE(1, Finder.ExactTags(search));
}

TEST_FIXTURE(ProjectTagActionTestClass, InitMultipleProjects) {
	// test that when we intialize 2 projects both projects are parsed

	CreateSubDirectory(wxT("src_project_1"));
	wxString srcFileProject1;
	MAKE_FILENAME(srcFileProject1, wxT("src_project_1"), wxT("User.php"));
	wxString contents = wxT("<?php class User {}");
	CreateFixtureFile(srcFileProject1, contents);

	CreateProject(AbsoluteDir(wxT("src_project_2")));
	CreateSubDirectory(wxT("src_project_2"));
	wxString srcFileProject2;
	MAKE_FILENAME(srcFileProject2, wxT("src_project_2"), wxT("Role.php"));
	contents = wxT("<?php class Role {}");
	CreateFixtureFile(srcFileProject2, contents);
	
	CHECK(ProjectTagAction.Init(Globals));

	ProjectTagAction.BackgroundWork();

	mvceditor::TagSearchClass searchFirst(UNICODE_STRING_SIMPLE("User"));
	CHECK_VECTOR_SIZE(1, Finder.ExactTags(searchFirst));

	mvceditor::TagSearchClass searchSecond(UNICODE_STRING_SIMPLE("Role"));
	CHECK_VECTOR_SIZE(1, Finder.ExactTags(searchSecond));
}

}