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
 * we will stop tracking errors after we have reached this
 * many.  The user cannot possibly go through all 
 * of them
 */
const static size_t MAX_ERRORS = 100;

/**
 * @return int the index to the start of the next variable in the signature
 *         if there are no more variables then the function returns -1
 *         the function correctly handles the case where the signature
 *         contains default values with '()' like for  example the 
 *         date signature
 *         
 *         date($format, [ $timestamp = now() ])
 *//*
static int NextVariableStart(const UnicodeString& signature, int32_t start) {
	int braceDepth = 0;
	int32_t i = start;
	for (; i < signature.length(); ++i) {
		if (signature[i] == '(') {
			braceDepth++;
		}
		if (signature[i] == ')') {
			braceDepth--;
		}
		
		if (braceDepth < 0) {
			i++;
			break;
		}
		if (signature[i] == ',') {
			i++;
			break;
		}
	}
	return i == signature.length() ? -1 : i;
}
*/

/**
 * @return int the index to the end of the current variable in the signature
 *         if there are no more variables then the function returns the index of
 *         the ending close parenthesis
 *         the function correctly handles the case where the signature
 *         contains default values with '()' like for  example the 
 *         date signature
 *         
 *         date($format, [ $timestamp = now() ])
 */
static int VariableEnd(const UnicodeString& signature, int32_t start) {
	int braceDepth = 0;
	int32_t i = start;
	for (; i < signature.length(); ++i) {
		if (signature[i] == '(') {
			braceDepth++;
		}
		if (signature[i] == ')') {
			braceDepth--;
		}
		
		if (signature[i] == ',') {
			i++;
			break;
		}
		if (signature[i] == ')' && braceDepth < 0) {
			
			// the end of the signature
			break;
		}
	}
	return i;
}

/**
 * @param the function signature to parse
 * @param [out] the number of required arguments in the signature
 * @param [out] the number of total arguments (required + optional) in the signature
 */
static void CountArgs(const UnicodeString& signature, int& required, int& total) {
	required = 0;
	total = 0;
	
 	int32_t start = signature.indexOf('(');
	int32_t next = VariableEnd(signature, start + 1);
	if (next > (start + 1)) {
		
		// here means we have at least 1 argument watch out for "string function doIt()"
		while (start >= 0 && start < signature.length()) {
				
			// this is the argument that we want to 
			// check for default arguments (they will have an '='
			// in the signature)
			// also, native functions define optional arguments (ie
			// not necessary but no default value) with square
			// brackets "[]"
			UnicodeString variable;
			signature.extract(start, (next - start + 1), variable);
			if (variable.indexOf('=') < 0 && variable.indexOf('[') < 0) {
				required++;
			}
			total++;
			
			// find the end of the next variable
			start = next + 1;
			next = VariableEnd(signature, start);
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
		if (!signature.isEmpty() && !hasVariableArgs) {
			CountArgs(signature, signatureRequiredArgCount, signatureTotalArgCount);
			found = true;
		}
	}
	else if (nativeFunctionLookup.ReExec(error) && nativeFunctionLookup.Found()) {
				
		// some native functions have more than 1 signature we look at
		// all of the signatures.
		// since we are looking ar multiple signature, the required 
		// args is the least number of required args, the 
		// total args is the most number of total args
		//
		signatureRequiredArgCount = -1;   // -1 so that zero can be set 
		signatureTotalArgCount = 0;      // as a req/total var count
		while (nativeFunctionLookup.More()) {
			nativeFunctionLookup.Next();
			
			signature = nativeFunctionLookup.Signature;
			hasVariableArgs = nativeFunctionLookup.HasVariableArgs;
			if (!hasVariableArgs && !signature.isEmpty()) {
				found = true;
				int required = 0;
				int total = 0;
				CountArgs(signature, required, total);
				if (signatureRequiredArgCount == -1 || required < signatureRequiredArgCount) {
					signatureRequiredArgCount = required;
				}
				if (signatureTotalArgCount < total) { 
					signatureTotalArgCount = total;
				}
			}
		}
		
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
 * @param methodLookup the prepared statement used to lookup the method
 * @param nativeMethodLookup the prepared statement used to lookup the method in the
 *        native tags
 * @return bool TRUE if all of the following are true:
 *         method exists only once
 *         method has known number of arguments (ie not variable arguments like sprintf)
 */
static bool LookupMethod(const UnicodeString& name, bool isStatic, int& signatureRequiredArgCount, 
		int& signatureTotalArgCount,
		t4p::MethodSignatureLookupClass& methodLookup,
		t4p::MethodSignatureLookupClass& nativeMethodLookup) {
	bool found = false;
	wxString error;
	UnicodeString signature;
	bool hasVariableArgs = false;
	int matchCount = 0;
	
	methodLookup.Set(name, isStatic);
	nativeMethodLookup.Set(name, isStatic);
	if (methodLookup.ReExec(error) && methodLookup.Found()) {
		methodLookup.Next();
		matchCount++;
		
		// we only return true if there is 1 and only 1 match
		if (!methodLookup.More()) {
			signature = methodLookup.Signature;
			hasVariableArgs = methodLookup.HasVariableArgs;
		}
	}
	if (nativeMethodLookup.ReExec(error) && nativeMethodLookup.Found()) {
		matchCount++;
		
		// we only return true if there is 1 and only 1 match
		if (!nativeMethodLookup.More()) {
			matchCount++;
			signature = nativeMethodLookup.Signature;
			hasVariableArgs = nativeMethodLookup.HasVariableArgs;
		}
	}
	
	if (!signature.isEmpty() && !hasVariableArgs) {
		CountArgs(signature, signatureRequiredArgCount, signatureTotalArgCount);
		found = true;
	}
	
	if (matchCount > 1) {
		
		// we found the same method name in the global tag and the native
		// tags.  we don't know which signature to choose, so lets ignore
		// for now.
		found = false;
		signatureRequiredArgCount = 0;
		signatureTotalArgCount = 0;
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
, NativeFunctionSignatureLookup() 
, NativeMethodSignatureLookup() {
	Parser.SetExpressionObserver(this);
}


void t4p::PhpFunctionCallLintClass::Init(t4p::TagCacheClass& tagCache) {
	tagCache.GlobalPrepare(FunctionSignatureLookup, true);
	tagCache.GlobalPrepare(MethodSignatureLookup, true);
	tagCache.NativePrepare(NativeFunctionSignatureLookup, true);
	tagCache.NativePrepare(NativeMethodSignatureLookup, true);
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
	if (Errors.size() > MAX_ERRORS) {
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
				// note that calls like parent::method() and self::method()
				// are not static method calls
				isStatic = isStatic 
						&& var->ChainList[0].Name.caseCompare(UNICODE_STRING_SIMPLE("parent"), 0) != 0
						&& var->ChainList[0].Name.caseCompare(UNICODE_STRING_SIMPLE("self"), 0) != 0;
				
				found = LookupMethod(functionName, isStatic, 
					signatureRequiredArgCount, signatureTotalArgCount,
					MethodSignatureLookup, NativeMethodSignatureLookup);
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
