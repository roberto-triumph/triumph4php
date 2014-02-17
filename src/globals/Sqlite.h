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
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */

#ifndef __T4P_SQLITE_H__
#define __T4P_SQLITE_H__

#include <soci/soci.h>
#include <wx/filename.h>
#include <string>

namespace t4p {

/**
 * escape a value so that it is suitable for using in a LIKE SQL clause
 * ie. so that underscores, percents are treated literally
 * We escape ourselves because SOCI uses sqlite3_prepare instead of sqlite3_prepare_v2, and 
 * sqlite3_prepare will make LIKE queries not use the index. But this means we have to escape ourselves
 * see http://sqlite.org/optoverview.html (LIKE optimization)
 *
 * @param value the value to escape
 * @param c the character to USE for escaping. this should NOT be backslash,
 *        as this projet stores string that are likely to have backslashes
 *        (ie. PHP namespace names, file names) in the database.
 */
std::string SqliteSqlLikeEscape(const std::string& value, char c);

/**
 * Run multiple SQL statements that are located in fileName
 * using the given connection. Note that the database is completely
 * cleared (all tables are deleted) before this is run and any info is lost;
 * Tables are cleared so that the sql statements will never error out, 
 * for example this function is called
 * with a CREATE TABLE statement of a database that already has
 * a table.
 *
 * @param sqlScriptFileName location of SQL script
 * @param session the opened connection.  At the moment, this only works
 *        for SQLite connections
 * @param error if the sql statements error out, this string will be
 *        populated with the error message.
 *
 * @return bool TRUE if all statements where successfully executed;
 *         false on SQL error or if sql file does not exist.
 *
 * Exceptions are never thrown.
 */
bool SqliteSqlScript(const wxFileName& sqlScriptFileName, soci::session& session, wxString& error);

/**
 * Check the version number in the schem_version table.
 * This function will never throw an exception. On SQL exception, this
 * function will return 0 (an assert will be generated)
 *
 * @param session opened connection to detector db or tag db 
 * @return int the number in the schema_version table
 */
int SqliteSchemaVersion(soci::session& session);

/**
 * @param session opened connection
 * @param [out] vector of table names that exist in the opened connection. will be pushed into.
 * @param error will be filled in case of a db error.
 * @return bool TRUE if there was no db error.
 *
 * This function will never throw an exception. On SQL exception, this
 * function will return false (an assert will be generated)
 */
bool SqliteTables(soci::session& session, std::vector<std::string>& tableNames, wxString& error);

/**
 * set the busy timeout on the opened sqlite connection; the effect is that
 * when a sqlite db is locked the sqlite client api will sleep for a bit and then
 * re-attempt the query; this function should be called so that we get 
 * less soci exceptions due to sqlite complaining that the file is locked.
 *
 * @param session opened connection. MUST BE a SQLITE connection otherwise the program will crash!
 * @param timeoutMs the max number of milliseconds to wait for a locked file
 */
void SqliteSetBusyTimeout(soci::session& session, int timeoutMs = 100);

/**
 * Generic class that will encapsulate a single query, the prepared statement,
 * and the result. Subclassses will override the Prepare() method and 
 * create a statement for q query.
 */
class SqliteResultClass {

public:

	SqliteResultClass();
	virtual ~SqliteResultClass();

	/**
	 * in this method subclasses will build the SQL and execute it.
	 *
	 * @param session the connection.  must be around for as long as this result is alive.
	 * @param doLimit boolean if TRUE there should be a limit on the query
	 * @return bool subclasses should return TRUE if there is at least one result
	 */
	virtual bool Prepare(soci::session& session, bool doLimit) = 0;

	/**
	 * advance to the next row. in this method subclasses will construct their
	 * next object from the current DB row.
	 */
	virtual void Next() = 0;

	/**
	 * @return boolean TRUE if the query returned zero rows.
	 */
	bool Empty() const;

	/**
	 * @return boolean TRUE if there are more results to be iterated through
	 */
	bool More() const;


protected:

	/**
	 * run the statement in an exception-safe manner
	 *
	 * @param stmt this object will own the statement pointer
	 * @param error out parameter, will be set to the error message if one is encountered
	 * @return bool TRUE if statement was successfully run
	 */
	bool AdoptStatement(soci::statement* stmt, wxString& error);

	/**
	 * fetches the next row of the result.
	 * @param bool TRUE if there was another row to fetch
	 */
	bool Fetch();

private:

	/**
	 * the statement to iterate through
	 */
	soci::statement* Stmt;

	/**
	 * TRUE if the query returned zero rows.
	 * @var boolean
	 */
	bool IsEmpty;
};

/**
 * Class that holds connections to a SQLITE database. It will
 * close them when the class goes out of scope.
 */
class SqliteFinderClass {

public:
		
	SqliteFinderClass();

	virtual ~SqliteFinderClass();

	/**
	 * use an existing connection for this finder..
	 * This method can used to have the the finder query either  a file-backed db or a memory db.
	 * By using an in-memory database, lookups are faster.
	 * Note that this method assumes that the schema has already been created.
	 * 
	 * @param session the soci connection to use.  this class WILL NOT own the 
	 *        pointer,
	 */
	void InitSession(soci::session* session);

	/**
	 * removes the session
	 */
	void ClearSession();

	/**
	 * executes a query against this sqlite db.
	 *
	 * @param result contains the query to run and the parameters to bind
	 * @return bool TRUE if the query was successfully run
	 */
	bool Exec(t4p::SqliteResultClass* result);

protected:

	/**
	 * @return TRUE if this finder has an opened connection to a db
	 */
	bool IsInit() const;

	/**
	 * the opened connection to the detector databases.
	 * This class will NOT own the pointer.
	 */
	soci::session* Session;

};

}

#endif