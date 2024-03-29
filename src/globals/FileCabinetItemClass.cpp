/*
 * @copyright  2014 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
*/
#include "globals/FileCabinetItemClass.h"
#include <string>
#include "globals/String.h"

t4p::FileCabinetItemClass::FileCabinetItemClass()
    : Id(0)
    , FileName() {
}

t4p::FileCabinetItemClass::FileCabinetItemClass(const t4p::FileCabinetItemClass& src)
    : Id(0)
    , FileName() {
    Copy(src);
}

t4p::FileCabinetItemClass& t4p::FileCabinetItemClass::operator=(const t4p::FileCabinetItemClass& src) {
    Copy(src);
    return *this;
}

void t4p::FileCabinetItemClass::Copy(const t4p::FileCabinetItemClass& src) {
    Id = src.Id;
    FileName = src.FileName;
}

bool t4p::FileCabinetItemClass::Exists() const {
    return FileName.FileExists() || FileName.DirExists();
}

bool t4p::FileCabinetItemClass::IsDir() const {
    return FileName.DirExists();
}

bool t4p::FileCabinetItemClass::IsFile() const {
    return FileName.FileExists();
}


t4p::FileCabinetStoreClass::FileCabinetStoreClass() {
}

bool t4p::FileCabinetStoreClass::Store(soci::session& session, t4p::FileCabinetItemClass& item) {
    bool ret = false;
    wxString error;
    try {
        std::string sql = "INSERT INTO file_cabinet_items(file_cabinet_item_id, name, full_path) VALUES(NULL, ?, ?)";
        std::string name;
        std::string fullPath;

        // not using IsDir() because during tests we insert
        // directories that don't exist
        if (item.FileName.GetFullName().empty()) {
            name = t4p::WxToChar(item.FileName.GetDirs().Last());
            fullPath = item.FileName.GetPathWithSep();
        } else {
            name = t4p::WxToChar(item.FileName.GetFullName());
            fullPath = t4p::WxToChar(item.FileName.GetFullPath());
        }

        soci::statement stmt = (session.prepare << sql,
                                soci::use(name), soci::use(fullPath));
        stmt.execute(true);

        item.Id = t4p::SqliteInsertId(stmt);

        ret = true;
    } catch (std::exception& e) {
        error = e.what();
        wxASSERT_MSG(false, error);
    }

    return ret;
}


bool t4p::FileCabinetStoreClass::Delete(soci::session& session, int fileCabinetItemId) {
    bool ret = false;
    wxString error;
    try {
        std::string sql = "DELETE FROM file_cabinet_items WHERE file_cabinet_item_id = ?";
        session.once << sql, soci::use(fileCabinetItemId);
        ret = true;
    } catch (std::exception& e) {
        error = e.what();
        wxASSERT_MSG(false, error);
    }

    return ret;
}

t4p::FileCabinetBaseResultClass::FileCabinetBaseResultClass()
    : SqliteResultClass()
    , Item()
    , Id(0)
    , FullPath() {
}

void t4p::FileCabinetBaseResultClass::DoBind(soci::statement& stmt) {
    stmt.exchange(soci::into(Id));
    stmt.exchange(soci::into(FullPath));
}


void t4p::FileCabinetBaseResultClass::Next() {
    if (FullPath.empty()) {
        Item.Id = 0;
        Item.FileName.Assign(wxT(""));
    } else if (FullPath[FullPath.length() - 1] == '\\' ||
               FullPath[FullPath.length() - 1] == '/') {
        // check of the full path is a directory
        // check the string ending instead of actually hitting the
        // disk, don't want to do 100+ disk accesses when
        // looping through results

        Item.Id = Id;
        Item.FileName.AssignDir(FullPath);
    } else {
        Item.Id = Id;
        Item.FileName.Assign(FullPath);
    }
    Fetch();
}

t4p::AllFileCabinetResultClass::AllFileCabinetResultClass()
    : FileCabinetBaseResultClass() {
}

bool t4p::AllFileCabinetResultClass::DoPrepare(soci::statement& stmt, bool doLimit) {
    std::string sql = "SELECT file_cabinet_item_id, full_path FROM file_cabinet_items ORDER BY file_cabinet_item_id";
    if (doLimit) {
        sql += " LIMIT 100";
    }

    wxString error;
    bool good = false;
    try {
        stmt.prepare(sql);
        good = true;
    } catch (std::exception& e) {
        error = e.what();
        wxASSERT_MSG(false, error);
    }
    return good;
}

t4p::SingleFileCabinetResultClass::SingleFileCabinetResultClass()
    : FileCabinetBaseResultClass()
    , QueryId(0) {
}

void t4p::SingleFileCabinetResultClass::SetId(int id) {
    QueryId = id;
}

bool t4p::SingleFileCabinetResultClass::DoPrepare(soci::statement& stmt, bool doLimit) {
    std::string sql = "SELECT file_cabinet_item_id, full_path FROM file_cabinet_items WHERE file_cabinet_item_id = ?";

    wxString error;
    bool good = false;
    try {
        stmt.prepare(sql);
        stmt.exchange(soci::use(QueryId));
        good = true;
    } catch (std::exception& e) {
        error = e.what();
        wxASSERT_MSG(false, error);
    }
    return good;
}

t4p::FileCabinetExactSearchResultClass::FileCabinetExactSearchResultClass()
    : FileCabinetBaseResultClass()
    , Name() {
}

void t4p::FileCabinetExactSearchResultClass::SetName(const std::string& name) {
    Name = name;
}

bool t4p::FileCabinetExactSearchResultClass::DoPrepare(soci::statement& stmt, bool doLimit) {
    std::string sql = "SELECT file_cabinet_item_id, full_path FROM file_cabinet_items WHERE name = ?";
    if (doLimit) {
        sql += " LIMIT 100";
    }

    wxString error;
    bool good = false;
    try {
        stmt.prepare(sql);
        stmt.exchange(soci::use(Name));
        good = true;
    } catch (std::exception& e) {
        error = e.what();
        wxASSERT_MSG(false, error);
    }
    return good;
}

t4p::FileCabinetNearMatchResultClass::FileCabinetNearMatchResultClass()
    : FileCabinetBaseResultClass()
    , Name() {
}

void t4p::FileCabinetNearMatchResultClass::SetName(const std::string& name) {
    Name = name;
}

bool t4p::FileCabinetNearMatchResultClass::DoPrepare(soci::statement& stmt, bool doLimit) {
    std::string escaped = t4p::SqliteSqlLikeEscape(Name, '^');
    escaped = "'%" + escaped + "%'";

    std::string sql = "SELECT file_cabinet_item_id, full_path FROM file_cabinet_items ";
    sql += "WHERE name LIKE " + escaped + " ESCAPE '^' ";
    if (doLimit) {
        sql += " LIMIT 100";
    }

    wxString error;
    bool good = false;
    try {
        stmt.prepare(sql);
        good = true;
    } catch (std::exception& e) {
        error = e.what();
        wxASSERT_MSG(false, error);
    }
    return good;
}
