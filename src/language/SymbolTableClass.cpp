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
#include <language/TagFinderList.h>
#include <globals/String.h>
#include <pelet/TokenClass.h>
#include <wx/ffile.h>
#include <algorithm>

static UnicodeString VariableName(const pelet::VariableClass& var) {
	UnicodeString varName;
	if (!var.ChainList.empty()) {
		varName = var.ChainList[0].Name;
	}
	return varName;
}

/*
 * Checks that a tag matches visiblity rules.  Visibility rules include
 * 1) public/private/protected
 * 2) static/instance
 * 3) namespace aliases
 * 
 * @param tag the tag to check
 * @param originalParsedVariable the original query being asked for
 * @param scope the scope of the query being parsed
 * @param isStaticCall if TRUE, then tag is visible if the tag is also static
 * @param isThisCall if TRUE, then tag is visible if the tag is private, protected, or public
 * @param isParentCall if TRUE, then tag is visible if the tag is protected, or public
 * @return bool true if tag is visible
 */
static bool IsResourceVisible(const t4p::TagClass& tag, const pelet::VariableClass& originalParsedVariable,
		const pelet::ScopeClass& scope,
		bool isStaticCall, bool isThisCall, bool isParentCall) {
	bool passesStaticCheck = true;
	if (isStaticCall) {

		// only static methods can be accessed with the '::' operator
		passesStaticCheck = t4p::TagClass::CLASS_CONSTANT == tag.Type || tag.IsStatic;
	}
	else {

		// static methods can be accessed via the -> operator 
		passesStaticCheck = t4p::TagClass::CLASS_CONSTANT != tag.Type;
	}

	// $this => can access this tag's private, parent's protected/public, other public
	// parent => can access parent's protected/public
	// neither => can only access public
	bool passesVisibilityCheck = !tag.IsPrivate && !tag.IsProtected;
	if (!passesVisibilityCheck && isParentCall) {

		// this check assumes that the tag finder has traversed the inheritance chain
		// properly. then, by a process of elimination, if the tag class is not
		// the symbol then we only show protected/public resources
		passesVisibilityCheck = tag.IsProtected;
	}
	else if (!passesVisibilityCheck) {

		//not checking isThisCalled
		passesVisibilityCheck = isThisCall;
	}
	
	// if the scope has a declared namespace, then global classes are not visible by default
	// since tagFinder does not work on a file level it had no knowledge of namespace aliases
	// we must perform this logic here
	bool passesNamespaceCheck = true;			
	UnicodeString name = VariableName(originalParsedVariable);
	if (!name.startsWith(UNICODE_STRING_SIMPLE("$")) && !name.startsWith(UNICODE_STRING_SIMPLE("\\")) 
		&& t4p::TagClass::CLASS == tag.Type) {
		
		// if the tag is a global class and the current namespace is NOT the global namespace, 
		// then the class cannot be accessed
		// this assumes that tag finder was successful
		// but if the tag is already fully qualified then
		if (!scope.IsGlobalNamespace()) {
			passesNamespaceCheck = false;
			
			// be careful, we could get a fully qualified tag in which case the 
			// identifier will already be fully qualified
			UnicodeString resQualified = tag.FullyQualifiedClassName();
				
			// but if the tag is aliased then the class can be accessed
			std::map<UnicodeString, UnicodeString, pelet::UnicodeStringComparatorClass> aliases = scope.GetNamespaceAliases();
			std::map<UnicodeString, UnicodeString, pelet::UnicodeStringComparatorClass>::const_iterator it;
			for (it = aliases.begin(); it != aliases.end(); ++it) {
				
				// map key is the alias
				// check to see if the expression begins with the alias
				// need to watch out for the namespace operator
				// the expression may or may not have it
				UnicodeString alias(it->second);				
				if (resQualified.indexOf(alias) == 0) {
					passesNamespaceCheck = true;
					break;
				}
			}
			if (!passesNamespaceCheck) {
				
				// check to see if tag is from the declared namespace
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
static bool IsStaticVariable(const pelet::VariableClass& parsedVariable) {

	// "parent" is not static; "parent" could be used to call
	// methods that are overidden
	if (VariableName(parsedVariable).caseCompare(UNICODE_STRING_SIMPLE("parent"), 0) == 0) {
		return false;
	}
	return
		VariableName(parsedVariable).caseCompare(UNICODE_STRING_SIMPLE("self"), 0) == 0
		|| (parsedVariable.ChainList.size() > 1 && parsedVariable.ChainList[1].IsStatic);
}

/*
 * figure out a [local] variable's type by looking at the other variables at the symbol table.
 * Since the symbol table just stores the parsed assignment expression tree; the symbols in the symbol table
 * have a chain list that needs to be looked up (with the symbol table).
 * Yes, this will cause a recursive call (symbol table may call this function); but it will never be very deep.

 * @see VariableObserverClass
 * @param variableScope needed to use the symbol table
 * @param sourceDirs only tags from matching source directories will be returned
 * @param allTagFinders needed to use the symbol table
 * @param doDuckTyping
 * @param error any symbol table errors will be written here
 * @param variable the variable's name.  This is a single token, ie "$this", "$aglob" no object
 *        operations.
 * @param scopeSymbols the scope to look for the variable in 
 * @param symbolTable the symbol table is used to resolve the variable assigments.
 
 
 
 * @return the variable's type; could be empty string if type could not be determined 
 */
static UnicodeString ResolveVariableType(const pelet::ScopeClass& variableScope, 
										 const std::vector<wxFileName>& sourceDirs,
										 t4p::TagFinderListClass& tagFinderList,
										 bool doDuckTyping,
										 t4p::SymbolTableMatchErrorClass& error,
										 const UnicodeString& variable, 
										 const std::vector<t4p::SymbolClass>& scopeSymbols,
										 const t4p::SymbolTableClass& symbolTable) {
	UnicodeString type;
	if (scopeSymbols.empty()) {
		error.Type = t4p::SymbolTableMatchErrorClass::EMPTY_SYMBOL_TABLE;
		return type;
	}
	for (size_t i = 0; i < scopeSymbols.size(); ++i) {
		t4p::SymbolClass symbol = scopeSymbols[i];
		if (variable == symbol.Variable) {
			if (t4p::SymbolClass::SCALAR == symbol.Type) {
				type = "primitive";
			}
			else if (t4p::SymbolClass::ARRAY == symbol.Type) {
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
				type = tagFinderList.ResolveResourceType(resourceToLookup, sourceDirs);
			}
			else if (!symbol.ChainList.empty()) {
				
				
				// go through the chain list; the first item in the list may be a variable
				pelet::ScopeClass peletScope;
				pelet::VariableClass parsedVariable(peletScope);

				parsedVariable.ChainList = symbol.ChainList;
				std::vector<t4p::TagClass> resourceMatches;
				symbolTable.ResourceMatches(parsedVariable, variableScope, sourceDirs,
					tagFinderList, resourceMatches, doDuckTyping, false, error);
				if (!resourceMatches.empty()) {
					if (t4p::TagClass::CLASS == resourceMatches[0].Type) {
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
 * @param parsedVariable the expression to resolved.
 * @param sourceDirs tags from matching source directories will be returned
 * @param scope the scope (to resolve variables)
 * @param allTagFinders all of the finders to look in
 * @param openedResourceFinder the tag finder for the opened files
 * @return the tag's type; (for methods, it's the return type of the method) could be empty string if type could not be determined 
 */
static UnicodeString ResolveInitialLexemeType(const pelet::VariableClass& parsedVariable, 
											  const pelet::ScopeClass& variableScope, 
											  const std::vector<wxFileName>& sourceDirs,
											  t4p::TagFinderListClass& tagFinderList,
											  bool doDuckTyping,
											  t4p::SymbolTableMatchErrorClass& error,
											  const std::vector<t4p::SymbolClass>& scopeSymbols,
											  const t4p::SymbolTableClass& symbolTable) {
	UnicodeString start = VariableName(parsedVariable);
	UnicodeString typeToLookup;
	if (start.startsWith(UNICODE_STRING_SIMPLE("$"))) {
		
		// a variable. look at the type from the symbol table
		typeToLookup = ResolveVariableType(variableScope, sourceDirs, tagFinderList, doDuckTyping, error, 
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
		// this code assumes that the tag finders have parsed the same exact code as the code that the
		// symbol table has parsed.
		// also, determine the type of "parent" by looking at the scope
		UnicodeString scopeClass = variableScope.ClassName;
		UnicodeString scopeMethod = variableScope.MethodName;

		typeToLookup = tagFinderList.ParentClassName(scopeClass, 0);
		if (typeToLookup.isEmpty()) {
			error.Type = t4p::SymbolTableMatchErrorClass::PARENT_ERROR;
			error.ErrorLexeme = scopeClass;
		}
	}
	else if (parsedVariable.ChainList.size() > 1) {

		// a function or a class. need to get the type from the tag finders
		// when ChainList has only one item, the item may be a partial function/class name
		// so we may not find it. 
		if (IsStaticVariable(parsedVariable)) {
			typeToLookup = start;
		}
		else {
			typeToLookup = tagFinderList.ResolveResourceType(start, sourceDirs);
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


/**
 * @return the "scope string" used throughout this class for anonymous functions (closures), in the 
 *         Variables map and the ScopePositions map
 *         this in only the "function name" portion of the scope
 */
static UnicodeString ScopeStringAnonymousFunction(const UnicodeString& methodName, int anonymousFunctionCount) {
	// the scope string looks like this:
	// class::method@@anonymousFunction1
	//
	wxString stdAnonymousFunctionPostfix = wxString::Format("@@anonymousFunction_%d", anonymousFunctionCount);
	UnicodeString anonymousFunctionPostfix = t4p::WxToIcu(stdAnonymousFunctionPostfix);
	
	UnicodeString functionName = methodName + anonymousFunctionPostfix;
	return functionName;
}

t4p::SymbolTableMatchErrorClass::SymbolTableMatchErrorClass()
	: ErrorLexeme()
	, ErrorClass()
	, Type(NONE) {
}

void t4p::SymbolTableMatchErrorClass::Clear() {
	Type = NONE;
	ErrorLexeme.remove();
	ErrorClass.remove();
}

bool t4p::SymbolTableMatchErrorClass::HasError() const {
	return Type != NONE;
}

void t4p::SymbolTableMatchErrorClass::ToVisibility(const pelet::VariableClass& parsedVariable, const UnicodeString& className) {
	if (IsStaticVariable(parsedVariable)) {
		Type = t4p::SymbolTableMatchErrorClass::STATIC_ERROR;
	}
	else {
		Type = t4p::SymbolTableMatchErrorClass::VISIBILITY_ERROR;
	}
	if (!parsedVariable.ChainList.empty()) {
		ErrorLexeme = parsedVariable.ChainList.back().Name;
	}
	ErrorClass = className;	
}

void t4p::SymbolTableMatchErrorClass::ToTypeResolution(const UnicodeString& className, const UnicodeString& methodName) {
	
	// an error resolving one of the types in the ChainList (not necessarily the last item)
	Type = t4p::SymbolTableMatchErrorClass::TYPE_RESOLUTION_ERROR;
	ErrorLexeme = methodName;
	ErrorClass = className;
}

void t4p::SymbolTableMatchErrorClass::ToArrayError(const UnicodeString& className, const UnicodeString& methodName) {
	Type = t4p::SymbolTableMatchErrorClass::ARRAY_ERROR;
	ErrorLexeme = methodName;
	ErrorClass = className;
}

void t4p::SymbolTableMatchErrorClass::ToPrimitiveError(const UnicodeString& className, const UnicodeString& methodName) {
	Type = t4p::SymbolTableMatchErrorClass::PRIMITIVE_ERROR;
	ErrorLexeme = methodName;
	ErrorClass = className;
}

void t4p::SymbolTableMatchErrorClass::ToUnknownResource(const pelet::VariableClass& parsedVariable, const UnicodeString& className) {
	if (!parsedVariable.ChainList.empty()) {
		if (IsStaticVariable(parsedVariable)) {
			Type = t4p::SymbolTableMatchErrorClass::UNKNOWN_STATIC_RESOURCE;
		}
		else {
			Type = t4p::SymbolTableMatchErrorClass::UNKNOWN_RESOURCE;
		}
		ErrorClass = className;
		ErrorLexeme = parsedVariable.ChainList.back().Name;
	}
}

t4p::SymbolTableClass::SymbolTableClass() 
	: AnyExpressionObserverClass()
	, Parser()
	, Lexer()
	, Variables() {
	Parser.SetClassObserver(this);
	Parser.SetClassMemberObserver(this);
	Parser.SetFunctionObserver(this);
	Parser.SetVariableObserver(this);
	Parser.SetExpressionObserver(this);
}

void t4p::SymbolTableClass::DefineDeclarationFound(const UnicodeString& namespaceName, const UnicodeString& variableName, 
	const UnicodeString& variableValue, const UnicodeString& comment, const int lineNumber) {
	t4p::SymbolClass symbol(variableName, t4p::SymbolClass::SCALAR);
	GetScope(UNICODE_STRING_SIMPLE(""), UNICODE_STRING_SIMPLE("")).push_back(symbol);
}

void t4p::SymbolTableClass::FunctionFound(const UnicodeString& namespaceName, const UnicodeString& functionName, const UnicodeString& signature, 
		const UnicodeString& returnType, const UnicodeString& comment, const int lineNumber) {
	
	// this call will automatically create the predefined variables
	GetScope(UNICODE_STRING_SIMPLE(""), functionName);
}

void t4p::SymbolTableClass::MethodFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName, 
	const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment,
	pelet::TokenClass::TokenIds visibility, bool isStatic, const int lineNumber) {
	
	std::vector<t4p::SymbolClass>& methodScope = GetScope(className, methodName);

	// create the $this variable
	t4p::SymbolClass variableSymbol(UNICODE_STRING_SIMPLE("$this"), t4p::SymbolClass::OBJECT);
	pelet::VariablePropertyClass prop;

	// qualify class with namespace if needed
	UnicodeString qualifiedClassName;
	if (!namespaceName.isEmpty() && namespaceName != UNICODE_STRING_SIMPLE("\\")) {
		qualifiedClassName = namespaceName + UNICODE_STRING_SIMPLE("\\") + className;
	}
	else {
		qualifiedClassName = className;
	}
	prop.Name = qualifiedClassName;
	variableSymbol.ChainList.push_back(prop);
	methodScope.push_back(variableSymbol);
}

void t4p::SymbolTableClass::VariableFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName,
	const pelet::VariableClass& variable, pelet::ExpressionClass* expression, const UnicodeString& comment) {

	UnicodeString variableArrayAccessKey;
	if (!variable.ChainList.empty() && variable.ChainList[0].IsArrayAccess && variable.ChainList[0].ArrayAccess) {
		variableArrayAccessKey = ((pelet::ScalarExpressionClass*)variable.ChainList[0].ArrayAccess)->Value;
	}

	// ATTN: a single variable may have many assignments
	// for now just take the first one
	std::vector<t4p::SymbolClass>& symbols = GetScope(className, methodName);
	bool found = false;
	for (size_t i = 0; i < symbols.size(); ++i) {
		if (!variable.ChainList.empty() && symbols[i].Variable == variable.ChainList[0].Name) {
			found = true;
			if (!variableArrayAccessKey.isEmpty()) {
				
				// update any new Array keys used in the variable assignment
				// make sure not to have duplicates in case the same key is assigned
				// multiple times
				std::vector<UnicodeString>::iterator it = std::find(
					symbols[i].ArrayKeys.begin(), symbols[i].ArrayKeys.end(), variableArrayAccessKey);
				if (it == symbols[i].ArrayKeys.end()) {
					symbols[i].ArrayKeys.push_back(variableArrayAccessKey);
				}
			}
			break;
		}
	}
	std::vector<pelet::VariablePropertyClass> chainList;
	if (!found && !variable.ChainList.empty()) {
		UnicodeString name = variable.ChainList[0].Name;
		t4p::SymbolClass::Types type;
		std::vector<pelet::ArrayPairExpressionClass*> pairs;
		std::vector<pelet::ArrayPairExpressionClass*>::const_iterator pair;
		std::vector<UnicodeString> arrayKeys;
		if (variableArrayAccessKey.isEmpty()) {
			switch (expression->ExpressionType) {
			case pelet::ExpressionClass::SCALAR:
			type = t4p::SymbolClass::SCALAR;
				break;
			case pelet::ExpressionClass::ARRAY:
				type = t4p::SymbolClass::ARRAY;
				pairs = ((pelet::ArrayExpressionClass*)expression)->ArrayPairs;
				for (pair = pairs.begin(); pair != pairs.end(); ++pair) {
					if ((*pair)->Key && pelet::ExpressionClass::SCALAR == (*pair)->Key->ExpressionType) {
						arrayKeys.push_back(((pelet::ScalarExpressionClass*)(*pair)->Key)->Value);
					}
				}
				break;
			case pelet::ExpressionClass::VARIABLE:
				type = t4p::SymbolClass::OBJECT;
				chainList = ((pelet::VariableClass*)expression)->ChainList;
				break;
			case pelet::ExpressionClass::NEW_CALL:
				type = t4p::SymbolClass::OBJECT;
				chainList.push_back(pelet::VariablePropertyClass());
				chainList.back().Name = 
					((pelet::NewInstanceExpressionClass*)expression)->ClassName;
				break;
			default:
				type = t4p::SymbolClass::UNKNOWN;
				break;
			}
		}
		else {
			
			// in  PHP an array may be created by assigning
			// an array key-value to a non-existant variable
			arrayKeys.push_back(variableArrayAccessKey);
			type = t4p::SymbolClass::ARRAY;
		}
		t4p::SymbolClass newSymbol(name, type);
		newSymbol.ChainList = chainList;
		newSymbol.PhpDocType = variable.PhpDocType;
		newSymbol.ArrayKeys = arrayKeys;
		symbols.push_back(newSymbol);
	}
}

void t4p::SymbolTableClass::OnAnyExpression(pelet::ExpressionClass* expr) {
	if (expr->ExpressionType != pelet::ExpressionClass::CLOSURE) {
		return;
	}
	pelet::ClosureExpressionClass* closure = (pelet::ClosureExpressionClass*)expr;
	
	// anonymous functions have their own scope, so we given them their own unique
	// name.
	// the scope string looks like this:
	// class::method@@anonymousFunction1
	//
	UnicodeString functionName = ScopeStringAnonymousFunction(
		closure->Scope.MethodName, closure->Scope.GetAnonymousFunctionCount()
	);

	// loop through the closure's parameters and add them to the closure scope
	// we need a "right hand side" for each variable, lets make an empty one
	// for now
	pelet::NewInstanceExpressionClass newCallExpr(closure->Scope);
	for (size_t i = 0; i < closure->Parameters.size(); ++i) {
		VariableFound(closure->Scope.NamespaceName, closure->Scope.ClassName, functionName,
			*closure->Parameters[i], &newCallExpr, closure->Parameters[i]->Comment);
	}
	
	// loop through the closure's lexical variables "use" variables and add them to the scope
	for (size_t i = 0; i < closure->LexicalVars.size(); ++i) {
		VariableFound(closure->Scope.NamespaceName, closure->Scope.ClassName, functionName,
			*closure->LexicalVars[i], &newCallExpr, closure->LexicalVars[i]->Comment);
	}
	
	// loop through the closure's inner statements to get variable assignments
	for (size_t i = 0; i < closure->Statements.Size(); ++i) {
		if (closure->Statements.TypeAt(i) == pelet::StatementClass::EXPRESSION) {
			pelet::ExpressionClass* closureInnerExpr = (pelet::ExpressionClass*)closure->Statements.At(i);
			if (pelet::ExpressionClass::ASSIGNMENT == closureInnerExpr->ExpressionType) {
				pelet::AssignmentExpressionClass* assignment = (pelet::AssignmentExpressionClass*)closure->Statements.At(i);
				VariableFound(expr->Scope.NamespaceName, expr->Scope.ClassName, functionName, 
					assignment->Destination, assignment->Expression, assignment->Destination.Comment);
			}
			else if (pelet::ExpressionClass::ASSIGNMENT_LIST == closureInnerExpr->ExpressionType) {
				pelet::AssignmentListExpressionClass* assignmentList = (pelet::AssignmentListExpressionClass*)closureInnerExpr;
				for (size_t i = 0; i < assignmentList->Destinations.size(); ++i) {
					VariableFound(expr->Scope.NamespaceName, expr->Scope.ClassName, functionName, 
						assignmentList->Destinations[i], assignmentList->Expression, assignmentList->Destinations[i].Comment);
				}
			}
		}
	}
}

void t4p::SymbolTableClass::CreateSymbols(const UnicodeString& code) {
	Variables.clear();
	
	// if the given code has a syntax error, use a naive algorithm as fallback
	// that way we show results to the user if at all possible
	pelet::LintResultsClass results;
	bool good = Parser.ScanString(code, results);
	if (!good && Lexer.OpenString(code)) {
		CreateSymbolsFromTokens();
	}
}

void t4p::SymbolTableClass::CreateSymbolsFromFile(const wxString& fileName) {
	Variables.clear();
	
	// for now ignore parse errors
	pelet::LintResultsClass results;
	if (wxFileName::FileExists(fileName)) {
		wxFFile file(fileName, wxT("rb"));
		bool good = Parser.ScanFile(file.fp(), t4p::WxToIcu(fileName), results);
		
		// if the given file has a syntax error, use a naive algorithm as fallback
		// that way we show results to the user if at all possible
		if (!good) {
			wxFFile lexFile(fileName, wxT("rb"));
			if (Lexer.OpenFile(lexFile.fp())) {
				CreateSymbolsFromTokens();
			}
		}
	}
}

void t4p::SymbolTableClass::CreateSymbolsFromTokens() {
	Variables.clear();
	UnicodeString currentClass;
	UnicodeString currentMethod;
	UnicodeString variable;

	// note that this is a very naive way of getting variables, it is 
	// as a way to help the user a bit while editing code while the 
	// code is in a bad state
	int token = Lexer.NextToken();
	bool addedLastToken = false;
	while (token != pelet::T_END) {
		addedLastToken = false;
		if (pelet::T_CLASS == token) {
			token = Lexer.NextToken();
			currentClass.remove();
			Lexer.GetLexeme(currentClass);
		}				
		else if (pelet::T_FUNCTION == token) {
			token = Lexer.NextToken();
			currentMethod.remove();
			Lexer.GetLexeme(currentMethod);
		}
		else if (pelet::T_VARIABLE == token) {
			variable.remove();
			if (Lexer.GetLexeme(variable)) {
				bool found = false;
				std::vector<t4p::SymbolClass>& scope = GetScope(currentClass, currentMethod);
				for (size_t i = 0; i < scope.size(); ++i) {
					if (scope[i].Variable == variable) {
						found = true;
						break;
					}
				}
				if (!found) {
					t4p::SymbolClass variableSymbol(variable, t4p::SymbolClass::SCALAR);
					scope.push_back(variableSymbol);
					addedLastToken = true;
				}
			}
		}
		token = Lexer.NextToken();
	}
	
	// final step: if the last token was a variable, then we want to remove it
	// because this indicates that the statement is not complete. if we were to leave
	// it, the user would see the last symbol (what they are typing in) as a 
	// choice and that does not look good.
	// scenario:
	// user creates a new php file
	// user types in 
	//  <?php 
	//     function work($place) {
	//        $pl
	//
	// then we user attempts to complete on "$pl" we should
	// remove "$pl" as a choice because they probably are going to type in "$place" 
	if (addedLastToken) {
		std::vector<t4p::SymbolClass>& scope = GetScope(currentClass, currentMethod);
		scope.pop_back();
	}
}

void t4p::SymbolTableClass::ExpressionCompletionMatches(pelet::VariableClass parsedVariable, 
															  const pelet::ScopeClass& variableScope,
															  const std::vector<wxFileName>& sourceDirs,
															  t4p::TagFinderListClass& tagFinderList,
															  std::vector<UnicodeString>& autoCompleteVariableList,
															  std::vector<t4p::TagClass>& autoCompleteResourceList,
															  bool doDuckTyping,
															  t4p::SymbolTableMatchErrorClass& error) const {
	if (parsedVariable.ChainList.size() == 1 && VariableName(parsedVariable).startsWith(UNICODE_STRING_SIMPLE("$"))) {

		// if expression does not have more than one chained called AND it starts with a '$' then we want to match (local)
		// variables. This is just a SymbolTable search.
		std::vector<t4p::SymbolClass> scopeSymbols;
		std::map<UnicodeString, std::vector<t4p::SymbolClass>, t4p::UnicodeStringComparatorClass>::const_iterator it;
		
		// if the scope that we are looking for is an anonymous function, take that into account
		UnicodeString scopeString;
		UnicodeString functionName = variableScope.MethodName;
		if (variableScope.IsAnonymousScope()) {
			functionName = ScopeStringAnonymousFunction(variableScope.MethodName, variableScope.GetAnonymousFunctionCount());
		}
		scopeString = ScopeString(variableScope.ClassName, functionName);
		it = Variables.find(scopeString);
		if (it != Variables.end()) {
			scopeSymbols = it->second;
		}
		for (size_t i = 0; i < scopeSymbols.size(); ++i) {
			if (scopeSymbols[i].Variable.startsWith(VariableName(parsedVariable))) {
				autoCompleteVariableList.push_back(scopeSymbols[i].Variable);
			}
		}
	}
	else {

		// some kind of function call / method chain call
		ResourceMatches(parsedVariable, variableScope, sourceDirs, tagFinderList,
			autoCompleteResourceList, doDuckTyping, false, error);
	}	
}

void t4p::SymbolTableClass::ResourceMatches(pelet::VariableClass parsedVariable, 
												  const pelet::ScopeClass& variableScope, 
												  const std::vector<wxFileName>& sourceDirs,
												  t4p::TagFinderListClass& tagFinderList,
												  std::vector<t4p::TagClass>& resourceMatches,
												  bool doDuckTyping, bool doFullyQualifiedMatchOnly,
												  t4p::SymbolTableMatchErrorClass& error) const {
	std::vector<t4p::SymbolClass> scopeSymbols;
	
	// if the scope that we are looking for is an anonymous function, take that into account
	UnicodeString scopeString;
	UnicodeString functionName = variableScope.MethodName;
	if (variableScope.IsAnonymousScope()) {
		functionName = ScopeStringAnonymousFunction(variableScope.MethodName, variableScope.GetAnonymousFunctionCount());
	}
	scopeString = ScopeString(variableScope.ClassName, functionName);
	
	std::map<UnicodeString, std::vector<t4p::SymbolClass>, UnicodeStringComparatorClass>::const_iterator it = 
		Variables.find(scopeString);
	if (it != Variables.end()) {
		scopeSymbols = it->second;
	}
	
	// take care of the 'use' namespace importing
	pelet::VariableClass originalVariable = parsedVariable;
	ResolveNamespaceAlias(parsedVariable, variableScope);
	
	UnicodeString typeToLookup = ResolveInitialLexemeType(parsedVariable, variableScope, sourceDirs, tagFinderList, 
		doDuckTyping, error, scopeSymbols, *this);

	// continue to the next item in the chain up until the second to last one
	// if we can't resolve a type then just exit
	if (typeToLookup.caseCompare(UNICODE_STRING_SIMPLE("primitive"), 0) == 0) {
		error.ToPrimitiveError(UNICODE_STRING_SIMPLE(""), VariableName(parsedVariable));
	}
	else if (typeToLookup.caseCompare(UNICODE_STRING_SIMPLE("array"), 0) == 0) {
		error.ToArrayError(UNICODE_STRING_SIMPLE(""), VariableName(parsedVariable));
	}
	else if (!parsedVariable.ChainList.empty()) {

		// need the empty check so that we don't overflow when doing 0 - 1 with size_t 
		for (size_t i = 1;  i < (parsedVariable.ChainList.size() - 1) && !typeToLookup.isEmpty() && !error.HasError(); ++i) {	
			UnicodeString nextResource = typeToLookup + UNICODE_STRING_SIMPLE("::") + parsedVariable.ChainList[i].Name;
			UnicodeString resolvedType = tagFinderList.ResolveResourceType(nextResource, sourceDirs);

			if (resolvedType.isEmpty()) {
				error.ToTypeResolution(typeToLookup, parsedVariable.ChainList[i].Name);
			}
			else if (typeToLookup.caseCompare(UNICODE_STRING_SIMPLE("primitive"), 0) == 0) {
				error.ToPrimitiveError(typeToLookup, parsedVariable.ChainList[i].Name);
			}
			else if (typeToLookup.caseCompare(UNICODE_STRING_SIMPLE("array"), 0) == 0) {
				error.ToArrayError(typeToLookup, parsedVariable.ChainList[i].Name);
			}
			typeToLookup = resolvedType;
		}
	}

	UnicodeString resourceToLookup;
	if (!typeToLookup.isEmpty() && parsedVariable.ChainList.size() > 1 && !error.HasError()) {
		resourceToLookup = typeToLookup + UNICODE_STRING_SIMPLE("::") + parsedVariable.ChainList.back().Name;
	}
	else if (!typeToLookup.isEmpty() && !error.HasError()) {

		// in this case; chain list is of size 1 (looking for a function / class name)
		resourceToLookup = typeToLookup;
	}
	else if (!error.HasError() && parsedVariable.ChainList.size() > 1 && typeToLookup.isEmpty() && doDuckTyping) {
		
		// here, even if the type of previous items in the chain could not be resolved
		// but were also known NOT to be errors
		// perform "duck typing" lookups; just look for methods in any class
		resourceToLookup = UNICODE_STRING_SIMPLE("::") + parsedVariable.ChainList.back().Name;
	}

	// now do the "final" lookup; here we will also perform access checks
	// and static access checks
	bool visibilityError = false;
	bool isStaticCall = IsStaticVariable(parsedVariable);
	bool isThisCall = VariableName(parsedVariable).caseCompare(UNICODE_STRING_SIMPLE("$this"), 0) == 0
			|| VariableName(parsedVariable).caseCompare(UNICODE_STRING_SIMPLE("self"), 0) == 0;
	bool isParentCall = VariableName(parsedVariable).caseCompare(UNICODE_STRING_SIMPLE("parent"), 0) == 0;

	if (!error.HasError()) {
		t4p::TagSearchClass tagSearch(resourceToLookup);
		tagSearch.SetParentClasses(tagFinderList.ClassParents(tagSearch.GetClassName(), tagSearch.GetMethodName()));
		tagSearch.SetTraits(tagFinderList.ClassUsedTraits(tagSearch.GetClassName(), tagSearch.GetParentClasses(), tagSearch.GetMethodName(), sourceDirs));
		
		// only do duck typing if needed. otherwise, make sure that we have a type match first.
		std::vector<t4p::TagClass> matches;
		if (doDuckTyping || !typeToLookup.isEmpty()) {
			if (doFullyQualifiedMatchOnly) {
				tagFinderList.ExactMatchesFromAll(tagSearch, matches, sourceDirs);
				tagFinderList.ExactTraitAliasesFromAll(tagSearch, matches);
			}
			else  {
				tagFinderList.NearMatchesFromAll(tagSearch, matches, sourceDirs);
				tagFinderList.NearMatchTraitAliasesFromAll(tagSearch, matches);
			}
			std::sort(matches.begin(), matches.end());
		}

		// now we loop through the possbile matches and remove stuff that does not 
		// make sense because of visibility rules
		for (size_t i = 0; i < matches.size(); ++i) {
			t4p::TagClass tag = matches[i];
			bool isVisible = IsResourceVisible(tag, originalVariable, variableScope, isStaticCall, isThisCall, isParentCall);
			if (isVisible) {
				UnresolveNamespaceAlias(originalVariable, variableScope, tag);
				resourceMatches.push_back(tag);
			}
			else if (!isVisible) {
				visibilityError = true;
			}
		}
	}

	// don't overwrite a previous error (PRIMITIVE_ERROR, etc...)
	if (!error.HasError() && visibilityError && resourceMatches.empty()) {
		error.ToVisibility(parsedVariable, typeToLookup);
	}
	else if (!error.HasError() && resourceMatches.empty()) {
		error.ToUnknownResource(parsedVariable, typeToLookup);
	}
}

std::vector<t4p::SymbolClass>& t4p::SymbolTableClass::GetScope(const UnicodeString& className, 
		const UnicodeString& methodName) {
	UnicodeString scopeString = ScopeString(className , methodName);
	if (Variables[scopeString].empty()) {
		CreatePredefinedVariables(Variables[scopeString]);
	}
	return Variables[scopeString];
}

void t4p::SymbolTableClass::Print() const {
	UFILE *out = u_finit(stdout, NULL, NULL);
	for(std::map<UnicodeString, std::vector<t4p::SymbolClass>, UnicodeStringComparatorClass>::const_iterator it = Variables.begin(); it != Variables.end(); ++it) {
		std::vector<t4p::SymbolClass> scopedSymbols = it->second;
		UnicodeString s = it->first;
		u_fprintf(out, "Symbol Table For %S\n", s.getTerminatedBuffer());
		for (size_t j = 0; j < scopedSymbols.size(); ++j) {
			t4p::SymbolClass symbol = scopedSymbols[j];
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

void t4p::SymbolTableClass::CreatePredefinedVariables(std::vector<t4p::SymbolClass>& scope) {
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
		t4p::SymbolClass variableSymbol(variables[i], t4p::SymbolClass::SCALAR);
		scope.push_back(variableSymbol);
	}
}

void t4p::SymbolTableClass::ResolveNamespaceAlias(pelet::VariableClass& parsedVariable, const pelet::ScopeClass& scope) const {
	
	// aliases are only in the beginning of the expression
	// for example, for the expression 
	// \Class::func()
	// name variable will be "\Class"
	// skip over variable expressions since they can't be aliased
	// leave fully qualified names alone
	UnicodeString name = VariableName(parsedVariable);
	if (!name.startsWith(UNICODE_STRING_SIMPLE("$")) && !name.startsWith(UNICODE_STRING_SIMPLE("\\"))) {
		std::map<UnicodeString, UnicodeString, pelet::UnicodeStringComparatorClass> aliases = scope.GetNamespaceAliases();
		std::map<UnicodeString, UnicodeString, pelet::UnicodeStringComparatorClass>::const_iterator it;
		for (it = aliases.begin(); it != aliases.end(); ++it) {
			
			// map key is the alias
			// check to see if the expression begins with the alias
			// need to watch out for the namespace operator
			// the expression may or may not have it
			UnicodeString alias(it->first);
			UnicodeString aliasStart(it->first);
			if (!aliasStart.endsWith(UNICODE_STRING_SIMPLE("\\"))) {
				aliasStart += UNICODE_STRING_SIMPLE("\\");
			}
			if (name == alias || name.startsWith(aliasStart)) {
				UnicodeString afterAlias(name, it->first.length());
				name = it->second + afterAlias;
				parsedVariable.ChainList[0].Name = name;
				break;
			}
		}
	}
}

void t4p::SymbolTableClass::UnresolveNamespaceAlias(const pelet::VariableClass& originalVariable, const pelet::ScopeClass& scope, t4p::TagClass& tag) const {
	UnicodeString name = tag.Identifier;
	
	// leave variables and fully qualified names alone
	UnicodeString originalName = VariableName(originalVariable);
	if (!originalName.startsWith(UNICODE_STRING_SIMPLE("$")) && !originalName.startsWith(UNICODE_STRING_SIMPLE("\\"))) {
		
		std::map<UnicodeString, UnicodeString, pelet::UnicodeStringComparatorClass> aliases = scope.GetNamespaceAliases();
		std::map<UnicodeString, UnicodeString, pelet::UnicodeStringComparatorClass>::const_iterator it;
		for (it = aliases.begin(); it != aliases.end(); ++it) {
			
			// map value is the fully qualified name
			// check to see if the tag begins with the fully qualified aliased name
			// need to watch out for the namespace operator
			// the expression may or may not have it
			UnicodeString qualified(it->second);
			if (!qualified.endsWith(UNICODE_STRING_SIMPLE("\\"))) {
				qualified += UNICODE_STRING_SIMPLE("\\");
			}
			if (name.startsWith(qualified)) {
				UnicodeString afterQualified(name, it->second.length());
				name = it->first + afterQualified;
				tag.Identifier = name;
				break;
			}
		}
	}
}

void t4p::SymbolTableClass::SetVersion(pelet::Versions version) {
	Parser.SetVersion(version);
	Lexer.SetVersion(version);
}

t4p::ScopeFinderClass::ScopeFinderClass() 
	: Scope()
	, LastNamespace()
	, Parser() {
	Parser.SetClassObserver(this);
	Parser.SetClassMemberObserver(this);
	Parser.SetFunctionObserver(this);
}

void t4p::ScopeFinderClass::SetVersion(pelet::Versions version) {
	Parser.SetVersion(version);
}

void t4p::ScopeFinderClass::ClassEnd(const UnicodeString& namespaceName, const UnicodeString& className, int pos) {
	if (pos >= PosToCheck) {
		return;
	}
	Scope.MethodName = UNICODE_STRING_SIMPLE("");
}


void t4p::ScopeFinderClass::NamespaceDeclarationFound(const UnicodeString& namespaceName, int startingPos) {
	if (startingPos >= PosToCheck) {
		return;
	}
	CheckLastNamespace(namespaceName);
	
	// add support for the namespace static operator
	if (namespaceName != UNICODE_STRING_SIMPLE("\\")) {
		Scope.AddNamespaceAlias(namespaceName, UNICODE_STRING_SIMPLE("namespace"));
	}
}

void t4p::ScopeFinderClass::NamespaceUseFound(const UnicodeString& namespaceName, const UnicodeString& alias, int startingPos) {
	if (startingPos >= PosToCheck) {
		return;
	}
	Scope.AddNamespaceAlias(namespaceName, alias);
}

void t4p::ScopeFinderClass::MethodScope(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName,
		int startingPos, int endingPos) {
	if (startingPos <= PosToCheck && PosToCheck <= endingPos) {
		CheckLastNamespace(namespaceName);
		Scope.ClassName = className;
		Scope.MethodName = methodName;
	}
}

void t4p::ScopeFinderClass::FunctionScope(const UnicodeString& namespaceName, const UnicodeString& functionName, 
		int startingPos, int endingPos) {
	if (startingPos <= PosToCheck && PosToCheck <= endingPos) {
		CheckLastNamespace(namespaceName);
		Scope.ClassName = UNICODE_STRING_SIMPLE("");
		Scope.MethodName = functionName;
	}
}


void t4p::ScopeFinderClass::GetScopeString(const UnicodeString& code, int pos, pelet::ScopeClass& scope) {
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

void t4p::ScopeFinderClass::CheckLastNamespace(const UnicodeString& namespaceName) {
	if (LastNamespace.caseCompare(namespaceName, 0) != 0) {
		
		// cannot Clear() the scope result because it would delete the aliases, and the aliases
		// are created outside of any method/function scope
		Scope.NamespaceName = namespaceName;
		LastNamespace = namespaceName;
	}
}

t4p::SymbolClass::SymbolClass(const UnicodeString& variable, t4p::SymbolClass::Types type)
	: Variable(variable)
	, PhpDocType()
	, ChainList()
	, ArrayKeys()
	, Type(type) {
		
}