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
#include <Events.h>

mvceditor::EventSinkClass::EventSinkClass() 
	: Handlers() {
}

void mvceditor::EventSinkClass::PushHandler(wxEvtHandler *handler) {
	Handlers.push_back(handler);
}

void mvceditor::EventSinkClass::Publish(wxEvent& event) {
	for (size_t i = 0; i < Handlers.size(); ++i) {
		
		// wont use wxPostEvent for now
		// using wxPostEvent would cause any Popup menus triggered in the 
		// handlers to not work correctly in linux
		Handlers[i]->ProcessEvent(event);
	}
}


mvceditor::FileSavedEventClass::FileSavedEventClass(mvceditor::CodeControlClass* codeControl)
	: wxEvent(wxID_ANY, mvceditor::EVENT_PLUGIN_FILE_SAVED)
	, CodeControl(codeControl) {

}

mvceditor::CodeControlClass* mvceditor::FileSavedEventClass::GetCodeControl() const {
	return CodeControl;
}

wxEvent* mvceditor::FileSavedEventClass::Clone() const {
	mvceditor::FileSavedEventClass* newEvt = new mvceditor::FileSavedEventClass(CodeControl);
	return newEvt;
}

const wxEventType mvceditor::EVENT_PLUGIN_FILE_SAVED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_FILE_CLOSED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_PROJECT_OPENED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_PROJECT_CLOSED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_PROJECT_INDEXED = wxNewEventType();
const wxEventType mvceditor::EVENT_CMD_OPEN_PROJECT = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_PREFERENCES_UPDATED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_ENVIRONMENT_UPDATED = wxNewEventType();
const wxEventType mvceditor::EVENT_CMD_RE_INDEX = wxNewEventType();
const wxEventType mvceditor::EVENT_CMD_FILE_OPEN = wxNewEventType();
