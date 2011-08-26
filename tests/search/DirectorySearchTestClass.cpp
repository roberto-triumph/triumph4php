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
 * @date       $Date: 2011-06-10 17:19:31 -0700 (Fri, 10 Jun 2011) $
 * @version    $Rev: 555 $ 
 */
#include <UnitTest++.h>
#include <search/DirectorySearchClass.h>
#include <FileTestFixtureClass.h>
#include <wx/filename.h>
#include <algorithm>

class DirectorySearchTestClass : public FileTestFixtureClass {
public:
	DirectorySearchTestClass()
		: FileTestFixtureClass(wxT("directory_search")) {
		DirectorySearch = new mvceditor::DirectorySearchClass();
	}
	
	~DirectorySearchTestClass() {
		delete DirectorySearch;
	}
	
	mvceditor::DirectorySearchClass* DirectorySearch;
};

class FileTestDirectoryWalker : public mvceditor::DirectoryWalkerClass {

	virtual bool Walk(const wxString& file) {
		return true;
	}
};

SUITE(DirectorySearchTestClass) {
	
TEST_FIXTURE(DirectorySearchTestClass, WalkShouldRecurseThroughSubDirectories) {
	wxString files[] = { wxT("file_one.php"), wxT("file_two.php") };
	wxString directories[] =  { wxT("folder_one"), wxT("folder_two") };
	
	// create files in the dir, then the subdirs
	for (int j = 0; j < 2; ++j) {
		CreateFixtureFile(files[j], wxString::FromAscii(
			"<?php"
		));	
	}
	for (int i = 0; i < 2; ++i) {
		wxString subDirectory = directories[i];
		CreateSubDirectory(subDirectory);
		subDirectory += wxFileName::GetPathSeparator();
		for (int j = 0; j < 2; ++j) {
			CreateFixtureFile(subDirectory + files[j], wxString::FromAscii(
				"<?php"
			));	
		}
	}
	FileTestDirectoryWalker walker;
	CHECK(DirectorySearch->Init(TestProjectDir));
	while (DirectorySearch->More()) {
		DirectorySearch->Walk(walker);
	}
	CHECK_EQUAL((unsigned int)6, DirectorySearch->GetMatchedFiles().size());
	std::vector<wxString> matchedFiles = DirectorySearch->GetMatchedFiles();
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("file_one.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("file_two.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), 
		TestProjectDir + wxT("folder_one") + wxFileName::GetPathSeparator() + wxT("file_one.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), 
		TestProjectDir + wxT("folder_one") + wxFileName::GetPathSeparator() + wxT("file_two.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), 
		TestProjectDir + wxT("folder_two") + wxFileName::GetPathSeparator() + wxT("file_one.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), 
		TestProjectDir + wxT("folder_two") + wxFileName::GetPathSeparator() + wxT("file_two.php")));
}
#if 0
TEST_FIXTURE(DirectorySearchTestClass, WalkShouldRecurseThroughSubDirectoriesInPreciseMode) {
	wxString files[] = { wxT("file_one.php"), wxT("file_two.php") };
	wxString directories[] =  { wxT("folder_one"), wxT("folder_two") };
	
	// create files in the dir, then the subdirs
	for (int j = 0; j < 2; ++j) {
		CreateFixtureFile(files[j], wxString::FromAscii(
			"<?php"
		));	
	}
	for (int i = 0; i < 2; ++i) {
		wxString subDirectory = directories[i];
		CreateSubDirectory(subDirectory);
		subDirectory += wxFileName::GetPathSeparator();
		for (int j = 0; j < 2; ++j) {
			CreateFixtureFile(subDirectory + files[j], wxString::FromAscii(
				"<?php"
			));	
		}
	}
	FileTestDirectoryWalker walker;
	CHECK(DirectorySearch->Init(TestProjectDir, mvceditor::DirectorySearchClass::PRECISE));
	CHECK_EQUAL(6, DirectorySearch->GetTotalFileCount());
	while (DirectorySearch->More()) {
		DirectorySearch->Walk(walker);
	}
	CHECK_EQUAL((unsigned int)6, DirectorySearch->GetMatchedFiles().size());
	std::vector<wxString> matchedFiles = DirectorySearch->GetMatchedFiles();
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("file_one.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("file_two.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), 
		TestProjectDir + wxT("folder_one") + wxFileName::GetPathSeparator() + wxT("file_one.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), 
		TestProjectDir + wxT("folder_one") + wxFileName::GetPathSeparator() + wxT("file_two.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), 
		TestProjectDir + wxT("folder_two") + wxFileName::GetPathSeparator() + wxT("file_one.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), 
		TestProjectDir + wxT("folder_two") + wxFileName::GetPathSeparator() + wxT("file_two.php")));
}
#endif
}