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
 * @copyright  2014 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <UnitTest++.h>
#include <FileTestFixtureClass.h>
#include <SqliteTestFixtureClass.h>
#include <language/PhpFunctionCallLintClass.h>
#include <language/TagCacheClass.h>
#include <language/TagFinderList.h>
#include <globals/Assets.h>
#include <unicode/ustream.h> //get the << overloaded operator, needed by UnitTest++
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

class PhpFunctionCallLintTestFixtureClass : 
	public FileTestFixtureClass, public SqliteTestFixtureClass {
	
public:
	
	t4p::TagCacheClass TagCache;
	std::vector<wxString> PhpFileExtensions;
	std::vector<wxString> MiscFileExtensions;
	t4p::PhpFunctionCallLintClass Lint;
	std::vector<t4p::PhpFunctionCallLintResultClass> Results;
	bool HasError;
	
	PhpFunctionCallLintTestFixtureClass()
	: FileTestFixtureClass(wxT("php-function-call-lint"))
	, SqliteTestFixtureClass() 
	, TagCache()
	, PhpFileExtensions()
	, MiscFileExtensions()
	, Lint()
	, Results()
	, HasError(false) {
		Lint.SetVersion(pelet::PHP_53);
		PhpFileExtensions.push_back(wxT("*.php"));
		CreateSubDirectory(wxT("src"));
	}

	void Parse(const UnicodeString& code) {
		
		// create the file, so that we can index it 
		// and the tags get parsed from it
		SetupFile(wxT("test.php"), t4p::IcuToWx(code));
		BuildCache(true);
		Lint.Init(TagCache);
		
		HasError = Lint.ParseString(code, Results);
	}

	void SetupFile(const wxString& fileName, const wxString& contents) {
		CreateFixtureFile(wxT("src") + wxFileName::GetPathSeparators() + fileName, contents);
	}

	void BuildCache(bool includeNativeFunctions = false) {
		
		// make the cache consume the source code file; to prime it with the resources because the
		// function call linter won't work without the cache
		soci::session* session = new soci::session(*soci::factory_sqlite3(), ":memory:");
		CreateDatabase(*session, t4p::ResourceSqlSchemaAsset()); 

		t4p::TagFinderListClass* tagFinderList = new t4p::TagFinderListClass;
		tagFinderList->AdoptGlobalTag(session, PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);
		if (includeNativeFunctions) {
			tagFinderList->InitNativeTag(t4p::NativeFunctionsAsset());
		}
		t4p::DirectorySearchClass search;
		search.Init(TestProjectDir + wxT("src"));
		while (search.More()) {
			tagFinderList->Walk(search);
		}
		TagCache.RegisterGlobal(tagFinderList);
		Lint.Init(TagCache);
	}
};

SUITE(PhpFunctionCallLintTestClass) {

TEST_FIXTURE(PhpFunctionCallLintTestFixtureClass, FunctionArgumentMatch) {
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc($a) {\n"
		"}\n"
		"myFunc('123');\n"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpFunctionCallLintTestFixtureClass, FunctionArgumentTooMany) {
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc($a) {\n"
		"}\n"
		"myFunc('123', 888);\n"
	);
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(1, Results);
	CHECK_EQUAL(t4p::PhpFunctionCallLintResultClass::TOO_MANY_ARGS, Results[0].Type);
	CHECK_EQUAL(1, Results[0].ExpectedCount);
	CHECK_EQUAL(2, Results[0].ActualCount);
}


TEST_FIXTURE(PhpFunctionCallLintTestFixtureClass, FunctionArgumentMissing) {
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc($a) {\n"
		"}\n"
		"myFunc();\n"
	);
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(1, Results);
	CHECK_EQUAL(t4p::PhpFunctionCallLintResultClass::TOO_FEW_ARGS, Results[0].Type);
	CHECK_UNISTR_EQUALS("myFunc", Results[0].Identifier);
	CHECK_EQUAL(1, Results[0].ExpectedCount);
	CHECK_EQUAL(0, Results[0].ActualCount);
}

TEST_FIXTURE(PhpFunctionCallLintTestFixtureClass, FunctionArgumentDefaultArgs) {
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc($a = 1) {\n"
		"}\n"
		"myFunc();\n"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpFunctionCallLintTestFixtureClass, FunctionArgumentMultipleDefaultArgs) {
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc($a, $b = 1, $b = 3) {\n"
		"}\n"
		"myFunc('324', '324');\n"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpFunctionCallLintTestFixtureClass, NativeFunctionArgumentMatch) {
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"strlen('123');\n"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpFunctionCallLintTestFixtureClass, NativeFunctionArgumentMatchMultiple) {
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"$items = array(1, 3, 5, 6);"
		"in_array('123', $items);\n"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpFunctionCallLintTestFixtureClass, NativeFunctionArgumentVariableArgs) {
	
	// since printf has variable args
	// and date have default args
	// then the linter should recognize that
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"printf('123 today is %s', date());\n"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpFunctionCallLintTestFixtureClass, MethodArgumentMatch) {
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"class MyClass {\n"
		"  function myFunc($a) {\n"
		"  }\n"
		"}\n"
		"$obj = new MyClass();\n"
		"$obj->myFunc('123');\n"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpFunctionCallLintTestFixtureClass, MethodArgumentMissing) {
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"class MyClass {\n"
		"  function myFunc($a, $b, $c) {\n"
		"  }\n"
		"}\n"
		"$obj = new MyClass();\n"
		"$obj->myFunc('123', '0000');\n"
	);
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(1, Results);
	CHECK_EQUAL(t4p::PhpFunctionCallLintResultClass::TOO_FEW_ARGS, Results[0].Type);
	CHECK_UNISTR_EQUALS("myFunc", Results[0].Identifier);
	CHECK_EQUAL(3, Results[0].ExpectedCount);
	CHECK_EQUAL(2, Results[0].ActualCount);
}

}
