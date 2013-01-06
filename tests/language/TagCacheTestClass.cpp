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
#include <language/TagCacheClass.h>
#include <globals/String.h>
#include <FileTestFixtureClass.h>
#include <unicode/ustream.h> //get the << overloaded operator, needed by UnitTest++
#include <MvcEditorChecks.h>

/**
 * fixture that holds the object under test for 
 * the resource collection tests 
 */
class RegisterTestFixtureClass : public FileTestFixtureClass {

public:

	mvceditor::TagCacheClass TagCache;
	mvceditor::ParsedTagFinderClass Finder;
	mvceditor::DirectorySearchClass Search;
	std::vector<wxString> PhpFileExtensions;
	std::vector<wxString> MiscFileExtensions;
	std::vector<mvceditor::TagClass> Matches;
	wxFileName TagDbFileName;

	RegisterTestFixtureClass()
		: FileTestFixtureClass(wxT("resource-cache"))
		, TagCache()
		, Finder()
		, Search() 
		, PhpFileExtensions()
		, MiscFileExtensions()
		, Matches() 
		, TagDbFileName() {
		Search.Init(TestProjectDir);
		PhpFileExtensions.push_back(wxT("*.php"));
		
		// create the test dir, since FileTestFixture class is lazy
		if (!wxDirExists(TestProjectDir)) {
			wxMkdir(TestProjectDir, 0777);
		}
		TagDbFileName.Assign(TestProjectDir + wxT("resource_cache.db"));
	}

	void CollectNearMatchResourcesFromAll(const UnicodeString& search) {
		Matches = TagCache.CollectNearMatchResourcesFromAll(search);
	}

	mvceditor::WorkingCacheClass* CreateWorkingCache(const wxString& fileName, const UnicodeString& code) {
		mvceditor::WorkingCacheClass* cache = new mvceditor::WorkingCacheClass();
		cache->Init(fileName, fileName, true, pelet::PHP_53, false);
		cache->Update(code);
		return cache;
	}

	mvceditor::GlobalCacheClass* CreateGlobalCache(const wxString& srcDirectory) {
		mvceditor::GlobalCacheClass* cache = new mvceditor::GlobalCacheClass();
		cache->InitGlobalTag(TagDbFileName, PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);

		
		// must call init() here since we want to parse files from disk
		Search.Init(TestProjectDir + srcDirectory);
		cache->Walk(Search);
		return cache;
	}
};

/**
 * fixture that holds object under test and dependencies for
 * completion matches tests
 */
class ExpressionCompletionMatchesFixtureClass : public FileTestFixtureClass  {

public:

	mvceditor::TagCacheClass TagCache;
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
	std::vector<mvceditor::TagClass> ResourceMatches;
	mvceditor::SymbolTableMatchErrorClass Error;
	mvceditor::DirectorySearchClass Search;
	std::vector<wxString> PhpFileExtensions;
	std::vector<wxString> MiscFileExtensions;
	wxFileName TagDbFileName;
	
	
	ExpressionCompletionMatchesFixtureClass() 
		: FileTestFixtureClass(wxT("resource-cache"))
		, TagCache()
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
		, PhpFileExtensions()
		, MiscFileExtensions()
		, TagDbFileName() {
		CreateSubDirectory(wxT("src"));
		Search.Init(TestProjectDir + wxT("src"));
		PhpFileExtensions.push_back(wxT("*.php"));
		Scope.ClassName = UNICODE_STRING_SIMPLE("");
		Scope.MethodName = UNICODE_STRING_SIMPLE("");
		TagDbFileName.Assign(TestProjectDir + wxT("resource_cache.db"));
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

	mvceditor::WorkingCacheClass* CreateWorkingCache(const wxString& fileName, const UnicodeString& code) {
		mvceditor::WorkingCacheClass* cache = new mvceditor::WorkingCacheClass();
		cache->Init(fileName, fileName, true, pelet::PHP_53, false);
		cache->Update(code);
		return cache;
	}

	mvceditor::GlobalCacheClass* CreateGlobalCache(const wxString& srcDirectory) {
		mvceditor::GlobalCacheClass* cache = new mvceditor::GlobalCacheClass();
		cache->InitGlobalTag(TagDbFileName, PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);

		
		// must call init() here since we want to parse files from disk
		Search.Init(TestProjectDir + srcDirectory);
		cache->Walk(Search);
		return cache;
	}

	mvceditor::GlobalCacheClass* CreateGlobalCache(const wxFileName& resourceDbFile, const wxString& srcDirectory) {
		mvceditor::GlobalCacheClass* cache = new mvceditor::GlobalCacheClass();
		cache->InitGlobalTag(resourceDbFile, PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);

		
		// must call init() here since we want to parse files from disk
		Search.Init(TestProjectDir + srcDirectory);
		cache->Walk(Search);
		return cache;
	}
};

SUITE(ResourceCacheTestClass) {

TEST_FIXTURE(RegisterTestFixtureClass, RegisterShouldSucceed) {	
	
	// this pointer should get deleted by the TagCacheClass
	mvceditor::GlobalCacheClass* globalCache = new mvceditor::GlobalCacheClass();
	globalCache->InitGlobalTag(TagDbFileName, PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);
	CHECK(TagCache.RegisterGlobal(globalCache));
}

TEST_FIXTURE(RegisterTestFixtureClass, RegisterShouldFail) {

	// these pointers should get deleted by the TagCacheClass
	mvceditor::GlobalCacheClass* globalCache = new mvceditor::GlobalCacheClass();
	globalCache->InitGlobalTag(TagDbFileName, PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);
	
	mvceditor::GlobalCacheClass* secondGlobalCache = new mvceditor::GlobalCacheClass();
	secondGlobalCache->InitGlobalTag(TagDbFileName, PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);

	// test that the same resource DB cannot be added twice
	CHECK(TagCache.RegisterGlobal(globalCache));
	CHECK_EQUAL(false, TagCache.RegisterGlobal(secondGlobalCache));

	// must delete the new pointer since the cache did not take ownership
	delete secondGlobalCache;
}

TEST_FIXTURE(RegisterTestFixtureClass, RegisterShouldSucceedAfterSucceedAfterUnregistering) {

	// these pointers should get deleted by the TagCacheClass
	mvceditor::GlobalCacheClass* globalCache = new mvceditor::GlobalCacheClass();
	wxFileName cacheDb1 = TagDbFileName; 
	globalCache->InitGlobalTag(TagDbFileName, PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);
	
	mvceditor::GlobalCacheClass* secondGlobalCache = new mvceditor::GlobalCacheClass();
	secondGlobalCache->InitGlobalTag(TagDbFileName, PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);

	mvceditor::GlobalCacheClass* thirdGlobalCache = new mvceditor::GlobalCacheClass();
	thirdGlobalCache->InitGlobalTag(TagDbFileName, PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);

	// test that the same resource DB is added, removed, then added again
	CHECK(TagCache.RegisterGlobal(globalCache));
	CHECK_EQUAL(false, TagCache.RegisterGlobal(secondGlobalCache));
	TagCache.RemoveGlobal(TagDbFileName);
	CHECK(TagCache.RegisterGlobal(thirdGlobalCache));

	// must delete the new pointer since the cache did not take ownership
	delete secondGlobalCache;
}

TEST_FIXTURE(RegisterTestFixtureClass, CollectShouldGetFromAllFinders) {
	
	// going to create 3 'files'
	// create 2 working caches and 1 global cache
	wxString file1 = wxT("src") + wxFileName::GetPathSeparators() + wxT("file1.php");
	wxString file2 = wxT("src") + wxFileName::GetPathSeparators() + wxT("file2.php");
	wxString file3 = wxT("src") + wxFileName::GetPathSeparators() +wxT("file3.php");
	UnicodeString code1 = UNICODE_STRING_SIMPLE("<?php class ActionMy   { function w() {} }");
	UnicodeString code2 = UNICODE_STRING_SIMPLE("<?php class ActionYou  { function w() {} }");
	CreateSubDirectory(wxT("src"));
	CreateFixtureFile(file3, wxT("<?php class ActionThey { function w() {} }"));
	
	// parse the 3 files for resources. these pointers should get deleted by
	// TagCacheClass
	mvceditor::WorkingCacheClass* cache1 = CreateWorkingCache(file1, code1);
	mvceditor::WorkingCacheClass* cache2 = CreateWorkingCache(file2, code2);
	mvceditor::GlobalCacheClass* cache3 = CreateGlobalCache(wxT("src"));
	
	CHECK(TagCache.RegisterWorking(file1, cache1));
	CHECK(TagCache.RegisterWorking(file2, cache2));
	CHECK(TagCache.RegisterGlobal(cache3));	
	
	// now perform the search. will search for any resource that starts with 'Action'
	// all 3 caches should hit
	CollectNearMatchResourcesFromAll(UNICODE_STRING_SIMPLE("Action"));

	CHECK_VECTOR_SIZE(3, Matches);
		
	// results should be sorted
	CHECK_UNISTR_EQUALS("ActionMy", Matches[0].Identifier);
	CHECK_UNISTR_EQUALS("ActionThey", Matches[1].Identifier);
	CHECK_UNISTR_EQUALS("ActionYou", Matches[2].Identifier);

	TagCache.Clear();
}

TEST_FIXTURE(RegisterTestFixtureClass, NearMatchesShouldIgnoreStaleResources) {
	
	// create a class in file1 with methodA in the global cache
	// "open" file1 by creating a local cache, then  remove methodA from class
	// perform a search
	// methodA should not be a hit since it has been removed
	wxString file1 = wxT("src") + wxFileName::GetPathSeparators() + wxT("file1.php");
	wxString code1 = wxT("<?php class ActionMy   { function methodA() {} }");
	UnicodeString code2 = UNICODE_STRING_SIMPLE("<?php class ActionMy   { function methodB() {} }");
	CreateSubDirectory(wxT("src"));
	CreateFixtureFile(file1, code1);

	// working cache will contain the 'new' code that renamed methodA to methodB
	// use wxFileName to normalize the full paths, needed for the code to work
	wxFileName fileName(TestProjectDir + file1);
	mvceditor::WorkingCacheClass* cache1 = CreateWorkingCache(fileName.GetFullPath(), code2);
	mvceditor::GlobalCacheClass* cache2 = CreateGlobalCache(wxT("src"));

	CHECK(TagCache.RegisterWorking(fileName.GetFullPath(), cache1));
	CHECK(TagCache.RegisterGlobal(cache2));
	
	CollectNearMatchResourcesFromAll(UNICODE_STRING_SIMPLE("ActionMy::methodA"));
	CHECK_VECTOR_SIZE(0, Matches);

	CollectNearMatchResourcesFromAll(UNICODE_STRING_SIMPLE("ActionMy::methodB"));
	CHECK_VECTOR_SIZE(1, Matches);
}

TEST_FIXTURE(ExpressionCompletionMatchesFixtureClass, CompletionMatchesWithGlobalCache) {
	
	// in this test we will create a class in file1; file2 will use that class
	// the TagCache object should be able to detect the variable type of 
	// the variable in file2
	wxString code1 =  wxT("<?php class ActionYou  { function w() {} }");
	Code2 = UNICODE_STRING_SIMPLE("<?php $action = new ActionYou(); $action->w(); ");
	CreateFixtureFile(File1, code1);

	mvceditor::WorkingCacheClass* cache1 = CreateWorkingCache(File2, Code2);
	mvceditor::GlobalCacheClass* cache2 = CreateGlobalCache(wxT("src"));
	
	CHECK(TagCache.RegisterWorking(File2, cache1));
	CHECK(TagCache.RegisterGlobal(cache2));
	
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("w"));
	TagCache.ExpressionCompletionMatches(File2, ParsedExpression, Scope, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("w", ResourceMatches[0].Identifier);
}

TEST_FIXTURE(ExpressionCompletionMatchesFixtureClass, WorkingCachePrecedenceOverGlobalCache) {
	
	// in this test we will create a class in file2; file1 will use that class
	// the ResourceUpdate object should be able to detect the variable type of 
	// the variable in file1
	// the difference here is that the class is now defined in one of the registered files
	Code1 = UNICODE_STRING_SIMPLE("<?php $action = new ActionYou(); $action->w(); ");
	GlobalCode = wxT("<?php class ActionMe  { function yy() { $this;  } }");
	Code2 = UNICODE_STRING_SIMPLE("<?php class ActionYou  { function w() {} }");
	
	mvceditor::WorkingCacheClass* cache1 = CreateWorkingCache(File1, Code1);
	mvceditor::WorkingCacheClass* cache2 = CreateWorkingCache(File2, Code2);
	mvceditor::GlobalCacheClass* cache3 = CreateGlobalCache(wxT("src"));
	
	CHECK(TagCache.RegisterWorking(File1, cache1));
	CHECK(TagCache.RegisterWorking(File2, cache2));
	CHECK(TagCache.RegisterGlobal(cache3));
	
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("w"));
	TagCache.ExpressionCompletionMatches(File1, ParsedExpression, Scope, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);

	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("w", ResourceMatches[0].Identifier);
}

TEST_FIXTURE(ExpressionCompletionMatchesFixtureClass, ResourceMatchesWithGlobalCache) {
	
	// in this test we will create a class in file2; file1 will use that class
	// the ResourceUpdate object should be able to detect the variable type of 
	// the variable in file1
	Code1 = UNICODE_STRING_SIMPLE("<?php $action = new ActionYou(); $action->w(); ");
	GlobalCode = wxT("<?php class ActionYou  { function w() {} }");
	CreateFixtureFile(GlobalFile, GlobalCode);
	
	mvceditor::WorkingCacheClass* cache1 = CreateWorkingCache(File1, Code1);
	mvceditor::GlobalCacheClass* cache2 = CreateGlobalCache(wxT("src"));
	
	CHECK(TagCache.RegisterWorking(File1, cache1));
	CHECK(TagCache.RegisterGlobal(cache2));
	
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("w"));
	TagCache.ResourceMatches(File1, ParsedExpression, Scope, 
		ResourceMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("w", ResourceMatches[0].Identifier);
	CHECK_UNISTR_EQUALS("ActionYou", ResourceMatches[0].ClassName);
}

TEST_FIXTURE(ExpressionCompletionMatchesFixtureClass, ResourceMatchesWithWorkingCache) {

	// create a class in file1 with methodA
	// file2 will use the class from file1; file1 and file2 will be registered
	// perform a search
	// methodA should be a hit
	Code1 = UNICODE_STRING_SIMPLE("<?php class ActionMy   { function methodA() {} }");
	Code2 = UNICODE_STRING_SIMPLE("<?php $action = new ActionMy(); ");

	mvceditor::WorkingCacheClass* cache1 = CreateWorkingCache(File1, Code1);
	mvceditor::WorkingCacheClass* cache2 = CreateWorkingCache(File2, Code2);
	
	CHECK(TagCache.RegisterWorking(File1, cache1));
	CHECK(TagCache.RegisterWorking(File2, cache2));
	
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("methodA"));

	TagCache.ResourceMatches(File2, ParsedExpression, Scope, 
		ResourceMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("methodA", ResourceMatches[0].Identifier);
	CHECK_UNISTR_EQUALS("ActionMy", ResourceMatches[0].ClassName);
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

	mvceditor::WorkingCacheClass* cache1 = CreateWorkingCache(File1, Code1);
	mvceditor::GlobalCacheClass* cache2 = CreateGlobalCache(wxT("src"));

	CHECK(TagCache.RegisterWorking(File1, cache1));
	CHECK(TagCache.RegisterGlobal(cache2));
	
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("methodA"));
	TagCache.ResourceMatches(File1, ParsedExpression, Scope, 
		ResourceMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("methodA", ResourceMatches[0].Identifier);
	CHECK_UNISTR_EQUALS("ActionMy", ResourceMatches[0].ClassName);

	// now update the code by creating a working version of the global code.
	// ie. the user opening a file.
	mvceditor::WorkingCacheClass* cache3 = CreateWorkingCache(GlobalFile, Code2);
	CHECK(TagCache.RegisterWorking(GlobalFile, cache3));
	
	ResourceMatches.clear();
	TagCache.ResourceMatches(GlobalFile, ParsedExpression, Scope, 
		ResourceMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(0, ResourceMatches);
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
	wxFileName globalDb2(TestProjectDir + wxT("/resource_yours.db"));

	mvceditor::GlobalCacheClass* cache1 = CreateGlobalCache(globalDb1, globalRoot1);
	mvceditor::GlobalCacheClass* cache2 = CreateGlobalCache(globalDb2, globalRoot2);
	CHECK(TagCache.RegisterGlobal(cache1));
	CHECK(TagCache.RegisterGlobal(cache2));

	// initialize a symbol table
	Code1 = UNICODE_STRING_SIMPLE("<?php $action = new ActionMy(); ");
	mvceditor::WorkingCacheClass* cache3 = CreateWorkingCache(File1, Code1);
	CHECK(TagCache.RegisterWorking(File1, cache3));

	// try a completion
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("methodA"));
	TagCache.ResourceMatches(File1, ParsedExpression, Scope, 
		ResourceMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("methodA", ResourceMatches[0].Identifier);
	CHECK_UNISTR_EQUALS("ActionMy", ResourceMatches[0].ClassName);

	// create another resource cache, but we will initialize them with the
	// parsed cache; we won't need to walk over the files (re-parse them) and
	// completion should still work
	mvceditor::TagCacheClass newCache;
	mvceditor::GlobalCacheClass* cache4 = new mvceditor::GlobalCacheClass();
	cache4->InitGlobalTag(globalDb1, PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);
	mvceditor::GlobalCacheClass* cache5 = new mvceditor::GlobalCacheClass();
	cache5->InitGlobalTag(globalDb2, PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);

	CHECK(newCache.RegisterGlobal(cache4));
	CHECK(newCache.RegisterGlobal(cache5));
	
	// initialize a symbol table
	Code1 = UNICODE_STRING_SIMPLE("<?php $action = new ActionMy(); ");
	mvceditor::WorkingCacheClass* cache6 = CreateWorkingCache(File1, Code1);
	CHECK(newCache.RegisterWorking(File1, cache6));

	// try a completion
	ResourceMatches.clear();
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("methodA"));
	newCache.ResourceMatches(File1, ParsedExpression, Scope, 
		ResourceMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("methodA", ResourceMatches[0].Identifier);
	CHECK_UNISTR_EQUALS("ActionMy", ResourceMatches[0].ClassName);

	// try the second globe cache. need to build the symbol table
	// for the second file
	Code2 = UNICODE_STRING_SIMPLE("<?php $action = new ActionYours(); ");
	mvceditor::WorkingCacheClass* cache7 = CreateWorkingCache(File2, Code2);
	CHECK(newCache.RegisterWorking(File2, cache7));
	ResourceMatches.clear();
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("methodB"));
	newCache.ResourceMatches(File2, ParsedExpression, Scope, 
		ResourceMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("methodB", ResourceMatches[0].Identifier);
	CHECK_UNISTR_EQUALS("ActionYours", ResourceMatches[0].ClassName);
}

}