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


mvceditor::CallClass::CallClass()
	: Resource() 
	, Arguments() {
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
	while (!ResourcesRemaining.empty()) {
		ResourcesRemaining.pop();
	}
	FoundScope = false;
}

bool mvceditor::CallStackClass::Build(const wxFileName& fileName, const UnicodeString& className, const UnicodeString& methodName, Errors& error) {
	Clear();
	UnicodeString nextResource = className + UNICODE_STRING_SIMPLE("::") + methodName;
	
	ResourceWithFile item;
	item.FileName = fileName;
	item.Resource = nextResource;
	ResourcesRemaining.push(item);
	return Recurse(error);
}

bool mvceditor::CallStackClass::Recurse(Errors& error) {
	
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
	if (ret && !MatchError.HasError() && FoundScope) {
		
		// check to see if we have any new functions to parse
		ResourcesRemaining.pop();
		if (!ResourcesRemaining.empty()) {
			
			// need to get the file that the next function is in
			// make sure we don't go over the same function again in case there is a
			// recursive function call along the way
			bool hasNext = false;
			while (!hasNext && !ResourcesRemaining.empty()) {
				ResourceWithFile nextItem = ResourcesRemaining.front();
				UnicodeString nextResource = nextItem.Resource;
				wxFileName nextFile = nextItem.FileName;
				bool alreadyParsed = false;
				for (size_t i = 0; i < List.size(); ++i) {
					if (List[i].Resource.Resource == nextResource) {
						alreadyParsed = true;
						break;
					}
				}
				if (!alreadyParsed) {
					hasNext = true;
				}
				else {
					ResourcesRemaining.pop();
				}
			}
			if (hasNext) {
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
	else {
		
		// leave ret as true; we want may want an incomplete call stack
		error = RESOLUTION_ERROR;
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
		for (std::vector<mvceditor::CallClass>::const_iterator it = List.begin(); it != List.end(); ++it) {
			wxString line;
			mvceditor::ResourceClass callResource = it->Resource;
			line += mvceditor::StringHelperClass::IcuToWx(callResource.Identifier);
			line += wxT(",");
			line += mvceditor::StringHelperClass::IcuToWx(callResource.Resource);
			if (!it->Arguments.empty()) {
				line += wxT(",");
			}
			for (size_t i = 0; i < it->Arguments.size(); ++i) {
				
				// escape any double quotes in case of string constants
				line += wxT("\"");
				UnicodeString lexeme = it->Arguments[i].Lexeme;
				lexeme = lexeme.findAndReplace(UNICODE_STRING_SIMPLE("\""), UNICODE_STRING_SIMPLE("\\\""));
				line += mvceditor::StringHelperClass::IcuToWx(lexeme);
				line += wxT("\"");
				if (i < (it->Arguments.size() - 1)) {
					line += wxT(",");
				}
			}
			line += wxT("\n");
			if (mvceditor::ResourceClass::FUNCTION == callResource.Type) {
				write = file.Write(wxT("FUNCTION,")) && file.Write(line);
				
			}
			else if (mvceditor::ResourceClass::METHOD == callResource.Type) {
				write = file.Write(wxT("METHOD,")) && file.Write(line);
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
	if (item.Resource == CurrentFunction || (item.Resource == (CurrentClass + UNICODE_STRING_SIMPLE("::") + CurrentMethod))) {
		FoundScope = true;
		
		// this is the scope we are interested in. if the expression is a function call,
		// lets add it to the queue AND the final list
		// by adding it to the queue, the method will get parsed
		// also an expression can be a method call (variable + method)
		if (pelet::ExpressionClass::FUNCTION_CALL == expression.Type ||  expression.ChainList.size() >= (size_t)2) {			
			pelet::SymbolClass symbol;
			symbol.FromExpression(expression);
			
			std::vector<mvceditor::ResourceClass> matches;
			mvceditor::SymbolTableMatchErrorClass singleMatchError;
			
			ResourceCache.ResourceMatches(item.FileName.GetFullPath(), symbol, item.Resource, matches, false, singleMatchError);
			for (std::vector<mvceditor::ResourceClass>::iterator it = matches.begin(); it != matches.end(); ++it) {
				if (mvceditor::ResourceClass::FUNCTION == it->Type || mvceditor::ResourceClass::METHOD == it->Type) {
					ResourceWithFile newItem;
					newItem.FileName.Assign(it->GetFullPath());
					newItem.Resource = it->Resource;
					ResourcesRemaining.push(newItem);
					
					mvceditor::CallClass newCall;
					newCall.Resource = *it;
					newCall.Arguments = expression.CallArguments;
					List.push_back(newCall);
				}
			}
			if (singleMatchError.HasError() || matches.empty()) {

				// using 2 variables so that previous errors do not affect the ResourceMatches() call
				MatchError = singleMatchError;
			}
		}
	}
}

void mvceditor::CallStackClass::NamespaceUseFound(const UnicodeString& namespaceName, const UnicodeString& alias) {

}

void mvceditor::CallStackClass::TraitAliasFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& traitUsedClassName,
												  const UnicodeString& traitMethodName, const UnicodeString& alias, pelet::TokenClass::TokenIds visibility) {
}

void mvceditor::CallStackClass::TraitInsteadOfFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& traitUsedClassName,
													   const UnicodeString& traitMethodName, const std::vector<UnicodeString>& insteadOfList) {
}

void mvceditor::CallStackClass::TraitUseFound(const UnicodeString& namespaceName, const UnicodeString& className, 
												const UnicodeString& fullyQualifiedTraitName) {

}

void mvceditor::CallStackClass::MethodEnd(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName, int pos) {
}

void mvceditor::CallStackClass::MethodFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName, const UnicodeString& signature, 
											const UnicodeString& returnType, const UnicodeString& comment, pelet::TokenClass::TokenIds visibility, 
											bool isStatic, const int lineNumber) {
	CurrentClass = className;
	CurrentMethod = methodName;
	CurrentFunction.remove();
}

void mvceditor::CallStackClass::PropertyFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& propertyName, const UnicodeString& propertyType,
	const UnicodeString& comment, pelet::TokenClass::TokenIds visibility, bool isConst, bool isStatic, const int lineNumber) {
	
	// no need to do anything here as we only want to look at expressions
}

void mvceditor::CallStackClass::FunctionFound(const UnicodeString& namespaceName, const UnicodeString& functionName, const UnicodeString& signature, const UnicodeString& returnType, 
		const UnicodeString& comment, const int lineNumber) {
	CurrentClass.remove();
	CurrentMethod.remove();
	CurrentFunction = functionName;
}

void mvceditor::CallStackClass::FunctionEnd(const UnicodeString& namespaceName, const UnicodeString& functionName, int pos) {

	// no need to do anything here as we only want to look at expressions	
}

void mvceditor::CallStackClass::ClassFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& signature, 
										   const UnicodeString& comment, const int lineNumber) {

	// no need to do anything here as we only want to look at expressions		
}

void mvceditor::CallStackClass::IncludeFound(const UnicodeString& includeFile, const int lineNumber) {

	// no need to do anything here as we only want to look at expressions		
}

void mvceditor::CallStackClass::DefineDeclarationFound(const UnicodeString& variableName, const UnicodeString& variableValue, 
		const UnicodeString& comment, const int lineNumber) {

	// no need to do anything here as we only want to look at expressions			
}

void mvceditor::CallStackClass::VariableFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName, const pelet::SymbolClass& symbol, 
	const UnicodeString& comment) {
	
	// no need to do anything here as we only want to look at expressions		
}