/**
 * @copyright  2014 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
 */
#ifndef SRC_LANGUAGE_PHP_PHPFUNCTIONCALLLINTCLASS_H_
#define SRC_LANGUAGE_PHP_PHPFUNCTIONCALLLINTCLASS_H_

#include <pelet/ParserClass.h>
#include <unicode/unistr.h>
#include <vector>
#include "language_php/ParsedTagFinderClass.h"

namespace t4p {
// forward declaration, defined in another file
class TagCacheClass;

/**
 * name and position of an the function call that is
 * in error because of too many / too few arguments
 */
class PhpFunctionCallLintResultClass {
 public:
    enum Types {
        NONE,
        TOO_MANY_ARGS,
        TOO_FEW_ARGS
    };

    /**
     * the function that is in error
     */
    UnicodeString Identifier;

    /**
     * the file that the error was found in
     */
    UnicodeString File;

    /**
     * the line number (1- based) the uninitialized variable was found in
     */
    int LineNumber;

    /**
     * the character offset (0-based) in the file where the uninitialized variable was
     * found at
     */
    int Pos;

    Types Type;

    /**
     * the number of arguments that the called function definition has
     */
    int ExpectedCount;

    /**
     * the number of arguments that the function call has. Will never
     * be equal to ExpectedCount
     */
    int ActualCount;

    PhpFunctionCallLintResultClass();

    PhpFunctionCallLintResultClass(const t4p::PhpFunctionCallLintResultClass& src);

    t4p::PhpFunctionCallLintResultClass& operator=(const t4p::PhpFunctionCallLintResultClass& src);

    void Copy(const t4p::PhpFunctionCallLintResultClass& src);
};

/**
 * The PhpFunctionCallLintClass will check that each function
 * or method that is called has the exact number of arguments
 * as the function/method signature containts. Logic:
 *
 * 1. If the function has variable arguments, then the check is skipped.
 *    Variable arguments are detected by the parser; if the
 *    function body has calls to `func_get_args` and friends
 * 2. function signature with default values are accounted for
 * 3. if the function signature is not found in the tag cache
 *    then the check is skipped.
 * 4. if the function signature is not found could not be determined (not
 *    unique) then the check is skipped.
 */
class PhpFunctionCallLintClass : public pelet::AnyExpressionObserverClass {
 public:
    PhpFunctionCallLintClass();

    /**
     * prepares the internal cache used for function lookups. This needs
     * to be called before any file is checked.
     * the tag cache is used to lookup function signatures in order to
     * check if the argument count
     *
     * @param tagCache the tag dbs
     */
    void Init(t4p::TagCacheClass& tagCache);

    /**
     * Set the version that the PHP parser should use. This method should be
     * called BEFORE parsing a file / string
     */
    void SetVersion(pelet::Versions version);

    /**
     * Init() method should be called before a file is parsed. Note that
     * if a file has many errors, this class will stop adding
     * errors after a certain amount.
     *
     * @param fileName the file to parse and report errors on.
     * @param errors any argument count mismatch errors will be
     *        appended to this parameter.
     * @return bool TRUE if there is at least one error
     */
    bool ParseFile(const wxFileName& fileName, std::vector<t4p::PhpFunctionCallLintResultClass>& errors);

    /**
     * Init() method should be called before a string is parsed. Note that
     * if a string has many errors, this class will stop adding
     * errors after a certain amount.
     *
     * @param code the string to parse and report errors on
     * @param errors any uninitialized variable errors will be
     *        appended to this parameter.
     * @return bool TRUE if there is at least one error
     */
    bool ParseString(const UnicodeString& code, std::vector<t4p::PhpFunctionCallLintResultClass>& errors);

    /**
     * @param var the expression to check. A Check  will be
     *       done to see if any of the function calls used in the
     *       given expression have the correct number of
     *       arguments. if not, then a new error is
     *       created and appended to the Errors vector.
     */
    void OnAnyExpression(pelet::ExpressionClass* expr);

 private:
    /**
     * the parser will parse the PHP code and call the *Found() methods
     * when a variable, expression, etc.. is found
     */
    pelet::ParserClass Parser;

    /**
     * the file being currently parsed. empty string if a string of
     * code is being parsed
     */
    UnicodeString File;

    /**
     * errors will be pushed into this vector while the code
     * is being parsed
     */
    std::vector<t4p::PhpFunctionCallLintResultClass> Errors;

    /**
     * used to query the tag cache for function / method
     * signatures. This allows us to prepare the query only once
     * and execute it many times, where as if we use tag cache helper
     * methods they prepare the queries each time that they are called
     */
    t4p::FunctionSignatureLookupClass FunctionSignatureLookup;
    t4p::MethodSignatureLookupClass MethodSignatureLookup;
    t4p::FunctionSignatureLookupClass NativeFunctionSignatureLookup;
    t4p::MethodSignatureLookupClass NativeMethodSignatureLookup;
};
}  // namespace t4p

#endif  // SRC_LANGUAGE_PHP_PHPFUNCTIONCALLLINTCLASS_H_
