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
#ifndef SRC_LANGUAGE_JS_JSTAGCLASS_H_
#define SRC_LANGUAGE_JS_JSTAGCLASS_H_

#include <unicode/unistr.h>
#include <wx/string.h>

namespace t4p {
/**
 * A JsTag is a code artifact that was parsed out of a user's
 * project. An example of this is a function declaration. JsTags
 * are persisted in a SQLite file, and can be looked up by using
 * the JsTagFinderClass.
 */
class JsTagClass {
 public:
    /**
     * primary key, unique identifier for this tag.
     * @var int
     */
    int Id;

    /**
     * The index to the file where this tag was found.
     */
    int FileTagId;

    /**
     * The index to the source directory where this tag was found.
     */
    int SourceId;

    /**
     * This is the "key" that we will use for lookups. This is the string that will be used to index resources
     * by so that we can use binary search.
     * The key is a single identifier (function name, property  name),
     * or a a full object property member name (object.property or object.function)
     */
    UnicodeString Key;

    /**
     * The identifer name of this tag. The name of the function.
     * @var UnicodeString
     */
    UnicodeString Identifier;

    /**
     * The tag signature. For functions; it is the entire argument list
     * @var UnicodeString
     */
    UnicodeString Signature;

    /**
     * The comment attached to the tag.
     * @var UnicodeString
     */
    UnicodeString Comment;

    /**
     * Full path to the file where this tag was found.
     */
    wxString FullPath;

    /**
     * Same as FileTagClass::IsNew ie TRUE if this tag was parsed from contents
     * not yet written to disk
     * @see FileTagClass::IsNew
     */
    bool FileIsNew;

    /**
     * @var int 1-based number where this tag starts in the file
     */
    int LineNumber;

    /**
     * @var int 0-based number where this tag starts in the line
     */
    int ColumnPosition;

    JsTagClass();

    JsTagClass(const t4p::JsTagClass& src);

    t4p::JsTagClass& operator=(const t4p::JsTagClass& src);

    void Copy(const t4p::JsTagClass& src);
};
}  // namespace t4p

#endif  // SRC_LANGUAGE_JS_JSTAGCLASS_H_
