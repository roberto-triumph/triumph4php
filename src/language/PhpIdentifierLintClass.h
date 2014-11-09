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
#ifndef __T4P_PHPIDENTIFIERLINTCLASS_H__
#define __T4P_PHPIDENTIFIERLINTCLASS_H__

#include <pelet/ParserClass.h>
#include <pelet/ParserTypeClass.h>
#include <language/ParsedTagFinderClass.h>
#include <globals/String.h>
#include <globals/Sqlite.h>
#include <wx/filename.h>

namespace t4p {

// forward declaration
class TagCacheClass;

/**
 * name and position of an unknown class/function/method
 * found in a PHP file.
 */
class PhpIdentifierLintResultClass {
	
public:

	enum Types {
		NONE,
		UNKNOWN_CLASS,
		UNKNOWN_METHOD,
		UNKNOWN_PROPERTY,
		UNKNOWN_FUNCTION,
		UNKNOWN_INTERFACE
	};

	/**
	 * the identifier that is unknown
	 */
	UnicodeString Identifier;

	/**
	 * the file that ther identifier was found in
	 */
	UnicodeString File;

	/**
	 * the line number (1- based) the unknown identifier was found in
	 */
	int LineNumber;
	
	/**
	 * the character offset (0-based) in the file where the unknown identifier was
	 * found at
	 */
	int Pos;

	Types Type;

	PhpIdentifierLintResultClass();

	PhpIdentifierLintResultClass(const t4p::PhpIdentifierLintResultClass& src);

	t4p::PhpIdentifierLintResultClass& operator=(const t4p::PhpIdentifierLintResultClass& src);

	void Copy(const t4p::PhpIdentifierLintResultClass& src);
};

/**
 * The PhpIdentifierLintClass is a class that is used
 * to find any unknown class, method, function, or namespace in a PHP file. If a file
 * contains syntax errors, then no unkown identifiers will
 * not be found.
 */
class PhpIdentifierLintClass :
	public pelet::ClassObserverClass, 
	public pelet::ClassMemberObserverClass, 
	public pelet::FunctionObserverClass, 
	public pelet::ExpressionObserverClass  {

public:

	PhpIdentifierLintClass();
	
	/**
	 * prepares the internal cache used for function lookups. This needs
	 * to be called before any file is checked.
	 * the tag cache is used to lookup function, class, method or property
	 * names
	 * @param tagCache the tag dbs
	 */
	void Init(t4p::TagCacheClass& tagCache);

	/**
	 * Set the version that the PHP parser should use. This method should be
	 * called BEFORE parsing a file / string
	 */
	void SetVersion(pelet::Versions version);

	/**
	 * @param fileName the file to parse and report errors on.
	 * @param errors any unknown identifier errors will be 
	 *        appended to this parameter.
	 * @return bool TRUE if there is at least one unknown identifier
	 */
	bool ParseFile(const wxFileName& fileName, std::vector<t4p::PhpIdentifierLintResultClass>& errors);

	/**
	 * @param code the string to parse and report errors on
	 * @param errors any unknown identifier errors will be 
	 *        appended to this parameter.
	 * @return bool TRUE if there is at least one error
	 */
	bool ParseString(const UnicodeString& code, std::vector<t4p::PhpIdentifierLintResultClass>& errors);

	// these methods are callbacks from the parser
	// whenever we see an expression or statement, we 
	// will check it to see if it contains undefined
	// variables
	void DefineDeclarationFound(const UnicodeString& namespaceName, const UnicodeString& variableName, const UnicodeString& variableValue, 
			const UnicodeString& comment, const int lineNumber);

	void MethodFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName, 
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment,
		pelet::TokenClass::TokenIds visibility, bool isStatic, const int lineNumber);
	
	void FunctionFound(const UnicodeString& namespaceName, const UnicodeString& functionName, 
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment,
		const int lineNumber);

	void NamespaceUseFound(const UnicodeString& namespaceName, const UnicodeString& alias, int startingPos);
		
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
	std::vector<t4p::PhpIdentifierLintResultClass> Errors;

	/**
	 * the parser will parse the PHP code and call the *Found() methods
	 * when a variable, expression, etc.. is found
	 */
	pelet::ParserClass Parser;

	/**
	 * the file being currently parsed. empty string when a string of
	 * code is parsed.
	 */
	UnicodeString File;

	/**
	 * used to lookup classes, functions, methods
	 * use this to lookup into the tag cache
	 * we keep an instance stored as a class
	 * variable so that we prepare the statement once
	 * during the time we parse the ENTIRE set 
	 * of files
	 */
	t4p::ClassLookupClass ClassLookup;
	t4p::MethodLookupClass MethodLookup;
	t4p::PropertyLookupClass PropertyLookup;
	t4p::FunctionLookupClass FunctionLookup;
	t4p::ClassLookupClass NativeClassLookup;
	t4p::MethodLookupClass NativeMethodLookup;
	t4p::PropertyLookupClass NativePropertyLookup;
	t4p::FunctionLookupClass NativeFunctionLookup;

	/**
	 * caching results of methods and function lookups, will 
	 * only hit the tag cache once per file per method/function
	 */
	std::map<UnicodeString, int, t4p::UnicodeStringComparatorClass> 
		FoundClasses,
		FoundMethods,
		FoundProperties,
		FoundFunctions,
		FoundStaticMethods,
		FoundStaticProperties,
		NotFoundClasses,
		NotFoundMethods,
		NotFoundProperties,
		NotFoundFunctions,
		NotFoundStaticMethods,
		NotFoundStaticProperties;
	
	/**
	 * keep track of calls to method_exists() function; if a call
	 * is being made then disable method name checks as the code
	 * may never produce an actual error when ran. Example:
	 *
	 *  if (method_exists($route, 'getHost')) {
     *        $this->host = $route->getHost() ? : null;
     *   } else {
     *       $this->host = null;
     *   }
	 */
	bool HasMethodExistsCalled;

	/**
	 * @param var the expression to check. A Check  will be
	 *       done to see if any of the identifiers used in the 
	 *       given expression are not found in the cache
	 *       if not, then a new error is 
	 *       created and appended to the Errors vector.
	 */
	void CheckExpression(pelet::ExpressionClass* expr);

	/**
	 * @param var the variable to check. A Check  will be
	 *       done to see if the any of the identifiers
	 *       in the given variable are not found in the cache. if 
	 *       not, then a new error is 
	 *       created and appended to the Errors vector.
	 */
	void CheckVariable(pelet::VariableClass* var);

	/**
	 * @param expr array definition to check. check array($key => $value)
	 *        A Check  will be
	 *        done to see if the any of the identifiers
	 *        in the given array key-value pairs are not found in the cache. if 
	 *        not, then a new error is 
	 *        created and appended to the Errors vector.
	 */
	void CheckArrayDefinition(pelet::ArrayExpressionClass* expr);
	
	/**
	 * lookup the class name in the tag cache; if it is not found
	 * then add an error
	 * @param className the name of the class to look up,
	 * @param expression the parent expression, used to get line number to populate the
	 *        error string
	 */
	void CheckClassName(const UnicodeString& className, pelet::ExpressionClass* expression);
	
	/**
	 * lookup the method in the tag cache; if it is not found
	 * then add an error
	 * @param methodProp the name of the method to look up,
	 * @param var the parent variable expression, used to get line number to populate the
	 *        error string
	 */
	void CheckMethodName(const pelet::VariablePropertyClass& methodProp, pelet::VariableClass* var);

	/**
	 * lookup the property in the tag cache; if it is not found
	 * then add an error
	 * @param propertyProp the name of the function to look up,
	 * @param var the parent variable expression, used to get line number to populate the
	 *        error string
	 */
	void CheckPropertyName(const pelet::VariablePropertyClass& propertyProp, pelet::VariableClass* var);

	/**
	 * lookup the function in the tag cache; if it is not found
	 * then add an error
	 * @param functionProp the name of the function to look up,
	 * @param var the parent variable expression, used to get line number to populate the
	 *        error string
	 */
	void CheckFunctionName(const pelet::VariablePropertyClass& functionProp, pelet::VariableClass* var);

};

}

#endif
