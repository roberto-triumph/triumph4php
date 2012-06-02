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
#include <windows/StringHelperClass.h>
#include <FileTestFixtureClass.h>
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

class SymbolTableCompletionTestClass {

public:

	mvceditor::SymbolTableClass CompletionSymbolTable;
	pelet::ScopeClass Scope;
	pelet::ExpressionClass ParsedExpression;
	std::map<wxString, mvceditor::ResourceFinderClass*> OpenedFinders;
	mvceditor::ResourceFinderClass Finder1;
	mvceditor::ResourceFinderClass GlobalFinder;
	std::vector<UnicodeString> VariableMatches;
	std::vector<mvceditor::ResourceClass> ResourceMatches;
	bool DoDuckTyping;
	bool DoFullyQualifiedMatchOnly;
	mvceditor::SymbolTableMatchErrorClass Error;
	mvceditor::ScopeResultClass ScopeResult;

	SymbolTableCompletionTestClass()
		: CompletionSymbolTable()
		, Scope()
		, ParsedExpression(Scope)
		, OpenedFinders()
		, Finder1()
		, GlobalFinder()
		, VariableMatches()
		, ResourceMatches()
		, DoDuckTyping(false)
		, DoFullyQualifiedMatchOnly(false)
		, Error()
		, ScopeResult() {

	}

	void Init(const UnicodeString& sourceCode) {
		Finder1.BuildResourceCacheForFile(wxT("untitled"), sourceCode, true);
		OpenedFinders[wxT("untitled")] = &Finder1;
		CompletionSymbolTable.CreateSymbols(sourceCode);
		ScopeResult.Clear();
		
		// the global scope
		ScopeResult.MethodName = UNICODE_STRING_SIMPLE("::");
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
	mvceditor::ScopeResultClass ScopeResult;

	ScopeFinderTestClass() 
		: ScopeFinder()
		, ScopeResult() {
	}
};

SUITE(SymbolTableTestClass) {

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithFunctionName) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"function work() {}\n"
	);
	Init(sourceCode);	
	ToFunction(UNICODE_STRING_SIMPLE("wo"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK(!ResourceMatches.empty());
	if (!ResourceMatches.empty()) {
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("work"), ResourceMatches[0].Identifier);
	}
}

TEST_FIXTURE(SymbolTableCompletionTestClass, VariableMatchesWithVariableName) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"$globalOne = 1;\n"
		"$globalTwo = 2;\n"
	);
	Init(sourceCode);	
	ToVariable(UNICODE_STRING_SIMPLE("$global"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders,
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, VariableMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$globalOne"), VariableMatches[0]);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$globalTwo"), VariableMatches[1]);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, VariableMatchesWithLocalVariableOnly) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"$globalOne = 1;\n"
		" function work() { $globalTwo = 2; } \n"
	);
	Init(sourceCode);	
	ToVariable(UNICODE_STRING_SIMPLE("$global"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, VariableMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$globalOne"), VariableMatches[0]);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ManyVariableAssignments) {

	// completion matches should never return duplicates
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"$globalOne = 1;\n"
		" function work() { $globalTwo = 2; } \n"
		"$globalOne = 2;\n"
	);
	Init(sourceCode);	
	ToVariable(UNICODE_STRING_SIMPLE("$global"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, VariableMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$globalOne"), VariableMatches[0]);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, VariableMatchesWithPredefinedVariable) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"$globalOne = 1;\n"
		" function work() {  } \n"
	);
	Init(sourceCode);	
	ToVariable(UNICODE_STRING_SIMPLE("$_POS"));
	ScopeResult.MethodName = UNICODE_STRING_SIMPLE("::work");
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, VariableMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$_POST"), VariableMatches[0]);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithMethodCall) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("work"), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithMethodCallFromGlobalFinder) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"$my = new MyClass;\n"
	);
	UnicodeString sourceCodeGlobal = mvceditor::StringHelperClass::charToIcu(
		"<?php class MyClass { function workA() {} function workB() {} } \n"	
	);
	Init(sourceCode);
	GlobalFinder.BuildResourceCacheForFile(wxT("MyClass.php"), sourceCodeGlobal, true);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("work"), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithLocalFinderOverridesGlobalFinder) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"$my = new MyClass;\n"
	);

	// in this test, simulate method workA() being deleted; it should not show
	// as a match
	UnicodeString sourceCodeGlobal = mvceditor::StringHelperClass::charToIcu(
		"<?php class MyClass { function workA() {} function workB() {} } \n"	
	);
	UnicodeString sourceCodeOpened = mvceditor::StringHelperClass::charToIcu(
		"<?php class MyClass { function workB() {} } \n"	
	);
	Init(sourceCode);
	GlobalFinder.BuildResourceCacheForFile(wxT("MyClass.php"), sourceCodeGlobal, true);
	mvceditor::ResourceFinderClass localFinder;
	localFinder.BuildResourceCacheForFile(wxT("MyClass.php"), sourceCodeOpened, true);
	OpenedFinders[wxT("MyClass.php")] = &localFinder;

	GlobalFinder.BuildResourceCacheForFile(wxT("MyClass.php"), sourceCodeGlobal, true);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("work"), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithObjectWithoutMethodCall) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE(""), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithStaticMethodCall) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class MyClass { function workA() {} static function workB() {} } \n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("MyClass"), UNICODE_STRING_SIMPLE("work"), false, true);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithPrivateMethodCall) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class MyClass { function workA() {} private function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("work"), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithMethodChain) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class MyClass { /** @return OtherClass */ function workA() {} } \n"
		"class OtherClass { var $time; function toString() {} }\n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("workA"), true, false);
	ExpressionAppendChain(UNICODE_STRING_SIMPLE("ti"), false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("time"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithLongPropertyChain) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class MyClass { /** @return OtherClass */ function workA() {} } \n"
		"class OtherClass { /** @var OtherClass */ var $parent; function toString() {} }\n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("workA"), true, false);
	ExpressionAppendChain(UNICODE_STRING_SIMPLE("parent"), false);
	ExpressionAppendChain(UNICODE_STRING_SIMPLE("pare"), false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("parent"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithLongMethodChain) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
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
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("parent"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithFunctionChain) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
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
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("status"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithParentChain) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class FirstClass { /** @return OtherClass */ function status() {} } \n"
		"class OtherClass extends FirstClass { var $time; function status() { } }\n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("parent"), UNICODE_STRING_SIMPLE(""), false, true);
	ScopeResult.MethodName = UNICODE_STRING_SIMPLE("OtherClass::status");
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("status"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithVariableCreatedFunctionChain) {

	// variables created with a function should be resolved
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"/** @return OtherClass */ function workA() {} \n"
		"class OtherClass { var $time;  /** @return FirstClass */ function toString() {} }\n"
		"$my = workA();\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE(""), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("time"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("toString"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, MatchesWithVariableCreatedMethodChain) {

	// variables created with a function should be resolved
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class OtherClass { var $time;  /** @return OtherClass */ function parent() {} }\n"
		"$my = new OtherClass();\n"
		"$parent = $my->parent();\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$parent"), UNICODE_STRING_SIMPLE(""), false, false);
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("parent"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("time"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithClassname) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
	);
	Init(sourceCode);	
	ToClass(UNICODE_STRING_SIMPLE("MyCl"));
	std::vector<mvceditor::ResourceClass> resources;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, resources, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(1, resources);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("MyClass"), resources[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithDoFullyQualifiedNameOnly) {
	
	// function starts with the same name as the class
	// but since we are asking for full matches only the function should
	// be ignored
	GlobalFinder.BuildResourceCacheForNativeFunctions();
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class My { function workA() {} function workB() {} } \n"
	);
	DoFullyQualifiedMatchOnly = true;
	Init(sourceCode);	
	ToClass(UNICODE_STRING_SIMPLE("My"));
	std::vector<mvceditor::ResourceClass> resources;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, resources, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(1, resources);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("My"), resources[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithSimilarClassAndFunctionName) {
	
	// function starts with the same name as the class
	// but since we are asking for full matches only the function should
	// be ignored
	GlobalFinder.BuildResourceCacheForNativeFunctions();
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class My { function workA() {} function workB() {} } \n"
		"$my =  new My(); \n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE(""), false, false);
	std::vector<mvceditor::ResourceClass> resources;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, resources, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(2, resources);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), resources[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), resources[1].Identifier);
}


TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceNameCompletion) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
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
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\Second"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\Second\\SecClass"), ResourceMatches[1].Identifier);

	ToClass(UNICODE_STRING_SIMPLE("\\First\\C"));
	ResourceMatches.clear();
	
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\First\\Child\\OtherClass"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceAlias) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
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
	ScopeResult.NamespaceAliases[UNICODE_STRING_SIMPLE("S")] = UNICODE_STRING_SIMPLE("\\Second");
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("S\\work"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceStatic) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"namespace First\\Child { \n"
		"class OtherClass {} \n"
		"}"
	);
	Init(sourceCode);	
	ToClass(UNICODE_STRING_SIMPLE("namespace\\"));
	ScopeResult.NamespaceAliases[UNICODE_STRING_SIMPLE("namespace")] = UNICODE_STRING_SIMPLE("\\First\\Child");
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("namespace\\OtherClass"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceImporting) {
	
	// define namespaces and classes in a separate file
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"namespace Second {\n"
		"class MyClass { }\n"
		"}"
		"namespace First { \n"
		"class OtherClass { }\n"
		"}\n"
	);
	GlobalFinder.BuildResourceCacheForFile(wxT("defines.php"), sourceCode, true);
	
	// the code under test will import the namespaces
	sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"use First as F; \n"
		"}"
	);
	Init(sourceCode);	
	ToClass(UNICODE_STRING_SIMPLE("F\\"));
	ScopeResult.NamespaceAliases[UNICODE_STRING_SIMPLE("F")] = UNICODE_STRING_SIMPLE("\\First");
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("F\\OtherClass"), ResourceMatches[0].Identifier);
	
	ResourceMatches.clear();
	ToClass(UNICODE_STRING_SIMPLE("\\Sec"));
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\Second"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("\\Second\\MyClass"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceAndClassInGlobalNamespace) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class OtherClass {} \n"
	);
	Init(sourceCode);
	
	sourceCode = mvceditor::StringHelperClass::charToIcu(
		"namespace Second; \n"
		"class MyClass {}"
	);
	GlobalFinder.BuildResourceCacheForFile(wxT("untitled2.php"), sourceCode, true);
	ToClass(UNICODE_STRING_SIMPLE("Othe"));
	ScopeResult.NamespaceName = UNICODE_STRING_SIMPLE("\\Second");
	ScopeResult.NamespaceAliases[UNICODE_STRING_SIMPLE("namespace")] = UNICODE_STRING_SIMPLE("\\Second");
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
		
	// since code is declaring a namespace, then classes in the global namespace are not automatically
	// imported
	CHECK_VECTOR_SIZE(0, ResourceMatches);
	
	ResourceMatches.clear();
	Error.Clear();
	ScopeResult.Clear();
	  
	// fully qualified class names should be visible
	ToClass(UNICODE_STRING_SIMPLE("\\Othe"));
	
	ScopeResult.NamespaceName = UNICODE_STRING_SIMPLE("\\Second");
	ScopeResult.NamespaceAliases[UNICODE_STRING_SIMPLE("namespace")] = UNICODE_STRING_SIMPLE("\\Second");
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("\\OtherClass", ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceAndClassInDifferentNamespace) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"namespace First\\Child {\n"
		"class OtherClass {} \n"
		"}\n"
	);
	Init(sourceCode);
	
	sourceCode = mvceditor::StringHelperClass::charToIcu(
		"namespace Second { \n"
		"class MyClass {}  \n"
		"} \n"
	);
	GlobalFinder.BuildResourceCacheForFile(wxT("untitled2.php"), sourceCode, true);
	ToClass(UNICODE_STRING_SIMPLE("Othe"));
	ScopeResult.NamespaceName = UNICODE_STRING_SIMPLE("\\Second");
	ScopeResult.NamespaceAliases[UNICODE_STRING_SIMPLE("namespace")] = UNICODE_STRING_SIMPLE("\\Second");
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
		
	// since code is declaring a namespace, then classes in the global namespace are not automatically
	// imported
	CHECK_VECTOR_SIZE(0, ResourceMatches);
}


TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceAndFunctionInGlobalNamespace) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"function work() {}"
	);
	Init(sourceCode);
	
	sourceCode = mvceditor::StringHelperClass::charToIcu(
		"namespace First\\Child; \n"
		"class MyClass {}"
	);
	GlobalFinder.BuildResourceCacheForFile(wxT("untitled2.php"), sourceCode, true);
	
	// global functions ARE automatically imported
	ToFunction(UNICODE_STRING_SIMPLE("wor"));
	ScopeResult.NamespaceName = UNICODE_STRING_SIMPLE("\\First\\Child");
	ScopeResult.NamespaceAliases[UNICODE_STRING_SIMPLE("namespace")] = UNICODE_STRING_SIMPLE("\\First\\Child");
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("work", ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceAndClassInSameNamespace) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"Class OtherClass {}"
	);
	Init(sourceCode);
	
	sourceCode = mvceditor::StringHelperClass::charToIcu(
		"namespace First\\Child; \n"
		"class MyClass {}"
	);
	GlobalFinder.BuildResourceCacheForFile(wxT("untitled2.php"), sourceCode, true);
	
	// classes in the same namespace ARE automatically imported
	ToClass(UNICODE_STRING_SIMPLE("MyC"));
	ScopeResult.NamespaceName = UNICODE_STRING_SIMPLE("\\First\\Child");
	ScopeResult.NamespaceAliases[UNICODE_STRING_SIMPLE("namespace")] = UNICODE_STRING_SIMPLE("\\First\\Child");
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);	
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("MyClass", ResourceMatches[0].Identifier);		
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceGlobalClassIsNotImported) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class OtherClass {}"
	);
	Init(sourceCode);
	
	sourceCode = mvceditor::StringHelperClass::charToIcu(
		"namespace First\\Child; \n"
		"class MyClass {}"
	);
	GlobalFinder.BuildResourceCacheForFile(wxT("untitled2.php"), sourceCode, true);
	
	ToClass(UNICODE_STRING_SIMPLE("Other"));
	ScopeResult.NamespaceName = UNICODE_STRING_SIMPLE("\\First\\Child");
	ScopeResult.NamespaceAliases[UNICODE_STRING_SIMPLE("namespace")] = UNICODE_STRING_SIMPLE("\\First\\Child");
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
	CHECK_VECTOR_SIZE(0, ResourceMatches);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithNamespaceAndGlobalClassImported) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class OtherClass {} \n"
		"function work() {}"
	);
	Init(sourceCode);
	ToClass(UNICODE_STRING_SIMPLE("Other"));
	
	// declare a namespace and import the global class as well
	ScopeResult.NamespaceName = UNICODE_STRING_SIMPLE("\\First\\Child");
	ScopeResult.NamespaceAliases[UNICODE_STRING_SIMPLE("namespace")] = UNICODE_STRING_SIMPLE("\\First\\Child");
	ScopeResult.NamespaceAliases[UNICODE_STRING_SIMPLE("OtherClass")] = UNICODE_STRING_SIMPLE("\\OtherClass");
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
		
	// since code is declaring a namespace, then classes in the global namespace are not automatically
	// imported
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("OtherClass"), ResourceMatches[0].ClassName);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("OtherClass"), ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithMethodCall) {
	
	// only doing light testing on ResourceMatches because the Matches* tests
	// cover it already
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("work"), false, false);
	std::vector<mvceditor::ResourceClass> resources;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, resources, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_EQUAL((size_t)2, resources.size());
	if ((size_t)2 == resources.size()) {
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), resources[0].Identifier);
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("MyClass"), resources[0].ClassName);
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), resources[1].Identifier);
		CHECK_EQUAL(UNICODE_STRING_SIMPLE("MyClass"), resources[1].ClassName);
	}
}


TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithUnknownExpressionAndDuckTyping) {

	// there was an overflow error in this scenario and it was causing a crash
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);
	ToFunction(UNICODE_STRING_SIMPLE("unknown"));
	std::vector<mvceditor::ResourceClass> resources;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, resources, true, false, Error);
	CHECK_EQUAL((size_t)0, resources.size());
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithUnknownExpressionAndNoDuckTyping) {

	// if we specify to not do duck typing then any variables that cannot be resolved should not 
	// produce matches
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"/** no type here, should not work when duck typing flag is not set */\n"
		"function make() {} \n"
		"$my = make();"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("work"), false, false);
	std::vector<mvceditor::ResourceClass> resources;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, resources, false, false, Error);
	CHECK_EQUAL((size_t)0, resources.size());
}


TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithUnknownVariableAndDuckTyping) {

	// if we specify to not do duck typing then any variables that cannot be resolved should not 
	// produce matches
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"/** no type here, should still work when duck typing flag is set */\n"
		"function make() {} \n"
		"$my = make();"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("work"), false, false);
	std::vector<mvceditor::ResourceClass> resources;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, resources, true, false, Error);
	CHECK_VECTOR_SIZE(2, resources);
	CHECK_UNISTR_EQUALS("workA", resources[0].Identifier);
	CHECK_UNISTR_EQUALS("MyClass", resources[0].ClassName);
	CHECK_UNISTR_EQUALS("workB", resources[1].Identifier);
	CHECK_UNISTR_EQUALS("MyClass", resources[1].ClassName);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ResourceMatchesWithTraitInDifferentNamespace) {
	CompletionSymbolTable.SetVersion(pelet::PHP_54);
	GlobalFinder.SetVersion(pelet::PHP_54);
	Finder1.SetVersion(pelet::PHP_54);
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class TraitClass { \n"
		" function work() {}\n"
		"}\n"
	);
	GlobalFinder.BuildResourceCacheForFile(wxT("untitled2.php"), sourceCode, true);
	
	// TODO need to add the trait code to both caches for now because
	// the resource finder will look for trait methods from its own resource finder
	// meaning that if the class that uses the trait is only in Finder1 
	// and the trait class is only in GlobalFinder, this test will fail.
	// This means that if the user creates a trait, then creates a new file
	// then the user will not get code completion for trait methods until the
	// project get re-indexed. will need to fix this.
	Finder1.BuildResourceCacheForFile(wxT("untitled2.php"), sourceCode, true);
	sourceCode = mvceditor::StringHelperClass::charToIcu(
		"namespace Second { \n"
		"class MyClass {\n"
		"	use \\TraitClass; \n"
		"}  \n"
		"$my = new MyClass();"
		"} \n"
		
	);
	Init(sourceCode);
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE(""), false, false);
	ScopeResult.NamespaceName = UNICODE_STRING_SIMPLE("\\Second");
	ScopeResult.NamespaceAliases[UNICODE_STRING_SIMPLE("namespace")] = UNICODE_STRING_SIMPLE("\\Second");
	CompletionSymbolTable.ExpressionCompletionMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, VariableMatches, ResourceMatches, DoDuckTyping, Error);
		
	CHECK_VECTOR_SIZE(1, ResourceMatches);
	CHECK_UNISTR_EQUALS("work", ResourceMatches[0].Identifier);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ShouldFillUnknownResourceError) {

	// when a method could not be found make sure that the ErrorType
	// and class name are properly set
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("unknownFunc"), false, false);
	std::vector<mvceditor::ResourceClass> resources;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, resources, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_EQUAL((size_t)0, resources.size());
	CHECK_EQUAL(mvceditor::SymbolTableMatchErrorClass::UNKNOWN_RESOURCE, Error.Type);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("MyClass"),  Error.ErrorClass);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ShouldFillResolutionError) {

	// when a method cannot be resolved make sure that the ErrorType
	// and class name are properly set
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = new MyClass;\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("workB"), true, false);
	ExpressionAppendChain(UNICODE_STRING_SIMPLE("prop"), false);
	std::vector<mvceditor::ResourceClass> resources;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, resources, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_EQUAL((size_t)0, resources.size());
	CHECK_EQUAL(mvceditor::SymbolTableMatchErrorClass::TYPE_RESOLUTION_ERROR, Error.Type);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"),  Error.ErrorLexeme);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, ShouldFillPrimitveError) {

	// when a method is invoked on a primitive type make sure that the ErrorType
	// and lexeme are properly set
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"$my = '124';\n"
	);
	Init(sourceCode);	
	ToProperty(UNICODE_STRING_SIMPLE("$my"), UNICODE_STRING_SIMPLE("wor"), false, false);
	std::vector<mvceditor::ResourceClass> resources;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, resources, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_EQUAL((size_t)0, resources.size());
	CHECK_EQUAL(mvceditor::SymbolTableMatchErrorClass::PRIMITIVE_ERROR, Error.Type);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("$my"),  Error.ErrorLexeme);
}

TEST_FIXTURE(SymbolTableCompletionTestClass, WithDuckTyping) {

	// when a method cannot be resolved but DuckTyping flag is set
	// resource should be found
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class MyClass { function workA() {} function workB() {} } \n"
		"function factory() {}\n"
	);
	Init(sourceCode);	
	ToFunction(UNICODE_STRING_SIMPLE("factory"));
	ExpressionAppendChain(UNICODE_STRING_SIMPLE("work"), false);
	DoDuckTyping = true;
	CompletionSymbolTable.ResourceMatches(ParsedExpression, ScopeResult, OpenedFinders, 
		&GlobalFinder, ResourceMatches, DoDuckTyping, DoFullyQualifiedMatchOnly, Error);
	CHECK_VECTOR_SIZE(2, ResourceMatches);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workA"), ResourceMatches[0].Identifier);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("workB"), ResourceMatches[1].Identifier);
}

TEST_FIXTURE(ScopeFinderTestClass, GetScopeStringShouldFindMethodScope) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
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
	ScopeFinder.GetScopeString(sourceCode, pos, ScopeResult);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("UserClass::setName"), ScopeResult.MethodName);
}

TEST_FIXTURE(ScopeFinderTestClass, GetScopeStringShouldFindGlobalScopePastClass) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"class MyClass { function work() { } }\n"
		"$globalVar = new MyClass(); \n"
		"\t{CURSOR}"
		"?>\n"
	);
	int32_t pos;
	sourceCode = FindCursor(sourceCode, pos);
	ScopeFinder.GetScopeString(sourceCode, pos, ScopeResult);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("::"), ScopeResult.MethodName);
}


TEST_FIXTURE(ScopeFinderTestClass, GetScopeStringShouldFindFunctionScope) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
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
	ScopeFinder.GetScopeString(sourceCode, pos, ScopeResult);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("::setName"), ScopeResult.MethodName);
}

TEST_FIXTURE(ScopeFinderTestClass, GetScopeStringShouldHandleTypeHinting) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"function printUser(User $user) {\n"
		"\t$someName = '';\n"
		"\t{CURSOR}"
		"}\n"
		"?>\n"
	);
	int32_t pos;
	sourceCode = FindCursor(sourceCode, pos);
	ScopeFinder.GetScopeString(sourceCode, pos, ScopeResult);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("::printUser"), ScopeResult.MethodName);
}

TEST_FIXTURE(ScopeFinderTestClass, GetScopeStringShouldHandleMultipleBlocks) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
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
	ScopeFinder.GetScopeString(sourceCode, pos, ScopeResult);
	CHECK_EQUAL(UNICODE_STRING_SIMPLE("::"), ScopeResult.MethodName);
}

TEST_FIXTURE(ScopeFinderTestClass, GetScopeStringWithMultipleNamespaces) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
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
	ScopeFinder.GetScopeString(sourceCode, pos, ScopeResult);
	CHECK_UNISTR_EQUALS("::work", ScopeResult.MethodName);
	CHECK_UNISTR_EQUALS("\\PDOException", ScopeResult.NamespaceAliases[UNICODE_STRING_SIMPLE("PE")]);
	CHECK_UNISTR_EQUALS("\\Second\\Child", ScopeResult.NamespaceAliases[UNICODE_STRING_SIMPLE("namespace")]);
}

TEST_FIXTURE(ScopeFinderTestClass, GetScopeStringWithNamespaceOnly) {
	UnicodeString sourceCode = mvceditor::StringHelperClass::charToIcu(
		"<?php\n"
		"namespace First\\Child;"
		"{CURSOR}"
	);
	int32_t pos;
	sourceCode = FindCursor(sourceCode, pos);
	ScopeFinder.GetScopeString(sourceCode, pos, ScopeResult);
	CHECK_UNISTR_EQUALS("::", ScopeResult.MethodName);
	CHECK_UNISTR_EQUALS("\\First\\Child", ScopeResult.NamespaceName);
	CHECK_UNISTR_EQUALS("\\First\\Child", ScopeResult.NamespaceAliases[UNICODE_STRING_SIMPLE("namespace")]);
}

}