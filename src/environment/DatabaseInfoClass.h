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
 #ifndef __DATABASEINFOCLASS_H__
 #define __DATABASEINFOCLASS_H__
 
 #include <unicode/unistr.h>
 #include <soci.h>
 #include <vector>
 #include <wx/longlong.h>
 
 namespace mvceditor {

 class SqlQueryClass;
 
 /**
 * This is the database connection information used by the framework.
 * The information will actually be located via a PHP script
 * (MvcEditorFrameworkApp.php) and not the C++ code. this way we
 * can extend / modifify the code for different frameworks.
 */
class DatabaseInfoClass {

public:

	/**
	 * The RDBMS systems that both the frameworks and MVC Editor supports.
	 */
	enum Drivers {
		MYSQL
		
		// TODO: add support for these later
		// adding support means implementing meta data for SqlResourceFinderClass
		//POSTGRESQL,
		//SQLITE
	};
	
	/**
	 * The database host to connect to
	 */
	UnicodeString Host;
	
	/**
	 * The database user to connect as
	 */
	UnicodeString User;
	
	/**
	 * The database password
	 */
	UnicodeString Password;
	
	/**
	 * The database (schema name) to connect to
	 */
	UnicodeString DatabaseName;
	
	/**
	 * The full path to the database (in case of SQLite)
	 */
	UnicodeString FileName;
	
	/**
	 * A human friendly name for this info
	 */
	UnicodeString Name;
	
	/**
	 * the system that is used.
	 */
	Drivers Driver;
	
	/**
	 * The port to connect to
	 */
	int Port;
	
	DatabaseInfoClass();
	
	/**
	 * copy the attributes from src to this object.
	 */
	DatabaseInfoClass(const DatabaseInfoClass& other);
	
	/**
	 * copy the attributes from src to this object.
	 */
	void Copy(const DatabaseInfoClass& src);
	
	/**
	 * @return true if this info is the "equal to" another; equality is not
	 * based on pointers it is based on host and database name
	 */
	bool SameAs(const DatabaseInfoClass& other);
};

/**
 * a small structure used to pass results from the background thread to the main thread.
 * an instance of this struct will be created in the heap by the background thread,
 * and the foreground thread will delete it once it has been read.
 */
class SqlResultClass {
	
public:

	/**
	 * results are tied to a session. when reusing result objects you will need to call Init() method
	 * to allocate a new result.
	 */
	SqlResultClass();
	
	/**
	 * clean up resources
	 */
	virtual ~SqlResultClass();

	/**
	 * Dump all rows of the given statement into this result. Note that this method assumes that
	 * stmt.execute() already fetched the first row of the result.
	 *
	 * @param query the connection info
	 * @param session the active connection
	 * @param stmt the current statement that was executed.
	 * @param hasRows TRUE if the statement has results.
	 */
	void Init(mvceditor::SqlQueryClass& query, soci::session& session, soci::statement& stmt, bool hasRows);
	
	/**
	 * clean up resources. after a call to this object the statement has been cleaned up and is no longer valid.
	 * DO NOT ACCESS IT!
	 */
	void Close();

	/**
	 * Error string returned by the server.Can be empty string,
	 */
	UnicodeString Error;

	/**
	 * This variable is 'bound' to a statement; SOCI will put the results from the database into this variable.
	 */
	soci::row Row;
	
	/**
	 * The entire list result rows.
	 */
	std::vector<std::vector<UnicodeString> >StringResults;

	/**
	 * The names of the columns in the result.
	 */
	std::vector<UnicodeString> ColumnNames;
	
	/**
	 * The time that the query took to execute.
	 */
	wxLongLong QueryTime;
	
	/**
	 * The line number that the query was in
	 */
	int LineNumber;

	/**
	 * The number of records affected.  This is RDBMS dependant; don't count on it being always correct.
	 */
	int AffectedRows;
	
	/**
	 * If true then the query was sent successfully
	 * (this will be TRUE even if a query returned zero rows)
	 */
	bool Success;

	/**
	 * If true then result has rows that can iterated through. If this is false, then the
	 * query that was executed was a DML query (INSERT, UPDATE) or the query returned
	 * zero results.
	 */
	bool HasRows;
	
};

/**
 * A small class that will send queries to the database. Main purpose of this class is 
 * to wrap the soci API that uses exceptions into an API that does not
 * use exceptions.
 */
class SqlQueryClass {
	
	public:
	
	/**
	 * The database info to use when connecting
	 */
	DatabaseInfoClass Info;
	
	SqlQueryClass();
	
	/**
	 * Will only copy DatabaseInfo not results.
	 * Any statement that other has will NOT be copied
	 */
	SqlQueryClass(const SqlQueryClass& other);
	
	/**
	 * establish a connection to Info.
	 * @param session the connectino handle
	 * @param error if connection failed error string will be set here
	 * @return bool TRUE if connection was successfuly
	 */
	bool Connect(soci::session& session, UnicodeString& error);
	
	/**
	 * Close the connection and cleanup any results. This needs to be called
	 * after every call to Query()
	 */
	void Close(soci::statement& stmt);
	
	/**
	 * Close the connection and cleanup any results. This needs to be called
	 * after every call to Query()
	 */
	void Close(soci::session& session, soci::statement& stmt);
	
	/**
	 * Will only copy Host, User, Database, Port, and Password
	 * Any statement that other has will NOT be copied
	 * @param src the object to copy FROM
	 */
	void Copy(const SqlQueryClass& src);
	
	/**
	 * Sends ONE query to the server. Results can be traversed via the More() method
	 * 
	 * @param session the opened connection
	 * @param result the results of the query
	 * @param query to query to execute
	 * @return bool TRUE if query was successful (no connection errors)
	 */
	bool Execute(soci::session& session, SqlResultClass& result, const UnicodeString& query);
	
	/**
	 * Sends ONE query to the server. Results can be traversed via the More() method
	 * 
	 * @param session the opened connection
	 * @param stmt the statement that HAS ALREADY been prepared
	 * @param error if connection failed error string will be set here
	 * @return bool TRUE if connection was successfuly
	 */
	bool Execute(soci::statement& stmt, UnicodeString& error);
	
	/**
	 * This method can be used to check if a statement has data left.
	 * @return true if the stamement has rows in its result.
	 */
	bool GotData(soci::statement& stmt);
	
	/**
	 * Returns true if there are more results to be read. Also moves the result set cursor to 
	 * the next row
	 * @param stmt the statement that HAS ALREADY been executed
	 * @param hasError will be set to TRUE a fatal error ocurred
	 * @param error will be set to the error message
	 */
	bool More(soci::statement& stmt, bool& hasError, UnicodeString& error);
	
	/**
	 * Populates columnNames with the name of the columns in the current result.
	 * Returns false when there are no more results.
	 * 
	 * @param row the row of a result
	 * @param columnNames vector of column names will be populated
	 * @param error will be populated when an error ocurrs
	 * @return bool true on success, false on error
	 */
	bool ColumnNames(soci::row& row, std::vector<UnicodeString>& colummNames, UnicodeString& error);
	
	/**
	 * Populates columnNames with the name of the columns in the current result.
	 * Returns false when there are no more results.
	 * 
	 * ATTN: this is really not the best method for results that have floating points as 
	 * the caller will not have control over the precision that the floats will be displayed as.
	 * 
	 * @param row the row of a result
	 * @param columnValues vector of column values of the current row will be populated
	 * @param indicators vector of indicator for each column of the current row; can be used to find out if values are NULL
	 * @param error will be populated when an error ocurrs
	 * @return bool true on success, false on error
	 */
	bool NextRow(soci::row& row, std::vector<UnicodeString>& columnValues, std::vector<soci::indicator>& indicators, UnicodeString& error);
	
	/**
	 * Get the number of affected records.
	 */
	long long GetAffectedRows(soci::statement& stmt);
	
};

}

#endif