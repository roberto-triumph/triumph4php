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
 * @author     $Author: robertop2004@gmail.com $
 * @date       $Date: 2011-07-04 12:25:36 -0700 (Mon, 04 Jul 2011) $
 * @version    $Rev: 568 $ 
 */
#include <UnitTest++.h>
#include <php_frameworks/ProjectClass.h>
#include <FileTestFixtureClass.h>

class SymfonyProjectTestClass : public FileTestFixtureClass {
public:	
	SymfonyProjectTestClass() 
		: FileTestFixtureClass(wxT("new_project")) {
		mvceditor::ProjectOptionsClass options;
		options.RootPath = TestProjectDir;
		options.Framework = mvceditor::SYMFONY;
		Project = new mvceditor::SymfonyProjectClass(options);
		if (wxDirExists(TestProjectDir)) {
			RecursiveRmDir(TestProjectDir);
			//wxRmDir(TestProjectDir.fn_str());
		}
	}
	
	virtual ~SymfonyProjectTestClass() {
		delete Project;
	}

	mvceditor::SymfonyProjectClass* Project;	
};

// silence test for now
#if 0
TEST_FIXTURE(SymfonyProjectTestClass, CreateShouldCreateProjectFiles) {
	if (!wxDirExists(TestProjectDir)) {
		wxMkdir(TestProjectDir);
	}
	wxString errors;
	bool created = Project->Create(wxT("a new project"), errors);
	CHECK(created);
	bool exists = wxDirExists(TestProjectDir + wxT("apps"));
	CHECK(exists);
}
#endif
