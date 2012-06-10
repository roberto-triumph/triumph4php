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
#include <widgets/ResourceCacheClass.h>
#include <FileTestFixtureClass.h>
#include "unicode/ustream.h" //get the << overloaded operator, needed by UnitTest++


/**
 * fixture that holds the object under test for 
 * the resource collection tests 
 */
class RegisterTestFixtureClass : public FileTestFixtureClass {

public:

	mvceditor::ResourceCacheClass ResourceCache;
	mvceditor::ResourceFinderClass Finder;
	mvceditor::DirectorySearchClass Search;
	std::vector<wxString> PhpFileFilters;

	RegisterTestFixtureClass()
		: FileTestFixtureClass(wxT("resource-cache"))
		, ResourceCache()
		, Finder()
		, Search() 
		, PhpFileFilters() {
		Search.Init(TestProjectDir);
		PhpFileFilters.push_back(wxT("*.php"));
	}
};

/**
 * fixture that holds object under test and dependencies for
 * completion matches tests
 */
class ExpressionCompletionMatchesFixtureClass : public FileTestFixtureClass  {

public:

	mvceditor::ResourceCacheClass ResourceCache;
	wxString GlobalFile;
	wxString File1;
	wxString File2;
	wxString GlobalCode;
	UnicodeString Code1;
	UnicodeString Code2;
	bool DoDuckTyping;
	bool DoFullyQualifiedMatchOnly;
	pelet::ScopeClass Scope;
	pelet::ExpressionClass ParsedExpression;

	std::vector<UnicodeString> VariableMatches;
	std::vector<mvceditor::ResourceClass> ResourceMatches;
	mvceditor::SymbolTableMatchErrorClass Error;
	mvceditor::DirectorySearchClass Search;
	std::vector<wxString> PhpFileFilters;
	
	
	ExpressionCompletionMatchesFixtureClass() 
		: FileTestFixtureClass(wxT("resource-cache"))
		, ResourceCache()
		, GlobalFile(wxT("global.php"))
		, File1(wxT("file1.php"))
		, File2(wxT("file2.php"))
		, GlobalCode()
		, Code1()
		, Code2()
		, DoDuckTyping(false)
		, DoFullyQualifiedMatchOnly(false)
		, Scope()
		, ParsedExpression(Scope)
		, VariableMatches()
		, ResourceMatches()
		, Error()
		, Search()
		, PhpFileFilters() {
		Search.Init(TestProjectDir);
		PhpFileFilters.push_back(wxT("*.php"));
		Scope.ClassName = UNICODE_STRING_SIMPLE("");
		Scope.MethodName = UNICODE_STRING_SIMPLE("");
	}
	
	void ToProperty(const UnicodeString& variableName, const UnicodeString& methodName) {
		ParsedExpression.Clear();
		pelet::VariablePropertyClass classProp;
		classProp.Name = variableName;
		ParsedExpression.ChainList.push_back(classProp);
		
		pelet::VariablePropertyClass methodProp;
		methodProp.Name = methodName;
		ParsedExpression.ChainList.push_back(methodProp);
	}
};

SUITE(ResourceCacheTestClass) {

TEST_FIXTURE(RegisterTestFixtureClass, RegisterShouldSucceed) {	
	wxString fileName = wxT("MyFile.php");
	CHECK(ResourceCache.Register(fileName, false));
}

TEST_FIXTURE(RegisterTestFixtureClass, RegisterShouldFail) {	
	wxString fileName = wxT("MyFile.php");
	CHECK(ResourceCache.Register(fileName, false));
	CHECK_EQUAL(false, ResourceCache.Register(fileName, false));
	
}

TEST_FIXTURE(RegisterTestFixtureClass, RegisterShouldSucceedAfterSucceedAfterUnregistering) {	
	wxString fileName = wxT("MyFile.php");
	CHECK(ResourceCache.Register(fileName, false));
	ResourceCache.Unregister(fileName);
	CHECK(ResourceCache.Register(fileName, false));
}

TEST_FIXTURE(RegisterTestFixtureClass, CollectShouldGetFromAllFinders) {
	
	// going to create 3 'files'
	wxString file1 = wxT("file1.php");
	wxString file2 = wxT("file2.php");
	wxString file3 = wxT("file3.php");
	UnicodeString code1 = UNICODE_STRING_SIMPLE("<?php class ActionMy   { function w() {} }");
	UnicodeString code2 = UNICODE_STRING_SIMPLE("<?php class ActionYou  { function w() {} }");
	CreateFixtureFile(file3, wxT("<?php class ActionThey { function w() {} }"));
	
	// parse the 3 files for resources
	CHECK(ResourceCache.Register(file1, false));
	CHECK(ResourceCache.Register(file2, false));
	CHECK(ResourceCache.Update(file1, code1, true));
	CHECK(ResourceCache.Update(file2, code2, true));
	
	// must call init() here since file3 may have not existed before
	Search.Init(TestProjectDir);
	ResourceCache.WalkGlobal(Search, PhpFileFilters);
	
	// now perform the search. will search for any resource that starts with 'Action'
	// all 3 caches should hit
	CHECK(ResourceCache.PrepareAll(wxT("Action")));
	CHECK(ResourceCache.CollectNearMatchResourcesFromAll());
	
	
	std::vector<mvceditor::ResourceClass> matches = ResourceCache.Matches();
	CHECK_EQUAL((size_t)3, matches.size());
	if (3 == matches.size()) {
		
		// results should be sorted
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("ActionMy"), matches[0].Identifier);
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("ActionThey"), matches[1].Identifier);
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("ActionYou"), matches[2].Identifier);
	}
}

TEST_FIXTURE(RegisterTestFixtureClass, CollectShouldIgnoreStaleMatches) {
	
	// create a class in file1 with methodA in the global cache
	// "open" file1 by creating a local cache, then  remove methodA from class
	// perform a search
	// methodA should not be a hit since it has been removed
	wxString file1 = wxT("file1.php");
	wxString code1 = wxT("<?php class ActionMy   { function methodA() {} }");
	UnicodeString code2 = UNICODE_STRING_SIMPLE("<?php class ActionMy   { function methodB() {} }");

	CreateFixtureFile(file1, code1);
	ResourceCache.WalkGlobal(Search, PhpFileFilters);

	CHECK(ResourceCache.Register(TestProjectDir + file1, false));
	CHECK(ResourceCache.Update(TestProjectDir + file1, code2, true));

	CHECK(ResourceCache.PrepareAll(wxT("ActionMy::methodA")));
	CHECK(ResourceCache.CollectNearMatchResourcesFromAll());
	std::vector<mvceditor::ResourceClass> matches = ResourceCache.Matches();
	CHECK_EQUAL((size_t)0, matches.size());

	CHECK(ResourceCache.PrepareAll(wxT("ActionMy::methodB")));
	CHECK(ResourceCache.CollectNearMatchResourcesFromAll());
	matches = ResourceCache.Matches();
	CHECK_EQUAL((size_t)1, matches.size());
}

TEST_FIXTURE(ExpressionCompletionMatchesFixtureClass, GlobalFinder) {
	
	// in this test we will create a class in file1; file2 will use that class
	// the ResourceCache object should be able to detect the variable type of 
	// the variable in file2
	wxString file1 = wxT("file1.php");
	wxString code1 = wxT("<?php class ActionYou  { function w() {} }");
	Code2 = UNICODE_STRING_SIMPLE("<?php $action = new ActionYou(); $action->w(); ");

	CreateFixtureFile(file1, code1);
	ResourceCache.WalkGlobal(Search, PhpFileFilters);
	
	CHECK(ResourceCache.Register(File2, false));
	CHECK(ResourceCache.Update(File2, Code2, true));
	
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("w"));
	ResourceCache.ExpressionCompletionMatches(File2, ParsedExpression, Scope, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_EQUAL((size_t)1, ResourceMatches.size());
	if (!ResourceMatches.empty()) {
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("w"), ResourceMatches[0].Identifier);
	}
}

TEST_FIXTURE(ExpressionCompletionMatchesFixtureClass, RegisteredFinder) {
	
	// in this test we will create a class in file2; file1 will use that class
	// the ResourceUpdate object should be able to detect the variable type of 
	// the variable in file1
	// the difference here is that the class is now defined in one of the registered files
	Code1 = UNICODE_STRING_SIMPLE("<?php $action = new ActionYou(); $action->w(); ");
	GlobalCode = wxT("<?php class ActionMe  { function yy() { $this;  } }");
	Code2 = UNICODE_STRING_SIMPLE("<?php class ActionYou  { function w() {} }");
	ResourceCache.WalkGlobal(Search, PhpFileFilters);
	
	CHECK(ResourceCache.Register(File1, false));
	CHECK(ResourceCache.Update(File1, Code1, true));
	CHECK(ResourceCache.Register(File2, false));
	CHECK(ResourceCache.Update(File2, Code2, true));
	
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("w"));
	ResourceCache.ExpressionCompletionMatches(File1, ParsedExpression, Scope, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);

	CHECK_EQUAL((size_t)1, ResourceMatches.size());
	if (!ResourceMatches.empty()) {
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("w"), ResourceMatches[0].Identifier);
	}
}

TEST_FIXTURE(ExpressionCompletionMatchesFixtureClass, ResourceMatchesWithGlobalFinder) {
	
	// in this test we will create a class in file2; file1 will use that class
	// the ResourceUpdate object should be able to detect the variable type of 
	// the variable in file1
	Code1 = UNICODE_STRING_SIMPLE("<?php $action = new ActionYou(); $action->w(); ");
	GlobalCode = wxT("<?php class ActionYou  { function w() {} }");
	CreateFixtureFile(GlobalFile, GlobalCode);
	ResourceCache.WalkGlobal(Search, PhpFileFilters);
	
	CHECK(ResourceCache.Register(File1, false));
	CHECK(ResourceCache.Update(File1, Code1, true));
	
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("w"));
	ResourceCache.ResourceMatches(File1, ParsedExpression, Scope, 
		ResourceMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_EQUAL((size_t)1, ResourceMatches.size());
	if (!ResourceMatches.empty()) {
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("w"), ResourceMatches[0].Identifier);
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("ActionYou"), ResourceMatches[0].ClassName);
	}
}

TEST_FIXTURE(ExpressionCompletionMatchesFixtureClass, ResourceMatchesWithRegisteredFile) {

	// create a class in file1 with methodA
	// file2 will use the class from file1; file1 and file2 will be registered
	// perform a search
	// methodA should be a hit
	Code1 = UNICODE_STRING_SIMPLE("<?php class ActionMy   { function methodA() {} }");
	Code2 = UNICODE_STRING_SIMPLE("<?php $action = new ActionMy(); ");

	CHECK(ResourceCache.Register(File1, false));
	CHECK(ResourceCache.Register(File2, false));
	CHECK(ResourceCache.Update(File1, Code1, true));
	CHECK(ResourceCache.Update(File2, Code2, true));

	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("methodA"));

	ResourceCache.ResourceMatches(File2, ParsedExpression, Scope, 
		ResourceMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_EQUAL((size_t)1, ResourceMatches.size());
	if (!ResourceMatches.empty()) {
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("methodA"), ResourceMatches[0].Identifier);
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("ActionMy"), ResourceMatches[0].ClassName);
	}
}

TEST_FIXTURE(ExpressionCompletionMatchesFixtureClass, ResourceMatchesWithStaleMatches) {

	// create a class in global file with methodA
	// file2 will use the class from global file; file2 will be registered
	// then global file will be registered with file2 (invalidating methodA)
	// perform a search
	// methodA should not be a hit since it has been removed
	GlobalCode = wxT("<?php class ActionMy   { function methodA() {} }");
	Code1 = UNICODE_STRING_SIMPLE("<?php $action = new ActionMy(); ");
	Code2 = UNICODE_STRING_SIMPLE("<?php class ActionMy   { function methodB() {} }");

	CreateFixtureFile(GlobalFile, GlobalCode);
	ResourceCache.WalkGlobal(Search, PhpFileFilters);

	CHECK(ResourceCache.Register(File1, false));
	CHECK(ResourceCache.Update(File1, Code1, true));

	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("methodA"));

	ResourceCache.ResourceMatches(File1, ParsedExpression, Scope, 
		ResourceMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_EQUAL((size_t)1, ResourceMatches.size());
	if (!ResourceMatches.empty()) {
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("methodA"), ResourceMatches[0].Identifier);
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("ActionMy"), ResourceMatches[0].ClassName);
	}

	// now update the code
	CHECK(ResourceCache.Register(TestProjectDir + GlobalFile, false));
	CHECK(ResourceCache.Update(TestProjectDir + GlobalFile, Code2, true));

	ResourceMatches.clear();
	ResourceCache.ResourceMatches(GlobalFile, ParsedExpression, Scope, 
		ResourceMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_EQUAL((size_t)0, ResourceMatches.size());
}

}