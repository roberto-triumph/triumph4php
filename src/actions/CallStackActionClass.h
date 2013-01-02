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
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef __MVCEDITOR_CALLSTACKACTIONCLASS_H__
#define __MVCEDITOR_CALLSTACKACTIONCLASS_H__

#include <actions/ActionClass.h>
#include <php_frameworks/CallStackClass.h>

namespace mvceditor {

/**
 * Generates the call stack file in a background thread. This class
 * needs as input the starting entry point from which to start
 * recursing down the call stack. The class will generate a
 * EVENT_CALL_STACK_COMPLETE event as well
 * 
 */
class CallStackActionClass : public mvceditor::ActionClass {

public:

	/**
	 * @param runningThreads will be notified of EVENT_WORK_* events and EVENT_CALL_STACK_COMPLETE events as well
	 */
	CallStackActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);
	
	/**
	 * The SetCallStackStart method needs to
	 * be called before Init() so that we can know where to start iterating code from.
	 */
	bool Init(mvceditor::GlobalsClass& globals);
	
	/**
	 * The file, class, and method where to start the call stack recursion.
	 * This is usually a controller action. This method should be called before
	 * Init()
	 */
	bool SetCallStackStart(const wxFileName& fileName, const UnicodeString& className, 
		const UnicodeString& methodName, const wxFileName& detectorDbFileName);

	wxString GetLabel() const;

	void BackgroundWork();
	
private:

	/**
	 * This is used by the call stack class to find out the file name (full paths) of the
	 * classes / methods.
	 */
	mvceditor::ResourceCacheClass ResourceCache;
	
	/**
	 * Used to generate the call stack file (file of all function calls of a URL); call stack 
	 * file is required by the ViewInfos detector
	 */
	mvceditor::CallStackClass CallStack;
	
	/**
	 * The results of the call stack will be written to this file.
	 * This class will pick the file name (it will be a temporary file).
	 */
	wxFileName DetectorDbFileName;

	/**
	 * The file, class, and method where to start the call stack recursion.
	 * This is usually a controller action
	 */
	wxFileName StartFileName;
	UnicodeString StartClassName;
	UnicodeString StartMethodName;

	/**
	 * Need to know the PHP version of code so that we know which parser
	 * to use.
	 */
	pelet::Versions Version;
};

}

#endif