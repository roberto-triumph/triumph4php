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

mvceditor::WorkingCacheBuilderClass::WorkingCacheBuilderClass(
															mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: ThreadWithHeartbeatClass(runningThreads, eventId)
	, Mutex()
	, Semaphore(0 , 1)
	, CurrentCode() 
	, CurrentFileName() 
	, CurrentFileIdentifier()
	, CurrentFileIsNew(true) 
	, CurrentVersion(pelet::PHP_53) {
}

wxThreadError mvceditor::WorkingCacheBuilderClass::Init(wxThreadIdType& threadId) {
	wxThreadError error = CreateSingleInstance(threadId);
	return error;
}

void mvceditor::WorkingCacheBuilderClass::Wait() {
	Semaphore.Wait();
}
	
void mvceditor::WorkingCacheBuilderClass::Update(const wxString& fileName, 
												 const wxString& fileIdentifier,
												 const UnicodeString& code, bool isNew, pelet::Versions version) {
	
	// make sure to lock the mutex
	// in order to prevent Entry from reading them while we write to them
	wxMutexLocker lock(Mutex);
	wxASSERT(lock.IsOk());

	CurrentCode = code;

	// make sure this is a copy
	CurrentFileName = fileName.c_str();
	CurrentFileIdentifier = fileIdentifier.c_str();
	CurrentFileIsNew = isNew;
	CurrentVersion = version;	
}

void mvceditor::WorkingCacheBuilderClass::BackgroundWork() {
	while (!TestDestroy()) {
		UnicodeString code;
		wxString fileName;
		wxString fileIdentifier;
		bool isNew;
		pelet::Versions version;
		{

			// lock while we copy data into the current thread.
			wxMutexLocker lock(Mutex);
			wxASSERT(lock.IsOk());

			// use extract to perform a pure copy
			UErrorCode error = U_ZERO_ERROR;
			int32_t len = CurrentCode.length();
			UChar* buf = code.getBuffer(len);
			CurrentCode.extract(buf, CurrentCode.length(), error);
			code.releaseBuffer(len);

			fileName = CurrentFileName.c_str();
			fileIdentifier = CurrentFileIdentifier.c_str();
			isNew = CurrentFileIsNew;
			version = CurrentVersion;

			// cleanup.
			CurrentCode.truncate(0);
			CurrentFileName.resize(0);
			CurrentFileIdentifier.resize(0);
			CurrentFileIsNew = false;
		}

		if (!code.isEmpty()) {

			// make sure to use the local variables and not the class ones
			// since this code is outside the mutex
			mvceditor::WorkingCacheClass* cache = new mvceditor::WorkingCacheClass();
			cache->Init(fileName, fileIdentifier, isNew, version, false);
			bool good = cache->Update(code);
			if (good && !TestDestroy()) {

				// only send the event if the code passes the lint check
				// otherwise we will delete a good symbol table, we want auto completion
				// to work even if the code is broken
				// PostEvent will set the correct event Id
				mvceditor::WorkingCacheCompleteEventClass evt(wxID_ANY, fileName, fileIdentifier, cache);
				PostEvent(evt);
			}
			else {
				// we still own the pointer since we did not send the event
				delete cache;
			}
		}
		if (!TestDestroy()) {
			wxThread::Sleep(200);
		}
	}
	Semaphore.Post();
}
