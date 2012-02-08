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
#include <windows/StringHelperClass.h>

class CallStackFixtureTestClass : public FileTestFixtureClass {
	
public:

	mvceditor::ResourceCacheClass ResourceCache;
	mvceditor::CallStackClass CallStack;
	std::vector<wxString> PhpFileFilters;
	
	CallStackFixtureTestClass()
		: FileTestFixtureClass(wxT("call_stack")) 
		, ResourceCache() 
		, CallStack(ResourceCache)
		, PhpFileFilters() {
		PhpFileFilters.push_back(wxT("*.php"));
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
		CreateFixtureFile(fileName, contents);
		
		// make the cache consume the file; to prime it with the resources because the
		// call stack wont work without the cache
		mvceditor::DirectorySearchClass search;
		search.Init(TestProjectDir);
		ResourceCache.WalkGlobal(search, PhpFileFilters);
	}
};

SUITE(CallStackTestClass) {

TEST_FIXTURE(CallStackFixtureTestClass, FailOnUnknownResource) {
	SetupFile(wxT("news.php"), Simple());
	wxFileName file(TestProjectDir + wxT("news.php"));
	mvceditor::CallStackClass::Errors error = mvceditor::CallStackClass::NONE;
	CHECK_EQUAL(false, CallStack.Build(file, UNICODE_STRING_SIMPLE("UnknownClass"), UNICODE_STRING_SIMPLE("index"), error));
	CHECK_EQUAL(mvceditor::CallStackClass::RESOURCE_NOT_FOUND, error);
	
	error = mvceditor::CallStackClass::NONE;
	CHECK_EQUAL(false, CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("unknownMethod"), error));
	CHECK_EQUAL(mvceditor::CallStackClass::RESOURCE_NOT_FOUND, error);
}	
 
TEST_FIXTURE(CallStackFixtureTestClass, FailOnParseError) {
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
	
	wxFileName file(TestProjectDir + wxT("news.php"));
	mvceditor::CallStackClass::Errors error = mvceditor::CallStackClass::NONE;
	CHECK_EQUAL(false, CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), error));
	CHECK_EQUAL(file.GetFullPath(), CallStack.LintResults.File);
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
	
	wxFileName file(TestProjectDir + wxT("news.php"));
	mvceditor::CallStackClass::Errors error = mvceditor::CallStackClass::NONE;
	
	// we still want to return true because an incomplete call stack may be helpful in some cases
	CHECK_EQUAL(true, CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), error));
	CHECK_EQUAL(mvceditor::CallStackClass::RESOLUTION_ERROR, error);
	CHECK_EQUAL(mvceditor::SymbolTableMatchErrorClass::UNKNOWN_RESOURCE, CallStack.MatchError.Type);
	
	// since load can be resolved the call stack should have it
	CHECK_VECTOR_SIZE(1, CallStack.List);
	CHECK_UNISTR_EQUALS("CI_Loader::view", CallStack.List[0].Resource.Resource);
}	

TEST_FIXTURE(CallStackFixtureTestClass, FailOnStackLimit) {
	// TODO: not sure how to test this
}	

TEST_FIXTURE(CallStackFixtureTestClass, FailOnEmptyCache) {
	SetupFile(wxT("news.php"), Simple());
	
	mvceditor::ResourceCacheClass localCache;
	mvceditor::CallStackClass localCallStack(localCache);
	
	wxFileName file(TestProjectDir + wxT("news.php"));
	mvceditor::CallStackClass::Errors error = mvceditor::CallStackClass::NONE;
	CHECK_EQUAL(false, localCallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), error));
	CHECK_EQUAL(mvceditor::CallStackClass::EMPTY_CACHE, error);
}

TEST_FIXTURE(CallStackFixtureTestClass, SimpleMethodCall) {
	SetupFile(wxT("news.php"), Simple());
	wxFileName file(TestProjectDir + wxT("news.php"));
	mvceditor::CallStackClass::Errors error = mvceditor::CallStackClass::NONE;
	CHECK(CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), error));
	
	CHECK_EQUAL(mvceditor::CallStackClass::NONE, error);
	CHECK_EQUAL(mvceditor::SymbolTableMatchErrorClass::NONE, CallStack.MatchError.Type);
	CHECK_VECTOR_SIZE(1, CallStack.List);
	CHECK_UNISTR_EQUALS("CI_Loader::view", CallStack.List[0].Resource.Resource);
	CHECK_VECTOR_SIZE(2, CallStack.List[0].Arguments);
	CHECK_UNISTR_EQUALS("index", CallStack.List[0].Arguments[0].Lexeme);
	CHECK_EQUAL(mvceditor::ExpressionClass::SCALAR, CallStack.List[0].Arguments[0].Type);
	CHECK_UNISTR_EQUALS("$data", CallStack.List[0].Arguments[1].Lexeme);
	CHECK_EQUAL(mvceditor::ExpressionClass::VARIABLE, CallStack.List[0].Arguments[1].Type);
}

TEST_FIXTURE(CallStackFixtureTestClass, Persist) {
	SetupFile(wxT("news.php"), Simple());
	wxFileName file(TestProjectDir + wxT("news.php"));
	mvceditor::CallStackClass::Errors error = mvceditor::CallStackClass::NONE;
	CHECK(CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), error));
	CHECK_EQUAL(mvceditor::CallStackClass::NONE, error);
	CHECK_EQUAL(mvceditor::SymbolTableMatchErrorClass::NONE, CallStack.MatchError.Type);
	CHECK_VECTOR_SIZE(1, CallStack.List);
	
	wxFileName newFile(TestProjectDir + wxT("call_stack.txt"));
	CHECK(CallStack.Persist(newFile));
	
	wxString contents;
	wxFFile ffile;
	CHECK(ffile.Open(newFile.GetFullPath()));
	ffile.ReadAll(&contents);
	wxString expected = wxString::FromAscii(
		"METHOD,view,CI_Loader::view,\"index\",\"$data\"\n"
	);
	CHECK_EQUAL(expected, contents);
	
}

}