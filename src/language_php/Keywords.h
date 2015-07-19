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
#ifndef T4P_KEYWORDS_H
#define T4P_KEYWORDS_H

#include <wx/string.h>
#include <vector>
#include <pelet/TokenClass.h>

namespace t4p {
/**
 * keywords for PHP 5.3
 */
extern const wxString KEYWORDS_PHP;

/**
 * keywords for PHP 5.4
 */
extern const wxString KEYWORDS_PHP_54;

/**
 * HTML tag names
 * want to support both HTML4 and HTML5, using both sets of keywords.
 */
extern const wxString KEYWORDS_HTML_TAG_NAMES;

/**
 * HTML tag attributes
 * want to support both HTML4 and HTML5, using both sets of keywords.
 */
extern const wxString KEYWORDS_HTML_ATTRIBUTE_NAMES;

/**
 * keywords of mysql-flavored sql
 */
extern const wxString KEYWORDS_MYSQL;

/**
 * these are actually CSS 1, CSS 2 and CSS 3 keywords
 * got these from http://code.google.com/p/scite-files/wiki/bettercsspropertiesfile
 */
extern const wxString KEYWORDS_CSS;

/**
 * CSS pseudo-class keywords
 * these are for CSS 1, 2, and 3
 * got these from http://code.google.com/p/scite-files/wiki/bettercsspropertiesfile
 */
extern const wxString KEYWORDS_CSS_PSEUDOCLASSES;


/**
 * keywords for Javascript
 * from
 * https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Reserved_Words
 */
extern const wxString KEYWORDS_JAVASCRIPT;

/**
 * splits a keyword string into keywords, compares to see if
 * they begin with prefixMatch, and if so adds them to results.
 * comparison with prefixMatch is case SENSITIVE, but keywords
 * are usually all lower case.  if you want to do case-insentive
 * matching, lowercase prefixMatch before calling this function
 *
 * @param keywords the string to split, one of KEYWORDS_* constants
 * @param prefixMatch the string to compare each keyword against
 * @param results the matching keywords will be pushed into this vector
 * @return bool TRUE if at least 1 keyword matched
 */
bool KeywordsTokenizeMatch(const wxString& keywords, const wxString& prefixMatch,
	std::vector<wxString>& results);

/**
 * @return string list of keywords, including 5.4 keywords but only
 *         if the given version is 5.4
 */
wxString KeywordsPhpAll(pelet::Versions version);

/**
 * @return string list of all HTML keywords, tag names plus tag attributes
 */
wxString KeywordsHtmlAll();

}

#endif

