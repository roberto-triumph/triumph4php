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
#include <language_php/TagParserClass.h>
#include <language_php/FileTags.h>
#include <search/FinderClass.h>
#include <globals/String.h>
#include <globals/Assets.h>
#include <globals/Sqlite.h>
#include <wx/filename.h>
#include <wx/ffile.h>
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

std::vector<t4p::PhpTagClass> AllResources(soci::session& session) {
	std::string sql;
	sql += "SELECT r.file_item_id, r.source_id, key, identifier, class_name, type, namespace_name, signature, return_type, comment, full_path, ";
	sql += "is_protected, is_private, is_static, is_dynamic, is_native, has_variable_args, is_new ";
	sql += "FROM resources r LEFT JOIN file_items f ON(r.file_item_id = f.file_item_id) ";
	sql += " ORDER BY key";

	std::vector<t4p::PhpTagClass> matches;
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
	int hasVariableArgs;
	int fileIsNew;
	soci::indicator fileTagIdIndicator,
		fullPathIndicator,
		fileIsNewIndicator;
	try {
		soci::statement stmt = (session.prepare << sql,
			soci::into(fileTagId, fileTagIdIndicator), soci::into(sourceId), soci::into(key), soci::into(identifier), soci::into(className),
			soci::into(type), soci::into(namespaceName), soci::into(signature),
			soci::into(returnType), soci::into(comment), soci::into(fullPath, fullPathIndicator), soci::into(isProtected), soci::into(isPrivate),
			soci::into(isStatic), soci::into(isDynamic), soci::into(isNative), soci::into(hasVariableArgs),
			soci::into(fileIsNew, fileIsNewIndicator));
		if (stmt.execute(true)) {
			do {
				t4p::PhpTagClass tag;
				if (soci::i_ok == fileTagIdIndicator) {
					tag.FileTagId = fileTagId;
				}
				tag.Key = t4p::CharToIcu(key.c_str());
				tag.Identifier = t4p::CharToIcu(identifier.c_str());
				tag.ClassName = t4p::CharToIcu(className.c_str());
				tag.Type = (t4p::PhpTagClass::Types)type;
				tag.NamespaceName = t4p::CharToIcu(namespaceName.c_str());
				tag.Signature = t4p::CharToIcu(signature.c_str());
				tag.ReturnType = t4p::CharToIcu(returnType.c_str());
				tag.Comment = t4p::CharToIcu(comment.c_str());
				if (soci::i_ok == fullPathIndicator) {
					tag.SetFullPath(t4p::CharToWx(fullPath.c_str()));
				}
				tag.IsProtected = isProtected != 0;
				tag.IsPrivate = isPrivate != 0;
				tag.IsStatic = isStatic != 0;
				tag.IsDynamic = isDynamic != 0;
				tag.IsNative = isNative != 0;
				tag.HasVariableArgs = hasVariableArgs != 0;
				if (soci::i_ok == fileIsNewIndicator) {
					tag.FileIsNew = fileIsNew != 0;
				} else {
					tag.FileIsNew = true;
				}
				tag.SourceId = sourceId;

				matches.push_back(tag);
			} while (stmt.fetch());
		}
	} catch (std::exception& e) {
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		wxString msg = t4p::CharToWx(e.what());
		wxASSERT_MSG(false, msg);
	}
	return matches;
}

t4p::TagParserClass::TagParserClass()
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

t4p::TagParserClass::~TagParserClass() {
	Close();
}

void t4p::TagParserClass::SetVersion(pelet::Versions version) {
	Parser.SetVersion(version);
}

void t4p::TagParserClass::Init(soci::session* session) {
	Session = session;
	IsCacheInitialized = true;
}

void t4p::TagParserClass::Close() {
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

void t4p::TagParserClass::BeginSearch(const wxString& fullPath) {
	// get (or create) the source ID
	try {
		CurrentSourceId = PersistSource(fullPath);
		BeginTransaction();
	} catch (std::exception& e) {
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		e.what();
	}
}

int t4p::TagParserClass::PersistSource(const wxString& sourceDir) {
	// make sure we always insert with the trailing directory separator
	// to be consistent
	wxFileName dir;
	dir.AssignDir(sourceDir);
	int sourceId = 0;
	try {
		std::string stdFullPath = t4p::WxToChar(dir.GetPathWithSep());
		soci::statement stmt = (Session->prepare <<
			"SELECT source_id FROM sources WHERE directory = ?",
			soci::into(sourceId), soci::use(stdFullPath));
		if (!stmt.execute(true)) {
			// didn't find the source, create a new row
			soci::statement stmt = (Session->prepare <<
				"INSERT INTO sources(directory) VALUES(?)",
				soci::use(stdFullPath));
			stmt.execute(true);
			sourceId = t4p::SqliteInsertId(stmt);
		}
	}
	catch (std::exception& e) {
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		wxASSERT_MSG(false, e.what());
	}
	return sourceId;
}

void t4p::TagParserClass::BeginTransaction() {
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
		sql += "is_static, is_dynamic, is_native, has_variable_args";
		sql += ") VALUES(";
		sql += "?, ?, ?, ?, ?, ";
		sql += "?, ?, ?, ";
		sql += "?, ?, ?, ?, ";
		sql += "?, ?, ?, ?";
		sql += ");";

		wxASSERT_MSG(!InsertStmt, wxT("statement should be cleaned up"));
		InsertStmt = new soci::statement(*Session);
		*InsertStmt = (Session->prepare << sql,
			soci::use(FileTagId), soci::use(CurrentSourceId), soci::use(Key), soci::use(Identifier), soci::use(ClassName),
			soci::use(Type), soci::use(NamespaceName), soci::use(Signature),
			soci::use(ReturnType), soci::use(Comment), soci::use(IsProtected), soci::use(IsPrivate),
			soci::use(IsStatic), soci::use(IsDynamic), soci::use(IsNative), soci::use(HasVariableArgs));
	} catch (std::exception& e) {
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		wxString msg = t4p::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
		if (InsertStmt) {
			delete InsertStmt;
			InsertStmt = NULL;
		}
	}
}

void t4p::TagParserClass::EndSearch() {
	try {
		Transaction->commit();
	} catch (std::exception& e) {
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		wxString msg = t4p::CharToWx(e.what());
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

bool t4p::TagParserClass::Walk(const wxString& fileName) {
	bool matchedFilter = false;
	wxFileName file(fileName);
	for (size_t i = 0; i < PhpFileExtensions.size(); ++i) {
		wxString filter = PhpFileExtensions[i];
		if (!wxIsWild(filter)) {
			// exact match of the name portion of fileName
			matchedFilter = file.GetFullName().CmpNoCase(filter) == 0;
		} else {
			matchedFilter =  wxMatchWild(filter, fileName);
		}
		if (matchedFilter) {
			break;
		}
	}
	if (matchedFilter) {
		BuildResourceCache(fileName, true);
	} else {
		// check the misc file filters
		for (size_t i = 0; i < MiscFileExtensions.size(); ++i) {
			wxString filter = MiscFileExtensions[i];
			if (!wxIsWild(filter)) {
				// exact match of the name portion of fileName
				matchedFilter = file.GetFullName().CmpNoCase(filter) == 0;
			} else {
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

void t4p::TagParserClass::BuildResourceCacheForFile(const wxString& sourceDir, const wxString& fullPath, const UnicodeString& code, bool isNew) {
	BeginTransaction();
    CurrentSourceId = PersistSource(sourceDir);

	// remove all previous cached resources
	t4p::FileTagClass fileTag;
	bool foundFile = FindFileTagByFullPathExact(fullPath, fileTag);
	if (foundFile) {
		std::vector<int> fileTagIdsToRemove;
		fileTagIdsToRemove.push_back(fileTag.FileId);

		// dont remove file tag, since we set source ID for the file
		// we also want the new tags to have the file's proper source ID
		CurrentSourceId = fileTag.SourceId;
		RemovePersistedResources(fileTagIdsToRemove, false);
	} else {
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

void t4p::TagParserClass::BuildResourceCache(const wxString& fullPath, bool parseClasses) {
	wxFileName fileName(fullPath);
	wxDateTime fileLastModifiedDateTime = fileName.GetModificationTime();

	// if the file happens to be just deleted and we get a bad time, then
	// assume that it has been modified right now
	// we dont want to insert junk in the db
	if (!fileLastModifiedDateTime.IsValid()) {
		fileLastModifiedDateTime = wxDateTime::Now();
	}

	// have we looked at this file yet or is cache out of date? if not, then build the cache.
	bool cached = false;
	t4p::FileTagClass fileTag;
	bool foundFile = FindFileTagByFullPathExact(fullPath, fileTag);
	if (foundFile) {
		bool needsToBeParsed = fileTag.NeedsToBeParsed(fileLastModifiedDateTime);
		cached = !needsToBeParsed;
	} else {
		fileTag.MakeNew(fileName, fileLastModifiedDateTime, parseClasses);
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
				fileTag.MakeNew(fileName, fileLastModifiedDateTime, parseClasses);
				PersistFileTag(fileTag);
			}

			// for now silently ignore files with parser errors
			pelet::LintResultsClass lintResults;
			wxFFile file;
			if (file.Open(fullPath, wxT("rb"))) {
				CurrentFileTagId = fileTag.FileId;
				Parser.ScanFile(file.fp(), t4p::WxToIcu(fullPath), lintResults);

				PersistTraits(TraitCache, fileTag.FileId);
			}

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
					wxString msg = t4p::CharToWx(e.what());
					wxUnusedVar(msg);
					wxASSERT_MSG(false, msg);
				}
				delete Transaction;
				Transaction = new soci::transaction(*Session);
			}
		}
	}
}

void t4p::TagParserClass::ClassFound(const UnicodeString& namespaceName, const UnicodeString& className,
		const UnicodeString& signature,
		const UnicodeString& baseClassName,
		const UnicodeString& implementsList,
		const UnicodeString& comment, const int lineNumber) {
	t4p::PhpTagClass classItem;
	classItem.Identifier = className;
	classItem.ClassName = className;
	classItem.NamespaceName = namespaceName;
	classItem.Key = className;
	classItem.Type = t4p::PhpTagClass::CLASS;
	classItem.Signature = signature;
	classItem.ReturnType = UNICODE_STRING_SIMPLE("");
	classItem.Comment = comment;
	classItem.IsNative = false;
	PersistResources(classItem, CurrentFileTagId);

	if (IsNewNamespace(namespaceName)) {
		// a tag for the namespace itself
		t4p::PhpTagClass namespaceItem = t4p::PhpTagClass::MakeNamespace(namespaceName);
		PersistResources(namespaceItem, CurrentFileTagId);
	}

	classItem.Identifier = QualifyName(namespaceName, className);
	classItem.Key = QualifyName(namespaceName, className);
	PersistResources(classItem, CurrentFileTagId);
}

void t4p::TagParserClass::TraitAliasFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& traitUsedClassName,
												  const UnicodeString& traitMethodName, const UnicodeString& alias, pelet::TokenClass::TokenIds visibility) {
	// the trait has already been put in the cache; we just need to update it
	UnicodeString mapKey;
	mapKey += namespaceName;
	mapKey += UNICODE_STRING_SIMPLE("-");
	mapKey += className;
	mapKey += UNICODE_STRING_SIMPLE("-");
	mapKey += traitUsedClassName;

	// this code assumes that TraitUseFound() is called before TraitAliasFound()
	std::map<UnicodeString, std::vector<t4p::TraitTagClass>, UnicodeStringComparatorClass>::iterator it;
	it = TraitCache.find(mapKey);
	if (!it->second.empty()) {
		it->second.front().Aliased.push_back(alias);
		it->second.back().Aliased.push_back(alias);
	}
}

void t4p::TagParserClass::TraitInsteadOfFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& traitUsedClassName,
													   const UnicodeString& traitMethodName, const std::vector<UnicodeString>& insteadOfList) {
	// the trait has already been put in the cache; we just need to update it
	UnicodeString mapKey;

	mapKey += namespaceName;
	mapKey += UNICODE_STRING_SIMPLE("-");
	mapKey += className;
	mapKey += UNICODE_STRING_SIMPLE("-");
	mapKey += traitUsedClassName;

	// this code assumes that TraitUseFound() is called before TraitAliasFound()
	std::map<UnicodeString, std::vector<t4p::TraitTagClass>, UnicodeStringComparatorClass>::iterator it;
	it = TraitCache.find(mapKey);
	if (!it->second.empty()) {
		it->second.front().InsteadOfs.insert(it->second.front().InsteadOfs.end(), insteadOfList.begin(), insteadOfList.end());
		it->second.back().InsteadOfs.insert(it->second.back().InsteadOfs.end(), insteadOfList.begin(), insteadOfList.end());
	}
}

void t4p::TagParserClass::TraitUseFound(const UnicodeString& namespaceName, const UnicodeString& className,
												const UnicodeString& fullyQualifiedTraitName) {
	t4p::TraitTagClass newTraitTag;
	newTraitTag.ClassName = className;
	newTraitTag.NamespaceName = namespaceName;

	int32_t pos = fullyQualifiedTraitName.lastIndexOf(UNICODE_STRING_SIMPLE("\\"));
	if (pos > 0) {
		newTraitTag.TraitClassName.setTo(fullyQualifiedTraitName, 0, pos);
		newTraitTag.TraitNamespaceName.setTo(fullyQualifiedTraitName, pos + 1);
	} else if (0 == pos) {
		// root namespace
		newTraitTag.TraitClassName.setTo(fullyQualifiedTraitName, 1);
		newTraitTag.TraitNamespaceName.setTo(UNICODE_STRING_SIMPLE("\\"));
	} else {
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

void t4p::TagParserClass::DefineDeclarationFound(const UnicodeString& namespaceName, const UnicodeString& variableName,
		const UnicodeString& variableValue, const UnicodeString& comment, const int lineNumber) {
	t4p::PhpTagClass defineItem;
	defineItem.Identifier = variableName;
	defineItem.Key = variableName;
	defineItem.Type = t4p::PhpTagClass::DEFINE;
	defineItem.Signature = variableValue;
	defineItem.ReturnType = UNICODE_STRING_SIMPLE("");
	defineItem.Comment = comment;
	defineItem.IsNative = false;
	PersistResources(defineItem, CurrentFileTagId);

	defineItem.Identifier = QualifyName(namespaceName, variableName);
	defineItem.Key = QualifyName(namespaceName, variableName);
	PersistResources(defineItem, CurrentFileTagId);
}

void t4p::TagParserClass::MethodFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& methodName,
		const UnicodeString& signature, const UnicodeString& returnType, const UnicodeString& comment,
		pelet::TokenClass::TokenIds visibility, bool isStatic, const int lineNumber, bool hasVariableArguments) {
	t4p::PhpTagClass item;
	item.Identifier = methodName;
	item.ClassName = className;
	item.NamespaceName = namespaceName;
	item.Key = methodName;
	item.Type = t4p::PhpTagClass::METHOD;
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
	item.HasVariableArgs = hasVariableArguments;
	PersistResources(item, CurrentFileTagId);

	// insert a complete name so that we can quickly lookup all methods for a single class
	item.Key = className + UNICODE_STRING_SIMPLE("::") + methodName;
	PersistResources(item, CurrentFileTagId);

	// insert a fully qualified name and method so that we can quickly lookup all methods
	// for a namespaced class
	item.Key = QualifyName(namespaceName, className) + UNICODE_STRING_SIMPLE("::") + methodName;
	PersistResources(item, CurrentFileTagId);
}

void t4p::TagParserClass::PropertyFound(const UnicodeString& namespaceName, const UnicodeString& className, const UnicodeString& propertyName,
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
	t4p::PhpTagClass item;
	item.Identifier = filteredProperty;
	item.ClassName = className;
	item.NamespaceName = namespaceName;
	item.Key = filteredProperty;
	item.Type = isConst ? t4p::PhpTagClass::CLASS_CONSTANT : t4p::PhpTagClass::MEMBER;
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
	item.HasVariableArgs = false;
	PersistResources(item, CurrentFileTagId);

	// insert a complete name so that we can quickly lookup all methods for a single class
	item.Key = className + UNICODE_STRING_SIMPLE("::") + filteredProperty;
	PersistResources(item, CurrentFileTagId);

	// insert a fully qualified name and method so that we can quickly lookup all methods
	// for a namespaced class
	item.Key = QualifyName(namespaceName, className) + UNICODE_STRING_SIMPLE("::") + filteredProperty;
	PersistResources(item, CurrentFileTagId);
}

void t4p::TagParserClass::FunctionFound(const UnicodeString& namespaceName, const UnicodeString& functionName, const UnicodeString& signature,
		const UnicodeString& returnType, const UnicodeString& comment, const int lineNumber, bool hasVariableArguments) {
	t4p::PhpTagClass item;
	item.Identifier = functionName;
	item.NamespaceName = namespaceName;
	item.Key = functionName;
	item.Type = t4p::PhpTagClass::FUNCTION;
	item.Signature = signature;
	item.ReturnType = returnType;
	item.Comment = comment;
	item.IsNative = false;
	item.HasVariableArgs = hasVariableArguments;
	PersistResources(item, CurrentFileTagId);

	if (IsNewNamespace(namespaceName)) {
		t4p::PhpTagClass namespaceItem = t4p::PhpTagClass::MakeNamespace(namespaceName);
		PersistResources(namespaceItem, CurrentFileTagId);
	}

	// put in the namespace cache so that qualified name lookups work too
	item.Identifier = QualifyName(namespaceName, functionName);
	item.Key = QualifyName(namespaceName, functionName);
	PersistResources(item, CurrentFileTagId);
}

bool t4p::TagParserClass::IsNewNamespace(const UnicodeString& namespaceName) {
	std::string sql = "SELECT COUNT(*) FROM resources WHERE key = ? AND type = ?";
	std::string nm = t4p::IcuToChar(namespaceName);
	int type = t4p::PhpTagClass::NAMESPACE;
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

void t4p::TagParserClass::RemovePersistedResources(const std::vector<int>& fileTagIds, bool removeFileTag) {
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

		// TODO(roberto): remove trait_resources
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

void t4p::TagParserClass::Print() {
	UFILE *out = u_finit(stdout, NULL, NULL);
	std::vector<t4p::PhpTagClass> matches = AllResources(*Session);
	u_fprintf(out, "resource count=%d\n", matches.size());
	for (std::vector<t4p::PhpTagClass>::const_iterator it = matches.begin(); it != matches.end(); ++it) {
		switch (it->Type) {
			case t4p::PhpTagClass::CLASS :
			case t4p::PhpTagClass::DEFINE :
			case t4p::PhpTagClass::FUNCTION :

				u_fprintf(out, "RESOURCE: Key=%.*S Identifier=%.*S ClassName=%.*S Namespace=%.*S Type=%d FileID=%d SourceID=%d\n",
					it->Key.length(), it->Key.getBuffer(),
					it->Identifier.length(), it->Identifier.getBuffer(),
					it->ClassName.length(), it->ClassName.getBuffer(),
					it->NamespaceName.length(), it->NamespaceName.getBuffer(),
					it->Type, it->FileTagId, it->SourceId);
				break;
			case t4p::PhpTagClass::CLASS_CONSTANT :
			case t4p::PhpTagClass::MEMBER :
			case t4p::PhpTagClass::METHOD :
				u_fprintf(out, "MEMBER: Key=%.*S Identifier=%.*S ClassName=%.*S Namespace=%.*S ReturnType=%.*S Type=%d FileID=%d SourceID=%d\n",
					it->Key.length(), it->Key.getBuffer(),
					it->Identifier.length(), it->Identifier.getBuffer(),
					it->ClassName.length(), it->ClassName.getBuffer(),
					it->NamespaceName.length(), it->NamespaceName.getBuffer(),
					it->ReturnType.length(), it->ReturnType.getBuffer(), it->Type, it->FileTagId, it->SourceId);
				break;
			case t4p::PhpTagClass::NAMESPACE :
				u_fprintf(out, "NAMESPACE:Key=%.*S Identifier=%.*S ClassName=%.*S Namespace=%.*S  Type=%d FileID=%d SourceID=%d\n",
					it->Key.length(), it->Key.getBuffer(),
					it->Identifier.length(), it->Identifier.getBuffer(),
					it->ClassName.length(), it->ClassName.getBuffer(),
					it->NamespaceName.length(), it->NamespaceName.getBuffer(),
					it->Type, it->FileTagId, it->SourceId);
				break;
		}
	}
	for (std::map<UnicodeString, std::vector<t4p::TraitTagClass>, UnicodeStringComparatorClass>::const_iterator it = TraitCache.begin(); it != TraitCache.end(); ++it) {
		u_fprintf(out, "TRAITS USED BY: %.*S\n",
			it->first.length(), it->first.getBuffer());
		std::vector<t4p::TraitTagClass> traits =  it->second;
		for (size_t j = 0; j < traits.size(); ++j) {
			t4p::TraitTagClass trait = traits[j];
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


void t4p::TagParserClass::PersistFileTag(t4p::FileTagClass& fileTag) {
	if (!IsCacheInitialized) {
		return;
	}
	fileTag.SourceId = CurrentSourceId;
	t4p::FileTagPersist(*Session, fileTag);
}

bool t4p::TagParserClass::FindFileTagByFullPathExact(const wxString& fullPath, t4p::FileTagClass& fileTag) {
	if (!IsCacheInitialized) {
		return false;
	}
	int fileTagId;
	int sourceId;
	std::tm lastModified;
	int isParsed;
	int isNew;
	bool foundFile = false;

	std::string query = t4p::WxToChar(fullPath);
	std::string sql = "SELECT file_item_id, source_id, last_modified, is_parsed, is_new FROM file_items WHERE full_path = ?";
	try {
		soci::statement stmt = (Session->prepare << sql, soci::use(query),
			soci::into(fileTagId), soci::into(sourceId), soci::into(lastModified), soci::into(isParsed), soci::into(isNew));
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

void t4p::TagParserClass::WipeAll() {
	NamespaceCache.clear();
	TraitCache.clear();

	if (IsCacheInitialized) {
		try {
			Session->once << "DELETE FROM file_items;";
			Session->once << "DELETE FROM resources;";
			Session->once << "DELETE FROM trait_resources;";
			Session->once << "DELETE FROM sources;";
		} catch (std::exception& e) {
			// ATTN: at some point bubble these exceptions up?
			// to avoid unreferenced local variable warnings in MSVC
			e.what();
		}
	}
}

void t4p::TagParserClass::DeleteSource(const wxFileName& sourceDir) {
	NamespaceCache.clear();
	TraitCache.clear();

	if (IsCacheInitialized) {
		try {
			wxString errorMsg;

			// get the source ID to be deleted
			int sourceId = 0;
			std::string stdSourceDir = t4p::WxToChar(sourceDir.GetPathWithSep());

			std::string sql = "SELECT source_id FROM sources WHERE directory = ?";
			soci::statement stmt = (Session->prepare << sql, soci::into(sourceId), soci::use(stdSourceDir));

			if (stmt.execute(true)) {
				sql = "DELETE FROM resources WHERE source_id = ?";
				Session->once << sql, soci::use(sourceId);

				sql = "DELETE FROM trait_resources WHERE source_id = ?";
				Session->once << sql, soci::use(sourceId);

				sql = "DELETE FROM file_items WHERE source_id = ?";
				Session->once << sql, soci::use(sourceId);

				sql = "DELETE FROM sources WHERE source_id = ?";
				Session->once << sql, soci::use(sourceId);
			}
		} catch (std::exception& e) {
			// ATTN: at some point bubble these exceptions up?
			// to avoid unreferenced local variable warnings in MSVC
			e.what();
		}
	}
}

void t4p::TagParserClass::DeleteDirectories(const std::vector<wxFileName>& dirs) {
	NamespaceCache.clear();
	TraitCache.clear();

	if (IsCacheInitialized) {
		try {
			bool error = false;
			wxString errorMsg;
			std::vector<int> fileTagIds = t4p::FileTagIdsForDirs(*Session, dirs, error, errorMsg);
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

void t4p::TagParserClass::DeleteFromFile(const wxString& fullPath) {
	t4p::FileTagClass fileTag;
	if (FindFileTagByFullPathExact(fullPath, fileTag)) {
		std::vector<int> fileTagIdsToRemove;
		fileTagIdsToRemove.push_back(fileTag.FileId);
		RemovePersistedResources(fileTagIdsToRemove, true);
	}
}

void t4p::TagParserClass::PersistResources(const t4p::PhpTagClass& resource, int fileTagId) {
	if (!IsCacheInitialized) {
		return;
	}
	if (!InsertStmt) {
		return;
	}
	try {
		FileTagId = fileTagId;

		Key = t4p::IcuToChar(resource.Key);
		Identifier = t4p::IcuToChar(resource.Identifier);
		ClassName = t4p::IcuToChar(resource.ClassName);
		Type = resource.Type;
		NamespaceName = t4p::IcuToChar(resource.NamespaceName);
		Signature = t4p::IcuToChar(resource.Signature);
		ReturnType = t4p::IcuToChar(resource.ReturnType);
		Comment = t4p::IcuToChar(resource.Comment);
		IsProtected = resource.IsProtected;
		IsPrivate = resource.IsPrivate;
		IsStatic = resource.IsStatic;
		IsDynamic = resource.IsDynamic;
		IsNative = resource.IsNative;
		HasVariableArgs = resource.HasVariableArgs;
		InsertStmt->execute(true);
	} catch (std::exception& e) {
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		e.what();
	}
}

void t4p::TagParserClass::PersistTraits(
	const std::map<UnicodeString, std::vector<t4p::TraitTagClass>, UnicodeStringComparatorClass>& traitMap,
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
			soci::use(traitNamespaceName), soci::use(aliases), soci::use(insteadOfs));
		std::map<UnicodeString, std::vector<t4p::TraitTagClass>, UnicodeStringComparatorClass>::const_iterator it;
		for (it = traitMap.begin(); it != traitMap.end(); ++it) {
			std::vector<t4p::TraitTagClass>::const_iterator trait;
			for (trait = it->second.begin(); trait != it->second.end(); ++trait) {
				key = t4p::IcuToChar(trait->Key);
				className = t4p::IcuToChar(trait->ClassName);
				namespaceName = t4p::IcuToChar(trait->NamespaceName);
				traitClassName = t4p::IcuToChar(trait->TraitClassName);
				traitNamespaceName = t4p::IcuToChar(trait->TraitNamespaceName);
				aliases = "";
				for (std::vector<UnicodeString>::const_iterator alias = trait->Aliased.begin(); alias != trait->Aliased.end(); ++alias) {
					aliases += t4p::IcuToChar(*alias);
					aliases += ",";
				}
				if (!aliases.empty()) {
					aliases.erase(aliases.end() - 1);
				}
				insteadOfs = "";
				for (std::vector<UnicodeString>::const_iterator instead = trait->InsteadOfs.begin(); instead != trait->InsteadOfs.end(); ++instead) {
					insteadOfs += t4p::IcuToChar(*instead);
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
		wxString msg = t4p::CharToWx(e.what());
		wxUnusedVar(msg);
		wxASSERT_MSG(false, msg);
	}
}

std::vector<t4p::TraitTagClass> t4p::TagParserClass::FindTraitsByClassName(const std::vector<std::string>& keyStarts) {
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
	std::vector<t4p::TraitTagClass> matches;
	try {
		soci::statement stmt = (Session->prepare << sql,
			soci::into(key), soci::into(className), soci::into(namespaceName), soci::into(traitClassName),
			soci::into(traitNamespaceName), soci::into(aliases), soci::into(insteadOfs));
		if (stmt.execute(true)) {
			do {
				t4p::TraitTagClass trait;
				trait.Key = t4p::CharToIcu(key.c_str());
				trait.ClassName = t4p::CharToIcu(className.c_str());
				trait.NamespaceName = t4p::CharToIcu(namespaceName.c_str());
				trait.TraitClassName = t4p::CharToIcu(traitClassName.c_str());
				trait.TraitNamespaceName = t4p::CharToIcu(traitNamespaceName.c_str());

				size_t start = 0;
				size_t found = aliases.find_first_of(",");
				while (found != std::string::npos) {
					trait.Aliased.push_back(t4p::CharToIcu(aliases.substr(start, found).c_str()));
					start = found++;
				}
				if (!aliases.empty()) {
					trait.Aliased.push_back(t4p::CharToIcu(aliases.substr(start, found).c_str()));
				}

				start = 0;
				found = insteadOfs.find_first_of(",");
				while (found != std::string::npos) {
					trait.InsteadOfs.push_back(t4p::CharToIcu(insteadOfs.substr(start, found).c_str()));
					start = found++;
				}
				if (!insteadOfs.empty()) {
					trait.InsteadOfs.push_back(t4p::CharToIcu(insteadOfs.substr(start, found).c_str()));
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

void t4p::TagParserClass::RenameFile(const wxFileName& oldFile, const wxFileName& newFile) {
	try {
		std::string stdOldPath = t4p::WxToChar(oldFile.GetFullPath());
		std::string stdNewPath = t4p::WxToChar(newFile.GetFullPath());
		std::string stdNewName = t4p::WxToChar(newFile.GetFullName());
		std::string sql = "UPDATE file_items SET full_path = ?, name = ? WHERE full_path = ?";
		soci::statement stmt = (Session->prepare << sql,
			soci::use(stdNewPath), soci::use(stdNewName), soci::use(stdOldPath));
		stmt.execute(true);
	} catch (std::exception& e) {
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		wxASSERT_MSG(false, wxString(e.what()));
	}
}

void t4p::TagParserClass::RenameDir(const wxFileName& oldDir, const wxFileName& newDir) {
	try {
		std::string stdOldPath = t4p::WxToChar(oldDir.GetPathWithSep());
		std::string stdOldPathLike = stdOldPath + "%";
		std::string stdNewPath = t4p::WxToChar(newDir.GetPathWithSep());
		std::string sql = "UPDATE file_items SET full_path = REPLACE(full_path, ?, ?) WHERE full_path LIKE ?";
		soci::statement stmt = (Session->prepare << sql,
			soci::use(stdOldPath), soci::use(stdNewPath), soci::use(stdOldPathLike));
		stmt.execute(true);
	} catch (std::exception& e) {
		// ATTN: at some point bubble these exceptions up?
		// to avoid unreferenced local variable warnings in MSVC
		wxASSERT_MSG(false, wxString(e.what()));
	}
}
