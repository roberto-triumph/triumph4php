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
#include <MvcEditorChecks.h>
#include <FileTestFixtureClass.h>
#include <php_frameworks/CallStackClass.h>
#include <globals/String.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

class CallStackFixtureTestClass : public FileTestFixtureClass {
	
public:

	mvceditor::TagCacheClass TagCache;
	mvceditor::CallStackClass CallStack;
	std::vector<wxString> PhpFileExtensions;
	std::vector<wxString> MiscFileExtensions;
	wxFileName ResourceDbFileName;
	
	CallStackFixtureTestClass()
		: FileTestFixtureClass(wxT("call_stack")) 
		, TagCache() 
		, CallStack(TagCache)
		, PhpFileExtensions() 
		, MiscFileExtensions() {
		PhpFileExtensions.push_back(wxT("*.php"));
		CreateSubDirectory(wxT("src"));
	}

	wxString Simple() {
		return wxString::FromAscii(
			"<?php\n"
			"class CI_Loader {\n"
			"\tfunction view() {} \n"
			"}\n"
			"\n"
			"class News extends CI_Controller {\n"
			"\t/** @var CI_Loader */\n"
			"\tprivate $load;\n"
			"\tfunction index() {\n"
			"\t\t$data = array('title' => 'Welcome to the News Page');\n"
			"\t\t$this->load->view('index', $data);\n"
			"\t}\n"
			"}\n"
		);
	}
	
	void SetupFile(const wxString& fileName, const wxString& contents) {

		// make the cache consume the file; to prime it with the resources because the
		// call stack wont work without the cache
		CreateFixtureFile(wxT("src") + wxFileName::GetPathSeparators() + fileName, contents);
	}

	void BuildCache() {
		ResourceDbFileName.Assign(TestProjectDir + wxT("resource_cache.db"));
		mvceditor::GlobalCacheClass* globalCache = new mvceditor::GlobalCacheClass;
		globalCache->InitGlobalTag(ResourceDbFileName, PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);
		
		mvceditor::DirectorySearchClass search;
		search.Init(TestProjectDir + wxT("src"));
		while (search.More()) {
			globalCache->Walk(search);
		}
		
		// need to code it so that gcc does not think that good is an unused variable in release mode
		bool good = TagCache.RegisterGlobal(globalCache);
		wxUnusedVar(good);
		wxASSERT_MSG(good, _("global cache could not be registered"));
	}
};

#define CHECK_CALL_STACK(i, type, expression) \
	CHECK_EQUAL(type, CallStack.List[i].Type);\
	CHECK_EQUAL(expression, CallStack.List[i].ExpressionString());

SUITE(CallStackTestClass) {

TEST_FIXTURE(CallStackFixtureTestClass, FailOnUnknownResource) {
	SetupFile(wxT("news.php"), Simple());
	BuildCache();
	wxFileName file(TestProjectDir + wxT("src") + wxFileName::GetPathSeparators() + wxT("news.php"));
	mvceditor::CallStackClass::Errors error = mvceditor::CallStackClass::NONE;
	CHECK_EQUAL(false, CallStack.Build(file, UNICODE_STRING_SIMPLE("UnknownClass"), UNICODE_STRING_SIMPLE("index"), pelet::PHP_53, error));
	CHECK_EQUAL(mvceditor::CallStackClass::RESOURCE_NOT_FOUND, error);
	
	error = mvceditor::CallStackClass::NONE;
	CHECK_EQUAL(false, CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("unknownMethod"), pelet::PHP_53, error));
	CHECK_EQUAL(mvceditor::CallStackClass::RESOURCE_NOT_FOUND, error);
}
 
TEST_FIXTURE(CallStackFixtureTestClass, FailOnParseError) {
	
	// to populate the cache, that way the call stack does not report a empty cache error
	wxString goodCode = wxString::FromAscii(
		"<?php\n"
		"function printUser() {\n"
		"}"
	);
	SetupFile(wxT("user.php"), goodCode);
	
	wxString badCode = wxString::FromAscii(
	
		// missing '{' after 'CI_Controller'
		"<?php\n"
		"class News extends CI_Controller \n"
		"\tfunction index() {\n"
		"\t\t$data = array('title' => 'Welcome to the News Page');\n"
		"\t\t$this->load->view('index', $data);\n"
		"\t}"
		"}"
	);
	SetupFile(wxT("news.php"), badCode);
	BuildCache();
	
	wxFileName file(TestProjectDir + wxT("src") + wxFileName::GetPathSeparators() + wxT("news.php"));
	mvceditor::CallStackClass::Errors error = mvceditor::CallStackClass::NONE;
	CHECK_EQUAL(false, CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), pelet::PHP_53, error));

	UnicodeString expected = mvceditor::WxToIcu(file.GetFullPath());
	CHECK_EQUAL(expected, CallStack.LintResults.UnicodeFilename);
	CHECK_EQUAL(mvceditor::CallStackClass::PARSE_ERR0R, error);
}	

TEST_FIXTURE(CallStackFixtureTestClass, ResolutionError) {
	wxString badCode = wxString::FromAscii(
	
		// missing function $this->work()
		"<?php\n"
		"class CI_Loader {\n"
		"\tfunction view() {} \n"
		"}\n"
		"\n"
		"class News extends CI_Controller {\n"
		"\t/** @var CI_Loader */\n"
		"\tprivate $load;\n"
		"\tfunction index() {\n"
		"\t\t$data = array('title' => 'Welcome to the News Page');\n"
		"\t\t$this->work(); \n"
		"\t\t$this->load->view('index', $data);\n"
		"\t}\n"
		"}"
	);
	SetupFile(wxT("news.php"), badCode);
	BuildCache();
	
	wxFileName file(TestProjectDir + wxT("src") + wxFileName::GetPathSeparators() + wxT("news.php"));
	mvceditor::CallStackClass::Errors error = mvceditor::CallStackClass::NONE;
	
	// we still want to return true because an incomplete call stack may be helpful in some cases
	CHECK_EQUAL(true, CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), pelet::PHP_53, error));
	CHECK_EQUAL(mvceditor::CallStackClass::RESOLUTION_ERROR, error);
	CHECK_EQUAL(mvceditor::SymbolTableMatchErrorClass::UNKNOWN_RESOURCE, CallStack.MatchError.Type);
	
	// since load can be resolved the call stack should have it	
	CHECK_VECTOR_SIZE(7, CallStack.List);
	CHECK_CALL_STACK(0, mvceditor::CallClass::BEGIN_METHOD, "News,index");
	CHECK_CALL_STACK(1, mvceditor::CallClass::ARRAY, "$data,title");
	CHECK_CALL_STACK(2, mvceditor::CallClass::RETURN, "");
	CHECK_CALL_STACK(3, mvceditor::CallClass::BEGIN_METHOD, "CI_Loader,view");
	CHECK_CALL_STACK(4, mvceditor::CallClass::PARAM, "SCALAR,\"index\"");
	CHECK_CALL_STACK(5, mvceditor::CallClass::PARAM, "ARRAY,$data,title");
	CHECK_CALL_STACK(6, mvceditor::CallClass::RETURN, "");
}	

TEST_FIXTURE(CallStackFixtureTestClass, FailOnStackLimit) {
	// TODO: not sure how to test this
}	

TEST_FIXTURE(CallStackFixtureTestClass, FailOnEmptyCache) {
	SetupFile(wxT("news.php"), Simple());
	BuildCache();

	mvceditor::TagCacheClass localCache;
	mvceditor::CallStackClass localCallStack(localCache);
	
	wxFileName file(TestProjectDir + wxT("src") + wxFileName::GetPathSeparators() + wxT("news.php"));
	mvceditor::CallStackClass::Errors error = mvceditor::CallStackClass::NONE;
	CHECK_EQUAL(false, localCallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), pelet::PHP_53, error));
	CHECK_EQUAL(mvceditor::CallStackClass::EMPTY_CACHE, error);
}

TEST_FIXTURE(CallStackFixtureTestClass, SimpleMethodCall) {
	SetupFile(wxT("news.php"), Simple());
	BuildCache();

	wxFileName file(TestProjectDir + wxT("src") + wxFileName::GetPathSeparators() + wxT("news.php"));
	mvceditor::CallStackClass::Errors error = mvceditor::CallStackClass::NONE;
	CHECK(CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), pelet::PHP_53, error));
	
	CHECK_EQUAL(mvceditor::CallStackClass::NONE, error);
	CHECK_EQUAL(mvceditor::SymbolTableMatchErrorClass::NONE, CallStack.MatchError.Type);
	CHECK_VECTOR_SIZE(7, CallStack.List);
	CHECK_CALL_STACK(0, mvceditor::CallClass::BEGIN_METHOD, "News,index");
	CHECK_CALL_STACK(1, mvceditor::CallClass::ARRAY, "$data,title");
	CHECK_CALL_STACK(2, mvceditor::CallClass::RETURN, "");
	CHECK_CALL_STACK(3, mvceditor::CallClass::BEGIN_METHOD, "CI_Loader,view");
	CHECK_CALL_STACK(4, mvceditor::CallClass::PARAM, "SCALAR,\"index\"");
	CHECK_CALL_STACK(5, mvceditor::CallClass::PARAM, "ARRAY,$data,title");
	CHECK_CALL_STACK(6, mvceditor::CallClass::RETURN, "");
}

TEST_FIXTURE(CallStackFixtureTestClass, MultipleMethodCalls) {
	
	// many calls to the view method
	 wxString code = wxString::FromAscii(
			"<?php\n"
			"class CI_Loader {\n"
			"\tfunction view() {} \n"
			"}\n"
			"\n"
			"class News extends CI_Controller {\n"
			"\t/** @var CI_Loader */\n"
			"\tprivate $load;\n"
			"\tfunction index() {\n"
			"\t\t$data = array('title' => 'Welcome to the News Page');\n"
			"\t\t$this->load->view('header');\n"
			"\t\t$this->load->view('index', $data);\n"
			"\t\t$this->load->view('footer');\n"
			"\t}\n"
			"}\n"
		);
	SetupFile(wxT("news.php"), code);
	BuildCache();

	wxFileName file(TestProjectDir + wxT("src") + wxFileName::GetPathSeparators() + wxT("news.php"));
	mvceditor::CallStackClass::Errors error = mvceditor::CallStackClass::NONE;
	CHECK(CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), pelet::PHP_53, error));
	CHECK_EQUAL(mvceditor::CallStackClass::NONE, error);
	CHECK_EQUAL(mvceditor::SymbolTableMatchErrorClass::NONE, CallStack.MatchError.Type);
	CHECK_VECTOR_SIZE(13, CallStack.List);
	CHECK_CALL_STACK(0, mvceditor::CallClass::BEGIN_METHOD, "News,index");
	CHECK_CALL_STACK(1, mvceditor::CallClass::ARRAY, "$data,title");
	CHECK_CALL_STACK(2, mvceditor::CallClass::RETURN, "");
	CHECK_CALL_STACK(3, mvceditor::CallClass::BEGIN_METHOD, "CI_Loader,view");
	CHECK_CALL_STACK(4, mvceditor::CallClass::PARAM, "SCALAR,\"header\"");
	CHECK_CALL_STACK(5, mvceditor::CallClass::RETURN, "");
	CHECK_CALL_STACK(6, mvceditor::CallClass::BEGIN_METHOD, "CI_Loader,view");
	CHECK_CALL_STACK(7, mvceditor::CallClass::PARAM, "SCALAR,\"index\"");
	CHECK_CALL_STACK(8, mvceditor::CallClass::PARAM, "ARRAY,$data,title");
	CHECK_CALL_STACK(9, mvceditor::CallClass::RETURN, "");
	CHECK_CALL_STACK(10, mvceditor::CallClass::BEGIN_METHOD, "CI_Loader,view");
	CHECK_CALL_STACK(11, mvceditor::CallClass::PARAM, "SCALAR,\"footer\"");
	CHECK_CALL_STACK(12, mvceditor::CallClass::RETURN, "");
}

TEST_FIXTURE(CallStackFixtureTestClass, WithArrayKeyAssignment) {
	wxString code = wxString::FromAscii(
		"<?php\n"
		"class CI_Loader {\n"
		"\tfunction view() {} \n"
		"}\n"
		"\n"
		"class News extends CI_Controller {\n"
		"\t/** @var CI_Loader */\n"
		"\tprivate $load;\n"
		"\tfunction index() {\n"
		"\t\t$data['title'] = 'Welcome to the News Page';\n"
		"\t\t$data['name'] = 'Hello';\n"
		"\t\t$this->load->view('index', $data);\n"
		"\t}\n"
		"}\n"
	);
	SetupFile(wxT("news.php"), code);
	BuildCache();

	wxFileName file(TestProjectDir + wxT("src") + wxFileName::GetPathSeparators() + wxT("news.php"));
	mvceditor::CallStackClass::Errors error = mvceditor::CallStackClass::NONE;
	CHECK(CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), pelet::PHP_53, error));
	CHECK_EQUAL(mvceditor::CallStackClass::NONE, error);
	CHECK_EQUAL(mvceditor::SymbolTableMatchErrorClass::NONE, CallStack.MatchError.Type);
	CHECK_VECTOR_SIZE(7, CallStack.List);
	CHECK_CALL_STACK(0, mvceditor::CallClass::BEGIN_METHOD, "News,index");
	CHECK_CALL_STACK(1, mvceditor::CallClass::ARRAY, "$data,title,name");
	CHECK_CALL_STACK(2, mvceditor::CallClass::RETURN, "");
	CHECK_CALL_STACK(3, mvceditor::CallClass::BEGIN_METHOD, "CI_Loader,view");
	CHECK_CALL_STACK(4, mvceditor::CallClass::PARAM, "SCALAR,\"index\"");
	CHECK_CALL_STACK(5, mvceditor::CallClass::PARAM, "ARRAY,$data,title,name");
	CHECK_CALL_STACK(6, mvceditor::CallClass::RETURN, "");
}


TEST_FIXTURE(CallStackFixtureTestClass, WithMethodCall) {
	wxString code = wxString::FromAscii(
		"<?php\n"
		"class CI_Loader {\n"
		"\tfunction defaultVars() {} \n"
		"\n"
		"\tfunction view() {} \n"
		"}\n"
		"\n"
		"class News extends CI_Controller {\n"
		"\t/** @var CI_Loader */\n"
		"\tprivate $load;\n"
		"\tfunction index() {\n"
		"\t\t$data['title'] = $this->load->defaultVars();\n"
		"\t\t$data['name'] = 'Hello';\n"
		"\t\t$this->load->view('index', $data);\n"
		"\t}\n"
		"}\n"
	);
	SetupFile(wxT("news.php"), code);
	BuildCache();
	
	wxFileName file(TestProjectDir + wxT("src") + wxFileName::GetPathSeparators() + wxT("news.php"));
	mvceditor::CallStackClass::Errors error = mvceditor::CallStackClass::NONE;
	CHECK(CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), pelet::PHP_53, error));
	CHECK_EQUAL(mvceditor::CallStackClass::NONE, error);
	CHECK_EQUAL(mvceditor::SymbolTableMatchErrorClass::NONE, CallStack.MatchError.Type);
	CHECK_VECTOR_SIZE(9, CallStack.List);
	CHECK_CALL_STACK(0, mvceditor::CallClass::BEGIN_METHOD, "News,index");
	CHECK_CALL_STACK(1, mvceditor::CallClass::ARRAY, "$data,title,name");
	CHECK_CALL_STACK(2, mvceditor::CallClass::RETURN, "");
	CHECK_CALL_STACK(3, mvceditor::CallClass::BEGIN_METHOD, "CI_Loader,defaultVars");
	CHECK_CALL_STACK(4, mvceditor::CallClass::RETURN, "");
	CHECK_CALL_STACK(5, mvceditor::CallClass::BEGIN_METHOD, "CI_Loader,view");
	CHECK_CALL_STACK(6, mvceditor::CallClass::PARAM, "SCALAR,\"index\"");
	CHECK_CALL_STACK(7, mvceditor::CallClass::PARAM, "ARRAY,$data,title,name");
	CHECK_CALL_STACK(8, mvceditor::CallClass::RETURN, "");
}

TEST_FIXTURE(CallStackFixtureTestClass, Persist) {
	SetupFile(wxT("news.php"), Simple());
	BuildCache();
	wxFileName detectorDbFileName(TestProjectDir, wxT("detectors.db"));

	wxFileName file(TestProjectDir + wxT("src") + wxFileName::GetPathSeparators() + wxT("news.php"));
	mvceditor::CallStackClass::Errors error = mvceditor::CallStackClass::NONE;
	CHECK(CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), pelet::PHP_53, error));
	CHECK(CallStack.Persist(detectorDbFileName));

	CHECK_EQUAL(mvceditor::CallStackClass::NONE, error);
	CHECK_EQUAL(mvceditor::SymbolTableMatchErrorClass::NONE, CallStack.MatchError.Type);
	CHECK_EQUAL((size_t)7, CallStack.List.size());

	// now check the sqlite db contents
	soci::session session(*soci::factory_sqlite3(), mvceditor::WxToChar(detectorDbFileName.GetFullPath()));
	int stepNumber;
	std::string type;
	std::string expression;
	int rowCount;

	session.once << "SELECT COUNT(*) FROM call_stacks", soci::into(rowCount);
	CHECK_EQUAL(7, rowCount);

	soci::statement stmt = (session.prepare <<
		"SELECT step_number, step_type, expression FROM call_stacks",
		soci::into(stepNumber), soci::into(type), soci::into(expression)
	);
	CHECK(stmt.execute(true));
	CHECK_EQUAL(0, stepNumber);
	CHECK_EQUAL("BEGIN_METHOD", type);
	CHECK_EQUAL("News,index", expression);

	CHECK(stmt.fetch());
	CHECK_EQUAL(1, stepNumber);
	CHECK_EQUAL("ARRAY", type);
	CHECK_EQUAL("$data,title", expression);

	CHECK(stmt.fetch());
	CHECK_EQUAL(2, stepNumber);
	CHECK_EQUAL("RETURN", type);
	CHECK_EQUAL("", expression);

	CHECK(stmt.fetch());
	CHECK_EQUAL(3, stepNumber);
	CHECK_EQUAL("BEGIN_METHOD", type);
	CHECK_EQUAL("CI_Loader,view", expression);

	CHECK(stmt.fetch());
	CHECK_EQUAL(4, stepNumber);
	CHECK_EQUAL("PARAM", type);
	CHECK_EQUAL("SCALAR,\"index\"", expression);

	CHECK(stmt.fetch());
	CHECK_EQUAL(5, stepNumber);
	CHECK_EQUAL("PARAM", type);
	CHECK_EQUAL("ARRAY,$data,title", expression);

	CHECK(stmt.fetch());
	CHECK_EQUAL(6, stepNumber);
	CHECK_EQUAL("RETURN", type);
	CHECK_EQUAL("", expression);

}

}