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
#ifndef __T4P_LINTDIRECTORYWALKERCLASS_H__
#define __T4P_LINTDIRECTORYWALKERCLASS_H__ 

#include <search/DirectorySearchClass.h>
#include <language/PhpVariableLintClass.h>
#include <language/PhpIdentifierLintClass.h>
#include <language/PhpFunctionCallLintClass.h>
#include <language/LintSuppressionClass.h>

namespace t4p {
	
// forward declaration, defined in a different file
class TagCacheClass;

/**
 * Stores flags that determine how strict linting will be 
 */
class LintOptionsClass {

public:

	/**
	 * If TRUE, then when a file is saved; a lint check on that file
	 * will be performed.
	 */
	bool CheckOnSave;

	/**
	 * if TRUE, then a check for uninitialized variables is done
	 * it will done only on variables in a function or method.
	 */
	bool CheckUninitializedVariables;

	/**
	 * if TRUE, then methods, classes will be checked for existence
	 * when they are being called.  this requires that a project 
	 * be indexed.
	 */
	bool CheckUnknownIdentifiers;
	
	/**
	 * if TRUE, then a check is done to make sure that the arguments
	 * in a function/method call match the number of arguments of the 
	 * function/method declaration.  this requires that a project be indexed.
	 */
	bool CheckFunctionArgumentCount;

	/**
	 * if TRUE, then a check for uninitialized variables is done
	 * on global variables
	 */
	bool CheckGlobalScopeVariables;

	LintOptionsClass();

	LintOptionsClass(const t4p::LintOptionsClass& src);

	t4p::LintOptionsClass& operator=(const t4p::LintOptionsClass& src);

	void Copy(const t4p::LintOptionsClass& src);
};

/** 
 * This class will help in parsing the large project. It will enable access
 * to DirectorySearch and easily parse many files.
 */
class ParserDirectoryWalkerClass : public DirectoryWalkerClass {
public:

	ParserDirectoryWalkerClass(const t4p::LintOptionsClass& options, 
		const wxFileName& suppressionFile);
	
	/**
	 * initializes the linters  with the tag cache.  The linters use the
	 * tag cache to lookup classes, functions, methods.
	 */
	void Init(t4p::TagCacheClass& tagCache);
	
	/**
	 * @param bool to set the 'CheckUnknownIdentifiers' flag on the lint options
	 *        this method is useful to override the user's choice when we know
	 *        that perforning identifiers would result in many false positives;
	 *        for example when the user saves a file that exists outside of a 
	 *        project. Such files use classes/methods/functions that are not in the 
	 *        tag cache.
	 */
	void OverrideIdentifierCheck(bool doIdentifierCheck);
	
	/**
	 * This is the method where the parsing will take place. Will return true
	 * if and only if there is a parse error.  
	 */
	virtual bool Walk(const wxString& file);

	/**
	 * set the file counters (WithErrors, WitNoErrors) back to zero
	 */
	void ResetTotals();
	
	/**
	 * @param list of wildcard strings to perform lint checks on. Each item in the list 
	 *        is a wilcard suitable for passing into the wxMatchWild() function
	 * @param ignoreFilters regex string of files to ignore. This is a string suitable
	 *        for the FindInFilesClass::CreateFilesFilterRegEx() method.
	 */
	void SetFilters(std::vector<wxString> includeFilters, wxString ignoreFilters);
	
	/**
	 * Set the version of PHP to lint against
	 */
	void SetVersion(pelet::Versions version);
	
	/**
	 * The last parsing results; minus any errors that were suppressed due
	 * to the suppression rules.
	 */
	std::vector<pelet::LintResultsClass> GetLastErrors();

	/**
	 * Running count of files that had parse errors.
	 */
	int WithErrors;
	
	/**
	 * Running count of files that had zero parse errors.
	 */
	int WithNoErrors;
	
	/**
	 * Running count of files that were skipped because they
	 * are too big.
	 */
	int WithSkip;
	
private:

	// flags that control which checks to perform
	t4p::LintOptionsClass Options;

	// linters to perform different kinds of checks
	pelet::ParserClass Parser;
	t4p::PhpVariableLintOptionsClass VariableLinterOptions;
	t4p::PhpVariableLintClass VariableLinter;
	t4p::PhpIdentifierLintClass IdentifierLinter;
	t4p::PhpFunctionCallLintClass CallLinter;
	
	// to ignore files/classes/methods
	wxFileName SuppressionFile;
	t4p::LintSuppressionClass Suppressions;
	bool HasLoadedSuppressions;

	// the results for each linter
	pelet::LintResultsClass LastResults;
	std::vector<t4p::PhpVariableLintResultClass> VariableResults;
	std::vector<t4p::PhpIdentifierLintResultClass> IdentifierResults;
	std::vector<t4p::PhpFunctionCallLintResultClass> CallResults;
	
};

}

#endif
