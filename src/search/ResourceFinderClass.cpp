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


mvceditor::ResourceFinderClass::ResourceFinderClass()
	: FileFilters()
	, ResourceCache()
	, MembersCache()
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
	, IsCacheSorted(false)
	, IsCurrentFileNative(false) {
	Parser.SetClassObserver(this);
	Parser.SetClassMemberObserver(this);
	Parser.SetFunctionObserver(this);
}

bool mvceditor::ResourceFinderClass::Walk(const wxString& fileName) {
	bool matchedFilter = false;
	for (size_t i = 0; i < FileFilters.size(); ++i) {
		wxString filter = FileFilters[i];
		matchedFilter = !wxIsWild(filter) || wxMatchWild(filter, fileName);
		if (matchedFilter) {
			break;
		}
	}
	IsCurrentFileNative = false;
	if (matchedFilter) {
		switch (ResourceType) {
			case FILE_NAME:
			case FILE_NAME_LINE_NUMBER:
				BuildResourceCache(fileName, false);
				break;
			case CLASS_NAME:
			case CLASS_NAME_METHOD_NAME:
				BuildResourceCache(fileName, true);
				break;
		}
	}

	// no need to keep know which files have resources, most likely all files should have a resource
	return false;
}

void mvceditor::ResourceFinderClass::BuildResourceCacheForFile(const wxString& fullPath, const UnicodeString& code, bool isNew) {
	wxFileName nativeFunctionsFile = mvceditor::NativeFunctionsAsset();
	IsCurrentFileNative = fullPath.CmpNoCase(nativeFunctionsFile.GetFullPath()) == 0;

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
	mvceditor::LintResultsClass results;
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
	int scopeResolutionPos = resource.Resource.indexOf(UNICODE_STRING_SIMPLE("::"));
	if (scopeResolutionPos >= 0) {
		className.setTo(resource.Resource, 0, scopeResolutionPos);
		methodName.setTo(resource.Resource, scopeResolutionPos + 2, resource.Resource.length() - scopeResolutionPos + 2 - 1);

		mvceditor::FinderClass::EscapeRegEx(className);
		mvceditor::FinderClass::EscapeRegEx(methodName);
	}
	else {
		className = resource.Resource;
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

void mvceditor::ResourceFinderClass::BuildResourceCacheForNativeFunctions() {
	wxFileName fileName = mvceditor::NativeFunctionsAsset();	
	if (fileName.FileExists()) {
		IsCurrentFileNative = true;
		BuildResourceCache(fileName.GetFullPath(), true);
	}
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
	ResourceClass needle;
	needle.Identifier = ClassName;
	std::list<ResourceClass>::iterator it =  std::lower_bound(ResourceCache.begin(), ResourceCache.end(), needle);
	if (it != ResourceCache.end()) {
		
		// if there are exact matches; we only want to collect exact matches
		if(it->Identifier.caseCompare(needle.Identifier, 0) == 0) {
			bool exactMatches = true;
			while (it != ResourceCache.end() && exactMatches) {
				exactMatches = it->Identifier.caseCompare(needle.Identifier, 0) == 0;
				if (exactMatches) {
					ResourceClass match = *it;
					Matches.push_back(match);
				}
				++it;
			}
		}
		else {
			
			// if no exact matches, the collect near matches
			UnicodeString needleToLower(needle.Identifier);
			needleToLower.toLower();
			bool nearMatches = true;
			while (it != ResourceCache.end() && nearMatches) {
				UnicodeString identifierToLower(it->Identifier);
				identifierToLower.toLower();
				nearMatches = identifierToLower.indexOf(needleToLower) == 0;
				if (nearMatches) {
					ResourceClass match = *it;
					Matches.push_back(match);
				}
				++it;
			}
		}
	}
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
	}
	else {
		ResourceClass needle;
		needle.Resource = ClassName + UNICODE_STRING_SIMPLE("::") + MethodName;
		needle.Identifier = MethodName;
		UnicodeString needleToLower(needle.Identifier);
		needleToLower.toLower();
		std::list<ResourceClass>::iterator it = std::lower_bound(MembersCache.begin(), MembersCache.end(), needle);	
		while (it != MembersCache.end()) {
			UnicodeString identifierToLower(it->Identifier);
			identifierToLower.toLower();
			if (identifierToLower.indexOf(needleToLower) != 0) {
				break;
			}
			bool exactMatches = false;

			// if there are exact matches; we only want to collect exact matches
			if (it->Identifier.caseCompare(needle.Identifier, 0) == 0) {
				
				// parentClassNames is a list of all classes ancestors of ClassName.  A match is found when the method
				// is from one of these parents.  This prevents matches from classes that may have the sam method name
				// but are not related.
				for (size_t i = 0; i < parentClassNames.size(); ++i) {
					UnicodeString possibleResource = parentClassNames[i] + UNICODE_STRING_SIMPLE("::") + needle.Identifier;
					exactMatches = it->Resource.caseCompare(possibleResource, 0) == 0;
					if (ClassName.isEmpty()) {
						
						// just check method names. This ensures queries like '::getName' will work as well.
						exactMatches = it->Identifier.caseCompare(needle.Identifier, 0) == 0;
					}
					if (exactMatches) {
						ResourceClass match = *it;
						Matches.push_back(match);
						break;
					}
				}
			}
			if (!exactMatches) {
				bool nearMatches = false;
			
				// parentClassNames is a list of all classes ancestors of ClassName.  A match is found when the method
				// is from one of these parents.  This prevents matches from classes that may have the sam method name
				// but are not related.
				for(size_t i = 0; i < parentClassNames.size(); ++i) {
					UnicodeString possibleResource = parentClassNames[i] + UNICODE_STRING_SIMPLE("::") + needleToLower;
					possibleResource.toLower();
					UnicodeString resourceToLower(it->Resource);
					resourceToLower.toLower();
					nearMatches = resourceToLower.indexOf(possibleResource) == 0;
					if (ClassName.isEmpty()) {
						
						// just check method names. This ensures queries like '::getName' will work as well.
						UnicodeString identifierToLower(it->Identifier);
						identifierToLower.toLower();
						nearMatches = identifierToLower.indexOf(needleToLower) == 0;
					}
					if (nearMatches) {
						ResourceClass match = *it;
						Matches.push_back(match);
						break;
					}
				}
			}
			++it;
		}
	}
}

void mvceditor::ResourceFinderClass::CollectAllMembers(const std::vector<UnicodeString>& classNames) {
	std::vector<UnicodeString> normalizedClassNames;
	for (size_t i = 0; i < classNames.size(); ++i) {
		UnicodeString s(classNames[i]);
		s.toLower();
		
		// need to append this so that we only match methods and not classes that begin with the name
		// ie. match only full class names
		s.append(UNICODE_STRING_SIMPLE("::"));
		normalizedClassNames.push_back(s);
	}
	for (std::list<ResourceClass>::const_iterator it = MembersCache.begin(); it != MembersCache.end(); ++it) {
		UnicodeString possibleResource(it->Resource);
		possibleResource.toLower();
		bool match = false;
		for(size_t i = 0; i < normalizedClassNames.size(); ++i) {
			if (possibleResource.indexOf(normalizedClassNames[i]) == 0) {
				match = true;
				break;
			}
		}
		if (match) {
			ResourceClass match = *it;
			Matches.push_back(match);
		}
	}
}

std::vector<UnicodeString> mvceditor::ResourceFinderClass::ClassHierarchy(const UnicodeString& className) const {
	std::vector<UnicodeString> parentClassNames;
	parentClassNames.push_back(className);
	UnicodeString lastClassName(className);
	lastClassName.toLower();
	bool done = false;
	while (!done) {
		done = true;

		// we should probably try to use std::lower_bound here
		for (std::list<ResourceClass>::const_iterator it = ResourceCache.begin(); it != ResourceCache.end(); ++it) {
			if (it->Type == ResourceClass::CLASS && 0 == it->Resource.caseCompare(lastClassName, 0) && it->Signature.length()) {

				// TODO include interface names in this list??
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
	for (std::list<ResourceClass>::const_iterator it = ResourceCache.begin(); it != ResourceCache.end(); ++it) {
		if (it->Type == ResourceClass::CLASS &&  it->Resource == className) {
			parentClassName = ExtractParentClassFromSignature(it->Signature);
			break;
		}
	}
	if (!parentClassName.isEmpty() && !methodName.isEmpty()) {

		// check that the parent has the given method
		UnicodeString resource = parentClassName + UNICODE_STRING_SIMPLE("::") + methodName;
		UnicodeString parentClassSignature;
		bool found = false;
		for (std::list<ResourceClass>::const_iterator it = ResourceCache.begin(); it != ResourceCache.end(); ++it) {
			if (it->Type == ResourceClass::CLASS &&  it->Resource == parentClassName) {
				parentClassSignature = it->Resource;
			}
		}
		for (std::list<ResourceClass>::const_iterator it = MembersCache.begin(); it != MembersCache.end(); ++it) {
			if (it->Type == ResourceClass::METHOD && 0 == it->Resource.indexOf(resource)) {

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
	int pos = resource.find(wxT("::"));
	if (pos >= 0) {
		className = StringHelperClass::wxToIcu(resource.substr(0, pos));
		methodName = StringHelperClass::wxToIcu(resource.substr(pos + 2, -1));
		resourceType = CLASS_NAME_METHOD_NAME;
	}
	else {

		// : => fileName:lineNumber
		int pos = resource.find(wxT(":"));
		if (pos >= 0) {
			long int number;
			wxString after = resource.substr(pos + 1, -1);
			bool isNumber = after.ToLong(&number);
			lineNumber = isNumber ? number : 0;
			fileName = StringHelperClass::wxToIcu(resource.substr(0, pos));
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
	}
	return resourceType;
}

void mvceditor::ResourceFinderClass::BuildResourceCache(const wxString& fullPath, bool parseClasses) {
	wxFileName nativeFunctionsFile = mvceditor::NativeFunctionsAsset();
	IsCurrentFileNative = fullPath.CmpNoCase(nativeFunctionsFile.GetFullPath()) == 0;

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
			mvceditor::LintResultsClass lintResults;
			Parser.ScanFile(fullPath, lintResults);
			IsCacheSorted = false;
		}
	}
}

void mvceditor::ResourceFinderClass::ClassFound(const UnicodeString& rawClassName, const UnicodeString& rawSignature, 
		const UnicodeString& comment) {
	UnicodeString filteredClassName(rawClassName);
	UnicodeString filteredSignature(rawSignature);

	// this prefix is put there by the script that builds the PHP file for the native functions
	// the prefix is placed there so that we can run lint checks on the native functions file
	if (rawClassName.startsWith(UNICODE_STRING_SIMPLE("mvceditornative_"))) {
		int32_t pos = rawClassName.indexOf(UNICODE_STRING_SIMPLE("mvceditornative_"));
		filteredClassName.setTo(rawClassName, pos + 16); // 16 = length of prefix
	}
	if (rawSignature.indexOf(UNICODE_STRING_SIMPLE("mvceditornative_")) >= 0) {
		filteredSignature.findAndReplace(UNICODE_STRING_SIMPLE("mvceditornative_"), UNICODE_STRING_SIMPLE(""));
	}
	ResourceClass classItem;
	classItem.Resource = filteredClassName;
	classItem.Identifier = filteredClassName;
	classItem.Type = ResourceClass::CLASS;
	classItem.FileItemIndex = CurrentFileItemIndex;
	classItem.Signature = filteredSignature;
	classItem.ReturnType = UNICODE_STRING_SIMPLE("");
	classItem.Comment = comment;
	classItem.IsNative = IsCurrentFileNative;
	ResourceCache.push_back(classItem);
}

void mvceditor::ResourceFinderClass::DefineDeclarationFound(const UnicodeString& variableName, 
		const UnicodeString& variableValue, const UnicodeString& comment) {
	ResourceClass defineItem;
	defineItem.Resource = variableName;
	defineItem.Identifier = variableName;
	defineItem.Type = ResourceClass::DEFINE;
	defineItem.FileItemIndex = CurrentFileItemIndex;
	defineItem.Signature = variableValue;
	defineItem.ReturnType = UNICODE_STRING_SIMPLE("");
	defineItem.Comment = comment;
	defineItem.IsNative = IsCurrentFileNative;
	ResourceCache.push_back(defineItem);
}

void mvceditor::ResourceFinderClass::MethodFound(const UnicodeString& rawClassName, const UnicodeString& rawMethodName,
		const UnicodeString& rawSignature, const UnicodeString& returnType, const UnicodeString& comment,
		mvceditor::TokenClass::TokenIds visibility, bool isStatic) {
	UnicodeString filteredClassName(rawClassName);
	UnicodeString filteredMethodName(rawMethodName);
	UnicodeString filteredSignature(rawSignature);

	// this prefix is put there by the script that builds the PHP file for the native functions
	// the prefix is placed there so that we can run lint checks on the native functions file
	if (rawClassName.startsWith(UNICODE_STRING_SIMPLE("mvceditornative_"))) {
		int32_t pos = rawClassName.indexOf(UNICODE_STRING_SIMPLE("mvceditornative_"));
		filteredClassName.setTo(rawClassName, pos + 16); // 16 = length of prefix
	}
	if (rawMethodName.startsWith(UNICODE_STRING_SIMPLE("mvceditornative_"))) {
		int32_t pos = rawMethodName.indexOf(UNICODE_STRING_SIMPLE("mvceditornative_"));
		filteredMethodName.setTo(rawMethodName, pos + 16); // 16 = length of prefix
	}
	if (rawSignature.indexOf(UNICODE_STRING_SIMPLE("mvceditornative_")) >= 0) {
		filteredSignature.findAndReplace(UNICODE_STRING_SIMPLE("mvceditornative_"), UNICODE_STRING_SIMPLE(""));
	}
	ResourceClass item;
	item.Resource = filteredClassName + UNICODE_STRING_SIMPLE("::") + filteredMethodName;
	item.Identifier = filteredMethodName;
	item.Type = ResourceClass::METHOD;
	item.FileItemIndex = CurrentFileItemIndex;
	if (!returnType.isEmpty()) {
		item.Signature = returnType + UNICODE_STRING_SIMPLE(" ");
	}

	item.Signature += filteredSignature;
	item.ReturnType = returnType;
	item.Comment = comment;
	switch (visibility) {
	case mvceditor::TokenClass::PROTECTED:
		item.IsProtected = true;
		break;
	case mvceditor::TokenClass::PRIVATE:
		item.IsPrivate = true;
		break;
	default:
		break;
	}
	item.IsStatic = isStatic;
	item.IsNative = IsCurrentFileNative;
	MembersCache.push_back(item);
}

void mvceditor::ResourceFinderClass::MethodEnd(const UnicodeString& className, const UnicodeString& methodName, int pos) {

	// no need to do anything special when a function has ended
}

void mvceditor::ResourceFinderClass::PropertyFound(const UnicodeString& rawClassName, const UnicodeString& propertyName,
                                        const UnicodeString& propertyType, const UnicodeString& comment, 
										mvceditor::TokenClass::TokenIds visibility, bool isConst, bool isStatic) {
	UnicodeString filteredClassName(rawClassName);

	// this prefix is put there by the script that builds the PHP file for the native functions
	// the prefix is placed there so that we can run lint checks on the native functions file
	if (rawClassName.startsWith(UNICODE_STRING_SIMPLE("mvceditornative_"))) {
		int32_t pos = rawClassName.indexOf(UNICODE_STRING_SIMPLE("mvceditornative_"));
		filteredClassName.setTo(rawClassName, pos + 16); // 16 = length of prefix
	}
	UnicodeString filteredProperty(propertyName);
	if (!isStatic) {

		// remove the siguil from the property name when the variable is not static;
		// because when using non-static access ("->") the siguil is not used
		// this affects the code completion functionality
		filteredProperty.findAndReplace(UNICODE_STRING_SIMPLE("$"), UNICODE_STRING_SIMPLE(""));
	}
	ResourceClass item;
	item.Resource = filteredClassName + UNICODE_STRING_SIMPLE("::") + filteredProperty;
	item.Identifier = filteredProperty;
	item.Type = isConst ? ResourceClass::CLASS_CONSTANT : ResourceClass::MEMBER;
	item.FileItemIndex = CurrentFileItemIndex;
	item.Signature = item.Resource;
	item.ReturnType = propertyType;
	item.Comment = comment;
	switch (visibility) {
	case mvceditor::TokenClass::PROTECTED:
		item.IsProtected = true;
		break;
	case mvceditor::TokenClass::PRIVATE:
		item.IsPrivate = true;
		break;
	default:
		break;
	}
	item.IsStatic = isStatic;
	item.IsNative = IsCurrentFileNative;
	MembersCache.push_back(item);
}

void mvceditor::ResourceFinderClass::FunctionFound(const UnicodeString& rawFunctionName, const UnicodeString& rawSignature, 
		const UnicodeString& returnType, const UnicodeString& comment) {
	UnicodeString filteredFunctionName(rawFunctionName);
	UnicodeString filteredSignature(rawSignature);

	// this prefix is put there by the script that builds the PHP file for the native functions
	// the prefix is placed there so that we can run lint checks on the native functions file
	if (rawFunctionName.startsWith(UNICODE_STRING_SIMPLE("mvceditornative_"))) {
		int32_t pos = rawFunctionName.indexOf(UNICODE_STRING_SIMPLE("mvceditornative_"));
		filteredFunctionName.setTo(rawFunctionName, pos + 16); // 16 = length of prefix
	}
	if (rawSignature.indexOf(UNICODE_STRING_SIMPLE("mvceditornative_")) >= 0) {
		filteredSignature.findAndReplace(UNICODE_STRING_SIMPLE("mvceditornative_"), UNICODE_STRING_SIMPLE(""));
	}

	ResourceClass item;
	item.Resource = filteredFunctionName;
	item.Identifier = filteredFunctionName;
	item.Type = ResourceClass::FUNCTION;
	item.FileItemIndex = CurrentFileItemIndex;
	item.Signature = filteredSignature;
	item.ReturnType = returnType;
	item.Comment = comment;
	item.IsNative = IsCurrentFileNative;
	ResourceCache.push_back(item);
}

void mvceditor::ResourceFinderClass::FunctionEnd(const UnicodeString& functionName, int pos) {
	
	// no need to do anything special when a function has ended
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
	std::list<ResourceClass>::iterator it = ResourceCache.begin();
	while (it != ResourceCache.end()) {
		if (it->FileItemIndex == fileItemIndex) {
			it = ResourceCache.erase(it);
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
			needle.Resource = classHierarchy[i] + UNICODE_STRING_SIMPLE("::") + MethodName;
			needle.Identifier = MethodName;
			std::list<ResourceClass>::iterator it = std::lower_bound(MembersCache.begin(), MembersCache.end(), needle);
			
			// members are sorted by identifers; need to acount for the case when the same method name
			// is used in multiple classes
			int classCount = 0;
			ResourceClass exactMatchResource;
			while (it != MembersCache.end() && it->Identifier.caseCompare(needle.Identifier, 0) == 0) {
				if (it->Resource.caseCompare(needle.Resource, 0) == 0) {
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
	else {
		needle.Resource = ClassName;
		needle.Identifier = ClassName;
		std::list<ResourceClass>::iterator it = std::lower_bound(ResourceCache.begin(), ResourceCache.end(), needle);
		if (it != ResourceCache.end() && it->Resource.caseCompare(needle.Resource, 0) == 0) {
			ResourceClass exactMatchResource = *it;
			++it;
			
			// make sure there is one and only one item that matches the search.
			if (it == ResourceCache.end() || it->Resource.caseCompare(needle.Resource, 0) > 0) {
				Matches.push_back(exactMatchResource);
			}
		}
	}
	EnsureMatchesExist();
	return !Matches.empty();
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

void mvceditor::ResourceFinderClass::Print() const {
	UFILE *out = u_finit(stdout, NULL, NULL);
	u_fprintf(out, "LookingFor=%.*S,%.*S\n", ClassName.length(), ClassName.getBuffer(), MethodName.length(), MethodName.getBuffer());
	for (std::list<ResourceClass>::const_iterator it = ResourceCache.begin(); it != ResourceCache.end(); ++it) {
		u_fprintf(out, "RESOURCE=%.*S  Identifier=%.*S Type=%d\n",
			it->Resource.length(), it->Resource.getBuffer(), it->Identifier.length(), it->Identifier.getBuffer(),  it->Type);
	}
	for (std::list<ResourceClass>::const_iterator it = MembersCache.begin(); it != MembersCache.end(); ++it) {
		u_fprintf(out, "MEMBER=%.*S  Identifier=%.*S ReturnType=%.*S Type=%d\n", 
			it->Resource.length(), it->Resource.getBuffer(), it->Identifier.length(), it->Identifier.getBuffer(),  
				it->ReturnType.length(), it->ReturnType.getBuffer(), it->Type);
	}
	u_fclose(out);
}

bool mvceditor::ResourceFinderClass::IsEmpty() const {
	bool isEmpty = FileCache.empty();
	if (FileCache.size() == (size_t)1) {
		wxString cachedFile = FileCache[0].FullPath;
		if (cachedFile == mvceditor::NativeFunctionsAsset().GetFullPath()) {
			isEmpty = true;
		}
	}
	return isEmpty;
}

void mvceditor::ResourceFinderClass::CopyResourcesFrom(const mvceditor::ResourceFinderClass& src) {
	ResourceCache.clear();

	// since resource caches can be quite large, avoid using push_back
	ResourceCache.resize(src.ResourceCache.size());
	std::list<ResourceClass>::const_iterator it;
	std::list<ResourceClass>::iterator destIt;
	it = src.ResourceCache.begin();
	destIt = ResourceCache.begin();
	while(it != src.ResourceCache.end()) {
		*destIt = *it;
		++destIt;
		++it;
	}
	MembersCache.clear();
	MembersCache.resize(src.MembersCache.size());
	it = src.MembersCache.begin();
	destIt = MembersCache.begin();
	while (it != src.MembersCache.end()) {
		*destIt = *it;
		++destIt;
		++it;
	}
	FileCache.clear();
	FileCache.resize(src.FileCache.size());
	std::vector<FileItem>::const_iterator fit = src.FileCache.begin();
	std::vector<FileItem>::iterator destFit = FileCache.begin();
	while(fit != src.FileCache.end()) {
		destFit->DateTime = fit->DateTime;
		destFit->FullPath = fit->FullPath;
		destFit->Parsed = fit->Parsed;
		destFit->IsNew = fit->IsNew;
		++destFit;
		++fit;
	}
	IsCacheSorted = src.IsCacheSorted;
}

void mvceditor::ResourceFinderClass::AddDynamicResources(const std::vector<mvceditor::ResourceClass>& dynamicResources) {
	for (std::vector<mvceditor::ResourceClass>::const_iterator it = dynamicResources.begin(); it != dynamicResources.end(); ++it) {
		mvceditor::ResourceClass resource = *it;
		if (mvceditor::ResourceClass::MEMBER == resource.Type || mvceditor::ResourceClass::METHOD == resource.Type) {
			
			// cannot assume cache is sorted
			// need to account for duplicates; if so then only update
			bool updated = false;
			for (std::list<mvceditor::ResourceClass>::iterator itMember = MembersCache.begin(); itMember != MembersCache.end(); ++itMember) {
				if (itMember->Resource == resource.Resource) {
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
				MembersCache.push_back(resource);
				IsCacheSorted = false;
			}
		}
		else {

			// look at the class, function, cache
			bool updated = false;
			for (std::list<mvceditor::ResourceClass>::iterator itResource = ResourceCache.begin(); itResource != ResourceCache.end(); ++itResource) {
				if (itResource->Resource == resource.Resource) {
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
				ResourceCache.push_back(resource);
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
	for (std::list<mvceditor::ResourceClass>::const_iterator it = src.ResourceCache.begin(); it != src.ResourceCache.end(); ++it) {
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
			ResourceCache.push_back(res);
		}
		else {
			ResourceCache.push_back(*it);
		}
	}

	// same thing for MembersCache
	for (std::list<mvceditor::ResourceClass>::const_iterator it = src.MembersCache.begin(); it != src.MembersCache.end(); ++it) {
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
}

void mvceditor::ResourceFinderClass::EnsureSorted() {
	Matches.clear();
	Matches.reserve(10);
	if (!IsCacheSorted) {
		ResourceCache.sort();
		MembersCache.sort();

		// if a dyamic resource has been added and is a dup; merge it with the corresponding 'real' resource
		std::list<mvceditor::ResourceClass>::iterator itDynamic = ResourceCache.begin();
		while (itDynamic != ResourceCache.end()) {
			bool erased = false;
			if (itDynamic->IsDynamic) {
				ResourceClass needle;
				needle.Identifier = itDynamic->Identifier;
				std::list<mvceditor::ResourceClass>::iterator found =  std::lower_bound(ResourceCache.begin(), ResourceCache.end(), needle);
				while (found != ResourceCache.end() &&
						found->Resource == itDynamic->Resource && found->Identifier == itDynamic->Identifier && found->Type == itDynamic->Type) {
					if (!found->IsDynamic) {

						// merge by copying the ReturnType from the dynamic resource then 
						// delete the dynamic resource
						found->ReturnType = itDynamic->ReturnType;
						itDynamic = ResourceCache.erase(itDynamic);
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
				std::list<mvceditor::ResourceClass>::iterator found =  std::lower_bound(MembersCache.begin(), MembersCache.end(), needle);
				while (found != MembersCache.end() &&
						found->Resource == itDynamic->Resource && found->Identifier == itDynamic->Identifier && found->Type == itDynamic->Type) {
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

	// TODO: fn_str() would not compile in MSW
	// what about unicode file names?
	UFILE* uf = u_fopen(outputFile.GetFullPath().ToAscii(), "wb", NULL, NULL);
	if (!uf) {
		return false;
	}
	int32_t written;
	bool error = false;
	std::list<mvceditor::ResourceClass>::const_iterator it;
	for (it = ResourceCache.begin(); it != ResourceCache.end() && !error; ++it) {
		if (it->FileItemIndex >= 0) {

			// watch out for dynamic resources and 'native' functions
			// we dont want to persist those for now 
			if (mvceditor::ResourceClass::CLASS == it->Type && !it->IsNative) {
				UnicodeString uniFile = mvceditor::StringHelperClass::wxToIcu(FileCache[it->FileItemIndex].FullPath);
				written = u_fprintf(uf, "CLASS,%S,%.*S,\n", 
					uniFile.getTerminatedBuffer(), it->Resource.length(), it->Resource.getBuffer());
				error = 0 == written;
			}
			else if (mvceditor::ResourceClass::FUNCTION == it->Type && !it->IsNative) {
				UnicodeString uniFile = mvceditor::StringHelperClass::wxToIcu(FileCache[it->FileItemIndex].FullPath);
				written = u_fprintf(uf, "FUNCTION,%S,%.*S,\n", 
					uniFile.getTerminatedBuffer(), it->Resource.length(), it->Resource.getBuffer());
				error = 0 == written;
			}
		}
	}
	for (it = MembersCache.begin(); it != MembersCache.end() && !error; ++it) {
		if (it->FileItemIndex >= 0) {
			UnicodeString className(it->Resource, 0, it->Resource.indexOf(it->Identifier));
			className.findAndReplace(UNICODE_STRING_SIMPLE("::"), UNICODE_STRING_SIMPLE(""));

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
	all.resize(ResourceCache.size() + MembersCache.size());
	std::list<mvceditor::ResourceClass>::const_iterator it;
	size_t index = 0;
	for (it = ResourceCache.begin(); it != ResourceCache.end(); ++it, ++index) {
		all[index] = *it;
	}
	for (it = MembersCache.begin(); it != MembersCache.end(); ++it, ++index) {
		all[index] = *it;
	}
	return all;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::AllNonNativeClasses() const {
	std::vector<mvceditor::ResourceClass> all;
	all.reserve(ResourceCache.size());
	std::list<mvceditor::ResourceClass>::const_iterator it;
	for (it = ResourceCache.begin(); it != ResourceCache.end(); ++it) {
		if (mvceditor::ResourceClass::CLASS == it->Type && !it->IsNative) {
			all.push_back(*it);
		}
	}
	return all;
}

void mvceditor::ResourceFinderClass::Clear() {
	ResourceCache.clear();
	MembersCache.clear();
	FileCache.clear();
	Matches.clear();
	IsCacheSorted = false;
	IsCurrentFileNative = false;
}

mvceditor::ResourceClass::ResourceClass()
	: Resource()
	, Identifier()
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
	, FileItemIndex(-1) {
		
}

void mvceditor::ResourceClass::operator=(const ResourceClass& src) {
	Resource = src.Resource;
	Identifier = src.Identifier;
	Signature = src.Signature;
	ReturnType = src.ReturnType;
	Comment = src.Comment;
	Type = src.Type;
	FullPath = src.FullPath;
	FileItemIndex = src.FileItemIndex;
	IsProtected = src.IsProtected;
	IsPrivate = src.IsPrivate;
	IsStatic = src.IsStatic;
	IsDynamic = src.IsDynamic;
	IsNative = src.IsNative;
}

bool mvceditor::ResourceClass::operator<(const mvceditor::ResourceClass& a) const {
	return Identifier.caseCompare(a.Identifier, 0) < 0;
}

wxString mvceditor::ResourceClass::GetFullPath() const {
	return FullPath;
}
