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

void mvceditor::EventSinkClass::Post(wxEvent& event) {
	for (size_t i = 0; i < Handlers.size(); ++i) {
		wxPostEvent(Handlers[i], event);
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

mvceditor::RenameEventClass::RenameEventClass(wxEventType type, const wxString& oldPath, const wxString& newPath)
: wxEvent(wxID_ANY, type)
, OldPath()
, NewPath() {
	if (mvceditor::EVENT_APP_FILE_RENAMED == type) {
		OldPath.Assign(oldPath);
		NewPath.Assign(newPath);
	}
	else if (mvceditor::EVENT_APP_DIR_RENAMED == type) {
		OldPath.AssignDir(oldPath);
		NewPath.AssignDir(newPath);
	}
}

wxEvent* mvceditor::RenameEventClass::Clone() const {
	if (mvceditor::EVENT_APP_DIR_RENAMED == GetEventType()) {
		return new mvceditor::RenameEventClass(GetEventType(), OldPath.GetPath(), OldPath.GetPath());
	}
	return new mvceditor::RenameEventClass(GetEventType(), OldPath.GetFullPath(), OldPath.GetFullPath());
}

mvceditor::OpenFileCommandEventClass::OpenFileCommandEventClass(const wxString& fullPath, int startingPos, int length)
: wxEvent(wxID_ANY, mvceditor::EVENT_CMD_FILE_OPEN) 
, FullPath(fullPath.c_str())
, StartingPos(startingPos)
, Length(length) {
	
}

wxEvent* mvceditor::OpenFileCommandEventClass::Clone() const {
	return new mvceditor::OpenFileCommandEventClass(FullPath, StartingPos, Length);
}


mvceditor::OpenDbTableCommandEventClass::OpenDbTableCommandEventClass(wxEventType type, const wxString& dbTable, 
	const wxString& connectionHash)
: wxEvent(wxID_ANY, type)
, DbTableName(dbTable.c_str())
, ConnectionHash(connectionHash.c_str()) {
}

wxEvent* mvceditor::OpenDbTableCommandEventClass::Clone() const {
	mvceditor::OpenDbTableCommandEventClass* evt = new mvceditor::OpenDbTableCommandEventClass(
		GetEventType(), DbTableName, ConnectionHash);
	return evt;
}


const wxEventType mvceditor::EVENT_APP_READY = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_EXIT = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_FILE_OPENED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_FILE_CREATED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_FILE_NEW = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_FILE_SAVED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_FILE_CLOSED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_FILE_REVERTED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_FILE_DELETED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_FILE_EXTERNALLY_CREATED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_FILE_EXTERNALLY_MODIFIED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_FILE_RENAMED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_DIR_CREATED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_DIR_DELETED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_DIR_RENAMED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_PREFERENCES_SAVED = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_PREFERENCES_EXTERNALLY_UPDATED = wxNewEventType();
const wxEventType mvceditor::EVENT_CMD_FILE_OPEN = wxNewEventType();
const wxEventType mvceditor::EVENT_CMD_RUN_COMMAND = wxNewEventType();
const wxEventType mvceditor::EVENT_CMD_DB_TABLE_DATA_OPEN = wxNewEventType();
const wxEventType mvceditor::EVENT_CMD_DB_TABLE_DEFINITION_OPEN = wxNewEventType();

const long mvceditor::ID_TOOLS_NOTEBOOK = 1003;
const long mvceditor::ID_OUTLINE_NOTEBOOK = 1002;

/*
 *ATTN: this is hardcoded because of the way that the notebook is created
 * within the MainFrameForms (created by wxFormBuilder)
 */
const long mvceditor::ID_CODE_NOTEBOOK = 1001;
