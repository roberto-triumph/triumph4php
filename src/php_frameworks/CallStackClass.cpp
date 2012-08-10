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
#include <php_frameworks/CallStackClass.h>
#include <windows/StringHelperClass.h>
#include <algorithm>


static UnicodeString EscapeScalar(const UnicodeString& expr) {
	UnicodeString content = expr;
	content = content.findAndReplace(UNICODE_STRING_SIMPLE("\""), UNICODE_STRING_SIMPLE("\\\""));
	content = content.findAndReplace(UNICODE_STRING_SIMPLE("\r\n"), UNICODE_STRING_SIMPLE(" "));
	content = content.findAndReplace(UNICODE_STRING_SIMPLE("\n"), UNICODE_STRING_SIMPLE(" "));
	content = content.findAndReplace(UNICODE_STRING_SIMPLE("\r"), UNICODE_STRING_SIMPLE(" "));
	content = content.findAndReplace(UNICODE_STRING_SIMPLE("\t"), UNICODE_STRING_SIMPLE(" "));
	
	UnicodeString line;
	line += UNICODE_STRING_SIMPLE("\"");
	line += content;
	line += UNICODE_STRING_SIMPLE("\"");
	return line;
}

mvceditor::CallClass::CallClass()
	: Resource()
	, Symbol(UNICODE_STRING_SIMPLE(""), mvceditor::SymbolClass::UNKNOWN)
	, Scope()
	, Expression(Scope)
	, Type(NONE) {
}

mvceditor::CallStackClass::CallStackClass(mvceditor::ResourceCacheClass& resourceCache)
	: List()
	, LintResults()
	, MatchError()
	, Parser()
	, CurrentClass()
	, CurrentMethod()
	, CurrentFunction()
	, ResourcesRemaining()
	, ResourceCache(resourceCache)
	, ScopeVariables()
	, ScopeFunctionCalls()
	, ParsedMethods()
	, FoundScope(false) {
	Parser.SetClassObserver(this);
	Parser.SetClassMemberObserver(this);
	Parser.SetFunctionObserver(this);
	Parser.SetVariableObserver(this);
	Parser.SetExpressionObserver(this);
}

void mvceditor::CallStackClass::Clear() {
	List.clear();
	LintResults.Clear();
	MatchError.Clear();
	CurrentClass.remove();
	CurrentMethod.remove();
	CurrentFunction.remove();
	ScopeVariables.clear();
	ScopeFunctionCalls.clear();
	ParsedMethods.clear();
	while (!ResourcesRemaining.empty()) {
		ResourcesRemaining.pop();
	}
	FoundScope = false;
}

bool mvceditor::CallStackClass::Build(const wxFileName& fileName, const UnicodeString& className, const UnicodeString& methodName, 
		mvceditor::CallStackClass::Errors& error) {
	Clear();
	mvceditor::ResourceClass nextResource;
	nextResource.Type = mvceditor::ResourceClass::METHOD;
	nextResource.ClassName = className;
	nextResource.Identifier = methodName;

	ResourceWithFile item;
	item.FileName = fileName;
	item.Resource = nextResource;
	ResourcesRemaining.push(item);
	return Recurse(error);
}

bool mvceditor::CallStackClass::Recurse(mvceditor::CallStackClass::Errors& error) {

	// base case: no more functions to parse
	if (ResourcesRemaining.empty()) {
		return true;
	}

	// at a certain point, just stop the recursion.
	if (List.size() >= (size_t)100) {
		error = STACK_LIMIT;
		return false;
	}
	if (ResourceCache.IsResourceCacheEmpty()) {
		error = EMPTY_CACHE;
		return false;
	}

	ResourceWithFile item = ResourcesRemaining.front();

	// don't pop() yet; the parser callbacks need to know the resource that we want to examine
	// this is because we only want to look at expressions in one function
	wxFileName fileName = item.FileName;

	// ScanFile will call the callbacks MethodFound, ExpressionFound; any function calls for this file will be collected
	FoundScope = false;

	// need to create the symbols for the file if the cache does not have them yet; symbols allow us to know the variable
	// types
	bool newlyRegistered = ResourceCache.Register(fileName.GetFullPath(), true);

	wxFFile file(fileName.GetFullPath(), wxT("rb"));
	bool ret = Parser.ScanFile(file.fp(), mvceditor::StringHelperClass::wxToIcu(fileName.GetFullPath()), LintResults);
	file.Close();
	UnicodeString key = item.Resource.ClassName + UNICODE_STRING_SIMPLE("::")  + item.Resource.Identifier;
	ParsedMethods[key] = true;
	if (ret && FoundScope) {
		CreateCalls();
		
		// CreateCalls() method can fill in another error too
		if (MatchError.HasError()) {
			
			// leave ret as true; we want may want an incomplete call stack
			error = RESOLUTION_ERROR;
		}

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
					ScopeVariables.clear();
					CreateCalls();					
					ResourcesRemaining.pop();
				}
			}
			if (hasNext) {
				ScopeFunctionCalls.clear();
				ScopeVariables.clear();
				return Recurse(error);
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
		ResourceCache.Unregister(fileName.GetFullPath());
	}
	return ret;

}

bool mvceditor::CallStackClass::Persist(wxFileName& fileName) {
	bool write = false;
	wxFFile file;
	if (file.Open(fileName.GetFullPath(), wxT("wb"))) {
		write = true;
		wxString line;
		for (std::vector<mvceditor::CallClass>::const_iterator it = List.begin(); it != List.end() && write; ++it) {
			line.Clear();
			line += mvceditor::StringHelperClass::IcuToWx(it->Serialize());
			if (!line.IsEmpty()) {
				line += wxT("\n");
				write = file.Write(line);
			}
		}
		file.Close();
	}
	return write;
}

void mvceditor::CallStackClass::ExpressionFound(const pelet::ExpressionClass& expression) {

	// only collect expressions that are in the scope we want
	if (ResourcesRemaining.empty()) {
		return;
	}

	ResourceWithFile item = ResourcesRemaining.front();

	if (item.Resource.Identifier == CurrentFunction || (item.Resource.ClassName == CurrentClass && item.Resource.Identifier == CurrentMethod)) {
		FoundScope = true;

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

		// ATTN: a single variable may have many assignments
		// for now just take the first one
		bool found = false;
		for (size_t i = 0; i < ScopeVariables.size(); ++i) {
			if (!variable.ChainList.empty() && ScopeVariables[i].Variable == variable.ChainList[0].Name) {
				found = true;

				if (!variable.ArrayKey.isEmpty()) {

					// update any new Array keys used in the variable assignment
					// make sure not to have duplicates in case the same key is assigned
					// multiple times
					std::vector<UnicodeString>::iterator it = std::find(
					            ScopeVariables[i].ArrayKeys.begin(), ScopeVariables[i].ArrayKeys.end(), variable.ArrayKey);
					if (it == ScopeVariables[i].ArrayKeys.end()) {
						ScopeVariables[i].ArrayKeys.push_back(variable.ArrayKey);
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
				type = mvceditor::SymbolClass::ARRAY;
				
				// in  PHP an array may be created by assiging
				// an array key-value to a non-existant variable
				arrayKeys.push_back(variable.ArrayKey);
			}

			mvceditor::SymbolClass newSymbol(name, type);
			newSymbol.ChainList = expression.ChainList;
			newSymbol.PhpDocType = variable.PhpDocType;
			newSymbol.ArrayKeys = arrayKeys;
			ScopeVariables.push_back(newSymbol);
		}
	}
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
	}
}

void mvceditor::CallStackClass::CreateCalls() {
	if (ResourcesRemaining.empty()) {
		return;
	}
	
	ResourceWithFile item = ResourcesRemaining.front();

	// add it to the code to signal the start of a function call
	mvceditor::CallClass cBegin;
	if (mvceditor::ResourceClass::FUNCTION == item.Resource.Type) {
		cBegin.ToBeginFunction(item.Resource);
	}
	else if (mvceditor::ResourceClass::METHOD == item.Resource.Type) {
		cBegin.ToBeginMethod(item.Resource);
	}
	if (mvceditor::CallClass::NONE != cBegin.Type) {
		List.push_back(cBegin);
	}
	
	// add it to the code to signal function call parameters
	for (size_t k = 0; k < item.CallArguments.size(); ++k) {
		
		// look for the parameter in the scopeVariables list
		mvceditor::CallClass c;
		c.ToParam(item.CallArguments[k]);
		for (size_t m = 0; m < item.ScopeVariables.size(); ++m) {
			if (item.ScopeVariables[m].Variable == item.CallArguments[k].FirstValue()) {
				c.Symbol = item.ScopeVariables[m];
				break;
			}
		}
		List.push_back(c);
	}

	// go through the symbols and create the CallClass instances
	for (size_t i = 0; i < ScopeVariables.size(); ++i) {
		mvceditor::CallClass c;
		switch (ScopeVariables[i].Type) {
			case mvceditor::SymbolClass::ARRAY:
				c.ToArray(ScopeVariables[i]);
				break;
			case mvceditor::SymbolClass::OBJECT:
				c.ToObject(ScopeVariables[i]);
				break;
			case mvceditor::SymbolClass::SCALAR:
				c.ToScalar(ScopeVariables[i]);
				break;
			case mvceditor::SymbolClass::UNKNOWN:
				break;
		}
		if (mvceditor::CallClass::NONE != c.Type) {
			List.push_back(c);
		}
	}

	// go through the function calls and add them to the recursion list
	for (size_t i = 0; i < ScopeFunctionCalls.size(); ++i) {

		// here expr is a FULL line like this
		// $this->load->view('welcome/index', $data);
		// $this->template()->set('user', $this->getUser());
		pelet::ExpressionClass expr = ScopeFunctionCalls[i];
		pelet::ExpressionClass subExpr(expr.Scope);
		for (size_t j = 0; j < expr.ChainList.size(); ++j) {

			// go through each expression property and look for function calls
			// we keep adding the property to the subExpr so that we can properly resolve
			// the class of the method being called
			// for example; subExpr will first be "$this", then "$this->template()", then
			// "$this->template()->set()"
			// Since expr was parsed properly by pelet::ParserClass, we will handle function
			// call arguments properly
			subExpr.ChainList.push_back(expr.ChainList[j]);
			if (expr.ChainList[j].IsFunction) {
				std::vector<mvceditor::ResourceClass> matches;
				mvceditor::SymbolTableMatchErrorClass singleMatchError;
				pelet::ScopeClass scopeResult;
				scopeResult.ClassName = expr.Scope.ClassName;
				scopeResult.MethodName = expr.Scope.MethodName;

				ResourceCache.ResourceMatches(item.FileName.GetFullPath(), subExpr, scopeResult, matches, false, true, singleMatchError);
				for (std::vector<mvceditor::ResourceClass>::iterator it = matches.begin(); it != matches.end(); ++it) {

					// if we get here; we are able to know which class and method are being called
					// lets add it to the queue so that this class/method will get recursed into next
					if (mvceditor::ResourceClass::FUNCTION == it->Type || mvceditor::ResourceClass::METHOD == it->Type) {
						if (it->FileName().IsOk()) {
							
							// dynamic resources may not have a file path to go to
							ResourceWithFile newItem;
							newItem.FileName = it->FileName();
							newItem.Resource = *it;
							newItem.CallArguments = expr.ChainList[j].CallArguments;
							newItem.ScopeVariables = ScopeVariables;
							ResourcesRemaining.push(newItem);
						}
					}
				}
				if (!MatchError.HasError() && (singleMatchError.HasError() || matches.empty())) {

					// using 2 variables so that previous errors do not affect the ResourceMatches() call
					MatchError = singleMatchError;
				}
			}
		}
	}
	
	// signal the end of this function
	mvceditor::CallClass cReturn;
	cReturn.ToReturn();
	List.push_back(cReturn);
}

void mvceditor::CallClass::ToArray(const mvceditor::SymbolClass& symbol) {
	Type = mvceditor::CallClass::ARRAY;
	Symbol = symbol;
}

void mvceditor::CallClass::ToBeginFunction(const mvceditor::ResourceClass& resource) {
	Type = mvceditor::CallClass::BEGIN_FUNCTION;
	Resource = resource;
}

void mvceditor::CallClass::ToBeginMethod(const mvceditor::ResourceClass& resource) {
	Type = mvceditor::CallClass::BEGIN_METHOD;
	Resource = resource;
}

void mvceditor::CallClass::ToObject(const mvceditor::SymbolClass& symbol) {
	Type = mvceditor::CallClass::OBJECT;
	Symbol = symbol;
}

void mvceditor::CallClass::ToParam(const pelet::ExpressionClass& expr) {
	Type = mvceditor::CallClass::PARAM;
	Expression = expr;
}

void mvceditor::CallClass::ToReturn() {
	Type = mvceditor::CallClass::RETURN;
}

void mvceditor::CallClass::ToScalar(const mvceditor::SymbolClass& symbol) {
	Type = mvceditor::CallClass::SCALAR;
	Symbol = symbol;
}

UnicodeString mvceditor::CallClass::Serialize() const {
	UnicodeString line;
	switch (Type) {
		case mvceditor::CallClass::ARRAY:
			line += UNICODE_STRING_SIMPLE("ARRAY,");
			line += Symbol.Variable;
			for (size_t j = 0; j < Symbol.ArrayKeys.size(); ++j) {
				line += UNICODE_STRING_SIMPLE(",");
				line += Symbol.ArrayKeys[j];
			}
			break;

		case mvceditor::CallClass::BEGIN_FUNCTION:
			line += UNICODE_STRING_SIMPLE("BEGIN_FUNCTION,");
			line += Resource.Identifier;
			break;

		case mvceditor::CallClass::BEGIN_METHOD:
			line += UNICODE_STRING_SIMPLE("BEGIN_METHOD,");
			line += Resource.ClassName;
			line += UNICODE_STRING_SIMPLE(",");
			line += Resource.Identifier;
			break;

		case mvceditor::CallClass::OBJECT:
			line += UNICODE_STRING_SIMPLE("OBJECT,");
			line += Symbol.Variable;
			break;

		case mvceditor::CallClass::NONE:

			// none will be skipped always
			break;
		case mvceditor::CallClass::PARAM:
			line += UNICODE_STRING_SIMPLE("PARAM,");
			if (mvceditor::SymbolClass::ARRAY == Symbol.Type) {
				line += UNICODE_STRING_SIMPLE("ARRAY,");
				line += Symbol.Variable;
				for (size_t j = 0; j < Symbol.ArrayKeys.size(); ++j) {
					line += UNICODE_STRING_SIMPLE(",");
					line += Symbol.ArrayKeys[j];
				}
			}
			else if (mvceditor::SymbolClass::OBJECT == Symbol.Type) {
				line += UNICODE_STRING_SIMPLE("OBJECT,");
				for (size_t j = 0; j < Symbol.ChainList.size(); ++j) {
					if (j > 0 && Symbol.ChainList[j].IsStatic) {
						line += UNICODE_STRING_SIMPLE("::");
					}
					else if (j > 0) {
						line += UNICODE_STRING_SIMPLE("->");
					}
					line += Symbol.ChainList[j].Name;
					if (Symbol.ChainList[j].IsFunction) {
						line += UNICODE_STRING_SIMPLE("()");
					}
				}
			}
			else if (mvceditor::SymbolClass::SCALAR == Symbol.Type) {
				line += UNICODE_STRING_SIMPLE("SCALAR,");
				
				// escape any double quotes in case of string constants
				line += EscapeScalar(Symbol.ChainList[0].Name);
			}
			else if (pelet::ExpressionClass::SCALAR == Expression.ExpressionType) {
				line += UNICODE_STRING_SIMPLE("SCALAR,");
				
				// escape any double quotes in case of string constants
				line += EscapeScalar(Expression.ChainList[0].Name);
			}
			else {
				line += UNICODE_STRING_SIMPLE("VARIABLE,");
				for (size_t j = 0; j < Expression.ChainList.size(); ++j) {
					if (j > 0 && Expression.ChainList[j].IsStatic) {
						line += UNICODE_STRING_SIMPLE("::");
					}
					else if (j > 0) {
						line += UNICODE_STRING_SIMPLE("->");
					}
					line += Expression.ChainList[j].Name;
					if (Expression.ChainList[j].IsFunction) {
						line += UNICODE_STRING_SIMPLE("()");
					}
				}
			}
			break;

		case mvceditor::CallClass::RETURN:
			line += UNICODE_STRING_SIMPLE("RETURN");
			break;

		case mvceditor::CallClass::SCALAR:
			line += Symbol.Variable;
			line += UNICODE_STRING_SIMPLE(",");
			if (!Symbol.ChainList.empty()) {

				// escape any double quotes in case of string constants
				line += EscapeScalar(Symbol.ChainList[0].Name);
			}
			break;
	}
	return line;
}