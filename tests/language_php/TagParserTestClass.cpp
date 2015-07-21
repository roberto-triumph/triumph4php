/**
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
 */
#include <UnitTest++.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <string>
#include <vector>
#include "globals/Assets.h"
#include "globals/Sqlite.h"
#include "language_php/TagParserClass.h"
#include "SqliteTestFixtureClass.h"

class TagParserTestFixtureClass : public SqliteTestFixtureClass {
	public:
	t4p::TagParserClass TagParser;

	TagParserTestFixtureClass()
		: SqliteTestFixtureClass(t4p::ResourceSqlSchemaAsset())
		, TagParser() {
		TagParser.Init(&Session);
		TagParser.PhpFileExtensions.push_back(wxT("*.php"));
	}

	void AddFile(const wxFileName& fileName, const UnicodeString& contents) {
		TagParser.BuildResourceCacheForFile(wxT(""), fileName.GetFullPath(), contents, true);
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
 * because the logic of what is now TagParserClass and ParsedTagFinderClass were originally
 * in the same class.
 */
SUITE(TagParserTestClass) {
TEST_FIXTURE(TagParserTestFixtureClass, WipeAll) {
	wxFileName file1 = TestFile(wxT("project1"), wxT("user.php"));
	AddFile(file1, t4p::CharToIcu(
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
	AddFile(file1, t4p::CharToIcu(
		"class User {}\n"
	));

	wxFileName file2 = TestFile(wxT("project2"), wxT("admin.php"));
	AddFile(file2, t4p::CharToIcu(
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
	AddFile(file1, t4p::CharToIcu(
		"class User {}\n"
	));

	wxFileName file2 = TestFile(wxT("project2"), wxT("admin.php"));
	AddFile(file2, t4p::CharToIcu(
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

TEST_FIXTURE(TagParserTestFixtureClass, RenameFile) {
	wxFileName file1 = TestFile(wxT("project1"), wxT("user.php"));
	AddFile(file1, t4p::CharToIcu(
		"class User {}\n"
		"class Admin {}\n"
	));

	std::string stdFullPath = t4p::WxToChar(file1.GetFullPath());
	int count = 0;
	Session << ("SELECT COUNT(*) FROM file_items WHERE full_path= ?"),
		soci::use(stdFullPath), soci::into(count);
	CHECK_EQUAL(1, count);

	wxFileName newFileName(file1.GetPath(), wxT("admin.php"));
	TagParser.RenameFile(file1, newFileName);

	stdFullPath = t4p::WxToChar(newFileName.GetFullPath());
	count = 0;
	std::string newName;
	Session << ("SELECT name FROM file_items WHERE full_path= ?"),
		soci::use(stdFullPath), soci::into(newName);
	CHECK_EQUAL("admin.php", newName);
}

TEST_FIXTURE(TagParserTestFixtureClass, RenameDir) {
	wxFileName file1 = TestFile(wxT("project1"), wxT("user.php"));
	AddFile(file1, t4p::CharToIcu(
		"class User {}\n"
		"class Admin {}\n"
	));

	wxFileName oldDir;
	oldDir.AssignDir(file1.GetPath());

	// change project_1 to project_2
	wxFileName newDir;
	newDir.AssignDir(file1.GetPath());
	newDir.RemoveLastDir();
	newDir.AppendDir(wxT("project_2"));


	TagParser.RenameDir(oldDir, newDir);

	wxFileName newFileName(newDir.GetPath(), wxT("user.php"));
	std::string stdFullPath = t4p::WxToChar(newFileName.GetFullPath());

	int count = 0;
	Session << ("SELECT COUNT(*) FROM file_items WHERE full_path= ?"),
		soci::use(stdFullPath), soci::into(count);
	CHECK_EQUAL(1, count);
}
}
