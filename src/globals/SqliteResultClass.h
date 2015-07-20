/**
 * @copyright  2012 Roberto Perpuly
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

#ifndef SRC_GLOBALS_SQLITERESULTCLASS_H_
#define SRC_GLOBALS_SQLITERESULTCLASS_H_

#include <soci/soci.h>
#include <wx/string.h>

namespace t4p {
/**
 * Generic class that will encapsulate a single query, the prepared statement,
 * and the result. Subclassses will override the DoPrepare(), DoBind(), and Next()
 * methods in order to create specific queries.
 *
 * Usage
 * -----
 *
 * There are 2 ways of using a SqliteResultClass
 * 1. Use Exec(); this method creates a prepared statement and executes the
 *    query in one shot
 * 2. Call Init(), then call ReExec(); Init() creates a prepared statement
 *    and ReExec changes the varaibles bound to the result set. This way
 *    of using a SqliteResult ensures that a prepared statement is only
 *    created once and re-used multiple times; this is the preferred method
 *    to perform many lookup queries.
 *
 * Result Rows Iteration
 * ---------------------
 * Call More() as the loop condition, then Next() to get the results into
 * the bound variables; like so:
 *
 *   while (result.More()) {
 *     result.Next();
 *     // at this this point the variables bound to the soci
 *     // statement have been updated and can be used
 *   }
 *
 */
class SqliteResultClass {
	public:
	SqliteResultClass();
	virtual ~SqliteResultClass();

	/**
	 * initializes this result by preparing the statement. Init
	 * creates the statement in an exception-safe manner (no exceptions
	 * are thrown from this method)
	 *
	 * @param session the connection.  must be around for as long as this result is alive.
	 * @param doLimit boolean if TRUE there should be a limit on the query
	 * @return bool TRUE if statement was prepare.  False if there was a connection
	 *         failure, or invalid sql.
	 */
	bool Init(soci::session& session, bool doLimit);

	/**
	 * will prepare the SQL if its not already prepared, and will execute the statement.
	 * the statement is created / executed in an exception-safe manner (no execeptions
	 * are thrown from this method)
	 *
	 * @param session the connection.  must be around for as long as this result is alive.
	 * @param doLimit boolean if TRUE there should be a limit on the query
	 * @return bool will return TRUE if there is at least one result row
	 */
	bool Exec(soci::session& session, bool doLimit);

	/**
	 * advance to the next row. in this method subclasses will construct their
	 * next object from the current DB row.
	 */
	virtual void Next() = 0;

	/**
	 * @return boolean TRUE if this result has been prepared
	 */
	bool IsOk() const;

	/**
	 * @return boolean TRUE if the query returned zero rows.
	 */
	bool Empty() const;

	/**
	 * @return boolean TRUE if there are more results to be iterated through
	 */
	bool More() const;

	/**
	 * Executes the prepared statement again
	 * This should be called if the variables that were bound to the
	 * statement were changed and the query need to be executed again
	 *
	 * @param error out parameter, will be set to the error message if one is encountered
	 * @param bool TRUE if the statement was successfully run
	 */
	bool ReExec(wxString& error);

	protected:
	/**
	 * in this method subclasses will build the SQL and bind the input
	 * parameters as needed.
	 *
	 * @param stmt the statement to prepare and bind input parameters to.
	 * @param doLimit boolean if TRUE there should be a limit on the query
	 * @return bool subclasses should return TRUE if the statement was prepared
	 *         successfully.  A false here means a connection failure or bad sql
	 */
	virtual bool DoPrepare(soci::statement& stmt, bool doLimit) = 0;

	/**
	 * in this method subclasses will bind output parameters (columns of
	 * the sql result set to instance variables).
	 *
	 * @param stmt the statement to bind the output parameters to.
	 */
	virtual void DoBind(soci::statement& stmt) = 0;

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

	/**
	 * TRUE if the entire result set has been fetched.
	 * @var boolean
	 */
	bool IsFetched;
};
}

#endif  // SRC_GLOBALS_SQLITERESULTCLASS_H_
