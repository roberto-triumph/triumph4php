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
#include <globals/Events.h>
#include <algorithm>

mvceditor::EventSinkClass::EventSinkClass() 
	: Handlers() {
}

void mvceditor::EventSinkClass::PushHandler(wxEvtHandler *handler) {
	Handlers.push_back(handler);
}

void mvceditor::EventSinkClass::RemoveAllHandlers() {
	Handlers.clear();
}

void mvceditor::EventSinkClass::Publish(wxEvent& event) {
	for (size_t i = 0; i < Handlers.size(); ++i) {
		
		// wont use wxPostEvent for now
		// using wxPostEvent would cause any Popup menus triggered in the 
		// handlers to not work correctly in linux
		Handlers[i]->ProcessEvent(event);
	}
}


mvceditor::CodeControlEventClass::CodeControlEventClass(wxEventType type, mvceditor::CodeControlClass* codeControl)
	: wxEvent(wxID_ANY, type)
	, CodeControl(codeControl) {

}

mvceditor::CodeControlClass* mvceditor::CodeControlEventClass::GetCodeControl() const {
	return CodeControl;
}

wxEvent* mvceditor::CodeControlEventClass::Clone() const {
	mvceditor::CodeControlEventClass* newEvt = new mvceditor::CodeControlEventClass(GetEventType(), CodeControl);
	return newEvt;
}

const wxEventType mvceditor::EVENT_APP_READY = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_EXIT = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_FILE_OPENED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_FILE_CREATED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_FILE_NEW = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_FILE_SAVED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_FILE_CLOSED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_PREFERENCES_SAVED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_PREFERENCES_EXTERNALLY_UPDATED = wxNewEventType();
const wxEventType mvceditor::EVENT_CMD_FILE_OPEN = wxNewEventType();
const wxEventType mvceditor::EVENT_CMD_RUN_COMMAND = wxNewEventType();

const long mvceditor::ID_TOOLS_NOTEBOOK = 1003;
const long mvceditor::ID_OUTLINE_NOTEBOOK = 1002;

/*
 *ATTN: this is hardcoded because of the way that the notebook is created
 * within the MainFrameForms (created by wxFormBuilder)
 */
const long mvceditor::ID_CODE_NOTEBOOK = 1001;