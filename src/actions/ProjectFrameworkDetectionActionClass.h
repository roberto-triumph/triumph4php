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
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef __MVCEDITOR_PROJECTFRAMEWORKDETECTIONACTIONCLASS_H__
#define __MVCEDITOR_PROJECTFRAMEWORKDETECTIONACTIONCLASS_H__

#include <php_frameworks/FrameworkDetectorClass.h>
#include <widgets/ThreadWithHeartbeatClass.h>
#include <actions/ProjectResourceActionClass.h>
#include <actions/SqlMetaDataActionClass.h>
#include <wx/string.h>
#include <wx/event.h>

namespace mvceditor {


class ProjectFrameworkDetectionActionClass : public ActionClass {

public:

	ProjectFrameworkDetectionActionClass(mvceditor::RunningThreadsClass& runningThread, int eventId);

	~ProjectFrameworkDetectionActionClass();

	bool Init(mvceditor::GlobalsClass& globals);

	bool DoAsync();

	wxString GetLabel() const;

	void BackgroundWork();

private:

	void OnFrameworkDetectionComplete(wxCommandEvent& event);
	void OnFrameworkDetectionFailed(wxCommandEvent& event);
	void OnFrameworkDetectionInProgress(wxCommandEvent& event);
	void OnFrameworkFound(mvceditor::FrameworkFoundEventClass& event);

	/**
	 * This object will be used to detct the various PHP framework artifacts (resources,
	 * database connections, route URLs)
	 */
	mvceditor::PhpFrameworkDetectorClass PhpFrameworks;

	/**
	 * a 'queue' of folders to perform framework detection on
	 */
	std::vector<wxString> DirectoriesToDetect;

	/**
	 * the PhpFrameworks instance needs this to find out the PHP executable location
	 */
	mvceditor::EnvironmentClass Environment;
};

}

#endif