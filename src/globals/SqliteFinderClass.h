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

#ifndef __T4P_SQLITEFINDERCLASS_H__
#define __T4P_SQLITEFINDERCLASS_H__

#include <globals/SqliteResultClass.h>

namespace t4p {

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
	 * prepares  a query against this sqlite db.
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
