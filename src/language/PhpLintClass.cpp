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
#include <language/PhpLintClass.h>
#include <globals/String.h>
#include <algorithm>

mvceditor::PhpLintResultClass::PhpLintResultClass()
: VariableName()
, LineNumber(0)
, Pos(0) 
, Type(NONE) {

}

mvceditor::PhpLintResultClass::PhpLintResultClass(const mvceditor::PhpLintResultClass& src)
: VariableName()
, LineNumber(0)
, Pos(0) 
, Type(NONE) {
	Copy(src);
}

mvceditor::PhpLintResultClass& 
mvceditor::PhpLintResultClass::operator=(const mvceditor::PhpLintResultClass& src) {
	Copy(src);
	return *this;
}

void mvceditor::PhpLintResultClass::Copy(const mvceditor::PhpLintResultClass& src) {
	VariableName = src.VariableName;
	LineNumber = src.LineNumber;
	Pos = src.Pos;
	Type = src.Type;
}


mvceditor::PhpLintClass::PhpLintClass()
: Errors()
, ScopeVariables()
, PredefinedVariables()
, Parser() {
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
		PredefinedVariables.push_back(variables[i]);
	}
}

void mvceditor::PhpLintClass::SetVersion(pelet::Versions version) {
	Parser.SetVersion(version);
}

bool mvceditor::PhpLintClass::ParseFile(const wxFileName& fileName, 
															std::vector<mvceditor::PhpLintResultClass>& errors) {
	return true;																
}

bool mvceditor::PhpLintClass::ParseString(const UnicodeString& code, 
															  std::vector<mvceditor::PhpLintResultClass>& errors) {
	
	Errors.clear();
	pelet::LintResultsClass lintResult;
	bool good = Parser.ScanString(code, lintResult);
	errors = Errors;
	return !errors.empty();
}

void mvceditor::PhpLintClass::DefineDeclarationFound(const UnicodeString& namespaceName, 
													 const UnicodeString& variableName, 
													 const UnicodeString& variableValue, 
													 const UnicodeString& comment, 
													 const int lineNumber) {

}

void mvceditor::PhpLintClass::MethodFound(const UnicodeString& namespaceName, const UnicodeString& className, 
										  const UnicodeString& methodName, const UnicodeString& signature, 
										  const UnicodeString& returnType, const UnicodeString& comment,
										  pelet::TokenClass::TokenIds visibility, bool isStatic, const int lineNumber) {
	ScopeVariables.clear();
}

void mvceditor::PhpLintClass::FunctionFound(const UnicodeString& namespaceName, const UnicodeString& functionName, 
											const UnicodeString& signature, const UnicodeString& returnType, 
											const UnicodeString& comment, const int lineNumber) {
	ScopeVariables.clear();
}

void mvceditor::PhpLintClass::ExpressionFunctionArgumentFound(pelet::VariableClass* variable) {
	
	// functino arguments go in the initialized list
	if (!variable->ChainList.empty()) {
		ScopeVariables.push_back(variable->ChainList[0].Name);
	}
}

void mvceditor::PhpLintClass::ExpressionVariableFound(pelet::VariableClass* expression) {
	CheckVariable(expression);
}

void mvceditor::PhpLintClass::ExpressionAssignmentFound(pelet::AssignmentExpressionClass* expression) {
	
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
		
		ScopeVariables.push_back(expression->Destination.ChainList[0].Name);
	}
}

void mvceditor::PhpLintClass::ExpressionAssignmentCompoundFound(pelet::AssignmentCompoundExpressionClass* expression) {
	CheckExpression(expression->RightOperand);

	// for now, ignore assignments to properties ie. $obj->prop1
	if (expression->Variable.ChainList.size() == 1 && 
		!expression->Variable.ChainList[0].IsFunction) {
		
		ScopeVariables.push_back(expression->Variable.ChainList[0].Name);
	}
}

void mvceditor::PhpLintClass::ExpressionBinaryOperationFound(pelet::BinaryOperationClass* expression) {
	CheckExpression(expression->LeftOperand);
	CheckExpression(expression->RightOperand);
}

void mvceditor::PhpLintClass::ExpressionUnaryOperationFound(pelet::UnaryOperationClass* expression) {
	CheckExpression(expression->Operand);
}

void mvceditor::PhpLintClass::ExpressionUnaryVariableOperationFound(pelet::UnaryVariableOperationClass* expression) {
	CheckVariable(&expression->Variable);
}

void mvceditor::PhpLintClass::ExpressionTernaryOperationFound(pelet::TernaryOperationClass* expression) {
	CheckExpression(expression->Expression1);
	CheckExpression(expression->Expression2);
	if (expression->Expression3) {
		CheckExpression(expression->Expression3);
	}
}

void mvceditor::PhpLintClass::ExpressionScalarFound(pelet::ScalarExpressionClass* expression) {
	
	// nothing as scalars cannot be undefined
}

void mvceditor::PhpLintClass::ExpressionNewInstanceFound(pelet::NewInstanceExpressionClass* expression) {
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

void mvceditor::PhpLintClass::StatementGlobalVariablesFound(pelet::GlobalVariableStatementClass* variables) {
	
	// global statement brings in variables, so we add them to 
	// the defined list
	for (size_t i = 0; i < variables->Variables.size(); ++i) {
		if (!variables->Variables[i]->ChainList.empty()) {
			UnicodeString varName = variables->Variables[i]->ChainList[0].Name;
			ScopeVariables.push_back(varName);
		}
	}
}

void mvceditor::PhpLintClass::StatementStaticVariablesFound(pelet::StaticVariableStatementClass* variables) {

	// static statement brings in variables, so we add them to 
	// the defined list
	for (size_t i = 0; i < variables->Variables.size(); ++i) {
		if (!variables->Variables[i]->ChainList.empty()) {
			UnicodeString varName = variables->Variables[i]->ChainList[0].Name;
			ScopeVariables.push_back(varName);
		}
	}
}

void mvceditor::PhpLintClass::ExpressionIncludeFound(pelet::IncludeExpressionClass* expr) {
	CheckExpression(expr->Expression);
}

void mvceditor::PhpLintClass::ExpressionClosureFound(pelet::ClosureExpressionClass* expr) {
	
	// for a closure, we add the closure parameters and the lexical
	// var ("use" variables) as into the scope.  we also define a new
	// scope for the closure.
	std::vector<UnicodeString> closureScopeVariables;
	for (size_t i = 0; i < expr->Parameters.size(); ++i) {
		pelet::VariableClass* param = expr->Parameters[i];
		if (!param->ChainList.empty()) {
			closureScopeVariables.push_back(param->ChainList[0].Name);
		}
	}
	for (size_t i = 0; i < expr->LexicalVars.size(); ++i) {
		pelet::VariableClass* param = expr->LexicalVars[i];
		if (!param->ChainList.empty()) {
			closureScopeVariables.push_back(param->ChainList[0].Name);
		}
	}

	// copy the current scope to be replaced back after we deal with the closure
	std::vector<UnicodeString> oldScope = ScopeVariables;
	ScopeVariables = closureScopeVariables;

	for (size_t i = 0; i < expr->Statements.Size(); ++i) {
		if (pelet::StatementClass::EXPRESSION == expr->Statements.TypeAt(i)) {
			CheckExpression((pelet::ExpressionClass*)expr->Statements.At(i));
		}
	}
	
	// put the old scope back
	ScopeVariables = oldScope;
}
void mvceditor::PhpLintClass::ExpressionAssignmentListFound(pelet::AssignmentListExpressionClass* expression) {

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
			
			ScopeVariables.push_back(var.ChainList[0].Name);
		}
	}
}

void mvceditor::PhpLintClass::CheckExpression(pelet::ExpressionClass* expr) {
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

void mvceditor::PhpLintClass::CheckVariable(pelet::VariableClass* var) {
	if (var->ChainList.empty()) {
		return;
	}

	// TODO:
	// more checks that would be great to implement
	//  1. variable inside interporlated strings  "this is your name: {$name}"
	   ///  2. functions calls to functions that are not defined in a project
  	   ///  3. new calls with classes that are not defined in a project
	   ///  4. undefined properties of an object
	   ///  5. undefined methods of an object
	//  6. variable variables  "$obj->{$methodName}"
	  ///  9. attempt to inherit from undefined base classes
	  /// 10. attempt to implement undefined interfaces
	  /// 11. type hints with classes that are not defined
	  /// 12. namespace declarations with namespaces that are not defined
	if (!var->ChainList[0].IsFunction) {
		UnicodeString varName = var->ChainList[0].Name;
		if (ScopeVariables.end() == std::find(ScopeVariables.begin(), ScopeVariables.end(), varName)
			&& PredefinedVariables.end() == std::find(PredefinedVariables.begin(), PredefinedVariables.end(), varName)) {
			mvceditor::PhpLintResultClass lintResult;
			lintResult.LineNumber = var->LineNumber;
			lintResult.Pos = 0;
			lintResult.Type = mvceditor::PhpLintResultClass::UNINITIALIZED_VARIABLE;
			lintResult.VariableName = varName;
			Errors.push_back(lintResult);
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

void mvceditor::PhpLintClass::CheckArrayDefinition(pelet::ArrayExpressionClass* expr) {
	std::vector<pelet::ArrayPairExpressionClass*>::const_iterator it;
	for (it = expr->ArrayPairs.begin(); it != expr->ArrayPairs.end(); ++it) {
		pelet::ArrayPairExpressionClass* pair = *it;
		if (pair->Key) {
			CheckExpression(pair->Key);
		}
		CheckExpression(pair->Value);
	}
}