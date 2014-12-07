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
#include <language/PhpIdentifierLintClass.h>
#include <language/TagCacheClass.h>
#include <globals/String.h>
#include <language/ParsedTagFinderClass.h>
#include <wx/ffile.h>
#include <unicode/ustring.h>
#include <algorithm>

/**
 * we will stop tracking errors after we have reached this
 * many.  The user cannot possibly go through all 
 * of them
 */
const static size_t MAX_ERRORS = 100;

static void AddMagicMethods(std::map<UnicodeString, int, t4p::UnicodeStringComparatorClass>& methods) {

	// magic methods, never unknown 
	methods[UNICODE_STRING_SIMPLE("__construct")] = 1;
	methods[UNICODE_STRING_SIMPLE("__destruct")] = 1;
	methods[UNICODE_STRING_SIMPLE("__call()")] = 1;
	methods[UNICODE_STRING_SIMPLE("__callStatic")] = 1;
	methods[UNICODE_STRING_SIMPLE("__get")] = 1;
	methods[UNICODE_STRING_SIMPLE("__set")] = 1;
	methods[UNICODE_STRING_SIMPLE("__isset")] = 1;
	methods[UNICODE_STRING_SIMPLE("__unset")] = 1;
	methods[UNICODE_STRING_SIMPLE("__sleep")] = 1;
	methods[UNICODE_STRING_SIMPLE("__wakeup")] = 1;
	methods[UNICODE_STRING_SIMPLE("__toString")] = 1;
	methods[UNICODE_STRING_SIMPLE("__invoke")] = 1;
	methods[UNICODE_STRING_SIMPLE("__set_state")] = 1;
	methods[UNICODE_STRING_SIMPLE("__clone")] = 1;
}

t4p::PhpIdentifierLintResultClass::PhpIdentifierLintResultClass()
: Identifier()
, File()
, LineNumber(0)
, Pos(0) 
, Type(NONE) {

}

t4p::PhpIdentifierLintResultClass::PhpIdentifierLintResultClass(const t4p::PhpIdentifierLintResultClass& src)
: Identifier()
, File()
, LineNumber(0)
, Pos(0) 
, Type(NONE) {
	Copy(src);
}

t4p::PhpIdentifierLintResultClass& 
t4p::PhpIdentifierLintResultClass::operator=(const t4p::PhpIdentifierLintResultClass& src) {
	Copy(src);
	return *this;
}

void t4p::PhpIdentifierLintResultClass::Copy(const t4p::PhpIdentifierLintResultClass& src) {
	Identifier = src.Identifier;
	File =  src.File;
	LineNumber = src.LineNumber;
	Pos = src.Pos;
	Type = src.Type;
}


t4p::PhpIdentifierLintClass::PhpIdentifierLintClass()
: ExpressionObserverClass()
, Errors()
, Parser() 
, File()
, ClassLookup()
, MethodLookup()
, PropertyLookup()
, FunctionLookup()
, NamespaceLookup()
, NativeClassLookup()
, NativeMethodLookup()
, NativePropertyLookup()
, NativeFunctionLookup()
, FoundClasses()
, FoundMethods()
, FoundProperties()
, FoundFunctions()
, FoundStaticMethods()
, FoundStaticProperties()
, NotFoundClasses()
, NotFoundMethods()
, NotFoundProperties()
, NotFoundFunctions()
, NotFoundStaticMethods()
, NotFoundStaticProperties()
, HasMethodExistsCalled(false)
, CurrentClassName() {
	Parser.SetClassMemberObserver(this);
	Parser.SetClassObserver(this);
	Parser.SetExpressionObserver(this);
	Parser.SetFunctionObserver(this);
}

void t4p::PhpIdentifierLintClass::Init(t4p::TagCacheClass& tagCache) {
	tagCache.GlobalPrepare(ClassLookup, true);
	tagCache.GlobalPrepare(MethodLookup, true);
	tagCache.GlobalPrepare(PropertyLookup, true);
	tagCache.GlobalPrepare(FunctionLookup, true);
	tagCache.GlobalPrepare(NamespaceLookup, true);
	tagCache.NativePrepare(NativeClassLookup, true);
	tagCache.NativePrepare(NativeMethodLookup, true);
	tagCache.NativePrepare(NativePropertyLookup, true);
	tagCache.NativePrepare(NativeFunctionLookup, true);
	HasMethodExistsCalled = false;
	CurrentClassName = UNICODE_STRING_SIMPLE("");
}

void t4p::PhpIdentifierLintClass::SetVersion(pelet::Versions version) {
	Parser.SetVersion(version);
}

bool t4p::PhpIdentifierLintClass::ParseFile(const wxFileName& fileName, 
															std::vector<t4p::PhpIdentifierLintResultClass>& errors) {
	Errors.clear();
	FoundClasses.clear();
	FoundMethods.clear();
	FoundProperties.clear();
	FoundFunctions.clear();
	FoundStaticMethods.clear();
	FoundStaticProperties.clear();
	NotFoundClasses.clear();
	NotFoundMethods.clear();
	NotFoundProperties.clear();
	NotFoundFunctions.clear();
	NotFoundStaticMethods.clear();
	NotFoundStaticProperties.clear();
	HasMethodExistsCalled = false;
	CurrentClassName = UNICODE_STRING_SIMPLE("");

	AddMagicMethods(FoundMethods);
	AddMagicMethods(FoundStaticMethods);

	File = t4p::WxToIcu(fileName.GetFullPath());
	pelet::LintResultsClass lintResult;

	wxFFile file;
	if (file.Open(fileName.GetFullPath(), wxT("rb"))) {
		Parser.ScanFile(file.fp(), t4p::WxToIcu(fileName.GetFullPath()), lintResult);
		errors = Errors;
	}
	return !errors.empty();
}

bool t4p::PhpIdentifierLintClass::ParseString(const UnicodeString& code, 
															  std::vector<t4p::PhpIdentifierLintResultClass>& errors) {
	
	Errors.clear();
	FoundClasses.clear();
	FoundMethods.clear();
	FoundProperties.clear();
	FoundFunctions.clear();
	FoundStaticMethods.clear();
	FoundStaticProperties.clear();
	NotFoundClasses.clear();
	NotFoundMethods.clear();
	NotFoundProperties.clear();
	NotFoundFunctions.clear();
	NotFoundStaticMethods.clear();
	NotFoundStaticProperties.clear();
	CurrentClassName = UNICODE_STRING_SIMPLE("");

	// magic methods, never unknown 
	AddMagicMethods(FoundMethods);
	AddMagicMethods(FoundStaticMethods);

	File = UNICODE_STRING_SIMPLE("");
	pelet::LintResultsClass lintResult;
	Parser.ScanString(code, lintResult);
	errors = Errors;

	return !errors.empty();
}

void t4p::PhpIdentifierLintClass::DefineDeclarationFound(const UnicodeString& namespaceName, 
													 const UnicodeString& variableName, 
													 const UnicodeString& variableValue, 
													 const UnicodeString& comment, 
													 const int lineNumber) {

}

void t4p::PhpIdentifierLintClass::ClassFound(const UnicodeString& namespaceName, const UnicodeString& className, 
		const UnicodeString& signature, 
		const UnicodeString& baseClassName,
		const UnicodeString& implementsList,
		const UnicodeString& comment, const int lineNumber) {
	CheckClassNameAndLog(baseClassName, lineNumber, 0);
	
	if (implementsList.isEmpty()) {
		return;
	}
	
	UChar* state = NULL;
	UChar* buf = new UChar[implementsList.length() + 1];
	u_memmove(buf, implementsList.getBuffer(), implementsList.length());
	buf[implementsList.length()] = '\0';
	
	UChar delims[3] = { ',', ' ', '\0' };
	UChar* next = u_strtok_r(buf, delims, &state);
	while (next) {
		UnicodeString nextInterface(next);
		CheckClassNameAndLog(nextInterface, lineNumber, 0);
		
		next = u_strtok_r(NULL, delims, &state);
	}
	
	delete[] buf;
}


void t4p::PhpIdentifierLintClass::MethodFound(const UnicodeString& namespaceName, const UnicodeString& className, 
										  const UnicodeString& methodName, const UnicodeString& signature, 
										  const UnicodeString& returnType, const UnicodeString& comment,
										  pelet::TokenClass::TokenIds visibility, bool isStatic, const int lineNumber, 
										  bool hasVariableArguments) {
	HasMethodExistsCalled = false;
	CurrentClassName = className;
}

void t4p::PhpIdentifierLintClass::FunctionFound(const UnicodeString& namespaceName, const UnicodeString& functionName, 
											const UnicodeString& signature, const UnicodeString& returnType, 
											const UnicodeString& comment, const int lineNumber, bool hasVariableArguments) {
	HasMethodExistsCalled = false;
	CurrentClassName = UNICODE_STRING_SIMPLE("");
}

void t4p::PhpIdentifierLintClass::NamespaceUseFound(const UnicodeString& namespaceName, const UnicodeString& alias, int lineNumber, 
		int startingPos) {
	if (Errors.size() > MAX_ERRORS) {
		return;
	}
	bool isKnown = CheckClassName(namespaceName);
	if (!isKnown) {
		isKnown = CheckNamespace(namespaceName);
	}
	if (!isKnown) {
		t4p::PhpIdentifierLintResultClass lintResult;
		lintResult.File = File;
		lintResult.LineNumber = lineNumber;
		lintResult.Pos = startingPos;
		lintResult.Type = t4p::PhpIdentifierLintResultClass::UNKNOWN_CLASS;
		lintResult.Identifier = namespaceName;
		Errors.push_back(lintResult);	
	}
}

void t4p::PhpIdentifierLintClass::ExpressionFunctionArgumentFound(pelet::VariableClass* variable) {
}

void t4p::PhpIdentifierLintClass::ExpressionVariableFound(pelet::VariableClass* expression) {
	CheckVariable(expression);
}

void t4p::PhpIdentifierLintClass::ExpressionAssignmentFound(pelet::AssignmentExpressionClass* expression) {
	CheckExpression(&expression->Destination);
	CheckExpression(expression->Expression);
}

void t4p::PhpIdentifierLintClass::ExpressionAssignmentCompoundFound(pelet::AssignmentCompoundExpressionClass* expression) {
	CheckExpression(&expression->Variable);
	CheckExpression(expression->RightOperand);
}

void t4p::PhpIdentifierLintClass::ExpressionBinaryOperationFound(pelet::BinaryOperationClass* expression) {
	CheckExpression(expression->LeftOperand);
	CheckExpression(expression->RightOperand);
}

void t4p::PhpIdentifierLintClass::ExpressionUnaryOperationFound(pelet::UnaryOperationClass* expression) {
	CheckExpression(expression->Operand);
}

void t4p::PhpIdentifierLintClass::ExpressionUnaryVariableOperationFound(pelet::UnaryVariableOperationClass* expression) {
	CheckVariable(&expression->Variable);
}

void t4p::PhpIdentifierLintClass::ExpressionTernaryOperationFound(pelet::TernaryOperationClass* expression) {
	CheckExpression(expression->Expression1);
	CheckExpression(expression->Expression2);
	if (expression->Expression3) {
		CheckExpression(expression->Expression3);
	}
}

void t4p::PhpIdentifierLintClass::ExpressionInstanceOfOperationFound(pelet::InstanceOfOperationClass* expression) {
	CheckExpression(expression->Expression1);
	CheckClassNameAndLog(expression->ClassName, expression->LineNumber, expression->Pos);	
}

void t4p::PhpIdentifierLintClass::ExpressionScalarFound(pelet::ScalarExpressionClass* expression) {
	
	// nothing as scalars cannot be undefined
}

void t4p::PhpIdentifierLintClass::ExpressionNewInstanceFound(pelet::NewInstanceExpressionClass* expression) {
	std::vector<pelet::ExpressionClass*>::const_iterator constructorArg = expression->CallArguments.begin();
	for (; constructorArg != expression->CallArguments.end(); ++constructorArg) {
		CheckExpression(*constructorArg);
	}
	CheckClassNameAndLog(expression->ClassName, expression->LineNumber, expression->Pos);

	// check any function args to any chained method calls.
	std::vector<pelet::VariablePropertyClass>::const_iterator prop = expression->ChainList.begin();
	for (; prop != expression->ChainList.end(); ++prop) {
		std::vector<pelet::ExpressionClass*>::const_iterator chainArg = prop->CallArguments.begin();
		for (; chainArg != prop->CallArguments.end(); ++chainArg) {
			CheckExpression(*chainArg);
		}
	}
} 

void t4p::PhpIdentifierLintClass::StatementGlobalVariablesFound(pelet::GlobalVariableStatementClass* variables) {
	
	// global statement only contain "simple" variables, ie no function/method calls
}

void t4p::PhpIdentifierLintClass::StatementStaticVariablesFound(pelet::StaticVariableStatementClass* variables) {

	// static statements only contain "simple" variables, ie no function/method calls
}

void t4p::PhpIdentifierLintClass::ExpressionIncludeFound(pelet::IncludeExpressionClass* expr) {
	CheckExpression(expr->Expression);
}

void t4p::PhpIdentifierLintClass::ExpressionClosureFound(pelet::ClosureExpressionClass* expr) {
	
	// for a closure, we add the closure parameters and the lexical
	// var ("use" variables) as into the scope.  we also define a new
	// scope for the closure.
	std::vector<UnicodeString> closureScopeVariables;
	for (size_t i = 0; i < expr->Parameters.size(); ++i) {
		pelet::VariableClass* param = expr->Parameters[i];
		CheckExpression(param);
	}
	for (size_t i = 0; i < expr->LexicalVars.size(); ++i) {
		pelet::VariableClass* var = expr->LexicalVars[i];
		CheckExpression(var);
	}

	for (size_t i = 0; i < expr->Statements.Size(); ++i) {
		if (pelet::StatementClass::EXPRESSION == expr->Statements.TypeAt(i)) {
			CheckExpression((pelet::ExpressionClass*)expr->Statements.At(i));
		}
	}
}

void t4p::PhpIdentifierLintClass::ExpressionAssignmentListFound(pelet::AssignmentListExpressionClass* expression) {

	// check any array accesses in the destination variables
	// ie $user[$name]
	for (size_t i = 0; i < expression->Destinations.size(); ++i) {
		pelet::VariableClass var = expression->Destinations[i];
		CheckExpression(&var);
	}
	
	CheckExpression(expression->Expression);
}

void t4p::PhpIdentifierLintClass::ExpressionIssetFound(pelet::IssetExpressionClass* expression) {

	// check any array accesses in the destination variables
	// ie $user[$name]
	for (size_t i = 0; i < expression->Expressions.size(); ++i) {
		CheckExpression(expression->Expressions[i]);
	}
}

void t4p::PhpIdentifierLintClass::ExpressionEvalFound(pelet::EvalExpressionClass* expression) {
	CheckExpression(expression->Expression);
}

void t4p::PhpIdentifierLintClass::CheckExpression(pelet::ExpressionClass* expr) {
	switch (expr->ExpressionType) {
	case pelet::ExpressionClass::ARRAY:
		CheckArrayDefinition((pelet::ArrayExpressionClass*)expr);
		break;
	case pelet::ExpressionClass::ASSIGNMENT:
		ExpressionAssignmentFound((pelet::AssignmentExpressionClass*)expr);
		break;
	case pelet::ExpressionClass::ASSIGNMENT_COMPOUND:
		ExpressionAssignmentCompoundFound((pelet::AssignmentCompoundExpressionClass*)expr);
		break;
	case pelet::ExpressionClass::ASSIGNMENT_LIST:
		ExpressionAssignmentListFound((pelet::AssignmentListExpressionClass*)expr);
		break;
	case pelet::ExpressionClass::BINARY_OPERATION:
		ExpressionBinaryOperationFound((pelet::BinaryOperationClass*)expr);
		break;
	case pelet::ExpressionClass::NEW_CALL:
		ExpressionNewInstanceFound((pelet::NewInstanceExpressionClass*)expr);
		break;
	case pelet::ExpressionClass::SCALAR:
		ExpressionScalarFound((pelet::ScalarExpressionClass*)expr);
		break;
	case pelet::ExpressionClass::TERNARY_OPERATION:
		ExpressionTernaryOperationFound((pelet::TernaryOperationClass*)expr);
		break;
	case pelet::ExpressionClass::UNARY_OPERATION:
		ExpressionUnaryOperationFound((pelet::UnaryOperationClass*)expr);
		break;
	case pelet::ExpressionClass::UNARY_VARIABLE_OPERATION:
		ExpressionUnaryVariableOperationFound((pelet::UnaryVariableOperationClass*)expr);
		break;
	case pelet::ExpressionClass::INSTANCEOF_OPERATION:
		ExpressionInstanceOfOperationFound((pelet::InstanceOfOperationClass*)expr);
		break;
	case pelet::ExpressionClass::VARIABLE:
		CheckVariable((pelet::VariableClass*)expr);
		break;
	case pelet::ExpressionClass::INCLUDE:
		ExpressionIncludeFound((pelet::IncludeExpressionClass*)expr);
		break;
	case pelet::ExpressionClass::CLOSURE:
		ExpressionClosureFound((pelet::ClosureExpressionClass*)expr);
		break;
	case pelet::ExpressionClass::ISSET:
		ExpressionIssetFound((pelet::IssetExpressionClass*)expr);
		break;
	case pelet::ExpressionClass::EVAL:
		ExpressionEvalFound((pelet::EvalExpressionClass*)expr);
		break;
	case pelet::ExpressionClass::ARRAY_PAIR:
	
		// we dont event get array pairs by themselves, they come in 
		// with the array
		break;
	case pelet::ExpressionClass::UNKNOWN:
	
		// cannot check unknown expressions
		break;
	}
}

void t4p::PhpIdentifierLintClass::CheckVariable(pelet::VariableClass* var) {
	if (var->ChainList.empty()) {
		return;
	}

	// TODO:
	// checks to implement
	// 1. type hints with classes that are not defined
	if (var->ChainList[0].IsFunction) {
		CheckFunctionName(var->ChainList[0], var);
		
		// check the function parameters
		std::vector<pelet::ExpressionClass*>::const_iterator it;
		for (it = var->ChainList[0].CallArguments.begin(); it != var->ChainList[0].CallArguments.end(); ++it) {
			CheckExpression(*it);
		}
	}
	else if (var->ChainList[0].Name.charAt(0) != '$' && var->ChainList.size() > 1) {

		// a classname in a static method call, ie User::DEFAULT_NAME
		CheckClassNameAndLog(var->ChainList[0].Name, var->LineNumber, var->Pos);
	}

	// check the rest of the variable property/method accesses
	for (size_t i = 1; i < var->ChainList.size(); ++i) {
		pelet::VariablePropertyClass prop = var->ChainList[i];
		if (prop.IsFunction) {
			CheckMethodName(prop, var);

			// check the function parameters
			std::vector<pelet::ExpressionClass*>::const_iterator itArg;
			for (itArg = prop.CallArguments.begin(); itArg != prop.CallArguments.end(); ++itArg) {
				CheckExpression(*itArg);
			}
		}
		else if (prop.IsArrayAccess && prop.ArrayAccess) {

			// check for array accesees ie $user[$name]
			CheckExpression(var->ChainList[i].ArrayAccess);
		}
		else if (!prop.IsArrayAccess && prop.IsStatic) {
				
			// only check for static properties (and constants)
			// for now
			// testing for instance properties results in many false 
			// positives when the analyzed code deals with serializing
			// and unserializing data from strings (json_encode)
			CheckPropertyName(prop, var);
		}
		
	}
}

void t4p::PhpIdentifierLintClass::CheckArrayDefinition(pelet::ArrayExpressionClass* expr) {
	std::vector<pelet::ArrayPairExpressionClass*>::const_iterator it;
	for (it = expr->ArrayPairs.begin(); it != expr->ArrayPairs.end(); ++it) {
		pelet::ArrayPairExpressionClass* pair = *it;
		if (pair->Key) {
			CheckExpression(pair->Key);
		}
		CheckExpression(pair->Value);
	}
}

void t4p::PhpIdentifierLintClass::CheckFunctionName(const pelet::VariablePropertyClass& functionProp, pelet::VariableClass* var) {
	if (Errors.size() > MAX_ERRORS) {
		return;
	}
	
	// we check to see if the function is a native function first
	// native functions never have a namespace
	// but the parser always returns a fully qualified name because
	// it does not know what functions are native and which aren't
	UnicodeString functionName = functionProp.Name;
	if (functionName == UNICODE_STRING_SIMPLE("\\method_exists") || functionName == UNICODE_STRING_SIMPLE("method_exists")) {
		HasMethodExistsCalled = true;
	}
	
	UnicodeString unqualifiedName;
	int32_t pos = functionName.lastIndexOf(UNICODE_STRING_SIMPLE("\\"));
	bool isUnknown = false;
	bool foundInMap = false;
	wxString error;
	if (pos >= 0) {
		functionName.extract(pos + 1, functionName.length() - pos - 1, unqualifiedName);
	}
	
	// first see if we have looked up this function before
	if (!unqualifiedName.isEmpty() && FoundFunctions.find(unqualifiedName) != FoundFunctions.end()) {
		isUnknown = false;
		foundInMap = true;
	}
	else if (!unqualifiedName.isEmpty() && NotFoundFunctions.find(unqualifiedName) != NotFoundFunctions.end()) {
		isUnknown = true;
		foundInMap = true;
	}
	else if (FoundFunctions.find(functionName) != FoundFunctions.end()) {
		isUnknown = false;
		foundInMap = true;
	}
	else if (NotFoundFunctions.find(functionName) != NotFoundFunctions.end()) {
		isUnknown = true;
		foundInMap = true;
	}
	
	// if we have not found the answer in the maps, then perform 
	// a tag cache lookup, lookup the unqualified name in the native
	// function tag cache
	bool foundInTagCache = false;
	if (!foundInMap && !unqualifiedName.isEmpty() && NativeFunctionLookup.IsOk()) {
		NativeFunctionLookup.Set(unqualifiedName);
		NativeFunctionLookup.ReExec(error);
		if (NativeFunctionLookup.Found()) {
			FoundFunctions[unqualifiedName] = 1;
			isUnknown = false;
			foundInTagCache = true;
		}
		else {
			NotFoundFunctions[unqualifiedName] = 1;
			isUnknown = true;
		}
	}
	
	// lookup the qualified name in the global tag cache
	if (!foundInMap && !foundInTagCache && NativeFunctionLookup.IsOk()) {
		NativeFunctionLookup.Set(functionName);
		NativeFunctionLookup.ReExec(error);
		if (NativeFunctionLookup.Found()) {
			FoundFunctions[functionName] = 1;
			isUnknown = false;
			foundInTagCache = true;
		}
		else {
			NotFoundFunctions[functionName] = 1;
			isUnknown = true;
		}
	}
	
	// lookup the unqualified name in the global tag cache
	if (!foundInMap && !foundInTagCache && FunctionLookup.IsOk()) {
		FunctionLookup.Set(unqualifiedName);
		FunctionLookup.ReExec(error);
		if (FunctionLookup.Found()) {
			FoundFunctions[unqualifiedName] = 1;
			isUnknown = false;
			foundInTagCache = true;
		}
		else {
			NotFoundFunctions[unqualifiedName] = 1;
			isUnknown = true;
		}
	}
	
	// lookup the qualified name in the global tag cache
	if (!foundInMap && !foundInTagCache && FunctionLookup.IsOk()) {
		FunctionLookup.Set(functionName);
		FunctionLookup.ReExec(error);
		wxASSERT_MSG(error.empty(), error);
		bool isFound = FunctionLookup.Found();		
		if (isFound) {
			FoundFunctions[functionName] = 1;
			isUnknown = false;
		}
		else {
			NotFoundFunctions[functionName] = 1;
			isUnknown = true;
		}
	}
	if (isUnknown) {
		t4p::PhpIdentifierLintResultClass lintResult;
		lintResult.File = File;
		lintResult.LineNumber = var->LineNumber;
		lintResult.Pos = var->Pos;
		lintResult.Type = t4p::PhpIdentifierLintResultClass::UNKNOWN_FUNCTION;
		lintResult.Identifier = functionName;
		Errors.push_back(lintResult);
	}
}

void t4p::PhpIdentifierLintClass::CheckMethodName(const pelet::VariablePropertyClass& methodProp, pelet::VariableClass* var) {
	if (Errors.size() > MAX_ERRORS) {
		return;
	}
	
	// skip empty methods or "variable methods names
	// it $this->$methodName()
	// also skip if we have seen a call to method_exists just exit; as the code
	// may not ever produce an error
	if (methodProp.Name.isEmpty() ||
		methodProp.Name[0] == '$' ||
		HasMethodExistsCalled) {
		return;
	}
	
	// magic methods, constructors are always put in the cache automatically above
	// this same code will work for those also
	bool isStaticCall = methodProp.IsStatic;
	if (isStaticCall && var->ChainList.size() == 2) {
		
		// check for calls to the base class
		// these calls are NOT static
		//
		// parent::baseMethod()
		// self::method()
		// class MyClass {  
		//    function mm() { MyClass::mm(); } 
		// }
		
		UnicodeString unqualifiedClassName;
		int32_t pos = var->ChainList[0].Name.lastIndexOf(UNICODE_STRING_SIMPLE("\\"));
		if (pos >= 0) {
			var->ChainList[0].Name.extract(pos + 1, var->ChainList[0].Name.length() - pos - 1, unqualifiedClassName);
		}
		else {
			unqualifiedClassName = var->ChainList[0].Name;
		}
	
		isStaticCall = var->ChainList[0].Name.caseCompare(UNICODE_STRING_SIMPLE("parent"), 0) != 0
			&& var->ChainList[0].Name.caseCompare(UNICODE_STRING_SIMPLE("self"), 0) != 0
			&& unqualifiedClassName.caseCompare(CurrentClassName, 0) != 0;
	}

	bool isUnknown = false;
	wxString error;
	// check out little cache, different maps depending on static vs instances methods
	if (isStaticCall && FoundStaticMethods.find(methodProp.Name) != FoundStaticMethods.end()) {
		isUnknown = false;
	}
	else if (isStaticCall && NotFoundStaticMethods.find(methodProp.Name) != NotFoundStaticMethods.end()) {
		isUnknown = true;
	}
	else if (!isStaticCall && FoundMethods.find(methodProp.Name) != FoundMethods.end()) {
		isUnknown = false;
	}
	else if (!isStaticCall && NotFoundMethods.find(methodProp.Name) != NotFoundMethods.end()) {
		isUnknown = true;
	}
	else {
		bool found = false;
		if (MethodLookup.IsOk()) {
			MethodLookup.Set(methodProp.Name, isStaticCall);
			MethodLookup.ReExec(error);
			found = MethodLookup.Found();
		}
		if (!found && NativeMethodLookup.IsOk()) {
			NativeMethodLookup.Set(methodProp.Name, isStaticCall);
			NativeMethodLookup.ReExec(error);
			found = NativeMethodLookup.Found();
		}
		if (found && !isStaticCall) {
			FoundMethods[methodProp.Name] = 1;
			isUnknown = false;
		}
		else if (!found && !isStaticCall) {
			NotFoundMethods[methodProp.Name] = 1;
			isUnknown = true;
		}
		else if (found && isStaticCall) {
			FoundStaticMethods[methodProp.Name] = 1;
			isUnknown = false;
		}
		else if (!found && isStaticCall) {
			NotFoundStaticMethods[methodProp.Name] = 1;
			isUnknown = true;
		}
	}

	if (isUnknown) {
		t4p::PhpIdentifierLintResultClass lintResult;
		lintResult.File = File;
		lintResult.LineNumber = var->LineNumber;
		lintResult.Pos = var->Pos;
		lintResult.Type = t4p::PhpIdentifierLintResultClass::UNKNOWN_METHOD;
		lintResult.Identifier = methodProp.Name;
		Errors.push_back(lintResult);
	}
}

void t4p::PhpIdentifierLintClass::CheckPropertyName(const pelet::VariablePropertyClass& propertyProp, pelet::VariableClass* var) {
	if (Errors.size() > MAX_ERRORS) {
		return;
	}
	
	// only check for static properties (and constants)
	// for now
	// testing for instance properties results in many false 
	// positives when the analyzed code deals with serializing
	// and unserializing data from strings (json_encode)
	if (!propertyProp.IsStatic) {
		return;
	}
	if (propertyProp.Name.isEmpty()) {
		return;
	}
			
	bool isUnknown = false;
	wxString error;
	
	// check our internal class cache, if not found
	// then query the tags db and cache the value
	if (propertyProp.IsStatic && FoundStaticProperties.find(propertyProp.Name) != FoundStaticProperties.end()) {
		isUnknown = false;
	}
	else if (propertyProp.IsStatic && NotFoundStaticProperties.find(propertyProp.Name) != NotFoundStaticProperties.end()) {
		isUnknown = true;
	}
	else {
		bool found = false;
		if (PropertyLookup.IsOk()) {
			PropertyLookup.Set(propertyProp.Name, propertyProp.IsStatic);
			PropertyLookup.ReExec(error);
			found = PropertyLookup.Found();
		}
		if (!found && NativePropertyLookup.IsOk()) {
			NativePropertyLookup.Set(propertyProp.Name, propertyProp.IsStatic);
			NativePropertyLookup.ReExec(error);
			found = NativePropertyLookup.Found();
		}
		if (found && propertyProp.IsStatic) {
			FoundStaticProperties[propertyProp.Name] = 1;
			isUnknown = false;
		}
		else if (!found && propertyProp.IsStatic) {
			NotFoundStaticProperties[propertyProp.Name] = 1;
			isUnknown = true;
		}
	}

	if (isUnknown) {
		t4p::PhpIdentifierLintResultClass lintResult;
		lintResult.File = File;
		lintResult.LineNumber = var->LineNumber;
		lintResult.Pos = var->Pos;
		lintResult.Type = t4p::PhpIdentifierLintResultClass::UNKNOWN_PROPERTY;
		lintResult.Identifier = propertyProp.Name;
		Errors.push_back(lintResult);
	}
}


void t4p::PhpIdentifierLintClass::CheckClassNameAndLog(const UnicodeString& className, int lineNumber, int pos) {
	if (Errors.size() > MAX_ERRORS) {
		return;
	}
	bool isKnown = CheckClassName(className);
	if (!isKnown) {
		t4p::PhpIdentifierLintResultClass lintResult;
		lintResult.File = File;
		lintResult.LineNumber = lineNumber;
		lintResult.Pos = pos;
		lintResult.Type = t4p::PhpIdentifierLintResultClass::UNKNOWN_CLASS;
		lintResult.Identifier = className;
		Errors.push_back(lintResult);
	}
}

bool t4p::PhpIdentifierLintClass::CheckClassName(const UnicodeString& className) {
	
	// some class names are never unknown
	if (className.isEmpty() ||
		className.compare(UNICODE_STRING_SIMPLE("parent")) == 0 ||
		className.compare(UNICODE_STRING_SIMPLE("self")) == 0 ||
		className.compare(UNICODE_STRING_SIMPLE("static")) == 0 ||
		className.caseCompare(UNICODE_STRING_SIMPLE("stdClass"), 0) == 0 ||
		className.caseCompare(UNICODE_STRING_SIMPLE("\\stdClass"), 0) == 0 ||
		className.caseCompare(UNICODE_STRING_SIMPLE("\\__PHP_Incomplete_Class"), 0) == 0 ||
		className.caseCompare(UNICODE_STRING_SIMPLE("__PHP_Incomplete_Class"), 0) == 0) {
		return true;
	}
	if (FoundClasses.find(className) != FoundClasses.end()) {
		return true;
	}
	bool isUnknown = false;
	wxString error;
	if (NotFoundClasses.find(className) != NotFoundClasses.end()) {
		isUnknown = true;
	}
	else {
		bool found = false;
		if (ClassLookup.IsOk()) {
			ClassLookup.Set(className);
			ClassLookup.ReExec(error);
			found = ClassLookup.Found();
		}
		if (!found && NativeClassLookup.IsOk()) {
			NativeClassLookup.Set(className);
			NativeClassLookup.ReExec(error);
			found = NativeClassLookup.Found();
		}
		if (!found) {
			isUnknown = true;
			NotFoundClasses[className] = 1;
		}
		else {
			FoundClasses[className] = 1;
		}
	}
	return !isUnknown;
}

bool t4p::PhpIdentifierLintClass::CheckNamespace(const UnicodeString& namespaceName) {
	bool found = false;
	if (NamespaceLookup.IsOk()) {
		NamespaceLookup.Set(namespaceName);
		wxString error;
		NamespaceLookup.ReExec(error);
		found = NamespaceLookup.Found();
	}
	return found;
}