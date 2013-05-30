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
#include <language/SymbolTableClass.h>
#include <language/TagParserClass.h>
#include <globals/String.h>
#include <globals/Sqlite.h>
#include <globals/Assets.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <FileTestFixtureClass.h>
#include <SqliteTestFixtureClass.h>
#include <MvcEditorChecks.h>
#include <UnitTest++.h>

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
	 , ParsedExpression(Scope)  {
		if (wxDirExists(TestProjectDir)) {
			RecursiveRmDir(TestProjectDir);
		}
	}

	mvceditor::SymbolTableClass SymbolTable;
	pelet::ScopeClass Scope;
	pelet::ExpressionClass ParsedExpression;
};

class SymbolTableCompletionTestClass : public SqliteTestFixtureClass {

public:

	mvceditor::SymbolTableClass CompletionSymbolTable;
	pelet::ScopeClass Scope;
	pelet::ExpressionClass ParsedExpression;
	std::vector<mvceditor::TagFinderClass*> AllFinders;
	mvceditor::TagFinderClass* OpenedFinder;
	soci::session SessionGlobal;
	soci::session Session1;
	mvceditor::TagParserClass TagParserGlobal;
	mvceditor::TagParserClass TagParser1;
	mvceditor::ParsedTagFinderClass Finder1;
	mvceditor::ParsedTagFinderClass GlobalFinder;
	std::vector<UnicodeString> VariableMatches;
	std::vector<mvceditor::TagClass> ResourceMatches;
	bool DoDuckTyping;
	bool DoFullyQualifiedMatchOnly;
	mvceditor::SymbolTableMatchErrorClass Error;

	SymbolTableCompletionTestClass()
		: SqliteTestFixtureClass()
		, CompletionSymbolTable()
		, Scope()
		, ParsedExpression(Scope)
		, SessionGlobal(*soci::factory_sqlite3(), ":memory:")
		, Session1(*soci::factory_sqlite3(), ":memory:")
		, TagParserGlobal()
		, TagParser1()
		, Finder1()
		, GlobalFinder()
		, VariableMatches()
		, ResourceMatches()
		, DoDuckTyping(false)
		, DoFullyQualifiedMatchOnly(false)
		, Error() {
		CreateDatabase(SessionGlobal, mvceditor::ResourceSqlSchemaAsset());
		CreateDatabase(Session1, mvceditor::ResourceSqlSchemaAsset());
		TagParserGlobal.Init(&SessionGlobal);
		GlobalFinder.Init(&SessionGlobal);

		TagParser1.Init(&Session1);
		Finder1.Init(&Session1);	
	}

	void Init(const UnicodeString& sourceCode) {
		TagParser1.BuildResourceCacheForFile(wxT("untitled"), sourceCode, true);
		OpenedFinder = &Finder1;
		AllFinders.push_back(&Finder1);
		AllFinders.push_back(&GlobalFinder);
		CompletionSymbolTable.CreateSymbols(sourceCode);
		Scope.Clear();
	}

	void ToFunction(const UnicodeString& functionName) {
		
		// same as ToVariable, but it makes tests easier to read
		ParsedExpression.Clear();
		pelet::VariablePropertyClass prop;
		prop.Name = functionName;
		
		// dont set prop.IsFunction since functionName may be a partial function name
		ParsedExpression.ChainList.push_back(prop);
	}

	void ToVariable(const UnicodeString& variableName) {
		ParsedExpression.Clear();
		pelet::VariablePropertyClass prop;
		prop.Name = variableName;
		ParsedExpression.ChainList.push_back(prop);
	}
	
	void ToClass(const UnicodeString& className) {

		// same as ToVariable, but it makes tests easier to read
		ParsedExpression.Clear();
		pelet::VariablePropertyClass prop;
		prop.Name = className;
		ParsedExpression.ChainList.push_back(prop);
	}

	void ToProperty(const UnicodeString& variableName, const UnicodeString& methodName, bool isMethod, bool isStatic) {
		ParsedExpression.Clear();
		pelet::VariablePropertyClass classProp;
		classProp.Name = variableName;
		ParsedExpression.ChainList.push_back(classProp);
		
		pelet::VariablePropertyClass methodProp;
		methodProp.Name = methodName;
		methodProp.IsFunction = isMethod;
		methodProp.IsStatic = isStatic;
		ParsedExpression.ChainList.push_back(methodProp);
	}
	
	void ExpressionAppendChain(const UnicodeString& propertyName, bool isMethod) {
		pelet::VariablePropertyClass prop;
		prop.Name = propertyName;
		prop.IsFunction = isMethod;
		ParsedExpression.ChainList.push_back(prop);
	}
};

class ScopeFinderTestClass {

public:

	mvceditor::ScopeFinderClass ScopeFinder;
	pelet::ScopeClass Scope;

	ScopeFinderTestClass() 
		: ScopeFinder()
		, Scope() {
	}
};

SUITE(SymbolTableTestClass) {

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithFunctionName) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"function work() {}\n"
	);
	Init(sourceCode);	
	ToFunction(UNICODE_STRING_SIMPLE("wo"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK(!ResourceMatches.empty());
	if (!ResourceMatches.empty()) {
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("work"), ResourceMatches[0].Identifier);
	}
}

TEST_FIXTURE(SymbolTableCompletionTestClass, VariableMatchesWithVariableName) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"$globalOne = 1;\n"
		"$globalTwo = 2;\n"
	);
	Init(sourceCode);	
	ToVariable(UNICODE_STRING_SIMPLE("$global"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders,
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, VariableMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$globalOne"), VariableMatches[0]);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$globalTwo"), VariableMatches[1]);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, VariableMatchesWithLocalVariableOnly) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"$globalOne = 1;\n"
		" function work() { $globalTwo = 2; } \n"
	);
	Init(sourceCode);	
	ToVariable(UNICODE_STRING_SIMPLE("$global"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, VariableMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$globalOne"), VariableMatches[0]);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ManyVariableAssignments) {

	// completion matches should never return duplicates
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"$globalOne = 1;\n"
		" function work() { $globalTwo = 2; } \n"
		"$globalOne = 2;\n"
	);
	Init(sourceCode);	
	ToVariable(UNICODE_STRING_SIMPLE("$global"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, VariableMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$globalOne"), VariableMatches[0]);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, VariableMatchesWithPredefinedVariable) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"$globalOne = 1;\n"
		" function work() {  } \n"
	);
	Init(sourceCode);	
	ToVariable(UNICODE_STRING_SIMPLE("$_POS"));
	Scope.MethodName = UNICODE_STRING_SIMPLE("work");
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, VariableMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$_POST"), VariableMatches[0]);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithMethodCall) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("work"), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithMethodCallFromGlobalFinder) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"$my = new MyClass;\n"
	);
	UnicodeString sourceCodeGlobal = mvceditor::CharToIcu(
		"<?php class MyClass { function workA() {} function workB() {} } \n"	
	);
	Init(sourceCode);
	TagParserGlobal.BuildResourceCacheForFile(wxT("MyClass.php"), sourceCodeGlobal, true);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("work"), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithObjectWithoutMethodCall) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE(""), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithStaticMethodCall) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} static function workB() {} } \n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("MyClass"), UNICODE_STRING_SIMPLE("work"), false, true);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithClassConstantCall) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class MyClass { const workA = 3; const workB = 4; } \n"
		"$my = new MyClass(); \n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("MyClass"), UNICODE_STRING_SIMPLE("work"), false, true);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), ResourceMatches[1].Identifier);

	// check that a non-static call returns no matches
	ResourceMatches.clear();
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE(""), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(0, ResourceMatches);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithPrivateMethodCall) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} private function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("work"), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithMethodChain) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class MyClass { /** @return OtherClass */ function workA() {} } \n"
		"class OtherClass { var $time; function toString() {} }\n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("workA"), true, false);
	ExpressionAppendChain(UNICODE_STRING_SIMPLE("ti"), false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("time"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithLongPropertyChain) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class MyClass { /** @return OtherClass */ function workA() {} } \n"
		"class OtherClass { /** @var OtherClass */ var $parent; function toString() {} }\n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("workA"), true, false);
	ExpressionAppendChain(UNICODE_STRING_SIMPLE("parent"), false);
	ExpressionAppendChain(UNICODE_STRING_SIMPLE("pare"), false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("parent"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithLongMethodChain) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
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
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("parent"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithFunctionChain) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
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
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("status"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithParentChain) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class FirstClass { /** @return OtherClass */ function status() {} } \n"
		"class OtherClass extends FirstClass { var $time; function status() { } }\n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("parent"), UNICODE_STRING_SIMPLE(""), false, true);
	Scope.ClassName = UNICODE_STRING_SIMPLE("OtherClass");
	Scope.MethodName = UNICODE_STRING_SIMPLE("status");
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("status"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithVariableCreatedFunctionChain) {

	// variables created with a function should be resolved
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"/** @return OtherClass */ function workA() {} \n"
		"class OtherClass { var $time;  /** @return FirstClass */ function toString() {} }\n"
		"$my = workA();\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE(""), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("time"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("toString"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithVariableCreatedMethodChain) {

	// variables created with a function should be resolved
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class OtherClass { var $time;  /** @return OtherClass */ function parent() {} }\n"
		"$my = new OtherClass();\n"
		"$parent = $my->parent();\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$parent"), UNICODE_STRING_SIMPLE(""), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("parent"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("time"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithClassname) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
	);
	Init(sourceCode);	
	ToClass(UNICODE_STRING_SIMPLE("MyCl"));
	std::vector<mvceditor::TagClass> tags;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, Scope, AllFinders, 
		tags, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(1, tags);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("MyClass"), tags[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithDoFullyQualifiedNameOnly) {
	
	// function starts with the same name as the class
	// but since we are asking for full matches only the function should
	// be ignored
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class My { function workA() {} function workB() {} } \n"
		"function Mysql_query() {}"
	);
	DoFullyQualifiedMatchOnly = true;
	Init(sourceCode);	
	ToClass(UNICODE_STRING_SIMPLE("My"));
	std::vector<mvceditor::TagClass> tags;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, Scope, AllFinders, 
		tags, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(1, tags);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("My"), tags[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithSimilarClassAndFunctionName) {
	
	// function starts with the same name as the class
	// but since we are asking for full matches only the function should
	// be ignored
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"function mysql_query() {} \n"
		"class My { function workA() {} function workB() {} } \n"
		"$my =  new My(); \n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE(""), false, false);
	std::vector<mvceditor::TagClass> tags;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, Scope, AllFinders, 
		tags, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(2, tags);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), tags[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), tags[1].Identifier);
}


TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceNameCompletion) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
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
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\Second"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\Second\\SecClass"), ResourceMatches[1].Identifier);

	ToClass(UNICODE_STRING_SIMPLE("\\First\\C"));
	ResourceMatches.clear();
	
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\OtherClass"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceAlias) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
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
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("S\\work"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceStatic) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"namespace First\\Child { \n"
		"class OtherClass {} \n"
		"}"
	);
	Init(sourceCode);	
	ToClass(UNICODE_STRING_SIMPLE("namespace\\"));
	Scope.AddNamespaceAlias(UNICODE_STRING_SIMPLE("\\First\\Child"), UNICODE_STRING_SIMPLE("namespace"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("namespace\\OtherClass"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceImporting) {
	
	// define namespaces and classes in a separate file
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"namespace Second {\n"
		"class MyClass { }\n"
		"}"
		"namespace First { \n"
		"class OtherClass { }\n"
		"}\n"
	);
	TagParserGlobal.BuildResourceCacheForFile(wxT("defines.php"), sourceCode, true);
	
	// the code under test will import the namespaces
	sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"use First as F; \n"
		"}"
	);
	Init(sourceCode);	
	ToClass(UNICODE_STRING_SIMPLE("F\\"));
	Scope.AddNamespaceAlias(UNICODE_STRING_SIMPLE("\\First"), UNICODE_STRING_SIMPLE("F"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("F\\OtherClass"), ResourceMatches[0].Identifier);
	
	ResourceMatches.clear();
	ToClass(UNICODE_STRING_SIMPLE("\\Sec"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\Second"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\Second\\MyClass"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceAndClassInGlobalNamespace) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class OtherClass {} \n"
	);
	Init(sourceCode);
	
	sourceCode = mvceditor::CharToIcu(
		"namespace Second; \n"
		"class MyClass {}"
	);
	TagParserGlobal.BuildResourceCacheForFile(wxT("untitled2.php"), sourceCode, true);
	ToClass(UNICODE_STRING_SIMPLE("Othe"));
	Scope.NamespaceName = UNICODE_STRING_SIMPLE("\\Second");
	Scope.AddNamespaceAlias(UNICODE_STRING_SIMPLE("\\Second"), UNICODE_STRING_SIMPLE("namespace"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
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
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("\\OtherClass", ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceAndClassInDifferentNamespace) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"namespace First\\Child {\n"
		"class OtherClass {} \n"
		"}\n"
	);
	Init(sourceCode);
	
	sourceCode = mvceditor::CharToIcu(
		"namespace Second { \n"
		"class MyClass {}  \n"
		"} \n"
	);
	TagParserGlobal.BuildResourceCacheForFile(wxT("untitled2.php"), sourceCode, true);
	ToClass(UNICODE_STRING_SIMPLE("Othe"));
	Scope.NamespaceName = UNICODE_STRING_SIMPLE("\\Second");
	Scope.AddNamespaceAlias(UNICODE_STRING_SIMPLE("\\Second"), UNICODE_STRING_SIMPLE("namespace"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
		
	// since code is declaring a namespace, then classes in the global namespace are not automatically
	// imported
	CHECK_VECTOR_SIZE(0, ResourceMatches);
}


TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceAndFunctionInGlobalNamespace) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"function work() {}"
	);
	Init(sourceCode);
	
	sourceCode = mvceditor::CharToIcu(
		"namespace First\\Child; \n"
		"class MyClass {}"
	);
	TagParserGlobal.BuildResourceCacheForFile(wxT("untitled2.php"), sourceCode, true);
	
	// global functions ARE automatically imported
	ToFunction(UNICODE_STRING_SIMPLE("wor"));
	Scope.NamespaceName = UNICODE_STRING_SIMPLE("\\First\\Child");
	Scope.AddNamespaceAlias(UNICODE_STRING_SIMPLE("\\First\\Child"), UNICODE_STRING_SIMPLE("namespace"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("work", ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceAndClassInSameNamespace) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"Class OtherClass {}"
	);
	Init(sourceCode);
	
	sourceCode = mvceditor::CharToIcu(
		"namespace First\\Child; \n"
		"class MyClass {}"
	);
	TagParserGlobal.BuildResourceCacheForFile(wxT("untitled2.php"), sourceCode, true);
	
	// classes in the same namespace ARE automatically imported
	ToClass(UNICODE_STRING_SIMPLE("MyC"));
	Scope.NamespaceName = UNICODE_STRING_SIMPLE("\\First\\Child");
	Scope.AddNamespaceAlias(UNICODE_STRING_SIMPLE("\\First\\Child"), UNICODE_STRING_SIMPLE("namespace"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);	
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("MyClass", ResourceMatches[0].Identifier);		
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceGlobalClassIsNotImported) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class OtherClass {}"
	);
	Init(sourceCode);
	
	sourceCode = mvceditor::CharToIcu(
		"namespace First\\Child; \n"
		"class MyClass {}"
	);
	TagParserGlobal.BuildResourceCacheForFile(wxT("untitled2.php"), sourceCode, true);
	
	ToClass(UNICODE_STRING_SIMPLE("Other"));
	Scope.NamespaceName = UNICODE_STRING_SIMPLE("\\First\\Child");
	Scope.AddNamespaceAlias(UNICODE_STRING_SIMPLE("\\First\\Child"), UNICODE_STRING_SIMPLE("namespace"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(0, ResourceMatches);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceAndGlobalClassImported) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
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
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
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
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("work"), false, false);
	std::vector<mvceditor::TagClass> tags;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, Scope, AllFinders, 
		tags, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_EQUAL((size_t)2, tags.size());
	if ((size_t)2 == tags.size()) {
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), tags[0].Identifier);
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("MyClass"), tags[0].ClassName);
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), tags[1].Identifier);
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("MyClass"), tags[1].ClassName);
	}
}


TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithUnknownExpressionAndDuckTyping) {

	// there was an overflow error in this scenario and it was causing a crash
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);
	ToFunction(UNICODE_STRING_SIMPLE("unknown"));
	std::vector<mvceditor::TagClass> tags;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, Scope, AllFinders, 
		tags, true, false, Error);
	CHECK_EQUAL((size_t)0, tags.size());
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithUnknownExpressionAndNoDuckTyping) {

	// if we specify to not do duck typing then any variables that cannot be resolved should not 
	// produce matches
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"/** no type here, should not work when duck typing flag is not set */\n"
		"function make() {} \n"
		"$my = make();"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("work"), false, false);
	std::vector<mvceditor::TagClass> tags;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, Scope, AllFinders, 
		tags, false, false, Error);
	CHECK_EQUAL((size_t)0, tags.size());
}


TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithUnknownVariableAndDuckTyping) {

	// if we specify to not do duck typing then any variables that cannot be resolved should not 
	// produce matches
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"/** no type here, should still work when duck typing flag is set */\n"
		"function make() {} \n"
		"$my = make();"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("work"), false, false);
	std::vector<mvceditor::TagClass> tags;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, Scope, AllFinders, 
		tags, true, false, Error);
	CHECK_VECTOR_SIZE(2, tags);
	CHECK_UNISTR_EQUALS("workA", tags[0].Identifier);
	CHECK_UNISTR_EQUALS("MyClass", tags[0].ClassName);
	CHECK_UNISTR_EQUALS("workB", tags[1].Identifier);
	CHECK_UNISTR_EQUALS("MyClass", tags[1].ClassName);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithTraitInDifferentNamespace) {
	CompletionSymbolTable.SetVersion(pelet::PHP_54);
	TagParserGlobal.SetVersion(pelet::PHP_54);
	TagParser1.SetVersion(pelet::PHP_54);

	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"trait TraitClass { \n"
		" function work() {}\n"
		"}\n"
	);
	TagParserGlobal.BuildResourceCacheForFile(wxT("untitled2.php"), sourceCode, true);

	sourceCode = mvceditor::CharToIcu(
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
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
		
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("work", ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ShouldFillUnknownResourceError) {

	// when a method could not be found make sure that the ErrorType
	// and class name are properly set
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("unknownFunc"), false, false);
	std::vector<mvceditor::TagClass> tags;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, Scope, AllFinders, 
		tags, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_EQUAL((size_t)0, tags.size());
	CHECK_EQUAL(mvceditor::SymbolTableMatchErrorClass::UNKNOWN_RESOURCE, Error.Type);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("MyClass"),  Error.ErrorClass);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ShouldFillResolutionError) {

	// when a method cannot be resolved make sure that the ErrorType
	// and class name are properly set
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("workB"), true, false);
	ExpressionAppendChain(UNICODE_STRING_SIMPLE("prop"), false);
	std::vector<mvceditor::TagClass> tags;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, Scope, AllFinders, 
		tags, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_EQUAL((size_t)0, tags.size());
	CHECK_EQUAL(mvceditor::SymbolTableMatchErrorClass::TYPE_RESOLUTION_ERROR, Error.Type);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"),  Error.ErrorLexeme);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ShouldFillPrimitveError) {

	// when a method is invoked on a primitive type make sure that the ErrorType
	// and lexeme are properly set
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = '124';\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("wor"), false, false);
	std::vector<mvceditor::TagClass> tags;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, Scope, AllFinders, 
		tags, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_EQUAL((size_t)0, tags.size());
	CHECK_EQUAL(mvceditor::SymbolTableMatchErrorClass::PRIMITIVE_ERROR, Error.Type);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$my"),  Error.ErrorLexeme);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, WithDuckTyping) {

	// when a method cannot be resolved but DuckTyping flag is set
	// resource should be found
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"function factory() {}\n"
	);
	Init(sourceCode);	
	ToFunction(UNICODE_STRING_SIMPLE("factory"));
	ExpressionAppendChain(UNICODE_STRING_SIMPLE("work"), false);
	DoDuckTyping = true;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, Scope, AllFinders, 
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
	UnicodeString sourceCodeParent = mvceditor::CharToIcu(
		"<?php\n"
		"class MyBaseClass { function workBase() {}  } \n"
	);
	UnicodeString sourceCode = mvceditor::CharToIcu(
		"<?php\n"
		"class MyClass extends MyBaseClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	TagParserGlobal.BuildResourceCacheForFile(wxT("untitled 2"), sourceCodeParent, true);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE(""), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, Scope, AllFinders, 
		VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(3, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), ResourceMatches[1].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workBase"), ResourceMatches[2].Identifier);
}

TEST_FIXTURE(ScopeFinderTestClass, GetScopeStringShouldFindMethodScope) {
	UnicodeString sourceCode = mvceditor::CharToIcu(
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
	UnicodeString sourceCode = mvceditor::CharToIcu(
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
	UnicodeString sourceCode = mvceditor::CharToIcu(
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
	UnicodeString sourceCode = mvceditor::CharToIcu(
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
	UnicodeString sourceCode = mvceditor::CharToIcu(
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
	UnicodeString sourceCode = mvceditor::CharToIcu(
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
	UnicodeString sourceCode = mvceditor::CharToIcu(
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
	UnicodeString sourceCode = mvceditor::CharToIcu(
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
	UnicodeString sourceCode = mvceditor::CharToIcu(
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

}