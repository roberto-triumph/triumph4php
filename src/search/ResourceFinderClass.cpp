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
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <search/ResourceFinderClass.h>
#include <search/FinderClass.h>
#include <windows/StringHelperClass.h>
#include <MvcEditorAssets.h>
#include <wx/filename.h>
#include <algorithm>
#include <fstream>
#include <unicode/uchar.h>

/**
 * @param resource the resource object that the parsed data will go into
 * @param line the tag line to parse
 */
static bool ParseTag(mvceditor::ResourceClass& resource, UChar* line) {
	resource.Clear();
	resource.NamespaceName = UNICODE_STRING_SIMPLE("\\");
	bool good = false;

	// summary of tag format
	// column 1 : identifier
	// column 2: file name  (location of tag) - currently not used because we are using tags files
	//           for native resources, and user will never be able to jump to the native resources
	// column 3: line number (+ VIM comment) -  currently not used because we are using tags files
	//           for native resources, and user will never be able to jump to the native resources
	// column 4-n: extension fields
	UnicodeString lineString(line);
	int32_t end = lineString.indexOf('\t');
	if (end >= 0) {
		resource.Identifier.setTo(lineString, 0, end);
		end = lineString.indexOf('\t', end + 1); // file column - not used
		end = lineString.indexOf('\t', end + 1); // vim magic column -  not used
		UChar kind = 0;
		UnicodeString className,
			accessModifiers,
			inheritance,
			implementation,
			signature;
		int32_t extensionStart = end + 1;
		int32_t extensionEnd = lineString.indexOf('\t', extensionStart);
		UnicodeString extension;
		if (extensionEnd > 0) {
			extension.setTo(lineString, extensionStart, extensionEnd - extensionStart + 1);
		}
		else {
			extension.setTo(lineString, extensionStart);
		}
		while (extensionStart > 0 && !extension.isEmpty()) {
			int32_t colonIndex = extension.indexOf(':');
			if (colonIndex < 0) {

				// if no key, then default to 'kind' extension
				kind = extension.charAt(0);
			}
			else if (extension.caseCompare(0, 5, UNICODE_STRING_SIMPLE("kind:"), 0) == 0) {
				kind = extension.charAt(6);
			}
			else if (extension.caseCompare(0, 2, UNICODE_STRING_SIMPLE("k:"), 0) == 0) {
				kind = extension.charAt(2);
			}
			else if (extension.caseCompare(0, 6, UNICODE_STRING_SIMPLE("class:"), 0) == 0) {
				className.setTo(extension, colonIndex + 1);
				className.trim();
			}
			else if (extension.caseCompare(0, 2, UNICODE_STRING_SIMPLE("a:"), 0) == 0) {
				accessModifiers.setTo(extension, colonIndex + 1);
			}
			else if (extension.caseCompare(0, 7, UNICODE_STRING_SIMPLE("access:"), 0) == 0) {
				accessModifiers.setTo(extension, colonIndex + 1);
			}
			else if (extension.caseCompare(0, 2, UNICODE_STRING_SIMPLE("i:"), 0) == 0) {
				inheritance.setTo(extension, colonIndex + 1);
			}
			else if (extension.caseCompare(0, 12, UNICODE_STRING_SIMPLE("inheritance:"), 0) == 0) {
				inheritance.setTo(extension, colonIndex + 1);
			}
			else if (extension.caseCompare(0, 2, UNICODE_STRING_SIMPLE("m:"), 0) == 0) {
				implementation.setTo(extension, colonIndex + 1);
			}
			else if (extension.caseCompare(0, 15, UNICODE_STRING_SIMPLE("implementation:"), 0) == 0) {
				implementation.setTo(extension, colonIndex + 1);
			}
			else if (extension.compare(0, 2, UNICODE_STRING_SIMPLE("S:")) == 0) {
				signature.setTo(extension, colonIndex + 1);
				signature.trim();
			}
			else if (extension.caseCompare(0, 10, UNICODE_STRING_SIMPLE("signature:"), 0) == 0) {
				signature.setTo(extension, colonIndex + 1);
				signature.trim();
			}
			if (extensionEnd > 0) {
				extensionStart = extensionEnd + 1;
				extensionEnd = lineString.indexOf('\t', extensionStart);
				if (extensionEnd > 0) {
					extension.setTo(lineString, extensionStart, extensionEnd - extensionStart + 1);
				}
				else {
					extension.setTo(lineString, extensionStart);
				}
			}
			else {
				extensionStart = -1;
			}
		}
		if (kind) {

			// kinds
			// c = class
			// d = define
			// f = function / method
			// o = class member (constant)
			// p = class member (variable)
			// v = variable
			//
			// other extensions 
			// a = Access (or export) of class members
			// f = File-restricted scoping - not used
			// i = Inheritance information
			// k, kind = Kind of tag
			// l Language of source file containing tag - not used
			// m Implementation information 
			// n Line number of tag definition - not used
			// s Scope of tag definition - not used
			// S Signature of routine (e.g. prototype or parameter list)
			// t Type and name of a variable or typedef as "typeref:" field - not used
			if ('c' == kind) {
				resource.ClassName = resource.Identifier;
				resource.Type = mvceditor::ResourceClass::CLASS;
				good = true;
			}
			else if ('d' == kind) {
				resource.Type = mvceditor::ResourceClass::DEFINE;
				good = true;
			}
			else if ('f' == kind && className.isEmpty()) {
				resource.Type = mvceditor::ResourceClass::FUNCTION;
				resource.Signature.setTo(signature);

				// return type is in the signature
				int32_t index = resource.Signature.indexOf(UNICODE_STRING_SIMPLE("function"));
				if (index > 0) {
					resource.ReturnType.setTo(signature, 0, index);
					resource.ReturnType.trim();
				}
				good = true;
			}
			else if ('f' == kind && !className.isEmpty()) {
				resource.Type = mvceditor::ResourceClass::METHOD;
				resource.ClassName = className;
				resource.Signature.setTo(signature);

				// return type is in the signature
				int32_t index = resource.Signature.indexOf(UNICODE_STRING_SIMPLE("function"));
				if (index > 0) {
					resource.ReturnType.setTo(signature, 0, index);
					resource.ReturnType.trim();
				}
				good = true;
			}
			else if ('o' == kind) {
				resource.Type = mvceditor::ResourceClass::CLASS_CONSTANT;
				resource.ClassName = className;
				good = true;
			}
			else if ('p' == kind) {
				resource.Type = mvceditor::ResourceClass::MEMBER;
				resource.ClassName = className;
				good = true;
			}
			// 'v' kind super global variables there is no type for them...
			
			resource.IsProtected = accessModifiers.indexOf(UNICODE_STRING_SIMPLE("protected")) != -1;

			// class constants are always static
			resource.IsStatic = accessModifiers.indexOf(UNICODE_STRING_SIMPLE("static")) != -1 || 'o' == kind;
		}
	}
	return good;
}

/**
 * @param resource the resource to check, for example MyClass::Method
 * @param classNames list of fully qualified class names, for example  [ MyClass, MySecondClass, \First\Class ]
 * @return bool TRUE if the given [member] resource is from any of the given classses
 */
static bool MatchesAnyClass(const mvceditor::ResourceClass& resource, const std::vector<UnicodeString>& classNames) {
	bool match = false;
	for (size_t i = 0; i < classNames.size(); ++i) {
		if (resource.ClassName.caseCompare(classNames[i], 0) == 0) {
			match = true;
			break;
		}
	}
	return match;
}

/**
 * @return TRUE if the given resource belongs to a class
 */
static bool IsClassMember(const mvceditor::ResourceClass& resource) {
	return mvceditor::ResourceClass::CLASS_CONSTANT == resource.Type
		|| mvceditor::ResourceClass::MEMBER == resource.Type
		|| mvceditor::ResourceClass::METHOD == resource.Type;
}

/**
 * A predicate class useful for STL algorithms.  This class will match TRUE
 * if a resource has the exact same key as the identifier given in
 * the constructor (case insensitive). For example, if this predicate is 
 * give to std::count_if function, then the function will count all resources
 * that have the same key.
 * 
 * IsNegative flag will invert the comparison, and the predicate will instead
 * return false when a resource has the exact same key as the key
 * given in the constructor. This makes it possible to use this predicate in
 * std::remove_if to remove items that do NOT match an identifier.
 */
class KeyPredicateClass {
	
public:

	UnicodeString Key;
	
	bool IsNegative;

	KeyPredicateClass(const UnicodeString& key, bool isNegative) 
		: Key(key) 
		, IsNegative(isNegative) {
		
	}

	bool operator()(const mvceditor::ResourceClass& resource) const {
		if (!IsNegative) {
			return resource.IsKeyEqualTo(Key) == 0;
		}
		return !resource.IsKeyEqualTo(Key);
	}
};

/**
 * appends name to namespace
 */
static UnicodeString QualifyName(const UnicodeString& namespaceName, const UnicodeString& name) {
	UnicodeString qualifiedName;
	qualifiedName.append(namespaceName);
	if (!qualifiedName.endsWith(UNICODE_STRING_SIMPLE("\\"))) {
		qualifiedName.append(UNICODE_STRING_SIMPLE("\\"));
	}
	qualifiedName.append(name);
	return qualifiedName;
}

mvceditor::ResourceFinderClass::ResourceFinderClass()
	: FileFilters()
	, IdentifierCache()
	, MembersCache()
	, NamespaceCache()
	, TraitCache()
	, FileCache()
	, Matches()
	, Lexer()
	, Parser()
	, FileName()
	, ClassName()
	, MethodName()
	, ResourceType(FILE_NAME)
	, LineNumber(0)
	, CurrentFileItemIndex(-1)
	, IsCacheSorted(false) {
	Parser.SetClassObserver(this);
	Parser.SetClassMemberObserver(this);
	Parser.SetFunctionObserver(this);
}

void mvceditor::ResourceFinderClass::SetVersion(pelet::Versions version) {
	Parser.SetVersion(version);
}

bool mvceditor::ResourceFinderClass::Walk(const wxString& fileName) {
	bool matchedFilter = false;
	wxFileName file(fileName);
	for (size_t i = 0; i < FileFilters.size(); ++i) {
		wxString filter = FileFilters[i];
		if (!wxIsWild(filter)) {

			// exact match of the name portion of fileName
			matchedFilter = file.GetFullName().CmpNoCase(filter) == 0;
		}
		else {
			matchedFilter =  wxMatchWild(filter, fileName);
		}
		if (matchedFilter) {
			break;
		}
	}
	if (matchedFilter) {
		switch (ResourceType) {
			case FILE_NAME:
			case FILE_NAME_LINE_NUMBER:
				BuildResourceCache(fileName, false);
				break;
			case CLASS_NAME:
			case CLASS_NAME_METHOD_NAME:
			case NAMESPACE_NAME:
				BuildResourceCache(fileName, true);
				break;
		}
	}

	// no need to keep know which files have resources, most likely all files should have a resource
	return false;
}

void mvceditor::ResourceFinderClass::BuildResourceCacheForFile(const wxString& fullPath, const UnicodeString& code, bool isNew) {

	// remove all previous cached resources
	int fileItemIndex = -1;
	mvceditor::ResourceFinderClass::FileItem fileItem;
	bool foundFile = FindInFileCache(fullPath, fileItemIndex, fileItem);
	if (foundFile) {
		RemoveCachedResources(fileItemIndex);
	}
	if (!foundFile) {

		// if caller just calls this method without calling Walk(); then file cache will be empty
		// need to add an entry so that GetResourceMatchFullPathFromResource works correctly
		fileItemIndex = PushIntoFileCache(fullPath, false, isNew);
	}
	CurrentFileItemIndex = fileItemIndex;
	
	// for now silently ignore parse errors
	pelet::LintResultsClass results;
	Parser.ScanString(code, results);
	IsCacheSorted = false;
}

size_t mvceditor::ResourceFinderClass::GetResourceMatchCount() const {
	return Matches.size();
}

mvceditor::ResourceClass mvceditor::ResourceFinderClass::GetResourceMatch(size_t index) const {
	ResourceClass match;
	if (index >= 0 && index < Matches.size()) {
		match = Matches[index];
	}
	return match;
}

wxString mvceditor::ResourceFinderClass::GetResourceMatchFullPath(size_t index) const {
	if (index >= 0 && index < Matches.size()) {
		size_t fileItemIndex = Matches[index].FileItemIndex;
		if (fileItemIndex >= 0 && fileItemIndex < FileCache.size()) {
			return FileCache[fileItemIndex].FullPath;
		}
	}
	return wxT("");
}

wxString  mvceditor::ResourceFinderClass::GetResourceMatchFullPathFromResource(const mvceditor::ResourceClass& resource) const {
	size_t fileItemIndex = resource.FileItemIndex;
	if (fileItemIndex >= 0 && fileItemIndex < FileCache.size()) {
		return FileCache[fileItemIndex].FullPath;
	}
	return wxT("");
}

bool mvceditor::ResourceFinderClass::GetResourceMatchPosition(const mvceditor::ResourceClass& resource, const UnicodeString& text, int32_t& pos, 
		int32_t& length) {
	size_t start = 0;
	mvceditor::FinderClass finder;
	finder.Mode = FinderClass::REGULAR_EXPRESSION;

	UnicodeString className,
		methodName;
	if (!resource.ClassName.isEmpty()) {
		className = resource.ClassName;
		methodName = resource.Identifier;

		mvceditor::FinderClass::EscapeRegEx(className);
		mvceditor::FinderClass::EscapeRegEx(methodName);
	}
	else {
		className = resource.Identifier;
		mvceditor::FinderClass::EscapeRegEx(className);
	}
	switch (resource.Type) {
		case ResourceClass::CLASS:
			finder.Expression = UNICODE_STRING_SIMPLE("\\sclass\\s+") + className + UNICODE_STRING_SIMPLE("\\s");
			break;
		case ResourceClass::METHOD:
			//advance past the class header so that if  a function with the same name exists we will skip it
			finder.Expression = UNICODE_STRING_SIMPLE("\\sclass\\s+") + className + UNICODE_STRING_SIMPLE("\\s");
			if (finder.Prepare() && finder.FindNext(text, start)) {			
				finder.GetLastMatch(pos, length);
			}
			start = pos + length;

			// method may return a reference (&)
			finder.Expression = UNICODE_STRING_SIMPLE("\\sfunction\\s*(&\\s*)?") + methodName + UNICODE_STRING_SIMPLE("\\s*\\(");
			break;
		case ResourceClass::FUNCTION:

			// function may return a reference (&)
			finder.Expression = UNICODE_STRING_SIMPLE("\\sfunction\\s*(&\\s*)?") + className + UNICODE_STRING_SIMPLE("\\s*\\(");
			break;
		case ResourceClass::MEMBER:
			//advance past the class header so that if  a variable with the same name exists we will skip it				:
			finder.Expression = UNICODE_STRING_SIMPLE("\\sclass\\s+") + className + UNICODE_STRING_SIMPLE("\\s");
			if (finder.Prepare() && finder.FindNext(text, start)) {			
				finder.GetLastMatch(pos, length);
			}
			start = pos + length;
			finder.Expression = UNICODE_STRING_SIMPLE("\\s((var)|(public)|(protected)|(private)).+") + methodName + UNICODE_STRING_SIMPLE(".*;");
			break;
		case ResourceClass::DEFINE:
			finder.Expression = UNICODE_STRING_SIMPLE("\\sdefine\\(\\s*('|\")") + className + UNICODE_STRING_SIMPLE("('|\")");
			break;
		case ResourceClass::CLASS_CONSTANT:
			//advance past the class header so that if  a variable with the same name exists we will skip it				:
			finder.Expression = UNICODE_STRING_SIMPLE("\\sclass\\s+") + className + UNICODE_STRING_SIMPLE("\\s");
			if (finder.Prepare() && finder.FindNext(text, start)) {			
				finder.GetLastMatch(pos, length);
			}
			start = pos + length;
			finder.Expression = UNICODE_STRING_SIMPLE("\\sconst\\s+") + methodName + UNICODE_STRING_SIMPLE("\\s*=");
		default:
			break;
	}
	if (finder.Prepare() && finder.FindNext(text, start) && finder.GetLastMatch(pos, length)) {
		++pos; //eat the first space
		--length;
		return true;
	}
	return false;
}

bool mvceditor::ResourceFinderClass::Prepare(const wxString& resource) {
	ResourceType = ParseGoToResource(resource, FileName, ClassName, MethodName, LineNumber);

	// maybe later do some error checking
	return !resource.IsEmpty();
}

bool mvceditor::ResourceFinderClass::BuildResourceCacheForNativeFunctions() {
	bool loaded = false;
	wxFileName fileName = mvceditor::NativeFunctionsAsset();	
	if (fileName.FileExists()) {
		loaded = LoadTagFile(fileName, true);
	}
	return loaded;
}

bool mvceditor::ResourceFinderClass::CollectNearMatchResources() {
	EnsureSorted();	
	switch (ResourceType) {
		case FILE_NAME:
		case FILE_NAME_LINE_NUMBER:
			CollectNearMatchFiles();
			break;
		case CLASS_NAME:
			CollectNearMatchNonMembers();
			break;
		case CLASS_NAME_METHOD_NAME:
			CollectNearMatchMembers();
			break;
		case NAMESPACE_NAME:
			CollectNearMatchNamespaces();
			break;
	}
	EnsureMatchesExist();
	sort(Matches.begin(), Matches.end());
	return !Matches.empty();
}

void mvceditor::ResourceFinderClass::CollectNearMatchFiles() {
	wxString path,
		currentFileName,
		extension;
	wxString fileName =  StringHelperClass::IcuToWx(FileName).Lower();
	for (size_t i = 0; i < FileCache.size(); ++i) {
		wxFileName::SplitPath(FileCache[i].FullPath, &path, &currentFileName, &extension);
		currentFileName += wxT(".") + extension;
		if (wxNOT_FOUND != currentFileName.Lower().Find(fileName)) {
			if (0 == LineNumber || GetLineCountFromFile(FileCache[i].FullPath) >= LineNumber) {
				ResourceClass newItem;
				newItem.FileItemIndex = i;
				Matches.push_back(newItem);
			}
		}
	}
}

void mvceditor::ResourceFinderClass::CollectNearMatchNonMembers() {
	UnicodeString key = ClassName;
	BoundedCacheSearch(IdentifierCache, key, Matches, 50);
	
	// bounded search uses near matches, here if the identifier is an exact match then 
	// just use the exact match
	std::vector<mvceditor::ResourceClass>::iterator end = Matches.end();
	end = std::remove_if(Matches.begin(), end, IsClassMember);
	
	if (!Matches.empty() && Matches[0].Identifier.caseCompare(key, 0) == 0) {
		KeyPredicateClass pred(key, true);
		end = std::remove_if(Matches.begin(), end, pred);
	}
	
	// need to actually delete the items from the results; remove_if does not actually erase them
	Matches.erase(end, Matches.end());
}

void mvceditor::ResourceFinderClass::CollectNearMatchMembers() {
	
	// look for the parent classes. when we are searching for members, we want to look
	// for members from the class or any class it inherited from
	// whenever we insert into parentClassNames, need to make sure strings are all lowercase
	// that way we dont have do lower case for every resource comparison
	std::vector<UnicodeString> parentClassNames = ClassHierarchy(ClassName);	
	if (MethodName.isEmpty()) {
		
		// special case; query for all methods for a class (UserClass::)
		CollectAllMembers(parentClassNames);
		CollectAllTraitMembers(ClassName);
	}
	else if (ClassName.isEmpty()) {
		
		// special case, query accross all classes for a method (::getName)
		// if ClassName is empty, then just check method names (which BoundedCacheSearch does). This ensures 
		// queries like '::getName' will work as well.
		UnicodeString key = MethodName;
		BoundedCacheSearch(IdentifierCache, key, Matches, 50);
		
		// BoundedCacheSearch got all resources that match on the identifier. we need extra logic
		// to remove matches that come from resources that are not methods or properties
		std::vector<ResourceClass>::iterator it = Matches.begin();
		while (it != Matches.end()) {
			bool keep = mvceditor::ResourceClass::MEMBER == it->Type ||
				mvceditor::ResourceClass::METHOD == it->Type ||
				mvceditor::ResourceClass::CLASS_CONSTANT == it->Type;
			if (keep) {
				++it;
			}
			else {
				it = Matches.erase(it);
			}
		}
	}
	else {
		UnicodeString key = MethodName;
		BoundedCacheSearch(IdentifierCache, key, Matches, 50);
		std::vector<ResourceClass>::iterator it = Matches.begin();
		
		// BoundedCacheSearch got all resources that match on the identifier. we need extra logic
		// to remove matches that come from resources that have the same identifier but are from
		// a different, unrelated classes. A related class is a class that is in the inheritance chain
		// for the query class, or a trait that is used by the query class.
		while (it != Matches.end()) {
			bool keep = MatchesAnyClass(*it, parentClassNames) || IsTraitInherited(*it, ClassName);
			if (keep) {
				++it;
			}
			else {
				it = Matches.erase(it);
			}
		}
	}
}

void mvceditor::ResourceFinderClass::CollectAllMembers(const std::vector<UnicodeString>& classNames) {
	
	for (size_t i = 0; i < classNames.size(); ++i) {
		UnicodeString key = classNames[i] + UNICODE_STRING_SIMPLE("::");
		BoundedCacheSearch(MembersCache, key, Matches, 50);
	}
}

void mvceditor::ResourceFinderClass::CollectAllTraitMembers(const UnicodeString& className) {
	
	std::vector<mvceditor::TraitResourceClass> traits = TraitCache[className];
	for (size_t j = 0; j < traits.size(); ++j) {
		
		// lets unqualify, the members cache key does not have the namespace name
		int32_t index = traits[j].TraitClassName.lastIndexOf(UNICODE_STRING_SIMPLE("\\"));
		UnicodeString traitClassNameOnly(traits[j].TraitClassName, index + 1);
		UnicodeString key =  traitClassNameOnly + UNICODE_STRING_SIMPLE("::");
		BoundedCacheSearch(MembersCache, key, Matches, 50);
		
		// TODO weed out Matches from methods from the wrong namespace
	}
	
	// now go through the result and change the method names of any aliased 
	for (size_t i = 0; i < traits.size(); i++) {
		std::vector<UnicodeString> aliases = traits[i].Aliased;
		for (size_t a = 0; a < aliases.size(); a++) {
			mvceditor::ResourceClass res;
			res.ClassName = traits[i].TraitClassName;
			res.Identifier = aliases[a];
			Matches.push_back(res);
		}
	}
}

void mvceditor::ResourceFinderClass::CollectNearMatchNamespaces() {
	
	// needle identifier contains a namespace operator; but it may be
	// a namespace or a fully qualified name
	UnicodeString key = ClassName;
	BoundedCacheSearch(NamespaceCache, key, Matches, 50);
	
	std::vector<mvceditor::ResourceClass>::iterator end = Matches.end();
	
	// if there are exact matches; we only want to collect exact matches
	if (!Matches.empty() && Matches[0].IsKeyEqualTo(key)) {
		KeyPredicateClass pred(key, true);
		end = std::remove_if(Matches.begin(), end, pred);
	}
	
	// need to actually erase them,, std::remove_if does not
	Matches.erase(end, Matches.end());
}

std::vector<UnicodeString> mvceditor::ResourceFinderClass::ClassHierarchy(const UnicodeString& className) const {
	std::vector<UnicodeString> parentClassNames;
	parentClassNames.push_back(className);
	UnicodeString lastClassName(className);
	lastClassName.toLower();
	bool done = false;
	while (!done) {
		done = true;

		// TODO we should probably try to use std::lower_bound here
		for (std::vector<ResourceClass>::const_iterator it = IdentifierCache.begin(); it != IdentifierCache.end(); ++it) {
			if (it->Type == ResourceClass::CLASS && 0 == it->ClassName.caseCompare(lastClassName, 0) && it->Signature.length()) {

				// wont include interface names in this list, since it is very likely that the same function
				// is actually implemented and we want to jump to the implementation instead
				int32_t extendsPos = it->Signature.indexOf(UNICODE_STRING_SIMPLE("extends "));
				if (extendsPos >= 0) {
					lastClassName = ExtractParentClassFromSignature(it->Signature);
					parentClassNames.push_back(lastClassName);
					done = false;
					break;
				}
			}
		}
	}
	return parentClassNames;
}

UnicodeString mvceditor::ResourceFinderClass::GetResourceParentClassName(const UnicodeString& className, 
		const UnicodeString& methodName) const {
	UnicodeString parentClassName;
	
	// TODO not very efficient
	for (std::vector<ResourceClass>::const_iterator it = IdentifierCache.begin(); it != IdentifierCache.end(); ++it) {
		if (it->Type == ResourceClass::CLASS &&  it->Identifier == className) {
			parentClassName = ExtractParentClassFromSignature(it->Signature);
			break;
		}
	}
	if (!parentClassName.isEmpty() && !methodName.isEmpty()) {

		// check that the parent has the given method
		UnicodeString parentClassSignature;
		bool found = false;
		for (std::vector<ResourceClass>::const_iterator it = IdentifierCache.begin(); it != IdentifierCache.end(); ++it) {
			if (it->Type == ResourceClass::CLASS &&  it->ClassName == parentClassName) {
				parentClassSignature = it->ClassName;
			}
		}
		for (std::vector<ResourceClass>::const_iterator it = MembersCache.begin(); it != MembersCache.end(); ++it) {
			if (it->Type == ResourceClass::METHOD && 0 == it->ClassName.caseCompare(parentClassName, 0) && 
				mvceditor::CaseStartsWith(it->Identifier, methodName)) {

				// this ancestor has the method
				found = true;
				break;
			}
		}
		if (!found && !parentClassSignature.isEmpty()) {

			//  keep searching up the inheritance chain
			return GetResourceParentClassName(parentClassName, methodName);
		}
		else if (!found) {
			parentClassName = UNICODE_STRING_SIMPLE("");
		}
	}
	return parentClassName;
}

UnicodeString mvceditor::ResourceFinderClass::GetClassName() const {
	return ClassName;
}

UnicodeString mvceditor::ResourceFinderClass::GetMethodName() const {
	return MethodName;
}

int mvceditor::ResourceFinderClass::GetLineNumber() const {
	return LineNumber;
}

mvceditor::ResourceFinderClass::ResourceTypes mvceditor::ResourceFinderClass::GetResourceType() const {
	return ResourceType;
}

mvceditor::ResourceFinderClass::ResourceTypes mvceditor::ResourceFinderClass::ParseGoToResource(const wxString& resource, UnicodeString& fileName, 
		UnicodeString& className, UnicodeString& methodName, int& lineNumber) {
	fileName = UNICODE_STRING_SIMPLE("");
	className = UNICODE_STRING_SIMPLE("");
	methodName = UNICODE_STRING_SIMPLE("");
	lineNumber = 0;
	ResourceTypes resourceType = CLASS_NAME;

	// :: => Class::method
	// \ => \namespace\namespace
	// : => filename : line number
	int scopePos = resource.find(wxT("::"));
	int namespacePos = resource.find(wxT("\\"));
	int colonPos = resource.find(wxT(":"));
	if (scopePos >= 0) {
		className = StringHelperClass::wxToIcu(resource.substr(0, scopePos));
		methodName = StringHelperClass::wxToIcu(resource.substr(scopePos + 2, -1));
		resourceType = CLASS_NAME_METHOD_NAME;
	}
	else if (namespacePos >= 0) {
		className = StringHelperClass::wxToIcu(resource);
		resourceType = NAMESPACE_NAME;
	}
	else if (colonPos >= 0) {
		
		// : => filename : line number
		long int number;
		wxString after = resource.substr(colonPos + 1, -1);
		bool isNumber = after.ToLong(&number);
		lineNumber = isNumber ? number : 0;
		fileName = StringHelperClass::wxToIcu(resource.substr(0, colonPos));
		resourceType = FILE_NAME_LINE_NUMBER;
	}
	else {

		// class names can only have alphanumerics or underscores
		int pos = resource.find_first_of(wxT("`!@#$%^&*()+={}|\\:;\"',./?"));
		if (pos >= 0) {
			fileName = StringHelperClass::wxToIcu(resource);
			resourceType = FILE_NAME;
		}
		else {
			className = StringHelperClass::wxToIcu(resource);
			resourceType = CLASS_NAME;
		}
	}
	return resourceType;
}

void mvceditor::ResourceFinderClass::BuildResourceCache(const wxString& fullPath, bool parseClasses) {
	wxFileName fileName(fullPath);
	wxDateTime fileLastModifiedDateTime = fileName.GetModificationTime();
	
	// have we looked at this file yet or is cache out of date? if not, then build the cache.
	// we count the file as cached if (1) we have seen the file before (modified timestamp)
	bool cached = false;
	bool filePreviouslyCached = false;
	int fileItemIndex = -1;
	mvceditor::ResourceFinderClass::FileItem fileItem;
	bool foundFile = FindInFileCache(fullPath, fileItemIndex, fileItem);
	if (foundFile) {
		bool modified = fileLastModifiedDateTime.IsLaterThan(fileItem.DateTime);
		cached = !modified;
		FileCache[fileItemIndex].DateTime = fileLastModifiedDateTime;
		filePreviouslyCached = true;
	}
	else {
		fileItemIndex = PushIntoFileCache(fullPath, false, false);
		FileCache[fileItemIndex].DateTime = fileLastModifiedDateTime;
	}
	fileItem = FileCache[fileItemIndex];
	if (parseClasses) {
		if (!cached || !fileItem.Parsed) {
			FileCache[fileItemIndex].Parsed = true;

			// no need to look for resources if the file had not yet existed, this will save much time
			// this optimization was found by using the profiler
			if (filePreviouslyCached) {
				RemoveCachedResources(fileItemIndex);
			}
			CurrentFileItemIndex = fileItemIndex;
			
			// for now silently ignore files with parser errors
			pelet::LintResultsClass lintResults;
			wxFFile file(fullPath, wxT("rb"));
			Parser.ScanFile(file.fp(), mvceditor::StringHelperClass::wxToIcu(fullPath), lintResults);
			IsCacheSorted = false;
		}
	}
}

void mvceditor::ResourceFinderClass::ClassFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& signature, 
		const UnicodeString& comment, const int lineNumber) {
	ResourceClass classItem;
	classItem.Identifier = className;
	classItem.ClassName = className;
	classItem.NamespaceName = namespaceName;
	classItem.Key = className;
	classItem.Type = ResourceClass::CLASS;
	classItem.FileItemIndex = CurrentFileItemIndex;
	classItem.Signature = signature;
	classItem.ReturnType = UNICODE_STRING_SIMPLE("");
	classItem.Comment = comment;
	classItem.IsNative = false;
	IdentifierCache.push_back(classItem);
		
	// a resource for the namespace itself
	ResourceClass namespaceItem;
	namespaceItem.NamespaceName = namespaceName;
	namespaceItem.Identifier = namespaceName;
	namespaceItem.Key = namespaceName;
	if (!namespaceName.isEmpty() && std::find(NamespaceCache.begin(), NamespaceCache.end(), namespaceItem) == NamespaceCache.end()) {
		NamespaceCache.push_back(namespaceItem);
	}

	classItem.Identifier = QualifyName(namespaceName, className);
	classItem.Key = QualifyName(namespaceName, className);
	NamespaceCache.push_back(classItem);

}

void mvceditor::ResourceFinderClass::TraitAliasFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& traitUsedClassName,
												  const UnicodeString& traitMethodName, const UnicodeString& alias, pelet::TokenClass::TokenIds visibility) {
	std::vector<mvceditor::TraitResourceClass> traitResources = TraitCache[QualifyName(namespaceName, className)];
	for (size_t i = 0; i < traitResources.size(); ++i) {
		if (traitResources[i].TraitClassName.caseCompare(traitUsedClassName, 0) == 0) {
			traitResources[i].Aliased.push_back(alias);
		}
	}
	
	// since traitResources was copied not a reference
	TraitCache[QualifyName(namespaceName, className)] = traitResources;
	
	// put a non-qualified version too, sometimes the query will not contain a fully qualified name
	TraitCache[className] = traitResources;
	
	if (!alias.isEmpty()) {
	
		// write the alias as a resource on the class in scope since alias will not show up in the trait methods
		// put the alias in the current class as opposed to the trait class, since the same method may be aliased
		// by 2 different classes that use the trait
		ResourceClass item;
		item.ClassName = className;
		item.Identifier = alias;
		item.NamespaceName = namespaceName;
		item.Key = alias;
		item.Type = ResourceClass::METHOD;
		item.FileItemIndex = CurrentFileItemIndex;

		// probably should get these from the original (trait)
		//if (!returnType.isEmpty()) {
		//	item.Signature = returnType + UNICODE_STRING_SIMPLE(" ");
		//}
		//item.Signature
		//item.ReturnType 
		//item.Comment;
		switch (visibility) {
		case pelet::TokenClass::PROTECTED:
			item.IsProtected = true;
			break;
		case pelet::TokenClass::PRIVATE:
			item.IsPrivate = true;
			break;
		default:
			break;
		}
		item.IsStatic = false;
		item.IsNative = false;
		///MembersCache.push_back(item);
		
		///item.Key = className + UNICODE_STRING_SIMPLE("::") + alias;
		//MembersCache.push_back(item);
	}
}

void mvceditor::ResourceFinderClass::TraitInsteadOfFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& traitUsedClassName,
													   const UnicodeString& traitMethodName, const std::vector<UnicodeString>& insteadOfList) {
	
	std::vector<mvceditor::TraitResourceClass> traitResources = TraitCache[QualifyName(namespaceName, className)];
	for (size_t i = 0; i < traitResources.size(); ++i) {
		if (traitResources[i].TraitClassName.caseCompare(traitUsedClassName, 0) == 0) {
			for (size_t j = 0; j < insteadOfList.size(); ++j) {
				traitResources[i].Excluded.push_back(insteadOfList[j]);
			}
		}
	}
	
	// since traitResources was copied not a reference
	TraitCache[QualifyName(namespaceName, className)] = traitResources;
	
	// put a non-qualified version too, sometimes the query will not contain a fully qualified name
	TraitCache[className] = traitResources;
}

void mvceditor::ResourceFinderClass::TraitUseFound(const UnicodeString& namespaceName, const UnicodeString& className, 
												const UnicodeString& fullyQualifiedTraitName) {
	mvceditor::TraitResourceClass newTraitResource;
	newTraitResource.TraitClassName = fullyQualifiedTraitName;
	
	// only add if not already there
	bool found = false;
	std::vector<mvceditor::TraitResourceClass> traitResources = TraitCache[QualifyName(namespaceName, className)];
	for (size_t i = 0; i < traitResources.size(); ++i) {
		if (traitResources[i].TraitClassName.caseCompare(fullyQualifiedTraitName, 0) == 0) {
			found = true;
			break;
		}
	}
	if (!found) {
		TraitCache[QualifyName(namespaceName, className)].push_back(newTraitResource);
		
		// put a non-qualified version too, sometimes the query will not contain a fully qualified name
		TraitCache[className].push_back(newTraitResource);
	}
}

void mvceditor::ResourceFinderClass::DefineDeclarationFound(const UnicodeString& variableName, 
		const UnicodeString& variableValue, const UnicodeString& comment, const int lineNumber) {
	ResourceClass defineItem;
	defineItem.Identifier = variableName;
	defineItem.Key = variableName;
	defineItem.Type = ResourceClass::DEFINE;
	defineItem.FileItemIndex = CurrentFileItemIndex;
	defineItem.Signature = variableValue;
	defineItem.ReturnType = UNICODE_STRING_SIMPLE("");
	defineItem.Comment = comment;
	defineItem.IsNative = false;
	IdentifierCache.push_back(defineItem);
}

void mvceditor::ResourceFinderClass::MethodFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName,
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment,
		pelet::TokenClass::TokenIds visibility, bool isStatic, const int lineNumber) {
	ResourceClass item;
	item.Identifier = methodName;
	item.ClassName = className;
	item.NamespaceName = namespaceName;
	item.Key = methodName;
	item.Type = ResourceClass::METHOD;
	item.FileItemIndex = CurrentFileItemIndex;
	if (!returnType.isEmpty()) {
		item.Signature = returnType + UNICODE_STRING_SIMPLE(" ");
	}

	item.Signature += signature;
	item.ReturnType = returnType;
	item.Comment = comment;
	switch (visibility) {
	case pelet::TokenClass::PROTECTED:
		item.IsProtected = true;
		break;
	case pelet::TokenClass::PRIVATE:
		item.IsPrivate = true;
		break;
	default:
		break;
	}
	item.IsStatic = isStatic;
	item.IsNative = false;
	IdentifierCache.push_back(item);
	
	// insert a complete name so that we can quickly lookup all methods for a single class
	item.Key = className + UNICODE_STRING_SIMPLE("::") + methodName;
	MembersCache.push_back(item);
}

void mvceditor::ResourceFinderClass::PropertyFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& propertyName,
                                        const UnicodeString& propertyType, const UnicodeString& comment, 
										pelet::TokenClass::TokenIds visibility, bool isConst, bool isStatic,
										const int lineNumber) {
	UnicodeString filteredProperty(propertyName);
	if (!isStatic) {

		// remove the siguil from the property name when the variable is not static;
		// because when using non-static access ("->") the siguil is not used
		// this affects the code completion functionality
		filteredProperty.findAndReplace(UNICODE_STRING_SIMPLE("$"), UNICODE_STRING_SIMPLE(""));
	}
	ResourceClass item;
	item.Identifier = filteredProperty;
	item.ClassName = className;
	item.NamespaceName = namespaceName;
	item.Key = filteredProperty;
	item.Type = isConst ? ResourceClass::CLASS_CONSTANT : ResourceClass::MEMBER;
	item.FileItemIndex = CurrentFileItemIndex;
	item.Signature =  className + UNICODE_STRING_SIMPLE("::") + filteredProperty;
	item.ReturnType = propertyType;
	item.Comment = comment;
	switch (visibility) {
	case pelet::TokenClass::PROTECTED:
		item.IsProtected = true;
		break;
	case pelet::TokenClass::PRIVATE:
		item.IsPrivate = true;
		break;
	default:
		break;
	}
	item.IsStatic = isStatic;
	item.IsNative = false;
	IdentifierCache.push_back(item);
	
	// insert a complete name so that we can quickly lookup all methods for a single class
	item.Key = className + UNICODE_STRING_SIMPLE("::") + filteredProperty;
	MembersCache.push_back(item);
}

void mvceditor::ResourceFinderClass::FunctionFound(const UnicodeString& namespaceName, const UnicodeString& functionName, const UnicodeString& signature, 
		const UnicodeString& returnType, const UnicodeString& comment, const int lineNumber) {
	ResourceClass item;
	item.Identifier = functionName;
	item.NamespaceName = namespaceName;
	item.Key = functionName;
	item.Type = ResourceClass::FUNCTION;
	item.FileItemIndex = CurrentFileItemIndex;
	item.Signature = signature;
	item.ReturnType = returnType;
	item.Comment = comment;
	item.IsNative = false;
	IdentifierCache.push_back(item);
		
	ResourceClass namespaceItem;
	namespaceItem.NamespaceName = namespaceName;
	namespaceItem.Identifier = namespaceName;
	namespaceItem.Key = namespaceName;
	if (!namespaceName.isEmpty() && std::find(NamespaceCache.begin(), NamespaceCache.end(), namespaceItem) == NamespaceCache.end()) {
		NamespaceCache.push_back(namespaceItem);
	}
		
	// put in the namespace cache so that qualified name lookups work too
	item.Identifier = QualifyName(namespaceName, functionName);
	item.Key = QualifyName(namespaceName, functionName);
	NamespaceCache.push_back(item);
}

int mvceditor::ResourceFinderClass::GetLineCountFromFile(const wxString& fullPath) const {
	int lineCount = 0;
	std::ifstream file;
	file.open(fullPath.fn_str(), std::ios::binary);
	if (file.good()) {
		char buffer[512];
		file.rdbuf()->pubsetbuf(buffer, sizeof(buffer));
		char ch[2];
		file.read(ch, 2);
		int read = file.gcount();
		while (read) {

			// produce accurate line counts in unix, windows, and mac created files
			// ATTN: code not unicode safe
			if ('\r' == ch[0] && '\n' == ch[1] && 2 == read) {
				++lineCount;
			}
			else if ('\r' == ch[0]) {
				++lineCount;
				file.putback(ch[1]);
			}
			else if ('\r' == ch[1]) {
				++lineCount;
			}
			else if ('\n' == ch[0]) {
				++lineCount;
				file.putback(ch[1]);
			}
			else if ('\n' == ch[1]) {
				++lineCount;
			}
			file.read(ch, 2);
			read = file.gcount();
		}
		file.close();
	}
	return lineCount;
}

void mvceditor::ResourceFinderClass::RemoveCachedResources(int fileItemIndex) {
	std::vector<ResourceClass>::iterator it = IdentifierCache.begin();
	while (it != IdentifierCache.end()) {
		if (it->FileItemIndex == fileItemIndex) {
			it = IdentifierCache.erase(it);
		}
		else {
			++it;
		}
	}
	it = MembersCache.begin();
	while (it != MembersCache.end()) {
		if (it->FileItemIndex == fileItemIndex) {
			it = MembersCache.erase(it);
		}
		else {
			++it;
		}
	}
	it = NamespaceCache.begin();
	while (it != NamespaceCache.end()) {
		if (it->FileItemIndex == fileItemIndex) {
			it = NamespaceCache.erase(it);
		}
		else {
			++it;
		}
	}
}

UnicodeString mvceditor::ResourceFinderClass::ExtractParentClassFromSignature(const UnicodeString& signature) const {

	// look for the parent class. note that the lexer has consumed any extra spaces, so it is safe
	// to assumes that the signature of the class contains only one space.
	UnicodeString parentClassName;
	int32_t extendsPos = signature.indexOf(UNICODE_STRING_SIMPLE("extends "));
	if (0 <= extendsPos) {
		int extendsEndPos =  signature.indexOf(UNICODE_STRING_SIMPLE(" "), extendsPos + 1);
		if (0 > extendsEndPos) {
			extendsEndPos = signature.length();
		}
		// 8 = length of 'extends '
		signature.extract(extendsPos + 8, signature.length() - extendsEndPos, parentClassName);
		parentClassName = parentClassName.trim();
	}
	return parentClassName;
}

bool mvceditor::ResourceFinderClass::CollectFullyQualifiedResource() {
	EnsureSorted();
	ResourceClass needle;
	if (ResourceType == CLASS_NAME_METHOD_NAME) {

		// check the entire class hierachy; stop as soon as we found it
		std::vector<UnicodeString> classHierarchy = ClassHierarchy(ClassName);
		for (size_t i = 0; i < classHierarchy.size(); ++i) {
			
			needle.Key = classHierarchy[i] + UNICODE_STRING_SIMPLE("::") + MethodName;
			std::vector<ResourceClass>::iterator it = std::lower_bound(MembersCache.begin(), MembersCache.end(), needle);
			
			// members are sorted by identifers; need to acount for the case when the same method name
			// is used in multiple classes
			int classCount = 0;
			ResourceClass exactMatchResource;
			while (it != MembersCache.end() && it->Identifier.caseCompare(MethodName, 0) == 0) {
				if (it->ClassName.caseCompare(classHierarchy[i], 0) == 0) {
					classCount++;
					exactMatchResource = *it;
				}
				++it;
			}
			if (1 == classCount) {
				Matches.push_back(exactMatchResource);
				break;
			}
		}
	}
	else if (NAMESPACE_NAME == ResourceType) {
		needle.Key = ClassName;
		std::vector<ResourceClass>::iterator it = std::lower_bound(NamespaceCache.begin(), NamespaceCache.end(), needle);
		if (it != NamespaceCache.end() && it->Key.caseCompare(needle.Key, 0) == 0) {
			ResourceClass exactMatchResource = *it;
			++it;
			
			// make sure there is one and only one item that matches the search.
			if (it == NamespaceCache.end() || it->Key.caseCompare(needle.Key, 0) > 0) {
				Matches.push_back(exactMatchResource);
			}
		}
	}
	else {
		needle.Key = ClassName;
		std::vector<ResourceClass>::iterator it = std::lower_bound(IdentifierCache.begin(), IdentifierCache.end(), needle);
		if (it != IdentifierCache.end() && it->Identifier.caseCompare(ClassName, 0) == 0) {
			ResourceClass exactMatchResource = *it;
			++it;
			
			// make sure there is one and only one item that matches the search.
			if (it == IdentifierCache.end() || it->Identifier.caseCompare(ClassName, 0) > 0) {
				Matches.push_back(exactMatchResource);
			}
		}
	}
	EnsureMatchesExist();
	return !Matches.empty();
}

bool mvceditor::ResourceFinderClass::IsTraitInherited(const mvceditor::ResourceClass& memberResource, const UnicodeString& fullyQualifiedClassName) {
	bool match = false;
	std::vector<mvceditor::TraitResourceClass> traits = TraitCache[fullyQualifiedClassName];
	for (size_t i = 0; i < traits.size(); ++i) {
		UnicodeString traitClassName = traits[i].TraitClassName;
		if (memberResource.ClassName.caseCompare(traitClassName, 0) == 0) {
			
			// trait is used unless there is an explicit insteadof 
			match = true;
			for (size_t j = 0; j < traits[i].Excluded.size(); ++j) {
				if (traits[i].Excluded[j].caseCompare(fullyQualifiedClassName, 0) == 0) {
					match = false;
					break;
				}
			}
			break;
		}
	}
	
	return match;
}

void mvceditor::ResourceFinderClass::EnsureMatchesExist() {
	std::vector<int> files;
	for(std::vector<ResourceClass>::iterator it = Matches.begin(); it != Matches.end(); ++it) {

		// set the full path; initially the cache only holds the FileItemIndex to preserve space
		// this is needed for ResourceClass::GetFullPath() to work as promised
		it->FullPath = GetResourceMatchFullPathFromResource(*it);

		// FileItemIndex is meaningless for dynamic resources
		if (!it->IsDynamic) {
			files.push_back(it->FileItemIndex);
		}
	}
	unique(files.begin(), files.end());
	for(std::vector<int>::const_iterator it = files.begin(); it != files.end(); ++it) {
		int fileItemIndex = *it;
		if (fileItemIndex >= 0 && fileItemIndex < (int)FileCache.size()) {

			// is a file is new it wont be on disk; results are not stale.
			if (!FileCache[fileItemIndex].IsNew && !wxFileName::FileExists(FileCache[fileItemIndex].FullPath)) {
				
				// file is gone. remove all cached resources
				RemoveCachedResources(fileItemIndex);
				
				// remove from matches
				std::vector<ResourceClass>::iterator it = Matches.begin();
				while (it != Matches.end()) {
					if (it->FileItemIndex == fileItemIndex) {
						it = Matches.erase(it);
					}
					else {
						++it;
					}
				}
			}
		}
	}
}

void mvceditor::ResourceFinderClass::BoundedCacheSearch(const std::vector<mvceditor::ResourceClass>& cache,
		const UnicodeString& key,
		std::vector<mvceditor::ResourceClass>& matches, int maxMatches)  const {
	int matchCount = 0;
	mvceditor::ResourceClass needle;
	needle.Key = key;
	std::vector<mvceditor::ResourceClass>::const_iterator it =  std::lower_bound(cache.begin(), cache.end(), needle);
	if (it != cache.end()) {
		
		// collect near matches
		while (it != cache.end() && mvceditor::CaseStartsWith(it->Key, key)) {
			matches.push_back(*it);
			matchCount++;
			if (matchCount > maxMatches) {
				break;
			}
			++it;
		}
	}
}

void mvceditor::ResourceFinderClass::Print() const {
	UFILE *out = u_finit(stdout, NULL, NULL);
	u_fprintf(out, "LookingFor=%.*S,%.*S\n", ClassName.length(), ClassName.getBuffer(), MethodName.length(), MethodName.getBuffer());
	for (std::vector<ResourceClass>::const_iterator it = IdentifierCache.begin(); it != IdentifierCache.end(); ++it) {
		u_fprintf(out, "RESOURCE: Key=%.*S Identifier=%.*S ClassName=%.*S Namespace=%.*S Type=%d\n",
			it->Key.length(), it->Key.getBuffer(),
			it->Identifier.length(), it->Identifier.getBuffer(),  
			it->ClassName.length(), it->ClassName.getBuffer(),
			it->NamespaceName.length(), it->NamespaceName.getBuffer(),
			it->Type);
	}
	for (std::vector<ResourceClass>::const_iterator it = MembersCache.begin(); it != MembersCache.end(); ++it) {
		u_fprintf(out, "MEMBER: Key=%.*S Identifier=%.*S ClassName=%.*S Namespace=%.*S ReturnType=%.*S Type=%d\n", 
			it->Key.length(), it->Key.getBuffer(),
			it->Identifier.length(), it->Identifier.getBuffer(),  
			it->ClassName.length(), it->ClassName.getBuffer(),
			it->NamespaceName.length(), it->NamespaceName.getBuffer(),
			it->ReturnType.length(), it->ReturnType.getBuffer(), it->Type);
	}
	for (std::vector<ResourceClass>::const_iterator it = NamespaceCache.begin(); it != NamespaceCache.end(); ++it) {
		u_fprintf(out, "NAMESPACE:Key=%.*S Identifier=%.*S ClassName=%.*S Namespace=%.*S  Type=%d\n", 
			it->Key.length(), it->Key.getBuffer(),
			it->Identifier.length(), it->Identifier.getBuffer(),  
			it->ClassName.length(), it->ClassName.getBuffer(),
			it->NamespaceName.length(), it->NamespaceName.getBuffer(),
			it->Type);
	}
	for (std::map<UnicodeString, std::vector<mvceditor::TraitResourceClass>, UnicodeStringComparatorClass>::const_iterator it = TraitCache.begin(); it != TraitCache.end(); ++it) {
		u_fprintf(out, "TRAITS USED BY: %.*S\n",
			it->first.length(), it->first.getBuffer());
		std::vector<mvceditor::TraitResourceClass> traits =  it->second;
		for (size_t j = 0; j < traits.size(); ++j) {
			mvceditor::TraitResourceClass trait = traits[j];
			u_fprintf(out, "\tTRAIT NAME=%.*S\n",
				trait.TraitClassName.length(), trait.TraitClassName.getBuffer());
			for (size_t k = 0; k < trait.Aliased.size(); k++) {
				u_fprintf(out, "\tALIASES METHOD=%S\n", trait.Aliased[k].getTerminatedBuffer());
			}
			for (size_t k = 0; k < trait.Excluded.size(); k++) {
				u_fprintf(out, "\tEXCLUDED METHOD=%S\n", trait.Excluded[k].getTerminatedBuffer());
			}
		}		
		
	}
	u_fclose(out);
}

bool mvceditor::ResourceFinderClass::IsFileCacheEmpty() const {
	return 0 == FileCache.size();
}

bool mvceditor::ResourceFinderClass::IsResourceCacheEmpty() const {
	bool isEmpty = IdentifierCache.empty() && MembersCache.empty();
	if (!isEmpty) {
		isEmpty = true;

		// make sure only parsed resource came from the native functions file.
		for (std::vector<mvceditor::ResourceClass>::const_iterator it = IdentifierCache.begin(); isEmpty && it != IdentifierCache.end(); ++it) {
			if (!it->IsNative) {
				isEmpty = false;
			}
		}
		if (isEmpty) {
			for (std::vector<mvceditor::ResourceClass>::const_iterator it = MembersCache.begin(); isEmpty && it != MembersCache.end(); ++it) {
				if (!it->IsNative) {
					isEmpty = false;
				}
			}
		}
	}
	return isEmpty;
}

void mvceditor::ResourceFinderClass::CopyResourcesFrom(const mvceditor::ResourceFinderClass& src) {

	// since resource caches can be quite large, avoid using push_back
	IdentifierCache.resize(src.IdentifierCache.size());
	std::copy(src.IdentifierCache.begin(), src.IdentifierCache.end(), IdentifierCache.begin());
	
	MembersCache.resize(src.MembersCache.size());
	std::copy(src.MembersCache.begin(), src.MembersCache.end(), MembersCache.begin());
	
	NamespaceCache.resize(src.NamespaceCache.size());
	std::copy(src.NamespaceCache.begin(), src.NamespaceCache.end(), NamespaceCache.begin());
	
	FileCache.resize(src.FileCache.size());
	std::copy(src.FileCache.begin(), src.FileCache.end(), FileCache.begin());
	IsCacheSorted = src.IsCacheSorted;
}

void mvceditor::ResourceFinderClass::AddDynamicResources(const std::vector<mvceditor::ResourceClass>& dynamicResources) {
	for (std::vector<mvceditor::ResourceClass>::const_iterator it = dynamicResources.begin(); it != dynamicResources.end(); ++it) {
		mvceditor::ResourceClass resource = *it;
		if (mvceditor::ResourceClass::MEMBER == resource.Type || mvceditor::ResourceClass::METHOD == resource.Type) {
			
			// cannot assume cache is sorted
			// need to account for duplicates; if so then only update
			bool updated = false;
			for (std::vector<mvceditor::ResourceClass>::iterator itMember = MembersCache.begin(); itMember != MembersCache.end(); ++itMember) {
				if (itMember->ClassName == resource.ClassName && itMember->Identifier == resource.Identifier) {
					if (!resource.ReturnType.isEmpty()) {
						itMember->ReturnType = resource.ReturnType;
					}
					updated = true;
					break;
				}
			}
			if (!updated) {
				resource.IsDynamic = true;

				// we dont want these resource to point to a file; 
				// GetResourceMatchFullPathFromResource() and GetResourceMatchFullPath() will return empty for dynamic 
				// resources since there is no source code we can show the user.
				resource.FileItemIndex = -1;
				resource.Key = resource.Identifier;
				MembersCache.push_back(resource);
				
				// the fully qualified version
				resource.Key = resource.ClassName + UNICODE_STRING_SIMPLE("::") + resource.Identifier;
				MembersCache.push_back(resource);
				
				IsCacheSorted = false;
			}
		}
		else {

			// look at the class, function, cache
			bool updated = false;
			for (std::vector<mvceditor::ResourceClass>::iterator itResource = IdentifierCache.begin(); itResource != IdentifierCache.end(); ++itResource) {
				if (itResource->ClassName == resource.ClassName && itResource->Identifier == resource.Identifier) {
					if (!resource.ReturnType.isEmpty()) {
						itResource->ReturnType = resource.ReturnType;
					}
					updated = true;
					break;
				}
			}
			if (!updated) {
				resource.IsDynamic = true;

				// we dont want these resource to point to a file; 
				// GetResourceMatchFullPathFromResource() and GetResourceMatchFullPath() will return empty for dynamic 
				// resources since there is no source code we can show the user.
				resource.FileItemIndex = -1;
				resource.Key = resource.Identifier;
				IdentifierCache.push_back(resource);
				
				// the fully qualified version
				resource.Key = QualifyName(resource.NamespaceName, resource.ClassName);
				IdentifierCache.push_back(resource);
				
				IsCacheSorted = false;
			}
		}
	}
}

void mvceditor::ResourceFinderClass::UpdateResourcesFrom(const wxString& fullPath, const mvceditor::ResourceFinderClass& src) {
	int fileItemIndex = -1;
	mvceditor::ResourceFinderClass::FileItem fileItem;
	
	// check to see if we have cached the given file, if so then clear the file's resources
	bool foundFile = FindInFileCache(fullPath, fileItemIndex, fileItem);
	if (foundFile) {
		RemoveCachedResources(fileItemIndex);
	}

	// now add the new resources from src
	// we cannot copy them as is because FileCache indices may conflict
	for (std::vector<mvceditor::ResourceClass>::const_iterator it = src.IdentifierCache.begin(); it != src.IdentifierCache.end(); ++it) {
		if (it->FileItemIndex >= 0 && it->FileItemIndex < (int)src.FileCache.size()) {
			wxString resourceFullPath = src.FileCache[it->FileItemIndex].FullPath;
			fileItemIndex = -1;
			mvceditor::ResourceClass res = *it;
			foundFile = FindInFileCache(resourceFullPath, fileItemIndex, fileItem);
		
			// resource comes from am known file; need to change the index 
			if (foundFile) {				
				res.FileItemIndex = fileItemIndex;
			}
			else {

				// resource comes from a file that is not in this cache. need to add to the file cache 
				int newFileItemIndex = PushIntoFileCache(src.FileCache[it->FileItemIndex].FullPath, src.FileCache[it->FileItemIndex].Parsed, src.FileCache[it->FileItemIndex].IsNew);
				res.FileItemIndex = newFileItemIndex;
			}

			// put in the appropiate cache
			IdentifierCache.push_back(res);
		}
		else {
			IdentifierCache.push_back(*it);
		}
	}

	// same thing for MembersCache
	for (std::vector<mvceditor::ResourceClass>::const_iterator it = src.MembersCache.begin(); it != src.MembersCache.end(); ++it) {
		if (it->FileItemIndex >= 0 && it->FileItemIndex < (int)src.FileCache.size()) {
			wxString resourceFullPath = src.FileCache[it->FileItemIndex].FullPath;
			fileItemIndex = -1;
			mvceditor::ResourceClass res = *it;
			foundFile = FindInFileCache(resourceFullPath, fileItemIndex, fileItem);
		
			// resource comes from am known file; need to change the index 
			if (foundFile) {				
				res.FileItemIndex = fileItemIndex;
			}
			else {

				// resource comes from a file that is not in this cache. need to add to the file cache 
				int newFileItemIndex = PushIntoFileCache(src.FileCache[it->FileItemIndex].FullPath, src.FileCache[it->FileItemIndex].Parsed, src.FileCache[it->FileItemIndex].IsNew);
				res.FileItemIndex = newFileItemIndex;
			}

			// put in the appropiate cache
			MembersCache.push_back(res);
		}
		else {
			MembersCache.push_back(*it);
		}
	}
	
	// same thing for NamespaceCache
	for (std::vector<mvceditor::ResourceClass>::const_iterator it = src.NamespaceCache.begin(); it != src.NamespaceCache.end(); ++it) {
		if (it->FileItemIndex >= 0 && it->FileItemIndex < (int)src.FileCache.size()) {
			wxString resourceFullPath = src.FileCache[it->FileItemIndex].FullPath;
			fileItemIndex = -1;
			mvceditor::ResourceClass res = *it;
			foundFile = FindInFileCache(resourceFullPath, fileItemIndex, fileItem);
		
			// resource comes from am known file; need to change the index 
			if (foundFile) {				
				res.FileItemIndex = fileItemIndex;
			}
			else {

				// resource comes from a file that is not in this cache. need to add to the file cache 
				int newFileItemIndex = PushIntoFileCache(src.FileCache[it->FileItemIndex].FullPath, src.FileCache[it->FileItemIndex].Parsed, src.FileCache[it->FileItemIndex].IsNew);
				res.FileItemIndex = newFileItemIndex;
			}

			// put in the appropiate cache
			NamespaceCache.push_back(res);
		}
		else {
			NamespaceCache.push_back(*it);
		}
	}
}

bool mvceditor::ResourceFinderClass::LoadTagFile(const wxFileName& fileName, bool isNativeTags) {
	bool good = false;
	UFILE* uf = u_fopen(fileName.GetFullPath().ToAscii(), "rb", NULL, NULL);
	if (uf) {
		UChar buffer[512];
		mvceditor::ResourceClass res;
		while (u_fgets(buffer, 512, uf)) {
			if (UNICODE_STRING_SIMPLE("!_TAG_").caseCompare(buffer, 6, 0) != 0) {
				// dont worry about whether the tags file is sorted or not,
				// the cache will be sorted as needed.

				// do an empty line check here. we want to skip empty lines
				// but still process the rest of the file.
				bool isEmpty = true;
				for (int i = 0; i < u_strlen(buffer); ++i) {
					if (!u_isWhitespace(buffer[i])) {
						isEmpty = false;
						break;
					}
				}
				if (!isEmpty) {

					// skip the comment tags and any other non-tags
					good = ParseTag(res, buffer);
					if (good) {
						res.IsNative = isNativeTags;
						res.Key = res.Identifier;
						IdentifierCache.push_back(res);
						switch (res.Type) {
						case mvceditor::ResourceClass::CLASS:
						case mvceditor::ResourceClass::FUNCTION:
						case mvceditor::ResourceClass::NAMESPACE:
							
							// the fully qualified version
							res.Identifier =  QualifyName(res.NamespaceName, res.Identifier);
							res.Key = res.Identifier;
							NamespaceCache.push_back(res);
							break;
						case mvceditor::ResourceClass::MEMBER:
						case mvceditor::ResourceClass::METHOD:
						case mvceditor::ResourceClass::CLASS_CONSTANT:

							// the fully scoped version
							res.Key = res.ClassName + UNICODE_STRING_SIMPLE("::") + res.Identifier;;
							MembersCache.push_back(res);
							break;
						case mvceditor::ResourceClass::DEFINE:
							// PHP does not define any namespaced constants?
							break;
						}
					}
				}
			}
		}
		u_fclose(uf);
	}
	return good;
}

void mvceditor::ResourceFinderClass::EnsureSorted() {
	Matches.clear();
	Matches.reserve(10);
	if (!IsCacheSorted) {
		std::sort(IdentifierCache.begin(), IdentifierCache.end());
		std::sort(MembersCache.begin(), MembersCache.end());
		std::sort(NamespaceCache.begin(), NamespaceCache.end());

		// if a dyamic resource has been added and is a dup; merge it with the corresponding 'real' resource
		std::vector<mvceditor::ResourceClass>::iterator itDynamic = IdentifierCache.begin();
		while (itDynamic != IdentifierCache.end()) {
			bool erased = false;
			if (itDynamic->IsDynamic) {
				ResourceClass needle;
				needle.Identifier = itDynamic->Identifier;
				std::vector<mvceditor::ResourceClass>::iterator found =  std::lower_bound(IdentifierCache.begin(), IdentifierCache.end(), needle);
				while (found != IdentifierCache.end() &&
						found->ClassName == itDynamic->ClassName && found->Identifier == itDynamic->Identifier &&
						found->NamespaceName == itDynamic->NamespaceName &&
						found->Type == itDynamic->Type) {
					if (!found->IsDynamic) {

						// merge by copying the ReturnType from the dynamic resource then 
						// delete the dynamic resource
						found->ReturnType = itDynamic->ReturnType;
						itDynamic = IdentifierCache.erase(itDynamic);
						erased = true;
						break;
					}
					++found;
				}
			}
			if (!erased) {
				++itDynamic;
			}
		}
		itDynamic = MembersCache.begin();
		while (itDynamic != MembersCache.end()) {
			bool erased = false;
			if (itDynamic->IsDynamic) {	
				ResourceClass needle;
				needle.Identifier = itDynamic->Identifier;
				std::vector<mvceditor::ResourceClass>::iterator found =  std::lower_bound(MembersCache.begin(), MembersCache.end(), needle);
				while (found != MembersCache.end() &&
						found->ClassName == itDynamic->ClassName && found->Identifier == itDynamic->Identifier &&
						found->NamespaceName == itDynamic->NamespaceName &&
						found->Type == itDynamic->Type) {
					if (!found->IsDynamic) {

						// merge by copying the ReturnType from the dynamic resource then 
						// delete the dynamic resource
						found->ReturnType = itDynamic->ReturnType;
						itDynamic = MembersCache.erase(itDynamic);
						erased = true;
						break;
					}
					++found;
				}
			}
			if (!erased) {
				++itDynamic;
			}
		}
		IsCacheSorted = true;
	}
}

int mvceditor::ResourceFinderClass::PushIntoFileCache(const wxString& fullPath, bool isParsed, bool isNew) {
	int newFileItemIndex = FileCache.size();
	mvceditor::ResourceFinderClass::FileItem fileItem;
	fileItem.FullPath = fullPath;
	fileItem.Parsed = isParsed;
	fileItem.IsNew = isNew;
	FileCache.push_back(fileItem);
	return newFileItemIndex;
}

bool mvceditor::ResourceFinderClass::FindInFileCache(const wxString& fullPath, int& fileItemIndex, mvceditor::ResourceFinderClass::FileItem& fileItem) const {
	bool foundFile = false;
	for (size_t i = 0; i < FileCache.size(); ++i) {
		if (FileCache[i].FullPath == fullPath) {
			fileItemIndex = i;
			fileItem = FileCache[i];
			foundFile = true;
			break;
		}
	}
	return foundFile;
}

std::vector<wxString> mvceditor::ResourceFinderClass::GetCachedFiles() const {
	std::vector<wxString> files;
	for (size_t i = 0; i < FileCache.size(); ++i) {
		files.push_back(FileCache[i].FullPath);
	}
	return files;
}

bool mvceditor::ResourceFinderClass::Persist(const wxFileName& outputFile) const {
	if (!outputFile.IsOk()) {
		return false;
	}

	wxFFile file(outputFile.GetFullPath(), wxT("wb"));
	if (!file.IsOpened()) {
		return false;
	}
	UFILE* uf = u_finit(file.fp(), NULL, NULL);
	if (!uf) {
		return false;
	}
	int32_t written;
	bool error = false;
	std::vector<mvceditor::ResourceClass>::const_iterator it;
	for (it = IdentifierCache.begin(); it != IdentifierCache.end() && !error; ++it) {
		if (it->FileItemIndex >= 0) {

			// watch out for dynamic resources and 'native' functions
			// we dont want to persist those for now 
			if (mvceditor::ResourceClass::CLASS == it->Type && !it->IsNative) {
				UnicodeString uniFile = mvceditor::StringHelperClass::wxToIcu(FileCache[it->FileItemIndex].FullPath);
				written = u_fprintf(uf, "CLASS,%S,%.*S,\n", 
					uniFile.getTerminatedBuffer(), it->ClassName.length(), it->ClassName.getBuffer());
				error = 0 == written;
			}
			else if (mvceditor::ResourceClass::FUNCTION == it->Type && !it->IsNative) {
				UnicodeString uniFile = mvceditor::StringHelperClass::wxToIcu(FileCache[it->FileItemIndex].FullPath);
				written = u_fprintf(uf, "FUNCTION,%S,%.*S,\n", 
					uniFile.getTerminatedBuffer(), it->Identifier.length(), it->Identifier.getBuffer());
				error = 0 == written;
			}
		}
	}
	for (it = MembersCache.begin(); it != MembersCache.end() && !error; ++it) {
		if (it->FileItemIndex >= 0) {
			UnicodeString className(it->ClassName);

			// watch out for dynamic resources and 'native' functions
			// we dont want to persist those for now 
			if (mvceditor::ResourceClass::MEMBER == it->Type && !it->IsNative) {
				UnicodeString uniFile = mvceditor::StringHelperClass::wxToIcu(FileCache[it->FileItemIndex].FullPath);
				written = u_fprintf(uf, "MEMBER,%S,%S,%.*S\n", 
					uniFile.getTerminatedBuffer(), 
					className.getTerminatedBuffer(),
					it->Identifier.length(), it->Identifier.getBuffer());
				error = 0 == written;
			}
			else if (mvceditor::ResourceClass::METHOD == it->Type && !it->IsNative) {
				UnicodeString uniFile = mvceditor::StringHelperClass::wxToIcu(FileCache[it->FileItemIndex].FullPath);
				written = u_fprintf(uf, "METHOD,%S,%S,%.*S\n", 
					uniFile.getTerminatedBuffer(), 
					className.getTerminatedBuffer(),
					it->Identifier.length(), it->Identifier.getBuffer());
				error = 0 == written;
			}
		}
	}
	u_fclose(uf);
	return !error;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::All() const {
	std::vector<mvceditor::ResourceClass> all;
	all.resize(IdentifierCache.size() + MembersCache.size());
	std::vector<mvceditor::ResourceClass>::const_iterator it;
	size_t index = 0;
	for (it = IdentifierCache.begin(); it != IdentifierCache.end(); ++it, ++index) {
		all[index] = *it;
	}
	for (it = MembersCache.begin(); it != MembersCache.end(); ++it, ++index) {
		all[index] = *it;
	}
	return all;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::AllNonNativeClasses() const {
	std::vector<mvceditor::ResourceClass> all;
	all.reserve(IdentifierCache.size());
	std::vector<mvceditor::ResourceClass>::const_iterator it;
	for (it = IdentifierCache.begin(); it != IdentifierCache.end(); ++it) {
		if (mvceditor::ResourceClass::CLASS == it->Type && !it->IsNative) {
			all.push_back(*it);
		}
	}
	return all;
}

void mvceditor::ResourceFinderClass::Clear() {
	IdentifierCache.clear();
	MembersCache.clear();
	NamespaceCache.clear();
	FileCache.clear();
	Matches.clear();
	IsCacheSorted = false;
}

mvceditor::ResourceClass::ResourceClass()
	: Identifier()
	, ClassName()
	, NamespaceName()
	, Signature()
	, ReturnType()
	, Comment()
	, Type(CLASS) 
	, IsProtected(false)
	, IsPrivate(false) 
	, IsStatic(false)
	, IsDynamic(false)
	, IsNative(false)
	, FullPath()
	, Key()
	, FileItemIndex(-1) {
		
}

void mvceditor::ResourceClass::operator=(const ResourceClass& src) {
	Identifier = src.Identifier;
	ClassName = src.ClassName;
	NamespaceName = src.NamespaceName;
	Signature = src.Signature;
	ReturnType = src.ReturnType;
	Comment = src.Comment;
	Type = src.Type;
	FullPath = src.FullPath;
	Key = src.Key;
	FileItemIndex = src.FileItemIndex;
	IsProtected = src.IsProtected;
	IsPrivate = src.IsPrivate;
	IsStatic = src.IsStatic;
	IsDynamic = src.IsDynamic;
	IsNative = src.IsNative;
}

bool mvceditor::ResourceClass::operator<(const mvceditor::ResourceClass& a) const {
	return Key.caseCompare(a.Key, 0) < 0;
}

bool mvceditor::ResourceClass::operator==(const mvceditor::ResourceClass& a) const {
	return Identifier == a.Identifier && ClassName == a.ClassName && NamespaceName == a.NamespaceName;
} 

wxString mvceditor::ResourceClass::GetFullPath() const {
	return FullPath;
}

bool mvceditor::ResourceClass::IsKeyEqualTo(const UnicodeString& key) const {
	return Key.caseCompare(key, 0) == 0;
}

void mvceditor::ResourceClass::Clear() {
	Identifier.remove();
	ClassName.remove();
	NamespaceName.remove();
	Signature.remove();
	ReturnType.remove();
	Comment.remove();
	Type = CLASS;
	FileItemIndex = -1;
	FullPath = wxT("");
	Key.remove();
	IsProtected = false;
	IsPrivate = false;
	IsStatic = false;
	IsDynamic = false;
	IsNative = false;
}

mvceditor::TraitResourceClass::TraitResourceClass() 
	: TraitClassName()
	, Aliased()
	, Excluded() {
		
}