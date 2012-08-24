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
#include <search/ResourceFinderClass.h>
#include <MvcEditorString.h>
#include <FileTestFixtureClass.h>
#include <MvcEditorAssets.h>
#include <MvcEditorChecks.h>
#include <wx/filefn.h>
#include <wx/timer.h>
#include <wx/tokenzr.h>
#include <algorithm>

/**
 * This is a fixture to test that the resource finder works with
 * files.  We will use this sparingly since the parser (really the
 * LexicalAnalyzerClass) is the one responsible for tokenizing 
 * the input.
 * Using the file fixture less often means that tests run faster.
 */
class ResourceFinderFileTestClass : public FileTestFixtureClass {
public:	
	ResourceFinderFileTestClass() 
		: FileTestFixtureClass(wxT("resource_finder"))
		, ResourceFinder()
		, TestFile(wxT("test.php")) {
		ResourceFinder.PhpFileExtensions.push_back(wxT("*.php"));
		if (wxDirExists(TestProjectDir)) {
			RecursiveRmDir(TestProjectDir);
		}
		ResourceFinder.InitMemory();
	}
	
	/**
	 * creates a file that will contain the given contents.
	 * Then the resource finder is run on the new file
	 */
	void Prep(const wxString& source) {
		CreateFixtureFile(TestFile, source);
	}

	void CollectNearMatchResources(const UnicodeString& search) {
		mvceditor::ResourceSearchClass resourceSearch(search);
		Matches = ResourceFinder.CollectNearMatchResources(resourceSearch);
	}

	mvceditor::ResourceFinderClass ResourceFinder;
	wxString TestFile;
	std::vector<mvceditor::ResourceClass> Matches;
};

/**
 * Fixture that will be used for most tests.
 * Using a memory buffer for holding the code being parsed; this will
 * lead to the tests running faster since we dont have to repeatedly
 * create and delete actual files.
 */
class ResourceFinderMemoryTestClass {
public:	
	ResourceFinderMemoryTestClass() 
		: ResourceFinder()
		, TestFile(wxT("test.php"))
		, Matches() {
		ResourceFinder.PhpFileExtensions.push_back(wxT("*.php"));
		ResourceFinder.InitMemory();
	}
	
	/**
	 * will call the object under test (will make the resource finder 
	 * parse the given source code. this is a bit different than the
	 * ResourceFinderFileTestClass::Prep method
	 */
	void Prep(const UnicodeString& source) {
		ResourceFinder.BuildResourceCacheForFile(TestFile, source, true);
	}

	void CollectNearMatchResources(const UnicodeString& search) {
		mvceditor::ResourceSearchClass resourceSearch(search);
		Matches = ResourceFinder.CollectNearMatchResources(resourceSearch);
	}

	mvceditor::ResourceFinderClass ResourceFinder;
	wxString TestFile;
	std::vector<mvceditor::ResourceClass> Matches;
};

class DynamicResourceTestClass {

public:

	mvceditor::ResourceFinderClass ResourceFinder;
	std::vector<mvceditor::ResourceClass> DynamicResources;
	std::vector<mvceditor::ResourceClass> Matches;
	wxString TestFile;

	DynamicResourceTestClass() 
		: ResourceFinder() 
		, DynamicResources()
		, Matches()
		, TestFile(wxT("test.php")) {
		ResourceFinder.PhpFileExtensions.push_back(wxT("*.php"));
		ResourceFinder.InitMemory();
		
		// create a small class that implements a magic method
		// then add a dynamic resource that will be used to mimic the resource
		// returned by the magic method.
		ResourceFinder.BuildResourceCacheForFile(TestFile, mvceditor::CharToIcu(
			"<?php\n"
			"class MyDynamicClass {\n"
			"\tfunction work() {} \n"
			"\tfunction __get($name) {\n"
			"\t\treturn $name == 'address' ? '123 main st.' : '';\n"
			"\t}\n"
			"}\n"
			"function globalHandle() {\n"
			"\treturn new MyDynamicClass;\n"
			"}\n"
		), true);

		mvceditor::ResourceClass res;
		res.Identifier = UNICODE_STRING_SIMPLE("address");
		res.ReturnType = UNICODE_STRING_SIMPLE("string");
		res.Type = mvceditor::ResourceClass::MEMBER;
		res.ClassName = UNICODE_STRING_SIMPLE("MyDynamicClass");
		DynamicResources.push_back(res);
	}

	void CollectNearMatchResources(const UnicodeString& search) {
		mvceditor::ResourceSearchClass resourceSearch(search);
		Matches = ResourceFinder.CollectNearMatchResources(resourceSearch);
	}
};

class ResourceSearchTestClass {

public:

	mvceditor::ResourceSearchClass* ResourceSearch;

	ResourceSearchTestClass()
		: ResourceSearch(NULL) {
	
	}

	~ResourceSearchTestClass() {
		if (ResourceSearch) {
			delete ResourceSearch;
		}
	}

	void Make(const char* query) {
		ResourceSearch = new mvceditor::ResourceSearchClass(mvceditor::CharToIcu(query));
	}

};

#define CHECK_MEMBER_RESOURCE(className, identifier, resource) \
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(className), resource.ClassName);\
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(identifier), resource.Identifier);

#define CHECK_RESOURCE(identifier, resource) \
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(identifier), resource.Identifier);

#define CHECK_NAMESPACE_RESOURCE(namespaceName, identifier, resource) \
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(namespaceName), resource.NamespaceName);\
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(identifier), resource.Identifier);


SUITE(ResourceFinderTestClass) {

TEST_FIXTURE(ResourceFinderFileTestClass, CollectNearMatchResourcesShouldFindFileWhenFileNameMatches) {
	Prep(wxString::FromAscii(
		"<?php\n"
		"$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources(mvceditor::WxToIcu(TestFile));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].GetFullPath());
}

TEST_FIXTURE(ResourceFinderFileTestClass, CollectNearMatchResourcesShouldFindFileWhenFileNameMatchesMiscFiles) {
	Prep(wxString::FromAscii(
		"<?php\n"
		"$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	ResourceFinder.MiscFileExtensions.push_back(wxT("*.yml"));
	wxString miscFile = wxT("config.yml");
	CreateFixtureFile(miscFile, wxString::FromAscii(
		"app:\n"
		"  debug: true\n"
	));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	ResourceFinder.Walk(TestProjectDir + miscFile);
	CollectNearMatchResources(mvceditor::WxToIcu(miscFile));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + miscFile, Matches[0].GetFullPath());
}

TEST_FIXTURE(ResourceFinderFileTestClass, CollectNearMatchResourcesShouldFindFileWhenFileNameIsASubset) {
	Prep(wxString::FromAscii(
		"<?php\n"
		"$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("est.php"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].GetFullPath());
}

TEST_FIXTURE(ResourceFinderFileTestClass, CollectNearMatchResourcesShouldNotFindFileWhenFileNameMatchesButLineNumberIsTooBig) {
	Prep(wxString::FromAscii(
		"<?php\n"
		"$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("test.php:100"));
	CHECK_VECTOR_SIZE(0, Matches);
	mvceditor::ResourceSearchClass resourceSearch(UNICODE_STRING_SIMPLE("test.php:100"));
	CHECK_EQUAL(100, resourceSearch.GetLineNumber());
}

TEST_FIXTURE(ResourceFinderFileTestClass, CollectNearMatchResourcesShouldFindFileWhenFileNameMatchesAndFileHasDifferentLineEndings) {
	
	// should count unix, windows & mac line endings
	Prep(wxString::FromAscii(
		"<?php\n"
		"\t\r\n"
		"\t\r"
		"\t$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("test.php:6"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].GetFullPath());
}

TEST_FIXTURE(ResourceFinderFileTestClass, CollectNearMatchResourcesShouldFindFileWhenFileNameDoesNotMatchCase) {
	TestFile = wxT("TeST.php");
	Prep(wxString::FromAscii(
		"<?php\n"
		"\t\r\n"
		"\t\r"
		"\t$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("test.php:6"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].GetFullPath());
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFileWhenClassNameMatches) {
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
	mvceditor::ResourceClass resource = Matches[0];
	CHECK_NAMESPACE_RESOURCE("\\", "UserClass", resource);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), resource.ReturnType);
	CHECK_UNISTR_EQUALS("class UserClass", resource.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** this is my class */"), resource.Comment);
	CHECK_EQUAL(mvceditor::ResourceClass::CLASS, resource.Type);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldNotFindFileWhenClassNameDoesNotMatch) {
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

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFileWhenClassNameIsNotTheExactSame) {
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

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFileWhenClassNameHasAnExtends) {
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

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFileWhenClassNameAndMethodNameMatch) {
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
	mvceditor::ResourceClass resource = Matches[0];
	CHECK_MEMBER_RESOURCE("UserClass", "getName", resource);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\"), resource.NamespaceName);
	CHECK_UNISTR_EQUALS("string", resource.ReturnType);
	CHECK_UNISTR_EQUALS("string public function getName()", resource.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** returns the name @return string */"), resource.Comment);
	CHECK_EQUAL(mvceditor::ResourceClass::METHOD, resource.Type);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFileWhenClassNameAndSecondMethodNameMatch) {
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

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldNotFindFileWhenClassNameMatchesButMethodNameDoesNot) {
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

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFileWhendMethodNameMatchesButClassIsNotGiven) {
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

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldOnlySaveTheExactMatchWhenAnExactMatchIsFound) {
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

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindTwoFilesWhenClassNameMatches) {
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

TEST_FIXTURE(ResourceFinderFileTestClass, CollectNearMatchResourcesShouldNotFindFileWhenItHasBeenModified) {
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
	ResourceFinder.Walk(TestProjectDir + TestFile);
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
	ResourceFinder.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass"));
	CHECK_VECTOR_SIZE(0, Matches);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindClassAfterFindingFile) {
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

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFunctionWhenFunctionNameMatches) {
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
	mvceditor::ResourceClass resource = Matches[0];
	CHECK_UNISTR_EQUALS("printUser", resource.Identifier);
	CHECK_UNISTR_EQUALS("void", resource.ReturnType);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("function printUser($user)"), resource.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** print a user @return void */"), resource.Comment);
	CHECK_EQUAL(mvceditor::ResourceClass::FUNCTION, resource.Type);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindMatchesForClassesAndFunctions) {
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

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindMatchesForClassMembers) {
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
	mvceditor::ResourceClass resource = Matches[0];
	CHECK_MEMBER_RESOURCE("UserClass", "name", resource);
	CHECK_UNISTR_EQUALS("string", resource.ReturnType);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::name"), resource.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** the user name @var string */"), resource.Comment);
	CHECK_EQUAL(mvceditor::ResourceClass::MEMBER, resource.Type);
}


TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindMatchesForClassConstant) {
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
	mvceditor::ResourceClass resource = Matches[0];
	CHECK_MEMBER_RESOURCE("UserClass", "MAX", resource);
	CHECK_UNISTR_EQUALS("int", resource.ReturnType);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::MAX"), resource.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** the max constant @var int */"), resource.Comment);
	CHECK_EQUAL(mvceditor::ResourceClass::CLASS_CONSTANT, resource.Type);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindMatchesForDefines) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"/** the max constant @var int */\n"
		"define('MAX_ITEMS', 1);\n"
		"?>\n"
	));	
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("MAX_ITEMS"));
	CHECK_VECTOR_SIZE(1, Matches);
	mvceditor::ResourceClass resource = Matches[0];
	CHECK_UNISTR_EQUALS("MAX_ITEMS", resource.Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), resource.ReturnType);
	CHECK_UNISTR_EQUALS("1", resource.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** the max constant @var int */"), resource.Comment);
	CHECK_EQUAL(mvceditor::ResourceClass::DEFINE, resource.Type);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindMatchesForCorrectClassMethod) {
	
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

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindPartialMatchesForCorrectClassMethod) {
	
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

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindPartialMatchesForClassMethodsWithNoClassName) {
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

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindMatchesForNativeFunctions) {
	ResourceFinder.InitFile(mvceditor::NativeFunctionsAsset());

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

TEST_FIXTURE(ResourceFinderFileTestClass, CollectNearMatchResourcesShouldFindMatchesWhenUsingBuildResourceCacheForFile) {
	
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
	ResourceFinder.Walk(TestProjectDir + TestFile);
	ResourceFinder.BuildResourceCacheForFile(TestProjectDir + TestFile, uniCode, false);
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("printUse"));
	CHECK_VECTOR_SIZE(2, Matches);
	CHECK_UNISTR_EQUALS("printUser", Matches[0].Identifier);
	CHECK_UNISTR_EQUALS("printUserList", Matches[1].Identifier);
}

TEST_FIXTURE(ResourceFinderFileTestClass, InitFileShouldLoadDbFromFile) {
	wxFileName newDbFileName(TestProjectDir, wxT("test.db"));
	ResourceFinder.InitFile(newDbFileName);
	Prep(wxString::FromAscii(
		"<?php\n"
		"$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources(mvceditor::WxToIcu(TestFile));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].GetFullPath());

	// do a lookup in the new resource finder that only looks at the DB file
	// and does not parse the file system
	mvceditor::ResourceFinderClass newResourceFinder;
	newResourceFinder.InitFile(newDbFileName);
	Matches = newResourceFinder.CollectNearMatchResources(mvceditor::WxToIcu(TestFile));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].GetFullPath());
}


TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindMatchesWhenUsingBuildResourceCacheForFileAndUsingNewFile) {
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
	ResourceFinder.BuildResourceCacheForFile(fileName, uniCode, true);
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("printUse"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("printUser", Matches[0].Identifier);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldCollectAllMethodsWhenClassIsNotGiven) {
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

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldNotCollectParentClassesWhenInheritedClassNameIsGiven) {
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

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectFullyQualifiedResourcesShouldFindFileWhenClassNameMatches) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
	));	
	mvceditor::ResourceSearchClass resourceSearch(UNICODE_STRING_SIMPLE("UserClass"));
	Matches = ResourceFinder.CollectFullyQualifiedResource(resourceSearch);
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].GetFullPath());
	CHECK_EQUAL(TestFile, Matches[0].GetFullPath());
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectFullyQualifiedResourcesShouldFindFileWhenClassAndPropertyNameAreTheSame) {
		
	// the name of the class and the name of the property are the same
	// the method under test should know the difference and only return the class resource
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
	mvceditor::ResourceSearchClass resourceSearch(UNICODE_STRING_SIMPLE("SetClass"));
	Matches = ResourceFinder.CollectFullyQualifiedResource(resourceSearch);
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("SetClass", Matches[0].Identifier);
	CHECK_UNISTR_EQUALS("SetClass", Matches[0].ClassName);
	CHECK_EQUAL(mvceditor::ResourceClass::CLASS, Matches[0].Type);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectFullyQualifiedResourcesShouldFindMatchesForCorrectClassMethod) {
	
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
	mvceditor::ResourceSearchClass resourceSearch(UNICODE_STRING_SIMPLE("UserClass::get"));
	Matches = ResourceFinder.CollectFullyQualifiedResource(resourceSearch);
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("UserClass", "get", Matches[0]);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectFullyQualifiedResourcesShouldNotFindFileWhenClassNameDoesNotMatch) {
	Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
	));	
	mvceditor::ResourceSearchClass resourceSearch(UNICODE_STRING_SIMPLE("User"));
	Matches = ResourceFinder.CollectFullyQualifiedResource(resourceSearch);
	CHECK_VECTOR_SIZE(0, Matches);
}

TEST_FIXTURE(ResourceFinderFileTestClass, CollectFullyQualifiedResourcesShouldFindClassWhenFileHasBeenModified) {
	
	// this method is testing the scenario where the resource cache is modified and when the CollectFullyQualifiedResource
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
	ResourceFinder.Walk(TestProjectDir + TestFile);
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
	ResourceFinder.Walk(TestProjectDir + TestFile);
	mvceditor::ResourceSearchClass resourceSearch(UNICODE_STRING_SIMPLE("AdminClass"));
	Matches = ResourceFinder.CollectFullyQualifiedResource(resourceSearch);
	CHECK_VECTOR_SIZE(1, Matches);
}

TEST_FIXTURE(ResourceFinderFileTestClass, CollectFullyQualifiedResourcesShouldFindClassWhenFileHasBeenDeleted) {
	
	// this method is testing the scenario where the resource cache invalidates matches when files have been deleted
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
	mvceditor::ResourceSearchClass resourceSearch(UNICODE_STRING_SIMPLE("UserClass"));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].GetFullPath());
	CHECK(wxRemoveFile(TestProjectDir + TestFile));
	Matches = ResourceFinder.CollectFullyQualifiedResource(resourceSearch);

	// before this was expected to be zero, but the code that ensures matched files
	// exist is gone from the CollectNearMatchResources method because of performance
	// issues and we want the method to be fast because it is performed during 
	// user clicks
	CHECK_VECTOR_SIZE(1, Matches);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, GetResourceMatchShouldReturnSignatureForConstructors) {
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
	mvceditor::ResourceSearchClass resourceSearch(UNICODE_STRING_SIMPLE("UserClass::__construct"));
	Matches = ResourceFinder.CollectFullyQualifiedResource(resourceSearch);
	mvceditor::ResourceClass resource = Matches[0];
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("public function __construct($name)"), resource.Signature);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, GetResourceParentClassShouldReturnParentClass) {
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
	CHECK_UNISTR_EQUALS("UserClass", ResourceFinder.GetResourceParentClassName(UNICODE_STRING_SIMPLE("AdminClass")));
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, GetResourceParentClassShouldReturnParentClassForDeepHierarchy) {
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
	CHECK_UNISTR_EQUALS("AdminClass", ResourceFinder.GetResourceParentClassName(UNICODE_STRING_SIMPLE("SuperAdminClass")));
	CHECK_UNISTR_EQUALS("UserClass", ResourceFinder.GetResourceParentClassName(UNICODE_STRING_SIMPLE("AdminClass")));
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, GetResourceMatchPositionShouldReturnValidPositionsForClassMethodFunctionAndMember) {
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
	CHECK(mvceditor::ResourceFinderClass::GetResourceMatchPosition(Matches[0], icuCode, pos, length));
	CHECK_EQUAL(6, pos);
	CHECK_EQUAL(16, length);
	
	// checking methods
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("::getName"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("UserClass", "getName", Matches[0]);
	CHECK(mvceditor::ResourceFinderClass::GetResourceMatchPosition(Matches[0], icuCode, pos, length));
	CHECK_EQUAL(icuCode.indexOf(UNICODE_STRING_SIMPLE("function getName()")), (int32_t)pos);
	CHECK_EQUAL(17, length);
	
	// checking properties
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("UserClass::name"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("UserClass", "name", Matches[0]);
	CHECK(mvceditor::ResourceFinderClass::GetResourceMatchPosition(Matches[0], icuCode, pos, length));
	CHECK_EQUAL(icuCode.indexOf(UNICODE_STRING_SIMPLE("private $name")), (int32_t)pos);
	CHECK_EQUAL(14, length);
	
	// checking functions
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("printUser"));
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("printUser", Matches[0].Identifier);
	CHECK(mvceditor::ResourceFinderClass::GetResourceMatchPosition(Matches[0], icuCode, pos, length));
	CHECK_EQUAL(icuCode.indexOf(UNICODE_STRING_SIMPLE("function printUser")), (int32_t)pos);
	CHECK_EQUAL(19, length);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectQualifiedResourceNamespaces) {
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
	mvceditor::ResourceSearchClass resourceSearch(UNICODE_STRING_SIMPLE("\\First\\Child\\MyClass"));
	Matches = ResourceFinder.CollectFullyQualifiedResource(resourceSearch);
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\MyClass"), Matches[0].Identifier);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectQualifiedResourceNamespacesShouldNotFindDuplicates) {
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
	mvceditor::ResourceSearchClass resourceSearch(UNICODE_STRING_SIMPLE("\\First\\Child\\MyClass"));
	Matches = ResourceFinder.CollectFullyQualifiedResource(resourceSearch);
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\MyClass"), Matches[0].Identifier);

	std::vector<mvceditor::ResourceClass> all = ResourceFinder.All();

	//  6 resources total
	// namespace First\\Child 
	// class MyClass
	// function MyClass::work
	// function singleWork
	// class TwoClass
	// function TwoClass::twoWork
	CHECK_VECTOR_SIZE(6, all);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectResourceInGlobalNamespaces) {
	 Prep(mvceditor::CharToIcu(
		"<?php\n"
		"class MyClass {\n"
		"	function work() {} \n"
		"}\n"
		"\n"
		"function singleWork() { } \n"
		"?>\n"
	));
	 mvceditor::ResourceSearchClass resourceSearch(UNICODE_STRING_SIMPLE("\\MyClass"));
	Matches = ResourceFinder.CollectFullyQualifiedResource(resourceSearch);
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("\\MyClass", Matches[0].Identifier);
	
	mvceditor::ResourceSearchClass resourceSearchFunction(UNICODE_STRING_SIMPLE("\\singleWork"));
	Matches = ResourceFinder.CollectFullyQualifiedResource(resourceSearchFunction);
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("\\singleWork", Matches[0].Identifier);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchNamespaces) {
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

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchNamespaceQualifiedClassesAndFunctions) {
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

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchNamespaceQualifiedClassesShouldIgnoreOtherNamespaces) {
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

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchesShouldFindTraitMembers) {
	ResourceFinder.SetVersion(pelet::PHP_54);
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
	
	mvceditor::ResourceSearchClass resourceSearch(UNICODE_STRING_SIMPLE("ezcReflectionMethod::getReturn"));
	
	// tell the resource finder to look for traits
	std::vector<UnicodeString> traits;
	traits.push_back(UNICODE_STRING_SIMPLE("ezcReflectionReturnInfo"));
	resourceSearch.SetTraits(traits);

	Matches = ResourceFinder.CollectNearMatchResources(resourceSearch);

	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("ezcReflectionReturnInfo", "getReturnType", Matches[0]);
	CHECK_UNISTR_EQUALS("getReturnType", Matches[0].Identifier);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchesShouldFindTraitsWhenLookingForAllMethods) {
	ResourceFinder.SetVersion(pelet::PHP_54); 
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
	mvceditor::ResourceSearchClass resourceSearch(UNICODE_STRING_SIMPLE("ezcReflectionMethod::"));
	
	// tell the resource finder to look for traits
	std::vector<UnicodeString> traits;
	traits.push_back(UNICODE_STRING_SIMPLE("ezcReflectionReturnInfo"));
	traits.push_back(UNICODE_STRING_SIMPLE("ezcReflectionFunctionInfo"));
	resourceSearch.SetTraits(traits);

	Matches = ResourceFinder.CollectNearMatchResources(resourceSearch);
	
	// for now just show both the aliased and original methods
	CHECK_VECTOR_SIZE(3, Matches);
	CHECK_MEMBER_RESOURCE("ezcReflectionReturnInfo", "getFunctionReturnType", Matches[0]);
	CHECK_UNISTR_EQUALS("getFunctionReturnType", Matches[0].Identifier);
	CHECK_MEMBER_RESOURCE("ezcReflectionFunctionInfo", "getReturnType", Matches[1]);
	CHECK_UNISTR_EQUALS("getReturnType", Matches[1].Identifier);
	CHECK_MEMBER_RESOURCE("ezcReflectionReturnInfo", "getReturnType", Matches[2]);
	CHECK_UNISTR_EQUALS("getReturnType", Matches[2].Identifier);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, GetResourceTraitsShouldReturnAllTraits) {
	//ResourceFinder.InitFile(wxFileName(wxT("c:\\users\\rperpuly\\desktop\\t.db")));
	ResourceFinder.SetVersion(pelet::PHP_54); 
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
	std::vector<UnicodeString> traits = ResourceFinder.GetResourceTraits(
		UNICODE_STRING_SIMPLE("ezcReflectionMethod"), UNICODE_STRING_SIMPLE(""));
	CHECK_VECTOR_SIZE(2, traits);

	// because traits are not guaranteed to be in order
	std::sort(traits.begin(), traits.end());
	CHECK_UNISTR_EQUALS("ezcReflectionFunctionInfo", traits[0]);
	CHECK_UNISTR_EQUALS("ezcReflectionReturnInfo", traits[1]);
}

TEST_FIXTURE(DynamicResourceTestClass, AddDynamicResourcesShouldWorkWithCollect) {
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("MyDynamicClass::"));
	CHECK_VECTOR_SIZE(2, Matches);
	CHECK_UNISTR_EQUALS("__get", Matches[0].Identifier);
	CHECK_UNISTR_EQUALS("work", Matches[1].Identifier);

	// now add the dyamic property to the cache
	ResourceFinder.AddDynamicResources(DynamicResources);

	// now test the Collect functionality works as it does for resources that were parsed
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("MyDynamicClass::"));
	CHECK_VECTOR_SIZE(3, Matches);
	mvceditor::ResourceClass match = Matches[0];
	CHECK_MEMBER_RESOURCE("MyDynamicClass", "__get", match);
	CHECK_UNISTR_EQUALS("__get", match.Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), match.ReturnType);
	CHECK_EQUAL(mvceditor::ResourceClass::METHOD, match.Type);

	match = Matches[1];
	CHECK_MEMBER_RESOURCE("MyDynamicClass", "address", match);
	CHECK_UNISTR_EQUALS("address", match.Identifier);
	CHECK_UNISTR_EQUALS("string", match.ReturnType);
	CHECK_EQUAL(mvceditor::ResourceClass::MEMBER, match.Type);

	match = Matches[2];
	CHECK_MEMBER_RESOURCE("MyDynamicClass", "work", match);
	CHECK_UNISTR_EQUALS("work", match.Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), match.ReturnType);
	CHECK_EQUAL(mvceditor::ResourceClass::METHOD, match.Type);
}

TEST_FIXTURE(DynamicResourceTestClass, AddDynamicResourcesShouldWorkWhenUsedBeforeWalk) {

	// now add the dyamic property to the cache before calling Walk()
	ResourceFinder.AddDynamicResources(DynamicResources);

	// now test the Collect functionality works as it does for resources that were parsed
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("MyDynamicClass::"));
	CHECK_VECTOR_SIZE(3, Matches);
	mvceditor::ResourceClass match = Matches[0];
	CHECK_MEMBER_RESOURCE("MyDynamicClass", "__get", match);
	CHECK_UNISTR_EQUALS("__get", match.Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), match.ReturnType);
	CHECK_EQUAL(mvceditor::ResourceClass::METHOD, match.Type);

	match = Matches[1];
	CHECK_MEMBER_RESOURCE("MyDynamicClass", "address", match);
	CHECK_UNISTR_EQUALS("address", match.Identifier);
	CHECK_UNISTR_EQUALS("string", match.ReturnType);
	CHECK_EQUAL(mvceditor::ResourceClass::MEMBER, match.Type);

	match = Matches[2];
	CHECK_MEMBER_RESOURCE("MyDynamicClass", "work", match);
	CHECK_UNISTR_EQUALS("work", match.Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), match.ReturnType);
	CHECK_EQUAL(mvceditor::ResourceClass::METHOD, match.Type);
}

TEST_FIXTURE(DynamicResourceTestClass, AddDynamicResourcesShouldNotDuplicateExistingResources) {
	
	// in this test check that we can assign a return type to a function that already
	// exists
	mvceditor::ResourceClass res;
	res.Identifier = UNICODE_STRING_SIMPLE("globalHandle");
	res.ClassName = UNICODE_STRING_SIMPLE("");
	res.ReturnType = UNICODE_STRING_SIMPLE("MyDynamicClass");
	res.Type = mvceditor::ResourceClass::FUNCTION;
	DynamicResources.push_back(res);

	// now add the dyamic property to the cache before calling Walk()
	ResourceFinder.AddDynamicResources(DynamicResources);

	// now test the Collect functionality works as it does for resources that were parsed
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("globalHandle"));
	CHECK_VECTOR_SIZE(1, Matches);
	mvceditor::ResourceClass match = Matches[0];
	CHECK_UNISTR_EQUALS("", match.ClassName);
	CHECK_UNISTR_EQUALS("globalHandle", match.Identifier);
	CHECK_UNISTR_EQUALS("MyDynamicClass", match.ReturnType);
	CHECK_EQUAL(false, match.IsDynamic);
	CHECK_UNISTR_EQUALS("function globalHandle()", match.Signature);
	CHECK_EQUAL(mvceditor::ResourceClass::FUNCTION, match.Type);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, IsFileCacheEmptyWithNativeFunctions) {
	CHECK(ResourceFinder.IsFileCacheEmpty());
	CHECK(ResourceFinder.IsResourceCacheEmpty());

	ResourceFinder.InitFile(mvceditor::NativeFunctionsAsset());
	
	// still empty
	CHECK(ResourceFinder.IsFileCacheEmpty());
	CHECK(ResourceFinder.IsResourceCacheEmpty());
}

TEST_FIXTURE(ResourceFinderFileTestClass, IsFileCacheEmptyWithAnotherFile) {
	CHECK(ResourceFinder.IsFileCacheEmpty());
	CHECK(ResourceFinder.IsResourceCacheEmpty());

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
	ResourceFinder.Walk(TestProjectDir + TestFile);

	CHECK_EQUAL(false, ResourceFinder.IsFileCacheEmpty());
	CHECK_EQUAL(false, ResourceFinder.IsResourceCacheEmpty());
}

TEST_FIXTURE(ResourceFinderFileTestClass, PhpFileExtensionsShouldWorkWithNoWildcards) {

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
	ResourceFinder.PhpFileExtensions.clear();
	ResourceFinder.PhpFileExtensions.push_back(wxT("good.php"));
	
	ResourceFinder.Walk(TestProjectDir + goodFile);
	ResourceFinder.Walk(TestProjectDir + badFile);

	CollectNearMatchResources(UNICODE_STRING_SIMPLE("good.php"));
	CollectNearMatchResources(UNICODE_STRING_SIMPLE("bad.php"));
}

TEST_FIXTURE(ResourceSearchTestClass, ShouldParseClassAndMethod) {
	Make("News_model::get_news");
	CHECK_UNISTR_EQUALS("News_model", ResourceSearch->GetClassName());
	CHECK_UNISTR_EQUALS("get_news", ResourceSearch->GetMethodName());
}

TEST_FIXTURE(ResourceSearchTestClass, ShouldParseFileName) {
	Make("class.User.php");
	CHECK_UNISTR_EQUALS("class.User.php", ResourceSearch->GetFileName());
}

TEST_FIXTURE(ResourceSearchTestClass, ShouldParseFileNameAndLineNumber) {
	Make("class.User.php:100");
	CHECK_UNISTR_EQUALS("class.User.php", ResourceSearch->GetFileName());
	CHECK_EQUAL(100, ResourceSearch->GetLineNumber());
}

}