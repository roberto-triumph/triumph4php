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

#ifndef __MVCEDITOR_DETECTEDTAGFINDERCLASS_H__
#define __MVCEDITOR_DETECTEDTAGFINDERCLASS_H__

#include <globals/Sqlite.h>
#include <globals/TagClass.h>

namespace mvceditor {

class DetectedTagTotalCountResultClass : public mvceditor::SqliteResultClass {

public:

	DetectedTagTotalCountResultClass();

	bool Prepare(soci::session& session, bool doLimit);

	int GetTotalCount() const;

	void Next();

private:

	int TotalCount;
};

class DetectedTagExactMemberResultClass : public mvceditor::SqliteResultClass {

public:

	/**
	 * this object is the current row of the result.
	 */
	mvceditor::TagClass Tag;

	DetectedTagExactMemberResultClass();

	bool Prepare(soci::session& session, bool doLimit);

	virtual void Set(const std::vector<UnicodeString>& classNames, const UnicodeString& memberName);

	void Next();

protected:
	
	std::vector<std::string> Keys;

	std::string MemberName;

	std::vector<int> TagTypes;

	// the output variables to bind to the statement
	std::string Key;
	int Type;
	std::string ClassName;
	std::string Identifier;
	std::string ReturnType;
	std::string NamespaceName;
	std::string Comment;

	/**
	 *  binds the variables to this statement
	 * @param stmt this class will own the pointer
	 * @return bool TRUE if there was at least one result
	 */
	bool Init(soci::statement* stmt);
};

class DetectedTagNearMatchMemberResultClass : public mvceditor::DetectedTagExactMemberResultClass {

public:

	DetectedTagNearMatchMemberResultClass();

	bool Prepare(soci::session& session, bool doLimit);

	void Set(const std::vector<UnicodeString>& classNames, const UnicodeString& memberName);

private:

	std::string KeyUpper;

	int ClassCount;
};

}

#endif