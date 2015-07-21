/**
 * @copyright  2009-2011 Roberto Perpuly
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
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <UnitTest++.h>
#include <string>
#include <vector>
#include "FileTestFixtureClass.h"
#include "globals/Assets.h"
#include "globals/Sqlite.h"
#include "globals/String.h"
#include "language_php/SymbolTableClass.h"
#include "language_php/TagFinderList.h"
#include "language_php/TagParserClass.h"
#include "SqliteTestFixtureClass.h"
#include "TriumphChecks.h"

UnicodeString CURSOR = UNICODE_STRING_SIMPLE("{CURSOR}");

/**
 *  In order to simulate actual usage of the SymbolTableClass as closely as possible, we need to
 *  test use character positions as required by the symbol table. In order to improve the readability of these tests, we will
 *  use this function to calculate positions instead of harcoding them.  This will allow easy testing of
 *  various positions. When writing test fixture, we can do the following:
 *
 * <code>
 *    UnicodeString code = UNICODE_STRING_SIMPLE("
 * 	  class User {
 *       private $name;
 *
 *      function setName($name) {
 *        $this->na{CURSOR}
 *    ");
 * </code>
 *
 * This function  will calculate the position of {CURSOR} and also remove it from the code
 */
UnicodeString FindCursor(const UnicodeString& code, int32_t& cursorPosition) {
	cursorPosition = code.indexOf(CURSOR);
	UnicodeString codeWithoutCursor(code);
	codeWithoutCursor.findAndReplace(CURSOR, UNICODE_STRING_SIMPLE(""));
	return codeWithoutCursor;
}

class SymbolTableTestClass : public FileTestFixtureClass {
	public:
	SymbolTableTestClass()
	 : FileTestFixtureClass(wxT("symbol_table"))
	 , SymbolTable()
	 , Scope()
	 , ParsedVariable(Scope)
	 , SourceDirs() {
		if (wxDirExists(TestProjectDir)) {
			RecursiveRmDir(TestProjectDir);
		}
	}

	t4p::SymbolTableClass SymbolTable;
	pelet::ScopeClass Scope;
	pelet::VariableClass ParsedVariable;
	std::vector<wxFileName> SourceDirs;
};

class SymbolTableCompletionTestClass : public SqliteTestFixtureClass {
	public:
	t4p::SymbolTableClass CompletionSymbolTable;
	pelet::ScopeClass Scope;
	pelet::VariableClass ParsedVariable;
	std::vector<wxFileName> SourceDirs;
	t4p::TagFinderListClass TagFinderList;
	std::vector<UnicodeString> VariableMatches;
	std::vector<t4p::PhpTagClass> ResourceMatches;
	bool DoDuckTyping;
	bool DoFullyQualifiedMatchOnly;
	t4p::SymbolTableMatchErrorClass Error;
	std::vector<wxString> PhpExtensions;
	std::vector<wxString> MiscExtensions;

	SymbolTableCompletionTestClass()
		: SqliteTestFixtureClass(t4p::ResourceSqlSchemaAsset())
		, CompletionSymbolTable()
		, Scope()
		, ParsedVariable(Scope)
		, SourceDirs()
		, TagFinderList()
		, VariableMatches()
		, ResourceMatches()
		, DoDuckTyping(false)
		, DoFullyQualifiedMatchOnly(false)
		, Error()
		, PhpExtensions()
		, MiscExtensions() {
		TagFinderList.CreateGlobalTag(PhpExtensions, MiscExtensions, pelet::PHP_53);
	}

	void Init(const UnicodeString& sourceCode) {
		TagFinderList.TagParser.BuildResourceCacheForFile(wxT(""), wxT("untitled"), sourceCode, true);
		t4p::SymbolTableClass emptyTable;
		CompletionSymbolTable.CreateSymbols(sourceCode, emptyTable);
		Scope.Clear();
	}

	void ToFunction(const UnicodeString& functionName) {
		// same as ToVariable, but it makes tests easier to read
		ParsedVariable.Clear();
		pelet::VariablePropertyClass prop;
		prop.Name = functionName;

		// dont set prop.IsFunction since functionName may be a partial function name
		ParsedVariable.ChainList.push_back(prop);
	}

	void ToVariable(const UnicodeString& variableName) {
		ParsedVariable.Clear();
		pelet::VariablePropertyClass prop;
		prop.Name = variableName;
		ParsedVariable.ChainList.push_back(prop);
	}

	void ToClass(const UnicodeString& className) {
		// same as ToVariable, but it makes tests easier to read
		ParsedVariable.Clear();
		pelet::VariablePropertyClass prop;
		prop.Name = className;
		ParsedVariable.ChainList.push_back(prop);
	}

	void ToProperty(const UnicodeString& variableName, const UnicodeString& methodName, bool isMethod, bool isStatic) {
		ParsedVariable.Clear();
		pelet::VariablePropertyClass classProp;
		classProp.Name = variableName;
		ParsedVariable.ChainList.push_back(classProp);

		pelet::VariablePropertyClass methodProp;
		methodProp.Name = methodName;
		methodProp.IsFunction = isMethod;
		methodProp.IsStatic = isStatic;
		ParsedVariable.ChainList.push_back(methodProp);
	}

	void ExpressionAppendChain(const UnicodeString& propertyName, bool isMethod) {
		pelet::VariablePropertyClass prop;
		prop.Name = propertyName;
		prop.IsFunction = isMethod;
		ParsedVariable.ChainList.push_back(prop);
	}
};

class ScopeFinderTestClass {
	public:
	t4p::ScopeFinderClass ScopeFinder;
	pelet::ScopeClass Scope;

	ScopeFinderTestClass()
		: ScopeFinder()
		, Scope() {
	}
};

SUITE(SymbolTableTestClass) {
TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithFunctionName) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"function work() {}\n"
	);
	Init(sourceCode);
	ToFunction(UNICODE_STRING_SIMPLE("wo"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("work"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, VariableMatchesWithVariableName) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"$globalOne = 1;\n"
		"$globalTwo = 2;\n"
	);
	Init(sourceCode);
	ToVariable(UNICODE_STRING_SIMPLE("$global"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, VariableMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$globalOne"), VariableMatches[0]);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$globalTwo"), VariableMatches[1]);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, VariableMatchesWithLocalVariableOnly) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"$globalOne = 1;\n"
		" function work() { $globalTwo = 2; } \n"
	);
	Init(sourceCode);
	ToVariable(UNICODE_STRING_SIMPLE("$global"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, VariableMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$globalOne"), VariableMatches[0]);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ManyVariableAssignments) {
	// completion matches should never return duplicates
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"$globalOne = 1;\n"
		" function work() { $globalTwo = 2; } \n"
		"$globalOne = 2;\n"
	);
	Init(sourceCode);
	ToVariable(UNICODE_STRING_SIMPLE("$global"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, VariableMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$globalOne"), VariableMatches[0]);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, VariableMatchesWithPredefinedVariable) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"$globalOne = 1;\n"
		" function work() {  } \n"
	);
	Init(sourceCode);
	ToVariable(UNICODE_STRING_SIMPLE("$_POS"));
	Scope.MethodName = UNICODE_STRING_SIMPLE("work");
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, VariableMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$_POST"), VariableMatches[0]);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, VariableMatchesWithInvalidSyntax) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"$globalOne = 1;\n"
		" function work() { \n"
		"   $functionVar = 2;"
	);
	Init(sourceCode);
	ToVariable(UNICODE_STRING_SIMPLE("$func"));
	Scope.MethodName = UNICODE_STRING_SIMPLE("work");
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, VariableMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$functionVar"), VariableMatches[0]);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithPreviousSymbol) {
	// this rather involved test exercises the code that will use a previous
	// symbol table's variables to aid in code completion when the code
	// has a syntax error.

	// step 1 build a (correct) symbol table
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);

	// step 2: create a new symbol table
	// seed it with invalid code, but make it the table that was built above
	UnicodeString invalidSourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {}  \n"
		"$my = new MyClass;\n"
	);
	t4p::SymbolTableClass updatedSymbolTable;
	updatedSymbolTable.CreateSymbols(invalidSourceCode, CompletionSymbolTable);

	// step 3: perform code completion
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("work"), false, false);
	updatedSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithMethodCall) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("work"), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithMethodCallFromGlobalFinder) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"$my = new MyClass;\n"
	);
	UnicodeString sourceCodeGlobal = t4p::CharToIcu(
		"<?php class MyClass { function workA() {} function workB() {} } \n"
	);
	Init(sourceCode);
	TagFinderList.TagParser.BuildResourceCacheForFile(wxT(""), wxT("MyClass.php"), sourceCodeGlobal, true);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("work"), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithObjectWithoutMethodCall) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE(""), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithStaticMethodCall) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} static function workB() {} } \n"
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("MyClass"), UNICODE_STRING_SIMPLE("work"), false, true);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithClassConstantCall) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { const workA = 3; const workB = 4; } \n"
		"$my = new MyClass(); \n"
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("MyClass"), UNICODE_STRING_SIMPLE("work"), false, true);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), ResourceMatches[1].Identifier);

	// check that a non-static call returns no matches
	ResourceMatches.clear();
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE(""), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(0, ResourceMatches);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithPrivateMethodCall) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} private function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("work"), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithPrivateStaticMethodCall) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { static function workA() {} private static function workBB() {} } \n"
		"$my = new MyClass;\n"
	);

	// in this test, make sure that auto completion works for private static methods
	// when calling a method statically
	// since we are calling a private method, the scope must be the same
	// class
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("self"), UNICODE_STRING_SIMPLE("workB"), true, true);
	Scope.ClassName = UNICODE_STRING_SIMPLE("MyClass");
	Scope.MethodName = UNICODE_STRING_SIMPLE("workA");
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workBB"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithMethodChain) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { /** @return OtherClass */ function workA() {} } \n"
		"class OtherClass { var $time; function toString() {} }\n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("workA"), true, false);
	ExpressionAppendChain(UNICODE_STRING_SIMPLE("ti"), false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("time"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithLongPropertyChain) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { /** @return OtherClass */ function workA() {} } \n"
		"class OtherClass { /** @var OtherClass */ var $parent; function toString() {} }\n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("workA"), true, false);
	ExpressionAppendChain(UNICODE_STRING_SIMPLE("parent"), false);
	ExpressionAppendChain(UNICODE_STRING_SIMPLE("pare"), false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("parent"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithLongMethodChain) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { /** @return OtherClass */ function workA() {} } \n"
		"class OtherClass { /** @return OtherClass */ function parent() {} }\n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("workA"), true, false);
	ExpressionAppendChain(UNICODE_STRING_SIMPLE("parent"), true);
	ExpressionAppendChain(UNICODE_STRING_SIMPLE("parent"), true);
	ExpressionAppendChain(UNICODE_STRING_SIMPLE("p"), false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("parent"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithFunctionChain) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"/** @return OtherClass */ function workA() {} \n"
		"class FirstClass { /** @return OtherClass */ function status() {} } \n"
		"class OtherClass { var $time;  /** @return FirstClass */ function toString() {} }\n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);
	ToFunction(UNICODE_STRING_SIMPLE("workA"));
	ExpressionAppendChain(UNICODE_STRING_SIMPLE("toString"), true);
	ExpressionAppendChain(UNICODE_STRING_SIMPLE("stat"), false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("status"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithParentChain) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class FirstClass { /** @return OtherClass */ function status() {} } \n"
		"class OtherClass extends FirstClass { var $time; function status() { } }\n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("parent"), UNICODE_STRING_SIMPLE(""), false, true);
	Scope.ClassName = UNICODE_STRING_SIMPLE("OtherClass");
	Scope.MethodName = UNICODE_STRING_SIMPLE("status");
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("status"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithVariableCreatedFunctionChain) {
	// variables created with a function should be resolved
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"/** @return OtherClass */ function workA() {} \n"
		"class OtherClass { var $time;  /** @return FirstClass */ function toString() {} }\n"
		"$my = workA();\n"
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE(""), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("time"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("toString"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithVariableCreatedMethodChain) {
	// variables created with a function should be resolved
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class OtherClass { var $time;  /** @return OtherClass */ function parent() {} }\n"
		"$my = new OtherClass();\n"
		"$parent = $my->parent();\n"
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$parent"), UNICODE_STRING_SIMPLE(""), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("parent"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("time"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithVariableInClosure) {
	// a closure should be its own scope, without being able to access
	// the variables in the containing function/method
	UnicodeString sourceCode = t4p::CharToIcu(
		"function printUser(User $user) {\n"
		"  $functionOne = 1;\n"
		"  call_user_func(function() {\n"
		"    $someName = '';\n"
		"  });\n"
		"}\n"
	);
	Init(sourceCode);
	ToVariable(UNICODE_STRING_SIMPLE("$some"));
	Scope.ClassName = UNICODE_STRING_SIMPLE("");
	Scope.MethodName = UNICODE_STRING_SIMPLE("printUser");
	Scope.SetIsAnonymous(true, 0);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, VariableMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$someName"), VariableMatches[0]);
}


TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithVariableInClosureWithLexicalVars) {
	// a closure should have access to the variables that are passed in via the
	// use statement
	UnicodeString sourceCode = t4p::CharToIcu(
		"function printUser(User $user) {\n"
		"  $functionOne = 1;\n"
		"  $functionTwo = 2;\n"
		"  call_user_func(function() use($functionTwo) {\n"
		"    $someName = '';\n"
		"  });\n"
		"}\n"
	);
	Init(sourceCode);
	ToVariable(UNICODE_STRING_SIMPLE("$func"));
	Scope.ClassName = UNICODE_STRING_SIMPLE("");
	Scope.MethodName = UNICODE_STRING_SIMPLE("printUser");
	Scope.SetIsAnonymous(true, 0);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, VariableMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$functionTwo"), VariableMatches[0]);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithVariableOutsideClosure) {
	// a closure should be its own scope, without being able to access
	// the var	iables in the containing function/method
	UnicodeString sourceCode = t4p::CharToIcu(
		"function printUser(User $user) {\n"
		"  $functionOne = 1;\n"
		"  call_user_func(function() {\n"
		"    $someName = '';\n"
		"  });\n"
		"}\n"
	);
	Init(sourceCode);
	ToVariable(UNICODE_STRING_SIMPLE("$functio"));
	Scope.ClassName = UNICODE_STRING_SIMPLE("");
	Scope.MethodName = UNICODE_STRING_SIMPLE("printUser");
	Scope.SetIsAnonymous(true, 0);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(0, VariableMatches);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithClassname) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
	);
	Init(sourceCode);
	ToClass(UNICODE_STRING_SIMPLE("MyCl"));
	std::vector<t4p::PhpTagClass> tags;
	CompletionSymbolTable.ResourceMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		tags, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(1, tags);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("MyClass"), tags[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithDoFullyQualifiedNameOnly) {
	// function starts with the same name as the class
	// but since we are asking for full matches only the function should
	// be ignored
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class My { function workA() {} function workB() {} } \n"
		"function Mysql_query() {}"
	);
	DoFullyQualifiedMatchOnly = true;
	Init(sourceCode);
	ToClass(UNICODE_STRING_SIMPLE("My"));
	std::vector<t4p::PhpTagClass> tags;
	CompletionSymbolTable.ResourceMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		tags, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(1, tags);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("My"), tags[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithSimilarClassAndFunctionName) {
	// function starts with the same name as the class
	// but since we are asking for full matches only the function should
	// be ignored
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"function mysql_query() {} \n"
		"class My { function workA() {} function workB() {} } \n"
		"$my =  new My(); \n"
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE(""), false, false);
	std::vector<t4p::PhpTagClass> tags;
	CompletionSymbolTable.ResourceMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		tags, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(2, tags);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), tags[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), tags[1].Identifier);
}


TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceNameCompletion) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"namespace Second {\n"
		"class SecClass {}\n"
		"}"
		"namespace First\\Child { \n"
		"class OtherClass { }\n"
		"}"
	);
	Init(sourceCode);
	ToClass(UNICODE_STRING_SIMPLE("\\Sec"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\Second"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\Second\\SecClass"), ResourceMatches[1].Identifier);

	ToClass(UNICODE_STRING_SIMPLE("\\First\\C"));
	ResourceMatches.clear();

	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\OtherClass"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceAlias) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"namespace Second {\n"
		"function work() {} \n"
		"}"
		"namespace First\\Child { \n"
		"use Second as S; \n"
		"class OtherClass {} \n"
		"}"
	);
	Init(sourceCode);
	ToClass(UNICODE_STRING_SIMPLE("S\\"));
	Scope.AddNamespaceAlias(UNICODE_STRING_SIMPLE("\\Second"), UNICODE_STRING_SIMPLE("S"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("S\\work"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceStatic) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"namespace First\\Child { \n"
		"class OtherClass {} \n"
		"}"
	);
	Init(sourceCode);
	ToClass(UNICODE_STRING_SIMPLE("namespace\\"));
	Scope.AddNamespaceAlias(UNICODE_STRING_SIMPLE("\\First\\Child"), UNICODE_STRING_SIMPLE("namespace"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("namespace\\OtherClass"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceImporting) {
	// define namespaces and classes in a separate file
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"namespace Second {\n"
		"class MyClass { }\n"
		"}"
		"namespace First { \n"
		"class OtherClass { }\n"
		"}\n"
	);
	TagFinderList.TagParser.BuildResourceCacheForFile(wxT(""), wxT("defines.php"), sourceCode, true);

	// the code under test will import the namespaces
	sourceCode = t4p::CharToIcu(
		"<?php\n"
		"use First as F; \n"
		"}"
	);
	Init(sourceCode);
	ToClass(UNICODE_STRING_SIMPLE("F\\"));
	Scope.AddNamespaceAlias(UNICODE_STRING_SIMPLE("\\First"), UNICODE_STRING_SIMPLE("F"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("F\\OtherClass"), ResourceMatches[0].Identifier);

	ResourceMatches.clear();
	ToClass(UNICODE_STRING_SIMPLE("\\Sec"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\Second"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\Second\\MyClass"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceAndClassInGlobalNamespace) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class OtherClass {} \n"
	);
	Init(sourceCode);

	sourceCode = t4p::CharToIcu(
		"namespace Second; \n"
		"class MyClass {}"
	);
	TagFinderList.TagParser.BuildResourceCacheForFile(wxT(""), wxT("untitled2.php"), sourceCode, true);
	ToClass(UNICODE_STRING_SIMPLE("Othe"));
	Scope.NamespaceName = UNICODE_STRING_SIMPLE("\\Second");
	Scope.AddNamespaceAlias(UNICODE_STRING_SIMPLE("\\Second"), UNICODE_STRING_SIMPLE("namespace"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);

	// since code is declaring a namespace, then classes in the global namespace are not automatically
	// imported
	CHECK_VECTOR_SIZE(0, ResourceMatches);

	ResourceMatches.clear();
	Error.Clear();
	Scope.Clear();

	// fully qualified class names should be visible
	ToClass(UNICODE_STRING_SIMPLE("\\Othe"));

	Scope.NamespaceName = UNICODE_STRING_SIMPLE("\\Second");
	Scope.AddNamespaceAlias(UNICODE_STRING_SIMPLE("\\Second"), UNICODE_STRING_SIMPLE("namespace"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("\\OtherClass", ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceAndClassInDifferentNamespace) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"namespace First\\Child {\n"
		"class OtherClass {} \n"
		"}\n"
	);
	Init(sourceCode);

	sourceCode = t4p::CharToIcu(
		"namespace Second { \n"
		"class MyClass {}  \n"
		"} \n"
	);
	TagFinderList.TagParser.BuildResourceCacheForFile(wxT(""), wxT("untitled2.php"), sourceCode, true);
	ToClass(UNICODE_STRING_SIMPLE("Othe"));
	Scope.NamespaceName = UNICODE_STRING_SIMPLE("\\Second");
	Scope.AddNamespaceAlias(UNICODE_STRING_SIMPLE("\\Second"), UNICODE_STRING_SIMPLE("namespace"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);

	// since code is declaring a namespace, then classes in the global namespace are not automatically
	// imported
	CHECK_VECTOR_SIZE(0, ResourceMatches);
}


TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceAndFunctionInGlobalNamespace) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"function work() {}"
	);
	Init(sourceCode);

	sourceCode = t4p::CharToIcu(
		"namespace First\\Child; \n"
		"class MyClass {}"
	);
	TagFinderList.TagParser.BuildResourceCacheForFile(wxT(""), wxT("untitled2.php"), sourceCode, true);

	// global functions ARE automatically imported
	ToFunction(UNICODE_STRING_SIMPLE("wor"));
	Scope.NamespaceName = UNICODE_STRING_SIMPLE("\\First\\Child");
	Scope.AddNamespaceAlias(UNICODE_STRING_SIMPLE("\\First\\Child"), UNICODE_STRING_SIMPLE("namespace"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("work", ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceAndClassInSameNamespace) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"Class OtherClass {}"
	);
	Init(sourceCode);

	sourceCode = t4p::CharToIcu(
		"namespace First\\Child; \n"
		"class MyClass {}"
	);
	TagFinderList.TagParser.BuildResourceCacheForFile(wxT(""), wxT("untitled2.php"), sourceCode, true);

	// classes in the same namespace ARE automatically imported
	ToClass(UNICODE_STRING_SIMPLE("MyC"));
	Scope.NamespaceName = UNICODE_STRING_SIMPLE("\\First\\Child");
	Scope.AddNamespaceAlias(UNICODE_STRING_SIMPLE("\\First\\Child"), UNICODE_STRING_SIMPLE("namespace"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("MyClass", ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceGlobalClassIsNotImported) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class OtherClass {}"
	);
	Init(sourceCode);

	sourceCode = t4p::CharToIcu(
		"namespace First\\Child; \n"
		"class MyClass {}"
	);
	TagFinderList.TagParser.BuildResourceCacheForFile(wxT(""), wxT("untitled2.php"), sourceCode, true);

	ToClass(UNICODE_STRING_SIMPLE("Other"));
	Scope.NamespaceName = UNICODE_STRING_SIMPLE("\\First\\Child");
	Scope.AddNamespaceAlias(UNICODE_STRING_SIMPLE("\\First\\Child"), UNICODE_STRING_SIMPLE("namespace"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(0, ResourceMatches);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceAndGlobalClassImported) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class OtherClass {} \n"
		"function work() {}"
	);
	Init(sourceCode);
	ToClass(UNICODE_STRING_SIMPLE("Other"));

	// declare a namespace and import the global class as well
	Scope.NamespaceName = UNICODE_STRING_SIMPLE("\\First\\Child");
	Scope.AddNamespaceAlias(UNICODE_STRING_SIMPLE("\\First\\Child"), UNICODE_STRING_SIMPLE("namespace"));
	Scope.AddNamespaceAlias(UNICODE_STRING_SIMPLE("\\OtherClass"), UNICODE_STRING_SIMPLE("OtherClass"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);

	// since code is declaring a namespace, then classes in the global namespace are not automatically
	// imported
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("OtherClass"), ResourceMatches[0].ClassName);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("OtherClass"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithMethodCall) {
	// only doing light testing on ResourceMatches because the Matches* tests
	// cover it already
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("work"), false, false);
	std::vector<t4p::PhpTagClass> tags;
	CompletionSymbolTable.ResourceMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		tags, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(2, tags);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), tags[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("MyClass"), tags[0].ClassName);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), tags[1].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("MyClass"), tags[1].ClassName);
}


TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithUnknownExpressionAndDuckTyping) {
	// there was an overflow error in this scenario and it was causing a crash
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);
	ToFunction(UNICODE_STRING_SIMPLE("unknown"));
	std::vector<t4p::PhpTagClass> tags;
	CompletionSymbolTable.ResourceMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		tags, true, false, Error);
	CHECK_VECTOR_SIZE(0, tags);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithUnknownExpressionAndNoDuckTyping) {
	// if we specify to not do duck typing then any variables that cannot be resolved should not
	// produce matches
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"/** no type here, should not work when duck typing flag is not set */\n"
		"function make() {} \n"
		"$my = make();"
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("work"), false, false);
	std::vector<t4p::PhpTagClass> tags;
	CompletionSymbolTable.ResourceMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		tags, false, false, Error);
	CHECK_VECTOR_SIZE(0, tags);
}


TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithUnknownVariableAndDuckTyping) {
	// if we specify to not do duck typing then any variables that cannot be resolved should not
	// produce matches
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"/** no type here, should still work when duck typing flag is set */\n"
		"function make() {} \n"
		"$my = make();"
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("work"), false, false);
	std::vector<t4p::PhpTagClass> tags;
	CompletionSymbolTable.ResourceMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		tags, true, false, Error);
	CHECK_VECTOR_SIZE(2, tags);
	CHECK_UNISTR_EQUALS("workA", tags[0].Identifier);
	CHECK_UNISTR_EQUALS("MyClass", tags[0].ClassName);
	CHECK_UNISTR_EQUALS("workB", tags[1].Identifier);
	CHECK_UNISTR_EQUALS("MyClass", tags[1].ClassName);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithTraitInDifferentNamespace) {
	CompletionSymbolTable.SetVersion(pelet::PHP_54);
	TagFinderList.TagParser.SetVersion(pelet::PHP_54);

	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"trait TraitClass { \n"
		" function work() {}\n"
		"}\n"
	);
	TagFinderList.TagParser.BuildResourceCacheForFile(wxT(""), wxT("untitled2.php"), sourceCode, true);

	sourceCode = t4p::CharToIcu(
		"namespace Second { \n"
		"class MyClass {\n"
		"	use \\TraitClass; \n"
		"}  \n"
		"$my = new MyClass();"
		"} \n"

	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE(""), false, false);
	Scope.NamespaceName = UNICODE_STRING_SIMPLE("\\Second");
	Scope.AddNamespaceAlias(UNICODE_STRING_SIMPLE("\\Second"), UNICODE_STRING_SIMPLE("namespace"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);

	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("work", ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ShouldFillUnknownResourceError) {
	// when a method could not be found make sure that the ErrorType
	// and class name are properly set
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("unknownFunc"), false, false);
	std::vector<t4p::PhpTagClass> tags;
	CompletionSymbolTable.ResourceMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		tags, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(0, tags);
	CHECK_EQUAL(t4p::SymbolTableMatchErrorClass::UNKNOWN_RESOURCE, Error.Type);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\MyClass"),  Error.ErrorClass);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ShouldFillResolutionError) {
	// when a method cannot be resolved make sure that the ErrorType
	// and class name are properly set
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("workB"), true, false);
	ExpressionAppendChain(UNICODE_STRING_SIMPLE("prop"), false);
	std::vector<t4p::PhpTagClass> tags;
	CompletionSymbolTable.ResourceMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		tags, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(0, tags);
	CHECK_EQUAL(t4p::SymbolTableMatchErrorClass::TYPE_RESOLUTION_ERROR, Error.Type);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"),  Error.ErrorLexeme);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ShouldFillPrimitveError) {
	// when a method is invoked on a primitive type make sure that the ErrorType
	// and lexeme are properly set
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = '124';\n"
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("wor"), false, false);
	std::vector<t4p::PhpTagClass> tags;
	CompletionSymbolTable.ResourceMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		tags, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(0, tags);
	CHECK_EQUAL(t4p::SymbolTableMatchErrorClass::PRIMITIVE_ERROR, Error.Type);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$my"),  Error.ErrorLexeme);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, WithDuckTyping) {
	// when a method cannot be resolved but DuckTyping flag is set
	// resource should be found
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"function factory() {}\n"
	);
	Init(sourceCode);
	ToFunction(UNICODE_STRING_SIMPLE("factory"));
	ExpressionAppendChain(UNICODE_STRING_SIMPLE("work"), false);
	DoDuckTyping = true;
	CompletionSymbolTable.ResourceMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		ResourceMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithClassHierarchyInMultipleResourceFinders) {
	/*
	 * test scenario:
	 * user opens a file, writes code for a base class
	 * user opens another file, writes code for a class that uses the base class
	 * code completion should recognize the inheritance chain, even though
	 * the classes are stored in multiple tags finders
	 */
	UnicodeString sourceCodeParent = t4p::CharToIcu(
		"<?php\n"
		"class MyBaseClass { function workBase() {}  } \n"
	);
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass extends MyBaseClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	TagFinderList.TagParser.BuildResourceCacheForFile(wxT(""), wxT("untitled 2"), sourceCodeParent, true);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE(""), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(3, ResourceMatches);

	// sorted by method name
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), ResourceMatches[1].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workBase"), ResourceMatches[2].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithNativeTags) {
	TagFinderList.InitNativeTag(t4p::NativeFunctionsAsset());
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"$pdo = new PDO;\n"
	);
	TagFinderList.TagParser.BuildResourceCacheForFile(wxT(""), wxT("untitled 2"), sourceCode, true);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$pdo"), UNICODE_STRING_SIMPLE("que"), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);

	// 4 == there are 4 different signature for PDO query
	// see http://php.net/manual/en/pdo.query.php
	CHECK_VECTOR_SIZE(4, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("query"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithNativeTagsAndSourceDirs) {
	// native tags should work even when source directories are defined
	wxFileName sourceDir;
	sourceDir.Assign(t4p::TagDetectorsGlobalAsset());
	SourceDirs.push_back(sourceDir);
	TagFinderList.InitNativeTag(t4p::NativeFunctionsAsset());
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"$pdo = new PDO;\n"
	);
	TagFinderList.TagParser.BuildResourceCacheForFile(wxT(""), wxT("untitled 2"), sourceCode, true);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$pdo"), UNICODE_STRING_SIMPLE("que"), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);

	// 4 == there are 4 different signature for PDO query
	// see http://php.net/manual/en/pdo.query.php
	CHECK_VECTOR_SIZE(4, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("query"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithDetectedTags) {
	TagFinderList.CreateDetectorTag();
	std::string key = "CI_Controller::email",
		className = "CI_Controller",
		methodName = "email",
		returnType = "CI_Email",
		namespaceName = "\\",
		signature = "CI_Email email",
		comment = "";

	int sourceId = 0;
	int isStatic = 0;

	// create the source row
	std::string stdDir = t4p::WxToChar("");
	soci::statement sourceStmt = (TagFinderList.DetectedTagDbSession.prepare << "INSERT INTO sources(directory) VALUES(?)",
		soci::use(stdDir));
	sourceStmt.execute(true);
	soci::sqlite3_statement_backend* backend = static_cast<soci::sqlite3_statement_backend*>(sourceStmt.get_backend());
	sourceId = sqlite3_last_insert_rowid(backend->session_.conn_);

	int type = t4p::PhpTagClass::METHOD;
	std::string sql = "INSERT INTO detected_tags";
	sql += "(key, source_id, type, class_name, method_name, return_type, namespace_name, signature, comment, is_static) ";
	sql += "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	soci::statement stmt = (TagFinderList.DetectedTagDbSession.prepare << sql,
		soci::use(key), soci::use(sourceId), soci::use(type),
		soci::use(className), soci::use(methodName), soci::use(returnType),
		soci::use(namespaceName), soci::use(signature), soci::use(comment),
		soci::use(isStatic)
	);
	stmt.execute(true);

	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class CI_Controller {}\n"
		"$ci = new CI_Controller();\n"
	);
	TagFinderList.TagParser.BuildResourceCacheForFile(wxT(""), wxT("untitled 2"), sourceCode, true);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$ci"), UNICODE_STRING_SIMPLE(""), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedVariable, Scope, SourceDirs, TagFinderList,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("email"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(ScopeFinderTestClass, GetScopeStringShouldFindMethodScope) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"$globalVar = 34; \n"
		"/** This is the user class */\n"
		"class UserClass {\n"
		"\t/** @var string */\n"
		"\tprivate $name;\n"
		"\t/** @var string */\n"
		"\tprivate $address;\n"
		"\t/** @return string */\n"
		"\tfunction getName() {\n"
		"\t\t$unknownName = 'unknown';\n"
		"\t\treturn $this->name;\n"
		"\t}\n"
		"\t/** @return void */\n"
		"\tprivate function setName($anotherName) {\n"
		"\t\t{CURSOR} "
		"\t\t$someName = '';\n"
		"\t}\n"
		"}\n"
		"?>\n"
	);
	int32_t pos;
	sourceCode = FindCursor(sourceCode, pos);
	ScopeFinder.GetScopeString(sourceCode, pos, Scope);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass"), Scope.ClassName);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("setName"), Scope.MethodName);
}

TEST_FIXTURE(ScopeFinderTestClass, GetScopeStringShouldFindGlobalScopePastClass) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"class MyClass { function work() { } }\n"
		"$globalVar = new MyClass(); \n"
		"\t{CURSOR}"
		"?>\n"
	);
	int32_t pos;
	sourceCode = FindCursor(sourceCode, pos);
	ScopeFinder.GetScopeString(sourceCode, pos, Scope);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), Scope.ClassName);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), Scope.MethodName);
}


TEST_FIXTURE(ScopeFinderTestClass, GetScopeStringShouldFindFunctionScope) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"$globalVar = 34; \n"
		"function setName($anotherName, $defaultName = 'Guest') {\n"
		"\t$someName = '';\n"
		"\t{CURSOR}"
		"}\n"
		"?>\n"
	);
	int32_t pos;
	sourceCode = FindCursor(sourceCode, pos);
	ScopeFinder.GetScopeString(sourceCode, pos, Scope);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), Scope.ClassName);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("setName"), Scope.MethodName);
}

TEST_FIXTURE(ScopeFinderTestClass, GetScopeStringShouldHandleUnfinishedBlock) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"$globalOne = 1;\n"
		"function printUser(User $user) {\n"
		"\t$someName = '';\n"
		"{CURSOR}"
	);
	int32_t pos;
	sourceCode = FindCursor(sourceCode, pos);
	ScopeFinder.GetScopeString(sourceCode, pos, Scope);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), Scope.ClassName);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("printUser"), Scope.MethodName);
}

TEST_FIXTURE(ScopeFinderTestClass, GetScopeStringShouldHandleUnfinishedClassMethod) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"$globalOne = 1;\n"
		"class MyClass {\n"
		"function printUser(User $user) {\n"
		"\t$someName = '';\n"
		"{CURSOR}"
	);
	int32_t pos;
	sourceCode = FindCursor(sourceCode, pos);
	ScopeFinder.GetScopeString(sourceCode, pos, Scope);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("MyClass"), Scope.ClassName);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("printUser"), Scope.MethodName);
}

TEST_FIXTURE(ScopeFinderTestClass, GetScopeStringShouldHandleTypeHinting) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"function printUser(User $user) {\n"
		"\t$someName = '';\n"
		"\t{CURSOR}"
		"}\n"
		"?>\n"
	);
	int32_t pos;
	sourceCode = FindCursor(sourceCode, pos);
	ScopeFinder.GetScopeString(sourceCode, pos, Scope);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), Scope.ClassName);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("printUser"), Scope.MethodName);
}

TEST_FIXTURE(ScopeFinderTestClass, GetScopeStringShouldHandleMultipleBlocks) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"$globalOne = 1;\n"
		"function printUser(User $user) {\n"
		"\t$someName = '';\n"
		"}\n"
		"?>\n"
		"<html><body><?php echo $globalOne; ?></body></html>\n"
		"<?php\n"
		"$globalTwo = 2;\n"
		"{CURSOR}"
	);
	int32_t pos;
	sourceCode = FindCursor(sourceCode, pos);
	ScopeFinder.GetScopeString(sourceCode, pos, Scope);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), Scope.ClassName);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), Scope.MethodName);
}

TEST_FIXTURE(ScopeFinderTestClass, GetScopeStringWithMultipleNamespaces) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"namespace First\\Child {"
		"	use ArrayObject, Exception; \n"
		"}"
		"namespace Second\\Child {"
		"	use PDOException as PE; \n"
		" 	function  work() {"
		"{CURSOR}\n"
		"   }\n"
		"}\n"
	);
	int32_t pos;
	sourceCode = FindCursor(sourceCode, pos);
	ScopeFinder.GetScopeString(sourceCode, pos, Scope);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), Scope.ClassName);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("work"), Scope.MethodName);
	CHECK_UNISTR_EQUALS("\\PDOException", Scope.ResolveAlias(UNICODE_STRING_SIMPLE("PE")));
	CHECK_UNISTR_EQUALS("\\Second\\Child", Scope.ResolveAlias(UNICODE_STRING_SIMPLE("namespace")));
}

TEST_FIXTURE(ScopeFinderTestClass, GetScopeStringWithNamespaceOnly) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"namespace First\\Child;"
		"{CURSOR}"
	);
	int32_t pos;
	sourceCode = FindCursor(sourceCode, pos);
	ScopeFinder.GetScopeString(sourceCode, pos, Scope);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), Scope.ClassName);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), Scope.MethodName);
	CHECK_UNISTR_EQUALS("\\First\\Child", Scope.NamespaceName);
	CHECK_UNISTR_EQUALS("\\First\\Child", Scope.ResolveAlias(UNICODE_STRING_SIMPLE("namespace")));
}

TEST_FIXTURE(ScopeFinderTestClass, GetScopeStringShouldHandleGlobalClosures) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"$globalOne = 1;\n"
		"call_user_func(function() use ($globalOne) {\n"
		"  $someName = '';\n"
		"  {CURSOR}"
		"\n"
	);
	int32_t pos;
	sourceCode = FindCursor(sourceCode, pos);
	ScopeFinder.GetScopeString(sourceCode, pos, Scope);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), Scope.ClassName);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), Scope.MethodName);
	CHECK(Scope.IsAnonymousScope());
	CHECK_EQUAL(0, Scope.GetAnonymousFunctionCount());
}

TEST_FIXTURE(ScopeFinderTestClass, GetScopeStringShouldHandleUnfinishedClosuresInFunctions) {
	UnicodeString sourceCode = t4p::CharToIcu(
		"<?php\n"
		"function printUser(User $user) {\n"
		"  $functionOne = 1;\n"
		"  call_user_func(function() {\n"
		"    $someName = '';\n"
		"    {CURSOR}\n"
	);
	int32_t pos;
	sourceCode = FindCursor(sourceCode, pos);
	ScopeFinder.GetScopeString(sourceCode, pos, Scope);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE(""), Scope.ClassName);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("printUser"), Scope.MethodName);
	CHECK(Scope.IsAnonymousScope());
	CHECK_EQUAL(0, Scope.GetAnonymousFunctionCount());
}
}
