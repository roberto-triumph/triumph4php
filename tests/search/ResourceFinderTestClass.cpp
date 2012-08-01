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
#include <windows/StringHelperClass.h>
#include <FileTestFixtureClass.h>
#include <MvcEditorAssets.h>
#include <MvcEditorChecks.h>
#include <wx/filefn.h>
#include <wx/timer.h>
#include <wx/tokenzr.h>


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
		ResourceFinder.FileFilters.push_back(wxT("*.php"));
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

	void CollectNearMatchResources() {
		Matches = ResourceFinder.CollectNearMatchResources();
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
		ResourceFinder.FileFilters.push_back(wxT("*.php"));
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

	void CollectNearMatchResources() {
		Matches = ResourceFinder.CollectNearMatchResources();
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
		ResourceFinder.FileFilters.push_back(wxT("*.php"));
		ResourceFinder.InitMemory();
		
		// create a small class that implements a magic method
		// then add a dynamic resource that will be used to mimic the resource
		// returned by the magic method.
		ResourceFinder.BuildResourceCacheForFile(TestFile, mvceditor::StringHelperClass::charToIcu(
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

	void CollectNearMatchResources() {
		Matches = ResourceFinder.CollectNearMatchResources();
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
	CHECK(ResourceFinder.Prepare(TestFile));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].FullPath.GetFullPath());
}

TEST_FIXTURE(ResourceFinderFileTestClass, CollectNearMatchResourcesShouldFindFileWhenFileNameIsASubset) {
	Prep(wxString::FromAscii(
		"<?php\n"
		"$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	CHECK(ResourceFinder.Prepare(wxT("est.php")));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].FullPath.GetFullPath());
}

/*
 * TODO: fix this case
  currently searching "test" will not find the file named "config.test.php" but it should
 
TEST_FIXTURE(ResourceFinderFileTestClass, CollectNearMatchResourcesShouldFindFileWhenDoesNotStartWithSearch) {
	TestFile = wxT("config.test.php");
	Prep(wxString::FromAscii(
		"<?php\n"
		"function work() {\n"
		"\t$s = 'hello';\n"
		"}\n"
		"?>\n"
	));
	CHECK(ResourceFinder.Prepare(wxT("test")));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + wxT("config.test.php"), Matches[0].FullPath.GetFullPath(););
}
 */

TEST_FIXTURE(ResourceFinderFileTestClass, CollectNearMatchResourcesShouldNotFindFileWhenFileNameMatchesButLineNumberIsTooBig) {
	Prep(wxString::FromAscii(
		"<?php\n"
		"$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	CHECK(ResourceFinder.Prepare(wxT("test.php:100")));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(0, Matches);
	CHECK_EQUAL(100, ResourceFinder.GetLineNumber());
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
	CHECK(ResourceFinder.Prepare(wxT("test.php:6")));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].FullPath.GetFullPath());
	CHECK_EQUAL(6, ResourceFinder.GetLineNumber());
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
	CHECK(ResourceFinder.Prepare(wxT("test.php:6")));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].FullPath.GetFullPath());
	CHECK_EQUAL(6, ResourceFinder.GetLineNumber());
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFileWhenClassNameMatches) {
	Prep(mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("UserClass")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].FullPath.GetFullPath());
	mvceditor::ResourceClass resource = Matches[0];
	CHECK_NAMESPACE_RESOURCE("\\", "UserClass", resource);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), resource.ReturnType);
	CHECK_UNISTR_EQUALS("class UserClass", resource.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** this is my class */"), resource.Comment);
	CHECK_EQUAL(mvceditor::ResourceClass::CLASS, resource.Type);
	CHECK_UNISTR_EQUALS("UserClass", ResourceFinder.GetClassName());
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldNotFindFileWhenClassNameDoesNotMatch) {
	Prep(mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));
	CHECK(ResourceFinder.Prepare(wxT("BlogPostClass")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(0, Matches);
	CHECK_UNISTR_EQUALS("BlogPostClass", ResourceFinder.GetClassName());
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFileWhenClassNameIsNotTheExactSame) {
	Prep(mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class UserAdmin {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));
	CHECK(ResourceFinder.Prepare(wxT("User")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].FullPath.GetFullPath());
	CHECK_UNISTR_EQUALS("User", ResourceFinder.GetClassName());
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFileWhenClassNameHasAnExtends) {
	Prep(mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class User extends Human {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));
	CHECK(ResourceFinder.Prepare(wxT("User")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("User", ResourceFinder.GetClassName());
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFileWhenClassNameAndMethodNameMatch) {
	Prep(mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("UserClass::getName")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].FullPath.GetFullPath());
	mvceditor::ResourceClass resource = Matches[0];
	CHECK_MEMBER_RESOURCE("UserClass", "getName", resource);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\"), resource.NamespaceName);
	CHECK_UNISTR_EQUALS("string", resource.ReturnType);
	CHECK_UNISTR_EQUALS("string public function getName()", resource.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** returns the name @return string */"), resource.Comment);
	CHECK_EQUAL(mvceditor::ResourceClass::METHOD, resource.Type);
	
	CHECK_UNISTR_EQUALS("UserClass", ResourceFinder.GetClassName());
	CHECK_UNISTR_EQUALS("getName", ResourceFinder.GetMethodName());
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFileWhenClassNameAndSecondMethodNameMatch) {
	Prep(mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("UserClass::getName")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].FullPath.GetFullPath());
	CHECK_UNISTR_EQUALS("UserClass", ResourceFinder.GetClassName());
	CHECK_UNISTR_EQUALS("getName", ResourceFinder.GetMethodName());
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldNotFindFileWhenClassNameMatchesButMethodNameDoesNot) {
	Prep(mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	CHECK(ResourceFinder.Prepare(wxT("UserClass::getAddress")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(0, Matches);
	CHECK_UNISTR_EQUALS("UserClass", ResourceFinder.GetClassName());
	CHECK_UNISTR_EQUALS("getAddress", ResourceFinder.GetMethodName());
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFileWhendMethodNameMatchesButClassIsNotGiven) {
	Prep(mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));
	CHECK(ResourceFinder.Prepare(wxT("::getName")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].FullPath.GetFullPath());
	CHECK_MEMBER_RESOURCE("UserClass", "getName", Matches[0]);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldOnlySaveTheExactMatchWhenAnExactMatchIsFound) {
	Prep(mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("UserClass")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].FullPath.GetFullPath());
	CHECK_UNISTR_EQUALS("UserClass", Matches[0].Identifier);
	CHECK_EQUAL(false, Matches[0].IsNative);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindTwoFilesWhenClassNameMatches) {
	wxString testFile = wxT("test.php");
	wxString testFile2 = wxT("test2.php");
	TestFile = testFile;
	Prep(mvceditor::StringHelperClass::charToIcu(
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
	Prep(mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	CHECK(ResourceFinder.Prepare(wxT("UserClass")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(2, Matches);
	CHECK_EQUAL(testFile, Matches[0].FullPath.GetFullPath());
	CHECK_EQUAL(testFile2, Matches[1].FullPath.GetFullPath());
	CHECK_UNISTR_EQUALS("UserClass", ResourceFinder.GetClassName());
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
	CHECK(ResourceFinder.Prepare(wxT("UserClass")));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].FullPath.GetFullPath());
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
	CHECK(ResourceFinder.Prepare(wxT("UserClass")));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources();
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindClassAfterFindingFile) {
	Prep(mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
	));	
	CHECK(ResourceFinder.Prepare(wxT("test.php")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].FullPath.GetFullPath());
	CHECK(ResourceFinder.Prepare(wxT("UserClass")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].FullPath.GetFullPath());
	CHECK(ResourceFinder.Prepare(wxT("test.php")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].FullPath.GetFullPath());
	CHECK(ResourceFinder.Prepare(wxT("UserClass")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].FullPath.GetFullPath());
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindFunctionWhenFunctionNameMatches) {
	Prep(mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("printUser")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	mvceditor::ResourceClass resource = Matches[0];
	CHECK_UNISTR_EQUALS("printUser", resource.Identifier);
	CHECK_UNISTR_EQUALS("void", resource.ReturnType);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("function printUser($user)"), resource.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** print a user @return void */"), resource.Comment);
	CHECK_EQUAL(mvceditor::ResourceClass::FUNCTION, resource.Type);
	CHECK_UNISTR_EQUALS("printUser", ResourceFinder.GetClassName());
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindMatchesForClassesAndFunctions) {
	Prep(mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("userClas")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(2, Matches);
	CHECK_UNISTR_EQUALS("UserClass", Matches[0].Identifier);
	CHECK_UNISTR_EQUALS("userClassPrint", Matches[1].Identifier);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindMatchesForClassMembers) {
	Prep(mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("UserClass::name")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	mvceditor::ResourceClass resource = Matches[0];
	CHECK_MEMBER_RESOURCE("UserClass", "name", resource);
	CHECK_UNISTR_EQUALS("string", resource.ReturnType);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::name"), resource.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** the user name @var string */"), resource.Comment);
	CHECK_EQUAL(mvceditor::ResourceClass::MEMBER, resource.Type);
}


TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindMatchesForClassConstant) {
	Prep(mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("UserClass::MAX")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	mvceditor::ResourceClass resource = Matches[0];
	CHECK_MEMBER_RESOURCE("UserClass", "MAX", resource);
	CHECK_UNISTR_EQUALS("int", resource.ReturnType);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::MAX"), resource.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** the max constant @var int */"), resource.Comment);
	CHECK_EQUAL(mvceditor::ResourceClass::CLASS_CONSTANT, resource.Type);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindMatchesForDefines) {
	Prep(mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"/** the max constant @var int */\n"
		"define('MAX_ITEMS', 1);\n"
		"?>\n"
	));	
	CHECK(ResourceFinder.Prepare(wxT("MAX_ITEMS")));
	CollectNearMatchResources();
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
	Prep(mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("UserClass::get")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("UserClass", "get", Matches[0]);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindPartialMatchesForCorrectClassMethod) {
	
	// adding 2 classes to the file because we want to test that the code can differentiate the two classes and
	// match only on the class given
	Prep(mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("UserClass::get")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("UserClass", "getName", Matches[0]);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindPartialMatchesForClassMethodsWithNoClassName) {
	Prep(mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("::getNa")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("UserClass", "getName", Matches[0]);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindMatchesForNativeFunctions) {
	
	CHECK(ResourceFinder.Prepare(wxT("array_key")));
	ResourceFinder.BuildResourceCacheForNativeFunctions();
	CollectNearMatchResources();
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
	CHECK(ResourceFinder.Prepare(wxT("pdo::que")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("query", Matches[0].Identifier);
	CHECK_UNISTR_EQUALS("PDO", Matches[0].ClassName);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("PDOStatement function query($statement, $PDO::FETCH_COLUMN, $colno, $PDO::FETCH_CLASS, $classname, $ctorargs, $PDO::FETCH_INTO, $object)"), Matches[0].Signature);
	CHECK_UNISTR_EQUALS("PDOStatement", Matches[0].ReturnType);
	CHECK(Matches[0].IsNative);
	
	// a fully qualified search
	CHECK(ResourceFinder.Prepare(wxT("\\Exception")));
	CollectNearMatchResources();
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
	UnicodeString uniCode  = mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("printUse")));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	ResourceFinder.BuildResourceCacheForFile(TestProjectDir + TestFile, uniCode, false);
	CollectNearMatchResources();
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
	CHECK(ResourceFinder.Prepare(TestFile));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].FullPath.GetFullPath());

	// do a lookup in the new resource finder that only looks at the DB file
	// and does not parse the file system
	mvceditor::ResourceFinderClass newResourceFinder;
	newResourceFinder.InitFile(newDbFileName);
	newResourceFinder.Prepare(TestFile);
	Matches = newResourceFinder.CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].FullPath.GetFullPath());
}


TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldFindMatchesWhenUsingBuildResourceCacheForFileAndUsingNewFile) {
	Prep(mvceditor::StringHelperClass::charToIcu(
		
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
	UnicodeString uniCode  = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"function printUser($user) {\n"
		"\t echo $user->getName() . \"\\n\";"
		"}\n"
		"$user = new UserClass();\n"
	);
	wxString fileName = wxT("Untitled");
	CHECK(ResourceFinder.Prepare(wxT("printUse")));
	ResourceFinder.BuildResourceCacheForFile(fileName, uniCode, true);
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("printUser", Matches[0].Identifier);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldCollectNearMatchResourcesForParentClass) {
	Prep(mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("AdminClass::")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(6, Matches);

	// sorted by resource key (class name :: member name)
	CHECK_MEMBER_RESOURCE("AdminClass", "deleteUser", Matches[0]);
	CHECK_MEMBER_RESOURCE("SuperUserClass", "disableUser", Matches[1]);
	CHECK_MEMBER_RESOURCE("UserClass", "address", Matches[2]);
	CHECK_MEMBER_RESOURCE("UserClass", "clearName", Matches[3]);
	CHECK_MEMBER_RESOURCE("UserClass", "getName", Matches[4]);	
	CHECK_MEMBER_RESOURCE("UserClass", "name", Matches[5]);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldCollectAllMethodsWhenClassIsNotGiven) {
	Prep(mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("::user")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(3, Matches);
	CHECK_MEMBER_RESOURCE("AdminClass", "userDelete", Matches[0]);
	CHECK_MEMBER_RESOURCE("SuperUserClass", "userDisable", Matches[1]);
	CHECK_MEMBER_RESOURCE("UserClass", "userName", Matches[2]);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchResourcesShouldNotCollectParentClassesWhenInheritedClassNameIsGiven) {
	Prep(mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("AdminClass")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("AdminClass", Matches[0].Identifier);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectFullyQualifiedResourcesShouldFindFileWhenClassNameMatches) {
	Prep(mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
	));	
	CHECK(ResourceFinder.Prepare(wxT("UserClass")));
	Matches = ResourceFinder.CollectFullyQualifiedResource();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestFile, Matches[0].FullPath.GetFullPath());
	CHECK_EQUAL(TestFile, Matches[0].FullPath.GetFullPath());
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectFullyQualifiedResourcesWithClassHierarchy) {
	Prep(mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"class AdminClass {\n"
		"\tfunction getName() {\n"
		"\t\treturn 'Admin:' . $this->name;\n"
		"\t}\n"
		"}\n"
		"class SuperUserClass extends AdminClass {}"
	));	
	CHECK(ResourceFinder.Prepare(wxT("UserClass::getName")));
	Matches = ResourceFinder.CollectFullyQualifiedResource();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("UserClass", "getName", Matches[0]);
	
	// should only use the 'highest' possible resource (only Admin and not User)
	CHECK(ResourceFinder.Prepare(wxT("AdminClass::getName")));
	Matches = ResourceFinder.CollectFullyQualifiedResource();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("AdminClass", "getName", Matches[0]);
	
	CHECK(ResourceFinder.Prepare(wxT("SuperUserClass::getName")));
	Matches = ResourceFinder.CollectFullyQualifiedResource();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("AdminClass", "getName", Matches[0]);
	
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectFullyQualifiedResourcesShouldFindMatchesForCorrectClassMethod) {
	
	// adding 2 classes to the file because we want to test that the code can differentiate the two classes and
	// match only on the class given
	Prep(mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("UserClass::get")));
	Matches = ResourceFinder.CollectFullyQualifiedResource();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("UserClass", "get", Matches[0]);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectFullyQualifiedResourcesShouldNotFindFileWhenClassNameDoesNotMatch) {
	Prep(mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
	));	
	CHECK(ResourceFinder.Prepare(wxT("User")));
	Matches = ResourceFinder.CollectFullyQualifiedResource();
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
	CHECK(ResourceFinder.Prepare(wxT("UserClass")));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].FullPath.GetFullPath());
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
	CHECK(ResourceFinder.Prepare(wxT("AdminClass")));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	Matches = ResourceFinder.CollectFullyQualifiedResource();
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
	CHECK(ResourceFinder.Prepare(wxT("UserClass")));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(TestProjectDir + TestFile, Matches[0].FullPath.GetFullPath());
	CHECK(wxRemoveFile(TestProjectDir + TestFile));
	CHECK(ResourceFinder.Prepare(wxT("UserClass")));
	Matches = ResourceFinder.CollectFullyQualifiedResource();
	CHECK_VECTOR_SIZE(0, Matches);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, GetResourceMatchShouldReturnSignatureForConstructors) {
	 Prep(mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("UserClass::__construct")));
	Matches = ResourceFinder.CollectFullyQualifiedResource();
	mvceditor::ResourceClass resource = Matches[0];
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("public function __construct($name)"), resource.Signature);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, GetResourceMatchShouldReturnSignatureForInheritedMethods) {
	 Prep(mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"class AdminClass extends UserClass {\n"
		"\tfunction deleteUser() { } \n"
		"}\n"
		"function userClassPrint($user) {\n"
		"\t echo $user->getName() . \"\\n\";"
		"}\n"
		"?>\n"
	));
	CHECK(ResourceFinder.Prepare(wxT("AdminClass::getName")));
	CollectNearMatchResources();
	mvceditor::ResourceClass resource = Matches[0];
	CHECK_MEMBER_RESOURCE("UserClass", "getName", resource);
	CHECK_UNISTR_EQUALS("public function getName()", resource.Signature);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, GetResourceParentClassShouldReturnParentClass) {
	 Prep(mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("userClas")));
	CHECK_UNISTR_EQUALS("UserClass", ResourceFinder.GetResourceParentClassName(UNICODE_STRING_SIMPLE("AdminClass"), 
		UNICODE_STRING_SIMPLE("")));
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, GetResourceParentClassShouldReturnParentClassWhenGivenAMethod) {
	 Prep(mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("userClas")));
	CHECK_UNISTR_EQUALS("UserClass", ResourceFinder.GetResourceParentClassName(UNICODE_STRING_SIMPLE("SuperAdminClass"), 
		UNICODE_STRING_SIMPLE("getName")));
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, ClassHierarchyShouldReturnAllAncestors) {
	 Prep(mvceditor::StringHelperClass::charToIcu(
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
		" class SuperUserClass extends AdminClass {\n"
		"}\n"
		"?>\n"
	));
	CHECK(ResourceFinder.Prepare(wxT("SuperUser")));

	std::vector<UnicodeString> classes = ResourceFinder.ClassHierarchy(UNICODE_STRING_SIMPLE("SuperUserClass"));
	CHECK_EQUAL((size_t)3, classes.size());
	if ((size_t)3 == classes.size()) {
		CHECK_UNISTR_EQUALS("SuperUserClass", classes[0]);
		CHECK_UNISTR_EQUALS("AdminClass", classes[1]);
		CHECK_UNISTR_EQUALS("UserClass", classes[2]);
	}
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, GetResourceMatchPositionShouldReturnValidPositionsForClassMethodFunctionAndMember) {
	 UnicodeString icuCode = mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("UserClass")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("UserClass", Matches[0].Identifier);
	CHECK(mvceditor::ResourceFinderClass::GetResourceMatchPosition(Matches[0], icuCode, pos, length));
	CHECK_EQUAL(6, pos);
	CHECK_EQUAL(16, length);
	
	// checking methods
	CHECK(ResourceFinder.Prepare(wxT("::getName")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("UserClass", "getName", Matches[0]);
	CHECK(mvceditor::ResourceFinderClass::GetResourceMatchPosition(Matches[0], icuCode, pos, length));
	CHECK_EQUAL(icuCode.indexOf(UNICODE_STRING_SIMPLE("function getName()")), (int32_t)pos);
	CHECK_EQUAL(17, length);
	
	// checking properties
	CHECK(ResourceFinder.Prepare(wxT("UserClass::name")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("UserClass", "name", Matches[0]);
	CHECK(mvceditor::ResourceFinderClass::GetResourceMatchPosition(Matches[0], icuCode, pos, length));
	CHECK_EQUAL(icuCode.indexOf(UNICODE_STRING_SIMPLE("private $name")), (int32_t)pos);
	CHECK_EQUAL(14, length);
	
	// checking functions
	CHECK(ResourceFinder.Prepare(wxT("printUser")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("printUser", Matches[0].Identifier);
	CHECK(mvceditor::ResourceFinderClass::GetResourceMatchPosition(Matches[0], icuCode, pos, length));
	CHECK_EQUAL(icuCode.indexOf(UNICODE_STRING_SIMPLE("function printUser")), (int32_t)pos);
	CHECK_EQUAL(19, length);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectQualifiedResourceNamespaces) {
	 Prep(mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"namespace First\\Child; \n"
		"class MyClass {\n"
		"	function work() {} \n"
		"}\n"
		"\n"
		"function singleWork() { } \n"
		"?>\n"
	));
	CHECK(ResourceFinder.Prepare(wxT("\\First\\Child\\MyClass")));
	Matches = ResourceFinder.CollectFullyQualifiedResource();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\MyClass"), Matches[0].Identifier);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectQualifiedResourceNamespacesShouldNotFindDuplicates) {
	 Prep(mvceditor::StringHelperClass::charToIcu(
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
	Prep(mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"namespace First\\Child; \n"
		"class TwoClass {\n"
		"	function twoWork() {} \n"
		"}\n"
		"\n"
		"?>\n"
	));	
	CHECK(ResourceFinder.Prepare(wxT("\\First\\Child\\MyClass")));
	Matches = ResourceFinder.CollectFullyQualifiedResource();
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
	 Prep(mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class MyClass {\n"
		"	function work() {} \n"
		"}\n"
		"\n"
		"function singleWork() { } \n"
		"?>\n"
	));
	CHECK(ResourceFinder.Prepare(wxT("\\MyClass")));
	Matches = ResourceFinder.CollectFullyQualifiedResource();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("\\MyClass", Matches[0].Identifier);
	
	CHECK(ResourceFinder.Prepare(wxT("\\singleWork")));
	Matches = ResourceFinder.CollectFullyQualifiedResource();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_UNISTR_EQUALS("\\singleWork", Matches[0].Identifier);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchNamespaces) {
	 Prep(mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"namespace First\\Child; \n"
		"class MyClass {\n"
		"	function work() {} \n"
		"}\n"
		"\n"
		"function singleWork() { } \n"
		"?>\n"
	));
	CHECK(ResourceFinder.Prepare(wxT("\\First")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(3, Matches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child"), Matches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\MyClass"), Matches[1].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\singleWork"), Matches[2].Identifier);
	
	CHECK(ResourceFinder.Prepare(wxT("\\First\\Ch")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(3, Matches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child"), Matches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\MyClass"), Matches[1].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\singleWork"), Matches[2].Identifier);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchNamespaceQualifiedClassesAndFunctions) {
	 Prep(mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"namespace First\\Child; \n"
		"class MyClass {\n"
		"	function work() {} \n"
		"}\n"
		"\n"
		"function singleWork() { } \n"
		"?>\n"
	));
	CHECK(ResourceFinder.Prepare(wxT("\\First\\Child\\si")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\singleWork"), Matches[0].Identifier);
	
	CHECK(ResourceFinder.Prepare(wxT("\\First\\Child\\M")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\MyClass"), Matches[0].Identifier);
	
	CHECK(ResourceFinder.Prepare(wxT("\\First\\Child\\")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(2, Matches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\MyClass"), Matches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\singleWork"), Matches[1].Identifier);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchNamespaceQualifiedClassesShouldIgnoreOtherNamespaces) {
	 Prep(mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"namespace First\\Child { \n"
		"class MyClass { }\n"
		"}\n"
		"namespace Second\\Child { \n"
		"class MyClass { }\n"
		"}\n"
		"?>\n"
	));
	CHECK(ResourceFinder.Prepare(wxT("\\Second\\Child\\My")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\Second\\Child\\MyClass"), Matches[0].Identifier);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchesShouldFindTraits) {
	ResourceFinder.SetVersion(pelet::PHP_54);
	Prep(mvceditor::StringHelperClass::charToIcu(
		"trait ezcReflectionReturnInfo { "
		"    function getReturnType() { } "
		"} "
		" "
		"class ezcReflectionMethod { "
		"    use ezcReflectionReturnInfo;"
		"    /* ... */ "
		"}"
	));
	CHECK(ResourceFinder.Prepare(wxT("ezcReflectionMethod::getReturn")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	CHECK_MEMBER_RESOURCE("ezcReflectionReturnInfo", "getReturnType", Matches[0]);
	CHECK_UNISTR_EQUALS("getReturnType", Matches[0].Identifier);
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, CollectNearMatchesShouldFindTraitsWhenLookingForAllMethods) {
	ResourceFinder.SetVersion(pelet::PHP_54); 
	Prep(mvceditor::StringHelperClass::charToIcu(
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
	CHECK(ResourceFinder.Prepare(wxT("ezcReflectionMethod::")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(3, Matches);
	
	// for now just show both the aliased and original methods
	CHECK_MEMBER_RESOURCE("ezcReflectionReturnInfo", "getFunctionReturnType", Matches[0]);
	CHECK_UNISTR_EQUALS("getFunctionReturnType", Matches[0].Identifier);
	CHECK_MEMBER_RESOURCE("ezcReflectionFunctionInfo", "getReturnType", Matches[1]);
	CHECK_UNISTR_EQUALS("getReturnType", Matches[1].Identifier);
	CHECK_MEMBER_RESOURCE("ezcReflectionReturnInfo", "getReturnType", Matches[2]);
	CHECK_UNISTR_EQUALS("getReturnType", Matches[2].Identifier);
}

TEST_FIXTURE(DynamicResourceTestClass, AddDynamicResourcesShouldWorkWithCollect) {

	CHECK(ResourceFinder.Prepare(wxT("MyDynamicClass::")));
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(2, Matches);
	CHECK_UNISTR_EQUALS("__get", Matches[0].Identifier);
	CHECK_UNISTR_EQUALS("work", Matches[1].Identifier);

	// now add the dyamic property to the cache
	ResourceFinder.AddDynamicResources(DynamicResources);

	// now test the Collect functionality works as it does for resources that were parsed
	CHECK(ResourceFinder.Prepare(wxT("MyDynamicClass::")));
	CollectNearMatchResources();
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
	CHECK(ResourceFinder.Prepare(wxT("MyDynamicClass::")));
	
	CollectNearMatchResources();
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
	CHECK(ResourceFinder.Prepare(wxT("globalHandle")));
	
	CollectNearMatchResources();
	CHECK_VECTOR_SIZE(1, Matches);
	mvceditor::ResourceClass match = Matches[0];
	CHECK_UNISTR_EQUALS("", match.ClassName);
	CHECK_UNISTR_EQUALS("globalHandle", match.Identifier);
	CHECK_UNISTR_EQUALS("MyDynamicClass", match.ReturnType);
	CHECK_EQUAL(false, match.IsDynamic);
	CHECK_UNISTR_EQUALS("function globalHandle()", match.Signature);
	CHECK_EQUAL(mvceditor::ResourceClass::FUNCTION, match.Type);
}

TEST_FIXTURE(ResourceFinderFileTestClass, Persist) {
	Prep(wxString::FromAscii(
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
	));
	CHECK(ResourceFinder.Prepare(wxT("UserClass")));
	ResourceFinder.Walk(TestProjectDir + TestFile);
	
	wxFileName outputFile(TestProjectDir + wxT("index.csv"));
	CHECK(ResourceFinder.Persist(outputFile));
	wxString contents = GetFileContents(wxT("index.csv"));
	wxStringTokenizer tokenizer(contents, wxT("\n"));
	CHECK_EQUAL(wxT("CLASS,") + TestProjectDir + TestFile + wxT(",UserClass,"), tokenizer.GetNextToken());
	CHECK_EQUAL(wxT("METHOD,") + TestProjectDir + TestFile + wxT(",UserClass,getName"), tokenizer.GetNextToken());
	CHECK_EQUAL(wxT("MEMBER,") + TestProjectDir + TestFile + wxT(",UserClass,name"), tokenizer.GetNextToken());
	CHECK_EQUAL(wxT("FUNCTION,") + TestProjectDir + TestFile + wxT(",printUser,"), tokenizer.GetNextToken());
}

TEST_FIXTURE(ResourceFinderMemoryTestClass, IsFileCacheEmptyWithNativeFunctions) {
	CHECK(ResourceFinder.IsFileCacheEmpty());
	CHECK(ResourceFinder.IsResourceCacheEmpty());

	ResourceFinder.BuildResourceCacheForNativeFunctions();
	
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
	CHECK(ResourceFinder.Prepare(wxT("UserClass")));
	ResourceFinder.Walk(TestProjectDir + TestFile);

	CHECK_EQUAL(false, ResourceFinder.IsFileCacheEmpty());
	CHECK_EQUAL(false, ResourceFinder.IsResourceCacheEmpty());
}

TEST_FIXTURE(ResourceFinderFileTestClass, FileFiltersShouldWorkWithNoWildcards) {

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
	ResourceFinder.FileFilters.clear();
	ResourceFinder.FileFilters.push_back(wxT("good.php"));
	
	ResourceFinder.Prepare(wxT("good.php"));
	ResourceFinder.Walk(TestProjectDir + goodFile);
	ResourceFinder.Walk(TestProjectDir + badFile);

	ResourceFinder.Prepare(wxT("good.php"));
	CollectNearMatchResources();
	ResourceFinder.Prepare(wxT("bad.php"));
	CollectNearMatchResources();
}

}