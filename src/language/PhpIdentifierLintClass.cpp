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
#include <algorithm>

mvceditor::PhpIdentifierLintResultClass::PhpIdentifierLintResultClass()
: Identifier()
, File()
, LineNumber(0)
, Pos(0) 
, Type(NONE) {

}

mvceditor::PhpIdentifierLintResultClass::PhpIdentifierLintResultClass(const mvceditor::PhpIdentifierLintResultClass& src)
: Identifier()
, File()
, LineNumber(0)
, Pos(0) 
, Type(NONE) {
	Copy(src);
}

mvceditor::PhpIdentifierLintResultClass& 
mvceditor::PhpIdentifierLintResultClass::operator=(const mvceditor::PhpIdentifierLintResultClass& src) {
	Copy(src);
	return *this;
}

void mvceditor::PhpIdentifierLintResultClass::Copy(const mvceditor::PhpIdentifierLintResultClass& src) {
	Identifier = src.Identifier;
	File =  src.File;
	LineNumber = src.LineNumber;
	Pos = src.Pos;
	Type = src.Type;
}


mvceditor::PhpIdentifierLintClass::PhpIdentifierLintClass(mvceditor::TagCacheClass& tagCache)
: Errors()
, Parser() 
, File()
, TagCache(tagCache) {
	Parser.SetClassMemberObserver(this);
	Parser.SetClassObserver(this);
	Parser.SetExpressionObserver(this);
	Parser.SetFunctionObserver(this);
}

void mvceditor::PhpIdentifierLintClass::SetVersion(pelet::Versions version) {
	Parser.SetVersion(version);
}

bool mvceditor::PhpIdentifierLintClass::ParseFile(const wxFileName& fileName, 
															std::vector<mvceditor::PhpIdentifierLintResultClass>& errors) {
	Errors.clear();
	File = mvceditor::WxToIcu(fileName.GetFullPath());
	pelet::LintResultsClass lintResult;

	wxFFile file;
	if (file.Open(fileName.GetFullPath(), wxT("rb"))) {
		bool good = Parser.ScanFile(file.fp(), mvceditor::WxToIcu(fileName.GetFullPath()), lintResult);
		errors = Errors;
	}
	return !errors.empty();
}

bool mvceditor::PhpIdentifierLintClass::ParseString(const UnicodeString& code, 
															  std::vector<mvceditor::PhpIdentifierLintResultClass>& errors) {
	
	Errors.clear();
	File = UNICODE_STRING_SIMPLE("");
	pelet::LintResultsClass lintResult;
	bool good = Parser.ScanString(code, lintResult);
	errors = Errors;
	return !errors.empty();
}

void mvceditor::PhpIdentifierLintClass::DefineDeclarationFound(const UnicodeString& namespaceName, 
													 const UnicodeString& variableName, 
													 const UnicodeString& variableValue, 
													 const UnicodeString& comment, 
													 const int lineNumber) {

}

void mvceditor::PhpIdentifierLintClass::MethodFound(const UnicodeString& namespaceName, const UnicodeString& className, 
										  const UnicodeString& methodName, const UnicodeString& signature, 
										  const UnicodeString& returnType, const UnicodeString& comment,
										  pelet::TokenClass::TokenIds visibility, bool isStatic, const int lineNumber) {
	
}

void mvceditor::PhpIdentifierLintClass::FunctionFound(const UnicodeString& namespaceName, const UnicodeString& functionName, 
											const UnicodeString& signature, const UnicodeString& returnType, 
											const UnicodeString& comment, const int lineNumber) {
}

void mvceditor::PhpIdentifierLintClass::NamespaceUseFound(const UnicodeString& namespaceName, const UnicodeString& alias, int startingPos) {
}

void mvceditor::PhpIdentifierLintClass::ExpressionFunctionArgumentFound(pelet::VariableClass* variable) {
}

void mvceditor::PhpIdentifierLintClass::ExpressionVariableFound(pelet::VariableClass* expression) {
	CheckVariable(expression);
}

void mvceditor::PhpIdentifierLintClass::ExpressionAssignmentFound(pelet::AssignmentExpressionClass* expression) {
	CheckExpression(&expression->Destination);
	CheckExpression(expression->Expression);
}

void mvceditor::PhpIdentifierLintClass::ExpressionAssignmentCompoundFound(pelet::AssignmentCompoundExpressionClass* expression) {
	CheckExpression(&expression->Variable);
	CheckExpression(expression->RightOperand);
}

void mvceditor::PhpIdentifierLintClass::ExpressionBinaryOperationFound(pelet::BinaryOperationClass* expression) {
	CheckExpression(expression->LeftOperand);
	CheckExpression(expression->RightOperand);
}

void mvceditor::PhpIdentifierLintClass::ExpressionUnaryOperationFound(pelet::UnaryOperationClass* expression) {
	CheckExpression(expression->Operand);
}

void mvceditor::PhpIdentifierLintClass::ExpressionUnaryVariableOperationFound(pelet::UnaryVariableOperationClass* expression) {
	CheckVariable(&expression->Variable);
}

void mvceditor::PhpIdentifierLintClass::ExpressionTernaryOperationFound(pelet::TernaryOperationClass* expression) {
	CheckExpression(expression->Expression1);
	CheckExpression(expression->Expression2);
	if (expression->Expression3) {
		CheckExpression(expression->Expression3);
	}
}

void mvceditor::PhpIdentifierLintClass::ExpressionScalarFound(pelet::ScalarExpressionClass* expression) {
	
	// nothing as scalars cannot be undefined
}

void mvceditor::PhpIdentifierLintClass::ExpressionNewInstanceFound(pelet::NewInstanceExpressionClass* expression) {
	std::vector<pelet::ExpressionClass*>::const_iterator constructorArg = expression->CallArguments.begin();
	for (; constructorArg != expression->CallArguments.end(); ++constructorArg) {
		CheckExpression(*constructorArg);
	}

	UnicodeString fullyQualifiedClassName = expression->ClassName;
	std::vector<mvceditor::TagClass> tags = TagCache.ExactClass(fullyQualifiedClassName);
	if (tags.empty()) {
		mvceditor::PhpIdentifierLintResultClass lintResult;
		lintResult.File = File;
		lintResult.LineNumber = expression->LineNumber;
		lintResult.Pos = expression->Pos;
		lintResult.Type = mvceditor::PhpIdentifierLintResultClass::UNKNOWN_CLASS;
		lintResult.Identifier = expression->ClassName;
		Errors.push_back(lintResult);	
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

void mvceditor::PhpIdentifierLintClass::StatementGlobalVariablesFound(pelet::GlobalVariableStatementClass* variables) {
	
	// global statement only contain "simple" variables, ie no function/method calls
}

void mvceditor::PhpIdentifierLintClass::StatementStaticVariablesFound(pelet::StaticVariableStatementClass* variables) {

	// static statements only contain "simple" variables, ie no function/method calls
}

void mvceditor::PhpIdentifierLintClass::ExpressionIncludeFound(pelet::IncludeExpressionClass* expr) {
	CheckExpression(expr->Expression);
}

void mvceditor::PhpIdentifierLintClass::ExpressionClosureFound(pelet::ClosureExpressionClass* expr) {
	
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
void mvceditor::PhpIdentifierLintClass::ExpressionAssignmentListFound(pelet::AssignmentListExpressionClass* expression) {

	// check any array accesses in the destination variables
	// ie $user[$name]
	for (size_t i = 0; i < expression->Destinations.size(); ++i) {
		pelet::VariableClass var = expression->Destinations[i];
		CheckExpression(&var);
	}
	
	CheckExpression(expression->Expression);
}

void mvceditor::PhpIdentifierLintClass::CheckExpression(pelet::ExpressionClass* expr) {
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

void mvceditor::PhpIdentifierLintClass::CheckVariable(pelet::VariableClass* var) {
	if (var->ChainList.empty()) {
		return;
	}

	// TODO:
	// checks to implement
	// 1. attempt to inherit from undefined base classes
	// 2. attempt to implement undefined interfaces
	// 3. type hints with classes that are not defined
	// 4. namespace declarations with namespaces that are not defined
	// 5. unused namespace imports
	if (var->ChainList[0].IsFunction) {
		
		// check the function name
		// we check to see if the function is a native function first
		// native functions never have a namespace
		// but the parser always returns a fully qualified name because
		// it does not know what functions are native and which aren't
		UnicodeString functionName = var->ChainList[0].Name;
		int32_t pos = functionName.lastIndexOf(UNICODE_STRING_SIMPLE("\\"));
		bool functionFound = false;
		if (pos >= 0) {
			UnicodeString unqualifiedName;
			functionName.extract(pos + 1, functionName.length() - pos - 1, unqualifiedName);
			mvceditor::TagResultClass* result = TagCache.ExactNativeTags(unqualifiedName);
			if (result) {
				functionFound = !result->Empty();
			}
		}
		if (!functionFound) {
			std::vector<mvceditor::TagClass> tags = TagCache.ExactFunction(functionName);
			if (tags.empty()) {
				mvceditor::PhpIdentifierLintResultClass lintResult;
				lintResult.File = File;
				lintResult.LineNumber = var->LineNumber;
				lintResult.Pos = var->Pos;
				lintResult.Type = mvceditor::PhpIdentifierLintResultClass::UNKNOWN_FUNCTION;
				lintResult.Identifier = functionName;
				Errors.push_back(lintResult);
			}
		}

		// check the function parameters
		std::vector<pelet::ExpressionClass*>::const_iterator it;
		for (it = var->ChainList[0].CallArguments.begin(); it != var->ChainList[0].CallArguments.end(); ++it) {
			CheckExpression(*it);
		}
	}

	// check the rest of the variable property/method accesses
	for (size_t i = 1; i < var->ChainList.size(); ++i) {
		pelet::VariablePropertyClass prop = var->ChainList[i];
		if (prop.IsFunction) {
			std::vector<mvceditor::TagClass> tags = TagCache.ExactMethod(prop.Name, prop.IsStatic);
			if (tags.empty()) {
				mvceditor::PhpIdentifierLintResultClass lintResult;
				lintResult.File = File;
				lintResult.LineNumber = var->LineNumber;
				lintResult.Pos = var->Pos;
				lintResult.Type = mvceditor::PhpIdentifierLintResultClass::UNKNOWN_METHOD;
				lintResult.Identifier = prop.Name;
				Errors.push_back(lintResult);
			}
		}
		else if (prop.IsArrayAccess && prop.ArrayAccess) {

			// check for array accesees ie $user[$name]
			CheckExpression(var->ChainList[i].ArrayAccess);
		}
		else if (!prop.IsArrayAccess) {
			std::vector<mvceditor::TagClass> tags = TagCache.ExactProperty(prop.Name, prop.IsStatic);
			if (tags.empty()) {
				mvceditor::PhpIdentifierLintResultClass lintResult;
				lintResult.File = File;
				lintResult.LineNumber = var->LineNumber;
				lintResult.Pos = var->Pos;
				lintResult.Type = mvceditor::PhpIdentifierLintResultClass::UNKNOWN_PROPERTY;
				lintResult.Identifier = prop.Name;
				Errors.push_back(lintResult);
			}
		}
		
	}
}

void mvceditor::PhpIdentifierLintClass::CheckArrayDefinition(pelet::ArrayExpressionClass* expr) {
	std::vector<pelet::ArrayPairExpressionClass*>::const_iterator it;
	for (it = expr->ArrayPairs.begin(); it != expr->ArrayPairs.end(); ++it) {
		pelet::ArrayPairExpressionClass* pair = *it;
		if (pair->Key) {
			CheckExpression(pair->Key);
		}
		CheckExpression(pair->Value);
	}
}