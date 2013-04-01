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
#include <search/TagFinderClass.h>
#include <search/FinderClass.h>
#include <globals/String.h>
#include <globals/Sqlite.h>
#include <globals/Errors.h>
#include <language/FileTags.h>
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

static void InClause(const std::vector<int>& values, std::ostringstream& stream) {
	size_t size = values.size();
	for (size_t i = 0; i < size; ++i) {
		stream << values[i];
		if (i < (size - 1)) {
			stream << ",";
		}
	}
}

mvceditor::TagSearchClass::TagSearchClass(UnicodeString resourceQuery)
	: FileName()
	, ClassName()
	, MethodName()
	, Dirs()
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

void mvceditor::TagSearchClass::SetParentClasses(const std::vector<UnicodeString>& parents) {
	ParentClasses = parents;
}

std::vector<UnicodeString> mvceditor::TagSearchClass::GetParentClasses() const {
	return ParentClasses;
}

void mvceditor::TagSearchClass::SetTraits(const std::vector<UnicodeString>& traits) {
	Traits = traits;
}

std::vector<UnicodeString> mvceditor::TagSearchClass::GetTraits() const {
	return Traits;
}

void mvceditor::TagSearchClass::SetDirs(const std::vector<wxFileName>& dirs) {
	Dirs = dirs;
}

std::vector<wxFileName> mvceditor::TagSearchClass::GetDirs() const {
	return Dirs;
}

UnicodeString mvceditor::TagSearchClass::GetClassName() const {
	return ClassName;
}

UnicodeString mvceditor::TagSearchClass::GetFileName() const {
	return FileName;
}

UnicodeString mvceditor::TagSearchClass::GetMethodName() const {
	return MethodName;
}

int mvceditor::TagSearchClass::GetLineNumber() const {
	return LineNumber;
}

mvceditor::TagSearchClass::ResourceTypes mvceditor::TagSearchClass::GetResourceType() const {
	return ResourceType;
}


mvceditor::TagFinderClass::TagFinderClass()
	: Session(NULL)
	, IsCacheInitialized(false) {
}

mvceditor::TagFinderClass::~TagFinderClass() {

}

void mvceditor::TagFinderClass::Init(soci::session* session) {
	Session = session;
	IsCacheInitialized = true;
}

bool mvceditor::TagFinderClass::GetResourceMatchPosition(const mvceditor::TagClass& tag, const UnicodeString& text, int32_t& pos, 
		int32_t& length) {
	size_t start = 0;
	mvceditor::FinderClass finder;
	finder.Mode = FinderClass::REGULAR_EXPRESSION;

	UnicodeString className,
		methodName;
	if (!tag.ClassName.isEmpty()) {
		className = tag.ClassName;
		methodName = tag.Identifier;

		mvceditor::FinderClass::EscapeRegEx(className);
		mvceditor::FinderClass::EscapeRegEx(methodName);
	}
	else {
		className = tag.Identifier;
		mvceditor::FinderClass::EscapeRegEx(className);
	}
	switch (tag.Type) {
		case TagClass::CLASS:
			finder.Expression = UNICODE_STRING_SIMPLE("\\sclass\\s+") + className + UNICODE_STRING_SIMPLE("\\s");
			break;
		case TagClass::METHOD:
			//advance past the class header so that if  a function with the same name exists we will skip it
			finder.Expression = UNICODE_STRING_SIMPLE("\\sclass\\s+") + className + UNICODE_STRING_SIMPLE("\\s");
			if (finder.Prepare() && finder.FindNext(text, start)) {			
				finder.GetLastMatch(pos, length);
			}
			start = pos + length;

			// method may return a reference (&)
			finder.Expression = UNICODE_STRING_SIMPLE("\\sfunction\\s*(&\\s*)?") + methodName + UNICODE_STRING_SIMPLE("\\s*\\(");
			break;
		case TagClass::FUNCTION:

			// function may return a reference (&)
			finder.Expression = UNICODE_STRING_SIMPLE("\\sfunction\\s*(&\\s*)?") + className + UNICODE_STRING_SIMPLE("\\s*\\(");
			break;
		case TagClass::MEMBER:
			//advance past the class header so that if  a variable with the same name exists we will skip it				:
			finder.Expression = UNICODE_STRING_SIMPLE("\\sclass\\s+") + className + UNICODE_STRING_SIMPLE("\\s");
			if (finder.Prepare() && finder.FindNext(text, start)) {			
				finder.GetLastMatch(pos, length);
			}
			start = pos + length;
			finder.Expression = UNICODE_STRING_SIMPLE("\\s((var)|(public)|(protected)|(private)).+") + methodName + UNICODE_STRING_SIMPLE(".*;");
			break;
		case TagClass::DEFINE:
			finder.Expression = UNICODE_STRING_SIMPLE("\\sdefine\\(\\s*('|\")") + className + UNICODE_STRING_SIMPLE("('|\")");
			break;
		case TagClass::CLASS_CONSTANT:
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

std::vector<mvceditor::TagClass> mvceditor::TagFinderClass::NearMatchTags(
	const mvceditor::TagSearchClass& tagSearch,
	bool doCollectFileNames) {

	
	// at one point there was a check here to see if the  tag files existed
	// it was removed because it caused performance issues, since this method
	// is called while the user is typing text.
	// take care when coding; make sure that any code called by this method does not touch the file system
	std::vector<mvceditor::TagClass> matches;
	bool error = false;
	wxString errorMsg;
	std::vector<int> fileTagIds = mvceditor::FileTagIdsForDirs(*Session, tagSearch.GetDirs(), error, errorMsg);
	wxASSERT_MSG(!error, errorMsg);
	switch (tagSearch.GetResourceType()) {
		case mvceditor::TagSearchClass::FILE_NAME:
		case mvceditor::TagSearchClass::FILE_NAME_LINE_NUMBER:
			matches = NearMatchFiles(tagSearch.GetFileName(), tagSearch.GetLineNumber(), fileTagIds);
			break;
		case mvceditor::TagSearchClass::CLASS_NAME:
			matches = NearMatchNonMembers(tagSearch, true, true, true);
			if (matches.empty() && doCollectFileNames) {
				matches = NearMatchFiles(tagSearch.GetClassName(), tagSearch.GetLineNumber(), fileTagIds);
			}
			break;
		case mvceditor::TagSearchClass::CLASS_NAME_METHOD_NAME:
			matches = NearMatchMembers(tagSearch);
			break;
		case mvceditor::TagSearchClass::NAMESPACE_NAME:
			matches = NearMatchNamespaces(tagSearch);
			break;
	}
	sort(matches.begin(), matches.end());
	return matches;
}

std::vector<mvceditor::TagClass> mvceditor::TagFinderClass::NearMatchClassesOrFiles(
	const mvceditor::TagSearchClass& tagSearch) {

	// at one point there was a check here to see if the  tag files existed
	// it was removed because it caused performance issues, since this method
	// is called while the user is typing text.
	// take care when coding; make sure that any code called by this method does not touch the file system
	std::vector<mvceditor::TagClass> matches;
	bool error = false;
	wxString errorMsg;
	std::vector<int> fileTagIds = mvceditor::FileTagIdsForDirs(*Session, tagSearch.GetDirs(), error, errorMsg);
	wxASSERT_MSG(!error, errorMsg);
	switch (tagSearch.GetResourceType()) {
		case mvceditor::TagSearchClass::FILE_NAME:
		case mvceditor::TagSearchClass::FILE_NAME_LINE_NUMBER:
			matches = NearMatchFiles(tagSearch.GetFileName(), tagSearch.GetLineNumber(), fileTagIds);
			break;
		case mvceditor::TagSearchClass::CLASS_NAME:
		case mvceditor::TagSearchClass::CLASS_NAME_METHOD_NAME:
		case mvceditor::TagSearchClass::NAMESPACE_NAME:
			matches = NearMatchNonMembers(tagSearch, true, false, false);
			if (matches.empty()) {
				matches = NearMatchFiles(tagSearch.GetClassName(), tagSearch.GetLineNumber(), fileTagIds);
			}
			break;
	}
	sort(matches.begin(), matches.end());
	return matches;
}

std::vector<mvceditor::TagClass> mvceditor::TagFinderClass::NearMatchNonMembers(const mvceditor::TagSearchClass& tagSearch, bool doClasses, bool doDefines, bool doFunctions) {
	std::string key = mvceditor::IcuToChar(tagSearch.GetClassName());
	std::vector<int> types;
	if (doClasses) {
		types.push_back(mvceditor::TagClass::CLASS);
	}
	if (doDefines) {
		types.push_back(mvceditor::TagClass::DEFINE);
	}
	if (doFunctions) {
		types.push_back(mvceditor::TagClass::FUNCTION);
	}
	bool error = false;
	wxString errorMsg;
	std::vector<int> fileTagIds = mvceditor::FileTagIdsForDirs(*Session, tagSearch.GetDirs(), error, errorMsg);
	wxASSERT_MSG(!error, errorMsg);
	std::vector<mvceditor::TagClass> matches = FindByKeyExactAndTypes(key, types, fileTagIds, true);
	if (matches.empty()) {
	
		// if nothing matches exactly then execure the LIKE query
		matches = FindByKeyStartAndTypes(key, types, fileTagIds, true);
	}
	return matches;
}

std::vector<mvceditor::TagClass> mvceditor::TagFinderClass::NearMatchMembers(const mvceditor::TagSearchClass& tagSearch) {
	std::vector<mvceditor::TagClass> matches;
	
	// when looking for members we need to look
	// 1. in the class itself
	// 2. in any parent classes
	// 3. in any used traits
	std::vector<UnicodeString> classesToSearch = tagSearch.GetParentClasses();
	classesToSearch.push_back(tagSearch.GetClassName());
	std::vector<UnicodeString> traits = tagSearch.GetTraits();
	classesToSearch.insert(classesToSearch.end(), traits.begin(), traits.end());
	bool error = false;
	wxString errorMsg;
	std::vector<int> fileTagIds = mvceditor::FileTagIdsForDirs(*Session, tagSearch.GetDirs(), error, errorMsg);
	wxASSERT_MSG(!error, errorMsg);
	if (tagSearch.GetMethodName().isEmpty()) {
		
		// special case; query for all methods for a class (UserClass::)
		std::vector<mvceditor::TagClass> memberMatches = AllMembers(classesToSearch, fileTagIds);

		//get the methods that belong to a used trait
		std::vector<mvceditor::TagClass> traitMatches = TraitAliases(classesToSearch, UNICODE_STRING_SIMPLE(""), fileTagIds);

		matches.insert(matches.end(), memberMatches.begin(), memberMatches.end());
		matches.insert(matches.end(), traitMatches.begin(), traitMatches.end());
	}
	else if (tagSearch.GetClassName().isEmpty()) {
		
		// special case, query across all classes for a method (::getName)
		// if ClassName is empty, then just check method names This ensures 
		// queries like '::getName' will work as well.
		// make sure to NOT get fully qualified  matches (key=identifier)
		std::string identifier = mvceditor::IcuToChar(tagSearch.GetMethodName());
		std::vector<int> types;
		types.push_back(mvceditor::TagClass::MEMBER);
		types.push_back(mvceditor::TagClass::METHOD);
		types.push_back(mvceditor::TagClass::CLASS_CONSTANT);
		matches = FindByIdentifierExactAndTypes(identifier, types, fileTagIds, true);
		if (matches.empty()) {
		
			// use LIKE to get near matches
			matches = FindByIdentifierStartAndTypes(identifier, types, fileTagIds, true);
		}
	}
	else {
		std::vector<std::string> keyStarts;

		// now that we found the parent classes, combine the parent class name and the queried method
		// to make all of the keys we need to look for. remember that a tag class key is of the form
		// ClassName::MethodName
		for (std::vector<UnicodeString>::iterator it = classesToSearch.begin(); it != classesToSearch.end(); ++it) {
			std::string keyStart = mvceditor::IcuToChar(*it);
			keyStart += "::";
			keyStart += mvceditor::IcuToChar(tagSearch.GetMethodName());
			keyStarts.push_back(keyStart);
		}
		matches = FindByKeyStartMany(keyStarts, fileTagIds, true);

		// get any aliases
		std::vector<mvceditor::TagClass> traitMatches = TraitAliases(classesToSearch, tagSearch.GetMethodName(), fileTagIds);
		matches.insert(matches.end(), traitMatches.begin(), traitMatches.end());
			
	}
	return matches;
}

std::vector<mvceditor::TagClass> mvceditor::TagFinderClass::AllMembers(const std::vector<UnicodeString>& classNames, const std::vector<int>& fileTagIds) {
	std::vector<mvceditor::TagClass> matches;
	std::vector<std::string> keyStarts;
	for (size_t i = 0; i < classNames.size(); ++i) {
		std::string keyStart = mvceditor::IcuToChar(classNames[i]);
		keyStart += "::";
		keyStarts.push_back(keyStart);
	}
	matches = FindByKeyStartMany(keyStarts, fileTagIds, true);
	return matches;
}

std::vector<mvceditor::TagClass>  mvceditor::TagFinderClass::NearMatchNamespaces(const mvceditor::TagSearchClass& tagSearch) {
	std::vector<mvceditor::TagClass> matches;

	// needle identifier contains a namespace operator; but it may be
	// a namespace or a fully qualified name
	UnicodeString key = tagSearch.GetClassName();
	std::string stdKey = mvceditor::IcuToChar(key);
	bool error = false;
	wxString errorMsg;
	std::vector<int> fileTagIds = mvceditor::FileTagIdsForDirs(*Session, tagSearch.GetDirs(), error, errorMsg);
	wxASSERT_MSG(!error, errorMsg);
	matches = FindByKeyExact(stdKey, fileTagIds);
	if (matches.empty()) {
		matches = FindByKeyStart(stdKey, fileTagIds, true);
	}
	return matches;
}

UnicodeString mvceditor::TagFinderClass::ParentClassName(const UnicodeString& className) {
	UnicodeString parentClassName;

	// first query to get the parent class name
	std::vector<int> types;
	types.push_back(mvceditor::TagClass::CLASS);
	std::string key = mvceditor::IcuToChar(className);
	
	// empty file items == search on all files
	std::vector<int> fileTagIds;
	std::vector<mvceditor::TagClass> matches = FindByKeyExactAndTypes(key, types, fileTagIds, true);
	if (!matches.empty()) {
		mvceditor::TagClass tag = matches[0];
		parentClassName = ExtractParentClassFromSignature(tag.Signature);
	}
	return parentClassName;
}

std::vector<UnicodeString> mvceditor::TagFinderClass::GetResourceTraits(const UnicodeString& className, 
																		const UnicodeString& methodName,
																		const std::vector<wxFileName>& dirs) {
	bool error = false;
	wxString errorMsg;
	std::vector<int> fileTagIds = mvceditor::FileTagIdsForDirs(*Session, dirs, error, errorMsg);
	wxASSERT_MSG(!error, errorMsg);
	std::vector<UnicodeString> inheritedTraits;
	bool match = false;
	
	std::vector<std::string> keys;
	keys.push_back(mvceditor::IcuToChar(className));
	std::vector<mvceditor::TraitTagClass> matches = UsedTraits(keys, fileTagIds);
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

int mvceditor::TagFinderClass::GetLineCountFromFile(const wxString& fullPath) const {
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

UnicodeString mvceditor::TagFinderClass::ExtractParentClassFromSignature(const UnicodeString& signature) const {

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

std::vector<mvceditor::TagClass> mvceditor::TagFinderClass::ExactTags(const mvceditor::TagSearchClass& tagSearch) {

	// at one point there was a check here to see if the  tag files existed
	// it was removed because it caused performance issues, since this method
	// is called while the user is typing text.
	// take care when coding; make sure that any code called by this method does not touch the file system
	std::vector<mvceditor::TagClass> allMatches;
	std::vector<int> types;
	bool error = false;
	wxString errorMsg;
	std::vector<int> fileTagIds = mvceditor::FileTagIdsForDirs(*Session, tagSearch.GetDirs(), error, errorMsg);
	wxASSERT_MSG(!error, errorMsg);
	if (tagSearch.GetResourceType() == mvceditor::TagSearchClass::CLASS_NAME_METHOD_NAME) {

		// check the entire class hierachy; stop as soon as we found it
		// combine the parent classes with the class being searched
		std::vector<UnicodeString> classHierarchy = tagSearch.GetParentClasses();
		classHierarchy.push_back(tagSearch.GetClassName());
		
		types.push_back(mvceditor::TagClass::MEMBER);
		types.push_back(mvceditor::TagClass::METHOD);
		types.push_back(mvceditor::TagClass::CLASS_CONSTANT);
		for (size_t i = 0; i < classHierarchy.size(); ++i) {
			UnicodeString key = classHierarchy[i] + UNICODE_STRING_SIMPLE("::") + tagSearch.GetMethodName();
			std::string stdKey = mvceditor::IcuToChar(key);
			std::vector<mvceditor::TagClass> matches = FindByKeyExactAndTypes(stdKey, types, fileTagIds, true);
			allMatches.insert(allMatches.end(), matches.begin(), matches.end());
		}
	}
	else if (mvceditor::TagSearchClass::NAMESPACE_NAME == tagSearch.GetResourceType()) {
		UnicodeString key = tagSearch.GetClassName();
		std::string stdKey = mvceditor::IcuToChar(key);
		types.push_back(mvceditor::TagClass::DEFINE);
		types.push_back(mvceditor::TagClass::CLASS);
		types.push_back(mvceditor::TagClass::FUNCTION);

		allMatches = FindByKeyExactAndTypes(stdKey, types, fileTagIds, true);
	}
	else {
		UnicodeString key = tagSearch.GetClassName();
		std::string stdKey = mvceditor::IcuToChar(key);
		types.push_back(mvceditor::TagClass::DEFINE);
		types.push_back(mvceditor::TagClass::CLASS);
		types.push_back(mvceditor::TagClass::FUNCTION);

		allMatches = FindByKeyExactAndTypes(stdKey, types, fileTagIds, true);
	}
	return allMatches;
}

std::vector<mvceditor::TagClass> mvceditor::TagFinderClass::ExactClassOrFile(const mvceditor::TagSearchClass& tagSearch) {

	// at one point there was a check here to see if the  tag files existed
	// it was removed because it caused performance issues, since this method
	// is called while the user is typing text.
	// take care when coding; make sure that any code called by this method does not touch the file system
	std::vector<mvceditor::TagClass> allMatches;
	std::vector<int> types;
	bool error = false;
	wxString errorMsg;
	std::vector<int> fileTagIds = mvceditor::FileTagIdsForDirs(*Session, tagSearch.GetDirs(), error, errorMsg);
	wxASSERT_MSG(!error, errorMsg);
	if (mvceditor::TagSearchClass::FILE_NAME == tagSearch.GetResourceType() || 
		mvceditor::TagSearchClass::FILE_NAME_LINE_NUMBER == tagSearch.GetResourceType()) {
		allMatches = ExactFiles(tagSearch.GetFileName(), tagSearch.GetLineNumber());
	}
	else {
		UnicodeString key = tagSearch.GetClassName();
		std::string stdKey = mvceditor::IcuToChar(key);
		types.push_back(mvceditor::TagClass::CLASS);
		allMatches = FindByKeyExactAndTypes(stdKey, types, fileTagIds, true);
	}
	return allMatches;
}

void mvceditor::TagFinderClass::EnsureMatchesExist(std::vector<TagClass>& matches) {

	// remove from matches that have a file that is no longer in the file system
	std::vector<mvceditor::TagClass>::iterator it = matches.begin();
	std::vector<int> fileTagIdsToRemove;
	while (it != matches.end()) {

		// native matches wont have a FileTag assigned to them since they come from the tag file
		// FileTagId is meaningless for dynamic resources
		// is a file is new it wont be on disk; results are never stale in this case.
		bool remove = false;
		if (!it->IsNative && !it->IsDynamic && !it->FileIsNew) {
			wxFileName fileName = it->FileName();
			if (fileName.IsOk() && !fileName.FileExists()) {
				remove = true;
			}
		}
		if (remove) {
			fileTagIdsToRemove.push_back(it->FileTagId);
			it = matches.erase(it);
		}
		else {
			++it;
		}
	}
}

bool mvceditor::TagFinderClass::IsFileCacheEmpty() {
	if (!IsCacheInitialized) {
		return true;
	}
	int count = 0;
	try {
		Session->once << "SELECT COUNT(*) FROM file_items;", soci::into(count);
	} catch (std::exception& e) {
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return count <= 0;
}

bool mvceditor::TagFinderClass::IsResourceCacheEmpty() {
	if (!IsCacheInitialized) {
		return true;
	}

	// make sure only parsed tag came from the native functions file.
	int count = 0;
	try {
		Session->once << "SELECT COUNT(*) FROM resources WHERE is_native = 0;", soci::into(count);
	} catch (std::exception& e) {
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return count <= 0;
}

bool mvceditor::TagFinderClass::HasFullPath(const wxString& fullPath) {
	mvceditor::FileTagClass fileTag;
	return FindFileTagByFullPathExact(fullPath, fileTag);
}

bool mvceditor::TagFinderClass::FindFileTagByFullPathExact(const wxString& fullPath, mvceditor::FileTagClass& fileTag) {
	if (!IsCacheInitialized) {
		return false;
	}
	int fileTagId;
	std::tm lastModified;
	int isParsed;
	int isNew;
	bool foundFile = false;

	std::string query = mvceditor::WxToChar(fullPath);
	std::string sql = "SELECT file_item_id, last_modified, is_parsed, is_new FROM file_items WHERE full_path = ?";
	try {
		soci::statement stmt = (Session->prepare << sql, soci::use(query), 
			soci::into(fileTagId), soci::into(lastModified), soci::into(isParsed), soci::into(isNew)
		);
		foundFile = stmt.execute(true);
		if (foundFile) {
			fileTag.DateTime.Set(lastModified);
			fileTag.FileId = fileTagId;
			fileTag.FullPath = fullPath;
			fileTag.IsNew = isNew != 0;
			fileTag.IsParsed = isParsed != 0;
		}
	} catch (std::exception& e) {
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return foundFile;
}

std::vector<mvceditor::TagClass> mvceditor::TagFinderClass::FindByKeyExact(const std::string& key, const std::vector<int>& fileTagIds) {
	
	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	// TODO escape key
	std::ostringstream stream;
	stream << "key = '" + key + "'";
	if (!fileTagIds.empty()) {
		stream << " AND f.file_item_id IN(";
		InClause(fileTagIds, stream);
		stream << ")";
	}
	return ResourceStatementMatches(stream.str(), false);
}

std::vector<mvceditor::TagClass> mvceditor::TagFinderClass::FindByKeyExactAndTypes(const std::string& key, const std::vector<int>& types, 
																				   const std::vector<int>& fileTagIds, bool doLimit) {
	std::ostringstream stream;
	
	// TODO escape key
	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	stream << "key = '" << key << "' AND type IN(";
	InClause(types, stream);
	stream << ")";
	if (!fileTagIds.empty()) {
		stream << " AND f.file_item_id IN(";
		InClause(fileTagIds, stream);
		stream << ")";
	}
	return ResourceStatementMatches(stream.str(), doLimit);
}

std::vector<mvceditor::TagClass> mvceditor::TagFinderClass::FindByKeyStart(const std::string& keyStart, const std::vector<int>& fileTagIds, bool doLimit) {
	std::string escaped = mvceditor::SqliteSqlEscape(keyStart, '^');
	std::ostringstream stream;
	stream << "key LIKE '" << escaped << "%' ESCAPE '^' ";
	if (!fileTagIds.empty()) {
		stream << " AND f.file_item_id IN(";
		InClause(fileTagIds, stream);
		stream << ")";
	}
	return ResourceStatementMatches(stream.str(), doLimit);
}

std::vector<mvceditor::TagClass> mvceditor::TagFinderClass::FindByKeyStartAndTypes(const std::string& keyStart, 
																				   const std::vector<int>& types, 
																				   const std::vector<int>& fileTagIds,
																				   bool doLimit) {
	std::ostringstream stream;
	std::string escaped = mvceditor::SqliteSqlEscape(keyStart, '^');
	stream << "key LIKE '" << escaped << "%' ESCAPE '^' AND type IN(";
	InClause(types, stream);
	stream << ")";
	if (!fileTagIds.empty()) {
		stream << " AND f.file_item_id IN(";
		InClause(fileTagIds, stream);
		stream << ")";
	}
	return ResourceStatementMatches(stream.str(), doLimit);
}

std::vector<mvceditor::TagClass> mvceditor::TagFinderClass::FindByKeyStartMany(const std::vector<std::string>& keyStarts, const std::vector<int>& fileTagIds, bool doLimit) {
	if (keyStarts.empty()) {
		std::vector<mvceditor::TagClass> matches;
		return matches;
	}
	std::string escaped = mvceditor::SqliteSqlEscape(keyStarts[0], '^');
	std::ostringstream stream;
	stream << "(key LIKE '" << escaped << "%' ESCAPE '^' ";
	for (size_t i = 1; i < keyStarts.size(); ++i) {
		escaped = mvceditor::SqliteSqlEscape(keyStarts[i], '^');
		stream << " OR key LIKE '" << escaped << "%' ESCAPE '^' ";
	}
	stream << ")";
	if (!fileTagIds.empty()) {
		stream << " AND f.file_item_id IN(";
		InClause(fileTagIds, stream);
		stream << ")";
	}
	return ResourceStatementMatches(stream.str(), true);
}

std::vector<mvceditor::TagClass> mvceditor::TagFinderClass::All() {
	std::vector<mvceditor::TagClass> all = ResourceStatementMatches("1=1", false);
	
	// remove the 'duplicates' ie. extra fully qualified entries to make lookups faster
	std::vector<mvceditor::TagClass>::iterator it = all.begin();
	while (it != all.end()) {
		if (it->Key.indexOf(UNICODE_STRING_SIMPLE("::")) > 0) {

			// fully qualified methods
			it = all.erase(it);
		}
		else if (it->Type != mvceditor::TagClass::NAMESPACE && it->Key.indexOf(UNICODE_STRING_SIMPLE("\\")) >= 0) {
			
			// fully qualified classes / functions (with namespace)
			it = all.erase(it);
		}
		else {
			++it;
		}
	}
	return all;
}

mvceditor::ParsedTagFinderClass::ParsedTagFinderClass()
	: TagFinderClass() {

}

std::vector<mvceditor::TagClass> mvceditor::ParsedTagFinderClass::ResourceStatementMatches(std::string whereCond, bool doLimit) {
	std::string sql;
	sql += "SELECT r.file_item_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON(r.file_item_id = f.file_item_id) WHERE ";
	sql += whereCond;
	sql += " ORDER BY key";
	if (doLimit) {
		sql += " LIMIT 100";
	}

	std::vector<mvceditor::TagClass> matches;
	if (!IsCacheInitialized) {
		return matches;
	}
	int fileTagId;
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
	soci::indicator fileTagIdIndicator,
		fullPathIndicator,
		fileIsNewIndicator;
	try {
		soci::statement stmt = (Session->prepare << sql,
			soci::into(fileTagId, fileTagIdIndicator), soci::into(key), soci::into(identifier), soci::into(className), 
			soci::into(type), soci::into(namespaceName), soci::into(signature), 
			soci::into(returnType), soci::into(comment), soci::into(fullPath, fullPathIndicator), soci::into(isProtected), soci::into(isPrivate), 
			soci::into(isStatic), soci::into(isDynamic), soci::into(isNative), soci::into(fileIsNew, fileIsNewIndicator)
		);
		if (stmt.execute(true)) {
			do {
				mvceditor::TagClass tag;
				if (soci::i_ok == fileTagIdIndicator) {
					tag.FileTagId = fileTagId;
				}
				tag.Key = mvceditor::CharToIcu(key.c_str());
				tag.Identifier = mvceditor::CharToIcu(identifier.c_str());
				tag.ClassName = mvceditor::CharToIcu(className.c_str());
				tag.Type = (mvceditor::TagClass::Types)type;
				tag.NamespaceName = mvceditor::CharToIcu(namespaceName.c_str());
				tag.Signature = mvceditor::CharToIcu(signature.c_str());
				tag.ReturnType = mvceditor::CharToIcu(returnType.c_str());
				tag.Comment = mvceditor::CharToIcu(comment.c_str());
				if (soci::i_ok == fullPathIndicator) {
					tag.SetFullPath(mvceditor::CharToWx(fullPath.c_str()));
				}
				tag.IsProtected = isProtected != 0;
				tag.IsPrivate = isPrivate != 0;
				tag.IsStatic = isStatic != 0;
				tag.IsDynamic = isDynamic != 0;
				tag.IsNative = isNative != 0;
				if (soci::i_ok == fileIsNewIndicator) {
					tag.FileIsNew = fileIsNew != 0;
				}
				else {
					tag.FileIsNew = true;
				}

				matches.push_back(tag);
			} while (stmt.fetch());
		}
	} catch (std::exception& e) {
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return matches;
}

std::vector<mvceditor::TagClass> mvceditor::ParsedTagFinderClass::NearMatchFiles(const UnicodeString& search, int lineNumber,
																				 const std::vector<int>& fileTagIds) {
	wxString path,
		currentFileName,
		extension;
	std::vector<mvceditor::TagClass> matches;
	std::string query = mvceditor::IcuToChar(search);

	// add the SQL wildcards
	std::string escaped = mvceditor::SqliteSqlEscape(query, '^');
	query = "'%" + escaped + "%'";
	std::string match;
	int fileTagId;
	int isNew;
	try {
		std::ostringstream stream;
		stream << "SELECT full_path, file_item_id, is_new FROM file_items WHERE full_path LIKE " + query + " ESCAPE '^'";
		if (!fileTagIds.empty()) {
			stream << " AND file_item_id IN(";
			InClause(fileTagIds, stream);
			stream << ")";
		}
		soci::statement stmt = (Session->prepare << stream.str(), 
			soci::into(match), soci::into(fileTagId), soci::into(isNew));
		if (stmt.execute(true)) {
			do {
				wxString fullPath = mvceditor::CharToWx(match.c_str());
				wxFileName::SplitPath(fullPath, &path, &currentFileName, &extension);
				currentFileName += wxT(".") + extension;
				wxString fileName = mvceditor::IcuToWx(search);
				fileName = fileName.Lower();
				if (wxNOT_FOUND != currentFileName.Lower().Find(fileName)) {
					if (0 == lineNumber || GetLineCountFromFile(fullPath) >= lineNumber) {
						TagClass newTag;
						newTag.FileTagId = fileTagId;
						newTag.Identifier = mvceditor::WxToIcu(currentFileName);
						newTag.SetFullPath(fullPath);
						newTag.FileIsNew = isNew > 0;
						matches.push_back(newTag);
					}
				}
			} while (stmt.fetch());
		}
	} catch (std::exception& e) {
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return matches;
}

std::vector<mvceditor::TagClass> mvceditor::ParsedTagFinderClass::ExactFiles(const UnicodeString& search, int lineNumber) {
	wxString path,
		currentFileName,
		extension;
	std::vector<mvceditor::TagClass> matches;
	std::string query = mvceditor::IcuToChar(search);
	std::string escaped = mvceditor::SqliteSqlEscape(query, '^');
	query = "'" + escaped + "'";
	std::string match;
	int fileTagId;
	int isNew;
	try {
		soci::statement stmt = (Session->prepare << 
			"SELECT full_path, file_item_id, is_new FROM file_items WHERE full_path = " + query + " ESCAPE '^'",
			soci::into(match), soci::into(fileTagId), soci::into(isNew));
		if (stmt.execute(true)) {
			do {
				wxString fullPath = mvceditor::CharToWx(match.c_str());
				wxFileName::SplitPath(fullPath, &path, &currentFileName, &extension);
				currentFileName += wxT(".") + extension;
				wxString fileName = mvceditor::IcuToWx(search);
				fileName = fileName.Lower();
				if (wxNOT_FOUND != currentFileName.Lower().Find(fileName)) {
					if (0 == lineNumber || GetLineCountFromFile(fullPath) >= lineNumber) {
						TagClass newTag;
						newTag.FileTagId = fileTagId;
						newTag.Identifier = mvceditor::WxToIcu(currentFileName);
						newTag.SetFullPath(fullPath);
						newTag.FileIsNew = isNew > 0;
						matches.push_back(newTag);
					}
				}
			} while (stmt.fetch());
		}
	} catch (std::exception& e) {
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return matches;
}

std::vector<mvceditor::TagClass> mvceditor::ParsedTagFinderClass::TraitAliases(const std::vector<UnicodeString>& classNames, const UnicodeString& methodName, 
																			   const std::vector<int>& fileTagIds) {
	std::vector<mvceditor::TagClass> matches;

	std::vector<std::string> classNamesToLookFor;
	for (std::vector<UnicodeString>::const_iterator it = classNames.begin(); it != classNames.end(); ++it) {
		classNamesToLookFor.push_back(mvceditor::IcuToChar(*it));
	}

	// TODO use the correct namespace when querying for traits
	std::vector<mvceditor::TraitTagClass> traits = UsedTraits(classNamesToLookFor, fileTagIds);
	
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
				mvceditor::TagClass res;
				res.ClassName = traits[i].TraitClassName;
				res.Identifier = alias;
				matches.push_back(res);
			}
		}
	}
	return matches;
}

std::vector<mvceditor::TraitTagClass> mvceditor::ParsedTagFinderClass::UsedTraits(const std::vector<std::string>& keyStarts, const std::vector<int>& fileTagIds) {
	std::ostringstream stream;
	stream << "SELECT key, file_item_id, class_name, namespace_name, trait_name, trait_namespace_name, aliases, instead_ofs "
		<< "FROM trait_resources WHERE key IN(";
	for (size_t i = 0; i < keyStarts.size(); ++i) {
		stream << "'"
			<< keyStarts[i]
			<< "'";
		if (i < (keyStarts.size() - 1)) {
			stream << ",";
		}
	}
	stream << ")";
	if (!fileTagIds.empty()) {
		stream << "AND file_item_id IN(";
		InClause(fileTagIds, stream);
		stream << ")";
	}
	std::string key;
	int fileTagId;
	std::string className;
	std::string namespaceName;
	std::string traitClassName;
	std::string traitNamespaceName;
	std::string aliases;
	std::string insteadOfs;
	std::vector<mvceditor::TraitTagClass> matches;
	std::string sql = stream.str();
	try {		
		soci::statement stmt = (Session->prepare << sql,
			soci::into(key), soci::into(fileTagId), soci::into(className), soci::into(namespaceName), soci::into(traitClassName),
			soci::into(traitNamespaceName), soci::into(aliases), soci::into(insteadOfs)
		);
		if (stmt.execute(true)) {
			do {
				mvceditor::TraitTagClass trait;
				trait.Key = mvceditor::CharToIcu(key.c_str());
				trait.FileTagId = fileTagId;
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
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return matches;
}

std::vector<mvceditor::TagClass> mvceditor::ParsedTagFinderClass::FindByIdentifierExactAndTypes(const std::string& identifier, const std::vector<int>& types, const std::vector<int>& fileTagIds, bool doLimit) {
	std::ostringstream stream;

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	// do not get fully qualified resources
	// make sure to use the key because it is indexed
	stream << "key = '" << identifier << "' AND identifier = key AND type IN(";
	InClause(types, stream);
	stream << ")";
	if (!fileTagIds.empty()) {
		stream << " AND f.file_item_id IN(";
		InClause(fileTagIds, stream);
		stream << ")";
	}
	return ResourceStatementMatches(stream.str(), doLimit);

}

std::vector<mvceditor::TagClass> mvceditor::ParsedTagFinderClass::FindByIdentifierStartAndTypes(const std::string& identifierStart, const std::vector<int>& types, 
																								const std::vector<int>& fileTagIds, bool doLimit) {
	std::ostringstream stream;

	// do not get fully qualified resources
	// make sure to use the key because it is indexed
	std::string escaped = mvceditor::SqliteSqlEscape(identifierStart, '^');
	stream << "key LIKE '" << escaped << "%' ESCAPE '^' AND identifier = key AND type IN(";
	InClause(types, stream);
	stream << ")";
	if (!fileTagIds.empty()) {
		stream << " AND f.file_item_id IN(";
		InClause(fileTagIds, stream);
		stream << ")";
	}
	return ResourceStatementMatches(stream.str(), doLimit);
}


std::vector<mvceditor::TagClass> mvceditor::ParsedTagFinderClass::AllTagsInFile(const wxString& fullPath) {
	std::vector<mvceditor::TagClass> tags;
	mvceditor::FileTagClass fileTag;
	if (FindFileTagByFullPathExact(fullPath, fileTag)) {
		std::ostringstream stream;

		// remove the duplicates from fully qualified namespaces
		// fully qualified classes / functions will start with backslash; but we want the
		// tags that don't begin with backslash
		stream << "r.file_item_id = " << fileTag.FileId << " AND Type IN(" << mvceditor::TagClass::CLASS
			<< "," << mvceditor::TagClass::DEFINE << "," << mvceditor::TagClass::FUNCTION
			<< ") AND key NOT LIKE '\\%' ESCAPE '^' ";
		tags = ResourceStatementMatches(stream.str(), true);
	}
	return tags;
}

void mvceditor::TagFinderClass::Print() {
	std::vector<mvceditor::TagClass> tags = All();
	std::vector<mvceditor::TagClass>::iterator tag;
	for (tag = tags.begin(); tag != tags.end(); ++tag) {
		printf("key=%s\n", mvceditor::IcuToChar(tag->Key).c_str());
		printf("type=%d fileItemId=%d\n", tag->Type, tag->FileTagId);
		printf("identifier=%s\n", mvceditor::IcuToChar(tag->Identifier).c_str());
		printf("\n\n");
	}
}

mvceditor::DetectedTagFinderClass::DetectedTagFinderClass()
	: TagFinderClass() {

}

std::vector<mvceditor::TagClass> mvceditor::DetectedTagFinderClass::ResourceStatementMatches(std::string whereCond, bool doLimit) {
	std::string sql;
	sql += "SELECT key, type, class_name, method_name, return_type, namespace_name, comment ";
	sql += "FROM detected_tags WHERE ";
	sql += whereCond;
	sql += " ORDER BY key";
	if (doLimit) {
		sql += " LIMIT 100";
	}

	std::vector<mvceditor::TagClass> matches;
	if (!IsCacheInitialized) {
		return matches;
	}
	std::string key;
	int type;
	std::string className;
	std::string identifier;
	std::string returnType;
	std::string namespaceName;
	std::string comment;
	int isDynamic = true;
	try {
		soci::statement stmt = (Session->prepare << sql,
			soci::into(key), soci::into(type), soci::into(className), 
			soci::into(identifier), soci::into(returnType), soci::into(namespaceName), 
			soci::into(comment)
		);
		if (stmt.execute(true)) {
			do {
				mvceditor::TagClass tag;
				tag.Key = mvceditor::CharToIcu(key.c_str());
				tag.Type = (mvceditor::TagClass::Types)type;
				tag.ClassName = mvceditor::CharToIcu(className.c_str());
				tag.Identifier = mvceditor::CharToIcu(identifier.c_str());
				tag.ReturnType = mvceditor::CharToIcu(returnType.c_str());
				tag.NamespaceName = mvceditor::CharToIcu(namespaceName.c_str());		
				tag.Comment = mvceditor::CharToIcu(comment.c_str());
				tag.IsDynamic = isDynamic != 0;

				matches.push_back(tag);
			} while (stmt.fetch());
		}
	} catch (std::exception& e) {
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	return matches;
}

std::vector<mvceditor::TagClass> mvceditor::DetectedTagFinderClass::NearMatchFiles(const UnicodeString& search, int lineNumber, const std::vector<int>& fileTagIds) {
	
	// detector db does not have  a file_items table
	std::vector<mvceditor::TagClass> matches;
	return matches;
}

std::vector<mvceditor::TagClass> mvceditor::DetectedTagFinderClass::ExactFiles(const UnicodeString& search, int lineNumber) {
	
	// detector db does not have  a file_items table
	std::vector<mvceditor::TagClass> matches;
	return matches;
}

std::vector<mvceditor::TagClass> mvceditor::DetectedTagFinderClass::TraitAliases(const std::vector<UnicodeString>& classNames, const UnicodeString& methodName, const std::vector<int>& fileTagIds) {
	
	// detector db does not have  a trait_resources table
	std::vector<mvceditor::TagClass> matches;
	return matches;
}

std::vector<mvceditor::TraitTagClass> mvceditor::DetectedTagFinderClass::UsedTraits(const std::vector<std::string>& keyStarts, const std::vector<int>& fileTagIds) {
	
	// detector db does not have  a trait_resources table
	std::vector<mvceditor::TraitTagClass> matches;
	return matches;
}


std::vector<mvceditor::TagClass> mvceditor::DetectedTagFinderClass::AllTagsInFile(const wxString& fullPath) {

	// detector db does not have a file_items table
	std::vector<mvceditor::TagClass> tags;
	return tags;
}


std::vector<mvceditor::TagClass> mvceditor::DetectedTagFinderClass::FindByIdentifierExactAndTypes(const std::string& identifier, const std::vector<int>& types,
																								  const std::vector<int>& fileTagIds, bool doLimit) {
	std::ostringstream stream;

	// case sensitive issues are taken care of by SQLite collation capabilities (so that pdo = PDO)
	// do not get fully qualified resources
	// make sure to use the key because it is indexed
	stream << "key = '" << identifier << "' AND method_name = key AND type IN(";
	InClause(types, stream);
	stream << ")";
	if (!fileTagIds.empty()) {
		stream << " AND f.file_item_id IN(";
		InClause(fileTagIds, stream);
		stream << ")";
	}
	return ResourceStatementMatches(stream.str(), doLimit);
}

std::vector<mvceditor::TagClass> mvceditor::DetectedTagFinderClass::FindByIdentifierStartAndTypes(const std::string& identifierStart, const std::vector<int>& types,
																								  const std::vector<int>& fileTagIds, bool doLimit) {
	std::ostringstream stream;

	// do not get fully qualified resources
	// make sure to use the key because it is indexed
	std::string escaped = mvceditor::SqliteSqlEscape(identifierStart, '^');
	stream << "key LIKE '" << escaped << "%' ESCAPE '^' AND method_name = key AND type IN(";
	InClause(types, stream);
	stream << ")";
	return ResourceStatementMatches(stream.str(), doLimit);
}
