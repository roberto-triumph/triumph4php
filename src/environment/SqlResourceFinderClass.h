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
 #ifndef _SQLRESOURCEFINDERCLASS_H__
 #define _SQLRESOURCEFINDERCLASS_H__
 
 #include <environment/DatabaseInfoClass.h>
 #include <unicode/unistr.h>
 #include <vector>
 #include <map>
 
 namespace mvceditor {
	 
class SqlResourceFinderClass {
	
public:

	SqlResourceFinderClass();
	
	/**
	 * Connects to the given database and queries the table meta data
	 * for the connection.
	 * 
	 * @param info the connection parameters
	 * @return bool false on error, error gets filled in with error message
	 */
	bool Fetch(const DatabaseInfoClass& info, UnicodeString& error);
	
	/**
	 * @param info the connection to search in. only tables from this connection will be returned
	 * @param partialTableName table name to search for
	 * @return vector<UnicodeString> all table names that start with the partial table name
	 * returned table names will be sorted in ascending order
	 */
	std::vector<UnicodeString> FindTables(const DatabaseInfoClass& info, const UnicodeString& partialTableName);
	
	/**
	 * @param info the connection to search in. only tables from this connection will be returned
	 * @param tableName only columns from this table will be returned. tableName cannot be empty
	 * @param partialColumnName column name to search for
	 * @return vector<UnicodeString> all table names that start with the partial table name
	 * will return empty vector when table name is empty
	 * returned column names will be sorted in ascending order
	 */
	std::vector<UnicodeString> FindColumns(const DatabaseInfoClass& info, const UnicodeString& tableName, const UnicodeString& partialColumnName);
	
	private:

	/**
	 * to prevent nested templates
	 * gives warning about '>>>' being an operator in C++ 0x
	 */
	typedef std::vector<UnicodeString> UnicodeStringVector;

	/**
	 * turn the info into a string that way we can link tables to a connection and we don't 
	 * have to keep a pointer to the info around.
	 */
	UnicodeString Hash(const DatabaseInfoClass& info);
	
	/**
	 * turn the info AND table into a string that way we can link tables to a connection and we don't 
	 * have to keep a pointer to the info around. This hash is useful to quickly lookup
	 * columns.
	 */
	UnicodeString Hash(const DatabaseInfoClass& info, const UnicodeString& tableName);
	
	/**
	 * To make the queries
	 */
	SqlQueryClass Query;
	
	/**
	 * To keep the tables linked to a specific connection
	 * key will be the info hash, value will be the list of table for that info
	 */
	std::map<UnicodeString, UnicodeStringVector> Tables;
	
	/**
	 * To keep the columns linked to a specific connection AND table
	 * key will be info hash + table name, value will be the list
	 * of columns for that table
	 */
	std::map<UnicodeString, UnicodeStringVector> Columns;
	
};
	 
 }
 #endif