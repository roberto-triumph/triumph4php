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
#ifndef __CALLSTACKCLASS_H
#define __CALLSTACKCLASS_H

#include <pelet/ParserClass.h>
#include <language_php/TagCacheClass.h>
#include <language_php/SymbolTableClass.h>
#include <unicode/unistr.h>
#include <wx/filename.h>
#include <vector>
#include <queue>

namespace t4p {
	
/**
 * This is a representation of a single variable assignment.  A variable
 * may be assigned with different values (scalar, object, return
 * values from methods, properties).
 * Note that this represents "simple" assignments. For example,
 * this class is no suitable to hold assignments such as 
 * 
 * $name = Person::getInstance()->name;
 * 
 * Statements like above are broken up into multiple temporary
 * variables.  
 */
class VariableSymbolClass {

public:

	enum Types {
		
		/** 
		 * variable is assigned a scalar (string or number) 
		 */
		SCALAR,
		
		/** 
		 * variable is assigned an array. Note that there will be one ARRAY_KEY 
		 *  assignment for each array key
		 */
		ARRAY,
		
		/** 
		 * a new key being assigned to an array.
		 */
		ARRAY_KEY,
		
		/** 
		 * variable is created using the new operator
		 */
		NEW_OBJECT,
		
		/** 
		 * variable is assigned another variable
		 */
		ASSIGN,
		
		/** 
		 * variable is assigned an object property
		 */
		PROPERTY,
		
		/** 
		 * variable is assigned the return value of an object method
		 */
		METHOD_CALL,
		
		/** 
		 * variable is assigned the return value of a function
		 */
		FUNCTION_CALL,
		
		/** 
		 * not a variable; this symbol declares that a new scope
		 * has started.  All of the symbols that follow this symbol belong
		 * in the scope of this function.
		 * FunctionName will contain the function name 
		 * of the scope
		 */
		BEGIN_FUNCTION,
		
		/** 
		 * not a variable; this symbol declares that a new scope
		 * has started.  All of the symbols that follow this symbol belong
		 * in the scope of this method.
		 * ClassName and MethodName will contain the  class and method
		 * of the scope
		 */
		BEGIN_METHOD
	};
	
	Types Type; 
	
	/**
	 *  the variable being assigned (the left side of 
	 *  $a = $b). Will contain the siguil ('$')
	 */
	UnicodeString DestinationVariable;
	
	/**
	 *  if type = SCALAR, then this will be the scalar's lexeme (the contents)
	 *  for the code:
	 *    $s = 'hello';
	 *  ScalarValue will be 'hello' (no quotes)
	 */
	UnicodeString ScalarValue;
	
	/**
	 * if type = ARRAY_KEY this is the key that was assigned to the array
	 *  for the code:
	 *    $s['greeting'] = 'hello';
	 * ArrayKey will be 'greeting' (no quotes)
	 */
	UnicodeString ArrayKey;
	
	/**
	 *  the name of the variable to assign (the right side of 
	 *  $a = $b)
	 */
	UnicodeString SourceVariable;
	
	/**
	 * if type = METHOD or type = PROPERTY, this is the name of the
	 * variable used
	 *  for the code:
	 *    $name =  $person->firstName
	 * ObjectName will be $person
	 */
	UnicodeString ObjectName;
	
	/**
	 * if type = PROPERTY, this is the name of the property
	 * .  this never contains a siguil
	 * for the code:
	 *    $name = $person->firstName
	 * PropertyName will be firstName
	 */
	UnicodeString PropertyName;
	
	/**
	 * if type = METHOD, this is the name of the method being 
	 * called.
	 *  for the code:
	 *    $name = $person->getName()
	 * MethodName will be getName
	 * 
	 * if type = BEGIN_METHOD, this is the name of the 
	 * method where all of the subsquent variables are located in
	 */
	UnicodeString MethodName;
	
	/**
	 * if type = FUNCTION, this is the name of the function being 
	 * called.
	 *  for the code:
	 *    $name = getName()
	 * FunctionName will be getName
	 * 
	 * if type = BEGIN_FUNCTION, this is the name of the 
	 * function where all of the subsquent variables are located in
	 */
	UnicodeString FunctionName;
	
	/**
	 * if type = NEW_OBJECT, this is the name of the class being 
	 * instatiated.
	 *  for the code:
	 *    $user = new UserClass;
	 * ClassName will be UserClass
	 
	 * if type = BEGIN_METHOD, this is the name of the 
	 * class where all of the subsquent variables are located in
	 */
	UnicodeString ClassName;
	
	/**
	 * if type = METHOD or type = FUNCTION this is the list of
	 * variables that were passed into the function / method. These are the
	 * "simple" variables; ie the temporary variables that are
	 * assigned for example the code:
	 * 
	 * $name = buildName(getFirstName($a), getLastName($a));
	 * 
	 * then FunctionArguments are [ $@tmp1, $@tmp2 ]
	 * because the resul of getFirstName and getLastName get assigned to
	 * a temp variable.
	 * 
	 */
	std::vector<UnicodeString> FunctionArguments;
	
	VariableSymbolClass();
	
	void ToScalar(const UnicodeString& variableName, const UnicodeString& scalar);
	
	void ToArray(const UnicodeString& variableName);
	
	void ToArrayKey(const UnicodeString& variableName, const UnicodeString& keyName);
	
	void ToNewObject(const UnicodeString& variableName, const UnicodeString& className);
	
	void ToAssignment(const UnicodeString& variableName, const UnicodeString& sourceVariableName);
	
	void ToProperty(const UnicodeString& variableName, const UnicodeString& objectName, const UnicodeString& propertyName);
	
	void ToMethodCall(const UnicodeString& variableName, const UnicodeString& objectName, const UnicodeString& methodName, const std::vector<UnicodeString> arguments);
	
	void ToFunctionCall(const UnicodeString& variableName, const UnicodeString& functionName, const std::vector<UnicodeString> arguments);
	
	void ToBeginMethod(const UnicodeString& className, const UnicodeString& methodName);
	
	void ToBeginFunction(const UnicodeString& functionName);
	
	/**
	 *  @return std::string serialization
	 */
	std::string ToString() const;
	
	std::string TypeString() const;
};

/**
 * This class will keep track of all function / method calls of a piece of source;
 * this is helpful in building a call stack where a user can easily see what
 * functions are getting called.
 *
 */
class CallStackClass : 
	public pelet::ClassMemberObserverClass, 
	public pelet::FunctionObserverClass, 
	public pelet::ExpressionObserverClass,
	public pelet::ClassObserverClass {

public:

	/**
	 * reasons why building of the call stack could not be completeed
	 */
	enum Errors {
	    NONE,

	    /**
	     * while recursing the call stack, we encountered a non-existant class / method / function
	     */
	    RESOURCE_NOT_FOUND,

	    /**
	     * while recursing the call stack, we encountered one of the files had a parse error
	     */
	    PARSE_ERR0R,

	    /**
	     * while recursing the call stack, we encountered a tag that could not be resolved (an object
	     * without a type
	     */
	    RESOLUTION_ERROR,

	    /**
	     * the call stack has gone past an reasonable length. Since the code checks for recursive methods,
	     * this error code may mean that the function observer cannot handle a certain piece of code.
	     */
	    STACK_LIMIT,
		
		/**
		 * the given tag cache is empty; the call stack class wont be able to quickly know where a 
		 * specific class is located.
		 */
		EMPTY_CACHE
	};

	/**
	 * All of the resolved function calls that took place; this vector is overwritten each time
	 * Build() method is called.
	 */
	std::vector<VariableSymbolClass> Variables;
	
	/**
	 * If given code has a parser error (PARSE_ERR0R), the error will be stored here
	 */
	pelet::LintResultsClass LintResults;
	
	/**
	 * If the call stack could not be completed because a variable method could not be resolved (RESOLUTION_ERROR)
	 * then the error will be stored here. Note that this is NOT a fatal error; CallStack.List is populated, and the 
	 * list is correct; it just means that the call stack may be incomplete.
	 */
	SymbolTableMatchErrorClass MatchError;

	/**
	 * @param TagCacheClass& need the tag cache so that functions can be resolved
	 *        into their corresponding file locations
	 */
	CallStackClass(TagCacheClass& tagCache);

	/**
	 * Will open up the file, parse it, and collect all function calls. This is recursive (will open up
	 * each function call, parse it, and collect all function calls that they first funtion calls).
	 *-
	 * @param fileName the file to parse
	 * @param className the class to start collecting
	 * @param methodName the method to start collecting
	 * @param version the PHP version to parse against
	 * @param error any error is encountered, the cause of the error will be set here
	 * @return TRUE on success, if FALSE then error will be filled.
	 */
	bool Build(const wxFileName& fileName, const UnicodeString& className, const UnicodeString& methodName, pelet::Versions version, Errors& error);
	
	/**
	 * Saves the call stack to a file; in CSV format
	 * Format is as follows:
	 * ResourceType,Identifier,Resource, Arg1 lexeme, Arg2 lexeme, ... Arg N lexeme
	 * 
	 * where
	 * ResourceType = FUNCTION | METHOD
	 * Identifier is the name of the function / method
	 * Resource is the fully qualified name (ie. ClassName::MethodName)
	 * ArgN Lexeme is the lexeme (string) of the Nth argument; lexeme is either the constant (when argument is a string / number)
	 * or it can be a variable name.
	 *  TODO: object operator chains are not currently supported
	 * 
	 * @param session connection where data will be INSERTed to 
	 * @return TRUE if file was successfully written to
	 */
	bool Persist(soci::session& session);
	
	void MethodFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName, 
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment, 
		pelet::TokenClass::TokenIds visibility, bool isStatic, const int lineNumber, bool hasVariableArguments);

	void FunctionFound(const UnicodeString& namespaceName, const UnicodeString& functionName, const UnicodeString& signature, const UnicodeString& returnType, 
		const UnicodeString& comment, const int lineNumber, bool hasVariableArguments);
		
	void ExpressionVariableFound(pelet::VariableClass* expression);

	void ExpressionAssignmentFound(pelet::AssignmentExpressionClass* expression);

	void ExpressionAssignmentCompoundFound(pelet::AssignmentCompoundExpressionClass* expression);

	void ExpressionBinaryOperationFound(pelet::BinaryOperationClass* expression);

	void ExpressionUnaryOperationFound(pelet::UnaryOperationClass* expression);

	void ExpressionUnaryVariableOperationFound(pelet::UnaryVariableOperationClass* expression);

	void ExpressionTernaryOperationFound(pelet::TernaryOperationClass* expression);

	void ExpressionScalarFound(pelet::ScalarExpressionClass* expression);

	void ExpressionNewInstanceFound(pelet::NewInstanceExpressionClass* expression);

	
private:

	/**
	 * this is the class that will parse the source code
	 */
	pelet::ParserClass Parser;

	/**
	 * the current scope; the class that the parser is currently working on. Will
	 * use this to filter out the expressions that we want
	 */
	UnicodeString CurrentClass;

	/**
	 * the current scope; the method that the parser is currently working on. Will
	 * use this to filter out the expressions that we want
	 */
	UnicodeString CurrentMethod;

	/**
	 * the current scope; the function that the parser is currently working on. Will
	 * use this to filter out the expressions that we want
	 */
	UnicodeString CurrentFunction;
	
	/**
	 * a small struct to keep file and resources together while
	 * jumping through the code
	 */
	struct ResourceWithFile {
		
		wxFileName FileName;
		
		t4p::TagClass Resource;
		
		std::vector<pelet::ExpressionClass> CallArguments;
		
		std::vector<t4p::SymbolClass> ScopeVariables;
	};
	
	/**
	 * These are the scopes that we want to collect function calls from.
	 * this is a queue because we want to "follow" calls (open the functions and look inside
	 * those functions for new function calls)
	 */
	std::queue<ResourceWithFile> ResourcesRemaining;
	
	/**
	 * Used to 'jump' to the calling function
	 */
	TagCacheClass& TagCache;
		
	/**
	 * flag each method after we parse it, that way recursice functions
	 * don't cause infinte loops
	 */
	std::map<UnicodeString, bool, UnicodeStringComparatorClass> ParsedMethods;

	/**
	 * index appended to the temporary variables. will be reset every scope.
	 */
	int TempVarIndex;
	
	/**
	 * this flag will be set when the parser hits the scope (tag) we are looking for.  If we dont
	 * find the scope we are looking for, then either the input scope was wrong, or some other
	 * unknown error has occurred
	 */
	bool FoundScope;
	
	void Clear();
	
	bool Recurse(pelet::Versions version, Errors& error);
	
	/**
	 *  breaks up an assignment expression into multiple VariableSymbolClass instances
	 *  We break up a potentially huge statements into simple assignments so that
	 *  we can tell exactly what methods are being called.
	 * 
	 *  For example, take this assignment expression:
	 * 
	 *    $this->view->name = $this->getFullNameFromRequest($this->getRequest());
	 * 
	 * the assignment will be broken up into these variable symbols:
	 * 
	 *   variable      |     type       |     lexemes (class + method + args or class + property or variable)
	 *   -----------------------------------------------------------------------------------------------
	 *   $this         |   ASSIGN       |     
	 *   $@tmp1        |   METHOD       |     $this, getRequest
	 *   $@tmp2        |   METHOD       |     $this, getFullNameFromRequest, $@tmp1
	 *   $@tmp3        |   PROPERTY     |     $this, view
	 *   $@tmp4        |   PROPERTY     |     $@tmp3, name
	 *   $@tmp4        |   ASSIGN       |     $@tmp2
	 * 
	 * note that this method creates "temporary" variables that are not present in the actual
	 * source code (as denoted by "$@", which can never happen in valid php).
	 */
	void SymbolsFromVariable(const pelet::VariableClass& variable, pelet::ExpressionClass* expression);
	
	
	void SymbolFromVariableProperty(const UnicodeString& objectName, const pelet::VariablePropertyClass& property, std::vector<t4p::VariableSymbolClass>& symbols);
	
	void SymbolFromExpression(pelet::ExpressionClass* expression, std::vector<t4p::VariableSymbolClass>& symbols);
	
	/**
	 * @return the name for a new template variable; the variable name will be unique for the current scope
	 */
	UnicodeString NewTempVariable();

	/**
	 * @return bool TRUE if the current function is the method / function that we want to 
	 *         capture variables from.
	 */
	bool InDesiredScope() const;
};

}

#endif
