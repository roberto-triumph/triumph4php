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
#include <language/ParsedTagFinderClass.h>
#include <search/FinderClass.h>
#include <globals/String.h>
#include <globals/Errors.h>
#include <language/FileTags.h>
#include <wx/filename.h>
#include <algorithm>
#include <fstream>
#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include <unicode/fmtable.h>
#include <unicode/numfmt.h>
#include <soci/soci.h>
#include <string>

namespace mvceditor {

class AllMembersTagResultClass : public mvceditor::ExactMemberTagResultClass {

public:

	AllMembersTagResultClass();

	void Set(const std::vector<UnicodeString>& classNames, const UnicodeString& memberName, const std::vector<wxFileName>& sourceDirs);

	bool Prepare(soci::session& session, bool doLimit);

private:

	int ClassCount;
};

class ExactNonMemberTagResultClass : public mvceditor::TagResultClass {

public:

	ExactNonMemberTagResultClass();

	bool Prepare(soci::session& session, bool doLimit);
	
	virtual void Set(const UnicodeString& key, const std::vector<wxFileName>& sourceDirs);
	
	void SetFileTagId(int fileTagId);

	void SetTagType(mvceditor::TagClass::Types type);

protected:

	std::string Key;

	std::vector<int> TagTypes;

	std::vector<std::string> SourceDirs;
	
	int FileTagIdSearch;
};

class NearMatchNonMemberTagResultClass : public mvceditor::ExactNonMemberTagResultClass {

public:

	NearMatchNonMemberTagResultClass();

	void Set(const UnicodeString& key, const std::vector<wxFileName>& sourceDirs);

	void AddTagType(mvceditor::TagClass::Types type);

	bool Prepare(soci::session& session, bool doLimit);

private:

	std::string KeyUpper;
};

// special case, query across all classes for a method (::getName)
// if ClassName is empty, then just check method names This ensures 
// queries like '::getName' will work as well.
// make sure to NOT get fully qualified  matches (key=identifier)
class ExactMemberOnlyTagResultClass : public mvceditor::TagResultClass {

public:

	ExactMemberOnlyTagResultClass();

	bool Prepare(soci::session& session, bool doLimit);
	
	void Set(const UnicodeString& key, const std::vector<wxFileName>& sourceDirs);

protected:

	std::string Key;

	std::vector<int> TagTypes;

	std::vector<std::string> SourceDirs;
};

class NearMatchMemberOnlyTagResultClass : public ExactMemberOnlyTagResultClass {

public:
	NearMatchMemberOnlyTagResultClass();

	void Set(const UnicodeString& key, const std::vector<wxFileName>& sourceDirs);

	bool Prepare(soci::session& session, bool doLimit);

private:

	std::string KeyUpper;
};

class TopLevelTagInFileResultClass : public TagResultClass {

public:
	TopLevelTagInFileResultClass();

	void Set(const wxString& fullPath);

	bool Prepare(soci::session& session, bool doLimit);

private:

	std::string FullPath;

	std::vector<int> TagTypes;
};

class AllTagsResultClass : public TagResultClass {

public:

	AllTagsResultClass();

	bool Prepare(soci::session& session, bool doLimit);
};

/**
 * queries for tags by their primary key
 */
class TagByIdResultClass : public TagResultClass {

public:

	TagByIdResultClass();

	void Set(int id);

	bool Prepare(soci::session& session, bool doLimit);

private:

	int Id;
};


}

/**
 * appends name to namespace
 */
static UnicodeString QualifyName(const UnicodeString& namespaceName, const UnicodeString& name) {
	UnicodeString qualifiedName;
	qualifiedName.append(namespaceName);
	if (!qualifiedName.endsWith(UNICODE_STRING_SIMPLE("\\"))) {
		qualifiedName.append(UNICODE_STRING_SIMPLE("\\"));
	}
	qualifiedName.append(name);
	return qualifiedName;
}

mvceditor::ExactMemberTagResultClass::ExactMemberTagResultClass()
	: TagResultClass()
	, Keys()
	, TagTypes()
	, SourceDirs() {
	TagTypes.push_back(mvceditor::TagClass::CLASS_CONSTANT);
	TagTypes.push_back(mvceditor::TagClass::MEMBER);
	TagTypes.push_back(mvceditor::TagClass::METHOD);
}

void mvceditor::ExactMemberTagResultClass::Set(const std::vector<UnicodeString>& classNames, const UnicodeString& memberName, 
											   const std::vector<wxFileName>& sourceDirs) {
	wxASSERT_MSG(!classNames.empty(), wxT("classNames must not be empty"));
	for (size_t i = 0; i < classNames.size(); i++) {
		UnicodeString key = classNames[i] + UNICODE_STRING_SIMPLE("::") + memberName;
		Keys.push_back(mvceditor::IcuToChar(key));
	}

	// add trailing separator to lookup exact matches that include the last directory 
	// we dont want to get directories that start with a prefix
	for (size_t i = 0; i < sourceDirs.size(); i++) {
		SourceDirs.push_back(mvceditor::WxToChar(sourceDirs[i].GetPathWithSep()));
	}
}

bool mvceditor::ExactMemberTagResultClass::Prepare(soci::session& session,  bool doLimit) {

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON(r.file_item_id = f.file_item_id) LEFT JOIN sources s ON (s.source_id = r.source_id) WHERE ";
	
	sql += "key IN (?";
	for (size_t i = 1; i < Keys.size(); ++i) {
		sql += ",?";
	}
	sql += ")";
	sql += " AND type IN(?, ?, ?)";
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
	soci::statement* stmt = new soci::statement(session);
	stmt->prepare(sql);

	for (size_t i = 0; i < Keys.size(); i++) {
		stmt->exchange(soci::use(Keys[i]));
	}
	for (size_t i = 0; i < TagTypes.size(); i++) {
		stmt->exchange(soci::use(TagTypes[i]));
	}
	for (size_t i = 0; i < SourceDirs.size(); i++) {
		stmt->exchange(soci::use(SourceDirs[i]));
	}
	return Init(stmt);
}

mvceditor::AllMembersTagResultClass::AllMembersTagResultClass()
	: ExactMemberTagResultClass()
	, ClassCount(0) {
}

void mvceditor::AllMembersTagResultClass::Set(const std::vector<UnicodeString>& classNames, const UnicodeString& memberName, 
											   const std::vector<wxFileName>& sourceDirs) {
	wxASSERT_MSG(!classNames.empty(), wxT("classNames must not be empty"));
	for (size_t i = 0; i < classNames.size(); i++) {
		UnicodeString key = classNames[i] + UNICODE_STRING_SIMPLE("::");

		// put the key in two times so that we can buld a BETWEEN expression to emulate a LIKE
		Keys.push_back(mvceditor::IcuToChar(key));
		Keys.push_back(mvceditor::IcuToChar(key) + "zzzzzzzzzz");
	}
	// add trailing separator to lookup exact matches that include the last directory 
	// we dont want to get directories that start with a prefix
	for (size_t i = 0; i < sourceDirs.size(); i++) {
		SourceDirs.push_back(mvceditor::WxToChar(sourceDirs[i].GetPathWithSep()));
	}
	ClassCount = classNames.size();
}

bool mvceditor::AllMembersTagResultClass::Prepare(soci::session& session,  bool doLimit) {
	
	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON(r.file_item_id = f.file_item_id) LEFT JOIN sources s ON(s.source_id = r.source_id) WHERE ";

	// not using LIKE operator here, there are way too many situations where it wont use the index
	// index won't be used when ESCAPE is used or when sqlite3_prepare_v2 is NOT used (which soci does not use)
	// see http://sqlite.org/optoverview.html (LIKE optimization)
	// instead we use a trick to mimic LIKE using the BETWEEN
	// http://stackoverflow.com/questions/13056193/escape-wildcards-in-sqlite-like-without-sacrificing-index-use
	sql += "((key BETWEEN ? AND ?) ";
	for (int i = 1; i < ClassCount; ++i) {
		sql += " OR (key BETWEEN ? AND ?)";
	}
	sql += ") ";
	sql += " AND type IN(?, ?, ?)";
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
	soci::statement* stmt = new soci::statement(session);
	stmt->prepare(sql);
	for (size_t i = 0; i < Keys.size(); i++) {
		stmt->exchange(soci::use(Keys[i]));
	}
	for (size_t i = 0; i < TagTypes.size(); i++) {
		stmt->exchange(soci::use(TagTypes[i]));
	}
	for (size_t i = 0; i < SourceDirs.size(); i++) {
		stmt->exchange(soci::use(SourceDirs[i]));
	}
	return Init(stmt);
}


mvceditor::NearMatchMemberTagResultClass::NearMatchMemberTagResultClass()
	: ExactMemberTagResultClass() 
	, ClassCount(0) 
	, FileItemId(0) {
}

void mvceditor::NearMatchMemberTagResultClass::Set(const std::vector<UnicodeString>& classNames, const UnicodeString& memberName,
												int fileItemId,
											   const std::vector<wxFileName>& sourceDirs) {
	wxASSERT_MSG(!classNames.empty(), wxT("classNames must not be empty"));
	for (size_t i = 0; i < classNames.size(); i++) {
		UnicodeString key = classNames[i] + UNICODE_STRING_SIMPLE("::") + memberName;

		// put the key in two times so that we can buld a BETWEEN expression to emulate a LIKE
		Keys.push_back(mvceditor::IcuToChar(key));
		Keys.push_back(mvceditor::IcuToChar(key) + "zzzzzzzzzz");
	}

	// add trailing separator to lookup exact matches that include the last directory 
	// we dont want to get directories that start with a prefix
	for (size_t i = 0; i < sourceDirs.size(); i++) {
		SourceDirs.push_back(mvceditor::WxToChar(sourceDirs[i].GetPathWithSep()));
	}
	ClassCount = classNames.size();
	FileItemId = fileItemId;
}


bool mvceditor::NearMatchMemberTagResultClass::Prepare(soci::session& session,  bool doLimit) {
	wxASSERT_MSG(!Keys.empty(), wxT("keys cannot be empty"));

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON(r.file_item_id = f.file_item_id) LEFT JOIN sources s  ON(s.source_id = r.source_id) WHERE ";

	// not using LIKE operator here, there are way too many situations where it wont use the index
	// index won't be used when ESCAPE is used or when sqlite3_prepare_v2 is NOT used (which soci does not use)
	// see http://sqlite.org/optoverview.html (LIKE optimization)
	// instead we use a trick to mimic LIKE using the BETWEEN
	// http://stackoverflow.com/questions/13056193/escape-wildcards-in-sqlite-like-without-sacrificing-index-use
	sql += "((key BETWEEN ? AND ?) ";
	for (int i = 1; i < ClassCount; ++i) {
		sql += " OR (key BETWEEN ? AND ?)";
	}
	sql += ") ";
	sql += " AND type IN(?, ?, ?)";
	if (!SourceDirs.empty()) {
		sql += " AND s.directory IN(?";
		for (size_t i = 1; i <  SourceDirs.size(); ++i) {
			sql += ",?"; 
		}
		sql += ")";
	}
	if (FileItemId) {
		sql += " AND f.file_item_id = ? ";
	}
	sql += " ORDER BY key";
	if (doLimit) {
		sql += " LIMIT 100";
	}
	soci::statement* stmt = new soci::statement(session);
	stmt->prepare(sql);
	for (size_t i = 0; i < Keys.size(); i++) {
		stmt->exchange(soci::use(Keys[i]));
	}
	for (size_t i = 0; i < TagTypes.size(); i++) {
		stmt->exchange(soci::use(TagTypes[i]));
	}
	for (size_t i = 0; i < SourceDirs.size(); i++) {
		stmt->exchange(soci::use(SourceDirs[i]));
	}
	if (FileItemId) {
		stmt->exchange(soci::use(FileItemId));
	}
	return Init(stmt);
}

mvceditor::ExactNonMemberTagResultClass::ExactNonMemberTagResultClass()
	: TagResultClass()
	, Key() 
	, TagTypes()
	, SourceDirs() 
	, FileTagIdSearch(0) {
	TagTypes.push_back(mvceditor::TagClass::DEFINE);
	TagTypes.push_back(mvceditor::TagClass::CLASS);
	TagTypes.push_back(mvceditor::TagClass::FUNCTION);
}

void mvceditor::ExactNonMemberTagResultClass::Set(const UnicodeString& key, const std::vector<wxFileName>& sourceDirs) {
	Key = mvceditor::IcuToChar(key);
	
	// add trailing separator to lookup exact matches that include the last directory 
	// we dont want to get directories that start with a prefix
	for (size_t i = 0; i < sourceDirs.size(); i++) {
		SourceDirs.push_back(mvceditor::WxToChar(sourceDirs[i].GetPathWithSep()));
	}
}

void mvceditor::ExactNonMemberTagResultClass::SetTagType(mvceditor::TagClass::Types type) {
	TagTypes.clear();
	TagTypes.push_back(type);
}

void mvceditor::ExactNonMemberTagResultClass::SetFileTagId(int fileTagId) {
	FileTagIdSearch = fileTagId;
}

bool mvceditor::ExactNonMemberTagResultClass::Prepare(soci::session& session, bool doLimit) {

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON(r.file_item_id = f.file_item_id) LEFT JOIN sources s ON(s.source_id = r.source_id) WHERE ";
	sql += "key = ? AND type IN(?";
	for (size_t i = 1; i < TagTypes.size(); i++) {
		sql += ",?";
	}
	sql += ")";
	if (!SourceDirs.empty()) {
		sql += " AND s.directory IN(?";
		for (size_t i = 1; i < SourceDirs.size(); ++i) {
			sql += ",?"; 
		}
		sql += ")";
	}
	if (FileTagIdSearch) {
		sql += " AND f.file_item_id = ? ";
	}
	sql += " ORDER BY key";
	if (doLimit) {
		sql += " LIMIT 100";
	}
	soci::statement* stmt = new soci::statement(session);
	stmt->prepare(sql);
	stmt->exchange(soci::use(Key));
	for (size_t i = 0; i < TagTypes.size(); i++) {
		stmt->exchange(soci::use(TagTypes[i]));
	}
	for (size_t i = 0; i < SourceDirs.size(); i++) {
		stmt->exchange(soci::use(SourceDirs[i]));
	}
	if (FileTagIdSearch) {
		stmt->exchange(soci::use(FileTagIdSearch));
	}
	return Init(stmt);
}

mvceditor::NearMatchNonMemberTagResultClass::NearMatchNonMemberTagResultClass()
	: ExactNonMemberTagResultClass() 
	, KeyUpper() {

}

void mvceditor::NearMatchNonMemberTagResultClass::Set(const UnicodeString& key, const std::vector<wxFileName>& sourceDirs) {
	Key = mvceditor::IcuToChar(key);
	KeyUpper = mvceditor::IcuToChar(key) + "zzzzzzzzzz";
	
	// add trailing separator to lookup exact matches that include the last directory 
	// we dont want to get directories that start with a prefix
	for (size_t i = 0; i < sourceDirs.size(); i++) {
		SourceDirs.push_back(mvceditor::WxToChar(sourceDirs[i].GetPathWithSep()));
	}
}

void mvceditor::NearMatchNonMemberTagResultClass::AddTagType(mvceditor::TagClass::Types type) {
	TagTypes.push_back(type);
}

bool mvceditor::NearMatchNonMemberTagResultClass::Prepare(soci::session& session, bool doLimit) {

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON(r.file_item_id = f.file_item_id) LEFT JOIN sources s ON(s.source_id = r.source_id) WHERE";

	// not using LIKE operator here, there are way too many situations where it wont use the index
	// index won't be used when ESCAPE is used or when sqlite3_prepare_v2 is NOT used (which soci does not use)
	// see http://sqlite.org/optoverview.html (LIKE optimization)
	// instead we use a trick to mimic LIKE using the BETWEEN
	// http://stackoverflow.com/questions/13056193/escape-wildcards-in-sqlite-like-without-sacrificing-index-use
	sql += " key BETWEEN ? AND ? ";
	sql += " AND r.type IN(?";
	for (size_t i = 1; i < TagTypes.size(); ++i) {
		sql += ",?"; 
	}
	sql += ")";

	if (!SourceDirs.empty()) {
		sql += " AND s.directory IN(?";
		for (size_t i = 1; i < SourceDirs.size(); ++i) {
			sql += ",?"; 
		}
		sql += ")";
	}
	sql += " ORDER BY key";
	if (doLimit) {
		sql += " LIMIT 100";
	}
	soci::statement* stmt = new soci::statement(session);
	stmt->prepare(sql);
	stmt->exchange(soci::use(Key));
	stmt->exchange(soci::use(KeyUpper));
	for (size_t i = 0; i < TagTypes.size(); i++) {
		stmt->exchange(soci::use(TagTypes[i]));
	}
	for (size_t i = 0; i < SourceDirs.size(); i++) {
		stmt->exchange(soci::use(SourceDirs[i]));
	}
	return Init(stmt);
}

mvceditor::ExactMemberOnlyTagResultClass::ExactMemberOnlyTagResultClass()
	: TagResultClass()
	, Key()
	, TagTypes()
	, SourceDirs() {
	TagTypes.push_back(mvceditor::TagClass::CLASS_CONSTANT);
	TagTypes.push_back(mvceditor::TagClass::MEMBER);
	TagTypes.push_back(mvceditor::TagClass::METHOD);
}

void mvceditor::ExactMemberOnlyTagResultClass::Set(const UnicodeString& memberName, 
											   const std::vector<wxFileName>& sourceDirs) {
	Key = mvceditor::IcuToChar(memberName);
	
	// add trailing separator to lookup exact matches that include the last directory 
	// we dont want to get directories that start with a prefix
	for (size_t i = 0; i < sourceDirs.size(); i++) {
		SourceDirs.push_back(mvceditor::WxToChar(sourceDirs[i].GetPathWithSep()));
	}
}

bool mvceditor::ExactMemberOnlyTagResultClass::Prepare(soci::session& session,  bool doLimit) {

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON(r.file_item_id = f.file_item_id) LEFT JOIN sources s ON(s.source_id = r.source_id) WHERE ";
	
	// make sure to use the key because it is indexed
	sql += "key = ? AND identifier = key";
	sql += " AND type IN(?, ?, ?)";
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
	soci::statement* stmt = new soci::statement(session);
	stmt->prepare(sql);

	stmt->exchange(soci::use(Key));
	for (size_t i = 0; i < TagTypes.size(); i++) {
		stmt->exchange(soci::use(TagTypes[i]));
	}
	for (size_t i = 0; i < SourceDirs.size(); i++) {
		stmt->exchange(soci::use(SourceDirs[i]));
	}
	return Init(stmt);
}

mvceditor::NearMatchMemberOnlyTagResultClass::NearMatchMemberOnlyTagResultClass()
: ExactMemberOnlyTagResultClass() 
, KeyUpper() {

}

void mvceditor::NearMatchMemberOnlyTagResultClass::Set(const UnicodeString& key, const std::vector<wxFileName>& sourceDirs) {
	Key = mvceditor::IcuToChar(key);
	KeyUpper = mvceditor::IcuToChar(key) + "zzzzzzzzzz";
	
	// add trailing separator to lookup exact matches that include the last directory 
	// we dont want to get directories that start with a prefix
	for (size_t i = 0; i < sourceDirs.size(); i++) {
		SourceDirs.push_back(mvceditor::WxToChar(sourceDirs[i].GetPathWithSep()));
	}
}

bool mvceditor::NearMatchMemberOnlyTagResultClass::Prepare(soci::session& session, bool doLimit) {

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON(r.file_item_id = f.file_item_id) LEFT JOIN sources s ON(s.source_id = r.source_id) WHERE ";
	
	// not using LIKE operator here, there are way too many situations where it wont use the index
	// index won't be used when ESCAPE is used or when sqlite3_prepare_v2 is NOT used (which soci does not use)
	// see http://sqlite.org/optoverview.html (LIKE optimization)
	// instead we use a trick to mimic LIKE using the BETWEEN
	// http://stackoverflow.com/questions/13056193/escape-wildcards-in-sqlite-like-without-sacrificing-index-use
	sql += "key BETWEEN ? AND ? AND identifier = key";
	sql += " AND type IN(?, ?, ?)";
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
	soci::statement* stmt = new soci::statement(session);
	stmt->prepare(sql);
	stmt->exchange(soci::use(Key));
	stmt->exchange(soci::use(KeyUpper));
	for (size_t i = 0; i < TagTypes.size(); i++) {
		stmt->exchange(soci::use(TagTypes[i]));
	}
	for (size_t i = 0; i < SourceDirs.size(); i++) {
		stmt->exchange(soci::use(SourceDirs[i]));
	}
	return Init(stmt);
}

mvceditor::TopLevelTagInFileResultClass::TopLevelTagInFileResultClass()
	: TagResultClass()
	, FullPath() 
	, TagTypes() {
	TagTypes.push_back(mvceditor::TagClass::CLASS);
	TagTypes.push_back(mvceditor::TagClass::FUNCTION);
	TagTypes.push_back(mvceditor::TagClass::DEFINE);
}

void mvceditor::TopLevelTagInFileResultClass::Set(const wxString& fullPath) {
	FullPath = mvceditor::WxToChar(fullPath);
}

bool mvceditor::TopLevelTagInFileResultClass::Prepare(soci::session& session, bool doLimit) {
	
	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	// remove the duplicates from fully qualified namespaces
	// fully qualified classes / functions will start with backslash; but we want the
	// tags that don't begin with backslash
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON(r.file_item_id = f.file_item_id) WHERE ";
	sql += "f.full_path = ? AND Type IN(?, ?, ?) AND key NOT LIKE '\\%' ORDER BY key ";
	
	soci::statement* stmt = new soci::statement(session);
	stmt->prepare(sql);
	stmt->exchange(soci::use(FullPath));
	for (size_t i = 0; i < TagTypes.size(); i++) {
		stmt->exchange(soci::use(TagTypes[i]));
	}
	return Init(stmt);
}

mvceditor::AllTagsResultClass::AllTagsResultClass()
	: TagResultClass() {

}

bool mvceditor::AllTagsResultClass::Prepare(soci::session& session, bool doLimit) {
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON (f.file_item_id = r.file_item_id)";
	
	soci::statement* stmt = new soci::statement(session);
	stmt->prepare(sql);
	return Init(stmt);
}

mvceditor::TagByIdResultClass::TagByIdResultClass()
	: TagResultClass()
	, Id(0) {

}

void mvceditor::TagByIdResultClass::Set(int id) {
	Id = id;
}

bool mvceditor::TagByIdResultClass::Prepare(soci::session& session, bool doLimit) {
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON (f.file_item_id = r.file_item_id) ";
	sql += "WHERE r.id = ?";
	
	soci::statement* stmt = new soci::statement(session);
	stmt->prepare(sql);
	stmt->exchange(soci::use(Id));
	return Init(stmt);
}


mvceditor::FileTagResultClass::FileTagResultClass()
	: SqliteResultClass()
	, FileTag()
	, SourceDirs()
	, FilePart() 
	, LineNumber(0)
	, ExactMatch(false)
	, FileTagId(0)
	, FullPath()
	, IsNew(0) {
}

void mvceditor::FileTagResultClass::Next() {
	FileTag.FileId = FileTagId;
	FileTag.FullPath = mvceditor::CharToWx(FullPath.c_str());
	FileTag.IsNew = IsNew > 0;
	Fetch();
}

void mvceditor::FileTagResultClass::Set(const UnicodeString& filePart, int lineNumber, bool exactMatch, const std::vector<wxFileName>& sourceDirs) {
	FilePart = mvceditor::IcuToChar(filePart);
	LineNumber = lineNumber;
	
	// add trailing separator to lookup exact matches that include the last directory 
	// we dont want to get directories that start with a prefix
	for (size_t i = 0; i < sourceDirs.size(); i++) {
		SourceDirs.push_back(mvceditor::WxToChar(sourceDirs[i].GetPathWithSep()));
	}
	ExactMatch = exactMatch;
}

bool mvceditor::FileTagResultClass::Prepare(soci::session& session, bool doLimit) {

	// add the SQL wildcards
	std::string escaped = mvceditor::SqliteSqlLikeEscape(FilePart, '^');
	std::string query;
	std::string sql;
	if (ExactMatch) {
		sql += "SELECT f.full_path, file_item_id, is_new ";
		sql += "FROM file_items f LEFT JOIN sources s ON (f.source_id = s.source_id) ";
		sql += "WHERE f.full_path = ? OR f.name = ?";
	}
	else {
		query = "'%" + escaped + "%'";
		sql += "SELECT f.full_path, file_item_id, is_new ";
		sql += "FROM file_items f LEFT JOIN sources s ON (f.source_id = s.source_id) ";
		sql += "WHERE f.full_path LIKE " + query + " ESCAPE '^' ";
	}
	if (!SourceDirs.empty()) {
		sql += " AND s.directory IN(?";
		for (size_t i = 1; i < SourceDirs.size(); ++i) {
			sql += ",?"; 
		}
		sql += ")";
	}
	if (doLimit) {
		sql += "LIMIT 100";
	}
	wxString error;
	bool ret = false;
	try {
		soci::statement* stmt = new soci::statement(session);
		stmt->prepare(sql);
		if (ExactMatch) {
			stmt->exchange(soci::use(FilePart));
			stmt->exchange(soci::use(FilePart));
		}
		for (size_t i = 0; i < SourceDirs.size(); i++) {
			stmt->exchange(soci::use(SourceDirs[i]));
		}
	
		stmt->exchange(soci::into(FullPath));
		stmt->exchange(soci::into(FileTagId));
		stmt->exchange(soci::into(IsNew));
	
		ret = AdoptStatement(stmt, error);
	} catch (std::exception& e) {
		error = mvceditor::CharToWx(e.what());
		wxASSERT_MSG(false, error);
	}
	return ret;
}

std::vector<mvceditor::FileTagClass> mvceditor::FileTagResultClass::Matches() {
	std::vector<mvceditor::FileTagClass> matches;
	while (More()) {
		Next();
		if (0 == LineNumber || GetLineCountFromFile(FileTag.FullPath) >= LineNumber) {
			matches.push_back(FileTag);
		}
	}
	return matches;
}

std::vector<mvceditor::TagClass> mvceditor::FileTagResultClass::MatchesAsTags() {
	std::vector<mvceditor::TagClass> matches;
	while (More()) {
		Next();
		wxString path;
		if (0 == LineNumber || GetLineCountFromFile(FileTag.FullPath) >= LineNumber) {
			mvceditor::TagClass newTag;
			wxFileName fileName(FileTag.FullPath);
			newTag.FileTagId = FileTag.FileId;
			newTag.Identifier = mvceditor::WxToIcu(fileName.GetFullName());
			newTag.FileIsNew = FileTag.IsNew;
			newTag.SetFullPath(FileTag.FullPath);
			matches.push_back(newTag);
		}
	}
	return matches;
}

int mvceditor::FileTagResultClass::GetLineCountFromFile(const wxString& fullPath) const {
	int lineCount = 0;
	std::ifstream file;
	file.open(fullPath.fn_str(), std::ios::binary);
	if (file.good()) {
		char buffer[512];
		file.rdbuf()->pubsetbuf(buffer, sizeof(buffer));
		char ch[2];
		file.read(ch, 2);
		int read = file.gcount();
		while (read) {

			// produce accurate line counts in unix, windows, and mac created files
			// ATTN: code not unicode safe
			if ('\r' == ch[0] && '\n' == ch[1] && 2 == read) {
				++lineCount;
			}
			else if ('\r' == ch[0]) {
				++lineCount;
				file.putback(ch[1]);
			}
			else if ('\r' == ch[1]) {
				++lineCount;
			}
			else if ('\n' == ch[0]) {
				++lineCount;
				file.putback(ch[1]);
			}
			else if ('\n' == ch[1]) {
				++lineCount;
			}
			file.read(ch, 2);
			read = file.gcount();
		}
		file.close();
	}
	return lineCount;
}

mvceditor::TraitTagResultClass::TraitTagResultClass()
: SqliteResultClass()
, TraitTag()
, SourceDirs()
, Keys()
, MemberName()
, ExactMatch(false)
, Key()
, FileTagId()
, ClassName()
, NamespaceName()
, TraitClassName()
, TraitNamespaceName()
, Aliases()
, InsteadOfs() {

}

void mvceditor::TraitTagResultClass::Set(const std::vector<UnicodeString>& classNames, const UnicodeString& memberName, bool exactMatch, const std::vector<wxFileName>& sourceDirs) {
	for (size_t i = 0; i < classNames.size(); ++i) {
		Keys.push_back(mvceditor::IcuToChar(classNames[i]));
	}

	// add trailing separator to lookup exact matches that include the last directory 
	// we dont want to get directories that start with a prefix
	for (size_t i = 0; i < sourceDirs.size(); i++) {
		SourceDirs.push_back(mvceditor::WxToChar(sourceDirs[i].GetPathWithSep()));
	}
	MemberName = memberName;
	ExactMatch = exactMatch;
}

bool mvceditor::TraitTagResultClass::Prepare(soci::session& session, bool doLimit) {
	wxASSERT_MSG(!Keys.empty(), wxT("keys must not be empty"));
	std::string sql;

	sql =  "SELECT key, file_item_id, class_name, namespace_name, trait_name, trait_namespace_name, aliases, instead_ofs ";
	sql += "FROM trait_resources t LEFT JOIN sources s ON(t.source_id = s.source_id) ";
	sql += "WHERE key IN(?";
	for (size_t i = 1; i < Keys.size(); ++i) {
		sql += ",?";
	}
	sql += ") ";
	if (!SourceDirs.empty()) {
		sql += "AND s.directory IN(?";
		for (size_t i = 1; i < SourceDirs.size(); ++i) {
			sql += ",?";
		}
		sql += ") ";
	}
	sql += "ORDER BY key ";
	if (doLimit) {
		sql += "LIMIT 100";
	}

	bool good = false;
	wxString error;
	try {
		soci::statement* stmt = new soci::statement(session);
		stmt->prepare(sql);
		for (size_t i = 0; i < Keys.size(); ++i) {
			stmt->exchange(soci::use(Keys[i]));
		}
		for (size_t i = 0; i < SourceDirs.size(); ++i) {
			stmt->exchange(soci::use(SourceDirs[i]));
		}
		stmt->exchange(soci::into(Key));
		stmt->exchange(soci::into(FileTagId));
		stmt->exchange(soci::into(ClassName));
		stmt->exchange(soci::into(NamespaceName));
		stmt->exchange(soci::into(TraitClassName));
		stmt->exchange(soci::into(TraitNamespaceName));
		stmt->exchange(soci::into(Aliases));
		stmt->exchange(soci::into(InsteadOfs));

		good = AdoptStatement(stmt, error);
	} catch (std::exception& e) {
		error = mvceditor::CharToWx(e.what());
		wxASSERT_MSG(false, error);
	}
	return good;
}

void mvceditor::TraitTagResultClass::Next() {
	TraitTag.Key = mvceditor::CharToIcu(Key.c_str());
	TraitTag.FileTagId = FileTagId;
	TraitTag.ClassName = mvceditor::CharToIcu(ClassName.c_str());
	TraitTag.NamespaceName = mvceditor::CharToIcu(NamespaceName.c_str());
	TraitTag.TraitClassName = mvceditor::CharToIcu(TraitClassName.c_str());
	TraitTag.TraitNamespaceName = mvceditor::CharToIcu(TraitNamespaceName.c_str());
	
	size_t start = 0;
	size_t found = Aliases.find_first_of(",");
	while (found != std::string::npos) {
		TraitTag.Aliased.push_back(mvceditor::CharToIcu(Aliases.substr(start, found).c_str()));	
		start = found++;
	}
	if (!Aliases.empty()) {
		TraitTag.Aliased.push_back(mvceditor::CharToIcu(Aliases.substr(start, found).c_str()));
	}

	start = 0;
	found = InsteadOfs.find_first_of(",");
	while (found != std::string::npos) {
		TraitTag.InsteadOfs.push_back(mvceditor::CharToIcu(InsteadOfs.substr(start, found).c_str()));	
		start = found++;
	}
	if (!InsteadOfs.empty()) {
		TraitTag.InsteadOfs.push_back(mvceditor::CharToIcu(InsteadOfs.substr(start, found).c_str()));
	}
	Fetch();
}

std::vector<mvceditor::TagClass> mvceditor::TraitTagResultClass::MatchesAsTags() {
	std::vector<mvceditor::TagClass> tags;
	while (More()) {
		Next();

		// now go through the result and add the method names of any aliased methods
		UnicodeString lowerMethodName(MemberName);
		lowerMethodName.toLower();
		for (size_t a = 0; a < TraitTag.Aliased.size(); a++) {
			UnicodeString alias(TraitTag.Aliased[a]);
			UnicodeString lowerAlias(alias);
			lowerAlias.toLower();
			bool useAlias = false;
			if (ExactMatch) {
				useAlias = lowerMethodName.caseCompare(lowerAlias, 0) == 0;
			}
			else {
				useAlias = MemberName.isEmpty() || lowerMethodName.indexOf(lowerAlias) == 0;
			}
			if (useAlias) {
				mvceditor::TagClass res;
				res.ClassName = TraitTag.TraitClassName;
				res.Identifier = alias;
				tags.push_back(res);
			}
		}
	}
	return tags;
}

mvceditor::TagSearchClass::TagSearchClass(UnicodeString resourceQuery)
	: FileName()
	, ClassName()
	, MethodName()
	, NamespaceName()
	, SourceDirs()
	, FileItemId(0)
	, ResourceType(FILE_NAME)
	, LineNumber(0) {
	ResourceType = CLASS_NAME;

	// :: => Class::method
	// \ => \namespace\subnamespace\class
	// : => filename : line number
	int scopePos = resourceQuery.indexOf(UNICODE_STRING_SIMPLE("::"));
	int namespacePos = resourceQuery.lastIndexOf(UNICODE_STRING_SIMPLE("\\"));
	int colonPos = resourceQuery.indexOf(UNICODE_STRING_SIMPLE(":"));
	if (namespacePos >= 0 && scopePos >= 0) {
		// this query has a namespace AND a method name
		// note that the last identifier after the final backslash is the class name
		// make sure to account for the root namespace ie '\Exception'
		if (namespacePos > 0) {
			NamespaceName.setTo(resourceQuery, 0, namespacePos);
		}
		else {
			NamespaceName.setTo(resourceQuery, 0, 1);
		}

		// +1 to remove the trailing slash in the namespace name
		ClassName.setTo(resourceQuery, namespacePos + 1, scopePos - namespacePos - 1);
		MethodName.setTo(resourceQuery, scopePos + 2);
		ResourceType = CLASS_NAME_METHOD_NAME;
	}
	else if (namespacePos >= 0) {
		
		// query has a namespace, parse it out
		// note that the last identifier after the final backslash is the class name
		// make sure to account for the root namespace ie '\Exception'
		if (namespacePos > 0) {
			NamespaceName.setTo(resourceQuery, 0, namespacePos);
		}
		else {
			NamespaceName.setTo(resourceQuery, 0, 1);
		}

		// +1 to remove the trailing slash in the namespace name
		ClassName.setTo(resourceQuery, namespacePos + 1, scopePos - namespacePos - 1);
		ClassName.setTo(resourceQuery, namespacePos + 1);
		ResourceType = scopePos > 0 ? CLASS_NAME_METHOD_NAME : NAMESPACE_NAME;
	}
	else if (scopePos >= 0) {
		ClassName.setTo(resourceQuery, 0, scopePos);
		MethodName.setTo(resourceQuery, scopePos + 2);
		ResourceType = CLASS_NAME_METHOD_NAME;
	}
	else if (colonPos >= 0) {
		
		// : => filename : line number
		UnicodeString after(resourceQuery, colonPos + 1);
		Formattable fmtable((int32_t)0);
		UErrorCode error = U_ZERO_ERROR;
		NumberFormat* format = NumberFormat::createInstance(error);
		if (U_SUCCESS(error)) {
			format->parse(after, fmtable, error);
			if (U_SUCCESS(error)) {
				LineNumber = fmtable.getLong();
			}
		}
		delete format;
		FileName.setTo(resourceQuery, 0, colonPos);
		ResourceType = FILE_NAME_LINE_NUMBER;
	}
	else {

		// class names can only have alphanumerics or underscores
		UnicodeString symbols = UNICODE_STRING_SIMPLE("`!@#$%^&*()+={}|\\:;\"',./?");
		
		// not using getTerminatedBuffer() because that method triggers valgrind warnings
		UChar* queryBuf = new UChar[resourceQuery.length() + 1];
		u_memmove(queryBuf, resourceQuery.getBuffer(), resourceQuery.length());
		queryBuf[resourceQuery.length()] = '\0';
	
		UChar* symbolBuf = new UChar[symbols.length() + 1];
		u_memmove(symbolBuf, symbols.getBuffer(), symbols.length());
		symbolBuf[symbols.length()] = '\0';
		bool hasSymbols = NULL != u_strpbrk(queryBuf, symbolBuf);
		if (hasSymbols) {
			FileName = resourceQuery;
			ResourceType = FILE_NAME;
		}
		else {
			ClassName = resourceQuery;
			ResourceType = CLASS_NAME;
		}
		delete[] queryBuf;
		delete[] symbolBuf;
	}
}

void mvceditor::TagSearchClass::SetParentClasses(const std::vector<UnicodeString>& parents) {
	ParentClasses = parents;
}

std::vector<UnicodeString> mvceditor::TagSearchClass::GetParentClasses() const {
	return ParentClasses;
}

void mvceditor::TagSearchClass::SetTraits(const std::vector<UnicodeString>& traits) {
	Traits = traits;
}

std::vector<UnicodeString> mvceditor::TagSearchClass::GetTraits() const {
	return Traits;
}

void mvceditor::TagSearchClass::SetFileItemId(int id) {
	FileItemId = id;
}

int mvceditor::TagSearchClass::GetFileItemId() const {
	return FileItemId;
}

std::vector<UnicodeString> mvceditor::TagSearchClass::GetClassHierarchy() const {
	std::vector<UnicodeString> allClassNames;
	if (!ClassName.isEmpty()) { 
		allClassNames.push_back(ClassName);
	}
	allClassNames.insert(allClassNames.end(), ParentClasses.begin(), ParentClasses.end());
	allClassNames.insert(allClassNames.end(), Traits.begin(), Traits.end());
	return allClassNames;

}

void mvceditor::TagSearchClass::SetSourceDirs(const std::vector<wxFileName>& sourceDirs) {
	SourceDirs = sourceDirs;
}

std::vector<wxFileName> mvceditor::TagSearchClass::GetSourceDirs() const {
	return SourceDirs;
}

UnicodeString mvceditor::TagSearchClass::GetClassName() const {
	return ClassName;
}

UnicodeString mvceditor::TagSearchClass::GetFileName() const {
	return FileName;
}

UnicodeString mvceditor::TagSearchClass::GetMethodName() const {
	return MethodName;
}

int mvceditor::TagSearchClass::GetLineNumber() const {
	return LineNumber;
}

mvceditor::TagSearchClass::ResourceTypes mvceditor::TagSearchClass::GetResourceType() const {
	return ResourceType;
}

UnicodeString mvceditor::TagSearchClass::GetNamespaceName() const {
	return NamespaceName;
}

mvceditor::TagResultClass* mvceditor::TagSearchClass::CreateExactResults() const {
	mvceditor::TagResultClass* results = NULL; 
	if (GetResourceType() == mvceditor::TagSearchClass::CLASS_NAME_METHOD_NAME) {
		mvceditor::ExactMemberTagResultClass* memberResults = new mvceditor::ExactMemberTagResultClass();

		// check the entire class hierachy
		// combine the parent classes with the class being searched
		std::vector<UnicodeString> classHierarchy = GetParentClasses();
		classHierarchy.insert(classHierarchy.end(), Traits.begin(), Traits.end());
		classHierarchy.push_back(QualifyName(GetNamespaceName(), GetClassName()));
		
		memberResults->Set(classHierarchy, GetMethodName(), GetSourceDirs());
		results = memberResults;
	}
	else if (mvceditor::TagSearchClass::NAMESPACE_NAME == GetResourceType()) {
		mvceditor::ExactNonMemberTagResultClass* nonMemberResults = new mvceditor::ExactNonMemberTagResultClass();

		std::vector<UnicodeString> classNames;
		UnicodeString key = QualifyName(GetNamespaceName(), GetClassName());
		nonMemberResults->Set(key, GetSourceDirs());
		results = nonMemberResults;
	}
	else if (!ClassName.isEmpty()) {
		mvceditor::ExactNonMemberTagResultClass* nonMemberResults = new mvceditor::ExactNonMemberTagResultClass();
		nonMemberResults->Set(GetClassName(), GetSourceDirs());
		results = nonMemberResults;
	}
	else {
		mvceditor::ExactNonMemberTagResultClass* nonMemberResults = new mvceditor::ExactNonMemberTagResultClass();
		nonMemberResults->Set(GetFileName(), GetSourceDirs());
		results = nonMemberResults;
	}
	return results;
}

mvceditor::TagResultClass* mvceditor::TagSearchClass::CreateNearMatchResults() const {
	mvceditor::TagResultClass* results = NULL; 
	if (mvceditor::TagSearchClass::CLASS_NAME_METHOD_NAME == GetResourceType() && !GetClassName().isEmpty()) {
		mvceditor::NearMatchMemberTagResultClass* nearMatchMembersResult = new mvceditor::NearMatchMemberTagResultClass();

		// check the entire class hierachy
		// combine the parent classes with the class being searched
		std::vector<UnicodeString> classHierarchy = GetParentClasses();
		if (GetNamespaceName().isEmpty()) {
			classHierarchy.push_back(GetClassName());
		}
		else {
			classHierarchy.push_back(QualifyName(GetNamespaceName(), GetClassName()));
		}
		classHierarchy.insert(classHierarchy.end(), Traits.begin(), Traits.end());

		nearMatchMembersResult->Set(classHierarchy, GetMethodName(), GetFileItemId(), GetSourceDirs());
		results = nearMatchMembersResult;
	}
	else if ((mvceditor::TagSearchClass::CLASS_NAME_METHOD_NAME == GetResourceType())
			&& GetClassName().isEmpty()) {
		mvceditor::NearMatchMemberOnlyTagResultClass* nearMatchMemberOnlyResult = new mvceditor::NearMatchMemberOnlyTagResultClass();
		nearMatchMemberOnlyResult->Set(GetMethodName(), GetSourceDirs());
		results = nearMatchMemberOnlyResult;
	}
	else if (mvceditor::TagSearchClass::NAMESPACE_NAME == GetResourceType()) {

		// needle identifier contains a namespace operator; but it may be
		// a namespace or a fully qualified name
		UnicodeString namespaceKey;
		namespaceKey = QualifyName(GetNamespaceName(), GetClassName());
		mvceditor::NearMatchNonMemberTagResultClass* nearMatchNonMemberNamespaced = new mvceditor::NearMatchNonMemberTagResultClass();
		nearMatchNonMemberNamespaced->Set(namespaceKey, GetSourceDirs());
		nearMatchNonMemberNamespaced->AddTagType(mvceditor::TagClass::NAMESPACE);
		results = nearMatchNonMemberNamespaced;
	} 
	else if (!ClassName.isEmpty()) {
		UnicodeString key;
		// if query does not have a namespace then get the non-namespaced tags
		if (GetNamespaceName().isEmpty()) {
			key = GetClassName();
		}
		else {
			key = QualifyName(GetNamespaceName(), GetClassName());
		}
		mvceditor::NearMatchNonMemberTagResultClass* nearMatchNonMembers = new mvceditor::NearMatchNonMemberTagResultClass();
		nearMatchNonMembers->Set(key, GetSourceDirs());
		results = nearMatchNonMembers;
	}
	else {
		UnicodeString key = GetFileName();
		mvceditor::NearMatchNonMemberTagResultClass* nearMatchNonMembers = new mvceditor::NearMatchNonMemberTagResultClass();
		nearMatchNonMembers->Set(key, GetSourceDirs());
		results = nearMatchNonMembers;
	}
	return results;
}

mvceditor::FileTagResultClass* mvceditor::TagSearchClass::CreateExactFileResults() const {
	mvceditor::FileTagResultClass* result = new mvceditor::FileTagResultClass();
	if (mvceditor::TagSearchClass::FILE_NAME_LINE_NUMBER == GetResourceType()) {
		UnicodeString query = GetFileName();
		result->Set(query, GetLineNumber(), true, GetSourceDirs());
	}
	else if (mvceditor::TagSearchClass::FILE_NAME == GetResourceType()) {
		UnicodeString query = GetFileName();

		// empty file name = no period, get the query from the class name variable
		if (query.isEmpty()) {
			query = GetClassName();
		}
		result->Set(query, 0, true, GetSourceDirs());
	}
	else if (!GetNamespaceName().isEmpty()) {

		// backslash (dir separator) ==  PHP namespace separator
		// if query contains a backslash then the type will be namespace name
		UnicodeString query = GetNamespaceName() + UNICODE_STRING_SIMPLE("\\") + GetClassName();
		result->Set(query, 0, true, GetSourceDirs());
	}
	else {
		UnicodeString query = GetClassName();
		result->Set(query, 0, false, GetSourceDirs());
	}
	return result;
}

mvceditor::FileTagResultClass* mvceditor::TagSearchClass::CreateNearMatchFileResults() const {
	mvceditor::FileTagResultClass* result = new mvceditor::FileTagResultClass();
	if (mvceditor::TagSearchClass::FILE_NAME_LINE_NUMBER == GetResourceType()) {
		UnicodeString query = GetFileName();
		result->Set(query, GetLineNumber(), false, GetSourceDirs());
	}
	else if (mvceditor::TagSearchClass::FILE_NAME == GetResourceType()) {
		UnicodeString query = GetFileName();

		// empty file name = no period, get the query from the class name variable
		if (query.isEmpty()) {
			query = GetClassName();
		}
		result->Set(query, 0, false, GetSourceDirs());
	}
	else if (!GetNamespaceName().isEmpty()) {

		// backslash (dir separator) ==  PHP namespace separator
		// if query contains a backslash then the type will be namespace name
		UnicodeString query = GetNamespaceName() + UNICODE_STRING_SIMPLE("\\") + GetClassName();
		result->Set(query, 0, false, GetSourceDirs());
	}
	else {
		UnicodeString query = GetClassName();
		result->Set(query, 0, false, GetSourceDirs());
	}
	return result;
}

mvceditor::TagResultClass::TagResultClass() 
	: SqliteResultClass()
	, Tag()
	, Id(0)
	, FileTagId(0)
	, SourceId(0)
	, Key()
	, Identifier()
	, ClassName()
	, Type(0)
	, NamespaceName()
	, Signature()
	, ReturnType()
	, Comment()
	, FullPath()
	, IsProtected(false)
	, IsPrivate(false)
	, IsStatic(false)
	, IsDynamic(false)
	, IsNative(false)
	, FileIsNew(false)
	, FileTagIdIndicator()
	, FullPathIndicator()
	, FileIsNewIndicator()
{
}

bool mvceditor::TagResultClass::Init(soci::statement* stmt) {
	wxString error;
	bool ret = false;
	try {
		stmt->exchange(soci::into(Id));
		stmt->exchange(soci::into(FileTagId, FileTagIdIndicator));
		stmt->exchange(soci::into(SourceId));
		stmt->exchange(soci::into(Key));
		stmt->exchange(soci::into(Identifier));
		stmt->exchange(soci::into(ClassName));
		stmt->exchange(soci::into(Type));
		stmt->exchange(soci::into(NamespaceName));
		stmt->exchange(soci::into(Signature));
		stmt->exchange(soci::into(ReturnType));
		stmt->exchange(soci::into(Comment));
		stmt->exchange(soci::into(FullPath, FullPathIndicator));
		stmt->exchange(soci::into(IsProtected));
		stmt->exchange(soci::into(IsPrivate));
		stmt->exchange(soci::into(IsStatic));
		stmt->exchange(soci::into(IsDynamic));
		stmt->exchange(soci::into(IsNative));
		stmt->exchange(soci::into(FileIsNew, FileIsNewIndicator));
		
		ret = AdoptStatement(stmt, error);
	} catch (std::exception& e) {
		error = mvceditor::CharToWx(e.what());
		wxASSERT_MSG(false, error);
	}
	return ret;
}

void mvceditor::TagResultClass::Next() {
	Tag.Id = Id;
	if (soci::i_ok == FileTagIdIndicator) {
		Tag.FileTagId = FileTagId;
	}
	Tag.SourceId = SourceId;
	Tag.Key = mvceditor::CharToIcu(Key.c_str());
	Tag.Identifier = mvceditor::CharToIcu(Identifier.c_str());
	Tag.ClassName = mvceditor::CharToIcu(ClassName.c_str());
	Tag.Type = (mvceditor::TagClass::Types)Type;
	Tag.NamespaceName = mvceditor::CharToIcu(NamespaceName.c_str());
	Tag.Signature = mvceditor::CharToIcu(Signature.c_str());
	Tag.ReturnType = mvceditor::CharToIcu(ReturnType.c_str());
	Tag.Comment = mvceditor::CharToIcu(Comment.c_str());
	if (soci::i_ok == FullPathIndicator) {
		Tag.SetFullPath(mvceditor::CharToWx(FullPath.c_str()));
	}
	Tag.IsProtected = IsProtected != 0;
	Tag.IsPrivate = IsPrivate != 0;
	Tag.IsStatic = IsStatic != 0;
	Tag.IsDynamic = IsDynamic != 0;
	Tag.IsNative = IsNative != 0;
	if (soci::i_ok == FileIsNewIndicator) {
		Tag.FileIsNew = FileIsNew != 0;
	}
	else {
		Tag.FileIsNew = true;
	}
	Fetch();
}

std::vector<mvceditor::TagClass> mvceditor::TagResultClass::Matches() {
	std::vector<mvceditor::TagClass> matches;
	while (More()) {
		Next();
		matches.push_back(Tag);
	}
	return matches;
}

bool mvceditor::ParsedTagFinderClass::GetResourceMatchPosition(const mvceditor::TagClass& tag, const UnicodeString& text, int32_t& pos, 
		int32_t& length) {
	size_t start = 0;
	mvceditor::FinderClass finder;
	finder.Mode = FinderClass::REGULAR_EXPRESSION;

	UnicodeString className,
		methodName;
	if (!tag.ClassName.isEmpty()) {
		className = tag.ClassName;
		methodName = tag.Identifier;

		mvceditor::FinderClass::EscapeRegEx(className);
		mvceditor::FinderClass::EscapeRegEx(methodName);
	}
	else {
		className = tag.Identifier;
		mvceditor::FinderClass::EscapeRegEx(className);
	}
	switch (tag.Type) {
		case TagClass::CLASS:
			finder.Expression = UNICODE_STRING_SIMPLE("\\sclass\\s+") + className + UNICODE_STRING_SIMPLE("\\s");
			break;
		case TagClass::METHOD:
			//advance past the class header so that if  a function with the same name exists we will skip it
			finder.Expression = UNICODE_STRING_SIMPLE("\\sclass\\s+") + className + UNICODE_STRING_SIMPLE("\\s");
			if (finder.Prepare() && finder.FindNext(text, start)) {			
				finder.GetLastMatch(pos, length);
			}
			start = pos + length;

			// method may return a reference (&)
			finder.Expression = UNICODE_STRING_SIMPLE("\\sfunction\\s*(&\\s*)?") + methodName + UNICODE_STRING_SIMPLE("\\s*\\(");
			break;
		case TagClass::FUNCTION:

			// function may return a reference (&)
			finder.Expression = UNICODE_STRING_SIMPLE("\\sfunction\\s*(&\\s*)?") + className + UNICODE_STRING_SIMPLE("\\s*\\(");
			break;
		case TagClass::MEMBER:
			//advance past the class header so that if  a variable with the same name exists we will skip it				:
			finder.Expression = UNICODE_STRING_SIMPLE("\\sclass\\s+") + className + UNICODE_STRING_SIMPLE("\\s");
			if (finder.Prepare() && finder.FindNext(text, start)) {			
				finder.GetLastMatch(pos, length);
			}
			start = pos + length;
			finder.Expression = UNICODE_STRING_SIMPLE("\\s((var)|(public)|(protected)|(private)).+") + methodName + UNICODE_STRING_SIMPLE(".*;");
			break;
		case TagClass::DEFINE:
			finder.Expression = UNICODE_STRING_SIMPLE("\\sdefine\\(\\s*('|\")") + className + UNICODE_STRING_SIMPLE("('|\")");
			break;
		case TagClass::CLASS_CONSTANT:
			//advance past the class header so that if  a variable with the same name exists we will skip it				:
			finder.Expression = UNICODE_STRING_SIMPLE("\\sclass\\s+") + className + UNICODE_STRING_SIMPLE("\\s");
			if (finder.Prepare() && finder.FindNext(text, start)) {			
				finder.GetLastMatch(pos, length);
			}
			start = pos + length;
			finder.Expression = UNICODE_STRING_SIMPLE("\\sconst\\s+") + methodName + UNICODE_STRING_SIMPLE("\\s*=");
		default:
			break;
	}
	if (finder.Prepare() && finder.FindNext(text, start) && finder.GetLastMatch(pos, length)) {
		++pos; //eat the first space
		--length;
		return true;
	}
	return false;
}

std::vector<mvceditor::TagClass> mvceditor::ParsedTagFinderClass::NearMatchClassesOrFiles(
	const mvceditor::TagSearchClass& tagSearch) {
	std::vector<mvceditor::TagClass> matches;

	// at one point there was a check here to see if the  tag files existed
	// it was removed because it caused performance issues, since this method
	// is called while the user is typing text.
	// take care when coding; make sure that any code called by this method does not touch the file system
	mvceditor::FileTagResultClass fileTagResult;
	fileTagResult.Set(tagSearch.GetFileName(), tagSearch.GetLineNumber(), false, tagSearch.GetSourceDirs());
	mvceditor::NearMatchNonMemberTagResultClass nonMembers;
	switch (tagSearch.GetResourceType()) {
		case mvceditor::TagSearchClass::FILE_NAME:
		case mvceditor::TagSearchClass::FILE_NAME_LINE_NUMBER:
			fileTagResult.Prepare(*Session, true);
			matches = fileTagResult.MatchesAsTags();
			break;
		case mvceditor::TagSearchClass::NAMESPACE_NAME:
			nonMembers.Set(QualifyName(tagSearch.GetNamespaceName(), tagSearch.GetClassName()), tagSearch.GetSourceDirs());
			nonMembers.SetTagType(mvceditor::TagClass::CLASS);
			nonMembers.Prepare(*Session, true);
			matches = nonMembers.Matches();
			if (matches.empty()) {
				fileTagResult.Prepare(*Session, true);
				matches = fileTagResult.MatchesAsTags();
			}
			break;
		case mvceditor::TagSearchClass::CLASS_NAME:
		case mvceditor::TagSearchClass::CLASS_NAME_METHOD_NAME:
			nonMembers.Set(tagSearch.GetClassName(), tagSearch.GetSourceDirs());
			nonMembers.SetTagType(mvceditor::TagClass::CLASS);
			nonMembers.Prepare(*Session, true);
			matches = nonMembers.Matches();
			if (matches.empty()) {
				fileTagResult.Prepare(*Session, true);
				matches = fileTagResult.MatchesAsTags();
			}
			break;
	}
	sort(matches.begin(), matches.end());
	return matches;
}

std::vector<mvceditor::TagClass> mvceditor::ParsedTagFinderClass::NearMatchMembers(const mvceditor::TagSearchClass& tagSearch) {
	std::vector<mvceditor::TagClass> matches;
	
	// when looking for members we need to look
	// 1. in the class itself
	// 2. in any parent classes
	// 3. in any used traits
	std::vector<UnicodeString> classesToSearch = tagSearch.GetParentClasses();

	// make sure to fully qualified class name
	UnicodeString qualifiedClassName;
	if (!tagSearch.GetNamespaceName().isEmpty()) {
		qualifiedClassName = QualifyName(tagSearch.GetNamespaceName(), tagSearch.GetClassName());
	}
	else {
		qualifiedClassName = tagSearch.GetClassName();
	}
	classesToSearch.push_back(qualifiedClassName);
	std::vector<UnicodeString> traits = tagSearch.GetTraits();
	classesToSearch.insert(classesToSearch.end(), traits.begin(), traits.end());
	if (tagSearch.GetMethodName().isEmpty()) {
		
		// special case; query for all methods for a class (UserClass::)
		mvceditor::AllMembersTagResultClass allMembersResult;
		allMembersResult.Set(classesToSearch, UNICODE_STRING_SIMPLE(""), tagSearch.GetSourceDirs());
		
		// do not limit, a class may have more than 100 members
		allMembersResult.Prepare(*Session, false);
		std::vector<mvceditor::TagClass> memberMatches = allMembersResult.Matches();

		matches.insert(matches.end(), memberMatches.begin(), memberMatches.end());
	}
	else if (tagSearch.GetClassName().isEmpty()) {
		
		// special case, query across all classes for a method (::getName)
		// if ClassName is empty, then just check method names This ensures 
		// queries like '::getName' will work as well.
		// make sure to NOT get fully qualified  matches (key=identifier)
		mvceditor::ExactMemberOnlyTagResultClass exactResult;
		exactResult.Set(tagSearch.GetMethodName(), tagSearch.GetSourceDirs());
		exactResult.Prepare(*Session, true);
		matches = exactResult.Matches();
		if (matches.empty()) {
			mvceditor::NearMatchMemberOnlyTagResultClass nearMatchResult;
			nearMatchResult.Set(tagSearch.GetMethodName(), tagSearch.GetSourceDirs());
			nearMatchResult.Prepare(*Session, true);

			// use LIKE to get near matches
			matches = nearMatchResult.Matches();
		}
	}
	else {
		std::vector<std::string> keyStarts;

		// now that we found the parent classes, combine the parent class name and the queried method
		// to make all of the keys we need to look for. remember that a tag class key is of the form
		// ClassName::MethodName
		mvceditor::NearMatchMemberTagResultClass nearMatchMemberResult;
		nearMatchMemberResult.Set(classesToSearch, tagSearch.GetMethodName(), tagSearch.GetFileItemId(), tagSearch.GetSourceDirs());
		nearMatchMemberResult.Prepare(*Session, true);
		matches = nearMatchMemberResult.Matches();			
	}
	return matches;
}

UnicodeString mvceditor::ParsedTagFinderClass::ParentClassName(const UnicodeString& fullyQualifiedClassName, int fileTagId) {
	UnicodeString parentClassName;
	
	// empty file items == search on all files
	std::vector<wxFileName> sourceDirs;
	mvceditor::ExactNonMemberTagResultClass exactResult;
	exactResult.Set(fullyQualifiedClassName, sourceDirs);
	exactResult.SetFileTagId(fileTagId);
	exactResult.SetTagType(mvceditor::TagClass::CLASS);
	exactResult.Prepare(*Session, true);
	std::vector<mvceditor::TagClass> matches = exactResult.Matches();
	if (!matches.empty()) {
		mvceditor::TagClass tag = matches[0];
		parentClassName = ExtractParentClassFromSignature(tag.Signature);
	}
	return parentClassName;
}

std::vector<UnicodeString> mvceditor::ParsedTagFinderClass::GetResourceTraits(const UnicodeString& className, 
																		const UnicodeString& methodName,
																		const std::vector<wxFileName>& sourceDirs) {
	std::vector<UnicodeString> inheritedTraits;
	
	std::vector<UnicodeString> classNames;
	classNames.push_back(className);

	mvceditor::TraitTagResultClass traitResult;
	traitResult.Set(classNames, UNICODE_STRING_SIMPLE(""), false, sourceDirs);
	if (traitResult.Prepare(*Session, false)) {
		while (traitResult.More()) {
			traitResult.Next();
			mvceditor::TraitTagClass trait = traitResult.TraitTag;
			UnicodeString fullyQualifiedTrait = QualifyName(trait.TraitNamespaceName, trait.TraitClassName);
			
			// trait is used unless there is an explicit insteadof 
			bool match = true;
			for (size_t j = 0; j < trait.InsteadOfs.size(); ++j) {
				if (trait.InsteadOfs[j].caseCompare(fullyQualifiedTrait, 0) == 0) {
					match = false;
					break;
				}
			}
			if (match) {
				inheritedTraits.push_back(trait.TraitClassName);
			}
		}
	}
	
	// we will get duplicate traits, since we looped over get the trait methods
	// not just the traits
	// lets remove all dups
	std::sort(inheritedTraits.begin(), inheritedTraits.end());
	std::vector<UnicodeString>::iterator end = std::unique(inheritedTraits.begin(), inheritedTraits.end());
	inheritedTraits.erase(end, inheritedTraits.end());
	return inheritedTraits;
}

UnicodeString mvceditor::ParsedTagFinderClass::ExtractParentClassFromSignature(const UnicodeString& signature) const {

	// look for the parent class. tokenize the signature and get the
	// class name after the 'extends' keyword note that since the signature is re-constructed by the parser
	// the parent class is always fully qualified
	UnicodeString parentClassName;
	UChar* saveState = NULL;
	const UChar* delims =  UNICODE_STRING_SIMPLE(" ").getTerminatedBuffer();
	UChar* sig = new UChar[signature.length() + 1];
	u_strncpy(sig, signature.getBuffer(), signature.length());
	sig[signature.length()] = '\0';
	UChar* next = u_strtok_r(sig, delims, &saveState);
	if (next) {
		do {
			UnicodeString token(next);
			if (token.caseCompare(UNICODE_STRING_SIMPLE("extends"), 0) == 0) {
				next = u_strtok_r(NULL, delims, &saveState);
				if (next) {
					parentClassName.setTo(next, u_strlen(next));
				}
			}
			next = u_strtok_r(NULL, delims, &saveState);
		} while (next != NULL);
	}
	delete[] sig;
	return parentClassName;
}

std::vector<mvceditor::TagClass> mvceditor::ParsedTagFinderClass::ExactClassOrFile(const mvceditor::TagSearchClass& tagSearch) {

	// at one point there was a check here to see if the  tag files existed
	// it was removed because it caused performance issues, since this method
	// is called while the user is typing text.
	// take care when coding; make sure that any code called by this method does not touch the file system
	std::vector<mvceditor::TagClass> allMatches;
	std::vector<int> types;
	bool error = false;
	wxString errorMsg;
	std::vector<int> fileTagIds = mvceditor::FileTagIdsForDirs(*Session, tagSearch.GetSourceDirs(), error, errorMsg);
	wxASSERT_MSG(!error, errorMsg);
	if (mvceditor::TagSearchClass::FILE_NAME == tagSearch.GetResourceType() || 
		mvceditor::TagSearchClass::FILE_NAME_LINE_NUMBER == tagSearch.GetResourceType()) {
		mvceditor::FileTagResultClass fileTagResult;
		fileTagResult.Set(tagSearch.GetFileName(), tagSearch.GetLineNumber(), true, tagSearch.GetSourceDirs());
		fileTagResult.Prepare(*Session, true);
		allMatches = fileTagResult.MatchesAsTags();
	}
	else {
		mvceditor::ExactNonMemberTagResultClass exactResult;
		exactResult.SetTagType(mvceditor::TagClass::CLASS);
		exactResult.Set(tagSearch.GetClassName(), tagSearch.GetSourceDirs());
		exactResult.Prepare(*Session, true);
		allMatches = exactResult.Matches();
	}
	return allMatches;
}

void mvceditor::ParsedTagFinderClass::EnsureMatchesExist(std::vector<TagClass>& matches) {

	// remove from matches that have a file that is no longer in the file system
	std::vector<mvceditor::TagClass>::iterator it = matches.begin();
	std::vector<int> fileTagIdsToRemove;
	while (it != matches.end()) {

		// native matches wont have a FileTag assigned to them since they come from the tag file
		// FileTagId is meaningless for dynamic resources
		// is a file is new it wont be on disk; results are never stale in this case.
		bool remove = false;
		if (!it->IsNative && !it->IsDynamic && !it->FileIsNew) {
			wxFileName fileName = it->FileName();
			if (fileName.IsOk() && !fileName.FileExists()) {
				remove = true;
			}
		}
		if (remove) {
			fileTagIdsToRemove.push_back(it->FileTagId);
			it = matches.erase(it);
		}
		else {
			++it;
		}
	}
}

bool mvceditor::ParsedTagFinderClass::IsFileCacheEmpty() {
	if (!IsInit()) {
		return true;
	}
	int count = 0;
	try {
		Session->once << "SELECT COUNT(*) FROM file_items;", soci::into(count);
	} catch (std::exception& e) {
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return count <= 0;
}

bool mvceditor::ParsedTagFinderClass::IsResourceCacheEmpty() {
	if (!IsInit()) {
		return true;
	}

	// make sure only parsed tag came from the native functions file.
	int count = 0;
	try {
		Session->once << "SELECT COUNT(*) FROM resources WHERE is_native = 0;", soci::into(count);
	} catch (std::exception& e) {
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return count <= 0;
}

bool mvceditor::ParsedTagFinderClass::HasFullPath(const wxString& fullPath) {
	mvceditor::FileTagClass fileTag;
	return FindFileTagByFullPathExact(fullPath, fileTag);
}


bool mvceditor::ParsedTagFinderClass::HasDir(const wxString& dir) {
	bool foundDir = false;
	if (!IsInit()) {
		return foundDir;
	}

	// at this point we dont store directories in the db, we look for a file
	// that contains the dir in question
	wxFileName dirName;
	dirName.AssignDir(dir);
	std::string result;
	std::string query = mvceditor::WxToChar(dirName.GetPathWithSep() + wxT("%"));
	std::string sql = "SELECT full_path FROM file_items WHERE full_path LIKE ? LIMIT 1";
	try {
		soci::statement stmt = (Session->prepare << sql, soci::use(query), 
			soci::into(result)
		);
		if (stmt.execute(true) && !result.empty()) {
			foundDir = true;
		}
	} catch (std::exception& e) {
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return foundDir;
}

bool mvceditor::ParsedTagFinderClass::FindFileTagByFullPathExact(const wxString& fullPath, mvceditor::FileTagClass& fileTag) {
	if (!IsInit()) {
		return false;
	}
	int fileTagId;
	std::tm lastModified;
	int isParsed;
	int isNew;
	bool foundFile = false;

	std::string query = mvceditor::WxToChar(fullPath);
	std::string sql = "SELECT file_item_id, last_modified, is_parsed, is_new FROM file_items WHERE full_path = ?";
	try {
		soci::statement stmt = (Session->prepare << sql, soci::use(query), 
			soci::into(fileTagId), soci::into(lastModified), soci::into(isParsed), soci::into(isNew)
		);
		foundFile = stmt.execute(true);
		if (foundFile) {
			fileTag.DateTime.Set(lastModified);
			fileTag.FileId = fileTagId;
			fileTag.FullPath = fullPath;
			fileTag.IsNew = isNew != 0;
			fileTag.IsParsed = isParsed != 0;
		}
	} catch (std::exception& e) {
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return foundFile;
}

std::vector<mvceditor::TagClass> mvceditor::ParsedTagFinderClass::All() {
	mvceditor::AllTagsResultClass result;
	result.Prepare(*Session, false);

	// remove the 'duplicates' ie. extra fully qualified entries to make lookups faster
	std::vector<mvceditor::TagClass> all;
	while (result.More()) {
		result.Next();
		if (result.Tag.Key.indexOf(UNICODE_STRING_SIMPLE("::")) > 0) {

			// fully qualified methods
		}
		else if (result.Tag.Type != mvceditor::TagClass::NAMESPACE && result.Tag.Key.indexOf(UNICODE_STRING_SIMPLE("\\")) >= 0) {
			
			// fully qualified classes / functions (with namespace)
		}
		else {
			all.push_back(result.Tag);
		}
	}
	return all;
}

mvceditor::ParsedTagFinderClass::ParsedTagFinderClass()
	: SqliteFinderClass() {

}

std::vector<mvceditor::TagClass> mvceditor::ParsedTagFinderClass::ClassesFunctionsDefines(const wxString& fullPath) {
	std::vector<mvceditor::TagClass> tags;
	mvceditor::TopLevelTagInFileResultClass result;
	result.Set(fullPath);
	result.Prepare(*Session, false);
	tags = result.Matches();
	return tags;
}

void mvceditor::ParsedTagFinderClass::Print() {
	std::vector<mvceditor::TagClass> tags = All();
	std::vector<mvceditor::TagClass>::iterator tag;
	for (tag = tags.begin(); tag != tags.end(); ++tag) {
		printf("key=%s\n", mvceditor::IcuToChar(tag->Key).c_str());
		printf("type=%d fileItemId=%d\n", tag->Type, tag->FileTagId);
		printf("identifier=%s\n", mvceditor::IcuToChar(tag->Identifier).c_str());
		printf("\n\n");
	}
}

bool mvceditor::ParsedTagFinderClass::FindById(int id, mvceditor::TagClass& tag) {
	mvceditor::TagByIdResultClass result;
	result.Set(id);
	bool found = Exec(&result);
	if (found) {
		result.Next();
		tag = result.Tag;
	}
	return found;
}

wxString mvceditor::ParsedTagFinderClass::SourceDirFromFile(int fileTagId) {
	wxString sourcePath;
	std::string stdSourcePath;
	if (Session) {
		soci::statement stmt = (Session->prepare <<
			"SELECT directory FROM sources s JOIN file_items f ON(s.source_id = f.source_id) WHERE f.file_item_id = ?",
			soci::use(fileTagId), soci::into(stdSourcePath)
		);
		if (stmt.execute(true)) {
			sourcePath = mvceditor::CharToWx(stdSourcePath.c_str());
		}
	}
	return sourcePath;
}