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
#ifndef __parserclass__
#define __parserclass__

#include <language/LexicalAnalyzerClass.h>
#include <language/TokenClass.h>
#include <language/ParserObserverClass.h>
#include <wx/string.h>
#include <unicode/unistr.h>

namespace mvceditor {

/**
 * The parser class is designed in a way that can utilized by different pieces of code.  The parser will analyze
 * given code and make calls to the different registered observers.  There are observers for classes, functions, and 
 * methods. Not all observers have to be set; for example if a FunctionObserverClass is never registered then the 
 * parser will not notify when a function has been found in a piece of code.
 * 
 * @code
 *   class EchoAndObserverClass : public ClassObserverClass {
 * 
 *     virtual void ClassFound(const UnicodeString& className, const UnicodeString& signature, 
 *          const UnicodeString& comment) {
 *       printf("Found Class %s\n", (const char*)className.ToUTF8());
 *     }
 *   }
 * 
 *   EchoAndObserverClass echoObserver;
 *   ParserClass parser;
 *   parser.SetClassObserver(&echoObserver);
 *   wxString someFileName = wxT("/some/file.php");
 *   if (!parser.ScanFile(someFileName)) {
 *     puts("Could not find file to parse!");
 *   }
 * @endcode
 * 
 * Observers follow the PHP parsing rules to the letter.  If source code is not valid; then observers may not
 * get called.
 *
 * Lint functionality
 * 
 * The parser class has the ability to check PHP code for syntax errors. This is done via the Lint() method.
 * 
 * @code
 *   ParserClass parser;
 *   wxString file = wxT("/path/to/phpfile.php");
 *   LintResultsClass lintResults;
 *   if (parser.LintFile(file, parserResults)) {
 *     printf("No syntax errors in file %s", (const char*)file.ToAscii());
 *   }
 *   else {
 *     printf("%s. Error found in file %s on line %d.\n", parserResults.Error, (const char*)file.ToAscii(), parserResults.LineNumber);
 *   }
 * @encode
 */

/**
 * Holds the results of the lint check.  Currently lint check will stop when 
 * the first error is encountered.
 */
class LintResultsClass {
public:

	/**
	 * A short description of the error; this is generated by the bison parser and is
	 * not the most user-friendly but it is exactly what PHP displays; might as well
	 * keep it consistant.
	 */
	UnicodeString Error;

	/**
	 * Path to the file in which the error ocurred. 
	 * This is what was given to the LintFile() method.
	 * For LintString() results this will be the empty string.
	 */
	wxString File;

	/**
	 * The line in which the error ocurred. This is 1-based.
	 */
	int LineNumber;

	/**
	 * The character offset in which the error ocurred (with regargs to the start of
	 * the file). This is 0-based.
	 */
	int CharacterPosition;

	LintResultsClass();

	/**
	 * copy the attributes from src to this object.
	 */
	void Copy(const LintResultsClass& src);
};

class ParserClass {


public:
	
	ParserClass();
	
	/**
	 * Opens and scans the given file; This function will return once the entire
	 * file has been parsed; it will call the proper observers when it encounters
	 * a class, function, or variable declaration. This means that this
	 * parser should not be modified in the observer calls.
	 * 
	 * @param const wxString& file the file to parse.  Must be a full path.
	 * @return bool if file was found.
	 */
	bool ScanFile(const wxString& file);
	
	/**
	 * Scans the given string. This function will return once the entire
	 * string has been parsed; it will call the proper observers when it encounters
	 * a class, function, or variable declaration. This means that this
	 * parser should not be modified in the observer calls.
	 * 
	 * @param const UnicodeString& code the code to parse.
	 * @return bool alway true for now, just to give this method symmetry with ParseFile() method.
	 */
	bool ScanString(const UnicodeString& code);
	
	/**
	 * Set the class observer.  The observer will get notified when a class is encountered.
	 * Memory management of this pointer should be done by the caller.
	 * 
	 * @param ClassObserverClass* observer the object to sent notifications to 
	 */
	void SetClassObserver(ClassObserverClass* observer);
	
	/**
	 * Set the class member observer.  The observer will get notified when a class member is encountered.
	 * Memory management of this pointer should be done by the caller.
	 * 
	 * @param ClassMemberObserverClass* observer the object to sent notifications to 
	 */
	void SetClassMemberObserver(ClassMemberObserverClass* observer);
	
	/**
	 * Set the function observer.  The observer will get notified when a function is encountered.
	 * Memory management of this pointer should be done by the caller.
	 * 
	 * @param FunctionObserverClass* observer the object to sent notifications to 
	 */
	void SetFunctionObserver(FunctionObserverClass* observer);
	
	/**
	 * Set the variable observer.  The observer will get notified when a new variable has been created.
	 * Memory management of this pointer should be done by the caller.
	 * 
	 * @param VariableObserverClass* observer the object to sent notifications to 
	 */
	void SetVariableObserver(VariableObserverClass* observer);
	
	/**
	 * Perform a TRUE PHP syntax check on the entire file. This syntax check is based on PHP 5.3.
	 * Note that this is not entirely the same as 'php -l' command; the PHP lint command detects 
	 * duplicate function  / class names where as this lint check method does not.
	 *
	 * Returns true if the file had no syntax errors. Note that a file that does not have
	 * any PHP code will be considered a good file (a PHP file that has only HTML is
	 * considered good and true will be returned).
	 * 
	 * @param const wxString& file the file to parse.  Must be a full path.
	 * @param LintResultsClass& results any error message will be populated here
	 * @return bool true if file was found and had no syntax errors.
	 */
	bool LintFile(const wxString& file, LintResultsClass& results);
	
	/**
	 * Perform a syntax check on the given source code. Source code is assumed to be
	 * all code (HTML will not be skipped, and will result in syntax errors). The PHP 
	 * open tag is optional.
	 * Returns true if the code had no syntax errors.
	 * 
	 * @param const UnicodeString& code the actual code to parse.
	 * @param LintResultsClass& results any error message will be populated here
	 * @return bool true if the code has no syntax errors.
	 */
	bool LintString(const UnicodeString& code, LintResultsClass& results);

	/**
	 * @return the character position where the parser is currently parsing. This can be called
	 * inside an observer callback; in which case the character position is right PAST the
	 * current token.
	 */
	int GetCharacterPosition() const;

	
	/**
	 * Parses a given PHP expression.  This method will parse the given expression into a list of
	 * of "chained" calls.
	 *
	 * A PHP expression is  
	 *  - a variable  ($obj)
	 *  - a function call (myFunc())
	 *  - an object operation ("$obj->prop")
	 *  - a static object operation ("MyClass::Prop")
	 * 
	 * Object operations can be chained; like "$obj->prop->anotherFunc()". While indirect variables are allowed
	 * in PHP (ie $this->$prop)  this method will not handle them as it is nearly impossible to resolve them at parse time.
	 *
	 * The most extreme example is this expression: "$obj->prop->anotherFunc()"
	 * This method will parse the expression into
	 * $obj
	 * ->prop
	 * ->anotherFunc()

	 * For example, if sourceCode represented this string:
	 * 
	 *   @code
	 *     UnicodeString sourceCode = UNICODE_STRING_SIMPLE("
	 *       class UserClass {
	 *         private $name;
	 * 
	 *         function getName() {
	 *           return $this->
	 *     ");
	 *   @endcode
	 *  then the following C++ code can be used to find a variable's type
	 * 
	 *   @code
	 *     ParserClass parser;
	 *     UnicodeString expression = UNICODE_STRING_SIMPLE("$this->");
	 *     mvceditor::SymbolClass exprResult;
	 *     if (parser.ParseExpression(expression, exprResult)) {
	 *     	// if successful, symbol.Lexeme will be set to "$this"
	 *     }
	 *   @endcode
	 * 
	 * 
	 * @param expression the code string of the expression to resolve. This must be the code for a single expression.
	 *        Examples:
	 *        $anObject
	 *        $this->prop
	 *        $this->work()->another
	 *        $this->
	 *		  work()->another
	 *		  work()
	 *        self::prop
	 *        self::prop::
	 *        self::func()->prop
	 *        parent::prop
	 *        parent::fun()->prop
	 *        aFunction
	 *        An expression can have whitespace like this
	 *        $anObject
	 *			->method1()
	 *			->method2()
	 *			->method3()
	 *
	 * A special case that happens when the given expression ends with the object operator:
	 *        $this->
	 *        MyClass::
	 * In this case, the operator will be added the chain list; this way the client code can determine that
	 * the variable name actually ended.
	 * @param symbol the expression's name and "chain" list. The  properties of this object will be reset every call.
	 */
	void ParseExpression(UnicodeString expression, SymbolClass& symbol);
	
private:

	/**
	 * Clean up any resources after parsing a file. This is also very important if the 
	 * parser opens a string; without closing the string will not be released (if it's a
	 * long string).
	 */
	void Close();

	/**
	 * Used to tokenize code
	 * 
	 * @var LexicalAnalyzerClass
	 */
	LexicalAnalyzerClass Lexer;
	
	/**
	 * Notify the ClassObserver when a class has been found. Memory management of this pointer should be
	 * done by the caller.
	 * 
	 * @var ClassObserverClass*
	 */
	ClassObserverClass* ClassObserver;

	/**
	 * Notify the ClassMemberObserver when a class member has been found. Memory management of this pointer should be
	 * done by the caller.
	 * 
	 * @var ClassMemberObserverClass*
	 */
	ClassMemberObserverClass* ClassMemberObserver;
	
	/**
	 * Notify the FunctionObserver when a function has been found. Memory management of this pointer should be
	 * done by the caller.
	 * 
	 * @var ClassObserverClass*
	 */	
	FunctionObserverClass* FunctionObserver;
	
	/**
	 * Notify the VariableObserver when a variable has been created. Memory management of this pointer should be
	 * done by the caller.
	 * 
	 * @var VariableObserverClass*
	 */		
	VariableObserverClass* VariableObserver;
};



}
#endif // __parserclass__
