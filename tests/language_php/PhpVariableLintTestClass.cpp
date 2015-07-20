/**
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
 */
#include <UnitTest++.h>
#include <language_php/PhpVariableLintClass.h>
#include <language_php/TagCacheClass.h>
#include <language_php/TagFinderList.h>
#include <globals/Assets.h>
#include <globals/String.h>
#include <TriumphChecks.h>
#include <FileTestFixtureClass.h>
#include <SqliteTestFixtureClass.h>
#include <unicode/ustream.h>  // get the << overloaded operator, needed by UnitTest++
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <vector>

/**
 * fixture for the PHP lint tests. Will create a
 * linter, default it to suer PHP 5.3 syntax
 */
class PhpVariableLintTestFixtureClass : public FileTestFixtureClass, public SqliteTestFixtureClass {
	public:
	t4p::TagCacheClass TagCache;
	std::vector<wxString> PhpFileExtensions;
	std::vector<wxString> MiscFileExtensions;
	t4p::PhpVariableLintOptionsClass Options;
	t4p::PhpVariableLintClass Lint;
	std::vector<t4p::PhpVariableLintResultClass> Results;
	bool HasError;

	PhpVariableLintTestFixtureClass()
	: FileTestFixtureClass(wxT("variable_lint"))
	, SqliteTestFixtureClass(t4p::ResourceSqlSchemaAsset())
	, TagCache()
	, PhpFileExtensions()
	, MiscFileExtensions()
	, Options()
	, Lint()
	, Results()
	, HasError(false) {
		Options.Version = pelet::PHP_53;
		PhpFileExtensions.push_back(wxT("*.php"));
	}

	void Parse(const UnicodeString& code) {
		Lint.SetOptions(Options);

		// create the file, so that we can index it
		// and the tags get parsed from it
		SetupFile(wxT("test.php"), t4p::IcuToWx(code));
		BuildCache(true);
		Lint.Init(TagCache);

		HasError = Lint.ParseString(code, Results);
	}

	void SetupFile(const wxString& fileName, const wxString& contents) {
		CreateSubDirectory(wxT("src"));
		CreateFixtureFile(wxT("src") + wxFileName::GetPathSeparators() + fileName, contents);
	}

	void BuildCache(bool includeNativeFunctions = false) {
		// make the cache consume the source code file; to prime it with the resources because the
		// variable linter uses the tag cache
		t4p::TagFinderListClass* tagFinderList = new t4p::TagFinderListClass;
		tagFinderList->CreateGlobalTag(PhpFileExtensions, MiscFileExtensions, pelet::PHP_53);
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


SUITE(PhpVariableLintTestClass) {
TEST_FIXTURE(PhpVariableLintTestFixtureClass, IntializedWithObject) {
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc() {\n"
		"  $a = new MyClass();\n"
		"  $a->work();\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, IntializedWithVariable) {
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc($b) {\n"
		"  $a = $b;\n"
		"  $a->work();\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, IntializedWithArray) {
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc($b) {\n"
		"  $a = array(1, 2, 3);\n"
		"  var_dump($a);\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, IntializedWithStatic) {
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc($b) {\n"
		"  static $a = 1;\n"
		"  var_dump($a);\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, IntializedWithGlobal) {
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc($b) {\n"
		"  global $a;\n"
		"  var_dump($a);\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, IntializedWithList) {
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc($b) {\n"
		"  list($k) = $b;\n"
		"  var_dump($k);\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, PredefinedVariables) {
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc() {\n"
		"  $a = $_GET['help'];\n"
		"  $a->work();\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, FunctionParameters) {
	// function parameters should be automatically declared
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc($a, &$b) {\n"
		"  $a->work();\n"
		"  if (!$b) {\n"
		"    $b = true;\n"
		"  }\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, ExceptionBlocks) {
	// no errors on exception blocks as exceptions
	// caught are already initialized
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc($a, $b) {\n"
		"  try {\n"
		"    $a = $a + 99;\n"
		"    $b->work();\n"
		"  }\n"
		"  catch ($exception) {\n"
		"    print $exception->getMessage() . \"\\n\"; \n"
		"    var_dump($exception->getTrace());\n"
		"  }\n"
		"}\n"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, DoNotCheckGlobalScope) {
	// if the options say to not check global variables, then
	// don't check global variables
	Options.CheckGlobalScope = false;
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"  $a->work();\n"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, ThisInMethod) {
	// $this variable is always defined in methods
	Options.CheckGlobalScope = false;
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"class MyClass {\n"
		"  function work() {\n"
		"    $this->work();\n"
		"  }\n"
		"}\n"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, StaticMethods) {
	Options.CheckGlobalScope = false;
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"class MyClass {\n"
		"  function work() {\n"
		"    if (self::workOnce()) {\n"
		"        // nothing \n"
		"    }\n"
		"  }\n"
		"}\n"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, WithExtract) {
	// when the extract() function is used, we can't really
	// detect wich variables have been initialized, as
	// extract defines variables based on the array keys.
	// in this case, we disable variable linting on
	// the scope
	Options.CheckGlobalScope = false;
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"class MyClass {\n"
		"  function work($arrVars) {\n"
		"    extract($arrVars);\n"
		"    echo $name;\n"
		"  }\n"
		"}\n"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}


TEST_FIXTURE(PhpVariableLintTestFixtureClass, WithExtractInNamespace) {
	// when the extract() function is used, we can't really
	// detect wich variables have been initialized, as
	// extract defines variables based on the array keys.
	// in this case, we disable variable linting on
	// the scope
	// here, even though the code is using namespaces, we
	// should ignore variables since PHP falls back to
	// the global functions when a function does not exist in
	// the namespace
	Options.CheckGlobalScope = false;
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"namespace Models;\n"
		"class MyClass {\n"
		"  function work($arrVars) {\n"
		"    extract($arrVars);\n"
		"    echo $name;\n"
		"  }\n"
		"}\n"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}


TEST_FIXTURE(PhpVariableLintTestFixtureClass, WithEval) {
	// when the eval() function is used, we can't really
	// detect wich variables have been initialized, as
	// eval can create new variables.
	// in this case, we disable variable linting on
	// the scope
	Options.CheckGlobalScope = false;
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"class MyClass {\n"
		"  function work($arrVars) {\n"
		"    eval('$arr3 = 4;');\n"
		"    echo $arr3;\n"
		"  }\n"
		"}\n"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, WithInclude) {
	// when include() is used, we can't really
	// detect wich variables have been initialized, as
	// include brings in new variables to the current scope
	// in this case, we disable variable linting on
	// the scope
	Options.CheckGlobalScope = false;
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"class MyClass {\n"
		"  function work($arrVars) {\n"
		"    include('config.php');\n"
		"    echo $config->database->host;\n"
		"  }\n"
		"}\n"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, InitializedArrays) {
	// arrays can be initialized and assigned at the same time
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc($a) {\n"
		"  $data['name'] = 'John';\n"
		"  var_dump($data);\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, AssignmentInConditonal) {
	// assignment in conditional still counts as initialized
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc($a) {\n"
		"  $fp = fopen('data.txt');\n"
		"  while (!feof($fp) && ($line = fgets($fp))) {\n"
		"      var_dump($line);\n"
		"  }\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, AssignmentInSwitch) {
	Options.Version = pelet::PHP_54;

	// assignment in switch still counts as initialized
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function getCommandId(CommandInterface $command)\n"
		"{\n"
		"  switch (($commandId = $command->getId())) {\n"
		"  case 'AUTH':\n"
		"    throw new NotSupportedException(\"Disabled command: {$command->getId()}\");\n"
		"\n"
		"  default:\n"
		"    return $commandId;\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, WithIsset) {
	// a variable being checked with the isset keyword
	// should not be labeled as unitialized
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc(ReflectionClass $class, $arrNames, $arrValues) {\n"
		"   if (isset($arrPairs)) {\n"
		"    return $arrNames[0];\n"
		"  }\n"
		"  return '';\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, WithIssetArrayKeys) {
	// a variable that is used as a key inside an isset statement
	// should in fact be labeled as uninitialized
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc(ReflectionClass $class, $arrNames, $arrValues) {\n"
		"   if (isset($arrPairs[$name])) {\n"
		"    return $arrNames;\n"
		"  }\n"
		"  return '';\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(1, Results);
	CHECK_UNISTR_EQUALS("$name", Results[0].VariableName);
	CHECK_EQUAL(3, Results[0].LineNumber);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, AssignmentInArrayAccess) {
	// assignment in conditional still count as initialized
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc(ReflectionClass $class, $arrNames, $arrValues) {\n"
		"   if (isset($arrNames[$name = $class->getName()])) {\n"
		"    return $arrValues[$name];\n"
		"  }\n"
		"  return '';\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, UnitializedVariable) {
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc() {\n"
		"  $a = $x + $y;\n"
		"  $b->work();\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(3, Results);
	CHECK_UNISTR_EQUALS("$x", Results[0].VariableName);
	CHECK_EQUAL(3, Results[0].LineNumber);
	CHECK_UNISTR_EQUALS("$y", Results[1].VariableName);
	CHECK_EQUAL(3, Results[1].LineNumber);
	CHECK_UNISTR_EQUALS("$b", Results[2].VariableName);
	CHECK_EQUAL(4, Results[2].LineNumber);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, UnitializedVariableScopes) {
	// test that variables are stored by scope; meaning
	// that the same named-variable in different functions
	// must be initialized in bothe functions
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc() {\n"
		"  $x = myBFunc();\n"
		"  $a = $x + 99;\n"
		"  $b = new MyClass($a);\n"
		"  $b->work();\n"
		"}\n"
		"\n"
		"function myBFunc() {\n"
		"  $a = $x + $y;\n"
		"  $b->work();\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(3, Results);
	CHECK_UNISTR_EQUALS("$x", Results[0].VariableName);
	CHECK_EQUAL(10, Results[0].LineNumber);
	CHECK_UNISTR_EQUALS("$y", Results[1].VariableName);
	CHECK_EQUAL(10, Results[1].LineNumber);
	CHECK_UNISTR_EQUALS("$b", Results[2].VariableName);
	CHECK_EQUAL(11, Results[2].LineNumber);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, UnitializedVariableArguments) {
	// test that arguments to calling function are checked
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc($a, $b) {\n"
		"  $a = $a + 99;\n"
		"  $b->work();\n"
		"}\n"
		"\n"
		"function myBFunc($a) {\n"
		"  myFunc($a, $x);\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(1, Results);
	CHECK_UNISTR_EQUALS("$x", Results[0].VariableName);
	CHECK_EQUAL(8, Results[0].LineNumber);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, ReferenceArguments) {
	// test that when an argument is passed by reference, it is NOT counted
	// as uninitialized, since the function being called can
	// set the variable's value
	UnicodeString code = t4p::CharToIcu(
		"<?php \n"
		"function myFunc($a, &$b) {\n"
		"  $a = $a + 99;\n"
		"  $b->work();\n"
		"}\n"
		"\n"
		"function myBFunc($a) {\n"
		"  myFunc($a, $x);\n"
		"  $sum = $a + $x;\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
	CHECK_VECTOR_SIZE(0, Results);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, ReferenceMethodArguments) {
	// test that when an argument to a method is passed by reference, it is NOT counted
	// as uninitialized, since the method being called can
	// set the variable's value
	UnicodeString code = t4p::CharToIcu(
		"<?php \n"
		"class MyClass {\n"
		"  function myFunc(&$b) {\n"
		"    $b = 28;\n"
		"  }\n"
		"}"
		"\n"
		"function myBFunc($a) {\n"
		"  $obj = new MyClass();\n"
		"  $obj->myFunc($x);\n"
		"  $sum = $a + $x;\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
	CHECK_VECTOR_SIZE(0, Results);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, RecurseFunctionArguments) {
	// test that arguments to calling function are checked
	// must recurse down function calls in case an argument
	// is the result of another function call
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc($a, $b) {\n"
		"  $a = $a + 99;\n"
		"  $b->work();\n"
		"}\n"
		"function myFunc2($a) {}\n"
		"function myFunc3($a) {}\n"
		"\n"
		"function myBFunc($a) {\n"
		"  myFunc(myFunc($a), myFunc2(myFunc3($x)));\n"
		"}"
	);
	SetupFile(wxT("test.php"), t4p::IcuToWx(code));
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(1, Results);
	CHECK_UNISTR_EQUALS("$x", Results[0].VariableName);
	CHECK_EQUAL(10, Results[0].LineNumber);
}


TEST_FIXTURE(PhpVariableLintTestFixtureClass, RecurseConstructorArguments) {
	// test that arguments to a constructor are checked
	// must recurse down constructor call and any method chaining
	// in case an argument
	// is the result of another function call
	Options.Version = pelet::PHP_54;
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc($a, $b) {\n"
		"  $a = $a + 99;\n"
		"  $b->work();\n"
		"}\n"
		"\n"
		"function myBFunc($a) {\n"
		"  $b = (new MyClass($a, $c))->myMethod($r);\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(2, Results);
	CHECK_UNISTR_EQUALS("$c", Results[0].VariableName);
	CHECK_UNISTR_EQUALS("$r", Results[1].VariableName);
	CHECK_EQUAL(8, Results[0].LineNumber);
	CHECK_EQUAL(8, Results[1].LineNumber);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, UnitializedArrayKeys) {
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc($c, $d) {\n"
		"  $a = $c[$key];\n"
		"  $c[$k] = array($f => $g); \n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(4, Results);
	CHECK_UNISTR_EQUALS("$key", Results[0].VariableName);
	CHECK_EQUAL(3, Results[0].LineNumber);
	CHECK_UNISTR_EQUALS("$k", Results[1].VariableName);
	CHECK_EQUAL(4, Results[1].LineNumber);
	CHECK_UNISTR_EQUALS("$f", Results[2].VariableName);
	CHECK_EQUAL(4, Results[2].LineNumber);
	CHECK_UNISTR_EQUALS("$g", Results[3].VariableName);
	CHECK_EQUAL(4, Results[3].LineNumber);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, UnitializedIncludeVariables) {
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"$file2  = include __DIR__ . $file;\n"
		"include_once __DIR__ . $file;\n"
		"require __DIR__ . $file;\n"
		"require_once __DIR__ . $file2;\n"
	);
	Options.CheckGlobalScope = true;
	Parse(code);
	CHECK_EQUAL(true, HasError);

	// only the first $file variable should be
	// labeled as uninitialized, when we encounter an
	// include/require there could be variables imported
	// into the current scope.
	CHECK_VECTOR_SIZE(1, Results);
	CHECK_UNISTR_EQUALS("$file", Results[0].VariableName);
	CHECK_EQUAL(2, Results[0].LineNumber);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, UnitializedInClosure) {
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc($c, $d) {\n"
		"  $a = $c[$d];\n"
		"  $zz = function($i, $j) use ($d) {\n"
		"      $a[$c] = array($i => $d, 2 => $j, 3 => $k); \n"
		"   };\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(2, Results);
	CHECK_UNISTR_EQUALS("$c", Results[0].VariableName);
	CHECK_EQUAL(5, Results[0].LineNumber);
	CHECK_UNISTR_EQUALS("$k", Results[1].VariableName);
	CHECK_EQUAL(5, Results[1].LineNumber);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, FunctionDefinedInClosure) {
	// define a function inside of a closure, make sure
	// that the function arguments are correctly captured
	// as being initialized
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"call_user_func(function() {\n"
		"    function work($host) {\n"
		"      echo 'running ' . $host . PHP_EOL;\n"
		"    }\n"
		"    \n"
		"    work('localhost');\n"
		"  });\n"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
	CHECK_VECTOR_SIZE(0, Results);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, PassByReferenceClosure) {
	// test that when a closure uses variables that are passed by
	// reference, the linter does NOT label them as un-initialized
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"function myFunc($c, $d) {\n"
		"  $a = $c[$d];\n"
		"  $zz = function($i, $j) use ($c, $d, &$f) {\n"
		"      $a[$c] = array($i => $d, 2 => $j); \n"
		"      $f = true;\n"
		"   };\n"
		"  print $f;\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
	CHECK_VECTOR_SIZE(0, Results);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, ThisInFunction) {
	// $this variable is never defined in a function
	UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"  function work() {\n"
		"    $this->work();\n"
		"  }\n"
	);
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_UNISTR_EQUALS("$this", Results[0].VariableName);
	CHECK_EQUAL(3, Results[0].LineNumber);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, IndirectVariables) {
	// when we see a variable variable ie. '$$clazz' in the assignment
	// we should turn off uninitialized variable checks, since the
	// code is making variables assignments that we cannot know

		UnicodeString code = t4p::CharToIcu(
		"<?php\n"
		"  function work() {\n"
		"    $name = 'hello';\n"
		"    $$name = 'bye';\n"
		"    $msg = $hello . ' world';\n"
		"    echo $msg;\n"
		"  }\n"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
	CHECK_VECTOR_SIZE(0, Results);
}
}
