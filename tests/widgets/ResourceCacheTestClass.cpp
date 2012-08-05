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
#include <MvcEditorChecks.h>

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
	std::vector<mvceditor::ResourceClass> Matches;
	wxFileName ResourceDbFileName;

	RegisterTestFixtureClass()
		: FileTestFixtureClass(wxT("resource-cache"))
		, ResourceCache()
		, Finder()
		, Search() 
		, PhpFileFilters()
		, Matches() 
		, ResourceDbFileName() {
		Search.Init(TestProjectDir);
		PhpFileFilters.push_back(wxT("*.php"));
		
		// create the test dir, since FileTestFixture class is lazy
		if (!wxDirExists(TestProjectDir)) {
			wxMkdir(TestProjectDir, 0777);
		}
		ResourceDbFileName.Assign(TestProjectDir + wxT("resource_cache.db"));
		ResourceCache.InitGlobal(ResourceDbFileName);
	}

	void CollectNearMatchResourcesFromAll(const UnicodeString& search) {
		Matches = ResourceCache.CollectNearMatchResourcesFromAll(search);
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
	wxFileName ResourceDbFileName;
	
	
	ExpressionCompletionMatchesFixtureClass() 
		: FileTestFixtureClass(wxT("resource-cache"))
		, ResourceCache()
		, GlobalFile(wxT("src") + wxFileName::GetPathSeparators() + wxT("global.php"))
		, File1(wxT("src") + wxFileName::GetPathSeparators() + wxT("file1.php"))
		, File2(wxT("src") + wxFileName::GetPathSeparators() + wxT("file2.php"))
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
		, PhpFileFilters()
		, ResourceDbFileName() {
		CreateSubDirectory(wxT("src"));
		Search.Init(TestProjectDir + wxT("src"));
		PhpFileFilters.push_back(wxT("*.php"));
		Scope.ClassName = UNICODE_STRING_SIMPLE("");
		Scope.MethodName = UNICODE_STRING_SIMPLE("");
		ResourceDbFileName.Assign(TestProjectDir + wxT("resource_cache.db"));
		ResourceCache.InitGlobal(ResourceDbFileName);
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
	wxString file1 = wxT("src") + wxFileName::GetPathSeparators() + wxT("file1.php");
	wxString file2 = wxT("src") + wxFileName::GetPathSeparators() + wxT("file2.php");
	wxString file3 = wxT("src") + wxFileName::GetPathSeparators() +wxT("file3.php");
	UnicodeString code1 = UNICODE_STRING_SIMPLE("<?php class ActionMy   { function w() {} }");
	UnicodeString code2 = UNICODE_STRING_SIMPLE("<?php class ActionYou  { function w() {} }");
	CreateSubDirectory(wxT("src"));
	CreateFixtureFile(file3, wxT("<?php class ActionThey { function w() {} }"));
	
	// parse the 3 files for resources
	CHECK(ResourceCache.Register(file1, false));
	CHECK(ResourceCache.Register(file2, false));
	CHECK(ResourceCache.Update(file1, code1, true));
	CHECK(ResourceCache.Update(file2, code2, true));
	
	// must call init() here since file3 exists in the hard disk and we want to parse it from disk
	Search.Init(TestProjectDir + wxT("src"));
	ResourceCache.WalkGlobal(ResourceDbFileName, Search, PhpFileFilters);
	
	// now perform the search. will search for any resource that starts with 'Action'
	// all 3 caches should hit
	CollectNearMatchResourcesFromAll(UNICODE_STRING_SIMPLE("Action"));

	CHECK_VECTOR_SIZE(3, Matches);
		
	// results should be sorted
	CHECK_UNISTR_EQUALS("ActionMy", Matches[0].Identifier);
	CHECK_UNISTR_EQUALS("ActionThey", Matches[1].Identifier);
	CHECK_UNISTR_EQUALS("ActionYou", Matches[2].Identifier);
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
	ResourceCache.WalkGlobal(ResourceDbFileName, Search, PhpFileFilters);

	CHECK(ResourceCache.Register(TestProjectDir + file1, false));
	CHECK(ResourceCache.Update(TestProjectDir + file1, code2, true));

	CollectNearMatchResourcesFromAll(UNICODE_STRING_SIMPLE("ActionMy::methodA"));
	CHECK_VECTOR_SIZE(0, Matches);

	CollectNearMatchResourcesFromAll(UNICODE_STRING_SIMPLE("ActionMy::methodB"));
	CHECK_VECTOR_SIZE(1, Matches);
}

TEST_FIXTURE(ExpressionCompletionMatchesFixtureClass, GlobalFinder) {
	
	// in this test we will create a class in file1; file2 will use that class
	// the ResourceCache object should be able to detect the variable type of 
	// the variable in file2
	wxString code1 = wxT("<?php class ActionYou  { function w() {} }");
	Code2 = UNICODE_STRING_SIMPLE("<?php $action = new ActionYou(); $action->w(); ");

	CreateFixtureFile(File1, code1);
	ResourceCache.WalkGlobal(ResourceDbFileName, Search, PhpFileFilters);
	
	CHECK(ResourceCache.Register(File2, false));
	CHECK(ResourceCache.Update(File2, Code2, true));
	
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("w"));
	ResourceCache.ExpressionCompletionMatches(File2, ParsedExpression, Scope, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_EQUAL((size_t)1, ResourceMatches.size());
	if (!ResourceMatches.empty()) {
		CHECK_UNISTR_EQUALS("w", ResourceMatches[0].Identifier);
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
	ResourceCache.WalkGlobal(ResourceDbFileName, Search, PhpFileFilters);
	
	CHECK(ResourceCache.Register(File1, false));
	CHECK(ResourceCache.Update(File1, Code1, true));
	CHECK(ResourceCache.Register(File2, false));
	CHECK(ResourceCache.Update(File2, Code2, true));
	
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("w"));
	ResourceCache.ExpressionCompletionMatches(File1, ParsedExpression, Scope, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);

	CHECK_EQUAL((size_t)1, ResourceMatches.size());
	if (!ResourceMatches.empty()) {
		CHECK_UNISTR_EQUALS("w", ResourceMatches[0].Identifier);
	}
}

TEST_FIXTURE(ExpressionCompletionMatchesFixtureClass, ResourceMatchesWithGlobalFinder) {
	
	// in this test we will create a class in file2; file1 will use that class
	// the ResourceUpdate object should be able to detect the variable type of 
	// the variable in file1
	Code1 = UNICODE_STRING_SIMPLE("<?php $action = new ActionYou(); $action->w(); ");
	GlobalCode = wxT("<?php class ActionYou  { function w() {} }");
	CreateFixtureFile(GlobalFile, GlobalCode);
	ResourceCache.WalkGlobal(ResourceDbFileName, Search, PhpFileFilters);
	
	CHECK(ResourceCache.Register(File1, false));
	CHECK(ResourceCache.Update(File1, Code1, true));
	
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("w"));
	ResourceCache.ResourceMatches(File1, ParsedExpression, Scope, 
		ResourceMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_EQUAL((size_t)1, ResourceMatches.size());
	if (!ResourceMatches.empty()) {
		CHECK_UNISTR_EQUALS("w", ResourceMatches[0].Identifier);
		CHECK_UNISTR_EQUALS("ActionYou", ResourceMatches[0].ClassName);
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
		CHECK_UNISTR_EQUALS("methodA", ResourceMatches[0].Identifier);
		CHECK_UNISTR_EQUALS("ActionMy", ResourceMatches[0].ClassName);
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
	ResourceCache.WalkGlobal(ResourceDbFileName, Search, PhpFileFilters);

	CHECK(ResourceCache.Register(File1, false));
	CHECK(ResourceCache.Update(File1, Code1, true));

	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("methodA"));

	ResourceCache.ResourceMatches(File1, ParsedExpression, Scope, 
		ResourceMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_EQUAL((size_t)1, ResourceMatches.size());
	if (!ResourceMatches.empty()) {
		CHECK_UNISTR_EQUALS("methodA", ResourceMatches[0].Identifier);
		CHECK_UNISTR_EQUALS("ActionMy", ResourceMatches[0].ClassName);
	}

	// now update the code
	CHECK(ResourceCache.Register(TestProjectDir + GlobalFile, false));
	CHECK(ResourceCache.Update(TestProjectDir + GlobalFile, Code2, true));

	ResourceMatches.clear();
	ResourceCache.ResourceMatches(GlobalFile, ParsedExpression, Scope, 
		ResourceMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_EQUAL((size_t)0, ResourceMatches.size());
}

TEST_FIXTURE(ExpressionCompletionMatchesFixtureClass, MultipleGlobalFinders) {

	// this test will exercise multiple global finders. going to create
	// 2 global finders, each will parse a different file with different PHP classes. 
	// then we will make sure that completion works for both classes
	wxString globalCode1 = wxT("<?php class ActionMy   { function methodA() {} }");

	wxString globalCode2 = wxT("<?php class ActionYours   { function methodB() {} }");
	wxString globalRoot1 = wxT("my_src") + wxFileName::GetPathSeparators();
	wxString globalRoot2 = wxT("yours_src") + wxFileName::GetPathSeparators();
	CreateSubDirectory(globalRoot1);
	CreateSubDirectory(globalRoot2);
	CreateFixtureFile(globalRoot1 + wxT("ActionMy.php"), globalCode1);
	CreateFixtureFile(globalRoot2 + wxT("ActionYours.php"), globalCode2);

	// initialize 2 global finders and prime them, backed by files
	wxFileName globalDb1(TestProjectDir + wxT("/resource_my.db"));
	ResourceCache.InitGlobal(globalDb1);
	mvceditor::DirectorySearchClass search1;
	search1.Init(TestProjectDir + globalRoot1);
	std::vector<wxString> fileFilters;
	fileFilters.push_back(wxT("*.php"));
	while (search1.More()) {
		ResourceCache.WalkGlobal(globalDb1, search1, PhpFileFilters);
	}
	wxFileName globalDb2(TestProjectDir + wxT("/resource_yours.db"));
	ResourceCache.InitGlobal(globalDb2);
	mvceditor::DirectorySearchClass search2;
	search2.Init(TestProjectDir + globalRoot2);
	while (search2.More()) {
		ResourceCache.WalkGlobal(globalDb2, search2, fileFilters);
	}

	// initialize a symbol table
	Code1 = UNICODE_STRING_SIMPLE("<?php $action = new ActionMy(); ");
	CHECK(ResourceCache.Register(File1, false));
	CHECK(ResourceCache.Update(File1, Code1, true));

	// try a completion
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("methodA"));
	ResourceCache.ResourceMatches(File1, ParsedExpression, Scope, 
		ResourceMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_EQUAL((size_t)1, ResourceMatches.size());
	if (!ResourceMatches.empty()) {
		CHECK_UNISTR_EQUALS("methodA", ResourceMatches[0].Identifier);
		CHECK_UNISTR_EQUALS("ActionMy", ResourceMatches[0].ClassName);
	}

	// create another resource cache, but we will initialize them with the
	// parsed cache; we won't need to walk over the files (re-parse them) and
	// completion should still work
	mvceditor::ResourceCacheClass newCache;
	newCache.InitGlobal(globalDb1);
	newCache.InitGlobal(globalDb2);

	// initialize a symbol table
	Code1 = UNICODE_STRING_SIMPLE("<?php $action = new ActionMy(); ");
	CHECK(newCache.Register(File1, false));
	CHECK(newCache.Update(File1, Code1, true));

	// try a completion
	ResourceMatches.clear();
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("methodA"));
	newCache.ResourceMatches(File1, ParsedExpression, Scope, 
		ResourceMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_EQUAL((size_t)1, ResourceMatches.size());
	if (!ResourceMatches.empty()) {
		CHECK_UNISTR_EQUALS("methodA", ResourceMatches[0].Identifier);
		CHECK_UNISTR_EQUALS("ActionMy", ResourceMatches[0].ClassName);
	}

	// try the second resource finder
	Code2 = UNICODE_STRING_SIMPLE("<?php $action = new ActionYours(); ");
	CHECK(newCache.Register(File2, false));
	CHECK(newCache.Update(File2, Code2, true));
	ResourceMatches.clear();
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("methodB"));
	newCache.ResourceMatches(File2, ParsedExpression, Scope, 
		ResourceMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_EQUAL((size_t)1, ResourceMatches.size());
	if (!ResourceMatches.empty()) {
		CHECK_UNISTR_EQUALS("methodB", ResourceMatches[0].Identifier);
		CHECK_UNISTR_EQUALS("ActionYours", ResourceMatches[0].ClassName);
	}

}

}