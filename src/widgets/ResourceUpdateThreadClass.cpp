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
		delete itSymbols->second;
	}
	Finders.erase(fileName);
	SymbolTables.erase(fileName);
}

bool mvceditor::ResourceUpdateClass::Update(const wxString& fileName, const UnicodeString& code, bool isNew) {
	
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

bool mvceditor::ResourceUpdateClass::IsDirty(const ResourceClass& resource, mvceditor::ResourceFinderClass* resourceFinder) const {
	bool ret = false;
	wxString matchFullName = resourceFinder->GetResourceMatchFullPathFromResource(resource);
	std::map<wxString, mvceditor::ResourceFinderClass*>::const_iterator it = Finders.begin();
	while (it != Finders.end()) {

		// a match from one of the opened resource finders can never be 'dirty' because the resource cache 
		// has been updated by the Update() method
		if (it->first.CompareTo(matchFullName) == 0 && it->second != resourceFinder) {
			ret = true;
			break;
		}
		++it;
	}
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

std::vector<mvceditor::ResourceClass> mvceditor::ResourceUpdateClass::Matches(mvceditor::ResourceFinderClass* globalResourceFinder) {
	std::vector<mvceditor::ResourceClass> matches;
	std::vector<mvceditor::ResourceFinderClass*> finders = Iterator(globalResourceFinder);
	for (size_t r = 0; r < finders.size(); ++r) {
		mvceditor::ResourceFinderClass* resourceFinder = finders[r];
		size_t count = resourceFinder->GetResourceMatchCount();
		for (size_t j = 0; j < count; ++j) {
			mvceditor::ResourceClass resource = resourceFinder->GetResourceMatch(j);
			if (!IsDirty(resource, resourceFinder)) {
				matches.push_back(resource);
			}
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

void mvceditor::ResourceUpdateClass::ExpressionCompletionMatches(const wxString& fileName, const mvceditor::SymbolClass& parsedExpression, int expressionPos, 
													 mvceditor::ResourceFinderClass* resourceFinder, std::vector<UnicodeString>& autoCompleteList) {
	std::map<wxString, mvceditor::SymbolTableClass*>::iterator itSymbols = SymbolTables.find(fileName);
	if (itSymbols != SymbolTables.end()) {
		mvceditor::SymbolTableClass* symbolTable = itSymbols->second;
		if (symbolTable) {
			symbolTable->ExpressionCompletionMatches(parsedExpression, expressionPos, Iterator(resourceFinder), autoCompleteList);
		}
	}
}

void mvceditor::ResourceUpdateClass::ResourceMatches(const wxString& fileName, const mvceditor::SymbolClass& parsedExpression, int expressionPos, 
													 mvceditor::ResourceFinderClass* resourceFinder, std::vector<mvceditor::ResourceClass>& matches) {
	std::map<wxString, mvceditor::SymbolTableClass*>::iterator itSymbols = SymbolTables.find(fileName);
	if (itSymbols != SymbolTables.end()) {
		mvceditor::SymbolTableClass* symbolTable = itSymbols->second;
		if (symbolTable) {
			symbolTable->ResourceMatches(parsedExpression, expressionPos, Iterator(resourceFinder), matches);
		}
	}
}

std::vector<UnicodeString> mvceditor::ResourceUpdateClass::GetVariablesInScope(const wxString& fileName, int pos, const UnicodeString& code) {
	std::vector<UnicodeString> vars;
	std::map<wxString, mvceditor::SymbolTableClass*>::iterator itSymbols = SymbolTables.find(fileName);
	if (itSymbols != SymbolTables.end()) {
		mvceditor::SymbolTableClass* symbolTable = itSymbols->second;
		symbolTable->CreateSymbols(code);
		vars = symbolTable->GetVariablesInScope(pos);
	}
	return vars;
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


wxThreadError mvceditor::ResourceUpdateThreadClass::StartBackgroundUpdate(const wxString& fileName, const UnicodeString& code, bool isNew) {
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

void* mvceditor::ResourceUpdateThreadClass::Entry() {
	Worker.Update(CurrentFileName, CurrentCode, CurrentFileIsNew);
	
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