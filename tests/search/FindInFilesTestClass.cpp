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
#include <FileTestFixtureClass.h>
#include <search/FindInFilesClass.h>
#include <windows/StringHelperClass.h>

wxString FILE_1 = wxString::FromAscii(
	"<?php\n"
	"class UserClass {\n"
	"\tprivate $name;\n"
	"\tfunction getName() {\n"
	"\t\treturn $this->name;\n"
	"\t}\n"
	"\n"
	"?>\n"
);

wxString FILE_2 = wxString::FromAscii(
	"<?php\n"
	"class AdminClass extends UserClass {\n"
	"\tconst WORK_FILE = 'c:/wamp/www/project_work.html';\n"
	"\tfunction deleteWorkFile() {\n"
	"\t\tunlink(self::WORK_FILE);\n"
	"\t}\n"
	"\n"
	"?>\n"
);

class FindInFilesTestFixtureClass : public FileTestFixtureClass {
public:
	FindInFilesTestFixtureClass()
			: FileTestFixtureClass(wxT("find_in_files"))  {
		FindInFiles = new mvceditor::FindInFilesClass();
	}

	virtual ~FindInFilesTestFixtureClass() {
		delete FindInFiles;
	}

	mvceditor::FindInFilesClass* FindInFiles;
};

SUITE(FindInFilesTestClass) {
	
TEST_FIXTURE(FindInFilesTestFixtureClass, WalkShouldOnlySearchWhenWildcardsMatch) {
	CreateFixtureFile(wxT("user.php"), FILE_1);
	CreateFixtureFile(wxT("admin.inc"), FILE_1);
	CreateFixtureFile(wxT("admin.php_bak"), FILE_1);
	FindInFiles->Expression = UNICODE_STRING_SIMPLE("UserClass");
	FindInFiles->FilesFilter = wxT("*.php,*.inc");
	CHECK(FindInFiles->Prepare());
	CHECK(FindInFiles->Walk(TestProjectDir + wxT("user.php")));
	CHECK(FindInFiles->Walk(TestProjectDir + wxT("admin.inc")));
	CHECK_EQUAL(false, FindInFiles->Walk(TestProjectDir + wxT("admin.php_bak")));
}

TEST_FIXTURE(FindInFilesTestFixtureClass, WalkShouldLocateNextMatchWhenItReturnsTrue) {
	CreateFixtureFile(wxT("user.php"), FILE_1);
	FindInFiles->Expression = UNICODE_STRING_SIMPLE("UserClass");
	CHECK(FindInFiles->Prepare());
	CHECK(FindInFiles->Walk(TestProjectDir + wxT("user.php")));
	CHECK_EQUAL(2, FindInFiles->GetCurrentLineNumber());
}

TEST_FIXTURE(FindInFilesTestFixtureClass, FindNextShouldLocateNextMatchWhenSearchingFromTheMiddle) {
	CreateFixtureFile(wxT("user.php"), FILE_1);
	FindInFiles->Expression = UNICODE_STRING_SIMPLE("name");
	FindInFiles->CaseSensitive = false;
	CHECK(FindInFiles->Prepare());
	CHECK(FindInFiles->Walk(TestProjectDir + wxT("user.php")));
	CHECK_EQUAL(3, FindInFiles->GetCurrentLineNumber());
	CHECK(FindInFiles->FindNext());
	CHECK_EQUAL(4, FindInFiles->GetCurrentLineNumber());
	CHECK(FindInFiles->FindNext());
	CHECK_EQUAL(5, FindInFiles->GetCurrentLineNumber());
	CHECK_EQUAL(false, FindInFiles->FindNext());
	
}

TEST_FIXTURE(FindInFilesTestFixtureClass, FindNextShouldLocateNextMatchWhenSearchingCaseInsesitive) {
	CreateFixtureFile(wxT("user.php"), FILE_1);
	FindInFiles->Expression = UNICODE_STRING_SIMPLE("userclass");
	FindInFiles->CaseSensitive = false;
	CHECK(FindInFiles->Prepare());
	CHECK(FindInFiles->Walk(TestProjectDir + wxT("user.php")));
	CHECK_EQUAL(2, FindInFiles->GetCurrentLineNumber());
}

TEST_FIXTURE(FindInFilesTestFixtureClass, FindNextShouldNotLocateNextMatchWhenNoMatchIsFound) {
	CreateFixtureFile(wxT("user.php"), FILE_1);
	FindInFiles->Expression = UNICODE_STRING_SIMPLE("AnotherUserClass");
	CHECK(FindInFiles->Prepare());
	CHECK_EQUAL(false, FindInFiles->Walk(TestProjectDir + wxT("user.php")));
	CHECK_EQUAL(false, FindInFiles->FindNext());
}

TEST_FIXTURE(FindInFilesTestFixtureClass, PrepareShouldReturnTrueWhenExpressionsAreValid) {
	CreateFixtureFile(wxT("user.php"), FILE_1);
	FindInFiles->Expression = UNICODE_STRING_SIMPLE("\\s*UserClass\\s*");
	FindInFiles->ReplaceExpression = UNICODE_STRING_SIMPLE("\\s*GuestUserClass\\s*");
	FindInFiles->Mode = mvceditor::FinderClass::REGULAR_EXPRESSION;
	CHECK(FindInFiles->Prepare());
}

TEST_FIXTURE(FindInFilesTestFixtureClass, PrepareShouldReturnFalseWhenExpressionIsInvalid) {
	CreateFixtureFile(wxT("user.php"), FILE_1);
	FindInFiles->Expression = UNICODE_STRING_SIMPLE("\\s*(UserClass\\s*");
	FindInFiles->ReplaceExpression = UNICODE_STRING_SIMPLE("\\s*GuestUserClass\\s*");
	FindInFiles->Mode = mvceditor::FinderClass::REGULAR_EXPRESSION;
	CHECK_EQUAL(false, FindInFiles->Prepare());
}


TEST_FIXTURE(FindInFilesTestFixtureClass, ReplaceAllMatchesShouldReplaceAllMatches) {
	CreateFixtureFile(wxT("admin.inc"), FILE_2);
	FindInFiles->Expression = UNICODE_STRING_SIMPLE("WORK_FILE");
	FindInFiles->ReplaceExpression = UNICODE_STRING_SIMPLE("FILENAME_WORK");
	FindInFiles->Mode = mvceditor::FinderClass::REGULAR_EXPRESSION;
	CHECK(FindInFiles->Prepare());
	UnicodeString newText = mvceditor::StringHelperClass::wxToIcu(FILE_2);
	CHECK_EQUAL(2, FindInFiles->ReplaceAllMatches(newText));
	UnicodeString expectedText = mvceditor::StringHelperClass::wxToIcu(FILE_2);
	expectedText.findAndReplace(UNICODE_STRING_SIMPLE("WORK_FILE"), UNICODE_STRING_SIMPLE("FILENAME_WORK"));
	CHECK_EQUAL(expectedText, newText);
}

TEST_FIXTURE(FindInFilesTestFixtureClass, ReplaceAllMatchesInFileShouldReplaceAllMatches) {
	CreateFixtureFile(wxT("user.php"), FILE_1);
	CreateFixtureFile(wxT("admin.inc"), FILE_2);
	CreateFixtureFile(wxT("admin.php_bak"), FILE_2);
	FindInFiles->Expression = UNICODE_STRING_SIMPLE("\\s*(UserClass)\\s*");
	FindInFiles->ReplaceExpression = UNICODE_STRING_SIMPLE(" Guest$1 ");
	FindInFiles->Mode = mvceditor::FinderClass::REGULAR_EXPRESSION;
	CHECK(FindInFiles->Prepare());
	int matches = 0;
	matches += FindInFiles->ReplaceAllMatchesInFile(TestProjectDir + wxT("user.php"));
	matches += FindInFiles->ReplaceAllMatchesInFile(TestProjectDir + wxT("admin.inc"));
	matches += FindInFiles->ReplaceAllMatchesInFile(TestProjectDir + wxT("admin.php_bak"));
	CHECK_EQUAL(3, matches);
	
	// check that files were actually modified
	 wxString fileContents = GetFileContents(wxT("user.php"));
	 wxString expectedContents(FILE_1);
	 expectedContents.Replace(wxT("UserClass"), wxT("GuestUserClass"));
	 CHECK_EQUAL(expectedContents, fileContents);
	 
	 //check file #2
	 fileContents = GetFileContents(wxT("admin.inc"));
	 expectedContents = FILE_2;
	 expectedContents.Replace(wxT("UserClass"), wxT("GuestUserClass"));
	 CHECK_EQUAL(expectedContents, fileContents);
	 
	 //check file #3
	 fileContents = GetFileContents(wxT("admin.php_bak"));
	 expectedContents = FILE_2;
	 expectedContents.Replace(wxT("UserClass"), wxT("GuestUserClass"));
	 CHECK_EQUAL(expectedContents, fileContents);
}

//TEST_FIXTURE(FindInFilesTestFixtureClass, FindInCurrentFileShouldFindExpressionWhenFileHasHighAsciiCharacters) {
//	FindInFiles->Expression = UNICODE_STRING_SIMPLE("ExtendedRecordSetForUnitTestAddGetLeftJoin");
//	DirectorySearchClass search;
//	FindInFiles->FilesFilter = UNICODE_STRING_SIMPLE("*RecordSet.php");
//	search.Init(wxT("/home/roberto/workspace/sample_php_project/developer_tests/library/db/"));
//	CHECK(FindInFiles->Prepare());
//	while (search.More()) {
//		search.Walk(*FindInFiles);
//	}
//	printf("matches found:%d\n", search.GetMatchedFiles().size());
//}
}