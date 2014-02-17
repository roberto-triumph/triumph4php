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
#include <actions/GlobalActionClass.h>

t4p::GlobalActionClass::GlobalActionClass(t4p::RunningThreadsClass& runningThreads, int eventId)
	: ActionClass(runningThreads, eventId) {

}

bool t4p::GlobalActionClass::DoAsync() {
	return true;
}

t4p::InitializerGlobalActionClass::InitializerGlobalActionClass(t4p::RunningThreadsClass& runningThreads, int eventId)
	: GlobalActionClass(runningThreads, eventId) {

}

bool t4p::InitializerGlobalActionClass::Init(t4p::GlobalsClass& globals) {
	Work(globals);
	
	t4p::ActionEventClass evt(GetEventId(), t4p::EVENT_ACTION_COMPLETE, wxT(""));
	PostEvent(evt);
	return true;
}


bool t4p::InitializerGlobalActionClass::DoAsync() {
	return false;
}

void t4p::InitializerGlobalActionClass::BackgroundWork() {
	
}