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

#include <language/DetectedTagFinderResultClass.h>

mvceditor::DetectedTagTotalCountResultClass::DetectedTagTotalCountResultClass()
: SqliteResultClass() 
, TotalCount(0) {

}

bool mvceditor::DetectedTagTotalCountResultClass::Prepare(soci::session& session, bool doLimit) {
	std::string sql;
	sql += "SELECT COUNT(*) FROM detected_tags";

	bool ret = false;
	try {
		soci::statement stmt = (session.prepare << sql, 
			soci::into(TotalCount)
		);
		stmt.execute(true);
		ret = true;
	} catch (std::exception& e) {
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return ret;
}

void mvceditor::DetectedTagTotalCountResultClass::Next() {
	// nothing since there is only one row in the result
}

int mvceditor::DetectedTagTotalCountResultClass::GetTotalCount() const {
	return TotalCount;
}

mvceditor::DetectedTagExactMemberResultClass::DetectedTagExactMemberResultClass()
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
, Comment() {
	TagTypes.push_back(mvceditor::TagClass::MEMBER);
	TagTypes.push_back(mvceditor::TagClass::METHOD);
	TagTypes.push_back(mvceditor::TagClass::CLASS_CONSTANT);
}

bool mvceditor::DetectedTagExactMemberResultClass::Init(soci::statement* stmt) {
	wxString error;
	bool ret = false;
	try {
		stmt->exchange(soci::into(Key));
		stmt->exchange(soci::into(Type));
		stmt->exchange(soci::into(ClassName));
		stmt->exchange(soci::into(Identifier));
		stmt->exchange(soci::into(ReturnType));
		stmt->exchange(soci::into(NamespaceName));
		stmt->exchange(soci::into(Comment));

		ret = AdoptStatement(stmt, error);
	} catch (std::exception& e) {
		error = mvceditor::CharToWx(e.what());
		wxASSERT_MSG(false, error);
	}
	return ret;
}

void mvceditor::DetectedTagExactMemberResultClass::Next() {
	Tag.Key = mvceditor::CharToIcu(Key.c_str());
	Tag.Type = (mvceditor::TagClass::Types)Type;
	Tag.ClassName = mvceditor::CharToIcu(ClassName.c_str());
	Tag.Identifier = mvceditor::CharToIcu(Identifier.c_str());
	Tag.ReturnType = mvceditor::CharToIcu(ReturnType.c_str());
	Tag.NamespaceName = mvceditor::CharToIcu(NamespaceName.c_str());
	Tag.Comment = mvceditor::CharToIcu(Comment.c_str());
	Tag.IsPrivate = false;
	Tag.IsProtected = false;
	Tag.IsStatic = false;

	Fetch();
}

void mvceditor::DetectedTagExactMemberResultClass::Set(const std::vector<UnicodeString>& classNames, const UnicodeString &memberName,
		const std::vector<wxFileName>& sourceDirectories) {
	wxASSERT_MSG(!classNames.empty(), wxT("classNames must not be empty"));
	for (size_t i = 0; i < classNames.size(); i++) {
		UnicodeString key = classNames[i] + UNICODE_STRING_SIMPLE("::") + memberName;
		Keys.push_back(mvceditor::IcuToChar(key));
	}
	for (size_t i = 0; i < sourceDirectories.size(); i++) {
		SourceDirectories.push_back(mvceditor::WxToChar(sourceDirectories[i].GetPathWithSep()));
	}
}

bool mvceditor::DetectedTagExactMemberResultClass::Prepare(soci::session &session, bool doLimit) {
	std::string sql;
	sql += "SELECT key, type, class_name, method_name, return_type, namespace_name, comment ";
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

	soci::statement* stmt = new soci::statement(session);
	stmt->prepare(sql);

	for (size_t i = 0; i < Keys.size(); i++) {
		stmt->exchange(soci::use(Keys[i]));
	}
	for (size_t i = 0; i < TagTypes.size(); i++) {
		stmt->exchange(soci::use(TagTypes[i]));
	}
	for (size_t i = 0; i < SourceDirectories.size(); i++) {
		stmt->exchange(soci::use(SourceDirectories[i]));
	}
	return Init(stmt);
}

mvceditor::DetectedTagNearMatchMemberResultClass::DetectedTagNearMatchMemberResultClass() 
: DetectedTagExactMemberResultClass()
, KeyUpper() {

}

void mvceditor::DetectedTagNearMatchMemberResultClass::Set(const std::vector<UnicodeString>& classNames, const UnicodeString& memberName,
		const std::vector<wxFileName>& sourceDirectories) {
	wxASSERT_MSG(!classNames.empty(), wxT("classNames must not be empty"));
	for (size_t i = 0; i < classNames.size(); i++) {
		UnicodeString key = classNames[i] + UNICODE_STRING_SIMPLE("::") + memberName;

		// put the key in two times so that we can buld a BETWEEN expression to emulate a LIKE
		Keys.push_back(mvceditor::IcuToChar(key));
		Keys.push_back(mvceditor::IcuToChar(key) + "zzzzzzzzzz");
	}
	ClassCount = classNames.size();
	for (size_t i = 0; i < sourceDirectories.size(); i++) {
		SourceDirectories.push_back(mvceditor::WxToChar(sourceDirectories[i].GetPathWithSep()));
	}
}

bool mvceditor::DetectedTagNearMatchMemberResultClass::Prepare(soci::session &session, bool doLimit) {
	std::string sql;
	sql += "SELECT key, type, class_name, method_name, return_type, namespace_name, comment ";
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

	soci::statement* stmt = new soci::statement(session);
	stmt->prepare(sql);

	for (size_t i = 0; i < Keys.size(); i++) {
		stmt->exchange(soci::use(Keys[i]));
	}
	for (size_t i = 0; i < TagTypes.size(); i++) {
		stmt->exchange(soci::use(TagTypes[i]));
	}
	for (size_t i = 0; i < SourceDirectories.size(); i++) {
		stmt->exchange(soci::use(SourceDirectories[i]));
	}
	return Init(stmt);
}
