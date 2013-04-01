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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <UnitTest++.h>
#include <language/TagParserClass.h>
#include <globals/Assets.h>
#include <globals/Sqlite.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

class TagParserTestFixtureClass {

public:

	mvceditor::TagParserClass TagParser;
	soci::session Session;

	TagParserTestFixtureClass() 
		: TagParser() 
		, Session() {
		Session.open(*soci::factory_sqlite3(),":memory:");
		wxString error;
		if (!mvceditor::SqliteSqlScript(mvceditor::ResourceSqlSchemaAsset(), Session, error)) {
			wxASSERT_MSG(false, error);
		}
		TagParser.Init(&Session);
		TagParser.PhpFileExtensions.push_back(wxT("*.php"));
	}

	void AddFile(const wxFileName& fileName, const UnicodeString& contents) {
		TagParser.BuildResourceCacheForFile(fileName.GetFullPath(), contents, true);
	}

	wxFileName TestFile(const wxString& subdir, const wxString& name) {
		wxFileName file;
		file.AssignDir(wxFileName::GetTempDir());
		file.AppendDir(subdir);
		file.SetFullName(name);
		return file;
	}

	int RowCount(const std::string& tableName) {
		int count = 0;
		Session << ("SELECT COUNT(*) FROM " + tableName), soci::into(count);
		return count;
	}
};

/**
 * ATTN: currently most of the testing for TagParserClass is done in ParsedTagFinderTestClass
 * because the logic of what is now TagParserClass and ParsedTagFinderClass werer originally 
 * in the same class.
 */
SUITE(TagParserTestClass) {

TEST_FIXTURE(TagParserTestFixtureClass, WipeAll) {
	wxFileName file1 = TestFile(wxT("project1"), wxT("user.php"));
	AddFile(file1, mvceditor::CharToIcu(
		"class User {}\n"
		"class Admin {}\n"
	));
	int count = RowCount("file_items");
	CHECK_EQUAL(1, count);
	count = RowCount("resources");

	// 2 classes + 2 fully namespaced tags + 1 namespace tag
	CHECK_EQUAL(5, count);

	TagParser.WipeAll();
	count = RowCount("file_items");
	CHECK_EQUAL(0, count);
	count = RowCount("resources");
	CHECK_EQUAL(0, count);
	
}

TEST_FIXTURE(TagParserTestFixtureClass, DeleteDirectories) {

	// create 2 files, each in different directories
	wxFileName file1 = TestFile(wxT("project1"), wxT("user.php"));
	AddFile(file1, mvceditor::CharToIcu(
		"class User {}\n"
	));

	wxFileName file2 = TestFile(wxT("project2"), wxT("admin.php"));
	AddFile(file2, mvceditor::CharToIcu(
		"class Admin {}\n"
	));
	int count = RowCount("file_items");
	CHECK_EQUAL(2, count);
	count = RowCount("resources");

	// 2 classes + 2 fully namespaced tags + 1 namespace tag
	CHECK_EQUAL(5, count);

	std::vector<wxFileName> dirsToDelete;
	wxFileName dir2;
	dir2.AssignDir(file2.GetPath());
	dirsToDelete.push_back(dir2);
	TagParser.DeleteDirectories(dirsToDelete);

	count = RowCount("file_items");
	CHECK_EQUAL(1, count);
	count = RowCount("resources");
	CHECK_EQUAL(3, count);
}

TEST_FIXTURE(TagParserTestFixtureClass, DeleteDirectoriesWithSubDirectories) {

	// create 2 files, each in different directories
	wxFileName file1 = TestFile(wxT("project1"), wxT("user.php"));
	AddFile(file1, mvceditor::CharToIcu(
		"class User {}\n"
	));

	wxFileName file2 = TestFile(wxT("project2"), wxT("admin.php"));
	AddFile(file2, mvceditor::CharToIcu(
		"class Admin {}\n"
	));
	int count = RowCount("file_items");
	CHECK_EQUAL(2, count);
	count = RowCount("resources");

	// 2 classes + 2 fully namespaced tags + 1 namespace tag
	CHECK_EQUAL(5, count);

	// delete the parent of dir2, both files should be gone
	std::vector<wxFileName> dirsToDelete;
	wxFileName dir2;
	dir2.AssignDir(file2.GetPath());
	dir2.RemoveLastDir();
	dirsToDelete.push_back(dir2);
	TagParser.DeleteDirectories(dirsToDelete);
	
	count = RowCount("file_items");
	CHECK_EQUAL(0, count);
	count = RowCount("resources");
	CHECK_EQUAL(0, count);
}

}
