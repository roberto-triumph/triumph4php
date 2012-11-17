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
#include <language/ResourceCacheClass.h>
#include <globals/Assets.h>
#include <algorithm>

mvceditor::WorkingCacheCompleteEventClass::WorkingCacheCompleteEventClass(int eventId, const wxString& fileIdentifier, mvceditor::WorkingCacheClass* cache)
	: wxEvent(eventId, mvceditor::EVENT_WORKING_CACHE_COMPLETE)
	, WorkingCache(cache) 
	, FileIdentifier(fileIdentifier.c_str()) {

}

wxEvent* mvceditor::WorkingCacheCompleteEventClass::Clone() const {
	mvceditor::WorkingCacheCompleteEventClass* evt = new mvceditor::WorkingCacheCompleteEventClass(GetId(), FileIdentifier, WorkingCache);
	return evt;
}

wxString mvceditor::WorkingCacheCompleteEventClass::GetFileIdentifier() const {
	return FileIdentifier;
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
	: ResourceFinder()	
	, ResourceDbFileName() {

}

void mvceditor::GlobalCacheClass::Init(const wxFileName& resourceDbFileName, 
									   const std::vector<wxString>& phpFileExtensions, 
									   const std::vector<wxString>& miscFileExtensions,
									   pelet::Versions version, int fileParsingBuffer) {
	ResourceDbFileName = resourceDbFileName;
	ResourceFinder.PhpFileExtensions = phpFileExtensions;
	ResourceFinder.MiscFileExtensions = miscFileExtensions;
	ResourceFinder.SetVersion(version);
	ResourceFinder.InitFile(resourceDbFileName, fileParsingBuffer);
}

void mvceditor::GlobalCacheClass::Walk(mvceditor::DirectorySearchClass& search) {
	search.Walk(ResourceFinder);
}

mvceditor::WorkingCacheClass::WorkingCacheClass()
	: ResourceFinder()
	, SymbolTable()
	, FileName() 
	, IsNew(true)
	, Parser() {

}

bool mvceditor::WorkingCacheClass::Update(const UnicodeString& code) {
	pelet::LintResultsClass results;

	// check for syntax so that only 'good' code modifies the cache
	bool ret = false;
	if (Parser.LintString(code, results)) {
		ResourceFinder.BuildResourceCacheForFile(FileName, code, IsNew);
		SymbolTable.CreateSymbols(code);
		ret = true;
	}
	return ret;
}

void mvceditor::WorkingCacheClass::Init(const wxString& fileName, bool isNew, pelet::Versions version, bool createSymbols) {
	FileName = fileName;
	IsNew = isNew;
	ResourceFinder.SetVersion(version);
	SymbolTable.SetVersion(version);
	Parser.SetVersion(version);
	ResourceFinder.InitMemory();
	if (createSymbols) {

		// ATTN: not using the configured php file filters, file is assume to be a PHP file
		ResourceFinder.PhpFileExtensions.push_back(wxT("*.*"));
		ResourceFinder.BeginSearch();
		ResourceFinder.Walk(fileName);
		ResourceFinder.EndSearch();
		SymbolTable.CreateSymbolsFromFile(fileName);
	}
}

mvceditor::ResourceCacheClass::ResourceCacheClass()
	: GlobalCaches()
	, WorkingCaches() {
	
}

mvceditor::ResourceCacheClass::~ResourceCacheClass() {
	Clear();
 }

bool mvceditor::ResourceCacheClass::RegisterWorking(const wxString& fileName, mvceditor::WorkingCacheClass* cache) {
	bool ret = false;

	// careful to not overwrite the symbol table, resource finder pointers
	std::map<wxString, mvceditor::WorkingCacheClass*>::iterator it = WorkingCaches.find(fileName);
	if (it == WorkingCaches.end()) {
		WorkingCaches[fileName] = cache;
		ret = true;
	}
	return ret;
}

bool mvceditor::ResourceCacheClass::ReplaceWorking(const wxString& fileName, mvceditor::WorkingCacheClass* cache) {
	RemoveWorking(fileName);
	return RegisterWorking(fileName, cache);
}

void mvceditor::ResourceCacheClass::RemoveWorking(const wxString& fileName) {
	std::map<wxString, mvceditor::WorkingCacheClass*>::iterator it = WorkingCaches.find(fileName);
	if (it != WorkingCaches.end()) {
		delete it->second;
		WorkingCaches.erase(it);
	}
}

bool mvceditor::ResourceCacheClass::RegisterGlobal(mvceditor::GlobalCacheClass* cache) {
	bool found = IsInitGlobal(cache->ResourceDbFileName);
	if (!found) {
		GlobalCaches.push_back(cache);
		return true;
	}
	return false;
}

bool mvceditor::ResourceCacheClass::IsInitGlobal(const wxFileName& resourceDbFileName) const {
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

void mvceditor::ResourceCacheClass::RemoveGlobal(const wxFileName& resourceDbFileName) {
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

std::vector<mvceditor::ResourceClass> mvceditor::ResourceCacheClass::AllNonNativeClassesGlobal() const {
	std::vector<mvceditor::ResourceClass> res; 
	std::vector<mvceditor::GlobalCacheClass*>::const_iterator it;
	for (it = GlobalCaches.begin(); it != GlobalCaches.end(); ++it) {
		std::vector<mvceditor::ResourceClass> finderResources = (*it)->ResourceFinder.AllNonNativeClasses();
		res.insert(res.end(), finderResources.begin(), finderResources.end());
	}
	return res;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceCacheClass::CollectFullyQualifiedResourceFromAll(const UnicodeString& search) {
	std::vector<mvceditor::ResourceClass> matches;
	mvceditor::ResourceSearchClass resourceSearch(search);

	// return all of the matches from all finders that were found by the Collect* call.
	// This is a bit tricky because we want to prioritize matches in opened files 
	// instead of the global finder, since the global finder will be outdated.
	std::vector<mvceditor::ResourceFinderClass*> finders = AllFinders();
	std::map<wxString, mvceditor::ResourceFinderClass*> openedFinders;
	std::map<wxString, mvceditor::WorkingCacheClass*>::const_iterator it;
	for (it = WorkingCaches.begin(); it != WorkingCaches.end(); ++it) {
		openedFinders[it->first] = &it->second->ResourceFinder;
	}
	for (size_t i = 0; i < finders.size(); ++i) {
		mvceditor::ResourceFinderClass* resourceFinder = finders[i];
		std::vector<mvceditor::ResourceClass> finderMatches = resourceFinder->CollectFullyQualifiedResource(resourceSearch);
		size_t count = finderMatches.size();
		for (size_t j = 0; j < count; ++j) {
			mvceditor::ResourceClass resource = finderMatches[j];
			if (!mvceditor::IsResourceDirty(openedFinders, resource, resourceFinder)) {
				matches.push_back(resource);
			}
		}
	}
	std::sort(matches.begin(), matches.end());
	return matches;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceCacheClass::CollectNearMatchResourcesFromAll(const UnicodeString& search) {
	std::vector<mvceditor::ResourceClass> matches;
	mvceditor::ResourceSearchClass resourceSearch(search);

	// return all of the matches from all finders that were found by the Collect* call.
	// This is a bit tricky because we want to prioritize matches in opened files 
	// instead of the global finder, since the global finder will be outdated.
	std::vector<mvceditor::ResourceFinderClass*> finders = AllFinders();
	std::map<wxString, mvceditor::ResourceFinderClass*> openedFinders;
	std::map<wxString, mvceditor::WorkingCacheClass*>::const_iterator it;
	for (it = WorkingCaches.begin(); it != WorkingCaches.end(); ++it) {
		openedFinders[it->first] = &it->second->ResourceFinder;
	}
	for (size_t i = 0; i < finders.size(); ++i) {
		mvceditor::ResourceFinderClass* resourceFinder = finders[i];
		std::vector<mvceditor::ResourceClass> finderMatches = resourceFinder->CollectNearMatchResources(resourceSearch);
		size_t count = finderMatches.size();
		for (size_t j = 0; j < count; ++j) {
			mvceditor::ResourceClass resource = finderMatches[j];
			if (!mvceditor::IsResourceDirty(openedFinders, resource, resourceFinder)) {
				matches.push_back(resource);
			}
		}
	}
	std::sort(matches.begin(), matches.end());
	return matches;
}

std::vector<mvceditor::ResourceFinderClass*> mvceditor::ResourceCacheClass::AllFinders() {
	std::vector<mvceditor::ResourceFinderClass*> allResourceFinders;
	std::map<wxString, mvceditor::WorkingCacheClass*>::iterator it;
	for (it = WorkingCaches.begin(); it != WorkingCaches.end(); ++it) {
		allResourceFinders.push_back(&it->second->ResourceFinder);
	}
	std::vector<mvceditor::GlobalCacheClass*>::iterator itGlobal;
	for (itGlobal = GlobalCaches.begin(); itGlobal != GlobalCaches.end(); ++itGlobal) {
		allResourceFinders.push_back(&((*itGlobal)->ResourceFinder));
	}
	return allResourceFinders;
}

void mvceditor::ResourceCacheClass::ExpressionCompletionMatches(const wxString& fileName, const pelet::ExpressionClass& parsedExpression, 
													const pelet::ScopeClass& expressionScope, 
													 std::vector<UnicodeString>& autoCompleteList,
													 std::vector<mvceditor::ResourceClass>& resourceMatches,
													 bool doDuckTyping,
													 mvceditor::SymbolTableMatchErrorClass& error) {
	std::map<wxString, mvceditor::WorkingCacheClass*>::const_iterator itWorkingCache = WorkingCaches.find(fileName);
	bool foundSymbolTable = false;
	if (itWorkingCache != WorkingCaches.end()) {
		foundSymbolTable = true;
		std::vector<mvceditor::ResourceFinderClass*> allFinders = AllFinders();
		std::map<wxString, mvceditor::ResourceFinderClass*> openedFinders;
		std::map<wxString, mvceditor::WorkingCacheClass*>::const_iterator it;
		for (it = WorkingCaches.begin(); it != WorkingCaches.end(); ++it) {
			openedFinders[it->first] = &it->second->ResourceFinder;
		}
		mvceditor::WorkingCacheClass* cache = itWorkingCache->second;
		cache->SymbolTable.ExpressionCompletionMatches(parsedExpression, expressionScope, allFinders, openedFinders, 
			autoCompleteList, resourceMatches, doDuckTyping, error);
	
	}
	if (!foundSymbolTable) {
		error.Type = mvceditor::SymbolTableMatchErrorClass::UNREGISTERED_FILE;
	}
}

void mvceditor::ResourceCacheClass::ResourceMatches(const wxString& fileName, const pelet::ExpressionClass& parsedExpression, 
													const pelet::ScopeClass& expressionScope, 
													std::vector<mvceditor::ResourceClass>& matches,
													bool doDuckTyping, bool doFullyQualifiedMatchOnly,
													mvceditor::SymbolTableMatchErrorClass& error) {
	std::map<wxString, mvceditor::WorkingCacheClass*>::const_iterator itWorkingCache = WorkingCaches.find(fileName);
	bool foundSymbolTable = false;
	if (itWorkingCache != WorkingCaches.end()) {
		foundSymbolTable = true;
		std::vector<mvceditor::ResourceFinderClass*> allFinders = AllFinders();
		std::map<wxString, mvceditor::ResourceFinderClass*> openedFinders;
		std::map<wxString, mvceditor::WorkingCacheClass*>::const_iterator it;
		for (it = WorkingCaches.begin(); it != WorkingCaches.end(); ++it) {
			openedFinders[it->first] = &it->second->ResourceFinder;
		}
		mvceditor::WorkingCacheClass* cache = itWorkingCache->second;
		cache->SymbolTable.ResourceMatches(parsedExpression, expressionScope, allFinders, openedFinders, 
			matches, doDuckTyping, doFullyQualifiedMatchOnly, error);	
	}
	if (!foundSymbolTable) {
		error.Type = mvceditor::SymbolTableMatchErrorClass::UNREGISTERED_FILE;
	}
}

void mvceditor::ResourceCacheClass::GlobalAddDynamicResources(const std::vector<mvceditor::ResourceClass>& resources) {
	if (WorkingCaches.count(wxT(":memory:")) == 0) {

		// this is the case when Clear() is called right before this method
		mvceditor::WorkingCacheClass* cache = new mvceditor::WorkingCacheClass;
		cache->Init(wxT(":memory:"), true, pelet::PHP_53, false);
		WorkingCaches[wxT(":memory:")] = cache;
	}
	WorkingCaches[wxT(":memory:")]->ResourceFinder.AddDynamicResources(resources);
}

void mvceditor::ResourceCacheClass::Print() {
	UFILE* ufout = u_finit(stdout, NULL, NULL);
	u_fprintf(ufout, "Number of global caches: %d\n", GlobalCaches.size());
	u_fprintf(ufout, "Number of working caches: %d\n", WorkingCaches.size());
	u_fclose(ufout);
	std::map<wxString, mvceditor::WorkingCacheClass*>::const_iterator it = WorkingCaches.begin();
	for (; it != WorkingCaches.end(); ++it) {
		it->second->SymbolTable.Print();
	}
}

bool mvceditor::ResourceCacheClass::IsFileCacheEmpty() {
	bool isEmpty = true;

	// if at least one resource finder is not empty, return false
	std::vector<mvceditor::GlobalCacheClass*>::iterator it;
	for (it = GlobalCaches.begin(); isEmpty && it != GlobalCaches.end(); ++it) {
		isEmpty = (*it)->ResourceFinder.IsFileCacheEmpty();
	}
	return isEmpty;
}

bool mvceditor::ResourceCacheClass::IsResourceCacheEmpty() {
	bool isEmpty = true;

	// if at least one resource finder is not empty, return false
	std::vector<mvceditor::GlobalCacheClass*>::iterator it;
	for (it = GlobalCaches.begin(); isEmpty && it != GlobalCaches.end(); ++it) {
		isEmpty = (*it)->ResourceFinder.IsResourceCacheEmpty();
	}
	return isEmpty;
}

void mvceditor::ResourceCacheClass::Clear() {

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

void mvceditor::ResourceCacheClass::WipeGlobal() {

	// ATTN: DO wipe finders this is an explict call
	std::vector<mvceditor::GlobalCacheClass*>::iterator it;
	wxFileName nativeFunctionsFileName = mvceditor::NativeFunctionsAsset();
	it = GlobalCaches.begin();
	while(it != GlobalCaches.end()) {
		
		// do not remove the native cache; it never changes 
		if ((*it)->ResourceDbFileName != nativeFunctionsFileName) {
			(*it)->ResourceFinder.Wipe();
			delete *it;
			it = GlobalCaches.erase(it);
		}
		else {
			++it;
		}
	}
}