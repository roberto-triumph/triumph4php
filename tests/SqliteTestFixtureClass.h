/*
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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef __SQLITETESTFIXTURECLASS_H__
#define __SQLITETESTFIXTURECLASS_H__

#include <soci.h>
#include <string>
#include <wx/filename.h>

/**
 * This is a test fixture that will initialize an SQLITE in-memory database. This 
 * fixture is useful for the tests that deal with tag searching and tag parsing.
 */
class SqliteTestFixtureClass {

public:

	SqliteTestFixtureClass();
	
	/**
	 * cleans up the database
	 */
	virtual ~SqliteTestFixtureClass();
	
	/**
	 * creates the given database. WILL
	 * throw an exception if could it not connect to database or create
	 * the database.
	 * @param seesion must be an opened connection to a sqlite db
	 * @param sqlScriptFile file that contains sql to execute on the session
	 */
	void CreateDatabase(soci::session& session, const wxFileName& sqlScriptFile);
	
	/**
	 * executes the given query. WILL
	 * throw an exception if could it not connect to database or execute the query.
	 * @return bool TRUE if query was successful
	 */
	bool Exec(const std::string& query);

protected:

	/**
	 * The active connection
	 */
	soci::session Session;
	
	/**
	 * The connection info
	 */
	std::string ConnectionString;
 };
 
 #endif
