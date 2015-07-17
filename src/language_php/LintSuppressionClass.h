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
#ifndef T4P_LINTSUPRESSIONCLASS_H
#define T4P_LINTSUPRESSIONCLASS_H

#include <unicode/unistr.h>
#include <wx/filename.h>
#include <vector>

namespace t4p {

/**
 * A suppression rule represents a single item that will be ignored
 * during lint checks. Each rule has a type, a target, and a 
 * location. The types are as follows:
 * 
 * SKIP_UNDEFINED_CLASS   - suppresses "unknown class" errors
 * SKIP_UNKNOWN_METHOD    - suppresses "unknown method" errors
 * SKIP_UNKNOWN_FUNCTION  - suppresses "unknown function" errors
 * SKIP_UNINITIALIZED_VAR - suppresses "uninitialized variable" errors
 * SKIP_ALL               - suppresses all errors
 * 
 * A supression target is the name of the item being suppressed,
 * for example to supress the class "Couchbase" then the target
 * is "Couchbase".  Targets are case-insensitve.
 *
 * A suppression location is the file or directory to suppress. 
 * When a directory is suppressed, the rule is applied recursively
 * to all sub-directories.
 */
class SuppressionRuleClass {
	
	public:

	enum Types {
		SKIP_UNKNOWN_CLASS,
		SKIP_UNKNOWN_METHOD,
		SKIP_UNKNOWN_PROPERTY,
		SKIP_UNKNOWN_FUNCTION,
		SKIP_UNINITIALIZED_VAR,
		SKIP_FUNCTION_ARGUMENT_MISMATCH,
		SKIP_ALL
	};
	
	/**
	 * the type of lint check to ignore
	 */
	Types Type;
	
	/**
	 * the name of the class / function / method
	 * being supressed. It will be empty when
	 * a rule is of type SKIP_ALL
	 */
	UnicodeString Target;
	
	/**
	 * The file or directory being suppressed.
	 */
	wxFileName Location;
	
	SuppressionRuleClass();
	
	SuppressionRuleClass(const t4p::SuppressionRuleClass& src);
	
	/**
	 * turn this rule into a skip-all rule 
	 * for the given file or directory
	 * 
	 * @param location to skip
	 */
	void SkipAllRule(const wxFileName& location);
	
	/**
	 * turn this rule into a skip unknown class rule 
	 * for the given file or directory
	 * 
	 * @param location to skip
	 * @param className fully qualified class name to ignore
	 */
	void SkipUnknownClassRule(const wxFileName& location, const UnicodeString& className);
	
	/**
	 * turn this rule into a skip-unknown method rule 
	 * for the given file or directory
	 * 
	 * @param location to skip
	 * @param methodName name of the method to ignore
	 */
	void SkipUnknownMethodRule(const wxFileName& location, const UnicodeString& methodName);
	
	/**
	 * turn this rule into a skip-all rule 
	 * for the given file or directory
	 * 
	 * @param location to skip
	 * @param functionName name of the function to ignore
	 */
	void SkipUnknownFunctionRule(const wxFileName& location, const UnicodeString& functionName);
	
	
	t4p::SuppressionRuleClass& operator=(const t4p::SuppressionRuleClass& src);
	
	void Copy(const t4p::SuppressionRuleClass& src);
};

/**
 * The lint suppression class describes items that a lint check
 * should disregard (ignore).  The user may want to ignore certain
 * checks for various reasons:
 * 
 * - some files are never modified by the user; the user
 *   uses them as libraries which means that the user doesn't
 *   care if the lint fails
 * - some classes are defined in a PHP extension that is not
 *   documented in PHP.net (for example, a PHP extension
 *   for couchbase). The user wants to supress these classes
 *   because they are defined, but not in the code so Triumph
 *   will tag them as unknown classes.
 * - clases may be dynamically created via a mocking mechanism
 *   (eval), Triumph does not find these classes and
 *   will tag them as unknown classes.
 */
class LintSuppressionClass {

public:

	/**
	 * the items to supress
	 */
	std::vector<t4p::SuppressionRuleClass> Rules;

	LintSuppressionClass();
	
	/**
	 * load suppressions from a file
	 * @return errors list of lines that did not contain well-formatted rules will be 
	 *         added to this vector
	 * @return bool TRUE if file can be read and opened
	 */
	bool Init(const wxFileName& suppressionFile, std::vector<UnicodeString>& errors);
	
	/**
	 * save suppressions to a file
	 * @return bool TRUE if file can be written to and opened
	 */
	bool Save(const wxFileName& suppressionFile);
	
	/**
	 * Adds the given suppression rule to the list
	 * 
	 * @param rule the rule to add
	 */
	void Add(const t4p::SuppressionRuleClass& rule);
	
	/**
	 * Will check to see if there is a suppression rule that matches
	 * the given file, target and type. Matching logic:
	 * 
	 * - Only suppressions of the given type are checked.
	 * - If the given file matches the rule file (or a rule directory) AND
	 *   the given target matches the rule target (or the rule is all rule)
	 *   then this method returns true. Target is checked in a case-insensitive manner.
	 *   If target is a class name, it must be a fully qualified class name.
	 * 
	 * @param file the file to check
	 * @param target the target to check
	 * @param  type the type to check
	 * @return bool TRUE if the given suppression should be
	 *         ignored
	 */
	bool ShouldIgnore(const wxFileName& file, const UnicodeString& target, 
		t4p::SuppressionRuleClass::Types type) const;
	
	/**
	 * loops through all of the rules and removes the rules whose location
	 * is EQUAL TO the given directory
	 * 
	 * @param dir the location to check
	 * @return bool TRUE if a rule was removed
	 */
	bool RemoveRulesForDirectory(const wxFileName& dir);
	
	/**
	 * Adds a SKIP_ALL rule for the given directory, but only
	 * if another SKIP_ALL rule for the directory does not yet
	 * exist
	 *
	 * @param dir the directory to check and add
	 * @return bool TRUE if a new rule was actually added
	 */
	bool AddSkipAllRuleForDirectory(const wxFileName& dir);
};

}

#endif // T4P_LINTSUPRESSIONCLASS_H
