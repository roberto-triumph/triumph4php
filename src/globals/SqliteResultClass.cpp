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
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */

#include <globals/SqliteResultClass.h>
#include <globals/String.h>

t4p::SqliteResultClass::SqliteResultClass()
: IsEmpty(true)
, IsFetched(false) {
	Stmt = NULL;
}

t4p::SqliteResultClass::~SqliteResultClass() {
	if (Stmt) {
		delete Stmt;
	}
}

bool t4p::SqliteResultClass::Init(soci::session& session, bool doLimit) {
	bool good = false;
	wxASSERT_MSG(NULL == Stmt, "result can only be initialized once");
	try {
		soci::statement* stmt = new soci::statement(session);
		Stmt = stmt;
		DoPrepare(*stmt, doLimit);
		DoBind(*stmt);
		Stmt->define_and_bind();
		good = true;
	} catch (std::exception& exception) {
		wxASSERT_MSG(false, exception.what());
	}
	return good;
}

bool t4p::SqliteResultClass::IsOk() const {
	return Stmt != NULL;
}

bool t4p::SqliteResultClass::Exec(soci::session& session, bool doLimit) {
	bool prepped = Stmt != NULL;
	if (!Stmt) {
		prepped = Init(session, doLimit);
	}
	if (!prepped) {
		return false;
	}

	IsEmpty = true;
	IsFetched = false;
	wxString error;
	try {
		bool hasData = Stmt->execute(true);
		if (hasData) {
			IsEmpty = false;
		} else {
			IsEmpty = true;
			IsFetched = true;
		}
	} catch (std::exception& e) {
		error = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, error);
		delete Stmt;
		Stmt = NULL;
	}
	return !IsEmpty;
}

bool t4p::SqliteResultClass::Empty() const {
	return IsEmpty;
}

bool t4p::SqliteResultClass::More() const {
	return !IsEmpty && !IsFetched;
}

bool t4p::SqliteResultClass::Fetch() {
	if (!Stmt || IsFetched) {
		return false;
	}
	bool more = Stmt->fetch();
	if (!more) {
		IsFetched = true;
	}
	return more;
}

bool t4p::SqliteResultClass::ReExec(wxString& error)  {
	IsEmpty = true;
	IsFetched = false;
	if (!Stmt) {
		error = wxT("Result has not been prepared");
		return false;
	}
	try {
		bool hasData = Stmt->execute(true);
		if (hasData) {
			IsEmpty = false;
		} else {
			IsEmpty = true;
		}
	} catch (std::exception& e) {
		error = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, error);
		delete Stmt;
		Stmt = NULL;
	}
	return !IsEmpty;
}
