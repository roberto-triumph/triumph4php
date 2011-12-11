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
#include <php_frameworks/ProjectClass.h>
#include <FileTestFixtureClass.h>

class ProjectTestFixtureClass : public FileTestFixtureClass {
public:	
	ProjectTestFixtureClass() 
		: FileTestFixtureClass(wxT("project_test")) {
		mvceditor::ProjectOptionsClass options;
		options.RootPath = TestProjectDir;
		Project = new mvceditor::ProjectClass(options, Environment);
	}
	
	virtual ~ProjectTestFixtureClass() {
		delete Project;
	}

	mvceditor::ProjectClass* Project;
	mvceditor::EnvironmentClass Environment;
};

SUITE(ProjectTestClass) {

TEST_FIXTURE(ProjectTestFixtureClass, ShouldParseFrameworkResponse) {
	wxString result = wxString::FromAscii(
		"-----START-MVC-EDITOR-----\n"
		"framework_0 = \"Test\"\n"
		"framework_1 = \"Symfony\"\n"
	);
	mvceditor::ProjectClass::DetectError error = mvceditor::ProjectClass::NONE;
	CHECK(Project->DetectFrameworkResponse(result, error));
	std::vector<wxString> identifiers = Project->FrameworkIdentifiers();
	CHECK_EQUAL((size_t)2, identifiers.size());
	CHECK_EQUAL(wxT("Test"), identifiers[0]);
	CHECK_EQUAL(wxT("Symfony"), identifiers[1]);
}
	
TEST_FIXTURE(ProjectTestFixtureClass, ShouldParseDatabaseResponse) {
	wxString result = wxString::FromAscii(
		"-----START-MVC-EDITOR-----\n"
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

	mvceditor::ProjectClass::DetectError error = mvceditor::ProjectClass::NONE;
	CHECK(Project->DetectDatabaseResponse(result, error));
	
	std::vector<mvceditor::DatabaseInfoClass> frameworks = Project->DatabaseInfo();
	CHECK_EQUAL((size_t)2, frameworks.size());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("127.0.0.1"), frameworks[0].Host);
}

}