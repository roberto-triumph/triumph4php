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
#include <widgets/ResourceUpdateThreadClass.h>
#include <algorithm>

mvceditor::ResourceUpdateClass::ResourceUpdateClass()
	: Finders()
	, SymbolTables() {
}

mvceditor::ResourceUpdateClass::~ResourceUpdateClass() {
	for (std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it =  Finders.begin(); it != Finders.end(); ++it) {
		delete it->second;
	}
	for (std::map<wxString, mvceditor::SymbolTableClass*>::iterator it =  SymbolTables.begin(); it != SymbolTables.end(); ++it) {
		delete it->second;
	}
}

bool mvceditor::ResourceUpdateClass::Register(const wxString& fileName) {
	bool ret = false;
	
	// careful to not overwrite the symbol table, resource finder pointers
	std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it = Finders.find(fileName);
	std::map<wxString, mvceditor::SymbolTableClass*>::iterator itSymbols = SymbolTables.find(fileName);
	if (it == Finders.end() && itSymbols == SymbolTables.end()) {
		mvceditor::ResourceFinderClass* res = new mvceditor::ResourceFinderClass;
		mvceditor::SymbolTableClass* table = new mvceditor::SymbolTableClass();
		Finders[fileName] = res;
		SymbolTables[fileName] = table;
		ret = true;
	}
	return ret;
}

void mvceditor::ResourceUpdateClass::Unregister(const wxString& fileName) {
	std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it = Finders.find(fileName);
	if (it != Finders.end()) {
		delete it->second;
	}
	std::map<wxString, mvceditor::SymbolTableClass*>::iterator itSymbols = SymbolTables.find(fileName);
	if (itSymbols != SymbolTables.end()) {
		delete it->second;
	}
	Finders.erase(fileName);
	SymbolTables.erase(fileName);
}

bool mvceditor::ResourceUpdateClass::Update(const wxString& fileName, const UnicodeString& code) {
	std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it = Finders.find(fileName);
	std::map<wxString, mvceditor::SymbolTableClass*>::iterator itSymbols = SymbolTables.find(fileName);
	bool ret = false;
	if (it != Finders.end() && itSymbols != SymbolTables.end()) {
		mvceditor::ResourceFinderClass* finder = it->second;
		mvceditor::SymbolTableClass* symbolTable = itSymbols->second;
		finder->BuildResourceCacheForFile(fileName, code);
		finder->EnsureSorted();
		symbolTable->CreateSymbols(code);
		ret = true;
	}
	return ret;
}

bool mvceditor::ResourceUpdateClass::IsDirty(const ResourceClass& resource) {
	bool ret = false;
	
	return ret;
}

bool mvceditor::ResourceUpdateClass::PrepareAll(mvceditor::ResourceFinderClass* resourceFinder, const wxString& resource) {
	std::vector<mvceditor::ResourceFinderClass*> finders = Iterator(resourceFinder);
	bool prep = true;
	for (size_t i = 0; i < finders.size(); ++i) {
		prep &= finders[i]->Prepare(resource);
	}
	return prep;
}

bool mvceditor::ResourceUpdateClass::CollectFullyQualifiedResourceFromAll(mvceditor::ResourceFinderClass* resourceFinder) {
	std::vector<mvceditor::ResourceFinderClass*> finders = Iterator(resourceFinder);
	bool found = false;
	for (size_t i = 0; i < finders.size(); ++i) {
		found |= finders[i]->CollectFullyQualifiedResource();
	}
	return found;
}

bool mvceditor::ResourceUpdateClass::CollectNearMatchResourcesFromAll(mvceditor::ResourceFinderClass* resourceFinder) {
	std::vector<mvceditor::ResourceFinderClass*> finders = Iterator(resourceFinder);
	bool found = false;
	for (size_t i = 0; i < finders.size(); ++i) {
		found |= finders[i]->CollectNearMatchResources();
	}
	return found;
}

std::vector<mvceditor::ResourceClass> mvceditor::ResourceUpdateClass::Matches(mvceditor::ResourceFinderClass* resourceFinder) {
	std::vector<mvceditor::ResourceClass> matches;
	std::vector<mvceditor::ResourceFinderClass*> finders = Iterator(resourceFinder);
	for (size_t r = 0; r < finders.size(); ++r) {
		mvceditor::ResourceFinderClass* resourceFinder = finders[r];
		size_t count = resourceFinder->GetResourceMatchCount();
		for (size_t j = 0; j < count; ++j) {
			mvceditor::ResourceClass resource = resourceFinder->GetResourceMatch(j);
			matches.push_back(resource);
		}
	}
	std::sort(matches.begin(), matches.end());
	return matches;
}

std::vector<mvceditor::ResourceFinderClass*> mvceditor::ResourceUpdateClass::Iterator(mvceditor::ResourceFinderClass* resourceFinder) {
	std::vector<mvceditor::ResourceFinderClass*> finders;
	finders.push_back(resourceFinder);
	for (std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it =  Finders.begin(); it != Finders.end(); ++it) {
		finders.push_back(it->second);
	}
	return finders;
}

UnicodeString mvceditor::ResourceUpdateClass::GetSymbolAt(const wxString& fileName, int pos, mvceditor::ResourceFinderClass* resourceFinder) {
	UnicodeString symbol;
	std::map<wxString, mvceditor::SymbolTableClass*>::iterator itSymbols = SymbolTables.find(fileName);
	if (itSymbols != SymbolTables.end()) {
		mvceditor::SymbolTableClass* symbolTable = itSymbols->second;
		SymbolClass::Types type = mvceditor::SymbolClass::ARRAY;
		UnicodeString objectType;
		UnicodeString objectMember;
		UnicodeString comment;
		bool isThisCall = false;
		bool isParentCall = false;
		bool isStaticCall = false;
			
		// TODO: what about the opened files? will need to look at the opened files resource finders (ResourceUpdates object)
		if (symbolTable->Lookup(pos, *resourceFinder, type, objectType, objectMember, comment, isThisCall, isParentCall, isStaticCall)) {
			bool isObjectMethodOrProperty = SymbolClass::OBJECT == type ||SymbolClass::METHOD == type || SymbolClass::PROPERTY == type;
			if (isObjectMethodOrProperty)  {
				
				// even if objectType is empty, symbol will be something like '::METHOD' which the 
				// ResourceFinder will interpret to look for methods only (which is what we want here)
				symbol = objectType + UNICODE_STRING_SIMPLE("::") + objectMember;
			}
			else {
				symbol = objectType;
			}
		}
	}
	return symbol;
}

mvceditor::ResourceUpdateThreadClass::ResourceUpdateThreadClass(wxEvtHandler& handler, int eventId)
	: ThreadWithHeartbeatClass(handler, eventId)
	, Worker()
	, Handlers()
	, CurrentCode() 
	, CurrentFileName() {
}

bool mvceditor::ResourceUpdateThreadClass::Register(const wxString& fileName, wxEvtHandler* handler) {
	bool ret = Worker.Register(fileName);
	if (ret) {
		Handlers[fileName] = handler;
		ret = true;
	}
	return ret;
}

void mvceditor::ResourceUpdateThreadClass::Unregister(const wxString& fileName) {
	Worker.Unregister(fileName);
	Handlers.erase(fileName);
}


wxThreadError mvceditor::ResourceUpdateThreadClass::StartBackgroundUpdate(const wxString& fileName, const UnicodeString& code) {
	wxThreadError err = Create();
	if (wxTHREAD_NO_ERROR == err && !GetThread()->IsRunning()) {
		CurrentCode = code;
		CurrentFileName = fileName;
		GetThread()->Run();
		SignalStart();
	}
	return err;
}

void* mvceditor::ResourceUpdateThreadClass::Entry() {
	Worker.Update(CurrentFileName, CurrentCode);
	
	std::map<wxString, wxEvtHandler*>::iterator itHandler = Handlers.find(CurrentFileName);
	if (itHandler != Handlers.end()) {
		wxCommandEvent evt(mvceditor::EVENT_WORK_COMPLETE, wxID_ANY);
		wxPostEvent(itHandler->second, evt);
	}
	
	// cleanup.
	CurrentFileName.resize(0);
	CurrentCode.truncate(0);
	return 0;
}