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

#ifndef __MVCEDITOR_SQLITE_H__
#define __MVCEDITOR_SQLITE_H__

#include <soci/soci.h>
#include <wx/filename.h>
#include <string>

namespace mvceditor {

/**
 * escape a value so that it is suitable for using in a LIKE SQL clause
 * ie. so that an underscore is treated literally and not as a wildcard
 * @param value the value to escape
 * @param c the character to USE for escaping. this should NOT be backslash,
 *        as this projet stores string that are likely to have backslashes
 *        (ie. PHP namespace names, file names) in the database.
 */
std::string SqliteSqlEscape(const std::string& value, char c);

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
 * Class that holds connections to all attached databases. It will
 * close them when the class goes out of scope.
 */
class SqliteFinderClass {

public:
		
	SqliteFinderClass();

	virtual ~SqliteFinderClass();

	/**
	 * opens the sqlite file at the given file location
	 * This can be called multiple times safely. If the filename does 
	 * not exist, this method will return FALSE.
	 *
	 * @param fileName the location of the detectors sqlite database.
	 *        if fileName does not exist, it will be created.
	 * @return bool TRUE if cache could be successfully opened / created
	 */
	bool AttachExistingFile(const wxFileName& fileName);

	/**
	 * use an existing connection and adopt it as our own.
	 *
	 * @param session the soci connection to use.  this class WILL own the 
	 *        pointer and delete it when this object goes out of scope
	 */
	void AdoptSession(soci::session* session);

	/**
	 * Closes the opened connections; but the backing databases are left intact.
	 */
	void Close();

protected:

	/**
	 * the opened connection to the detector databases.
	 */
	std::vector<soci::session*> Sessions;

};

}

#endif