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
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <algorithm>
#include <fstream>


mvceditor::ResourceFinderClass::ResourceFinderClass()
		: FilesFilter()
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
		, IsCacheSorted(false) {
	Parser.SetClassObserver(this);
	Parser.SetClassMemberObserver(this);
	Parser.SetFunctionObserver(this);
}

bool mvceditor::ResourceFinderClass::Walk(const wxString& fileName) {
	if (!wxIsWild(FilesFilter) || wxMatchWild(FilesFilter, fileName)) {
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

void mvceditor::ResourceFinderClass::BuildResourceCacheForFile(const wxString& fullPath, const UnicodeString& code) {
	// remove all previous cached resources
	int fileItemIndex = -1;
	for (size_t i = 0; i < FileCache.size(); ++i) {
		if (FileCache[i].FullPath == fullPath) {
			fileItemIndex = i;
			break;
		}
	}
	RemoveCachedResources(fileItemIndex);
	CurrentFileItemIndex = fileItemIndex;
	Parser.ScanString(code);
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

bool mvceditor::ResourceFinderClass::GetResourceMatchPosition(size_t index, const UnicodeString& text, int32_t& pos, 
		int32_t& length) const {
	size_t start = 0;
	mvceditor::FinderClass finder;
	finder.Mode = FinderClass::REGULAR_EXPRESSION;
	if (index >= 0 && index < Matches.size()) {
		ResourceClass item = Matches[index];
		UnicodeString resource(item.Resource);
		UnicodeString className,
			methodName;
		int scopeResolutionPos = resource.indexOf(UNICODE_STRING_SIMPLE("::"));
		if (scopeResolutionPos >= 0) {
			className.setTo(resource, 0, scopeResolutionPos);
			methodName.setTo(resource, scopeResolutionPos + 2, resource.length() - scopeResolutionPos + 2 - 1);
		}
		else {
			className = resource;
		}
		switch (item.Type) {
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
				finder.Expression = UNICODE_STRING_SIMPLE("\\sfunction\\s+") + methodName + UNICODE_STRING_SIMPLE("\\s*\\(");
				break;
			case ResourceClass::FUNCTION:
				finder.Expression = UNICODE_STRING_SIMPLE("\\sfunction\\s+") + className + UNICODE_STRING_SIMPLE("\\s*\\(");
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
	}
	if (finder.Prepare() && finder.FindNext(text, start) && finder.GetLastMatch(pos, length)) {
		++pos; //eat the first space
		--length;
		return true;
	}
	return false;
}

bool mvceditor::ResourceFinderClass::Prepare(const wxString& resource) {
	ParseGoToResource(resource);
	// maybe later do some error checking
	return !resource.IsEmpty();
}

void mvceditor::ResourceFinderClass::BuildResourceCacheForNativeFunctions() {

	// add the php built in functions
	wxStandardPaths paths;
	wxFileName pathExecutableFileName(paths.GetExecutablePath());
	wxString nativeFileName = pathExecutableFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) +
	                          wxT("..") + wxFileName::GetPathSeparator() +
	                          wxT("resources") + wxFileName::GetPathSeparator() +
	                          wxT("native.php");
	wxFileName fileName(nativeFileName);
	fileName.Normalize();
	if (fileName.FileExists()) {
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
	std::vector<UnicodeString> parentClassNames;
	UnicodeString lastClassName(ClassName);
	lastClassName.toLower();
	parentClassNames.push_back(lastClassName);
	bool done = false;
	while (!done) {
		done = true;
		for (std::list<ResourceClass>::iterator it = ResourceCache.begin(); it != ResourceCache.end(); ++it) {
			if (it->Type == ResourceClass::CLASS && 0 == it->Resource.caseCompare(lastClassName, 0) && it->Signature.length()) {
				int32_t extendsPos = it->Signature.indexOf(UNICODE_STRING_SIMPLE("extends "));
				if (extendsPos >= 0) {
					lastClassName = ExtractParentClassFromSignature(it->Signature);
					lastClassName.toLower();
					parentClassNames.push_back(lastClassName);
					done = false;
					break;
				}
			}
		}
	}	
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
			if(it->Identifier.caseCompare(needle.Identifier, 0) == 0) {
				
				// parentClassNames is a list of all classes ancestors of ClassName.  A match is found when the method
				// is from one of these parents.  This prevents matches from classes that may have the sam method name
				// but are not related.
				for(size_t i = 0; i < parentClassNames.size(); ++i) {
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

UnicodeString mvceditor::ResourceFinderClass::GetResourceSignature(const UnicodeString& resource) const {
	UnicodeString signature;
	
	// a function
	for (std::list<ResourceClass>::const_iterator it = ResourceCache.begin(); it != ResourceCache.end(); ++it) {
		if (it->Type != ResourceClass::CLASS && it->Type != ResourceClass::DEFINE && it->Resource == resource) {
			signature = it->Signature;
			break;
		}
	}
	if (signature.isEmpty()) {
		
		//search methods
		UnicodeString constructorSignature = resource + UNICODE_STRING_SIMPLE("::") + resource;
		for (std::list<ResourceClass>::const_iterator it = MembersCache.begin(); it != MembersCache.end(); ++it) {
			if (it->Type == ResourceClass::METHOD && (it->Resource == resource || it->Resource == constructorSignature)) {
				signature = it->Signature;
				break;
			}
		}	
	}
	return signature;
}

UnicodeString mvceditor::ResourceFinderClass::GetResourceReturnType(const UnicodeString& resource) const {
	UnicodeString icuResource = resource;
	UnicodeString returnType;
	
	// functions
	for (std::list<ResourceClass>::const_iterator it = ResourceCache.begin(); it != ResourceCache.end(); ++it) {
		if (it->Type != ResourceClass::CLASS && it->Type != ResourceClass::DEFINE && it->Resource == icuResource) {
			returnType = it->ReturnType;
			break;
		}
	}
	if (returnType.isEmpty()) {
		UnicodeString constructorSignature = icuResource + UNICODE_STRING_SIMPLE("::") + icuResource;
		for (std::list<ResourceClass>::const_iterator it = MembersCache.begin(); it != MembersCache.end(); ++it) {
			if (it->Type == ResourceClass::METHOD && (it->Resource == icuResource || it->Resource == constructorSignature)) {
				returnType = it->ReturnType;
				break;
			}
		}
	}
	return returnType;
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

void mvceditor::ResourceFinderClass::ParseGoToResource(const wxString& resource) {
	FileName = UNICODE_STRING_SIMPLE("");
	ClassName = UNICODE_STRING_SIMPLE("");
	MethodName = UNICODE_STRING_SIMPLE("");
	LineNumber = 0;

	// :: => Class::method
	int pos = resource.find(wxT("::"));
	if (pos >= 0) {
		ClassName = StringHelperClass::wxToIcu(resource.substr(0, pos));
		MethodName = StringHelperClass::wxToIcu(resource.substr(pos + 2, -1));
		ResourceType = CLASS_NAME_METHOD_NAME;
	}
	else {

		// : => fileName:lineNumber
		int pos = resource.find(wxT(":"));
		if (pos >= 0) {
			long int number;
			wxString after = resource.substr(pos + 1, -1);
			bool isNumber = after.ToLong(&number);
			LineNumber = isNumber ? number : 0;
			FileName = StringHelperClass::wxToIcu(resource.substr(0, pos));
			ResourceType = FILE_NAME_LINE_NUMBER;
		}
		else {

			// class names can only have alphanumerics or underscores
			int pos = resource.find_first_of(wxT("`!@#$%^&*()+={}|\\:;\"',./?"));
			if (pos >= 0) {
				FileName = StringHelperClass::wxToIcu(resource);
				ResourceType = FILE_NAME;
			}
			else {
				ClassName = StringHelperClass::wxToIcu(resource);
				ResourceType = CLASS_NAME;
			}
		}
	}
}

void mvceditor::ResourceFinderClass::BuildResourceCache(const wxString& fullPath, bool parseClasses) {
	wxFileName fileName(fullPath);
	wxDateTime fileLastModifiedDateTime = fileName.GetModificationTime();
	
	// have we looked at this file yet or is cache out of date? if not, then build the cache.
	// we count the file as cached if (1) we have seen the file before (modified timestamp)
	bool cached = false;
	int fileItemIndex = -1;
	for (size_t i = 0; i < FileCache.size(); ++i) {
		if (FileCache[i].FullPath == fullPath) {
			fileItemIndex = i;
			bool modified = fileLastModifiedDateTime.IsLaterThan(FileCache[i].DateTime);
			cached = !modified;
			break;
		}
	}
	FileItem fileItem;
	bool isNewFile = false;
	if (fileItemIndex > -1) {
		FileCache[fileItemIndex].FullPath = fullPath;
		FileCache[fileItemIndex].DateTime = fileLastModifiedDateTime;
		fileItem = FileCache[fileItemIndex];
	}
	else {
		fileItem.FullPath = fullPath;
		fileItem.DateTime = fileLastModifiedDateTime;
		fileItem.Parsed = false;
		fileItemIndex = FileCache.size();
		FileCache.push_back(fileItem);
		isNewFile = true;
	}
	if (parseClasses) {
		if (!cached || !fileItem.Parsed) {
			FileCache[fileItemIndex].Parsed = true;

			// no need to look for resources if the file had not yet existed, this will save much time
			// this optimization was found by using the profiler
			if (!isNewFile) {
				RemoveCachedResources(fileItemIndex);
			}
			CurrentFileItemIndex = fileItemIndex;
			Parser.ScanFile(fullPath);
			IsCacheSorted = false;
		}
	}
}

void mvceditor::ResourceFinderClass::ClassFound(const UnicodeString& className, const UnicodeString& signature, 
		const UnicodeString& comment) {
	ResourceClass classItem;
	classItem.Resource = className;
	classItem.Identifier = className;
	classItem.Type = ResourceClass::CLASS;
	classItem.FileItemIndex = CurrentFileItemIndex;
	classItem.Signature = signature;
	classItem.ReturnType = UNICODE_STRING_SIMPLE("");
	classItem.Comment = comment;
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
	ResourceCache.push_back(defineItem);
}

void mvceditor::ResourceFinderClass::MethodFound(const UnicodeString& className, const UnicodeString& methodName,
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment) {
	ResourceClass item;
	item.Resource = className + UNICODE_STRING_SIMPLE("::") + methodName;
	item.Identifier = methodName;
	item.Type = ResourceClass::METHOD;
	item.FileItemIndex = CurrentFileItemIndex;
	if (!returnType.isEmpty()) {
		item.Signature = returnType + UNICODE_STRING_SIMPLE(" ");
	}
	item.Signature += signature;
	item.ReturnType = returnType;
	item.Comment = comment;
	MembersCache.push_back(item);

	if (methodName.caseCompare(UNICODE_STRING_SIMPLE("__construct"), 0) == 0) {
		
		// for constructors, rename them to have the same name as the class. IF this logic is changed, then the logic
		// in the Collect methods must be changed as well.
		ResourceClass itemConstruct;
		itemConstruct.Resource = className + UNICODE_STRING_SIMPLE("::") + className;
		itemConstruct.Identifier = className;
		itemConstruct.Type = ResourceClass::METHOD;
		itemConstruct.FileItemIndex = CurrentFileItemIndex;
		if (!returnType.isEmpty()) {
			itemConstruct.Signature = returnType + UNICODE_STRING_SIMPLE(" ");
		}
		itemConstruct.Signature += signature;
		itemConstruct.Signature.findAndReplace(UNICODE_STRING_SIMPLE("__construct"), className);
		itemConstruct.ReturnType = returnType;
		itemConstruct.Comment = comment;
		MembersCache.push_back(itemConstruct);
	}
}

void mvceditor::ResourceFinderClass::PropertyFound(const UnicodeString& className, const UnicodeString& propertyName,
                                        const UnicodeString& propertyType, const UnicodeString& comment, bool isConst) {
	ResourceClass item;
	item.Resource = className + UNICODE_STRING_SIMPLE("::") + propertyName;
	item.Identifier = propertyName;
	item.Type = isConst ? ResourceClass::CLASS_CONSTANT : ResourceClass::MEMBER;
	item.FileItemIndex = CurrentFileItemIndex;
	item.Signature = item.Resource;
	item.ReturnType = propertyType;
	item.Comment = comment;
	MembersCache.push_back(item);
}

void mvceditor::ResourceFinderClass::FunctionFound(const UnicodeString& functionName, const UnicodeString& signature, 
		const UnicodeString& returnType, const UnicodeString& comment) {
	ResourceClass item;
	item.Resource = functionName;
	item.Identifier = functionName;
	item.Type = ResourceClass::FUNCTION;
	item.FileItemIndex = CurrentFileItemIndex;
	item.Signature = signature;
	item.ReturnType = returnType;
	item.Comment = comment;
	ResourceCache.push_back(item);
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
	needle.Resource = ClassName;
	std::list<ResourceClass>::iterator it;
	if (ResourceType == CLASS_NAME_METHOD_NAME) {
		needle.Resource.append(UNICODE_STRING_SIMPLE("::")).append(MethodName);
		needle.Identifier = MethodName;
		it = std::lower_bound(MembersCache.begin(), MembersCache.end(), needle);
		
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
		}
	}
	else {
		needle.Identifier = ClassName;
		it = std::lower_bound(ResourceCache.begin(), ResourceCache.end(), needle);
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
	for(std::vector<ResourceClass>::const_iterator it = Matches.begin(); it != Matches.end(); ++it) {
		files.push_back(it->FileItemIndex);
	}
	unique(files.begin(), files.end());
	for(std::vector<int>::const_iterator it = files.begin(); it != files.end(); ++it) {
		int fileItemIndex = *it;
		if (fileItemIndex >= 0 && fileItemIndex < (int)FileCache.size()) {
			if (!wxFileName::FileExists(FileCache[fileItemIndex].FullPath)) {
				
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

void mvceditor::ResourceFinderClass::Print() {
	UFILE *out = u_finit(stdout, NULL, NULL);
	u_fprintf(out, "LookingFor=%S,%S\n", ClassName.getTerminatedBuffer(), MethodName.getTerminatedBuffer());
	for (std::list<ResourceClass>::iterator it = ResourceCache.begin(); it != ResourceCache.end(); ++it) {
		u_fprintf(out, "RESOURCE=%S  Identifier=%S Type=%d\n",
			it->Resource.getTerminatedBuffer(), it->Identifier.getTerminatedBuffer(),  it->Type);
	}
	for (std::list<ResourceClass>::iterator it = MembersCache.begin(); it != MembersCache.end(); ++it) {
		u_fprintf(out, "MEMBER=%S  Identifier=%S Type=%d\n", 
			it->Resource.getTerminatedBuffer(), it->Identifier.getTerminatedBuffer(),  it->Type);
	}
	u_fclose(out);
}

void mvceditor::ResourceFinderClass::EnsureSorted() {
	Matches.clear();
	Matches.reserve(10);
	if (!IsCacheSorted) {
		ResourceCache.sort();
		MembersCache.sort();
		IsCacheSorted = true;
	}
}

mvceditor::ResourceClass::ResourceClass()
	: Resource()
	, Identifier()
	, Signature()
	, ReturnType()
	, Comment()
	, Type(CLASS) 
	, FileItemIndex(-1) {
		
}

void mvceditor::ResourceClass::operator=(const ResourceClass& src) {
	Resource = src.Resource;
	Identifier = src.Identifier;
	Signature = src.Signature;
	ReturnType = src.ReturnType;
	Comment = src.Comment;
	Type = src.Type;
	FileItemIndex = src.FileItemIndex;
}

bool mvceditor::ResourceClass::operator<(const mvceditor::ResourceClass& a) const {
	return Identifier.caseCompare(a.Identifier, 0) < 0;
}

