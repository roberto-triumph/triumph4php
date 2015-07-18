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
#ifndef T4P_EVENTS_H
#define T4P_EVENTS_H

#include <globals/ProjectClass.h>
#include <wx/event.h>
#include <wx/filename.h>
#include <vector>

namespace t4p {

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
 * that wants to receive menu events, tool bar events, STC events, or AUI events can do so
 * automatically. ** HERE 'most of them' MEANS ALL EVENTS THAT HAVE A MENU ID
 * THAT IS WITHIN THE MenuIds::MENU_START to MenuIds::MENU_END, and a few stock menu IDs !!
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
 * EVT_STC_SAVEPOINT_LEFT (for all code controls)
 * EVT_STC_SAVEPOINT_REACHED (for all code controls)
 *
 *
 * The event sink is NOT suitable for inter-thread communication, use EventSinkWithLocker
 * for communication among theads
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
	 * forget all of the pushed handlers. After a call to this method, further calls
	 * to Publish() will do nothing.
	 */
	void RemoveAllHandlers();

	/**
	 * removes a single event handler from the sink. After a call to this method, the
	 * given handler will no longer be notified of any events.  Note that the
	 * handler is NOT deleted since this class does not own the pointer.
	 */
	void RemoveHandler(wxEvtHandler *handler);

	/**
	 * Note that the event is processed IMMEDIATELY ie. within the same
	 * event loop. This method will not return control until all of the
	 * handlers have processed the event.
	 *
	 * @param event send the event to all registered handlers
	 */
	void Publish(wxEvent& event);

	/**
	 * Note that the event is processed IN THE NEXT EVENT LOOP.
	 *
	 * @param event send the event to all registered handlers
	 */
	void Post(wxEvent& event);

private:

	/**
	 * the list of handlers to send events to. this class will NOT
	 * own these pointers.
	 */
	std::vector<wxEvtHandler*> Handlers;
};

/**
 * A EventSinkLocker is an event sink that uses a mutex
 * for all of this operations: PushHandler, RemoveHandler, RemoveAllHandlers,
 * and Post.
 *
 * The event sink is suitable for inter-thread communication.
 */
class EventSinkLockerClass {

public:

	EventSinkLockerClass();

	/**
	 * @param handler will get notified of every event that is published. This class
	 * will not own the pointer.
	 */
	void PushHandler(wxEvtHandler* handler);

	/**
	 * forget all of the pushed handlers. After a call to this method, further calls
	 * to Publish() will do nothing.
	 */
	void RemoveAllHandlers();

	/**
	 * removes a single event handler from the sink. After a call to this method, the
	 * given handler will no longer be notified of any events.  Note that the
	 * handler is NOT deleted since this class does not own the pointer.
	 */
	void RemoveHandler(wxEvtHandler *handler);

	/**
	 * Note that the event is processed IN THE NEXT EVENT LOOP.
	 *
	 * @param event send the event to all registered handlers
	 */
	void Post(wxEvent& event);

private:

	/**
	 * holds the list of event handlers
	 */
	t4p::EventSinkClass EventSink;

	/**
	 * to guard against simulatenous access of the event sink
	 */
	wxMutex Mutex;
};

// forward declaration to prevent recompilation when CodeControlClass is updated
class CodeControlClass;

/**
 * event gets generated when a file is saved. The event will be
 * of type CodeControlEventClass
 */
extern const wxEventType EVENT_APP_FILE_SAVED;

/**
 * event gets generated when a file is closed. The event will be
 * of type CodeControlEventClass
 */
extern const wxEventType EVENT_APP_FILE_CLOSED;


/**
 * Notification that an existing file has been opened. The event will be
 * of type CodeControlEventClass
 */
extern const wxEventType EVENT_APP_FILE_OPENED;

/**
 * Notification that a new code control tab has been created.  Since this is a
 * file not yet on the file system, there is no filename associated with the new
 * tab. The event will be of type CodeControlEventClass
 */
extern const wxEventType EVENT_APP_FILE_NEW;

/**
 * Notification that a code control tab has been "changed" (made active).  This
 * is analogous to the EVT_AUINOTEBOOK_PAGE_CHANGED event, but EVENT_APP_FILE_CHANGED event
 * will give you access to code control tabs. The event will be of type CodeControlEventClass.

 */
extern const wxEventType EVENT_APP_FILE_PAGE_CHANGED;

/**
 * Notification that a code control tab has been moved from one notebook to
 * another. The event will be of type CodeControlEventClass. The event object
 * is the source notebook (the notebook that the code control was removed from).
 */
extern const wxEventType EVENT_APP_FILE_NOTEBOOK_CHANGED;

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
class CodeControlEventClass : public wxEvent {

public:

	/**
	 * @param codeControl caller will still own the pointer
	 */
	CodeControlEventClass(wxEventType type, CodeControlClass* codeControl);

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

/**
 * This event will get generated when a file or directory has been renamed, either inside the editor
 * or outside the editor (ie in a file shell or command prompt).
 * The event type determines whether paths are files or directories.
 */
class RenameEventClass : public wxEvent {

public:

	wxFileName OldPath;

	wxFileName NewPath;

	RenameEventClass(wxEventType type, const wxString& oldPath, const wxString& newPath);

	wxEvent* Clone() const;

};

/**
 * This command can be used by any feature to tell the application to
 * open a file. A file can be opened to a specific line number.
 */
class OpenFileCommandEventClass : public wxEvent {

public:

	/**
	 * the full path of the file to open
	 */
	wxString FullPath;

	/**
	 * position to focus on and start highlighting, -1 to not highlight anything
	 * * This number is 0-based
	 */
	int StartingPos;

	/**
	 * number of characters to highlight, -1 to not highlight anything
	 */
	int Length;

	/**
	 * line number to seek to after the file is opened, -1 to not jump to a file
	 * This number is 1-based
	 */
	int LineNumber;


	OpenFileCommandEventClass(const wxString& fullPath, int startingPos = -1, int length = -1, int lineNumber = -1);

	wxEvent* Clone() const;

};

class OpenDbTableCommandEventClass : public wxEvent {

public:

	/**
	 * The name of the table to open
	 */
	wxString DbTableName;

	/**
	 * The hash of the db connection to connect to
	 */
	wxString ConnectionHash;

	OpenDbTableCommandEventClass(wxEventType type, const wxString& dbTable, const wxString& connectionHash);

	wxEvent* Clone() const;
};

typedef void (wxEvtHandler::*CodeControlEventClassFunction)(CodeControlEventClass&);

#define EVT_APP_FILE_NEW(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_APP_FILE_NEW, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( CodeControlEventClassFunction, & fn ), (wxObject *) NULL ),

#define EVT_APP_FILE_OPEN(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_APP_FILE_OPENED, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( CodeControlEventClassFunction, & fn ), (wxObject *) NULL ),

#define EVT_APP_FILE_SAVED(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_APP_FILE_SAVED, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( CodeControlEventClassFunction, & fn ), (wxObject *) NULL ),

#define EVT_APP_FILE_CLOSED(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_APP_FILE_CLOSED, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( CodeControlEventClassFunction, & fn ), (wxObject *) NULL ),

#define EVT_APP_FILE_PAGE_CHANGED(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_APP_FILE_PAGE_CHANGED, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( CodeControlEventClassFunction, & fn ), (wxObject *) NULL ),

#define EVT_APP_FILE_NOTEBOOK_CHANGED(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_APP_FILE_NOTEBOOK_CHANGED, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( CodeControlEventClassFunction, & fn ), (wxObject *) NULL ),

#define EVT_APP_FILE_REVERTED(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_APP_FILE_REVERTED, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( CodeControlEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*RenameEventClassFunction)(RenameEventClass&);

#define EVT_APP_FILE_RENAMED(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_APP_FILE_RENAMED, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( RenameEventClassFunction, & fn ), (wxObject *) NULL ),

#define EVT_APP_DIR_RENAMED(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_APP_DIR_RENAMED, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( RenameEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*OpenFileCommandEventClassFunction)(OpenFileCommandEventClass&);

#define EVT_CMD_FILE_OPEN(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_CMD_FILE_OPEN, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( OpenFileCommandEventClassFunction, & fn ), (wxObject *) NULL ),

typedef void (wxEvtHandler::*OpenDbTableCommandEventClassFunction)(OpenDbTableCommandEventClass&);

#define EVT_APP_DB_TABLE_DATA_OPEN(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_CMD_DB_TABLE_DATA_OPEN, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( OpenDbTableCommandEventClassFunction, & fn ), (wxObject *) NULL ),

#define EVT_APP_DB_TABLE_DEFINITION_OPEN(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_CMD_DB_TABLE_DEFINITION_OPEN, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( OpenDbTableCommandEventClassFunction, & fn ), (wxObject *) NULL ),

/**
 * This is a one-time event that gets generated after the application main
 * window is first shown to the user. When this event is generated, all
 * settings have been loaded from the config files. Features should not
 * do heavy work in any of the menu or config related methods; they should
 * do their heavy work in this event handler.
 */
extern const wxEventType EVENT_APP_READY;

/**
 * This is an event that gets generated after the application is
 * re-activated after being put in the background.  Basically
 * this is an EVT_ACTIVATE_APP event, but this event's callback
 * accepts a wxCommandEvent
 */
extern const wxEventType EVENT_APP_ACTIVATED;

/**
 * This is a one-time event that gets generated when the user wants to close
 * the application main frame. Event handlers have a chance to veto the
 * event to prevent the user from closing the main frame. The event is of
 * type wxNotifyEvent.
 */
extern const wxEventType EVENT_APP_FRAME_CLOSE;

typedef void (wxEvtHandler::*NotifyEventFunction)(wxNotifyEvent&);
#define EVT_APP_FRAME_CLOSE(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_APP_FRAME_CLOSE, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( NotifyEventFunction, & fn ), (wxObject *) NULL ),


/**
 * This is a one-time event that gets generated after the user has clicked the
 * Exit or Close button on the main frame AND the user has chosen to
 * not save the open files (or has already saved them).  Features can
 * do any final cleanup in this event handler. Note that in OS X, the
 * application will stay running even after this event is generated.
 */
extern const wxEventType EVENT_APP_EXIT;

/**
 * Notification that a new file has been created on the file system.  A file is created when
 * the user opens a new buffer and then saves it. The event will contain
 * the full path of the file that was opened in the GetString() method;
 */
extern const wxEventType EVENT_APP_FILE_CREATED;

/**
 * Notification that a file has been reverted (all changed in memory undone). The event
 * is of type CodeControlEventClass
 */
extern const wxEventType EVENT_APP_FILE_REVERTED;

/**
 * Notification that a single file has been deleted. The event will contain
 * the full path of the file that was opened in the GetString() method.
 * This event gets generated if the user deletes a files from the internal explorer or from
 * an external process (command prompt or OS shell)
 * NOTE: this event will only be generated when single files are deleted. if an entire directory is deleted,
 * then only 1 event will be generated: an EVENT_APP_DIR_DELETED, no EVENT_APP_FILE_DELETED events will be
 * generated.
 */
extern const wxEventType EVENT_APP_FILE_DELETED;

/**
 * Notification that the one of the files in any of the enabled projects has been updated by an
 * external process.
 * An example is when a user performs a SVN update / GIT pull to get the latest version of their
 * code; the editor will generated events of this type for each file that was updated by the source control
 * program.
 *
 * The event generated will be of type wxCommandEvent, the file that was modified will be available via GetString() method
 * GetString() will return the full path to the file.
 *
 * NOTE: This event will only be generated if the file is NOT created in Triumph.
 * NOTE: This event will only be generated when single files are created. if an entire directory is created,
 * then only 1 event will be generated: an EVENT_APP_DIR_CREATED, no EVENT_APP_FILE_EXTERNALLY_CREATED events will be
 * generated.
 */
extern const wxEventType EVENT_APP_FILE_EXTERNALLY_CREATED;

/**
 * Notification that a file was renamed. The event will contain the old and new
 * paths. This event gets generated if the user creates a directory from
 * an external process (command prompt or OS shell).
 */
extern const wxEventType EVENT_APP_FILE_RENAMED;

/**
 * Notification that the one of the files in any of the enabled projects has been updated by an
 * external process.
 * An example is when a user performs a SVN update / GIT pull to get the latest version of their
 * code; the editor will generated events of this type for each file that was updated by the source control
 * program.
 *
 * The event generated will be of type wxCommandEvent, the file that was modified will be available via GetString() method
 * GetString() will return the full path to the file.
 *
 * NOTE: This event will only be generated if the file is NOT already opened in Triumph. In the case that
 * a file that is opened in Triumph is modified externally, the user will be prompted to ignore or reload
 * contents from disk, and if the user chooses to reload contents from disk then a "normal" EVENT_APP_FILE_REVERT
 * event is generated instead.
 */
extern const wxEventType EVENT_APP_FILE_EXTERNALLY_MODIFIED;

/**
 * Notification that a single directory has been created. The event will contain
 * the full path of the directory that was created in the GetString() method.
 * This event gets generated if the user creates a directory from
 * an external process (command prompt or OS shell). Note: if an entire directory structure
 * is copied from one place to another, then only one EVENT_APP_DIR_CREATED is created for the
 * base directory that was created, no events will be generated for the sub directories.
 */
extern const wxEventType EVENT_APP_DIR_CREATED;

/**
 * Notification that a single directory has been deleted. The event will contain
 * the full path of the file that was opened in the GetString() method.
 * This event gets generated if the user deletes a files from the internal explorer or from
 * an external process (command prompt or OS shell)
 * NOTE: If an entire directory structure is deleted, then only 1 event will be generated:
 * an EVENT_APP_DIR_DELETED for the base directory. No events will be generated for the sub directories.
 */
extern const wxEventType EVENT_APP_DIR_DELETED;

/**
 * Notification that a directory was renamed. The event will contain the old and new
 * paths. This event gets generated if the user creates a directory from
 * an external process (command prompt or OS shell).
 */
extern const wxEventType EVENT_APP_DIR_RENAMED;

/**
 * Notification that the user preferences have been saved by the user.
 * This event will be genreated after the user changes the settings via Edit ... Preferences.
 * Listeners of this event will  need to repaint any windows that are affected by the changes. Listeners
 * can access the new preferences via the App Preferences global. Listeners also
 * need to save the preferences to persistent storage (config) that were updated
 * in the preferences forms (windows added in the AddPreferenceWindow() method).
 * The global config (wxConfig::Get()) should be used.
 * Note that the app will flush the changes after all handlers have been called, so there
 * is no need for each handler to call wxConfig::Flush()
 */
extern const wxEventType EVENT_APP_PREFERENCES_SAVED;

/**
 * Notification that the user preferences (the config INI file) has been updated by an
 * external process. An example of this is the user having 2 instances of Triumph open
 * and the user changed preferences in one of the instances.
 *
 * Listeners of this event will  need to repaint any windows that are affected by the changes. Listeners
 * can access the new preferences via the App Preferences, Environment globals.
 * The global config (wxConfig::Get()) should be used.
 */
extern const wxEventType EVENT_APP_PREFERENCES_EXTERNALLY_UPDATED;

/**
 * Notification that a new project was created via the "New Project"
 * menu (not the "Defined Projects" menu). The generated event will be
 * a wxCommandEvent, its GetString() will be contain the directory
 * of the created project.
 */
extern const wxEventType EVENT_APP_PROJECT_CREATED;

/**
 * Notification that a one or more projects were removed via the "Defined Projects"
 * menu. The generated event will be a ProjectEventClass, it
 * will contain the projects that were removed.
 */
extern const wxEventType EVENT_APP_PROJECTS_REMOVED;

/**
 * Notification that a one or more projects were updated via the "Defined Projects"
 * menu. The generated event will be a ProjectEventClass, it
 * will contain ONLY the projects that were actually updated.
 * Note that a project update could just have a label change and not
 * have any new source directories.
 */
extern const wxEventType EVENT_APP_PROJECTS_UPDATED;

/**
 * Project event gets generated when one or more projects are
 * added, removed, or updated
 */
class ProjectEventClass : public wxEvent {

public:

	/**
	 * the projects that were removed or updated. If this is a
	 * remove event, the these are the projects that were removed
	 * if this is a update event, then these projects were
	 * actually updated. Note that a project update could just
	 * have a label change and not have any new source directories.
	 */
	const std::vector<t4p::ProjectClass>& Projects;

	ProjectEventClass(wxEventType type, const std::vector<t4p::ProjectClass>& projects);

	wxEvent* Clone() const;

};

typedef void (wxEvtHandler::*ProjectEventClassFunction)(ProjectEventClass&);

#define EVT_APP_PROJECTS_REMOVED(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_APP_PROJECTS_REMOVED, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( ProjectEventClassFunction, & fn ), (wxObject *) NULL ),

#define EVT_APP_PROJECTS_UPDATED(fn) \
	DECLARE_EVENT_TABLE_ENTRY(t4p::EVENT_APP_PROJECTS_UPDATED, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction) \
    wxStaticCastEvent( ProjectEventClassFunction, & fn ), (wxObject *) NULL ),

/**
 * Tell the app to open a new file.
 * Note that the app will do NOTHING if the path is invalid; the feature should
 * make sure the path is valid.
 * The event is of type t4p::OpenFileCommandEventClass, the full path should be
 * located in event.GetString()
 */
extern const wxEventType EVENT_CMD_FILE_OPEN;

/**
 * Tell the app to open an explorer panel to a directory.
 * Note that the app will do NOTHING if the path is invalid; the feature should
 * make sure the path is valid.
 * The event is of type wxCommandEvent, the full path should be
 * located in event.GetString()
 */
extern const wxEventType EVENT_CMD_DIR_OPEN;

/**
 * Tell the app to run a command. The app will run the command line
 * and will dispolay the output in a panel.
 * The command line to run should  be set with event.SetString()
 */
extern const wxEventType EVENT_CMD_RUN_COMMAND;

/**
 * Tell the app to open a database table and show it's rows
 */
extern const wxEventType EVENT_CMD_DB_TABLE_DATA_OPEN;

/**
 * Tell the app to open a database table and show it's column structure
 */
extern const wxEventType EVENT_CMD_DB_TABLE_DEFINITION_OPEN;

/**
 * Tell the app to request attention from the user. The app's title
 * bar in the task bar will blink
 */
extern const wxEventType EVENT_CMD_APP_USER_ATTENTION;

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

}

#endif
