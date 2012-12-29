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
#include <globals/UrlResourceClass.h>
#include <globals/String.h>
#include <globals/Assets.h>
#include <globals/Sqlite.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <wx/ffile.h>

mvceditor::UrlResourceClass::UrlResourceClass()
 : Url()
 , FileName()
 , ClassName()
 , MethodName() {

 }

mvceditor::UrlResourceClass::UrlResourceClass(wxString uri) 
	: Url(uri)
	, FileName()
	, ClassName()
	, MethodName() {
}

mvceditor::UrlResourceClass::UrlResourceClass(const mvceditor::UrlResourceClass& src)
	: Url()	
	, FileName()
	, ClassName()
	, MethodName() {
	Copy(src);
 }

void mvceditor::UrlResourceClass::Reset() {
	Url.Create(wxT(""));
	FileName.Clear();
	ClassName.Clear();
	MethodName.Clear();
}

mvceditor::UrlResourceClass& mvceditor::UrlResourceClass::operator=(const mvceditor::UrlResourceClass& src) {
	Copy(src);
	return *this;
}

void mvceditor::UrlResourceClass::Copy(const mvceditor::UrlResourceClass& src) {
	Url = src.Url;
	FileName = src.FileName;
	ClassName = src.ClassName.c_str();
	MethodName = src.MethodName.c_str();
}

mvceditor::UrlResourceFinderClass::UrlResourceFinderClass()
	: Sessions() {
		
}

mvceditor::UrlResourceFinderClass::~UrlResourceFinderClass() {
	Close();
}

bool mvceditor::UrlResourceFinderClass::AttachFile(const wxFileName& fileName) {
	wxASSERT_MSG(fileName.IsOk(), _("File name given to UrlResourceFinderClass::InitFile is not OK."));
	if (!fileName.IsOk()) {
		return false;
	}
	bool isOpened = false;
	std::string stdDbName = mvceditor::WxToChar(fileName.GetFullPath());
	soci::session* session = new soci::session();
	try {
		session->open(*soci::factory_sqlite3(), stdDbName);
	
		// open the SQL script that contains the table creation statements
		// the script is "nice" it takes care to not create the tables if
		// they already exist
		wxFileName sqlScriptFileName = mvceditor::DetectorSqlSchemaAsset();
		wxString error;
		isOpened = mvceditor::SqlScript(sqlScriptFileName, *session, error);
		wxASSERT_MSG(isOpened, error);
		if (isOpened) {
			Sessions.push_back(session);
		}
	} catch(std::exception const& e) {
		isOpened = false;
		wxString msg = mvceditor::CharToWx(e.what());
		wxASSERT_MSG(isOpened, msg);
	}
	if (!isOpened) {
		session->close();
		delete session;
	}
	return isOpened;
}


bool mvceditor::UrlResourceFinderClass::FindByUrl(const wxURI& url, mvceditor::UrlResourceClass& urlResource) {
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
				"SELECT url, full_path, class_name, method_name FROM url_resources WHERE url = ? ",
				soci::use(stdUrlWhere),
				soci::into(stdUrl), soci::into(stdFullPath), 
				soci::into(stdClassName), soci::into(stdMethodName) 
			);
			if (stmt.execute(true)) {
				urlResource.Url.Create(mvceditor::CharToWx(stdUrl.c_str()));
				urlResource.FileName.Assign(mvceditor::CharToWx(stdFullPath.c_str()));
				urlResource.ClassName = mvceditor::CharToWx(stdClassName.c_str());
				urlResource.MethodName = mvceditor::CharToWx(stdMethodName.c_str());
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

void mvceditor::UrlResourceFinderClass::DeleteUrl(const wxURI& url) {
	if (Sessions.empty()) {
		return;
	}
	std::string stdUrl = mvceditor::WxToChar(url.BuildURI());
	std::vector<soci::session*>::iterator session;
	for (session = Sessions.begin(); session != Sessions.end(); ++session) {
		try {
			soci::statement stmt = ((*session)->prepare << 
				"DELETE FROM url_resources WHERE url = ? ",
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

void mvceditor::UrlResourceFinderClass::FilterUrls(const wxString& filter, std::vector<UrlResourceClass>& matchedUrls) {
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
		std::string escaped = mvceditor::SqlEscape(stdFilter, '^');

		// hmmm... query might not be optimal for 1000s of urls
		// not sure if the number of urls will go into the 1000s
		std::string sql = "SELECT url, full_path, class_name, method_name ";
		sql += "FROM url_resources WHERE url LIKE '%" + escaped + "%' ESCAPE '^' LIMIT 100";

		try {
			soci::statement stmt = ((*session)->prepare << sql,
				soci::into(stdUrl), soci::into(stdFullPath),
				soci::into(stdClassName), soci::into(stdMethodName)
			);
			if (stmt.execute(true)) {
				do {
					mvceditor::UrlResourceClass urlResource;
					urlResource.Url.Create(mvceditor::CharToWx(stdUrl.c_str()));
					urlResource.FileName.Assign(mvceditor::CharToWx(stdFullPath.c_str()));
					urlResource.ClassName = mvceditor::CharToWx(stdClassName.c_str());
					urlResource.MethodName = mvceditor::CharToWx(stdMethodName.c_str());

					matchedUrls.push_back(urlResource);
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

void mvceditor::UrlResourceFinderClass::Wipe() {
	if (Sessions.empty()) {
		return;
	}
	std::vector<soci::session*>::iterator session;
	for (session = Sessions.begin(); session != Sessions.end(); ++session) {
		try {
			(*session)->once << "DELETE FROM url_resources";
		} catch (std::exception& e) {
			wxUnusedVar(e);
			wxString msg = mvceditor::CharToWx(e.what());
			wxASSERT_MSG(false, msg);
		}
	}
}

int mvceditor::UrlResourceFinderClass::Count() {
	int totalCount = 0;
	if (Sessions.empty()) {
		return totalCount;
	}
	std::vector<soci::session*>::iterator session;
	for (session = Sessions.begin(); session != Sessions.end(); ++session) {
		int dbCount;
		try {
			soci::statement stmt = (
				(*session)->prepare << "SELECT COUNT(*) FROM url_resources", 
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

void mvceditor::UrlResourceFinderClass::Close() {
	std::vector<soci::session*>::iterator session;
	for (session = Sessions.begin(); session != Sessions.end(); ++session) {
		try {
			(*session)->close();
		} catch (std::exception& e) {
			wxUnusedVar(e);
			// ignore close exceptions since we want to clean up
		}
		delete (*session);
	}
	Sessions.clear();
}