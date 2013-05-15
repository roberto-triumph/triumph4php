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
#include <globals/Assets.h>
#include <globals/Sqlite.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

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
	wxFileName WorkingTagDbFileName;

	RegisterTestFixtureClass()
		: FileTestFixtureClass(wxT("resource-cache"))
		, TagCache()
		, Finder()
		, Search() 
		, PhpFileExtensions()
		, MiscFileExtensions()
		, Matches() 
		, TagDbFileName()
		, WorkingTagDbFileName() {
		Search.Init(TestProjectDir);
		PhpFileExtensions.push_back(wxT("*.php"));
		
		// create the test dir, since FileTestFixture class is lazy
		if (!wxDirExists(TestProjectDir)) {
			wxMkdir(TestProjectDir, 0777);
		}
		TagDbFileName.Assign(TestProjectDir + wxT("resource_cache.db"));
		WorkingTagDbFileName.Assign(TestProjectDir + wxT("working_resource_cache.db"));

		soci::session session(*soci::factory_sqlite3(), mvceditor::WxToChar(TagDbFileName.GetFullPath()));
		wxString error;
		mvceditor::SqliteSqlScript(mvceditor::ResourceSqlSchemaAsset(), session, error);

		soci::session workingSession(*soci::factory_sqlite3(), mvceditor::WxToChar(WorkingTagDbFileName.GetFullPath()));
		mvceditor::SqliteSqlScript(mvceditor::ResourceSqlSchemaAsset(), workingSession, error);
	}

	void NearMatchTags(const UnicodeString& search) {
		Matches = TagCache.NearMatchTags(search);
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
	wxFileName WorkingTagDbFileName;
	
	ExpressionCompletionMatchesFixtureClass() 
		: FileTestFixtureClass(wxT("resource-cache"))
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
		, ParsedExpression(Scope)
		, VariableMatches()
		, ResourceMatches()
		, Error()
		, Search()
		, PhpFileExtensions()
		, MiscFileExtensions()
		, TagDbFileName() 
		, WorkingTagDbFileName() {
		CreateSubDirectory(wxT("src"));
		Search.Init(TestProjectDir + wxT("src"));
		PhpFileExtensions.push_back(wxT("*.php"));
		Scope.ClassName = UNICODE_STRING_SIMPLE("");
		Scope.MethodName = UNICODE_STRING_SIMPLE("");
		TagDbFileName.Assign(TestProjectDir + wxT("tags.db"));
		WorkingTagDbFileName.Assign(TestProjectDir + wxT("working_tags.db"));

		soci::session session(*soci::factory_sqlite3(), mvceditor::WxToChar(TagDbFileName.GetFullPath()));
		wxString error;
		mvceditor::SqliteSqlScript(mvceditor::ResourceSqlSchemaAsset(), session, error);

		soci::session workingSession(*soci::factory_sqlite3(), mvceditor::WxToChar(WorkingTagDbFileName.GetFullPath()));
		mvceditor::SqliteSqlScript(mvceditor::ResourceSqlSchemaAsset(), workingSession, error);
		session.close();
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
		soci::session session(*soci::factory_sqlite3(), mvceditor::WxToChar(resourceDbFile.GetFullPath()));
		wxString errorString;
		mvceditor::SqliteSqlScript(mvceditor::ResourceSqlSchemaAsset(), session, errorString);

		mvceditor::GlobalCacheClass* cache = new mvceditor::GlobalCacheClass();
		cache->InitGlobalTag(resourceDbFile, PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);

		
		// must call init() here since we want to parse files from disk
		Search.Init(TestProjectDir + srcDirectory);
		cache->Walk(Search);
		return cache;
	}
};

SUITE(ResourceCacheTestClass) {

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
	TagCache.RegisterGlobal(cache2);
	
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("w"));
	TagCache.ExpressionCompletionMatches(File2, ParsedExpression, Scope, 
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
	TagCache.RegisterGlobal(cache2);
	
	ToProperty(UNICODE_STRING_SIMPLE("$action"), UNICODE_STRING_SIMPLE("w"));
	TagCache.ResourceMatches(File1, ParsedExpression, Scope, 
		ResourceMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("w", ResourceMatches[0].Identifier);
	CHECK_UNISTR_EQUALS("ActionYou", ResourceMatches[0].ClassName);
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
	TagCache.RegisterGlobal(cache2);
	
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

}