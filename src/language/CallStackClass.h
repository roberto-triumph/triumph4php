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
#ifndef __CALLSTACKCLASS_H__
#define __CALLSTACKCLASS_H__

#include <pelet/ParserClass.h>
#include <language/TagCacheClass.h>
#include <language/SymbolTableClass.h>
#include <unicode/unistr.h>
#include <wx/filename.h>
#include <vector>
#include <queue>

namespace mvceditor
{

/**
 * A small class to keep track of the function / method call along with the arguments
 * to the function / method call. This class groups together the function name, file 
 * location, class name (if a method) and the list of arguments.
 */
class CallClass {

	public:

	/**
	 * All of the different types of 
	 * NONE a sentinel value
	 * BEGIN_METHOD a method (cass + function) was called
	 * BEGIN_FUNCTION a function was called
	 * ARRAY an array variable was created using the array() or [] syntax
	 * SCALAR a scalar variable was created
	 * OBJECT an object variable was created
	 * PARAM this is a variable that is a function argument
	 * RETURN the function ended
	 */
	enum Types {
		NONE,
		BEGIN_METHOD,
		BEGIN_FUNCTION,
		ARRAY,
		SCALAR,
		OBJECT,
		PARAM,
		RETURN
	};

	/**
	 * the function / method being called
	 */
	TagClass Resource;
	
	/**
	 * If a variable, then this is the variable name
	 */
	mvceditor::SymbolClass Symbol;
	
private:

	pelet::ScopeClass Scope;	
	
public:

	/**
	 * If this is a parameter, then the expression is here
	 */
	pelet::ExpressionClass Expression;
	
	Types Type;
	
	CallClass();
	
	// various methods to set the appropriate properties
	void ToArray(const mvceditor::SymbolClass& symbol);
	void ToScalar(const mvceditor::SymbolClass& symbol);
	void ToObject(const mvceditor::SymbolClass& symbol);
	void ToParam(const pelet::ExpressionClass& expr);
	void ToBeginFunction(const mvceditor::TagClass& tag);
	void ToBeginMethod(const mvceditor::TagClass& tag);
	void ToReturn();
	
	std::string StepTypeString() const;

	std::string ExpressionString() const;

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
	public pelet::ClassObserverClass, 
	public pelet::VariableObserverClass {

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
	std::vector<CallClass> List;

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
	 * into their corresponding file locations
	 */
	CallStackClass(TagCacheClass& tagCache);

	/**
	 * Will open up the file, parse it, and collect all function calls. This is recursive (will open up
	 * each function call, parse it, and collect all function calls that they first funtion calls).
	 *
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
	 * @param fileName the full path to the file that will be written to
	 * @return TRUE if file was successfully written to
	 */
	bool Persist(wxFileName& fileName);
	
	void MethodFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName, 
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment, 
		pelet::TokenClass::TokenIds visibility, bool isStatic, const int lineNumber);

	void FunctionFound(const UnicodeString& namespaceName, const UnicodeString& functionName, const UnicodeString& signature, const UnicodeString& returnType, 
		const UnicodeString& comment, const int lineNumber);
		
	void VariableFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName, 
		const pelet::VariableClass& variable, const pelet::ExpressionClass& expression, const UnicodeString& comment);
		
	void ExpressionFound(const pelet::ExpressionClass& expression);
	
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
		
		mvceditor::TagClass Resource;
		
		std::vector<pelet::ExpressionClass> CallArguments;
		
		std::vector<mvceditor::SymbolClass> ScopeVariables;
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
	 * Holds all variables for the current scope
	 * The value of each item in the vector is the parsed Symbol.
	 * @var std::vector<mvceditor::SymbolClass>>
	 */
	std::vector<mvceditor::SymbolClass> ScopeVariables;
	
	/**
	 * Holds all of the function calls that were performed in the current scope
	 */
	std::vector<pelet::ExpressionClass> ScopeFunctionCalls;
	
	std::map<UnicodeString, bool, UnicodeStringComparatorClass> ParsedMethods;
	
	/**
	 * this flag will be set when the parser hits the scope (tag) we are looking for.  If we dont
	 * find the scope we are looking for, then either the input scope was wrong, or some other
	 * unknown error has occurred
	 */
	bool FoundScope;
	
	void Clear();
	
	bool Recurse(pelet::Versions version, Errors& error);
	
	void CreateCalls();
	
};

}

#endif
