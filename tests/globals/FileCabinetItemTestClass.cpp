
#include <UnitTest++.h>
#include <SqliteTestFixtureClass.h>
#include <globals/FileCabinetItemClass.h>
#include <wx/platinfo.h>

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
	Exec("BEGIN;");
	Exec("INSERT INTO file_cabinet_items(file_cabinet_item_id, name, full_path) VALUES(NULL, 'test.php', '/home/user/www/test.php');");
	Exec("INSERT INTO file_cabinet_items(file_cabinet_item_id, name, full_path) VALUES(NULL, 'config', '/home/user/www/config/');");
	Exec("COMMIT;");
	
	bool hasRows = SqliteFinder.Exec(&AllResults);
	CHECK(hasRows);
	AllResults.Next();
	CHECK_EQUAL(wxT("/home/user/www/test.php"), AllResults.Item.FileName.GetFullPath());
	
	CHECK(AllResults.More());
	AllResults.Next();
	CHECK_EQUAL(wxT("/home/user/www/config"), AllResults.Item.FileName.GetPath());
	
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