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

bool mvceditor::SqlResourceFinderClass::Fetch(const mvceditor::DatabaseInfoClass& info, UnicodeString& error) {
	bool ret = true;
	Query.Info.Copy(info);
	soci::session session;
	if (Query.Connect(session, error)) {
		UnicodeString hash = Hash(info);
		Tables[hash].clear();
		try {
			std::string schema = mvceditor::StringHelperClass::IcuToChar(info.DatabaseName);
			std::string tableName;
			std::string query = "SELECT table_name FROM information_schema.tables WHERE table_schema=(:schema)";
			soci::statement stmt = (session.prepare << query, soci::into(tableName), soci::use(schema));
			stmt.execute();
			while (Query.More(stmt, ret, error)) {
				UnicodeString uni = mvceditor::StringHelperClass::charToIcu(tableName.c_str());
				Tables[hash].push_back(uni);
				UnicodeString columnHash = Hash(info, uni);
				
				// clear any old columns for this table that were cached
				Columns[columnHash].clear();
			}
			Query.Close(stmt);
			if (ret) {
				std::string columnName;
				query = "SELECT table_name, column_name FROM information_schema.columns WHERE table_schema=(:schema)";
				stmt = (session.prepare << query, soci::into(tableName), soci::into(columnName), soci::use(schema));
				stmt.execute();
				while (Query.More(stmt, ret, error)) {
					UnicodeString uniTable = mvceditor::StringHelperClass::charToIcu(tableName.c_str());
					UnicodeString uniColumn = mvceditor::StringHelperClass::charToIcu(columnName.c_str());
					UnicodeString hash = Hash(info, uniTable);
					Columns[hash].push_back(uniColumn);
				}
				Query.Close(stmt);
			}
			std::sort(Tables[hash].begin(), Tables[hash].end());
			for (std::map<UnicodeString, UnicodeStringVector>::iterator it = Columns.begin(); it != Columns.end(); ++it) {
				std::sort(it->second.begin(), it->second.end());
			}
			
		} 
		catch (std::exception const& e) {
			ret = false;
			error = mvceditor::StringHelperClass::charToIcu(e.what());
		}
	}
	return ret;
}

std::vector<UnicodeString> mvceditor::SqlResourceFinderClass::FindTables(const mvceditor::DatabaseInfoClass& info, const UnicodeString& partialTableName) {
	std::vector<UnicodeString> ret;
	UnicodeString hash = Hash(info);
	std::vector<UnicodeString> infoTables = Tables[hash];
	std::vector<UnicodeString>::iterator it = std::lower_bound(infoTables.begin(), infoTables.end(), partialTableName);
	while (it != infoTables.end() && it->indexOf(partialTableName) == 0) {
		ret.push_back(*it);
		it++;
	}
	return ret;
}

std::vector<UnicodeString> mvceditor::SqlResourceFinderClass::FindColumns(const mvceditor::DatabaseInfoClass& info, const UnicodeString& tableName, 
		const UnicodeString& partialColumnName) {
	std::vector<UnicodeString> ret;
	UnicodeString hash =  Hash(info, tableName);
	std::vector<UnicodeString> infoColumns = Columns[hash];
	std::vector<UnicodeString>::iterator it = std::lower_bound(infoColumns.begin(), infoColumns.end(), partialColumnName);
	while (it != infoColumns.end() && it->indexOf(partialColumnName) == 0) {
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

UnicodeString mvceditor::SqlResourceFinderClass::Hash(const mvceditor::DatabaseInfoClass& info, const UnicodeString& tableName) {
	UnicodeString hash = info.Host + UNICODE_STRING_SIMPLE("--") + info.DatabaseName + UNICODE_STRING_SIMPLE("--") +
		info.FileName + UNICODE_STRING_SIMPLE("--") + tableName;
	return hash;
}