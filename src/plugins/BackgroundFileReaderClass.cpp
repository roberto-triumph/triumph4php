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
#include <plugins/BackgroundFileReaderClass.h>


mvceditor::BackgroundFileReaderClass::BackgroundFileReaderClass(wxEvtHandler& handler)
	: ThreadWithHeartbeatClass(handler)
	, DirectorySearch()
	, Mode(WALK) {
}

bool mvceditor::BackgroundFileReaderClass::Init(const wxString& path, mvceditor::DirectorySearchClass::Modes mode,
												bool doHiddenFiles) {
	Mode = WALK;
	return DirectorySearch.Init(path, mode, doHiddenFiles);
}

bool mvceditor::BackgroundFileReaderClass::InitMatched() {
	Mode = MATCHED;
	return !DirectorySearch.GetMatchedFiles().empty();
}

bool mvceditor::BackgroundFileReaderClass::StartReading(StartError& error) {
	error = NONE;
	bool ret = false;
	bool isAlive = IsRunning();
	if (!isAlive) {
		wxThreadError threadError = CreateSingleInstance();
		if (threadError == wxTHREAD_NO_RESOURCE) {
			error = NO_RESOURCES;
		}
		else if (threadError == wxTHREAD_RUNNING) {
			error = ALREADY_RUNNING;
		}
		else {
			GetThread()->Run();
			ret = true;
			SignalStart();
		}		
	}
	else {
		error = ALREADY_RUNNING;
	}
	return ret;
}

void mvceditor::BackgroundFileReaderClass::StopReading() {
	if (GetThread()) {
		GetThread()->Delete();
		m_thread = NULL;
	}
}

bool mvceditor::BackgroundFileReaderClass::TestDestroy() {
	bool ret = true;
	if (GetThread()) {
		ret = GetThread()->TestDestroy();
	}
	return ret;
}

void* mvceditor::BackgroundFileReaderClass::Entry() {
	bool isDestroy = false;
	if (GetThread()) {
		isDestroy = GetThread()->TestDestroy();
	}
	int counter = 0;
	if (Mode == WALK) {
		while(DirectorySearch.More() && !isDestroy) {
			bool res = FileRead(DirectorySearch);

			// signal that the background thread has finished one file
			counter++;
			wxCommandEvent singleEvent(EVENT_FILE_READ, wxNewId());
			
			// when isDestroy returns TRUE, must exit as soon as possible
			// for example, when app exists the ThreadWithHeartbeat class kills the
			// thread, many things are no longer valid
			// this IF block prevents crashes on app exit
			isDestroy = GetThread()->TestDestroy();
			if (!isDestroy) {
				singleEvent.SetInt(counter);
				singleEvent.SetClientData((void*) res);
				wxPostEvent(&Handler, singleEvent);
			}
		}
	}
	else if (Mode == MATCHED) {
		std::vector<wxString> matchedFiles = DirectorySearch.GetMatchedFiles();
		int count = matchedFiles.size();
		for (int i = 0; i < count && !isDestroy; i++) {
			FileMatch(matchedFiles[i]);

			// signal that the background thread has finished one file
			counter++;
			isDestroy = GetThread()->TestDestroy();
			if (!isDestroy) {
				wxCommandEvent singleEvent(EVENT_FILE_READ, wxNewId());
				singleEvent.SetInt(counter);
				wxPostEvent(&Handler, singleEvent);
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
		wxPostEvent(&Handler, endEvent);
		SignalEnd();
	}
	return 0;
}

const wxEventType mvceditor::EVENT_FILE_READ_COMPLETE = wxNewEventType();
const wxEventType mvceditor::EVENT_FILE_READ = wxNewEventType();