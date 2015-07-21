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
#include "language_sql/DatabaseTagClass.h"
#include <soci/mysql/soci-mysql.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <unicode/ustdio.h>
#include <wx/datetime.h>
#include <algorithm>
#include <string>
#include <vector>
#include "globals/Errors.h"
#include "globals/String.h"

t4p::DatabaseTagClass::DatabaseTagClass()
	: Label()
	, Host()
	, User()
	, Password()
	, Schema()
	, FileName()
	, Driver(MYSQL)
	, Port()
	, IsDetected(false)
	, IsEnabled(true) {
}

t4p::DatabaseTagClass::DatabaseTagClass(const t4p::DatabaseTagClass& other)
	: Label()
	, Host()
	, User()
	, Password()
	, Schema()
	, FileName()
	, Driver(MYSQL)
	, Port()
	, IsDetected(false)
	, IsEnabled(true) {
	Copy(other);
}

void t4p::DatabaseTagClass::Copy(const t4p::DatabaseTagClass& src) {
	Label = src.Label;
	Host = src.Host;
	User = src.User;
	Password = src.Password;
	Schema = src.Schema;
	FileName.Assign(src.FileName.GetFullPath());
	Driver = src.Driver;
	Port = src.Port;
	IsDetected = src.IsDetected;
	IsEnabled = src.IsEnabled;
}

bool t4p::DatabaseTagClass::SameAs(const t4p::DatabaseTagClass& other) {
	return Host.caseCompare(other.Host, 0) == 0 && Schema.caseCompare(other.Schema, 0) == 0;
}

UnicodeString t4p::DatabaseTagClass::ConnectionHash() const {
	UnicodeString hash = Host + UNICODE_STRING_SIMPLE("--") + Schema + UNICODE_STRING_SIMPLE("---") +
		t4p::WxToIcu(FileName.GetFullPath());
	return hash;
}

t4p::SqlResultClass::SqlResultClass()
	: Error()
	, Row()
	, StringResults()
	, ColumnNames()
	, TableNames()
	, Query()
	, QueryTime()
	, LineNumber(0)
	, AffectedRows(0)
	, Success(false)
	, HasRows(false) {
}

t4p::SqlResultClass::~SqlResultClass() {
	Close();
}

void t4p::SqlResultClass::Close() {
	ColumnNames.clear();
	TableNames.clear();
	StringResults.clear();
	AffectedRows = 0;
	HasRows = false;
	Success = false;
}

void t4p::SqlResultClass::Init(t4p::SqlQueryClass& query, soci::session& session, soci::statement& stmt,
									 const UnicodeString& sqlString, bool hasRows) {
	ColumnNames.clear();
	TableNames.clear();
	StringResults.clear();
	Query = sqlString;
	std::vector<soci::indicator> columnIndicators;
	HasRows = hasRows;
	AffectedRows = query.GetAffectedRows(stmt);
	if (Success && HasRows && query.ColumnNames(Row, ColumnNames, Error)
			&& query.TableNames(session, stmt, Row.size(), TableNames, Error)) {
		bool more = true;
		bool hasError = false;

		// perform a DO ... WHILE loop since we called statement.execute() method [in SqlQueryClass.Execute() ]
		// with TRUE so it will fetch the first row for us
		do {
			std::vector<UnicodeString> columnValues;
			more = query.NextRow(Row, columnValues, columnIndicators, Error);
			if (more) {
				for (size_t colNumber = 0; colNumber < columnValues.size(); colNumber++) {
					if (columnIndicators[colNumber] == soci::i_null) {
						columnValues[colNumber] = UNICODE_STRING_SIMPLE("<NULL>");
					}
				}
				StringResults.push_back(columnValues);
			}
			columnIndicators.clear();
		} while (more && query.More(stmt, hasError, Error) && !hasError);
	}
}

t4p::SqlQueryClass::SqlQueryClass()
	: DatabaseTag() {
}

t4p::SqlQueryClass::SqlQueryClass(const t4p::SqlQueryClass& other)
	: DatabaseTag(other.DatabaseTag) {
}

void t4p::SqlQueryClass::Copy(const t4p::SqlQueryClass& src) {
	DatabaseTag.Copy(src.DatabaseTag);
}

void t4p::SqlQueryClass::Close(soci::statement& stmt) {
	try {
		stmt.clean_up();
	} catch (std::exception const& e) {
		printf("SqlQuery close error:%s\n", e.what());
	}
}

void t4p::SqlQueryClass::Close(soci::session& session, soci::statement& stmt) {
	try {
		stmt.clean_up();
		session.close();
	} catch (std::exception const& e) {
		printf("SqlQuery close error:%s\n", e.what());
	}
}

bool t4p::SqlQueryClass::Connect(soci::session& session, UnicodeString& error) {
		switch(DatabaseTag.Driver) {
		case t4p::DatabaseTagClass::MYSQL:
			return ConnectMysql(session, error);
		case t4p::DatabaseTagClass::SQLITE:
			return ConnectSqlite(session, error);
		}
		error = UNICODE_STRING_SIMPLE("bad driver");
		return false;
}

bool t4p::SqlQueryClass::ConnectSqlite(soci::session& session, UnicodeString& error) {
		std::string stdDbName = t4p::WxToChar(DatabaseTag.FileName.GetFullPath());
		bool success = false;
		try {
			session.open(*soci::factory_sqlite3(), stdDbName);
			success = true;
		} catch (soci::soci_error const& e) {
			error = t4p::CharToIcu(e.what());
		} catch (std::exception const& e) {
			error = t4p::CharToIcu(e.what());
		}
		return success;
}

bool t4p::SqlQueryClass::ConnectMysql(soci::session& session, UnicodeString& error) {
	bool success = false;
	UnicodeString host = DatabaseTag.Host;

	// using localhost triggers socket file lookups in linux
	// and socket files can be in various places on various distros
	// make it simple and use TCP/IP connections for now
	if (host.caseCompare(UNICODE_STRING_SIMPLE("localhost"), 0) == 0) {
		host = UNICODE_STRING_SIMPLE("127.0.0.1");
	}

	// 6 =  max length of port (65535) + null character
	UnicodeString portString;
	UChar* buffer = portString.getBuffer(7);
	int32_t written = u_sprintf(buffer, "%d", DatabaseTag.Port);
	portString.releaseBuffer(written);

	UnicodeString connString;
	connString = UNICODE_STRING_SIMPLE("db=");
	connString += DatabaseTag.Schema;
	connString += UNICODE_STRING_SIMPLE(" host=");
	connString += host;
	connString += UNICODE_STRING_SIMPLE(" port=");
	connString += portString;
	connString += UNICODE_STRING_SIMPLE(" user=");
	connString += DatabaseTag.User;
	connString += UNICODE_STRING_SIMPLE(" password='");
	connString += DatabaseTag.Password;
	connString +=  UNICODE_STRING_SIMPLE("'");


	std::string s = t4p::IcuToChar(connString);
	try {
		session.open(*soci::factory_mysql(), s.c_str());
		success = true;
	} catch (soci::mysql_soci_error const& e) {
		success = false;
		error = t4p::CharToIcu(e.what());
	} catch (std::exception const& e) {
		success = false;
		error = t4p::CharToIcu(e.what());
		if (host.caseCompare(UNICODE_STRING_SIMPLE("localhost"), 0) == 0) {
			error += UNICODE_STRING_SIMPLE("localhost connections are routed to use TCP/IP. Is the server listening for TCP/IP connections?");
		}
	}
	return success;
}

bool t4p::SqlQueryClass::Execute(soci::session& session, t4p::SqlResultClass& results, const UnicodeString& query) {
	results.Success = false;
	try {
		std::string queryStd = t4p::IcuToChar(query);
		soci::statement stmt = (session.prepare << queryStd, soci::into(results.Row));

		// dont pass TRUE to execute; it will fetch the first row and it makes it
		// easy to skip past the first row.
		bool hasRows = stmt.execute(true);

		// execute will return false if statement does not return any rows
		// but we want to return true for INSERTs and UPDATEs too
		results.Success = true;
		results.Init(*this, session, stmt, query, hasRows);
		stmt.clean_up();
	} catch (std::exception const& e) {
		results.Success = false;
		results.Error = t4p::CharToIcu(e.what());
	}
	return results.Success;
}

bool t4p::SqlQueryClass::Execute(soci::statement& stmt, UnicodeString& error) {
	bool success = false;
	try {
		stmt.execute(false);

		// execute will return false if statement does not return any rows
		// but we want to return true for INSERTs and UPDATEs too
		success = true;
	} catch (std::exception const& e) {
		success = false;
		error = t4p::CharToIcu(e.what());
	}
	return success;
}

bool t4p::SqlQueryClass::More(soci::statement& stmt, bool& hasError, UnicodeString& error) {
	bool ret = false;
	try {
		ret = stmt.fetch() && stmt.got_data();
	} catch (std::exception const& e) {
		hasError = true;
		error = t4p::CharToIcu(e.what());
	}
	return ret;
}

long long t4p::SqlQueryClass::GetAffectedRows(soci::statement& stmt) {
	long long rows = 0;
	try {
		rows = stmt.get_affected_rows();
	} catch (std::exception const& e) {
		printf("SqlQuery close error:%s\n", e.what());
	}
	return rows;
}

bool t4p::SqlQueryClass::ColumnNames(soci::row& row, std::vector<UnicodeString>& columnNames, UnicodeString& error) {
	bool data = false;
	try {
		for (size_t i = 0; i < row.size(); i++) {
			soci::column_properties props = row.get_properties(i);
			UnicodeString col = t4p::CharToIcu(props.get_name().c_str());
			columnNames.push_back(col);
		}
		data = true;
	}
	catch (std::exception const& e) {
		data = false;
		error = t4p::CharToIcu(e.what());
	}
	return data;
}

bool t4p::SqlQueryClass::TableNames(soci::session& session, soci::statement& stmt, int columnCount,
	std::vector<UnicodeString>& tableNames, UnicodeString& error) {
	bool data = false;
	try {
		// soci does not have an interface for getting table names,
		// we do it ourselves by accessing the "backends"
		std::string backendName = session.get_backend_name();
		if (backendName.compare("mysql") == 0) {
			soci::mysql_statement_backend* backend = static_cast<soci::mysql_statement_backend*>(stmt.get_backend());
			for (int i = 0; i < columnCount; ++i) {
				MYSQL_FIELD* field = mysql_fetch_field(backend->result_);
				const char* tbl = field->org_table;

				UnicodeString uniTable = t4p::CharToIcu(tbl);
				if (std::find(tableNames.begin(), tableNames.end(), uniTable) == tableNames.end()) {
					tableNames.push_back(uniTable);
				}
			}
		} else if (backendName.compare("sqlite3") == 0) {
			soci::sqlite3_statement_backend* backend = static_cast<soci::sqlite3_statement_backend*>(stmt.get_backend());
			for (int i = 0; i < columnCount; ++i) {
				const char* tbl = sqlite3_column_table_name(backend->stmt_, i);
				if (tbl) {
					UnicodeString uniTable = t4p::CharToIcu(tbl);
					if (std::find(tableNames.begin(), tableNames.end(), uniTable) == tableNames.end()) {
						tableNames.push_back(uniTable);
					}
				}
			}
		}
		data = true;
	}
	catch (std::exception const& e) {
		data = false;
		error = t4p::CharToIcu(e.what());
	}
	return data;
}

bool t4p::SqlQueryClass::NextRow(soci::row& row, std::vector<UnicodeString>& columnValues,
	std::vector<soci::indicator>& columnIndicators, UnicodeString& error) {
	bool data = false;
	try {
		for (size_t i = 0; i < row.size(); i++) {
			soci::indicator indicator = row.get_indicator(i);
			columnIndicators.push_back(indicator);
			soci::column_properties props = row.get_properties(i);
			std::ostringstream out;
			UnicodeString col;
			std::tm tm;
			if (soci::i_null != indicator) {
				switch(props.get_data_type()) {
				case soci::dt_string:
					out << row.get<std::string>(i);
					break;
				case soci::dt_double:
					out << row.get<double>(i);
					break;
				case soci::dt_integer:
					out << row.get<int>(i);
					break;
				case soci::dt_unsigned_long:
					out << row.get<unsigned long>(i);
					break;
				case soci::dt_unsigned_long_long:
					out << row.get<unsigned long long>(i);
					break;
				case soci::dt_long_long:
					out << row.get<long long>(i);
					break;
				case soci::dt_date:
					tm = row.get<std::tm>(i);
					if (tm.tm_year != 0 && tm.tm_mday != 0) {
						wxDateTime date(tm);
						out << date.Format(wxT("%Y-%m-%d %H:%M:%S")).ToAscii();
					} else {
						// mysql allows dates of 0000-00-00
						out << "0000-00-00 00:00:00";
					}
					break;
				}
				col = t4p::CharToIcu(out.str().c_str());
			}
			columnValues.push_back(col);
		}
		data = true;
	}
	catch (std::exception const& e) {
		data = false;
		error = t4p::CharToIcu(e.what());
	}
	return data;
}

void t4p::SqlQueryClass::ConnectionIdentifier(soci::session& session, t4p::ConnectionIdentifierClass& connectionIdentifier) {
	connectionIdentifier.Set(0);
	if (t4p::DatabaseTagClass::MYSQL == DatabaseTag.Driver) {
		soci::mysql_session_backend* backend = static_cast<soci::mysql_session_backend*>(session.get_backend());
		MYSQL* mysql = backend->conn_;
		unsigned long id = mysql_thread_id(mysql);
		connectionIdentifier.Set(id);
	}
}

bool t4p::SqlQueryClass::KillConnection(soci::session& session,
											  t4p::ConnectionIdentifierClass& connectionIdentifier,
											  UnicodeString& error) {
	bool ret = false;
	int id = connectionIdentifier.Get();
	if (id > 0 && t4p::DatabaseTagClass::MYSQL == DatabaseTag.Driver) {
		try {
			std::ostringstream stream;
			stream << "KILL " << connectionIdentifier.Get();
			session.once << stream.str();
			ret = true;
		}
		catch (std::exception const& e) {
			ret = false;
			error = t4p::CharToIcu(e.what());
		}
	}
	return ret;
}

t4p::ConnectionIdentifierClass::ConnectionIdentifierClass()
	: Mutex()
	, ConnectionId(0) {
}

unsigned long t4p::ConnectionIdentifierClass::Get() {
	wxMutexLocker locker(Mutex);
	wxASSERT(locker.IsOk());
	return ConnectionId;
}

void t4p::ConnectionIdentifierClass::Set(unsigned long id) {
	wxMutexLocker locker(Mutex);
	wxASSERT(locker.IsOk());
	ConnectionId = id;
}

t4p::DatabaseTagFinderClass::DatabaseTagFinderClass(soci::session& session)
	: SqliteFinderClass(session) {
}

std::vector<t4p::DatabaseTagClass> t4p::DatabaseTagFinderClass::All(const std::vector<wxFileName>& sourceDirectories) {
	std::vector<t4p::DatabaseTagClass> allDbTags;
	if (sourceDirectories.empty()) {
		return allDbTags;
	}
	std::vector<std::string> stdSourceDirectories;
	std::string sql = "SELECT label, \"schema\", driver, host, port, \"user\", password ";
	sql += "FROM database_tags JOIN sources ON(sources.source_id = database_tags.source_id) ";
	sql += "WHERE directory IN(";
	for (size_t i = 0; i < sourceDirectories.size(); ++i) {
		stdSourceDirectories.push_back(t4p::WxToChar(sourceDirectories[i].GetPathWithSep()));
		if (0 == i) {
			sql += "?";
		} else {
			sql += ",?";
		}
	}
	sql += ")";
	std::string label,
		schema,
		driver,
		host,
		user,
		password;
	int port;
	try {
		soci::statement stmt = Session.prepare << sql;
		stmt.exchange(soci::into(label));
		stmt.exchange(soci::into(schema));
		stmt.exchange(soci::into(driver));
		stmt.exchange(soci::into(host));
		stmt.exchange(soci::into(port));
		stmt.exchange(soci::into(user));
		stmt.exchange(soci::into(password));
		for (size_t i = 0; i < stdSourceDirectories.size(); ++i) {
			stmt.exchange(soci::use(stdSourceDirectories[i]));
		}
		stmt.define_and_bind();
		if (stmt.execute(true)) {
			do {
				t4p::DatabaseTagClass dbTag;
				dbTag.Schema = t4p::CharToIcu(schema.c_str());
				if (driver == "MYSQL") {
					dbTag.Driver = t4p::DatabaseTagClass::MYSQL;
				}
				dbTag.Host = t4p::CharToIcu(host.c_str());
				dbTag.IsDetected = true;
				dbTag.IsEnabled = true;
				dbTag.Label = t4p::CharToIcu(label.c_str());
				dbTag.Password = t4p::CharToIcu(password.c_str());
				dbTag.Port = port;
				dbTag.User = t4p::CharToIcu(user.c_str());

				allDbTags.push_back(dbTag);
			} while (stmt.fetch());
		}
	} catch (std::exception& e) {
		wxString msg = t4p::CharToWx(e.what());
		wxUnusedVar(msg);
		t4p::EditorLogError(t4p::ERR_TAG_READ, msg);
	}
	return allDbTags;
}

