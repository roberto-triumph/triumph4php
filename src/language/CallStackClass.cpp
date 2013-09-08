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
#include <language/CallStackClass.h>
#include <globals/String.h>
#include <globals/Assets.h>
#include <algorithm>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <globals/Sqlite.h>
#include <wx/ffile.h>

mvceditor::VariableSymbolClass::VariableSymbolClass() 
	: Type(mvceditor::VariableSymbolClass::SCALAR)
	, DestinationVariable()
	, ScalarValue()
	, ArrayKey()
	, SourceVariable()
	, ObjectName()
	, PropertyName()
	, FunctionName() 
	, ClassName()
	, FunctionArguments() {
		
}

void mvceditor::VariableSymbolClass::ToScalar(const UnicodeString& variableName, const UnicodeString& scalar) {
	Type = mvceditor::VariableSymbolClass::SCALAR;
	DestinationVariable = variableName;
	ScalarValue = scalar;
}

void mvceditor::VariableSymbolClass::ToArray(const UnicodeString& variableName) {
	Type = mvceditor::VariableSymbolClass::ARRAY;
	DestinationVariable = variableName;
}

void mvceditor::VariableSymbolClass::ToArrayKey(const UnicodeString& variableName, const UnicodeString& keyName) {
	Type = mvceditor::VariableSymbolClass::ARRAY_KEY;
	DestinationVariable = variableName;
	ArrayKey = keyName;
}

void mvceditor::VariableSymbolClass::ToNewObject(const UnicodeString& variableName, const UnicodeString& className) {
	Type = mvceditor::VariableSymbolClass::NEW_OBJECT;
	DestinationVariable = variableName;
	ClassName = className;
}

void mvceditor::VariableSymbolClass::ToAssignment(const UnicodeString& variableName, const UnicodeString& sourceVariableName) {
	Type = mvceditor::VariableSymbolClass::ASSIGN;
	DestinationVariable = variableName;
	SourceVariable = sourceVariableName;
}

void mvceditor::VariableSymbolClass::ToProperty(const UnicodeString& variableName, const UnicodeString& objectName, const UnicodeString& propertyName) {
	Type = mvceditor::VariableSymbolClass::PROPERTY;
	DestinationVariable = variableName;
	ObjectName = objectName;
	PropertyName = propertyName;
}

void mvceditor::VariableSymbolClass::ToMethodCall(const UnicodeString& variableName, const UnicodeString& objectName, const UnicodeString& methodName, const std::vector<UnicodeString> arguments) {
	Type = mvceditor::VariableSymbolClass::METHOD_CALL;
	DestinationVariable = variableName;
	ObjectName = objectName;
	MethodName = methodName;
	FunctionArguments = arguments;
}

void mvceditor::VariableSymbolClass::ToFunctionCall(const UnicodeString& variableName, const UnicodeString& functionName, const std::vector<UnicodeString> arguments) {
	Type = mvceditor::VariableSymbolClass::FUNCTION_CALL;
	DestinationVariable = variableName;
	FunctionName = functionName;
	FunctionArguments = arguments;
}

void mvceditor::VariableSymbolClass::ToBeginMethod(const UnicodeString& className, const UnicodeString& methodName) {
	Type = mvceditor::VariableSymbolClass::BEGIN_METHOD;
	ClassName = className;
	MethodName = methodName;
}

void mvceditor::VariableSymbolClass::ToBeginFunction(const UnicodeString& functionName) {
	Type = mvceditor::VariableSymbolClass::FUNCTION_CALL;
	FunctionName = functionName;
}

std::string mvceditor::VariableSymbolClass::ToString() const {
	UnicodeString line;
	switch (Type) {
	case ARRAY:
		line = DestinationVariable;
		break;
	case ARRAY_KEY:
		line = DestinationVariable;
		line += UNICODE_STRING_SIMPLE(",");
		line += ArrayKey;
		break;
	case ASSIGN:
		line = DestinationVariable;
		line += UNICODE_STRING_SIMPLE(",");
		line += SourceVariable;
		break;
	case SCALAR:
		line = DestinationVariable;
		line += UNICODE_STRING_SIMPLE(",");
		line += ScalarValue;
		break;
	case NEW_OBJECT:
		line = DestinationVariable;
		line += UNICODE_STRING_SIMPLE(",");
		line += ClassName;
		break;
	case PROPERTY:
		line = DestinationVariable;
		line += UNICODE_STRING_SIMPLE(",");
		line += ObjectName;
		line += UNICODE_STRING_SIMPLE(",");
		line += PropertyName;
		break;
	case METHOD_CALL:
		line = DestinationVariable;
		line += UNICODE_STRING_SIMPLE(",");
		line += ObjectName;
		line += UNICODE_STRING_SIMPLE(",");
		line += MethodName;
		for (std::vector<UnicodeString>::const_iterator it = FunctionArguments.begin(); it != FunctionArguments.end(); ++it) {
			line += UNICODE_STRING_SIMPLE(",");
			line += *it;
		}
		break;
	case FUNCTION_CALL:
		line = DestinationVariable;
		line += UNICODE_STRING_SIMPLE(",");
		line += ObjectName;
		line += UNICODE_STRING_SIMPLE(",");
		line += MethodName;
		for (std::vector<UnicodeString>::const_iterator it = FunctionArguments.begin(); it != FunctionArguments.end(); ++it) {
			line += UNICODE_STRING_SIMPLE(",");
			line += *it;
		}
		break;
	case BEGIN_METHOD:
		line += ClassName;
		line += UNICODE_STRING_SIMPLE(",");
		line += MethodName;
		break;
	case BEGIN_FUNCTION:
		line += FunctionName;
		break;
	}	
	std::string stdLine = mvceditor::IcuToChar(line);
	return stdLine;
}

std::string mvceditor::VariableSymbolClass::TypeString() const {
	std::string line;
	switch (Type) {
	case ARRAY:
		line = "ARRAY";
		break;
	case ARRAY_KEY:
		line = "ARRAY_KEY";
		break;
	case ASSIGN:
		line = "ASSIGN";
		break;
	case SCALAR:
		line = "SCALAR";
		break;
	case NEW_OBJECT:
		line = "NEW_OBJECT";
		break;
	case PROPERTY:
		line = "PROPERTY";
		break;
	case METHOD_CALL:
		line = "METHOD_CALL";
		break;
	case FUNCTION_CALL:
		line = "FUNCTION_CALL";
		break;
	case BEGIN_METHOD:
		line = "BEGIN_METHOD";
		break;
	case BEGIN_FUNCTION:
		line = "BEGIN_FUNCTION";
		break;
	}	
	return line;
}

mvceditor::CallStackClass::CallStackClass(mvceditor::TagCacheClass& tagCache)
	: Variables()
	, LintResults()
	, MatchError()
	, Parser()
	, CurrentClass()
	, CurrentMethod()
	, CurrentFunction()
	, ResourcesRemaining()
	, TagCache(tagCache)
	, ScopeFunctionCalls()
	, ParsedMethods()
	, TempVarIndex(1)
	, FoundScope(false) {
	Parser.SetClassObserver(this);
	Parser.SetClassMemberObserver(this);
	Parser.SetFunctionObserver(this);
	Parser.SetExpressionObserver(this);
}

void mvceditor::CallStackClass::Clear() {
	Variables.clear();
	LintResults.Clear();
	MatchError.Clear();
	CurrentClass.remove();
	CurrentMethod.remove();
	CurrentFunction.remove();
	ScopeFunctionCalls.clear();
	ParsedMethods.clear();
	while (!ResourcesRemaining.empty()) {
		ResourcesRemaining.pop();
	}
	TempVarIndex = 1;
	FoundScope = false;
}

bool mvceditor::CallStackClass::Build(const wxFileName& fileName, const UnicodeString& className, const UnicodeString& methodName, 
		pelet::Versions version, mvceditor::CallStackClass::Errors& error) {
	Clear();
	mvceditor::TagClass nextResource;
	nextResource.Type = mvceditor::TagClass::METHOD;
	nextResource.ClassName = className;
	nextResource.Identifier = methodName;

	ResourceWithFile item;
	item.FileName = fileName;
	item.Resource = nextResource;
	ResourcesRemaining.push(item);
	return Recurse(version, error);
}

bool mvceditor::CallStackClass::Recurse(pelet::Versions version, mvceditor::CallStackClass::Errors& error) {

	// base case: no more functions to parse
	if (ResourcesRemaining.empty()) {
		return true;
	}

	// at a certain point, just stop the recursion.
	if (Variables.size() >= (size_t)3000) {
		error = STACK_LIMIT;
		return false;
	}
	if (TagCache.IsResourceCacheEmpty()) {
		error = EMPTY_CACHE;
		return false;
	}

	ResourceWithFile item = ResourcesRemaining.front();

	// don't pop() yet; the parser callbacks need to know the tag that we want to examine
	// this is because we only want to look at expressions in one function
	wxFileName fileName = item.FileName;

	// ScanFile will call the callbacks MethodFound, ExpressionFound; any function calls for this file will be collected
	FoundScope = false;

	// need to create the symbols for the file if the cache does not have them yet; symbols allow us to know the variable
	// types
	mvceditor::WorkingCacheClass* workingCache = new mvceditor::WorkingCacheClass;

	// here file identifier == file name because file name exists and is unique
	workingCache->Init(fileName.GetFullPath(), fileName.GetFullPath(), false, version, true);
	bool newlyRegistered = TagCache.RegisterWorking(fileName.GetFullPath(), workingCache);

	wxFFile file(fileName.GetFullPath(), wxT("rb"));
	bool ret = Parser.ScanFile(file.fp(), mvceditor::WxToIcu(fileName.GetFullPath()), LintResults);
	file.Close();
	UnicodeString key = item.Resource.ClassName + UNICODE_STRING_SIMPLE("::")  + item.Resource.Identifier;
	ParsedMethods[key] = true;
	if (ret && FoundScope) {
		
		// check to see if we have any new functions to parse
		ResourcesRemaining.pop();
		if (!ResourcesRemaining.empty()) {

			// need to get the file that the next function is in
			// make sure we don't go over the same function again in case there is a
			// recursive function call along the way
			bool hasNext = false;
			while (!hasNext && !ResourcesRemaining.empty()) {
				ResourceWithFile nextItem = ResourcesRemaining.front();
				wxFileName nextFile = nextItem.FileName;
				bool alreadyParsed = false;
				UnicodeString key = nextItem.Resource.ClassName + UNICODE_STRING_SIMPLE("::")  + nextItem.Resource.Identifier;
				alreadyParsed =  ParsedMethods.find(key) == ParsedMethods.end();				
				if (alreadyParsed) {
					
					// already been parsed; write the function arguments for this call and nothing else
					// this is because we want to write a function call if the same function is called
					// twice but we don't want to parse it twice
					ScopeFunctionCalls.clear();
					TempVarIndex = 1;
					ResourcesRemaining.pop();
				}
			}
			if (hasNext) {
				ScopeFunctionCalls.clear();
				TempVarIndex = 1;
				return Recurse(version, error);
			}
		}
	}
	else if (!ret) {
		error = PARSE_ERR0R;
	}
	else if (!FoundScope) {
		ret = false;
		error = RESOURCE_NOT_FOUND;
	}
	if (newlyRegistered) {

		// clean up, but only if this method created the symbols
		// this call will delete the WorkingCache pointer for us
		TagCache.RemoveWorking(fileName.GetFullPath());
	}
	else {

		// tag cache did not use the cache, delete it ourselves
		delete workingCache;
	}
	return ret;

}

bool mvceditor::CallStackClass::Persist(soci::session& session) {
	wxString error;
	bool good = false;
	try {		
		int stepNumber = 0;
		std::string stepType;
		std::string expression;
		soci::transaction transaction(session);
		
		// TODO: prograte?
		int sourceId = 0;

		// delete any old rows; we only store one call stack for the active URL
		session.once << "DELETE FROM call_stacks";
		soci::statement stmt = (session.prepare <<
			"INSERT INTO call_stacks(step_number, step_type, expression, source_id) VALUES (?, ?, ?, ?)",
			soci::use(stepNumber), soci::use(stepType), soci::use(expression), soci::use(sourceId)
		);
		for (std::vector<mvceditor::VariableSymbolClass>::const_iterator it = Variables.begin(); it != Variables.end(); ++it) {
			stepType = it->TypeString();
			expression = it->ToString();
			stmt.execute(true);
			stepNumber++;
		}
		transaction.commit();
		good = true;
	} catch (std::exception& e) {
		wxUnusedVar(e);
		error = mvceditor::CharToWx(e.what());
		good = false;
		wxASSERT_MSG(good, error);
	}
	return good;
}

void mvceditor::CallStackClass::ExpressionFound(const pelet::ExpressionClass& expression) {

	// only collect expressions that are in the scope we want
	if (ResourcesRemaining.empty()) {
		return;
	}

	ResourceWithFile item = ResourcesRemaining.front();

	if (item.Resource.Identifier == CurrentFunction || (item.Resource.ClassName == CurrentClass && item.Resource.Identifier == CurrentMethod)) {
		FoundScope = true;
		
		if (expression.Type == pelet::StatementClass::ASSIGNMENT) {
			pelet::AssignmentExpressionClass* assignmentExpression = (pelet::AssignmentExpressionClass*)&expression;
				VariableFound(assignmentExpression->Destination.Scope.NamespaceName, 
					assignmentExpression->Destination.Scope.ClassName, assignmentExpression->Destination.Scope.MethodName, 
					assignmentExpression->Destination, *assignmentExpression, assignmentExpression->Comment);
		}
		else if (expression.Type == pelet::StatementClass::ASSIGNMENT_LIST) {
			pelet::AssignmentListExpressionClass* assignmentListExpression = (pelet::AssignmentListExpressionClass*)&expression;
				for (size_t i = 0; i < assignmentListExpression->Destinations.size(); ++i) {
					VariableFound(assignmentListExpression->Scope.NamespaceName, 
						assignmentListExpression->Scope.ClassName, assignmentListExpression->Scope.MethodName, 
						assignmentListExpression->Destinations[i], *assignmentListExpression, assignmentListExpression->Comment);
				}
		}
		else {
			SymbolFromExpression(expression, Variables);
		}

		// this is the scope we are interested in. if the expression is a function call
		// note that variable may contain function calls too
		if (pelet::ExpressionClass::FUNCTION_CALL == expression.ExpressionType ||  pelet::ExpressionClass::VARIABLE == expression.ExpressionType) {
			ScopeFunctionCalls.push_back(expression);
		}
	}
}

void mvceditor::CallStackClass::VariableFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName,
        const pelet::VariableClass& variable, const pelet::ExpressionClass& expression, const UnicodeString& comment) {

	// only collect expressions that are in the scope we want
	if (ResourcesRemaining.empty()) {
		return;
	}

	ResourceWithFile item = ResourcesRemaining.front();
	if (item.Resource.Identifier == CurrentFunction || (item.Resource.ClassName == CurrentClass && item.Resource.Identifier == CurrentMethod)) {
		FoundScope = true;
		SymbolsFromVariable(variable, expression);
	}
}

void mvceditor::CallStackClass::SymbolsFromVariable(const pelet::VariableClass& variable, const pelet::ExpressionClass& expression) {
	mvceditor::VariableSymbolClass expressionResultSymbol;
	
	// follow associativity, do the right hand side first
	size_t oldSize = Variables.size();
	SymbolFromExpression(expression, Variables);
	if (Variables.size() > oldSize) {
		expressionResultSymbol = Variables.back();
	}
	
	if (!variable.ArrayKey.isEmpty()) {
		
		// dont need to insert the same array key multiple times
		// add the variable to the list only if we have not added it yet
		// an array key may be assigned if the variable is not yet
		// seen; need to look for this case also
		// ie.  $arr[] = 'name';  
		// as the array initialization for $arr
		std::vector<mvceditor::VariableSymbolClass>::iterator var;
		bool foundIndex = false;
		bool foundVariable = false;
		for (var = Variables.begin(); var != Variables.end(); ++var) {
			if (var->DestinationVariable == variable.ChainList[0].Name) {
				foundVariable = true;
			}
			if (var->DestinationVariable == variable.ChainList[0].Name && var->ArrayKey == variable.ArrayKey) {
				foundIndex = true;
			}
		}
		if (!foundVariable) {
			mvceditor::VariableSymbolClass arrayVariableSymbol;
			arrayVariableSymbol.ToArray(variable.ChainList[0].Name);
			Variables.push_back(arrayVariableSymbol);
		}
		if (!foundIndex) {
			mvceditor::VariableSymbolClass arrayVariableKeySymbol;
			arrayVariableKeySymbol.ToArrayKey(variable.ChainList[0].Name, variable.ArrayKey);
			Variables.push_back(arrayVariableKeySymbol);
		}
		
	}
	else if (!variable.ChainList.empty()) {
		UnicodeString destinationVariable = variable.ChainList[0].Name;
		if (variable.ChainList.size() > 1) {
			std::vector<pelet::VariablePropertyClass>::const_iterator prop = variable.ChainList.begin();
			prop++;
			size_t oldSize = Variables.size();
			UnicodeString nextObjectName = variable.ChainList[0].Name;
			for (; prop != variable.ChainList.end(); ++prop) {
				SymbolFromVariableProperty(nextObjectName, *prop, Variables);
				nextObjectName = Variables.back().DestinationVariable;
			}
			if (Variables.size() > oldSize) {
				destinationVariable = Variables.back().DestinationVariable;
			}
		}
		
		// now assign the right side of the expression to the left side of the
		// expression
		mvceditor::VariableSymbolClass assignSymbol;
		assignSymbol.ToAssignment(destinationVariable, expressionResultSymbol.DestinationVariable);
		Variables.push_back(assignSymbol);
	}
}

void mvceditor::CallStackClass::SymbolFromVariableProperty(const UnicodeString& objectName, const pelet::VariablePropertyClass& property, std::vector<mvceditor::VariableSymbolClass>& symbols) {
	
	// recurse down the arguments first
	std::vector<UnicodeString> argumentVariables;
	if (property.IsFunction && !property.CallArguments.empty()) {
		std::vector<pelet::ExpressionClass>::const_iterator expr;
		for (expr = property.CallArguments.begin(); expr != property.CallArguments.end(); ++expr) {
			size_t oldSize = symbols.size();
			SymbolFromExpression(*expr, symbols);
			if (symbols.size() > oldSize) {
				argumentVariables.push_back(symbols.back().DestinationVariable);
			}
			else {
				// a new variable symbol was not created because the argument already exists in the symbols list
				argumentVariables.push_back(expr->FirstValue());
			}
		}
	}
	
	// now the symbol for this property
	UnicodeString tempVarName = NewTempVariable();
	mvceditor::VariableSymbolClass symbol;
	if (property.IsFunction) {
		symbol.ToMethodCall(tempVarName, objectName, property.Name, argumentVariables);
	}
	else {
		symbol.ToProperty(tempVarName, objectName, property.Name);
	}
	symbols.push_back(symbol);
}

void mvceditor::CallStackClass::SymbolFromExpression(const pelet::ExpressionClass& expression, std::vector<mvceditor::VariableSymbolClass>& symbols) {	
	if (pelet::ExpressionClass::SCALAR == expression.ExpressionType) {
		UnicodeString tempVarName = NewTempVariable();
		mvceditor::VariableSymbolClass scalarSymbol;
		scalarSymbol.ToScalar(tempVarName, expression.FirstValue());
		symbols.push_back(scalarSymbol);
	}
	else if (pelet::ExpressionClass::ARRAY == expression.ExpressionType) {
		UnicodeString tempVarName = NewTempVariable();
		mvceditor::VariableSymbolClass arraySymbol;
		arraySymbol.ToArray(tempVarName);
		symbols.push_back(arraySymbol);
		for (std::vector<UnicodeString>::const_iterator key = expression.ArrayKeys.begin(); key != expression.ArrayKeys.end(); ++key) {
			mvceditor::VariableSymbolClass keySymbol;
			keySymbol.ToArrayKey(tempVarName, *key);
			symbols.push_back(keySymbol);
		}
	}
	else if (pelet::ExpressionClass::NEW_CALL == expression.ExpressionType) {
		UnicodeString tempVarName = NewTempVariable();
		mvceditor::VariableSymbolClass newSymbol;
		newSymbol.ToNewObject(tempVarName, expression.FirstValue());
		symbols.push_back(newSymbol);
	}
	else if (pelet::ExpressionClass::FUNCTION_CALL == expression.ExpressionType && !expression.ChainList.empty()) {
		
		// do the function calls first
		// a function will never have more than 1 item in the chain list because the following code
		// is not possible
		// func1() funct2();
		std::vector<pelet::ExpressionClass>::const_iterator arg;
		std::vector<UnicodeString> argumentVariables;
		for (arg = expression.ChainList[0].CallArguments.begin(); arg != expression.ChainList[0].CallArguments.end(); ++arg) {
			SymbolFromExpression(*arg, symbols);
			if (!symbols.empty()) {
				argumentVariables.push_back(symbols.back().DestinationVariable);
			}
		}
		
		// variable for the function result
		UnicodeString tempVarName = NewTempVariable();
		mvceditor::VariableSymbolClass functionSymbol;
		functionSymbol.ToFunctionCall(tempVarName, expression.FirstValue(), argumentVariables);
	}
	else if (pelet::ExpressionClass::VARIABLE == expression.ExpressionType && !expression.ChainList.empty()) {
		
		// add the variable to the list only if we have not added it yet
		std::vector<mvceditor::VariableSymbolClass>::iterator var;
		bool found = false;
		for (var = symbols.begin(); var != symbols.end(); ++var) {
			if (var->DestinationVariable == expression.ChainList[0].Name) {
				found = true;
				break;
			}
		}
		if (!found) {
			mvceditor::VariableSymbolClass varSymbol;
			varSymbol.ToAssignment(expression.ChainList[0].Name, UNICODE_STRING_SIMPLE(""));
			symbols.push_back(varSymbol);
		}
		
		if (expression.ChainList.size() > 1) {
			
			// now add any property / method accesses
			std::vector<pelet::VariablePropertyClass>::const_iterator prop = expression.ChainList.begin();
			prop++;
			UnicodeString nextObjectName = expression.ChainList[0].Name;
			for (; prop != expression.ChainList.end(); ++prop) {
				size_t oldSize = symbols.size();
				SymbolFromVariableProperty(nextObjectName, *prop, symbols);
				if (symbols.size() > oldSize) {
					nextObjectName = symbols.back().DestinationVariable;
				}
			}			
		}
	}
}

UnicodeString mvceditor::CallStackClass::NewTempVariable() {
	UnicodeString newName;
	
	// 11 == length of "$@tmp" + a 5 digit number + NUL should be big enough
	// using $@tmp so that a temp variable will never collide with a variable found in
	// the source
	int32_t len = u_sprintf(newName.getBuffer(11), "$@tmp%d", TempVarIndex);
	newName.releaseBuffer(len);
	TempVarIndex++;
	return newName;
}

void mvceditor::CallStackClass::MethodFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName, const UnicodeString& signature,
        const UnicodeString& returnType, const UnicodeString& comment, pelet::TokenClass::TokenIds visibility,
        bool isStatic, const int lineNumber) {
	CurrentClass = className;
	CurrentMethod = methodName;
	CurrentFunction.remove();
	
	if (ResourcesRemaining.empty()) {
		return;
	}

	ResourceWithFile item = ResourcesRemaining.front();
	
	// if a method was found set the flag
	// we need to do this because the method iself may be empty or not contain variable
	// and we dont want to flag this as an error
	if (item.Resource.ClassName == CurrentClass && item.Resource.Identifier == CurrentMethod) {
		FoundScope = true;
		
		mvceditor::VariableSymbolClass beginScope;
		beginScope.ToBeginMethod(className, methodName);
		Variables.push_back(beginScope);
	}
}

void mvceditor::CallStackClass::FunctionFound(const UnicodeString& namespaceName, const UnicodeString& functionName, const UnicodeString& signature, const UnicodeString& returnType,
        const UnicodeString& comment, const int lineNumber) {
	CurrentClass.remove();
	CurrentMethod.remove();
	CurrentFunction = functionName;
	
	if (ResourcesRemaining.empty()) {
		return;
	}

	ResourceWithFile item = ResourcesRemaining.front();
	
	// if a method was found set the flag
	// we need to do this because the method iself may be empty or not contain variable
	// and we dont want to flag this as an error
	if (item.Resource.Identifier == CurrentFunction) {
		FoundScope = true;
		mvceditor::VariableSymbolClass beginScope;
		beginScope.ToBeginFunction(functionName);
		Variables.push_back(beginScope);
	}
}
