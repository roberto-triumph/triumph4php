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
#include <wx/filefn.h>
#include <wx/timer.h>

class ResourceFinderTestClass : public FileTestFixtureClass {
public:	
	ResourceFinderTestClass() 
		: FileTestFixtureClass(wxT("resource_finder")) {
		ResourceFinder = new mvceditor::ResourceFinderClass();
		ResourceFinder->FileFilters.push_back(wxT("*.php"));
		if (wxDirExists(TestProjectDir)) {
			RecursiveRmDir(TestProjectDir);
		}
	}
	
	virtual ~ResourceFinderTestClass() {
		delete ResourceFinder;
	}

	mvceditor::ResourceFinderClass* ResourceFinder;
};

SUITE(ResourceFinderTestClass) {

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindFileWhenFileNameMatches) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	CHECK(ResourceFinder->Prepare(testFile));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("test.php"), ResourceFinder->GetResourceMatchFullPath(0));
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindFileWhenFileNameIsASubset) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	CHECK(ResourceFinder->Prepare(wxT("est.php")));
	CHECK(ResourceFinder->Prepare(testFile));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("test.php"), ResourceFinder->GetResourceMatchFullPath(0));
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindFileWhenDoesNotStartWithSearch) {
	wxString testFile = wxT("config.test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"function work() {\n"
		"\t$s = 'hello';\n"
		"}\n"
		"?>\n"
	));
	CHECK(ResourceFinder->Prepare(wxT("test")));
	CHECK(ResourceFinder->Prepare(testFile));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("config.test.php"), ResourceFinder->GetResourceMatchFullPath(0));
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldNotFindFileWhenFileNameMatchesButLineNumberIsTooBig) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	CHECK(ResourceFinder->Prepare(wxT("test.php:100")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK_EQUAL(false, ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)0, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(100, ResourceFinder->GetLineNumber());
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindFileWhenFileNameMatchesAndFileHasDifferentLineEndings) {
	
	// should count unix, windows & mac line endings
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"\t\r\n"
		"\t\r"
		"\t$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	CHECK(ResourceFinder->Prepare(wxT("test.php:6")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("test.php"), ResourceFinder->GetResourceMatchFullPath(0));
	CHECK_EQUAL(6, ResourceFinder->GetLineNumber());
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindFileWhenFileNameDoesNotMatchCase) {
	wxString testFile = wxT("Test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"\t\r\n"
		"\t\r"
		"\t$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	CHECK(ResourceFinder->Prepare(wxT("test.php:6")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("Test.php"), ResourceFinder->GetResourceMatchFullPath(0));
	CHECK_EQUAL(6, ResourceFinder->GetLineNumber());
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindFileWhenClassNameMatches) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
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
	CHECK(ResourceFinder->Prepare(wxT("UserClass")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("test.php"), ResourceFinder->GetResourceMatchFullPath(0));
	mvceditor::ResourceClass resource = ResourceFinder->GetResourceMatch(0);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass"), resource.Resource);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), resource.ReturnType);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass"), resource.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** this is my class */"), resource.Comment);
	CHECK_EQUAL(mvceditor::ResourceClass::CLASS, resource.Type);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass"), ResourceFinder->GetClassName());
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldNotFindFileWhenClassNameDoesNotMatch) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));
	CHECK(ResourceFinder->Prepare(wxT("BlogPostClass")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK_EQUAL(false, ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)0, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("BlogPostClass"), ResourceFinder->GetClassName());
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindFileWhenClassNameIsNotTheExactSame) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"class UserAdmin {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));
	CHECK(ResourceFinder->Prepare(wxT("User")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK_EQUAL(true, ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("test.php"), ResourceFinder->GetResourceMatchFullPath(0));
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("User"), ResourceFinder->GetClassName());
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindFileWhenClassNameHasAnExtends) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"class User extends Human {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));
	CHECK(ResourceFinder->Prepare(wxT("User")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("User"), ResourceFinder->GetClassName());
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindFileWhenClassNameAndMethodNameMatch) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
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
	CHECK(ResourceFinder->Prepare(wxT("UserClass::getName")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("test.php"), ResourceFinder->GetResourceMatchFullPath(0));	
	mvceditor::ResourceClass resource = ResourceFinder->GetResourceMatch(0);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::getName"), resource.Resource);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("string"), resource.ReturnType);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("string getName()"), resource.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** returns the name @return string */"), resource.Comment);
	CHECK_EQUAL(mvceditor::ResourceClass::METHOD, resource.Type);
	
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass"), ResourceFinder->GetClassName());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("getName"), ResourceFinder->GetMethodName());
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindFileWhenClassNameAndSecondMethodNameMatch) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
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
	CHECK(ResourceFinder->Prepare(wxT("UserClass::getName")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("test.php"), ResourceFinder->GetResourceMatchFullPath(0));	
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass"), ResourceFinder->GetClassName());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("getName"), ResourceFinder->GetMethodName());
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldNotFindFileWhenClassNameMatchesButMethodNameDoesNot) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	CHECK(ResourceFinder->Prepare(wxT("UserClass::getAddress")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK_EQUAL(false, ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)0, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass"), ResourceFinder->GetClassName());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("getAddress"), ResourceFinder->GetMethodName());
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindFileWhendMethodNameMatchesButClassIsNotGiven) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));
	CHECK(ResourceFinder->Prepare(wxT("::getName")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("test.php"), ResourceFinder->GetResourceMatchFullPath(0));
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::getName"), ResourceFinder->GetResourceMatch(0).Resource);
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldOnlySaveTheExactMatchWhenAnExactMatchIsFound) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
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
	CHECK(ResourceFinder->Prepare(wxT("UserClass")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("test.php"), ResourceFinder->GetResourceMatchFullPath(0));
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass"), ResourceFinder->GetResourceMatch(0).Resource);
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindTwoFilesWhenClassNameMatches) {
	wxString testFile = wxT("test.php");
	wxString testFile2 = wxT("test2.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	CreateFixtureFile(testFile2, wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	CHECK(ResourceFinder->Prepare(wxT("UserClass")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	ResourceFinder->Walk(TestProjectDir + testFile2);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)2, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("test.php"), ResourceFinder->GetResourceMatchFullPath(0));
	CHECK_EQUAL(TestProjectDir + wxT("test2.php"), ResourceFinder->GetResourceMatchFullPath(1));
	CHECK_EQUAL(wxT(""), ResourceFinder->GetResourceMatchFullPath(2));
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass"), ResourceFinder->GetClassName());
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldNotFindFileWhenItHasBeenModified) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	CHECK(ResourceFinder->Prepare(wxT("UserClass")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("test.php"), ResourceFinder->GetResourceMatchFullPath(0));
	wxSleep(2);
	CreateFixtureFile(wxT("test.php"), wxString::FromAscii(
		"<?php\n"
		"class AdminClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));
	CHECK(ResourceFinder->Prepare(wxT("UserClass")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK_EQUAL(false, ResourceFinder->CollectNearMatchResources());
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindClassAfterFindingFile) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
	));	
	CHECK(ResourceFinder->Prepare(wxT("test.php")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("test.php"), ResourceFinder->GetResourceMatchFullPath(0));
	CHECK(ResourceFinder->Prepare(wxT("UserClass")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("test.php"), ResourceFinder->GetResourceMatchFullPath(0));
	CHECK(ResourceFinder->Prepare(wxT("test.php")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("test.php"), ResourceFinder->GetResourceMatchFullPath(0));
	CHECK(ResourceFinder->Prepare(wxT("UserClass")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("test.php"), ResourceFinder->GetResourceMatchFullPath(0));
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindFunctionWhenFunctionNameMatches) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
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
	CHECK(ResourceFinder->Prepare(wxT("printUser")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	mvceditor::ResourceClass resource = ResourceFinder->GetResourceMatch(0);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("printUser"), resource.Resource);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("void"), resource.ReturnType);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("printUser($user)"), resource.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** print a user @return void */"), resource.Comment);
	CHECK_EQUAL(mvceditor::ResourceClass::FUNCTION, resource.Type);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("printUser"), ResourceFinder->GetClassName());
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindMatchesForClassesAndFunctions) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
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
	CHECK(ResourceFinder->Prepare(wxT("userClas")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)2, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass"), ResourceFinder->GetResourceMatch(0).Resource);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("userClassPrint"), ResourceFinder->GetResourceMatch(1).Resource);
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindMatchesForClassMembers) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\t/** the user name @var string */n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	CHECK(ResourceFinder->Prepare(wxT("UserClass::name")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	mvceditor::ResourceClass resource = ResourceFinder->GetResourceMatch(0);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::name"), resource.Resource);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("string"), resource.ReturnType);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::name"), resource.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** the user name @var string */"), resource.Comment);
	CHECK_EQUAL(mvceditor::ResourceClass::MEMBER, resource.Type);
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindMatchesForClassConstant) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\t/** the max constant @var int */n"
		"\tconst MAX = 1;\n"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	CHECK(ResourceFinder->Prepare(wxT("UserClass::MAX")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	mvceditor::ResourceClass resource = ResourceFinder->GetResourceMatch(0);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::MAX"), resource.Resource);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("int"), resource.ReturnType);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::MAX"), resource.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** the max constant @var int */"), resource.Comment);
	CHECK_EQUAL(mvceditor::ResourceClass::CLASS_CONSTANT, resource.Type);
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindMatchesForDefines) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"/** the max constant @var int */\n"
		"define('MAX_ITEMS', 1);\n"
		"?>\n"
	));	
	CHECK(ResourceFinder->Prepare(wxT("MAX_ITEMS")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	mvceditor::ResourceClass resource = ResourceFinder->GetResourceMatch(0);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("MAX_ITEMS"), resource.Resource);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), resource.ReturnType);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("1"), resource.Signature);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/** the max constant @var int */"), resource.Comment);
	CHECK_EQUAL(mvceditor::ResourceClass::DEFINE, resource.Type);
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindMatchesForCorrectClassMethod) {
	
	// adding 2 classes to the file because we want to test that the code can differentiate the two classes and
	// match only on the class given
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
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
	CHECK(ResourceFinder->Prepare(wxT("UserClass::get")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::get"), ResourceFinder->GetResourceMatch(0).Resource);
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindPartialMatchesForCorrectClassMethod) {
	
	// adding 2 classes to the file because we want to test that the code can differentiate the two classes and
	// match only on the class given
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
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
	CHECK(ResourceFinder->Prepare(wxT("UserClass::get")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::getName"), ResourceFinder->GetResourceMatch(0).Resource);
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindPartialMatchesForClassMethodsWithNoClassName) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
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
	CHECK(ResourceFinder->Prepare(wxT("::getNa")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::getName"), ResourceFinder->GetResourceMatch(0).Resource);
}


TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindMatchesForNativeFunctions) {
	CHECK(ResourceFinder->Prepare(wxT("array_key")));
	ResourceFinder->BuildResourceCacheForNativeFunctions();
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)2, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("array_key_exists"), ResourceFinder->GetResourceMatch(0).Resource);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("array_keys"), ResourceFinder->GetResourceMatch(1).Resource);
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindMatchesWhenUsingBuildResourceCacheForFile) {
	
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
	wxString fileName = wxT("test.php");
	CreateFixtureFile(fileName, code);
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
		"printUser{CURSOR}"
		"?>\n"
	);
	CHECK(ResourceFinder->Prepare(wxT("printUse")));
	ResourceFinder->Walk(TestProjectDir + fileName);
	ResourceFinder->BuildResourceCacheForFile(TestProjectDir + fileName, uniCode);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)2, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("printUser"), ResourceFinder->GetResourceMatch(0).Resource);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("printUserList"), ResourceFinder->GetResourceMatch(1).Resource);
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldFindMatchesWhenUsingBuildResourceCacheForFileAndUsingNewFile) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		
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
		"printUse{CURSOR}"
	);
	wxString fileName = wxT("Untitled");
	CHECK(ResourceFinder->Prepare(wxT("printUse")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	ResourceFinder->BuildResourceCacheForFile(fileName, uniCode);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("printUser"), ResourceFinder->GetResourceMatch(0).Resource);
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldCollectNearMatchResourcessForParentClass) {
	wxString sourceCode = wxString::FromAscii(
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
	);
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, sourceCode);
	CHECK(ResourceFinder->Prepare(wxT("AdminClass::")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)6, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::address"), ResourceFinder->GetResourceMatch(0).Resource);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::clearName"), ResourceFinder->GetResourceMatch(1).Resource);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("AdminClass::deleteUser"), ResourceFinder->GetResourceMatch(2).Resource);	
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("SuperUserClass::disableUser"), ResourceFinder->GetResourceMatch(3).Resource);	
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::getName"), ResourceFinder->GetResourceMatch(4).Resource);	
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::name"), ResourceFinder->GetResourceMatch(5).Resource);		
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldCollectAllMethodsWhenClassIsNotGiven) {
	wxString sourceCode = wxString::FromAscii(
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
	);
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, sourceCode);
	CHECK(ResourceFinder->Prepare(wxT("::user")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)3, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("AdminClass::userDelete"), ResourceFinder->GetResourceMatch(0).Resource);	
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("SuperUserClass::userDisable"), ResourceFinder->GetResourceMatch(1).Resource);	
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::userName"), ResourceFinder->GetResourceMatch(2).Resource);		
}

TEST_FIXTURE(ResourceFinderTestClass, CollectNearMatchResourcesShouldNotCollectParentClassesWhenInheritedClassNameIsGiven) {
	wxString sourceCode = wxString::FromAscii(
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
	);
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, sourceCode);
	CHECK(ResourceFinder->Prepare(wxT("AdminClass")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("AdminClass"), ResourceFinder->GetResourceMatch(0).Resource);
}

TEST_FIXTURE(ResourceFinderTestClass, CollectFullyQualifiedResourcesShouldFindFileWhenClassNameMatches) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
	));	
	CHECK(ResourceFinder->Prepare(wxT("UserClass")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("test.php"), ResourceFinder->GetResourceMatchFullPath(0));
}

TEST_FIXTURE(ResourceFinderTestClass, CollectFullyQualifiedResourcesShouldFindMatchesForCorrectClassMethod) {
	
	// adding 2 classes to the file because we want to test that the code can differentiate the two classes and
	// match only on the class given
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
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
	CHECK(ResourceFinder->Prepare(wxT("UserClass::get")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectFullyQualifiedResource());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::get"), ResourceFinder->GetResourceMatch(0).Resource);
}

TEST_FIXTURE(ResourceFinderTestClass, CollectFullyQualifiedResourcesShouldNotFindFileWhenClassNameDoesNotMatch) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
	));	
	CHECK(ResourceFinder->Prepare(wxT("User")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK_EQUAL(false, ResourceFinder->CollectFullyQualifiedResource());
	CHECK_EQUAL((size_t)0, ResourceFinder->GetResourceMatchCount());
}

TEST_FIXTURE(ResourceFinderTestClass, CollectFullyQualifiedResourcesShouldFindClassWhenFileHasBeenModified) {
	
	// this method is testing the scenario where the resource cache is modified and when the CollectFullyQualifiedResource
	// method is checked that the cache is re-sorted and searched correctly
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	CHECK(ResourceFinder->Prepare(wxT("UserClass")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("test.php"), ResourceFinder->GetResourceMatchFullPath(0));
	wxSleep(2);
	CreateFixtureFile(wxT("test.php"), wxString::FromAscii(
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
	CHECK(ResourceFinder->Prepare(wxT("AdminClass")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK_EQUAL(true, ResourceFinder->CollectFullyQualifiedResource());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
}

TEST_FIXTURE(ResourceFinderTestClass, CollectFullyQualifiedResourcesShouldFindClassWhenFileHasBeenDeleted) {
	
	// this method is testing the scenario where the resource cache invalidates matches when files have been deleted
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"?>\n"
	));	
	CHECK(ResourceFinder->Prepare(wxT("UserClass")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("test.php"), ResourceFinder->GetResourceMatchFullPath(0));
	wxRemoveFile(TestProjectDir + testFile);
	CHECK(ResourceFinder->Prepare(wxT("UserClass")));
	CHECK_EQUAL(false, ResourceFinder->CollectFullyQualifiedResource());
	CHECK_EQUAL((size_t)0, ResourceFinder->GetResourceMatchCount());
}

TEST_FIXTURE(ResourceFinderTestClass, GetResourceReturnTypeShouldReturnReturnTypeForClassesMethodsAndFunctions) {
	 wxString code = wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;\n"
		"\t/**\n"
		"\t * @return string\n"
		"\t */\n"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"class AdminClass extends UserClass {\n"
		"}\n"
		"\t/**\n"
		"\t * @return void\n"
		"\t */\n"
		"function userClassPrint($user) {\n"
		"\t echo $user->getName() . \"\\n\";"
		"}\n"
		"?>\n"
	);
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, code);
	CHECK(ResourceFinder->Prepare(wxT("userClas")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("string"), ResourceFinder->GetResourceReturnType(UNICODE_STRING_SIMPLE("UserClass::getName")));
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("void"), ResourceFinder->GetResourceReturnType(UNICODE_STRING_SIMPLE("userClassPrint")));
}

TEST_FIXTURE(ResourceFinderTestClass, GetResourceMatchShouldReturnSignatureForConstructors) {
	 wxString code = wxString::FromAscii(
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
	);
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, code);
	CHECK(ResourceFinder->Prepare(wxT("UserClass::UserClass")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectFullyQualifiedResource());
	mvceditor::ResourceClass resource = ResourceFinder->GetResourceMatch(0);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass($name)"), resource.Signature);
}

TEST_FIXTURE(ResourceFinderTestClass, GetResourceMatchShouldReturnSignatureForInheritedMethods) {
	 wxString code = wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"class AdminClass extends UserClass {\n"
		"\tfunction deleteUser() {\n"
		"\t\t$this->getNam{CURSOR}\n"
		"}\n"
		"function userClassPrint($user) {\n"
		"\t echo $user->getName() . \"\\n\";"
		"}\n"
		"?>\n"
	);
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, code);
	CHECK(ResourceFinder->Prepare(wxT("AdminClass::getName")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	mvceditor::ResourceClass resource = ResourceFinder->GetResourceMatch(0);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::getName"), resource.Resource);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("getName()"), resource.Signature);
}

TEST_FIXTURE(ResourceFinderTestClass, GetResourceSignatureShouldReturnSignatureForClassesMethodsAndFunctions) {
	 wxString code = wxString::FromAscii(
		"<?php\n"
		"class UserClass {\n"
		"\tprivate $name;"
		"/**\n\tGets the user's name\n*/\n"
		"\tfunction getName() {\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"}\n"
		"class AdminClass extends UserClass {\n"
		"}\n"
		"/**\nPrint the user's name to the STDOUT\n*/\n"
		"function userClassPrint($user) {\n"
		"\t echo $user->getName() . \"\\n\";"
		"}\n"
		"?>\n"
	);
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, code);
	CHECK(ResourceFinder->Prepare(wxT("userClas")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	UnicodeString comment;
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("getName()"), 
		ResourceFinder->GetResourceSignature(UNICODE_STRING_SIMPLE("UserClass::getName"), comment));
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/**\n\tGets the user's name\n*/"), comment);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("userClassPrint($user)"), 
		ResourceFinder->GetResourceSignature(UNICODE_STRING_SIMPLE("userClassPrint"), comment));
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("/**\nPrint the user's name to the STDOUT\n*/"), comment);
}

TEST_FIXTURE(ResourceFinderTestClass, GetResourceParentClassShouldReturnParentClass) {
	 wxString code = wxString::FromAscii(
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
	);
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, code);
	CHECK(ResourceFinder->Prepare(wxT("userClas")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass"), ResourceFinder->GetResourceParentClassName(UNICODE_STRING_SIMPLE("AdminClass"), 
		UNICODE_STRING_SIMPLE("")));
}

TEST_FIXTURE(ResourceFinderTestClass, GetResourceParentClassShouldReturnParentClassWhenGivenAMethod) {
	 wxString code = wxString::FromAscii(
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
	);
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, code);
	CHECK(ResourceFinder->Prepare(wxT("userClas")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass"), ResourceFinder->GetResourceParentClassName(UNICODE_STRING_SIMPLE("SuperAdminClass"), 
		UNICODE_STRING_SIMPLE("getName")));
}

TEST_FIXTURE(ResourceFinderTestClass, GetResourceMatchPositionShouldReturnValidPositionsForClassMethodFunctionAndMember) {
	 wxString code = wxString::FromAscii(
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
	UnicodeString icuCode = mvceditor::StringHelperClass::wxToIcu(code);
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, code);
	int32_t pos, 
		length;
	CHECK(ResourceFinder->Prepare(wxT("UserClass")));
	ResourceFinder->Walk(TestProjectDir + testFile);
	ResourceFinder->CollectNearMatchResources();
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass"), ResourceFinder->GetResourceMatch(0).Resource);
	CHECK(ResourceFinder->GetResourceMatchPosition(0, icuCode, pos, length));
	CHECK_EQUAL(6, pos);
	CHECK_EQUAL(16, length);
	
	// checking methods
	CHECK(ResourceFinder->Prepare(wxT("::getName")));
	ResourceFinder->CollectNearMatchResources();
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::getName"), ResourceFinder->GetResourceMatch(0).Resource);
	CHECK(ResourceFinder->GetResourceMatchPosition(0, icuCode, pos, length));
	CHECK_EQUAL(code.find(wxT("function getName()")), (size_t)pos);
	CHECK_EQUAL(17, length);
	
	// checking properties
	CHECK(ResourceFinder->Prepare(wxT("UserClass::name")));
	ResourceFinder->CollectNearMatchResources();
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::name"), ResourceFinder->GetResourceMatch(0).Resource);
	CHECK(ResourceFinder->GetResourceMatchPosition(0, icuCode, pos, length));
	CHECK_EQUAL(code.find(wxT("private $name")), (size_t)pos);
	CHECK_EQUAL(14, length);
	
	// checking functions
	CHECK(ResourceFinder->Prepare(wxT("printUser")));
	ResourceFinder->CollectNearMatchResources();
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("printUser"), ResourceFinder->GetResourceMatch(0).Resource);
	CHECK(ResourceFinder->GetResourceMatchPosition(0, icuCode, pos, length));
	CHECK_EQUAL(code.find(wxT("function printUser")), (size_t)pos);
	CHECK_EQUAL(19, length);
}

TEST_FIXTURE(ResourceFinderTestClass, CopyResourcesFromShouldCopyCopy) {
	wxString testFile = wxT("test.php");
	CreateFixtureFile(testFile, wxString::FromAscii(
		"<?php\n"
		"$s = 'hello';\n"
		"\n"
		"?>\n"
	));
	CHECK(ResourceFinder->Prepare(testFile));
	ResourceFinder->Walk(TestProjectDir + testFile);
	CHECK(ResourceFinder->CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, ResourceFinder->GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("test.php"), ResourceFinder->GetResourceMatchFullPath(0));

	mvceditor::ResourceFinderClass copy;
	copy.CopyResourcesFrom(*ResourceFinder);

	CHECK(copy.Prepare(testFile));

	// notice that we do not need to parse the file again
	CHECK(copy.CollectNearMatchResources());
	CHECK_EQUAL((size_t)1, copy.GetResourceMatchCount());
	CHECK_EQUAL(TestProjectDir + wxT("test.php"), copy.GetResourceMatchFullPath(0));
}

}
