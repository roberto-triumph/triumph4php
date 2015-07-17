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
 * @copyright  2015 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef T4P_FILEMODIFIEDCHECKVIEWCLASS_H
#define T4P_FILEMODIFIEDCHECKVIEWCLASS_H

#include <views/FeatureViewClass.h>
#include <actions/FileModifiedCheckActionClass.h>

namespace t4p {

// forward declaration, defined in another file
class FileModifiedCheckFeatureClass;

class FileModifiedCheckViewClass : public t4p::FeatureViewClass {

public:

	FileModifiedCheckViewClass(t4p::FileModifiedCheckFeatureClass& feature);
	
private:
	
	/**
	 * when the user puts this app in the foreground, check for
	 * file modifications.  maybe the user went to another editor
	 * and modified one of the files that is opened in triumph. Note that we
	 * will check the file modified times in a background thread
	 */
	void OnActivateApp(wxCommandEvent& event);

	/**
	 * handle the files that are open (being modified by the user)
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
	 * After we check the file modified times in a background thread this
	 * method gets called. here we will prompt the user to reload/close the
	 * files that were modified outside of triumph
	 */
	void OnFileModifiedPollComplete(t4p::FilesModifiedEventClass& event);
	
	t4p::FileModifiedCheckFeatureClass& Feature;
	
	/**
	 * flag to prevent multiple modified dialogs during activate app event.
	 * needed for linux
	 */
	bool JustReactivated;
	
	
	DECLARE_EVENT_TABLE()
};

}

#endif

