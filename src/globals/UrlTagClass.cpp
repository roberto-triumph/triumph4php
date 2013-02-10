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
#include <globals/UrlTagClass.h>
#include <globals/String.h>
#include <globals/Assets.h>
#include <globals/Sqlite.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <wx/ffile.h>

mvceditor::UrlTagClass::UrlTagClass()
 : Url()
 , FileName()
 , ClassName()
 , MethodName() {

 }

mvceditor::UrlTagClass::UrlTagClass(wxString uri) 
	: Url(uri)
	, FileName()
	, ClassName()
	, MethodName() {
}

mvceditor::UrlTagClass::UrlTagClass(const mvceditor::UrlTagClass& src)
	: Url()	
	, FileName()
	, ClassName()
	, MethodName() {
	Copy(src);
 }

void mvceditor::UrlTagClass::Reset() {
	Url.Create(wxT(""));
	FileName.Clear();
	ClassName.Clear();
	MethodName.Clear();
}

mvceditor::UrlTagClass& mvceditor::UrlTagClass::operator=(const mvceditor::UrlTagClass& src) {
	Copy(src);
	return *this;
}

void mvceditor::UrlTagClass::Copy(const mvceditor::UrlTagClass& src) {
	Url = src.Url;
	FileName = src.FileName;
	ClassName = src.ClassName.c_str();
	MethodName = src.MethodName.c_str();
}

mvceditor::UrlTagFinderClass::UrlTagFinderClass()
	: SqliteFinderClass() {
		
}

bool mvceditor::UrlTagFinderClass::FindByUrl(const wxURI& url, mvceditor::UrlTagClass& urlTag) {
	bool ret = false;
	if (Sessions.empty()) {
		return ret;
	}
	std::vector<soci::session*>::iterator session;
	for (session = Sessions.begin(); session != Sessions.end(); ++session) {
		std::string stdUrlWhere = mvceditor::WxToChar(url.BuildURI());
		std::string stdUrl;
		std::string stdFullPath;
		std::string stdClassName;
		std::string stdMethodName;
		try {
			soci::statement stmt = ((*session)->prepare << 
				"SELECT url, full_path, class_name, method_name FROM url_tags WHERE url = ? ",
				soci::use(stdUrlWhere),
				soci::into(stdUrl), soci::into(stdFullPath), 
				soci::into(stdClassName), soci::into(stdMethodName) 
			);
			if (stmt.execute(true)) {
				urlTag.Url.Create(mvceditor::CharToWx(stdUrl.c_str()));
				urlTag.FileName.Assign(mvceditor::CharToWx(stdFullPath.c_str()));
				urlTag.ClassName = mvceditor::CharToWx(stdClassName.c_str());
				urlTag.MethodName = mvceditor::CharToWx(stdMethodName.c_str());
				ret = true;
			}
		} catch (std::exception& e) {
			wxUnusedVar(e);
			wxString msg = mvceditor::CharToWx(e.what());
			wxASSERT_MSG(false, msg);
		}
	}
	return ret;
}

bool mvceditor::UrlTagFinderClass::FindByClassMethod(const wxString& className, const wxString& methodName, mvceditor::UrlTagClass& urlTag) {
	bool ret = false;
	if (Sessions.empty()) {
		return ret;
	}
	std::string stdClassNameWhere = mvceditor::WxToChar(className);
	std::string stdMethodNameWhere = mvceditor::WxToChar(methodName);
	std::vector<soci::session*>::iterator session;
	for (session = Sessions.begin(); session != Sessions.end(); ++session) {
		std::string stdUrl;
		std::string stdFullPath;
		std::string stdClassName;
		std::string stdMethodName;
		try {
			soci::statement stmt = ((*session)->prepare << 
				"SELECT url, full_path, class_name, method_name FROM url_tags WHERE class_name = ? AND method_name = ?",
				soci::use(stdClassNameWhere), soci::use(stdMethodNameWhere),
				soci::into(stdUrl), soci::into(stdFullPath), 
				soci::into(stdClassName), soci::into(stdMethodName) 
			);
			if (stmt.execute(true)) {
				urlTag.Url.Create(mvceditor::CharToWx(stdUrl.c_str()));
				urlTag.FileName.Assign(mvceditor::CharToWx(stdFullPath.c_str()));
				urlTag.ClassName = mvceditor::CharToWx(stdClassName.c_str());
				urlTag.MethodName = mvceditor::CharToWx(stdMethodName.c_str());
				ret = true;
			}
		} catch (std::exception& e) {
			wxUnusedVar(e);
			wxString msg = mvceditor::CharToWx(e.what());
			wxASSERT_MSG(false, msg);
		}
	}
	return ret;
}

bool mvceditor::UrlTagFinderClass::FilterByFullPath(const wxString& fullPath, std::vector<UrlTagClass>& urlTags) {
	bool ret = false;
	if (Sessions.empty()) {
		return ret;
	}
	std::string stdFullPathWhere = mvceditor::WxToChar(fullPath);
	std::vector<soci::session*>::iterator session;
	for (session = Sessions.begin(); session != Sessions.end(); ++session) {
		std::string stdUrl;
		std::string stdFullPath;
		std::string stdClassName;
		std::string stdMethodName;
		try {
			soci::statement stmt = ((*session)->prepare << 
				"SELECT url, full_path, class_name, method_name FROM url_tags WHERE full_path = ?",
				soci::use(stdFullPathWhere),
				soci::into(stdUrl), soci::into(stdFullPath), 
				soci::into(stdClassName), soci::into(stdMethodName) 
			);
			if (stmt.execute(true)) {
				mvceditor::UrlTagClass urlTag;
				urlTag.Url.Create(mvceditor::CharToWx(stdUrl.c_str()));
				urlTag.FileName.Assign(mvceditor::CharToWx(stdFullPath.c_str()));
				urlTag.ClassName = mvceditor::CharToWx(stdClassName.c_str());
				urlTag.MethodName = mvceditor::CharToWx(stdMethodName.c_str());

				urlTags.push_back(urlTag);
				ret = true;
			}
		} catch (std::exception& e) {
			wxUnusedVar(e);
			wxString msg = mvceditor::CharToWx(e.what());
			wxASSERT_MSG(false, msg);
		}
	}
	return ret;
}

void mvceditor::UrlTagFinderClass::DeleteUrl(const wxURI& url) {
	if (Sessions.empty()) {
		return;
	}
	std::string stdUrl = mvceditor::WxToChar(url.BuildURI());
	std::vector<soci::session*>::iterator session;
	for (session = Sessions.begin(); session != Sessions.end(); ++session) {
		try {
			soci::statement stmt = ((*session)->prepare << 
				"DELETE FROM url_tags WHERE url = ? ",
				soci::use(stdUrl)
			);
			stmt.execute(true);
		} catch (std::exception& e) {
			wxUnusedVar(e);
			wxString msg = mvceditor::CharToWx(e.what());
			wxASSERT_MSG(false, msg);
		}
	}
}

void mvceditor::UrlTagFinderClass::FilterUrls(const wxString& filter, std::vector<UrlTagClass>& matchedUrls) {
	if (Sessions.empty()) {
		return;
	}
	std::vector<soci::session*>::iterator session;
	for (session = Sessions.begin(); session != Sessions.end(); ++session) {
		std::string stdUrl;
		std::string stdFullPath;
		std::string stdClassName;
		std::string stdMethodName;
		std::string stdFilter = mvceditor::WxToChar(filter);
		std::string escaped = mvceditor::SqliteSqlEscape(stdFilter, '^');

		// hmmm... query might not be optimal for 1000s of urls
		// not sure if the number of urls will go into the 1000s
		std::string sql = "SELECT url, full_path, class_name, method_name ";
		sql += "FROM url_tags WHERE url LIKE '%" + escaped + "%' ESCAPE '^' LIMIT 100";

		try {
			soci::statement stmt = ((*session)->prepare << sql,
				soci::into(stdUrl), soci::into(stdFullPath),
				soci::into(stdClassName), soci::into(stdMethodName)
			);
			if (stmt.execute(true)) {
				do {
					mvceditor::UrlTagClass urlTag;
					urlTag.Url.Create(mvceditor::CharToWx(stdUrl.c_str()));
					urlTag.FileName.Assign(mvceditor::CharToWx(stdFullPath.c_str()));
					urlTag.ClassName = mvceditor::CharToWx(stdClassName.c_str());
					urlTag.MethodName = mvceditor::CharToWx(stdMethodName.c_str());

					matchedUrls.push_back(urlTag);
				} while (stmt.fetch());
			}
		}
		catch (std::exception& e) {
			wxUnusedVar(e);
			wxString msg = mvceditor::CharToWx(e.what());
			wxASSERT_MSG(false, msg);
		}
	}
}

void mvceditor::UrlTagFinderClass::Wipe() {
	if (Sessions.empty()) {
		return;
	}
	std::vector<soci::session*>::iterator session;
	for (session = Sessions.begin(); session != Sessions.end(); ++session) {
		try {
			(*session)->once << "DELETE FROM url_tags";
		} catch (std::exception& e) {
			wxUnusedVar(e);
			wxString msg = mvceditor::CharToWx(e.what());
			wxASSERT_MSG(false, msg);
		}
	}
}

int mvceditor::UrlTagFinderClass::Count() {
	int totalCount = 0;
	if (Sessions.empty()) {
		return totalCount;
	}
	std::vector<soci::session*>::iterator session;
	for (session = Sessions.begin(); session != Sessions.end(); ++session) {
		int dbCount;
		try {
			soci::statement stmt = (
				(*session)->prepare << "SELECT COUNT(*) FROM url_tags", 
				soci::into(dbCount));
			stmt.execute(true);

			// aggregate counts across all dbs
			totalCount += dbCount;
		} catch (std::exception& e) {
			wxUnusedVar(e);
			wxString msg = mvceditor::CharToWx(e.what());
			wxASSERT_MSG(false, msg);
		}
	}
	return totalCount;
}

std::vector<wxString> mvceditor::UrlTagFinderClass::AllControllerNames() {
	std::vector<wxString> controllerNames;
	std::vector<soci::session*>::iterator session;
	std::string controller;
	for (session = Sessions.begin(); session != Sessions.end(); ++session) {
		try {
			soci::statement stmt =((*session)->prepare <<
				"SELECT DISTINCT class_name FROM url_tags", soci::into(controller)
			);
			if (stmt.execute(true)) {
				do {
					controllerNames.push_back(mvceditor::CharToWx(controller.c_str()));
				} while (stmt.fetch());
			}
		} catch (std::exception& e) {
			wxString msg = mvceditor::CharToWx(e.what());
			wxUnusedVar(e);
			wxUnusedVar(msg);
			wxASSERT_MSG(false, msg);
		}
	}
	return controllerNames;
}

std::vector<wxString> mvceditor::UrlTagFinderClass::AllMethodNames(const wxString& controllerClassName) {
	std::vector<wxString> methodNames;
	std::vector<soci::session*>::iterator session;
	std::string methodName;
	std::string controllerWhere = mvceditor::WxToChar(controllerClassName);
	for (session = Sessions.begin(); session != Sessions.end(); ++session) {
		try {
			soci::statement stmt =((*session)->prepare <<
				"SELECT DISTINCT method_name FROM url_tags WHERE class_name = ?", 
				soci::use(controllerWhere), soci::into(methodName)
			);
			if (stmt.execute(true)) {
				do {
					methodNames.push_back(mvceditor::CharToWx(methodName.c_str()));
				} while (stmt.fetch());
			}
		} catch (std::exception& e) {
			wxString msg = mvceditor::CharToWx(e.what());
			wxUnusedVar(e);
			wxUnusedVar(msg);
			wxASSERT_MSG(false, msg);
		}
	}
	return methodNames;
}