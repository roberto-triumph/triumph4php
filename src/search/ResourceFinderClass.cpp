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
#include <unicode/ustring.h>
#include <unicode/fmtable.h>
#include <unicode/numfmt.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <sqlite3.h>
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
	ResourceTypes resourceType = CLASS_NAME;

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
		bool hasSymbols = NULL != u_strpbrk(resourceQuery.getTerminatedBuffer(), symbols.getTerminatedBuffer());
		if (hasSymbols) {
			FileName = resourceQuery;
			ResourceType = FILE_NAME;
		}
		else {
			ClassName = resourceQuery;
			ResourceType = CLASS_NAME;
		}
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
	: FileFilters()
	, FileParsingCache()
	, NamespaceCache()
	, TraitCache()
	, Parser()
	, Session()
	, Transaction(NULL)
	, FileParsingBufferSize(32)
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
	OpenAndCreateTables(wxT(":memory:"));
	FileParsingBufferSize = 32;
	FileParsingCache.clear();
	FileParsingCache.resize(0);
	FileParsingCache.reserve(FileParsingBufferSize);
}

void mvceditor::ResourceFinderClass::InitFile(const wxFileName& fileName, int fileParsingBufferSize) {
	OpenAndCreateTables(fileName.GetFullPath());
	FileParsingBufferSize = fileParsingBufferSize;
}

void mvceditor::ResourceFinderClass::OpenAndCreateTables(const wxString& dbName) {
	try {

		// close any existing connection
		if (IsCacheInitialized) {
			Session.close();
			IsCacheInitialized = false;
		}
		if (dbName.CompareTo(wxT(":memory:")) == 0) {
			Session.open(*soci::factory_sqlite3(), ":memory:");
		}
		else {
			std::string stdDbName = mvceditor::StringHelperClass::wxToChar(dbName);
			Session.open(*soci::factory_sqlite3(), stdDbName);
		}
			
		// open the SQL script that contains the table creation statements
		// the script is "nice" it takes care to not create the tables if
		// they already exist
		wxFileName sqlScriptFileName = mvceditor::ResourceSqlSchemaAsset();
		if (sqlScriptFileName.FileExists()) {
			wxFFile ffile(sqlScriptFileName.GetFullPath());
			wxString sql;
			ffile.ReadAll(&sql);
			std::string stdSql = mvceditor::StringHelperClass::wxToChar(sql);

			// get the 'raw' connection because it can handle multiple statements at once
			char *errorMessage = NULL;
			soci::sqlite3_session_backend* backend = static_cast<soci::sqlite3_session_backend*>(Session.get_backend());
			sqlite_api::sqlite3_exec(backend->conn_, stdSql.c_str(), NULL, NULL, &errorMessage);
			IsCacheInitialized = NULL == errorMessage;
			if (errorMessage) {
				wxString msg = mvceditor::StringHelperClass::charToWx(errorMessage);
				wxASSERT_MSG(IsCacheInitialized, msg);
				sqlite_api::sqlite3_free(errorMessage);
			}
		}
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
	TraitCache.clear();

	FileParsingCache.clear();
	FileParsingCache.resize(0);
	FileParsingCache.reserve(FileParsingBufferSize);
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
	TraitCache.clear();

	// reclaim mem since we no longer need it
	FileParsingCache.resize(1);
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
		BuildResourceCache(fileName, true);
	}

	// no need to keep know which files have resources, most likely all files should have a resource
	return false;
}

void mvceditor::ResourceFinderClass::BuildResourceCacheForFile(const wxString& fullPath, const UnicodeString& code, bool isNew) {
	BeginSearch();

	// remove all previous cached resources
	mvceditor::FileItemClass fileItem;
	bool foundFile = FindFileItemByFullPathExact(fullPath, fileItem);
	if (foundFile) {
		std::vector<int> fileItemIdsToRemove;
		fileItemIdsToRemove.push_back(fileItem.FileId);
		RemovePersistedResources(fileItemIdsToRemove);
	}
	if (!foundFile) {

		// if caller just calls this method without calling Walk(); then file cache will be empty
		// need to add an entry so that GetResourceMatchFullPathFromResource works correctly
		fileItem.FullPath = fullPath;
		fileItem.IsNew = isNew;
		fileItem.DateTime = wxDateTime::Now();
		fileItem.IsParsed = false;
		PersistFileItem(fileItem);
	}
	
	FileParsingCache.clear();

	// for now silently ignore parse errors
	pelet::LintResultsClass results;
	Parser.ScanString(code, results);

	PersistResources(FileParsingCache, fileItem.FileId);
	PersistTraits(TraitCache);
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

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::CollectNearMatchResources(const mvceditor::ResourceSearchClass& resourceSearch) {
	std::vector<mvceditor::ResourceClass> matches;
	switch (resourceSearch.GetResourceType()) {
		case mvceditor::ResourceSearchClass::FILE_NAME:
		case mvceditor::ResourceSearchClass::FILE_NAME_LINE_NUMBER:
			matches = CollectNearMatchFiles(resourceSearch);
			break;
		case mvceditor::ResourceSearchClass::CLASS_NAME:
			matches = CollectNearMatchNonMembers(resourceSearch);
			break;
		case mvceditor::ResourceSearchClass::CLASS_NAME_METHOD_NAME:
			matches = CollectNearMatchMembers(resourceSearch);
			break;
		case mvceditor::ResourceSearchClass::NAMESPACE_NAME:
			matches = CollectNearMatchNamespaces(resourceSearch);
			break;
	}
	EnsureMatchesExist(matches);
	sort(matches.begin(), matches.end());
	return matches;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::CollectNearMatchFiles(const mvceditor::ResourceSearchClass& resourceSearch) {
	wxString path,
		currentFileName,
		extension;
	std::vector<mvceditor::ResourceClass> matches;
	std::string query = mvceditor::StringHelperClass::IcuToChar(resourceSearch.GetFileName());

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
			wxString fileName = mvceditor::StringHelperClass::IcuToWx(resourceSearch.GetFileName());
			if (wxNOT_FOUND != currentFileName.Lower().Find(fileName)) {
				if (0 == resourceSearch.GetLineNumber() || GetLineCountFromFile(fullPath) >= resourceSearch.GetLineNumber()) {
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

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::CollectNearMatchNonMembers(const mvceditor::ResourceSearchClass& resourceSearch) {
	std::string key = mvceditor::StringHelperClass::IcuToChar(resourceSearch.GetClassName());
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
	
	// look for the parent classes. when we are searching for members, we want to look
	// for members from the class or any class it inherited from
	std::vector<UnicodeString> parentClassNames = resourceSearch.GetParentClasses();
	parentClassNames.push_back(resourceSearch.GetClassName());
	std::vector<UnicodeString> traits = resourceSearch.GetTraits();
	traits.push_back(resourceSearch.GetClassName());

	if (resourceSearch.GetMethodName().isEmpty()) {
		
		// special case; query for all methods for a class (UserClass::)
		std::vector<mvceditor::ResourceClass> memberMatches = CollectAllMembers(parentClassNames);

		//get the methods that belong to a used trait
		std::vector<mvceditor::ResourceClass> traitMatches = CollectAllTraitMembers(traits);

		matches.insert(matches.end(), memberMatches.begin(), memberMatches.end());
		matches.insert(matches.end(), traitMatches.begin(), traitMatches.end());
	}
	else if (resourceSearch.GetClassName().isEmpty()) {
		
		// special case, query across all classes for a method (::getName)
		// if ClassName is empty, then just check method names This ensures 
		// queries like '::getName' will work as well.
		// make sure to NOT get fully qualified  matches (key=identifier)
		std::string identifier = mvceditor::StringHelperClass::IcuToChar(resourceSearch.GetMethodName());
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
		for (std::vector<UnicodeString>::iterator it = parentClassNames.begin(); it != parentClassNames.end(); ++it) {
			std::string keyStart = mvceditor::StringHelperClass::IcuToChar(*it);
			keyStart += "::";
			keyStart += mvceditor::StringHelperClass::IcuToChar(resourceSearch.GetMethodName());
			keyStarts.push_back(keyStart);
		}
		for (std::vector<UnicodeString>::iterator it = traits.begin(); it != traits.end(); ++it) {
			std::string keyStart = mvceditor::StringHelperClass::IcuToChar(*it);
			keyStart += "::";
			keyStart += mvceditor::StringHelperClass::IcuToChar(resourceSearch.GetMethodName());
			keyStarts.push_back(keyStart);
		}
		matches = FindByKeyStartMany(keyStarts, true);
	}
	return matches;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::CollectAllMembers(const std::vector<UnicodeString>& classNames) {
	std::vector<mvceditor::ResourceClass> matches;
	std::vector<std::string> keyStarts;
	for (size_t i = 0; i < classNames.size(); ++i) {
		std::string keyStart = mvceditor::StringHelperClass::IcuToChar(classNames[i]);
		keyStart += "::";
		keyStarts.push_back(keyStart);
	}
	matches = FindByKeyStartMany(keyStarts, true);
	return matches;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::CollectAllTraitMembers(const std::vector<UnicodeString>& classNames) {
	std::vector<mvceditor::ResourceClass> matches;

	std::vector<std::string> classNamesToLookFor;
	for (std::vector<UnicodeString>::const_iterator it = classNames.begin(); it != classNames.end(); ++it) {
		classNamesToLookFor.push_back(mvceditor::StringHelperClass::IcuToChar(*it));
	}

	// TODO use the correct namespace when querying for traits
	std::vector<mvceditor::TraitResourceClass> traits = FindTraitsByClassName(classNamesToLookFor);
	for (size_t j = 0; j < traits.size(); ++j) {
		
		// lets unqualify, the members cache key does not have the namespace name
		UnicodeString key =  traits[j].TraitClassName + UNICODE_STRING_SIMPLE("::");
		
		std::string stdKey = mvceditor::StringHelperClass::IcuToChar(key);
		std::vector<mvceditor::ResourceClass> traitMatches = FindByKeyStart(stdKey, true);
		for (std::vector<mvceditor::ResourceClass>::iterator traitMember = traitMatches.begin(); traitMember != traitMatches.end(); ++traitMember) {
			
			// make sure we got the trait from the correct namespace
			if (traits[j].NamespaceName.caseCompare(traitMember->NamespaceName, 0) == 0) {
				matches.push_back(*traitMember);
			}
		}
		
		
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

std::vector<mvceditor::ResourceClass>  mvceditor::ResourceFinderClass::CollectNearMatchNamespaces(const mvceditor::ResourceSearchClass& resourceSearch) {
	std::vector<mvceditor::ResourceClass> matches;

	// needle identifier contains a namespace operator; but it may be
	// a namespace or a fully qualified name
	UnicodeString key = resourceSearch.GetClassName();
	std::string stdKey = mvceditor::StringHelperClass::IcuToChar(key);
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
	std::string key = mvceditor::StringHelperClass::IcuToChar(className);
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
	keys.push_back(mvceditor::StringHelperClass::IcuToChar(className));
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


void mvceditor::ResourceFinderClass::BuildResourceCache(const wxString& fullPath, bool parseClasses) {
	wxFileName fileName(fullPath);
	wxDateTime fileLastModifiedDateTime = fileName.GetModificationTime();
	
	// have we looked at this file yet or is cache out of date? if not, then build the cache.
	bool cached = false;
	mvceditor::FileItemClass fileItem;
	bool foundFile = FindFileItemByFullPathExact(fullPath, fileItem);
	if (foundFile) {
		bool needsToBeParsed = fileItem.NeedsToBeParsed(fileLastModifiedDateTime);
		cached = !needsToBeParsed;
	}
	else {
		fileItem.MakeNew(fileName, parseClasses);
		PersistFileItem(fileItem);
	}
	if (parseClasses) {
		if (!cached || !fileItem.IsParsed) {

			// no need to look for resources if the file had not yet existed, this will save much time
			// this optimization was found by using the profiler
			if (foundFile) {
				std::vector<int> fileItemIdsToRemove;
				fileItemIdsToRemove.push_back(fileItem.FileId);
				RemovePersistedResources(fileItemIdsToRemove);
			}			
			FileParsingCache.clear();
			
			// for now silently ignore files with parser errors
			pelet::LintResultsClass lintResults;
			wxFFile file(fullPath, wxT("rb"));
			Parser.ScanFile(file.fp(), mvceditor::StringHelperClass::wxToIcu(fullPath), lintResults);
			PersistResources(FileParsingCache, fileItem.FileId);
			PersistTraits(TraitCache);
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
	
	// the trait has already been put in the cache; we just need to update it
	UnicodeString mapKey;
	int capacity = 10 + namespaceName.length() + 1 + className.length() + 1 + traitUsedClassName.length();
	int written = u_sprintf(mapKey.getBuffer(capacity), "%.*S-%.*S-%.*S", 
		namespaceName.length(), namespaceName.getBuffer(), 
		className.length(), className.getBuffer(),
		traitUsedClassName.length(), traitUsedClassName.getBuffer());
	mapKey.releaseBuffer(written);
	
	// this code assumes that TraitUseFound() is called before TraitAliasFound()
	std::map<UnicodeString, std::vector<mvceditor::TraitResourceClass>, UnicodeStringComparatorClass>::iterator it;
	it = TraitCache.find(mapKey);
	if (!it->second.empty()) {
		it->second.front().Aliased.push_back(alias);
		it->second.back().Aliased.push_back(alias);
	}
}

void mvceditor::ResourceFinderClass::TraitInsteadOfFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& traitUsedClassName,
													   const UnicodeString& traitMethodName, const std::vector<UnicodeString>& insteadOfList) {
	
	// the trait has already been put in the cache; we just need to update it
	UnicodeString mapKey;
	int capacity = 10 + namespaceName.length() + 1 + className.length() + 1 + traitUsedClassName.length();
	int written = u_sprintf(mapKey.getBuffer(capacity), "%.*S-%.*S-%.*S", 
		namespaceName.length(), namespaceName.getBuffer(), 
		className.length(), className.getBuffer(),
		traitUsedClassName.length(), traitUsedClassName.getBuffer());
	mapKey.releaseBuffer(written);
	
	// this code assumes that TraitUseFound() is called before TraitAliasFound()
	std::map<UnicodeString, std::vector<mvceditor::TraitResourceClass>, UnicodeStringComparatorClass>::iterator it;
	it = TraitCache.find(mapKey);
	if (!it->second.empty()) {

		it->second.front().InsteadOfs.insert(it->second.front().InsteadOfs.end(), insteadOfList.begin(), insteadOfList.end());
		it->second.back().InsteadOfs.insert(it->second.back().InsteadOfs.end(), insteadOfList.begin(), insteadOfList.end());
	}
}

void mvceditor::ResourceFinderClass::TraitUseFound(const UnicodeString& namespaceName, const UnicodeString& className, 
												const UnicodeString& fullyQualifiedTraitName) {
	
	mvceditor::TraitResourceClass newTraitResource;
	newTraitResource.ClassName = className;
	newTraitResource.NamespaceName = namespaceName;

	int32_t pos = fullyQualifiedTraitName.lastIndexOf(UNICODE_STRING_SIMPLE("\\"));
	if (pos > 0) {
		newTraitResource.TraitClassName.setTo(fullyQualifiedTraitName, 0, pos);
		newTraitResource.TraitNamespaceName.setTo(fullyQualifiedTraitName, pos + 1);
	}
	else {
		newTraitResource.TraitClassName = fullyQualifiedTraitName;
		newTraitResource.TraitNamespaceName = UNICODE_STRING_SIMPLE("\\");
	}

	// only add if not already there
	bool found = false;
	
	UnicodeString mapKey;
	
	// 10 = max space for the FileItemId as a string
	// key is a concatenation of file item id, fully qualified class and fully qualified trait
	// this will make the alias and instead easier to update
	int capacity = 10 + namespaceName.length() + 1 + className.length() + 1 + fullyQualifiedTraitName.length();
	int written = u_sprintf(mapKey.getBuffer(capacity), "%.*S-%.*S-%.*S", 
		namespaceName.length(), namespaceName.getBuffer(), 
		className.length(), className.getBuffer(),
		fullyQualifiedTraitName.length(), fullyQualifiedTraitName.getBuffer());
	mapKey.releaseBuffer(written);

	int count = TraitCache.count(mapKey);
	if (count <= 0) {
		newTraitResource.Key = QualifyName(namespaceName, className);
		TraitCache[mapKey].push_back(newTraitResource);
		
		// put a non-qualified version too, sometimes the query will not contain a fully qualified name
		newTraitResource.Key = className;
		TraitCache[mapKey].push_back(newTraitResource);
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

void mvceditor::ResourceFinderClass::RemovePersistedResources(const std::vector<int>& fileItemIds) {
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

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::CollectFullyQualifiedResource(const mvceditor::ResourceSearchClass& resourceSearch) {
	ResourceClass needle;
	std::vector<mvceditor::ResourceClass> allMatches;
	if (resourceSearch.GetResourceType() == mvceditor::ResourceSearchClass::CLASS_NAME_METHOD_NAME) {

		// check the entire class hierachy; stop as soon as we found it
		// combine the parent classes with the class being searched
		std::vector<UnicodeString> classHierarchy = resourceSearch.GetParentClasses();
		classHierarchy.push_back(resourceSearch.GetClassName());
		std::vector<int> types;
		types.push_back(mvceditor::ResourceClass::MEMBER);
		types.push_back(mvceditor::ResourceClass::METHOD);
		types.push_back(mvceditor::ResourceClass::CLASS_CONSTANT);
		for (size_t i = 0; i < classHierarchy.size(); ++i) {
			UnicodeString key = classHierarchy[i] + UNICODE_STRING_SIMPLE("::") + resourceSearch.GetMethodName();
			std::string stdKey = mvceditor::StringHelperClass::IcuToChar(key);
			std::vector<mvceditor::ResourceClass> matches = FindByKeyExactAndTypes(stdKey, types, true);
			if (!matches.empty()) {
				allMatches.push_back(matches[0]);
			}
		}
	}
	else if (mvceditor::ResourceSearchClass::NAMESPACE_NAME == resourceSearch.GetResourceType()) {
		UnicodeString key = resourceSearch.GetClassName();
		std::string stdKey = mvceditor::StringHelperClass::IcuToChar(key);

		// make sure there is one and only one item that matches the search.
		std::vector<mvceditor::ResourceClass> matches = FindByKeyExact(stdKey);
		if (matches.size() == 1) {
			allMatches.push_back(matches[0]);
		}
	}
	else {
		UnicodeString key = resourceSearch.GetClassName();
		std::string stdKey = mvceditor::StringHelperClass::IcuToChar(key);

		// make sure there is one and only one item that matches the search.
		std::vector<mvceditor::ResourceClass> matches = FindByKeyExact(stdKey);
		if (matches.size() == 1) {
			allMatches.push_back(matches[0]);
		}
	}
	EnsureMatchesExist(allMatches);
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
		RemovePersistedResources(fileItemIdsToRemove);
	}
}

void mvceditor::ResourceFinderClass::Print() {
	UFILE *out = u_finit(stdout, NULL, NULL);
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
			for (size_t k = 0; k < trait.InsteadOfs.size(); k++) {
				u_fprintf(out, "\tINSTEADOF METHOD=%S\n", trait.InsteadOfs[k].getTerminatedBuffer());
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

				// resource already exists, but it does not have a return type. only update the return type
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
			else if (returnType.empty() && classNameOut.empty() && identifierOut.empty()) {
				
				// make sure to not insert duplicate items (if we found them)
				resource.IsDynamic = true;
				resource.Key = resource.Identifier;
				newResources.push_back(resource);
				
				// the fully qualified version
				resource.Key = resource.ClassName + UNICODE_STRING_SIMPLE("::") + resource.Identifier;
				newResources.push_back(resource);
			}
		}
		else {

			// look at the function, cache
			bool updated = false;
			std::ostringstream stream;
			stream << "SELECT return_type, identifier FROM resources WHERE key = ? AND type IN("
				<< mvceditor::ResourceClass::FUNCTION
				<< ")";
			std::string identifier = mvceditor::StringHelperClass::IcuToChar(resource.Identifier);
			std::string returnType;
			std::string identifierOut;
			Session.once << stream.str(), soci::use(identifier), soci::into(returnType), soci::into(identifierOut);
			if (returnType.empty() && !identifier.empty()) {

				// function already exists, just update the return type
				returnType = mvceditor::StringHelperClass::IcuToChar(resource.ReturnType);
				std::ostringstream updateStream;
				updateStream << "UPDATE resources SET return_type = ? WHERE key = ? AND type IN("
					<< mvceditor::ResourceClass::FUNCTION
					<< ")";				
				Session.once << updateStream.str(), soci::use(returnType), soci::use(identifier);
				updated = true;
			}
			else if (returnType.empty() && identifierOut.empty()) {

				// make sure to not insert duplicate items (if we found them)
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
	PersistResources(newResources, fileItemIndex);
	EndSearch();
}

void mvceditor::ResourceFinderClass::PersistFileItem(mvceditor::FileItemClass& fileItem) {
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

bool mvceditor::ResourceFinderClass::FindFileItemByFullPathExact(const wxString& fullPath, mvceditor::FileItemClass& fileItem) {
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
	std::string whereCond = "key LIKE '" + keyStart + "%'";
	return ResourceStatementMatches(whereCond, doLimit);
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceFinderClass::FindByKeyStartAndTypes(const std::string& keyStart, const std::vector<int>& types, bool doLimit) {
	std::ostringstream stream;
	stream << "key LIKE '" << keyStart << "%' AND type IN(";
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
	std::ostringstream stream;
	stream << "key LIKE '" << keyStarts[0] << "%'";
	for (size_t i = 1; i < keyStarts.size(); ++i) {
		stream << " OR key LIKE '" << keyStarts[i] << "%'";
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
	stream << "key LIKE '" << identifierStart << "%' AND identifier = key AND type IN(";
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

void mvceditor::ResourceFinderClass::Clear() {
	if (!IsCacheInitialized) {
		return;
	}
	FileParsingCache.clear();
	Session.once << "DELETE FROM file_items;";
	Session.once << "DELETE FROM resources;";
}

void mvceditor::ResourceFinderClass::PersistResources(const std::vector<mvceditor::ResourceClass>& resources, int fileItemId) {
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

void mvceditor::ResourceFinderClass::PersistTraits(
	const std::map<UnicodeString, std::vector<mvceditor::TraitResourceClass>, UnicodeStringComparatorClass>& traitMap) {
	if (!IsCacheInitialized) {
		return;
	}
	std::string sql;
	sql += "INSERT OR IGNORE INTO trait_resources(";
	sql += "key, class_name, namespace_name, trait_name, ";
	sql += "trait_namespace_name, aliases, instead_ofs) VALUES ("; 
	sql += "?, ?, ?, ?, ";
	sql += "?, ?, ?)";
	std::string key;
	std::string className;
	std::string namespaceName;
	std::string traitClassName;
	std::string traitNamespaceName;
	std::string aliases;
	std::string insteadOfs;

	soci::statement stmt = (Session.prepare << sql,
		soci::use(key), soci::use(className), soci::use(namespaceName), soci::use(traitClassName),
		soci::use(traitNamespaceName), soci::use(aliases), soci::use(insteadOfs)
	);
	std::map<UnicodeString, std::vector<mvceditor::TraitResourceClass>, UnicodeStringComparatorClass>::const_iterator it;
	for (it = traitMap.begin(); it != traitMap.end(); ++it) {
		std::vector<mvceditor::TraitResourceClass>::const_iterator trait;
		for (trait = it->second.begin(); trait != it->second.end(); ++trait) {
			key = mvceditor::StringHelperClass::IcuToChar(trait->Key);
			className = mvceditor::StringHelperClass::IcuToChar(trait->ClassName);
			namespaceName = mvceditor::StringHelperClass::IcuToChar(trait->NamespaceName);
			traitClassName = mvceditor::StringHelperClass::IcuToChar(trait->TraitClassName);
			traitNamespaceName = mvceditor::StringHelperClass::IcuToChar(trait->TraitNamespaceName);
			aliases = "";
			for (std::vector<UnicodeString>::const_iterator alias = trait->Aliased.begin(); alias != trait->Aliased.end(); ++alias) {
				aliases += mvceditor::StringHelperClass::IcuToChar(*alias);
				aliases += ",";
			}
			if (!aliases.empty()) {
				aliases.erase(aliases.end() - 1);
			}
			insteadOfs = "";
			for (std::vector<UnicodeString>::const_iterator instead = trait->InsteadOfs.begin(); instead != trait->InsteadOfs.end(); ++instead) {
				insteadOfs += mvceditor::StringHelperClass::IcuToChar(*instead);
				insteadOfs += ",";
			}
			if (!insteadOfs.empty()) {
				insteadOfs.erase(insteadOfs.end() - 1);
			}
			stmt.execute(true);
		}
	}
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
	
	soci::statement stmt = (Session.prepare << sql,
		soci::into(key), soci::into(className), soci::into(namespaceName), soci::into(traitClassName),
		soci::into(traitNamespaceName), soci::into(aliases), soci::into(insteadOfs)
	);
	std::vector<mvceditor::TraitResourceClass> matches;
	if (stmt.execute()) {
		while (stmt.fetch()) {
			mvceditor::TraitResourceClass trait;
			trait.Key = mvceditor::StringHelperClass::charToIcu(key.c_str());
			trait.ClassName = mvceditor::StringHelperClass::charToIcu(className.c_str());
			trait.NamespaceName = mvceditor::StringHelperClass::charToIcu(namespaceName.c_str());
			trait.TraitClassName = mvceditor::StringHelperClass::charToIcu(traitClassName.c_str());
			trait.TraitNamespaceName = mvceditor::StringHelperClass::charToIcu(traitNamespaceName.c_str());
			
			size_t start = 0;
			size_t found = aliases.find_first_of(",");
			while (found != std::string::npos) {
				trait.Aliased.push_back(mvceditor::StringHelperClass::charToIcu(aliases.substr(start, found).c_str()));	
				start = found++;
			}
			if (!aliases.empty()) {
				trait.Aliased.push_back(mvceditor::StringHelperClass::charToIcu(aliases.substr(start, found).c_str()));
			}

			start = 0;
			found = insteadOfs.find_first_of(",");
			while (found != std::string::npos) {
				trait.InsteadOfs.push_back(mvceditor::StringHelperClass::charToIcu(insteadOfs.substr(start, found).c_str()));	
				start = found++;
			}
			if (!insteadOfs.empty()) {
				trait.InsteadOfs.push_back(mvceditor::StringHelperClass::charToIcu(insteadOfs.substr(start, found).c_str()));
			}

			matches.push_back(trait);
		}
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