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

#ifndef T4P_SQLITE_H
#define T4P_SQLITE_H

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
 * Get the ID of the last insert, useful for auto incremented primary keys
 *
 * @param stmt the insert statement that was executed
 * @return int the last insert ID, -1 if there's an error
 */
int SqliteInsertId(soci::statement& stmt);

/**
 * create the database connection to the given db in an exception-safe
 * way. Additionally, we set a query timeout so that we cancel long-running
 * queries.
 *
 * @param session the db connection to open.
 * @param wxString dbName, given to SQLite.  db can be a full path to a file  The
 *        file neeeds to exist and have been initialized with the schema
 * @return bool TRUE if db connection was opened.
 */
bool SqliteOpen(soci::session& session, const wxString& dbName);
}

#endif
