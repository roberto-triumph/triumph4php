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
#include <language_js/JsTagClass.h>

t4p::JsTagClass::JsTagClass()
: Id(0)
, FileTagId(0)
, SourceId(0)
, Key()
, Identifier()
, Signature()
, Comment()
, FullPath()
, FileIsNew(false)
, LineNumber(0)
, ColumnPosition(0) {
}

t4p::JsTagClass::JsTagClass(const t4p::JsTagClass& src)
: Id(0)
, FileTagId(0)
, SourceId(0)
, Key()
, Identifier()
, Signature()
, Comment()
, FullPath()
, FileIsNew(false)
, LineNumber(0)
, ColumnPosition(0) {
	Copy(src);
}

t4p::JsTagClass& t4p::JsTagClass::operator=(const t4p::JsTagClass& src) {
	Copy(src);
	return *this;
}

void t4p::JsTagClass::Copy(const t4p::JsTagClass& src) {
	Id = src.Id;
	FileTagId = src.FileTagId;
	SourceId = src.SourceId;
	Key = src.Key;
	Identifier = src.Identifier;
	Signature = src.Signature;
	Comment = src.Comment;
	FullPath = src.FullPath;
	FileIsNew = src.FileIsNew;
	LineNumber = src.LineNumber;
	ColumnPosition = src.ColumnPosition;
}
