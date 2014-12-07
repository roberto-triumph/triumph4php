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
 * @copyright  2014 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <language/PhpFunctionCallLintClass.h>
#include <language/TagCacheClass.h>
#include <wx/ffile.h>

/**
 * @param the function signature to parse
 * @param [out] the number of required arguments in the signature
 * @param [out] the number of total arguments (required + optional) in the signature
 */
static void CountArgs(const UnicodeString& signature, int& required, int& total) {
	required = 0;
	total = 0;
	int32_t start = signature.indexOf('(');
	int32_t next = signature.indexOf(',', start);
	if (next < 0) {
			
		// only 1 argument, look for the ending )
		next = signature.indexOf(')', next + 1);
	}
	while (next > 0 && next > (start + 1)) {
			
		// this is the argument that we want to 
		// check for default arguments (they will have an '='
		// in the signature)
		UnicodeString variable;
		signature.extract(start, (next - start + 1), variable);
		if (variable.indexOf('=') < 0) {
			required++;
		}
		total++;
		
		// find the end of the next variable
		start = next;
		next = signature.indexOf(',', start + 1);
		if (next < 0) {
			next = start;
			
			// last argument, look for the ending )
			next = signature.indexOf(')', next + 1);
		}
	}
}

/**
 * lookup a function in the tag cache. if the function is found, determine the number
 * of arguments in the function signature. The function is looked up in both
 * the native tags and the global tags.
 * 
 * @param name the function to look up. fully qualified name
 * @param [out] signatureRequiredArgCount the number of arguments (that don't have a default value)
 *              will be set in this variable if the function is found.
 * @param [out] signatureTotalArgCount the total number of arguments (required + optional)
 *              will be set in this variable if the function is found.
 * @param functionLookup the prepared statement used to lookup the function
 * @param nativeFunctionLookup the prepared statement used to lookup the function
 *        in the native tags
 * @return bool TRUE if all of the following are true:
 *         function exists
 *         there is only 1 function with this name
 *         function has known number of arguments (ie not variable arguments like sprintf)
 */
static bool LookupFunction(const UnicodeString& name, int& signatureRequiredArgCount, 
		int& signatureTotalArgCount,
		t4p::FunctionSignatureLookupClass& functionLookup,
		t4p::FunctionSignatureLookupClass& nativeFunctionLookup) {
	bool found = false;
	wxString error;
	UnicodeString signature;
	bool hasVariableArgs = false;
	
	functionLookup.Set(name);
	nativeFunctionLookup.Set(name);
	if (functionLookup.ReExec(error) && functionLookup.Found()) {
		functionLookup.Next();
		
		// we only return true if there is 1 and only 1 match
		if (!functionLookup.More()) {
			signature = functionLookup.Signature;
			hasVariableArgs = functionLookup.HasVariableArgs;
		}
	}
	else if (nativeFunctionLookup.ReExec(error) && nativeFunctionLookup.Found()) {
		nativeFunctionLookup.Next();
		
		// we only return true if there is 1 and only 1 match
		if (!nativeFunctionLookup.More()) {		
			signature = nativeFunctionLookup.Signature;
			hasVariableArgs = nativeFunctionLookup.HasVariableArgs;
		}
	}
	
	if (!signature.isEmpty() && !hasVariableArgs) {
		CountArgs(signature, signatureRequiredArgCount, signatureTotalArgCount);
		found = true;
	}
	return found;
}

/**
 * lookup a method in the tag cache. if the method is found, determine the number
 * of arguments in the method signature. The method is looked up in 
 * the global tags.
 * 
 * @param name the method to look up. name of the method only
 * @param [out] signatureRequiredArgCount the number of arguments (that don't have a default value)
 *              will be set in this variable if the function is found.
 * @param [out] signatureTotalArgCount the total number of arguments (required + optional)
 *              will be set in this variable if the function is found.
 * @param methodLookup the prepared statement used to lookup the function
 * @return bool TRUE if all of the following are true:
 *         method exists only once
 *         method has known number of arguments (ie not variable arguments like sprintf)
 */
static bool LookupMethod(const UnicodeString& name, bool isStatic, int& signatureRequiredArgCount, 
		int& signatureTotalArgCount,
		t4p::MethodSignatureLookupClass& methodLookup) {
	bool found = false;
	wxString error;
	UnicodeString signature;
	bool hasVariableArgs = false;
	
	methodLookup.Set(name, isStatic);
	if (methodLookup.ReExec(error) && methodLookup.Found()) {
		methodLookup.Next();
		
		// we only return true if there is 1 and only 1 match
		if (!methodLookup.More()) {		
			signature = methodLookup.Signature;
			hasVariableArgs = methodLookup.HasVariableArgs;
		}
	}
	
	if (!signature.isEmpty() && !hasVariableArgs) {
		 CountArgs(signature, signatureRequiredArgCount, signatureTotalArgCount);
		found = true;
	}
	return found;
}

t4p::PhpFunctionCallLintResultClass::PhpFunctionCallLintResultClass() 
: Identifier()
, File()
, LineNumber(0)
, Pos(0)
, Type(t4p::PhpFunctionCallLintResultClass::NONE)
, ExpectedCount(0)
, ActualCount(0) {
}

t4p::PhpFunctionCallLintResultClass::PhpFunctionCallLintResultClass(const t4p::PhpFunctionCallLintResultClass& src) 
: Identifier()
, File()
, LineNumber(0)
, Pos(0)
, Type(t4p::PhpFunctionCallLintResultClass::NONE)
, ExpectedCount(0)
, ActualCount(0) {
	Copy(src);
}

t4p::PhpFunctionCallLintResultClass& t4p::PhpFunctionCallLintResultClass::operator=(
	const t4p::PhpFunctionCallLintResultClass& src) {
	Copy(src);
	return *this;
}

void t4p::PhpFunctionCallLintResultClass::Copy(const t4p::PhpFunctionCallLintResultClass& src) {
	Identifier = src.Identifier;
	File = src.File;
	LineNumber = src.LineNumber;
	Pos = src.Pos;
	Type = src.Type;
	ExpectedCount = src.ExpectedCount;
	ActualCount = src.ActualCount;
}

t4p::PhpFunctionCallLintClass::PhpFunctionCallLintClass()
: AnyExpressionObserverClass()
, Parser()
, File()
, FunctionSignatureLookup()
, MethodSignatureLookup()
, NativeFunctionSignatureLookup() {
	Parser.SetExpressionObserver(this);
}


void t4p::PhpFunctionCallLintClass::Init(t4p::TagCacheClass& tagCache) {
	tagCache.GlobalPrepare(FunctionSignatureLookup, true);
	tagCache.GlobalPrepare(MethodSignatureLookup, true);
	tagCache.GlobalPrepare(NativeFunctionSignatureLookup, true);
}

void t4p::PhpFunctionCallLintClass::SetVersion(pelet::Versions version) {
	Parser.SetVersion(version);
}

bool t4p::PhpFunctionCallLintClass::ParseFile(const wxFileName& fileName, 
	std::vector<t4p::PhpFunctionCallLintResultClass>& errors) {
		
	Errors.clear();
	File.remove();
	
	File = t4p::WxToIcu(fileName.GetFullPath());
	pelet::LintResultsClass syntaxResults;
	wxFFile file;
	if (file.Open(fileName.GetFullPath(), "r")) {
		if (Parser.ScanFile(file.fp(), File, syntaxResults)) {
			errors = Errors;
		}
	}
	return !Errors.empty();
}

bool t4p::PhpFunctionCallLintClass::ParseString(const UnicodeString& code, 
	std::vector<t4p::PhpFunctionCallLintResultClass>& errors) {
	
	Errors.clear();
	File.remove();
	
	pelet::LintResultsClass syntaxResults;
	if (Parser.ScanString(code, syntaxResults)) {
		errors = Errors;
	}
	return !Errors.empty();
}


void t4p::PhpFunctionCallLintClass::OnAnyExpression(pelet::ExpressionClass* expr) {
	if (expr->ExpressionType != pelet::ExpressionClass::VARIABLE) {
		return;
	}
	
	// loop through each function call.  note that function calls
	// arguments could themselves be function calls, we must check these
	// also
	// luckily the base class already does this for us!
	pelet::VariableClass* var = (pelet::VariableClass*)expr;
	if (var->ChainList.empty()) {
		return;
	}
	
	for (size_t i = 0; i < var->ChainList.size(); ++i) {
		if (var->ChainList[i].IsFunction) {
			int signatureRequiredArgCount = 0;
			int signatureTotalArgCount = 0;
			int callArgCount = var->ChainList[i].CallArguments.size();
			bool found = false;
			UnicodeString functionName = var->ChainList[i].Name;
			bool isStatic = var->ChainList[i].IsStatic;
			if (0 == i) {
				
				// this is a function call
				found = LookupFunction(functionName, 
					signatureRequiredArgCount, signatureTotalArgCount,
					FunctionSignatureLookup, NativeFunctionSignatureLookup);
			}
			else {
				
				// a long variable calls, ie $this->user->getName()
				found = LookupMethod(functionName, isStatic, 
					signatureRequiredArgCount, signatureTotalArgCount,
					MethodSignatureLookup);
			}
			
			if ((found && callArgCount < signatureRequiredArgCount) || 
			    (found && callArgCount > signatureTotalArgCount)) {
				t4p::PhpFunctionCallLintResultClass error;
				error.File = File;
				error.LineNumber = expr->LineNumber;
				error.Pos = expr->Pos;
				error.ExpectedCount = signatureRequiredArgCount;
				error.ActualCount = callArgCount;
				error.Identifier = functionName;
				if (signatureTotalArgCount > callArgCount) {
					error.Type = t4p::PhpFunctionCallLintResultClass::TOO_FEW_ARGS;
				}
				else {
					error.Type = t4p::PhpFunctionCallLintResultClass::TOO_MANY_ARGS;
				}
				
				Errors.push_back(error);
			}
		}
	}
}
