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
#include <language_php/UrlTagClass.h>
#include <globals/String.h>
#include <globals/Assets.h>
#include <globals/Sqlite.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <wx/ffile.h>

t4p::UrlTagClass::UrlTagClass()
 : Url()
 , FileName()
 , ClassName()
 , MethodName() {

 }

t4p::UrlTagClass::UrlTagClass(wxString uri)
	: Url(uri)
	, FileName()
	, ClassName()
	, MethodName() {
}

t4p::UrlTagClass::UrlTagClass(const t4p::UrlTagClass& src)
	: Url()
	, FileName()
	, ClassName()
	, MethodName() {
	Copy(src);
 }

void t4p::UrlTagClass::Reset() {
	Url.Create(wxT(""));
	FileName.Clear();
	ClassName.Clear();
	MethodName.Clear();
}

t4p::UrlTagClass& t4p::UrlTagClass::operator=(const t4p::UrlTagClass& src) {
	Copy(src);
	return *this;
}

void t4p::UrlTagClass::Copy(const t4p::UrlTagClass& src) {
	Url = src.Url;
	FileName = src.FileName;
	ClassName = src.ClassName.c_str();
	MethodName = src.MethodName.c_str();
}

t4p::UrlTagFinderClass::UrlTagFinderClass(soci::session& session)
	: SqliteFinderClass(session) {

}

bool t4p::UrlTagFinderClass::FindByUrl(const wxURI& url, const std::vector<wxFileName>& sourceDirs, t4p::UrlTagClass& urlTag) {
	bool ret = false;
	if (sourceDirs.empty()) {
		return ret;
	}
	std::string stdUrlWhere = t4p::WxToChar(url.BuildURI());
	std::string stdUrl;
	std::string stdFullPath;
	std::string stdClassName;
	std::string stdMethodName;
	std::string sql = "SELECT url, full_path, class_name, method_name ";
	sql += "FROM url_tags LEFT JOIN sources ON sources.source_id = url_tags.source_id ";
	sql += "WHERE  url = ? AND directory IN(";
	std::vector<std::string> stdSourceDirs;
	for (size_t i = 0; i < sourceDirs.size(); ++i) {
		stdSourceDirs.push_back(t4p::WxToChar(sourceDirs[i].GetPathWithSep()));
		if (0 == i) {
			sql += "?";
		}
		else {
			sql += ",?";
		}
	}
	sql += ")";
	try {
		soci::statement stmt = Session.prepare << sql;
		stmt.exchange(soci::use(stdUrlWhere));
		for (size_t i = 0; i < stdSourceDirs.size(); ++i) {
			stmt.exchange(soci::use(stdSourceDirs[i]));
		}
		stmt.exchange(soci::into(stdUrl));
		stmt.exchange(soci::into(stdFullPath));
		stmt.exchange(soci::into(stdClassName));
		stmt.exchange(soci::into(stdMethodName));
		stmt.define_and_bind();
		if (stmt.execute(true)) {
			urlTag.Url.Create(t4p::CharToWx(stdUrl.c_str()));
			urlTag.FileName.Assign(t4p::CharToWx(stdFullPath.c_str()));
			urlTag.ClassName = t4p::CharToWx(stdClassName.c_str());
			urlTag.MethodName = t4p::CharToWx(stdMethodName.c_str());
			ret = true;
		}
	} catch (std::exception& e) {
		wxUnusedVar(e);
		wxString msg = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, msg);
	}
	return ret;
}

bool t4p::UrlTagFinderClass::FindByClassMethod(const wxString& className, const wxString& methodName, const std::vector<wxFileName>& sourceDirs, t4p::UrlTagClass& urlTag) {
	bool ret = false;
	if (sourceDirs.empty()) {
		return ret;
	}
	std::string stdClassNameWhere = t4p::WxToChar(className);
	std::string stdMethodNameWhere = t4p::WxToChar(methodName);

	std::string stdUrl;
	std::string stdFullPath;
	std::string stdClassName;
	std::string stdMethodName;
	std::string sql = "SELECT url, full_path, class_name, method_name ";
	sql += "FROM url_tags LEFT JOIN sources ON sources.source_id = url_tags.source_id ";
	sql += "WHERE class_name = ? AND method_name = ? AND directory IN(";
	std::vector<std::string> stdSourceDirs;
	for (size_t i = 0; i < sourceDirs.size(); ++i) {
		stdSourceDirs.push_back(t4p::WxToChar(sourceDirs[i].GetPathWithSep()));
		if (0 == i) {
			sql += "?";
		}
		else {
			sql += ",?";
		}
	}
	sql += ")";
	try {
		soci::statement stmt = Session.prepare << sql;
		stmt.exchange(soci::into(stdUrl));
		stmt.exchange(soci::into(stdFullPath));
		stmt.exchange(soci::into(stdClassName));
		stmt.exchange(soci::into(stdMethodName));
		stmt.exchange(soci::use(stdClassNameWhere));
		stmt.exchange(soci::use(stdMethodNameWhere));
		for (size_t i = 0; i < stdSourceDirs.size(); ++i) {
			stmt.exchange(soci::use(stdSourceDirs[i]));
		}
		stmt.define_and_bind();
		if (stmt.execute(true)) {
			urlTag.Url.Create(t4p::CharToWx(stdUrl.c_str()));
			urlTag.FileName.Assign(t4p::CharToWx(stdFullPath.c_str()));
			urlTag.ClassName = t4p::CharToWx(stdClassName.c_str());
			urlTag.MethodName = t4p::CharToWx(stdMethodName.c_str());
			ret = true;
		}
	} catch (std::exception& e) {
		wxUnusedVar(e);
		wxString msg = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, msg);
	}
	return ret;
}

bool t4p::UrlTagFinderClass::FilterByFullPath(const wxString& fullPath, const std::vector<wxFileName>& sourceDirs, std::vector<UrlTagClass>& urlTags) {
	bool ret = false;
	std::string stdFullPathWhere = t4p::WxToChar(fullPath);

	std::string stdUrl;
	std::string stdFullPath;
	std::string stdClassName;
	std::string stdMethodName;
	std::string sql = "SELECT url, full_path, class_name, method_name ";
	sql += "FROM url_tags LEFT JOIN sources ON sources.source_id = url_tags.source_id ";
	sql += "WHERE full_path = ? AND directory IN(";
	std::vector<std::string> stdSourceDirs;
	for (size_t i = 0; i < sourceDirs.size(); ++i) {
		stdSourceDirs.push_back(t4p::WxToChar(sourceDirs[i].GetPathWithSep()));
		if (0 == i) {
			sql += "?";
		}
		else {
			sql += ",?";
		}
	}
	sql += ")";
	try {
		soci::statement stmt = Session.prepare << sql;
		stmt.exchange(soci::into(stdUrl));
		stmt.exchange(soci::into(stdFullPath));
		stmt.exchange(soci::into(stdClassName));
		stmt.exchange(soci::into(stdMethodName));
		stmt.exchange(soci::use(stdFullPathWhere));
		for (size_t i = 0; i < stdSourceDirs.size(); ++i) {
			stmt.exchange(soci::use(stdSourceDirs[i]));
		}
		stmt.define_and_bind();
		if (stmt.execute(true)) {
			t4p::UrlTagClass urlTag;
			urlTag.Url.Create(t4p::CharToWx(stdUrl.c_str()));
			urlTag.FileName.Assign(t4p::CharToWx(stdFullPath.c_str()));
			urlTag.ClassName = t4p::CharToWx(stdClassName.c_str());
			urlTag.MethodName = t4p::CharToWx(stdMethodName.c_str());

			urlTags.push_back(urlTag);
			ret = true;
		}
	} catch (std::exception& e) {
		wxUnusedVar(e);
		wxString msg = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, msg);
	}
	return ret;
}

void t4p::UrlTagFinderClass::DeleteUrl(const wxURI& url, const std::vector<wxFileName>& sourceDirs) {
	if (sourceDirs.empty()) {
		return;
	}
	std::string stdUrl = t4p::WxToChar(url.BuildURI());
	try {
		std::vector<std::string> stdSourceDirs;
		std::string sql = "DELETE FROM url_tags WHERE url = ? AND source_id IN(SELECT source_id FROM sources WHERE directory IN(";
		for (size_t i = 0; i < sourceDirs.size(); ++i) {
			stdSourceDirs.push_back(t4p::WxToChar(sourceDirs[i].GetPathWithSep()));
			if (0 == i) {
				sql += "?";
			}
			else {
				sql += ",?";
			}
		}
		sql += "))";
		soci::statement stmt = Session.prepare << sql;
		stmt.exchange(soci::use(stdUrl));
		for (size_t i = 0; i < stdSourceDirs.size(); ++i) {
			stmt.exchange(soci::use(stdSourceDirs[i]));
		}
		stmt.define_and_bind();
		stmt.execute(true);

	} catch (std::exception& e) {
		wxUnusedVar(e);
		wxString msg = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, msg);
	}
}

void t4p::UrlTagFinderClass::FilterUrls(const wxString& filter, const std::vector<wxFileName>& sourceDirs, std::vector<UrlTagClass>& matchedUrls) {
	std::string stdUrl;
	std::string stdFullPath;
	std::string stdClassName;
	std::string stdMethodName;
	std::string stdFilter = t4p::WxToChar(filter);
	std::string escaped = t4p::SqliteSqlLikeEscape(stdFilter, '^');

	// hmmm... query might not be optimal for 1000s of urls
	// not sure if the number of urls will go into the 1000s
	std::string sql = "SELECT url, full_path, class_name, method_name ";
	sql += "FROM url_tags LEFT JOIN sources ON sources.source_id = url_tags.source_id ";
	sql += "WHERE url LIKE '%" + escaped + "%' ESCAPE '^'  AND directory IN(";
	std::vector<std::string> stdSourceDirs;
	for (size_t i = 0; i < sourceDirs.size(); ++i) {
		stdSourceDirs.push_back(t4p::WxToChar(sourceDirs[i].GetPathWithSep()));
		if (0 == i) {
			sql += "?";
		}
		else {
			sql += ",?";
		}
	}
	sql += ") LIMIT 100";
	try {
		soci::statement stmt = Session.prepare << sql;
		stmt.exchange(soci::into(stdUrl));
		stmt.exchange(soci::into(stdFullPath));
		stmt.exchange(soci::into(stdClassName));
		stmt.exchange(soci::into(stdMethodName));
		for (size_t i = 0; i < stdSourceDirs.size(); ++i) {
			stmt.exchange(soci::use(stdSourceDirs[i]));
		}
		stmt.define_and_bind();
		if (stmt.execute(true)) {
			do {
				t4p::UrlTagClass urlTag;
				urlTag.Url.Create(t4p::CharToWx(stdUrl.c_str()));
				urlTag.FileName.Assign(t4p::CharToWx(stdFullPath.c_str()));
				urlTag.ClassName = t4p::CharToWx(stdClassName.c_str());
				urlTag.MethodName = t4p::CharToWx(stdMethodName.c_str());

				matchedUrls.push_back(urlTag);
			} while (stmt.fetch());
		}
	}
	catch (std::exception& e) {
		wxUnusedVar(e);
		wxString msg = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, msg);
	}
}

void t4p::UrlTagFinderClass::Wipe(const std::vector<wxFileName>& sourceDirs) {
	if (sourceDirs.empty()) {
		return;
	}
	try {
		std::vector<std::string> stdSourceDirs;
		std::string sql = "DELETE FROM url_tags WHERE source_id IN(SELECT source_id FROM sources WHERE directory IN(";
		for (size_t i = 0; i < sourceDirs.size(); ++i) {
			stdSourceDirs.push_back(t4p::WxToChar(sourceDirs[i].GetPathWithSep()));
			if (0 == i) {
				sql += "?";
			}
			else {
				sql += ",?";
			}
		}
		sql += "))";
		soci::statement stmt = Session.prepare << sql;
		for (size_t i = 0; i < stdSourceDirs.size(); ++i) {
			stmt.exchange(soci::use(stdSourceDirs[i]));
		}
		stmt.define_and_bind();
		stmt.execute(true);
	} catch (std::exception& e) {
		wxUnusedVar(e);
		wxString msg = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, msg);
	}
}

int t4p::UrlTagFinderClass::Count(const std::vector<wxFileName>& sourceDirs) {
	int totalCount = 0;
	if (sourceDirs.empty()) {
		return totalCount;
	}
	int dbCount;
	std::string sql = "SELECT COUNT(*) ";
	sql += "FROM url_tags LEFT JOIN sources ON sources.source_id = url_tags.source_id ";
	sql += "WHERE directory IN(";
	std::vector<std::string> stdSourceDirs;
	for (size_t i = 0; i < sourceDirs.size(); ++i) {
		stdSourceDirs.push_back(t4p::WxToChar(sourceDirs[i].GetPathWithSep()));
		if (0 == i) {
			sql += "?";
		}
		else {
			sql += ",?";
		}
	}
	sql += ")";

	try {
		soci::statement stmt = Session.prepare << sql;
		stmt.exchange(soci::into(dbCount));
		for (size_t i = 0; i < stdSourceDirs.size(); ++i) {
			stmt.exchange(soci::use(stdSourceDirs[i]));
		}
		stmt.define_and_bind();
		stmt.execute(true);
		totalCount += dbCount;
	} catch (std::exception& e) {
		wxUnusedVar(e);
		wxString msg = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, msg);
	}
	return totalCount;
}

std::vector<wxString> t4p::UrlTagFinderClass::AllControllerNames(const std::vector<wxFileName>& sourceDirs) {
	std::vector<wxString> controllerNames;
	if (sourceDirs.empty()) {
		return controllerNames;
	}
	std::string controller;
	std::string sql = "SELECT DISTINCT class_name ";
	sql += "FROM url_tags LEFT JOIN sources ON sources.source_id = url_tags.source_id ";
	sql += "WHERE directory IN(";
	std::vector<std::string> stdSourceDirs;
	for (size_t i = 0; i < sourceDirs.size(); ++i) {
		stdSourceDirs.push_back(t4p::WxToChar(sourceDirs[i].GetPathWithSep()));
		if (0 == i) {
			sql += "?";
		}
		else {
			sql += ",?";
		}
	}
	sql += ")";
	try {
		soci::statement stmt = Session.prepare << sql;
		stmt.exchange(soci::into(controller));
		for (size_t i = 0; i < stdSourceDirs.size(); ++i) {
			stmt.exchange(soci::use(stdSourceDirs[i]));
		}
		stmt.define_and_bind();
		if (stmt.execute(true)) {
			do {
				controllerNames.push_back(t4p::CharToWx(controller.c_str()));
			} while (stmt.fetch());
		}
	} catch (std::exception& e) {
		wxString msg = t4p::CharToWx(e.what());
		wxUnusedVar(e);
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return controllerNames;
}

std::vector<wxString> t4p::UrlTagFinderClass::AllMethodNames(const wxString& controllerClassName, const std::vector<wxFileName>& sourceDirs) {
	std::vector<wxString> methodNames;
	if (sourceDirs.empty()) {
		return methodNames;
	}
	std::string methodName;
	std::string controllerWhere = t4p::WxToChar(controllerClassName);
	std::string sql = "SELECT DISTINCT method_name ";
	sql += "FROM url_tags LEFT JOIN sources ON sources.source_id = url_tags.source_id ";
	sql += "WHERE class_name  = ? AND directory IN(";
	std::vector<std::string> stdSourceDirs;
	for (size_t i = 0; i < sourceDirs.size(); ++i) {
		stdSourceDirs.push_back(t4p::WxToChar(sourceDirs[i].GetPathWithSep()));
		if (0 == i) {
			sql += "?";
		}
		else {
			sql += ",?";
		}
	}
	sql += ")";

	try {
		soci::statement stmt = Session.prepare << sql;
		stmt.exchange(soci::into(methodName));
		stmt.exchange(soci::use(controllerWhere));
		for (size_t i = 0; i < stdSourceDirs.size(); ++i) {
			stmt.exchange(soci::use(stdSourceDirs[i]));
		}
		stmt.define_and_bind();
		if (stmt.execute(true)) {
			do {
				methodNames.push_back(t4p::CharToWx(methodName.c_str()));
			} while (stmt.fetch());
		}
	} catch (std::exception& e) {
		wxString msg = t4p::CharToWx(e.what());
		wxUnusedVar(e);
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return methodNames;
}
