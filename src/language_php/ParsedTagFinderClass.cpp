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
#include <language_php/ParsedTagFinderClass.h>
#include <search/FinderClass.h>
#include <globals/String.h>
#include <globals/Errors.h>
#include <language_php/FileTags.h>
#include <wx/filename.h>
#include <algorithm>
#include <fstream>
#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include <unicode/fmtable.h>
#include <unicode/numfmt.h>
#include <soci/soci.h>
#include <string>

namespace t4p {

class AllMembersTagResultClass : public t4p::ExactMemberTagResultClass {

	public:

	AllMembersTagResultClass();

	void Set(const std::vector<UnicodeString>& classNames, const UnicodeString& memberName, const std::vector<wxFileName>& sourceDirs);

	protected:

	bool DoPrepare(soci::statement& stmt, bool doLimit);

	private:

	int ClassCount;
};

class ExactNonMemberTagResultClass : public t4p::TagResultClass {

	public:

	ExactNonMemberTagResultClass();

	virtual void Set(const UnicodeString& key, const std::vector<wxFileName>& sourceDirs);

	void SetFileTagId(int fileTagId);

	void SetTagType(t4p::PhpTagClass::Types type);

	protected:

	bool DoPrepare(soci::statement& stmt, bool doLimit);

	std::string Key;

	std::vector<int> TagTypes;

	std::vector<std::string> SourceDirs;

	int FileTagIdSearch;
};

class NearMatchNonMemberTagResultClass : public t4p::ExactNonMemberTagResultClass {

	public:

	NearMatchNonMemberTagResultClass();

	void Set(const UnicodeString& key, const std::vector<wxFileName>& sourceDirs);

	void AddTagType(t4p::PhpTagClass::Types type);

	protected:

	bool DoPrepare(soci::statement& stmt, bool doLimit);

	private:

	std::string KeyUpper;
};

// special case, query across all classes for a method (::getName)
// if ClassName is empty, then just check method names This ensures
// queries like '::getName' will work as well.
// make sure to NOT get fully qualified  matches (key=identifier)
class ExactMemberOnlyTagResultClass : public t4p::TagResultClass {

	public:

	ExactMemberOnlyTagResultClass();


	void Set(const UnicodeString& key, const std::vector<wxFileName>& sourceDirs);

	void SetMethodType(bool onlyStatic);

	void SetPropertyType(bool onlyStatic);

	protected:

	bool DoPrepare(soci::statement& stmt, bool doLimit);

	std::string Key;

	std::vector<int> TagTypes;

	std::vector<std::string> SourceDirs;

	bool OnlyStatic;
};

class NearMatchMemberOnlyTagResultClass : public ExactMemberOnlyTagResultClass {

	public:
	NearMatchMemberOnlyTagResultClass();

	void Set(const UnicodeString& key, const std::vector<wxFileName>& sourceDirs);

	protected:

	bool DoPrepare(soci::statement& stmt, bool doLimit);

	private:

	std::string KeyUpper;
};

class TopLevelTagInFileResultClass : public TagResultClass {

	public:
	TopLevelTagInFileResultClass();

	void Set(const wxString& fullPath);

	protected:

	bool DoPrepare(soci::statement& stmt, bool doLimit);

	private:

	std::string FullPath;

	std::vector<int> TagTypes;
};

class AllTagsResultClass : public TagResultClass {

	public:

	AllTagsResultClass();

	protected:

	bool DoPrepare(soci::statement& stmt, bool doLimit);
};

/**
 * queries for tags by their primary key
 */
class TagByIdResultClass : public TagResultClass {

	public:

	TagByIdResultClass();

	void Set(int id);

	protected:

	bool DoPrepare(soci::statement& stmt, bool doLimit);

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

t4p::ExactMemberTagResultClass::ExactMemberTagResultClass()
	: TagResultClass()
	, Keys()
	, TagTypes()
	, SourceDirs() {
	TagTypes.push_back(t4p::PhpTagClass::CLASS_CONSTANT);
	TagTypes.push_back(t4p::PhpTagClass::MEMBER);
	TagTypes.push_back(t4p::PhpTagClass::METHOD);
}

void t4p::ExactMemberTagResultClass::Set(const std::vector<UnicodeString>& classNames, const UnicodeString& memberName,
											   const std::vector<wxFileName>& sourceDirs) {
	wxASSERT_MSG(!classNames.empty(), wxT("classNames must not be empty"));
	for (size_t i = 0; i < classNames.size(); i++) {
		UnicodeString key = classNames[i] + UNICODE_STRING_SIMPLE("::") + memberName;
		Keys.push_back(t4p::IcuToChar(key));
	}

	// add trailing separator to lookup exact matches that include the last directory
	// we dont want to get directories that start with a prefix
	for (size_t i = 0; i < sourceDirs.size(); i++) {
		SourceDirs.push_back(t4p::WxToChar(sourceDirs[i].GetPathWithSep()));
	}
}

bool t4p::ExactMemberTagResultClass::DoPrepare(soci::statement& stmt,  bool doLimit) {

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, has_variable_args, is_new ";
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

	stmt.prepare(sql);

	for (size_t i = 0; i < Keys.size(); i++) {
		stmt.exchange(soci::use(Keys[i]));
	}
	for (size_t i = 0; i < TagTypes.size(); i++) {
		stmt.exchange(soci::use(TagTypes[i]));
	}
	for (size_t i = 0; i < SourceDirs.size(); i++) {
		stmt.exchange(soci::use(SourceDirs[i]));
	}
	return true;
}

t4p::AllMembersTagResultClass::AllMembersTagResultClass()
	: ExactMemberTagResultClass()
	, ClassCount(0) {
}

void t4p::AllMembersTagResultClass::Set(const std::vector<UnicodeString>& classNames, const UnicodeString& memberName,
											   const std::vector<wxFileName>& sourceDirs) {
	wxASSERT_MSG(!classNames.empty(), wxT("classNames must not be empty"));
	for (size_t i = 0; i < classNames.size(); i++) {
		UnicodeString key = classNames[i] + UNICODE_STRING_SIMPLE("::");

		// put the key in two times so that we can buld a BETWEEN expression to emulate a LIKE
		Keys.push_back(t4p::IcuToChar(key));
		Keys.push_back(t4p::IcuToChar(key) + "zzzzzzzzzz");
	}
	// add trailing separator to lookup exact matches that include the last directory
	// we dont want to get directories that start with a prefix
	for (size_t i = 0; i < sourceDirs.size(); i++) {
		SourceDirs.push_back(t4p::WxToChar(sourceDirs[i].GetPathWithSep()));
	}
	ClassCount = classNames.size();
}

bool t4p::AllMembersTagResultClass::DoPrepare(soci::statement& stmt,  bool doLimit) {

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, has_variable_args, is_new ";
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
	stmt.prepare(sql);
	for (size_t i = 0; i < Keys.size(); i++) {
		stmt.exchange(soci::use(Keys[i]));
	}
	for (size_t i = 0; i < TagTypes.size(); i++) {
		stmt.exchange(soci::use(TagTypes[i]));
	}
	for (size_t i = 0; i < SourceDirs.size(); i++) {
		stmt.exchange(soci::use(SourceDirs[i]));
	}
	return true;
}


t4p::NearMatchMemberTagResultClass::NearMatchMemberTagResultClass()
	: ExactMemberTagResultClass()
	, ClassCount(0)
	, FileItemId(0) {
}

void t4p::NearMatchMemberTagResultClass::SetNearMatchArgs(const std::vector<UnicodeString>& classNames, const UnicodeString& memberName,
												int fileItemId,
											   const std::vector<wxFileName>& sourceDirs) {
	wxASSERT_MSG(!classNames.empty(), wxT("classNames must not be empty"));
	for (size_t i = 0; i < classNames.size(); i++) {
		UnicodeString key = classNames[i] + UNICODE_STRING_SIMPLE("::") + memberName;

		// put the key in two times so that we can buld a BETWEEN expression to emulate a LIKE
		Keys.push_back(t4p::IcuToChar(key));
		Keys.push_back(t4p::IcuToChar(key) + "zzzzzzzzzz");
	}

	// add trailing separator to lookup exact matches that include the last directory
	// we dont want to get directories that start with a prefix
	for (size_t i = 0; i < sourceDirs.size(); i++) {
		SourceDirs.push_back(t4p::WxToChar(sourceDirs[i].GetPathWithSep()));
	}
	ClassCount = classNames.size();
	FileItemId = fileItemId;
}


bool t4p::NearMatchMemberTagResultClass::DoPrepare(soci::statement& stmt,  bool doLimit) {
	wxASSERT_MSG(!Keys.empty(), wxT("keys cannot be empty"));

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, has_variable_args, is_new ";
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
	stmt.prepare(sql);
	for (size_t i = 0; i < Keys.size(); i++) {
		stmt.exchange(soci::use(Keys[i]));
	}
	for (size_t i = 0; i < TagTypes.size(); i++) {
		stmt.exchange(soci::use(TagTypes[i]));
	}
	for (size_t i = 0; i < SourceDirs.size(); i++) {
		stmt.exchange(soci::use(SourceDirs[i]));
	}
	if (FileItemId) {
		stmt.exchange(soci::use(FileItemId));
	}
	return true;
}

t4p::ExactNonMemberTagResultClass::ExactNonMemberTagResultClass()
	: TagResultClass()
	, Key()
	, TagTypes()
	, SourceDirs()
	, FileTagIdSearch(0) {
	TagTypes.push_back(t4p::PhpTagClass::DEFINE);
	TagTypes.push_back(t4p::PhpTagClass::CLASS);
	TagTypes.push_back(t4p::PhpTagClass::FUNCTION);
}

void t4p::ExactNonMemberTagResultClass::Set(const UnicodeString& key, const std::vector<wxFileName>& sourceDirs) {
	Key = t4p::IcuToChar(key);

	// add trailing separator to lookup exact matches that include the last directory
	// we dont want to get directories that start with a prefix
	for (size_t i = 0; i < sourceDirs.size(); i++) {
		SourceDirs.push_back(t4p::WxToChar(sourceDirs[i].GetPathWithSep()));
	}
}

void t4p::ExactNonMemberTagResultClass::SetTagType(t4p::PhpTagClass::Types type) {
	TagTypes.clear();
	TagTypes.push_back(type);
}

void t4p::ExactNonMemberTagResultClass::SetFileTagId(int fileTagId) {
	FileTagIdSearch = fileTagId;
}

bool t4p::ExactNonMemberTagResultClass::DoPrepare(soci::statement& stmt, bool doLimit) {

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, has_variable_args, is_new ";
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
	stmt.prepare(sql);
	stmt.exchange(soci::use(Key));
	for (size_t i = 0; i < TagTypes.size(); i++) {
		stmt.exchange(soci::use(TagTypes[i]));
	}
	for (size_t i = 0; i < SourceDirs.size(); i++) {
		stmt.exchange(soci::use(SourceDirs[i]));
	}
	if (FileTagIdSearch) {
		stmt.exchange(soci::use(FileTagIdSearch));
	}
	return true;
}

t4p::NearMatchNonMemberTagResultClass::NearMatchNonMemberTagResultClass()
	: ExactNonMemberTagResultClass()
	, KeyUpper() {

}

void t4p::NearMatchNonMemberTagResultClass::Set(const UnicodeString& key, const std::vector<wxFileName>& sourceDirs) {
	Key = t4p::IcuToChar(key);
	KeyUpper = t4p::IcuToChar(key) + "zzzzzzzzzz";

	// add trailing separator to lookup exact matches that include the last directory
	// we dont want to get directories that start with a prefix
	for (size_t i = 0; i < sourceDirs.size(); i++) {
		SourceDirs.push_back(t4p::WxToChar(sourceDirs[i].GetPathWithSep()));
	}
}

void t4p::NearMatchNonMemberTagResultClass::AddTagType(t4p::PhpTagClass::Types type) {
	TagTypes.push_back(type);
}

bool t4p::NearMatchNonMemberTagResultClass::DoPrepare(soci::statement& stmt, bool doLimit) {

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, has_variable_args, is_new ";
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
	stmt.prepare(sql);
	stmt.exchange(soci::use(Key));
	stmt.exchange(soci::use(KeyUpper));
	for (size_t i = 0; i < TagTypes.size(); i++) {
		stmt.exchange(soci::use(TagTypes[i]));
	}
	for (size_t i = 0; i < SourceDirs.size(); i++) {
		stmt.exchange(soci::use(SourceDirs[i]));
	}
	return true;
}

t4p::ExactMemberOnlyTagResultClass::ExactMemberOnlyTagResultClass()
	: TagResultClass()
	, Key()
	, TagTypes()
	, SourceDirs()
	, OnlyStatic(false) {
	TagTypes.push_back(t4p::PhpTagClass::CLASS_CONSTANT);
	TagTypes.push_back(t4p::PhpTagClass::MEMBER);
	TagTypes.push_back(t4p::PhpTagClass::METHOD);
}

void t4p::ExactMemberOnlyTagResultClass::Set(const UnicodeString& memberName,
											   const std::vector<wxFileName>& sourceDirs) {
	Key = t4p::IcuToChar(memberName);

	// add trailing separator to lookup exact matches that include the last directory
	// we dont want to get directories that start with a prefix
	for (size_t i = 0; i < sourceDirs.size(); i++) {
		SourceDirs.push_back(t4p::WxToChar(sourceDirs[i].GetPathWithSep()));
	}
}

void t4p::ExactMemberOnlyTagResultClass::SetMethodType(bool onlyStatic) {
	TagTypes.clear();
	TagTypes.push_back(t4p::PhpTagClass::METHOD);
	OnlyStatic = onlyStatic;
}

void t4p::ExactMemberOnlyTagResultClass::SetPropertyType(bool onlyStatic) {
	TagTypes.clear();
	TagTypes.push_back(t4p::PhpTagClass::CLASS_CONSTANT);
	TagTypes.push_back(t4p::PhpTagClass::MEMBER);
	OnlyStatic = onlyStatic;
}

bool t4p::ExactMemberOnlyTagResultClass::DoPrepare(soci::statement& stmt,  bool doLimit) {

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, has_variable_args, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON(r.file_item_id = f.file_item_id) LEFT JOIN sources s ON(s.source_id = r.source_id) WHERE ";

	// make sure to use the key because it is indexed
	sql += "key = ? AND identifier = key";
	sql += " AND type IN(?";
	for (size_t i = 1; i <  TagTypes.size(); ++i) {
		sql += ",?";
	}
	sql += ")";
	if (!SourceDirs.empty()) {
		sql += " AND s.directory IN(?";
		for (size_t i = 1; i <  SourceDirs.size(); ++i) {
			sql += ",?";
		}
		sql += ")";
	}
	if (OnlyStatic) {
		sql += " AND is_static = 1";
	}
	sql += " ORDER BY key";
	if (doLimit) {
		sql += " LIMIT 100";
	}
	stmt.prepare(sql);

	stmt.exchange(soci::use(Key));
	for (size_t i = 0; i < TagTypes.size(); i++) {
		stmt.exchange(soci::use(TagTypes[i]));
	}
	for (size_t i = 0; i < SourceDirs.size(); i++) {
		stmt.exchange(soci::use(SourceDirs[i]));
	}
	return true;
}

t4p::NearMatchMemberOnlyTagResultClass::NearMatchMemberOnlyTagResultClass()
: ExactMemberOnlyTagResultClass()
, KeyUpper() {

}

void t4p::NearMatchMemberOnlyTagResultClass::Set(const UnicodeString& key, const std::vector<wxFileName>& sourceDirs) {
	Key = t4p::IcuToChar(key);
	KeyUpper = t4p::IcuToChar(key) + "zzzzzzzzzz";

	// add trailing separator to lookup exact matches that include the last directory
	// we dont want to get directories that start with a prefix
	for (size_t i = 0; i < sourceDirs.size(); i++) {
		SourceDirs.push_back(t4p::WxToChar(sourceDirs[i].GetPathWithSep()));
	}
}

bool t4p::NearMatchMemberOnlyTagResultClass::DoPrepare(soci::statement& stmt, bool doLimit) {

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, has_variable_args, is_new ";
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
	stmt.prepare(sql);
	stmt.exchange(soci::use(Key));
	stmt.exchange(soci::use(KeyUpper));
	for (size_t i = 0; i < TagTypes.size(); i++) {
		stmt.exchange(soci::use(TagTypes[i]));
	}
	for (size_t i = 0; i < SourceDirs.size(); i++) {
		stmt.exchange(soci::use(SourceDirs[i]));
	}
	return true;
}

t4p::TopLevelTagInFileResultClass::TopLevelTagInFileResultClass()
	: TagResultClass()
	, FullPath()
	, TagTypes() {
	TagTypes.push_back(t4p::PhpTagClass::CLASS);
	TagTypes.push_back(t4p::PhpTagClass::FUNCTION);
	TagTypes.push_back(t4p::PhpTagClass::DEFINE);
}

void t4p::TopLevelTagInFileResultClass::Set(const wxString& fullPath) {
	FullPath = t4p::WxToChar(fullPath);
}

bool t4p::TopLevelTagInFileResultClass::DoPrepare(soci::statement& stmt, bool doLimit) {

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	// remove the duplicates from fully qualified namespaces
	// fully qualified classes / functions will start with backslash; but we want the
	// tags that don't begin with backslash
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, has_variable_args, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON(r.file_item_id = f.file_item_id) WHERE ";
	sql += "f.full_path = ? AND Type IN(?, ?, ?) AND key NOT LIKE '\\%' ORDER BY key ";

	stmt.prepare(sql);
	stmt.exchange(soci::use(FullPath));
	for (size_t i = 0; i < TagTypes.size(); i++) {
		stmt.exchange(soci::use(TagTypes[i]));
	}
	return true;
}

t4p::AllTagsResultClass::AllTagsResultClass()
	: TagResultClass() {

}

bool t4p::AllTagsResultClass::DoPrepare(soci::statement& stmt, bool doLimit) {
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, has_variable_args, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON (f.file_item_id = r.file_item_id)";

	stmt.prepare(sql);
	return true;
}

t4p::TagByIdResultClass::TagByIdResultClass()
	: TagResultClass()
	, Id(0) {

}

void t4p::TagByIdResultClass::Set(int id) {
	Id = id;
}

bool t4p::TagByIdResultClass::DoPrepare(soci::statement& stmt, bool doLimit) {
	std::string sql;
	sql += "SELECT r.id, r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, f.full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, has_variable_args, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON (f.file_item_id = r.file_item_id) ";
	sql += "WHERE r.id = ?";

	stmt.prepare(sql);
	stmt.exchange(soci::use(Id));
	return true;
}


t4p::FileTagResultClass::FileTagResultClass()
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

void t4p::FileTagResultClass::Next() {
	FileTag.FileId = FileTagId;
	FileTag.FullPath = t4p::CharToWx(FullPath.c_str());
	FileTag.IsNew = IsNew > 0;
	Fetch();
}

void t4p::FileTagResultClass::Set(const UnicodeString& filePart, int lineNumber, bool exactMatch, const std::vector<wxFileName>& sourceDirs) {
	FilePart = t4p::IcuToChar(filePart);
	LineNumber = lineNumber;

	// add trailing separator to lookup exact matches that include the last directory
	// we dont want to get directories that start with a prefix
	for (size_t i = 0; i < sourceDirs.size(); i++) {
		SourceDirs.push_back(t4p::WxToChar(sourceDirs[i].GetPathWithSep()));
	}
	ExactMatch = exactMatch;
}

bool t4p::FileTagResultClass::DoPrepare(soci::statement& stmt, bool doLimit) {

	// add the SQL wildcards
	std::string escaped = t4p::SqliteSqlLikeEscape(FilePart, '^');
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
		stmt.prepare(sql);
		if (ExactMatch) {

			// yes this is correct; there are 2 input params
			// that use the file name
			stmt.exchange(soci::use(FilePart));
			stmt.exchange(soci::use(FilePart));
		}
		for (size_t i = 0; i < SourceDirs.size(); i++) {
			stmt.exchange(soci::use(SourceDirs[i]));
		}
		ret = true;
	} catch (std::exception& e) {
		error = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, error);
	}
	return ret;
}

void t4p::FileTagResultClass::DoBind(soci::statement& stmt) {
	stmt.exchange(soci::into(FullPath));
	stmt.exchange(soci::into(FileTagId));
	stmt.exchange(soci::into(IsNew));
}

std::vector<t4p::FileTagClass> t4p::FileTagResultClass::Matches() {
	std::vector<t4p::FileTagClass> matches;
	while (More()) {
		Next();
		if (0 == LineNumber || GetLineCountFromFile(FileTag.FullPath) >= LineNumber) {
			matches.push_back(FileTag);
		}
	}
	return matches;
}

std::vector<t4p::PhpTagClass> t4p::FileTagResultClass::MatchesAsTags() {
	std::vector<t4p::PhpTagClass> matches;
	while (More()) {
		Next();
		wxString path;
		if (0 == LineNumber || GetLineCountFromFile(FileTag.FullPath) >= LineNumber) {
			t4p::PhpTagClass newTag;
			wxFileName fileName(FileTag.FullPath);
			newTag.FileTagId = FileTag.FileId;
			newTag.Identifier = t4p::WxToIcu(fileName.GetFullName());
			newTag.FileIsNew = FileTag.IsNew;
			newTag.SetFullPath(FileTag.FullPath);
			matches.push_back(newTag);
		}
	}
	return matches;
}

int t4p::FileTagResultClass::GetLineCountFromFile(const wxString& fullPath) const {
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

t4p::TraitTagResultClass::TraitTagResultClass()
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

void t4p::TraitTagResultClass::Set(const std::vector<UnicodeString>& classNames, const UnicodeString& memberName, bool exactMatch, const std::vector<wxFileName>& sourceDirs) {
	for (size_t i = 0; i < classNames.size(); ++i) {
		Keys.push_back(t4p::IcuToChar(classNames[i]));
	}

	// add trailing separator to lookup exact matches that include the last directory
	// we dont want to get directories that start with a prefix
	for (size_t i = 0; i < sourceDirs.size(); i++) {
		SourceDirs.push_back(t4p::WxToChar(sourceDirs[i].GetPathWithSep()));
	}
	MemberName = memberName;
	ExactMatch = exactMatch;
}

bool t4p::TraitTagResultClass::DoPrepare(soci::statement& stmt, bool doLimit) {
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
		stmt.prepare(sql);
		for (size_t i = 0; i < Keys.size(); ++i) {
			stmt.exchange(soci::use(Keys[i]));
		}
		for (size_t i = 0; i < SourceDirs.size(); ++i) {
			stmt.exchange(soci::use(SourceDirs[i]));
		}
		good = true;
	} catch (std::exception& e) {
		error = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, error);
	}
	return good;
}

void t4p::TraitTagResultClass::DoBind(soci::statement& stmt) {
	stmt.exchange(soci::into(Key));
	stmt.exchange(soci::into(FileTagId));
	stmt.exchange(soci::into(ClassName));
	stmt.exchange(soci::into(NamespaceName));
	stmt.exchange(soci::into(TraitClassName));
	stmt.exchange(soci::into(TraitNamespaceName));
	stmt.exchange(soci::into(Aliases));
	stmt.exchange(soci::into(InsteadOfs));
}

void t4p::TraitTagResultClass::Next() {
	TraitTag.Key = t4p::CharToIcu(Key.c_str());
	TraitTag.FileTagId = FileTagId;
	TraitTag.ClassName = t4p::CharToIcu(ClassName.c_str());
	TraitTag.NamespaceName = t4p::CharToIcu(NamespaceName.c_str());
	TraitTag.TraitClassName = t4p::CharToIcu(TraitClassName.c_str());
	TraitTag.TraitNamespaceName = t4p::CharToIcu(TraitNamespaceName.c_str());

	size_t start = 0;
	size_t found = Aliases.find_first_of(",");
	while (found != std::string::npos) {
		TraitTag.Aliased.push_back(t4p::CharToIcu(Aliases.substr(start, found).c_str()));
		start = found++;
	}
	if (!Aliases.empty()) {
		TraitTag.Aliased.push_back(t4p::CharToIcu(Aliases.substr(start, found).c_str()));
	}

	start = 0;
	found = InsteadOfs.find_first_of(",");
	while (found != std::string::npos) {
		TraitTag.InsteadOfs.push_back(t4p::CharToIcu(InsteadOfs.substr(start, found).c_str()));
		start = found++;
	}
	if (!InsteadOfs.empty()) {
		TraitTag.InsteadOfs.push_back(t4p::CharToIcu(InsteadOfs.substr(start, found).c_str()));
	}
	Fetch();
}

std::vector<t4p::PhpTagClass> t4p::TraitTagResultClass::MatchesAsTags() {
	std::vector<t4p::PhpTagClass> tags;
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
				t4p::PhpTagClass res;
				res.ClassName = TraitTag.TraitClassName;
				res.Identifier = alias;
				tags.push_back(res);
			}
		}
	}
	return tags;
}

t4p::FunctionLookupClass::FunctionLookupClass()
: t4p::SqliteResultClass()
, FunctionName()
, TagType(0)
, Id(0) {
	TagType = t4p::PhpTagClass::FUNCTION;
}

void t4p::FunctionLookupClass::Set(const UnicodeString& functionName) {
	FunctionName = t4p::IcuToChar(functionName);
}

bool t4p::FunctionLookupClass::DoPrepare(soci::statement& stmt, bool doLimit) {
	std::string sql = "SELECT id FROM resources WHERE key = ? AND type = ?";
	bool good = false;
	wxString error;
	try {
		stmt.prepare(sql);
		stmt.exchange(soci::use(FunctionName));
		stmt.exchange(soci::use(TagType));

	} catch (std::exception& e) {
		error = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, error);
	}
	return good;
}

void t4p::FunctionLookupClass::DoBind(soci::statement& stmt) {
	stmt.exchange(soci::into(Id));
}

void t4p::FunctionLookupClass::Next() {

	// nothing else as variable is already bound
	// fetch gets the next row
	Fetch();
}

bool t4p::FunctionLookupClass::Found() {
	return !Empty() && Id > 0;
}

t4p::ClassLookupClass::ClassLookupClass()
: t4p::SqliteResultClass()
, ClassName()
, TagType(0)
, Id(0) {
	TagType = t4p::PhpTagClass::CLASS;
}

void t4p::ClassLookupClass::Set(const UnicodeString& className) {
	ClassName = t4p::IcuToChar(className);
}

bool t4p::ClassLookupClass::DoPrepare(soci::statement& stmt, bool doLimit) {
	std::string sql = "SELECT id FROM resources WHERE key = ? AND type = ?";
	bool good = false;
	wxString error;
	try {
		stmt.prepare(sql);
		stmt.exchange(soci::use(ClassName));
		stmt.exchange(soci::use(TagType));

	} catch (std::exception& e) {
		error = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, error);
	}
	return good;
}

void t4p::ClassLookupClass::DoBind(soci::statement& stmt) {
	stmt.exchange(soci::into(Id));
}

void t4p::ClassLookupClass::Next() {

	// nothing else as variable is already bound
	// fetch gets the next row
	Fetch();
}

bool t4p::ClassLookupClass::Found() {
	return !Empty() && Id > 0;
}


t4p::NamespaceLookupClass::NamespaceLookupClass()
: t4p::SqliteResultClass()
, NamespaceName()
, TagType(0)
, Id(0) {
	TagType = t4p::PhpTagClass::NAMESPACE;
}

void t4p::NamespaceLookupClass::Set(const UnicodeString& namespaceName) {
	NamespaceName = t4p::IcuToChar(namespaceName);
}

bool t4p::NamespaceLookupClass::DoPrepare(soci::statement& stmt, bool doLimit) {
	std::string sql = "SELECT id FROM resources WHERE key = ? AND type = ?";
	bool good = false;
	wxString error;
	try {
		stmt.prepare(sql);
		stmt.exchange(soci::use(NamespaceName));
		stmt.exchange(soci::use(TagType));

	} catch (std::exception& e) {
		error = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, error);
	}
	return good;
}

void t4p::NamespaceLookupClass::DoBind(soci::statement& stmt) {
	stmt.exchange(soci::into(Id));
}

void t4p::NamespaceLookupClass::Next() {

	// nothing else as variable is already bound
	// fetch gets the next row
	Fetch();
}

bool t4p::NamespaceLookupClass::Found() {
	return !Empty() && Id > 0;
}

t4p::MethodLookupClass::MethodLookupClass()
: t4p::SqliteResultClass()
, MethodName()
, TagType(0)
, IsStaticTrue(0)
, IsStaticFalse(0)
, Id(0) {
	TagType = t4p::PhpTagClass::METHOD;
}

void t4p::MethodLookupClass::Set(const UnicodeString& methodName, bool isStatic) {
	MethodName = t4p::IcuToChar(methodName);
	if (isStatic) {
		IsStaticTrue = 1;
		IsStaticFalse = 1;
	}
	else {
		IsStaticTrue = 1;
		IsStaticFalse = 0;
	}
}

bool t4p::MethodLookupClass::DoPrepare(soci::statement& stmt, bool doLimit) {

	// we use two conditions for is_static
	// because the semantics of the query are that
	// if we don't want static methods, then we want to query
	// for both static and instance methods.  Further, we
	// don't want the query to change when the isStatic flag changes
	// otherwise we would need to re-prepare the statement (and negate
	// any performance improvement)
	std::string sql = "SELECT id FROM resources WHERE key = ? AND type = ? AND is_static IN(?, ?)";
	bool good = false;
	wxString error;
	try {
		stmt.prepare(sql);
		stmt.exchange(soci::use(MethodName));
		stmt.exchange(soci::use(TagType));
		stmt.exchange(soci::use(IsStaticTrue));
		stmt.exchange(soci::use(IsStaticFalse));

	} catch (std::exception& e) {
		error = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, error);
	}
	return good;
}

void t4p::MethodLookupClass::DoBind(soci::statement& stmt) {
	stmt.exchange(soci::into(Id));
}

void t4p::MethodLookupClass::Next() {

	// nothing else as variable is already bound
	// fetch gets the next row
	Fetch();
}

bool t4p::MethodLookupClass::Found() {
	return !Empty() && Id > 0;
}


t4p::PropertyLookupClass::PropertyLookupClass()
: t4p::SqliteResultClass()
, PropertyName()
, TagTypeMember(0)
, TagTypeConstant(0)
, IsStaticTrue(0)
, IsStaticFalse(0)
, Id(0) {
	TagTypeMember = t4p::PhpTagClass::MEMBER;
	TagTypeConstant = t4p::PhpTagClass::CLASS_CONSTANT;
}

void t4p::PropertyLookupClass::Set(const UnicodeString& propertyName, bool isStatic) {
	PropertyName = t4p::IcuToChar(propertyName);
	if (isStatic) {
		IsStaticTrue = 1;
		IsStaticFalse = 1;
	}
	else {
		IsStaticTrue = 1;
		IsStaticFalse = 0;
	}
}

bool t4p::PropertyLookupClass::DoPrepare(soci::statement& stmt, bool doLimit) {

	// we use two conditions for is_static
	// because the semantics of the query are that
	// if we don't want static methods, then we want to query
	// for both static and instance methods.  Further, we
	// don't want the query to change when the isStatic flag changes
	// otherwise we would need to re-prepare the statement (and negate
	// any performance improvement)
	std::string sql = "SELECT id FROM resources WHERE key = ? AND type IN(?, ?) AND is_static IN(?, ?)";
	bool good = false;
	wxString error;
	try {
		stmt.prepare(sql);
		stmt.exchange(soci::use(PropertyName));
		stmt.exchange(soci::use(TagTypeMember));
		stmt.exchange(soci::use(TagTypeConstant));
		stmt.exchange(soci::use(IsStaticTrue));
		stmt.exchange(soci::use(IsStaticFalse));

	} catch (std::exception& e) {
		error = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, error);
	}
	return good;
}

void t4p::PropertyLookupClass::DoBind(soci::statement& stmt) {
	stmt.exchange(soci::into(Id));
}

void t4p::PropertyLookupClass::Next() {

	// nothing else as variable is already bound
	// fetch gets the next row
	Fetch();
}

bool t4p::PropertyLookupClass::Found() {
	return !Empty() && Id > 0;
}

t4p::FunctionSignatureLookupClass::FunctionSignatureLookupClass()
: t4p::SqliteResultClass()
, Signature()
, HasVariableArgs(false)
, FunctionName()
, TagType(0)
, Id(0)
, RowHasVariableArgs(0)
, StdSignature() {
	TagType = t4p::PhpTagClass::FUNCTION;
}

void t4p::FunctionSignatureLookupClass::Set(const UnicodeString& functionName) {
	FunctionName = t4p::IcuToChar(functionName);
}

bool t4p::FunctionSignatureLookupClass::DoPrepare(soci::statement& stmt, bool doLimit) {
	std::string sql = "SELECT id, has_variable_args, signature FROM resources WHERE key = ? AND type = ?";
	bool good = false;
	wxString error;
	try {
		stmt.prepare(sql);
		stmt.exchange(soci::use(FunctionName));
		stmt.exchange(soci::use(TagType));

	} catch (std::exception& e) {
		error = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, error);
	}
	return good;
}

void t4p::FunctionSignatureLookupClass::DoBind(soci::statement& stmt) {
	stmt.exchange(soci::into(Id));
	stmt.exchange(soci::into(RowHasVariableArgs));
	stmt.exchange(soci::into(StdSignature));
}

void t4p::FunctionSignatureLookupClass::Next() {

	// very important to assign to instance variables
	// AND THEN fetch the next row, what way
	// we can do this when looping
	//
	//  if (lookup.Exec()) {
	//    while (lookup.More()) {
	//      lookup.Next();
	//      // HasVariableArgs, Signature is the first row
	//      // in the first loop iteration
	//
	//    }
	// }
	HasVariableArgs = RowHasVariableArgs > 0;
	Signature = t4p::CharToIcu(StdSignature.c_str());

	Fetch();
}

bool t4p::FunctionSignatureLookupClass::Found() {
	return !Empty() && Id > 0;
}

t4p::MethodSignatureLookupClass::MethodSignatureLookupClass()
: t4p::SqliteResultClass()
, Signature()
, HasVariableArgs(false)
, MethodName()
, IsStaticTrue(0)
, IsStaticFalse(0)
, TagType(0)
, Id(0)
, StdSignature() {
	TagType = t4p::PhpTagClass::METHOD;
}

void t4p::MethodSignatureLookupClass::Set(const UnicodeString& methodName, bool isStatic) {
	MethodName = t4p::IcuToChar(methodName);
	if (isStatic) {
		IsStaticTrue = 1;
		IsStaticFalse = 1;
	}
	else {
		IsStaticTrue = 1;
		IsStaticFalse = 0;
	}
}

bool t4p::MethodSignatureLookupClass::DoPrepare(soci::statement& stmt, bool doLimit) {

	// we use two conditions for is_static
	// because the semantics of the query are that
	// if we don't want static methods, then we want to query
	// for both static and instance methods.  Further, we
	// don't want the query to change when the isStatic flag changes
	// otherwise we would need to re-prepare the statement (and negate
	// any performance improvement)
	std::string sql = "SELECT id, has_variable_args, signature ";
	sql += "FROM resources ";
	sql += "WHERE key = ? AND type = ? AND is_static IN(?, ?)";
	bool good = false;
	wxString error;
	try {
		stmt.prepare(sql);
		stmt.exchange(soci::use(MethodName));
		stmt.exchange(soci::use(TagType));
		stmt.exchange(soci::use(IsStaticTrue));
		stmt.exchange(soci::use(IsStaticFalse));

	} catch (std::exception& e) {
		error = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, error);
	}
	return good;
}

void t4p::MethodSignatureLookupClass::DoBind(soci::statement& stmt) {
	stmt.exchange(soci::into(Id));
	stmt.exchange(soci::into(RowHasVariableArgs));
	stmt.exchange(soci::into(StdSignature));
}

void t4p::MethodSignatureLookupClass::Next() {

	// very important to assign to instance variables
	// AND THEN fetch the next row, what way
	// we can do this when looping
	//
	//  if (lookup.Exec()) {
	//    while (lookup.More()) {
	//      lookup.Next();
	//      // HasVariableArgs, Signature is the first row
	//      // in the first loop iteration
	//
	//    }
	// }
	HasVariableArgs = RowHasVariableArgs > 0;
	Signature = t4p::CharToIcu(StdSignature.c_str());

	Fetch();
}

bool t4p::MethodSignatureLookupClass::Found() {
	return !Empty() && Id > 0;
}

t4p::TagSearchClass::TagSearchClass(UnicodeString resourceQuery)
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

void t4p::TagSearchClass::SetParentClasses(const std::vector<UnicodeString>& parents) {
	ParentClasses = parents;
}

std::vector<UnicodeString> t4p::TagSearchClass::GetParentClasses() const {
	return ParentClasses;
}

void t4p::TagSearchClass::SetTraits(const std::vector<UnicodeString>& traits) {
	Traits = traits;
}

std::vector<UnicodeString> t4p::TagSearchClass::GetTraits() const {
	return Traits;
}

void t4p::TagSearchClass::SetFileItemId(int id) {
	FileItemId = id;
}

int t4p::TagSearchClass::GetFileItemId() const {
	return FileItemId;
}

std::vector<UnicodeString> t4p::TagSearchClass::GetClassHierarchy() const {
	std::vector<UnicodeString> allClassNames;
	if (!ClassName.isEmpty()) {
		allClassNames.push_back(ClassName);
	}
	allClassNames.insert(allClassNames.end(), ParentClasses.begin(), ParentClasses.end());
	allClassNames.insert(allClassNames.end(), Traits.begin(), Traits.end());
	return allClassNames;

}

void t4p::TagSearchClass::SetSourceDirs(const std::vector<wxFileName>& sourceDirs) {
	SourceDirs = sourceDirs;
}

std::vector<wxFileName> t4p::TagSearchClass::GetSourceDirs() const {
	return SourceDirs;
}

UnicodeString t4p::TagSearchClass::GetClassName() const {
	return ClassName;
}

UnicodeString t4p::TagSearchClass::GetFileName() const {
	return FileName;
}

UnicodeString t4p::TagSearchClass::GetMethodName() const {
	return MethodName;
}

int t4p::TagSearchClass::GetLineNumber() const {
	return LineNumber;
}

t4p::TagSearchClass::ResourceTypes t4p::TagSearchClass::GetResourceType() const {
	return ResourceType;
}

UnicodeString t4p::TagSearchClass::GetNamespaceName() const {
	return NamespaceName;
}

t4p::TagResultClass* t4p::TagSearchClass::CreateExactResults() const {
	t4p::TagResultClass* results = NULL;
	if (GetResourceType() == t4p::TagSearchClass::CLASS_NAME_METHOD_NAME) {
		t4p::ExactMemberTagResultClass* memberResults = new t4p::ExactMemberTagResultClass();

		// check the entire class hierachy
		// combine the parent classes with the class being searched
		std::vector<UnicodeString> classHierarchy = GetParentClasses();
		classHierarchy.insert(classHierarchy.end(), Traits.begin(), Traits.end());
		classHierarchy.push_back(QualifyName(GetNamespaceName(), GetClassName()));

		memberResults->Set(classHierarchy, GetMethodName(), GetSourceDirs());
		results = memberResults;
	}
	else if (t4p::TagSearchClass::NAMESPACE_NAME == GetResourceType()) {
		t4p::ExactNonMemberTagResultClass* nonMemberResults = new t4p::ExactNonMemberTagResultClass();

		std::vector<UnicodeString> classNames;
		UnicodeString key = QualifyName(GetNamespaceName(), GetClassName());
		nonMemberResults->Set(key, GetSourceDirs());
		results = nonMemberResults;
	}
	else if (!ClassName.isEmpty()) {
		t4p::ExactNonMemberTagResultClass* nonMemberResults = new t4p::ExactNonMemberTagResultClass();
		nonMemberResults->Set(GetClassName(), GetSourceDirs());
		results = nonMemberResults;
	}
	else {
		t4p::ExactNonMemberTagResultClass* nonMemberResults = new t4p::ExactNonMemberTagResultClass();
		nonMemberResults->Set(GetFileName(), GetSourceDirs());
		results = nonMemberResults;
	}
	return results;
}

t4p::TagResultClass* t4p::TagSearchClass::CreateNearMatchResults() const {
	t4p::TagResultClass* results = NULL;
	if (t4p::TagSearchClass::CLASS_NAME_METHOD_NAME == GetResourceType() && !GetClassName().isEmpty()) {
		t4p::NearMatchMemberTagResultClass* nearMatchMembersResult = new t4p::NearMatchMemberTagResultClass();

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

		nearMatchMembersResult->SetNearMatchArgs(classHierarchy, GetMethodName(), GetFileItemId(), GetSourceDirs());
		results = nearMatchMembersResult;
	}
	else if ((t4p::TagSearchClass::CLASS_NAME_METHOD_NAME == GetResourceType())
			&& GetClassName().isEmpty()) {
		t4p::NearMatchMemberOnlyTagResultClass* nearMatchMemberOnlyResult = new t4p::NearMatchMemberOnlyTagResultClass();
		nearMatchMemberOnlyResult->Set(GetMethodName(), GetSourceDirs());
		results = nearMatchMemberOnlyResult;
	}
	else if (t4p::TagSearchClass::NAMESPACE_NAME == GetResourceType()) {

		// needle identifier contains a namespace operator; but it may be
		// a namespace or a fully qualified name
		UnicodeString namespaceKey;
		namespaceKey = QualifyName(GetNamespaceName(), GetClassName());
		t4p::NearMatchNonMemberTagResultClass* nearMatchNonMemberNamespaced = new t4p::NearMatchNonMemberTagResultClass();
		nearMatchNonMemberNamespaced->Set(namespaceKey, GetSourceDirs());
		nearMatchNonMemberNamespaced->AddTagType(t4p::PhpTagClass::NAMESPACE);
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
		t4p::NearMatchNonMemberTagResultClass* nearMatchNonMembers = new t4p::NearMatchNonMemberTagResultClass();
		nearMatchNonMembers->Set(key, GetSourceDirs());
		results = nearMatchNonMembers;
	}
	else {
		UnicodeString key = GetFileName();
		t4p::NearMatchNonMemberTagResultClass* nearMatchNonMembers = new t4p::NearMatchNonMemberTagResultClass();
		nearMatchNonMembers->Set(key, GetSourceDirs());
		results = nearMatchNonMembers;
	}
	return results;
}

t4p::FileTagResultClass* t4p::TagSearchClass::CreateExactFileResults() const {
	t4p::FileTagResultClass* result = new t4p::FileTagResultClass();
	if (t4p::TagSearchClass::FILE_NAME_LINE_NUMBER == GetResourceType()) {
		UnicodeString query = GetFileName();
		result->Set(query, GetLineNumber(), true, GetSourceDirs());
	}
	else if (t4p::TagSearchClass::FILE_NAME == GetResourceType()) {
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

t4p::FileTagResultClass* t4p::TagSearchClass::CreateNearMatchFileResults() const {
	t4p::FileTagResultClass* result = new t4p::FileTagResultClass();
	if (t4p::TagSearchClass::FILE_NAME_LINE_NUMBER == GetResourceType()) {
		UnicodeString query = GetFileName();
		result->Set(query, GetLineNumber(), false, GetSourceDirs());
	}
	else if (t4p::TagSearchClass::FILE_NAME == GetResourceType()) {
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

t4p::TagResultClass::TagResultClass()
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
	, HasVariableArgs(false)
	, FileIsNew(false)
	, FileTagIdIndicator()
	, FullPathIndicator()
	, FileIsNewIndicator() {
}

void t4p::TagResultClass::DoBind(soci::statement& stmt) {
	wxString error;
	try {
		stmt.exchange(soci::into(Id));
		stmt.exchange(soci::into(FileTagId, FileTagIdIndicator));
		stmt.exchange(soci::into(SourceId));
		stmt.exchange(soci::into(Key));
		stmt.exchange(soci::into(Identifier));
		stmt.exchange(soci::into(ClassName));
		stmt.exchange(soci::into(Type));
		stmt.exchange(soci::into(NamespaceName));
		stmt.exchange(soci::into(Signature));
		stmt.exchange(soci::into(ReturnType));
		stmt.exchange(soci::into(Comment));
		stmt.exchange(soci::into(FullPath, FullPathIndicator));
		stmt.exchange(soci::into(IsProtected));
		stmt.exchange(soci::into(IsPrivate));
		stmt.exchange(soci::into(IsStatic));
		stmt.exchange(soci::into(IsDynamic));
		stmt.exchange(soci::into(IsNative));
		stmt.exchange(soci::into(HasVariableArgs));
		stmt.exchange(soci::into(FileIsNew, FileIsNewIndicator));

	} catch (std::exception& e) {
		error = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, error);
	}
}

void t4p::TagResultClass::Next() {
	Tag.Id = Id;
	if (soci::i_ok == FileTagIdIndicator) {
		Tag.FileTagId = FileTagId;
	}
	Tag.SourceId = SourceId;
	Tag.Key = t4p::CharToIcu(Key.c_str());
	Tag.Identifier = t4p::CharToIcu(Identifier.c_str());
	Tag.ClassName = t4p::CharToIcu(ClassName.c_str());
	Tag.Type = (t4p::PhpTagClass::Types)Type;
	Tag.NamespaceName = t4p::CharToIcu(NamespaceName.c_str());
	Tag.Signature = t4p::CharToIcu(Signature.c_str());
	Tag.ReturnType = t4p::CharToIcu(ReturnType.c_str());
	Tag.Comment = t4p::CharToIcu(Comment.c_str());
	if (soci::i_ok == FullPathIndicator) {
		Tag.SetFullPath(t4p::CharToWx(FullPath.c_str()));
	}
	Tag.IsProtected = IsProtected != 0;
	Tag.IsPrivate = IsPrivate != 0;
	Tag.IsStatic = IsStatic != 0;
	Tag.IsDynamic = IsDynamic != 0;
	Tag.IsNative = IsNative != 0;
	Tag.HasVariableArgs = HasVariableArgs != 0;
	if (soci::i_ok == FileIsNewIndicator) {
		Tag.FileIsNew = FileIsNew != 0;
	}
	else {
		Tag.FileIsNew = true;
	}
	Fetch();
}

std::vector<t4p::PhpTagClass> t4p::TagResultClass::Matches() {
	std::vector<t4p::PhpTagClass> matches;
	while (More()) {
		Next();
		matches.push_back(Tag);
	}
	return matches;
}

bool t4p::ParsedTagFinderClass::GetResourceMatchPosition(const t4p::PhpTagClass& tag, const UnicodeString& text, int32_t& pos,
		int32_t& length) {
	size_t start = 0;
	t4p::FinderClass finder;
	finder.Mode = FinderClass::REGULAR_EXPRESSION;

	UnicodeString className,
		methodName;
	if (!tag.ClassName.isEmpty()) {
		className = tag.ClassName;
		methodName = tag.Identifier;

		t4p::FinderClass::EscapeRegEx(className);
		t4p::FinderClass::EscapeRegEx(methodName);
	}
	else {
		className = tag.Identifier;
		t4p::FinderClass::EscapeRegEx(className);
	}
	switch (tag.Type) {
		case t4p::PhpTagClass::CLASS:
			finder.Expression = UNICODE_STRING_SIMPLE("\\sclass\\s+") + className + UNICODE_STRING_SIMPLE("\\s");
			break;
		case t4p::PhpTagClass::METHOD:
			//advance past the class header so that if  a function with the same name exists we will skip it
			finder.Expression = UNICODE_STRING_SIMPLE("\\sclass\\s+") + className + UNICODE_STRING_SIMPLE("\\s");
			if (finder.Prepare() && finder.FindNext(text, start)) {
				finder.GetLastMatch(pos, length);
			}
			start = pos + length;

			// method may return a reference (&)
			finder.Expression = UNICODE_STRING_SIMPLE("\\sfunction\\s*(&\\s*)?") + methodName + UNICODE_STRING_SIMPLE("\\s*\\(");
			break;
		case t4p::PhpTagClass::FUNCTION:

			// function may return a reference (&)
			finder.Expression = UNICODE_STRING_SIMPLE("\\sfunction\\s*(&\\s*)?") + className + UNICODE_STRING_SIMPLE("\\s*\\(");
			break;
		case t4p::PhpTagClass::MEMBER:
			//advance past the class header so that if  a variable with the same name exists we will skip it				:
			finder.Expression = UNICODE_STRING_SIMPLE("\\sclass\\s+") + className + UNICODE_STRING_SIMPLE("\\s");
			if (finder.Prepare() && finder.FindNext(text, start)) {
				finder.GetLastMatch(pos, length);
			}
			start = pos + length;
			finder.Expression = UNICODE_STRING_SIMPLE("\\s((var)|(public)|(protected)|(private)).+") + methodName + UNICODE_STRING_SIMPLE(".*;");
			break;
		case t4p::PhpTagClass::DEFINE:
			finder.Expression = UNICODE_STRING_SIMPLE("\\sdefine\\(\\s*('|\")") + className + UNICODE_STRING_SIMPLE("('|\")");
			break;
		case t4p::PhpTagClass::CLASS_CONSTANT:
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

std::vector<t4p::PhpTagClass> t4p::ParsedTagFinderClass::NearMatchClassesOrFiles(
	const t4p::TagSearchClass& tagSearch) {
	std::vector<t4p::PhpTagClass> matches;

	// at one point there was a check here to see if the  tag files existed
	// it was removed because it caused performance issues, since this method
	// is called while the user is typing text.
	// take care when coding; make sure that any code called by this method does not touch the file system
	t4p::FileTagResultClass fileTagResult;
	fileTagResult.Set(tagSearch.GetFileName(), tagSearch.GetLineNumber(), false, tagSearch.GetSourceDirs());
	t4p::NearMatchNonMemberTagResultClass nonMembers;
	switch (tagSearch.GetResourceType()) {
		case t4p::TagSearchClass::FILE_NAME:
		case t4p::TagSearchClass::FILE_NAME_LINE_NUMBER:
			fileTagResult.Exec(Session, true);
			matches = fileTagResult.MatchesAsTags();
			break;
		case t4p::TagSearchClass::NAMESPACE_NAME:
			nonMembers.Set(QualifyName(tagSearch.GetNamespaceName(), tagSearch.GetClassName()), tagSearch.GetSourceDirs());
			nonMembers.SetTagType(t4p::PhpTagClass::CLASS);
			nonMembers.Exec(Session, true);
			matches = nonMembers.Matches();
			if (matches.empty()) {
				fileTagResult.Exec(Session, true);
				matches = fileTagResult.MatchesAsTags();
			}
			break;
		case t4p::TagSearchClass::CLASS_NAME:
		case t4p::TagSearchClass::CLASS_NAME_METHOD_NAME:
			nonMembers.Set(tagSearch.GetClassName(), tagSearch.GetSourceDirs());
			nonMembers.SetTagType(t4p::PhpTagClass::CLASS);
			nonMembers.Exec(Session, true);
			matches = nonMembers.Matches();
			if (matches.empty()) {
				fileTagResult.Exec(Session, true);
				matches = fileTagResult.MatchesAsTags();
			}
			break;
	}
	sort(matches.begin(), matches.end());
	return matches;
}

std::vector<t4p::PhpTagClass> t4p::ParsedTagFinderClass::NearMatchMembers(const t4p::TagSearchClass& tagSearch) {
	std::vector<t4p::PhpTagClass> matches;

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
		t4p::AllMembersTagResultClass allMembersResult;
		allMembersResult.Set(classesToSearch, UNICODE_STRING_SIMPLE(""), tagSearch.GetSourceDirs());

		// do not limit, a class may have more than 100 members
		allMembersResult.Exec(Session, false);
		std::vector<t4p::PhpTagClass> memberMatches = allMembersResult.Matches();

		matches.insert(matches.end(), memberMatches.begin(), memberMatches.end());
	}
	else if (tagSearch.GetClassName().isEmpty()) {

		// special case, query across all classes for a method (::getName)
		// if ClassName is empty, then just check method names This ensures
		// queries like '::getName' will work as well.
		// make sure to NOT get fully qualified  matches (key=identifier)
		t4p::ExactMemberOnlyTagResultClass exactResult;
		exactResult.Set(tagSearch.GetMethodName(), tagSearch.GetSourceDirs());
		exactResult.Exec(Session, true);
		matches = exactResult.Matches();
		if (matches.empty()) {
			t4p::NearMatchMemberOnlyTagResultClass nearMatchResult;
			nearMatchResult.Set(tagSearch.GetMethodName(), tagSearch.GetSourceDirs());
			nearMatchResult.Exec(Session, true);

			// use LIKE to get near matches
			matches = nearMatchResult.Matches();
		}
	}
	else {
		std::vector<std::string> keyStarts;

		// now that we found the parent classes, combine the parent class name and the queried method
		// to make all of the keys we need to look for. remember that a tag class key is of the form
		// ClassName::MethodName
		t4p::NearMatchMemberTagResultClass nearMatchMemberResult;
		nearMatchMemberResult.SetNearMatchArgs(classesToSearch, tagSearch.GetMethodName(), tagSearch.GetFileItemId(), tagSearch.GetSourceDirs());
		nearMatchMemberResult.Exec(Session, true);
		matches = nearMatchMemberResult.Matches();
	}
	return matches;
}

UnicodeString t4p::ParsedTagFinderClass::ParentClassName(const UnicodeString& fullyQualifiedClassName, int fileTagId) {
	UnicodeString parentClassName;

	// empty file items == search on all files
	std::vector<wxFileName> sourceDirs;
	t4p::ExactNonMemberTagResultClass exactResult;
	exactResult.Set(fullyQualifiedClassName, sourceDirs);
	exactResult.SetFileTagId(fileTagId);
	exactResult.SetTagType(t4p::PhpTagClass::CLASS);
	exactResult.Exec(Session, true);
	std::vector<t4p::PhpTagClass> matches = exactResult.Matches();
	if (!matches.empty()) {
		t4p::PhpTagClass tag = matches[0];
		parentClassName = ExtractParentClassFromSignature(tag.Signature);
	}
	return parentClassName;
}

std::vector<UnicodeString> t4p::ParsedTagFinderClass::GetResourceTraits(const UnicodeString& className,
																		const UnicodeString& methodName,
																		const std::vector<wxFileName>& sourceDirs) {
	std::vector<UnicodeString> inheritedTraits;

	std::vector<UnicodeString> classNames;
	classNames.push_back(className);

	t4p::TraitTagResultClass traitResult;
	traitResult.Set(classNames, UNICODE_STRING_SIMPLE(""), false, sourceDirs);
	if (traitResult.Exec(Session, false)) {
		while (traitResult.More()) {
			traitResult.Next();
			t4p::TraitTagClass trait = traitResult.TraitTag;
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

UnicodeString t4p::ParsedTagFinderClass::ExtractParentClassFromSignature(const UnicodeString& signature) const {

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

std::vector<t4p::PhpTagClass> t4p::ParsedTagFinderClass::ExactClassOrFile(const t4p::TagSearchClass& tagSearch) {

	// at one point there was a check here to see if the  tag files existed
	// it was removed because it caused performance issues, since this method
	// is called while the user is typing text.
	// take care when coding; make sure that any code called by this method does not touch the file system
	std::vector<t4p::PhpTagClass> allMatches;
	if (t4p::TagSearchClass::FILE_NAME == tagSearch.GetResourceType() ||
		t4p::TagSearchClass::FILE_NAME_LINE_NUMBER == tagSearch.GetResourceType()) {
		t4p::FileTagResultClass fileTagResult;
		fileTagResult.Set(tagSearch.GetFileName(), tagSearch.GetLineNumber(), true, tagSearch.GetSourceDirs());
		fileTagResult.Exec(Session, true);
		allMatches = fileTagResult.MatchesAsTags();
	}
	else {
		t4p::ExactNonMemberTagResultClass exactResult;
		exactResult.SetTagType(t4p::PhpTagClass::CLASS);
		exactResult.Set(tagSearch.GetClassName(), tagSearch.GetSourceDirs());
		exactResult.Exec(Session, true);
		allMatches = exactResult.Matches();
	}
	return allMatches;
}

std::vector<t4p::PhpTagClass> t4p::ParsedTagFinderClass::ExactClass(const t4p::TagSearchClass& tagSearch) {
	std::vector<t4p::PhpTagClass> allMatches;
	t4p::ExactNonMemberTagResultClass exactResult;
	exactResult.SetTagType(t4p::PhpTagClass::CLASS);
	UnicodeString fullName = QualifyName(tagSearch.GetNamespaceName(), tagSearch.GetClassName());
	exactResult.Set(fullName, tagSearch.GetSourceDirs());
	exactResult.Exec(Session, true);
	allMatches = exactResult.Matches();

	return allMatches;
}

std::vector<t4p::PhpTagClass> t4p::ParsedTagFinderClass::ExactFunction(const t4p::TagSearchClass& tagSearch) {
	std::vector<t4p::PhpTagClass> allMatches;
	t4p::ExactNonMemberTagResultClass exactResult;
	exactResult.SetTagType(t4p::PhpTagClass::FUNCTION);
	UnicodeString fullName = QualifyName(tagSearch.GetNamespaceName(), tagSearch.GetClassName());
	exactResult.Set(fullName, tagSearch.GetSourceDirs());
	exactResult.Exec(Session, true);
	allMatches = exactResult.Matches();

	return allMatches;
}

std::vector<t4p::PhpTagClass> t4p::ParsedTagFinderClass::ExactMethod(const t4p::TagSearchClass& tagSearch, bool onlyStatic) {
	std::vector<t4p::PhpTagClass> allMatches;
	t4p::ExactMemberOnlyTagResultClass exactResult;
	exactResult.SetMethodType(onlyStatic);
	exactResult.Set(tagSearch.GetMethodName(), tagSearch.GetSourceDirs());
	exactResult.Exec(Session, true);
	allMatches = exactResult.Matches();

	return allMatches;
}

std::vector<t4p::PhpTagClass> t4p::ParsedTagFinderClass::ExactProperty(const t4p::TagSearchClass& tagSearch, bool onlyStatic) {
	std::vector<t4p::PhpTagClass> allMatches;
	t4p::ExactMemberOnlyTagResultClass exactResult;
	exactResult.SetPropertyType(onlyStatic);
	exactResult.Set(tagSearch.GetMethodName(), tagSearch.GetSourceDirs());
	exactResult.Exec(Session, true);
	allMatches = exactResult.Matches();

	return allMatches;
}

void t4p::ParsedTagFinderClass::EnsureMatchesExist(std::vector<t4p::PhpTagClass>& matches) {

	// remove from matches that have a file that is no longer in the file system
	std::vector<t4p::PhpTagClass>::iterator it = matches.begin();
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

bool t4p::ParsedTagFinderClass::IsFileCacheEmpty() {
	int fileItemId = 0;
	try {
		Session.once << "SELECT file_item_id FROM file_items LIMIT 1;", soci::into(fileItemId);
	} catch (std::exception& e) {
		wxString msg = t4p::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return fileItemId <= 0;
}

bool t4p::ParsedTagFinderClass::IsResourceCacheEmpty() {

	// make sure only parsed tag came from the native functions file.
	int count = 0;
	try {
		Session.once << "SELECT COUNT(*) FROM resources WHERE is_native = 0;", soci::into(count);
	} catch (std::exception& e) {
		wxString msg = t4p::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return count <= 0;
}

bool t4p::ParsedTagFinderClass::HasFullPath(const wxString& fullPath) {
	t4p::FileTagClass fileTag;
	return FindFileTagByFullPathExact(fullPath, fileTag);
}


bool t4p::ParsedTagFinderClass::HasDir(const wxString& dir) {
	bool foundDir = false;

	// at this point we dont store directories in the db, we look for a file
	// that contains the dir in question
	wxFileName dirName;
	dirName.AssignDir(dir);
	std::string result;
	std::string query = t4p::WxToChar(dirName.GetPathWithSep() + wxT("%"));
	std::string sql = "SELECT full_path FROM file_items WHERE full_path LIKE ? LIMIT 1";
	try {
		soci::statement stmt = (Session.prepare << sql, soci::use(query),
			soci::into(result));
		if (stmt.execute(true) && !result.empty()) {
			foundDir = true;
		}
	} catch (std::exception& e) {
		wxString msg = t4p::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return foundDir;
}

bool t4p::ParsedTagFinderClass::FindFileTagByFullPathExact(const wxString& fullPath, t4p::FileTagClass& fileTag) {
	int fileTagId;
	std::tm lastModified;
	int isParsed;
	int isNew;
	bool foundFile = false;

	std::string query = t4p::WxToChar(fullPath);
	std::string sql = "SELECT file_item_id, last_modified, is_parsed, is_new FROM file_items WHERE full_path = ?";
	try {
		soci::statement stmt = (Session.prepare << sql, soci::use(query),
			soci::into(fileTagId), soci::into(lastModified), soci::into(isParsed), soci::into(isNew));
		foundFile = stmt.execute(true);
		if (foundFile) {
			fileTag.DateTime.Set(lastModified);
			fileTag.FileId = fileTagId;
			fileTag.FullPath = fullPath;
			fileTag.IsNew = isNew != 0;
			fileTag.IsParsed = isParsed != 0;
		}
	} catch (std::exception& e) {
		wxString msg = t4p::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return foundFile;
}

std::vector<t4p::PhpTagClass> t4p::ParsedTagFinderClass::All() {
	t4p::AllTagsResultClass result;
	result.Exec(Session, false);

	// remove the 'duplicates' ie. extra fully qualified entries to make lookups faster
	std::vector<t4p::PhpTagClass> all;
	while (result.More()) {
		result.Next();
		if (result.Tag.Key.indexOf(UNICODE_STRING_SIMPLE("::")) > 0) {

			// fully qualified methods
		}
		else if (result.Tag.Type != t4p::PhpTagClass::NAMESPACE && result.Tag.Key.indexOf(UNICODE_STRING_SIMPLE("\\")) >= 0) {

			// fully qualified classes / functions (with namespace)
		}
		else {
			all.push_back(result.Tag);
		}
	}
	return all;
}

t4p::ParsedTagFinderClass::ParsedTagFinderClass(soci::session& session)
	: SqliteFinderClass(session) {

}

std::vector<t4p::PhpTagClass> t4p::ParsedTagFinderClass::ClassesFunctionsDefines(const wxString& fullPath) {
	std::vector<t4p::PhpTagClass> tags;
	t4p::TopLevelTagInFileResultClass result;
	result.Set(fullPath);
	result.Exec(Session, false);
	tags = result.Matches();
	return tags;
}

void t4p::ParsedTagFinderClass::Print() {
	std::vector<t4p::PhpTagClass> tags = All();
	std::vector<t4p::PhpTagClass>::iterator tag;
	for (tag = tags.begin(); tag != tags.end(); ++tag) {
		printf("key=%s\n", t4p::IcuToChar(tag->Key).c_str());
		printf("type=%d fileItemId=%d\n", tag->Type, tag->FileTagId);
		printf("identifier=%s\n", t4p::IcuToChar(tag->Identifier).c_str());
		printf("\n\n");
	}
}

bool t4p::ParsedTagFinderClass::FindById(int id, t4p::PhpTagClass& tag) {
	t4p::TagByIdResultClass result;
	result.Set(id);
	bool found = Exec(&result);
	if (found) {
		result.Next();
		tag = result.Tag;
	}
	return found;
}

wxString t4p::ParsedTagFinderClass::SourceDirFromFile(int fileTagId) {
	wxString sourcePath;
	std::string stdSourcePath;
	soci::statement stmt = (Session.prepare <<
		"SELECT directory FROM sources s JOIN file_items f ON(s.source_id = f.source_id) WHERE f.file_item_id = ?",
		soci::use(fileTagId), soci::into(stdSourcePath)
	);
	if (stmt.execute(true)) {
		sourcePath = t4p::CharToWx(stdSourcePath.c_str());
	}
	return sourcePath;
}
