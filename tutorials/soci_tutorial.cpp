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
 #include <stdio.h>
 #include <mysql/mysql.h>
 
 /**
  * This is an example that will query a MySQL database using the SOCI library. The example
  * will attempt to connect to MySQL and execute a query to get the list of databases.
  * This example will use dynamic binding (where the number and types of columns is not
  * known at compile time; as in a "SELECT * FROM users type query).
  */
 void query() {
	 try {
		soci::session session(soci::mysql, "db=mysql user=root password='' ");
		std::string query = "-- a query\n SHOW DATABASES;";
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

int main() {
	query();
	
	// to cleanup any state to remove valgrind warnings
	mysql_library_end();
}