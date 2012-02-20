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
#include <widgets/ResourceCacheClass.h>
#include <algorithm>

mvceditor::ResourceCacheClass::ResourceCacheClass()
	: Mutex()
	, Finders()
	, SymbolTables()
	, GlobalResourceFinder() {
}

mvceditor::ResourceCacheClass::~ResourceCacheClass() {
	for (std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it =  Finders.begin(); it != Finders.end(); ++it) {
		delete it->second;
	}
	for (std::map<wxString, mvceditor::SymbolTableClass*>::iterator it =  SymbolTables.begin(); it != SymbolTables.end(); ++it) {
		delete it->second;
	}
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
		Finders[fileName] = res;
		SymbolTables[fileName] = table;
		if (createSymbols) {
			
			// ATTN: not using the configured php file filters?
			res->FileFilters.push_back(wxT("*.*"));
			res->Prepare(wxT("Fakeclass"));
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
		mvceditor::ParserClass parser;
		mvceditor::LintResultsClass results;
		if (parser.LintString(code, results)) {
			mvceditor::ResourceFinderClass* finder = it->second;
			mvceditor::SymbolTableClass* symbolTable = itSymbols->second;
			finder->BuildResourceCacheForFile(fileName, code, isNew);
			finder->EnsureSorted();
			symbolTable->CreateSymbols(code);
			ret = true;
		}
	}
	return ret;
}

bool mvceditor::ResourceCacheClass::WalkGlobal(mvceditor::DirectorySearchClass& search, const std::vector<wxString>& phpFileFilters) {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return false;
	}

	// need to do this so that the resource finder attempts to parse the files
	// FileFilters and query string needs to be non-empty
	GlobalResourceFinder.FileFilters = phpFileFilters;
	GlobalResourceFinder.Prepare(wxT("FakeFakeClass"));
	search.Walk(GlobalResourceFinder);
	return true;
}

bool mvceditor::ResourceCacheClass::PersistGlobal(const wxFileName& outputFile) {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return false;
	}
	return GlobalResourceFinder.Persist(outputFile);
}

void mvceditor::ResourceCacheClass::EnsureSortedGlobal() {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return;
	}
	GlobalResourceFinder.EnsureSorted();
}

bool mvceditor::ResourceCacheClass::PrepareAll(const wxString& resource) {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return false;
	}
	std::vector<mvceditor::ResourceFinderClass*> finders = Iterator(&GlobalResourceFinder);
	bool prep = true;
	for (size_t i = 0; i < finders.size(); ++i) {
		prep &= finders[i]->Prepare(resource);
	}
	return prep;
}

bool mvceditor::ResourceCacheClass::CollectFullyQualifiedResourceFromAll() {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return false;
	}
	std::vector<mvceditor::ResourceFinderClass*> finders = Iterator(&GlobalResourceFinder);
	bool found = false;
	for (size_t i = 0; i < finders.size(); ++i) {
		found |= finders[i]->CollectFullyQualifiedResource();
	}
	return found;
}

bool mvceditor::ResourceCacheClass::CollectNearMatchResourcesFromAll() {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return false;
	}
	std::vector<mvceditor::ResourceFinderClass*> finders = Iterator(&GlobalResourceFinder);
	bool found = false;
	for (size_t i = 0; i < finders.size(); ++i) {
		found |= finders[i]->CollectNearMatchResources();
	}
	return found;
}

std::vector<mvceditor::ResourceClass>
mvceditor::ResourceCacheClass::PrepareAndCollectNearMatchResourcesFromAll(const wxString& resource) {
	std::vector<mvceditor::ResourceClass> matches;
	if (PrepareAll(resource)) {
		if (CollectNearMatchResourcesFromAll()) {
			matches = Matches();
		}
	}
	return matches;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceCacheClass::Matches() {
	std::vector<mvceditor::ResourceClass> matches;
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return matches;
	}
	std::vector<mvceditor::ResourceFinderClass*> finders = Iterator(&GlobalResourceFinder);
	for (size_t r = 0; r < finders.size(); ++r) {
		mvceditor::ResourceFinderClass* resourceFinder = finders[r];
		size_t count = resourceFinder->GetResourceMatchCount();
		for (size_t j = 0; j < count; ++j) {
			mvceditor::ResourceClass resource = resourceFinder->GetResourceMatch(j);
			if (!mvceditor::IsResourceDirty(Finders, resource, resourceFinder)) {
				matches.push_back(resource);
			}
		}
	}
	std::sort(matches.begin(), matches.end());
	return matches;
}

std::vector<mvceditor::ResourceFinderClass*> mvceditor::ResourceCacheClass::Iterator(mvceditor::ResourceFinderClass* resourceFinder) {
	std::vector<mvceditor::ResourceFinderClass*> finders;
	finders.push_back(resourceFinder);
	for (std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it =  Finders.begin(); it != Finders.end(); ++it) {
		finders.push_back(it->second);
	}
	return finders;
}

void mvceditor::ResourceCacheClass::ExpressionCompletionMatches(const wxString& fileName, const mvceditor::SymbolClass& parsedExpression, const UnicodeString& expressionScope, 
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
			symbolTable->ExpressionCompletionMatches(parsedExpression, expressionScope, Finders, &GlobalResourceFinder, 
				autoCompleteList, resourceMatches, doDuckTyping, error);
		}
	}
	if (!foundSymbolTable) {
		error.Type = mvceditor::SymbolTableMatchErrorClass::UNREGISTERED_FILE;
	}
}

void mvceditor::ResourceCacheClass::ResourceMatches(const wxString& fileName, const mvceditor::SymbolClass& parsedExpression, const UnicodeString& expressionScope, 
													 std::vector<mvceditor::ResourceClass>& matches,
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
			symbolTable->ResourceMatches(parsedExpression, expressionScope, Finders, &GlobalResourceFinder, 
				matches, doDuckTyping, error);
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
	GlobalResourceFinder.AddDynamicResources(resources);
}

void mvceditor::ResourceCacheClass::Print() {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return;
	}
	GlobalResourceFinder.Print();
	UFILE* ufout = u_finit(stdout, NULL, NULL);
	u_fprintf(ufout, "Number of registered caches: %d\n", Finders.size());
	u_fprintf(ufout, "Number of registered symbol tables: %d\n", SymbolTables.size());
	u_fclose(ufout);
	std::map<wxString, mvceditor::SymbolTableClass*>::const_iterator it = SymbolTables.begin();
	for (; it != SymbolTables.end(); ++it) {
		it->second->Print();
	}
	
}

bool mvceditor::ResourceCacheClass::IsEmpty() {
	wxMutexLocker locker(Mutex);
	if (!locker.IsOk()) {
		return true;
	}
	bool isEmpty = GlobalResourceFinder.IsEmpty() && SymbolTables.empty() && Finders.empty();
	return isEmpty;
}

mvceditor::ResourceCacheUpdateThreadClass::ResourceCacheUpdateThreadClass(mvceditor::ResourceCacheClass* resourceCache, wxEvtHandler& handler, int eventId)
	: ThreadWithHeartbeatClass(handler, eventId)
	, ResourceCache(resourceCache)
	, CurrentCode() 
	, CurrentFileName() {
}

wxThreadError mvceditor::ResourceCacheUpdateThreadClass::StartBackgroundUpdate(const wxString& fileName, const UnicodeString& code, bool isNew) {
	if (!ResourceCache) {
		return wxTHREAD_NO_ERROR;
	}
	wxThreadError error = CreateSingleInstance();
	if (wxTHREAD_NO_ERROR == error) {
		CurrentCode = code;
		CurrentFileName = fileName;
		CurrentFileIsNew = isNew;
		GetThread()->Run();
		SignalStart();		
	}
	return error;
}

void* mvceditor::ResourceCacheUpdateThreadClass::Entry() {
	ResourceCache->Update(CurrentFileName, CurrentCode, CurrentFileIsNew);
	
	// cleanup.
	CurrentFileName.resize(0);
	CurrentCode.truncate(0);
	SignalEnd();
	return 0;
}