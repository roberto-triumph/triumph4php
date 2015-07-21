/*
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
#ifndef TESTS_DATABASETESTFIXTURECLASS_H_
#define TESTS_DATABASETESTFIXTURECLASS_H_

#include <soci.h>
#include <unicode/ustream.h>  // get the << overloaded operator, needed by UnitTest++
#include <string>

/**
 * This is a fxture class that can be used for unit tests that need to have database
 * calls. This class will connect to the database and create a test database upon
 * object creation.
 * Note that the user/password credentials come from a set of macros (T4P_DB_PASSWORD
 * and T4P_DB_USER.
 */
class DatabaseTestFixtureClass {
 public:
    /**
     * Connects to the database and creates a test database
     * This method may throw an exception if it could not connect
     * to the database or create the test database
     */
    DatabaseTestFixtureClass(const std::string& testDatabaseName);

    /**
     * disconnects from the database
     */
    ~DatabaseTestFixtureClass();

    /**
     * drops the given database if it exists. will not
     * throw an exception if database does not exist. WILL
     * throw an exception if could it not connect to database.
     */
    void DropDatabase(const std::string& database);

    /**
     * creates the given database. WILL
     * throw an exception if could it not connect to database or create
     * the database.
     * @return bool TRUE if query was successful
     */
    bool CreateDatabase(const std::string& name);

    /**
     * executes the given query. WILL
     * throw an exception if could it not connect to database or execute the query.
     * @return bool TRUE if query was successful
     */
    bool Exec(const std::string& query);

    /**
     * Returns the user name, as configured by the T4P_DB_USER macro
     */
    std::string UserName() const;

    /**
     * Returns the user name, as configured by the T4P_DB_PASSWORD macro
     */
    std::string Password() const;

 private:
    /**
     * The active connection
     */
    soci::session Session;

    /**
     * The connection info
     */
    std::string ConnectionString;
};

#endif  // TESTS_DATABASETESTFIXTURECLASS_H_
