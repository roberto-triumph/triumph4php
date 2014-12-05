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


t4p::TagClass::TagClass()
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
	, HasVariableArgs(false)
	, Key()
	, FullPath()
	, FileTagId(-1) 
	, SourceId(-1) {
		
}

t4p::TagClass::TagClass(const t4p::TagClass& src)
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
	, HasVariableArgs(false)
	, Key()
	, FullPath()
	, FileTagId(-1) 
	, SourceId(-1) {
	Copy(src);
}

void t4p::TagClass::operator=(const TagClass& src) {
	Copy(src);
}

void t4p::TagClass::Copy(const t4p::TagClass& src) {
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
	HasVariableArgs = src.HasVariableArgs;
	FileIsNew = src.FileIsNew;
}

bool t4p::TagClass::operator<(const t4p::TagClass& a) const {
	return Key.caseCompare(a.Key, 0) < 0;
}

bool t4p::TagClass::operator==(const t4p::TagClass& a) const {
	return Identifier == a.Identifier && ClassName == a.ClassName && NamespaceName == a.NamespaceName;
} 

bool t4p::TagClass::IsKeyEqualTo(const UnicodeString& key) const {
	return Key.caseCompare(key, 0) == 0;
}

void t4p::TagClass::Clear() {
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
	HasVariableArgs = false;
	FileIsNew = false;
}

t4p::TagClass t4p::TagClass::MakeNamespace(const UnicodeString& namespaceName) {
	t4p::TagClass namespaceItem;
	namespaceItem.Type = t4p::TagClass::NAMESPACE;
	namespaceItem.NamespaceName = namespaceName;
	namespaceItem.Identifier = namespaceName;
	namespaceItem.Key = namespaceName;
	return namespaceItem;
}

wxFileName t4p::TagClass::FileName() const {
	wxFileName fileName(FullPath);
	return fileName;
}

wxString  t4p::TagClass::GetFullPath() const {
	return FullPath;
}

void t4p::TagClass::SetFullPath(const wxString& fullPath) {
	FullPath = fullPath;
}

bool t4p::TagClass::HasParameters() const {

	// watch out for default argument of "array()"
	// look for the function name followed by parentheses
	return Signature.indexOf(Identifier + UNICODE_STRING_SIMPLE("()")) < 0;
}

UnicodeString t4p::TagClass::FullyQualifiedClassName() const {
	UnicodeString qualifiedName = NamespaceName;
	if (!qualifiedName.endsWith(UNICODE_STRING_SIMPLE("\\"))) {
		qualifiedName.append(UNICODE_STRING_SIMPLE("\\"));
	}
	qualifiedName.append(ClassName);
	return qualifiedName;
}

t4p::TraitTagClass::TraitTagClass() 
	: TraitClassName()
	, Aliased()
	, InsteadOfs() 
	, FileTagId(0) {
		
}

t4p::FileTagClass::FileTagClass() 
	: FullPath()
	, DateTime()
	, FileId(0)
	, SourceId(0)
	, IsParsed(false)
	, IsNew(true) {

}

bool t4p::FileTagClass::NeedsToBeParsed(const wxDateTime& fileLastModifiedDateTime) const {
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

void t4p::FileTagClass::MakeNew(const wxFileName& fileName, wxDateTime modTime, bool isParsed) {
	wxASSERT_MSG(modTime.IsValid(), _("file modification time is not valid"));
	FullPath = fileName.GetFullPath();
	DateTime = modTime;
	FileId = 0;
	IsParsed = isParsed;
	IsNew = false;
}

wxString t4p::FileTagClass::Name() const {
	wxFileName fileName(FullPath);
	return fileName.GetFullName();
}