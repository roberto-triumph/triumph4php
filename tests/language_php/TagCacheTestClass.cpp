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
#include <language_php/TagCacheClass.h>
#include <language_php/TagFinderList.h>
#include <globals/String.h>
#include <FileTestFixtureClass.h>
#include <SqliteTestFixtureClass.h>
#include <unicode/ustream.h> //get the << overloaded operator, needed by UnitTest++
#include <TriumphChecks.h>
#include <globals/Assets.h>
#include <globals/Sqlite.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

/**
 * fixture that holds the object under test for 
 * the resource collection tests 
 */
class RegisterTestFixtureClass : public FileTestFixtureClass, public SqliteTestFixtureClass {

public:

	t4p::TagCacheClass TagCache;
	t4p::DirectorySearchClass Search;
	std::vector<wxString> PhpFileExtensions;
	std::vector<wxString> MiscFileExtensions;
	std::vector<t4p::PhpTagClass> Matches;
	std::vector<wxFileName> SourceDirs;
	soci::session* Session1;

	RegisterTestFixtureClass()
		: FileTestFixtureClass(wxT("resource-cache"))
		, SqliteTestFixtureClass(t4p::ResourceSqlSchemaAsset())
		, TagCache()
		, Search() 
		, PhpFileExtensions()
		, MiscFileExtensions()
		, Matches() 
		, SourceDirs() {
		Search.Init(TestProjectDir);
		PhpFileExtensions.push_back(wxT("*.php"));
		
		// create the test dir, since FileTestFixture class is lazy
		if (!wxDirExists(TestProjectDir)) {
			wxMkdir(TestProjectDir, 0777);
		}
	}

	void NearMatchTags(const UnicodeString& search) {
		std::vector<wxFileName> dirs;
		t4p::TagResultClass* result = TagCache.NearMatchTags(search, dirs);
		Matches = result->Matches();
		delete result;
	}

	t4p::WorkingCacheClass* CreateWorkingCache(const wxString& fileName, const UnicodeString& code) {
		t4p::WorkingCacheClass* cache = new t4p::WorkingCacheClass();
		t4p::SymbolTableClass emptyTable;
		cache->Init(fileName, fileName, true, pelet::PHP_53, false, emptyTable);
		cache->Update(code, emptyTable);
		return cache;
	}

	t4p::TagFinderListClass* CreateTagFinderList(const wxString& srcDirectory) {
		t4p::TagFinderListClass* cache = new t4p::TagFinderListClass();
		cache->CreateGlobalTag(PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);
			
		wxFileName srcDir;
		srcDir.AssignDir(TestProjectDir + srcDirectory);
		SourceDirs.push_back(srcDir);
			
		// must call init() here since we want to parse files from disk
		Search.Init(TestProjectDir + srcDirectory);
		cache->Walk(Search);
		return cache;
	}

	wxFileName MakeFile(wxString subDir, wxString name) {
		wxFileName fileName(TestProjectDir, name);
		fileName.AppendDir(subDir);
		return fileName;
	}
};

/**
 * fixture that holds object under test and dependencies for
 * completion matches tests
 */
class ExpressionCompletionMatchesFixtureClass : public FileTestFixtureClass, SqliteTestFixtureClass  {

public:

	t4p::TagCacheClass TagCache;
	wxString GlobalFile;
	wxString File1;
	wxString File2;
	wxString GlobalCode;
	UnicodeString Code1;
	UnicodeString Code2;
	bool DoDuckTyping;
	bool DoFullyQualifiedMatchOnly;
	pelet::ScopeClass Scope;
	pelet::VariableClass ParsedVariable;
	std::vector<wxFileName> SourceDirs;

	std::vector<UnicodeString> VariableMatches;
	std::vector<t4p::PhpTagClass> TagMatches;
	t4p::SymbolTableMatchErrorClass Error;
	t4p::DirectorySearchClass Search;
	std::vector<wxString> PhpFileExtensions;
	std::vector<wxString> MiscFileExtensions;
	soci::session* Session1;
	
	ExpressionCompletionMatchesFixtureClass() 
		: FileTestFixtureClass(wxT("tag-cache"))
		, SqliteTestFixtureClass(t4p::ResourceSqlSchemaAsset())
		, TagCache()
		, GlobalFile(wxT("src") + wxString(wxFileName::GetPathSeparator()) + wxT("global.php"))
		, File1(wxT("src") + wxString(wxFileName::GetPathSeparator()) + wxT("file1.php"))
		, File2(wxT("src") + wxString(wxFileName::GetPathSeparator()) + wxT("file2.php"))
		, GlobalCode()
		, Code1()
		, Code2()
		, DoDuckTyping(false)
		, DoFullyQualifiedMatchOnly(false)
		, Scope()
		, ParsedVariable(Scope)
		, SourceDirs()
		, VariableMatches()
		, TagMatches()
		, Error()
		, Search()
		, PhpFileExtensions()
		, MiscFileExtensions() {
		CreateSubDirectory(wxT("src"));
		Search.Init(TestProjectDir + wxT("src"));
		PhpFileExtensions.push_back(wxT("*.php"));
		Scope.ClassName = UNICODE_STRING_SIMPLE("");
		Scope.MethodName = UNICODE_STRING_SIMPLE("");
		Session1 = new soci::session(*soci::factory_sqlite3(), ":memory:");
		CreateDatabase(*Session1, t4p::ResourceSqlSchemaAsset());
		
		wxFileName srcDir;
		srcDir.AssignDir(TestProjectDir + wxT("src"));
		SourceDirs.push_back(srcDir);
	}
	
	void ToProperty(const UnicodeString& variableName, const UnicodeString& methodName) {
		ParsedVariable.Clear();
		pelet::VariablePropertyClass classProp;
		classProp.Name = variableName;
		ParsedVariable.ChainList.push_back(classProp);
		
		pelet::VariablePropertyClass methodProp;
		methodProp.Name = methodName;
		ParsedVariable.ChainList.push_back(methodProp);
	}

	t4p::WorkingCacheClass* CreateWorkingCache(const wxString& fileName, const UnicodeString& code) {
		t4p::WorkingCacheClass* cache = new t4p::WorkingCacheClass();
		t4p::SymbolTableClass emptyTable;
		cache->Init(fileName, fileName, true, pelet::PHP_53, false, emptyTable);
		cache->Update(code, emptyTable);
		return cache;
	}

	t4p::TagFinderListClass* CreateTagFinderList(const wxString& srcDirectory) {
		t4p::TagFinderListClass* cache = new t4p::TagFinderListClass();
		cache->CreateGlobalTag(PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);
		
		// must call init() here since we want to parse files from disk
		Search.Init(TestProjectDir + srcDirectory);
		cache->Walk(Search);
		return cache;
	}
};

class TagCacheSearchFixtureClass : public FileTestFixtureClass, public SqliteTestFixtureClass {

public:

	t4p::TagCacheClass TagCache;
	t4p::DirectorySearchClass Search;
	std::vector<wxString> PhpFileExtensions;
	std::vector<wxString> MiscFileExtensions;

	// using a new session instead of sqlitefixture's session
	// because GlobalTagCache class takes ownership of the session pointer
	soci::session* Session1;

	TagCacheSearchFixtureClass()
		: FileTestFixtureClass(wxT("tag-cache"))
		, SqliteTestFixtureClass(t4p::ResourceSqlSchemaAsset())
		, TagCache() 
		, Search() 
		, PhpFileExtensions() 
		, MiscFileExtensions() 
		, Session1(NULL) {
		PhpFileExtensions.push_back(wxT("*.php"));
		Session1 = new soci::session(*soci::factory_sqlite3(), ":memory:");
		CreateDatabase(*Session1, t4p::ResourceSqlSchemaAsset());
	}

	t4p::TagFinderListClass* CreateTagFinderList(const wxString& srcDirectory) {
		t4p::TagFinderListClass* cache = new t4p::TagFinderListClass();
		cache->CreateGlobalTag(PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);
			
		// must call init() here since we want to parse files from disk
		Search.Init(TestProjectDir + srcDirectory);
		cache->Walk(Search);
		return cache;
	}
};

SUITE(TagCacheTestClass) {

TEST_FIXTURE(ExpressionCompletionMatchesFixtureClass, CompletionMatchesWithTagFinderList) {
	
	// in this test we will create a class in file1; file2 will use that class
	// the TagCache object should be able to detect the variable type of 
	// the variable in file2
	wxString code1 =  wxT("<?php class ActionYou  { function w() {} }");
	Code2 = UNICODE_STRING_SIMPLE("<?php $action = new ActionYou(); $action->w(); ");
	CreateFixtureFile(File1, code1);

	t4p::WorkingCacheClass* cache1 = CreateWorkingCache(File2, Code2);
	t4p::TagFinderListClass* cache2 = CreateTagFinderList(wxT("src"));
	
	CHECK(TagCache.RegisterWorking(File2, cache1));
	TagCache.RegisterGlobal(cache2);
	
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("w"));
	TagCache.ExpressionCompletionMatches(File2, ParsedVariable, Scope, SourceDirs,
		VariableMatches, TagMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, TagMatches);
	CHECK_UNISTR_EQUALS("w", TagMatches[0].Identifier);
}

TEST_FIXTURE(ExpressionCompletionMatchesFixtureClass, TagMatchesWithTagFinderList) {
	
	// in this test we will create a class in file2; file1 will use that class
	// the ResourceUpdate object should be able to detect the variable type of 
	// the variable in file1
	Code1 = UNICODE_STRING_SIMPLE("<?php $action = new ActionYou(); $action->w(); ");
	GlobalCode = wxT("<?php class ActionYou  { function w() {} }");
	CreateFixtureFile(GlobalFile, GlobalCode);
	
	t4p::WorkingCacheClass* cache1 = CreateWorkingCache(File1, Code1);
	t4p::TagFinderListClass* cache2 = CreateTagFinderList(wxT("src"));
	
	CHECK(TagCache.RegisterWorking(File1, cache1));
	TagCache.RegisterGlobal(cache2);
	
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("w"));
	TagCache.ResourceMatches(File1, ParsedVariable, Scope, SourceDirs,
		TagMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(1, TagMatches);
	CHECK_UNISTR_EQUALS("w", TagMatches[0].Identifier);
	CHECK_UNISTR_EQUALS("ActionYou", TagMatches[0].ClassName);
}

TEST_FIXTURE(ExpressionCompletionMatchesFixtureClass, TagMatchesWithStaleMatches) {

	// create a class in global file with methodA
	// file2 will use the class from global file; file2 will be registered
	// then global file will be registered with file2 (invalidating methodA)
	// perform a search
	// methodA should not be a hit since it has been removed
	GlobalCode = wxT("<?php class ActionMy   { function methodA() {} }");
	Code1 = UNICODE_STRING_SIMPLE("<?php $action = new ActionMy(); ");
	Code2 = UNICODE_STRING_SIMPLE("<?php class ActionMy   { function methodB() {} }");

	CreateFixtureFile(GlobalFile, GlobalCode);

	t4p::WorkingCacheClass* cache1 = CreateWorkingCache(File1, Code1);
	t4p::TagFinderListClass* cache2 = CreateTagFinderList(wxT("src"));

	CHECK(TagCache.RegisterWorking(File1, cache1));
	TagCache.RegisterGlobal(cache2);
	
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("methodA"));
	TagCache.ResourceMatches(File1, ParsedVariable, Scope, SourceDirs,
		TagMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(1, TagMatches);
	CHECK_UNISTR_EQUALS("methodA", TagMatches[0].Identifier);
	CHECK_UNISTR_EQUALS("ActionMy", TagMatches[0].ClassName);

	// now update the code by creating a working version of the global code.
	// ie. the user opening a file.
	t4p::WorkingCacheClass* cache3 = CreateWorkingCache(GlobalFile, Code2);
	CHECK(TagCache.RegisterWorking(GlobalFile, cache3));
	
	TagMatches.clear();
	TagCache.ResourceMatches(GlobalFile, ParsedVariable, Scope, SourceDirs,
		TagMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(0, TagMatches);
}

TEST_FIXTURE(TagCacheSearchFixtureClass, ExactTags) {
	wxString code = wxT("<?php class ActionYou  { function w() {} }");
	CreateSubDirectory(wxT("src"));
	CreateFixtureFile(wxT("src") + wxString(wxFileName::GetPathSeparator()) + wxT("file1.php"), code);
	
	t4p::TagFinderListClass* cache = CreateTagFinderList(wxT("src"));
	TagCache.RegisterGlobal(cache);
	
	// empty means search all dirs
	std::vector<wxFileName> searchDirs;
	t4p::TagResultClass* result = TagCache.ExactTags(UNICODE_STRING_SIMPLE("ActionYou"), searchDirs);

	CHECK(result);
	if (!result) {
		return;
	}
	CHECK(result->More());
	result->Next();
	CHECK_UNISTR_EQUALS("ActionYou", result->Tag.Identifier);
	CHECK_UNISTR_EQUALS("ActionYou", result->Tag.ClassName);
	CHECK_EQUAL(false, result->More());

	// do this now so that we dont use the inherited Session
	delete result;
}

}
