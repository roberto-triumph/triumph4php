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
#ifndef __MVCEDITOR_TAGDETECTORACTIONCLASS_H__
#define __MVCEDITOR_TAGDETECTORACTIONCLASS_H__

#include <actions/ActionClass.h>
#include <widgets/ProcessWithHeartbeatClass.h>
#include <queue>

namespace mvceditor {

/**
 * the set of parameters that will be used for each 
 * external tag detector PHP script call. All of 
 * these params are required with the exception of
 * OutputDbFileName.
 */
class TagDetectorParamsClass {

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
	 * Argument to the URL detector script; the base directory of the project to scan
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

	TagDetectorParamsClass();

	/**
	 * build the command line to be executed for each url detector
	 */
	wxString BuildCmdLine() const;
};

}

#endif
