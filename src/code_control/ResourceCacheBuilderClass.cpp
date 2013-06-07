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
#include <code_control/ResourceCacheBuilderClass.h>
#include <globals/Assets.h>
#include <globals/GlobalsClass.h>

mvceditor::WorkingCacheBuilderClass::WorkingCacheBuilderClass(
															mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: ActionClass(runningThreads, eventId)
	, TagCacheDbFileName()
	, Code()
	, FileName() 
	, FileIdentifier()
	, Version(pelet::PHP_53)
	, FileIsNew(true) {
}
	
void mvceditor::WorkingCacheBuilderClass::Update(mvceditor::GlobalsClass& globals,
												 const wxString& fileName, 
												 const wxString& fileIdentifier,
												 const UnicodeString& code, bool isNew, pelet::Versions version) {
	
	// make sure these is are deep copies
	TagCacheDbFileName.Assign(globals.TagCacheDbFileName);
	Code = code;
	FileName = fileName.c_str();
	FileIdentifier = fileIdentifier.c_str();
	Version = version;
	FileIsNew = isNew;		
}

void mvceditor::WorkingCacheBuilderClass::BackgroundWork() {
	if (!FileIdentifier.IsEmpty()) {

		// make sure to use the local variables and not the class ones
		// since this code is outside the mutex
		// even if code is empty, lets create a working cache so that the 
		// file is registered and code completion works
		mvceditor::GlobalCacheClass* globalCache = new mvceditor::GlobalCacheClass();
		std::vector<wxString> phpFileExtensions;
		wxFileName wxf(FileName);
		phpFileExtensions.push_back(wxf.GetFullName());
		std::vector<wxString> miscFileExtensions;
		globalCache->InitGlobalTag(TagCacheDbFileName, phpFileExtensions, miscFileExtensions, Version);
		if (FileIsNew) {

			// new files will not have a name, use the identifier as the name
			FileName = FileIdentifier;
		}
		mvceditor::WorkingCacheClass* workingCache = new mvceditor::WorkingCacheClass();
		workingCache->Init(FileName, FileIdentifier, FileIsNew, Version, true);
		bool good = workingCache->Update(Code);
		if (good && !IsCancelled()) {

			// only send the event if the code passes the lint check
			// otherwise we will delete a good symbol table, we want auto completion
			// to work even if the code is broken
			// PostEvent will set the correct event Id
			mvceditor::WorkingCacheCompleteEventClass evt(wxID_ANY, FileName, FileIdentifier, workingCache);
			PostEvent(evt);
		}
		else {
			// we still own the pointer since we did not send the event
			delete workingCache;
		}
		delete globalCache;
	}
}

wxString mvceditor::WorkingCacheBuilderClass::GetLabel() const {
	return wxT("Working Tag Cache");
}