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
#ifndef SRC_LANGUAGE_SQL_SQLRESOURCEFINDERCLASS_H_
#define SRC_LANGUAGE_SQL_SQLRESOURCEFINDERCLASS_H_

#include <unicode/unistr.h>
#include <map>
#include <string>
#include <vector>
#include "language_sql/DatabaseTagClass.h"

namespace t4p {
/**
 * class that will open a connection to a database, get all of the tables
 * and columns, and store them in the Triumph resources schema.
 */
class SqlResourceFetchClass {
 public:
    /**
     *  @param soci::session the session that holds the connection
     *         to the Triumph tag cache
     */
    SqlResourceFetchClass(soci::session& session);

    /**
     * Connects to the given database and queries the table meta data
     * for the connection.
     *
     * @param info the connection parameters
     * @return bool false on error, error gets filled in with error message
     */
    bool Fetch(const DatabaseTagClass& info, UnicodeString& error);

    /**
     * delete all stored tables/column info
     */
    bool Wipe();

 private:
    /**
     * Connects to the given mysql database and queries the table meta data
     * for the connection.
     *
     * @param info the connection parameters
     * @return bool false on error, error gets filled in with error message
     */
    bool FetchMysql(const DatabaseTagClass& info, UnicodeString& error);

    /**
     * Connects to the given sqlite database and queries the table meta data
     * for the connection.
     *
     * @param info the connection parameters
     * @return bool false on error, error gets filled in with error message
     */
    bool FetchSqlite(const DatabaseTagClass& info, UnicodeString& error);

    bool StoreTables(const std::string& hash, const std::vector<std::string>& tables);

    bool StoreColumns(const std::string& hash, const std::vector<std::string>& columns);

    /**
     * connection where to store the fetched tables/columns
     */
    soci::session& Session;
};

/**
 * Performs a prefix lookup on table names
 */
class SqlResourceTableResultClass : public t4p::SqliteResultClass {
 public:
    /**
     * The matched table name
     */
    std::string TableName;

    /**
     * The name of the connection that the table was found in
     */
    std::string Connection;

    SqlResourceTableResultClass();

    /**
     * set the (partial) name to lookup
     */
    void SetLookup(const wxString& lookup, const std::string& connectionHash);

    /**
     * get the next result. the result can be read from the
     * TableName, Connection members
     */
    void Next();

 protected:
    /**
     * build the SQL to query for tables and bind the input
     * parameters.
     *
     * @param stmt the statement to prepare and bind input parameters to.
     * @param doLimit boolean if TRUE there should be a limit on the query
     * @return bool TRUE if the statement was prepared
     *         successfully.  A false here means a connection failure or bad sql
     */
    bool DoPrepare(soci::statement& stmt, bool doLimit);

    /**
     * binds the statement result to
     * the class members
     */
    void DoBind(soci::statement& stmt);

 private:
    std::string Lookup;

    std::string LookupEnd;

    std::string ConnectionHash;
};

/**
 * Performs an exact match lookup on table names
 */
class ExactSqlResourceTableResultClass : public t4p::SqliteResultClass {
 public:
    /**
     * The matched table name
     */
    std::string TableName;

    /**
     * The name of the connection that the table was found in
     */
    std::string Connection;

    ExactSqlResourceTableResultClass();

    /**
     * set the (partial) name to lookup
     */
    void SetLookup(const wxString& lookup, const std::string& connectionHash);

    /**
     * get the next result. the result can be read from the
     * TableName, Connection members
     */
    void Next();

 protected:
    /**
     * build the SQL to query for tables and bind the input
     * parameters.
     *
     * @param stmt the statement to prepare and bind input parameters to.
     * @param doLimit boolean if TRUE there should be a limit on the query
     * @return bool TRUE if the statement was prepared
     *         successfully.  A false here means a connection failure or bad sql
     */
    bool DoPrepare(soci::statement& stmt, bool doLimit);

    /**
     * binds the output columns to the TableName string instance
     * variable
     */
    void DoBind(soci::statement& stmt);

 private:
    std::string Lookup;

    std::string ConnectionHash;
};

/**
 * Performs a prefix lookup on column names
 */
class SqlResourceColumnResultClass : public t4p::SqliteResultClass {
 public:
    /**
     * The matched column name
     */
    std::string ColumnName;

    SqlResourceColumnResultClass();

    /**
     * set the (partial) name to lookup
     */
    void SetLookup(const wxString& lookup, const std::string& connectionHash);

    /**
     * get the next result. the result can be read from the
     * ColumnName member
     */
    void Next();

 protected:
    /**
     * prepares and binds the query
     */
    bool DoPrepare(soci::statement& stmt, bool doLimit);

    /**
     * adopts the statement and binds the statement result to
     * the class members
     */
    void DoBind(soci::statement& stmt);

 private:
    std::string Lookup;

    std::string LookupEnd;

    std::string ConnectionHash;
};

/**
 * This class is used to make read-only queries to the
 * resources sqlite db to get all of the sql metadata
 * for the database connections that the user configured
 */
class SqlResourceFinderClass : public t4p::SqliteFinderClass {
 public:
    SqlResourceFinderClass(soci::session& session);

    /**
     * @param info the connection to search in. only tables from this connection will be returned
     * @param partialTableName table name to search for
     * @return vector<UnicodeString> all table names that start with the partial table name
     * returned table names will be sorted in ascending order
     */
    std::vector<UnicodeString> FindTables(const DatabaseTagClass& info, const UnicodeString& partialTableName);

    /**
     * Searches ALL columns of ALL tables.
     * @param info the connection to search in. only tables from this connection will be returned
     * @param partialColumnName column name to search for
     * @return vector<UnicodeString> all table names that start with the partial table name
     * will return empty vector when table name is empty
     * returned column names will be sorted in ascending order
     */
    std::vector<UnicodeString> FindColumns(const DatabaseTagClass& info, const UnicodeString& partialColumnName);
};
}  // namespace t4p

#endif  // SRC_LANGUAGE_SQL_SQLRESOURCEFINDERCLASS_H_
