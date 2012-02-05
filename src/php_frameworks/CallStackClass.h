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

#include <language/ParserClass.h>
#include <widgets/ResourceCacheClass.h>
#include <unicode/unistr.h>
#include <wx/filename.h>
#include <vector>
#include <queue>

namespace mvceditor
{

/**
 * This class will keep track of all function / method calls of a piece of source;
 * this is helpful in building a call stack where a user can easily see what
 * functions are getting called.
 *
 */
class CallStackClass : public ClassMemberObserverClass, public FunctionObserverClass, public ExpressionObserverClass,
	public ClassObserverClass, public VariableObserverClass {

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
	     * while recursing the call stack, we encountered a resource that could not be resolved (an object
	     * without a type
	     */
	    RESOLUTION_ERROR,

	    /**
	     * the call stack has gone past an reasonable length. Since the code checks for recursive methods,
	     * this error code may mean that the function observer cannot handle a certain piece of code.
	     */
	    STACK_LIMIT,
		
		/**
		 * the given resource cache is empty; the call stack class wont be able to quickly know where a 
		 * specific class is located.
		 */
		EMPTY_CACHE
	};

	/**
	 * All of the resolved function calls that took place; this vector is overwritten each time
	 * Build() method is called.
	 */
	std::vector<ResourceClass> List;

	/**
	 * If given code has a parser error (PARSE_ERR0R), the error will be stored here
	 */
	LintResultsClass LintResults;
	
	/**
	 * If the call stack could not be completed because a variable method could not be resolved (RESOLUTION_ERROR)
	 * then the error will be stored here. Note that this is NOT a fatal error; CallStack.List is populated, and the 
	 * list is correct; it just means that the call stack may be incomplete.
	 */
	SymbolTableMatchErrorClass MatchError;

	/**
	 * @param ResourceCacheClass& need the resource cache so that functions can be resolved
	 * into their corresponding file locations
	 */
	CallStackClass(ResourceCacheClass& resourceCache);

	/**
	 * Will open up the file, parse it, and collect all function calls. This is recursive (will open up
	 * each function call, parse it, and collect all function calls that they first funtion calls).
	 *
	 * @param fileName the file to parse
	 * @param className the class to start collecting
	 * @param methodName the method to start collecting
	 * @param error any error is encountered, the cause of the error will be set here
	 * @return TRUE on success, if FALSE then error will be filled.
	 */
	bool Build(const wxFileName& fileName, const UnicodeString& className, const UnicodeString& methodName, Errors& error);

	void MethodFound(const UnicodeString& className, const UnicodeString& methodName, 
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment, 
		TokenClass::TokenIds visibility, bool isStatic);

	void MethodEnd(const UnicodeString& className, const UnicodeString& methodName, int pos);
	
	void PropertyFound(const UnicodeString& className, const UnicodeString& propertyName, const UnicodeString& propertyType,
		const UnicodeString& comment, TokenClass::TokenIds visibility, bool isConst, bool isStatic);
		
	void FunctionFound(const UnicodeString& functionName, const UnicodeString& signature, const UnicodeString& returnType, 
		const UnicodeString& comment);
		
	void FunctionEnd(const UnicodeString& functionName, int pos);
	
	void ExpressionFound(const ExpressionClass& expression);
	
	void ClassFound(const UnicodeString& className, const UnicodeString& signature, const UnicodeString& comment);
	
	 void DefineDeclarationFound(const UnicodeString& variableName, const UnicodeString& variableValue, 
		const UnicodeString& comment);
	
	void VariableFound(const UnicodeString& className, const UnicodeString& methodName, const SymbolClass& symbol, const UnicodeString& comment);

private:

	/**
	 * this is the class that will parse the source code
	 */
	ParserClass Parser;

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
	 * a small struct to keep file and resoruces together while
	 * jumping through the code
	 */
	struct ResourceWithFile {
		
		wxFileName FileName;
		
		UnicodeString Resource;
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
	ResourceCacheClass& ResourceCache;
	
	/**
	 * this flag will be set when the parser hits the scope (resource) we are looking for.  If we dont
	 * find the scope we are looking for, then either the input scope was wrong, or some other
	 * unknown error has occurred
	 */
	bool FoundScope;
	
	void Clear();
	
	bool Recurse(Errors& error);
	
};

}

#endif
