/**
 * @copyright  2009-2011 Roberto Perpuly
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
#include <features/BookmarkFeatureClass.h>
#include <algorithm>

t4p::BookmarkClass::BookmarkClass()
: FileName()
, LineNumber(0)
, Handle(-1) {
}

t4p::BookmarkClass::BookmarkClass(const wxFileName& fileName, int lineNumber, int handle)
: FileName(fileName)
, LineNumber(lineNumber)
, Handle(handle) {
}

t4p::BookmarkClass::BookmarkClass(const t4p::BookmarkClass& src)
: FileName()
, LineNumber(0) {
	Copy(src);
}

bool t4p::BookmarkClass::operator==(const t4p::BookmarkClass& other) {
	return other.LineNumber == LineNumber
		&& other.FileName == FileName;
}

t4p::BookmarkClass& t4p::BookmarkClass::operator=(const t4p::BookmarkClass& src) {
	Copy(src);
	return *this;
}

void t4p::BookmarkClass::Copy(const t4p::BookmarkClass& src) {
	FileName = src.FileName;
	LineNumber = src.LineNumber;
	Handle = src.Handle;
}

t4p::BookmarkFeatureClass::BookmarkFeatureClass(t4p::AppClass& app)
: FeatureClass(app)
, Bookmarks()
, CurrentBookmarkIndex(-1) {
}

