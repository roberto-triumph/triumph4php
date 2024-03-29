/**
 * @copyright  2012 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
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
 */
#ifndef SRC_ACTIONS_URLTAGDETECTORACTIONCLASS_H_
#define SRC_ACTIONS_URLTAGDETECTORACTIONCLASS_H_

#include <queue>
#include <vector>
#include "actions/GlobalActionClass.h"
#include "widgets/ProcessWithHeartbeatClass.h"

namespace t4p {
/**
 * the set of parameters that will be used for each
 * external url detector PHP script call. All of
 * these params are required with the exception of
 * OutputDbFileName.
 */
class UrlTagDetectorParamsClass {
 public:
    /**
     * location to the php executable (php.exe / php)
     * this is usually retrieved from t4p::EnvironmentClass
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
     * Argument to the URL detector script; the base directory of the project to scan
     */
    wxFileName SourceDir;

    /**
     * Argument to the URL detector script; the location of the resource cache SQLite db.
     * this db is created when the project is indexed and can be accessed via the
     * GlobalsClass::Projects list
     */
    wxFileName ResourceDbFileName;

    /**
     * Argument to the URL detector script; the base URL of the project in question. This is
     * usually calculated with the help of ApacheClass.
     * Examples: "http://localhost" ; "http://dev.localhost"
     */
    wxString RootUrl;

    /**
     * Argument to the URL detector script; the location of th detectors cache SQLite db.
     * this db is created when the project is indexed and can be accessed via the
     * GlobalsClass::Projects list. This argument is optional; it can be empty in which
     * case the script outputs to STDOUT; leaving this argument empty is useful
     * for testing url detectors; so that the results are visible in the console wthout
     * needing to query the SQLite db.
     */
    wxString OutputDbFileName;

    UrlTagDetectorParamsClass();

    /**
     * build the command line to be executed for each url detector
     */
    wxString BuildCmdLine() const;
};

/**
 * This class will run all url detectors across all enabled projects. This means that there is
 * one external process execution for each project source directory / url detector combination
 */
class UrlTagDetectorActionClass : public wxEvtHandler, public t4p::GlobalActionClass {
 public:
    UrlTagDetectorActionClass(t4p::RunningThreadsClass& runningThreads, int eventId);

    /**
     * @return boolean false if php is not installed in the system
     */
    bool Init(t4p::GlobalsClass& globals);

    bool DoAsync();

    wxString GetLabel() const;

    void BackgroundWork();

 private:
    /**
     * used to run the external url detector PHP script
     */
    t4p::ProcessWithHeartbeatClass Process;

    /**
     * we will perform one external call for each item in this
     * queue
     */
    std::queue<t4p::UrlTagDetectorParamsClass> ParamsQueue;

    /**
     * pop the next set of params from the queue and call the php url
     * detector.
     * @return bool TRUE if the detection was started succesfully
     */
    bool NextDetection();

    /**
     * @return all of the PHP URL detector scripts. These can be either ones
     * provided by default or ones created by the user.
     */
    std::vector<wxString> DetectorScripts();

    void OnProcessComplete(wxCommandEvent& event);

    void OnProcessFailed(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};
}  // namespace t4p

#endif  // SRC_ACTIONS_URLTAGDETECTORACTIONCLASS_H_
