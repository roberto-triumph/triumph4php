/**
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
 */
#include "features/BackgroundFileReaderClass.h"
#include <vector>

t4p::BackgroundFileReaderClass::BackgroundFileReaderClass(t4p::RunningThreadsClass& runningThreads, int eventId)
	: ActionClass(runningThreads, eventId)
	, DirectorySearch()
	, Mode(WALK) {
}

bool t4p::BackgroundFileReaderClass::Init(const wxString& path, t4p::DirectorySearchClass::Modes mode,
												bool doHiddenFiles) {
	Mode = WALK;
	return DirectorySearch.Init(path, mode, doHiddenFiles);
}

bool t4p::BackgroundFileReaderClass::Init(std::vector<t4p::SourceClass> sources, t4p::DirectorySearchClass::Modes mode,
												bool doHiddenFiles) {
	Mode = WALK;
	return DirectorySearch.Init(sources, mode, doHiddenFiles);
}


bool t4p::BackgroundFileReaderClass::InitMatched(const std::vector<wxString>& matchedFiles) {
	Mode = MATCHED;
	MatchedFiles = matchedFiles;
	return !matchedFiles.empty();
}

void t4p::BackgroundFileReaderClass::BackgroundWork() {
	bool isDestroy = IsCancelled();
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
			isDestroy = IsCancelled();
			if (!isDestroy) {
				singleEvent.SetInt(counter);
				singleEvent.SetClientData(reinterpret_cast<void*>(res));
				PostEvent(singleEvent);
			}
		}
	} else if (Mode == MATCHED) {
		int count = MatchedFiles.size();
		for (int i = 0; i < count && !isDestroy; i++) {
			BackgroundFileMatch(MatchedFiles[i]);

			// signal that the background thread has finished one file
			counter++;
			isDestroy = IsCancelled();
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

const wxEventType t4p::EVENT_FILE_READ_COMPLETE = wxNewEventType();
const wxEventType t4p::EVENT_FILE_READ = wxNewEventType();
