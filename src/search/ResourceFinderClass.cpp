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
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <sqlite3.h>

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
	, FileParsingCache()
	, NamespaceCache()
	, TraitCache()
	, Parser()
	, Session()
	, Transaction(NULL)
	, FileName()
	, ClassName()
	, MethodName()
	, ResourceType(FILE_NAME)
	, LineNumber(0) 
	, IsCacheInitialized(false) {
	Parser.SetClassObserver(this);
	Parser.SetClassMemberObserver(this);
	Parser.SetFunctionObserver(this);
}

mvceditor::ResourceFinderClass::~ResourceFinderClass() {
	if (Transaction) {
		delete Transaction;
	}
}

void mvceditor::ResourceFinderClass::SetVersion(pelet::Versions version) {
	Parser.SetVersion(version);
}

void mvceditor::ResourceFinderClass::InitMemory() {
	OpenAndCreateTables(":memory:");
}

void mvceditor::ResourceFinderClass::InitFile(const wxFileName& fileName) {
	std::string str = mvceditor::StringHelperClass::wxToChar(fileName.GetFullPath());
	OpenAndCreateTables(str);
}

void mvceditor::ResourceFinderClass::OpenAndCreateTables(std::string db) {
	try {

		// close any existing connection
		if (IsCacheInitialized) {
			Session.close();
			IsCacheInitialized = false;
		}
		Session.open(*soci::factory_sqlite3(), db);
		std::string sql;
		sql += "CREATE TABLE IF NOT EXISTS file_items (",
		sql += "  file_item_id INTEGER PRIMARY KEY, full_path TEXT, last_modified DATETIME, is_parsed INTEGER, is_new INTEGER ";
		sql += ");";
		Session.once << sql;
		sql = "";
		sql += "CREATE TABLE IF NOT EXISTS resources (";
		sql += "  file_item_id INTEGER, key TEXT, identifier TEXT, class_name TEXT, ";
		sql += "  type INTEGER, namespace_name TEXT, signature TEXT, comment TEXT, ";
		sql += "  return_type TEXT, is_protected INTEGER, is_private INTEGER, ";
		sql += "  is_static INTEGER, is_dynamic INTEGER, is_native INTEGER";
		sql += ");";
		Session.once << sql;
		IsCacheInitialized = true;
	} catch(std::exception const& e) {
		Session.close();
		IsCacheInitialized = false;
		wxString msg = mvceditor::StringHelperClass::charToWx(e.what());
		wxASSERT_MSG(IsCacheInitialized, msg);
	}
}

void mvceditor::ResourceFinderClass::BeginSearch() {
	if (IsCacheInitialized) {

		// rollback any previous transaction, as commits should be explicit
		if (Transaction) {
			delete Transaction;
		}
		Transaction = new soci::transaction(Session);
	}
	FileParsingCache.clear();
	NamespaceCache.clear();
}

void mvceditor::ResourceFinderClass::EndSearch() {
	if (Transaction) {

		// commit until the very end; profiling showed that sqlite3 was
		// being flushed to disk after every file and that was 
		// resulting in very slow walks.
		Transaction->commit();
		delete Transaction;
		Transaction = NULL;
	}
	FileParsingCache.clear();
	NamespaceCache.clear();
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
	BeginSearch();

	// remove all previous cached resources
	mvceditor::FileItemClass fileItem;
	bool foundFile = FindInFileCache(fullPath, fileItem);
	if (foundFile) {
		std::vector<int> fileItemIdsToRemove;
		fileItemIdsToRemove.push_back(fileItem.FileId);
		RemoveCachedResources(fileItemIdsToRemove);
	}
	if (!foundFile) {

		// if caller just calls this method without calling Walk(); then file cache will be empty
		// need to add an entry so that GetResourceMatchFullPathFromResource works correctly
		fileItem.FullPath = fullPath;
		fileItem.IsNew = isNew;
		fileItem.DateTime = wxDateTime::Now();
		fileItem.IsParsed = false;
		PushIntoFileCache(fileItem);
	}
	
	FileParsingCache.clear();

	// for now silently ignore parse errors
	pelet::LintResultsClass results;
	Parser.ScanString(code, results);

	PushIntoResourceCache(FileParsingCache, fileItem.FileId);
	EndSearch();
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
	BeginSearch();
	wxFileName fileName = mvceditor::NativeFunctionsAsset();	
	if (fileName.FileExists()) {
		loaded = LoadTagFile(fileName, true);
	}
	EndSearch();
	return loaded;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::CollectNearMatchResources() {
	std::vector<mvceditor::ResourceClass> matches;
	switch (ResourceType) {
		case FILE_NAME:
		case FILE_NAME_LINE_NUMBER:
			matches = CollectNearMatchFiles();
			break;
		case CLASS_NAME:
			matches = CollectNearMatchNonMembers();
			break;
		case CLASS_NAME_METHOD_NAME:
			matches = CollectNearMatchMembers();
			break;
		case NAMESPACE_NAME:
			matches = CollectNearMatchNamespaces();
			break;
	}
	EnsureMatchesExist(matches);
	sort(matches.begin(), matches.end());
	return matches;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::CollectNearMatchFiles() {
	wxString path,
		currentFileName,
		extension;
	std::vector<mvceditor::ResourceClass> matches;
	wxString fileName =  StringHelperClass::IcuToWx(FileName).Lower();
	std::string query = mvceditor::StringHelperClass::wxToChar(fileName);

	// add the SQL wildcards
	query = "'%" + query + "%'";
	std::string match;
	int fileItemId;
	int isNew;
	soci::statement stmt = (Session.prepare << "SELECT full_path, file_item_id, is_new FROM file_items WHERE full_path LIKE " + query,
		soci::into(match), soci::into(fileItemId), soci::into(isNew));
	if (stmt.execute(true)) {
		do {
			wxString fullPath = mvceditor::StringHelperClass::charToWx(match.c_str());
			wxFileName::SplitPath(fullPath, &path, &currentFileName, &extension);
			currentFileName += wxT(".") + extension;
			if (wxNOT_FOUND != currentFileName.Lower().Find(fileName)) {
				if (0 == LineNumber || GetLineCountFromFile(fullPath) >= LineNumber) {
					ResourceClass newItem;
					newItem.FileItemId = fileItemId;
					newItem.FullPath.Assign(fullPath);
					newItem.FileIsNew = isNew > 0;
					matches.push_back(newItem);
				}
			}
		} while (stmt.fetch());
	}
	return matches;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::CollectNearMatchNonMembers() {
	UnicodeString key = ClassName;
	key.findAndReplace(UNICODE_STRING_SIMPLE("'"),UNICODE_STRING_SIMPLE(""));
	std::string query = mvceditor::StringHelperClass::IcuToChar(key);
	std::ostringstream stream;
	stream << " key = '" << query << "' AND type IN(" 
		<< mvceditor::ResourceClass::CLASS << ","
		<< mvceditor::ResourceClass::DEFINE << ","
		<< mvceditor::ResourceClass::FUNCTION << ")";
	std::vector<mvceditor::ResourceClass> matches = ResourceStatementMatches(stream.str(), true);
	if (matches.empty()) {
	
		// if nothing matches exactly then execure the LIKE query
		std::ostringstream streamLike;
		streamLike << " key LIKE '" << query << "%' AND type IN(" 
		<< mvceditor::ResourceClass::CLASS << ","
		<< mvceditor::ResourceClass::DEFINE << ","
		<< mvceditor::ResourceClass::FUNCTION << ")";
		matches = ResourceStatementMatches(streamLike.str(), true);
	}
	return matches;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::CollectNearMatchMembers() {
	std::vector<mvceditor::ResourceClass> matches;
	
	// look for the parent classes. when we are searching for members, we want to look
	// for members from the class or any class it inherited from
	// whenever we insert into parentClassNames, need to make sure strings are all lowercase
	// that way we dont have do lower case for every resource comparison
	std::vector<UnicodeString> parentClassNames = ClassHierarchy(ClassName);	
	if (MethodName.isEmpty()) {
		
		// special case; query for all methods for a class (UserClass::)
		std::vector<mvceditor::ResourceClass> memberMatches = CollectAllMembers(parentClassNames);
		std::vector<mvceditor::ResourceClass> traitMatches = CollectAllTraitMembers(ClassName);

		matches.insert(matches.end(), memberMatches.begin(), memberMatches.end());
		matches.insert(matches.end(), traitMatches.begin(), traitMatches.end());
	}
	else if (ClassName.isEmpty()) {
		
		// special case, query accross all classes for a method (::getName)
		// if ClassName is empty, then just check method names This ensures 
		// queries like '::getName' will work as well.
		// make sure to NOT get fully qualified  matches (key=identifier)
		UnicodeString key = MethodName;
		std::string query = mvceditor::StringHelperClass::IcuToChar(key);
		std::ostringstream stream;
		stream << " identifier LIKE '" << query << "' AND key = identifier AND type IN ("
				<< mvceditor::ResourceClass::MEMBER << ","
				<< mvceditor::ResourceClass::METHOD << ","
				<< mvceditor::ResourceClass::CLASS_CONSTANT << ")";
		matches = ResourceStatementMatches(stream.str(), true);
		if (matches.empty()) {
		
			// use LIKE to get near matches
			std::ostringstream streamLike;
			streamLike << "identifier LIKE '" << query << "%' AND key = identifier AND type IN ("
				<< mvceditor::ResourceClass::MEMBER << ","
				<< mvceditor::ResourceClass::METHOD << ","
				<< mvceditor::ResourceClass::CLASS_CONSTANT << ")";
			matches = ResourceStatementMatches(streamLike.str(), true);
		}
	}
	else {
		InheritedTraits(ClassName, parentClassNames);

		UnicodeString key = MethodName;
		std::string query = mvceditor::StringHelperClass::IcuToChar(key);
		std::ostringstream streamLike;

		// make sure to NOT get the fully qualified resources
		streamLike << " key = identifier AND identifier LIKE '" << query << "%' AND class_name IN(";
		
		// we use an IN() query and query for classes and traits at the same time
		for (std::vector<UnicodeString>::iterator it = parentClassNames.begin(); it != parentClassNames.end(); ++it) {
			streamLike << "'" << mvceditor::StringHelperClass::IcuToChar(*it) << "'";
			if (it != (parentClassNames.end() - 1)) {
				streamLike << ",";
			}
		}
		streamLike << ") AND type IN ("
				<< mvceditor::ResourceClass::MEMBER << ","
				<< mvceditor::ResourceClass::METHOD << ","
				<< mvceditor::ResourceClass::CLASS_CONSTANT << ")";
		matches = ResourceStatementMatches(streamLike.str(), true);
	}
	return matches;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::CollectAllMembers(const std::vector<UnicodeString>& classNames) {
	std::vector<mvceditor::ResourceClass> matches;
	std::string query; 
	for (size_t i = 0; i < classNames.size(); ++i) {
		query += "'";
		query += mvceditor::StringHelperClass::IcuToChar(classNames[i]);
		query += "'";
		if (i < (classNames.size() - 1)) {
			query += ",";
		}
	}

	// make sure to NOT get fully qualified matches since they are dups.  
	std::ostringstream stream;
	stream << " class_name IN (" << query << ") AND type IN(" << mvceditor::ResourceClass::CLASS_CONSTANT << ","
		<< mvceditor::ResourceClass::MEMBER << "," << mvceditor::ResourceClass::METHOD << ") AND key = Identifier";
	matches = ResourceStatementMatches(stream.str(), true);
	return matches;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::CollectAllTraitMembers(const UnicodeString& className) {
	std::vector<mvceditor::ResourceClass> matches;
	std::vector<mvceditor::TraitResourceClass> traits = TraitCache[className];
	for (size_t j = 0; j < traits.size(); ++j) {
		
		// lets unqualify, the members cache key does not have the namespace name
		int32_t index = traits[j].TraitClassName.lastIndexOf(UNICODE_STRING_SIMPLE("\\"));
		UnicodeString traitClassNameOnly(traits[j].TraitClassName, index + 1);
		UnicodeString key =  traitClassNameOnly + UNICODE_STRING_SIMPLE("::");
		
		std::string query = mvceditor::StringHelperClass::IcuToChar(key);
		std::ostringstream stream;
		stream << " key LIKE '" << query << "%'";
		std::vector<mvceditor::ResourceClass> traitMatches = ResourceStatementMatches(stream.str(), true);
		matches.insert(matches.end(), traitMatches.begin(), traitMatches.end());
		
		// TODO weed out Matches from methods from the wrong namespace
	}
	
	// now go through the result and change the method names of any aliased 
	for (size_t i = 0; i < traits.size(); i++) {
		std::vector<UnicodeString> aliases = traits[i].Aliased;
		for (size_t a = 0; a < aliases.size(); a++) {
			mvceditor::ResourceClass res;
			res.ClassName = traits[i].TraitClassName;
			res.Identifier = aliases[a];
			matches.push_back(res);
		}
	}
	return matches;
}

std::vector<mvceditor::ResourceClass>  mvceditor::ResourceFinderClass::CollectNearMatchNamespaces() {
	std::vector<mvceditor::ResourceClass> matches;

	// needle identifier contains a namespace operator; but it may be
	// a namespace or a fully qualified name
	UnicodeString key = ClassName;
	std::string query = mvceditor::StringHelperClass::IcuToChar(key);
	std::ostringstream stream;
	stream << " key LIKE '" << query << "%'";
	matches = ResourceStatementMatches(stream.str(), true);

	std::vector<mvceditor::ResourceClass>::iterator end = matches.end();
	
	// if there are exact matches; we only want to collect exact matches
	if (!matches.empty() && matches[0].IsKeyEqualTo(key)) {
		KeyPredicateClass pred(key, true);
		end = std::remove_if(matches.begin(), end, pred);
	}
	
	// need to actually erase them,, std::remove_if does not
	matches.erase(end, matches.end());
	return matches;
}

std::vector<UnicodeString> mvceditor::ResourceFinderClass::ClassHierarchy(const UnicodeString& className) {
	std::vector<UnicodeString> parentClassNames;
	parentClassNames.push_back(className);
	UnicodeString lastClassName(className);
	bool done = false;
	while (!done) {
		done = true;

		std::string query = mvceditor::StringHelperClass::IcuToChar(lastClassName);
		std::ostringstream stream;
		stream << " key = '" << query << "' AND TYPE = " << mvceditor::ResourceClass::CLASS;
		std::vector<mvceditor::ResourceClass> matches = ResourceStatementMatches(stream.str(), true);
		for (std::vector<mvceditor::ResourceClass>::const_iterator it = matches.begin(); it != matches.end(); ++it) {
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
		const UnicodeString& methodName) {
	UnicodeString parentClassName;

	// first query to get the parent class name
	std::string query = mvceditor::StringHelperClass::IcuToChar(className);
	std::ostringstream stream;
	stream << " key = '" << query << "' AND type = " << mvceditor::ResourceClass::CLASS;
	std::vector<mvceditor::ResourceClass> matches = ResourceStatementMatches(stream.str(), false);
	if (!matches.empty()) {
		mvceditor::ResourceClass resource = matches[0];
		parentClassName = ExtractParentClassFromSignature(resource.Signature);
	}
	if (!parentClassName.isEmpty() && !methodName.isEmpty()) {

		// now to check that the parent has the given method
		std::string query = mvceditor::StringHelperClass::IcuToChar(parentClassName);
		query += "::";
		query += mvceditor::StringHelperClass::IcuToChar(methodName);
		std::ostringstream stream;
		stream << " key LIKE '" << query << "%' AND type = " << mvceditor::ResourceClass::METHOD;
		matches = ResourceStatementMatches(stream.str(), true);
		UnicodeString parentClassSignature;
		bool found = false;
		if (!matches.empty()) {

			// this ancestor has the method
			found = true;
		}
		if (!found) {

			//  keep searching up the inheritance chain
			return GetResourceParentClassName(parentClassName, methodName);
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
	bool cached = false;
	mvceditor::FileItemClass fileItem;
	bool foundFile = FindInFileCache(fullPath, fileItem);
	if (foundFile) {
		bool needsToBeParsed = fileItem.NeedsToBeParsed(fileLastModifiedDateTime);
		cached = !needsToBeParsed;
	}
	else {
		fileItem.MakeNew(fileName, parseClasses);
		PushIntoFileCache(fileItem);
	}
	if (parseClasses) {
		if (!cached || !fileItem.IsParsed) {

			// no need to look for resources if the file had not yet existed, this will save much time
			// this optimization was found by using the profiler
			if (foundFile) {
				std::vector<int> fileItemIdsToRemove;
				fileItemIdsToRemove.push_back(fileItem.FileId);
				RemoveCachedResources(fileItemIdsToRemove);
			}			
			FileParsingCache.clear();
			
			// for now silently ignore files with parser errors
			pelet::LintResultsClass lintResults;
			wxFFile file(fullPath, wxT("rb"));
			Parser.ScanFile(file.fp(), mvceditor::StringHelperClass::wxToIcu(fullPath), lintResults);
			PushIntoResourceCache(FileParsingCache, fileItem.FileId);
			FileParsingCache.clear();
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
	classItem.Signature = signature;
	classItem.ReturnType = UNICODE_STRING_SIMPLE("");
	classItem.Comment = comment;
	classItem.IsNative = false;
	FileParsingCache.push_back(classItem);

	if (IsNewNamespace(namespaceName)) {

		// a resource for the namespace itself
		mvceditor::ResourceClass namespaceItem = mvceditor::ResourceClass::MakeNamespace(namespaceName);
		FileParsingCache.push_back(namespaceItem);
	}

	classItem.Identifier = QualifyName(namespaceName, className);
	classItem.Key = QualifyName(namespaceName, className);
	FileParsingCache.push_back(classItem);

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

void mvceditor::ResourceFinderClass::DefineDeclarationFound(const UnicodeString& namespaceName, const UnicodeString& variableName, 
		const UnicodeString& variableValue, const UnicodeString& comment, const int lineNumber) {
	ResourceClass defineItem;
	defineItem.Identifier = variableName;
	defineItem.Key = variableName;
	defineItem.Type = ResourceClass::DEFINE;
	defineItem.Signature = variableValue;
	defineItem.ReturnType = UNICODE_STRING_SIMPLE("");
	defineItem.Comment = comment;
	defineItem.IsNative = false;
	FileParsingCache.push_back(defineItem);

	defineItem.Identifier = QualifyName(namespaceName, variableName);
	defineItem.Key = QualifyName(namespaceName, variableName);
	FileParsingCache.push_back(defineItem);
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
	FileParsingCache.push_back(item);
	
	// insert a complete name so that we can quickly lookup all methods for a single class
	item.Key = className + UNICODE_STRING_SIMPLE("::") + methodName;
	FileParsingCache.push_back(item);
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
	FileParsingCache.push_back(item);
	
	// insert a complete name so that we can quickly lookup all methods for a single class
	item.Key = className + UNICODE_STRING_SIMPLE("::") + filteredProperty;
	FileParsingCache.push_back(item);
}

void mvceditor::ResourceFinderClass::FunctionFound(const UnicodeString& namespaceName, const UnicodeString& functionName, const UnicodeString& signature, 
		const UnicodeString& returnType, const UnicodeString& comment, const int lineNumber) {
	ResourceClass item;
	item.Identifier = functionName;
	item.NamespaceName = namespaceName;
	item.Key = functionName;
	item.Type = ResourceClass::FUNCTION;
	item.Signature = signature;
	item.ReturnType = returnType;
	item.Comment = comment;
	item.IsNative = false;
	FileParsingCache.push_back(item);
		
	if (IsNewNamespace(namespaceName)) {
		mvceditor::ResourceClass namespaceItem = mvceditor::ResourceClass::MakeNamespace(namespaceName);
		FileParsingCache.push_back(namespaceItem);
	}
		
	// put in the namespace cache so that qualified name lookups work too
	item.Identifier = QualifyName(namespaceName, functionName);
	item.Key = QualifyName(namespaceName, functionName);
	FileParsingCache.push_back(item);
}

bool mvceditor::ResourceFinderClass::IsNewNamespace(const UnicodeString& namespaceName) {
	std::string sql = "SELECT COUNT(*) FROM resources WHERE key = ? AND type = ?";
	std::string nm = mvceditor::StringHelperClass::IcuToChar(namespaceName);
	int type = mvceditor::ResourceClass::NAMESPACE;
	int count = 0;
	soci::statement stmt = (Session.prepare << sql, soci::use(nm), soci::use(type), soci::into(count));
	stmt.execute(true);
	bool isNew = false;
	if (count <= 0) {
	 
		// look in the current namespace cache, stuff that has not yet been added to the database
		if (NamespaceCache.count(namespaceName) <= 0) {
			isNew = true;
			NamespaceCache[namespaceName] = 1;
		}	
	}
	return isNew;
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

void mvceditor::ResourceFinderClass::RemoveCachedResources(const std::vector<int>& fileItemIds) {
	if (!IsCacheInitialized || fileItemIds.empty()) {
		return;
	}
	std::ostringstream stream;
	stream << "WHERE file_item_id IN (";
	for (size_t i = 0; i < fileItemIds.size(); ++i) {
		stream << fileItemIds[i];
		if (i < (fileItemIds.size() - 1)) {
			stream << ",";
		}
	}
	stream << ")";
	Session.once << "DELETE FROM resources " << stream.str();
	Session.once << "DELETE FROM file_items " << stream.str();
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

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::CollectFullyQualifiedResource() {
	ResourceClass needle;
	std::vector<mvceditor::ResourceClass> allMatches;
	if (ResourceType == CLASS_NAME_METHOD_NAME) {

		// check the entire class hierachy; stop as soon as we found it
		std::vector<UnicodeString> classHierarchy = ClassHierarchy(ClassName);
		for (size_t i = 0; i < classHierarchy.size(); ++i) {
			UnicodeString key = classHierarchy[i] + UNICODE_STRING_SIMPLE("::") + MethodName;
			std::string query = mvceditor::StringHelperClass::IcuToChar(key);
			std::ostringstream stream;
			stream << " key = '" << query << "' AND type IN(" << mvceditor::ResourceClass::MEMBER << "," << mvceditor::ResourceClass::METHOD << ")";
			std::vector<mvceditor::ResourceClass> matches = ResourceStatementMatches(stream.str(), true);
			if (!matches.empty()) {
				allMatches.push_back(matches[0]);
			}
		}
	}
	else if (NAMESPACE_NAME == ResourceType) {
		UnicodeString key = ClassName;
		std::string query = mvceditor::StringHelperClass::IcuToChar(key);
		std::ostringstream stream;
		stream << " key = '" << query << "'";

		// make sure there is one and only one item that matches the search.
		std::vector<mvceditor::ResourceClass> matches = ResourceStatementMatches(stream.str(), true);
		if (matches.size() == 1) {
			allMatches.push_back(matches[0]);
		}
	}
	else {
		UnicodeString key = ClassName;
		std::string query = mvceditor::StringHelperClass::IcuToChar(key);
		std::ostringstream stream;
		stream << " key = '" << query << "'";

		// make sure there is one and only one item that matches the search.
		std::vector<mvceditor::ResourceClass> matches = ResourceStatementMatches(stream.str(), true);
		if (matches.size() == 1) {
			allMatches.push_back(matches[0]);
		}
	}
	EnsureMatchesExist(allMatches);
	return allMatches;
}

void mvceditor::ResourceFinderClass::InheritedTraits(const UnicodeString& fullyQualifiedClassName, std::vector<UnicodeString>& inheritedTraits) {
	bool match = false;
	std::vector<mvceditor::TraitResourceClass> traits = TraitCache[fullyQualifiedClassName];
	for (size_t i = 0; i < traits.size(); ++i) {
		UnicodeString traitClassName = traits[i].TraitClassName;
			
		// trait is used unless there is an explicit insteadof 
		match = true;
		for (size_t j = 0; j < traits[i].Excluded.size(); ++j) {
			if (traits[i].Excluded[j].caseCompare(fullyQualifiedClassName, 0) == 0) {
				match = false;
				break;
			}
		}
		if (match) {
			inheritedTraits.push_back(traitClassName);
		}
	}
}

void mvceditor::ResourceFinderClass::EnsureMatchesExist(std::vector<ResourceClass>& matches) {

	// remove from matches that have a file that is no longer in the file system
	std::vector<mvceditor::ResourceClass>::iterator it = matches.begin();
	std::vector<int> fileItemIdsToRemove;
	while (it != matches.end()) {

		// native matches wont have a FileItem assigned to them since they come from the tag file
		// FileItemId is meaningless for dynamic resources
		// is a file is new it wont be on disk; results are never stale in this case.
		if (it->IsNative || it->IsDynamic || it->FileIsNew || !it->FullPath.IsOk() || it->FullPath.FileExists()) {
			++it;
		}
		else {
			fileItemIdsToRemove.push_back(it->FileItemId);
			it = matches.erase(it);
		}
	}
	if (!fileItemIdsToRemove.empty()) {
		
		// remove all cached resources from files that no longer exist
		std::vector<int>::iterator it = std::unique(fileItemIdsToRemove.begin(), fileItemIdsToRemove.end());
		fileItemIdsToRemove.resize(it - fileItemIdsToRemove.begin());
		RemoveCachedResources(fileItemIdsToRemove);
	}
}

void mvceditor::ResourceFinderClass::Print() {
	UFILE *out = u_finit(stdout, NULL, NULL);
	u_fprintf(out, "LookingFor=%.*S,%.*S\n", ClassName.length(), ClassName.getBuffer(), MethodName.length(), MethodName.getBuffer());
	std::vector<mvceditor::ResourceClass> matches = ResourceStatementMatches("1=1", false);
	for (std::vector<mvceditor::ResourceClass>::const_iterator it = matches.begin(); it != matches.end(); ++it) {
		switch (it->Type) {
			case mvceditor::ResourceClass::CLASS :
			case mvceditor::ResourceClass::DEFINE :
			case mvceditor::ResourceClass::FUNCTION :

				u_fprintf(out, "RESOURCE: Key=%.*S Identifier=%.*S ClassName=%.*S Namespace=%.*S Type=%d\n",
					it->Key.length(), it->Key.getBuffer(),
					it->Identifier.length(), it->Identifier.getBuffer(),  
					it->ClassName.length(), it->ClassName.getBuffer(),
					it->NamespaceName.length(), it->NamespaceName.getBuffer(),
					it->Type);
				break;
			case mvceditor::ResourceClass::CLASS_CONSTANT :
			case mvceditor::ResourceClass::MEMBER :
			case mvceditor::ResourceClass::METHOD :
				u_fprintf(out, "MEMBER: Key=%.*S Identifier=%.*S ClassName=%.*S Namespace=%.*S ReturnType=%.*S Type=%d\n", 
					it->Key.length(), it->Key.getBuffer(),
					it->Identifier.length(), it->Identifier.getBuffer(),  
					it->ClassName.length(), it->ClassName.getBuffer(),
					it->NamespaceName.length(), it->NamespaceName.getBuffer(),
					it->ReturnType.length(), it->ReturnType.getBuffer(), it->Type);
				break;
			case mvceditor::ResourceClass::NAMESPACE :
				u_fprintf(out, "NAMESPACE:Key=%.*S Identifier=%.*S ClassName=%.*S Namespace=%.*S  Type=%d\n", 
					it->Key.length(), it->Key.getBuffer(),
					it->Identifier.length(), it->Identifier.getBuffer(),  
					it->ClassName.length(), it->ClassName.getBuffer(),
					it->NamespaceName.length(), it->NamespaceName.getBuffer(),
					it->Type);
				break;
		}
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

bool mvceditor::ResourceFinderClass::IsFileCacheEmpty() {
	if (!IsCacheInitialized) {
		return true;
	}
	int count = 0;
	Session.once << "SELECT COUNT(*) FROM file_items;", soci::into(count);
	return count <= 0;
}

bool mvceditor::ResourceFinderClass::IsResourceCacheEmpty() {
	if (!IsCacheInitialized) {
		return true;
	}

	// make sure only parsed resource came from the native functions file.
	int count = 0;
	Session.once << "SELECT COUNT(*) FROM resources WHERE is_native = 0;", soci::into(count);
	return count <= 0;
}

void mvceditor::ResourceFinderClass::AddDynamicResources(const std::vector<mvceditor::ResourceClass>& dynamicResources) {
	BeginSearch();
	std::vector<mvceditor::ResourceClass> newResources;
	
	for (std::vector<mvceditor::ResourceClass>::const_iterator it = dynamicResources.begin(); it != dynamicResources.end(); ++it) {
		mvceditor::ResourceClass resource = *it;
		if (mvceditor::ResourceClass::MEMBER == resource.Type || mvceditor::ResourceClass::METHOD == resource.Type) {

			// need to account for duplicates; if so then only update
			bool updated = false;
			std::ostringstream stream;
			stream << "SELECT return_type, class_name, identifier FROM resources WHERE class_name = ? AND identifier = ? AND type IN("
				<< mvceditor::ResourceClass::MEMBER
				<< ","
				<< mvceditor::ResourceClass::METHOD
				<< ")";

			std::string returnType, 
				classNameOut, 
				identifierOut;
			std::string className = mvceditor::StringHelperClass::IcuToChar(resource.ClassName);
			std::string identifier = mvceditor::StringHelperClass::IcuToChar(resource.Identifier);
			Session.once << stream.str(), soci::use(className), soci::use(identifier), soci::into(returnType), soci::into(classNameOut)
				, soci::into(identifierOut);
			if (returnType.empty() && !classNameOut.empty() && !identifierOut.empty()) {

				// resource already exists, only update the return type
				returnType = mvceditor::StringHelperClass::IcuToChar(resource.ReturnType);
				std::ostringstream updateStream;
				updateStream << "UPDATE resources SET return_type = ? WHERE class_name = ? AND identifier = ? AND type IN("
					<< mvceditor::ResourceClass::MEMBER
					<< ","
					<< mvceditor::ResourceClass::METHOD
					<< ")";
				Session.once << updateStream.str(), soci::use(returnType), soci::use(className), soci::use(identifier);
				updated = true;
			}
			if (!updated) {
				resource.IsDynamic = true;
				resource.Key = resource.Identifier;
				newResources.push_back(resource);
				
				// the fully qualified version
				resource.Key = resource.ClassName + UNICODE_STRING_SIMPLE("::") + resource.Identifier;
				newResources.push_back(resource);
			}
		}
		else {

			// look at the class, function, cache
			bool updated = false;
			std::ostringstream stream;
			stream << "SELECT return_type, identifier FROM resources WHERE class_name = ? AND identifier = ? AND type IN("
				<< mvceditor::ResourceClass::FUNCTION
				<< ")";
			std::string className = mvceditor::StringHelperClass::IcuToChar(resource.ClassName);
			std::string identifier = mvceditor::StringHelperClass::IcuToChar(resource.Identifier);
			std::string returnType;
			std::string identifierOut;
			Session.once << stream.str(), soci::use(identifier), soci::into(returnType), soci::into(identifierOut);
			if (returnType.empty() && !identifier.empty()) {

				// function already exists, just update the return type
				returnType = mvceditor::StringHelperClass::IcuToChar(resource.ReturnType);
				std::ostringstream updateStream;
				updateStream << "UPDATE resources SET return_type = ? WHERE class_name = ? AND identifier = ? AND type IN("
					<< mvceditor::ResourceClass::FUNCTION
					<< ")";				
				Session.once << updateStream.str(), soci::use(returnType), soci::use(className), soci::use(identifier);
				updated = true;
			}
			if (!updated) {
				resource.IsDynamic = true;
				resource.Key = resource.Identifier;
				newResources.push_back(resource);
				
				// the fully qualified version
				resource.Key = QualifyName(resource.NamespaceName, resource.ClassName);
				newResources.push_back(resource);
			}
		}
	}

	// we dont want these resource to point to a file; 
	// GetResourceMatchFullPathFromResource() and GetResourceMatchFullPath() will return empty for dynamic 
	// resources since there is no source code we can show the user.
	int fileItemIndex = -1;
	PushIntoResourceCache(newResources, fileItemIndex);
	EndSearch();
}

bool mvceditor::ResourceFinderClass::LoadTagFile(const wxFileName& fileName, bool isNativeTags) {
	bool good = false;
	UFILE* uf = u_fopen(fileName.GetFullPath().ToAscii(), "rb", NULL, NULL);
	if (uf) {
		UChar buffer[512];

		std::vector<mvceditor::ResourceClass> newResources;
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
						newResources.push_back(res);
						switch (res.Type) {
						case mvceditor::ResourceClass::CLASS:
						case mvceditor::ResourceClass::FUNCTION:
						case mvceditor::ResourceClass::NAMESPACE:
							
							// the fully qualified version
							res.Identifier =  QualifyName(res.NamespaceName, res.Identifier);
							res.Key = res.Identifier;
							newResources.push_back(res);
							break;
						case mvceditor::ResourceClass::MEMBER:
						case mvceditor::ResourceClass::METHOD:
						case mvceditor::ResourceClass::CLASS_CONSTANT:

							// the fully scoped version
							res.Key = res.ClassName + UNICODE_STRING_SIMPLE("::") + res.Identifier;;
							newResources.push_back(res);
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
		PushIntoResourceCache(newResources, -1);
	}
	return good;
}

void mvceditor::ResourceFinderClass::PushIntoFileCache(mvceditor::FileItemClass& fileItem) {
	if (!IsCacheInitialized) {
		return;
	}
	std::string fullPath = mvceditor::StringHelperClass::wxToChar(fileItem.FullPath);
	std::tm tm;
	int isParsed = fileItem.IsParsed ? 1 : 0;
	int isNew = fileItem.IsNew ? 1 : 0;
	if (fileItem.DateTime.IsValid()) {
		wxDateTime::Tm wxTm = fileItem.DateTime.GetTm();
		tm.tm_hour = wxTm.hour;
		tm.tm_isdst = fileItem.DateTime.IsDST();
		tm.tm_mday = wxTm.mday;
		tm.tm_min = wxTm.min;
		tm.tm_mon = wxTm.mon;
		tm.tm_sec = wxTm.sec;
		tm.tm_wday = fileItem.DateTime.GetWeekDay();
		tm.tm_yday = fileItem.DateTime.GetDayOfYear();

		// tm holds number of years since 1900 (2012 = 112)
		tm.tm_year = wxTm.year - 1900;
	}
	soci::statement stmt = (Session.prepare <<
		"INSERT INTO file_items (file_item_id, full_path, last_modified, is_parsed, is_new) VALUES(NULL, ?, ?, ?, ?)",
		soci::use(fullPath), soci::use(tm), soci::use(isParsed), soci::use(isNew)
	);
	stmt.execute(true);
	soci::sqlite3_statement_backend* backend = static_cast<soci::sqlite3_statement_backend*>(stmt.get_backend());
	fileItem.FileId = sqlite3_last_insert_rowid(backend->session_.conn_);
}

bool mvceditor::ResourceFinderClass::FindInFileCache(const wxString& fullPath, mvceditor::FileItemClass& fileItem) {
	if (!IsCacheInitialized) {
		return false;
	}
	int fileItemId;
	std::tm lastModified;
	int isParsed;
	int isNew;

	std::string query = mvceditor::StringHelperClass::wxToChar(fullPath);
	std::string sql = "SELECT file_item_id, last_modified, is_parsed, is_new FROM file_items WHERE full_path = ?";
	soci::statement stmt = (Session.prepare << sql, soci::use(query), 
		soci::into(fileItemId), soci::into(lastModified), soci::into(isParsed), soci::into(isNew)
	);
	bool foundFile = stmt.execute(true);
	if (foundFile) {
		fileItem.DateTime.Set(lastModified);
		fileItem.FileId = fileItemId;
		fileItem.FullPath = fullPath;
		fileItem.IsNew = isNew != 0;
		fileItem.IsParsed = isParsed != 0;
	}	
	return foundFile;
}

std::vector<mvceditor::FileItemClass> mvceditor::ResourceFinderClass::FileItems(const std::vector<int>& fileItemIds) {
	std::vector<mvceditor::FileItemClass> fileItems;
	if (!IsCacheInitialized || fileItemIds.empty()) {
		return fileItems;
	}
	std::ostringstream stream;
	for (size_t i = 0; i < fileItemIds.size(); ++i) {
		stream << fileItemIds[i];
		if (i < (fileItemIds.size() - 1)) {
			stream << ",";
		}
	}
	std::string ids = stream.str();
	int fileItemId;
	std::string fullPath;
	std::tm tm;
	int isParsed;
	int isNew;
	std::string sql = "SELECT file_item_id, full_path, last_modified, is_parsed, is_new FROM file_items WHERE file_item_id IN(" + ids + ")";
	soci::statement stmt = (Session.prepare << sql,
		soci::into(fileItemId), soci::into(fullPath), soci::into(tm), soci::into(isParsed), soci::into(isNew)
	);
	stmt.execute();
	while (stmt.fetch()) {
		mvceditor::FileItemClass fileItem;
		fileItem.DateTime.Set(tm);
		fileItem.FileId = fileItemId;
		fileItem.FullPath = mvceditor::StringHelperClass::charToWx(fullPath.c_str());
		fileItem.IsNew = isNew != 0;
		fileItem.IsParsed = isParsed != 0;
		fileItems.push_back(fileItem);
	}
	return fileItems;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::ResourceStatementMatches(std::string whereCond, bool doLimit) {
	std::string sql;
	sql += "SELECT r.file_item_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON(r.file_item_id = f.file_item_id) WHERE ";
	sql += whereCond;
	sql += " ORDER BY key";
	if (doLimit) {
		sql += " LIMIT 50";
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
	soci::statement stmt = (Session.prepare << sql,
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
			resource.Key = mvceditor::StringHelperClass::charToIcu(key.c_str());
			resource.Identifier = mvceditor::StringHelperClass::charToIcu(identifier.c_str());
			resource.ClassName = mvceditor::StringHelperClass::charToIcu(className.c_str());
			resource.Type = (mvceditor::ResourceClass::Types)type;
			resource.NamespaceName = mvceditor::StringHelperClass::charToIcu(namespaceName.c_str());
			resource.Signature = mvceditor::StringHelperClass::charToIcu(signature.c_str());
			resource.ReturnType = mvceditor::StringHelperClass::charToIcu(returnType.c_str());
			resource.Comment = mvceditor::StringHelperClass::charToIcu(comment.c_str());
			if (soci::i_ok == fullPathIndicator) {
				resource.FullPath.Assign(mvceditor::StringHelperClass::charToWx(fullPath.c_str()));
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
	return matches;
}

bool mvceditor::ResourceFinderClass::Persist(const wxFileName& outputFile) {
	if (!outputFile.IsOk()) {
		return false;
	}
	bool error = false;
	
	// TODO can we juse use the SQLite database file
	wxFFile file(outputFile.GetFullPath(), wxT("wb"));
	if (!file.IsOpened()) {
		return false;
	}
	UFILE* uf = u_finit(file.fp(), NULL, NULL);
	if (!uf) {
		return false;
	}
	int32_t written;
	
	// watch out for dynamic resources and 'native' functions
	// we dont want to persist those for now 
	std::string whereCond = "is_native = 0 AND is_dynamic = 0 AND r.file_item_id > 0";
	std::vector<mvceditor::ResourceClass> matches = ResourceStatementMatches(whereCond, false);
	std::vector<mvceditor::ResourceClass>::iterator it;
	for (it = matches.begin(); it != matches.end() && !error; ++it) {
		if (it->Key.indexOf(UNICODE_STRING_SIMPLE("::")) >= 0 ||
			it->Key.indexOf(UNICODE_STRING_SIMPLE("\\")) >= 0) {

			// this is a duplicate, fully qualified match.  skip it
			continue;
		}
		wxString fullPath = it->FullPath.GetFullPath();
		if (mvceditor::ResourceClass::CLASS == it->Type) {
			UnicodeString uniFile = mvceditor::StringHelperClass::wxToIcu(fullPath);
			written = u_fprintf(uf, "CLASS,%S,%.*S,\n", 
				uniFile.getTerminatedBuffer(), it->ClassName.length(), it->ClassName.getBuffer());
			error = 0 == written;
		}
		else if (mvceditor::ResourceClass::FUNCTION == it->Type) {
			UnicodeString uniFile = mvceditor::StringHelperClass::wxToIcu(fullPath);
			written = u_fprintf(uf, "FUNCTION,%S,%.*S,\n", 
				uniFile.getTerminatedBuffer(), it->Identifier.length(), it->Identifier.getBuffer());
			error = 0 == written;
		}
		else if (mvceditor::ResourceClass::MEMBER == it->Type) {
			UnicodeString uniFile = mvceditor::StringHelperClass::wxToIcu(fullPath);
			written = u_fprintf(uf, "MEMBER,%S,%S,%.*S\n", 
				uniFile.getTerminatedBuffer(), 
				it->ClassName.getTerminatedBuffer(),
				it->Identifier.length(), it->Identifier.getBuffer());
			error = 0 == written;
		}
		else if (mvceditor::ResourceClass::METHOD == it->Type && !it->IsNative) {
			UnicodeString uniFile = mvceditor::StringHelperClass::wxToIcu(fullPath);
			written = u_fprintf(uf, "METHOD,%S,%S,%.*S\n", 
				uniFile.getTerminatedBuffer(), 
				it->ClassName.getTerminatedBuffer(),
				it->Identifier.length(), it->Identifier.getBuffer());
			error = 0 == written;
		}
	}
	u_fclose(uf);
	return !error;
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

void mvceditor::ResourceFinderClass::Clear() {
	if (!IsCacheInitialized) {
		return;
	}
	FileParsingCache.clear();
	Session.once << "DELETE FROM file_items;";
	Session.once << "DELETE FROM resources;";
}

void mvceditor::ResourceFinderClass::PushIntoResourceCache(const std::vector<mvceditor::ResourceClass>& resources, int fileItemId) {
	if (!IsCacheInitialized) {
		return;
	}
	std::string sql;
	sql += "INSERT OR IGNORE INTO resources (";
	sql += "file_item_id, key, identifier, class_name, ";
	sql += "type, namespace_name, signature, ";
	sql += "return_type, comment, is_protected, is_private, ";
	sql += "is_static, is_dynamic, is_native";
	sql += ") VALUES(";
	sql += "?, ?, ?, ?, ";
	sql += "?, ?, ?, ";
	sql += "?, ?, ?, ?, ";
	sql += "?, ?, ?";
	sql += ");";

	std::string key;
	std::string identifier;
	std::string className;
	int type;
	std::string namespaceName;
	std::string signature;
	std::string returnType;
	std::string comment;
	int isProtected;
	int isPrivate;
	int isStatic;
	int isDynamic;
	int isNative;

	
	soci::statement stmt = (Session.prepare << sql,
		soci::use(fileItemId), soci::use(key), soci::use(identifier), soci::use(className), 
		soci::use(type), soci::use(namespaceName), soci::use(signature), 
		soci::use(returnType), soci::use(comment), soci::use(isProtected), soci::use(isPrivate), 
		soci::use(isStatic), soci::use(isDynamic), soci::use(isNative)
	);
	std::vector<mvceditor::ResourceClass>::const_iterator it;
	for (it = resources.begin(); it != resources.end(); ++it) {
		key = mvceditor::StringHelperClass::IcuToChar(it->Key);
		identifier = mvceditor::StringHelperClass::IcuToChar(it->Identifier);
		className = mvceditor::StringHelperClass::IcuToChar(it->ClassName);
		type = it->Type;
		namespaceName = mvceditor::StringHelperClass::IcuToChar(it->NamespaceName);
		signature = mvceditor::StringHelperClass::IcuToChar(it->Signature);
		returnType = mvceditor::StringHelperClass::IcuToChar(it->ReturnType);
		comment = mvceditor::StringHelperClass::IcuToChar(it->Comment);
		isProtected = it->IsProtected;
		isPrivate = it->IsPrivate;
		isStatic = it->IsStatic;
		isDynamic = it->IsDynamic;
		isNative = it->IsNative;
		stmt.execute(true);
	}
}

mvceditor::ResourceClass::ResourceClass()
	: Identifier()
	, ClassName()
	, NamespaceName()
	, Signature()
	, ReturnType()
	, Comment()
	, FullPath()
	, Type(CLASS) 
	, IsProtected(false)
	, IsPrivate(false) 
	, IsStatic(false)
	, IsDynamic(false)
	, IsNative(false)
	, Key()
	, FileItemId(-1) 
	, FileIsNew(false) {
		
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
	FullPath.Assign(wxT(""));
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

mvceditor::TraitResourceClass::TraitResourceClass() 
	: TraitClassName()
	, Aliased()
	, Excluded() {
		
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
	bool modified = fileLastModifiedDateTime.IsLaterThan(DateTime);
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