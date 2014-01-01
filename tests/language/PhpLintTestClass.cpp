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
#include <language/PhpLintClass.h>
#include <globals/String.h>
#include <MvcEditorChecks.h>
#include <unicode/ustream.h> //get the << overloaded operator, needed by UnitTest++

/**
 * fixture for the PHP lint tests. Will create a
 * linter, default it to suer PHP 5.3 syntax
 */
class PhpLintTestFixtureClass {

public:

	mvceditor::PhpLintClass Lint;
	std::vector<mvceditor::PhpLintResultClass> Results;
	bool HasError;

	PhpLintTestFixtureClass() 
	: Lint() 
	, Results()
	, HasError(false) {
		Lint.SetVersion(pelet::PHP_53);
	}

	void Parse(const UnicodeString& code) {
		HasError = Lint.ParseString(code, Results);
	}

};


SUITE(PhpLintTestClass) {

TEST_FIXTURE(PhpLintTestFixtureClass, IntializedWithObject) {
	UnicodeString code = mvceditor::CharToIcu(
		"function myFunc() {\n"
		"  $a = new MyClass();\n"
		"  $a->work();\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpLintTestFixtureClass, IntializedWithVariable) {
	UnicodeString code = mvceditor::CharToIcu(
		"function myFunc($b) {\n"
		"  $a = $b;\n"
		"  $a->work();\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpLintTestFixtureClass, IntializedWithArray) {
	UnicodeString code = mvceditor::CharToIcu(
		"function myFunc($b) {\n"
		"  $a = array(1, 2, 3);\n"
		"  var_dump($a);\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}


TEST_FIXTURE(PhpLintTestFixtureClass, PredefinedVariables) {
	UnicodeString code = mvceditor::CharToIcu(
		"function myFunc() {\n"
		"  $a = $_GET['help'];\n"
		"  $a->work();\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpLintTestFixtureClass, FunctionParameters) {

	// function parameters should be automatically declared
	UnicodeString code = mvceditor::CharToIcu(
		"function myFunc($a) {\n"
		"  $a->work();\n"
		"}"
	);
	Parse(code);
	CHECK_EQUAL(false, HasError);
}

TEST_FIXTURE(PhpLintTestFixtureClass, ExceptionBlocks) {
	
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

TEST_FIXTURE(PhpLintTestFixtureClass, UnitializedVariable) {
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

TEST_FIXTURE(PhpLintTestFixtureClass, UnitializedVariableScopes) {

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

TEST_FIXTURE(PhpLintTestFixtureClass, UnitializedVariableArguments) {

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

TEST_FIXTURE(PhpLintTestFixtureClass, RecurseFunctionArguments) {

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

TEST_FIXTURE(PhpLintTestFixtureClass, RecurseConstructorArguments) {

	// test that arguments to a constructor are checked
	// must recurse down constructor call and any method chaining 
	// in case an argument
	// is the result of another function call
	Lint.SetVersion(pelet::PHP_54);
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


}
