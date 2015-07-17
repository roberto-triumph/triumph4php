/*
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
 * @copyright  2014 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <UnitTest++.h>
#include <TriumphChecks.h>
#include <SqliteTestFixtureClass.h>
#include <globals/FileCabinetItemClass.h>
#include <globals/SqliteFinderClass.h>
#include <globals/String.h>
#include <wx/platinfo.h>
#include <iostream>

class FileCabinetFixtureClass : public SqliteTestFixtureClass {
	
public:

	FileCabinetFixtureClass() 
	: SqliteTestFixtureClass()
	, SqliteFinder()
	, AllResults() {
		SqliteFinder.InitSession(&Session);
	}
	
	t4p::SqliteFinderClass SqliteFinder;
	t4p::AllFileCabinetResultClass AllResults;

};

SUITE(FileCabinet) {
	
TEST_FIXTURE(FileCabinetFixtureClass, FindAll) {
	
	// verify that we can load files and directories
	// from the file cabinet items table
	// user wxFileName so that tests work on all OSes
	wxFileName file;
	file.AppendDir("home");
	file.AppendDir("user");
	file.AppendDir("www");
	file.SetFullName("test.php");

	wxFileName dir;
	dir.AppendDir("home");
	dir.AppendDir("user");
	dir.AppendDir("www");
	dir.AppendDir("config");

	Exec("BEGIN;");
	Exec("INSERT INTO file_cabinet_items(file_cabinet_item_id, name, full_path) VALUES(NULL, 'test.php', '" + 
		t4p::WxToChar(file.GetFullPath()) + "');");
	Exec("INSERT INTO file_cabinet_items(file_cabinet_item_id, name, full_path) VALUES(NULL, 'config', '" + 
		t4p::WxToChar(dir.GetPathWithSep()) + "');");
	Exec("COMMIT;");
	
	bool hasRows = SqliteFinder.Exec(&AllResults);
	CHECK(hasRows);
	AllResults.Next();
	CHECK_EQUAL(file.GetFullPath(), AllResults.Item.FileName.GetFullPath());
	
	CHECK(AllResults.More());
	AllResults.Next();
	CHECK_EQUAL(dir.GetPath(), AllResults.Item.FileName.GetPath());
	
	CHECK_EQUAL(false, AllResults.More());
}

TEST_FIXTURE(FileCabinetFixtureClass, StoreFile) {
	
	// test that a file can be stored in the file cabinet
	// properly
	t4p::FileCabinetStoreClass store;
	t4p::FileCabinetItemClass item;
	
	wxPlatformInfo info;
	std::string fileToSave;
	if (info.GetOperatingSystemId() & wxOS_WINDOWS_NT) {
		fileToSave = "C:\\home\\user\\www\\test.php";
	}
	else {
		fileToSave = "/home/user/www/test.php";
	}
	
	item.FileName.Assign(fileToSave);
	
	CHECK(store.Store(Session, item));
	
	// check that it has been inserted in the table
	int id;
	std::string name;
	std::string fullPath;
	soci::statement stmt = (Session.prepare << "SELECT file_cabinet_item_id, name, full_path FROM file_cabinet_items",
		soci::into(id), soci::into(name), soci::into(fullPath)
	);
	stmt.execute(true);
	CHECK_EQUAL("test.php", name);
	CHECK_EQUAL(fileToSave, fullPath);
	CHECK_EQUAL(id, item.Id);
}

TEST_FIXTURE(FileCabinetFixtureClass, StoreDirectory) {
	
	// test that a directory can be stored in the file cabinet
	// properly
	t4p::FileCabinetStoreClass store;
	t4p::FileCabinetItemClass item;
	
	wxPlatformInfo info;
	std::string fileToSave;
	if (info.GetOperatingSystemId() & wxOS_WINDOWS_NT) {
		fileToSave = "C:\\home\\user\\www\\config\\";
	}
	else {
		fileToSave = "/home/user/www/config/";
	}
	
	item.FileName.AssignDir(fileToSave);
	
	CHECK(store.Store(Session, item));
	
	// check that it has been inserted in the table
	int id;
	std::string name;
	std::string fullPath;
	soci::statement stmt = (Session.prepare << "SELECT file_cabinet_item_id, name, full_path FROM file_cabinet_items",
		soci::into(id), soci::into(name), soci::into(fullPath)
	);
	stmt.execute(true);
	CHECK_EQUAL("config", name);
	CHECK_EQUAL(fileToSave, fullPath);
	CHECK_EQUAL(id, item.Id);
}

TEST_FIXTURE(FileCabinetFixtureClass, DeleteItem) {
	
	// verify that we can delete items 
	// from the file cabinet items table
	Exec("BEGIN;");
	Exec("INSERT INTO file_cabinet_items(file_cabinet_item_id, name, full_path) VALUES(3, 'test.php', '/home/user/www/test.php');");
	Exec("INSERT INTO file_cabinet_items(file_cabinet_item_id, name, full_path) VALUES(4, 'config', '/home/user/www/config/');");
	Exec("COMMIT;");
	
	t4p::FileCabinetStoreClass store;
	CHECK(store.Delete(Session, 4));
	
	// make sure the correct item is removed from the db
	int count = -1;
	Session.once << "SELECT COUNT(*) FROM file_cabinet_items WHERE file_cabinet_item_id = 3",
		soci::into(count);
	CHECK_EQUAL(1, count);
	
	count = -1;
	Session.once << "SELECT COUNT(*) FROM file_cabinet_items WHERE file_cabinet_item_id = 4",
		soci::into(count);
	CHECK_EQUAL(0, count);
	
}

}