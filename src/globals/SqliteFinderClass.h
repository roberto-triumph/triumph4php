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

#ifndef T4P_SQLITEFINDERCLASS_H
#define T4P_SQLITEFINDERCLASS_H

#include <globals/SqliteResultClass.h>
#include <soci/soci.h>

namespace t4p {

/**
 * A SqliteFinderClass holds a reference of a soci database connection
 * and run many SqliteResultClass instances.
 *
 * Usage:
 * -----
 * The SqliteFinderClass is a very small class that delegates
 * most of its work to SqliteResultClass, take a look at that
 * class for more info.
 */
class SqliteFinderClass {

	public:

	/**
	 * use an existing connection for this finder.
	 * This method can used to have the the finder query either  a file-backed db or a memory db.
	 * By using an in-memory database, lookups are faster.
	 * The session does not necessarily have to be initialized at this point,
	 * but it should definitely be initialized before calling Prepare() or
	 * Exec()
	 *
	 * @param session the soci connection to use.
	 */
	SqliteFinderClass(soci::session& session);

	virtual ~SqliteFinderClass();

	/**
	 * prepares  a query against this sqlite db. There is no need to
	 * call this call this for one-time queries; as Exec() will prepare
	 * the query if its not prepared.  The advantage of calling Prepare()
	 * is that it can be called once, then SqliteResultClass::ReExec
	 * can be called as many times as needed in order to make queries
	 * performant.
	 *
	 * @param result contains the query to run and the parameters to bind
	 * @param doLimit if true then a limit will be added to the query
	 * @return bool TRUE if the query was successfully prepared
	 */
	bool Prepare(t4p::SqliteResultClass* result, bool doLimit);

	/**
	 * executes a query against this sqlite db.
	 *
	 * @param result contains the query to run and the parameters to bind
	 * @return bool TRUE if the query was successfully run
	 */
	bool Exec(t4p::SqliteResultClass* result);

protected:

	/**
	 * the opened connection to the detector databases.
	 */
	soci::session& Session;

};

}

#endif
