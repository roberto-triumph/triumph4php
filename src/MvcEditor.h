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
#include <environment/StructsClass.h>
#include <plugins/EditorMessagesPluginClass.h>
#include <PluginClass.h>
#include <PreferencesClass.h>
#include <Events.h>
#include <wx/app.h>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/Timer.h>
#include <vector>

namespace mvceditor {

// forward declaration
class AppFrameClass;

// defined below
class AppClass;

/**
 * A timer for a single-use.  We use this to 
 * generate a one-time ready event that plugins can
 * listen for when they want to perform something
 * right after the main frame has been shown to the
 * user for the first time.
 */
class SingleTimerClass : public wxTimer {

public:

	/**
	 * create AND begin the timer
	 */
	SingleTimerClass(mvceditor::AppClass& app);

	/**
	 * when the timer ends, generate an EVENT_APP_READY event
	 */
	void Notify();

	mvceditor::AppClass& App;

};

class AppClass : public wxApp {

public:

	/**
	 * Important source code program structures, like resources (tags)
	 * URLs, templates.
	 */
	mvceditor::StructsClass Structs;
	
	/**
	 * To keep track of background threads
	 */
	mvceditor::RunningThreadsClass RunningThreads;

	/**
	 * Any plugins should post any useful events to this event handler
	 * then all other plugins will get notified.  This is how
	 * inter-plugin communication is done in a way to reduce coupling
	 * Plugins need not push themselves onto this sink, the application will do 
	 * so at app start.
	 */
	EventSinkClass EventSink;

	/** 
	 * Initialize the application 
	 */
	virtual bool OnInit();
	
	AppClass();

	~AppClass();

private:

	/**
	 * Parses any command line arguments.  Returns false if arguments are invalid.
	 */
	bool CommandLine();

	/**
	 * create plugins. only instantiates and nothing else
	 */
	void CreatePlugins();
	
	/**
	 * delete plugins from memory
	 */
	void DeletePlugins();

	/**
	 * asks plugins for any windows they want to create
	 */
	void PluginWindows();

	/**
	 * Additional functionality
	 */
	std::vector<PluginClass*> Plugins;
	
	/**
	 * The user preferences.  
	 * @var PreferencesClass;
	 */
	PreferencesClass Preferences;

	/**
	 * The main application frame.
	 */
	AppFrameClass* AppFrame;

	/**
	 * With this timer, we will generate an EVENT_APP_INITIALIZED after the
	 * window is initially shown to the user. We want to show the main
	 * window to the user as fast as possible to make the app seem
	 * fast.
	 */
	mvceditor::SingleTimerClass* Timer;

	/**
	 * Shows the user various editor messages (not related to their code)
	 */
	EditorMessagesPluginClass* EditorMessagesPlugin;

};

}