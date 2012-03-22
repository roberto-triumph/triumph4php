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
#include <environment/SqlResourceFinderClass.h>
#include <windows/StringHelperClass.h>
#include <soci.h>
#include <algorithm>

mvceditor::SqlResourceFinderClass::SqlResourceFinderClass() 
	: Query()
	, Tables()
	, Columns() {
		
}

void mvceditor::SqlResourceFinderClass::Copy(const mvceditor::SqlResourceFinderClass& src) {
	Query.Info.Copy(src.Query.Info);
	Tables = src.Tables;
	Columns = src.Columns;
}

bool mvceditor::SqlResourceFinderClass::Fetch(const mvceditor::DatabaseInfoClass& info, UnicodeString& error) {
	bool hasError = false;
	Query.Info.Copy(info);
	soci::session session;
	if (Query.Connect(session, error)) {
		UnicodeString hash = Hash(info);
		
		// clear any cache for this info
		Tables[hash].clear();
		Columns[hash].clear();
		try {
			std::string schema = mvceditor::StringHelperClass::IcuToChar(info.DatabaseName);
			std::string tableName;

			// populate information_schema tables we want SQL code completion to work for the 
			// information_schema tables / columns
			std::string query = "SELECT table_name FROM information_schema.tables WHERE table_schema IN((:schema), 'information_schema')";
			soci::statement stmt = (session.prepare << query, soci::into(tableName), soci::use(schema));
			stmt.execute();
			while (Query.More(stmt, hasError, error)) {
				UnicodeString uni = mvceditor::StringHelperClass::charToIcu(tableName.c_str());

				// lower case so that the lookup method dont have to worry about case sensitivity
				uni.toLower();
				Tables[hash].push_back(uni);				
			}
			Query.Close(stmt);
			if (!hasError) {

				// getting the schema names, populate into tables for now
				query = "SELECT schema_name FROM information_schema.schemata";
				std::string schemaName;
				stmt = (session.prepare << query, soci::into(schemaName));
				stmt.execute();
				while (Query.More(stmt, hasError, error)) {
					UnicodeString uniSchema = mvceditor::StringHelperClass::charToIcu(schemaName.c_str());

					// lower case so that the lookup method dont have to worry about case sensitivity
					uniSchema.toLower();
					Tables[hash].push_back(uniSchema);
				}
				Query.Close(stmt);
			}
			if (!hasError) {
				std::string columnName;
				
				// only getting unique columns names for now
				// no need to know what tables they came from since we are not yet able to
				// auto complete properly. proper auto complete would require a proper SQL lexer and parser
				// and its not worth it for now
				query = "SELECT DISTINCT column_name FROM information_schema.columns WHERE table_schema IN((:schema), 'information_schema')";
				stmt = (session.prepare << query, soci::into(columnName), soci::use(schema));
				stmt.execute();
				while (Query.More(stmt, hasError, error)) {
					UnicodeString uniColumn = mvceditor::StringHelperClass::charToIcu(columnName.c_str());

					// lower case so that the lookup method dont have to worry about case sensitivity
					uniColumn.toLower();
					UnicodeString hash = Hash(info);
					Columns[hash].push_back(uniColumn);
				}
				Query.Close(stmt);
			}
			std::sort(Tables[hash].begin(), Tables[hash].end());
			std::sort(Columns[hash].begin(), Columns[hash].end());			
		} 
		catch (std::exception const& e) {
			hasError = true;
			error = mvceditor::StringHelperClass::charToIcu(e.what());
		}
	}
	return !hasError;
}

std::vector<UnicodeString> mvceditor::SqlResourceFinderClass::FindTables(const mvceditor::DatabaseInfoClass& info, const UnicodeString& partialTableName) {
	std::vector<UnicodeString> ret;
	UnicodeString hash = Hash(info);
	std::vector<UnicodeString> infoTables = Tables[hash];

	// find is case insensitive
	UnicodeString lower(partialTableName);
	lower.toLower();
	std::vector<UnicodeString>::iterator it = std::lower_bound(infoTables.begin(), infoTables.end(), lower);
	while (it != infoTables.end() && it->indexOf(lower) == 0) {
		ret.push_back(*it);
		it++;
	}
	return ret;
}

std::vector<UnicodeString> mvceditor::SqlResourceFinderClass::FindColumns(const mvceditor::DatabaseInfoClass& info, const UnicodeString& partialColumnName) {
	std::vector<UnicodeString> ret;
	UnicodeString hash =  Hash(info);
	std::vector<UnicodeString> infoColumns = Columns[hash];

	// find is case insensitive
	UnicodeString lower(partialColumnName);
	lower.toLower();
	std::vector<UnicodeString>::iterator it = std::lower_bound(infoColumns.begin(), infoColumns.end(), lower);
	while (it != infoColumns.end() && it->indexOf(lower) == 0) {
		ret.push_back(*it);
		it++;
	}
	return ret;
}

UnicodeString mvceditor::SqlResourceFinderClass::Hash(const mvceditor::DatabaseInfoClass& info) {
	UnicodeString hash = info.Host + UNICODE_STRING_SIMPLE("--") + info.DatabaseName + UNICODE_STRING_SIMPLE("---") +
		info.FileName;
	return hash;
}