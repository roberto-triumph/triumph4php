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
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <DatabaseTestFixtureClass.h>
#include <soci-mysql.h>
#include <stdio.h>
 
 
DatabaseTestFixtureClass::DatabaseTestFixtureClass(const std::string& testDatabaseName)
	: Session()
	, ConnectionString("host=127.0.0.1 port=3306 user=root ") {
	Session.open(*soci::factory_mysql(), (ConnectionString + "db=mysql").c_str());
	DropDatabase(testDatabaseName);
	CreateDatabase(testDatabaseName);
	Exec("USE " + testDatabaseName);
}
	
DatabaseTestFixtureClass::~DatabaseTestFixtureClass() {
	Session.close();
}

void DatabaseTestFixtureClass::DropDatabase(const std::string& databaseName) {
	std::string query("DROP DATABASE IF EXISTS ");
	query += databaseName;
	Exec(query);
}

bool DatabaseTestFixtureClass::CreateDatabase(const std::string& name) {
	std::string query("CREATE DATABASE ");
	query += name;
	return Exec(query);
}

bool DatabaseTestFixtureClass::Exec(const std::string& query) {
	soci::statement stmt = (Session.prepare << query);
	try {
		
		// execute returns true only when there are results; we want to return
		// true on success
		stmt.execute(false);
		return true;
	} catch (std::exception& e) {
		printf("exception=%s\n", e.what());
	}
	return false;
}