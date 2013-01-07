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
 * @copyright  2009-20Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#include <actions/ActionClass.h>

mvceditor::ActionClass::ActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: ThreadWithHeartbeatClass(runningThreads, eventId) 
	, Status()
	, Mutex() {

}

mvceditor::ActionClass::~ActionClass() {

}

bool mvceditor::ActionClass::DoAsync() {
	return true;
}

void mvceditor::ActionClass::SetStatus(const wxString& status) {

	// make sure to synchronize since this may be called from multiple threads
	wxMutexLocker locker(Mutex);

	// make sure to copy, since this may be called from multiple threads
	Status.Clear();
	Status.Append(status.c_str());
}

wxString mvceditor::ActionClass::GetStatus() {

	// make sure to synchronize since this may be called from multiple threads
	wxMutexLocker locker(Mutex);
	return Status;
}

mvceditor::InitializerActionClass::InitializerActionClass(mvceditor::RunningThreadsClass& runningThreads, int eventId)
	: ActionClass(runningThreads, eventId) {

}

bool mvceditor::InitializerActionClass::Init(mvceditor::GlobalsClass& globals) {
	Work(globals);
	
	wxCommandEvent evt(mvceditor::EVENT_WORK_COMPLETE);
	PostEvent(evt);
	return true;
}


bool mvceditor::InitializerActionClass::DoAsync() {
	return false;
}

void mvceditor::InitializerActionClass::BackgroundWork() {
	
}

const int mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE_INIT = wxNewId();
const int mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE = wxNewId();
const int mvceditor::ID_EVENT_ACTION_SQL_METADATA_INIT = wxNewId();
const int mvceditor::ID_EVENT_ACTION_SQL_METADATA = wxNewId();
const int mvceditor::ID_EVENT_ACTION_URL_DETECTOR_INIT = wxNewId();
const int mvceditor::ID_EVENT_ACTION_URL_DETECTOR = wxNewId();
const int mvceditor::ID_EVENT_ACTION_TAG_DETECTOR_INIT = wxNewId();
const int mvceditor::ID_EVENT_ACTION_TAG_DETECTOR = wxNewId();
const int mvceditor::ID_EVENT_ACTION_GLOBAL_CACHE_WIPE = wxNewId();
const int mvceditor::ID_EVENT_ACTION_TEMPLATE_FILE_DETECTOR = wxNewId();
const int mvceditor::ID_EVENT_ACTION_CALL_STACK = wxNewId();