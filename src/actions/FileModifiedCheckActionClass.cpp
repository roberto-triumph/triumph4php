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
#include <globals/FileName.h>

t4p::FileModifiedTimeClass::FileModifiedTimeClass()
: FileName()
, ModifiedTime() {

}

t4p::FileModifiedTimeClass::FileModifiedTimeClass(const t4p::FileModifiedTimeClass& src)
: FileName()
, ModifiedTime() {
	Copy(src);
}

void t4p::FileModifiedTimeClass::Copy(const t4p::FileModifiedTimeClass& src) {

	// make these copies thread-safe
	FileName = t4p::FileNameCopy(src.FileName);
	ModifiedTime = src.ModifiedTime;
}

t4p::FilesModifiedEventClass::FilesModifiedEventClass(int eventId, const std::vector<wxFileName>& modified,
															const std::vector<wxDateTime>& modifiedTimes,
															const std::vector<wxFileName>& deleted)
: wxEvent(eventId, t4p::EVENT_FILES_EXTERNALLY_MODIFIED)
, Modified()
, ModifiedTimes()
, Deleted() {
	Modified = t4p::DeepCopyFileNames(modified);
	ModifiedTimes = modifiedTimes;
	Deleted =  t4p::DeepCopyFileNames(deleted);
}

wxEvent* t4p::FilesModifiedEventClass::Clone() const {
	return new t4p::FilesModifiedEventClass(GetId(), Modified, ModifiedTimes, Deleted);
}

t4p::FileModifiedCheckActionClass::FileModifiedCheckActionClass(t4p::RunningThreadsClass& runningThreads, int eventId)
: ActionClass(runningThreads, eventId)
, FilesToCheck() {

}

void t4p::FileModifiedCheckActionClass::SetFiles(const std::vector<t4p::FileModifiedTimeClass>& files) {
	FilesToCheck = files;
}

void t4p::FileModifiedCheckActionClass::BackgroundWork() {
	std::vector<wxFileName> filesModified;
	std::vector<wxDateTime> modifiedTimes;
	std::vector<wxFileName> filesDeleted;
	std::vector<t4p::FileModifiedTimeClass>::const_iterator file;
	for (file = FilesToCheck.begin(); !IsCancelled() && file != FilesToCheck.end(); ++file) {
		bool exists = file->FileName.FileExists();
		wxDateTime modifiedDateTime;
		if (exists) {
			modifiedDateTime = file->FileName.GetModificationTime();
			if (modifiedDateTime.IsValid()) {

				// use time span, to compare in seconds and not milli/micro
				// seconds precision
				// also, consider files modified in the past as having changed.
				wxTimeSpan span =  modifiedDateTime.Subtract(file->ModifiedTime);
				if(span.GetSeconds() > 1 || span.GetSeconds() < -60) {
					filesModified.push_back(file->FileName);
					modifiedTimes.push_back(modifiedDateTime);
				}
			}
		}
		else {
			filesDeleted.push_back(file->FileName);
		}
	}
	if (!IsCancelled()) {

		// PostEvent() will set the correct id
		t4p::FilesModifiedEventClass evt(wxID_ANY, filesModified, modifiedTimes, filesDeleted);
		PostEvent(evt);
	}
}

wxString t4p::FileModifiedCheckActionClass::GetLabel() const {
	return wxT("Checking File Modifications");
}

const wxEventType t4p::EVENT_FILES_EXTERNALLY_MODIFIED = wxNewEventType();
