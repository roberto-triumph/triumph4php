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
#include <globals/GlobalsClass.h>
#include <main_frame/PreferencesClass.h>
#include <globals/Events.h>
#include <actions/GlobalsChangeHandlerClass.h>
#include <actions/SequenceClass.h>
#include <actions/FileModifiedCheckActionClass.h>
#include <wx/app.h>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/timer.h>
#include <vector>

namespace t4p {

// forward declaration
class MainFrameClass;
class FeatureClass;
class FeatureViewClass;
class EditorMessagesFeatureClass;

// defined below
class AppClass;

/**
 * We use this to generate a one-time ready event that features can
 * listen for when they want to perform something
 * right after the main frame has been shown to the
 * user for the first time. We also use this to check for changes
 * to the global config file.
 */
class AppTimerClass : public wxTimer {

public:

	/**
	 * create AND begin the timer
	 */
	AppTimerClass(t4p::AppClass& app);

	/**
	 * when the timer ends, generate an EVENT_APP_READY event
	 */
	void Notify();

private:

	t4p::AppClass& App;

	void OnConfigFileModified(t4p::FilesModifiedEventClass& event);

	DECLARE_EVENT_TABLE()

};

class AppClass : public wxApp {

public:

	/**
	 * Important source code program structures, like resources (tags)
	 * URLs, templates.
	 */
	t4p::GlobalsClass Globals;
	
	/**
	 * To keep track of background threads
	 */
	t4p::RunningThreadsClass RunningThreads;


	/**
	 * This is a separate instance of running threads  used specifically for
	 * updating the tag cache. since sqlite locks the DB on inserts, updates,
	 * or deletes, we must queue actions that modify the tag cache in order
	 * to prevent locking errors.
	 */
	t4p::RunningThreadsClass SqliteRunningThreads;

	/**
	 * Any features should post any useful events to this event handler
	 * then all other features will get notified.  This is how
	 * inter-feature communication is done in a way to reduce coupling
	 * Features need not push themselves onto this sink, the application will do 
	 * so at app start.
	 */
	EventSinkClass EventSink;

	/**
	 * This object will handle any events that are generated by background
	 * actions and will update the global data structures accordingly
	 */
	t4p::GlobalsChangeHandlerClass GlobalsChangeHandler;

	/**
	 * To run multiple steps.
	 */
	t4p::SequenceClass Sequences;
	
	/**
	 * The user preferences.  
	 * @var PreferencesClass;
	 */
	PreferencesClass Preferences;

	/**
	 * keep track of the last time the config was modified. will use
	 * this to check for external changes to the config file
	 * by another instance of this application
	 */
	wxDateTime ConfigLastModified;

	/** 
	 * Initialize the application 
	 */
	virtual bool OnInit();
	
	AppClass();

	~AppClass();

	void LoadPreferences();

	void SavePreferences(const wxFileName& settingsDir, bool changedDirectory);

	/**
	 * signal that this app has modified the config file, that way the external
	 * modification check fails and the user will not be prompted to reload the config
	 */
	void UpdateConfigModifiedTime();

	/**
	 * stop watching the confid file for external modifications. This would be done
	 * when the user is editing the config from within the application.
	 */
	void StopConfigModifiedCheck();

	/**
	 * delete features from memory in a safe way
	 */
	void DeleteFeatures();

private:

	/**
	 * Parses any command line arguments.  Returns false if arguments are invalid.
	 */
	bool CommandLine();

	/**
	 * create features. only instantiates and nothing else
	 */
	void CreateFeatures();

	/**
	 * asks features for any windows they want to create
	 */
	void FeatureWindows();

	/**
	 * when the app is re-activated, tell the features about it
	 */
	void OnActivateApp(wxActivateEvent& event);

	/**
	 * Almost all functionality is encapsulated in features; the 
	 * app just creates them and hangs on to them until the end. 
	 * Each feature is created only once at app start and is
	 * deleted at app end.
	 * pointers are owned by this class.
	 */
	std::vector<FeatureClass*> Features;
	
	/**
	 * Feature views are view specific functionality; a view
	 * is the class that makes updates to the GUI of the editor
	 * (adds menu items, buttons, panels, etc).
	 * Each feature view is created once at app start, and will
	 * be deleted when the app main frame is deleted. Note that
	 * on Mac OS X, an application may run without a main frame,
	 * so during a run feature views may be created more than once
	 * (but there will be at most 1 instance to each view).
	 */
	std::vector<FeatureViewClass*> FeatureViews;

	/**
	 * With this timer, we will generate an EVENT_APP_INITIALIZED after the
	 * window is initially shown to the user. We want to show the main
	 * window to the user as fast as possible to make the app seem
	 * fast.
	 * Also, in this timer we will check for external updates to the 
	 * global config file; and if the global config file has changed
	 * reload those changes
	 */
	t4p::AppTimerClass Timer;

	/**
	 * The main application frame.
	 */
	MainFrameClass* MainFrame;

	/**
	 * Shows the user various editor messages (not related to their code)
	 */
	EditorMessagesFeatureClass* EditorMessagesFeature;

	/**
	 * TRUE if the EVENT_APP_READY has already been generated.
	 */
	bool IsAppReady;

	friend class t4p::AppTimerClass;

	DECLARE_EVENT_TABLE()
};

}