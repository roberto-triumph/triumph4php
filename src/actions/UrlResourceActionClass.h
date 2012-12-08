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
#ifndef __MVCEDITOR_URLRESOURCEACTIONCLASS_H__
#define __MVCEDITOR_URLRESOURCEACTIONCLASS_H__

#include <actions/ActionClass.h>
#include <queue>

namespace mvceditor {

class UrlResourceActionClass : public mvceditor::ActionClass {

public:

	UrlResourceActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);

	~UrlResourceActionClass();

	bool Init(mvceditor::GlobalsClass& globals);

	bool DoAsync();

	void BackgroundWork();

	wxString GetLabel() const;

private:

	/**
	 * When URL detection succeeds, propagate the event to the thread event handlers
	 */
	void OnUrlDetectionComplete(UrlDetectedEventClass& event);

	/**
	 * while the url detection is in progress, propagate the events up the
	 * running threads so that they ultimately get to the main frame
	 */
	void OnUrlDetectionInProgress(wxCommandEvent& event);

	/**
	 * If URL detection fails, then most likely this is an environment issue
	 * (PHP binary not found)
	 */
	void OnUrlDetectionFailed(wxCommandEvent& event);

	/**
	 * This object will be used to detcet the various PHP framework route URLs). 
	 */
	mvceditor::PhpFrameworkDetectorClass PhpFrameworks;

	/**
	 * we need to know what framework is being used so that we can know how to
	 * interpret the urls
	 */
	std::vector<mvceditor::FrameworkClass> DetectedFrameworks;

	/**
	 * the resource cache database files; each db file will be run through
	 * url detection.
	 */
	std::queue<wxString> ResourceDbFileNames;

	/**
	 * the root urls; each directory will be run through
	 * url detection.
	 */
	std::queue<wxString> RootUrls;

	/**
	 * the PhpFrameworks instance needs this to find out the PHP executable location
	 */
	mvceditor::EnvironmentClass Environment;

};

}

#endif