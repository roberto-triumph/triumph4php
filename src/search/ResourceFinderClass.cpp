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
#include <globals/String.h>
#include <globals/Sqlite.h>
#include <wx/filename.h>
#include <algorithm>
#include <fstream>
#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include <unicode/fmtable.h>
#include <unicode/numfmt.h>
#include <soci/soci.h>
#include <string>

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

mvceditor::ResourceSearchClass::ResourceSearchClass(UnicodeString resourceQuery)
	: FileName()
	, ClassName()
	, MethodName()
	, ResourceType(FILE_NAME)
	, LineNumber(0) {
	ResourceType = CLASS_NAME;

	// :: => Class::method
	// \ => \namespace\namespace
	// : => filename : line number
	int scopePos = resourceQuery.indexOf(UNICODE_STRING_SIMPLE("::"));
	int namespacePos = resourceQuery.indexOf(UNICODE_STRING_SIMPLE("\\"));
	int colonPos = resourceQuery.indexOf(UNICODE_STRING_SIMPLE(":"));
	if (scopePos >= 0) {
		ClassName.setTo(resourceQuery, 0, scopePos);
		MethodName.setTo(resourceQuery, scopePos + 2);
		ResourceType = CLASS_NAME_METHOD_NAME;
	}
	else if (namespacePos >= 0) {
		ClassName = resourceQuery;
		ResourceType = NAMESPACE_NAME;
	}
	else if (colonPos >= 0) {
		
		// : => filename : line number
		UnicodeString after(resourceQuery, colonPos + 1);
		Formattable fmtable((int32_t)0);
		UErrorCode error = U_ZERO_ERROR;
		NumberFormat* format = NumberFormat::createInstance(error);
		if (U_SUCCESS(error)) {
			format->parse(after, fmtable, error);
			if (U_SUCCESS(error)) {
				LineNumber = fmtable.getLong();
			}
		}
		delete format;
		FileName.setTo(resourceQuery, 0, colonPos);
		ResourceType = FILE_NAME_LINE_NUMBER;
	}
	else {

		// class names can only have alphanumerics or underscores
		UnicodeString symbols = UNICODE_STRING_SIMPLE("`!@#$%^&*()+={}|\\:;\"',./?");
		
		// not using getTerminatedBuffer() because that method triggers valgrind warnings
		UChar* queryBuf = new UChar[resourceQuery.length() + 1];
		u_memmove(queryBuf, resourceQuery.getBuffer(), resourceQuery.length());
		queryBuf[resourceQuery.length()] = '\0';
	
		UChar* symbolBuf = new UChar[symbols.length() + 1];
		u_memmove(symbolBuf, symbols.getBuffer(), symbols.length());
		symbolBuf[symbols.length()] = '\0';
		bool hasSymbols = NULL != u_strpbrk(queryBuf, symbolBuf);
		if (hasSymbols) {
			FileName = resourceQuery;
			ResourceType = FILE_NAME;
		}
		else {
			ClassName = resourceQuery;
			ResourceType = CLASS_NAME;
		}
		delete[] queryBuf;
		delete[] symbolBuf;
	}
}

void mvceditor::ResourceSearchClass::SetParentClasses(const std::vector<UnicodeString>& parents) {
	ParentClasses = parents;
}

std::vector<UnicodeString> mvceditor::ResourceSearchClass::GetParentClasses() const {
	return ParentClasses;
}

void mvceditor::ResourceSearchClass::SetTraits(const std::vector<UnicodeString>& traits) {
	Traits = traits;
}

std::vector<UnicodeString> mvceditor::ResourceSearchClass::GetTraits() const {
	return Traits;
}

UnicodeString mvceditor::ResourceSearchClass::GetClassName() const {
	return ClassName;
}

UnicodeString mvceditor::ResourceSearchClass::GetFileName() const {
	return FileName;
}

UnicodeString mvceditor::ResourceSearchClass::GetMethodName() const {
	return MethodName;
}

int mvceditor::ResourceSearchClass::GetLineNumber() const {
	return LineNumber;
}

mvceditor::ResourceSearchClass::ResourceTypes mvceditor::ResourceSearchClass::GetResourceType() const {
	return ResourceType;
}


mvceditor::ResourceFinderClass::ResourceFinderClass()
	: Session(NULL)
	, IsCacheInitialized(false) {
}

mvceditor::ResourceFinderClass::~ResourceFinderClass() {

}

void mvceditor::ResourceFinderClass::Init(soci::session* session) {
	Session = session;
	IsCacheInitialized = true;
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

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::CollectNearMatchResources(
	const mvceditor::ResourceSearchClass& resourceSearch,
	bool doCollectFileNames) {

	
	// at one point there was a check here to see if the  resource files existed
	// it was removed because it caused performance issues, since this method
	// is called while the user is typing text.
	// take care when coding; make sure that any code called by this method does not touch the file system
	std::vector<mvceditor::ResourceClass> matches;
	switch (resourceSearch.GetResourceType()) {
		case mvceditor::ResourceSearchClass::FILE_NAME:
		case mvceditor::ResourceSearchClass::FILE_NAME_LINE_NUMBER:
			matches = CollectNearMatchFiles(resourceSearch.GetFileName(), resourceSearch.GetLineNumber());
			break;
		case mvceditor::ResourceSearchClass::CLASS_NAME:
			matches = CollectNearMatchNonMembers(resourceSearch);
			if (matches.empty() && doCollectFileNames) {
				matches = CollectNearMatchFiles(resourceSearch.GetClassName(), resourceSearch.GetLineNumber());
			}
			break;
		case mvceditor::ResourceSearchClass::CLASS_NAME_METHOD_NAME:
			matches = CollectNearMatchMembers(resourceSearch);
			break;
		case mvceditor::ResourceSearchClass::NAMESPACE_NAME:
			matches = CollectNearMatchNamespaces(resourceSearch);
			break;
	}
	sort(matches.begin(), matches.end());
	return matches;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::CollectNearMatchFiles(const UnicodeString& search, int lineNumber) {
	wxString path,
		currentFileName,
		extension;
	std::vector<mvceditor::ResourceClass> matches;
	std::string query = mvceditor::IcuToChar(search);

	// add the SQL wildcards
	std::string escaped = SqlEscape(query, '^');
	query = "'%" + escaped + "%'";
	std::string match;
	int fileItemId;
	int isNew;
	try {
		soci::statement stmt = (Session->prepare << 
			"SELECT full_path, file_item_id, is_new FROM file_items WHERE full_path LIKE " + query + " ESCAPE '^'",
			soci::into(match), soci::into(fileItemId), soci::into(isNew));
		if (stmt.execute(true)) {
			do {
				wxString fullPath = mvceditor::CharToWx(match.c_str());
				wxFileName::SplitPath(fullPath, &path, &currentFileName, &extension);
				currentFileName += wxT(".") + extension;
				wxString fileName = mvceditor::IcuToWx(search);
				fileName = fileName.Lower();
				if (wxNOT_FOUND != currentFileName.Lower().Find(fileName)) {
					if (0 == lineNumber || GetLineCountFromFile(fullPath) >= lineNumber) {
						ResourceClass newItem;
						newItem.FileItemId = fileItemId;
						newItem.Identifier = mvceditor::WxToIcu(currentFileName);
						newItem.SetFullPath(fullPath);
						newItem.FileIsNew = isNew > 0;
						matches.push_back(newItem);
					}
				}
			} while (stmt.fetch());
		}
	} catch (std::exception& e) {
		
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		e.what();
	}
	return matches;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::CollectNearMatchNonMembers(const mvceditor::ResourceSearchClass& resourceSearch) {
	std::string key = mvceditor::IcuToChar(resourceSearch.GetClassName());
	std::vector<int> types;
	types.push_back(mvceditor::ResourceClass::CLASS);
	types.push_back(mvceditor::ResourceClass::DEFINE);
	types.push_back(mvceditor::ResourceClass::FUNCTION);
	std::vector<mvceditor::ResourceClass> matches = FindByKeyExactAndTypes(key, types, true);
	if (matches.empty()) {
	
		// if nothing matches exactly then execure the LIKE query
		matches = FindByKeyStartAndTypes(key, types, true);
	}
	return matches;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::CollectNearMatchMembers(const mvceditor::ResourceSearchClass& resourceSearch) {
	std::vector<mvceditor::ResourceClass> matches;
	
	// when looking for members we need to look
	// 1. in the class itself
	// 2. in any parent classes
	// 3. in any used traits
	std::vector<UnicodeString> classesToSearch = resourceSearch.GetParentClasses();
	classesToSearch.push_back(resourceSearch.GetClassName());
	std::vector<UnicodeString> traits = resourceSearch.GetTraits();
	classesToSearch.insert(classesToSearch.end(), traits.begin(), traits.end());

	if (resourceSearch.GetMethodName().isEmpty()) {
		
		// special case; query for all methods for a class (UserClass::)
		std::vector<mvceditor::ResourceClass> memberMatches = CollectAllMembers(classesToSearch);

		//get the methods that belong to a used trait
		std::vector<mvceditor::ResourceClass> traitMatches = CollectAllTraitAliases(classesToSearch, UNICODE_STRING_SIMPLE(""));

		matches.insert(matches.end(), memberMatches.begin(), memberMatches.end());
		matches.insert(matches.end(), traitMatches.begin(), traitMatches.end());
	}
	else if (resourceSearch.GetClassName().isEmpty()) {
		
		// special case, query across all classes for a method (::getName)
		// if ClassName is empty, then just check method names This ensures 
		// queries like '::getName' will work as well.
		// make sure to NOT get fully qualified  matches (key=identifier)
		std::string identifier = mvceditor::IcuToChar(resourceSearch.GetMethodName());
		std::vector<int> types;
		types.push_back(mvceditor::ResourceClass::MEMBER);
		types.push_back(mvceditor::ResourceClass::METHOD);
		types.push_back(mvceditor::ResourceClass::CLASS_CONSTANT);
		matches = FindByIdentifierExactAndTypes(identifier, types, true);
		if (matches.empty()) {
		
			// use LIKE to get near matches
			matches = FindByIdentifierStartAndTypes(identifier, types, true);
		}
	}
	else {
		std::vector<std::string> keyStarts;

		// now that we found the parent classes, combine the parent class name and the queried method
		// to make all of the keys we need to look for. remember that a resource class key is of the form
		// ClassName::MethodName
		for (std::vector<UnicodeString>::iterator it = classesToSearch.begin(); it != classesToSearch.end(); ++it) {
			std::string keyStart = mvceditor::IcuToChar(*it);
			keyStart += "::";
			keyStart += mvceditor::IcuToChar(resourceSearch.GetMethodName());
			keyStarts.push_back(keyStart);
		}
		matches = FindByKeyStartMany(keyStarts, true);

		// get any aliases
		std::vector<mvceditor::ResourceClass> traitMatches = CollectAllTraitAliases(classesToSearch, resourceSearch.GetMethodName());
		matches.insert(matches.end(), traitMatches.begin(), traitMatches.end());
			
	}
	return matches;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::CollectAllMembers(const std::vector<UnicodeString>& classNames) {
	std::vector<mvceditor::ResourceClass> matches;
	std::vector<std::string> keyStarts;
	for (size_t i = 0; i < classNames.size(); ++i) {
		std::string keyStart = mvceditor::IcuToChar(classNames[i]);
		keyStart += "::";
		keyStarts.push_back(keyStart);
	}
	matches = FindByKeyStartMany(keyStarts, true);
	return matches;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::CollectAllTraitAliases(const std::vector<UnicodeString>& classNames, const UnicodeString& methodName) {
	std::vector<mvceditor::ResourceClass> matches;

	std::vector<std::string> classNamesToLookFor;
	for (std::vector<UnicodeString>::const_iterator it = classNames.begin(); it != classNames.end(); ++it) {
		classNamesToLookFor.push_back(mvceditor::IcuToChar(*it));
	}

	// TODO use the correct namespace when querying for traits
	std::vector<mvceditor::TraitResourceClass> traits = FindTraitsByClassName(classNamesToLookFor);
	
	// now go through the result and add the method names of any aliased methods
	UnicodeString lowerMethodName(methodName);
	lowerMethodName.toLower();
	for (size_t i = 0; i < traits.size(); i++) {
		std::vector<UnicodeString> aliases = traits[i].Aliased;
		for (size_t a = 0; a < aliases.size(); a++) {
			UnicodeString alias(aliases[a]);
			UnicodeString lowerAlias(alias);
			lowerAlias.toLower();
			bool useAlias = methodName.isEmpty() || lowerMethodName.indexOf(lowerAlias) == 0;
			if (useAlias) {
				mvceditor::ResourceClass res;
				res.ClassName = traits[i].TraitClassName;
				res.Identifier = alias;
				matches.push_back(res);
			}
		}
	}
	return matches;
}

std::vector<mvceditor::ResourceClass>  mvceditor::ResourceFinderClass::CollectNearMatchNamespaces(const mvceditor::ResourceSearchClass& resourceSearch) {
	std::vector<mvceditor::ResourceClass> matches;

	// needle identifier contains a namespace operator; but it may be
	// a namespace or a fully qualified name
	UnicodeString key = resourceSearch.GetClassName();
	std::string stdKey = mvceditor::IcuToChar(key);
	matches = FindByKeyExact(stdKey);
	if (matches.empty()) {
		matches = FindByKeyStart(stdKey, true);
	}
	return matches;
}

UnicodeString mvceditor::ResourceFinderClass::GetResourceParentClassName(const UnicodeString& className) {
	UnicodeString parentClassName;

	// first query to get the parent class name
	std::vector<int> types;
	types.push_back(mvceditor::ResourceClass::CLASS);
	std::string key = mvceditor::IcuToChar(className);
	std::vector<mvceditor::ResourceClass> matches = FindByKeyExactAndTypes(key, types, true);
	if (!matches.empty()) {
		mvceditor::ResourceClass resource = matches[0];
		parentClassName = ExtractParentClassFromSignature(resource.Signature);
	}
	return parentClassName;
}

std::vector<UnicodeString> mvceditor::ResourceFinderClass::GetResourceTraits(const UnicodeString& className, 
																			 const UnicodeString& methodName) {
	std::vector<UnicodeString> inheritedTraits;
	bool match = false;
	
	std::vector<std::string> keys;
	keys.push_back(mvceditor::IcuToChar(className));
	std::vector<mvceditor::TraitResourceClass> matches = FindTraitsByClassName(keys);
	for (size_t i = 0; i < matches.size(); ++i) {
		UnicodeString fullyQualifiedTrait = QualifyName(matches[i].TraitNamespaceName, matches[i].TraitClassName);
			
		// trait is used unless there is an explicit insteadof 
		match = true;
		for (size_t j = 0; j < matches[i].InsteadOfs.size(); ++j) {
			if (matches[i].InsteadOfs[j].caseCompare(fullyQualifiedTrait, 0) == 0) {
				match = false;
				break;
			}
		}
		if (match) {
			inheritedTraits.push_back(matches[i].TraitClassName);
		}
	}
	return inheritedTraits;
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

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::CollectFullyQualifiedResource(const mvceditor::ResourceSearchClass& resourceSearch) {

	// at one point there was a check here to see if the  resource files existed
	// it was removed because it caused performance issues, since this method
	// is called while the user is typing text.
	// take care when coding; make sure that any code called by this method does not touch the file system
	std::vector<mvceditor::ResourceClass> allMatches;
	std::vector<int> types;
	if (resourceSearch.GetResourceType() == mvceditor::ResourceSearchClass::CLASS_NAME_METHOD_NAME) {

		// check the entire class hierachy; stop as soon as we found it
		// combine the parent classes with the class being searched
		std::vector<UnicodeString> classHierarchy = resourceSearch.GetParentClasses();
		classHierarchy.push_back(resourceSearch.GetClassName());
		
		types.push_back(mvceditor::ResourceClass::MEMBER);
		types.push_back(mvceditor::ResourceClass::METHOD);
		types.push_back(mvceditor::ResourceClass::CLASS_CONSTANT);
		for (size_t i = 0; i < classHierarchy.size(); ++i) {
			UnicodeString key = classHierarchy[i] + UNICODE_STRING_SIMPLE("::") + resourceSearch.GetMethodName();
			std::string stdKey = mvceditor::IcuToChar(key);
			std::vector<mvceditor::ResourceClass> matches = FindByKeyExactAndTypes(stdKey, types, true);
			if (!matches.empty()) {
				allMatches.push_back(matches[0]);
			}
		}
	}
	else if (mvceditor::ResourceSearchClass::NAMESPACE_NAME == resourceSearch.GetResourceType()) {
		UnicodeString key = resourceSearch.GetClassName();
		std::string stdKey = mvceditor::IcuToChar(key);
		types.push_back(mvceditor::ResourceClass::DEFINE);
		types.push_back(mvceditor::ResourceClass::CLASS);
		types.push_back(mvceditor::ResourceClass::FUNCTION);

		// make sure there is one and only one item that matches the search.
		std::vector<mvceditor::ResourceClass> matches = FindByKeyExactAndTypes(stdKey, types, true);
		if (matches.size() == 1) {
			allMatches.push_back(matches[0]);
		}
	}
	else {
		UnicodeString key = resourceSearch.GetClassName();
		std::string stdKey = mvceditor::IcuToChar(key);
		types.push_back(mvceditor::ResourceClass::DEFINE);
		types.push_back(mvceditor::ResourceClass::CLASS);
		types.push_back(mvceditor::ResourceClass::FUNCTION);

		// make sure there is one and only one item that matches the search.
		std::vector<mvceditor::ResourceClass> matches = FindByKeyExactAndTypes(stdKey, types, true);
		if (matches.size() == 1) {
			allMatches.push_back(matches[0]);
		}
	}
	return allMatches;
}

void mvceditor::ResourceFinderClass::EnsureMatchesExist(std::vector<ResourceClass>& matches) {

	// remove from matches that have a file that is no longer in the file system
	std::vector<mvceditor::ResourceClass>::iterator it = matches.begin();
	std::vector<int> fileItemIdsToRemove;
	while (it != matches.end()) {

		// native matches wont have a FileItem assigned to them since they come from the tag file
		// FileItemId is meaningless for dynamic resources
		// is a file is new it wont be on disk; results are never stale in this case.
		bool remove = false;
		if (!it->IsNative && !it->IsDynamic && !it->FileIsNew) {
			wxFileName fileName = it->FileName();
			if (fileName.IsOk() && !fileName.FileExists()) {
				remove = true;
			}
		}
		if (remove) {
			fileItemIdsToRemove.push_back(it->FileItemId);
			it = matches.erase(it);
		}
		else {
			++it;
		}
	}
}

bool mvceditor::ResourceFinderClass::IsFileCacheEmpty() {
	if (!IsCacheInitialized) {
		return true;
	}
	int count = 0;
	try {
		Session->once << "SELECT COUNT(*) FROM file_items;", soci::into(count);
	} catch (std::exception& e) {
			
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		e.what();
	}
	return count <= 0;
}

bool mvceditor::ResourceFinderClass::IsResourceCacheEmpty() {
	if (!IsCacheInitialized) {
		return true;
	}

	// make sure only parsed resource came from the native functions file.
	int count = 0;
	try {
		Session->once << "SELECT COUNT(*) FROM resources WHERE is_native = 0;", soci::into(count);
	} catch (std::exception& e) {
		
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		e.what();
	}
	return count <= 0;
}

bool mvceditor::ResourceFinderClass::FindFileItemByFullPathExact(const wxString& fullPath, mvceditor::FileItemClass& fileItem) {
	if (!IsCacheInitialized) {
		return false;
	}
	int fileItemId;
	std::tm lastModified;
	int isParsed;
	int isNew;
	bool foundFile = false;

	std::string query = mvceditor::WxToChar(fullPath);
	std::string sql = "SELECT file_item_id, last_modified, is_parsed, is_new FROM file_items WHERE full_path = ?";
	try {
		soci::statement stmt = (Session->prepare << sql, soci::use(query), 
			soci::into(fileItemId), soci::into(lastModified), soci::into(isParsed), soci::into(isNew)
		);
		foundFile = stmt.execute(true);
		if (foundFile) {
			fileItem.DateTime.Set(lastModified);
			fileItem.FileId = fileItemId;
			fileItem.FullPath = fullPath;
			fileItem.IsNew = isNew != 0;
			fileItem.IsParsed = isParsed != 0;
		}
	} catch (std::exception& e) {
			
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		e.what();
	}
	return foundFile;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::ResourceStatementMatches(std::string whereCond, bool doLimit) {
	std::string sql;
	sql += "SELECT r.file_item_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON(r.file_item_id = f.file_item_id) WHERE ";
	sql += whereCond;
	sql += " ORDER BY key";
	if (doLimit) {
		sql += " LIMIT 100";
	}

	std::vector<mvceditor::ResourceClass> matches;
	if (!IsCacheInitialized) {
		return matches;
	}
	int fileItemId;
	std::string key;
	std::string identifier;
	std::string className;
	int type;
	std::string namespaceName;
	std::string signature;
	std::string returnType;
	std::string comment;
	std::string fullPath;
	int isProtected;
	int isPrivate;
	int isStatic;
	int isDynamic;
	int isNative;
	int fileIsNew;
	soci::indicator fileItemIdIndicator,
		fullPathIndicator,
		fileIsNewIndicator;
	try {
		soci::statement stmt = (Session->prepare << sql,
			soci::into(fileItemId, fileItemIdIndicator), soci::into(key), soci::into(identifier), soci::into(className), 
			soci::into(type), soci::into(namespaceName), soci::into(signature), 
			soci::into(returnType), soci::into(comment), soci::into(fullPath, fullPathIndicator), soci::into(isProtected), soci::into(isPrivate), 
			soci::into(isStatic), soci::into(isDynamic), soci::into(isNative), soci::into(fileIsNew, fileIsNewIndicator)
		);
		if (stmt.execute(true)) {
			do {
				mvceditor::ResourceClass resource;
				if (soci::i_ok == fileItemIdIndicator) {
					resource.FileItemId = fileItemId;
				}
				resource.Key = mvceditor::CharToIcu(key.c_str());
				resource.Identifier = mvceditor::CharToIcu(identifier.c_str());
				resource.ClassName = mvceditor::CharToIcu(className.c_str());
				resource.Type = (mvceditor::ResourceClass::Types)type;
				resource.NamespaceName = mvceditor::CharToIcu(namespaceName.c_str());
				resource.Signature = mvceditor::CharToIcu(signature.c_str());
				resource.ReturnType = mvceditor::CharToIcu(returnType.c_str());
				resource.Comment = mvceditor::CharToIcu(comment.c_str());
				if (soci::i_ok == fullPathIndicator) {
					resource.SetFullPath(mvceditor::CharToWx(fullPath.c_str()));
				}
				resource.IsProtected = isProtected != 0;
				resource.IsPrivate = isPrivate != 0;
				resource.IsStatic = isStatic != 0;
				resource.IsDynamic = isDynamic != 0;
				resource.IsNative = isNative != 0;
				if (soci::i_ok == fileIsNewIndicator) {
					resource.FileIsNew = fileIsNew != 0;
				}
				else {
					resource.FileIsNew = true;
				}

				matches.push_back(resource);
			} while (stmt.fetch());
		}
	} catch (std::exception& e) {
			
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		e.what();
	}
	return matches;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::FindByKeyExact(const std::string& key) {
	
	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	std::string whereCond = "key = '" + key + "'";
	return ResourceStatementMatches(whereCond, false);
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::FindByKeyExactAndTypes(const std::string& key, const std::vector<int>& types, bool doLimit) {
	std::ostringstream stream;

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	stream << "key = '" << key << "' AND type IN(";
	for (size_t i = 0; i < types.size(); ++i) {
		stream << types[i];
		if (i < (types.size() - 1)) {
			stream << ",";
		}
	}
	stream << ")";
	return ResourceStatementMatches(stream.str(), doLimit);
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::FindByKeyStart(const std::string& keyStart, bool doLimit) {
	std::string escaped = SqlEscape(keyStart, '^');
	std::string whereCond = "key LIKE '" + escaped + "%' ESCAPE '^' ";
	return ResourceStatementMatches(whereCond, doLimit);
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::FindByKeyStartAndTypes(const std::string& keyStart, const std::vector<int>& types, bool doLimit) {
	std::ostringstream stream;
	std::string escaped = SqlEscape(keyStart, '^');
	stream << "key LIKE '" << escaped << "%' ESCAPE '^' AND type IN(";
	for (size_t i = 0; i < types.size(); ++i) {
		stream << types[i];
		if (i < (types.size() - 1)) {
			stream << ",";
		}
	}
	stream << ")";
	return ResourceStatementMatches(stream.str(), doLimit);
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::FindByKeyStartMany(const std::vector<std::string>& keyStarts, bool doLimit) {
	if (keyStarts.empty()) {
		std::vector<mvceditor::ResourceClass> matches;
		return matches;
	}
	std::string escaped = SqlEscape(keyStarts[0], '^');
	std::ostringstream stream;
	stream << "key LIKE '" << escaped << "%' ESCAPE '^' ";
	for (size_t i = 1; i < keyStarts.size(); ++i) {
		escaped = SqlEscape(keyStarts[i], '^');
		stream << " OR key LIKE '" << escaped << "%' ESCAPE '^' ";
	}
	return ResourceStatementMatches(stream.str(), true);
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::FindByIdentifierExactAndTypes(const std::string& identifier, const std::vector<int>& types, bool doLimit) {
	std::ostringstream stream;

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	// do not get fully qualified resources
	// make sure to use the key because it is indexed
	stream << "key = '" << identifier << "' AND identifier = key AND type IN(";
	for (size_t i = 0; i < types.size(); ++i) {
		stream << types[i];
		if (i < (types.size() - 1)) {
			stream << ",";
		}
	}
	stream << ")";
	return ResourceStatementMatches(stream.str(), doLimit);

}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::FindByIdentifierStartAndTypes(const std::string& identifierStart, const std::vector<int>& types, bool doLimit) {
	std::ostringstream stream;

	// do not get fully qualified resources
	// make sure to use the key because it is indexed
	std::string escaped = SqlEscape(identifierStart, '^');
	stream << "key LIKE '" << escaped << "%' ESCAPE '^' AND identifier = key AND type IN(";
	for (size_t i = 0; i < types.size(); ++i) {
		stream << types[i];
		if (i < (types.size() - 1)) {
			stream << ",";
		}
	}
	stream << ")";
	return ResourceStatementMatches(stream.str(), doLimit);
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::All() {
	std::vector<mvceditor::ResourceClass> all = ResourceStatementMatches("1=1", false);
	
	// remove the 'duplicates' ie. extra fully qualified entries to make lookups faster
	std::vector<mvceditor::ResourceClass>::iterator it = all.begin();
	while (it != all.end()) {
		if (it->Key.indexOf(UNICODE_STRING_SIMPLE("::")) > 0) {

			// fully qualified methods
			it = all.erase(it);
		}
		else if (it->Type != mvceditor::ResourceClass::NAMESPACE && it->Key.indexOf(UNICODE_STRING_SIMPLE("\\")) >= 0) {
			
			// fully qualified classes / functions (with namespace)
			it = all.erase(it);
		}
		else {
			++it;
		}
	}
	return all;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::AllNonNativeClasses() {
	std::ostringstream stream;
	stream << " is_native = 0 AND type =" << mvceditor::ResourceClass::CLASS;
	std::vector<mvceditor::ResourceClass> all = ResourceStatementMatches(stream.str(), false);
	return all;
}

std::vector<mvceditor::TraitResourceClass> mvceditor::ResourceFinderClass::FindTraitsByClassName(const std::vector<std::string>& keyStarts) {
	std::string join = "";
	for (size_t i = 0; i < keyStarts.size(); ++i) {
		join += "'";
		join += keyStarts[i];
		join += "'";
		if (i < (keyStarts.size() - 1)) {
			join += ",";
		}
	}
	
	std::string sql = "SELECT key, class_name, namespace_name, trait_name, trait_namespace_name, aliases, instead_ofs ";
	sql += "FROM trait_resources WHERE key IN(" + join + ")";
	std::string key;
	std::string className;
	std::string namespaceName;
	std::string traitClassName;
	std::string traitNamespaceName;
	std::string aliases;
	std::string insteadOfs;
	std::vector<mvceditor::TraitResourceClass> matches;
	try {
		soci::statement stmt = (Session->prepare << sql,
			soci::into(key), soci::into(className), soci::into(namespaceName), soci::into(traitClassName),
			soci::into(traitNamespaceName), soci::into(aliases), soci::into(insteadOfs)
		);
		if (stmt.execute(true)) {
			do {
				mvceditor::TraitResourceClass trait;
				trait.Key = mvceditor::CharToIcu(key.c_str());
				trait.ClassName = mvceditor::CharToIcu(className.c_str());
				trait.NamespaceName = mvceditor::CharToIcu(namespaceName.c_str());
				trait.TraitClassName = mvceditor::CharToIcu(traitClassName.c_str());
				trait.TraitNamespaceName = mvceditor::CharToIcu(traitNamespaceName.c_str());
				
				size_t start = 0;
				size_t found = aliases.find_first_of(",");
				while (found != std::string::npos) {
					trait.Aliased.push_back(mvceditor::CharToIcu(aliases.substr(start, found).c_str()));	
					start = found++;
				}
				if (!aliases.empty()) {
					trait.Aliased.push_back(mvceditor::CharToIcu(aliases.substr(start, found).c_str()));
				}

				start = 0;
				found = insteadOfs.find_first_of(",");
				while (found != std::string::npos) {
					trait.InsteadOfs.push_back(mvceditor::CharToIcu(insteadOfs.substr(start, found).c_str()));	
					start = found++;
				}
				if (!insteadOfs.empty()) {
					trait.InsteadOfs.push_back(mvceditor::CharToIcu(insteadOfs.substr(start, found).c_str()));
				}

				matches.push_back(trait);
			} while (stmt.fetch());
		}
	} catch (std::exception& e) {
			
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		e.what();
	}
	return matches;
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
	, Key()
	, FullPath()
	, FileItemId(-1) 
	, FileIsNew(false) {
		
}

mvceditor::ResourceClass::ResourceClass(const mvceditor::ResourceClass& src)
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
	, Key()
	, FullPath()
	, FileItemId(-1) 
	, FileIsNew(false) {
	Copy(src);
}

void mvceditor::ResourceClass::operator=(const ResourceClass& src) {
	Copy(src);
}

void mvceditor::ResourceClass::Copy(const mvceditor::ResourceClass& src) {
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
	FileItemId = src.FileItemId;
	IsProtected = src.IsProtected;
	IsPrivate = src.IsPrivate;
	IsStatic = src.IsStatic;
	IsDynamic = src.IsDynamic;
	IsNative = src.IsNative;
	FileIsNew = src.FileIsNew;
}

bool mvceditor::ResourceClass::operator<(const mvceditor::ResourceClass& a) const {
	return Key.caseCompare(a.Key, 0) < 0;
}

bool mvceditor::ResourceClass::operator==(const mvceditor::ResourceClass& a) const {
	return Identifier == a.Identifier && ClassName == a.ClassName && NamespaceName == a.NamespaceName;
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
	FileItemId = -1;
	FullPath = wxT("");
	Key.remove();
	IsProtected = false;
	IsPrivate = false;
	IsStatic = false;
	IsDynamic = false;
	IsNative = false;
	FileIsNew = false;
}

mvceditor::ResourceClass mvceditor::ResourceClass::MakeNamespace(const UnicodeString& namespaceName) {
	mvceditor::ResourceClass namespaceItem;
	namespaceItem.Type = mvceditor::ResourceClass::NAMESPACE;
	namespaceItem.NamespaceName = namespaceName;
	namespaceItem.Identifier = namespaceName;
	namespaceItem.Key = namespaceName;
	return namespaceItem;
}

wxFileName mvceditor::ResourceClass::FileName() const {
	wxFileName fileName(FullPath);
	return fileName;
}

wxString  mvceditor::ResourceClass::GetFullPath() const {
	return FullPath;
}

void mvceditor::ResourceClass::SetFullPath(const wxString& fullPath) {
	FullPath = fullPath;
}

bool mvceditor::ResourceClass::HasParameters() const {

	// watch out for default argument of "array()"
	// look for the function name followed by parentheses
	return Signature.indexOf(Identifier + UNICODE_STRING_SIMPLE("()")) < 0;
}

mvceditor::TraitResourceClass::TraitResourceClass() 
	: TraitClassName()
	, Aliased()
	, InsteadOfs() {
		
}

mvceditor::FileItemClass::FileItemClass() 
	: FullPath()
	, DateTime()
	, FileId(0)
	, IsParsed(false)
	, IsNew(true) {

}

bool mvceditor::FileItemClass::NeedsToBeParsed(const wxDateTime& fileLastModifiedDateTime) const {
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

void mvceditor::FileItemClass::MakeNew(const wxFileName& fileName, bool isParsed) {
	wxDateTime fileLastModifiedDateTime = fileName.GetModificationTime();

	FullPath = fileName.GetFullPath();
	DateTime = fileLastModifiedDateTime;
	FileId = 0;
	IsParsed = isParsed;
	IsNew = false;
}