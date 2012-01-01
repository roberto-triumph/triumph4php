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
#include <language/ParserClass.h>
#include <language/TokenClass.h>
#include <stack>

/* 
 * generated by Bison according to the prefix of our choosing
 * Bison won't put this declaration in the header file so we must
 * manually make it available here by using "extern"
 */
extern int php53parse(mvceditor::LexicalAnalyzerClass &analyzer, mvceditor::ObserverQuadClass& observers);

mvceditor::ParserClass::ParserClass()
	: Lexer()
	, ClassObserver(0)
	, ClassMemberObserver(0)
	, FunctionObserver(0)
	, VariableObserver(0) {
}

bool mvceditor::ParserClass::ScanFile(const wxString& file) {
	bool ret = false;
	if (Lexer.OpenFile(file)) {
		
		// set to 1 and the parser will dump stuff to std out. useful for debugging only
		extern int php53debug;
		php53debug = 0;

		mvceditor::ObserverQuadClass observers(ClassObserver, ClassMemberObserver, FunctionObserver, VariableObserver, NULL);
		ret = php53parse(Lexer, observers) == 0;
		Close();
	}
	return ret;
}

bool mvceditor::ParserClass::ScanString(const UnicodeString& code) {
	bool ret = false;
	if (Lexer.OpenString(code)) {
		mvceditor::ObserverQuadClass observers(ClassObserver, ClassMemberObserver, FunctionObserver, VariableObserver, NULL);
		ret = php53parse(Lexer, observers) == 0;
		Close();
	}
	return ret;
}

void mvceditor::ParserClass::SetClassMemberObserver(ClassMemberObserverClass* observer) {
	ClassMemberObserver = observer;
}

void mvceditor::ParserClass::SetClassObserver(ClassObserverClass* observer) {
	ClassObserver = observer;
}

void mvceditor::ParserClass::SetFunctionObserver(FunctionObserverClass* observer) {
	FunctionObserver = observer;
}

void mvceditor::ParserClass::SetVariableObserver(VariableObserverClass* observer) {
	VariableObserver = observer;
}


bool mvceditor::ParserClass::LintFile(const wxString& file, LintResultsClass& results) {

	// set to 1 and the parser will dump stuff to std out. useful for debugging only
	extern int php53debug;
	php53debug = 0;
	
	bool ret = false;
	if (Lexer.OpenFile(file)) {
		mvceditor::ObserverQuadClass observers(NULL, NULL, NULL, NULL, NULL);
		ret = php53parse(Lexer, observers) == 0;
		results.Error = Lexer.ParserError;
		results.File = file;
		results.LineNumber = Lexer.GetLineNumber();
		results.CharacterPosition = Lexer.GetCharacterPosition();
		Lexer.Close();
	}
	return ret;
}

bool mvceditor::ParserClass::LintString(const UnicodeString& code, LintResultsClass& results) {
	bool ret = false;
	if (Lexer.OpenString(code)) {
		mvceditor::ObserverQuadClass observers(NULL, NULL, NULL, NULL, NULL);
		ret = php53parse(Lexer, observers) == 0;
		results.Error = Lexer.ParserError;
		results.File = wxEmptyString;
		results.LineNumber = Lexer.GetLineNumber();
		results.CharacterPosition = Lexer.GetCharacterPosition();
		Lexer.Close();
	}
	return ret;
}

int mvceditor::ParserClass::GetCharacterPosition() const {
	return Lexer.GetCharacterPosition();
}

void mvceditor::ParserClass::Close() {
	Lexer.Close();
}


namespace mvceditor {

class ParserVariableObserverClass : public ExpressionObserverClass {

public:

	std::vector<ExpressionClass> Expressions;
	
	virtual void ExpressionFound(const ExpressionClass& expression) {
		Expressions.push_back(expression);
	}
};
}

void mvceditor::ParserClass::ParseExpression(UnicodeString expression, mvceditor::SymbolClass& symbol) {
	expression.trim();

	// remove the operators if they are at the end; this prevents parse errors
	bool endsWithObject = false;
	bool endsWithStatic = false;
	if (expression.endsWith(UNICODE_STRING_SIMPLE("->"))) {
		expression.remove(expression.length() - 2, 2);
		endsWithObject = true;
	}
	if (expression.endsWith(UNICODE_STRING_SIMPLE("::"))) {
		expression.remove(expression.length() - 2, 2);
		endsWithStatic = true;
	}

	// make it so that the expression observer is always called by terminating the expression
	if (!expression.endsWith(UNICODE_STRING_SIMPLE(";"))) {
		expression += UNICODE_STRING_SIMPLE(";");
	}
	symbol.Clear();

	mvceditor::ParserVariableObserverClass localObserver;
	mvceditor::ObserverQuadClass observers(NULL, NULL, NULL, NULL, &localObserver);

	// parse the given expression code snippet
	// most of the time, the variable observer will NOT be called because 
	// the variable observer is only called for assignment expressions
	// but we want this method to be able to parse a single variable
	if (Lexer.OpenString(expression)) {	
  		php53parse(Lexer, observers);
		Lexer.Close();
	}
	if (!localObserver.Expressions.empty()) {
		mvceditor::ExpressionClass expr = localObserver.Expressions.back();
		if (mvceditor::ExpressionClass::ARRAY == expr.Type) {
			symbol.SetToArray();
		}
		else if (mvceditor::ExpressionClass::FUNCTION_CALL == expr.Type) {
			symbol.SetToObject();
		}
		else if (mvceditor::ExpressionClass::NEW_CALL == expr.Type) {
			symbol.SetToObject();
		}
		else if (mvceditor::ExpressionClass::SCALAR == expr.Type) {
			symbol.SetToPrimitive();
		}
		else if (mvceditor::ExpressionClass::UNKNOWN == expr.Type) {
			symbol.SetToUnknown();
		}
		else if (mvceditor::ExpressionClass::VARIABLE == expr.Type) {
			symbol.SetToObject();
		}
		symbol.Comment = expr.Comment;
		symbol.Lexeme = expr.Lexeme;
		symbol.ChainList = expr.ChainList;
		if (expr.Lexeme.isEmpty() && expr.ChainList.empty()) {
			
			 // when a static property; the "namespace_name" parser rule 
			 // is triggered
			symbol.Lexeme = expr.Name.ToSignature();
			symbol.ChainList.insert(symbol.ChainList.begin(), symbol.Lexeme);
		}
	}
	if (endsWithObject) {
		symbol.ChainList.push_back(UNICODE_STRING_SIMPLE("->"));
	}
	if (endsWithStatic) {
		symbol.ChainList.push_back(UNICODE_STRING_SIMPLE("::"));
	}
}

mvceditor::LintResultsClass::LintResultsClass()
	: Error()
	, File()
	, LineNumber(0)
	, CharacterPosition(0) {
}

void mvceditor::LintResultsClass::Copy(const mvceditor::LintResultsClass& other) {
	Error = other.Error;
	File = other.File;
	LineNumber = other.LineNumber;
	CharacterPosition = other.CharacterPosition;
}