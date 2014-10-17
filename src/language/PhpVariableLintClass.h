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
#ifndef __T4P_UNINITIALIZEDVARIABLEOBSERVERCLASS_H__
#define __T4P_UNINITIALIZEDVARIABLEOBSERVERCLASS_H__

#include <pelet/ParserClass.h>
#include <pelet/ParserTypeClass.h>
#include <wx/filename.h>
#include <globals/String.h>

namespace t4p {
	

// forward declaration
class TagCacheClass;

/**
 * name and position of an uninitialized variable
 * found in a PHP file.
 */
class PhpVariableLintResultClass {
	
public:

	enum Types {
		NONE,
		UNINITIALIZED_VARIABLE,
		UNUSED_VARIABLE
	};

	/**
	 * the variable that is uninitialized
	 */
	UnicodeString VariableName;

	/**
	 * the file that the error was found in
	 */
	UnicodeString File;

	/**
	 * the line number (1- based) the uninitialized variable was found in
	 */
	int LineNumber;
	
	/**
	 * the character offset (0-based) in the file where the uninitialized variable was
	 * found at
	 */
	int Pos;

	Types Type;

	PhpVariableLintResultClass();

	PhpVariableLintResultClass(const t4p::PhpVariableLintResultClass& src);

	t4p::PhpVariableLintResultClass& operator=(const t4p::PhpVariableLintResultClass& src);

	void Copy(const t4p::PhpVariableLintResultClass& src);
};

/**
 * options that control how the variable linter works
 */
class PhpVariableLintOptionsClass {

public:
	
	/** 
	 * if true, variables in the global scope will be checked.  if false,
	 * only variables inside a method / function will be checked. this is
	 * useful to turn off checking for global variables in templates,
	 * since templates inherit variables from their callers in an unknown fashion.
	 */
	bool CheckGlobalScope;

	/**
	 * PHP version to use when parsing source code
	 */
	pelet::Versions Version;

	PhpVariableLintOptionsClass();
	
	PhpVariableLintOptionsClass(const t4p::PhpVariableLintOptionsClass& src);

	PhpVariableLintOptionsClass& operator=(const t4p::PhpVariableLintOptionsClass& src);

	void Copy (const t4p::PhpVariableLintOptionsClass& src);
	
};

/**
 * The PhpVariableLintClass is a class that is used
 * to find any uninitialized variables in a PHP file. If a file
 * contains syntax errors, then uninitialized variables will
 * not be found.
 */
class PhpVariableLintClass :
	public pelet::ClassObserverClass, 
	public pelet::ClassMemberObserverClass, 
	public pelet::FunctionObserverClass, 
	public pelet::ExpressionObserverClass  {

public:

	PhpVariableLintClass(t4p::TagCacheClass& tagCache);

	/**
	 * Set the version that the PHP parser should use. This method should be
	 * called BEFORE parsing a file / string
	 */
	void SetOptions(const t4p::PhpVariableLintOptionsClass& options);

	/**
	 * @param fileName the file to parse and report errors on.
	 * @param errors any uninitialized variable errors will be 
	 *        appended to this parameter.
	 * @return bool TRUE if there is at least one error
	 */
	bool ParseFile(const wxFileName& fileName, std::vector<t4p::PhpVariableLintResultClass>& errors);

	/**
	 * @param code the string to parse and report errors on
	 * @param errors any uninitialized variable errors will be 
	 *        appended to this parameter.
	 * @return bool TRUE if there is at least one error
	 */
	bool ParseString(const UnicodeString& code, std::vector<t4p::PhpVariableLintResultClass>& errors);

	void DefineDeclarationFound(const UnicodeString& namespaceName, const UnicodeString& variableName, const UnicodeString& variableValue, 
			const UnicodeString& comment, const int lineNumber);

	void MethodFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName, 
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment,
		pelet::TokenClass::TokenIds visibility, bool isStatic, const int lineNumber);
	
	void FunctionFound(const UnicodeString& namespaceName, const UnicodeString& functionName, 
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment,
		const int lineNumber);
		
	void ExpressionVariableFound(pelet::VariableClass* expression);

	void ExpressionAssignmentFound(pelet::AssignmentExpressionClass* expression);

	void ExpressionAssignmentCompoundFound(pelet::AssignmentCompoundExpressionClass* expression);

	void ExpressionBinaryOperationFound(pelet::BinaryOperationClass* expression);

	void ExpressionUnaryOperationFound(pelet::UnaryOperationClass* expression);

	void ExpressionUnaryVariableOperationFound(pelet::UnaryVariableOperationClass* expression);

	void ExpressionTernaryOperationFound(pelet::TernaryOperationClass* expression);
	
	void ExpressionInstanceOfOperationFound(pelet::InstanceOfOperationClass* expression);

	void ExpressionScalarFound(pelet::ScalarExpressionClass* expression);

	void ExpressionNewInstanceFound(pelet::NewInstanceExpressionClass* expression);

	void ExpressionFunctionArgumentFound(pelet::VariableClass* variable);

	void StatementGlobalVariablesFound(pelet::GlobalVariableStatementClass* variables);

	void StatementStaticVariablesFound(pelet::StaticVariableStatementClass* variables);

	void ExpressionIncludeFound(pelet::IncludeExpressionClass* expr);

	void ExpressionClosureFound(pelet::ClosureExpressionClass* expr);
	
	void ExpressionIssetFound(pelet::IssetExpressionClass* expression);
	
	void ExpressionEvalFound(pelet::EvalExpressionClass* expression);

	void ExpressionAssignmentListFound(pelet::AssignmentListExpressionClass* expression);

private:

	/**
	 * errors found so far
	 */
	std::vector<t4p::PhpVariableLintResultClass> Errors;
	
	/**
	 * the tag cache is used to lookup function signatures in order to
	 * check if an argument is passed by reference;pass-by-reference
	 * variables are not tagged as uninitialized
	 */
	t4p::TagCacheClass& TagCache;

	/**
	 * variables found in the current scope
	 */
	std::map<UnicodeString, int, t4p::UnicodeStringComparatorClass> ScopeVariables;

	/**
	 * variables already defined when parsing this file. The 
	 * superglobals ($_GET, $_POST, ...) but it can be any
	 * other variables we know exist when the parsed file
	 * is included.
	 */
	std::map<UnicodeString, int, t4p::UnicodeStringComparatorClass> PredefinedVariables;
	
	/**
	 * flag that will be set when the extract function was called.
	 * we detect the use of extract so that we don't label variables
	 * as uninitialized after a call to extract
	 */
	bool HasExtractCall;
	
	/**
	 * flag that will be set when the eval function was called.
	 * we detect the use of eval so that we don't label variables
	 * as uninitialized after a call to eval
	 */
	bool HasEvalCall;
	
	/**
	 * flag that will be set when the include keyword was used.
	 * we detect the use of include so that we don't label variables
	 * as uninitialized after a call to include
	 */
	bool HasIncludeCall;

	/**
	 * the parser will parse the PHP code and call the *Found() methods
	 * when a variable, expression, etc.. is found
	 */
	pelet::ParserClass Parser;

	t4p::PhpVariableLintOptionsClass Options;

	/**
	 * the file being currently parsed. empty string if a string of
	 * code is being parsed
	 */
	UnicodeString File;

	/**
	 * @param var the expression to check. A Check  will be
	 *       done to see if any of the variables used in the 
	 *       given expression have been previously
	 *       initialized. if not, then a new error is 
	 *       created and appended to the Errors vector.
	 */
	void CheckExpression(pelet::ExpressionClass* expr);

	/**
	 * @param var the variable to check. A Check  will be
	 *       done to see if the variable has been previously
	 *       initialized. if not, then a new error is 
	 *       created and appended to the Errors vector.
	 */
	void CheckVariable(pelet::VariableClass* var);

	/**
	 * @param expr array definition to check. check array($key => $value)
	 *       A Check  will be
	 *       done to see if any variable in the key-value pairs has been previously
	 *       initialized. if not, then a new error is 
	 *       created and appended to the Errors vector.
	 */
	void CheckArrayDefinition(pelet::ArrayExpressionClass* expr);

};

}

#endif
