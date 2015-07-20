/**
 * @copyright  2013 Roberto Perpuly
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

#include <language_php/DetectedTagFinderResultClass.h>
#include <string>
#include <vector>

t4p::DetectedTagTotalCountResultClass::DetectedTagTotalCountResultClass()
: SqliteResultClass()
, TotalCount(0) {
}

bool t4p::DetectedTagTotalCountResultClass::DoPrepare(soci::statement& stmt, bool doLimit) {
	std::string sql;
	sql += "SELECT COUNT(*) FROM detected_tags";

	bool ret = false;
	try {
		stmt.prepare(sql);
	} catch (std::exception& e) {
		wxString msg = t4p::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return ret;
}

void t4p::DetectedTagTotalCountResultClass::DoBind(soci::statement& stmt) {
	stmt.exchange(soci::into(TotalCount));
}

void t4p::DetectedTagTotalCountResultClass::Next() {
	// nothing since there is only one row in the result
}

int t4p::DetectedTagTotalCountResultClass::GetTotalCount() const {
	return TotalCount;
}

t4p::DetectedTagExactMemberResultClass::DetectedTagExactMemberResultClass()
: SqliteResultClass()
, Tag()
, Keys()
, SourceDirectories()
, MemberName()
, TagTypes()
, Key()
, Type()
, ClassName()
, Identifier()
, ReturnType()
, NamespaceName()
, Signature()
, Comment()
, IsStatic(0) {
	TagTypes.push_back(t4p::PhpTagClass::MEMBER);
	TagTypes.push_back(t4p::PhpTagClass::METHOD);
	TagTypes.push_back(t4p::PhpTagClass::CLASS_CONSTANT);
}

void t4p::DetectedTagExactMemberResultClass::DoBind(soci::statement&  stmt) {
	wxString error;
	try {
		stmt.exchange(soci::into(Key));
		stmt.exchange(soci::into(Type));
		stmt.exchange(soci::into(ClassName));
		stmt.exchange(soci::into(Identifier));
		stmt.exchange(soci::into(ReturnType));
		stmt.exchange(soci::into(NamespaceName));
		stmt.exchange(soci::into(Signature));
		stmt.exchange(soci::into(Comment));
		stmt.exchange(soci::into(IsStatic));
	} catch (std::exception& e) {
		error = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, error);
	}
}

void t4p::DetectedTagExactMemberResultClass::Next() {
	Tag.Key = t4p::CharToIcu(Key.c_str());
	Tag.Type = (t4p::PhpTagClass::Types)Type;
	Tag.ClassName = t4p::CharToIcu(ClassName.c_str());
	Tag.Identifier = t4p::CharToIcu(Identifier.c_str());
	Tag.ReturnType = t4p::CharToIcu(ReturnType.c_str());
	Tag.NamespaceName = t4p::CharToIcu(NamespaceName.c_str());
	Tag.Signature = t4p::CharToIcu(Signature.c_str());
	Tag.Comment = t4p::CharToIcu(Comment.c_str());
	Tag.IsPrivate = false;
	Tag.IsProtected = false;
	Tag.IsStatic = IsStatic != 0;

	Fetch();
}

void t4p::DetectedTagExactMemberResultClass::Set(const std::vector<UnicodeString>& classNames, const UnicodeString &memberName,
		const std::vector<wxFileName>& sourceDirectories) {
	wxASSERT_MSG(!classNames.empty(), wxT("classNames must not be empty"));
	for (size_t i = 0; i < classNames.size(); i++) {
		UnicodeString key = classNames[i] + UNICODE_STRING_SIMPLE("::") + memberName;
		Keys.push_back(t4p::IcuToChar(key));
	}
	for (size_t i = 0; i < sourceDirectories.size(); i++) {
		SourceDirectories.push_back(t4p::WxToChar(sourceDirectories[i].GetPathWithSep()));
	}
}

bool t4p::DetectedTagExactMemberResultClass::DoPrepare(soci::statement& stmt, bool doLimit) {
	std::string sql;
	sql += "SELECT key, type, class_name, method_name, return_type, namespace_name, signature, comment, is_static ";
	sql += "FROM detected_tags LEFT JOIN sources ON (sources.source_id = detected_tags.source_id) WHERE ";

	sql += "key IN (?";
	for (size_t i = 1; i < Keys.size(); ++i) {
		sql += ", ?";
	}
	sql += ") ";

	sql += "AND type IN(?";
	for (size_t i = 1; i < TagTypes.size(); ++i) {
		sql += ", ?";
	}
	sql += ") ";

	if (!SourceDirectories.empty()) {
		sql += "AND directory IN(?";
		for (size_t i = 1; i < SourceDirectories.size(); ++i) {
			sql += ", ?";
		}
		sql += ") ";
	}
	sql += "ORDER BY key";
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
	for (size_t i = 0; i < SourceDirectories.size(); i++) {
		stmt.exchange(soci::use(SourceDirectories[i]));
	}
	return true;
}

t4p::DetectedTagNearMatchMemberResultClass::DetectedTagNearMatchMemberResultClass()
: DetectedTagExactMemberResultClass()
, KeyUpper() {
}

void t4p::DetectedTagNearMatchMemberResultClass::Set(const std::vector<UnicodeString>& classNames, const UnicodeString& memberName,
		const std::vector<wxFileName>& sourceDirectories) {
	wxASSERT_MSG(!classNames.empty(), wxT("classNames must not be empty"));
	for (size_t i = 0; i < classNames.size(); i++) {
		UnicodeString key = classNames[i] + UNICODE_STRING_SIMPLE("::") + memberName;

		// put the key in two times so that we can buld a BETWEEN expression to emulate a LIKE
		Keys.push_back(t4p::IcuToChar(key));
		Keys.push_back(t4p::IcuToChar(key) + "zzzzzzzzzz");
	}
	ClassCount = classNames.size();
	for (size_t i = 0; i < sourceDirectories.size(); i++) {
		SourceDirectories.push_back(t4p::WxToChar(sourceDirectories[i].GetPathWithSep()));
	}
}

bool t4p::DetectedTagNearMatchMemberResultClass::DoPrepare(soci::statement& stmt, bool doLimit) {
	std::string sql;
	sql += "SELECT key, type, class_name, method_name, return_type, namespace_name, signature, comment, is_static ";
	sql += "FROM detected_tags LEFT JOIN sources ON (sources.source_id = detected_tags.source_id) WHERE ";

	// not using LIKE operator here, there are way too many situations where sqlite won't use the index
	// index won't be used when ESCAPE is used or when sqlite3_prepare_v2 is NOT used (which soci does not use)
	// see http://sqlite.org/optoverview.html (LIKE optimization)
	// instead we use a trick to mimic LIKE using the BETWEEN
	// http://stackoverflow.com/questions/13056193/escape-wildcards-in-sqlite-like-without-sacrificing-index-use
	sql += "((key BETWEEN ? AND ?) ";
	for (int i = 1; i < ClassCount; ++i) {
		sql += " OR (key BETWEEN ? AND ?)";
	}
	sql += ") ";

	sql += "AND type IN(?";
	for (size_t i = 1; i < TagTypes.size(); ++i) {
		sql += ", ?";
	}
	sql += ") ";

	if (!SourceDirectories.empty()) {
		sql += "AND directory IN(?";
		for (size_t i = 1; i < SourceDirectories.size(); ++i) {
			sql += ", ?";
		}
		sql += ") ";
	}

	sql += "ORDER BY key";
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
	for (size_t i = 0; i < SourceDirectories.size(); i++) {
		stmt.exchange(soci::use(SourceDirectories[i]));
	}
	return true;
}
