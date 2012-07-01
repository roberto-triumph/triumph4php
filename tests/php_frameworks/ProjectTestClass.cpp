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
#include <environment/ProjectClass.h>
#include <php_frameworks/FrameworkDetectorClass.h>
#include <FileTestFixtureClass.h>
#include <wx/event.h>

class ProjectTestFixtureClass : public FileTestFixtureClass {
public:	
	ProjectTestFixtureClass() 
		: FileTestFixtureClass(wxT("project_test"))
		, Handler()
		, RunningThreads()
		, FrameworkDetector(Handler, RunningThreads) 
		, DatabaseDetector(Handler, RunningThreads)
		, ResourcesDetector(Handler, RunningThreads) {
	}
	
	virtual ~ProjectTestFixtureClass() {
	}

	wxEvtHandler Handler;
	mvceditor::RunningThreadsClass RunningThreads;
	mvceditor::FrameworkDetectorActionClass FrameworkDetector;
	mvceditor::DatabaseDetectorActionClass DatabaseDetector;
	mvceditor::ResourcesDetectorActionClass ResourcesDetector;
};

SUITE(ProjectTestClass) {

TEST_FIXTURE(ProjectTestFixtureClass, ShouldParseFrameworkResponse) {
	wxString result = wxString::FromAscii(
		"framework_0 = \"Test\"\n"
		"framework_1 = \"Symfony\"\n"
	);
	wxString fileName = wxT("test.ini"); 
	CreateFixtureFile(fileName, result);
	FrameworkDetector.InitFromFile(TestProjectDir + fileName);
	std::vector<wxString> identifiers = FrameworkDetector.Frameworks;
	CHECK_EQUAL((size_t)2, identifiers.size());
	if ((size_t)2 == identifiers.size()) {
		CHECK_EQUAL(wxT("Test"), identifiers[0]);
		CHECK_EQUAL(wxT("Symfony"), identifiers[1]);
	}
}
	
TEST_FIXTURE(ProjectTestFixtureClass, ShouldParseDatabaseResponse) {
	wxString result = wxString::FromAscii(
		"[local_dev]\n"
		"Environment = \"dev\"\n"
		"Name = \"local dev\"\n"
		"Driver = \"MYSQL\"\n"
		"FileName = \"\"\n"
		"Host = \"127.0.0.1\"\n"
		"Port = 3306\n"
		"DatabaseName = \"my_dev\"\n"
		"User = \"my_user_dev\"\n"
		"Password = \"123_dev\"\n"
		"\n"
		"[local_testing]\n"
		"Environment = \"test\"\n"
		"Name = \"local testing\"\n"
		"Driver = \"MYSQL\"\n"
		"FileName = \"\"\n"
		"Host = \"localhost\"\n"
		"Port = 3306\n"
		"DatabaseName = \"my_test\"\n"
		"User = \"my_user_test\"\n"
		"Password = \"123_test\"\n"
	);
	wxString fileName = wxT("test.ini"); 
	CreateFixtureFile(fileName, result);
	DatabaseDetector.InitFromFile(TestProjectDir + fileName);
	std::vector<mvceditor::DatabaseInfoClass> infos = DatabaseDetector.Databases;
	CHECK_EQUAL((size_t)2, infos.size());
	if ((size_t)2 == infos.size()) {
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("127.0.0.1"), infos[0].Host);
	}
}

TEST_FIXTURE(ProjectTestFixtureClass, ShouldParseResourcesResponse) {
	wxString result = wxString::FromAscii(
		"[Resource_72]\n"
		"Resource = \"CI_Controller::news_model\"\n"
		"Identifier = \"news_model\"\n"
		"ReturnType = \"News_model\"\n"
		"Signature = \"CI_Controller::news_model\"\n"
		"Comment = \"\"\n"
		"Type = \"MEMBER\"\n"
		"\n"
		"[Resource_73]\n"
		"Resource = \"CI_Controller::admin_user_model\"\n"
		"Identifier = \"admin_user_model\"\n"
		"ReturnType = \"Admin_user_model\"\n"
		"Signature = \"CI_Controller::admin_user_model\"\n"
		"Comment = \"\"\n"
		"Type = \"MEMBER\"\n"
	);
	wxString fileName = wxT("test.ini"); 
	CreateFixtureFile(fileName, result);
	ResourcesDetector.InitFromFile(TestProjectDir + fileName);
	CHECK_EQUAL(mvceditor::DetectorActionClass::NONE, ResourcesDetector.Error);
	std::vector<mvceditor::ResourceClass> resources = ResourcesDetector.Resources;
	CHECK_EQUAL((size_t)2, resources.size());
	if ((size_t)2 == resources.size()) {
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("CI_Controller"), resources[0].ClassName);
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("news_model"), resources[0].Identifier);
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("News_model"), resources[0].ReturnType);
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("CI_Controller::news_model"), resources[0].Signature);
		CHECK_EQUAL(mvceditor::ResourceClass::MEMBER, resources[0].Type);

		CHECK_EQUAL(UNICODE_STRING_SIMPLE("CI_Controller"), resources[1].ClassName);
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("admin_user_model"), resources[1].Identifier);
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("Admin_user_model"), resources[1].ReturnType);
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("CI_Controller::admin_user_model"), resources[1].Signature);
		CHECK_EQUAL(mvceditor::ResourceClass::MEMBER, resources[1].Type);
	}
}
}