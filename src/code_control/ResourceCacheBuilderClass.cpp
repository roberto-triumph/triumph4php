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

mvceditor::WorkingCacheBuilderClass::WorkingCacheBuilderClass(
															mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: ActionClass(runningThreads, eventId)
	, Code() 
	, FileName() 
	, FileIdentifier()
	, FileIsNew(true) 
	, Version(pelet::PHP_53) {
}
	
void mvceditor::WorkingCacheBuilderClass::Update(const wxString& fileName, 
												 const wxString& fileIdentifier,
												 const UnicodeString& code, bool isNew, pelet::Versions version) {
	Code = code;

	// make sure this is a copy
	FileName = fileName.c_str();
	FileIdentifier = fileIdentifier.c_str();
	FileIsNew = isNew;
	Version = version;	
}

void mvceditor::WorkingCacheBuilderClass::BackgroundWork() {
	if (!FileIdentifier.IsEmpty()) {

		// make sure to use the local variables and not the class ones
		// since this code is outside the mutex
		// even if code is empty, lets create a working cache so that the 
		// file is registered and code completion works
		mvceditor::WorkingCacheClass* cache = new mvceditor::WorkingCacheClass();
		if (FileIsNew) {

			// new files will not have a name, use the identifier as the name
			FileName = FileIdentifier;
		}
		cache->InitWorkingTag(mvceditor::TagCacheWorkingAsset());
		cache->Init(FileName, FileIdentifier, FileIsNew, Version, false);
		bool good = cache->Update(Code);
		if (good && !IsCancelled()) {

			// only send the event if the code passes the lint check
			// otherwise we will delete a good symbol table, we want auto completion
			// to work even if the code is broken
			// PostEvent will set the correct event Id
			mvceditor::WorkingCacheCompleteEventClass evt(wxID_ANY, FileName, FileIdentifier, cache);
			PostEvent(evt);
		}
		else {
			// we still own the pointer since we did not send the event
			delete cache;
		}
	}
}

wxString mvceditor::WorkingCacheBuilderClass::GetLabel() const {
	return wxT("Working Tag Cache");
}