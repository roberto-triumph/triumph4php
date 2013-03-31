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

#ifndef __ACTIONTESTFIXTURECLASS_H__ 
#define __ACTIONTESTFIXTURECLASS_H__ 

#include <wx/event.h>
#include <wx/filename.h>
#include <globals/GlobalsClass.h>
#include <widgets/ThreadWithHeartbeatClass.h>

/**
 * Note: because this class also derives from wxEvtHandler, this class
 * must be the first class in the inheritance chain.
 */
class ActionTestFixtureClass : public wxEvtHandler {

public:

	mvceditor::RunningThreadsClass RunningThreads;

	/**
	 * Since ActionClass::Init() needs a GlobalsClass parameter
	 */
	mvceditor::GlobalsClass Globals;

	ActionTestFixtureClass();

	virtual ~ActionTestFixtureClass();

	/**
	 * Set the directory location for the tag db files
	 * @param cacheDir the directory to place the cache db files
	 */
	void InitTagCache(const wxString& cacheDir);
	
	/**
	 * Creates a project and adds it to the end of Globals.Projects
	 * Only the data structure is created. File system is not touched
	 * at all. Any source dir or db cache files are NOT created.
	 *
	 * @param sourceDir the directory that contains source files (php)
	 */
	void CreateProject(const wxFileName& sourceDir);
};

#endif