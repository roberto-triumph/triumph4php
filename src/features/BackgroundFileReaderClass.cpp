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
#include <features/BackgroundFileReaderClass.h>


mvceditor::BackgroundFileReaderClass::BackgroundFileReaderClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: ThreadWithHeartbeatClass(runningThreads, eventId)
	, DirectorySearch()
	, Mode(WALK) {
}

bool mvceditor::BackgroundFileReaderClass::Init(const wxString& path, mvceditor::DirectorySearchClass::Modes mode,
												bool doHiddenFiles) {
	Mode = WALK;
	return DirectorySearch.Init(path, mode, doHiddenFiles);
}

bool mvceditor::BackgroundFileReaderClass::Init(std::vector<mvceditor::SourceClass> sources, mvceditor::DirectorySearchClass::Modes mode,
												bool doHiddenFiles) {
	Mode = WALK;
	return DirectorySearch.Init(sources, mode, doHiddenFiles);
}


bool mvceditor::BackgroundFileReaderClass::InitMatched(const std::vector<wxString>& matchedFiles) {
	Mode = MATCHED;
	MatchedFiles = matchedFiles;
	return !matchedFiles.empty();
}

bool mvceditor::BackgroundFileReaderClass::StartReading(StartError& error, wxThreadIdType& threadId) {
	error = NONE;
	bool ret = false;
	wxThreadError threadError = CreateSingleInstance(threadId);
	if (threadError == wxTHREAD_NO_RESOURCE) {
		error = NO_RESOURCES;
	}
	else if (threadError == wxTHREAD_RUNNING) {
		error = ALREADY_RUNNING;
	}
	else {
		ret = true;
	}
	return ret;
}

void mvceditor::BackgroundFileReaderClass::BackgroundWork() {
	bool isDestroy = TestDestroy();
	int counter = 0;
	if (Mode == WALK) {
		
		// careful to test for destroy first
		while (!isDestroy && DirectorySearch.More()) {
			bool res = BackgroundFileRead(DirectorySearch);

			// signal that the background thread has finished one file
			counter++;
			wxCommandEvent singleEvent(EVENT_FILE_READ, wxNewId());
			
			// when isDestroy returns TRUE, must exit as soon as possible
			// for example, when app exists the ThreadWithHeartbeat class kills the
			// thread, many things are no longer valid
			// this IF block prevents crashes on app exit
			isDestroy = TestDestroy();
			if (!isDestroy) {
				singleEvent.SetInt(counter);
				singleEvent.SetClientData((void*) res);
				PostEvent(singleEvent);
			}
		}
	}
	else if (Mode == MATCHED) {
		int count = MatchedFiles.size();
		for (int i = 0; i < count && !isDestroy; i++) {
			BackgroundFileMatch(MatchedFiles[i]);

			// signal that the background thread has finished one file
			counter++;
			isDestroy = TestDestroy();
			if (!isDestroy) {
				wxCommandEvent singleEvent(EVENT_FILE_READ, wxNewId());
				singleEvent.SetInt(counter);
				PostEvent(singleEvent);
			}
		}
	}
	
	// signal that the background thread has finished
	// when isDestroy returns TRUE, must exit as soon as possible
	// for example, when app exists the ThreadWithHeartbeat class kills the
	// thread, many things are no longer valid
	// this IF block prevents crashes on app exit
	if (!isDestroy) {
		wxCommandEvent endEvent(EVENT_FILE_READ_COMPLETE, wxNewId());
		endEvent.SetInt(Mode);
		PostEvent(endEvent);
	}
}

const wxEventType mvceditor::EVENT_FILE_READ_COMPLETE = wxNewEventType();
const wxEventType mvceditor::EVENT_FILE_READ = wxNewEventType();