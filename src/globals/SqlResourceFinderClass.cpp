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
#include <globals/SqlResourceFinderClass.h>
#include <globals/String.h>
#include <globals/Sqlite.h>
#include <soci.h>
#include <algorithm>

/**
 * turn the info into a string that way we can link tables to a connection and 
 */
static std::string Hash(const mvceditor::DatabaseTagClass& info) {
	UnicodeString hash = info.Host + UNICODE_STRING_SIMPLE("--") + info.Schema + UNICODE_STRING_SIMPLE("---") +
		mvceditor::WxToIcu(info.FileName.GetFullPath());
	return mvceditor::IcuToChar(hash);
}
	
mvceditor::SqlResourceFetchClass::SqlResourceFetchClass(soci::session& session)
: Session(session) {
		
}

bool mvceditor::SqlResourceFetchClass::Fetch(const DatabaseTagClass& info, UnicodeString& error) {
	switch(info.Driver) {
	case mvceditor::DatabaseTagClass::MYSQL:
		return FetchMysql(info, error);
	case mvceditor::DatabaseTagClass::SQLITE:
		return FetchSqlite(info, error);
	}
	error = UNICODE_STRING_SIMPLE("invalid driver");
	return false;
}

bool mvceditor::SqlResourceFetchClass::Wipe() {
	bool ret = false;
	try {
		Session.once << "DELETE FROM db_tables";
		Session.once << "DELETE FROM db_columns";
		ret = true;
	} catch (std::exception& e) {
		wxASSERT_MSG(false, e.what());
	}
	return ret;
}

bool mvceditor::SqlResourceFetchClass::FetchMysql(const DatabaseTagClass& info, UnicodeString& error) {
	bool hasError = false;
	mvceditor::SqlQueryClass query;
	query.DatabaseTag.Copy(info);
	
	// the local session connects to the db we want to extract the tables / columns from
	soci::session session;
	if (query.Connect(session, error)) {
		std::string hash = Hash(info);
		std::vector<std::string> tables;
		std::vector<std::string> columns;
		try {
			
			std::string schema = mvceditor::IcuToChar(info.Schema);
			std::string tableName;

			// populate information_schema tables we want SQL code completion to work for the 
			// information_schema tables / columns
			std::string sql = "SELECT table_name FROM information_schema.tables WHERE table_schema IN((:schema), 'information_schema')";
			soci::statement stmt = (session.prepare << sql, soci::into(tableName), soci::use(schema));
			stmt.execute();
			while (query.More(stmt, hasError, error)) {
				tables.push_back(tableName);
			}
			query.Close(stmt);
			if (!hasError) {

				// getting the schema names, populate into tables for now
				sql = "SELECT schema_name FROM information_schema.schemata";
				std::string schemaName;
				stmt = (session.prepare << sql, soci::into(schemaName));
				stmt.execute();
				while (query.More(stmt, hasError, error)) {
					tables.push_back(schemaName);
				}
				query.Close(stmt);
			}
			if (!hasError) {
				std::string columnName;
				
				// only getting unique columns names for now
				// no need to know what tables they came from since we are not yet able to
				// auto complete properly. proper auto complete would require a proper SQL lexer and parser
				// and its not worth it for now
				sql = "SELECT DISTINCT column_name FROM information_schema.columns WHERE table_schema IN((:schema), 'information_schema')";
				stmt = (session.prepare << sql, soci::into(columnName), soci::use(schema));
				stmt.execute();
				while (query.More(stmt, hasError, error)) {
					columns.push_back(columnName);
				}
				query.Close(stmt);
			}
			error = !StoreTables(hash, tables) || !StoreColumns(hash, columns);
		} 
		catch (std::exception const& e) {
			hasError = true;
			error = mvceditor::CharToIcu(e.what());
		}
	}
	else {
		hasError = true;
	}
	return !hasError;
}

bool mvceditor::SqlResourceFetchClass::FetchSqlite(const DatabaseTagClass& info, UnicodeString& error) {
	bool hasError = false;
	mvceditor::SqlQueryClass query;
	query.DatabaseTag.Copy(info);
	
	// this local session connects to the db we want to fetch tables/columns from
	soci::session session;
	if (query.Connect(session, error)) {
		std::string hash = Hash(info);
		std::vector<std::string> tables;
		std::vector<std::string> columns;
		
		try {

			// populate information_schema tables we want SQL code completion to work for the 
			// information_schema tables / columns
			wxString wxError;
			if (!mvceditor::SqliteTables(session, tables, wxError)) {
				hasError = true;
				error = mvceditor::WxToIcu(wxError);
			}
			if (!hasError) {
				for (size_t i = 0; i < tables.size(); ++i) {
					
					// get the columns for the table
					// only getting unique columns names for now
					// no need to know what tables they came from since we are not yet able to
					// auto complete properly. proper auto complete would require a proper SQL lexer and parser
					// and its not worth it for now
					std::string stdTable = tables[i];
					std::string sql = "pragma table_info('" + stdTable + "')";
					
					int cid;
					std::string name;
					std::string type;
					int notNull;
					std::string defaultValue;
					int pk;
					soci::indicator defaultValueNullIndicator;
					
					soci::statement stmt = (session.prepare << sql, soci::into(cid), soci::into(name),
						soci::into(type), soci::into(notNull), soci::into(defaultValue, defaultValueNullIndicator),
						soci::into(pk));
					stmt.execute();
					while (query.More(stmt, hasError, error)) {
						columns.push_back(name.c_str());
					}
					query.Close(stmt);
				}
			}
			std::sort(columns.begin(), columns.end());
			
			// want to get only unique cols for now
			std::vector<std::string>::iterator it = std::unique(columns.begin(), columns.end());
			if (it != columns.end()) {
				columns.erase(it, columns.end());
			}
			
			error = !StoreTables(hash, tables) || !StoreColumns(hash, columns);
		} 
		catch (std::exception const& e) {
			hasError = true;
			error = mvceditor::CharToIcu(e.what());
			puts(e.what());
		}
	}
	else {
		hasError = true;
	}
	return !hasError;
}


bool mvceditor::SqlResourceFetchClass::StoreTables(const std::string& hash, const std::vector<std::string>& tables) {
	std::string sql;
	sql += "INSERT OR IGNORE INTO db_tables (";
	sql += "connection_label, table_name";
	sql += ") VALUES(";
	sql += "?, ?";
	sql += ");";
	bool ret = false;
	
	try {
		std::string tableName;
		soci::transaction txn(Session);
		soci::statement stmt = (Session.prepare << sql,
			soci::use(hash), soci::use(tableName)
		);
		for (size_t i = 0; i < tables.size(); ++i) {
			tableName = tables[i];
			stmt.execute(true);
		}
		txn.commit();
		ret = true;
	} catch (std::exception& e) {
		wxASSERT_MSG(false, e.what());
	}
	return ret;
}

bool mvceditor::SqlResourceFetchClass::StoreColumns(const std::string& hash, const std::vector<std::string>& columns) {
	std::string sql;
	sql += "INSERT OR IGNORE INTO db_columns (";
	sql += "column_name, connection_label";
	sql += ") VALUES(";
	sql += "?, ?";
	sql += ");";
	bool ret = false;
	
	try {
		std::string columnName;
		soci::transaction txn(Session);
		soci::statement stmt = (Session.prepare << sql,
			soci::use(columnName), soci::use(hash)
		);
		for (size_t i = 0; i < columns.size(); ++i) {
			columnName = columns[i];
			stmt.execute(true);
		}
		txn.commit();
		ret = true;
	} catch (std::exception& e) {
		wxASSERT_MSG(false, e.what());
	}
	return ret;
}

mvceditor::SqlResourceTableResultClass::SqlResourceTableResultClass()
 : SqliteResultClass()
 , TableName()
 , Connection() 
 , Lookup() 
 , LookupEnd() 
 , ConnectionHash() {
	 
 }
 
void mvceditor::SqlResourceTableResultClass::SetLookup(const wxString& lookup, const std::string& connectionHash) {
	Lookup = mvceditor::WxToChar(lookup);
	LookupEnd = Lookup + "zzzzzzz";
	ConnectionHash = connectionHash;
}

bool mvceditor::SqlResourceTableResultClass::Prepare(soci::session& session, bool doLimit) {
	
	// not using LIKE operator here, there are way too many situations where it wont use the index
	// index won't be used when ESCAPE is used or when sqlite3_prepare_v2 is NOT used (which soci does not use)
	// see http://sqlite.org/optoverview.html (LIKE optimization)
	// instead we use a trick to mimic LIKE using the BETWEEN
	// http://stackoverflow.com/questions/13056193/escape-wildcards-in-sqlite-like-without-sacrificing-index-use
	std::string sql = "SELECT table_name, connection_label ";
	sql += "FROM db_tables ";
	sql += "WHERE table_name BETWEEN ? AND ? AND connection_label = ? ";
	sql += "ORDER BY table_name ";
	if (doLimit) {
		sql += "LIMIT 100";
	}
	
	soci::statement* stmt = new soci::statement(session);
	stmt->prepare(sql);
	stmt->exchange(soci::use(Lookup));
	stmt->exchange(soci::use(LookupEnd));
	stmt->exchange(soci::use(ConnectionHash));
	
	return Init(stmt);
}

bool mvceditor::SqlResourceTableResultClass::Init(soci::statement* stmt) {
	wxString error;
	bool ret = false;
	try {
		stmt->exchange(soci::into(TableName));
		stmt->exchange(soci::into(Connection));
		
		ret = AdoptStatement(stmt, error);
	} catch (std::exception& e) {
		error = mvceditor::CharToWx(e.what());
		wxASSERT_MSG(false, error);
	}
	return ret;
}

void mvceditor::SqlResourceTableResultClass::Next() {
	Fetch();
}


/**
 * Performs a prefix lookup on column names 
 */
mvceditor::SqlResourceColumnResultClass::SqlResourceColumnResultClass()
: SqliteResultClass()
, ColumnName()
, Lookup()
, LookupEnd() 
, ConnectionHash() {
	
}

void mvceditor::SqlResourceColumnResultClass::SetLookup(const wxString& lookup, const std::string& connectionHash) {
	Lookup = mvceditor::WxToChar(lookup);
	LookupEnd = Lookup + "zzzzzzz";
	ConnectionHash = connectionHash;
}

bool mvceditor::SqlResourceColumnResultClass::Prepare(soci::session& session, bool doLimit) {
	
	// not using LIKE operator here, there are way too many situations where it wont use the index
	// index won't be used when ESCAPE is used or when sqlite3_prepare_v2 is NOT used (which soci does not use)
	// see http://sqlite.org/optoverview.html (LIKE optimization)
	// instead we use a trick to mimic LIKE using the BETWEEN
	// http://stackoverflow.com/questions/13056193/escape-wildcards-in-sqlite-like-without-sacrificing-index-use
	std::string sql = "SELECT column_name ";
	sql += "FROM db_columns ";
	sql += "WHERE column_name BETWEEN ? AND ? AND connection_label = ? ";
	sql += "ORDER BY column_name ";
	if (doLimit) {
		sql += "LIMIT 100";
	}
	
	soci::statement* stmt = new soci::statement(session);
	stmt->prepare(sql);
	stmt->exchange(soci::use(Lookup));
	stmt->exchange(soci::use(LookupEnd));
	stmt->exchange(soci::use(ConnectionHash));
	
	return Init(stmt);
}

bool mvceditor::SqlResourceColumnResultClass::Init(soci::statement* stmt) {
	wxString error;
	bool ret = false;
	try {
		stmt->exchange(soci::into(ColumnName));
		
		ret = AdoptStatement(stmt, error);
	} catch (std::exception& e) {
		error = mvceditor::CharToWx(e.what());
		wxASSERT_MSG(false, error);
	}
	return ret;
}

void mvceditor::SqlResourceColumnResultClass::Next() {
	Fetch();
}

mvceditor::SqlResourceFinderClass::SqlResourceFinderClass() 
	: SqliteFinderClass() {
		
}

std::vector<UnicodeString> mvceditor::SqlResourceFinderClass::FindTables(const mvceditor::DatabaseTagClass& info, const UnicodeString& partialTableName) {
	std::vector<UnicodeString> ret;
	std::string hash = Hash(info);
	
	mvceditor::SqlResourceTableResultClass tableLookup;
	tableLookup.SetLookup(mvceditor::IcuToWx(partialTableName), hash);

	// case insensitive is taken care of by SqlResourceClass
	if (Exec(&tableLookup)) {
		while (tableLookup.More()) {
			ret.push_back(mvceditor::CharToIcu(tableLookup.TableName.c_str()));
			tableLookup.Next();
		}
	}
	return ret;
}

std::vector<UnicodeString> mvceditor::SqlResourceFinderClass::FindColumns(const mvceditor::DatabaseTagClass& info, const UnicodeString& partialColumnName) {
	std::vector<UnicodeString> ret;
	std::string hash = Hash(info);
	
	mvceditor::SqlResourceColumnResultClass columnLookup;
	columnLookup.SetLookup(mvceditor::IcuToWx(partialColumnName), hash);

	// case insensitive is taken care of by SqlResourceClass
	if (Exec(&columnLookup)) {
		while (columnLookup.More()) {
			ret.push_back(mvceditor::CharToIcu(columnLookup.ColumnName.c_str()));
			columnLookup.Next();
		}
	}
	return ret;
}
