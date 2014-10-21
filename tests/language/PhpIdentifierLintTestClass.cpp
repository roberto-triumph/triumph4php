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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <UnitTest++.h>
#include <language/PhpIdentifierLintClass.h>
#include <language/TagCacheClass.h>
#include <language/TagFinderList.h>
#include <globals/String.h>
#include <globals/Assets.h>
#include <globals/Sqlite.h>
#include <TriumphChecks.h>
#include <FileTestFixtureClass.h>
#include <SqliteTestFixtureClass.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

/**
 * fixture for the PHP identifier lint tests. Will create a
 * linter, default it to use PHP 5.3 syntax
 */
class PhpIdentifierLintTestFixtureClass : public FileTestFixtureClass, public SqliteTestFixtureClass {

public:

	t4p::TagCacheClass TagCache;
	std::vector<wxString> PhpFileExtensions;
	std::vector<wxString> MiscFileExtensions;
	t4p::PhpIdentifierLintClass Lint;
	std::vector<t4p::PhpIdentifierLintResultClass> Results;
	bool HasError;

	PhpIdentifierLintTestFixtureClass() 
	: FileTestFixtureClass(wxT("identifier_lint")) 
	, SqliteTestFixtureClass()
	, TagCache()
	, PhpFileExtensions()
	, MiscFileExtensions()
	, Lint(TagCache) 
	, Results()
	, HasError(false) {
		Lint.SetVersion(pelet::PHP_53);
		PhpFileExtensions.push_back(wxT("*.php"));
		CreateSubDirectory(wxT("src"));
	}

	void Parse(const UnicodeString& code) {
		HasError = Lint.ParseString(code, Results);
	}

	void SetupFile(const wxString& fileName, const wxString& contents) {

		// make the cache consume the file; to prime it with the resources because the
		// call stack wont work without the cache
		CreateFixtureFile(wxT("src") + wxFileName::GetPathSeparators() + fileName, contents);
	}

	void BuildCache(bool includeNativeFunctions = false) {
		soci::session* session = new soci::session(*soci::factory_sqlite3(), ":memory:");
		CreateDatabase(*session, t4p::ResourceSqlSchemaAsset()); 

		t4p::TagFinderListClass* tagFinderList = new t4p::TagFinderListClass;
		tagFinderList->AdoptGlobalTag(session, PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);
		if (includeNativeFunctions) {
			tagFinderList->InitNativeTag(t4p::NativeFunctionsAsset());
		}
		t4p::DirectorySearchClass search;
		search.Init(TestProjectDir + wxT("src"));
		while (search.More()) {
			tagFinderList->Walk(search);
		}
		TagCache.RegisterGlobal(tagFinderList);
	}
};


SUITE(PhpIdentifierLintTestClass) {

TEST_FIXTURE(PhpIdentifierLintTestFixtureClass, DefinedClassAndFunction) {

	// calling defined classes and defined functions should
	// not generate any errors
	wxString cacheCode = t4p::CharToWx(
		"<?php \n"  
		"class MyClass {} \n"
		"function doPrint($a) {} \n"
	);

	UnicodeString code = t4p::CharToIcu(
		"  $a = new MyClass();\n"
		"  doPrint($a);\n"
	);
	SetupFile(wxT("MyClass.php"), cacheCode);
	BuildCache();
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpIdentifierLintTestFixtureClass, ClassImportedNamespace) {

	// test that we resolve the "use" statement when
	// looking up class names
	wxString cacheCode = t4p::CharToWx(
		"<?php \n"  
		"namespace Util; \n"
		"class MyClass {} \n"
		"function doPrint($a) {} \n"
	);

	UnicodeString code = t4p::CharToIcu(
		"use Util\\MyClass as UtilClass;\n"
		"  $a = new UtilClass();\n"
	);
	SetupFile(wxT("MyClass.php"), cacheCode);
	BuildCache();
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpIdentifierLintTestFixtureClass, ClassSelfReference) {

	// test that the "self" keyword is never seen as an unknown class
	wxString cacheCode = t4p::CharToWx(
		"<?php \n"  
		"namespace Util; \n"
		"class MyClass {\n"
		"  const MAX = 1000; \n"
		"}\n"
	);

	UnicodeString code = t4p::CharToIcu(
		"class NextClass extends \\Util\\MyClass { \n"
		"   function work($d) { \n"
		"     return $d < self::MAX ; \n"
		"   }\n"
		"}\n"
	);
	SetupFile(wxT("MyClass.php"), cacheCode);
	BuildCache();
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpIdentifierLintTestFixtureClass, ClassParentReference) {

	// test that the "parent" keyword is never seen as an unknown class
	wxString cacheCode = t4p::CharToWx(
		"<?php \n"  
		"namespace Util; \n"
		"class MyClass {\n"
		"  const MAX = 1000; \n"
		"}\n"
	);

	UnicodeString code = t4p::CharToIcu(
		"class NextClass extends \\Util\\MyClass { \n"
		"   function work($d) { \n"
		"     return $d < parent::MAX ; \n"
		"   }\n"
		"}\n"
	);
	SetupFile(wxT("MyClass.php"), cacheCode);
	BuildCache();
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpIdentifierLintTestFixtureClass, ClassStaticReference) {

	// test that the "static" keyword is never seen as an unknown class
	wxString cacheCode = t4p::CharToWx(
		"<?php \n"  
		"namespace Util; \n"
		"class MyClass {\n"
		"  public static $iMax = 1000; \n"
		"}\n"
	);

	UnicodeString code = t4p::CharToIcu(
		"class NextClass extends \\Util\\MyClass { \n"
		"  public static $iMax = 100; \n"
		"   function work($d) { \n"
		"     return $d < static::MAX; \n"
		"   }\n"
		"}\n"
	);
	SetupFile(wxT("MyClass.php"), cacheCode);
	BuildCache();
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpIdentifierLintTestFixtureClass, ClassConstructor) {

	// test that the "__constructor" method is never seen as an unknown class
	wxString cacheCode = t4p::CharToWx(
		"<?php \n"  
		"namespace Util; \n"
		"class MyClass {\n"
		"  public static $iMax = 1000; \n"
		"}\n"
	);

	UnicodeString code = t4p::CharToIcu(
		"class NextClass extends \\Util\\MyClass { \n"
		"   function __construct() { \n"
		"     parent::__construct(); \n"
		"   }\n"
		"}\n"
	);
	SetupFile(wxT("MyClass.php"), cacheCode);
	BuildCache();
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpIdentifierLintTestFixtureClass, MagicMethod) {

	// test that the "magic" method is never seen as an unknown class
	wxString cacheCode = t4p::CharToWx(
		"<?php \n"  
		"namespace Util; \n"
		"class MyClass {\n"
		"  public static $iMax = 1000; \n"
		"}\n"
	);

	UnicodeString code = t4p::CharToIcu(
		"class NextClass extends \\Util\\MyClass { \n"
		"   function __construct() { \n"
		"     $this->__get(); \n"
		"   }\n"
		"}\n"
	);
	SetupFile(wxT("MyClass.php"), cacheCode);
	BuildCache();
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpIdentifierLintTestFixtureClass, NativeFunctionInNamespace) {

	// test that we don't flag native functions (strlen, strcmp, etc....)
	// as unkown, we should always look them up in the global
	// namespace even when the code is inside a namespace
	wxString cacheCode = t4p::CharToWx(
		"<?php \n"  
		"namespace Util; \n"
		"function doPrint($a) {} \n"
	);

	UnicodeString code = t4p::CharToIcu(
		"namespace Util; \n"
		"$a = strlen('a long string');\n"
	);

	SetupFile(wxT("MyClass.php"), cacheCode);
	BuildCache(true);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpIdentifierLintTestFixtureClass, NativeProperty) {
	
	// test that we don't flag native properties 
	// as unkown, we should always find them if we are guaranteed
	// to know the type involved
	wxString cacheCode = t4p::CharToWx(
		"<?php \n"  
		"namespace Util; \n"
		"function doPrint($a) {} \n"
	);

	UnicodeString code = t4p::CharToIcu(
		"$a = PDO::PARAM_INT;\n"
	);

	SetupFile(wxT("MyClass.php"), cacheCode);
	BuildCache(true);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpIdentifierLintTestFixtureClass, UnknownClass) {

	// instantiating an unknown class should generate
	// an error
	wxString cacheCode = t4p::CharToWx(
		"<?php \n"  
		"class MyClass {} \n"
	);

	UnicodeString code = t4p::CharToIcu(
		"  $a = new NoneClass();\n"
	);
	SetupFile(wxT("MyClass.php"), cacheCode);
	BuildCache();
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(1, Results);
	CHECK_UNISTR_EQUALS("NoneClass", Results[0].Identifier);
	CHECK_EQUAL(t4p::PhpIdentifierLintResultClass::UNKNOWN_CLASS, Results[0].Type);
}

TEST_FIXTURE(PhpIdentifierLintTestFixtureClass, UnknownClassInVariable) {

	// callint a static method on an unknown class should generate
	// an error
	wxString cacheCode = t4p::CharToWx(
		"<?php \n"  
		"class MyClass {\n"
		"  static function work() {} \n"
		"} \n"
	);

	UnicodeString code = t4p::CharToIcu(
		"  $a = NyClass::work();\n"
	);
	SetupFile(wxT("MyClass.php"), cacheCode);
	BuildCache();
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(1, Results);
	CHECK_UNISTR_EQUALS("NyClass", Results[0].Identifier);
	CHECK_EQUAL(t4p::PhpIdentifierLintResultClass::UNKNOWN_CLASS, Results[0].Type);
}

TEST_FIXTURE(PhpIdentifierLintTestFixtureClass, UnknownNamespacedClass) {

	// class name resolution should look at namespaces too
	// so if the class being used matches the class
	// but its from a different namespace then we should
	// generate an error
	wxString cacheCode = t4p::CharToWx(
		"<?php \n"  
		"namespace util {\n "
		"\n"
		"class MyClass {} \n"
		"\n"
		"}\n"
	);

	UnicodeString code = t4p::CharToIcu(
		"  $a = new MyClass();\n"
	);
	SetupFile(wxT("MyClass.php"), cacheCode);
	BuildCache();
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(1, Results);
	CHECK_UNISTR_EQUALS("MyClass", Results[0].Identifier);
	CHECK_EQUAL(t4p::PhpIdentifierLintResultClass::UNKNOWN_CLASS, Results[0].Type);
}

TEST_FIXTURE(PhpIdentifierLintTestFixtureClass, UnknownFunction) {

	// calling undefined functions should generate an
	// error
	wxString cacheCode = t4p::CharToWx(
		"<?php \n"  
		"function MyFirstFunction{} \n"
	);

	UnicodeString code = t4p::CharToIcu(
		"  myFirst();\n"
	);
	SetupFile(wxT("MyClass.php"), cacheCode);
	BuildCache();
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(1, Results);
	CHECK_UNISTR_EQUALS("myFirst", Results[0].Identifier);
	CHECK_EQUAL(t4p::PhpIdentifierLintResultClass::UNKNOWN_FUNCTION, Results[0].Type);
}

TEST_FIXTURE(PhpIdentifierLintTestFixtureClass, UnknownFunctionInArgument) {

	// when checking methods/classes, we should recurse through function
	// arguments.  so we should be able to find an undefined
	// function that is being called as an argument to another
	// function
	wxString cacheCode = t4p::CharToWx(
		"<?php \n"  
		"function MyFirstFunction() {} \n"
		"function MySecondFunction() {} \n"
	);

	UnicodeString code = t4p::CharToIcu(
		"MyFirstFunction(MySecondFunction(), MyThirdFunction());\n"
	);
	SetupFile(wxT("MyClass.php"), cacheCode);
	BuildCache();
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(1, Results);
	CHECK_UNISTR_EQUALS("MyThirdFunction", Results[0].Identifier);
	CHECK_EQUAL(t4p::PhpIdentifierLintResultClass::UNKNOWN_FUNCTION, Results[0].Type);
}

TEST_FIXTURE(PhpIdentifierLintTestFixtureClass, UnknownFunctionInAssignment) {

	// we should also lookup functions used in
	// the left hand of the assignment
	wxString cacheCode = t4p::CharToWx(
		"<?php \n"  
		"function MyFirstFunction() {} \n"
		"function MySecondFunction() {} \n"
		"class MyClass {\n"
		"  public $name;\n"
		"}\n"
	);

	UnicodeString code = t4p::CharToIcu(
		"MyThirdFunction()->name = MySecondFunction();\n"
	);
	SetupFile(wxT("MyClass.php"), cacheCode);
	BuildCache();
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(1, Results);
	CHECK_UNISTR_EQUALS("MyThirdFunction", Results[0].Identifier);
	CHECK_EQUAL(t4p::PhpIdentifierLintResultClass::UNKNOWN_FUNCTION, Results[0].Type);
}

TEST_FIXTURE(PhpIdentifierLintTestFixtureClass, UnknownMethod) {
	wxString cacheCode = t4p::CharToWx(
		"<?php \n"  
		"class MyClass { \n"
		"  function work() {} \n"
		"} \n"
	);

	UnicodeString code = t4p::CharToIcu(
		"$myc = new MyClass();\n"
		"$myc->work();\n"
		"$myc->notFound();\n"
	);
	SetupFile(wxT("MyClass.php"), cacheCode);
	BuildCache();
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(1, Results);
	CHECK_UNISTR_EQUALS("notFound", Results[0].Identifier);
	CHECK_EQUAL(t4p::PhpIdentifierLintResultClass::UNKNOWN_METHOD, Results[0].Type);
}

TEST_FIXTURE(PhpIdentifierLintTestFixtureClass, UnknownStaticMethod) {

	// when we lookup methods, we should lookup only static
	// methods when a static call is being done.  even
	// if we have the same method name, the method must
	// be static if the call is a static call
	wxString cacheCode = t4p::CharToWx(
		"<?php \n"  
		"class MyClass { \n"
		"  function work() {} \n"
		"} \n"
	);

	UnicodeString code = t4p::CharToIcu(
		"MyClass::work();\n"
	);
	SetupFile(wxT("MyClass.php"), cacheCode);
	BuildCache();
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(1, Results);
	CHECK_UNISTR_EQUALS("work", Results[0].Identifier);
	CHECK_EQUAL(t4p::PhpIdentifierLintResultClass::UNKNOWN_METHOD, Results[0].Type);
}

TEST_FIXTURE(PhpIdentifierLintTestFixtureClass, UnknownStaticProperty) {
	wxString cacheCode = t4p::CharToWx(
		"<?php \n"  
		"class MyClass { \n"
		"  var $name; \n"
		"} \n"
	);

	UnicodeString code = t4p::CharToIcu(
		"MyClass::$name;\n"
		"$myc = new MyClass();\n"
		"$myc->name;\n"
	);
	SetupFile(wxT("MyClass.php"), cacheCode);
	BuildCache();
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(1, Results);
	CHECK_UNISTR_EQUALS("$name", Results[0].Identifier);
	CHECK_EQUAL(t4p::PhpIdentifierLintResultClass::UNKNOWN_PROPERTY, Results[0].Type);
}

}
