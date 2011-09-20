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
		Project = new mvceditor::ProjectClass(options);
	}
	
	virtual ~ProjectTestFixtureClass() {
		delete Project;
	}

	mvceditor::ProjectClass* Project;	
};

SUITE(ProjectTestClass) {
	
TEST_FIXTURE(ProjectTestFixtureClass, ShouldBeDetected) {
	CreateFixtureFile(wxT("test.php"), wxT("<?php"));
	Project->Detect(true);
	std::vector<mvceditor::DatabaseInfoClass> frameworks = Project->DatabaseInfo();
	CHECK_EQUAL((size_t)2, frameworks.size());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("127.0.0.1"), frameworks[0].Host);
}

}