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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */

#ifndef T4P_DETECTEDTAGFINDERCLASS_H
#define T4P_DETECTEDTAGFINDERCLASS_H

#include <globals/Sqlite.h>
#include <globals/SqliteResultClass.h>
#include <language_php/PhpTagClass.h>

namespace t4p {
class DetectedTagTotalCountResultClass : public t4p::SqliteResultClass {
	public:
	DetectedTagTotalCountResultClass();

	int GetTotalCount() const;

	void Next();

	protected:
	bool DoPrepare(soci::statement& stmt, bool doLimit);

	void DoBind(soci::statement& stmt);

	private:
	int TotalCount;
};

class DetectedTagExactMemberResultClass : public t4p::SqliteResultClass {
	public:
	/**
	 * this object is the current row of the result.
	 */
	t4p::PhpTagClass Tag;

	DetectedTagExactMemberResultClass();

	virtual void Set(const std::vector<UnicodeString>& classNames, const UnicodeString& memberName,
		const std::vector<wxFileName>& sourceDirectories);

	void Next();

	protected:
	bool DoPrepare(soci::statement& stmt, bool doLimit);

	void DoBind(soci::statement& stmt);

	std::vector<std::string> Keys;

	std::vector<std::string> SourceDirectories;

	std::string MemberName;

	std::vector<int> TagTypes;

	// the output variables to bind to the statement
	std::string Key;
	int Type;
	std::string ClassName;
	std::string Identifier;
	std::string ReturnType;
	std::string NamespaceName;
	std::string Signature;
	std::string Comment;
	int IsStatic;
};

class DetectedTagNearMatchMemberResultClass : public t4p::DetectedTagExactMemberResultClass {
	public:
	DetectedTagNearMatchMemberResultClass();

	void Set(const std::vector<UnicodeString>& classNames, const UnicodeString& memberName,
		const std::vector<wxFileName>& sourceDirectories);

	protected:
	bool DoPrepare(soci::statement& stmt, bool doLimit);
	private:
	std::string KeyUpper;

	int ClassCount;
};
}

#endif
