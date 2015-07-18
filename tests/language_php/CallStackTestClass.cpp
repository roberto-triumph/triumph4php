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
#include <TriumphChecks.h>
#include <FileTestFixtureClass.h>
#include <SqliteTestFixtureClass.h>
#include <language_php/CallStackClass.h>
#include <language_php/TagFinderList.h>
#include <globals/String.h>
#include <globals/Sqlite.h>
#include <globals/Assets.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

class CallStackFixtureTestClass : public FileTestFixtureClass, public SqliteTestFixtureClass {

public:

	t4p::TagCacheClass TagCache;
	t4p::CallStackClass CallStack;
	std::vector<wxString> PhpFileExtensions;
	std::vector<wxString> MiscFileExtensions;

	CallStackFixtureTestClass()
		: FileTestFixtureClass(wxT("call_stack"))
		, SqliteTestFixtureClass(t4p::ResourceSqlSchemaAsset())
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
			"}\n");
	}

	void SetupFile(const wxString& fileName, const wxString& contents) {

		// make the cache consume the file; to prime it with the resources because the
		// call stack wont work without the cache
		CreateFixtureFile(wxT("src") + wxFileName::GetPathSeparators() + fileName, contents);
	}

	void BuildCache() {
		t4p::TagFinderListClass* tagFinderList = new t4p::TagFinderListClass;
		tagFinderList->CreateGlobalTag(PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);

		t4p::DirectorySearchClass search;
		search.Init(TestProjectDir + wxT("src"));
		while (search.More()) {
			tagFinderList->Walk(search);
		}

		// need to code it so that gcc does not think that good is an unused variable in release mode
		TagCache.RegisterGlobal(tagFinderList);
	}
};

#define CHECK_SYMBOL_IS_ASSIGN(i, destinationVariable, sourceVariable) \
		CHECK_EQUAL(t4p::VariableSymbolClass::ASSIGN, CallStack.Variables[i].Type);\
		CHECK_UNISTR_EQUALS(destinationVariable, CallStack.Variables[i].DestinationVariable);\
		CHECK_UNISTR_EQUALS(sourceVariable, CallStack.Variables[i].SourceVariable);

#define CHECK_SYMBOL_IS_ARRAY(i, destinationVariable) \
	CHECK_EQUAL(t4p::VariableSymbolClass::ARRAY, CallStack.Variables[i].Type);\
	CHECK_UNISTR_EQUALS(destinationVariable, CallStack.Variables[i].DestinationVariable);

#define CHECK_SYMBOL_IS_ARRAY_KEY(i, destinationVariable, keyName) \
	CHECK_EQUAL(t4p::VariableSymbolClass::ARRAY_KEY, CallStack.Variables[i].Type);\
	CHECK_UNISTR_EQUALS(destinationVariable, CallStack.Variables[i].DestinationVariable);\
	CHECK_UNISTR_EQUALS(keyName, CallStack.Variables[i].ArrayKey);

#define CHECK_SYMBOL_IS_FUNCTION_CALL(i, destinationVariable, functionName) \
	CHECK_EQUAL(t4p::VariableSymbolClass::FUNCTION_CALL, CallStack.Variables[i].Type);\
	CHECK_UNISTR_EQUALS(destinationVariable, CallStack.Variables[i].DestinationVariable);\
	CHECK_UNISTR_EQUALS(functionName, CallStack.Variables[i].FunctionName);

#define CHECK_SYMBOL_IS_METHOD_CALL(i, destinationVariable, objectName, methodName) \
	CHECK_EQUAL(t4p::VariableSymbolClass::METHOD_CALL, CallStack.Variables[i].Type);\
	CHECK_UNISTR_EQUALS(destinationVariable, CallStack.Variables[i].DestinationVariable);\
	CHECK_UNISTR_EQUALS(objectName, CallStack.Variables[i].ObjectName);\
	CHECK_UNISTR_EQUALS(methodName, CallStack.Variables[i].MethodName);

#define CHECK_SYMBOL_IS_METHOD_CALL_ARG(i, argIndex, argName) \
	CHECK((size_t)argIndex < CallStack.Variables[i].FunctionArguments.size());\
	CHECK_UNISTR_EQUALS(argName, CallStack.Variables[i].FunctionArguments[argIndex]);

#define CHECK_SYMBOL_IS_NEW_OBJECT(i, destinationVariable, className) \
	CHECK_EQUAL(t4p::VariableSymbolClass::NEW_OBJECT, CallStack.Variables[i].Type);\
	CHECK_UNISTR_EQUALS(destinationVariable, CallStack.Variables[i].DestinationVariable);\
	CHECK_UNISTR_EQUALS(className, CallStack.Variables[i].ClassName);

#define CHECK_SYMBOL_IS_PROPERTY(i, destinationVariable, objectName, propertyName) \
	CHECK_EQUAL(t4p::VariableSymbolClass::PROPERTY, CallStack.Variables[i].Type);\
	CHECK_UNISTR_EQUALS(destinationVariable, CallStack.Variables[i].DestinationVariable);\
	CHECK_UNISTR_EQUALS(objectName, CallStack.Variables[i].ObjectName);\
	CHECK_UNISTR_EQUALS(propertyName, CallStack.Variables[i].PropertyName);

#define CHECK_SYMBOL_IS_SCALAR(i, destinationVariable, scalar) \
	CHECK_EQUAL(t4p::VariableSymbolClass::SCALAR, CallStack.Variables[i].Type);\
	CHECK_UNISTR_EQUALS(destinationVariable, CallStack.Variables[i].DestinationVariable);\
	CHECK_UNISTR_EQUALS(scalar, CallStack.Variables[i].ScalarValue);

#define CHECK_SYMBOL_IS_BEGIN_METHOD(i, className, methodName) \
	CHECK_EQUAL(t4p::VariableSymbolClass::BEGIN_METHOD, CallStack.Variables[i].Type);\
	CHECK_UNISTR_EQUALS(className, CallStack.Variables[i].ClassName);\
	CHECK_UNISTR_EQUALS(methodName, CallStack.Variables[i].MethodName);

SUITE(CallStackTestClass) {

TEST_FIXTURE(CallStackFixtureTestClass, FailOnUnknownResource) {
	SetupFile(wxT("news.php"), Simple());
	BuildCache();
	wxFileName file(TestProjectDir + wxT("src") + wxFileName::GetPathSeparators() + wxT("news.php"));
	t4p::CallStackClass::Errors error = t4p::CallStackClass::NONE;
	CHECK_EQUAL(false, CallStack.Build(file, UNICODE_STRING_SIMPLE("UnknownClass"), UNICODE_STRING_SIMPLE("index"), pelet::PHP_53, error));
	CHECK_EQUAL(t4p::CallStackClass::RESOURCE_NOT_FOUND, error);

	error = t4p::CallStackClass::NONE;
	CHECK_EQUAL(false, CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("unknownMethod"), pelet::PHP_53, error));
	CHECK_EQUAL(t4p::CallStackClass::RESOURCE_NOT_FOUND, error);
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
	t4p::CallStackClass::Errors error = t4p::CallStackClass::NONE;
	CHECK_EQUAL(false, CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), pelet::PHP_53, error));

	UnicodeString expected = t4p::WxToIcu(file.GetFullPath());
	CHECK_EQUAL(expected, CallStack.LintResults.UnicodeFilename);
	CHECK_EQUAL(t4p::CallStackClass::PARSE_ERR0R, error);
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
	t4p::CallStackClass::Errors error = t4p::CallStackClass::NONE;

	// we still want to return true because an incomplete call stack may be helpful in some cases
	CHECK_EQUAL(true, CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), pelet::PHP_53, error));
	CHECK_EQUAL(t4p::CallStackClass::NONE, error);
	CHECK_EQUAL(t4p::SymbolTableMatchErrorClass::NONE, CallStack.MatchError.Type);

	CHECK_VECTOR_SIZE(9, CallStack.Variables);

	CHECK_SYMBOL_IS_BEGIN_METHOD(0, "News", "index");
	CHECK_SYMBOL_IS_ARRAY(1, "$@tmp1");
	CHECK_SYMBOL_IS_ARRAY_KEY(2, "$@tmp1", "title");
	CHECK_SYMBOL_IS_ASSIGN(3, "$data", "$@tmp1");
	CHECK_SYMBOL_IS_ASSIGN(4, "$this", "");
	CHECK_SYMBOL_IS_METHOD_CALL(5, "$@tmp2", "$this", "work");
	CHECK_SYMBOL_IS_PROPERTY(6, "$@tmp3", "$this", "load");
	CHECK_SYMBOL_IS_SCALAR(7, "$@tmp4", "index");

	CHECK_SYMBOL_IS_METHOD_CALL(8, "$@tmp5", "$@tmp3", "view");
	CHECK_SYMBOL_IS_METHOD_CALL_ARG(8, 0, "$@tmp4");
	CHECK_SYMBOL_IS_METHOD_CALL_ARG(8, 1, "$data");
}

TEST_FIXTURE(CallStackFixtureTestClass, FailOnStackLimit) {
	// TODO(roberto): not sure how to test this
}

TEST_FIXTURE(CallStackFixtureTestClass, FailOnEmptyCache) {
	SetupFile(wxT("news.php"), Simple());
	BuildCache();

	t4p::TagCacheClass localCache;
	t4p::CallStackClass localCallStack(localCache);

	wxFileName file(TestProjectDir + wxT("src") + wxFileName::GetPathSeparators() + wxT("news.php"));
	t4p::CallStackClass::Errors error = t4p::CallStackClass::NONE;
	CHECK_EQUAL(false, localCallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), pelet::PHP_53, error));
	CHECK_EQUAL(t4p::CallStackClass::EMPTY_CACHE, error);
}

TEST_FIXTURE(CallStackFixtureTestClass, SimpleMethodCall) {
	SetupFile(wxT("news.php"), Simple());
	BuildCache();

	wxFileName file(TestProjectDir + wxT("src") + wxFileName::GetPathSeparators() + wxT("news.php"));
	t4p::CallStackClass::Errors error = t4p::CallStackClass::NONE;
	CHECK(CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), pelet::PHP_53, error));

	CHECK_EQUAL(t4p::CallStackClass::NONE, error);
	CHECK_EQUAL(t4p::SymbolTableMatchErrorClass::NONE, CallStack.MatchError.Type);

	CHECK_VECTOR_SIZE(8, CallStack.Variables);
	CHECK_SYMBOL_IS_BEGIN_METHOD(0, "News", "index");
	CHECK_SYMBOL_IS_ARRAY(1, "$@tmp1");
	CHECK_SYMBOL_IS_ARRAY_KEY(2, "$@tmp1", "title");
	CHECK_SYMBOL_IS_ASSIGN(3, "$data", "$@tmp1");
	CHECK_SYMBOL_IS_ASSIGN(4, "$this", "");
	CHECK_SYMBOL_IS_PROPERTY(5, "$@tmp2", "$this", "load");
	CHECK_SYMBOL_IS_SCALAR(6, "$@tmp3", "index");

	CHECK_SYMBOL_IS_METHOD_CALL(7, "$@tmp4", "$@tmp2", "view");
	CHECK_SYMBOL_IS_METHOD_CALL_ARG(7, 0, "$@tmp3");
	CHECK_SYMBOL_IS_METHOD_CALL_ARG(7, 1, "$data");
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
			"}\n");
	SetupFile(wxT("news.php"), code);
	BuildCache();

	wxFileName file(TestProjectDir + wxT("src") + wxFileName::GetPathSeparators() + wxT("news.php"));
	t4p::CallStackClass::Errors error = t4p::CallStackClass::NONE;
	CHECK(CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), pelet::PHP_53, error));
	CHECK_EQUAL(t4p::CallStackClass::NONE, error);
	CHECK_EQUAL(t4p::SymbolTableMatchErrorClass::NONE, CallStack.MatchError.Type);

	CHECK_VECTOR_SIZE(14, CallStack.Variables);
	CHECK_SYMBOL_IS_BEGIN_METHOD(0, "News", "index");
	CHECK_SYMBOL_IS_ARRAY(1, "$@tmp1");
	CHECK_SYMBOL_IS_ARRAY_KEY(2, "$@tmp1", "title");
	CHECK_SYMBOL_IS_ASSIGN(3, "$data", "$@tmp1");
	CHECK_SYMBOL_IS_ASSIGN(4, "$this", "");
	CHECK_SYMBOL_IS_PROPERTY(5, "$@tmp2", "$this", "load");
	CHECK_SYMBOL_IS_SCALAR(6, "$@tmp3", "header");
	CHECK_SYMBOL_IS_METHOD_CALL(7, "$@tmp4", "$@tmp2", "view");
	CHECK_SYMBOL_IS_METHOD_CALL_ARG(7, 0, "$@tmp3");

	CHECK_SYMBOL_IS_PROPERTY(8, "$@tmp5", "$this", "load");
	CHECK_SYMBOL_IS_SCALAR(9, "$@tmp6", "index");
	CHECK_SYMBOL_IS_METHOD_CALL(10, "$@tmp7", "$@tmp5", "view");
	CHECK_SYMBOL_IS_METHOD_CALL_ARG(10, 0, "$@tmp6");

	CHECK_SYMBOL_IS_PROPERTY(11, "$@tmp8", "$this", "load");
	CHECK_SYMBOL_IS_SCALAR(12, "$@tmp9", "footer");
	CHECK_SYMBOL_IS_METHOD_CALL(13, "$@tmp10", "$@tmp8", "view");
	CHECK_SYMBOL_IS_METHOD_CALL_ARG(13, 0, "$@tmp9");
}

TEST_FIXTURE(CallStackFixtureTestClass, MultiplePropertyCalls) {

	// a property assignment on a long variable
	 wxString code = wxString::FromAscii(
			"<?php\n"
			"class News extends CI_Controller {\n"
			"\tfunction index() {\n"
			"\t\t$this->load->title = $this->title;\n"
			"\t}\n"
			"}\n");
	SetupFile(wxT("news.php"), code);
	BuildCache();

	wxFileName file(TestProjectDir + wxT("src") + wxFileName::GetPathSeparators() + wxT("news.php"));
	t4p::CallStackClass::Errors error = t4p::CallStackClass::NONE;
	CHECK(CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), pelet::PHP_53, error));
	CHECK_EQUAL(t4p::CallStackClass::NONE, error);
	CHECK_EQUAL(t4p::SymbolTableMatchErrorClass::NONE, CallStack.MatchError.Type);

	CHECK_VECTOR_SIZE(6, CallStack.Variables);
	CHECK_SYMBOL_IS_BEGIN_METHOD(0, "News", "index");
	CHECK_SYMBOL_IS_ASSIGN(1, "$this", "");
	CHECK_SYMBOL_IS_PROPERTY(2, "$@tmp1", "$this", "title");
	CHECK_SYMBOL_IS_PROPERTY(3, "$@tmp2", "$this", "load");
	CHECK_SYMBOL_IS_PROPERTY(4, "$@tmp3", "$@tmp2", "title");
	CHECK_SYMBOL_IS_ASSIGN(5, "$@tmp3", "$@tmp1");
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
	t4p::CallStackClass::Errors error = t4p::CallStackClass::NONE;
	CHECK(CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), pelet::PHP_53, error));
	CHECK_EQUAL(t4p::CallStackClass::NONE, error);
	CHECK_EQUAL(t4p::SymbolTableMatchErrorClass::NONE, CallStack.MatchError.Type);

	CHECK_VECTOR_SIZE(10, CallStack.Variables);
	CHECK_SYMBOL_IS_BEGIN_METHOD(0, "News", "index");
	CHECK_SYMBOL_IS_SCALAR(1, "$@tmp1", "Welcome to the News Page");
	CHECK_SYMBOL_IS_ARRAY(2, "$data");
	CHECK_SYMBOL_IS_ARRAY_KEY(3, "$data", "title");
	CHECK_SYMBOL_IS_SCALAR(4, "$@tmp2", "Hello");
	CHECK_SYMBOL_IS_ARRAY_KEY(5, "$data", "name");
	CHECK_SYMBOL_IS_ASSIGN(6, "$this", "");
	CHECK_SYMBOL_IS_PROPERTY(7, "$@tmp3", "$this", "load");
	CHECK_SYMBOL_IS_SCALAR(8, "$@tmp4", "index");
	CHECK_SYMBOL_IS_METHOD_CALL(9, "$@tmp5", "$@tmp3", "view");
	CHECK_SYMBOL_IS_METHOD_CALL_ARG(9, 0, "$@tmp4");
	CHECK_SYMBOL_IS_METHOD_CALL_ARG(9, 1, "$data");
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
	t4p::CallStackClass::Errors error = t4p::CallStackClass::NONE;
	CHECK(CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), pelet::PHP_53, error));
	CHECK_EQUAL(t4p::CallStackClass::NONE, error);
	CHECK_EQUAL(t4p::SymbolTableMatchErrorClass::NONE, CallStack.MatchError.Type);

	CHECK_VECTOR_SIZE(11, CallStack.Variables);
	CHECK_SYMBOL_IS_BEGIN_METHOD(0, "News", "index");
	CHECK_SYMBOL_IS_ASSIGN(1, "$this", "");
	CHECK_SYMBOL_IS_PROPERTY(2, "$@tmp1", "$this", "load");
	CHECK_SYMBOL_IS_METHOD_CALL(3, "$@tmp2", "$@tmp1", "defaultVars");
	CHECK_SYMBOL_IS_ARRAY(4, "$data");
	CHECK_SYMBOL_IS_ARRAY_KEY(5, "$data", "title");
	CHECK_SYMBOL_IS_SCALAR(6, "$@tmp3", "Hello");
	CHECK_SYMBOL_IS_ARRAY_KEY(7, "$data", "name");
	CHECK_SYMBOL_IS_PROPERTY(8, "$@tmp4", "$this", "load");
	CHECK_SYMBOL_IS_SCALAR(9, "$@tmp5", "index");
	CHECK_SYMBOL_IS_METHOD_CALL(10, "$@tmp6", "$@tmp4", "view");

	CHECK_VECTOR_SIZE(2, CallStack.Variables[10].FunctionArguments);
	CHECK_SYMBOL_IS_METHOD_CALL_ARG(10, 0, "$@tmp5");
	CHECK_SYMBOL_IS_METHOD_CALL_ARG(10, 1, "$data");
}

TEST_FIXTURE(CallStackFixtureTestClass, Persist) {
	SetupFile(wxT("news.php"), Simple());
	BuildCache();

	soci::session session(*soci::factory_sqlite3(), ":memory:");
	CreateDatabase(session, t4p::DetectorSqlSchemaAsset());

	wxFileName file(TestProjectDir + wxT("src") + wxFileName::GetPathSeparators() + wxT("news.php"));
	t4p::CallStackClass::Errors error = t4p::CallStackClass::NONE;
	CHECK(CallStack.Build(file, UNICODE_STRING_SIMPLE("News"), UNICODE_STRING_SIMPLE("index"), pelet::PHP_53, error));
	CHECK(CallStack.Persist(session));

	CHECK_EQUAL(t4p::CallStackClass::NONE, error);
	CHECK_EQUAL(t4p::SymbolTableMatchErrorClass::NONE, CallStack.MatchError.Type);

	CHECK_VECTOR_SIZE(8, CallStack.Variables);

	// now check the sqlite db contents
	int stepNumber;
	std::string type;
	std::string expression;
	int rowCount;

	session.once << "SELECT COUNT(*) FROM call_stacks", soci::into(rowCount);
	CHECK_EQUAL(8, rowCount);

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
	CHECK_EQUAL("$@tmp1", expression);

	CHECK(stmt.fetch());
	CHECK_EQUAL(2, stepNumber);
	CHECK_EQUAL("ARRAY_KEY", type);
	CHECK_EQUAL("$@tmp1,title", expression);

	CHECK(stmt.fetch());
	CHECK_EQUAL(3, stepNumber);
	CHECK_EQUAL("ASSIGN", type);
	CHECK_EQUAL("$data,$@tmp1", expression);

	CHECK(stmt.fetch());
	CHECK_EQUAL(4, stepNumber);
	CHECK_EQUAL("ASSIGN", type);
	CHECK_EQUAL("$this,", expression);

	CHECK(stmt.fetch());
	CHECK_EQUAL(5, stepNumber);
	CHECK_EQUAL("PROPERTY", type);
	CHECK_EQUAL("$@tmp2,$this,load", expression);

	CHECK(stmt.fetch());
	CHECK_EQUAL(6, stepNumber);
	CHECK_EQUAL("SCALAR", type);
	CHECK_EQUAL("$@tmp3,index", expression);

	CHECK(stmt.fetch());
	CHECK_EQUAL(7, stepNumber);
	CHECK_EQUAL("METHOD_CALL", type);
	CHECK_EQUAL("$@tmp4,$@tmp2,view,$@tmp3,$data", expression);

}

}
