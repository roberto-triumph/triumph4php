/**
 * @copyright  2013 Roberto Perpuly
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
#ifndef SRC_LANGUAGE_JS_JSTAGRESULTCLASS_H_
#define SRC_LANGUAGE_JS_JSTAGRESULTCLASS_H_

#include <globals/SqliteResultClass.h>
#include <language_js/JsTagClass.h>
#include <wx/filename.h>
#include <vector>
#include <string>

namespace t4p {
/**
 * A SQLite result set of JsTag rows.
 */
class JsTagResultClass : public t4p::SqliteResultClass {
	public:
	t4p::JsTagClass JsTag;

	JsTagResultClass();

	void Next();

	protected:
	void DoBind(soci::statement& stmt);

	private:
	// the variables bound to the sqlite result set
	int Id;
	int FileItemId;
	int SourceId;
	std::string Key;
	std::string Identifier;
	std::string Signature;
	std::string Comment;
	std::string FullPath;
	int IsFileNew;
	int LineNumber;
	int ColumnPosition;
};

/**
 * The ExactMatchJsTagResultClass executes a query that will look
 * for exact matches in the js tags SQLite file (where the input
 * is exactly equal to the Key column, in a case insensitive manner).
 */
class ExactMatchJsTagResultClass : public t4p::JsTagResultClass {
	public:
	ExactMatchJsTagResultClass();

	/**
	 * @param search the string to look for
	 * @param sourceDirs restrict to only tags found in the given source directories
	 */
	void SetSearch(const UnicodeString& search, const std::vector<wxFileName>& sourceDirs);

	protected:
	bool DoPrepare(soci::statement& stmt, bool doLimit);

	private:
	std::string Search;
	std::vector<std::string> SourceDirs;
};

/**
 * The NearMatchJsTagResultClass executes a query that will look
 * for near matches in the js tags SQLite file (where the key
 * starts with the input, in a case insensitive manner).
 */
class NearMatchJsTagResultClass : public t4p::JsTagResultClass {
	public:
	NearMatchJsTagResultClass();

	/**
	 * @param search the string to look for
	 * @param sourceDirs restrict to only tags found in the given source directories
	 */
	void SetSearch(const UnicodeString& search, const std::vector<wxFileName>& sourceDirs);

	protected:
	bool DoPrepare(soci::statement& stmt, bool doLimit);

	private:
	std::string Search;
	std::string SearchUpper;
	std::vector<std::string> SourceDirs;
};
}  // namespace t4p

#endif  // SRC_LANGUAGE_JS_JSTAGRESULTCLASS_H_
