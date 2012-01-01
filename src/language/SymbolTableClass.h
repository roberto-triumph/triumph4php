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

#include <language/ParserClass.h>
#include <language/ParserObserverClass.h>
#include <search/ResourceFinderClass.h>
#include <unicode/unistr.h>
#include <map>
#include <vector>

namespace mvceditor {

/**
 * Case-sensitive string comparator for use as STL Predicate
 */
class UnicodeStringComparatorClass {
public:
	bool operator()(const UnicodeString& str1, const UnicodeString& str2) const {
		return (str1.compare(str2) < (int8_t)0) ? true : false;
	}
};

/**
 * A Symbol table is the data structure that will hold all of the variables in the code along with their type information.
 * Symbols will be generated per each file according to PHP lanuguage semantics: 
 *  1. There are proper scopes (local variables)
 *  2. no distinction is made between numbers, bools, and strings.  They are all labeled as primitives because
 *     PHP casts transparently.
 * 
 */
class SymbolTableClass : public FunctionObserverClass, public ClassMemberObserverClass, public ClassObserverClass, 
	public VariableObserverClass {

public:
	SymbolTableClass();
	
	/**
	 * Builds symbols for the given source code. After symbols are created, lookups can be performed. 
	 * 
	 * @param UnicodeString code the code to analyze
	 */
	void CreateSymbols(const UnicodeString& code);

	/**
	 * This is the entry point into the code completion functionality; it will take a parsed expression (symbol)
	 * and will look up the each of the symbol's chain list items; resolve them against the given resource
	 * finders; After all of the items are resolved; the final matches will be added to the autoCompleteList.
	 * Example:
	 * Say symbol look likes the following:
	 * parsedExpression.Lexeme = "$this"
	 * parsedExpression.ChainList[0] = "->func1()"
	 * parsedExpression.ChainList[1] = "->prop2"
	 * 
	 * This method will look at $this and resolve it based on the scope that is located at position pos. Then
	 * it will look at the return value of $this->func1() (with the help of the given resource finders), say ClassA.  Once it 
	 * knows that, it will lookup ClassA::prop2 in all resource finders. It will then place all matches 
	 * into autoCompleteList. The end result is that autoCompleteList will have all resources 
	 * from ClassA that start with prop2 (ClassA::prop2, ClassA::prop2once, ClassA::prop2twice,...)
	 *
	 * This method will also resolve calls to "$this", "self", and "parent".
	 * Also, visibility rules will be taken into account; object properties that are accessed from the
	 * same class (ie "$this") will have access to protected / private methods, but properties accessed
	 * through from the outside will only have access to public members.
	 * 
	 * @param parsedExpression the expression to resolve. This is usually the result of the ParserClass::ParserExpression
	 * @param expressionPos the position where symbol is located.  The position is used to know the scope of 
	 *        the variable. See GetVariablesInScope(int) method for more info.
	 * @param resourceFinders the resource cache will be used to look up class methods and function return
	 *        values.
	 * @param autoCompleteList the results of the matches; these are the names of the items that
	 *        are "near matches" to the parsed expression.
	 */
	void ExpressionCompletionMatches(const SymbolClass& parsedExpression, int expressionPos, const std::vector<ResourceFinderClass*>& resourceFinders, 
		std::vector<UnicodeString>& autoCompleteList);

	/**
	 * This method will resolve the given parsed expression and will figure out the type of a resource. It will resolve
	 * each item in the parsed expression's chain list just like ExpressionCompletionMatches(), but this method will return
	 * resource objects.
	 *
	 * For example, let parsed expression be
	 * parsedExpression.Lexeme = "$this"
	 * parsedExpression.ChainList[0] = "->func1()"
	 * parsedExpression.ChainList[1] = "->prop2"
	 * 
	 * This method will return The resource that represents the "prop2" property of ClassA, wher ClassA is the return type of func1() method.
	 * In this case, the resource object for "ClassA::prop2" will be matched.
	 *
	 * @param parsedExpression the expression to resolve. This is usually the result of the ParserClass::ParserExpression
	 * @param expressionPos the position where symbol is located.  The position is used to know the scope of 
	 *        the variable. See GetVariablesInScope(int) method for more info.
	 * @param resourceFinders the resource cache will be used to look up class methods and function return
	 *        values.
	 * @param resourceMatches the resource matches; these are the names of the items that
	 *        are "near matches" to the parsed expression.
	 */
	void ResourceMatches(const SymbolClass& parsedExpression, int expressionPos, const std::vector<ResourceFinderClass*>& resourceFinders, 
		std::vector<ResourceClass>& resourceMatches);
	
	/**
	 * Returns the variables that are inside the scope at the given position i.e. what class/function is at position.
	 * 
	 * @param int position is index into code string given to the CreateSymbols() method.
	 * @return std::vector<UnicodeString> the variables in scope. The variables will contain the siguil ('$')
	 */
	std::vector<UnicodeString> GetVariablesInScope(int pos) const;
	
	/**
	 * outout to stdout
	 */
	void Print() const;
		
	virtual void ClassFound(const UnicodeString& className, const UnicodeString& signature, 
		const UnicodeString& comment);

	virtual void DefineDeclarationFound(const UnicodeString& variableName, const UnicodeString& variableValue, 
			const UnicodeString& comment);
	
	virtual void MethodFound(const UnicodeString& className, const UnicodeString& methodName, 
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment,
		TokenClass::TokenIds visibility, bool isStatic);

	void MethodEnd(const UnicodeString& className, const UnicodeString& methodName, int pos);
	
	virtual void PropertyFound(const UnicodeString& className, const UnicodeString& propertyName, 
		const UnicodeString& propertyType, const UnicodeString& comment, 
		TokenClass::TokenIds visibility, bool isConst, bool isStatic);
	
	virtual void FunctionFound(const UnicodeString& functionName, 
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment);

	void FunctionEnd(const UnicodeString& functionName, int pos);
		
	virtual void VariableFound(const UnicodeString& className, const UnicodeString& methodName, 
		const SymbolClass& symbol, const UnicodeString& comment);
private:

	/**
	 * Get the vector of variables for the given scope. If scope does not exist it will
	 * be created.
	 * 
	 * @return std::vector<SymbolClass>&
	 */
	std::vector<SymbolClass>& GetScope(const UnicodeString& className, const UnicodeString& functionName);

	/**
	 * Get the vector of variables for the scope that is at the given position. If scope
	 * does not exist then an empty vector will be returned.
	 * 
	 * @return std::vector<SymbolClass>
	 */
	std::vector<SymbolClass> GetScope(int pos) const;

	/**
	 * Stores the given scope and relates it to the given position. 
	 */
	void PushStartPos(const UnicodeString& className, const UnicodeString& functionName, int startPos);
	
	/**
	 * 	Add the super global PHP predefined variables into the given scope.  For example  $_GET, $_POST, ....
	 * 
	 *  @param vector<SymbolClass>& scope the scope list
	 */
	void CreatePredefinedVariables(std::vector<SymbolClass>& scope);

	/**
	 * @return the "scope string" used throughout this class, in the Variables map and the ScopePositions map
	 */
	UnicodeString ScopeString(const UnicodeString& className, const UnicodeString& functionName) const;

	/**
	 * The parser.
	 * 
	 * @var ParserClass
	 */
	ParserClass Parser;
	
	/**
	 * Holds all variables for the currently parsed piece of code. Each vector will represent its own scope.
	 * The key will be the scope name.  The scope name is a combination of the class, method name. 
	 * The scope string is that which is returned by ScopeString() method.
	 * The value is the parsed Symbol.
	 * @var std::map<UnicodeString, vector<SymbolClass>>
	 */
	std::map<UnicodeString, std::vector<SymbolClass>, UnicodeStringComparatorClass> Variables;

	/**
	 * This will store the character positions where the scope started and ended.
	 * The scope string is that which is returned by ScopeString() method.
	 * The global scope will always start at 0
	 * For purposes of this class; it is enough to know that the ranges will never overlap.
	 * For exact meanings of where the the class, method, and function start positions, see the
	 * ParserClass::GetCharacterPosition
	 * @see ParserClass::GetCharacterPosition
	 */
	std::map<UnicodeString, std::pair<int, int>, UnicodeStringComparatorClass> ScopePositions;
};

}

#endif // __symboltable__