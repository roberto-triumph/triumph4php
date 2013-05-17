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
#ifndef __MVCEDITOR_FILEMODFIIEDCHECKACTIONCLASS_H__
#define __MVCEDITOR_FILEMODFIIEDCHECKACTIONCLASS_H__

#include <actions/ActionClass.h>
#include <wx/filename.h>
#include <wx/datetime.h>
#include <wx/event.h>

namespace mvceditor {

/**
 * Class that holds a file along with its modified time. The modified time
 * is saved in a variable so that we can tell when it changes.
 */
class FileModifiedTimeClass {
	
public:

	wxFileName FileName;

	wxDateTime ModifiedTime;
	
	FileModifiedTimeClass();

	FileModifiedTimeClass(const mvceditor::FileModifiedTimeClass& src);

	void Copy(const mvceditor::FileModifiedTimeClass& src);
};

/**
 * event of the results of the file checks. lists the files that
 * have been modified or deleted externally
 */
class FilesModifiedEventClass : public wxEvent {

public:
	
	/**
	 * files that have been modified externally (outside the editor)
	 */
	std::vector<wxFileName> Modified;

	/**
	 * the new modification times for the files. this is parallel to
	 * Modified vector (index 0 of ModifiedTimes is the modified time for
	 * file at index 0 of Modified)
	 */
	std::vector<wxDateTime> ModifiedTimes;

	/**
	 * files that have been deleted externally (outside the editor)
	 */
	std::vector<wxFileName> Deleted;

	FilesModifiedEventClass(int eventId, const std::vector<wxFileName>& modified, 
		const std::vector<wxDateTime>& modifiedTimes,
		const std::vector<wxFileName>& deleted);

	wxEvent* Clone() const;
};

extern const wxEventType EVENT_FILES_EXTERNALLY_MODIFIED;

/**
 * A class that checks the file modified times and reports whether files have been
 * deleted or modified externally.
 * This class generates a EVENT_ACTION_FILES_MODIFIED with the results of the file checks.
 * note that even if files have not been externally modified the event is still posted.
 */
class FileModifiedCheckActionClass : public mvceditor::ActionClass {

public:

	FileModifiedCheckActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);

	/**
	 * set the files to be checked.
	 */
	void SetFiles(const std::vector<mvceditor::FileModifiedTimeClass>& files); 

	/**
	 * performs the file checks in a background thread and POSTs the results
	 */
	void BackgroundWork();

	/**
	 * @return wxString a short description of this action.
	 */
	wxString GetLabel() const;

private:

	std::vector<mvceditor::FileModifiedTimeClass> FilesToCheck;
};

}

typedef void (wxEvtHandler::*FilesModifiedEventClassFunction)(mvceditor::FilesModifiedEventClass&);

#define EVT_FILES_EXTERNALLY_MODIFIED_COMPLETE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_FILES_EXTERNALLY_MODIFIED, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( FilesModifiedEventClassFunction, & fn ), (wxObject *) NULL ),

#endif
