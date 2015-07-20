/**
 * @copyright  2009-2011 Roberto Perpuly
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
#ifndef T4P_DATABASETAGCLASS_H
#define T4P_DATABASETAGCLASS_H

#include <unicode/unistr.h>
#include <globals/Sqlite.h>
#include <globals/SqliteFinderClass.h>
#include <vector>
#include <wx/longlong.h>
#include <wx/thread.h>
#include <wx/filename.h>

namespace t4p {
// defined below
class SqlQueryClass;
class ConnectionIdentifierClass;

/**
 * This is the database connection information used by the framework.
 * The information will actually be located via a PHP script
 * (MvcEditorFrameworkApp.php) and not the C++ code. this way we
 * can extend / modifify the code for different frameworks.
 */
class DatabaseTagClass {
	public:
	/**
	 * The RDBMS systems that both the frameworks and Triumph supports.
	 */
	enum Drivers {
		MYSQL,
		SQLITE
	};

	/**
	 * The human friendly label for this info
	 */
	UnicodeString Label;

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
	UnicodeString Schema;

	/**
	 * The full path to the database (in case of SQLite)
	 */
	wxFileName FileName;

	/**
	 * the system that is used.
	 */
	Drivers Driver;

	/**
	 * The port to connect to
	 */
	int Port;

	/**
	 * True if this connection was detected by a DatabaseTagDetector. If this flag
	 * is set the connection will be treated specially (it will not be persisted
	 * to the config as it can be re-detected).
	 */
	bool IsDetected;

	/**
	 * TRUE if the user wants to use this connection.  The user may not want
	 * to use the connection info but they may want to keep it around for later
	 * use
	 */
	bool IsEnabled;

	DatabaseTagClass();

	/**
	 * copy the attributes from src to this object.
	 */
	DatabaseTagClass(const t4p::DatabaseTagClass& other);

	/**
	 * copy the attributes from src to this object.
	 */
	void Copy(const t4p::DatabaseTagClass& src);

	/**
	 * @return true if this info is the "equal to" another; equality is not
	 * based on pointers it is based on host and database name
	 */
	bool SameAs(const t4p::DatabaseTagClass& other);

	/**
	 * @return UnicodeString a connection hash; a string that uniquely
	 *         identifies this connection
	 */
	UnicodeString ConnectionHash() const;
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
	 * @param sqlString the SQL that was excuted
	 * @param hasRows TRUE if the statement has results.
	 */
	void Init(t4p::SqlQueryClass& query, soci::session& session, soci::statement& stmt, const UnicodeString& sqlString,
		bool hasRows);

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
	 * The names of the columns in the result.
	 */
	std::vector<UnicodeString> TableNames;

	/**
	 * The query that was executed.
	 */
	UnicodeString Query;

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
	DatabaseTagClass DatabaseTag;

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
	 * @param session the current open session
	 * @param the connection ID, used to kill a running query
	 */
	void ConnectionIdentifier(soci::session& session,
		t4p::ConnectionIdentifierClass& connectionIdentifier);

	/**
	 * kills the connection (stops any query). this method is useful when
	 * a query is executed in a background thread and we want to use the
	 * main thread to kill the query; but we want to kill the query in a
	 * nice way and avoid using wxThread::Kill
	 */
	bool KillConnection(soci::session& session,
						  t4p::ConnectionIdentifierClass& connectionIdentifier,
						  UnicodeString& error);

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
	 * Populates columnNames with the name of the tables in the current result.
	 * Returns false when there are no more results.
	 * Note: the tableNames vector may be populated with empty strings
	 * if the query contains an expression
	 *
	 * @param session the connection
	 * @param stmt the prepared statement
	 * @params columnCount the number of columns in the result
	 * @param tableNames vector of table names will be populated
	 * @param error will be populated when an error ocurrs
	 * @return bool true on success, false on error
	 */
	bool TableNames(soci::session& session, soci::statement& stmt, int columnCount,
		std::vector<UnicodeString>& tableNames, UnicodeString& error);

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

	private:
	/**
	 * establish a connection to mysql.
	 * @param session the connection handle
	 * @param error if connection failed error string will be set here
	 * @return bool TRUE if connection was successfuly
	 */
	bool ConnectMysql(soci::session& session, UnicodeString& error);

	/**
	 * establish a connection to sqlite
	 * @param session the connection handle
	 * @param error if connection failed error string will be set here
	 * @return bool TRUE if connection was successfuly
	 */
	bool ConnectSqlite(soci::session& session, UnicodeString& error);
};

/**
 * class that will share the connection ID between the main thread
 * and the query thread in a safe manner.  the main thread will
 * use the connection ID to issue a KILL SQL command when the
 * app exits or the panel is closed.
 */
class ConnectionIdentifierClass {
	public:
	ConnectionIdentifierClass();

	/**
	 * safely set the connection ID
	 */
	void Set(unsigned long id);


	/**
	 * safely get the connection ID
	 */
	unsigned long Get();

	private:
	/**
	 * for shared access protection
	 */
	wxMutex Mutex;

	unsigned long ConnectionId;
};

class DatabaseTagFinderClass : public t4p::SqliteFinderClass {
	public:
	DatabaseTagFinderClass(soci::session& session);

	/**
	 * @return all of the database tags in all of the attached databases
	 */
	std::vector<t4p::DatabaseTagClass> All(const std::vector<wxFileName>& sourceDirectories);
};
}

#endif
