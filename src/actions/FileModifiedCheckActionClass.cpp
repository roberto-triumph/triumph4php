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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <actions/FileModifiedCheckActionClass.h>


mvceditor::FileModifiedTimeClass::FileModifiedTimeClass()
: FileName()
, ModifiedTime() {

}
	
mvceditor::FileModifiedTimeClass::FileModifiedTimeClass(const mvceditor::FileModifiedTimeClass& src)
: FileName()
, ModifiedTime() {
	Copy(src);
}

mvceditor::FilesModifiedEventClass::FilesModifiedEventClass(int eventId, const std::vector<wxFileName>& modified, 
															const std::vector<wxDateTime>& modifiedTimes,
															const std::vector<wxFileName>& deleted)
: wxEvent(eventId, mvceditor::EVENT_FILES_EXTERNALLY_MODIFIED)
, Modified(modified)
, ModifiedTimes(modifiedTimes)
, Deleted(deleted) {
	
}

wxEvent* mvceditor::FilesModifiedEventClass::Clone() const {
	return new mvceditor::FilesModifiedEventClass(GetId(), Modified, ModifiedTimes, Deleted);
}

void mvceditor::FileModifiedTimeClass::Copy(const mvceditor::FileModifiedTimeClass& src) {	
	
	// make these copies thread-safe
	FileName.Assign(src.FileName.GetFullPath());
	ModifiedTime = src.ModifiedTime;
}


mvceditor::FileModifiedCheckActionClass::FileModifiedCheckActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
: ActionClass(runningThreads, eventId)
, FilesToCheck() {

}

void mvceditor::FileModifiedCheckActionClass::SetFiles(const std::vector<mvceditor::FileModifiedTimeClass>& files) {
	FilesToCheck = files;
}

void mvceditor::FileModifiedCheckActionClass::BackgroundWork() {
	std::vector<wxFileName> filesModified;
	std::vector<wxDateTime> modifiedTimes;
	std::vector<wxFileName> filesDeleted;
	std::vector<mvceditor::FileModifiedTimeClass>::const_iterator file;
	for (file = FilesToCheck.begin(); !IsCancelled() && file != FilesToCheck.end(); ++file) {
		bool exists = file->FileName.FileExists();
		wxDateTime modifiedDateTime;
		if (exists) {
			modifiedDateTime = file->FileName.GetModificationTime();
			if (modifiedDateTime.IsValid() && modifiedDateTime.IsLaterThan(file->ModifiedTime)) {
				filesModified.push_back(file->FileName);
				modifiedTimes.push_back(modifiedDateTime);
			}
		}
		else {
			filesDeleted.push_back(file->FileName);
		}
	}
	if (!IsCancelled()) {

		// PostEvent() will set the correct id
		mvceditor::FilesModifiedEventClass evt(wxID_ANY, filesModified, modifiedTimes, filesDeleted);
		PostEvent(evt);
	}
}

wxString mvceditor::FileModifiedCheckActionClass::GetLabel() const {
	return wxT("Checking File Modifications");
}

const wxEventType mvceditor::EVENT_FILES_EXTERNALLY_MODIFIED = wxNewEventType();