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
#include <language/PhpVariableLintClass.h>
#include <globals/String.h>
#include <MvcEditorChecks.h>
#include <unicode/ustream.h> //get the << overloaded operator, needed by UnitTest++

/**
 * fixture for the PHP lint tests. Will create a
 * linter, default it to suer PHP 5.3 syntax
 */
class PhpVariableLintTestFixtureClass {

public:

	mvceditor::PhpVariableLintOptionsClass Options;
	mvceditor::PhpVariableLintClass Lint;
	std::vector<mvceditor::PhpVariableLintResultClass> Results;
	bool HasError;

	PhpVariableLintTestFixtureClass() 
	: Options()
	, Lint() 
	, Results()
	, HasError(false) {
		Options.Version = pelet::PHP_53;
	}

	void Parse(const UnicodeString& code) {
		Lint.SetOptions(Options);
		HasError = Lint.ParseString(code, Results);
	}

};


SUITE(PhpVariableLintTestClass) {

TEST_FIXTURE(PhpVariableLintTestFixtureClass, IntializedWithObject) {
	UnicodeString code = mvceditor::CharToIcu(
		"function myFunc() {\n"
		"  $a = new MyClass();\n"
		"  $a->work();\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, IntializedWithVariable) {
	UnicodeString code = mvceditor::CharToIcu(
		"function myFunc($b) {\n"
		"  $a = $b;\n"
		"  $a->work();\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, IntializedWithArray) {
	UnicodeString code = mvceditor::CharToIcu(
		"function myFunc($b) {\n"
		"  $a = array(1, 2, 3);\n"
		"  var_dump($a);\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, IntializedWithStatic) {
	UnicodeString code = mvceditor::CharToIcu(
		"function myFunc($b) {\n"
		"  static $a = 1;\n"
		"  var_dump($a);\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, IntializedWithGlobal) {
	UnicodeString code = mvceditor::CharToIcu(
		"function myFunc($b) {\n"
		"  global $a;\n"
		"  var_dump($a);\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, IntializedWithList) {
	UnicodeString code = mvceditor::CharToIcu(
		"function myFunc($b) {\n"
		"  list($k) = $b;\n"
		"  var_dump($k);\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, PredefinedVariables) {
	UnicodeString code = mvceditor::CharToIcu(
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
	UnicodeString code = mvceditor::CharToIcu(
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
	UnicodeString code = mvceditor::CharToIcu(
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
	UnicodeString code = mvceditor::CharToIcu(
		"  $a->work();\n"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, ThisInMethod) {

	// $this variable is always defined in methods
	Options.CheckGlobalScope = false;
	UnicodeString code = mvceditor::CharToIcu(
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
	UnicodeString code = mvceditor::CharToIcu(
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
	UnicodeString code = mvceditor::CharToIcu(
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

TEST_FIXTURE(PhpVariableLintTestFixtureClass, WithInclude) {
	
	// when include() is used, we can't really
	// detect wich variables have been initialized, as 
	// include brings in new variables to the current scope
	// in this case, we disable variable linting on
	// the scope
	Options.CheckGlobalScope = false;
	UnicodeString code = mvceditor::CharToIcu(
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
	UnicodeString code = mvceditor::CharToIcu(
		"function myFunc($a) {\n"
		"  $data['name'] = 'John';\n"
		"  var_dump($data);\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, AssignmentInConditonal) {

	// assignment in conditional still count as initialized
	UnicodeString code = mvceditor::CharToIcu(
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
/*
TEST_FIXTURE(PhpVariableLintTestFixtureClass, WithIsset) {

	// a variable being checked with the isset keyword
	// should not be labeled as unitialized
	UnicodeString code = mvceditor::CharToIcu(
		"function myFunc(ReflectionClass $class, $arrNames, $arrValues) {\n"
		"   if (isset($arrPairs)) {\n"
		"    return $arrPairs[0];\n"
		"  }\n"
		"  return '';\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}
*/

TEST_FIXTURE(PhpVariableLintTestFixtureClass, AssignmentInArrayAccess) {

	// assignment in conditional still count as initialized
	UnicodeString code = mvceditor::CharToIcu(
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
	UnicodeString code = mvceditor::CharToIcu(
		"function myFunc() {\n"
		"  $a = $x + $y;\n"
		"  $b->work();\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(3, Results);
	CHECK_UNISTR_EQUALS("$x", Results[0].VariableName);
	CHECK_EQUAL(2, Results[0].LineNumber);
	CHECK_UNISTR_EQUALS("$y", Results[1].VariableName);
	CHECK_EQUAL(2, Results[1].LineNumber);
	CHECK_UNISTR_EQUALS("$b", Results[2].VariableName);
	CHECK_EQUAL(3, Results[2].LineNumber);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, UnitializedVariableScopes) {

	// test that variables are stored by scope; meaning
	// that the same named-variable in different functions
	// must be initialized in bothe functions
	UnicodeString code = mvceditor::CharToIcu(
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
	CHECK_EQUAL(9, Results[0].LineNumber);
	CHECK_UNISTR_EQUALS("$y", Results[1].VariableName);
	CHECK_EQUAL(9, Results[1].LineNumber);
	CHECK_UNISTR_EQUALS("$b", Results[2].VariableName);
	CHECK_EQUAL(10, Results[2].LineNumber);
}
/*
TEST_FIXTURE(PhpVariableLintTestFixtureClass, UnitializedVariableArguments) {

	// test that arguments to calling function are checked
	UnicodeString code = mvceditor::CharToIcu(
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
	CHECK_EQUAL(7, Results[0].LineNumber);
}


TEST_FIXTURE(PhpVariableLintTestFixtureClass, RecurseFunctionArguments) {

	// test that arguments to calling function are checked
	// must recurse down function calls in case an argument
	// is the result of another function call
	UnicodeString code = mvceditor::CharToIcu(
		"function myFunc($a, $b) {\n"
		"  $a = $a + 99;\n"
		"  $b->work();\n"
		"}\n"
		"\n"
		"function myBFunc($a) {\n"
		"  myFunc(myFunc($a), myFunc2(myFunc3($x)));\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(1, Results);
	CHECK_UNISTR_EQUALS("$x", Results[0].VariableName);
	CHECK_EQUAL(7, Results[0].LineNumber);
}
*/

TEST_FIXTURE(PhpVariableLintTestFixtureClass, RecurseConstructorArguments) {

	// test that arguments to a constructor are checked
	// must recurse down constructor call and any method chaining 
	// in case an argument
	// is the result of another function call
	Options.Version = pelet::PHP_54;
	UnicodeString code = mvceditor::CharToIcu(
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
	CHECK_EQUAL(7, Results[0].LineNumber);
	CHECK_EQUAL(7, Results[1].LineNumber);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, UnitializedArrayKeys) {
	UnicodeString code = mvceditor::CharToIcu(
		"function myFunc($c, $d) {\n"
		"  $a = $c[$key];\n"
		"  $c[$k] = array($f => $g); \n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_VECTOR_SIZE(4, Results);
	CHECK_UNISTR_EQUALS("$key", Results[0].VariableName);
	CHECK_EQUAL(2, Results[0].LineNumber);
	CHECK_UNISTR_EQUALS("$k", Results[1].VariableName);
	CHECK_EQUAL(3, Results[1].LineNumber);
	CHECK_UNISTR_EQUALS("$f", Results[2].VariableName);
	CHECK_EQUAL(3, Results[2].LineNumber);
	CHECK_UNISTR_EQUALS("$g", Results[3].VariableName);
	CHECK_EQUAL(3, Results[3].LineNumber);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, UnitializedIncludeVariables) {
	UnicodeString code = mvceditor::CharToIcu(
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
	CHECK_EQUAL(1, Results[0].LineNumber);
	
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, UnitializedInClosure) {
	UnicodeString code = mvceditor::CharToIcu(
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
	CHECK_EQUAL(4, Results[0].LineNumber);
	CHECK_UNISTR_EQUALS("$k", Results[1].VariableName);
	CHECK_EQUAL(4, Results[1].LineNumber);
}

TEST_FIXTURE(PhpVariableLintTestFixtureClass, ThisInFunction) {

	// $this variable is never defined in a function
	UnicodeString code = mvceditor::CharToIcu(
		"  function work() {\n"
		"    $this->work();\n"
		"  }\n"
	);
	Parse(code);
	CHECK_EQUAL(true, HasError);
	CHECK_UNISTR_EQUALS("$this", Results[0].VariableName);
	CHECK_EQUAL(2, Results[0].LineNumber);
}

}
