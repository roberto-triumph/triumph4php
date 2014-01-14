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
#include <language/PhpVariableLintClass.h>
#include <globals/String.h>
#include <wx/ffile.h>
#include <algorithm>

mvceditor::PhpVariableLintResultClass::PhpVariableLintResultClass()
: VariableName()
, File()
, LineNumber(0)
, Pos(0) 
, Type(NONE) {

}

mvceditor::PhpVariableLintResultClass::PhpVariableLintResultClass(const mvceditor::PhpVariableLintResultClass& src)
: VariableName()
, File()
, LineNumber(0)
, Pos(0) 
, Type(NONE) {
	Copy(src);
}

mvceditor::PhpVariableLintResultClass& 
mvceditor::PhpVariableLintResultClass::operator=(const mvceditor::PhpVariableLintResultClass& src) {
	Copy(src);
	return *this;
}

void mvceditor::PhpVariableLintResultClass::Copy(const mvceditor::PhpVariableLintResultClass& src) {
	VariableName = src.VariableName;
	File = src.File;
	LineNumber = src.LineNumber;
	Pos = src.Pos;
	Type = src.Type;
}

mvceditor::PhpVariableLintOptionsClass::PhpVariableLintOptionsClass(const mvceditor::PhpVariableLintOptionsClass& src)
: CheckGlobalScope(false)
, Version(pelet::PHP_53) {
	Copy(src);
}

mvceditor::PhpVariableLintOptionsClass& mvceditor::PhpVariableLintOptionsClass::operator=(const mvceditor::PhpVariableLintOptionsClass& src) {
	Copy(src);
	return *this;
}

void mvceditor::PhpVariableLintOptionsClass::Copy(const mvceditor::PhpVariableLintOptionsClass& src) {
	CheckGlobalScope = src.CheckGlobalScope;
	Version = src.Version;
}

mvceditor::PhpVariableLintOptionsClass::PhpVariableLintOptionsClass()
: CheckGlobalScope(false)
, Version(pelet::PHP_53) {

}

mvceditor::PhpVariableLintClass::PhpVariableLintClass()
: ExpressionObserverClass()
, Errors()
, ScopeVariables()
, PredefinedVariables()
, Parser() 
, Options()
, File() {
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

void mvceditor::PhpVariableLintClass::SetOptions(const mvceditor::PhpVariableLintOptionsClass& options) {
	Options = options;
}

bool mvceditor::PhpVariableLintClass::ParseFile(const wxFileName& fileName, 
															std::vector<mvceditor::PhpVariableLintResultClass>& errors) {
	Errors.clear();
	ScopeVariables.clear();
	Parser.SetVersion(Options.Version);
	File = mvceditor::WxToIcu(fileName.GetFullPath());
	
	pelet::LintResultsClass lintResult;
	wxFFile file;
	if (file.Open(fileName.GetFullPath(), wxT("rb"))) {
		bool good = Parser.ScanFile(file.fp(), mvceditor::WxToIcu(fileName.GetFullPath()), lintResult);
		errors = Errors;
	}
	return !errors.empty();
}

bool mvceditor::PhpVariableLintClass::ParseString(const UnicodeString& code, 
															  std::vector<mvceditor::PhpVariableLintResultClass>& errors) {
	
	Errors.clear();
	ScopeVariables.clear();
	Parser.SetVersion(Options.Version);
	File = UNICODE_STRING_SIMPLE("");

	pelet::LintResultsClass lintResult;
	bool good = Parser.ScanString(code, lintResult);
	errors = Errors;
	return !errors.empty();
}

void mvceditor::PhpVariableLintClass::DefineDeclarationFound(const UnicodeString& namespaceName, 
													 const UnicodeString& variableName, 
													 const UnicodeString& variableValue, 
													 const UnicodeString& comment, 
													 const int lineNumber) {

}

void mvceditor::PhpVariableLintClass::MethodFound(const UnicodeString& namespaceName, const UnicodeString& className, 
										  const UnicodeString& methodName, const UnicodeString& signature, 
										  const UnicodeString& returnType, const UnicodeString& comment,
										  pelet::TokenClass::TokenIds visibility, bool isStatic, const int lineNumber) {
	ScopeVariables.clear();
	ScopeVariables[UNICODE_STRING_SIMPLE("$this")] = 1;
}

void mvceditor::PhpVariableLintClass::FunctionFound(const UnicodeString& namespaceName, const UnicodeString& functionName, 
											const UnicodeString& signature, const UnicodeString& returnType, 
											const UnicodeString& comment, const int lineNumber) {
	ScopeVariables.clear();
}

void mvceditor::PhpVariableLintClass::ExpressionFunctionArgumentFound(pelet::VariableClass* variable) {
	
	// function arguments go in the initialized list
	if (!variable->ChainList.empty()) {
		ScopeVariables[variable->ChainList[0].Name] = 1;
	}
}

void mvceditor::PhpVariableLintClass::ExpressionVariableFound(pelet::VariableClass* expression) {
	CheckVariable(expression);
}

void mvceditor::PhpVariableLintClass::ExpressionAssignmentFound(pelet::AssignmentExpressionClass* expression) {
	
	// check any array accesses in the destination variable
	// ie $user[$name]
	for (size_t i = 0; i < expression->Destination.ChainList.size(); ++i) {
		if (expression->Destination.ChainList[i].IsArrayAccess && expression->Destination.ChainList[i].ArrayAccess) {
			CheckExpression(expression->Destination.ChainList[i].ArrayAccess);
		}
	}
	
	CheckExpression(expression->Expression);
	
	// for now, ignore assignments to properties ie. $obj->prop1
	if (expression->Destination.ChainList.size() == 1 && 
		!expression->Destination.ChainList[0].IsFunction) {
		
		ScopeVariables[expression->Destination.ChainList[0].Name] = 1;
	}
}

void mvceditor::PhpVariableLintClass::ExpressionAssignmentCompoundFound(pelet::AssignmentCompoundExpressionClass* expression) {
	CheckExpression(expression->RightOperand);

	// for now, ignore assignments to properties ie. $obj->prop1
	if (expression->Variable.ChainList.size() == 1 && 
		!expression->Variable.ChainList[0].IsFunction) {
		
		ScopeVariables[expression->Variable.ChainList[0].Name] = 1;
	}
}

void mvceditor::PhpVariableLintClass::ExpressionBinaryOperationFound(pelet::BinaryOperationClass* expression) {
	CheckExpression(expression->LeftOperand);
	CheckExpression(expression->RightOperand);
}

void mvceditor::PhpVariableLintClass::ExpressionUnaryOperationFound(pelet::UnaryOperationClass* expression) {
	CheckExpression(expression->Operand);
}

void mvceditor::PhpVariableLintClass::ExpressionUnaryVariableOperationFound(pelet::UnaryVariableOperationClass* expression) {
	CheckVariable(&expression->Variable);
}

void mvceditor::PhpVariableLintClass::ExpressionTernaryOperationFound(pelet::TernaryOperationClass* expression) {
	CheckExpression(expression->Expression1);
	CheckExpression(expression->Expression2);
	if (expression->Expression3) {
		CheckExpression(expression->Expression3);
	}
}

void mvceditor::PhpVariableLintClass::ExpressionScalarFound(pelet::ScalarExpressionClass* expression) {
	
	// nothing as scalars cannot be undefined
}

void mvceditor::PhpVariableLintClass::ExpressionNewInstanceFound(pelet::NewInstanceExpressionClass* expression) {
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

void mvceditor::PhpVariableLintClass::StatementGlobalVariablesFound(pelet::GlobalVariableStatementClass* variables) {
	
	// global statement brings in variables, so we add them to 
	// the defined list
	for (size_t i = 0; i < variables->Variables.size(); ++i) {
		if (!variables->Variables[i]->ChainList.empty()) {
			UnicodeString varName = variables->Variables[i]->ChainList[0].Name;
			ScopeVariables[varName] = 1;
		}
	}
}

void mvceditor::PhpVariableLintClass::StatementStaticVariablesFound(pelet::StaticVariableStatementClass* variables) {

	// static statement brings in variables, so we add them to 
	// the defined list
	for (size_t i = 0; i < variables->Variables.size(); ++i) {
		if (!variables->Variables[i]->ChainList.empty()) {
			UnicodeString varName = variables->Variables[i]->ChainList[0].Name;
			ScopeVariables[varName] = 1;
		}
	}
}

void mvceditor::PhpVariableLintClass::ExpressionIncludeFound(pelet::IncludeExpressionClass* expr) {
	CheckExpression(expr->Expression);
}

void mvceditor::PhpVariableLintClass::ExpressionClosureFound(pelet::ClosureExpressionClass* expr) {
	
	// for a closure, we add the closure parameters and the lexical
	// var ("use" variables) as into the scope.  we also define a new
	// scope for the closure.
	std::map<UnicodeString, int, mvceditor::UnicodeStringComparatorClass> closureScopeVariables;
	for (size_t i = 0; i < expr->Parameters.size(); ++i) {
		pelet::VariableClass* param = expr->Parameters[i];
		if (!param->ChainList.empty()) {
			closureScopeVariables[param->ChainList[0].Name] = 1;
		}
	}
	for (size_t i = 0; i < expr->LexicalVars.size(); ++i) {
		pelet::VariableClass* param = expr->LexicalVars[i];
		if (!param->ChainList.empty()) {
			closureScopeVariables[param->ChainList[0].Name] = 1;
		}
	}

	// copy the current scope to be replaced back after we deal with the closure
	std::map<UnicodeString, int, mvceditor::UnicodeStringComparatorClass> oldScope = ScopeVariables;
	ScopeVariables = closureScopeVariables;

	for (size_t i = 0; i < expr->Statements.Size(); ++i) {
		if (pelet::StatementClass::EXPRESSION == expr->Statements.TypeAt(i)) {
			CheckExpression((pelet::ExpressionClass*)expr->Statements.At(i));
		}
	}
	
	// put the old scope back
	ScopeVariables = oldScope;
}
void mvceditor::PhpVariableLintClass::ExpressionAssignmentListFound(pelet::AssignmentListExpressionClass* expression) {

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

void mvceditor::PhpVariableLintClass::CheckExpression(pelet::ExpressionClass* expr) {
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
	case pelet::ExpressionClass::VARIABLE:
		CheckVariable((pelet::VariableClass*)expr);
		break;
	case pelet::ExpressionClass::INCLUDE:
		ExpressionIncludeFound((pelet::IncludeExpressionClass*)expr);
		break;
	case pelet::ExpressionClass::CLOSURE:
		ExpressionClosureFound((pelet::ClosureExpressionClass*)expr);
		break;
	}
}

void mvceditor::PhpVariableLintClass::CheckVariable(pelet::VariableClass* var) {
	if (var->ChainList.empty()) {
		return;
	}

	// TODO:
	// more checks that would be great to implement
	//  1. variable inside interporlated strings  "this is your name: {$name}"
	//  2. variable variables  "$obj->{$methodName}"
	//
	// note that a "variable" could also be a static method call
	// ie "User::all()", but this does not contain a variable so
	// we skip this
	if (!var->ChainList[0].IsFunction &&  !var->ChainList[0].Name.isEmpty() &&
		var->ChainList[0].Name.charAt(0) == '$') {
		
		// if options say to not check variables in global scope, then dont check
		if (!var->Scope.IsGlobalScope() || Options.CheckGlobalScope) {
			UnicodeString varName = var->ChainList[0].Name;
			if (ScopeVariables.find(varName) == ScopeVariables.end()
				&& PredefinedVariables.find(varName) == PredefinedVariables.end()) {
				mvceditor::PhpVariableLintResultClass lintResult;
				lintResult.File = File;
				lintResult.LineNumber = var->LineNumber;
				lintResult.Pos = var->Pos;
				lintResult.Type = mvceditor::PhpVariableLintResultClass::UNINITIALIZED_VARIABLE;
				lintResult.VariableName = varName;
				Errors.push_back(lintResult);
			}
		}
	}
	if (var->ChainList[0].IsFunction) {
		
		// check the function parameters
		std::vector<pelet::ExpressionClass*>::const_iterator it;
		for (it = var->ChainList[0].CallArguments.begin(); it != var->ChainList[0].CallArguments.end(); ++it) {
			CheckExpression(*it);
		}
	}

	// check for array accesees ie $user[$name]
	for (size_t i = 0; i < var->ChainList.size(); ++i) {
		if (var->ChainList[i].IsArrayAccess && var->ChainList[i].ArrayAccess) {
			CheckExpression(var->ChainList[i].ArrayAccess);
		}
	}
}

void mvceditor::PhpVariableLintClass::CheckArrayDefinition(pelet::ArrayExpressionClass* expr) {
	std::vector<pelet::ArrayPairExpressionClass*>::const_iterator it;
	for (it = expr->ArrayPairs.begin(); it != expr->ArrayPairs.end(); ++it) {
		pelet::ArrayPairExpressionClass* pair = *it;
		if (pair->Key) {
			CheckExpression(pair->Key);
		}
		CheckExpression(pair->Value);
	}
}