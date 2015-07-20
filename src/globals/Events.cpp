/**
 * @copyright  2009-2011 Roberto Perpuly
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
#include <globals/Events.h>
#include <algorithm>

t4p::EventSinkClass::EventSinkClass()
	: Handlers() {
}

void t4p::EventSinkClass::PushHandler(wxEvtHandler *handler) {
	Handlers.push_back(handler);
}

void t4p::EventSinkClass::RemoveAllHandlers() {
	Handlers.clear();
}

void t4p::EventSinkClass::RemoveHandler(wxEvtHandler *handler) {
	std::vector<wxEvtHandler*>::iterator it = std::find(Handlers.begin(), Handlers.end(), handler);
	if (it != Handlers.end()) {
		Handlers.erase(it);
	}
}

void t4p::EventSinkClass::Publish(wxEvent& event) {
	for (size_t i = 0; i < Handlers.size(); ++i) {
		// wont use wxPostEvent for now
		// using wxPostEvent would cause any Popup menus triggered in the
		// handlers to not work correctly in linux
		Handlers[i]->ProcessEvent(event);
	}
}

void t4p::EventSinkClass::Post(wxEvent& event) {
	for (size_t i = 0; i < Handlers.size(); ++i) {
		wxPostEvent(Handlers[i], event);
	}
}

t4p::EventSinkLockerClass::EventSinkLockerClass()
: EventSink()
, Mutex() {
}

void t4p::EventSinkLockerClass::PushHandler(wxEvtHandler *handler) {
	wxMutexLocker locker(Mutex);
	wxASSERT(locker.IsOk());
	EventSink.PushHandler(handler);
}

void t4p::EventSinkLockerClass::RemoveAllHandlers() {
	wxMutexLocker locker(Mutex);
	wxASSERT(locker.IsOk());
	EventSink.RemoveAllHandlers();
}

void t4p::EventSinkLockerClass::RemoveHandler(wxEvtHandler *handler) {
	wxMutexLocker locker(Mutex);
	wxASSERT(locker.IsOk());
	EventSink.RemoveHandler(handler);
}

void t4p::EventSinkLockerClass::Post(wxEvent& event) {
	wxMutexLocker locker(Mutex);
	wxASSERT(locker.IsOk());
	EventSink.Post(event);
}

t4p::CodeControlEventClass::CodeControlEventClass(wxEventType type, t4p::CodeControlClass* codeControl)
	: wxEvent(wxID_ANY, type)
	, CodeControl(codeControl) {
}

t4p::CodeControlClass* t4p::CodeControlEventClass::GetCodeControl() const {
	return CodeControl;
}

wxEvent* t4p::CodeControlEventClass::Clone() const {
	t4p::CodeControlEventClass* newEvt = new t4p::CodeControlEventClass(GetEventType(), CodeControl);
	return newEvt;
}

t4p::RenameEventClass::RenameEventClass(wxEventType type, const wxString& oldPath, const wxString& newPath)
: wxEvent(wxID_ANY, type)
, OldPath()
, NewPath() {
	if (t4p::EVENT_APP_FILE_RENAMED == type) {
		OldPath.Assign(oldPath);
		NewPath.Assign(newPath);
	} else if (t4p::EVENT_APP_DIR_RENAMED == type) {
		OldPath.AssignDir(oldPath);
		NewPath.AssignDir(newPath);
	}
}

wxEvent* t4p::RenameEventClass::Clone() const {
	if (t4p::EVENT_APP_DIR_RENAMED == GetEventType()) {
		return new t4p::RenameEventClass(GetEventType(), OldPath.GetPath(), OldPath.GetPath());
	}
	return new t4p::RenameEventClass(GetEventType(), OldPath.GetFullPath(), OldPath.GetFullPath());
}

t4p::OpenFileCommandEventClass::OpenFileCommandEventClass(const wxString& fullPath, int startingPos,
	int length, int lineNumber)
: wxEvent(wxID_ANY, t4p::EVENT_CMD_FILE_OPEN)
, FullPath(fullPath.c_str())
, StartingPos(startingPos)
, Length(length)
, LineNumber(lineNumber) {
}

wxEvent* t4p::OpenFileCommandEventClass::Clone() const {
	return new t4p::OpenFileCommandEventClass(FullPath, StartingPos, Length, LineNumber);
}


t4p::OpenDbTableCommandEventClass::OpenDbTableCommandEventClass(wxEventType type, const wxString& dbTable,
	const wxString& connectionHash)
: wxEvent(wxID_ANY, type)
, DbTableName(dbTable.c_str())
, ConnectionHash(connectionHash.c_str()) {
}

wxEvent* t4p::OpenDbTableCommandEventClass::Clone() const {
	t4p::OpenDbTableCommandEventClass* evt = new t4p::OpenDbTableCommandEventClass(
		GetEventType(), DbTableName, ConnectionHash);
	return evt;
}

t4p::ProjectEventClass::ProjectEventClass(wxEventType type, const std::vector<t4p::ProjectClass>& projects)
: wxEvent(wxID_ANY, type)
, Projects(projects) {
}

wxEvent* t4p::ProjectEventClass::Clone() const {
	t4p::ProjectEventClass* evt = new t4p::ProjectEventClass(GetEventType(), Projects);
	return evt;
}



const wxEventType t4p::EVENT_APP_READY = wxNewEventType();
const wxEventType t4p::EVENT_APP_ACTIVATED = wxNewEventType();
const wxEventType t4p::EVENT_APP_FRAME_CLOSE = wxNewEventType();
const wxEventType t4p::EVENT_APP_EXIT = wxNewEventType();
const wxEventType t4p::EVENT_APP_FILE_OPENED = wxNewEventType();
const wxEventType t4p::EVENT_APP_FILE_CREATED = wxNewEventType();
const wxEventType t4p::EVENT_APP_FILE_NEW = wxNewEventType();
const wxEventType t4p::EVENT_APP_FILE_PAGE_CHANGED = wxNewEventType();
const wxEventType t4p::EVENT_APP_FILE_NOTEBOOK_CHANGED = wxNewEventType();
const wxEventType t4p::EVENT_APP_FILE_SAVED = wxNewEventType();
const wxEventType t4p::EVENT_APP_FILE_CLOSED = wxNewEventType();
const wxEventType t4p::EVENT_APP_FILE_REVERTED = wxNewEventType();
const wxEventType t4p::EVENT_APP_FILE_DELETED = wxNewEventType();
const wxEventType t4p::EVENT_APP_FILE_EXTERNALLY_CREATED = wxNewEventType();
const wxEventType t4p::EVENT_APP_FILE_EXTERNALLY_MODIFIED = wxNewEventType();
const wxEventType t4p::EVENT_APP_FILE_RENAMED = wxNewEventType();
const wxEventType t4p::EVENT_APP_DIR_CREATED = wxNewEventType();
const wxEventType t4p::EVENT_APP_DIR_DELETED = wxNewEventType();
const wxEventType t4p::EVENT_APP_DIR_RENAMED = wxNewEventType();
const wxEventType t4p::EVENT_APP_PREFERENCES_SAVED = wxNewEventType();
const wxEventType t4p::EVENT_APP_PREFERENCES_EXTERNALLY_UPDATED = wxNewEventType();
const wxEventType t4p::EVENT_APP_PROJECT_CREATED = wxNewEventType();
const wxEventType t4p::EVENT_APP_PROJECTS_REMOVED = wxNewEventType();
const wxEventType t4p::EVENT_APP_PROJECTS_UPDATED = wxNewEventType();
const wxEventType t4p::EVENT_CMD_FILE_OPEN = wxNewEventType();
const wxEventType t4p::EVENT_CMD_DIR_OPEN = wxNewEventType();
const wxEventType t4p::EVENT_CMD_RUN_COMMAND = wxNewEventType();
const wxEventType t4p::EVENT_CMD_DB_TABLE_DATA_OPEN = wxNewEventType();
const wxEventType t4p::EVENT_CMD_DB_TABLE_DEFINITION_OPEN = wxNewEventType();
const wxEventType t4p::EVENT_CMD_APP_USER_ATTENTION = wxNewEventType();

const long t4p::ID_TOOLS_NOTEBOOK = wxNewId();
const long t4p::ID_OUTLINE_NOTEBOOK = wxNewId();

