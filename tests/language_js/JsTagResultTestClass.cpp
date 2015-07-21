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
#include <UnitTest++.h>
#include <string>
#include <vector>
#include "globals/Assets.h"
#include "language_js/JsTagResultClass.h"
#include "language_php/FileTags.h"
#include "language_php/PhpTagClass.h"
#include "SqliteTestFixtureClass.h"
#include "TriumphChecks.h"

class JsTagResultFixtureClass : SqliteTestFixtureClass {
	public:
	/**
	 * The tags that we will insert, then expect to be
	 * found in the results
	 */
	t4p::JsTagClass Tag1;
	t4p::JsTagClass Tag2;
	t4p::FileTagClass File1Tag;
	t4p::FileTagClass File2Tag;

	/**
	 * the source dirs to use in testing
	 */
	std::vector<wxFileName> SourceDirs;

	JsTagResultFixtureClass()
	: SqliteTestFixtureClass(t4p::JsTagsSqlSchemaAsset())
	, Tag1()
	, Tag2()
	, File1Tag()
	, File2Tag() {
		wxFileName file1;
		file1.AssignDir(wxFileName::GetTempDir());
		file1.SetName(wxT("name.js"));
		File1Tag.MakeNew(
			file1,
			wxDateTime::Now(),
			1);
		File1Tag.SourceId = 12;
		File1Tag.IsParsed = 1;
		t4p::FileTagPersist(Session, File1Tag);

		wxFileName file2;
		file2.AssignDir(wxFileName::GetTempDir());
		file2.SetName("extract.js");
		File2Tag.SourceId = 49;
		File2Tag.DateTime = wxDateTime::Now();
		File2Tag.MakeNew(
			file2,
			wxDateTime::Now(),
			1);
		t4p::FileTagPersist(Session, File2Tag);

		Tag1.FileTagId = File1Tag.FileId;
		Tag1.SourceId = File1Tag.SourceId;
		Tag1.Key = UNICODE_STRING_SIMPLE("extractName");
		Tag1.Identifier = UNICODE_STRING_SIMPLE("extractName");
		Tag1.Signature = UNICODE_STRING_SIMPLE("function extractName()");
		Tag1.Comment = UNICODE_STRING_SIMPLE("this is the extractName function");
		Tag1.LineNumber = 4;
		Tag1.ColumnPosition = 4;

		PersistResource(Tag1);

		Tag2.FileTagId = File2Tag.FileId;
		Tag2.SourceId = File2Tag.SourceId;
		Tag2.Key = UNICODE_STRING_SIMPLE("dateFormat");
		Tag2.Identifier = UNICODE_STRING_SIMPLE("dateFormat");
		Tag2.Signature = UNICODE_STRING_SIMPLE("function dateFormat(d)");
		Tag2.Comment = UNICODE_STRING_SIMPLE("this is the dateFormat function");
		Tag2.LineNumber = 20;
		Tag2.ColumnPosition = 8;

		PersistResource(Tag2);
	}

	void PersistResource(t4p::JsTagClass& tag) {
		std::string key = t4p::IcuToChar(tag.Key);
		std::string identifier = t4p::IcuToChar(tag.Identifier);
		std::string signature = t4p::IcuToChar(tag.Signature);
		std::string comment = t4p::IcuToChar(tag.Comment);
		Session.once <<
			"INSERT INTO resources(" <<
			"file_item_id, source_id, key, identifier, signature, comment, " <<
			"line_number, column_position) " <<
			"VALUES (?,?,?,?,?,?,?,?)"
			, soci::use(tag.FileTagId)
			, soci::use(tag.SourceId)
			, soci::use(key)
			, soci::use(identifier)
			, soci::use(signature)
			, soci::use(comment)
			, soci::use(tag.LineNumber)
			, soci::use(tag.ColumnPosition);
	}

	bool Init(t4p::JsTagResultClass& result) {
		return result.Exec(Session, false);
	}
};

SUITE(JsTagResultTestClass) {
TEST_FIXTURE(JsTagResultFixtureClass, ExactMatch) {
	t4p::ExactMatchJsTagResultClass result;
	result.SetSearch(UNICODE_STRING_SIMPLE("dateFormat"), SourceDirs);
	bool good = Init(result);
	CHECK(good);
	CHECK(result.More());
	result.Next();

	CHECK_EQUAL(Tag2.FileTagId, result.JsTag.FileTagId);
	CHECK_EQUAL(Tag2.SourceId, result.JsTag.SourceId);
	CHECK_EQUAL(0, Tag2.Key.compare(result.JsTag.Key));
	CHECK_EQUAL(0, Tag2.Signature.compare(result.JsTag.Signature));
	CHECK_EQUAL(0, Tag2.Identifier.compare(result.JsTag.Identifier));
	CHECK_EQUAL(0, Tag2.Comment.compare(result.JsTag.Comment));
	CHECK_EQUAL(Tag2.LineNumber, result.JsTag.LineNumber);
	CHECK_EQUAL(Tag2.ColumnPosition, result.JsTag.ColumnPosition);

	CHECK(!result.More());
}

TEST_FIXTURE(JsTagResultFixtureClass, NearMatch) {
	t4p::NearMatchJsTagResultClass result;
	result.SetSearch(UNICODE_STRING_SIMPLE("extr"), SourceDirs);
	bool good = Init(result);
	CHECK(good);
	CHECK(result.More());
	result.Next();

	CHECK_EQUAL(Tag1.FileTagId, result.JsTag.FileTagId);
	CHECK_EQUAL(Tag1.SourceId, result.JsTag.SourceId);
	CHECK_EQUAL(0, Tag1.Key.compare(result.JsTag.Key));
	CHECK_EQUAL(0, Tag1.Signature.compare(result.JsTag.Signature));
	CHECK_EQUAL(0, Tag1.Identifier.compare(result.JsTag.Identifier));
	CHECK_EQUAL(0, Tag1.Comment.compare(result.JsTag.Comment));
	CHECK_EQUAL(Tag1.LineNumber, result.JsTag.LineNumber);
	CHECK_EQUAL(Tag1.ColumnPosition, result.JsTag.ColumnPosition);

	CHECK(!result.More());
}
}
