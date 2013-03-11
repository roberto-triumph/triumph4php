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
#ifndef __MVCEDITOR_CONFIGDETECTORACTIONCLASS_H__
#define __MVCEDITOR_CONFIGDETECTORACTIONCLASS_H__

#include <actions/ActionClass.h>
#include <widgets/ProcessWithHeartbeatClass.h>
#include <queue>

namespace mvceditor {

/**
 * the set of parameters that will be used for each 
 * external config detector PHP script call. All of 
 * these params are required with the exception of
 * OutputDbFileName.
 */
class ConfigTagDetectorParamsClass {

public:

	/**
	 * location to the php executable (php.exe / php)
	 * this is usually retrieved from mvceditor::EnvironmentClass
	 */
	wxString PhpExecutablePath;

	/**
	 * the base location of the php detector scripts. this is used as the 
	 * include path so that both local detectors and global detectors
	 * can access the php detector sources (since local detectors and global
	 * detectors are located in separate directories, we need to use
	 * include_path)
	 */
	wxFileName PhpIncludePath;

	/**
	 * The actual PHP script to run.
	 */
	wxFileName ScriptName;

	/**
	 * Argument to the config detector script; the base directory of the project to scan
	 */
	wxFileName SourceDir;

	/**
	 * Argument to the URL detector script; the location of th detectors cache SQLite db.
	 * this db is created when the project is indexed and can be accessed via the 
	 * GlobalsClass::Projects list. This argument is optional; it can be empty in which
	 * case the script outputs to STDOUT; leaving this argument empty is useful
	 * for testing url detectors; so that the results are visible in the console wthout
	 * needing to query the SQLite db.
	 */
	wxString OutputDbFileName;

	ConfigTagDetectorParamsClass();

	/**
	 * build the command line to be executed for each url detector
	 */
	wxString BuildCmdLine() const;
};

/**
 * This class will run all config detectors across all enabled projects. This means that there is
 * one external process execution for each project source directory / config detector combination
 */
class ConfigTagDetectorActionClass : public mvceditor::ActionClass {

public:

	ConfigTagDetectorActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);

	bool Init(mvceditor::GlobalsClass& globals);

	bool DoAsync();

	wxString GetLabel() const;

	void BackgroundWork();

private:

	/**
	 * used to run the external tag detector PHP script
	 */
	mvceditor::ProcessWithHeartbeatClass Process;

	/**
	 * we will perform one external call for each item in this
	 * queue
	 */
	std::queue<mvceditor::ConfigTagDetectorParamsClass> ParamsQueue;

	/**
	 * pop the next set of params from the queue and call the php database 
	 * detector.
	 */
	void NextDetection();

	/**
	 * @return all of the PHP Config detector scripts. These can be either ones
	 * provided by default or ones created by the user.
	 */
	std::vector<wxString> DetectorScripts();

	void OnProcessComplete(wxCommandEvent& event);

	void OnProcessInProgress(wxCommandEvent& event);

	void OnProcessFailed(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()

};

/**
 * This class will prime the detected Config cache with all of the
 * enabled projects. The Config cache will be primed; although
 * it will be primed with the existing cache file which may be
 * stale. 
 */
class ConfigDetectorInitActionClass : public mvceditor::InitializerActionClass {

public:

	ConfigDetectorInitActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);

	void Work(mvceditor::GlobalsClass& globals);

	wxString GetLabel() const;
};

}

#endif