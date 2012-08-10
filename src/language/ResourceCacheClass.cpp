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
#include <algorithm>

mvceditor::ResourceCacheClass::ResourceCacheClass()
	: Mutex()
	, Finders()
	, SymbolTables()
	, GlobalResourceFinders() 
	, Version(pelet::PHP_53) {
}

mvceditor::ResourceCacheClass::~ResourceCacheClass() {
	Clear();
}

bool mvceditor::ResourceCacheClass::Register(const wxString& fileName, bool createSymbols) {
	bool ret = false;
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return false;
	}

	// careful to not overwrite the symbol table, resource finder pointers
	std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it = Finders.find(fileName);
	std::map<wxString, mvceditor::SymbolTableClass*>::iterator itSymbols = SymbolTables.find(fileName);
	if (it == Finders.end() && itSymbols == SymbolTables.end()) {
		mvceditor::ResourceFinderClass* res = new mvceditor::ResourceFinderClass;
		mvceditor::SymbolTableClass* table = new mvceditor::SymbolTableClass();
		res->SetVersion(Version);
		res->InitMemory();
		table->SetVersion(Version);
		Finders[fileName] = res;
		SymbolTables[fileName] = table;
		if (createSymbols) {
			
			// ATTN: not using the configured php file filters?
			res->FileFilters.push_back(wxT("*.*"));
			res->Walk(fileName);
			table->CreateSymbolsFromFile(fileName);
		}
		ret = true;
	}
	return ret;
}

void mvceditor::ResourceCacheClass::Unregister(const wxString& fileName) {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return;
	}
	std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it = Finders.find(fileName);
	if (it != Finders.end()) {
		delete it->second;
	}
	std::map<wxString, mvceditor::SymbolTableClass*>::iterator itSymbols = SymbolTables.find(fileName);
	if (itSymbols != SymbolTables.end()) {
		delete itSymbols->second;
	}
	Finders.erase(fileName);
	SymbolTables.erase(fileName);
}

bool mvceditor::ResourceCacheClass::Update(const wxString& fileName, const UnicodeString& code, bool isNew) {
	
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return false;
	}
	std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it = Finders.find(fileName);
	std::map<wxString, mvceditor::SymbolTableClass*>::iterator itSymbols = SymbolTables.find(fileName);
	bool ret = false;
	if (it != Finders.end() && itSymbols != SymbolTables.end()) {
		pelet::ParserClass parser;
		parser.SetVersion(Version);
		pelet::LintResultsClass results;
		if (parser.LintString(code, results)) {
			mvceditor::ResourceFinderClass* finder = it->second;
			mvceditor::SymbolTableClass* symbolTable = itSymbols->second;
			finder->BuildResourceCacheForFile(fileName, code, isNew);
			symbolTable->CreateSymbols(code);
			ret = true;
		}
	}
	return ret;
}

bool mvceditor::ResourceCacheClass::InitGlobal(const wxFileName& resourceDbFileName, int fileParsingBufferSize) {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return false;
	}
	
	bool ret = false;
	int cnt = GlobalResourceFinders.count(resourceDbFileName.GetFullPath());
	if (cnt <= 0) {
		mvceditor::ResourceFinderClass* resourceFinder = new mvceditor::ResourceFinderClass();
		resourceFinder->InitFile(resourceDbFileName, fileParsingBufferSize);
		GlobalResourceFinders[resourceDbFileName.GetFullPath()] = resourceFinder;
		ret = true;
	}
	return ret;
}

void mvceditor::ResourceCacheClass::RemoveGlobal(const wxFileName& resourceDbFileName) {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return;
	}
	std::map<wxString, mvceditor::ResourceFinderClass*>::iterator item = GlobalResourceFinders.find(resourceDbFileName.GetFullPath());
	if (item != GlobalResourceFinders.end()) {
		delete item->second;
		GlobalResourceFinders.erase(item);
	}
}

bool mvceditor::ResourceCacheClass::IsInitGlobal(const wxFileName& resourceDbFileName) {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return false;
	}
	int cnt = GlobalResourceFinders.count(resourceDbFileName.GetFullPath());
	return cnt > 0;
}

bool mvceditor::ResourceCacheClass::WalkGlobal(const wxFileName& resourceDbFileName, mvceditor::DirectorySearchClass& search, const std::vector<wxString>& phpFileFilters) {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return false;
	}

	// need to do this so that the resource finder attempts to parse the files
	// FileFilters and query string needs to be non-empty
	for (std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it = GlobalResourceFinders.begin(); it != GlobalResourceFinders.end(); ++it) {
		if (it->first == resourceDbFileName.GetFullPath()) {
			it->second->FileFilters = phpFileFilters;
			search.Walk(*it->second);
		}
	}
	return true;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceCacheClass::AllNonNativeClassesGlobal() {
	wxMutexLocker locker(Mutex);
	std::vector<mvceditor::ResourceClass> res;
	if (!locker.IsOk()) {
		return res;
	}
	std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it;
	for (it = GlobalResourceFinders.begin(); it != GlobalResourceFinders.end(); ++it) {
		std::vector<mvceditor::ResourceClass> finderResources = it->second->AllNonNativeClasses();
		res.insert(res.end(), finderResources.begin(), finderResources.end());
	}
	return res;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceCacheClass::CollectFullyQualifiedResourceFromAll(const UnicodeString& search) {
	std::vector<mvceditor::ResourceClass> matches;
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return matches;
	}
	mvceditor::ResourceSearchClass resourceSearch(search);

	// return all of the matches from all finders that were found by the Collect* call.
	// This is a bit tricky because we want to prioritize matches in opened files 
	// instead of the global finder, since the global finder will be outdated.
	std::vector<mvceditor::ResourceFinderClass*> finders = AllFinders();
	for (size_t i = 0; i < finders.size(); ++i) {
		mvceditor::ResourceFinderClass* resourceFinder = finders[i];
		std::vector<mvceditor::ResourceClass> finderMatches = resourceFinder->CollectFullyQualifiedResource(resourceSearch);
		size_t count = finderMatches.size();
		for (size_t j = 0; j < count; ++j) {
			mvceditor::ResourceClass resource = finderMatches[j];
			if (!mvceditor::IsResourceDirty(Finders, resource, resourceFinder)) {
				matches.push_back(resource);
			}
		}
	}
	std::sort(matches.begin(), matches.end());
	return matches;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceCacheClass::CollectNearMatchResourcesFromAll(const UnicodeString& search) {
	std::vector<mvceditor::ResourceClass> matches;
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return matches;
	}
	mvceditor::ResourceSearchClass resourceSearch(search);

	// return all of the matches from all finders that were found by the Collect* call.
	// This is a bit tricky because we want to prioritize matches in opened files 
	// instead of the global finder, since the global finder will be outdated.
	std::vector<mvceditor::ResourceFinderClass*> finders = AllFinders();
	for (size_t i = 0; i < finders.size(); ++i) {
		mvceditor::ResourceFinderClass* resourceFinder = finders[i];
		std::vector<mvceditor::ResourceClass> finderMatches = resourceFinder->CollectNearMatchResources(resourceSearch);
		size_t count = finderMatches.size();
		for (size_t j = 0; j < count; ++j) {
			mvceditor::ResourceClass resource = finderMatches[j];
			if (!mvceditor::IsResourceDirty(Finders, resource, resourceFinder)) {
				matches.push_back(resource);
			}
		}
	}
	std::sort(matches.begin(), matches.end());
	return matches;
}

std::vector<mvceditor::ResourceFinderClass*> mvceditor::ResourceCacheClass::AllFinders() {
	std::vector<mvceditor::ResourceFinderClass*> allResourceFinders;
	std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it;
	for (it =  Finders.begin(); it != Finders.end(); ++it) {
		allResourceFinders.push_back(it->second);
	}	
	for (it = GlobalResourceFinders.begin(); it != GlobalResourceFinders.end(); ++it) {
		allResourceFinders.push_back(it->second);
	}
	return allResourceFinders;
}

void mvceditor::ResourceCacheClass::ExpressionCompletionMatches(const wxString& fileName, const pelet::ExpressionClass& parsedExpression, 
													const pelet::ScopeClass& expressionScope, 
													 std::vector<UnicodeString>& autoCompleteList,
													 std::vector<mvceditor::ResourceClass>& resourceMatches,
													 bool doDuckTyping,
													 mvceditor::SymbolTableMatchErrorClass& error) {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return;
	}
	std::map<wxString, mvceditor::SymbolTableClass*>::const_iterator itSymbols = SymbolTables.find(fileName);
	bool foundSymbolTable = false;
	if (itSymbols != SymbolTables.end()) {
		mvceditor::SymbolTableClass* symbolTable = itSymbols->second;
		if (symbolTable) {
			foundSymbolTable = true;
			std::vector<mvceditor::ResourceFinderClass*> allFinders = AllFinders();
			symbolTable->ExpressionCompletionMatches(parsedExpression, expressionScope, allFinders, Finders, 
				autoCompleteList, resourceMatches, doDuckTyping, error);
		}
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
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return;
	}
	std::map<wxString, mvceditor::SymbolTableClass*>::const_iterator itSymbols = SymbolTables.find(fileName);
	bool foundSymbolTable = false;
	if (itSymbols != SymbolTables.end()) {
		mvceditor::SymbolTableClass* symbolTable = itSymbols->second;
		if (symbolTable) {
			foundSymbolTable = true;
			///symbolTable->Print();
			///GlobalResourceFinder.Print();
			std::vector<mvceditor::ResourceFinderClass*> allFinders = AllFinders();
			symbolTable->ResourceMatches(parsedExpression, expressionScope, allFinders, Finders, 
				matches, doDuckTyping, doFullyQualifiedMatchOnly, error);
		}
	}
	if (!foundSymbolTable) {
		error.Type = mvceditor::SymbolTableMatchErrorClass::UNREGISTERED_FILE;
	}
}

void mvceditor::ResourceCacheClass::GlobalAddDynamicResources(const std::vector<mvceditor::ResourceClass>& resources) {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return;
	}
	if (GlobalResourceFinders.count(wxT(":memory:")) == 0) {

		// this is the case when Clear() is called right before this method
		mvceditor::ResourceFinderClass* finder = new mvceditor::ResourceFinderClass;
		finder->InitMemory();
		GlobalResourceFinders[wxT(":memory:")] = finder;
	}
	GlobalResourceFinders[wxT(":memory:")]->AddDynamicResources(resources);
}

void mvceditor::ResourceCacheClass::Print() {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return;
	}
	UFILE* ufout = u_finit(stdout, NULL, NULL);
	u_fprintf(ufout, "Number of global caches: %d\n", GlobalResourceFinders.size());
	u_fprintf(ufout, "Number of registered caches: %d\n", Finders.size());
	u_fprintf(ufout, "Number of registered symbol tables: %d\n", SymbolTables.size());
	u_fclose(ufout);
	std::map<wxString, mvceditor::SymbolTableClass*>::const_iterator it = SymbolTables.begin();
	for (; it != SymbolTables.end(); ++it) {
		it->second->Print();
	}
}

bool mvceditor::ResourceCacheClass::IsFileCacheEmpty() {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return true;
	}
	bool isEmpty = true;

	// if at least one resource finder is not empty, return false
	std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it;
	for (it = GlobalResourceFinders.begin(); isEmpty && it != GlobalResourceFinders.end(); ++it) {
		isEmpty = it->second->IsFileCacheEmpty();
	}
	return isEmpty;
}

bool mvceditor::ResourceCacheClass::IsResourceCacheEmpty() {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return true;
	}
	bool isEmpty = true;

	// if at least one resource finder is not empty, return false
	std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it;
	for (it = GlobalResourceFinders.begin(); isEmpty && it != GlobalResourceFinders.end(); ++it) {
		isEmpty = it->second->IsResourceCacheEmpty();
	}
	return isEmpty;
}

void mvceditor::ResourceCacheClass::Clear() {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return;
	}

	// ATTN: do NOT wipe finders, Clear() is meant for memory
	// cleanup only
	std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it;
	for (it = GlobalResourceFinders.begin(); it != GlobalResourceFinders.end(); ++it) {
		delete it->second;
	}
	for (std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it =  Finders.begin(); it != Finders.end(); ++it) {
		delete it->second;
	}
	for (std::map<wxString, mvceditor::SymbolTableClass*>::iterator it =  SymbolTables.begin(); it != SymbolTables.end(); ++it) {
		delete it->second;
	}
	GlobalResourceFinders.clear();
	Finders.clear();
	SymbolTables.clear();	
}

void mvceditor::ResourceCacheClass::Wipe() {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return;
	}

	// ATTN: DO wipe finders this is an explict call
	std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it;
	for (it = GlobalResourceFinders.begin(); it != GlobalResourceFinders.end(); ++it) {
		it->second->Wipe();
		delete it->second;
	}
	for (std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it =  Finders.begin(); it != Finders.end(); ++it) {
		delete it->second;
	}
	for (std::map<wxString, mvceditor::SymbolTableClass*>::iterator it =  SymbolTables.begin(); it != SymbolTables.end(); ++it) {
		delete it->second;
	}
	GlobalResourceFinders.clear();
	Finders.clear();
	SymbolTables.clear();	
}

void mvceditor::ResourceCacheClass::SetVersion(pelet::Versions version) {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return;
	}
	Version = version;
	std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it;
	for (it = GlobalResourceFinders.begin(); it != GlobalResourceFinders.end(); ++it) {
		it->second->SetVersion(version);
	}
	for (std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it =  Finders.begin(); it != Finders.end(); ++it) {
		it->second->SetVersion(version);
	}
	for (std::map<wxString, mvceditor::SymbolTableClass*>::iterator it =  SymbolTables.begin(); it != SymbolTables.end(); ++it) {
		it->second->SetVersion(version);
	}
	
}