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
#include <language/TagCacheClass.h>
#include <globals/Assets.h>
#include <globals/Sqlite.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

#include <algorithm>

mvceditor::WorkingCacheCompleteEventClass::WorkingCacheCompleteEventClass(int eventId, 
																		  const wxString& fileName,
																		  const wxString& fileIdentifier, mvceditor::WorkingCacheClass* cache)
	: wxEvent(eventId, mvceditor::EVENT_WORKING_CACHE_COMPLETE)
	, WorkingCache(cache) 
	, FileName(fileName.c_str())
	, FileIdentifier(fileIdentifier.c_str()) {

}

wxEvent* mvceditor::WorkingCacheCompleteEventClass::Clone() const {
	mvceditor::WorkingCacheCompleteEventClass* evt = new mvceditor::WorkingCacheCompleteEventClass(
			GetId(), FileName, FileIdentifier, WorkingCache);
	return evt;
}

wxString mvceditor::WorkingCacheCompleteEventClass::GetFileIdentifier() const {
	return FileIdentifier;
}

wxString mvceditor::WorkingCacheCompleteEventClass::GetFileName() const {
	return FileName;
}

mvceditor::GlobalCacheCompleteEventClass::GlobalCacheCompleteEventClass(int eventId, mvceditor::GlobalCacheClass* cache)
	: wxEvent(eventId, mvceditor::EVENT_GLOBAL_CACHE_COMPLETE)
	, GlobalCache(cache) {

}

wxEvent* mvceditor::GlobalCacheCompleteEventClass::Clone() const {
	mvceditor::GlobalCacheCompleteEventClass* evt = new mvceditor::GlobalCacheCompleteEventClass(GetId(), GlobalCache);
	return evt;
}

const wxEventType mvceditor::EVENT_WORKING_CACHE_COMPLETE = wxNewEventType();
const wxEventType mvceditor::EVENT_GLOBAL_CACHE_COMPLETE = wxNewEventType();

mvceditor::GlobalCacheClass::GlobalCacheClass()
	: Session()
	, TagParser() 
	, TagFinder(NULL)	
	, ResourceDbFileName() {

}

mvceditor::GlobalCacheClass::~GlobalCacheClass() {
	if (TagFinder) {
		delete TagFinder;
	}
}

void mvceditor::GlobalCacheClass::InitGlobalTag(const wxFileName& resourceDbFileName, 
									   const std::vector<wxString>& phpFileExtensions, 
									   const std::vector<wxString>& miscFileExtensions,
									   pelet::Versions version, int fileParsingBufferSize) {
	ResourceDbFileName = resourceDbFileName;
	TagParser.PhpFileExtensions = phpFileExtensions;
	TagParser.MiscFileExtensions = miscFileExtensions;
	OpenAndCreateTables(resourceDbFileName.GetFullPath(), mvceditor::ResourceSqlSchemaAsset());

	TagParser.SetVersion(version);
	TagParser.Init(&Session, fileParsingBufferSize);
	wxASSERT_MSG(TagFinder == NULL, wxT("GlobalCacheClass cannot be initialized more than once"));
	TagFinder = new mvceditor::ParsedTagFinderClass();
	TagFinder->Init(&Session);
}
void mvceditor::GlobalCacheClass::InitDetectorTag(const wxFileName& detectorDbFileName) {
	ResourceDbFileName = detectorDbFileName;
	OpenAndCreateTables(detectorDbFileName.GetFullPath(), mvceditor::DetectorSqlSchemaAsset());

	wxASSERT_MSG(TagFinder == NULL, wxT("GlobalCacheClass cannot be initialized more than once"));
	TagFinder = new mvceditor::DetectedTagFinderClass();
	TagFinder->Init(&Session);
}

void mvceditor::GlobalCacheClass::OpenAndCreateTables(const wxString& dbName, const wxFileName& schemaFileName) {
	try {
		std::string stdDbName = mvceditor::WxToChar(dbName);
		
		// we should be able to open this since it has been created by
		// the TagCacheDbVersionActionClass
		Session.open(*soci::factory_sqlite3(), stdDbName);
		/// TODO: remove since we are created 
		/*wxString error;
		if (!mvceditor::SqlScript(schemaFileName, Session, error)) {
			wxASSERT_MSG(false, error);
		}
		*/
	} catch(std::exception const& e) {
		Session.close();
		wxString msg = mvceditor::CharToWx(e.what());
		wxASSERT_MSG(false, msg);
	}
}

void mvceditor::GlobalCacheClass::Walk(mvceditor::DirectorySearchClass& search) {
	search.Walk(TagParser);
}

mvceditor::WorkingCacheClass::WorkingCacheClass()
	: TagParser()
	, TagFinder()
	, SymbolTable()
	, FileName() 
	, IsNew(true)
	, Parser() {

}

bool mvceditor::WorkingCacheClass::Update(const UnicodeString& code) {
	pelet::LintResultsClass results;

	// check for syntax so that only 'good' code modifies the cache
	// allow empty code to be valid; that way code completion works
	// on newly created files
	bool ret = false;
	if (code.isEmpty() || Parser.LintString(code, results)) {
		TagParser.BuildResourceCacheForFile(FileName, code, IsNew);
		SymbolTable.CreateSymbols(code);
		ret = true;
	}
	return ret;
}

void mvceditor::WorkingCacheClass::Init(const wxString& fileName, 
										const wxString& fileIdentifier, bool isNew, pelet::Versions version, bool createSymbols) {
	FileName = fileName;
	FileIdentifier = fileIdentifier;
	IsNew = isNew;
	OpenAndCreateTables();

	
	SymbolTable.SetVersion(version);
	Parser.SetVersion(version);
	
	TagParser.Init(&Session);
	TagParser.SetVersion(version);
	TagFinder.Init(&Session);
	if (createSymbols) {

		// ATTN: not using the configured php file filters, file is assume to be a PHP file
		TagParser.PhpFileExtensions.push_back(wxT("*.*"));
		TagParser.BeginSearch();
		TagParser.Walk(fileName);
		TagParser.EndSearch();
		SymbolTable.CreateSymbolsFromFile(fileName);
	}
}

void mvceditor::WorkingCacheClass::OpenAndCreateTables() {
	try {
		Session.open(*soci::factory_sqlite3(), ":memory:");
		wxString error;
		if (!mvceditor::SqliteSqlScript(mvceditor::ResourceSqlSchemaAsset(), Session, error)) {
			wxASSERT_MSG(false, error);
		}
	} catch(std::exception const& e) {
		Session.close();
		wxString msg = mvceditor::CharToWx(e.what());
		wxASSERT_MSG(false, msg);
	}
}

mvceditor::TagCacheClass::TagCacheClass()
	: GlobalCaches()
	, WorkingCaches() {
	
}

mvceditor::TagCacheClass::~TagCacheClass() {
	Clear();
 }

bool mvceditor::TagCacheClass::RegisterWorking(const wxString& fileName, mvceditor::WorkingCacheClass* cache) {
	bool ret = false;

	// careful to not overwrite the symbol table, tag finder pointers
	std::map<wxString, mvceditor::WorkingCacheClass*>::iterator it = WorkingCaches.find(fileName);
	if (it == WorkingCaches.end()) {
		WorkingCaches[fileName] = cache;
		ret = true;
	}
	return ret;
}

bool mvceditor::TagCacheClass::ReplaceWorking(const wxString& fileName, mvceditor::WorkingCacheClass* cache) {
	RemoveWorking(fileName);
	return RegisterWorking(fileName, cache);
}

void mvceditor::TagCacheClass::RemoveWorking(const wxString& fileName) {
	std::map<wxString, mvceditor::WorkingCacheClass*>::iterator it = WorkingCaches.find(fileName);
	if (it != WorkingCaches.end()) {
		delete it->second;
		WorkingCaches.erase(it);
	}
}

bool mvceditor::TagCacheClass::RegisterGlobal(mvceditor::GlobalCacheClass* cache) {
	wxASSERT_MSG(cache->ResourceDbFileName.IsOk(), _("Global cache cannot be an empty filename"));
	bool found = IsInitGlobal(cache->ResourceDbFileName);
	if (!found) {
		GlobalCaches.push_back(cache);
		return true;
	}
	return false;
}

bool mvceditor::TagCacheClass::IsInitGlobal(const wxFileName& resourceDbFileName) const {
	bool found = false;
	std::vector<mvceditor::GlobalCacheClass*>::const_iterator it;
	for (it = GlobalCaches.begin(); it != GlobalCaches.end(); ++it) {
		if ((*it)->ResourceDbFileName == resourceDbFileName) {
			found = true;
			break;
		}
	}
	return found;
}

void mvceditor::TagCacheClass::RemoveGlobal(const wxFileName& resourceDbFileName) {
	std::vector<mvceditor::GlobalCacheClass*>::iterator it;
	it = GlobalCaches.begin();
	while (it != GlobalCaches.end()) {
		if ((*it)->ResourceDbFileName == resourceDbFileName) {
			delete *it;
			it = GlobalCaches.erase(it);
		}
		else {
			it++;
		}
	}
}

std::vector<mvceditor::TagClass> mvceditor::TagCacheClass::AllNonNativeClassesGlobal() const {
	std::vector<mvceditor::TagClass> res; 
	std::vector<mvceditor::GlobalCacheClass*>::const_iterator it;
	for (it = GlobalCaches.begin(); it != GlobalCaches.end(); ++it) {
		std::vector<mvceditor::TagClass> finderResources = (*it)->TagFinder->AllNonNativeClasses();
		res.insert(res.end(), finderResources.begin(), finderResources.end());
	}
	return res;
}

std::vector<mvceditor::TagClass> mvceditor::TagCacheClass::CollectFullyQualifiedResourceFromAll(const UnicodeString& search) {
	std::vector<mvceditor::TagClass> matches;
	mvceditor::TagSearchClass tagSearch(search);

	// return all of the matches from all finders that were found by the Collect* call.
	// This is a bit tricky because we want to prioritize matches in opened files 
	// instead of the global finder, since the global finder will be outdated.
	std::vector<mvceditor::TagFinderClass*> finders = AllFinders();
	std::map<wxString, mvceditor::TagFinderClass*> openedFinders;
	std::map<wxString, mvceditor::WorkingCacheClass*>::const_iterator it;
	for (it = WorkingCaches.begin(); it != WorkingCaches.end(); ++it) {
		openedFinders[it->first] = &it->second->TagFinder;
	}
	for (size_t i = 0; i < finders.size(); ++i) {
		mvceditor::TagFinderClass* tagFinder = finders[i];
		std::vector<mvceditor::TagClass> finderMatches = tagFinder->CollectFullyQualifiedResource(tagSearch);
		size_t count = finderMatches.size();
		for (size_t j = 0; j < count; ++j) {
			mvceditor::TagClass tag = finderMatches[j];
			if (!mvceditor::IsResourceDirty(openedFinders, tag, tagFinder)) {
				matches.push_back(tag);
			}
		}
	}
	std::sort(matches.begin(), matches.end());
	return matches;
}

std::vector<mvceditor::TagClass> mvceditor::TagCacheClass::CollectNearMatchResourcesFromAll(const UnicodeString& search) {
	std::vector<mvceditor::TagClass> matches;
	mvceditor::TagSearchClass tagSearch(search);

	// return all of the matches from all finders that were found by the Collect* call.
	// This is a bit tricky because we want to prioritize matches in opened files 
	// instead of the global finder, since the global finder will be outdated.
	std::vector<mvceditor::TagFinderClass*> finders = AllFinders();
	std::map<wxString, mvceditor::TagFinderClass*> openedFinders;
	std::map<wxString, mvceditor::WorkingCacheClass*>::const_iterator it;
	for (it = WorkingCaches.begin(); it != WorkingCaches.end(); ++it) {
		if (it->second->IsNew) {

			// use the file identifier as a key to the map
			openedFinders[it->first] = &it->second->TagFinder;
		}
		else {

			// use the filename being edited as a key to the map
			// this is needed so that we can know to remove matches
			// that come from dirty files
			openedFinders[it->second->FileName] = &it->second->TagFinder;
		}
	}
	for (size_t i = 0; i < finders.size(); ++i) {
		mvceditor::TagFinderClass* tagFinder = finders[i];
		std::vector<mvceditor::TagClass> finderMatches = tagFinder->CollectNearMatchResources(tagSearch, true);
		size_t count = finderMatches.size();
		for (size_t j = 0; j < count; ++j) {
			mvceditor::TagClass tag = finderMatches[j];
			if (!mvceditor::IsResourceDirty(openedFinders, tag, tagFinder)) {
				matches.push_back(tag);
			}
		}
	}
	std::sort(matches.begin(), matches.end());
	return matches;
}

std::vector<mvceditor::TagFinderClass*> mvceditor::TagCacheClass::AllFinders() {
	std::vector<mvceditor::TagFinderClass*> allTagFinders;
	std::map<wxString, mvceditor::WorkingCacheClass*>::iterator it;
	for (it = WorkingCaches.begin(); it != WorkingCaches.end(); ++it) {
		allTagFinders.push_back(&it->second->TagFinder);
	}
	std::vector<mvceditor::GlobalCacheClass*>::iterator itGlobal;
	for (itGlobal = GlobalCaches.begin(); itGlobal != GlobalCaches.end(); ++itGlobal) {
		allTagFinders.push_back((*itGlobal)->TagFinder);
	}
	return allTagFinders;
}

void mvceditor::TagCacheClass::ExpressionCompletionMatches(const wxString& fileName, const pelet::ExpressionClass& parsedExpression, 
													const pelet::ScopeClass& expressionScope, 
													 std::vector<UnicodeString>& autoCompleteList,
													 std::vector<mvceditor::TagClass>& resourceMatches,
													 bool doDuckTyping,
													 mvceditor::SymbolTableMatchErrorClass& error) {
	std::map<wxString, mvceditor::WorkingCacheClass*>::const_iterator itWorkingCache = WorkingCaches.find(fileName);
	bool foundSymbolTable = false;
	if (itWorkingCache != WorkingCaches.end()) {
		foundSymbolTable = true;
		std::vector<mvceditor::TagFinderClass*> allFinders = AllFinders();
		std::map<wxString, mvceditor::TagFinderClass*> openedFinders;
		std::map<wxString, mvceditor::WorkingCacheClass*>::const_iterator it;
		for (it = WorkingCaches.begin(); it != WorkingCaches.end(); ++it) {
			if (it->second->IsNew) {

				// use the file identifier as a key to the map
				openedFinders[it->first] = &it->second->TagFinder;
			}
			else {
				
				// use the filename being edited as a key to the map
				// this is needed so that we can know to remove matches
				// that come from dirty files
				openedFinders[it->second->FileName] = &it->second->TagFinder;
			}
		}
		mvceditor::WorkingCacheClass* cache = itWorkingCache->second;
		cache->SymbolTable.ExpressionCompletionMatches(parsedExpression, expressionScope, allFinders, openedFinders, 
			autoCompleteList, resourceMatches, doDuckTyping, error);
	
	}
	if (!foundSymbolTable) {
		error.Type = mvceditor::SymbolTableMatchErrorClass::UNREGISTERED_FILE;
	}
}

void mvceditor::TagCacheClass::ResourceMatches(const wxString& fileName, const pelet::ExpressionClass& parsedExpression, 
													const pelet::ScopeClass& expressionScope, 
													std::vector<mvceditor::TagClass>& matches,
													bool doDuckTyping, bool doFullyQualifiedMatchOnly,
													mvceditor::SymbolTableMatchErrorClass& error) {
	std::map<wxString, mvceditor::WorkingCacheClass*>::const_iterator itWorkingCache = WorkingCaches.find(fileName);
	bool foundSymbolTable = false;
	if (itWorkingCache != WorkingCaches.end()) {
		foundSymbolTable = true;
		std::vector<mvceditor::TagFinderClass*> allFinders = AllFinders();
		std::map<wxString, mvceditor::TagFinderClass*> openedFinders;
		std::map<wxString, mvceditor::WorkingCacheClass*>::const_iterator it;
		for (it = WorkingCaches.begin(); it != WorkingCaches.end(); ++it) {
			if (it->second->IsNew) {

				// use the file identifier as a key to the map
				openedFinders[it->first] = &it->second->TagFinder;
			}
			else {
				
				// use the filename being edited as a key to the map
				// this is needed so that we can know to remove matches
				// that come from dirty files
				openedFinders[it->second->FileName] = &it->second->TagFinder;
			}
		}
		mvceditor::WorkingCacheClass* cache = itWorkingCache->second;
		cache->SymbolTable.ResourceMatches(parsedExpression, expressionScope, allFinders, openedFinders, 
			matches, doDuckTyping, doFullyQualifiedMatchOnly, error);	
	}
	if (!foundSymbolTable) {
		error.Type = mvceditor::SymbolTableMatchErrorClass::UNREGISTERED_FILE;
	}
}

void mvceditor::TagCacheClass::Print() {
	UFILE* ufout = u_finit(stdout, NULL, NULL);
	u_fprintf(ufout, "Number of global caches: %d\n", GlobalCaches.size());
	u_fprintf(ufout, "Number of working caches: %d\n", WorkingCaches.size());
	u_fclose(ufout);
	std::map<wxString, mvceditor::WorkingCacheClass*>::const_iterator it = WorkingCaches.begin();
	for (; it != WorkingCaches.end(); ++it) {
		it->second->SymbolTable.Print();
	}
}

bool mvceditor::TagCacheClass::IsFileCacheEmpty() {
	bool isEmpty = true;

	// if at least one tag finder is not empty, return false
	std::vector<mvceditor::GlobalCacheClass*>::iterator it;
	for (it = GlobalCaches.begin(); isEmpty && it != GlobalCaches.end(); ++it) {
		isEmpty = (*it)->TagFinder->IsFileCacheEmpty();
	}
	return isEmpty;
}

bool mvceditor::TagCacheClass::IsResourceCacheEmpty() {
	bool isEmpty = true;

	// if at least one tag finder is not empty, return false
	std::vector<mvceditor::GlobalCacheClass*>::iterator it;
	for (it = GlobalCaches.begin(); isEmpty && it != GlobalCaches.end(); ++it) {
		isEmpty = (*it)->TagFinder->IsResourceCacheEmpty();
	}
	return isEmpty;
}

void mvceditor::TagCacheClass::Clear() {

	// ATTN: do NOT wipe finders, Clear() is meant for memory
	// cleanup only
	std::vector<mvceditor::GlobalCacheClass*>::iterator it;
	for (it = GlobalCaches.begin(); it != GlobalCaches.end(); ++it) {
		delete *it;
	}

	std::map<wxString, mvceditor::WorkingCacheClass*>::iterator itWorking;
	for (itWorking = WorkingCaches.begin(); itWorking != WorkingCaches.end(); ++itWorking) {
		delete itWorking->second;
	}
	GlobalCaches.clear();
	WorkingCaches.clear();
}