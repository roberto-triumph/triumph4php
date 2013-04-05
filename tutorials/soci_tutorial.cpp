/**
 * The MIT License
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
#include <soci/soci.h>
#include <soci/mysql/soci-mysql.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <stdio.h>
 
/**
 * This is an example that will query a MySQL and a SQLite3 database using the SOCI library. The example
 * will attempt to connect to MySQL and execute a query to get the list of databases.
 * This example will use dynamic binding (where the number and types of columns is not
 * known at compile time; as in a "SELECT * FROM users type query).
 */
void queryMysql() {
	 try {
		soci::session session(*soci::factory_mysql(), "db=mysql user=root password='' host=127.0.0.1 ");
		std::string query = "SHOW DATABASES;";
		printf("querying MySQL: %s\n", query.c_str());

		soci::statement stmt(session);
		stmt.alloc();
		stmt.prepare(query);
		stmt.define_and_bind();
		soci::row row;
		stmt.exchange_for_rowset(soci::into(row));
		bool good = stmt.execute(true);
		if (good) {
			const soci::column_properties& props = row.get_properties(0);
			printf("%s\n", props.get_name().c_str());
		}
		while (good) {
			printf("%s\n", row.get<std::string>(0).c_str());
			good = stmt.fetch();
		}
		stmt.clean_up();
		session.close();
	}
	catch (std::exception const& e) {
		printf("Error: %s\n", e.what());
	}
}

/**
 * This is an example that will query a SQLite3 database using the SOCI library. The example
 * will create an SQLite3 database in memory, create a table, insert 2 rows, and then execute a 
 * query to get all of the inserted rows.
 * This example will use dynamic binding (where the number and types of columns is not
 * known at compile time; as in "SELECT * FROM" type query).
 */
void querySqlite() {
	try {
		std::string sqliteFile = ":memory:";
		soci::session session(*soci::factory_sqlite3(), sqliteFile);
		session.once << "CREATE TABLE users(id int, name varchar(255));";
		session.once << "INSERT INTO users(id, name) VALUES(1, 'John');";
		session.once << "INSERT INTO users(id, name) VALUES(2, 'Ron');";
		
		std::string query = "SELECT id, name FROM users;";
		printf("querying SQLite3: %s\n", query.c_str());
		soci::statement stmt(session);
		stmt.alloc();
		stmt.prepare(query);
		stmt.define_and_bind();
		soci::row row;
		stmt.exchange_for_rowset(soci::into(row));
		bool good = stmt.execute(true);
		if (good) {
			soci::column_properties props = row.get_properties(0);
			printf("%s\t", props.get_name().c_str());
			props = row.get_properties(1);
			printf("%s\n", props.get_name().c_str());
		}
		while (good) {
			printf("%d\t%s\n", row.get<int>(0), row.get<std::string>(1).c_str());
			good = stmt.fetch();
		}
		stmt.clean_up();
		session.close();
	}
	catch (std::exception const& e) {
		printf("Error: %s\n", e.what());
	}
}

int main() {
	queryMysql();
	querySqlite();
}