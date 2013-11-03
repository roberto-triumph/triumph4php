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
 
 #include <globals/DatabaseTagClass.h>
 #include <unicode/unistr.h>
 #include <vector>
 #include <map>
 
namespace mvceditor {

/**
 * Case-sensitive string comparator for use as STL Predicate
 */
class SqlResourceFinderUnicodeStringComparatorClass {
public:
	bool operator()(const UnicodeString& str1, const UnicodeString& str2) const {
		return (str1.compare(str2) < (int8_t)0) ? true : false;
	}
};

class SqlResourceClass {

	public:
	
	/**
	 * a lowercased version useful for sorting and searching. Searches
	 * will be made against this value.
	 */
	UnicodeString Key;
	
	/**
	 * The name of the resource. This is the name that will be returned
	 * to the callers.
	 */
	UnicodeString Name;
	
	SqlResourceClass(const UnicodeString& name);
	
	
	bool operator<(const SqlResourceClass& other) const;
	
	bool operator==(const SqlResourceClass& other) const;
	
};

class SqlResourceFinderClass {
	
public:

	SqlResourceFinderClass();

	/**
	 * copy the internal resources from src into this object.
	 *
	 * @param src the object to copy from.
	 */
	void Copy(const SqlResourceFinderClass& src);
	
	/**
	 * Connects to the given database and queries the table meta data
	 * for the connection.
	 * 
	 * @param info the connection parameters
	 * @return bool false on error, error gets filled in with error message
	 */
	bool Fetch(const DatabaseTagClass& info, UnicodeString& error);
	
	/**
	 * @param info the connection to search in. only tables from this connection will be returned
	 * @param partialTableName table name to search for
	 * @return vector<UnicodeString> all table names that start with the partial table name
	 * returned table names will be sorted in ascending order
	 */
	std::vector<UnicodeString> FindTables(const DatabaseTagClass& info, const UnicodeString& partialTableName);
	
	/**
	 * Searches ALL columns of ALL tables.
	 * @param info the connection to search in. only tables from this connection will be returned
	 * @param partialColumnName column name to search for
	 * @return vector<UnicodeString> all table names that start with the partial table name
	 * will return empty vector when table name is empty
	 * returned column names will be sorted in ascending order
	 */
	std::vector<UnicodeString> FindColumns(const DatabaseTagClass& info, const UnicodeString& partialColumnName);
	
	private:

	/**
	 * turn the info into a string that way we can link tables to a connection and we don't 
	 * have to keep a pointer to the info around.
	 */
	UnicodeString Hash(const DatabaseTagClass& info);
	
	/**
	 * Connects to the given mysql database and queries the table meta data
	 * for the connection.
	 * 
	 * @param info the connection parameters
	 * @return bool false on error, error gets filled in with error message
	 */
	bool FetchMysql(const DatabaseTagClass& info, UnicodeString& error);
	
	/**
	 * Connects to the given sqlite database and queries the table meta data
	 * for the connection.
	 * 
	 * @param info the connection parameters
	 * @return bool false on error, error gets filled in with error message
	 */
	bool FetchSqlite(const DatabaseTagClass& info, UnicodeString& error);
	
	/**
	 * To make the queries
	 */
	SqlQueryClass Query;
	
	/**
	 * To keep the tables linked to a specific connection
	 * key will be the info hash, value will be the list of table for that info
	 */
	std::map<UnicodeString, std::vector<SqlResourceClass>, SqlResourceFinderUnicodeStringComparatorClass> Tables;
	
	/**
	 * To keep the columns linked to a specific connection AND table
	 * key will be info hash + table name, value will be the list
	 * of columns for that table
	 * comparator is needed for MSW compiler
	 */
	std::map<UnicodeString, std::vector<SqlResourceClass>, SqlResourceFinderUnicodeStringComparatorClass> Columns;
	
};
	 
 }
 #endif