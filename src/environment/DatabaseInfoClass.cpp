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
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
 #include <environment/DatabaseInfoClass.h>
 #include <soci-mysql.h>
 #include <soci.h>
 #include <wx/datetime.h>
 #include <windows/StringHelperClass.h>
 #include <string>
 
mvceditor::DatabaseInfoClass::DatabaseInfoClass()
	: Host()
	, User()
	, Password()
	, DatabaseName()
	, FileName()
	, Name()
	, Driver(MYSQL)
	, Port() {
		
}

mvceditor::DatabaseInfoClass::DatabaseInfoClass(const mvceditor::DatabaseInfoClass& other) 
	: Host()
	, User()
	, Password()
	, DatabaseName()
	, FileName()
	, Name()
	, Driver(MYSQL)
	, Port() {
	Copy(other);
}

void mvceditor::DatabaseInfoClass::Copy(const mvceditor::DatabaseInfoClass& src) {
	Host = src.Host;
	User = src.User;
	Password = src.Password;
	DatabaseName = src.DatabaseName;
	FileName = src.FileName;
	Name = src.Name;
	Driver = src.Driver;
	Port = src.Port;
}

bool mvceditor::DatabaseInfoClass::SameAs(const mvceditor::DatabaseInfoClass& other) {
	return Host.caseCompare(other.Host, 0) == 0 && DatabaseName.caseCompare(other.DatabaseName, 0) == 0;
}

mvceditor::SqlResultClass::SqlResultClass() 
	: Error()
	, Row()
	, StringResults()
	, ColumnNames()
	, Query()
	, QueryTime()
	, LineNumber(0)
	, AffectedRows(0)
	, Success(false)
	, HasRows(false) {

}

mvceditor::SqlResultClass::~SqlResultClass() {
	Close();
}

void mvceditor::SqlResultClass::Close() {
	ColumnNames.clear();
	StringResults.clear();
	AffectedRows = 0;
	HasRows = false;
	Success = false;
}

void mvceditor::SqlResultClass::Init(mvceditor::SqlQueryClass& query, soci::session& session, soci::statement& stmt, 
									 const UnicodeString& sqlString, bool hasRows) {
	ColumnNames.clear();
	StringResults.clear();
	Query = sqlString;
	std::vector<soci::indicator> columnIndicators;
	HasRows = hasRows;
	AffectedRows = query.GetAffectedRows(stmt);
	if (Success && HasRows && query.ColumnNames(Row, ColumnNames, Error)) {
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

mvceditor::SqlQueryClass::SqlQueryClass()
	: Info() {
}

mvceditor::SqlQueryClass::SqlQueryClass(const mvceditor::SqlQueryClass& other) 
	: Info(other.Info) {
}

void mvceditor::SqlQueryClass::Copy(const mvceditor::SqlQueryClass& src) {
	Info.Copy(src.Info);
}

void mvceditor::SqlQueryClass::Close(soci::statement& stmt) {
	try {
		stmt.clean_up();
	} catch (std::exception const& e) {
		printf("SqlQuery close error:%s\n", e.what());
	}
}

void mvceditor::SqlQueryClass::Close(soci::session& session, soci::statement& stmt) {
	try {
		stmt.clean_up();
		session.close();
	} catch (std::exception const& e) {
		printf("SqlQuery close error:%s\n", e.what());
	}
}

bool mvceditor::SqlQueryClass::Connect(soci::session& session, UnicodeString& error) {
	bool success = false;
	const char* pattern = "db=%S host=%S port=%d user=%S password='%S'";
	UnicodeString connString;
	
	// 6 =  max length of port (65535)
	UChar* buffer = connString.getBuffer(strlen(pattern) + Info.DatabaseName.length() + Info.Host.length() +
		6 + Info.User.length() + Info.Password.length());
	UnicodeString host = Info.Host;
	
	// using localhost triggers socket file lookups in linux
	// and socket files can be in various places on various distros
	// make it simple and use TCP/IP connections for now
	if (host.caseCompare(UNICODE_STRING_SIMPLE("localhost"), 0) == 0) {
		host = UNICODE_STRING_SIMPLE("127.0.0.1");
	}
	
	int32_t written = u_sprintf(buffer, pattern, 
		Info.DatabaseName.getTerminatedBuffer(), host.getTerminatedBuffer(), 
		Info.Port, Info.User.getTerminatedBuffer(), Info.Password.getTerminatedBuffer());
	connString.releaseBuffer(written);
	
	std::string s = mvceditor::StringHelperClass::IcuToChar(connString);
	try {
		session.open(*soci::factory_mysql(), s.c_str());
		success = true;
		
	} catch (std::exception const& e) {
		success = false;
		error = mvceditor::StringHelperClass::charToIcu(e.what());
		if (host.caseCompare(UNICODE_STRING_SIMPLE("localhost"), 0) == 0) {
			error += UNICODE_STRING_SIMPLE("localhost connections are routed to use TCP/IP. Is the server listening for TCP/IP connections?");
		}
	}
	return success;
}

bool mvceditor::SqlQueryClass::Execute(soci::session& session, mvceditor::SqlResultClass& results, const UnicodeString& query) {
	results.Success = false;
	try {
		std::string queryStd = mvceditor::StringHelperClass::IcuToChar(query);
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
		results.Error = mvceditor::StringHelperClass::charToIcu(e.what());
	}
	return results.Success;
}

bool mvceditor::SqlQueryClass::Execute(soci::statement& stmt, UnicodeString& error) {
	bool success = false;
	try {
		stmt.execute(false);
		
		// execute will return false if statement does not return any rows
		// but we want to return true for INSERTs and UPDATEs too
		success = true;
		
	} catch (std::exception const& e) {
		success = false;
		error = mvceditor::StringHelperClass::charToIcu(e.what());
	}
	return success;
}

bool mvceditor::SqlQueryClass::GotData(soci::statement& stmt) {
	bool ret =  false;
	try {
		ret = stmt.got_data();
	}
	catch (std::exception const& e) {

		// TODO: make error accessible?
		printf(e.what());
	}
	return ret;
}

bool mvceditor::SqlQueryClass::More(soci::statement& stmt, bool& hasError, UnicodeString& error) {
	bool ret = false;
	try {
		ret = stmt.fetch() && stmt.got_data();
	} catch (std::exception const& e) {
		hasError = true;
		error = mvceditor::StringHelperClass::charToIcu(e.what());
	}
	return ret;
}

long long mvceditor::SqlQueryClass::GetAffectedRows(soci::statement& stmt) {
	long long rows = 0;
	try {
		rows = stmt.get_affected_rows();
	} catch (std::exception const& e) {
		printf("SqlQuery close error:%s\n", e.what());	
	}
	return rows;
}

bool mvceditor::SqlQueryClass::ColumnNames(soci::row& row, std::vector<UnicodeString>& columnNames, UnicodeString& error) {
	bool data = false;
	try {
		for (size_t i = 0; i < row.size(); i++) {
			soci::column_properties props = row.get_properties(i);
			UnicodeString col = mvceditor::StringHelperClass::charToIcu(props.get_name().c_str());
			columnNames.push_back(col);
		}
		data = true;
	}
	catch (std::exception const& e) {
		data = false;
		error = mvceditor::StringHelperClass::charToIcu(e.what());
	}
	return data;
}

bool mvceditor::SqlQueryClass::NextRow(soci::row& row, std::vector<UnicodeString>& columnValues, std::vector<soci::indicator>& columnIndicators, UnicodeString& error) {
	bool data = false;
	try {
		for (size_t i = 0; i < row.size(); i++) {
			soci::indicator indicator = row.get_indicator(i);
			columnIndicators.push_back(indicator);
			soci::column_properties props = row.get_properties(i);
			std::ostringstream out;
			UnicodeString col;
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
				case soci::dt_unsigned_long_long:
					out << row.get<unsigned long>(i);
					break;
				case soci::dt_long_long:
					out << row.get<long long>(i);
					break;
				case soci::dt_date:
					wxDateTime date(row.get<std::tm>(i));
					out << date.Format(wxT("%Y-%m-%d %H:%M:%S")).ToAscii();
					break;
				}
				col = mvceditor::StringHelperClass::charToIcu(out.str().c_str());
			}
			columnValues.push_back(col);
		}
		data = true;
	}
	catch (std::exception const& e) {
		data = false;
		error = mvceditor::StringHelperClass::charToIcu(e.what());
	}
	return data;
}