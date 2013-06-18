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
#include <globals/TagClass.h>


mvceditor::TagClass::TagClass()
	: Identifier()
	, ClassName()
	, NamespaceName()
	, Signature()
	, ReturnType()
	, Comment()
	, Id(0)
	, FileIsNew(false)
	, Type(CLASS) 
	, IsProtected(false)
	, IsPrivate(false) 
	, IsStatic(false)
	, IsDynamic(false)
	, IsNative(false)
	, Key()
	, FullPath()
	, FileTagId(-1) 
	, SourceId(-1) {
		
}

mvceditor::TagClass::TagClass(const mvceditor::TagClass& src)
	: Identifier()
	, ClassName()
	, NamespaceName()
	, Signature()
	, ReturnType()
	, Comment()
	, Id(0)
	, FileIsNew(false)
	, Type(CLASS)	
	, IsProtected(false)
	, IsPrivate(false) 
	, IsStatic(false)
	, IsDynamic(false)
	, IsNative(false)
	, Key()
	, FullPath()
	, FileTagId(-1) 
	, SourceId(-1) {
	Copy(src);
}

void mvceditor::TagClass::operator=(const TagClass& src) {
	Copy(src);
}

void mvceditor::TagClass::Copy(const mvceditor::TagClass& src) {
	Identifier = src.Identifier;
	ClassName = src.ClassName;
	NamespaceName = src.NamespaceName;
	Signature = src.Signature;
	ReturnType = src.ReturnType;
	Comment = src.Comment;
	Type = src.Type;

	// deep copy the wxString, as this object may be passed between threads
	FullPath = src.FullPath.c_str();
	Key = src.Key;
	Id = src.Id;
	FileTagId = src.FileTagId;
	SourceId = src.SourceId;
	IsProtected = src.IsProtected;
	IsPrivate = src.IsPrivate;
	IsStatic = src.IsStatic;
	IsDynamic = src.IsDynamic;
	IsNative = src.IsNative;
	FileIsNew = src.FileIsNew;
}

bool mvceditor::TagClass::operator<(const mvceditor::TagClass& a) const {
	return Key.caseCompare(a.Key, 0) < 0;
}

bool mvceditor::TagClass::operator==(const mvceditor::TagClass& a) const {
	return Identifier == a.Identifier && ClassName == a.ClassName && NamespaceName == a.NamespaceName;
} 

bool mvceditor::TagClass::IsKeyEqualTo(const UnicodeString& key) const {
	return Key.caseCompare(key, 0) == 0;
}

void mvceditor::TagClass::Clear() {
	Identifier.remove();
	ClassName.remove();
	NamespaceName.remove();
	Signature.remove();
	ReturnType.remove();
	Comment.remove();
	Id = 0;
	Type = CLASS;
	FileTagId = -1;
	SourceId = -1;
	FullPath = wxT("");
	Key.remove();
	IsProtected = false;
	IsPrivate = false;
	IsStatic = false;
	IsDynamic = false;
	IsNative = false;
	FileIsNew = false;
}

mvceditor::TagClass mvceditor::TagClass::MakeNamespace(const UnicodeString& namespaceName) {
	mvceditor::TagClass namespaceItem;
	namespaceItem.Type = mvceditor::TagClass::NAMESPACE;
	namespaceItem.NamespaceName = namespaceName;
	namespaceItem.Identifier = namespaceName;
	namespaceItem.Key = namespaceName;
	return namespaceItem;
}

wxFileName mvceditor::TagClass::FileName() const {
	wxFileName fileName(FullPath);
	return fileName;
}

wxString  mvceditor::TagClass::GetFullPath() const {
	return FullPath;
}

void mvceditor::TagClass::SetFullPath(const wxString& fullPath) {
	FullPath = fullPath;
}

bool mvceditor::TagClass::HasParameters() const {

	// watch out for default argument of "array()"
	// look for the function name followed by parentheses
	return Signature.indexOf(Identifier + UNICODE_STRING_SIMPLE("()")) < 0;
}

UnicodeString mvceditor::TagClass::FullyQualifiedClassName() const {
	UnicodeString qualifiedName = NamespaceName;
	if (!qualifiedName.endsWith(UNICODE_STRING_SIMPLE("\\"))) {
		qualifiedName.append(UNICODE_STRING_SIMPLE("\\"));
	}
	qualifiedName.append(ClassName);
	return qualifiedName;
}

mvceditor::TraitTagClass::TraitTagClass() 
	: TraitClassName()
	, Aliased()
	, InsteadOfs() 
	, FileTagId(0) {
		
}

mvceditor::FileTagClass::FileTagClass() 
	: FullPath()
	, DateTime()
	, FileId(0)
	, IsParsed(false)
	, IsNew(true) {

}

bool mvceditor::FileTagClass::NeedsToBeParsed(const wxDateTime& fileLastModifiedDateTime) const {
	if (IsNew || !IsParsed) {
		return true;
	}

	// precision that is stored in SQLite is up to the second;
	// lets truncate the given date to the nearest second before comparing
	wxDateTime truncated;
	truncated.Set(
		fileLastModifiedDateTime.GetDay(), fileLastModifiedDateTime.GetMonth(), fileLastModifiedDateTime.GetYear(),
		fileLastModifiedDateTime.GetHour(), fileLastModifiedDateTime.GetMinute(), fileLastModifiedDateTime.GetSecond(), 0
	);
	bool modified = truncated.IsLaterThan(DateTime);
	return modified;
}

void mvceditor::FileTagClass::MakeNew(const wxFileName& fileName, bool isParsed) {
	wxDateTime fileLastModifiedDateTime = fileName.GetModificationTime();

	FullPath = fileName.GetFullPath();
	DateTime = fileLastModifiedDateTime;
	FileId = 0;
	IsParsed = isParsed;
	IsNew = false;
}