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
#include <language/SymbolTableClass.h>
#include <windows/StringHelperClass.h>
#include <pelet/TokenClass.h>
#include <algorithm>

/*
 * Checks that a resource matches visiblity rules.  Visibility rules include
 * 1) public/private/protected
 * 2) static/instance
 * 3) namespace aliases
 * 
 * @param resource the resource to check
 * @param originalParsedExpression the original query being asked for
 * @param scope the scope of the query being parsed
 * @param isStaticCall if TRUE, then resource is visible if the resource is also static
 * @param isThisCall if TRUE, then resource is visible if the resource is private, protected, or public
 * @param isParentCall if TRUE, then resource is visible if the resource is protected, or public
 * @return bool true if resource is visible
 */
static bool IsResourceVisible(const mvceditor::ResourceClass& resource, const pelet::ExpressionClass& originalParsedExpression,
		const pelet::ScopeClass& scope,
		bool isStaticCall, bool isThisCall, bool isParentCall) {
	bool passesStaticCheck = true;
	if (isStaticCall) {
		passesStaticCheck = mvceditor::ResourceClass::CLASS_CONSTANT == resource.Type || resource.IsStatic;
	}
	else {
		passesStaticCheck = mvceditor::ResourceClass::CLASS_CONSTANT != resource.Type && !resource.IsStatic;
	}

	// $this => can access this resource's private, parent's protected/public, other public
	// parent => can access parent's protected/public
	// neither => can only access public
	bool passesVisibilityCheck = !resource.IsPrivate && !resource.IsProtected;
	if (!passesVisibilityCheck && isParentCall) {

		// this check assumes that the resource finder has traversed the inheritance chain
		// properly. then, by a process of elimination, if the resource class is not
		// the symbol then we only show protected/public resources
		passesVisibilityCheck = resource.IsProtected;
	}
	else if (!passesVisibilityCheck) {

		//not checking isThisCalled
		passesVisibilityCheck = isThisCall;
	}
	
	// if the scope has a declared namespace, then global classes are not visible by default
	// since ResourceFinder does not work on a file level it had no knowledge of namespace aliases
	// we must perform this logic here
	bool passesNamespaceCheck = true;			
	UnicodeString name = originalParsedExpression.FirstValue();
	if (!name.startsWith(UNICODE_STRING_SIMPLE("$")) && !name.startsWith(UNICODE_STRING_SIMPLE("\\")) && mvceditor::ResourceClass::CLASS == resource.Type) {
		
		// if the resource if a global class and the current namespace is NOT the global namespace, 
		// then the class cannot be accessed
		// this assumes that resource finder was successful
		if (!scope.IsGlobalNamespace()) {
			passesNamespaceCheck = false;
			
			UnicodeString resQualified(resource.NamespaceName);
			if (!resQualified.endsWith(UNICODE_STRING_SIMPLE("\\"))) {
				resQualified.append(UNICODE_STRING_SIMPLE("\\"));
			}
			resQualified.append(resource.Identifier);
				
			// but if the resource is aliased then the class can be accessed
			std::map<UnicodeString, UnicodeString, pelet::UnicodeStringComparatorClass> aliases = scope.GetNamespaceAliases();
			std::map<UnicodeString, UnicodeString, pelet::UnicodeStringComparatorClass>::const_iterator it;
			for (it = aliases.begin(); it != aliases.end(); ++it) {
				
				// map key is the alias
				// check to see if the expression begins with the alias
				// need to watch out for the namespace operator
				// the expression may or may not have it
				UnicodeString alias(it->second);				
				if (alias.caseCompare(resQualified, 0) == 0) {
					passesNamespaceCheck = true;
					break;
				}
			}
			if (!passesNamespaceCheck) {
				
				// check to see if resource is from the declared namespace
				// when comparing namespaces make sure both end with slash so that we compare
				// full namespace names
				UnicodeString scopeNs = scope.NamespaceName;
				if (!scopeNs.endsWith(UNICODE_STRING_SIMPLE("\\"))) {
					scopeNs.append(UNICODE_STRING_SIMPLE("\\"));
				}
				passesNamespaceCheck = resQualified.indexOf(scopeNs) == 0;
			}
		}
	}
	return passesStaticCheck && passesVisibilityCheck && passesNamespaceCheck;
}

/**
 * @return bool TRUE if the given parsed expression uses static access ("::")
 */
static bool IsStaticExpression(const pelet::ExpressionClass& parsedExpression) {

	// "parent" is not static; "parent" could be used to call
	// methods that are overidden
	if (parsedExpression.FirstValue().caseCompare(UNICODE_STRING_SIMPLE("parent"), 0) == 0) {
		return false;
	}
	return
		parsedExpression.FirstValue().caseCompare(UNICODE_STRING_SIMPLE("self"), 0) == 0
		|| (parsedExpression.ChainList.size() > 1 && parsedExpression.ChainList[1].IsStatic);
}

/**
 * @return vector of all of the classes that are parent classes of the given
 *         class. this method will search across all resource finders
 */
static std::vector<UnicodeString> ClassParents(UnicodeString className, UnicodeString methodName, 
												 const std::vector<mvceditor::ResourceFinderClass*>& allResourceFinders) {
	std::vector<UnicodeString> parents;
	bool found = false;
	UnicodeString classToLookup = className;
	do {

		// each parent class may be located in any of the finders. in practice this code is not as slow
		// as it looks; class hierarchies are usually not very deep (1-4 parents)
		found = false;
		for (size_t i = 0; i < allResourceFinders.size(); ++i) {
			UnicodeString parentClass = allResourceFinders[i]->GetResourceParentClassName(classToLookup);
			if (!parentClass.isEmpty()) {
				found = true;
				parents.push_back(parentClass);
				classToLookup = parentClass;

				// a class can have at most 1 parent, no need to look at other finders
				break;
			}
		}
	} while (found);
	return parents;
}

/**
 * @return vector of all of the traits that are used by any of the given class or parent classes.
 *         This method will search across all resource finders
 */
static std::vector<UnicodeString> ClassUsedTraits(const UnicodeString& className, 
												  const std::vector<UnicodeString>& parentClassNames, 
												  const UnicodeString& methodName, 
												  const std::vector<mvceditor::ResourceFinderClass*>& allResourceFinders) {

	// trait support; a class can use multiple traits; hence the different logic 
	std::vector<UnicodeString> classesToLookup;
	classesToLookup.push_back(className);
	classesToLookup.insert(classesToLookup.end(), parentClassNames.begin(), parentClassNames.end());
	std::vector<UnicodeString> usedTraits;
	bool found = false;
	do {
		found = false;
		std::vector<UnicodeString> nextTraitsToLookup;
		for (std::vector<UnicodeString>::iterator it = classesToLookup.begin(); it != classesToLookup.end(); ++it) {
			for (size_t i = 0; i < allResourceFinders.size(); ++i) {
				std::vector<UnicodeString> traits = allResourceFinders[i]->GetResourceTraits(*it, methodName);
				if (!traits.empty()) {
					found = true;
					nextTraitsToLookup.insert(nextTraitsToLookup.end(), traits.begin(), traits.end());
					usedTraits.insert(usedTraits.end(), traits.begin(), traits.end());
				}
			}
		}

		// next, look for traits used by the traits themselves
		classesToLookup = nextTraitsToLookup;
	} while (found);

	return usedTraits;
}

/**
 * Figure out a resource's type by looking at all of the given finders.
 * @param resourceToLookup MUST BE fully qualified (class name  + method name,  or function name).  string can have the
 *        object operator "::" that separates the class and method name.
 * @param allResourceFinders all of the finders to look in
 * @return the resource's type; (for methods, it's the return type of the method) could be empty string if type could 
 *         not be determined 
 */
static UnicodeString ResolveResourceType(UnicodeString resourceToLookup, 
										 const std::vector<mvceditor::ResourceFinderClass*>& allResourceFinders) {
	UnicodeString type;
	mvceditor::ResourceSearchClass resourceSearch(resourceToLookup);
	resourceSearch.SetParentClasses(ClassParents(resourceSearch.GetClassName(), resourceSearch.GetMethodName(), allResourceFinders));
	resourceSearch.SetTraits(ClassUsedTraits(resourceSearch.GetClassName(), resourceSearch.GetParentClasses(), resourceSearch.GetMethodName(), allResourceFinders));

	// need to get the type from the resource finders
	for (size_t j = 0; j < allResourceFinders.size(); ++j) {
		mvceditor::ResourceFinderClass* finder = allResourceFinders[j];
		std::vector<mvceditor::ResourceClass> matches = finder->CollectFullyQualifiedResource(resourceSearch);
		if (!matches.empty()) {

			// since we are doing fully qualified matches, all matches are from the inheritance chain; ie. all methods
			// will have the same signature (return type)
			mvceditor::ResourceClass match = matches[0];
			if (mvceditor::ResourceClass::CLASS == match.Type) {
				type = match.ClassName;
			}
			else {
				type =  match.ReturnType;
			}
		}
		if (!type.isEmpty()) {

			// since we are doing exact lookups, only one should be found
			break;
		}
	}
	return type;
}

/*
 * figure out a [local] variable's type by looking at the other variables at the symbol table.
 * Since the symbol table just stores the parsed assignment expression tree; the symbols in the symbol table
 * have a chain list that needs to be looked up (with the symbol table).
 * Yes, this will cause a recursive call (symbol table may call this function); but it will never be very deep.

 * @see VariableObserverClass
 * @param expressionScope needed to use the symbol table
 * @param allResourceFinders needed to use the symbol table
 * @param openedResourceFinders needed to use the symbol table  
 * @param doDuckTyping
 * @param error any symbol table errors will be written here
 * @param variable the variable's name.  This is a single token, ie "$this", "$aglob" no object
 *        operations.
 * @param scopeSymbols the scope to look for the variable in 
 * @param symbolTable the symbol table is used to resolve the variable assigments.
 
 
 
 * @return the variable's type; could be empty string if type could not be determined 
 */
static UnicodeString ResolveVariableType(const pelet::ScopeClass& expressionScope, 
										 const std::vector<mvceditor::ResourceFinderClass*>& allResourceFinders,
										 const std::map<wxString, mvceditor::ResourceFinderClass*>& openedResourceFinders, 
										 bool doDuckTyping,
										 mvceditor::SymbolTableMatchErrorClass& error,
										 const UnicodeString& variable, 
										 const std::vector<mvceditor::SymbolClass>& scopeSymbols,
										 const mvceditor::SymbolTableClass& symbolTable) {
	UnicodeString type;
	if (scopeSymbols.empty()) {
		error.Type = mvceditor::SymbolTableMatchErrorClass::EMPTY_SYMBOL_TABLE;
		return type;
	}
	for (size_t i = 0; i < scopeSymbols.size(); ++i) {
		mvceditor::SymbolClass symbol = scopeSymbols[i];
		if (variable == symbol.Variable) {
			if (mvceditor::SymbolClass::SCALAR == symbol.Type) {
				type = "primitive";
			}
			else if (mvceditor::SymbolClass::ARRAY == symbol.Type) {
				type = "array";
			}
			else if (!symbol.PhpDocType.isEmpty()) {
				
				// user declares a type (in a PHPDoc comment  @var $dog Dog
				type = symbol.PhpDocType;
			}
			else if (symbol.ChainList.size() == 1) {		
				
				// variable was created with a 'new' or single function call
				// the  ResolveResourceType will get the function return type
				// if the variable was created from a function.
				UnicodeString resourceToLookup = symbol.ChainList[0].Name;
				type = ResolveResourceType(resourceToLookup, allResourceFinders);

			}
			else if (!symbol.ChainList.empty()) {
				
				
				// go through the chain list; the first item in the list may be a variable
				pelet::ScopeClass peletScope;
				pelet::ExpressionClass parsedExpression(peletScope);

				parsedExpression.ChainList = symbol.ChainList;
				std::vector<mvceditor::ResourceClass> resourceMatches;
				symbolTable.ResourceMatches(parsedExpression, expressionScope, 
					allResourceFinders, openedResourceFinders, resourceMatches, doDuckTyping, false, error);
				if (!resourceMatches.empty()) {
					if (mvceditor::ResourceClass::CLASS == resourceMatches[0].Type) {
						type = resourceMatches[0].ClassName;
					}
					else {
						type =  resourceMatches[0].ReturnType;
					}
				}
			}
			break;
		}
	}
	return type;
}

/**
 * Figure out the type of the initial lexeme in an expression chain. The initial 
 * element may be a variable, a function call, or a static class access. This logic
 * is not trivial; that's why it was separated.
 *
 * @param parsedExpression the expression to resolved.
 * @param scope the scope (to resolve variables)
 * @param allResourceFinders all of the finders to look in
 * @param openedResourceFindesr the resource finders from the opened files
 * @return the resource's type; (for methods, it's the return type of the method) could be empty string if type could not be determined 
 */
static UnicodeString ResolveInitialLexemeType(const pelet::ExpressionClass& parsedExpression, 
											  const pelet::ScopeClass& expressionScope, 
											  const std::vector<mvceditor::ResourceFinderClass*>& allResourceFinders,
											  const std::map<wxString, mvceditor::ResourceFinderClass*>& openedResourceFinders,
											  bool doDuckTyping,
											  mvceditor::SymbolTableMatchErrorClass& error,
											  const std::vector<mvceditor::SymbolClass>& scopeSymbols,
											  const mvceditor::SymbolTableClass& symbolTable) {
	UnicodeString start = parsedExpression.FirstValue();
	UnicodeString typeToLookup;
	if (start.startsWith(UNICODE_STRING_SIMPLE("$"))) {
		
		// a variable. look at the type from the symbol table
		typeToLookup = ResolveVariableType(expressionScope, allResourceFinders, openedResourceFinders, doDuckTyping, error, 
				start, scopeSymbols, symbolTable);
	}
	else if (start.caseCompare(UNICODE_STRING_SIMPLE("self"), 0) == 0){
		
		// self is the static version of $this, need to look at the pseudo variable $this
		// that is put into the symbol table during parsing
		// and get the type from there
		for (size_t i = 0; i < scopeSymbols.size(); ++i) {
			if (UNICODE_STRING_SIMPLE("$this") == scopeSymbols[i].Variable && !scopeSymbols[i].ChainList.empty()) {
				typeToLookup = scopeSymbols[i].ChainList[0].Name;
			}
		}
	}
	else if (start.caseCompare(UNICODE_STRING_SIMPLE("parent"), 0) == 0) {
		
		// look at the class signature of the current class that is in scope; that will tell us
		// what class is the parent
		// this code assumes that the resource finders have parsed the same exact code as the code that the
		// symbol table has parsed.
		// also, determine the type of "parent" by looking at the scope
		UnicodeString scopeClass = expressionScope.ClassName;
		UnicodeString scopeMethod = expressionScope.MethodName;
		for (size_t i = 0; i < allResourceFinders.size(); ++i) {	
			typeToLookup = allResourceFinders[i]->GetResourceParentClassName(scopeClass);
			if (!typeToLookup.isEmpty()) {
				break;
			}
		}
		if (typeToLookup.isEmpty()) {
			error.Type = mvceditor::SymbolTableMatchErrorClass::PARENT_ERROR;
			error.ErrorLexeme = scopeClass;
		}
	}
	else if (parsedExpression.ChainList.size() > 1) {

		// a function or a class. need to get the type from the resource finders
		// when ChainList has only one item, the item may be a partial function/class name
		// so we may not find it. 
		if (IsStaticExpression(parsedExpression)) {
			typeToLookup = start;
		}
		else {
			typeToLookup = ResolveResourceType(start, allResourceFinders);
		}
	}
	else {

		// when symbol's chain list has one item, it is from an expression that 
		// contains a partial function.  In this case, there is not need to catenate
		// ChainList items; doing so will result in a bad lookup 
		typeToLookup = start;
	}
	return typeToLookup;
}

/**
 * @return the "scope string" used throughout this class, in the Variables map and the ScopePositions map
 */
static UnicodeString ScopeString(const UnicodeString& className, const UnicodeString& functionName) {
	return className + UNICODE_STRING_SIMPLE("::") + functionName;
}

mvceditor::SymbolTableMatchErrorClass::SymbolTableMatchErrorClass()
	: ErrorLexeme()
	, ErrorClass()
	, Type(NONE) {
}

void mvceditor::SymbolTableMatchErrorClass::Clear() {
	Type = NONE;
	ErrorLexeme.remove();
	ErrorClass.remove();
}

bool mvceditor::SymbolTableMatchErrorClass::HasError() const {
	return Type != NONE;
}

void mvceditor::SymbolTableMatchErrorClass::ToVisibility(const pelet::ExpressionClass& parsedExpression, const UnicodeString& className) {
	if (IsStaticExpression(parsedExpression)) {
		Type = mvceditor::SymbolTableMatchErrorClass::STATIC_ERROR;
	}
	else {
		Type = mvceditor::SymbolTableMatchErrorClass::VISIBILITY_ERROR;
	}
	if (!parsedExpression.ChainList.empty()) {
		ErrorLexeme = parsedExpression.ChainList.back().Name;
	}
	ErrorClass = className;	
}

void mvceditor::SymbolTableMatchErrorClass::ToTypeResolution(const UnicodeString& className, const UnicodeString& methodName) {
	
	// an error resolving one of the types in the ChainList (not necessarily the last item)
	Type = mvceditor::SymbolTableMatchErrorClass::TYPE_RESOLUTION_ERROR;
	ErrorLexeme = methodName;
	ErrorClass = className;
}

void mvceditor::SymbolTableMatchErrorClass::ToArrayError(const UnicodeString& className, const UnicodeString& methodName) {
	Type = mvceditor::SymbolTableMatchErrorClass::ARRAY_ERROR;
	ErrorLexeme = methodName;
	ErrorClass = className;
}

void mvceditor::SymbolTableMatchErrorClass::ToPrimitiveError(const UnicodeString& className, const UnicodeString& methodName) {
	Type = mvceditor::SymbolTableMatchErrorClass::PRIMITIVE_ERROR;
	ErrorLexeme = methodName;
	ErrorClass = className;
}

void mvceditor::SymbolTableMatchErrorClass::ToUnknownResource(const pelet::ExpressionClass& parsedExpression, const UnicodeString& className) {
	if (!parsedExpression.ChainList.empty()) {
		if (IsStaticExpression(parsedExpression)) {
			Type = mvceditor::SymbolTableMatchErrorClass::UNKNOWN_STATIC_RESOURCE;
		}
		else {
			Type = mvceditor::SymbolTableMatchErrorClass::UNKNOWN_RESOURCE;
		}
		ErrorClass = className;
		ErrorLexeme = parsedExpression.ChainList.back().Name;
	}
}

mvceditor::SymbolTableClass::SymbolTableClass() 
	: Parser()
	, Variables() {
	Parser.SetClassObserver(this);
	Parser.SetClassMemberObserver(this);
	Parser.SetFunctionObserver(this);
	Parser.SetVariableObserver(this);
}

void mvceditor::SymbolTableClass::DefineDeclarationFound(const UnicodeString& namespaceName, const UnicodeString& variableName, 
	const UnicodeString& variableValue, const UnicodeString& comment, const int lineNumber) {
	mvceditor::SymbolClass symbol(variableName, mvceditor::SymbolClass::SCALAR);
	GetScope(UNICODE_STRING_SIMPLE(""), UNICODE_STRING_SIMPLE("")).push_back(symbol);
}

void mvceditor::SymbolTableClass::FunctionFound(const UnicodeString& namespaceName, const UnicodeString& functionName, const UnicodeString& signature, 
		const UnicodeString& returnType, const UnicodeString& comment, const int lineNumber) {
	
	// this call will automatically create the predefined variables
	GetScope(UNICODE_STRING_SIMPLE(""), functionName);
}

void mvceditor::SymbolTableClass::MethodFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName, 
	const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment,
	pelet::TokenClass::TokenIds visibility, bool isStatic, const int lineNumber) {
	std::vector<mvceditor::SymbolClass>& methodScope = GetScope(className, methodName);

	// create the $this variable
	mvceditor::SymbolClass variableSymbol(UNICODE_STRING_SIMPLE("$this"), mvceditor::SymbolClass::OBJECT);
	pelet::VariablePropertyClass prop;
	prop.Name = className;
	variableSymbol.ChainList.push_back(prop);
	methodScope.push_back(variableSymbol);
}

void mvceditor::SymbolTableClass::VariableFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName,
	const pelet::VariableClass& variable, const pelet::ExpressionClass& expression, const UnicodeString& comment) {

	// ATTN: a single variable may have many assignments
	// for now just take the first one
	std::vector<mvceditor::SymbolClass>& symbols = GetScope(className, methodName);
	bool found = false;
	for (size_t i = 0; i < symbols.size(); ++i) {
		if (!variable.ChainList.empty() && symbols[i].Variable == variable.ChainList[0].Name) {
			found = true;
			
			if (!variable.ArrayKey.isEmpty()) {
				
				// update any new Array keys used in the variable assignment
				// make sure not to have duplicates in case the same key is assigned
				// multiple times
				std::vector<UnicodeString>::iterator it = std::find(
					symbols[i].ArrayKeys.begin(), symbols[i].ArrayKeys.end(), variable.ArrayKey);
				if (it == symbols[i].ArrayKeys.end()) {
					symbols[i].ArrayKeys.push_back(variable.ArrayKey);
				}
			}
			break;
		}
	}
	if (!found && !variable.ChainList.empty()) {
		UnicodeString name = variable.ChainList[0].Name;
		mvceditor::SymbolClass::Types type;
		std::vector<UnicodeString> arrayKeys;
		if (variable.ArrayKey.isEmpty()) {
			switch (expression.ExpressionType) {
			case pelet::ExpressionClass::SCALAR:
			type = mvceditor::SymbolClass::SCALAR;
				break;
			case pelet::ExpressionClass::ARRAY:
				type = mvceditor::SymbolClass::ARRAY;
				arrayKeys = expression.ArrayKeys;
				break;
			case pelet::ExpressionClass::VARIABLE:
			case pelet::ExpressionClass::FUNCTION_CALL:
			case pelet::ExpressionClass::NEW_CALL:
				type = mvceditor::SymbolClass::OBJECT;
				break;
			case pelet::ExpressionClass::UNKNOWN:
				type = mvceditor::SymbolClass::UNKNOWN;
				break;
			}
		}
		else {
			
			// in  PHP an array may be created by assiging
			// an array key-value to a non-existant variable
			arrayKeys.push_back(variable.ArrayKey);
			type = mvceditor::SymbolClass::ARRAY;
		}
		mvceditor::SymbolClass newSymbol(name, type);
		newSymbol.ChainList = expression.ChainList;
		newSymbol.PhpDocType = variable.PhpDocType;
		newSymbol.ArrayKeys = arrayKeys;
		symbols.push_back(newSymbol);
	}
}

void mvceditor::SymbolTableClass::CreateSymbols(const UnicodeString& code) {
	Variables.clear();
	
	// for now ignore parse errors
	pelet::LintResultsClass results;
	Parser.ScanString(code, results);
}

void mvceditor::SymbolTableClass::CreateSymbolsFromFile(const wxString& fileName) {
	Variables.clear();
	
	// for now ignore parse errors
	pelet::LintResultsClass results;
	wxFFile file(fileName, wxT("rb"));
	Parser.ScanFile(file.fp(), mvceditor::StringHelperClass::wxToIcu(fileName), results);
}

void mvceditor::SymbolTableClass::ExpressionCompletionMatches(pelet::ExpressionClass parsedExpression, const pelet::ScopeClass& expressionScope,
															  const std::vector<mvceditor::ResourceFinderClass*>& allResourceFinders,
															  const std::map<wxString, mvceditor::ResourceFinderClass*>& openedResourceFinders,
															  std::vector<UnicodeString>& autoCompleteVariableList,
															  std::vector<mvceditor::ResourceClass>& autoCompleteResourceList,
															  bool doDuckTyping,
															  mvceditor::SymbolTableMatchErrorClass& error) const {
	if (parsedExpression.ChainList.size() == 1 && parsedExpression.FirstValue().startsWith(UNICODE_STRING_SIMPLE("$"))) {

		// if expression does not have more than one chained called AND it starts with a '$' then we want to match (local)
		// variables. This is just a SymbolTable search.
		std::vector<mvceditor::SymbolClass> scopeSymbols;
		std::map<UnicodeString, std::vector<mvceditor::SymbolClass>, mvceditor::UnicodeStringComparatorClass>::const_iterator it;
		it = Variables.find(ScopeString(expressionScope.ClassName, expressionScope.MethodName));
		if (it != Variables.end()) {
			scopeSymbols = it->second;
		}
		for (size_t i = 0; i < scopeSymbols.size(); ++i) {
			if (scopeSymbols[i].Variable.startsWith(parsedExpression.FirstValue())) {
				autoCompleteVariableList.push_back(scopeSymbols[i].Variable);
			}
		}
	}
	else {

		// some kind of function call / method chain call
		ResourceMatches(parsedExpression, expressionScope, allResourceFinders, openedResourceFinders,
			autoCompleteResourceList, doDuckTyping, false, error);
	}	
}

void mvceditor::SymbolTableClass::ResourceMatches(pelet::ExpressionClass parsedExpression, const pelet::ScopeClass& expressionScope, 
												  const std::vector<mvceditor::ResourceFinderClass*>& allResourceFinders,
												  const std::map<wxString, mvceditor::ResourceFinderClass*>& openedResourceFinders,
												  std::vector<mvceditor::ResourceClass>& resourceMatches,
												  bool doDuckTyping, bool doFullyQualifiedMatchOnly,
												  mvceditor::SymbolTableMatchErrorClass& error) const {
	std::vector<mvceditor::SymbolClass> scopeSymbols;
	std::map<UnicodeString, std::vector<mvceditor::SymbolClass>, UnicodeStringComparatorClass>::const_iterator it = 
		Variables.find(ScopeString(expressionScope.ClassName, expressionScope.MethodName));
	if (it != Variables.end()) {
		scopeSymbols = it->second;
	}
	
	// take care of the 'use' namespace importing
	pelet::ExpressionClass originalExpression = parsedExpression;
	ResolveNamespaceAlias(parsedExpression, expressionScope);
	
	UnicodeString typeToLookup = ResolveInitialLexemeType(parsedExpression, expressionScope, allResourceFinders, openedResourceFinders, 
		doDuckTyping, error, scopeSymbols, *this);
		
	// continue to the next item in the chain up until the second to last one
	// if we can't resolve a type then just exit
	if (typeToLookup.caseCompare(UNICODE_STRING_SIMPLE("primitive"), 0) == 0) {
		error.ToPrimitiveError(UNICODE_STRING_SIMPLE(""), parsedExpression.FirstValue());
	}
	else if (typeToLookup.caseCompare(UNICODE_STRING_SIMPLE("array"), 0) == 0) {
		error.ToArrayError(UNICODE_STRING_SIMPLE(""), parsedExpression.FirstValue());
	}
	else if (!parsedExpression.ChainList.empty()) {

		// need the empty check so that we don't overflow when doing 0 - 1 with size_t 
		for (size_t i = 1;  i < (parsedExpression.ChainList.size() - 1) && !typeToLookup.isEmpty() && !error.HasError(); ++i) {	
			UnicodeString nextResource = typeToLookup + UNICODE_STRING_SIMPLE("::") + parsedExpression.ChainList[i].Name;
			UnicodeString resolvedType = ResolveResourceType(nextResource, allResourceFinders);

			if (resolvedType.isEmpty()) {
				error.ToTypeResolution(typeToLookup, parsedExpression.ChainList[i].Name);
			}
			else if (typeToLookup.caseCompare(UNICODE_STRING_SIMPLE("primitive"), 0) == 0) {
				error.ToPrimitiveError(typeToLookup, parsedExpression.ChainList[i].Name);
			}
			else if (typeToLookup.caseCompare(UNICODE_STRING_SIMPLE("array"), 0) == 0) {
				error.ToArrayError(typeToLookup, parsedExpression.ChainList[i].Name);
			}
			typeToLookup = resolvedType;
		}
	}

	UnicodeString resourceToLookup;
	if (!typeToLookup.isEmpty() && parsedExpression.ChainList.size() > 1 && !error.HasError()) {
		resourceToLookup = typeToLookup + UNICODE_STRING_SIMPLE("::") + parsedExpression.ChainList.back().Name;
	}
	else if (!typeToLookup.isEmpty() && !error.HasError()) {

		// in this case; chain list is of size 1 (looking for a function / class name)
		resourceToLookup = typeToLookup;
	}
	else if (!error.HasError() && parsedExpression.ChainList.size() > 1 && typeToLookup.isEmpty() && doDuckTyping) {
		
		// here, even if the type of previous items in the chain could not be resolved
		// but were also known NOT to be errors
		// perform "duck typing" lookups; just look for methods in any class
		resourceToLookup = UNICODE_STRING_SIMPLE("::") + parsedExpression.ChainList.back().Name;
	}

	// now do the "final" lookup; here we will also perform access checks
	// and static access checks
	bool visibilityError = false;
	bool isStaticCall = IsStaticExpression(parsedExpression);
	bool isThisCall = parsedExpression.FirstValue().caseCompare(UNICODE_STRING_SIMPLE("$this"), 0) == 0;
	bool isParentCall = parsedExpression.FirstValue().caseCompare(UNICODE_STRING_SIMPLE("parent"), 0) == 0;

	if (!error.HasError()) {
		mvceditor::ResourceSearchClass resourceSearch(resourceToLookup);
		resourceSearch.SetParentClasses(ClassParents(resourceSearch.GetClassName(), resourceSearch.GetMethodName(), allResourceFinders));
		resourceSearch.SetTraits(ClassUsedTraits(resourceSearch.GetClassName(), resourceSearch.GetParentClasses(), resourceSearch.GetMethodName(), allResourceFinders));
		for (size_t j = 0; j < allResourceFinders.size(); ++j) {
			mvceditor::ResourceFinderClass* finder = allResourceFinders[j];

			// only do duck typing if needed. otherwise, make sure that we have a type match first.
			if ((doDuckTyping || !typeToLookup.isEmpty())) {
				std::vector<mvceditor::ResourceClass> matches;
				if (doFullyQualifiedMatchOnly) {
					matches = finder->CollectFullyQualifiedResource(resourceSearch);
				}
				else {
					matches = finder->CollectNearMatchResources(resourceSearch);
				}
				
				// now we loop through the possbile matches and remove stuff that does not 
				// make sense because of visibility rules or resources that are 
				// duplicated in two separate caches
				for (size_t k = 0; k < matches.size(); ++k) {
					mvceditor::ResourceClass resource = matches[k];
					bool isVisible = IsResourceVisible(resource, originalExpression, expressionScope, isStaticCall, isThisCall, isParentCall);
					if (!mvceditor::IsResourceDirty(openedResourceFinders, resource, finder) && isVisible) {
						UnresolveNamespaceAlias(originalExpression, expressionScope, resource);
						resourceMatches.push_back(resource);
					}
					else if (!isVisible) {
						visibilityError = true;
					}
				}
			}
		}
	}

	// don't overwrite a previous error (PRIMITIVE_ERROR, etc...)
	if (!error.HasError() && visibilityError && resourceMatches.empty()) {
		error.ToVisibility(parsedExpression, typeToLookup);
	}
	else if (!error.HasError() && resourceMatches.empty()) {
		error.ToUnknownResource(parsedExpression, typeToLookup);
	}
}

std::vector<mvceditor::SymbolClass>& mvceditor::SymbolTableClass::GetScope(const UnicodeString& className, 
		const UnicodeString& methodName) {
	UnicodeString scopeString = ScopeString(className , methodName);
	if (Variables[scopeString].empty()) {
		CreatePredefinedVariables(Variables[scopeString]);
	}
	return Variables[scopeString];
}

void mvceditor::SymbolTableClass::Print() const {
	UFILE *out = u_finit(stdout, NULL, NULL);
	for(std::map<UnicodeString, std::vector<mvceditor::SymbolClass>, UnicodeStringComparatorClass>::const_iterator it = Variables.begin(); it != Variables.end(); ++it) {
		std::vector<mvceditor::SymbolClass> scopedSymbols = it->second;
		UnicodeString s = it->first;
		u_fprintf(out, "Symbol Table For %S\n", s.getTerminatedBuffer());
		for (size_t j = 0; j < scopedSymbols.size(); ++j) {
			mvceditor::SymbolClass symbol = scopedSymbols[j];
			u_fprintf(out, "%d\t%S\t", (int)j, 
				symbol.Variable.getTerminatedBuffer()); 
			for (size_t k = 0; k < symbol.ChainList.size(); ++k) {
				u_fprintf(out, "%S", 
					symbol.ChainList[k].Name.getTerminatedBuffer()); 
				if (k < (symbol.ChainList.size() - 1)) {
					u_fprintf(out, "->"); 
				}
			}
			u_fprintf(out, "\n");
		}
	}
	u_fclose(out);
}

void mvceditor::SymbolTableClass::CreatePredefinedVariables(std::vector<mvceditor::SymbolClass>& scope) {
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
		mvceditor::SymbolClass variableSymbol(variables[i], mvceditor::SymbolClass::SCALAR);
		scope.push_back(variableSymbol);
	}
}

void mvceditor::SymbolTableClass::ResolveNamespaceAlias(pelet::ExpressionClass& parsedExpression, const pelet::ScopeClass& scope) const {
	
	// aliases are only in the beginning of the expression
	// for example, for the expression 
	// \Class::func()
	// name variable will be "\Class"
	// skip over variable expressions since they can't be aliased
	// leave fully qualified names alon
	UnicodeString name = parsedExpression.FirstValue();
	if (!name.startsWith(UNICODE_STRING_SIMPLE("$")) && !name.startsWith(UNICODE_STRING_SIMPLE("\\"))) {
		std::map<UnicodeString, UnicodeString, pelet::UnicodeStringComparatorClass> aliases = scope.GetNamespaceAliases();
		std::map<UnicodeString, UnicodeString, pelet::UnicodeStringComparatorClass>::const_iterator it;
		for (it = aliases.begin(); it != aliases.end(); ++it) {
			
			// map key is the alias
			// check to see if the expression begins with the alias
			// need to watch out for the namespace operator
			// the expression may or may not have it
			UnicodeString alias(it->first);
			if (!alias.endsWith(UNICODE_STRING_SIMPLE("\\"))) {
				alias += UNICODE_STRING_SIMPLE("\\");
			}
			if (name.startsWith(alias)) {
				UnicodeString afterAlias(name, it->first.length());
				name = it->second + afterAlias;
				parsedExpression.ChainList[0].Name = name;
				break;
			}
		}
	}
}

void mvceditor::SymbolTableClass::UnresolveNamespaceAlias(const pelet::ExpressionClass& originalExpression, const pelet::ScopeClass& scope, mvceditor::ResourceClass& resource) const {
	UnicodeString name = resource.Identifier;
	
	// leave variables and fully qualified names alone
	if (!originalExpression.FirstValue().startsWith(UNICODE_STRING_SIMPLE("$")) && !originalExpression.FirstValue().startsWith(UNICODE_STRING_SIMPLE("\\"))) {
		
		std::map<UnicodeString, UnicodeString, pelet::UnicodeStringComparatorClass> aliases = scope.GetNamespaceAliases();
		std::map<UnicodeString, UnicodeString, pelet::UnicodeStringComparatorClass>::const_iterator it;
		for (it = aliases.begin(); it != aliases.end(); ++it) {
			
			// map value is the fully qualified name
			// check to see if the resource begins with the fully qualified aliased name
			// need to watch out for the namespace operator
			// the expression may or may not have it
			UnicodeString qualified(it->second);
			if (!qualified.endsWith(UNICODE_STRING_SIMPLE("\\"))) {
				qualified += UNICODE_STRING_SIMPLE("\\");
			}
			if (name.startsWith(qualified)) {
				UnicodeString afterQualified(name, it->second.length());
				name = it->first + afterQualified;
				resource.Identifier = name;
				break;
			}
		}
	}
}

void mvceditor::SymbolTableClass::SetVersion(pelet::Versions version) {
	Parser.SetVersion(version);
}

bool mvceditor::IsResourceDirty(const std::map<wxString, mvceditor::ResourceFinderClass*>& finders, 
								const ResourceClass& resource, mvceditor::ResourceFinderClass* resourceFinder) {
	bool ret = false;
	wxString matchFullName = resource.GetFullPath();
	std::map<wxString, mvceditor::ResourceFinderClass*>::const_iterator it = finders.begin();
	while (it != finders.end()) {

		// a match from one of the opened resource finders can never be 'dirty' because the resource cache 
		// has been updated by the Update() method
		if (it->first.CompareTo(matchFullName) == 0 && it->second != resourceFinder) {
			ret = true;
			break;
		}
		++it;
	}
	return ret;
}


mvceditor::ScopeFinderClass::ScopeFinderClass() 
	: Scope()
	, LastNamespace()
	, Parser() {
	Parser.SetClassObserver(this);
	Parser.SetClassMemberObserver(this);
	Parser.SetFunctionObserver(this);
}

void mvceditor::ScopeFinderClass::SetVersion(pelet::Versions version) {
	Parser.SetVersion(version);
}

void mvceditor::ScopeFinderClass::ClassEnd(const UnicodeString& namespaceName, const UnicodeString& className, int pos) {
	if (pos >= PosToCheck) {
		return;
	}
	Scope.MethodName = UNICODE_STRING_SIMPLE("");
}


void mvceditor::ScopeFinderClass::NamespaceDeclarationFound(const UnicodeString& namespaceName, int startingPos) {
	if (startingPos >= PosToCheck) {
		return;
	}
	CheckLastNamespace(namespaceName);
	
	// add support for the namespace static operator
	if (namespaceName != UNICODE_STRING_SIMPLE("\\")) {
		Scope.AddNamespace(namespaceName, UNICODE_STRING_SIMPLE("namespace"));
	}
}

void mvceditor::ScopeFinderClass::NamespaceUseFound(const UnicodeString& namespaceName, const UnicodeString& alias, int startingPos) {
	if (startingPos >= PosToCheck) {
		return;
	}
	Scope.AddNamespace(namespaceName, alias);
}

void mvceditor::ScopeFinderClass::MethodScope(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName,
		int startingPos, int endingPos) {
	if (startingPos <= PosToCheck && PosToCheck <= endingPos) {
		CheckLastNamespace(namespaceName);
		Scope.ClassName = className;
		Scope.MethodName = methodName;
	}
}

void mvceditor::ScopeFinderClass::FunctionScope(const UnicodeString& namespaceName, const UnicodeString& functionName, 
		int startingPos, int endingPos) {
	if (startingPos <= PosToCheck && PosToCheck <= endingPos) {
		CheckLastNamespace(namespaceName);
		Scope.ClassName = UNICODE_STRING_SIMPLE("");
		Scope.MethodName = functionName;
	}
}

void mvceditor::ScopeFinderClass::GetScopeString(const UnicodeString& code, int pos, pelet::ScopeClass& scope) {
	Scope.Clear();
	LastNamespace.remove();
	PosToCheck = pos;
	
	pelet::LintResultsClass lintResults;
	if (!Parser.ScanString(code, lintResults)) {
		Scope = lintResults.Scope;
	}
	
	
	// ScanString starts the parsing and will call of the observer methods. At this point the scope has been figured out
	scope.Copy(Scope);
}

void mvceditor::ScopeFinderClass::CheckLastNamespace(const UnicodeString& namespaceName) {
	if (LastNamespace.caseCompare(namespaceName, 0) != 0) {
		
		// cannot Clear() the scope result because it would delete the aliases, and the aliases
		// are created outside of any method/function scope
		Scope.NamespaceName = namespaceName;
		LastNamespace = namespaceName;
	}
}

mvceditor::SymbolClass::SymbolClass(const UnicodeString& variable, mvceditor::SymbolClass::Types type)
	: Variable(variable)
	, PhpDocType()
	, ChainList()
	, ArrayKeys()
	, Type(type) {
		
}