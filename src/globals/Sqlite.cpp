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
#include <globals/Sqlite.h>
#include <globals/String.h>
#include <wx/ffile.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <sqlite3.h>

/**
 * escape a value so that it is suitable for using in a LIKE SQL clause
 * ie. so that an underscore is treated literally
 * @param value the value to escape
 * @param c the character to USE for escaping. this should NOT be backslash,
 *        as we have namespaces in the database and they use backslash
 */
std::string mvceditor::SqlEscape(const std::string& value, char c) {
	size_t i = 0;
	std::string escaped;
	size_t next = value.find("_", i);
	while (next != std::string::npos) {
		escaped += value.substr(i, next - i);
		escaped += c;
		escaped += "_";

		i = next + 1;
		next = value.find("_", i);
	}
	if (i < value.length()) {
		escaped += value.substr(i);
	}
	return escaped;
}

bool mvceditor::SqlScript(const wxFileName& sqlScriptFileName, soci::session& session, wxString& error) {
	bool ret = false;

	// open the SQL script that contains the table creation statements
	// the script is "nice" it takes care to not create the tables if
	// they already exist
	if (sqlScriptFileName.FileExists()) {
		wxFFile ffile(sqlScriptFileName.GetFullPath());
		wxString sql;
		ffile.ReadAll(&sql);
		std::string stdSql = mvceditor::WxToChar(sql);
		try {

			// get the 'raw' connection because it can handle multiple statements at once
			char *errorMessage = NULL;
			soci::sqlite3_session_backend* backend = static_cast<soci::sqlite3_session_backend*>(session.get_backend());
			sqlite_api::sqlite3_exec(backend->conn_, stdSql.c_str(), NULL, NULL, &errorMessage);
			if (errorMessage) {
				error = mvceditor::CharToWx(errorMessage);
				sqlite_api::sqlite3_free(errorMessage);
			}
			else {
				ret = true;
			}
		} catch (std::exception& e) {
			error = mvceditor::CharToWx(e.what());
		}
	}
	return ret;
}