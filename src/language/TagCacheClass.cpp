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
#include <language/DetectedTagFinderResultClass.h>
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

mvceditor::TagFinderListCompleteEventClass::TagFinderListCompleteEventClass(int eventId)
	: wxEvent(eventId, mvceditor::EVENT_TAG_FINDER_LIST_COMPLETE) {

}

wxEvent* mvceditor::TagFinderListCompleteEventClass::Clone() const {
	mvceditor::TagFinderListCompleteEventClass* evt = new mvceditor::TagFinderListCompleteEventClass(GetId());
	return evt;
}

const wxEventType mvceditor::EVENT_WORKING_CACHE_COMPLETE = wxNewEventType();
const wxEventType mvceditor::EVENT_TAG_FINDER_LIST_COMPLETE = wxNewEventType();

mvceditor::WorkingCacheClass::WorkingCacheClass()
	: SymbolTable()
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
	
	SymbolTable.SetVersion(version);

	TagParser.Init(&Session);
	TagParser.SetVersion(version);
	if (createSymbols) {
		SymbolTable.CreateSymbolsFromFile(fileName);
	}
}

mvceditor::TagCacheClass::TagCacheClass()
	: TagFinderList(NULL)
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

void mvceditor::TagCacheClass::RegisterGlobal(mvceditor::TagFinderListClass* cache) {
	TagFinderList = cache;
}

mvceditor::TagResultClass* mvceditor::TagCacheClass::ExactTags(const UnicodeString& search, const std::vector<wxFileName>& sourceDirs) {
	mvceditor::TagSearchClass tagSearch(search);
	tagSearch.SetSourceDirs(sourceDirs);
	mvceditor::TagResultClass* result = tagSearch.CreateExactResults();
	TagFinderList->TagFinder.Exec(result);
	return result;
}

mvceditor::TagResultClass* mvceditor::TagCacheClass::ExactNativeTags(const UnicodeString& search) {
	mvceditor::TagSearchClass tagSearch(search);
	mvceditor::TagResultClass* result = tagSearch.CreateExactResults();
	TagFinderList->NativeTagFinder.Exec(result);
	return result;
}


mvceditor::DetectedTagExactMemberResultClass* mvceditor::TagCacheClass::ExactDetectedTags(const UnicodeString& search, const std::vector<wxFileName>& sourceDirs) {
	mvceditor::TagSearchClass tagSearch(search);
	mvceditor::DetectedTagExactMemberResultClass* result = new mvceditor::DetectedTagExactMemberResultClass();
	std::vector<UnicodeString> classNames;
	classNames.push_back(tagSearch.GetClassName());
	result->Set(classNames, tagSearch.GetMethodName());
	TagFinderList->DetectedTagFinder.Exec(result);
	return result;
}

mvceditor::TagResultClass* mvceditor::TagCacheClass::NearMatchTags(const UnicodeString& search, const std::vector<wxFileName>& sourceDirs) {
	mvceditor::TagSearchClass tagSearch(search);
	tagSearch.SetSourceDirs(sourceDirs);

	mvceditor::TagResultClass* result = tagSearch.CreateNearMatchResults();
	TagFinderList->TagFinder.Exec(result);
	return result;
}

mvceditor::DetectedTagNearMatchMemberResultClass* mvceditor::TagCacheClass::NearMatchDetectedTags(const UnicodeString& search, const std::vector<wxFileName>& sourceDirs) {
	mvceditor::TagSearchClass tagSearch(search);
	tagSearch.SetSourceDirs(sourceDirs);

	mvceditor::DetectedTagNearMatchMemberResultClass* result = new mvceditor::DetectedTagNearMatchMemberResultClass();
	std::vector<UnicodeString> classNames;
	classNames.push_back(tagSearch.GetClassName());
	result->Set(classNames, tagSearch.GetMethodName());
	TagFinderList->DetectedTagFinder.Exec(result);
	return result;
}

mvceditor::TagResultClass* mvceditor::TagCacheClass::NearMatchNativeTags(const UnicodeString& search) {
	mvceditor::TagSearchClass tagSearch(search);

	mvceditor::TagResultClass* result = tagSearch.CreateNearMatchResults();
	TagFinderList->NativeTagFinder.Exec(result);
	return result;
}


std::vector<mvceditor::TagClass> mvceditor::TagCacheClass::ExactClassOrFile(const UnicodeString& search) {
	std::vector<mvceditor::TagClass> matches;
	mvceditor::TagSearchClass tagSearch(search);

	// return all of the matches from all finders that were found by the Collect* call.
	// This is a bit tricky because we want to prioritize matches in opened files 
	// instead of the global finder, since the global finder will be outdated.
	std::vector<mvceditor::ParsedTagFinderClass*> finders = AllFinders();
	for (size_t i = 0; i < finders.size(); ++i) {
		mvceditor::ParsedTagFinderClass* tagFinder = finders[i];
		std::vector<mvceditor::TagClass> finderMatches = tagFinder->ExactClassOrFile(tagSearch);
		size_t count = finderMatches.size();
		for (size_t j = 0; j < count; ++j) {
			mvceditor::TagClass tag = finderMatches[j];
			matches.push_back(tag);
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
	std::vector<mvceditor::ParsedTagFinderClass*> finders = AllFinders();
	for (size_t i = 0; i < finders.size(); ++i) {
		mvceditor::ParsedTagFinderClass* tagFinder = finders[i];
		std::vector<mvceditor::TagClass> finderMatches = tagFinder->NearMatchClassesOrFiles(tagSearch);
		size_t count = finderMatches.size();
		for (size_t j = 0; j < count; ++j) {
			mvceditor::TagClass tag = finderMatches[j];
			matches.push_back(tag);
		}
	}
	std::sort(matches.begin(), matches.end());
	return matches;
}

std::vector<mvceditor::ParsedTagFinderClass*> mvceditor::TagCacheClass::AllFinders() {
	std::vector<mvceditor::ParsedTagFinderClass*> allTagFinders;
	if (TagFinderList) {
		if (TagFinderList->IsNativeTagFinderInit) {
			allTagFinders.push_back(&TagFinderList->NativeTagFinder);
		}
		if (TagFinderList->IsTagFinderInit) {
			allTagFinders.push_back(&TagFinderList->TagFinder);
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
		std::vector<mvceditor::ParsedTagFinderClass*> allFinders = AllFinders();
		mvceditor::WorkingCacheClass* cache = itWorkingCache->second;
		cache->SymbolTable.ExpressionCompletionMatches(parsedExpression, expressionScope, allFinders, 
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
		std::vector<mvceditor::ParsedTagFinderClass*> allFinders = AllFinders();
		mvceditor::WorkingCacheClass* cache = itWorkingCache->second;
		cache->SymbolTable.ResourceMatches(parsedExpression, expressionScope, allFinders, 
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
	if (TagFinderList && TagFinderList->IsNativeTagFinderInit && !TagFinderList->NativeTagFinder.IsFileCacheEmpty()) {
		return false;
	}
	if (TagFinderList && TagFinderList->IsTagFinderInit && !TagFinderList->TagFinder.IsFileCacheEmpty()) {
		return false;
	}
	return true;
}

bool mvceditor::TagCacheClass::IsResourceCacheEmpty() {

	// if at least one tag finder is not empty, return false
	if (TagFinderList && TagFinderList->IsNativeTagFinderInit && !TagFinderList->NativeTagFinder.IsResourceCacheEmpty()) {
		return false;
	}
	if (TagFinderList && TagFinderList->IsTagFinderInit && !TagFinderList->TagFinder.IsResourceCacheEmpty()) {
		return false;
	}
	if (TagFinderList && TagFinderList->IsDetectedTagFinderInit) {
		mvceditor::DetectedTagTotalCountResultClass result;
		TagFinderList->DetectedTagFinder.Exec(&result);
		if (result.GetTotalCount() <= 0) {
			return false;
		}
	}
	return true;
}

void mvceditor::TagCacheClass::Clear() {

	// ATTN: do NOT wipe finders, Clear() is meant for memory
	// cleanup only
	if (TagFinderList) {
		delete TagFinderList;
		TagFinderList	= NULL;
	}

	std::map<wxString, mvceditor::WorkingCacheClass*>::iterator itWorking;
	for (itWorking = WorkingCaches.begin(); itWorking != WorkingCaches.end(); ++itWorking) {
		delete itWorking->second;
	}
	WorkingCaches.clear();
}

std::vector<mvceditor::TagClass> mvceditor::TagCacheClass::AllMemberTags(const UnicodeString& fullyQualifiedClassName) {
	std::vector<mvceditor::ParsedTagFinderClass*> allTagFinders = AllFinders();

	// add the double colon so that we search for all members
	mvceditor::TagSearchClass tagSearch(fullyQualifiedClassName + UNICODE_STRING_SIMPLE("::"));
	tagSearch.SetParentClasses(mvceditor::TagFinderListClassParents(fullyQualifiedClassName, tagSearch.GetMethodName(), allTagFinders));
	tagSearch.SetTraits(mvceditor::TagFinderListClassUsedTraits(fullyQualifiedClassName, tagSearch.GetParentClasses(), 
		tagSearch.GetMethodName(), allTagFinders));

	std::vector<mvceditor::TagClass> allMatches;
	if (allMatches.empty()) {	
		for (size_t j = 0; j < allTagFinders.size(); ++j) {
			mvceditor::ParsedTagFinderClass* tagFinder = allTagFinders[j];
			mvceditor::TagResultClass* result = tagSearch.CreateNearMatchResults();
			
			tagFinder->Exec(result);
			std::vector<mvceditor::TagClass> finderMatches = result->Matches();
			allMatches.insert(allMatches.end(), finderMatches.begin(), finderMatches.end());

			delete result;
		}
	}
	return allMatches;
}

std::vector<mvceditor::TagClass> mvceditor::TagCacheClass::AllClassesFunctionsDefines(const wxString& fullPath) {
	std::vector<mvceditor::TagClass> allMatches;

	std::vector<mvceditor::ParsedTagFinderClass*> allTagFinders = AllFinders();
	for (size_t j = 0; j < allTagFinders.size(); ++j) {
		mvceditor::ParsedTagFinderClass* finder = allTagFinders[j];
		allMatches = finder->ClassesFunctionsDefines(fullPath);
		if (!allMatches.empty()) {

			// even if the file tag is is multiple finders, they should both be the 
			// same. stop when we find anything
			break;	
		}
	}
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
