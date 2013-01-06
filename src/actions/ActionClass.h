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
#ifndef __MVCEDITOR_ACTIONCLASS_H__
#define __MVCEDITOR_ACTIONCLASS_H__

#include <widgets/ThreadWithHeartbeatClass.h>
#include <globals/GlobalsClass.h>

namespace mvceditor {

/**
 * An action is any short or long-lived process that reads Global data structures
 * and performs an action. 
 * An action is an asynchronous operation. Take care when designing the class
 * so that any data structures are copied instead of referenced. 
 *
 * Pay special attention to wxString variables, as the default wxString
 * assignment operator and copy constructors are NOT thread-safe (produce
 * shallow copies)
 */
class ActionClass : public mvceditor::ThreadWithHeartbeatClass {

public:

	ActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);

	virtual ~ActionClass();

	virtual bool Init(mvceditor::GlobalsClass& globals) = 0;

	virtual wxString GetLabel() const = 0;

	virtual bool DoAsync();

	void SetStatus(const wxString& status);

	wxString GetStatus();

private:

	wxString Status;

	wxMutex Mutex;

};

/**
 * A specialization of ActionClass; these kinds of actions are
 * small, quick actions that modify GlobalsClass in a specific 
 * manner. 
 */
class InitializerActionClass : public mvceditor::ActionClass {

public:

	InitializerActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId);

	bool Init(mvceditor::GlobalsClass& globals);

	virtual void Work(mvceditor::GlobalsClass& globals) = 0;

	void BackgroundWork();

	bool DoAsync();

};

/**
 * event identifiers for all actions. These should be used
 * as the eventIDs for the corresponding actions unless 
 * it is not possible.
 */
extern const int ID_EVENT_ACTION_GLOBAL_CACHE_INIT;
extern const int ID_EVENT_ACTION_GLOBAL_CACHE;
extern const int ID_EVENT_ACTION_SQL_METADATA_INIT;
extern const int ID_EVENT_ACTION_SQL_METADATA;
extern const int ID_EVENT_ACTION_URL_DETECTOR_INIT;
extern const int ID_EVENT_ACTION_URL_DETECTOR;
extern const int ID_EVENT_ACTION_TEMPLATE_FILE_DETECTOR;
extern const int ID_EVENT_ACTION_TAG_DETECTOR_INIT;
extern const int ID_EVENT_ACTION_TAG_DETECTOR;
extern const int ID_EVENT_ACTION_GLOBAL_CACHE_WIPE;

}

#endif