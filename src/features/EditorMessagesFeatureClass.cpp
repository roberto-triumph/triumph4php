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
#include <features/EditorMessagesFeatureClass.h>
#include <Triumph.h>

t4p::EditorMessagesFeatureClass::EditorMessagesFeatureClass(t4p::AppClass& app)
	: FeatureClass(app) {
}

void t4p::EditorMessagesFeatureClass::OnAppReady(wxCommandEvent& event) {

	// this line is needed so that we get all the wxLogXXX messages
	// pointer will be managed by wxWidgets
	// need to put this here because the logger needs an initialized window state
	wxLog::SetActiveTarget(new t4p::EditorMessagesLoggerClass(*this));
}

t4p::EditorMessagesLoggerClass::EditorMessagesLoggerClass(t4p::EditorMessagesFeatureClass& feature)
	: wxLog()
	, Feature(feature) {
	SetLogLevel(wxLOG_Message);
}

void t4p::EditorMessagesLoggerClass::DoLogRecord(wxLogLevel level, const wxString &msg, const wxLogRecordInfo &info) {
	t4p::EditorLogEventClass evt(msg, level, info.timestamp);
	Feature.App.EventSink.Post(evt);
}

t4p::EditorLogEventClass::EditorLogEventClass(const wxString& msg, wxLogLevel level, time_t timestamp)
: wxEvent(wxID_ANY, t4p::EVENT_APP_LOG)

// make thread safe
, Message(msg.c_str())
, Level(level)
, Timestamp(timestamp) {

}

wxEvent* t4p::EditorLogEventClass::Clone() const {
	t4p::EditorLogEventClass* evt = new t4p::EditorLogEventClass(Message, Level, Timestamp);
	return evt;
}

const wxEventType t4p::EVENT_APP_LOG = wxNewEventType();

BEGIN_EVENT_TABLE(t4p::EditorMessagesFeatureClass, t4p::FeatureClass)
	EVT_COMMAND(wxID_ANY, t4p::EVENT_APP_READY, t4p::EditorMessagesFeatureClass::OnAppReady)
END_EVENT_TABLE()
