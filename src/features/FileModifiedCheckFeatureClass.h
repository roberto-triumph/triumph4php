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
#ifndef __T4P_FILEMODIFIEDCHECKFEATURECLASS_H__
#define __T4P_FILEMODIFIEDCHECKFEATURECLASS_H__

#include <features/FeatureClass.h>
#include <actions/FileModifiedCheckActionClass.h>
#include <wx/fswatcher.h>

namespace t4p {

/**
 * A feature that checks to see if any files that are currently
 * opened have been externally modified and / or deleted. If so, we
 * will prompt the reader to save and or reload them. 
 *
 * Note: attempted to read the EVENT_APP_FILE_RENAMED so that
 * we can show the user the rename (3-choice) prompt, but could
 * not successfully do it, since the fs watcher triggers the
 * event loop immediately, which calls the reactivate event and
 * then we perform the file check before we get the EVENT_APP_FILE_RENAMED
 * event (because this event is buffered)
 */
class FileModifiedCheckFeatureClass : public t4p::FeatureClass {

public:

	FileModifiedCheckFeatureClass(t4p::AppClass& app);

private:
	
	/**
	 * handle the files that are not part of the watched directories
	 * ie we will check the file modified times
	 */
	void OpenedCodeControlCheck();

	/**
	 * special handling for files that are open. For open files that were externally modified
	 * we will prompt the user to take action
	 */
	void HandleOpenedFiles(std::map<wxString, t4p::CodeControlClass*>& openedFiles, std::map<wxString, wxString>& pathsRenamed);

	/**
	 * prompt the user to reload modified files
	 */
	void FilesModifiedPrompt(std::map<wxString, t4p::CodeControlClass*>& filesToPrompt);

	/**
	 * prompt the user to save the deleted files
	 */
	void FilesDeletedPrompt(std::map<wxString, t4p::CodeControlClass*>& openedFiles, std::map<wxString, int>& deletedFiles);

	/**
	 * prompt the user to open or close the renamed files
	 */
	void FilesRenamedPrompt(std::map<wxString, t4p::CodeControlClass*>& openedFiles, std::map<wxString, wxString>& pathsRenamed);

	/**
	 * when the user puts this app in the foreground, check for
	 * file modifications.  maybe the user went to another editor
	 * and modified one of the files that is opened in triumph. Note that we
	 * will check the file modified times in a background thread
	 */
	void OnActivateApp(wxCommandEvent& event);

	/**
	 * After we check the file modified times in a background thread this
	 * method gets called. here we will prompt the user to reload/close the
	 * files that were modified outside of triumph
	 */
	void OnFileModifiedPollComplete(t4p::FilesModifiedEventClass& event);
	
	/**
	 * flag to prevent multiple modified dialogs during activate app event.
	 * needed for linux
	 */
	bool JustReactivated;
	
	DECLARE_EVENT_TABLE()

};

}

#endif