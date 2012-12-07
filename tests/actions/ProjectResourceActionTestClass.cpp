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
#include <actions/ProjectResourceActionClass.h>
#include <language/ResourceCacheClass.h>
#include <wx/event.h>

static int ID_EVENT = wxNewId();

#define MAKE_FILENAME(output, rootPath, dir1)\
	output = rootPath;\
	output += wxFileName::GetPathSeparator();\
	output += dir1;


class ProjectResourceActionTestClass : public ActionTestFixtureClass, public FileTestFixtureClass {

public:

	
	/**
	 * the object under test. we will assert that the object generates
	 * events.
	 */
	mvceditor::ProjectResourceActionClass ProjectResourceAction;

	/**
	 * Create projects to be scanned
	 */
	mvceditor::ProjectClass Project1, 
							Project2;

	mvceditor::GlobalsClass Globals;
	
	/**
	 * the results to check; these pointers com from the
	 * events. This class will owne the pointers.
	 * Therer should be one pointer per each scanned project.
	 */
	std::vector<mvceditor::GlobalCacheClass*> GlobalCaches;

	ProjectResourceActionTestClass() 
		: ActionTestFixtureClass()
		, FileTestFixtureClass(wxT("resource_file_reader"))
		, ProjectResourceAction(RunningThreads, ID_EVENT) 
		, Project1()
		, Project2()
		, Globals()
		, GlobalCaches() {		
		CreateSubDirectory(wxT("src_project_1"));
		CreateSubDirectory(wxT("src_project_2"));

		// set the project to the src directory
		AddSourceDir(Project1, wxT("src_project_1"));
		Project1.PhpFileExtensions.push_back(wxT("*.php"));

		AddSourceDir(Project2, wxT("src_project_2"));
		Project2.PhpFileExtensions.push_back(wxT("*.php"));
	}

	~ProjectResourceActionTestClass() {
		std::vector<mvceditor::GlobalCacheClass*>::iterator cache;
		for (cache = GlobalCaches.begin(); cache != GlobalCaches.end(); ++cache) {
			delete (*cache);
		}
	}

	void AddSourceDir(mvceditor::ProjectClass& project, const wxString& dir) {
		mvceditor::SourceClass srcProject;
		srcProject.RootDirectory.AssignDir(TestProjectDir);
		srcProject.RootDirectory.AppendDir(dir);
		project.AddSource(srcProject);
	}

	void OnGlobalCacheComplete(mvceditor::GlobalCacheCompleteEventClass& event) {
		GlobalCaches.push_back(event.GlobalCache);
	}

	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(ProjectResourceActionTestClass, ActionTestFixtureClass) 
	EVT_GLOBAL_CACHE_COMPLETE(ID_EVENT, ProjectResourceActionTestClass::OnGlobalCacheComplete)
END_EVENT_TABLE()

SUITE(ProjectResourceActionTestClass) {

TEST_FIXTURE(ProjectResourceActionTestClass, InitProject) {
	// test that when we intialize 1 project we get 1
	// global cache pointer

	wxString srcFile;
	MAKE_FILENAME(srcFile, wxT("src_project_1"), wxT("User.php"));

	wxString contents = wxT("<?php class User {}");
	Project1.Label = wxT("test project");
	CreateFixtureFile(srcFile, contents);

	Project1.ResourceDbFileName.Assign(TestProjectDir, wxT("cache_project_1.db"));
	Globals.Projects.push_back(Project1);

	CHECK(ProjectResourceAction.Init(Globals));

	ProjectResourceAction.BackgroundWork();

	CHECK_VECTOR_SIZE(1, GlobalCaches);
	CHECK(NULL != GlobalCaches[0]);
	mvceditor::ResourceSearchClass search(UNICODE_STRING_SIMPLE("User"));
	CHECK_VECTOR_SIZE(1, GlobalCaches[0]->ResourceFinder.CollectFullyQualifiedResource(search));
}

TEST_FIXTURE(ProjectResourceActionTestClass, InitMultipleProjects) {
	// test that when we intialize 2 project we get 2
	// global cache pointers

	wxString srcFileProject1;
	MAKE_FILENAME(srcFileProject1, wxT("src_project_1"), wxT("User.php"));
	wxString contents = wxT("<?php class User {}");
	Project1.Label = wxT("test project 1");
	CreateFixtureFile(srcFileProject1, contents);
	Project1.ResourceDbFileName.Assign(TestProjectDir, wxT("cache_project_1.db"));

	wxString srcFileProject2;
	MAKE_FILENAME(srcFileProject2, wxT("src_project_2"), wxT("Role.php"));
	contents = wxT("<?php class Role {}");
	Project2.Label = wxT("test project 2");
	CreateFixtureFile(srcFileProject2, contents);
	Project2.ResourceDbFileName.Assign(TestProjectDir, wxT("cache_project_2.db"));
	Globals.Projects.push_back(Project1);
	Globals.Projects.push_back(Project2);

	CHECK(ProjectResourceAction.Init(Globals));

	ProjectResourceAction.BackgroundWork();

	CHECK_VECTOR_SIZE(2, GlobalCaches);
	CHECK(NULL != GlobalCaches[0]);
	CHECK(NULL != GlobalCaches[1]);

	mvceditor::ResourceSearchClass searchFirst(UNICODE_STRING_SIMPLE("User"));
	CHECK_VECTOR_SIZE(1, GlobalCaches[0]->ResourceFinder.CollectFullyQualifiedResource(searchFirst));

	mvceditor::ResourceSearchClass searchSecond(UNICODE_STRING_SIMPLE("Role"));
	CHECK_VECTOR_SIZE(1, GlobalCaches[1]->ResourceFinder.CollectFullyQualifiedResource(searchSecond));
}

}