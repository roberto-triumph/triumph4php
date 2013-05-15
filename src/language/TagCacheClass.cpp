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
#include <language/TagFinderList.h>
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

mvceditor::GlobalCacheCompleteEventClass::GlobalCacheCompleteEventClass(int eventId)
	: wxEvent(eventId, mvceditor::EVENT_GLOBAL_CACHE_COMPLETE) {

}

wxEvent* mvceditor::GlobalCacheCompleteEventClass::Clone() const {
	mvceditor::GlobalCacheCompleteEventClass* evt = new mvceditor::GlobalCacheCompleteEventClass(GetId());
	return evt;
}

const wxEventType mvceditor::EVENT_WORKING_CACHE_COMPLETE = wxNewEventType();
const wxEventType mvceditor::EVENT_GLOBAL_CACHE_COMPLETE = wxNewEventType();

mvceditor::GlobalCacheClass::GlobalCacheClass()
	: TagDbSession()
	, NativeDbSession()
	, DetectedTagDbSession()
	, WorkingTagDbSession()
	, TagParser() 
	, TagFinder()
	, NativeTagFinder()
	, DetectedTagFinder()
	, WorkingTagFinder()
	, IsNativeTagFinderInit(false)
	, IsTagFinderInit(false)
	, IsDetectedTagFinderInit(false) 
	, IsWorkingTagFinderInit(false) {

}

void mvceditor::GlobalCacheClass::InitGlobalTag(const wxFileName& tagDbFileName, 
									   const std::vector<wxString>& phpFileExtensions, 
									   const std::vector<wxString>& miscFileExtensions,
									   pelet::Versions version, int fileParsingBufferSize) {
	wxASSERT_MSG(!IsTagFinderInit, wxT("tag finder can only be initialized once"));
	TagParser.PhpFileExtensions = phpFileExtensions;
	TagParser.MiscFileExtensions = miscFileExtensions;
	IsTagFinderInit = Open(TagDbSession, tagDbFileName.GetFullPath());
	if (IsTagFinderInit) {
		TagParser.SetVersion(version);
		TagParser.Init(&TagDbSession, fileParsingBufferSize);
		TagFinder.Init(&TagDbSession);
	}
}

void mvceditor::GlobalCacheClass::InitDetectorTag(const wxFileName& detectorDbFileName) {
	wxASSERT_MSG(!IsDetectedTagFinderInit, wxT("tag finder can only be initialized once"));
	IsDetectedTagFinderInit = Open(DetectedTagDbSession, detectorDbFileName.GetFullPath());
	if (IsDetectedTagFinderInit) {
		DetectedTagFinder.Init(&DetectedTagDbSession);
	}
}

void mvceditor::GlobalCacheClass::InitNativeTag(const wxFileName& nativeDbFileName) {
	wxASSERT_MSG(!IsNativeTagFinderInit, wxT("tag finder can only be initialized once"));
	IsNativeTagFinderInit = Open(NativeDbSession, nativeDbFileName.GetFullPath());
	if (IsNativeTagFinderInit) {
		NativeTagFinder.Init(&NativeDbSession);
	}
}

void mvceditor::GlobalCacheClass::InitWorkingTag(const wxFileName& workingTagDbFileName) {
	wxASSERT_MSG(!IsWorkingTagFinderInit, wxT("tag finder can only be initialized once"));
	IsWorkingTagFinderInit = Open(WorkingTagDbSession, workingTagDbFileName.GetFullPath());
	if (IsWorkingTagFinderInit) {
		WorkingTagFinder.Init(&WorkingTagDbSession);
	}
}


bool mvceditor::GlobalCacheClass::Open(soci::session& session, const wxString& dbName) {
	bool ret = false;
	try {
		std::string stdDbName = mvceditor::WxToChar(dbName);
		
		// we should be able to open this since it has been created by
		// the TagCacheDbVersionActionClass
		session.open(*soci::factory_sqlite3(), stdDbName);

		// set a busy handler so that if we attempt to query while the file is locked, we 
		// sleep for a bit then try again
		mvceditor::SqliteSetBusyTimeout(session, 100);
		ret = true;
	} catch(std::exception const& e) {
		session.close();
		wxString msg = mvceditor::CharToWx(e.what());
		wxASSERT_MSG(false, msg);
	}
	return ret;
}

void mvceditor::GlobalCacheClass::Walk(mvceditor::DirectorySearchClass& search) {
	search.Walk(TagParser);
}

mvceditor::WorkingCacheClass::WorkingCacheClass()
	: SymbolTable()
	, FileName() 
	, IsNew(true)
	, Session()
	, TagParser()
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

void mvceditor::WorkingCacheClass::InitWorkingTag(const wxFileName& workingTagDbFileName) {
	try {
		Session.open(*soci::factory_sqlite3(), mvceditor::WxToChar(workingTagDbFileName.GetFullPath()));
	} catch(std::exception const& e) {
		Session.close();
		wxString msg = mvceditor::CharToWx(e.what());
		wxASSERT_MSG(false, msg);
	}
}

void mvceditor::WorkingCacheClass::Init(const wxString& fileName, 
										const wxString& fileIdentifier, bool isNew, pelet::Versions version, bool createSymbols) {
	FileName = fileName;
	FileIdentifier = fileIdentifier;
	IsNew = isNew;
	
	SymbolTable.SetVersion(version);
	Parser.SetVersion(version);
	
	TagParser.Init(&Session);
	TagParser.SetVersion(version);
	if (createSymbols) {

		// ATTN: not using the configured php file filters, file is assume to be a PHP file
		TagParser.PhpFileExtensions.push_back(wxT("*.*"));
		TagParser.BeginSearch();
		TagParser.Walk(fileName);
		TagParser.EndSearch();
		SymbolTable.CreateSymbolsFromFile(fileName);
	}
}

mvceditor::TagCacheClass::TagCacheClass()
	: GlobalCache(NULL)
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

void mvceditor::TagCacheClass::RegisterGlobal(mvceditor::GlobalCacheClass* cache) {
	GlobalCache = cache;
}

std::vector<mvceditor::TagClass> mvceditor::TagCacheClass::ExactTags(const UnicodeString& search) {
	std::vector<mvceditor::TagClass> matches;
	mvceditor::TagSearchClass tagSearch(search);
	

	// return all of the matches from all finders that were found by the Collect* call.
	// This is a bit tricky because we want to prioritize matches in opened files 
	// instead of the global finder, since the global finder will be outdated.
	std::vector<mvceditor::TagFinderClass*> finders = AllFinders();
	for (size_t i = 0; i < finders.size(); ++i) {
		mvceditor::TagFinderClass* tagFinder = finders[i];
		std::vector<mvceditor::TagClass> finderMatches = tagFinder->ExactTags(tagSearch);
		size_t count = finderMatches.size();
		for (size_t j = 0; j < count; ++j) {
			mvceditor::TagClass tag = finderMatches[j];
			if (!mvceditor::IsResourceDirty(&GlobalCache->WorkingTagFinder, tag, tagFinder)) {
				matches.push_back(tag);
			}
		}
	}
	std::sort(matches.begin(), matches.end());
	return matches;
}

std::vector<mvceditor::TagClass> mvceditor::TagCacheClass::NearMatchTags(const UnicodeString& search) {
	std::vector<mvceditor::TagClass> matches;
	mvceditor::TagSearchClass tagSearch(search);

	// return all of the matches from all finders that were found by the Collect* call.
	// This is a bit tricky because we want to prioritize matches in opened files 
	// instead of the global finder, since the global finder will be outdated.
	std::vector<mvceditor::TagFinderClass*> finders = AllFinders();
	for (size_t i = 0; i < finders.size(); ++i) {
		mvceditor::TagFinderClass* tagFinder = finders[i];
		std::vector<mvceditor::TagClass> finderMatches = tagFinder->NearMatchTags(tagSearch, true);
		size_t count = finderMatches.size();
		for (size_t j = 0; j < count; ++j) {
			mvceditor::TagClass tag = finderMatches[j];
			if (!mvceditor::IsResourceDirty(&GlobalCache->WorkingTagFinder, tag, tagFinder)) {
				matches.push_back(tag);
			}
		}
	}
	std::sort(matches.begin(), matches.end());
	return matches;
}


std::vector<mvceditor::TagClass> mvceditor::TagCacheClass::ExactClassOrFile(const UnicodeString& search) {
	std::vector<mvceditor::TagClass> matches;
	mvceditor::TagSearchClass tagSearch(search);

	// return all of the matches from all finders that were found by the Collect* call.
	// This is a bit tricky because we want to prioritize matches in opened files 
	// instead of the global finder, since the global finder will be outdated.
	std::vector<mvceditor::TagFinderClass*> finders = AllFinders();
	for (size_t i = 0; i < finders.size(); ++i) {
		mvceditor::TagFinderClass* tagFinder = finders[i];
		std::vector<mvceditor::TagClass> finderMatches = tagFinder->ExactClassOrFile(tagSearch);
		size_t count = finderMatches.size();
		for (size_t j = 0; j < count; ++j) {
			mvceditor::TagClass tag = finderMatches[j];
			if (!mvceditor::IsResourceDirty(&GlobalCache->WorkingTagFinder, tag, tagFinder)) {
				matches.push_back(tag);
			}
		}
	}
	std::sort(matches.begin(), matches.end());
	return matches;
}

std::vector<mvceditor::TagClass> mvceditor::TagCacheClass::NearMatchClassesOrFiles(const UnicodeString& search) {
	std::vector<mvceditor::TagClass> matches;
	mvceditor::TagSearchClass tagSearch(search);

	// return all of the matches from all finders that were found by the Collect* call.
	// This is a bit tricky because we want to prioritize matches in opened files 
	// instead of the global finder, since the global finder will be outdated.
	std::vector<mvceditor::TagFinderClass*> finders = AllFinders();
	for (size_t i = 0; i < finders.size(); ++i) {
		mvceditor::TagFinderClass* tagFinder = finders[i];
		std::vector<mvceditor::TagClass> finderMatches = tagFinder->NearMatchClassesOrFiles(tagSearch);
		size_t count = finderMatches.size();
		for (size_t j = 0; j < count; ++j) {
			mvceditor::TagClass tag = finderMatches[j];
			if (!mvceditor::IsResourceDirty(&GlobalCache->WorkingTagFinder, tag, tagFinder)) {
				matches.push_back(tag);
			}
		}
	}
	std::sort(matches.begin(), matches.end());
	return matches;
}

std::vector<mvceditor::TagFinderClass*> mvceditor::TagCacheClass::AllFinders() {
	std::vector<mvceditor::TagFinderClass*> allTagFinders;
	if (GlobalCache) {
		if (GlobalCache->IsNativeTagFinderInit) {
			allTagFinders.push_back(&GlobalCache->NativeTagFinder);
		}
		if (GlobalCache->IsTagFinderInit) {
			allTagFinders.push_back(&GlobalCache->TagFinder);
		}
		if (GlobalCache->IsDetectedTagFinderInit) {
			allTagFinders.push_back(&GlobalCache->DetectedTagFinder);
		}
		if (GlobalCache->IsWorkingTagFinderInit) {
			allTagFinders.push_back(&GlobalCache->WorkingTagFinder);
		}
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
		mvceditor::TagFinderClass* openedFinder = NULL;
		if (GlobalCache && GlobalCache->IsWorkingTagFinderInit) {
			openedFinder = &GlobalCache->WorkingTagFinder;
		}
		mvceditor::WorkingCacheClass* cache = itWorkingCache->second;
		cache->SymbolTable.ExpressionCompletionMatches(parsedExpression, expressionScope, allFinders, openedFinder, 
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
		mvceditor::TagFinderClass* openedFinder = NULL;
		if (GlobalCache && GlobalCache->IsWorkingTagFinderInit) {
			openedFinder = &GlobalCache->WorkingTagFinder;
		}
		mvceditor::WorkingCacheClass* cache = itWorkingCache->second;
		cache->SymbolTable.ResourceMatches(parsedExpression, expressionScope, allFinders, openedFinder, 
			matches, doDuckTyping, doFullyQualifiedMatchOnly, error);	
	}
	if (!foundSymbolTable) {
		error.Type = mvceditor::SymbolTableMatchErrorClass::UNREGISTERED_FILE;
	}
}

void mvceditor::TagCacheClass::Print() {
	UFILE* ufout = u_finit(stdout, NULL, NULL);
	u_fprintf(ufout, "Number of working caches: %d\n", WorkingCaches.size());
	u_fclose(ufout);
	std::map<wxString, mvceditor::WorkingCacheClass*>::const_iterator it = WorkingCaches.begin();
	for (; it != WorkingCaches.end(); ++it) {
		it->second->SymbolTable.Print();
	}
}

bool mvceditor::TagCacheClass::IsFileCacheEmpty() {

	// if at least one tag finder is not empty, return false
	if (GlobalCache && GlobalCache->IsNativeTagFinderInit && !GlobalCache->NativeTagFinder.IsFileCacheEmpty()) {
		return false;
	}
	if (GlobalCache && GlobalCache->IsTagFinderInit && !GlobalCache->TagFinder.IsFileCacheEmpty()) {
		return false;
	}
	if (GlobalCache && GlobalCache->IsDetectedTagFinderInit && !GlobalCache->DetectedTagFinder.IsFileCacheEmpty()) {
		return false;
	}
	if (GlobalCache && GlobalCache->IsWorkingTagFinderInit && !GlobalCache->WorkingTagFinder.IsFileCacheEmpty()) {
		return false;
	}
	return true;
}

bool mvceditor::TagCacheClass::IsResourceCacheEmpty() {

	// if at least one tag finder is not empty, return false
	if (GlobalCache && GlobalCache->IsNativeTagFinderInit && !GlobalCache->NativeTagFinder.IsResourceCacheEmpty()) {
		return false;
	}
	if (GlobalCache && GlobalCache->IsTagFinderInit && !GlobalCache->TagFinder.IsResourceCacheEmpty()) {
		return false;
	}
	if (GlobalCache && GlobalCache->IsDetectedTagFinderInit && !GlobalCache->DetectedTagFinder.IsResourceCacheEmpty()) {
		return false;
	}
	if (GlobalCache && GlobalCache->IsWorkingTagFinderInit && !GlobalCache->WorkingTagFinder.IsResourceCacheEmpty()) {
		return false;
	}
	return true;
}

void mvceditor::TagCacheClass::Clear() {

	// ATTN: do NOT wipe finders, Clear() is meant for memory
	// cleanup only
	if (GlobalCache) {
		delete GlobalCache;
		GlobalCache	= NULL;
	}

	std::map<wxString, mvceditor::WorkingCacheClass*>::iterator itWorking;
	for (itWorking = WorkingCaches.begin(); itWorking != WorkingCaches.end(); ++itWorking) {
		delete itWorking->second;
	}
	WorkingCaches.clear();
}

std::vector<mvceditor::TagClass> mvceditor::TagCacheClass::AllMemberTags(const UnicodeString& fullyQualifiedClassName) {
	std::vector<mvceditor::TagFinderClass*> allTagFinders = AllFinders();

	// add the double colon so that we search for all members
	mvceditor::TagSearchClass tagSearch(fullyQualifiedClassName + UNICODE_STRING_SIMPLE("::"));
	tagSearch.SetParentClasses(mvceditor::TagFinderListClassParents(fullyQualifiedClassName, tagSearch.GetMethodName(), allTagFinders));
	tagSearch.SetTraits(mvceditor::TagFinderListClassUsedTraits(fullyQualifiedClassName, tagSearch.GetParentClasses(), 
		tagSearch.GetMethodName(), allTagFinders));

	// return all of the matches from all finders that were found by the Collect* call.
	// This is a bit tricky because we want to prioritize matches in opened files 
	// instead of the global finder, since the global finder will be outdated.
	std::vector<mvceditor::TagClass> allMatches;
	if (GlobalCache && GlobalCache->IsWorkingTagFinderInit) {
		allMatches = GlobalCache->WorkingTagFinder.NearMatchTags(tagSearch);
	}
	if (allMatches.empty()) {	
		for (size_t j = 0; j < allTagFinders.size(); ++j) {
			mvceditor::TagFinderClass* tagFinder = allTagFinders[j];
			if (tagFinder != &GlobalCache->WorkingTagFinder) {
				std::vector<mvceditor::TagClass> finderMatches = tagFinder->NearMatchTags(tagSearch);
				allMatches.insert(allMatches.end(), finderMatches.begin(), finderMatches.end());
			}
		}
	}
	//std::sort(allMatches.begin(), allMatches.end());
	return allMatches;
}

std::vector<mvceditor::TagClass> mvceditor::TagCacheClass::AllTagsInFile(const wxString& fullPath) {
	std::vector<mvceditor::TagClass> allMatches;

	std::vector<mvceditor::TagFinderClass*> allTagFinders = AllFinders();
	for (size_t j = 0; j < allTagFinders.size(); ++j) {
		mvceditor::TagFinderClass* finder = allTagFinders[j];
		allMatches = finder->AllTagsInFile(fullPath);
		if (!allMatches.empty()) {

			// even if the file tag is is multiple finders, they should both be the 
			// same. stop when we find anything
			break;	
		}
	}

	// now for each class, collect all methods/properties for that class. do it here
	// since CollectAllTagsInFile only
	std::vector<mvceditor::TagClass> classTags;
	std::vector<mvceditor::TagClass>::iterator tag;
	for (tag = allMatches.begin(); tag != allMatches.end(); ++tag) {
		if (tag->Type == mvceditor::TagClass::CLASS) {
			UnicodeString qualifiedName = tag->NamespaceName;
			if (!qualifiedName.endsWith(UNICODE_STRING_SIMPLE("\\"))) {
				qualifiedName.append(UNICODE_STRING_SIMPLE("\\"));
			}
			qualifiedName.append(tag->ClassName);
			std::vector<mvceditor::TagClass> matches = AllMemberTags(qualifiedName);
			classTags.insert(classTags.end(), matches.begin(), matches.end());
		}
	}
	allMatches.insert(allMatches.end(), classTags.begin(), classTags.end());
	return allMatches;
}

std::vector<UnicodeString> mvceditor::TagCacheClass::ParentClassesAndTraits(const UnicodeString& className) {
	std::vector<UnicodeString> classParents = mvceditor::TagFinderListClassParents(className, UNICODE_STRING_SIMPLE(""), AllFinders());
	std::vector<UnicodeString> classTraits = mvceditor::TagFinderListClassUsedTraits(className, classParents, UNICODE_STRING_SIMPLE(""), AllFinders());
	
	std::vector<UnicodeString> all;
	all.insert(all.end(), classParents.begin(), classParents.end());
	all.insert(all.end(), classTraits.begin(), classTraits.end());

	std::sort(all.begin(), all.end());
	std::vector<UnicodeString>::iterator it = std::unique(all.begin(), all.end());
	all.erase(it, all.end());
	return all;
}
