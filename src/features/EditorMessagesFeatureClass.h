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
#ifndef __EDITORMESSAGESFEATURECLASS_H
#define __EDITORMESSAGESFEATURECLASS_H

#include <features/FeatureClass.h>
#include <wx/log.h>

namespace t4p {
	
/**
 * event that gets generated whenever we get a log message from the
 * wxWidgets logging system. when we get a log message from the wxWidgets
 * system, we will POST an event to the panel so that the panel
 * log gets drawn in the event loop. It seems that wxWidgets calls
 * the logger in background threads.
 */
extern const wxEventType EVENT_APP_LOG;

class EditorLogEventClass : public wxEvent {

public:

	wxString Message;
	wxLogLevel Level;
	time_t Timestamp;
	
	EditorLogEventClass(const wxString& msg, wxLogLevel level, time_t timestamp);
	
	wxEvent* Clone() const;
};

typedef void (wxEvtHandler::*EditorLogEventClassFunction)(EditorLogEventClass&);

#define EVT_APP_LOG(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_APP_LOG, -1, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( EditorLogEventClassFunction, & fn ), (wxObject *) NULL ),



/**
 * This class is the handler for the editor message menu items.  
 */
class EditorMessagesFeatureClass : public FeatureClass {

public:

	EditorMessagesFeatureClass(t4p::AppClass& app);
	
private:

	void OnAppReady(wxCommandEvent& event);
	
	DECLARE_EVENT_TABLE()
};

/**
 * This class will take care of receiving the log messages
 * and proxying them to the feature. 
 * It is done this way because wxWidgets will delete the Log target
 * (pointer given to wxLog::SetLogTarget()) so we need to
 * give it a pointer that is managed by wxWidgets
 */
class EditorMessagesLoggerClass : public wxLog {

public:

	EditorMessagesLoggerClass(EditorMessagesFeatureClass& feature);

	void DoLogRecord(wxLogLevel level, const wxString &msg, const wxLogRecordInfo &info);

private:

	EditorMessagesFeatureClass& Feature;
};

}
#endif
