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
#include <language_php/FileTags.h>
#include <globals/Sqlite.h>
#include <globals/String.h>
#include <algorithm>

std::vector<int> t4p::FileTagIdsForDirs(soci::session& session, const std::vector<wxFileName>& dirs, bool& error, wxString& errorMsg) {
	std::vector<int> fileTagIds;
	if (dirs.empty()) {
		return fileTagIds;
	}
	std::string sql = "";
	for (size_t i = 0; i < dirs.size(); i++) {
		sql += "SELECT file_item_id FROM file_items WHERE full_path LIKE '" +
			t4p::WxToChar(dirs[i].GetPathWithSep()) + "%' ESCAPE '^'";
		if (i < (dirs.size() - 1)) {
			sql += " UNION ";
		}
	}
	int fileItemId = 0;
	try {
		soci::statement stmt = (session.prepare << sql,
			soci::into(fileItemId));
		bool foundFile = stmt.execute(true);
		if (foundFile) {
			fileTagIds.push_back(fileItemId);
			while (stmt.fetch()) {
				fileTagIds.push_back(fileItemId);
			}
		}
	} catch (std::exception& e) {
		error = true;
		errorMsg = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, errorMsg);
	}
	std::sort(fileTagIds.begin(), fileTagIds.end());
	std::vector<int>::iterator it = std::unique(fileTagIds.begin(), fileTagIds.end());
	fileTagIds.erase(it, fileTagIds.end());
	return fileTagIds;
}

bool t4p::FileTagPersist(soci::session& session, t4p::FileTagClass& fileTag) {
	std::string fullPath = t4p::WxToChar(fileTag.FullPath);
	std::string name = t4p::WxToChar(fileTag.Name());
	std::tm tm;
	int isParsed = fileTag.IsParsed ? 1 : 0;
	int isNew = fileTag.IsNew ? 1 : 0;
	int sourceId = fileTag.SourceId;
	if (fileTag.DateTime.IsValid()) {
		wxDateTime::Tm wxTm = fileTag.DateTime.GetTm();
		tm.tm_hour = wxTm.hour;
		tm.tm_isdst = fileTag.DateTime.IsDST();
		tm.tm_mday = wxTm.mday;
		tm.tm_min = wxTm.min;
		tm.tm_mon = wxTm.mon;
		tm.tm_sec = wxTm.sec;
		tm.tm_wday = fileTag.DateTime.GetWeekDay();
		tm.tm_yday = fileTag.DateTime.GetDayOfYear();

		// tm holds number of years since 1900 (2012 = 112)
		tm.tm_year = wxTm.year - 1900;
	}
	bool success = false;
	try {
		soci::statement stmt = (session.prepare <<
			"INSERT INTO file_items (file_item_id, source_id, full_path, name, last_modified, is_parsed, is_new) VALUES(NULL, ?, ?, ?, ?, ?, ?)",
			soci::use(sourceId), soci::use(fullPath), soci::use(name), soci::use(tm), soci::use(isParsed), soci::use(isNew));
		stmt.execute(true);
		fileTag.FileId = t4p::SqliteInsertId(stmt);
		success = true;
	} catch (std::exception& e) {
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		wxString msg = wxString::FromAscii(e.what());
		wxASSERT_MSG(false, msg);
	}
	return success;
}
