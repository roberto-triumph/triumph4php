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

mvceditor::ResourceUpdateThreadClass::ResourceUpdateThreadClass(wxEvtHandler& handler, int eventId)
	: ThreadWithHeartbeatClass(handler, eventId)
	, Finders()
	, CurrentCode() 
	, CurrentFileName()
	, BusyFinder(NULL) 
	, CurrentHandler(NULL) {
}

mvceditor::ResourceUpdateThreadClass::~ResourceUpdateThreadClass() {
	for (std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it =  Finders.begin(); it != Finders.end(); ++it) {
		delete it->second;
	}
}

bool mvceditor::ResourceUpdateThreadClass::CollectFullyQualifiedResourceFromAll(mvceditor::ResourceFinderClass* resourceFinder) {
	std::vector<mvceditor::ResourceFinderClass*> finders = Iterator(resourceFinder);
	bool found = false;
	for (size_t i = 0; i < finders.size(); ++i) {
		found |= finders[i]->CollectFullyQualifiedResource();
	}
	return found;
}

bool mvceditor::ResourceUpdateThreadClass::CollectNearMatchResourcesFromAll(mvceditor::ResourceFinderClass* resourceFinder) {
	std::vector<mvceditor::ResourceFinderClass*> finders = Iterator(resourceFinder);
	bool found = false;
	for (size_t i = 0; i < finders.size(); ++i) {
		found |= finders[i]->CollectNearMatchResources();
	}
	return found;
}

void* mvceditor::ResourceUpdateThreadClass::Entry() {
	BusyFinder->BuildResourceCacheForFile(CurrentFileName, CurrentCode);
	BusyFinder->EnsureSorted();
	
	wxCommandEvent evt(mvceditor::EVENT_WORK_COMPLETE, wxID_ANY);
	wxPostEvent(CurrentHandler, evt);
	
	// cleanup.
	// TODO do the string clearing methods actuall deallocate memory??
	BusyFinder = NULL;
	CurrentHandler = NULL;
	CurrentFileName.Clear();
	CurrentCode.remove();
	return 0;
}

bool mvceditor::ResourceUpdateThreadClass::IsDirty(const ResourceClass& resource) {
	bool ret = false;
	
	return ret;
}

bool mvceditor::ResourceUpdateThreadClass::PrepareAll(mvceditor::ResourceFinderClass* resourceFinder, const wxString& resource) {
	std::vector<mvceditor::ResourceFinderClass*> finders = Iterator(resourceFinder);
	bool prep = true;
	for (size_t i = 0; i < finders.size(); ++i) {
		prep &= finders[i]->Prepare(resource);
	}
	return prep;
}

void mvceditor::ResourceUpdateThreadClass::Register(const wxString& fileName, wxEvtHandler* handler) {
	std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it = Finders.find(fileName);
	if (it == Finders.end()) {
		
		// careful to not overwrite the resource finder pointer
		mvceditor::ResourceFinderClass* res = new mvceditor::ResourceFinderClass;
		Finders[fileName] = res;
		Handlers[fileName] = handler;
	}
	
}

void mvceditor::ResourceUpdateThreadClass::Unregister(const wxString& fileName) {
	std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it = Finders.find(fileName);
	if (it != Finders.end()) {
		delete it->second;
	}
	Finders.erase(fileName);
	Handlers.erase(fileName);
}

wxThreadError mvceditor::ResourceUpdateThreadClass::UpdateResources(const wxString& fileName, const UnicodeString& code) {
	wxThreadError err = Create();
	std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it = Finders.find(fileName);
	std::map<wxString, wxEvtHandler*>::iterator itHandler = Handlers.find(fileName);
	if (wxTHREAD_NO_ERROR == err && !GetThread()->IsRunning() && it != Finders.end() && itHandler != Handlers.end()) {
		BusyFinder = it->second;
		CurrentCode = code;
		CurrentFileName = fileName;
		CurrentHandler = itHandler->second;
		GetThread()->Run();
		SignalStart();
	}
	return err;
}

std::vector<mvceditor::ResourceFinderClass*> mvceditor::ResourceUpdateThreadClass::Iterator(mvceditor::ResourceFinderClass* resourceFinder) {
	std::vector<mvceditor::ResourceFinderClass*> finders;
	finders.push_back(resourceFinder);
	for (std::map<wxString, mvceditor::ResourceFinderClass*>::iterator it =  Finders.begin(); it != Finders.end(); ++it) {
		finders.push_back(it->second);
	}
	return finders;
}
