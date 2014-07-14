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
#ifndef __symboltable__
#define __symboltable__

#include <pelet/ParserClass.h>
#include <pelet/ParserTypeClass.h>
#include <globals/TagClass.h>
#include <globals/String.h>
#include <unicode/unistr.h>
#include <map>
#include <vector>

namespace t4p {

// forward declaration
class TagFinderListClass;

/**
 * A small class that will tell the outside world why the symbol table failed
 * to match a symbol or failed to complete an expression.
 */
class SymbolTableMatchErrorClass {

public:

	enum ErrorTypes {

		/** success */
		NONE,

		/** 
		 * an item in the parsedExpression ChainList could not be resolved.  This means that
		 * a method or property did not have a PHPDoc hint
		 */
		TYPE_RESOLUTION_ERROR,

		/**
		 * A tag was found; but it was a protected/private member attempting to
		 * be accessed from outside class; it was removed from matches
		 */
		VISIBILITY_ERROR,
		
		/**
		 * A static tag was found; but it was a protected/private member attempting to
		 * be accessed from outside class; it was removed from matches
		 */
		STATIC_ERROR,

		/**
		 * "parent" keyword could not be resolved because expression is not in a class scope.
		 */
		PARENT_ERROR,

		/**
		 * All items in the parsedExpression ChainList were able to be resolved, but the
		 * final item was not found in the resolved class.
		 */
		UNKNOWN_RESOURCE,

		/**
		 * A static tag was not found.
		 * All items in the parsedExpression ChainList were able to be resolved, but the
		 * final item was not found in the resolved class.
		 */
		UNKNOWN_STATIC_RESOURCE,

		/**
		 * Expression attempted to call the object operator on a primitive (string, int, double, ...)
		 */
		PRIMITIVE_ERROR,

		/**
		 * Expression attempted to call the object operator on an array
		 */
		ARRAY_ERROR,
		
		/**
		 * The symbol table has not been generated yet (this is a usage error, as CreateSymbols() needs
		 * to be called before ExpressionCompletionMatches() or ResourceMatches()
		 */
		EMPTY_SYMBOL_TABLE,

		/**
		 * This is also a usage error; a file must be Register() 'ed with the TagCache before 
		 * variables can be resolved.
		 */
		UNREGISTERED_FILE
	};

	/**
	 * The identifier that could not be resolved.
	 */
	UnicodeString ErrorLexeme;

	/**
	 * The name of the class that was searched; may only be filled
	 * when the parsed expression is a Chain list (chain of object
	 * operations).
	 */
	UnicodeString ErrorClass;

	/**
	 * Reason why matching failed.
	 */
	ErrorTypes Type;

	SymbolTableMatchErrorClass();

	/**
	 * @return true if ErrorType is any of the error types
	 */
	bool HasError() const;

	/**
	 * Set to ErrorTypes NONE and clears the error strings.
	 */
	void Clear();
	
	/**
	 * @param parsedVariable the variable that was attempted
	 * @param className the name of the class that was searched
	 */
	void ToVisibility(const pelet::VariableClass& parsedVariable, const UnicodeString& className);

	/**
	 * @param className the class name that was attempted
	 * @param memberName the name that was attempted 
	 */
	void ToTypeResolution(const UnicodeString& className, const UnicodeString& methodName);

	/**
	 * @param className the class name that was attempted
	 * @param memberName the name that was attempted 
	 */
	void ToArrayError(const UnicodeString& className, const UnicodeString& methodName);

	/**
	 * @param className the class name that was attempted
	 * @param memberName the name that was attempted 
	 */
	void ToPrimitiveError(const UnicodeString& className, const UnicodeString& methodName);

	/**
	 * @param parsedVariable the variable that was attempted
	 * @param className the name of the class that was searched
	 */
	void ToUnknownResource(const pelet::VariableClass& parsedVariable, const UnicodeString& className);

};

/**
 * This class represents one variable found in the source code, along with
 * the function call used to create the variable. A symbol is created ONLY
 * when a variable is guaranteed to have been created [according to PHP rules].
 * This includes:
 * 
 * 1. Assignments     $name = $person->name
 * 2. For-each loop   for ($list as $k => $v)
 * 3. lists asssignments  list ($a, $b, $c) = $listOfItems;
 * 4. catch blocks    catch (Exception $e)
 * 
 * For example, the source code
 * 
 * $person = new Person('John');
 * 
 * will create one symbol with the ChainList = [ 'Person' ]
 * See pelet::ExpressionClass for more info on ChainList.
 * 
 * In the case that a variable changes type from an array to another type in 
 * the same scope,  then only the first type will be used. For example, the 
 * source code
 * 
 *   $person = new Person('John');
 *   $person = $person->name;
 * 
 * Will create one symbol "person" with the type string.
 */
class SymbolClass {
	
public:

	/**
	 * The variable type
	 * UNKNOWN: could not determine / or a variable variable $us{$userName}
	 * SCALAR: definitely a string, or number
	 * ARRAY: definitely a PHP array
	 * OBJECT: definitely a PHP object
	 */
	enum Types {
		UNKNOWN,  
		SCALAR,
		ARRAY,
		OBJECT
	};
	
	/**
	 * The name of the PHP variable. Will always start with the
	 * siguil '$'
	 */
	UnicodeString Variable;
	
	/**
	 * The type that the PHPDoc attached to the variable (ie. \@var $dog DogClass)
	 * This is exactly what is in the DOC, it may or may not be valid.
	 */
	UnicodeString PhpDocType;
	
	/** 
	 * The chain of function / property calls used to create this variable
	 * 
	 * @see pelet::ExpressionClass
	 */
	std::vector<pelet::VariablePropertyClass> ChainList;
	
	/**
	 * The list of array keys that this array is know to have.
	 * These are only the constant keys; also these keys are found from both
	 * the initial array creation ANS subsequent assignments. For the following
	 * source code:
	 * 
	 *   $typesList = array( 'one' => 1, 'two' => 2);
	 *   $typesList['thre'] = 3;
	 * 
	 * There will be only 1 symbol, and ArrayKeys will contain 3 items: 'one', 'two'
	 * and 'three'
	 * 
	 * This array is only useful when a variable is an array.
	 */
	std::vector<UnicodeString> ArrayKeys;
	
	/**
	 * The symbol type; what kind of variable this variable is 
	 */
	Types Type;
	
	SymbolClass(const UnicodeString& variable, Types type = UNKNOWN);
	
};

/**
 * A Symbol table is the data structure that will hold all of the variables in the code along with their type information.
 * The symbol table is responsible for figuring out a variable's type as well as resolve any functions, methods,
 * and namespace names.
 * 
 * Symbols will be generated per each file according to PHP lanuguage semantics: 
 *  1. There are proper scopes (local variables)
 *  2. no distinction is made between numbers, bools, and strings.  They are all labeled as primitives because
 *     PHP casts transparently.
 * Symbol table will do its work one file at a time; since most of the time the symbol table responds to user
 * actions (ie. the user wants to jump to a specific variable declaration, or the user wants to complete
 * a variable method call).
 */
class SymbolTableClass : 
	public pelet::ClassObserverClass, 
	public pelet::ClassMemberObserverClass, 
	public pelet::FunctionObserverClass, 
	public pelet::VariableObserverClass,
	public pelet::AnyExpressionObserverClass {

public:
	SymbolTableClass();
	
	/**
	 * Builds symbols for the given source code. After symbols are created, lookups can be performed. 
	 * 
	 * @param UnicodeString code the code to analyze
	 */
	void CreateSymbols(const UnicodeString& code);
	

	/**
	 * Builds symbols for the given source code file. After symbols are created, lookups can be performed. 
	 * 
	 * @param wxString full path to the file that contains the code to analyze
	 */
	void CreateSymbolsFromFile(const wxString& fileName);

	/**
	 * This is the entry point into the code completion functionality; it will take a parsed expression (symbol)
	 * and will look up the each of the symbol's chain list items; resolve them against the given tag
	 * finders; After all of the items are resolved; the final matches will be added to the autoCompleteList.
	 * Example:
	 * Say symbol look likes the following:
	 * parsedExpression.Lexeme = "$this"
	 * parsedExpression.ChainList[0] = "->func1()"
	 * parsedExpression.ChainList[1] = "->prop2"
	 * 
	 * This method will look at $this and resolve it based on the scope that is located at position pos. Then
	 * it will look at the return value of $this->func1() (with the help of the given tag finders), say ClassA.  Once it 
	 * knows that, it will lookup ClassA::prop2 in all tag finders. It will then place all matches 
	 * into autoCompleteList. The end result is that autoCompleteList will have all resources 
	 * from ClassA that start with prop2 (ClassA::prop2, ClassA::prop2once, ClassA::prop2twice,...)
	 *
	 * This method will also resolve calls to "$this", "self", and "parent".
	 * Also, visibility rules will be taken into account; object properties that are accessed from the
	 * same class (ie "$this") will have access to protected / private methods, but properties accessed
	 * through from the outside will only have access to public members.
	 * None of the given resourc finders pointers will be owned by this class.
	 * 
	 * @param parsedVariable the variable to resolve. This is usually the result of the pelet::ParserClass::ParseExpression
	 * @param variableScope the scope where parsed expression is located.  The scope let's us know which variables are
	 *        available. See ScopeFinderClass for more info.
	 * @param sourceDirs the list of enabled source directories, only tags whose source_id matches source directories will be returned
	 * @param tagFinderList all of the tag finders to look in
	 * @param autoCompleteVariableList the results of the matches; these are the names of the variables that
	 *        are "near matches" to the parsed expression. This will be filled only when parsedExpression is a variable. 
	 * @param autoCompleteResourceList the results of the matches; these are the names of the items that
	 *        are "near matches" to the parsed expression. This will be filled only when parsedExpression is a variable "chain" or
	 *        a function / static class call. 
	 * @param doDuckTyping if an expression chain could not be fully resolved; then we could still
	 *        perform a search for the expression member in ALL classes. The lookups will not be
	 *        slower because TagFinderClass still handles them
	 * @param error any errors / explanations will be populated here. error must be set to no error (initial state of object; or use Clear() )
	 */
	void ExpressionCompletionMatches(pelet::VariableClass parsedVariable, 
		const pelet::ScopeClass& variableScope, 
		const std::vector<wxFileName>& sourceDirs,
		t4p::TagFinderListClass& tagFinderList,
		std::vector<UnicodeString>& autoCompleteVariableList,
		std::vector<TagClass>& autoCompleteResourceList,
		bool doDuckTyping,
		SymbolTableMatchErrorClass& error) const;

	/**
	 * This method will resolve the given parsed expression and will figure out the type of a tag. It will resolve
	 * each item in the parsed expression's chain list just like ExpressionCompletionMatches(), but this method will return
	 * tag objects.
	 *
	 * For example, let parsed expression be
	 * parsedExpression.Lexeme = "$this"
	 * parsedExpression.ChainList[0] = "->func1()"
	 * parsedExpression.ChainList[1] = "->prop2"
	 * 
	 * This method will return The tag that represents the "prop2" property of ClassA, wher ClassA is the return type of func1() method.
	 * In this case, the tag object for "ClassA::prop2" will be matched.
	 * None of the given resourc finders pointers will be owned by this class.
	 *
	 * @param parsedVariable the varaible to resolve. This is usually the result of the pelet::ParserClass::ParseExpression
	 * @param variableScope the scope where parsed expression is located.  The scope let's us know which variables are
	 *        available. See ScopeFinderClass for more info.
	 * @param sourceDirs the list of enabled source directories, only tags whose source_id matches source directories will be returned
	 * @param tagFinderList the tag finders to look in
	 * @param resourceMatches the tag matches; these are the names of the items that
	 *        are "near matches" to the parsed expression.
	 * @param doDuckTyping if an expression chain could not be fully resolved; then we could still
	 *        perform a search for the expression member in ALL classes. The lookups will not be
	 *        slower because TagFinderClass still handles them
	 * @param doFullyQualifiedMatchOnly if TRUE the only resources that match fully qualified resources will be
	 *        returned
	 * @param error any errors / explanations will be populated here. error must be set to no error (initial state of object; or use Clear())
	 */
	void ResourceMatches(pelet::VariableClass parsedVariable, 
		const pelet::ScopeClass& variableScope, 
		const std::vector<wxFileName>& sourceDirs,
		t4p::TagFinderListClass& tagFinderList,
		std::vector<TagClass>& resourceMatches,
		bool doDuckTyping, bool doFullyQualifiedMatchOnly,
		SymbolTableMatchErrorClass& error) const;
	
	/**
	 * outout to stdout
	 */
	void Print() const;
			
	void DefineDeclarationFound(const UnicodeString& namespaceName, const UnicodeString& variableName, const UnicodeString& variableValue, 
			const UnicodeString& comment, const int lineNumber);

	void MethodFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName, 
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment,
		pelet::TokenClass::TokenIds visibility, bool isStatic, const int lineNumber);
	
	void FunctionFound(const UnicodeString& namespaceName, const UnicodeString& functionName, 
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment,
		const int lineNumber);
		
	void VariableFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName,
		const pelet::VariableClass& variable, pelet::ExpressionClass* expression, const UnicodeString& comment);
	
	void OnAnyExpression(pelet::ExpressionClass* expr);
	
	/**
	 * Set the version that the PHP parser should use.
	 */
	void SetVersion(pelet::Versions version);
	
private:

	/**
	 * Get the vector of variables for the given scope. If scope does not exist it will
	 * be created.
	 * 
	 * @return std::vector<t4p::SymbolClass>&
	 */
	std::vector<t4p::SymbolClass>& GetScope(const UnicodeString& className, const UnicodeString& functionName);

	/**
	 * 	Add the super global PHP predefined variables into the given scope.  For example  $_GET, $_POST, ....
	 * 
	 *  @param vector<t4p::SymbolClass>& scope the scope list
	 */
	void CreatePredefinedVariables(std::vector<t4p::SymbolClass>& scope);
	
	/**
	 * Modifies the variable; resolving namespaces alias to their fully qualified equivalents
	 * 
	 * @param the variable to resolve
	 * @param scope the scope that containts the aliases to resolve against
	 */
	void ResolveNamespaceAlias(pelet::VariableClass& parsedVariable, const pelet::ScopeClass& scope) const;
	
	/**
	 * Modifies the tag; unresolving namespaces alias to their aliased equivalents. We need to 
	 * do this because the TagFinder class only deals with fully qualified namespaces, it knows nothing
	 * about the aliases
	 * 
	 * @param the original variable to resolve
	 * @param scope the scope that containts the aliases to resolve against
	 * @param tag a matched tag; will get modified an any namespace will be 'unresolved'
	 */
	void UnresolveNamespaceAlias(const pelet::VariableClass& originalVariable, const pelet::ScopeClass& scope, t4p::TagClass& tag) const;

	/**
	 * The parser.
	 * 
	 * @var pelet::ParserClass
	 */
	pelet::ParserClass Parser;
	
	/**
	 * Holds all variables for the currently parsed piece of code. Each vector will represent its own scope.
	 * The key will be the scope name.  The scope name is a combination of the class, method name. 
	 * The scope string is that which is returned by ScopeString() method.
	 * The value is the parsed Symbol.
	 * @var std::map<UnicodeString, vector<t4p::SymbolClass>>
	 */
	std::map<UnicodeString, std::vector<t4p::SymbolClass>, UnicodeStringComparatorClass> Variables;
};

/**
 * This class can be used to determine what function or namespace that a
 * particular line of code is in
 * 
 * note that for now, anonymous function scopes are only captured when
 * there is a parse error. this is because we don't want to use
 * ExpressionObserverClass here since that is kind of slow and 
 * we use scopefinder on each auto-complete request.
 */ 
class ScopeFinderClass : 
	public pelet::ClassObserverClass, 
	public pelet::ClassMemberObserverClass, 
	public pelet::FunctionObserverClass {
	
public:

	ScopeFinderClass();
	
	/**
	 * Returns the scope that is located at the given position i.e. what class/function is at position.
	 * 
	 * @param code the source code to process
	 * @param int position is index into code string for which to get the scope for
	 * @param scope instance to put the function, declared namespace, and aliases
	 *        that the position lies in.
	 */
	void GetScopeString(const UnicodeString& code, int pos, pelet::ScopeClass& scope);
	
	void ClassEnd(const UnicodeString& namespaceName, const UnicodeString& className, int pos);
		
	void NamespaceDeclarationFound(const UnicodeString& namespaceName, int startingPos);

	void NamespaceUseFound(const UnicodeString& namespaceName, const UnicodeString& alias, int startingPos);
		
	void MethodScope(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName,
		int startingPos, int endingPos);
			
	void FunctionScope(const UnicodeString& namespaceName, const UnicodeString& functionName, int startingPos, int endingPos);

	/**
	 * Set the version that the PHP parser should use.
	 */
	void SetVersion(pelet::Versions version);
		
private:

	/**
	 * if the given namespace is different than the current one, it indicates that the file has 
	 * switched to a new declared namespace.
	 * 
	 * @param namespaceName the current namespace
	 */
	void CheckLastNamespace(const UnicodeString& namespaceName);
	
	/**
	 * to keep track of the current namespace and method
	 */
	pelet::ScopeClass Scope;
	
	/**
	 * To keep know if a namespace has switched. If a namespace has switched we need to 
	 * remove all the aliases; a single PHP file can declare multiple namespaces
	 */
	UnicodeString LastNamespace;

	/**
	 * The parser.
	 * 
	 * @var pelet::ParserClass
	 */
	pelet::ParserClass Parser;
	
	/**
	 * this will tell us when to stop annotating
	 */
	int PosToCheck;

};

}


#endif // __symboltable__