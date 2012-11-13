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
#ifndef __MVCEDITOREVENTS_H__
#define __MVCEDITOREVENTS_H__

#include <wx/event.h>
#include <vector>

namespace mvceditor {

/**
 * This is the class that will listen to, and notify, of any 'interesting'
 * editor events.  This will allow for communication between various
 * components of the editor without needing to couple them.
 * The event sink will allow any handlers to handle any of the events defined
 * int this file (EVENT_APP_*, EVENT_CMD_*). What this means is that, from anywhere
 * in the app, a file can be affect the application, like for example
 * a file may be opened by sending the EVENT_CMD_OPEN_FILE
 *
 * In addition, the application main frame
 * frame will publish the following events to the event sink; this way any functionality
 * that wants to receive menu events, tool bar events, or AUI events can do so 
 * automatically. ** HERE 'most of them' MEANS ALL EVENTS THAT HAVE A MENU ID
 * THAT IS WITHIN THE MenuIds::MENU_START to MenuIds::MENU_END !! 
 *
 * EVT_MENU (most of them)
 * EVT_TOOL (most of them)
 * EVT_AUINOTEBOOK_PAGE_CHANGED (for the source code, tools, and outline notebooks)
 * EVT_AUINOTEBOOK_PAGE_CHANGING (for the source code, tools, and outline notebook)
 * EVT_AUINOTEBOOK_PAGE_CLOSE (for the source code, tools, and outline notebook)
 * EVT_AUINOTEBOOK_PAGE_CLOSED (for the source code, tools, and outline notebook)
 * EVT_AUITOOLBAR_BEGIN_DRAG (all of them)
 * EVT_AUITOOLBAR_MIDDLE_CLICK (all of them)
 * EVT_AUITOOLBAR_OVERFLOW_CLICK (all of them)
 * EVT_AUITOOLBAR_RIGHT_CLICK (all of them)
 * EVT_AUITOOLBAR_TOOL_DROPDOWN (all of them)
 */
class EventSinkClass {

public:

	EventSinkClass();

	/**
	 * @param handler will get notified of every event that is published. This class
	 * will not own the pointer.
	 */
	void PushHandler(wxEvtHandler* handler);

	/**
	 * @param event send the event to all registered handlers
	 */
	void Publish(wxEvent& event);

private:

	std::vector<wxEvtHandler*> Handlers;
};

// forward declaration to prevent recompilation when CodeControlClass is updated
class CodeControlClass;


extern const wxEventType EVENT_FEATURE_FILE_SAVED;

/**
 * This is an event that will tell a feature that a file has been saved.
 * The feature will get a pointer to the CodeControl that was saved.
 * From the code control; the event handler can get the file name
 * or file contents if so desired.
 * Using a new event class as opposed to a wxCommandEvent because
 * wxCommandEvents propagate by default; but we do not want
 * these events to propagate. This is because features are hooked into
 * the main frame's event table; and if the event were to propagate
 * then it would result in an infine loop.
 */
class FileSavedEventClass : public wxEvent {

public:

	/**
	 * @param codeControl caller will still own the pointer
	 */
	FileSavedEventClass(CodeControlClass* codeControl);

	/**
	 * @return the code control that was saved; do NOT delete the returned
	 * pointer.
	 */
	CodeControlClass* GetCodeControl() const;

	/** 
	 * required for sending with wxPostEvent()
	 */
    wxEvent* Clone() const;

private:

	CodeControlClass* CodeControl;
};

typedef void (wxEvtHandler::*FileSavedEventClassFunction)(FileSavedEventClass&);

#define EVT_FEATURE_FILE_SAVED(fn) \
	DECLARE_EVENT_TABLE_ENTRY(mvceditor::EVENT_FEATURE_FILE_SAVED, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( FileSavedEventClassFunction, & fn ), (wxObject *) NULL ),

/**
 * This is a one-time event that gets generated after the application main
 * window is first shown to the user. When this event is generated, all 
 * settings have been loaded from the config files. Features should not
 * do heavy work in any of the menu or config related methods; they should
 * do their heavy work in this event handler.
 */
extern const wxEventType EVENT_APP_READY;

/**
 * This is a one-time event that gets generated after the user has clicked the
 * Exit or Close button on the main frame AND the user has chosen to 
 * not save the open files (or has already saved them).  Features can
 * do any final cleanup in this event handler.
 */
extern const wxEventType EVENT_APP_EXIT;

/**
 * Notification that the projects list has been opened / closed or any project's source definitions have 
 * updated. This event is fired AFTER the framework
 * detection is complete; it is safe to access a project's database connections, dynamic
 * resources, etc... via the application globals.
 */
extern const wxEventType EVENT_APP_PROJECTS_UPDATED;

/**
 * Notification that the that the current project
 * has been indexed (project's resource cache has been udpated); the event listeners can 
 * get the project name
 * or resource cache from its [FeatureClass] member properties if so desired.
 * Note that the cache may be updated when the user clicks the 'index' button or by 
 * some other, automatic action.
 */
extern const wxEventType EVENT_APP_PROJECT_INDEXED;

/**
 * Notification that an existing file has been opened.  The event will contain
 * the full path of the file that was opened in the GetString() method; 
 */
extern const wxEventType EVENT_APP_FILE_OPENED;

/**
 * Notification that a file has been closed.  The event will contain
 * the full path of the file that was closed; the path may be the 
 * empty string when a new file is closed.
 */
extern const wxEventType EVENT_APP_FILE_CLOSED;

/**
 * Notification that the user preferences have been saved by the user. 
 * This event will be genreated after the user changes the settings via Edit ... Preferences.
 * Listeners of this event will  need to repaint any windows that are affected by the changes. Listeners
 * can access the new preferences via the App Preferences global. Listeners also
 * need to save the preferences to persistent storage (config) that were updated
 * in the preferences forms (windows added in the AddPreferenceWindow() method).
 * The global config (wxConfig::Get()) should be used.
 */
extern const wxEventType EVENT_APP_PREFERENCES_SAVED;
/**
 * Notification that the user preferences (the config INI file) has been updated by an 
 * external process. An example of this is the user having 2 instances of MVC Editor open
 * and the user changed preferences in one of the instances.
 *
 * Listeners of this event will  need to repaint any windows that are affected by the changes. Listeners
 * can access the new preferences via the App Preferences, Environment globals.
 * The global config (wxConfig::Get()) should be used.
 */
extern const wxEventType EVENT_APP_PREFERENCES_EXTERNALLY_UPDATED;

/**
 * Notification the that project's urls have been determined (AppClass::UrlResourceFinder 
 * has been filled with the project's urls)
 */
extern const wxEventType EVENT_APP_PROJECT_URLS;

/**
 * Tell the app to re-index the current project.
 */
extern const wxEventType EVENT_CMD_RE_INDEX;

/**
 * Tell the app to open a new file.
 * The command event should set the file to be opened with event.SetString()
 * Note that the app will do NOTHING if the path is invalid; the feature should
 * make sure the path is valid.
 */
extern const wxEventType EVENT_CMD_FILE_OPEN;

/**
 * Tell the app to build a list of the project's URLs
 */
extern const wxEventType EVENT_CMD_PROJECT_URLS;

/**
 * The window ID of the Tools Notebook. Use this to connect to the notebook
 * events for the tools notebook
 */
extern const long ID_TOOLS_NOTEBOOK;

/**
 * The window ID of the outline notebook. Use this to connect to the notebook
 * events for the outline notebook
 */
extern const long ID_OUTLINE_NOTEBOOK;

/**
 * The window ID of the source code notebook. Use this to connect to the notebook
 * events for the code notebook
 */
extern const long ID_CODE_NOTEBOOK;

}

#endif