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
#include <language/TagParserClass.h>
#include <language/FileTags.h>
#include <search/FinderClass.h>
#include <globals/String.h>
#include <globals/Assets.h>
#include <wx/filename.h>
#include <wx/ffile.h>
#include <algorithm>
#include <fstream>
#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include <unicode/fmtable.h>
#include <unicode/numfmt.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
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

std::vector<mvceditor::TagClass> AllResources(soci::session& session) {
	std::string sql;
	sql += "SELECT r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON(r.file_item_id = f.file_item_id) ";
	sql += " ORDER BY key";
	
	std::vector<mvceditor::TagClass> matches;
	int fileTagId;
	int sourceId;
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
		soci::statement stmt = (session.prepare << sql,
			soci::into(fileTagId, fileTagIdIndicator), soci::into(sourceId), soci::into(key), soci::into(identifier), soci::into(className), 
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
				tag.SourceId = sourceId;

				matches.push_back(tag);
			} while (stmt.fetch());
		}
	} catch (std::exception& e) {
			
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		wxString msg = mvceditor::CharToWx(e.what());
		wxASSERT_MSG(false, msg);
	}
	return matches;
}

mvceditor::TagParserClass::TagParserClass()
	: PhpFileExtensions()
	, MiscFileExtensions()
	, NamespaceCache()
	, TraitCache()
	, Parser()
	, Session(NULL)
	, Transaction(NULL)
	, InsertStmt(NULL)
	, CurrentSourceId(0)
	, FilesParsed(0)
	, IsCacheInitialized(false) {
	Parser.SetClassObserver(this);
	Parser.SetClassMemberObserver(this);
	Parser.SetFunctionObserver(this);
}

mvceditor::TagParserClass::~TagParserClass() {
	Close();
}

void mvceditor::TagParserClass::SetVersion(pelet::Versions version) {
	Parser.SetVersion(version);
}

void mvceditor::TagParserClass::Init(soci::session* session) {
	Session = session;
	IsCacheInitialized = true;
}

void mvceditor::TagParserClass::Close() {
	Session = NULL;
	if (Transaction) {
		Transaction->rollback();
		delete Transaction;
		Transaction = NULL;
	}
	if (InsertStmt) {
		delete InsertStmt;
		InsertStmt = NULL;
	}
}

void mvceditor::TagParserClass::BeginSearch(const wxString& fullPath) {

	// make sure we always insert with the trailing directory separator
	// to be consistent
	wxFileName dir;
	dir.AssignDir(fullPath);


	// get (or create) the source ID
	try {
		CurrentSourceId = 0;
		std::string stdFullPath = mvceditor::WxToChar(dir.GetPathWithSep());
		soci::statement stmt = (Session->prepare << 
			"SELECT source_id FROM sources WHERE directory = ?", 
			soci::into(CurrentSourceId), soci::use(stdFullPath)
		);
		if (!stmt.execute(true)) {

			// didn't find the source, create a new row
			soci::statement stmt = (Session->prepare <<
				"INSERT INTO sources(directory) VALUES(?)",
				soci::use(stdFullPath)
			);
			stmt.execute(true);
			soci::sqlite3_statement_backend* backend = static_cast<soci::sqlite3_statement_backend*>(stmt.get_backend());
			CurrentSourceId = sqlite3_last_insert_rowid(backend->session_.conn_);
		}
		BeginTransaction();
	} catch (std::exception& e) {
		
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		e.what();
	}
}

void mvceditor::TagParserClass::BeginTransaction() {
	NamespaceCache.clear();
	TraitCache.clear();

	// start a transaction here
	FilesParsed = 0;
	try {
		Transaction = new soci::transaction(*Session);

		std::string sql;
		sql += "INSERT OR IGNORE INTO resources (";
		sql += "file_item_id, source_id, key, identifier, class_name, ";
		sql += "type, namespace_name, signature, ";
		sql += "return_type, comment, is_protected, is_private, ";
		sql += "is_static, is_dynamic, is_native";
		sql += ") VALUES(";
		sql += "?, ?, ?, ?, ?, ";
		sql += "?, ?, ?, ";
		sql += "?, ?, ?, ?, ";
		sql += "?, ?, ?";
		sql += ");";
		
		wxASSERT_MSG(!InsertStmt, wxT("statement should be cleaned up"));
		InsertStmt = new soci::statement(*Session);
		*InsertStmt = (Session->prepare << sql,
			soci::use(FileTagId), soci::use(CurrentSourceId), soci::use(Key), soci::use(Identifier), soci::use(ClassName), 
			soci::use(Type), soci::use(NamespaceName), soci::use(Signature), 
			soci::use(ReturnType), soci::use(Comment), soci::use(IsProtected), soci::use(IsPrivate), 
			soci::use(IsStatic), soci::use(IsDynamic), soci::use(IsNative)
		);
	} catch (std::exception& e) {
		
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
		if (InsertStmt) {
			delete InsertStmt;
			InsertStmt = NULL;
		}
	}
}

void mvceditor::TagParserClass::EndSearch() {
	try {
		Transaction->commit();
	} catch (std::exception& e) {
		
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
	delete Transaction;
	Transaction = NULL;

	delete InsertStmt;
	InsertStmt = NULL;
	
	NamespaceCache.clear();
	TraitCache.clear();	
}

bool mvceditor::TagParserClass::Walk(const wxString& fileName) {
	bool matchedFilter = false;
	wxFileName file(fileName);
	for (size_t i = 0; i < PhpFileExtensions.size(); ++i) {
		wxString filter = PhpFileExtensions[i];
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
	else {
		// check the misc file filters
		for (size_t i = 0; i < MiscFileExtensions.size(); ++i) {
			wxString filter = MiscFileExtensions[i];
			if (!wxIsWild(filter)) {

				// exact match of the name portion of fileName
				matchedFilter = file.GetFullName().CmpNoCase(filter) == 0;
			}
			else {
				matchedFilter = wxMatchWild(filter, fileName);
			}
			if (matchedFilter) {
				break;
			}
		}
		if (matchedFilter) {
			BuildResourceCache(fileName, false);
		}
	}

	// no need to keep know which files have resources, most likely all files should have a tag
	return false;
}

void mvceditor::TagParserClass::BuildResourceCacheForFile(const wxString& fullPath, const UnicodeString& code, bool isNew) {
	CurrentSourceId = 0;
	BeginTransaction();

	// remove all previous cached resources
	mvceditor::FileTagClass fileTag;
	bool foundFile = FindFileTagByFullPathExact(fullPath, fileTag);
	if (foundFile) {
		std::vector<int> fileTagIdsToRemove;
		fileTagIdsToRemove.push_back(fileTag.FileId);

		// dont remove file tag, since we set source ID for the file
		// we also want the new tags to have the file's proper source ID
		CurrentSourceId = fileTag.SourceId;
		RemovePersistedResources(fileTagIdsToRemove, false);
	}
	else {
		// if caller just calls this method without calling Walk(); then file cache will be empty
		// need to add an entry so that GetResourceMatchFullPathFromResource works correctly
		fileTag.FullPath = fullPath;
		fileTag.IsNew = isNew;
		fileTag.DateTime = wxDateTime::Now();
		fileTag.IsParsed = false;
		PersistFileTag(fileTag);
	}
	CurrentFileTagId = fileTag.FileId;
	
	// for now silently ignore parse errors
	pelet::LintResultsClass results;
	Parser.ScanString(code, results);

	PersistTraits(TraitCache, fileTag.FileId);
	EndSearch();
}

void mvceditor::TagParserClass::BuildResourceCache(const wxString& fullPath, bool parseClasses) {
	wxFileName fileName(fullPath);
	wxDateTime fileLastModifiedDateTime = fileName.GetModificationTime();
	
	// have we looked at this file yet or is cache out of date? if not, then build the cache.
	bool cached = false;
	mvceditor::FileTagClass fileTag;
	bool foundFile = FindFileTagByFullPathExact(fullPath, fileTag);
	if (foundFile) {
		bool needsToBeParsed = fileTag.NeedsToBeParsed(fileLastModifiedDateTime);
		cached = !needsToBeParsed;
	}
	else {
		fileTag.MakeNew(fileName, parseClasses);
		PersistFileTag(fileTag);
	}
	if (parseClasses) {
		if (!cached || !fileTag.IsParsed) {

			// no need to look for resources if the file had not yet existed, this will save much time
			// this optimization was found by using the profiler
			if (foundFile) {
				std::vector<int> fileTagIdsToRemove;
				fileTagIdsToRemove.push_back(fileTag.FileId);
				RemovePersistedResources(fileTagIdsToRemove, true);

				// the previous line deleted the file from file_items
				// we need to re-add it
				fileTag.MakeNew(fileName, parseClasses);
				PersistFileTag(fileTag);
			}			
			
			// for now silently ignore files with parser errors
			pelet::LintResultsClass lintResults;
			wxFFile file(fullPath, wxT("rb"));

			CurrentFileTagId = fileTag.FileId;
			Parser.ScanFile(file.fp(), mvceditor::WxToIcu(fullPath), lintResults);
	
			PersistTraits(TraitCache, fileTag.FileId);

			FilesParsed++;

			// commit files as we are parsing
			// this is so that the db is no locked up the entire time
			// we want tag searches to be able to pass through
			// even while the projects are being parsed
			if (FilesParsed % 200 == 0) {
				try {
					Transaction->commit();
				} catch (std::exception& e) {
					
					// ATTN: at some point bubble these exceptions up?
					// to avoid unreferenced local variable warnings in MSVC
					wxString msg = mvceditor::CharToWx(e.what());
					wxUnusedVar(msg);
					wxASSERT_MSG(false, msg);
				}
				delete Transaction;
				Transaction = new soci::transaction(*Session);
			}
		}
	}
}

void mvceditor::TagParserClass::ClassFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& signature, 
		const UnicodeString& comment, const int lineNumber) {
	TagClass classItem;
	classItem.Identifier = className;
	classItem.ClassName = className;
	classItem.NamespaceName = namespaceName;
	classItem.Key = className;
	classItem.Type = TagClass::CLASS;
	classItem.Signature = signature;
	classItem.ReturnType = UNICODE_STRING_SIMPLE("");
	classItem.Comment = comment;
	classItem.IsNative = false;
	PersistResources(classItem, CurrentFileTagId);

	if (IsNewNamespace(namespaceName)) {

		// a tag for the namespace itself
		mvceditor::TagClass namespaceItem = mvceditor::TagClass::MakeNamespace(namespaceName);
		PersistResources(namespaceItem, CurrentFileTagId);
	}

	classItem.Identifier = QualifyName(namespaceName, className);
	classItem.Key = QualifyName(namespaceName, className);
	PersistResources(classItem, CurrentFileTagId);
}

void mvceditor::TagParserClass::TraitAliasFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& traitUsedClassName,
												  const UnicodeString& traitMethodName, const UnicodeString& alias, pelet::TokenClass::TokenIds visibility) {
	
	// the trait has already been put in the cache; we just need to update it
	UnicodeString mapKey;
	mapKey += namespaceName;
	mapKey += UNICODE_STRING_SIMPLE("-");
	mapKey += className;
	mapKey += UNICODE_STRING_SIMPLE("-");
	mapKey += traitUsedClassName;
	
	// this code assumes that TraitUseFound() is called before TraitAliasFound()
	std::map<UnicodeString, std::vector<mvceditor::TraitTagClass>, UnicodeStringComparatorClass>::iterator it;
	it = TraitCache.find(mapKey);
	if (!it->second.empty()) {
		it->second.front().Aliased.push_back(alias);
		it->second.back().Aliased.push_back(alias);
	}
}

void mvceditor::TagParserClass::TraitInsteadOfFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& traitUsedClassName,
													   const UnicodeString& traitMethodName, const std::vector<UnicodeString>& insteadOfList) {
	
	// the trait has already been put in the cache; we just need to update it
	UnicodeString mapKey;
	
	mapKey += namespaceName;
	mapKey += UNICODE_STRING_SIMPLE("-");
	mapKey += className;
	mapKey += UNICODE_STRING_SIMPLE("-");
	mapKey += traitUsedClassName;
	
	// this code assumes that TraitUseFound() is called before TraitAliasFound()
	std::map<UnicodeString, std::vector<mvceditor::TraitTagClass>, UnicodeStringComparatorClass>::iterator it;
	it = TraitCache.find(mapKey);
	if (!it->second.empty()) {

		it->second.front().InsteadOfs.insert(it->second.front().InsteadOfs.end(), insteadOfList.begin(), insteadOfList.end());
		it->second.back().InsteadOfs.insert(it->second.back().InsteadOfs.end(), insteadOfList.begin(), insteadOfList.end());
	}
}

void mvceditor::TagParserClass::TraitUseFound(const UnicodeString& namespaceName, const UnicodeString& className, 
												const UnicodeString& fullyQualifiedTraitName) {
	
	mvceditor::TraitTagClass newTraitTag;
	newTraitTag.ClassName = className;
	newTraitTag.NamespaceName = namespaceName;

	int32_t pos = fullyQualifiedTraitName.lastIndexOf(UNICODE_STRING_SIMPLE("\\"));
	if (pos > 0) {
		newTraitTag.TraitClassName.setTo(fullyQualifiedTraitName, 0, pos);
		newTraitTag.TraitNamespaceName.setTo(fullyQualifiedTraitName, pos + 1);
	}
	else if (0 == pos) {

		// root namespace
		newTraitTag.TraitClassName.setTo(fullyQualifiedTraitName, 1);
		newTraitTag.TraitNamespaceName.setTo(UNICODE_STRING_SIMPLE("\\"));
	}
	else {

		//this should never get here as the parser will always give us 
		// fully qualified trait names
		newTraitTag.TraitClassName = fullyQualifiedTraitName;
		newTraitTag.TraitNamespaceName = UNICODE_STRING_SIMPLE("\\");
	}

	// only add if not already there
	// key is a concatenation of file item id, fully qualified class and fully qualified trait
	// this will make the alias and instead easier to update
	UnicodeString mapKey;
	mapKey += namespaceName;
	mapKey += UNICODE_STRING_SIMPLE("-");
	mapKey += className;
	mapKey += UNICODE_STRING_SIMPLE("-");
	mapKey += fullyQualifiedTraitName;
	
	int count = TraitCache.count(mapKey);
	if (count <= 0) {
		newTraitTag.Key = QualifyName(namespaceName, className);
		TraitCache[mapKey].push_back(newTraitTag);
		
		// put a non-qualified version too, sometimes the query will not contain a fully qualified name
		newTraitTag.Key = className;
		TraitCache[mapKey].push_back(newTraitTag);
	}
}

void mvceditor::TagParserClass::DefineDeclarationFound(const UnicodeString& namespaceName, const UnicodeString& variableName, 
		const UnicodeString& variableValue, const UnicodeString& comment, const int lineNumber) {
	TagClass defineItem;
	defineItem.Identifier = variableName;
	defineItem.Key = variableName;
	defineItem.Type = TagClass::DEFINE;
	defineItem.Signature = variableValue;
	defineItem.ReturnType = UNICODE_STRING_SIMPLE("");
	defineItem.Comment = comment;
	defineItem.IsNative = false;
	PersistResources(defineItem, CurrentFileTagId);

	defineItem.Identifier = QualifyName(namespaceName, variableName);
	defineItem.Key = QualifyName(namespaceName, variableName);
	PersistResources(defineItem, CurrentFileTagId);
}

void mvceditor::TagParserClass::MethodFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName,
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment,
		pelet::TokenClass::TokenIds visibility, bool isStatic, const int lineNumber) {
	TagClass item;
	item.Identifier = methodName;
	item.ClassName = className;
	item.NamespaceName = namespaceName;
	item.Key = methodName;
	item.Type = TagClass::METHOD;
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
	PersistResources(item, CurrentFileTagId);
	
	// insert a complete name so that we can quickly lookup all methods for a single class
	item.Key = className + UNICODE_STRING_SIMPLE("::") + methodName;
	PersistResources(item, CurrentFileTagId);

	// insert a fully qualified name and method so that we can quickly lookup all methods
	// for a namespaced class
	item.Key = QualifyName(namespaceName, className) + UNICODE_STRING_SIMPLE("::") + methodName;
	PersistResources(item, CurrentFileTagId);
}

void mvceditor::TagParserClass::PropertyFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& propertyName,
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
	TagClass item;
	item.Identifier = filteredProperty;
	item.ClassName = className;
	item.NamespaceName = namespaceName;
	item.Key = filteredProperty;
	item.Type = isConst ? TagClass::CLASS_CONSTANT : TagClass::MEMBER;
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
	PersistResources(item, CurrentFileTagId);
	
	// insert a complete name so that we can quickly lookup all methods for a single class
	item.Key = className + UNICODE_STRING_SIMPLE("::") + filteredProperty;
	PersistResources(item, CurrentFileTagId);

	// insert a fully qualified name and method so that we can quickly lookup all methods
	// for a namespaced class
	item.Key = QualifyName(namespaceName, className) + UNICODE_STRING_SIMPLE("::") + filteredProperty;
	PersistResources(item, CurrentFileTagId);
}

void mvceditor::TagParserClass::FunctionFound(const UnicodeString& namespaceName, const UnicodeString& functionName, const UnicodeString& signature, 
		const UnicodeString& returnType, const UnicodeString& comment, const int lineNumber) {
	TagClass item;
	item.Identifier = functionName;
	item.NamespaceName = namespaceName;
	item.Key = functionName;
	item.Type = TagClass::FUNCTION;
	item.Signature = signature;
	item.ReturnType = returnType;
	item.Comment = comment;
	item.IsNative = false;
	PersistResources(item, CurrentFileTagId);
		
	if (IsNewNamespace(namespaceName)) {
		mvceditor::TagClass namespaceItem = mvceditor::TagClass::MakeNamespace(namespaceName);
		PersistResources(namespaceItem, CurrentFileTagId);
	}
		
	// put in the namespace cache so that qualified name lookups work too
	item.Identifier = QualifyName(namespaceName, functionName);
	item.Key = QualifyName(namespaceName, functionName);
	PersistResources(item, CurrentFileTagId);
}

bool mvceditor::TagParserClass::IsNewNamespace(const UnicodeString& namespaceName) {
	std::string sql = "SELECT COUNT(*) FROM resources WHERE key = ? AND type = ?";
	std::string nm = mvceditor::IcuToChar(namespaceName);
	int type = mvceditor::TagClass::NAMESPACE;
	int count = 0;
	bool isNew = false;
	try {
		soci::statement stmt = (Session->prepare << sql, soci::use(nm), soci::use(type), soci::into(count));
		stmt.execute(true);
		if (count <= 0) {
		 
			// look in the current namespace cache, stuff that has not yet been added to the database
			if (NamespaceCache.count(namespaceName) <= 0) {
				isNew = true;
				NamespaceCache[namespaceName] = 1;
			}	
		}
	} catch (std::exception& e) {
		
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		e.what();
	}
	return isNew;
}

void mvceditor::TagParserClass::RemovePersistedResources(const std::vector<int>& fileTagIds, bool removeFileTag) {
	if (!IsCacheInitialized || fileTagIds.empty()) {
		return;
	}
	std::ostringstream stream;
	stream << "WHERE file_item_id IN (";
	for (size_t i = 0; i < fileTagIds.size(); ++i) {
		stream << fileTagIds[i];
		if (i < (fileTagIds.size() - 1)) {
			stream << ",";
		}
	}
	stream << ")";
	try {
		std::string deleteResourceSql = "DELETE FROM resources " + stream.str();
		std::string deleteFileItemSql = "DELETE FROM file_items " + stream.str();

		// TODO remove trait_resources
		Session->once << deleteResourceSql;
		if (removeFileTag) {
			Session->once << deleteFileItemSql;
		}
	} catch (std::exception& e) {
		
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		e.what();
	}
}

void mvceditor::TagParserClass::Print() {
	UFILE *out = u_finit(stdout, NULL, NULL);
	std::vector<mvceditor::TagClass> matches = AllResources(*Session);
	u_fprintf(out, "resource count=%d\n", matches.size());
	for (std::vector<mvceditor::TagClass>::const_iterator it = matches.begin(); it != matches.end(); ++it) {
		switch (it->Type) {
			case mvceditor::TagClass::CLASS :
			case mvceditor::TagClass::DEFINE :
			case mvceditor::TagClass::FUNCTION :

				u_fprintf(out, "RESOURCE: Key=%.*S Identifier=%.*S ClassName=%.*S Namespace=%.*S Type=%d FileID=%d SourceID=%d\n",
					it->Key.length(), it->Key.getBuffer(),
					it->Identifier.length(), it->Identifier.getBuffer(),  
					it->ClassName.length(), it->ClassName.getBuffer(),
					it->NamespaceName.length(), it->NamespaceName.getBuffer(),
					it->Type, it->FileTagId, it->SourceId);
				break;
			case mvceditor::TagClass::CLASS_CONSTANT :
			case mvceditor::TagClass::MEMBER :
			case mvceditor::TagClass::METHOD :
				u_fprintf(out, "MEMBER: Key=%.*S Identifier=%.*S ClassName=%.*S Namespace=%.*S ReturnType=%.*S Type=%d FileID=%d SourceID=%d\n", 
					it->Key.length(), it->Key.getBuffer(),
					it->Identifier.length(), it->Identifier.getBuffer(),  
					it->ClassName.length(), it->ClassName.getBuffer(),
					it->NamespaceName.length(), it->NamespaceName.getBuffer(),
					it->ReturnType.length(), it->ReturnType.getBuffer(), it->Type, it->FileTagId, it->SourceId);
				break;
			case mvceditor::TagClass::NAMESPACE :
				u_fprintf(out, "NAMESPACE:Key=%.*S Identifier=%.*S ClassName=%.*S Namespace=%.*S  Type=%d FileID=%d SourceID=%d\n", 
					it->Key.length(), it->Key.getBuffer(),
					it->Identifier.length(), it->Identifier.getBuffer(),  
					it->ClassName.length(), it->ClassName.getBuffer(),
					it->NamespaceName.length(), it->NamespaceName.getBuffer(),
					it->Type, it->FileTagId, it->SourceId);
				break;
		}
	}
	for (std::map<UnicodeString, std::vector<mvceditor::TraitTagClass>, UnicodeStringComparatorClass>::const_iterator it = TraitCache.begin(); it != TraitCache.end(); ++it) {
		u_fprintf(out, "TRAITS USED BY: %.*S\n",
			it->first.length(), it->first.getBuffer());
		std::vector<mvceditor::TraitTagClass> traits =  it->second;
		for (size_t j = 0; j < traits.size(); ++j) {
			mvceditor::TraitTagClass trait = traits[j];
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


void mvceditor::TagParserClass::PersistFileTag(mvceditor::FileTagClass& fileTag) {
	if (!IsCacheInitialized) {
		return;
	}
	std::string fullPath = mvceditor::WxToChar(fileTag.FullPath);
	std::string name = mvceditor::WxToChar(fileTag.Name());
	std::tm tm;
	int isParsed = fileTag.IsParsed ? 1 : 0;
	int isNew = fileTag.IsNew ? 1 : 0;
	if (fileTag.DateTime.IsValid()) {
		wxDateTime::Tm wxTm = fileTag.DateTime.GetTm();
		tm.tm_hour = wxTm.hour;
		tm.tm_isdst = fileTag.DateTime.IsDST();
		tm.tm_mday = wxTm.mday;
		tm.tm_min = wxTm.min;
		tm.tm_mon = wxTm.mon;
		tm.tm_sec = wxTm.sec;
		tm.tm_wday = fileTag.DateTime.GetWeekDay();
		tm.tm_yday = fileTag.DateTime.GetDayOfYear();

		// tm holds number of years since 1900 (2012 = 112)
		tm.tm_year = wxTm.year - 1900;
	}
	try {
		soci::statement stmt = (Session->prepare <<
			"INSERT INTO file_items (file_item_id, source_id, full_path, name, last_modified, is_parsed, is_new) VALUES(NULL, ?, ?, ?, ?, ?, ?)",
			soci::use(CurrentSourceId), soci::use(fullPath), soci::use(name), soci::use(tm), soci::use(isParsed), soci::use(isNew)
		);
		stmt.execute(true);
		soci::sqlite3_statement_backend* backend = static_cast<soci::sqlite3_statement_backend*>(stmt.get_backend());
		fileTag.FileId = sqlite3_last_insert_rowid(backend->session_.conn_);
	} catch (std::exception& e) {
		
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		wxString msg = wxString::FromAscii(e.what());
		wxASSERT_MSG(false, msg);
	}
}

bool mvceditor::TagParserClass::FindFileTagByFullPathExact(const wxString& fullPath, mvceditor::FileTagClass& fileTag) {
	if (!IsCacheInitialized) {
		return false;
	}
	int fileTagId;
	int sourceId;
	std::tm lastModified;
	int isParsed;
	int isNew;
	bool foundFile = false;

	std::string query = mvceditor::WxToChar(fullPath);
	std::string sql = "SELECT file_item_id, source_id, last_modified, is_parsed, is_new FROM file_items WHERE full_path = ?";
	try {
		soci::statement stmt = (Session->prepare << sql, soci::use(query), 
			soci::into(fileTagId), soci::into(sourceId), soci::into(lastModified), soci::into(isParsed), soci::into(isNew)
		);
		foundFile = stmt.execute(true);
		if (foundFile) {
			fileTag.DateTime.Set(lastModified);
			fileTag.FileId = fileTagId;
			fileTag.SourceId = sourceId;
			fileTag.FullPath = fullPath;
			fileTag.IsNew = isNew != 0;
			fileTag.IsParsed = isParsed != 0;
		}
	} catch (std::exception& e) {
			
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		e.what();
	}
	return foundFile;
}

void mvceditor::TagParserClass::WipeAll() {
	NamespaceCache.clear();
	TraitCache.clear();

	if (IsCacheInitialized) {
		try {
			Session->once << "DELETE FROM file_items;";
			Session->once << "DELETE FROM resources;";
			Session->once << "DELETE FROM trait_resources;";
		} catch (std::exception& e) {
			
			// ATTN: at some point bubble these exceptions up?
			// to avoid unreferenced local variable warnings in MSVC
			e.what();
		}
	}
}

void mvceditor::TagParserClass::DeleteDirectories(const std::vector<wxFileName>& dirs) {
	NamespaceCache.clear();
	TraitCache.clear();

	if (IsCacheInitialized) {
		try {
			bool error = false;
			wxString errorMsg;
			std::vector<int> fileTagIds = mvceditor::FileTagIdsForDirs(*Session, dirs, error, errorMsg);
			wxASSERT_MSG(!error, errorMsg);
			if (!fileTagIds.empty()) {
				std::ostringstream stream;
				stream << "file_item_id IN(";
				for (size_t i = 0; i < fileTagIds.size(); ++i) {
					stream << fileTagIds[i];
					if (i < (fileTagIds.size() - 1)) {
						stream << ",";
					}
				}
				stream << ")";
				
				std::string sql = "DELETE FROM resources WHERE " + stream.str();
				Session->once << sql;

				sql = "DELETE FROM trait_resources WHERE " + stream.str();
				Session->once << sql;

				sql = "DELETE FROM file_items WHERE " + stream.str();
				Session->once << sql;
			}
		} catch (std::exception& e) {
			
			// ATTN: at some point bubble these exceptions up?
			// to avoid unreferenced local variable warnings in MSVC
			e.what();
		}
	}
}

void mvceditor::TagParserClass::DeleteFromFile(const wxString& fullPath) {
	mvceditor::FileTagClass fileTag;
	if (FindFileTagByFullPathExact(fullPath, fileTag)) {
		std::vector<int> fileTagIdsToRemove;
		fileTagIdsToRemove.push_back(fileTag.FileId);
		RemovePersistedResources(fileTagIdsToRemove, true);
	}
}

void mvceditor::TagParserClass::PersistResources(const mvceditor::TagClass& resource, int fileTagId) {
	if (!IsCacheInitialized) {
		return;
	}
	if (!InsertStmt) {
		return;
	}	
	try {
		FileTagId = fileTagId;

		Key = mvceditor::IcuToChar(resource.Key);
		Identifier = mvceditor::IcuToChar(resource.Identifier);
		ClassName = mvceditor::IcuToChar(resource.ClassName);
		Type = resource.Type;
		NamespaceName = mvceditor::IcuToChar(resource.NamespaceName);
		Signature = mvceditor::IcuToChar(resource.Signature);
		ReturnType = mvceditor::IcuToChar(resource.ReturnType);
		Comment = mvceditor::IcuToChar(resource.Comment);
		IsProtected = resource.IsProtected;
		IsPrivate = resource.IsPrivate;
		IsStatic = resource.IsStatic;
		IsDynamic = resource.IsDynamic;
		IsNative = resource.IsNative;
		InsertStmt->execute(true);

	} catch (std::exception& e) {
			
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		e.what();
	}
}

void mvceditor::TagParserClass::PersistTraits(
	const std::map<UnicodeString, std::vector<mvceditor::TraitTagClass>, UnicodeStringComparatorClass>& traitMap,
	int fileTagId) {
	if (!IsCacheInitialized) {
		return;
	}
	std::string sql;
	sql += "INSERT OR IGNORE INTO trait_resources(";
	sql += "key, file_item_id, source_id, class_name, namespace_name, trait_name, ";
	sql += "trait_namespace_name, aliases, instead_ofs) VALUES ("; 
	sql += "?, ?, ?, ?, ?, ?, ";
	sql += "?, ?, ?)";
	std::string key;
	std::string className;
	std::string namespaceName;
	std::string traitClassName;
	std::string traitNamespaceName;
	std::string aliases;
	std::string insteadOfs;

	try {
		soci::statement stmt = (Session->prepare << sql,
			soci::use(key), soci::use(fileTagId), soci::use(CurrentSourceId), soci::use(className), soci::use(namespaceName), soci::use(traitClassName),
			soci::use(traitNamespaceName), soci::use(aliases), soci::use(insteadOfs)
		);
		std::map<UnicodeString, std::vector<mvceditor::TraitTagClass>, UnicodeStringComparatorClass>::const_iterator it;
		for (it = traitMap.begin(); it != traitMap.end(); ++it) {
			std::vector<mvceditor::TraitTagClass>::const_iterator trait;
			for (trait = it->second.begin(); trait != it->second.end(); ++trait) {
				key = mvceditor::IcuToChar(trait->Key);
				className = mvceditor::IcuToChar(trait->ClassName);
				namespaceName = mvceditor::IcuToChar(trait->NamespaceName);
				traitClassName = mvceditor::IcuToChar(trait->TraitClassName);
				traitNamespaceName = mvceditor::IcuToChar(trait->TraitNamespaceName);
				aliases = "";
				for (std::vector<UnicodeString>::const_iterator alias = trait->Aliased.begin(); alias != trait->Aliased.end(); ++alias) {
					aliases += mvceditor::IcuToChar(*alias);
					aliases += ",";
				}
				if (!aliases.empty()) {
					aliases.erase(aliases.end() - 1);
				}
				insteadOfs = "";
				for (std::vector<UnicodeString>::const_iterator instead = trait->InsteadOfs.begin(); instead != trait->InsteadOfs.end(); ++instead) {
					insteadOfs += mvceditor::IcuToChar(*instead);
					insteadOfs += ",";
				}
				if (!insteadOfs.empty()) {
					insteadOfs.erase(insteadOfs.end() - 1);
				}
				stmt.execute(true);
			}
		}
	} catch (std::exception& e) {
			
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		wxString msg = mvceditor::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
}

std::vector<mvceditor::TraitTagClass> mvceditor::TagParserClass::FindTraitsByClassName(const std::vector<std::string>& keyStarts) {
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
	std::vector<mvceditor::TraitTagClass> matches;
	try {
		soci::statement stmt = (Session->prepare << sql,
			soci::into(key), soci::into(className), soci::into(namespaceName), soci::into(traitClassName),
			soci::into(traitNamespaceName), soci::into(aliases), soci::into(insteadOfs)
		);
		if (stmt.execute(true)) {
			do {
				mvceditor::TraitTagClass trait;
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
