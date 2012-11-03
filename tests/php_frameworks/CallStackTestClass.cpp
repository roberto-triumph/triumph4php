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

class CallStackFixtureTestClass : public FileTestFixtureClass {
	
public:

	mvceditor::ResourceCacheClass ResourceCache;
	mvceditor::CallStackClass CallStack;
	std::vector<wxString> PhpFileExtensions;
	std::vector<wxString> MiscFileExtensions;
	wxFileName ResourceDbFileName;
	
	CallStackFixtureTestClass()
		: FileTestFixtureClass(wxT("call_stack")) 
		, ResourceCache() 
		, CallStack(ResourceCache)
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
		globalCache->Init(ResourceDbFileName, PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);
		
		mvceditor::DirectorySearchClass search;
		search.Init(TestProjectDir + wxT("src"));
		while (search.More()) {
			globalCache->Walk(search);
		}
		
		// need to code it so that gcc does not think that good is an unused variable in release mode
		bool good = ResourceCache.RegisterGlobal(globalCache);
		wxUnusedVar(good);
		wxASSERT_MSG(good, _("global cache could not be registered"));
	}
};

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
	wxFileName newFile(TestProjectDir + wxT("call_stack.txt"));
	CHECK(CallStack.Persist(newFile));
	
	wxString contents;
	wxFFile ffile;
	CHECK(ffile.Open(newFile.GetFullPath()));
	ffile.ReadAll(&contents);
	wxString expected = wxString::FromAscii(
		"BEGIN_METHOD,News,index\n"
		"ARRAY,$data,title\n"
		"RETURN\n"
		"BEGIN_METHOD,CI_Loader,view\n"
		"PARAM,SCALAR,\"index\"\n"
		"PARAM,ARRAY,$data,title\n"
		"RETURN\n"
	);
	CHECK_EQUAL(expected, contents);
}	

TEST_FIXTURE(CallStackFixtureTestClass, FailOnStackLimit) {
	// TODO: not sure how to test this
}	

TEST_FIXTURE(CallStackFixtureTestClass, FailOnEmptyCache) {
	SetupFile(wxT("news.php"), Simple());
	BuildCache();

	mvceditor::ResourceCacheClass localCache;
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
	
	CHECK_EQUAL(mvceditor::CallClass::BEGIN_METHOD, CallStack.List[0].Type);
	CHECK_UNISTR_EQUALS("index", CallStack.List[0].Resource.Identifier);
	CHECK_UNISTR_EQUALS("News", CallStack.List[0].Resource.ClassName);
	
	CHECK_EQUAL(mvceditor::CallClass::ARRAY, CallStack.List[1].Type);
	CHECK_UNISTR_EQUALS("$data", CallStack.List[1].Symbol.Variable);
	CHECK_VECTOR_SIZE(1, CallStack.List[1].Symbol.ArrayKeys);
	CHECK_UNISTR_EQUALS("title", CallStack.List[1].Symbol.ArrayKeys[0]);
	
	CHECK_EQUAL(mvceditor::CallClass::RETURN, CallStack.List[2].Type);
	
	CHECK_EQUAL(mvceditor::CallClass::BEGIN_METHOD, CallStack.List[3].Type);
	CHECK_UNISTR_EQUALS("view", CallStack.List[3].Resource.Identifier);
	CHECK_UNISTR_EQUALS("CI_Loader", CallStack.List[3].Resource.ClassName);
	
	CHECK_EQUAL(mvceditor::CallClass::PARAM, CallStack.List[4].Type);
	CHECK_UNISTR_EQUALS("index", CallStack.List[4].Expression.FirstValue());
	CHECK_EQUAL(pelet::ExpressionClass::SCALAR, CallStack.List[4].Expression.ExpressionType);
	
	CHECK_EQUAL(mvceditor::CallClass::PARAM, CallStack.List[5].Type);
	CHECK_UNISTR_EQUALS("$data", CallStack.List[5].Expression.FirstValue());
	CHECK_EQUAL(mvceditor::SymbolClass::ARRAY, CallStack.List[5].Symbol.Type);
	CHECK_VECTOR_SIZE(1, CallStack.List[5].Symbol.ArrayKeys);
	CHECK_UNISTR_EQUALS("title", CallStack.List[5].Symbol.ArrayKeys[0]);
	
	CHECK_EQUAL(mvceditor::CallClass::RETURN, CallStack.List[6].Type);
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
	
	wxFileName newFile(TestProjectDir + wxT("call_stack.txt"));
	CHECK(CallStack.Persist(newFile));
	
	wxString contents;
	wxFFile ffile;
	CHECK(ffile.Open(newFile.GetFullPath()));
	ffile.ReadAll(&contents);
	wxString expected = wxString::FromAscii(
		"BEGIN_METHOD,News,index\n"
		"ARRAY,$data,title\n"
		"RETURN\n"
		"BEGIN_METHOD,CI_Loader,view\n"
		"PARAM,SCALAR,\"header\"\n"
		"RETURN\n"
		"BEGIN_METHOD,CI_Loader,view\n"
		"PARAM,SCALAR,\"index\"\n"
		"PARAM,ARRAY,$data,title\n"
		"RETURN\n"
		"BEGIN_METHOD,CI_Loader,view\n"
		"PARAM,SCALAR,\"footer\"\n"
		"RETURN\n"
	);
	CHECK_EQUAL(expected, contents);
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
	
	wxFileName newFile(TestProjectDir + wxT("call_stack.txt"));
	CHECK(CallStack.Persist(newFile));
	
	wxString contents;
	wxFFile ffile;
	CHECK(ffile.Open(newFile.GetFullPath()));
	ffile.ReadAll(&contents);
	wxString expected = wxString::FromAscii(
		"BEGIN_METHOD,News,index\n"
		"ARRAY,$data,title,name\n"
		"RETURN\n"
		"BEGIN_METHOD,CI_Loader,view\n"
		"PARAM,SCALAR,\"index\"\n"
		"PARAM,ARRAY,$data,title,name\n"
		"RETURN\n"
	);
	CHECK_EQUAL(expected, contents);
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
	
	wxFileName newFile(TestProjectDir + wxT("call_stack.txt"));
	CHECK(CallStack.Persist(newFile));
	
	wxString contents;
	wxFFile ffile;
	CHECK(ffile.Open(newFile.GetFullPath()));
	ffile.ReadAll(&contents);
	wxString expected = wxString::FromAscii(
		"BEGIN_METHOD,News,index\n"
		"ARRAY,$data,title,name\n"
		"RETURN\n"
		"BEGIN_METHOD,CI_Loader,defaultVars\n"
		"RETURN\n"
		"BEGIN_METHOD,CI_Loader,view\n"
		"PARAM,SCALAR,\"index\"\n"
		"PARAM,ARRAY,$data,title,name\n"
		"RETURN\n"
	);
	CHECK_EQUAL(expected, contents);
}

TEST_FIXTURE(CallStackFixtureTestClass, Persist) {
	SetupFile(wxT("news.php"), Simple());
	BuildCache();

	wxFileName file(TestProjectDir + wxT("src") + wxFileName::GetPathSeparators() + wxT("news.php"));
	mvceditor::CallStackClass::Errors error = mvceditor::CallStackClass::NONE;
	CHECK(CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), pelet::PHP_53, error));
	CHECK_EQUAL(mvceditor::CallStackClass::NONE, error);
	CHECK_EQUAL(mvceditor::SymbolTableMatchErrorClass::NONE, CallStack.MatchError.Type);
	CHECK_VECTOR_SIZE(7, CallStack.List);
	
	wxFileName newFile(TestProjectDir + wxT("call_stack.txt"));
	CHECK(CallStack.Persist(newFile));
	
	wxString contents;
	wxFFile ffile;
	CHECK(ffile.Open(newFile.GetFullPath()));
	ffile.ReadAll(&contents);
	wxString expected = wxString::FromAscii(
		"BEGIN_METHOD,News,index\n"
		"ARRAY,$data,title\n"
		"RETURN\n"
		"BEGIN_METHOD,CI_Loader,view\n"
		"PARAM,SCALAR,\"index\"\n"
		"PARAM,ARRAY,$data,title\n"
		"RETURN\n"
	);
	CHECK_EQUAL(expected, contents);	
}

}