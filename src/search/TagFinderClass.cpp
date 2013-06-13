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
#include <search/TagFinderClass.h>
#include <search/FinderClass.h>
#include <globals/String.h>
#include <globals/Sqlite.h>
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

	virtual void Prepare(soci::session& session, bool doLimit);

private:

	int ClassCount;
};

class ExactNonMemberTagResultClass : public mvceditor::TagResultClass {

public:

	ExactNonMemberTagResultClass();

	void Prepare(soci::session& session, bool doLimit);
	
	virtual void Set(const UnicodeString& key, const std::vector<wxFileName>& sourceDirs);

	void SetTagType(mvceditor::TagClass::Types type);

protected:

	std::string Key;

	std::vector<int> TagTypes;

	std::vector<std::string> SourceDirs;
};

class NearMatchNonMemberTagResultClass : public mvceditor::ExactNonMemberTagResultClass {

public:

	NearMatchNonMemberTagResultClass();

	void Set(const UnicodeString& key, const std::vector<wxFileName>& sourceDirs);

	void AddTagType(mvceditor::TagClass::Types type);

	void Prepare(soci::session& session, bool doLimit);

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

	void Prepare(soci::session& session, bool doLimit);
	
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

	void Prepare(soci::session& session, bool doLimit);

private:

	std::string KeyUpper;
};

class TopLevelTagInFileResultClass : public TagResultClass {

public:
	TopLevelTagInFileResultClass();

	void Set(const wxString& fullPath);

	void Prepare(soci::session& session);

private:

	std::string FullPath;

	std::vector<int> TagTypes;
};

class AllTagsResultClass : public TagResultClass {

public:

	AllTagsResultClass();

	void Prepare(soci::session& session);
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

static void InClause(const std::vector<int>& values, std::ostringstream& stream) {
	size_t size = values.size();
	for (size_t i = 0; i < size; ++i) {
		stream << values[i];
		if (i < (size - 1)) {
			stream << ",";
		}
	}
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

void mvceditor::ExactMemberTagResultClass::Prepare(soci::session& session,  bool doLimit) {

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
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
	Init(session, stmt);
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

void mvceditor::AllMembersTagResultClass::Prepare(soci::session& session,  bool doLimit) {
	
	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
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
	Init(session, stmt);
}


mvceditor::NearMatchMemberTagResultClass::NearMatchMemberTagResultClass()
	: ExactMemberTagResultClass() 
	, ClassCount(0) {
}

void mvceditor::NearMatchMemberTagResultClass::Set(const std::vector<UnicodeString>& classNames, const UnicodeString& memberName, 
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
}


void mvceditor::NearMatchMemberTagResultClass::Prepare(soci::session& session,  bool doLimit) {
	wxASSERT_MSG(!Keys.empty(), wxT("keys cannot be empty"));

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
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
	Init(session, stmt);
}

mvceditor::ExactNonMemberTagResultClass::ExactNonMemberTagResultClass()
	: TagResultClass()
	, Key() 
	, TagTypes()
	, SourceDirs() {
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

void mvceditor::ExactNonMemberTagResultClass::Prepare(soci::session& session, bool doLimit) {

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
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
	Init(session, stmt);
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

void mvceditor::NearMatchNonMemberTagResultClass::Prepare(soci::session& session, bool doLimit) {

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
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
	Init(session, stmt);
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

void mvceditor::ExactMemberOnlyTagResultClass::Prepare(soci::session& session,  bool doLimit) {

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
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
	Init(session, stmt);
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

void mvceditor::NearMatchMemberOnlyTagResultClass::Prepare(soci::session& session, bool doLimit) {

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
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
	Init(session, stmt);
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

void mvceditor::TopLevelTagInFileResultClass::Prepare(soci::session& session) {
	
	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	// remove the duplicates from fully qualified namespaces
	// fully qualified classes / functions will start with backslash; but we want the
	// tags that don't begin with backslash
	std::string sql;
	sql += "SELECT r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON(r.file_item_id = f.file_item_id) WHERE ";
	sql += "f.full_path = ? AND Type IN(?, ?, ?) AND key NOT LIKE '\\%' ORDER BY key ";
	
	soci::statement* stmt = new soci::statement(session);
	stmt->prepare(sql);
	stmt->exchange(soci::use(FullPath));
	for (size_t i = 0; i < TagTypes.size(); i++) {
		stmt->exchange(soci::use(TagTypes[i]));
	}
	Init(session, stmt);
}

mvceditor::AllTagsResultClass::AllTagsResultClass()
	: TagResultClass() {

}

void mvceditor::AllTagsResultClass::Prepare(soci::session& session) {
	std::string sql;
	sql += "SELECT r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON (f.file_item_id = r.file_item_id)";
	
	soci::statement* stmt = new soci::statement(session);
	stmt->prepare(sql);
	Init(session, stmt);
}

mvceditor::FileTagResultClass::FileTagResultClass()
	: FileTag()
	, SourceDirs()
	, FilePart() 
	, LineNumber(0)
	, ExactMatch(false)
	, Stmt(NULL)
	, IsEmpty(false) 
	, FileTagId(0)
	, FullPath()
	, IsNew(0) {
}

mvceditor::FileTagResultClass::~FileTagResultClass() {
	if (Stmt) {
		delete Stmt;
	}
}

void mvceditor::FileTagResultClass::Init(soci::session& session, soci::statement* stmt) {
	Stmt = stmt;
	try {
		Stmt->exchange(soci::into(FullPath));
		Stmt->exchange(soci::into(FileTagId));
		Stmt->exchange(soci::into(IsNew));
		Stmt->define_and_bind();
		bool hasData = Stmt->execute(true);
		if (hasData) {
			IsEmpty =  false;
		}
		else {
			IsEmpty = true;
			delete Stmt;
			Stmt = NULL;
		}
	} catch (std::exception& e) {
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
		delete Stmt;
		Stmt = NULL;
	}
}

bool mvceditor::FileTagResultClass::Empty() const {
	return IsEmpty;
}

bool mvceditor::FileTagResultClass::More() const {
	return !IsEmpty && NULL != Stmt;
}

void mvceditor::FileTagResultClass::Next() {
	FileTag.FileId = FileTagId;
	FileTag.FullPath = mvceditor::CharToWx(FullPath.c_str());
	FileTag.IsNew = IsNew > 0;
	bool more = Stmt->fetch();
	if (!more) {
		delete Stmt;
		Stmt = NULL;
	}
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

void mvceditor::FileTagResultClass::Prepare(soci::session& session, bool doLimit) {
	wxASSERT_MSG(!Stmt, wxT("statmement must be null"));

	// add the SQL wildcards
	std::string escaped = mvceditor::SqliteSqlLikeEscape(FilePart, '^');
	std::string query;
	std::string sql;
	if (ExactMatch) {
		escaped = "'" + escaped + "'";
		sql += "SELECT f.full_path, file_item_id, is_new FROM file_items f LEFT JOIN sources s ON (f.source_id = s.source_id) WHERE f.full_path = " + query + " ESCAPE '^'";
	}
	else {
		query = "'%" + escaped + "%'";
		sql += "SELECT f.full_path, file_item_id, is_new FROM file_items f LEFT JOIN sources s ON (f.source_id = s.source_id) WHERE f.full_path LIKE " + query + " ESCAPE '^' ";
	}
	if (!SourceDirs.empty()) {
		sql += " AND s.directory IN(?";
		for (size_t i = 1; i < SourceDirs.size(); ++i) {
			sql += ",?"; 
		}
		sql += ")";
	}
	soci::statement* stmt = new soci::statement(session);
	stmt->prepare(sql);
	for (size_t i = 0; i < SourceDirs.size(); i++) {
		stmt->exchange(soci::use(SourceDirs[i]));
	}
	Init(session, stmt);
}

std::vector<mvceditor::TagClass> mvceditor::FileTagResultClass::Matches() {
	std::vector<mvceditor::TagClass> matches;
	while (More()) {
		Next();
		wxString path,
			currentFileName,
			extension;
		wxFileName::SplitPath(FileTag.FullPath, &path, &currentFileName, &extension);
		currentFileName += wxT(".") + extension;
		wxString fileName = mvceditor::CharToWx(FilePart.c_str());
		fileName = fileName.Lower();
		if (wxNOT_FOUND != currentFileName.Lower().Find(fileName)) {
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

mvceditor::TagSearchClass::TagSearchClass(UnicodeString resourceQuery)
	: FileName()
	, ClassName()
	, MethodName()
	, NamespaceName()
	, SourceDirs()
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

		// check the entire class hierachy; stop as soon as we found it
		// combine the parent classes with the class being searched
		std::vector<UnicodeString> classHierarchy = GetParentClasses();
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
	else {
		mvceditor::ExactNonMemberTagResultClass* nonMemberResults = new mvceditor::ExactNonMemberTagResultClass();
		nonMemberResults->Set(GetClassName(), GetSourceDirs());
		results = nonMemberResults;
	}
	return results;
}

mvceditor::TagResultClass* mvceditor::TagSearchClass::CreateNearMatchResults() const {
	mvceditor::TagResultClass* results = NULL; 
	if (mvceditor::TagSearchClass::CLASS_NAME_METHOD_NAME == GetResourceType()) {
		mvceditor::NearMatchMemberTagResultClass* nearMatchMembersResult = new mvceditor::NearMatchMemberTagResultClass();

		// check the entire class hierachy; stop as soon as we found it
		// combine the parent classes with the class being searched
		std::vector<UnicodeString> classHierarchy = GetParentClasses();
		classHierarchy.push_back(QualifyName(GetNamespaceName(), GetClassName()));

		nearMatchMembersResult->Set(classHierarchy, GetMethodName(), GetSourceDirs());
		results = nearMatchMembersResult;
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
	else {
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
	return results;
}

mvceditor::FileTagResultClass* mvceditor::TagSearchClass::CreateNearMatchFileResults() const {
	mvceditor::FileTagResultClass* result = new mvceditor::FileTagResultClass();
	if (mvceditor::TagSearchClass::FILE_NAME_LINE_NUMBER == GetResourceType()) {
		result->Set(GetFileName(), GetLineNumber(), false, GetSourceDirs());
	}
	else {
		result->Set(GetClassName(), 0, false, GetSourceDirs());
	}
	return result;
}

mvceditor::TagResultClass::TagResultClass() 
	: Tag()
	, Stmt(NULL)
	, IsEmpty(true)
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

mvceditor::TagResultClass::~TagResultClass() {
	if (Stmt) {
		delete Stmt;
	}
}

void mvceditor::TagResultClass::Prepare(soci::session& session, bool doLimit) {
}


void mvceditor::TagResultClass::Init(soci::session& session, soci::statement* stmt) {
	try {
		Stmt = stmt;
		Stmt->exchange(soci::into(FileTagId, FileTagIdIndicator));
		Stmt->exchange(soci::into(SourceId));
		Stmt->exchange(soci::into(Key));
		Stmt->exchange(soci::into(Identifier));
		Stmt->exchange(soci::into(ClassName));
		Stmt->exchange(soci::into(Type));
		Stmt->exchange(soci::into(NamespaceName));
		Stmt->exchange(soci::into(Signature));
		Stmt->exchange(soci::into(ReturnType));
		Stmt->exchange(soci::into(Comment));
		Stmt->exchange(soci::into(FullPath, FullPathIndicator));
		Stmt->exchange(soci::into(IsProtected));
		Stmt->exchange(soci::into(IsPrivate));
		Stmt->exchange(soci::into(IsStatic));
		Stmt->exchange(soci::into(IsDynamic));
		Stmt->exchange(soci::into(IsNative));
		Stmt->exchange(soci::into(FileIsNew, FileIsNewIndicator));
		Stmt->define_and_bind();
		if (Stmt->execute(true)) {
			IsEmpty = false;
		}
		else {
			IsEmpty = true;
			delete Stmt;
			Stmt = NULL;
		}
	} catch (std::exception& e) {
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
}



void mvceditor::TagResultClass::Init(soci::session& session, const std::string& sql) {
	try {
		Stmt =  new soci::statement(session);
		*Stmt = (session.prepare << sql,
			soci::into(FileTagId, FileTagIdIndicator), soci::into(SourceId), soci::into(Key), soci::into(Identifier), soci::into(ClassName), 
			soci::into(Type), soci::into(NamespaceName), soci::into(Signature), 
			soci::into(ReturnType), soci::into(Comment), soci::into(FullPath, FullPathIndicator), soci::into(IsProtected), soci::into(IsPrivate), 
			soci::into(IsStatic), soci::into(IsDynamic), soci::into(IsNative), soci::into(FileIsNew, FileIsNewIndicator)
		);
		if (Stmt->execute(true)) {
			IsEmpty = false;
		}
		else {
			IsEmpty = true;
			delete Stmt;
			Stmt = NULL;
		}
	} catch (std::exception& e) {
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
		delete Stmt;
		Stmt = NULL;
	}
}

void mvceditor::TagResultClass::Next() {
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
	if (Stmt) {
		bool next = Stmt->fetch();
		if (!next) {
			delete Stmt;
			Stmt = NULL;
		}
	}
}

std::vector<mvceditor::TagClass> mvceditor::TagResultClass::Matches() {
	std::vector<mvceditor::TagClass> matches;
	while (More()) {
		Next();
		matches.push_back(Tag);
	}
	return matches;
}

bool mvceditor::TagResultClass::Empty() const {
	return IsEmpty || NULL == Stmt;
}

bool mvceditor::TagResultClass::More() const {
	return NULL != Stmt;
}

mvceditor::TagFinderClass::TagFinderClass()
	: Session(NULL)
	, IsCacheInitialized(false) {
}

mvceditor::TagFinderClass::~TagFinderClass() {

}

void mvceditor::TagFinderClass::Init(soci::session* session) {
	Session = session;
	IsCacheInitialized = true;
}

bool mvceditor::TagFinderClass::GetResourceMatchPosition(const mvceditor::TagClass& tag, const UnicodeString& text, int32_t& pos, 
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

std::vector<mvceditor::TagClass> mvceditor::TagFinderClass::NearMatchTags(
	const mvceditor::TagSearchClass& tagSearch,
	bool doCollectFileNames) {

	
	// at one point there was a check here to see if the  tag files existed
	// it was removed because it caused performance issues, since this method
	// is called while the user is typing text.
	// take care when coding; make sure that any code called by this method does not touch the file system
	std::vector<mvceditor::TagClass> matches;
	mvceditor::FileTagResultClass fileTagResult;
	fileTagResult.Set(tagSearch.GetFileName(), tagSearch.GetLineNumber(), false, tagSearch.GetSourceDirs());
	mvceditor::NearMatchNonMemberTagResultClass nonMembers;
	mvceditor::ExactNonMemberTagResultClass exactNonMembers;
	switch (tagSearch.GetResourceType()) {
		case mvceditor::TagSearchClass::FILE_NAME:
		case mvceditor::TagSearchClass::FILE_NAME_LINE_NUMBER:
			fileTagResult.Prepare(*Session, true);
			matches = fileTagResult.Matches();
			break;
		case mvceditor::TagSearchClass::CLASS_NAME:
			exactNonMembers.Set(tagSearch.GetClassName(), tagSearch.GetSourceDirs());
			exactNonMembers.Prepare(*Session, true);
			matches = exactNonMembers.Matches();
			if (matches.empty()) {
				nonMembers.Set(tagSearch.GetClassName(), tagSearch.GetSourceDirs());
				nonMembers.Prepare(*Session, true);
				matches = nonMembers.Matches();
				if (matches.empty() && doCollectFileNames) {
					fileTagResult.Prepare(*Session, true);
					matches = fileTagResult.Matches();
				}
			}
			break;
		case mvceditor::TagSearchClass::CLASS_NAME_METHOD_NAME:
			matches = NearMatchMembers(tagSearch);
			break;
		case mvceditor::TagSearchClass::NAMESPACE_NAME:
			nonMembers.Set(QualifyName(tagSearch.GetNamespaceName(), tagSearch.GetClassName()), tagSearch.GetSourceDirs());
			nonMembers.AddTagType(mvceditor::TagClass::NAMESPACE);
			nonMembers.Prepare(*Session, true);
			matches = nonMembers.Matches();
			break;
	}
	sort(matches.begin(), matches.end());
	return matches;
}

std::vector<mvceditor::TagClass> mvceditor::TagFinderClass::NearMatchClassesOrFiles(
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
			matches = fileTagResult.Matches();
			break;
		case mvceditor::TagSearchClass::NAMESPACE_NAME:
			nonMembers.Set(QualifyName(tagSearch.GetNamespaceName(), tagSearch.GetClassName()), tagSearch.GetSourceDirs());
			nonMembers.SetTagType(mvceditor::TagClass::CLASS);
			nonMembers.Prepare(*Session, true);
			matches = nonMembers.Matches();
			if (matches.empty()) {
				fileTagResult.Prepare(*Session, true);
				matches = fileTagResult.Matches();
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
				matches = fileTagResult.Matches();
			}
			break;
	}
	sort(matches.begin(), matches.end());
	return matches;
}

std::vector<mvceditor::TagClass> mvceditor::TagFinderClass::NearMatchMembers(const mvceditor::TagSearchClass& tagSearch) {
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
	bool error = false;
	wxString errorMsg;
	std::vector<int> fileTagIds = mvceditor::FileTagIdsForDirs(*Session, tagSearch.GetSourceDirs(), error, errorMsg);
	wxASSERT_MSG(!error, errorMsg);
	if (tagSearch.GetMethodName().isEmpty()) {
		
		// special case; query for all methods for a class (UserClass::)
		mvceditor::AllMembersTagResultClass allMembersResult;
		allMembersResult.Set(classesToSearch, UNICODE_STRING_SIMPLE(""), tagSearch.GetSourceDirs());
		
		// do not limit, a class may have more than 100 members
		allMembersResult.Prepare(*Session, false);
		std::vector<mvceditor::TagClass> memberMatches = allMembersResult.Matches();

		//get the methods that belong to a used trait
		std::vector<mvceditor::TagClass> traitMatches = TraitAliases(classesToSearch, UNICODE_STRING_SIMPLE(""), fileTagIds);

		matches.insert(matches.end(), memberMatches.begin(), memberMatches.end());
		matches.insert(matches.end(), traitMatches.begin(), traitMatches.end());
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
		nearMatchMemberResult.Set(classesToSearch, tagSearch.GetMethodName(), tagSearch.GetSourceDirs());
		nearMatchMemberResult.Prepare(*Session, true);
		matches = nearMatchMemberResult.Matches();

		// get any aliases
		std::vector<mvceditor::TagClass> traitMatches = TraitAliases(classesToSearch, tagSearch.GetMethodName(), fileTagIds);
		matches.insert(matches.end(), traitMatches.begin(), traitMatches.end());
			
	}
	return matches;
}

UnicodeString mvceditor::TagFinderClass::ParentClassName(const UnicodeString& fullyQualifiedClassName) {
	UnicodeString parentClassName;
	
	// empty file items == search on all files
	std::vector<wxFileName> sourceDirs;
	mvceditor::ExactNonMemberTagResultClass exactResult;
	exactResult.Set(fullyQualifiedClassName, sourceDirs);
	exactResult.SetTagType(mvceditor::TagClass::CLASS);
	exactResult.Prepare(*Session, true);
	std::vector<mvceditor::TagClass> matches = exactResult.Matches();
	if (!matches.empty()) {
		mvceditor::TagClass tag = matches[0];
		parentClassName = ExtractParentClassFromSignature(tag.Signature);
	}
	return parentClassName;
}

std::vector<UnicodeString> mvceditor::TagFinderClass::GetResourceTraits(const UnicodeString& className, 
																		const UnicodeString& methodName,
																		const std::vector<wxFileName>& sourceDirs) {
	bool error = false;
	wxString errorMsg;
	std::vector<int> fileTagIds = mvceditor::FileTagIdsForDirs(*Session, sourceDirs, error, errorMsg);
	wxASSERT_MSG(!error, errorMsg);
	std::vector<UnicodeString> inheritedTraits;
	bool match = false;
	
	std::vector<std::string> keys;
	keys.push_back(mvceditor::IcuToChar(className));
	std::vector<mvceditor::TraitTagClass> matches = UsedTraits(keys, fileTagIds);
	for (size_t i = 0; i < matches.size(); ++i) {
		UnicodeString fullyQualifiedTrait = QualifyName(matches[i].TraitNamespaceName, matches[i].TraitClassName);
			
		// trait is used unless there is an explicit insteadof 
		match = true;
		for (size_t j = 0; j < matches[i].InsteadOfs.size(); ++j) {
			if (matches[i].InsteadOfs[j].caseCompare(fullyQualifiedTrait, 0) == 0) {
				match = false;
				break;
			}
		}
		if (match) {
			inheritedTraits.push_back(matches[i].TraitClassName);
		}
	}
	return inheritedTraits;
}

UnicodeString mvceditor::TagFinderClass::ExtractParentClassFromSignature(const UnicodeString& signature) const {

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

bool mvceditor::TagFinderClass::ExactTags(mvceditor::TagResultClass* results) {

	// at one point there was a check here to see if the  tag files existed
	// it was removed because it caused performance issues, since this method
	// is called while the user is typing text.
	// take care when coding; make sure that any code called by this method does not touch the file system
	results->Prepare(*Session, true);
	return !results->Empty();
}

std::vector<mvceditor::TagClass> mvceditor::TagFinderClass::ExactClassOrFile(const mvceditor::TagSearchClass& tagSearch) {

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
		allMatches = fileTagResult.Matches();
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

void mvceditor::TagFinderClass::EnsureMatchesExist(std::vector<TagClass>& matches) {

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

bool mvceditor::TagFinderClass::IsFileCacheEmpty() {
	if (!IsCacheInitialized) {
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

bool mvceditor::TagFinderClass::IsResourceCacheEmpty() {
	if (!IsCacheInitialized) {
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

bool mvceditor::TagFinderClass::HasFullPath(const wxString& fullPath) {
	mvceditor::FileTagClass fileTag;
	return FindFileTagByFullPathExact(fullPath, fileTag);
}

bool mvceditor::TagFinderClass::FindFileTagByFullPathExact(const wxString& fullPath, mvceditor::FileTagClass& fileTag) {
	if (!IsCacheInitialized) {
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

std::vector<mvceditor::TagClass> mvceditor::TagFinderClass::All() {
	mvceditor::AllTagsResultClass result;
	result.Prepare(*Session);

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
	: TagFinderClass() {

}

std::vector<mvceditor::TagClass> mvceditor::ParsedTagFinderClass::TraitAliases(const std::vector<UnicodeString>& classNames, const UnicodeString& methodName, 
																			   const std::vector<int>& fileTagIds) {
	std::vector<mvceditor::TagClass> matches;

	std::vector<std::string> classNamesToLookFor;
	for (std::vector<UnicodeString>::const_iterator it = classNames.begin(); it != classNames.end(); ++it) {
		classNamesToLookFor.push_back(mvceditor::IcuToChar(*it));
	}

	// TODO use the correct namespace when querying for traits
	std::vector<mvceditor::TraitTagClass> traits = UsedTraits(classNamesToLookFor, fileTagIds);
	
	// now go through the result and add the method names of any aliased methods
	UnicodeString lowerMethodName(methodName);
	lowerMethodName.toLower();
	for (size_t i = 0; i < traits.size(); i++) {
		std::vector<UnicodeString> aliases = traits[i].Aliased;
		for (size_t a = 0; a < aliases.size(); a++) {
			UnicodeString alias(aliases[a]);
			UnicodeString lowerAlias(alias);
			lowerAlias.toLower();
			bool useAlias = methodName.isEmpty() || lowerMethodName.indexOf(lowerAlias) == 0;
			if (useAlias) {
				mvceditor::TagClass res;
				res.ClassName = traits[i].TraitClassName;
				res.Identifier = alias;
				matches.push_back(res);
			}
		}
	}
	return matches;
}

std::vector<mvceditor::TraitTagClass> mvceditor::ParsedTagFinderClass::UsedTraits(const std::vector<std::string>& keyStarts, const std::vector<int>& fileTagIds) {
	std::ostringstream stream;
	stream << "SELECT key, file_item_id, class_name, namespace_name, trait_name, trait_namespace_name, aliases, instead_ofs "
		<< "FROM trait_resources WHERE key IN(";
	for (size_t i = 0; i < keyStarts.size(); ++i) {
		stream << "'"
			<< keyStarts[i]
			<< "'";
		if (i < (keyStarts.size() - 1)) {
			stream << ",";
		}
	}
	stream << ")";
	if (!fileTagIds.empty()) {
		stream << "AND file_item_id IN(";
		InClause(fileTagIds, stream);
		stream << ")";
	}
	std::string key;
	int fileTagId;
	std::string className;
	std::string namespaceName;
	std::string traitClassName;
	std::string traitNamespaceName;
	std::string aliases;
	std::string insteadOfs;
	std::vector<mvceditor::TraitTagClass> matches;
	std::string sql = stream.str();
	try {		
		soci::statement stmt = (Session->prepare << sql,
			soci::into(key), soci::into(fileTagId), soci::into(className), soci::into(namespaceName), soci::into(traitClassName),
			soci::into(traitNamespaceName), soci::into(aliases), soci::into(insteadOfs)
		);
		if (stmt.execute(true)) {
			do {
				mvceditor::TraitTagClass trait;
				trait.Key = mvceditor::CharToIcu(key.c_str());
				trait.FileTagId = fileTagId;
				trait.ClassName = mvceditor::CharToIcu(className.c_str());
				trait.NamespaceName = mvceditor::CharToIcu(namespaceName.c_str());
				trait.TraitClassName = mvceditor::CharToIcu(traitClassName.c_str());
				trait.TraitNamespaceName = mvceditor::CharToIcu(traitNamespaceName.c_str());
				
				size_t start = 0;
				size_t found = aliases.find_first_of(",");
				while (found != std::string::npos) {
					trait.Aliased.push_back(mvceditor::CharToIcu(aliases.substr(start, found).c_str()));	
					start = found++;
				}
				if (!aliases.empty()) {
					trait.Aliased.push_back(mvceditor::CharToIcu(aliases.substr(start, found).c_str()));
				}

				start = 0;
				found = insteadOfs.find_first_of(",");
				while (found != std::string::npos) {
					trait.InsteadOfs.push_back(mvceditor::CharToIcu(insteadOfs.substr(start, found).c_str()));	
					start = found++;
				}
				if (!insteadOfs.empty()) {
					trait.InsteadOfs.push_back(mvceditor::CharToIcu(insteadOfs.substr(start, found).c_str()));
				}

				matches.push_back(trait);
			} while (stmt.fetch());
		}
	} catch (std::exception& e) {
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return matches;
}

std::vector<mvceditor::TagClass> mvceditor::ParsedTagFinderClass::FindByIdentifierExactAndTypes(const std::string& identifier, const std::vector<int>& types, const std::vector<int>& fileTagIds, bool doLimit) {
	// TODO remove
	std::vector<mvceditor::TagClass> tags;
	return tags;

}

std::vector<mvceditor::TagClass> mvceditor::ParsedTagFinderClass::FindByIdentifierStartAndTypes(const std::string& identifierStart, const std::vector<int>& types, 
																								const std::vector<int>& fileTagIds, bool doLimit) {
	// TODO remove
	std::vector<mvceditor::TagClass> tags;																					
	return tags;
}


std::vector<mvceditor::TagClass> mvceditor::ParsedTagFinderClass::ClassesFunctionsDefines(const wxString& fullPath) {
	std::vector<mvceditor::TagClass> tags;
	mvceditor::TopLevelTagInFileResultClass result;
	result.Set(fullPath);
	result.Prepare(*Session);
	tags = result.Matches();
	return tags;
}

void mvceditor::TagFinderClass::Print() {
	std::vector<mvceditor::TagClass> tags = All();
	std::vector<mvceditor::TagClass>::iterator tag;
	for (tag = tags.begin(); tag != tags.end(); ++tag) {
		printf("key=%s\n", mvceditor::IcuToChar(tag->Key).c_str());
		printf("type=%d fileItemId=%d\n", tag->Type, tag->FileTagId);
		printf("identifier=%s\n", mvceditor::IcuToChar(tag->Identifier).c_str());
		printf("\n\n");
	}
}

mvceditor::DetectedTagFinderClass::DetectedTagFinderClass()
	: TagFinderClass() {

}

std::vector<mvceditor::TagClass> mvceditor::DetectedTagFinderClass::ResourceStatementMatches(std::string whereCond, bool doLimit) {
	std::string sql;
	sql += "SELECT key, type, class_name, method_name, return_type, namespace_name, comment ";
	sql += "FROM detected_tags WHERE ";
	sql += whereCond;
	sql += " ORDER BY key";
	if (doLimit) {
		sql += " LIMIT 100";
	}

	std::vector<mvceditor::TagClass> matches;
	if (!IsCacheInitialized) {
		return matches;
	}
	std::string key;
	int type;
	std::string className;
	std::string identifier;
	std::string returnType;
	std::string namespaceName;
	std::string comment;
	int isDynamic = true;
	try {
		soci::statement stmt = (Session->prepare << sql,
			soci::into(key), soci::into(type), soci::into(className), 
			soci::into(identifier), soci::into(returnType), soci::into(namespaceName), 
			soci::into(comment)
		);
		if (stmt.execute(true)) {
			do {
				mvceditor::TagClass tag;
				tag.Key = mvceditor::CharToIcu(key.c_str());
				tag.Type = (mvceditor::TagClass::Types)type;
				tag.ClassName = mvceditor::CharToIcu(className.c_str());
				tag.Identifier = mvceditor::CharToIcu(identifier.c_str());
				tag.ReturnType = mvceditor::CharToIcu(returnType.c_str());
				tag.NamespaceName = mvceditor::CharToIcu(namespaceName.c_str());		
				tag.Comment = mvceditor::CharToIcu(comment.c_str());
				tag.IsDynamic = isDynamic != 0;

				matches.push_back(tag);
			} while (stmt.fetch());
		}
	} catch (std::exception& e) {
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return matches;
}

std::vector<mvceditor::TagClass> mvceditor::DetectedTagFinderClass::TraitAliases(const std::vector<UnicodeString>& classNames, const UnicodeString& methodName, const std::vector<int>& fileTagIds) {
	
	// detector db does not have  a trait_resources table
	std::vector<mvceditor::TagClass> matches;
	return matches;
}

std::vector<mvceditor::TraitTagClass> mvceditor::DetectedTagFinderClass::UsedTraits(const std::vector<std::string>& keyStarts, const std::vector<int>& fileTagIds) {
	
	// detector db does not have  a trait_resources table
	std::vector<mvceditor::TraitTagClass> matches;
	return matches;
}


std::vector<mvceditor::TagClass> mvceditor::DetectedTagFinderClass::ClassesFunctionsDefines(const wxString& fullPath) {

	// detector db does not have a file_items table
	std::vector<mvceditor::TagClass> tags;
	return tags;
}


std::vector<mvceditor::TagClass> mvceditor::DetectedTagFinderClass::FindByIdentifierExactAndTypes(const std::string& identifier, const std::vector<int>& types,
																								  const std::vector<int>& fileTagIds, bool doLimit) {
	std::ostringstream stream;

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	// do not get fully qualified resources
	// make sure to use the key because it is indexed
	stream << "key = '" << identifier << "' AND method_name = key AND type IN(";
	InClause(types, stream);
	stream << ")";
	return ResourceStatementMatches(stream.str(), doLimit);
}

std::vector<mvceditor::TagClass> mvceditor::DetectedTagFinderClass::FindByIdentifierStartAndTypes(const std::string& identifierStart, const std::vector<int>& types,
																								  const std::vector<int>& fileTagIds, bool doLimit) {
	std::ostringstream stream;

	// do not get fully qualified resources
	// make sure to use the key because it is indexed
	std::string escaped = mvceditor::SqliteSqlLikeEscape(identifierStart, '^');
	stream << "key LIKE '" << escaped << "%' ESCAPE '^' AND method_name = key AND type IN(";
	InClause(types, stream);
	stream << ")";
	return ResourceStatementMatches(stream.str(), doLimit);
}
