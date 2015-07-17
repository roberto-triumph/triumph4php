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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <language_js/JsTagResultClass.h>
#include <globals/String.h>

t4p::JsTagResultClass::JsTagResultClass()
: t4p::SqliteResultClass()
, JsTag()
, Id(0)
, FileItemId(0)
, SourceId(0)
, Key()
, Identifier()
, Signature()
, Comment()
, FullPath()
, IsFileNew(false)
, LineNumber(0)
, ColumnPosition(0) {
}

void t4p::JsTagResultClass::DoBind(soci::statement& stmt) {
	stmt.exchange(soci::into(Id));
	stmt.exchange(soci::into(FileItemId));
	stmt.exchange(soci::into(SourceId));
	stmt.exchange(soci::into(Key));
	stmt.exchange(soci::into(Identifier));
	stmt.exchange(soci::into(Signature));
	stmt.exchange(soci::into(Comment));
	stmt.exchange(soci::into(LineNumber));
	stmt.exchange(soci::into(ColumnPosition));
	stmt.exchange(soci::into(FullPath));
	stmt.exchange(soci::into(IsFileNew));
}

void t4p::JsTagResultClass::Next() {
	JsTag.Id = Id;
	JsTag.FileTagId = FileItemId;
	JsTag.SourceId = SourceId;
	JsTag.Key = t4p::CharToIcu(Key.c_str());
	JsTag.Identifier = t4p::CharToIcu(Identifier.c_str());
	JsTag.Signature = t4p::CharToIcu(Signature.c_str());
	JsTag.Comment = t4p::CharToIcu(Comment.c_str());
	JsTag.LineNumber = LineNumber;
	JsTag.ColumnPosition = ColumnPosition;
	JsTag.FullPath = t4p::CharToWx(FullPath.c_str());
	JsTag.FileIsNew = IsFileNew > 0;

	Fetch();
}

t4p::ExactMatchJsTagResultClass::ExactMatchJsTagResultClass()
: JsTagResultClass()
, Search()
, SourceDirs() {

}

void t4p::ExactMatchJsTagResultClass::SetSearch(const UnicodeString& search, const std::vector<wxFileName>& sourceDirs) {
	Search = t4p::IcuToChar(search);

	// add trailing separator to lookup sources that include the last directory
	// we don't want to get directories that start with a prefix
	for (size_t i = 0; i < sourceDirs.size(); i++) {
		SourceDirs.push_back(t4p::WxToChar(sourceDirs[i].GetPathWithSep()));
	}
}

bool t4p::ExactMatchJsTagResultClass::DoPrepare(soci::statement& stmt, bool doLimit) {

	// case sensitive issues are taken care of by SQLite collation capabilities
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, r.key, r.identifier, r.signature, r.comment, r.line_number, r.column_position, f.full_path, ";
	sql += "is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON(r.file_item_id = f.file_item_id) LEFT JOIN sources s ON(s.source_id = r.source_id) WHERE ";
	sql += "key = ? AND identifier = key";
	if (!SourceDirs.empty()) {
		sql += " AND s.directory IN(?";
		for (size_t i = 1; i <  SourceDirs.size(); ++i) {
			sql += ",?";
		}
		sql += ")";
	}
	sql += " ORDER BY key";
	if (doLimit) {
		sql += " LIMIT 100";
	}
	stmt.prepare(sql);
	stmt.exchange(soci::use(Search));
	for (size_t i = 0; i < SourceDirs.size(); i++) {
		stmt.exchange(soci::use(SourceDirs[i]));
	}
	return true;
}

t4p::NearMatchJsTagResultClass::NearMatchJsTagResultClass()
: JsTagResultClass()
, Search()
, SearchUpper()
, SourceDirs() {

}

void t4p::NearMatchJsTagResultClass::SetSearch(const UnicodeString& search, const std::vector<wxFileName>& sourceDirs) {
	Search = t4p::IcuToChar(search);
	SearchUpper = Search + "zzzzzzzzzz";

	// add trailing separator to lookup sources that include the last directory
	// we don't want to get directories that start with a prefix
	for (size_t i = 0; i < sourceDirs.size(); i++) {
		SourceDirs.push_back(t4p::WxToChar(sourceDirs[i].GetPathWithSep()));
	}
}

bool t4p::NearMatchJsTagResultClass::DoPrepare(soci::statement& stmt, bool doLimit) {

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, r.key, r.identifier, r.signature, r.comment, r.line_number, r.column_position, f.full_path, ";
	sql += "is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON(r.file_item_id = f.file_item_id) LEFT JOIN sources s ON(s.source_id = r.source_id) WHERE ";

	// not using LIKE operator here, there are way too many situations where it wont use the index
	// index won't be used when ESCAPE is used or when sqlite3_prepare_v2 is NOT used (which soci does not use)
	// see http://sqlite.org/optoverview.html (LIKE optimization)
	// instead we use a trick to mimic LIKE using the BETWEEN
	// http://stackoverflow.com/questions/13056193/escape-wildcards-in-sqlite-like-without-sacrificing-index-use
	sql += "key BETWEEN ? AND ? AND identifier = key";
	if (!SourceDirs.empty()) {
		sql += " AND s.directory IN(?";
		for (size_t i = 1; i <  SourceDirs.size(); ++i) {
			sql += ",?";
		}
		sql += ")";
	}
	sql += " ORDER BY key";
	if (doLimit) {
		sql += " LIMIT 100";
	}
	stmt.prepare(sql);
	stmt.exchange(soci::use(Search));
	stmt.exchange(soci::use(SearchUpper));
	for (size_t i = 0; i < SourceDirs.size(); i++) {
		stmt.exchange(soci::use(SourceDirs[i]));
	}
	return true;
}
