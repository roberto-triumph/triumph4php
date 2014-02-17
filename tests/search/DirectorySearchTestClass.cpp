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
#include <search/DirectorySearchClass.h>
#include <FileTestFixtureClass.h>
#include <TriumphChecks.h>
#include <wx/filename.h>
#include <algorithm>

class DirectorySearchTestClass : public FileTestFixtureClass {
public:
	DirectorySearchTestClass()
		: FileTestFixtureClass(wxT("directory_search")) 
		, DirectorySearch() {
	}
	
	/**
	 * Creates a set of files to test recursion into sub directories.
	 * The following files are created (relative to TestProjectDir):
	 *
	 * file_one.php
	 * file_two.php
	 * folder_one/file_one.php
	 * folder_one/file_two.php
	 * folder_two/file_one.php
	 * folder_two/file_two.php
	 *
	 */
	void CreateTestFiles() {
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
	}

	
	t4p::DirectorySearchClass DirectorySearch;
};

class SourceFixtureClass {
public:

	SourceFixtureClass() 
		: Source() {
	
	}

	/**
	 * removes all wildcards and assigns the given wildcards
	 */
	void Make(const wxString& src, const wxString& include, const wxString& exclude) {
		Source.RootDirectory.AssignDir(src);
		Source.ClearIncludeWildcards();
		Source.ClearExcludeWildcards();
		Source.SetIncludeWildcards(include);
		Source.SetExcludeWildcards(exclude);
	}

	t4p::SourceClass Source;

};

class FileTestDirectoryWalker : public t4p::DirectoryWalkerClass {

public:


	bool IsBeginCalled;

	bool IsEndCalled;

	std::vector<wxString> SourcesCalled;

	FileTestDirectoryWalker()
		: DirectoryWalkerClass()
		, IsBeginCalled(false)
		, IsEndCalled(false)
		, SourcesCalled() {

	}

	void BeginSearch(const wxString& fullPath) {
		IsBeginCalled = true;
		SourcesCalled.push_back(fullPath);
	}

	bool Walk(const wxString& file) {
		return true;
	}

	void EndSearch() {
		IsEndCalled = true;
	}
};

SUITE(DirectorySearchTestClass) {
	
TEST_FIXTURE(DirectorySearchTestClass, WalkShouldRecurseThroughSubDirectories) {
	CreateTestFiles();
	FileTestDirectoryWalker walker;
	CHECK(DirectorySearch.Init(TestProjectDir));
	while (DirectorySearch.More()) {
		DirectorySearch.Walk(walker);
	}
	CHECK_EQUAL((unsigned int)6, DirectorySearch.GetMatchedFiles().size());
	std::vector<wxString> matchedFiles = DirectorySearch.GetMatchedFiles();
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
	CHECK(walker.IsBeginCalled);
	CHECK(walker.IsEndCalled);
	CHECK_VECTOR_SIZE(1, walker.SourcesCalled);
	CHECK_EQUAL(TestProjectDir, walker.SourcesCalled[0]);
}

TEST_FIXTURE(DirectorySearchTestClass, WalkWithMultipleSources) {
	
	/* 
	 * folder_one/file_one.php
	 * folder_one/file_two.php
	 * folder_two/file_one.php
	 * folder_two/file_two.php
	 */
	CreateTestFiles();
	FileTestDirectoryWalker walker;
	std::vector<t4p::SourceClass> sources;
	t4p::SourceClass src1;
	src1.RootDirectory.AssignDir(TestProjectDir + wxT("folder_one"));
	src1.SetIncludeWildcards(wxT("*"));
	sources.push_back(src1);

	t4p::SourceClass src2;
	src2.RootDirectory.AssignDir(TestProjectDir + wxT("folder_two"));
	src2.SetIncludeWildcards(wxT("*"));
	sources.push_back(src2);

	CHECK(DirectorySearch.Init(sources));
	while (DirectorySearch.More()) {
		DirectorySearch.Walk(walker);
	}
	CHECK_EQUAL((unsigned int)4, DirectorySearch.GetMatchedFiles().size());
	std::vector<wxString> matchedFiles = DirectorySearch.GetMatchedFiles();
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), 
		TestProjectDir + wxT("folder_one") + wxFileName::GetPathSeparator() + wxT("file_one.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), 
		TestProjectDir + wxT("folder_one") + wxFileName::GetPathSeparator() + wxT("file_two.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), 
		TestProjectDir + wxT("folder_two") + wxFileName::GetPathSeparator() + wxT("file_one.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), 
		TestProjectDir + wxT("folder_two") + wxFileName::GetPathSeparator() + wxT("file_two.php")));
	CHECK(walker.IsBeginCalled);
	CHECK(walker.IsEndCalled);
	CHECK_VECTOR_SIZE(2, walker.SourcesCalled);
	CHECK_EQUAL(1, count(walker.SourcesCalled.begin(), walker.SourcesCalled.end(), 
		TestProjectDir + wxT("folder_one") + wxFileName::GetPathSeparator()));
	CHECK_EQUAL(1, count(walker.SourcesCalled.begin(), walker.SourcesCalled.end(), 
		TestProjectDir + wxT("folder_two") + wxFileName::GetPathSeparator()));
}

TEST_FIXTURE(DirectorySearchTestClass, WalkShouldRecurseThroughSubDirectoriesInPreciseMode) {
	CreateTestFiles();
	FileTestDirectoryWalker walker;
	CHECK(DirectorySearch.Init(TestProjectDir, t4p::DirectorySearchClass::PRECISE));
	CHECK_EQUAL(6, DirectorySearch.GetTotalFileCount());
	while (DirectorySearch.More()) {
		DirectorySearch.Walk(walker);
	}
	CHECK_EQUAL((unsigned int)6, DirectorySearch.GetMatchedFiles().size());
	std::vector<wxString> matchedFiles = DirectorySearch.GetMatchedFiles();
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
	CHECK_VECTOR_SIZE(1, walker.SourcesCalled);
	CHECK_EQUAL(TestProjectDir, walker.SourcesCalled[0]);
}
	
TEST_FIXTURE(DirectorySearchTestClass, WalkShouldSkipHiddenFiles) {
	CreateTestFiles();

	// hide all of the file_two.php
	HideFile(TestProjectDir + wxFileName::GetPathSeparator() + wxT("file_two.php"));
	HideFile(TestProjectDir + wxT("folder_one") + wxFileName::GetPathSeparator() + wxT("file_two.php"));
	HideFile(TestProjectDir + wxT("folder_two") + wxFileName::GetPathSeparator() + wxT("file_two.php"));
	
	FileTestDirectoryWalker walker;
	CHECK(DirectorySearch.Init(TestProjectDir));
	while (DirectorySearch.More()) {
		DirectorySearch.Walk(walker);
	}
	CHECK_EQUAL((unsigned int)3, DirectorySearch.GetMatchedFiles().size());
	std::vector<wxString> matchedFiles = DirectorySearch.GetMatchedFiles();
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("file_one.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("folder_one") + wxFileName::GetPathSeparator() + wxT("file_one.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("folder_two") + wxFileName::GetPathSeparator() + wxT("file_one.php")));
	
	CHECK_EQUAL(0, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("file_two.php")));
	CHECK_EQUAL(0, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("folder_one") + wxFileName::GetPathSeparator() + wxT("file_two.php")));
	CHECK_EQUAL(0, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("folder_two") + wxFileName::GetPathSeparator() + wxT("file_two.php")));
	
	CHECK_VECTOR_SIZE(1, walker.SourcesCalled);
	CHECK_EQUAL(TestProjectDir, walker.SourcesCalled[0]);
}

TEST_FIXTURE(DirectorySearchTestClass, WalkShouldMatchHiddenFilesInRecursive) {
	CreateTestFiles();

	// hide all of the file_two.php
	wxString hiddenFile1 = HideFile(TestProjectDir + wxT("file_two.php"));
	wxString hiddenFile2 = HideFile(TestProjectDir + wxT("folder_one") + wxFileName::GetPathSeparator() + wxT("file_two.php"));
	wxString hiddenFile3 = HideFile(TestProjectDir + wxT("folder_two") + wxFileName::GetPathSeparator() + wxT("file_two.php"));

	FileTestDirectoryWalker walker;
	CHECK(DirectorySearch.Init(TestProjectDir, t4p::DirectorySearchClass::RECURSIVE, true));
	while (DirectorySearch.More()) {
		DirectorySearch.Walk(walker);
	}
	CHECK_EQUAL((unsigned int)6, DirectorySearch.GetMatchedFiles().size());
	std::vector<wxString> matchedFiles = DirectorySearch.GetMatchedFiles();
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("file_one.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("folder_one") + wxFileName::GetPathSeparator() + wxT("file_one.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("folder_two") + wxFileName::GetPathSeparator() + wxT("file_one.php")));
	
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), hiddenFile1));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), hiddenFile2));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), hiddenFile3));

	CHECK_VECTOR_SIZE(1, walker.SourcesCalled);
	CHECK_EQUAL(TestProjectDir, walker.SourcesCalled[0]);
}

TEST_FIXTURE(DirectorySearchTestClass, WalkShouldMatchHiddenFilesInPreciseMode) {
	CreateTestFiles();
	wxString hiddenFile1 = HideFile(TestProjectDir + wxT("file_two.php"));
	
	FileTestDirectoryWalker walker;
	CHECK(DirectorySearch.Init(TestProjectDir, t4p::DirectorySearchClass::PRECISE, true));
	while (DirectorySearch.More()) {
		DirectorySearch.Walk(walker);
	}
	CHECK_EQUAL((unsigned int)6, DirectorySearch.GetMatchedFiles().size());
	std::vector<wxString> matchedFiles = DirectorySearch.GetMatchedFiles();
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("file_one.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("folder_one") + wxFileName::GetPathSeparator() + wxT("file_one.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("folder_two") + wxFileName::GetPathSeparator() + wxT("file_one.php")));
	
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), hiddenFile1));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("folder_one") + wxFileName::GetPathSeparator() + wxT("file_two.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("folder_two") + wxFileName::GetPathSeparator() + wxT("file_two.php")));

	CHECK_VECTOR_SIZE(1, walker.SourcesCalled);
	CHECK_EQUAL(TestProjectDir, walker.SourcesCalled[0]);
}

TEST_FIXTURE(DirectorySearchTestClass, WalkShouldSkipFileThatMatchExcludeWildcardInRecursive) {
	CreateTestFiles();

	// hide all of the file_two.php
	std::vector<t4p::SourceClass> sources;
	t4p::SourceClass src;
	src.RootDirectory.Assign(TestProjectDir);
	src.SetIncludeWildcards(wxT("*"));
	src.SetExcludeWildcards(wxT("file_two.php"));
	sources.push_back(src);
	
	FileTestDirectoryWalker walker;
	CHECK(DirectorySearch.Init(sources));
	while (DirectorySearch.More()) {
		DirectorySearch.Walk(walker);
	}
	std::vector<wxString> matchedFiles = DirectorySearch.GetMatchedFiles();
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("file_one.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("folder_one") + wxFileName::GetPathSeparator() + wxT("file_one.php")));
	CHECK_EQUAL(1, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("folder_two") + wxFileName::GetPathSeparator() + wxT("file_one.php")));
	
	CHECK_EQUAL(0, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("file_two.php")));
	CHECK_EQUAL(0, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("folder_one") + wxFileName::GetPathSeparator() + wxT("file_two.php")));
	CHECK_EQUAL(0, count(matchedFiles.begin(), matchedFiles.end(), TestProjectDir + wxT("folder_two") + wxFileName::GetPathSeparator() + wxT("file_two.php")));

	CHECK_VECTOR_SIZE(1, walker.SourcesCalled);
	CHECK_EQUAL(TestProjectDir, walker.SourcesCalled[0]);
}

TEST_FIXTURE(SourceFixtureClass, ContainsShouldReturnFalse) {
	wxString root = wxFileName::GetTempDir() + wxFileName::GetPathSeparator() + 
		wxT("temp");
	Make(root, wxT("*.php"), wxT("*.phtml"));

	// outside of  root dir
	wxString test = wxFileName::GetTempDir() + wxFileName::GetPathSeparator() + wxT("file.php");
	CHECK_EQUAL(false, Source.Contains(test));

	// matches an exclude wildcard
	test = root + wxFileName::GetPathSeparator() + wxT("file.phtml");
	CHECK_EQUAL(false, Source.Contains(test));

	// in sub-directory and matches exclude wildcard
	test = root + wxFileName::GetPathSeparator() + wxT("tmp") + wxFileName::GetPathSeparator() + wxT("file.phtml");
	CHECK_EQUAL(false, Source.Contains(test));

	// does not match an include wildcard 
	test = root + wxFileName::GetPathSeparator() + wxT("file.class");
	CHECK_EQUAL(false, Source.Contains(test));
	
	// does not match an include wildcard 
	test = root + wxFileName::GetPathSeparator() + wxT("file.php~");
	CHECK_EQUAL(false, Source.Contains(test));
}

TEST_FIXTURE(SourceFixtureClass, ContainsShouldReturnFalse2) {
	wxString root = wxFileName::GetTempDir() + wxFileName::GetPathSeparator() + 
		wxT("temp");
	
	// exclude wildcards take precedence
	Make(root, wxT("*"), wxT("file.phtml"));
	wxString test = root + wxFileName::GetPathSeparator() + wxT("file.phtml");
	CHECK_EQUAL(false, Source.Contains(test));
}

TEST_FIXTURE(SourceFixtureClass, ContainsShouldReturnTrue) {
	wxString root = wxFileName::GetTempDir();
	Make(root, wxT("*.php"), wxT(""));

	// same as root dir
	wxString test = root + wxFileName::GetPathSeparator() + wxT("file.php");
	CHECK(Source.Contains(test));

	// in sub-directory
	test = root + wxFileName::GetPathSeparator() + wxT("temp") + wxFileName::GetPathSeparator() + wxT("file.php");
	CHECK(Source.Contains(test));

	// does not match the exclude list
	Make(root, wxT("*"), wxT("file_two.php"));
	test = root + wxFileName::GetPathSeparator() + wxT("file.php");
	CHECK(Source.Contains(test));
}

TEST_FIXTURE(SourceFixtureClass, ContainsHandleWildcardsWithSpaces) {
	wxString root = wxFileName::GetTempDir() + wxFileName::GetPathSeparator() + 
		wxT("temp");

	// test that the wildcards are trimmed, since ending spaces are no signigicant
	// also test that consecutive semicolons are treated as one
	Make(root, wxT("*"), wxT(";; ;*.phtml; "));

	// outside of  root dir
	wxString test;

	// matches something not excluded
	test = root + wxFileName::GetPathSeparator() + wxT("file.php");
	CHECK(Source.Contains(test));
}

TEST_FIXTURE(SourceFixtureClass, ExcludeWildcardsShouldBeResetToEmpty) {
	wxString root = wxFileName::GetTempDir();
	Make(root, wxT("*.php"), wxT("*.inc"));
	CHECK_EQUAL(wxT("*.inc"), Source.ExcludeWildcardsString());

	Make(root, wxT("*.php"), wxT(""));
	CHECK_EQUAL(wxT(""), Source.ExcludeWildcardsString());
}

TEST_FIXTURE(SourceFixtureClass, IncludeWildcardsShouldBeResetToEmpty) {
	wxString root = wxFileName::GetTempDir();
	Make(root, wxT("*.php"), wxT("*.inc"));
	CHECK_EQUAL(wxT("*.php"), Source.IncludeWildcardsString());

	Make(root, wxT(""), wxT(""));
	CHECK_EQUAL(wxT(""), Source.IncludeWildcardsString());
}

}