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
#include <UnitTest++.h>
#include <FileTestFixtureClass.h>
#include <MvcEditorChecks.h>
#include <language/TagParserClass.h>
#include <search/TagFinderClass.h>
#include <globals/String.h>
#include <globals/Assets.h>
#include <globals/Sqlite.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <wx/filefn.h>
#include <wx/timer.h>
#include <wx/tokenzr.h>
#include <algorithm>

/**
 * This is a fixture to test that the tag finder works with
 * files.  We will use this sparingly since the parser (really the
 * LexicalAnalyzerClass) is the one responsible for tokenizing 
 * the input.
 * Using the file fixture less often means that tests run faster.
 */
class ParsedTagFinderFileTestClass : public FileTestFixtureClass {
public:	
	ParsedTagFinderFileTestClass() 
		: FileTestFixtureClass(wxT("tag_finder"))
		, TagParser()
		, ParsedTagFinder()
		, TestFile(wxT("test.php")) {
		Session.open(*soci::factory_sqlite3(), ":memory:");
		wxString error;
		if (!mvceditor::SqliteSqlScript(mvceditor::ResourceSqlSchemaAsset(), Session, error)) {
			wxASSERT_MSG(false, error);
		}
		TagParser.Init(&Session);
		TagParser.PhpFileExtensions.push_back(wxT("*.php"));
		if (wxDirExists(TestProjectDir)) {
			RecursiveRmDir(TestProjectDir);
		}
		ParsedTagFinder.Init(&Session);
	}
	
	/**
	 * creates a file that will contain the given contents.
	 * Then the tag finder is run on the new file
	 */
	void Prep(const wxString& source) {
		CreateFixtureFile(TestFile, source);
	}

	void CollectNearMatchResources(const UnicodeString& search, bool doCollectFileNames = false) {
		mvceditor::TagSearchClass tagSearch(search);
		Matches = ParsedTagFinder.CollectNearMatchResources(tagSearch, doCollectFileNames);
	}

	soci::session Session;
	mvceditor::TagParserClass TagParser;
	mvceditor::ParsedTagFinderClass ParsedTagFinder;
	wxString TestFile;
	std::vector<mvceditor::TagClass> Matches;
};

/**
 * Fixture that will be used for most tests.
 * Using a memory buffer for holding the code being parsed; this will
 * lead to the tests running faster since we dont have to repeatedly
 * create and delete actual files.
 */
class ParsedTagFinderMemoryTestClass {
public:	
	ParsedTagFinderMemoryTestClass() 
		: Session()
		, TagParser()
		, ParsedTagFinder()
		, TestFile(wxT("test.php"))
		, Matches() {
		Session.open(*soci::factory_sqlite3(), ":memory:");
		wxString error;
		if (!mvceditor::SqliteSqlScript(mvceditor::ResourceSqlSchemaAsset(), Session, error)) {
			wxASSERT_MSG(false, error);
		}
		TagParser.PhpFileExtensions.push_back(wxT("*.php"));
		TagParser.Init(&Session);
		ParsedTagFinder.Init(&Session);
	}
	
	/**
	 * will call the object under test (will make the tag finder 
	 * parse the given source code. this is a bit different than the
	 * ParsedTagFinderFileTestClass::Prep method
	 */
	void Prep(const UnicodeString& source) {
		TagParser.BuildResourceCacheForFile(TestFile, source, true);
	}

	void CollectNearMatchResources(const UnicodeString& search, bool doCollectFileNames = false) {
		mvceditor::TagSearchClass tagSearch(search);
		Matches = ParsedTagFinder.CollectNearMatchResources(tagSearch, doCollectFileNames);
	}

	void CollectNearMatchClassesOrFiles(const UnicodeString& search) {
		mvceditor::TagSearchClass tagSearch(search);
		Matches = ParsedTagFinder.CollectNearMatchClassesOrFiles(tagSearch);
	}

	soci::session Session;
	mvceditor::TagParserClass TagParser;
	mvceditor::ParsedTagFinderClass ParsedTagFinder;
	wxString TestFile;
	std::vector<mvceditor::TagClass> Matches;
};

class TagSearchTestClass {

public:

	mvceditor::TagSearchClass* TagSearch;

	TagSearchTestClass()
		: TagSearch(NULL) {
	
	}

	~TagSearchTestClass() {
		if (TagSearch) {
			delete TagSearch;
		}
	}

	void Make(const char* query) {
		TagSearch = new mvceditor::TagSearchClass(mvceditor::CharToIcu(query));
	}

};

#define CHECK_MEMBER_RESOURCE(className, identifier, tag) \
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(className), tag.ClassName);\
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(identifier), tag.Identifier);

#define CHECK_RESOURCE(identifier, tag) \
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(identifier), tag.Identifier);

#define CHECK_NAMESPACE_RESOURCE(namespaceName, identifier, tag) \
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(namespaceName), tag.NamespaceName);\
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(identifier), tag.Identifier);


SUITE(ParsedTagFinderTestClass) {

TEST_FIXTURE(ParsedTagFinderFileTestClass, CollectNearMatchResourcesShouldFindFileWhenFileNameMatches) {
	Prep(wxString::FromAscii(
		"<?php\n"
		"$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	TagParser.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources(mvceditor::WxToIcu(TestFile));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].GetFullPath());
}

TEST_FIXTURE(ParsedTagFinderFileTestClass, CollectNearMatchResourcesShouldFindFileWhenFileNameMatchesMiscFiles) {
	Prep(wxString::FromAscii(
		"<?php\n"
		"$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	TagParser.MiscFileExtensions.push_back(wxT("*.yml"));
	wxString miscFile = wxT("config.yml");
	CreateFixtureFile(miscFile, wxString::FromAscii(
		"app:\n"
		"  debug: true\n"
	));
	TagParser.Walk(TestProjectDir + TestFile);
	TagParser.Walk(TestProjectDir + miscFile);
	CollectNearMatchResources(mvceditor::WxToIcu(miscFile));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + miscFile, Matches[0].GetFullPath());
}

TEST_FIXTURE(ParsedTagFinderFileTestClass, CollectNearMatchResourcesShouldFindFileWhenFileNameIsASubset) {
	Prep(wxString::FromAscii(
		"<?php\n"
		"$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	TagParser.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("est.php"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].GetFullPath());
}

TEST_FIXTURE(ParsedTagFinderFileTestClass, CollectNearMatchResourcesShouldNotFindFileWhenFileNameMatchesButLineNumberIsTooBig) {
	Prep(wxString::FromAscii(
		"<?php\n"
		"$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	TagParser.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("test.php:100"));
	CHECK_VECTOR_SIZE(0, Matches);
	mvceditor::TagSearchClass tagSearch(UNICODE_STRING_SIMPLE("test.php:100"));
	CHECK_EQUAL(100, tagSearch.GetLineNumber());
}

TEST_FIXTURE(ParsedTagFinderFileTestClass, CollectNearMatchResourcesShouldFindFileWhenFileNameMatchesAndFileHasDifferentLineEndings) {
	
	// should count unix, windows & mac line endings
	Prep(wxString::FromAscii(
		"<?php\n"
		"\t\r\n"
		"\t\r"
		"\t$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	TagParser.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("test.php:6"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].GetFullPath());
}

TEST_FIXTURE(ParsedTagFinderFileTestClass, CollectNearMatchResourcesShouldFindFileWhenFileNameDoesNotMatchCase) {
	TestFile = wxT("TeST.php");
	Prep(wxString::FromAscii(
		"<?php\n"
		"\t\r\n"
		"\t\r"
		"\t$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	TagParser.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("test.php:6"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].GetFullPath());
}

TEST_FIXTURE(ParsedTagFinderFileTestClass, CollectNearMatchResourcesShouldFindFileWhenFileNameSearchDoesNotMatchCase) {
	TestFile = wxT("test.php");
	Prep(wxString::FromAscii(
		"<?php\n"
		"\t\r\n"
		"\t\r"
		"\t$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	TagParser.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("TEST.php"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].GetFullPath());
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFileWhenClassNameMatches) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"/** this is my class */\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].GetFullPath());
	mvceditor::TagClass tag = Matches[0];
	CHECK_NAMESPACE_RESOURCE("\\", "UserClass", tag);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), tag.ReturnType);
	CHECK_UNISTR_EQUALS("class UserClass", tag.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** this is my class */"), tag.Comment);
	CHECK_EQUAL(mvceditor::TagClass::CLASS, tag.Type);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldNotFindFileWhenClassNameDoesNotMatch) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("BlogPostClass"));
	CHECK_VECTOR_SIZE(0, Matches);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldUseFileSearchWhenResourceIsNotFound) {
	TestFile = wxT("UserClass.php");
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class AdminClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("user"), true);
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].GetFullPath());
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFileWhenClassNameIsNotTheExactSame) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserAdmin {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("User"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].GetFullPath());
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFileWhenClassNameHasAnExtends) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class User extends Human {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("User"));
	CHECK_VECTOR_SIZE(1, Matches);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFileWhenClassNameAndMethodNameMatch) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;\n"
		"\t/** returns the name @return string */\n"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass::getName"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].GetFullPath());
	mvceditor::TagClass tag = Matches[0];
	CHECK_MEMBER_RESOURCE("UserClass", "getName", tag);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\"), tag.NamespaceName);
	CHECK_UNISTR_EQUALS("string", tag.ReturnType);
	CHECK_UNISTR_EQUALS("string public function getName()", tag.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** returns the name @return string */"), tag.Comment);
	CHECK_EQUAL(mvceditor::TagClass::METHOD, tag.Type);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFileWhenClassNameAndSecondMethodNameMatch) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tpublic function getFullName() {\n"
		"\t\treturn $this->name . ' Last Name';\n"
		"\t}\n"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass::getName"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].GetFullPath());
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldNotFindFileWhenClassNameMatchesButMethodNameDoesNot) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass::getAddress"));
	CHECK_VECTOR_SIZE(0, Matches);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFileWhendMethodNameMatchesButClassIsNotGiven) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("::getName"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].GetFullPath());
	CHECK_MEMBER_RESOURCE("UserClass", "getName", Matches[0]);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldOnlySaveTheExactMatchWhenAnExactMatchIsFound) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"class UserClassAdmin extends UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}?>\n"
	));	
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].GetFullPath());
	CHECK_UNISTR_EQUALS("UserClass", Matches[0].Identifier);
	CHECK_EQUAL(false, Matches[0].IsNative);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldFindTwoFilesWhenClassNameMatches) {
	wxString testFile = wxT("test.php");
	wxString testFile2 = wxT("test2.php");
	TestFile = testFile;
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	TestFile = testFile2;
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass"));
	CHECK_VECTOR_SIZE(2, Matches);
	CHECK_EQUAL(testFile, Matches[0].GetFullPath());
	CHECK_EQUAL(testFile2, Matches[1].GetFullPath());
}

TEST_FIXTURE(ParsedTagFinderFileTestClass, CollectNearMatchResourcesShouldNotFindFileWhenItHasBeenModified) {
	Prep(wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	TagParser.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].GetFullPath());
	wxSleep(1);
	Prep(wxString::FromAscii(
		"<?php\n"
		"class AdminClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));
	TagParser.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass"));
	CHECK_VECTOR_SIZE(0, Matches);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldFindClassAfterFindingFile) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
	));	
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("test.php"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].GetFullPath());
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].GetFullPath());
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("test.php"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].GetFullPath());
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].GetFullPath());
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFunctionWhenFunctionNameMatches) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"/** print a user @return void */\n"
		"function printUser($user) {\n"
		"\t echo $user->getName() . \"\\n\";"
		"}\n"
		"?>\n"
	));	
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("printUser"));
	CHECK_VECTOR_SIZE(1, Matches);
	mvceditor::TagClass tag = Matches[0];
	CHECK_UNISTR_EQUALS("printUser", tag.Identifier);
	CHECK_UNISTR_EQUALS("void", tag.ReturnType);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("function printUser($user)"), tag.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** print a user @return void */"), tag.Comment);
	CHECK_EQUAL(mvceditor::TagClass::FUNCTION, tag.Type);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldFindMatchesForClassesAndFunctions) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"class AdminClass extends UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"function userClassPrint($user) {\n"
		"\t echo $user->getName() . \"\\n\";"
		"}\n"		
		"?>\n"
	));	
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("userClas"));
	CHECK_VECTOR_SIZE(2, Matches);
	CHECK_UNISTR_EQUALS("UserClass", Matches[0].Identifier);
	CHECK_UNISTR_EQUALS("userClassPrint", Matches[1].Identifier);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldFindMatchesForClassMembers) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\t/** the user name @var string */\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass::name"));
	CHECK_VECTOR_SIZE(1, Matches);
	mvceditor::TagClass tag = Matches[0];
	CHECK_MEMBER_RESOURCE("UserClass", "name", tag);
	CHECK_UNISTR_EQUALS("string", tag.ReturnType);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::name"), tag.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** the user name @var string */"), tag.Comment);
	CHECK_EQUAL(mvceditor::TagClass::MEMBER, tag.Type);
}


TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldFindMatchesForClassConstant) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\t/** the max constant @var int */\n"
		"\tconst MAX = 1;\n"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass::MAX"));
	CHECK_VECTOR_SIZE(1, Matches);
	mvceditor::TagClass tag = Matches[0];
	CHECK_MEMBER_RESOURCE("UserClass", "MAX", tag);
	CHECK_UNISTR_EQUALS("int", tag.ReturnType);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::MAX"), tag.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** the max constant @var int */"), tag.Comment);
	CHECK_EQUAL(mvceditor::TagClass::CLASS_CONSTANT, tag.Type);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldFindMatchesForDefines) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"/** the max constant @var int */\n"
		"define('MAX_ITEMS', 1);\n"
		"?>\n"
	));	
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("MAX_ITEMS"));
	CHECK_VECTOR_SIZE(1, Matches);
	mvceditor::TagClass tag = Matches[0];
	CHECK_UNISTR_EQUALS("MAX_ITEMS", tag.Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), tag.ReturnType);
	CHECK_UNISTR_EQUALS("1", tag.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** the max constant @var int */"), tag.Comment);
	CHECK_EQUAL(mvceditor::TagClass::DEFINE, tag.Type);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldFindMatchesForCorrectClassMethod) {
	
	// adding 2 classes to the file because we want to test that the code can differentiate the two classes and
	// match only on the class given
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction get() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"class SetClass {\n"
		"\tprivate $users;"
		"\tfunction get() {\n"
		"\t\treturn $this->users;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass::get"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("UserClass", "get", Matches[0]);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldFindPartialMatchesForCorrectClassMethod) {
	
	// adding 2 classes to the file because we want to test that the code can differentiate the two classes and
	// match only on the class given
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"class SetClass {\n"
		"\tprivate $users;"
		"\tfunction get() {\n"
		"\t\treturn $this->users;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass::get"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("UserClass", "getName", Matches[0]);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldFindPartialMatchesForClassMethodsWithNoClassName) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"class UserClassSet extends UserClass {\n"
		"\tprivate $users;"
		"\tfunction next() {\n"
		"\t\treturn next($this->users);\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("::getNa"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("UserClass", "getName", Matches[0]);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldFindMatchesForNativeFunctions) {
	soci::session session(*soci::factory_sqlite3(), mvceditor::WxToChar(mvceditor::NativeFunctionsAsset().GetFullPath()));
	ParsedTagFinder.Init(&session);

	CollectNearMatchResources(UNICODE_STRING_SIMPLE("array_key"));
	CHECK_VECTOR_SIZE(2, Matches);
	CHECK_UNISTR_EQUALS("array_key_exists", Matches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("bool function array_key_exists($key, $search)"), Matches[0].Signature);
	CHECK_UNISTR_EQUALS("bool", Matches[0].ReturnType);
	CHECK_UNISTR_EQUALS("array_keys", Matches[1].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("array function array_keys($input, $search_value, $strict = false)"), Matches[1].Signature);
	CHECK_UNISTR_EQUALS("array", Matches[1].ReturnType);
	CHECK(Matches[0].IsNative);
	CHECK(Matches[1].IsNative);

	// test a built-in object
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("pdo::que"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("query", Matches[0].Identifier);
	CHECK_UNISTR_EQUALS("PDO", Matches[0].ClassName);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("PDOStatement function query($statement, $PDO::FETCH_COLUMN, $colno, $PDO::FETCH_CLASS, $classname, $ctorargs, $PDO::FETCH_INTO, $object)"), Matches[0].Signature);
	CHECK_UNISTR_EQUALS("PDOStatement", Matches[0].ReturnType);
	CHECK(Matches[0].IsNative);

	// test a built-in object query for all methods
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("PDO::"));
	CHECK_VECTOR_SIZE(91, Matches);
	
	// a fully qualified search
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("\\Exception"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("\\Exception", Matches[0].Identifier);
	CHECK_UNISTR_EQUALS("Exception", Matches[0].ClassName);
}

TEST_FIXTURE(ParsedTagFinderFileTestClass, CollectNearMatchResourcesShouldFindMatchesWhenUsingBuildResourceCacheForFile) {
	
	// write a file, "modify" the file in memory by adding a method, we should find the new method
	wxString code  = wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"function printUser($user) {\n"
		"\t echo $user->getName() . \"\\n\";"
		"}\n"
		"?>\n"
	);
	Prep(code);
	UnicodeString uniCode  = mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"function printUser($user) {\n"
		"\t echo $user->getName() . \"\\n\";"
		"}\n"
		"function printUserList($userList) {\n"
		"\t array_map($userList, create_function('$user', 'echo $user->getName()')); \n"
		"\t echo \"\\n\";\n"
		"}\n"
		"?>\n"
	);
	TagParser.Walk(TestProjectDir + TestFile);
	TagParser.BuildResourceCacheForFile(TestProjectDir + TestFile, uniCode, false);
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("printUse"));
	CHECK_VECTOR_SIZE(2, Matches);
	CHECK_UNISTR_EQUALS("printUser", Matches[0].Identifier);
	CHECK_UNISTR_EQUALS("printUserList", Matches[1].Identifier);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldFindMatchesWhenUsingBuildResourceCacheForFileAndUsingNewFile) {
	Prep(mvceditor::CharToIcu(
		
		// this simulates the user writing a completely new file that has yet to be saved.
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"class AdminClass extends UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"function userClassPrint($user) {\n"
		"\t echo $user->getName() . \"\\n\";"
		"}\n"		
		"?>\n"
	));	
	UnicodeString uniCode  = mvceditor::CharToIcu(
		"<?php\n"
		"function printUser($user) {\n"
		"\t echo $user->getName() . \"\\n\";"
		"}\n"
		"$user = new UserClass();\n"
	);
	wxString fileName = wxT("Untitled");
	TagParser.BuildResourceCacheForFile(fileName, uniCode, true);
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("printUse"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("printUser", Matches[0].Identifier);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldCollectAllMethodsWhenClassIsNotGiven) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;\n"
		"\tprivate $address;\n"
		"\tfunction userName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"\tprivate function clearName() {\n"
		"\t}\n"
		"}\n"
		"class SuperUserClass {\n"
		"\tfunction userDisable(User $user) {\n"
		"\t}\n"
		"}\n"
		"class AdminClass {\n"
		"\tfunction userDelete(User $user) {\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("::user"));
	CHECK_VECTOR_SIZE(3, Matches);
	CHECK_MEMBER_RESOURCE("AdminClass", "userDelete", Matches[0]);
	CHECK_MEMBER_RESOURCE("SuperUserClass", "userDisable", Matches[1]);
	CHECK_MEMBER_RESOURCE("UserClass", "userName", Matches[2]);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchResourcesShouldNotCollectParentClassesWhenInheritedClassNameIsGiven) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;\n"
		"\tprivate $address;\n"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"\tprivate function clearName() {\n"
		"\t}\n"
		"}\n"
		"class SuperUserClass extends UserClass {\n"
		"\tfunction disableUser(User $user) {\n"
		"\t}\n"
		"}\n"
		"class AdminClass extends SuperUserClass {\n"
		"\tfunction deleteUser(User $user) {\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("AdminClass"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("AdminClass", Matches[0].Identifier);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchClassesOrFilesShouldCollectBoth) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;\n"
		"\tprivate $address;\n"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"\tprivate function clearName() {\n"
		"\t}\n"
		"}\n"
		"class SuperUserClass extends UserClass {\n"
		"\tfunction disableUser(User $user) {\n"
		"\t}\n"
		"}\n"
		"class UserAdminClass extends SuperUserClass {\n"
		"\tfunction deleteUser(User $user) {\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));
	CollectNearMatchClassesOrFiles(UNICODE_STRING_SIMPLE("user"));
	CHECK_VECTOR_SIZE(2, Matches);
	CHECK_UNISTR_EQUALS("UserAdminClass", Matches[0].Identifier);
	CHECK_UNISTR_EQUALS("UserClass", Matches[1].Identifier);

	CollectNearMatchClassesOrFiles(UNICODE_STRING_SIMPLE("test.php"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(wxT("test.php"), Matches[0].GetFullPath());
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectFullyQualifiedResourcesShouldFindFileWhenClassNameMatches) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
	));	
	mvceditor::TagSearchClass tagSearch(UNICODE_STRING_SIMPLE("UserClass"));
	Matches = ParsedTagFinder.CollectFullyQualifiedResource(tagSearch);
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].GetFullPath());
	CHECK_EQUAL(TestFile, Matches[0].GetFullPath());
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectFullyQualifiedResourcesShouldFindFileWhenClassAndPropertyNameAreTheSame) {
		
	// the name of the class and the name of the property are the same
	// the method under test should know the difference and only return the class tag
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $SetClass;\n"
		"\tprivate $name;\n"
		"\tfunction get() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"class SetClass {\n"
		"\tprivate $users;"
		"\tfunction get() {\n"
		"\t\treturn $this->users;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	mvceditor::TagSearchClass tagSearch(UNICODE_STRING_SIMPLE("SetClass"));
	Matches = ParsedTagFinder.CollectFullyQualifiedResource(tagSearch);
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("SetClass", Matches[0].Identifier);
	CHECK_UNISTR_EQUALS("SetClass", Matches[0].ClassName);
	CHECK_EQUAL(mvceditor::TagClass::CLASS, Matches[0].Type);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectFullyQualifiedResourcesShouldFindMatchesForCorrectClassMethod) {
	
	// adding 2 classes to the file because we want to test that the code can differentiate the two classes and
	// match only on the class given
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction get() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"class SetClass {\n"
		"\tprivate $users;"
		"\tfunction get() {\n"
		"\t\treturn $this->users;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	mvceditor::TagSearchClass tagSearch(UNICODE_STRING_SIMPLE("UserClass::get"));
	Matches = ParsedTagFinder.CollectFullyQualifiedResource(tagSearch);
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("UserClass", "get", Matches[0]);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectFullyQualifiedResourcesShouldNotFindFileWhenClassNameDoesNotMatch) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
	));	
	mvceditor::TagSearchClass tagSearch(UNICODE_STRING_SIMPLE("User"));
	Matches = ParsedTagFinder.CollectFullyQualifiedResource(tagSearch);
	CHECK_VECTOR_SIZE(0, Matches);
}

TEST_FIXTURE(ParsedTagFinderFileTestClass, CollectFullyQualifiedResourcesShouldFindClassWhenFileHasBeenModified) {
	
	// this method is testing the scenario where the tag cache is modified and when the CollectFullyQualifiedResource
	// method is checked that the cache is re-sorted and searched correctly
	Prep(wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	TagParser.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].GetFullPath());
	wxSleep(1);
	Prep(wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"class AdminClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));
	TagParser.Walk(TestProjectDir + TestFile);
	mvceditor::TagSearchClass tagSearch(UNICODE_STRING_SIMPLE("AdminClass"));
	Matches = ParsedTagFinder.CollectFullyQualifiedResource(tagSearch);
	CHECK_VECTOR_SIZE(1, Matches);

	// make sure that file lookups work as well
	mvceditor::TagSearchClass tagFileSearch(UNICODE_STRING_SIMPLE("test.php"));
	Matches = ParsedTagFinder.CollectNearMatchResources(tagFileSearch);
	CHECK_VECTOR_SIZE(1, Matches);
}

TEST_FIXTURE(ParsedTagFinderFileTestClass, CollectFullyQualifiedResourcesShouldFindClassWhenFileHasBeenDeleted) {
	
	// this method is testing the scenario where the tag cache invalidates matches when files have been deleted
	Prep(wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	mvceditor::TagSearchClass tagSearch(UNICODE_STRING_SIMPLE("UserClass"));
	TagParser.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].GetFullPath());
	CHECK(wxRemoveFile(TestProjectDir + TestFile));
	Matches = ParsedTagFinder.CollectFullyQualifiedResource(tagSearch);

	// before this was expected to be zero, but the code that ensures matched files
	// exist is gone from the CollectNearMatchResources method because of performance
	// issues and we want the method to be fast because it is performed during 
	// user clicks
	CHECK_VECTOR_SIZE(1, Matches);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectFullyQualifiedResourceShouldReturnSignatureForConstructors) {
	 Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"\tfunction __construct($name) {\n"
		"\t\t$this->name = 'guest';\n"
		"\t\tif ($name) \n"
		"\t\t\t$this->name = 'guest';\n"
		"\t}\n"
		"}\n"
		"class AdminClass extends UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));
	mvceditor::TagSearchClass tagSearch(UNICODE_STRING_SIMPLE("UserClass::__construct"));
	Matches = ParsedTagFinder.CollectFullyQualifiedResource(tagSearch);
	mvceditor::TagClass tag = Matches[0];
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("public function __construct($name)"), tag.Signature);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectFullyQualifiedResourceShouldReturnInheritedMember) {
	 Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprotected $name;\n"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"\tfunction __construct($name) {\n"
		"\t\t$this->name = 'guest';\n"
		"\t\tif ($name) \n"
		"\t\t\t$this->name = 'guest';\n"
		"\t}\n"
		"}\n"
		"class AdminClass extends UserClass {\n"
		"}\n"
		"?>\n"
	));
	mvceditor::TagSearchClass tagSearch(UNICODE_STRING_SIMPLE("AdminClass::name"));
	std::vector<UnicodeString> parents;
	parents.push_back(UNICODE_STRING_SIMPLE("UserClass"));
	tagSearch.SetParentClasses(parents);
	Matches = ParsedTagFinder.CollectFullyQualifiedResource(tagSearch);
	CHECK_VECTOR_SIZE(1, Matches);
	mvceditor::TagClass tag = Matches[0];
	CHECK_UNISTR_EQUALS("UserClass", tag.ClassName);
	CHECK_UNISTR_EQUALS("name", tag.Identifier);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectFullyQualifiedClassOrFile) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
	));	
	mvceditor::TagSearchClass tagSearch(UNICODE_STRING_SIMPLE("UserClass"));
	Matches = ParsedTagFinder.CollectFullyQualifiedClassOrFile(tagSearch);
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("UserClass", Matches[0].Identifier);

	CollectNearMatchClassesOrFiles(UNICODE_STRING_SIMPLE("test.php"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(wxT("test.php"), Matches[0].GetFullPath());
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, GetResourceParentClassShouldReturnParentClass) {
	 Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"/** The admin class */\n"
		"class AdminClass extends UserClass {\n"
		"}\n"
		"function userClassPrint($user) {\n"
		"\t echo $user->getName() . \"\\n\";"
		"}\n"
		"?>\n"
	));
	CHECK_UNISTR_EQUALS("UserClass", ParsedTagFinder.GetResourceParentClassName(UNICODE_STRING_SIMPLE("AdminClass")));
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, GetResourceParentClassShouldReturnParentClassForDeepHierarchy) {
	 Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"/** The admin class */\n"
		"class AdminClass extends UserClass {\n"
		"}\n"
		"class SuperAdminClass extends AdminClass {\n"
		"}\n"
		"?>\n"
	));
	CHECK_UNISTR_EQUALS("AdminClass", ParsedTagFinder.GetResourceParentClassName(UNICODE_STRING_SIMPLE("SuperAdminClass")));
	CHECK_UNISTR_EQUALS("UserClass", ParsedTagFinder.GetResourceParentClassName(UNICODE_STRING_SIMPLE("AdminClass")));
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, GetResourceMatchPositionShouldReturnValidPositionsForClassMethodFunctionAndMember) {
	 UnicodeString icuCode = mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"function printUser($user) {\n"
		"}\n"
		"?>\n"
	);
	Prep(icuCode);
	int32_t pos, 
		length;
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("UserClass", Matches[0].Identifier);
	CHECK(mvceditor::ParsedTagFinderClass::GetResourceMatchPosition(Matches[0], icuCode, pos, length));
	CHECK_EQUAL(6, pos);
	CHECK_EQUAL(16, length);
	
	// checking methods
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("::getName"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("UserClass", "getName", Matches[0]);
	CHECK(mvceditor::ParsedTagFinderClass::GetResourceMatchPosition(Matches[0], icuCode, pos, length));
	CHECK_EQUAL(icuCode.indexOf(UNICODE_STRING_SIMPLE("function getName()")), (int32_t)pos);
	CHECK_EQUAL(17, length);
	
	// checking properties
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass::name"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("UserClass", "name", Matches[0]);
	CHECK(mvceditor::ParsedTagFinderClass::GetResourceMatchPosition(Matches[0], icuCode, pos, length));
	CHECK_EQUAL(icuCode.indexOf(UNICODE_STRING_SIMPLE("private $name")), (int32_t)pos);
	CHECK_EQUAL(14, length);
	
	// checking functions
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("printUser"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("printUser", Matches[0].Identifier);
	CHECK(mvceditor::ParsedTagFinderClass::GetResourceMatchPosition(Matches[0], icuCode, pos, length));
	CHECK_EQUAL(icuCode.indexOf(UNICODE_STRING_SIMPLE("function printUser")), (int32_t)pos);
	CHECK_EQUAL(19, length);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectQualifiedResourceNamespaces) {
	 Prep(mvceditor::CharToIcu(
		"<?php\n"
		"namespace First\\Child; \n"
		"class MyClass {\n"
		"	function work() {} \n"
		"}\n"
		"\n"
		"function singleWork() { } \n"
		"?>\n"
	));
	mvceditor::TagSearchClass tagSearch(UNICODE_STRING_SIMPLE("\\First\\Child\\MyClass"));
	Matches = ParsedTagFinder.CollectFullyQualifiedResource(tagSearch);
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\MyClass"), Matches[0].Identifier);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectQualifiedResourceNamespacesShouldNotFindDuplicates) {
	 Prep(mvceditor::CharToIcu(
		"<?php\n"
		"namespace First\\Child; \n"
		"class MyClass {\n"
		"	function work() {} \n"
		"}\n"
		"\n"
		"function singleWork() { } \n"
		"?>\n"
	));
	TestFile = wxT("test_2.php");
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"namespace First\\Child; \n"
		"class TwoClass {\n"
		"	function twoWork() {} \n"
		"}\n"
		"\n"
		"?>\n"
	));	
	mvceditor::TagSearchClass tagSearch(UNICODE_STRING_SIMPLE("\\First\\Child\\MyClass"));
	Matches = ParsedTagFinder.CollectFullyQualifiedResource(tagSearch);
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\MyClass"), Matches[0].Identifier);

	std::vector<mvceditor::TagClass> all = ParsedTagFinder.All();

	//  6 tags total
	// namespace First\\Child 
	// class MyClass
	// function MyClass::work
	// function singleWork
	// class TwoClass
	// function TwoClass::twoWork
	CHECK_VECTOR_SIZE(6, all);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectResourceInGlobalNamespaces) {
	 Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class MyClass {\n"
		"	function work() {} \n"
		"}\n"
		"\n"
		"function singleWork() { } \n"
		"?>\n"
	));
	 mvceditor::TagSearchClass tagSearch(UNICODE_STRING_SIMPLE("\\MyClass"));
	Matches = ParsedTagFinder.CollectFullyQualifiedResource(tagSearch);
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("\\MyClass", Matches[0].Identifier);
	
	mvceditor::TagSearchClass tagSearchFunction(UNICODE_STRING_SIMPLE("\\singleWork"));
	Matches = ParsedTagFinder.CollectFullyQualifiedResource(tagSearchFunction);
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("\\singleWork", Matches[0].Identifier);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchNamespaces) {
	 Prep(mvceditor::CharToIcu(
		"<?php\n"
		"namespace First\\Child; \n"
		"class MyClass {\n"
		"	function work() {} \n"
		"}\n"
		"\n"
		"function singleWork() { } \n"
		"?>\n"
	));
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("\\First"));
	CHECK_VECTOR_SIZE(3, Matches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child"), Matches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\MyClass"), Matches[1].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\singleWork"), Matches[2].Identifier);
	
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("\\First\\Ch"));
	CHECK_VECTOR_SIZE(3, Matches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child"), Matches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\MyClass"), Matches[1].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\singleWork"), Matches[2].Identifier);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchNamespaceQualifiedClassesAndFunctions) {
	 Prep(mvceditor::CharToIcu(
		"<?php\n"
		"namespace First\\Child; \n"
		"class MyClass {\n"
		"	function work() {} \n"
		"}\n"
		"\n"
		"function singleWork() { } \n"
		"?>\n"
	));
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("\\First\\Child\\si"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\singleWork"), Matches[0].Identifier);
	
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("\\First\\Child\\M"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\MyClass"), Matches[0].Identifier);
	
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("\\First\\Child\\"));
	CHECK_VECTOR_SIZE(2, Matches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\MyClass"), Matches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\singleWork"), Matches[1].Identifier);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchNamespaceQualifiedClassesShouldIgnoreOtherNamespaces) {
	 Prep(mvceditor::CharToIcu(
		"<?php\n"
		"namespace First\\Child { \n"
		"class MyClass { }\n"
		"}\n"
		"namespace Second\\Child { \n"
		"class MyClass { }\n"
		"}\n"
		"?>\n"
	));
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("\\Second\\Child\\My"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\Second\\Child\\MyClass"), Matches[0].Identifier);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchesShouldFindTraitMembers) {
	TagParser.SetVersion(pelet::PHP_54);
	Prep(mvceditor::CharToIcu(
		"trait ezcReflectionReturnInfo { "
		"    function getReturnType() { } "
		"} "
		" "
		"class ezcReflectionMethod { "
		"    use ezcReflectionReturnInfo;"
		"    /* ... */ "
		"}"
	));
	
	mvceditor::TagSearchClass tagSearch(UNICODE_STRING_SIMPLE("ezcReflectionMethod::getReturn"));
	
	// tell the tag finder to look for traits
	std::vector<UnicodeString> traits;
	traits.push_back(UNICODE_STRING_SIMPLE("ezcReflectionReturnInfo"));
	tagSearch.SetTraits(traits);

	Matches = ParsedTagFinder.CollectNearMatchResources(tagSearch);

	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("ezcReflectionReturnInfo", "getReturnType", Matches[0]);
	CHECK_UNISTR_EQUALS("getReturnType", Matches[0].Identifier);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, CollectNearMatchesShouldFindTraitsWhenLookingForAllMethods) {
	TagParser.SetVersion(pelet::PHP_54); 
	Prep(mvceditor::CharToIcu(
		"trait ezcReflectionReturnInfo { "
		"    function getReturnType() { } "
		"} "
		"trait ezcReflectionFunctionInfo { "
		"    function getReturnType() {  } "
		"} "
		" "
		"class ezcReflectionMethod { "
		"    use ezcReflectionReturnInfo, ezcReflectionFunctionInfo { "
		"		ezcReflectionReturnInfo::getReturnType as getFunctionReturnType; "
		"	 }"
		"}"
	));
	mvceditor::TagSearchClass tagSearch(UNICODE_STRING_SIMPLE("ezcReflectionMethod::"));
	
	// tell the tag finder to look for traits
	std::vector<UnicodeString> traits;
	traits.push_back(UNICODE_STRING_SIMPLE("ezcReflectionReturnInfo"));
	traits.push_back(UNICODE_STRING_SIMPLE("ezcReflectionFunctionInfo"));
	tagSearch.SetTraits(traits);

	Matches = ParsedTagFinder.CollectNearMatchResources(tagSearch);
	
	// for now just show both the aliased and original methods
	CHECK_VECTOR_SIZE(3, Matches);
	CHECK_MEMBER_RESOURCE("ezcReflectionReturnInfo", "getFunctionReturnType", Matches[0]);
	CHECK_UNISTR_EQUALS("getFunctionReturnType", Matches[0].Identifier);
	CHECK_MEMBER_RESOURCE("ezcReflectionFunctionInfo", "getReturnType", Matches[1]);
	CHECK_UNISTR_EQUALS("getReturnType", Matches[1].Identifier);
	CHECK_MEMBER_RESOURCE("ezcReflectionReturnInfo", "getReturnType", Matches[2]);
	CHECK_UNISTR_EQUALS("getReturnType", Matches[2].Identifier);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, GetResourceTraitsShouldReturnAllTraits) {
	TagParser.SetVersion(pelet::PHP_54); 
	Prep(mvceditor::CharToIcu(
		"trait ezcReflectionReturnInfo { "
		"    function getReturnType() { } "
		"} "
		"trait ezcReflectionFunctionInfo { "
		"    function getReturnType() {  } "
		"} "
		" "
		"class ezcReflectionMethod { "
		"    use ezcReflectionReturnInfo, ezcReflectionFunctionInfo { "
		"		ezcReflectionReturnInfo::getReturnType as getFunctionReturnType; "
		"	 }"
		"}"
	));
	std::vector<UnicodeString> traits = ParsedTagFinder.GetResourceTraits(
		UNICODE_STRING_SIMPLE("ezcReflectionMethod"), UNICODE_STRING_SIMPLE(""));
	CHECK_VECTOR_SIZE(2, traits);

	// because traits are not guaranteed to be in order
	std::sort(traits.begin(), traits.end());
	CHECK_UNISTR_EQUALS("ezcReflectionFunctionInfo", traits[0]);
	CHECK_UNISTR_EQUALS("ezcReflectionReturnInfo", traits[1]);
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, IsFileCacheEmptyWithNativeFunctions) {
	CHECK(ParsedTagFinder.IsFileCacheEmpty());
	CHECK(ParsedTagFinder.IsResourceCacheEmpty());

	soci::session session(*soci::factory_sqlite3(), mvceditor::WxToChar(mvceditor::NativeFunctionsAsset().GetFullPath()));;
	ParsedTagFinder.Init(&session);
	
	// still empty
	CHECK(ParsedTagFinder.IsFileCacheEmpty());
	CHECK(ParsedTagFinder.IsResourceCacheEmpty());
}

TEST_FIXTURE(ParsedTagFinderMemoryTestClass, AllTagsInFileShouldReturnTags) {

	// create 2 files that way we can test that each file will only get its own
	// tags
	TestFile = wxT("test.php");
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"define('USER_MAX', 2000);\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"function printUsers($user) {}\n"
		"\n"
		"?>\n"
	));	
	
	TestFile = wxT("tan.php");
		Prep(mvceditor::CharToIcu(
		"<?php\n"
		"define('TAN_MAX', 2000);\n"
		"class TanClass {\n"
		"\tprivate $score;"
		"\tfunction getScore() {\n"
		"\t\treturn $this->score;\n"
		"\t}\n"
		"}\n"
		"function printScore($tan) {}\n"
		"\n"
		"?>\n"
	));	

	// this method only returns top-level tags for now
	Matches = ParsedTagFinder.AllTagsInFile(wxT("test.php"));
	CHECK_VECTOR_SIZE(3, Matches);
	CHECK_UNISTR_EQUALS("printUsers", Matches[0].Key);
	CHECK_UNISTR_EQUALS("USER_MAX", Matches[1].Key);
	CHECK_UNISTR_EQUALS("UserClass", Matches[2].Key);
}

TEST_FIXTURE(ParsedTagFinderFileTestClass, IsFileCacheEmptyWithAnotherFile) {
	CHECK(ParsedTagFinder.IsFileCacheEmpty());
	CHECK(ParsedTagFinder.IsResourceCacheEmpty());

	Prep(wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	TagParser.Walk(TestProjectDir + TestFile);

	CHECK_EQUAL(false, ParsedTagFinder.IsFileCacheEmpty());
	CHECK_EQUAL(false, ParsedTagFinder.IsResourceCacheEmpty());
}

TEST_FIXTURE(ParsedTagFinderFileTestClass, PhpFileExtensionsShouldWorkWithNoWildcards) {

	// create two files, a good.php and a bad.php. Set the filter to only
	// look for good.php.  When waliking over bad.php, it should be skipped
	wxString goodFile = wxT("good.php");
	CreateFixtureFile(goodFile, wxString::FromAscii(
		"<?php\n"
		"class GoodClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	wxString badFile = wxT("bad.php");
	CreateFixtureFile(badFile, wxString::FromAscii(
		"<?php\n"
		"class BadClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	TagParser.PhpFileExtensions.clear();
	TagParser.PhpFileExtensions.push_back(wxT("good.php"));
	
	TagParser.Walk(TestProjectDir + goodFile);
	TagParser.Walk(TestProjectDir + badFile);

	CollectNearMatchResources(UNICODE_STRING_SIMPLE("good.php"));
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("bad.php"));
}

TEST_FIXTURE(TagSearchTestClass, ShouldParseClassAndMethod) {
	Make("News_model::get_news");
	CHECK_UNISTR_EQUALS("News_model", TagSearch->GetClassName());
	CHECK_UNISTR_EQUALS("get_news", TagSearch->GetMethodName());
}

TEST_FIXTURE(TagSearchTestClass, ShouldParseFileName) {
	Make("class.User.php");
	CHECK_UNISTR_EQUALS("class.User.php", TagSearch->GetFileName());
}

TEST_FIXTURE(TagSearchTestClass, ShouldParseFileNameAndLineNumber) {
	Make("class.User.php:100");
	CHECK_UNISTR_EQUALS("class.User.php", TagSearch->GetFileName());
	CHECK_EQUAL(100, TagSearch->GetLineNumber());
}

}