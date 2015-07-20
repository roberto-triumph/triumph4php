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
#include <language_php/PhpVariableLintClass.h>
#include <language_php/TagCacheClass.h>
#include <language_php/ParsedTagFinderClass.h>
#include <globals/String.h>
#include <wx/ffile.h>
#include <algorithm>
#include <map>
#include <vector>

/**
 * we will stop tracking errors after we have reached this
 * many.  The user cannot possibly go through all
 * of them
 */
static const size_t MAX_ERRORS = 100;

/**
 * this function will check to see if an argument is passed by reference. The
 * function
 *
 * @param tag the function tag; it contains the function signature, the function
 *        signature contains a '&' if an argument is pass-by-reference
 * @param argIndex the argument to check. 0 == first argument
 * @return bool if TRUE, argument argIndex is passed by reference
 */
static bool IsFunctionArgumentByReference(const UnicodeString& signature, int argIndex) {
	bool isRef = false;
	int32_t start = signature.indexOf('(');
	int32_t next = signature.indexOf(',', start);
	if (next < 0) {
		// only 1 argument, look for the ending )
		next = signature.indexOf(')', next + 1);
	}
	int arg = 0;
	while (next >= 0) {
		if (arg == argIndex) {
			// this is the argument that we want to
			// check
			UnicodeString variable;
			signature.extract(start, (next - start + 1), variable);
			if (variable.indexOf('&') >= 0) {
				isRef = true;
			}

			// only need to check 1 variable
			break;
		}
		arg++;

		// find the end of the next variable
		start = next;
		next = signature.indexOf(',', start + 1);
		if (next < 0) {
			// last argument, look for the ending )
			next = signature.indexOf(')', next + 1);
		}
	}
	return isRef;
}

t4p::PhpVariableLintResultClass::PhpVariableLintResultClass()
: VariableName()
, File()
, LineNumber(0)
, Pos(0)
, Type(NONE) {
}

t4p::PhpVariableLintResultClass::PhpVariableLintResultClass(const t4p::PhpVariableLintResultClass& src)
: VariableName()
, File()
, LineNumber(0)
, Pos(0)
, Type(NONE) {
	Copy(src);
}

t4p::PhpVariableLintResultClass&
t4p::PhpVariableLintResultClass::operator=(const t4p::PhpVariableLintResultClass& src) {
	Copy(src);
	return *this;
}

void t4p::PhpVariableLintResultClass::Copy(const t4p::PhpVariableLintResultClass& src) {
	VariableName = src.VariableName;
	File = src.File;
	LineNumber = src.LineNumber;
	Pos = src.Pos;
	Type = src.Type;
}

t4p::PhpVariableLintOptionsClass::PhpVariableLintOptionsClass(const t4p::PhpVariableLintOptionsClass& src)
: CheckGlobalScope(false)
, Version(pelet::PHP_53) {
	Copy(src);
}

t4p::PhpVariableLintOptionsClass& t4p::PhpVariableLintOptionsClass::operator=(const t4p::PhpVariableLintOptionsClass& src) {
	Copy(src);
	return *this;
}

void t4p::PhpVariableLintOptionsClass::Copy(const t4p::PhpVariableLintOptionsClass& src) {
	CheckGlobalScope = src.CheckGlobalScope;
	Version = src.Version;
}

t4p::PhpVariableLintOptionsClass::PhpVariableLintOptionsClass()
: CheckGlobalScope(false)
, Version(pelet::PHP_53) {
}

t4p::PhpVariableLintClass::PhpVariableLintClass()
: ExpressionObserverClass()
, Errors()
, ScopeVariables()
, PredefinedVariables()
, HasExtractCall(false)
, HasEvalCall(false)
, HasIncludeCall(false)
, HasIndirectVariable(false)
, Parser()
, Options()
, File()
, FunctionSignatureLookup()
, MethodSignatureLookup()
, NativeFunctionSignatureLookup() {
	Parser.SetClassMemberObserver(this);
	Parser.SetClassObserver(this);
	Parser.SetExpressionObserver(this);
	Parser.SetFunctionObserver(this);

	UnicodeString variables[] =  {
		UNICODE_STRING_SIMPLE("$GLOBALS"),
		UNICODE_STRING_SIMPLE("$_SERVER"),
		UNICODE_STRING_SIMPLE("$_GET"),
		UNICODE_STRING_SIMPLE("$_POST"),
		UNICODE_STRING_SIMPLE("$_FILES"),
		UNICODE_STRING_SIMPLE("$_REQUEST"),
		UNICODE_STRING_SIMPLE("$_SESSION"),
		UNICODE_STRING_SIMPLE("$_ENV"),
		UNICODE_STRING_SIMPLE("$_COOKIE"),
		UNICODE_STRING_SIMPLE("$php_errormsg"),
		UNICODE_STRING_SIMPLE("$HTTP_RAW_POST_DATA"),
		UNICODE_STRING_SIMPLE("$http_response_header"),
		UNICODE_STRING_SIMPLE("$argc"),
		UNICODE_STRING_SIMPLE("$argv")
	};
	for (int i = 0; i < 14; ++i) {
		PredefinedVariables[variables[i]] = 1;
	}
}

void t4p::PhpVariableLintClass::Init(t4p::TagCacheClass& tagCache) {
	tagCache.GlobalPrepare(FunctionSignatureLookup, true);
	tagCache.GlobalPrepare(MethodSignatureLookup, true);
	tagCache.NativePrepare(NativeFunctionSignatureLookup, true);
}

void t4p::PhpVariableLintClass::SetOptions(const t4p::PhpVariableLintOptionsClass& options) {
	Options = options;
}

bool t4p::PhpVariableLintClass::ParseFile(const wxFileName& fileName,
															std::vector<t4p::PhpVariableLintResultClass>& errors) {
	Errors.clear();
	ScopeVariables.clear();
	Parser.SetVersion(Options.Version);
	HasExtractCall = false;
	HasEvalCall = false;
	HasIncludeCall = false;
	HasIndirectVariable = false;
	File = t4p::WxToIcu(fileName.GetFullPath());

	pelet::LintResultsClass lintResult;
	wxFFile file;
	if (file.Open(fileName.GetFullPath(), wxT("rb"))) {
		Parser.ScanFile(file.fp(), t4p::WxToIcu(fileName.GetFullPath()), lintResult);
		errors = Errors;
	}
	return !errors.empty();
}

bool t4p::PhpVariableLintClass::ParseString(const UnicodeString& code,
															  std::vector<t4p::PhpVariableLintResultClass>& errors) {
	Errors.clear();
	ScopeVariables.clear();
	Parser.SetVersion(Options.Version);
	HasExtractCall = false;
	HasEvalCall = false;
	HasIncludeCall = false;
	File = UNICODE_STRING_SIMPLE("");

	pelet::LintResultsClass lintResult;
	Parser.ScanString(code, lintResult);
	errors = Errors;
	return !errors.empty();
}

void t4p::PhpVariableLintClass::DefineDeclarationFound(const UnicodeString& namespaceName,
													 const UnicodeString& variableName,
													 const UnicodeString& variableValue,
													 const UnicodeString& comment,
													 const int lineNumber) {
}

void t4p::PhpVariableLintClass::MethodFound(const UnicodeString& namespaceName, const UnicodeString& className,
										  const UnicodeString& methodName, const UnicodeString& signature,
										  const UnicodeString& returnType, const UnicodeString& comment,
										  pelet::TokenClass::TokenIds visibility, bool isStatic, const int lineNumber,
										  bool hasVariableArguments) {
	ScopeVariables.clear();
	ScopeVariables[UNICODE_STRING_SIMPLE("$this")] = 1;
	HasExtractCall = false;
	HasEvalCall = false;
	HasIncludeCall = false;
	HasIndirectVariable = false;
}

void t4p::PhpVariableLintClass::FunctionFound(const UnicodeString& namespaceName, const UnicodeString& functionName,
											const UnicodeString& signature, const UnicodeString& returnType,
											const UnicodeString& comment, const int lineNumber, bool hasVariableArguments) {
	ScopeVariables.clear();
	HasExtractCall = false;
	HasEvalCall = false;
	HasIncludeCall = false;
	HasIndirectVariable = false;
}

void t4p::PhpVariableLintClass::ExpressionFunctionArgumentFound(pelet::VariableClass* variable) {
	// function arguments go in the initialized list
	if (!variable->ChainList.empty()) {
		ScopeVariables[variable->ChainList[0].Name] = 1;
	}
}

void t4p::PhpVariableLintClass::ExpressionVariableFound(pelet::VariableClass* expression) {
	CheckVariable(expression);
}

void t4p::PhpVariableLintClass::ExpressionAssignmentFound(pelet::AssignmentExpressionClass* expression) {
	// if an assigned variable is an indirect or variable variable
	// like so " $$name = '123'; "
	// we turn off uninitialized varaible checks
	// because we don't want to label false positives.
	if (expression->Destination.IsIndirect) {
		HasIndirectVariable = true;
	}

	// check any array accesses in the destination variable
	// ie $user[$name]
	for (size_t i = 0; i < expression->Destination.ChainList.size(); ++i) {
		if (expression->Destination.ChainList[i].IsArrayAccess && expression->Destination.ChainList[i].ArrayAccess) {
			CheckExpression(expression->Destination.ChainList[i].ArrayAccess);
		}
	}

	CheckExpression(expression->Expression);

	// for now, ignore assignments to properties ie. $obj->prop1
	// but we want to check arrays ie $data['name'] = '';
	if (expression->Destination.ChainList.size() == 1 &&
		!expression->Destination.ChainList[0].IsFunction) {
		ScopeVariables[expression->Destination.ChainList[0].Name] = 1;
	} else if (expression->Destination.ChainList.size() > 1 &&
		expression->Destination.ChainList[1].IsArrayAccess) {
		ScopeVariables[expression->Destination.ChainList[0].Name] = 1;
	}
}

void t4p::PhpVariableLintClass::ExpressionAssignmentCompoundFound(pelet::AssignmentCompoundExpressionClass* expression) {
	CheckExpression(expression->RightOperand);

	// for now, ignore assignments to properties ie. $obj->prop1
	if (expression->Variable.ChainList.size() == 1 &&
		!expression->Variable.ChainList[0].IsFunction) {
		ScopeVariables[expression->Variable.ChainList[0].Name] = 1;
	}
}

void t4p::PhpVariableLintClass::ExpressionBinaryOperationFound(pelet::BinaryOperationClass* expression) {
	CheckExpression(expression->LeftOperand);
	CheckExpression(expression->RightOperand);
}

void t4p::PhpVariableLintClass::ExpressionUnaryOperationFound(pelet::UnaryOperationClass* expression) {
	CheckExpression(expression->Operand);
}

void t4p::PhpVariableLintClass::ExpressionUnaryVariableOperationFound(pelet::UnaryVariableOperationClass* expression) {
	CheckVariable(&expression->Variable);
}

void t4p::PhpVariableLintClass::ExpressionTernaryOperationFound(pelet::TernaryOperationClass* expression) {
	CheckExpression(expression->Expression1);
	CheckExpression(expression->Expression2);
	if (expression->Expression3) {
		CheckExpression(expression->Expression3);
	}
}

void t4p::PhpVariableLintClass::ExpressionInstanceOfOperationFound(pelet::InstanceOfOperationClass* expression) {
	CheckExpression(expression->Expression1);
}

void t4p::PhpVariableLintClass::ExpressionScalarFound(pelet::ScalarExpressionClass* expression) {
	// nothing as scalars cannot be undefined
}

void t4p::PhpVariableLintClass::ExpressionNewInstanceFound(pelet::NewInstanceExpressionClass* expression) {
	std::vector<pelet::ExpressionClass*>::const_iterator constructorArg = expression->CallArguments.begin();
	for (; constructorArg != expression->CallArguments.end(); ++constructorArg) {
		CheckExpression(*constructorArg);
	}

	// check any function args to any chained method calls.
	std::vector<pelet::VariablePropertyClass>::const_iterator prop = expression->ChainList.begin();
	for (; prop != expression->ChainList.end(); ++prop) {
		std::vector<pelet::ExpressionClass*>::const_iterator chainArg = prop->CallArguments.begin();
		for (; chainArg != prop->CallArguments.end(); ++chainArg) {
			CheckExpression(*chainArg);
		}
	}
}

void t4p::PhpVariableLintClass::StatementGlobalVariablesFound(pelet::GlobalVariableStatementClass* variables) {
	// global statement brings in variables, so we add them to
	// the defined list
	for (size_t i = 0; i < variables->Variables.size(); ++i) {
		if (!variables->Variables[i]->ChainList.empty()) {
			UnicodeString varName = variables->Variables[i]->ChainList[0].Name;
			ScopeVariables[varName] = 1;
		}
	}
}

void t4p::PhpVariableLintClass::StatementStaticVariablesFound(pelet::StaticVariableStatementClass* variables) {
	// static statement brings in variables, so we add them to
	// the defined list
	for (size_t i = 0; i < variables->Variables.size(); ++i) {
		if (!variables->Variables[i]->ChainList.empty()) {
			UnicodeString varName = variables->Variables[i]->ChainList[0].Name;
			ScopeVariables[varName] = 1;
		}
	}
}

void t4p::PhpVariableLintClass::ExpressionIncludeFound(pelet::IncludeExpressionClass* expr) {
	CheckExpression(expr->Expression);

	// set the flag after we check the expression on the right; that way
	// we can find unitialized variables in the expression on the right
	HasIncludeCall = true;
}

void t4p::PhpVariableLintClass::ExpressionClosureFound(pelet::ClosureExpressionClass* expr) {
	// for a closure, we add the closure parameters and the lexical
	// var ("use" variables) as into the scope.  we also define a new
	// scope for the closure.
	std::map<UnicodeString, int, t4p::UnicodeStringComparatorClass> closureScopeVariables;
	for (size_t i = 0; i < expr->Parameters.size(); ++i) {
		pelet::VariableClass* param = expr->Parameters[i];
		if (!param->ChainList.empty()) {
			UnicodeString paramName = param->ChainList[0].Name;
			if (param->IsReference) {
				// if this is a parameter passed by reference, then add
				// it to the function scope
				ScopeVariables[paramName] = 1;
			}
			closureScopeVariables[paramName] = 1;
		}
	}
	for (size_t i = 0; i < expr->LexicalVars.size(); ++i) {
		pelet::VariableClass* param = expr->LexicalVars[i];
		if (!param->ChainList.empty()) {
			UnicodeString paramName = param->ChainList[0].Name;
			if (param->IsReference) {
				// if this is a parameter passed by reference, then add
				// it to the function scope
				ScopeVariables[paramName] = 1;
			}
			closureScopeVariables[paramName] = 1;
		}
	}

	// copy the current scope to be replaced back after we deal with the closure
	std::map<UnicodeString, int, t4p::UnicodeStringComparatorClass> oldScope = ScopeVariables;
	bool oldExtractCalled = HasExtractCall;
	bool oldEvalCalled = HasEvalCall;
	bool oldIncludeCalled = HasIncludeCall;

	ScopeVariables = closureScopeVariables;
	HasExtractCall = false;
	HasEvalCall = false;
	HasIncludeCall = false;

	for (size_t i = 0; i < expr->Statements.Size(); ++i) {
		pelet::StatementClass::Types stmtType = expr->Statements.TypeAt(i);
		if (pelet::StatementClass::EXPRESSION == stmtType) {
			CheckExpression((pelet::ExpressionClass*)expr->Statements.At(i));
		} else if (pelet::StatementClass::STATIC_VARIABLE_DECLARATION == stmtType) {
			StatementStaticVariablesFound((pelet::StaticVariableStatementClass*)expr->Statements.At(i));
		} else if (pelet::StatementClass::GLOBAL_VARIABLE_DECLARATION == stmtType) {
			StatementGlobalVariablesFound((pelet::GlobalVariableStatementClass*)expr->Statements.At(i));
		}
	}

	// put the old scope back
	ScopeVariables = oldScope;
	HasExtractCall = oldExtractCalled;
	HasEvalCall = oldEvalCalled;
	HasIncludeCall = oldIncludeCalled;
}

void t4p::PhpVariableLintClass::ExpressionIssetFound(pelet::IssetExpressionClass* expression) {
	// isset expressions have a bit of logic
	//
	// examples that don't trigger PHP notices
	//  isset($arrNames)
	//  isset($arrNames['123'])
	//
	// examples that do trigger PHP notices
	//  isset($arrNames[$name])   when $name has not been initialized
	//

	// first check to see if this isset is for a single, simple variable
	// if so then we should never label it as uninitialized
	if (expression->Expressions.size() == 1 &&
		pelet::ExpressionClass::VARIABLE == expression->Expressions[0]->ExpressionType) {
		pelet::VariableClass* var = (pelet::VariableClass*) expression->Expressions[0];
		if (var->ChainList.size() == 1) {
			return;
		}

		// only check the array keys
		for (size_t i = 0; i < var->ChainList.size(); ++i) {
			if (var->ChainList[i].IsArrayAccess && var->ChainList[i].ArrayAccess) {
				CheckExpression(var->ChainList[i].ArrayAccess);
			}
		}
	}
}

void t4p::PhpVariableLintClass::ExpressionEvalFound(pelet::EvalExpressionClass* expression) {
	HasEvalCall = true;
	CheckExpression(expression->Expression);
}

void t4p::PhpVariableLintClass::ExpressionAssignmentListFound(pelet::AssignmentListExpressionClass* expression) {
	// check any array accesses in the destination variables
	// ie $user[$name]
	for (size_t i = 0; i < expression->Destinations.size(); ++i) {
		pelet::VariableClass var = expression->Destinations[i];
		for (size_t j = 0; j < var.ChainList.size(); ++j) {
			if (var.ChainList[j].IsArrayAccess && var.ChainList[j].ArrayAccess) {
				CheckExpression(var.ChainList[j].ArrayAccess);
			}
		}
	}

	CheckExpression(expression->Expression);

	// add the assigned variables to the scope
	// for now, ignore assignments to properties ie. $obj->prop1
	for (size_t i = 0; i < expression->Destinations.size(); ++i) {
		pelet::VariableClass var = expression->Destinations[i];
		if (var.ChainList.size() == 1 &&
			!var.ChainList[0].IsFunction) {
			ScopeVariables[var.ChainList[0].Name] = 1;
		}
	}
}

/**
 * @param functionName the function or method to look up. If its a function, it
 *        needs to be a fully qualified function name.
 * @param isMethod TRUE if the search should be restricted to methods (from all classes)
 * @param isStatic TRUE if the search should be restriced to static methods ('::')
 * @param functionTag the t4p::PhpTagClass that was found; will contain the function signature
 * @return bool TRUE if functionName was found ONLY ONCE in the tag cache
 */
bool t4p::PhpVariableLintClass::LookupSignature(UnicodeString& signature, const UnicodeString& functionName, bool isMethod, bool isStatic) {
	bool found = false;

	// calling IsOk() on the lookup classes to account for the possibility of
	// them not being initialized; only really occurs during unit tests
	// since some unit tests dont create sqlite dbs
	// note that we only return TRUE if there is one match only; multiple
	// matches mean that there are many functions/methods named the same,
	// and we don't know which to use
	wxString error;
	if (!isMethod) {
		FunctionSignatureLookup.Set(functionName);
		FunctionSignatureLookup.ReExec(error);
		if (FunctionSignatureLookup.IsOk() && FunctionSignatureLookup.Found()) {
			// we only return true if there is 1 and only 1 match
			FunctionSignatureLookup.Next();
			if (!FunctionSignatureLookup.More()) {
				signature =  FunctionSignatureLookup.Signature;
				found = true;
			}
		}
	} else if (MethodSignatureLookup.IsOk()) {
		MethodSignatureLookup.Set(functionName, isStatic);
		MethodSignatureLookup.ReExec(error);
		if (MethodSignatureLookup.Found()) {
			// we only return true if there is 1 and only 1 match
			MethodSignatureLookup.Next();
			if (!MethodSignatureLookup.More()) {
				signature =  MethodSignatureLookup.Signature;
				found = true;
			}
		}
	}
	if (!found && NativeFunctionSignatureLookup.IsOk()) {
		// as a last resort, check to see if this is a native function
		// in PHP, native functions do not need to be fully qualified
		UnicodeString unqualifiedName;
		bool hasSet = false;
		int32_t pos = functionName.lastIndexOf(UNICODE_STRING_SIMPLE("\\"));
		if (pos >= 0) {
			functionName.extract(pos + 1, functionName.length() - pos - 1, unqualifiedName);
			NativeFunctionSignatureLookup.Set(unqualifiedName);
			hasSet = true;
		} else {
			NativeFunctionSignatureLookup.Set(functionName);
			hasSet = true;
		}
		if (hasSet) {
			NativeFunctionSignatureLookup.ReExec(error);
			if (NativeFunctionSignatureLookup.Found()) {
				NativeFunctionSignatureLookup.Next();

				// we only return true if there is 1 and only 1 match
				if (!NativeFunctionSignatureLookup.More()) {
					signature = NativeFunctionSignatureLookup.Signature;
					found = true;
				}
			}
		}
	}
	return found;
}


void t4p::PhpVariableLintClass::CheckExpression(pelet::ExpressionClass* expr) {
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
	case pelet::ExpressionClass::INSTANCEOF_OPERATION:
		ExpressionInstanceOfOperationFound((pelet::InstanceOfOperationClass*)expr);
		break;
	case pelet::ExpressionClass::UNARY_OPERATION:
		ExpressionUnaryOperationFound((pelet::UnaryOperationClass*)expr);
		break;
	case pelet::ExpressionClass::UNARY_VARIABLE_OPERATION:
		ExpressionUnaryVariableOperationFound((pelet::UnaryVariableOperationClass*)expr);
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
		ExpressionEvalFound((pelet::EvalExpressionClass*) expr);
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

void t4p::PhpVariableLintClass::CheckVariable(pelet::VariableClass* var) {
	if (var->ChainList.empty()) {
		return;
	}
	if (Errors.size() > MAX_ERRORS) {
		return;
	}

	if (HasIndirectVariable) {
		// an indirect assignment was found, lets not check for uninitialized
		// variables in this scope; we don't want to show false positives.
		return;
	}

	// TODO(roberto):
	// more checks that would be great to implement
	//  1. variable inside interporlated strings  "this is your name: {$name}"
	//
	// note that a "variable" could also be a static method call
	// ie "User::all()", but this does not contain a variable so
	// we skip this
	if (!var->ChainList[0].IsFunction &&  !var->ChainList[0].Name.isEmpty() &&
		var->ChainList[0].Name.charAt(0) == '$') {
		// if options say to not check variables in global scope, then dont check
		if (!var->Scope.IsGlobalScope() || Options.CheckGlobalScope) {
			UnicodeString varName = var->ChainList[0].Name;
			if (!HasExtractCall
				&& !HasEvalCall
				&& !HasIncludeCall
				&& ScopeVariables.find(varName) == ScopeVariables.end()
				&& PredefinedVariables.find(varName) == PredefinedVariables.end()) {
				t4p::PhpVariableLintResultClass lintResult;
				lintResult.File = File;
				lintResult.LineNumber = var->LineNumber;
				lintResult.Pos = var->Pos;
				lintResult.Type = t4p::PhpVariableLintResultClass::UNINITIALIZED_VARIABLE;
				lintResult.VariableName = varName;
				Errors.push_back(lintResult);
			}
		}
	}
	if (var->ChainList[0].IsFunction) {
		// is this a call to extract()
		if (var->ChainList[0].Name.caseCompare(UNICODE_STRING_SIMPLE("extract"), 0) == 0) {
			HasExtractCall = true;
		}
		if (var->ChainList[0].Name.endsWith(UNICODE_STRING_SIMPLE("\\extract"))) {
			// this is a call to extract but this call is within a declared namespace.
			// PHP has a fallback mechanism for namespaced functions; if a function is
			// called inside  namespace, it looks for a function in the declared
			// namespace AND in the global namespace
			HasExtractCall = true;
		}
	}

	// check the function parameters
	// note that function call arguments may themselves be function calls, we
	// need to check all of them
	for (size_t i = 0; i < var->ChainList.size(); ++i) {
		if (var->ChainList[i].IsFunction) {
			UnicodeString functionSignature;
			bool isMethod = i > 0;
			bool foundFunctionTag = LookupSignature(functionSignature, var->ChainList[i].Name, isMethod, var->ChainList[i].IsStatic);

			std::vector<pelet::ExpressionClass*>::const_iterator it;
			int argIndex = 0;
			for (it = var->ChainList[i].CallArguments.begin(); it != var->ChainList[i].CallArguments.end(); ++it) {
				if ((*it)->ExpressionType == pelet::ExpressionClass::VARIABLE) {
					pelet::VariableClass* argVar = (pelet::VariableClass*)*it;
					if (argVar->ChainList.size() == 1 && !argVar->ChainList[0].IsFunction) {
						// check to see if the argument is passed-by-reference. a
						// variable passed by reference could be counted as initialized
						// by the function being called; we want to not label these
						// arguments as uninitialized.
						// we get the called function's signature via the tag cache
						// one of the main questions becomes: how do we
						// know which method to query? for now, we will query using the
						// method name using exact searching. if the method is found, then
						// we know for sure its the right now.  if we find many matches,
						// then a method can be in any number of classes; it
						// becomes really hard to know which method to pick.
						// in the case of multiple functions/methods, we "skip" the reference checks
						if (!foundFunctionTag || IsFunctionArgumentByReference(functionSignature, argIndex)) {
							ScopeVariables[argVar->ChainList[0].Name] = 1;
						}
					}
				}
				argIndex++;
				CheckExpression(*it);
			}
		}
	}

	// check for array accesses ie $user[$name]
	for (size_t i = 0; i < var->ChainList.size(); ++i) {
		if (var->ChainList[i].IsArrayAccess && var->ChainList[i].ArrayAccess) {
			CheckExpression(var->ChainList[i].ArrayAccess);
		}
	}
}

void t4p::PhpVariableLintClass::CheckArrayDefinition(pelet::ArrayExpressionClass* expr) {
	std::vector<pelet::ArrayPairExpressionClass*>::const_iterator it;
	for (it = expr->ArrayPairs.begin(); it != expr->ArrayPairs.end(); ++it) {
		pelet::ArrayPairExpressionClass* pair = *it;
		if (pair->Key) {
			CheckExpression(pair->Key);
		}
		CheckExpression(pair->Value);
	}
}
