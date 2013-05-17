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
#ifndef __MVCEDITOR_FILEMODIFIEDCHECKFEATURECLASS_H__
#define __MVCEDITOR_FILEMODIFIEDCHECKFEATURECLASS_H__

#include <features/FeatureClass.h>
#include <actions/FileModifiedCheckActionClass.h>

namespace mvceditor {

/**
 * A small feature that checks to see if any files that are currently
 * opened have been externally modified and / or deleted. If so, we
 * will prompt the reader to save and or reload them.
 */
class FileModifiedCheckFeatureClass : public mvceditor::FeatureClass {

public:

	FileModifiedCheckFeatureClass(mvceditor::AppClass& app);


private:

	/**
	 * when the app starts then start the timer
	 */
	void OnAppReady(wxCommandEvent& event);

	/**
	 * when the app stops then stop the timer
	 */
	void OnAppExit(wxCommandEvent& event);

	/**
	 * when the timer is up then check for file modifications.
	 */
	void OnTimer(wxTimerEvent& event);

	/**
	 * prompt the user to reload modified files or save deleted files
	 */
	void OnFilesCheckComplete(mvceditor::FilesModifiedEventClass& event);

	/**
	 * prompt the user to reload modified files
	 */
	void OnFilesModified(mvceditor::FilesModifiedEventClass& event);

	/**
	 * prompt the user to save the deleted files
	 */
	void OnFilesDeleted(mvceditor::FilesModifiedEventClass& event);

	wxTimer Timer;

	DECLARE_EVENT_TABLE()

};

}

#endif