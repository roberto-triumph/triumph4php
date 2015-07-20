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
#include <globals/GlobalsClass.h>
#include <main_frame/PreferencesClass.h>
#include <globals/Events.h>
#include <actions/GlobalsChangeHandlerClass.h>
#include <actions/SequenceClass.h>
#include <actions/FileModifiedCheckActionClass.h>
#include <FeatureFactoryClass.h>
#include <wx/app.h>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/timer.h>
#include <vector>
#include <wx/bookctrl.h>

namespace t4p {
// forward declaration
class FeatureClass;
class FeatureViewClass;
class FeatureFactoryClass;

class AppClass {
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

	AppClass(wxTimer& configModifiedTimer);

	~AppClass();

	/**
	 * Initialize the application
	 */
	void Init();

	void LoadPreferences();

	void SavePreferences(const wxFileName& settingsDir, bool changedDirectory);

	/**
	 * signal that this app has modified the config file, that way the external
	 * modification check fails and the user will not be prompted to reload the config
	 */
	void UpdateConfigModifiedTime();

	/**
	 * delete features from memory in a safe way
	 */
	void DeleteFeatures();

	/**
	 * Deletes all views. This is done when the main frame is closed, because
	 * frames could have pointers to windows.Feature views are deleted when]
	 * the main frame is closed.
	 */
	void DeleteFeatureViews();

	/**
	 * Adds all features' preferences panels into the given parent.
	 *
	 * @param parent the parent book control
	 */
	void AddPreferencesWindows(wxBookCtrlBase* parent);

	/**
	 * @return TRUE if this app is active (in the foreground)
	 */
	bool IsActive();

	private:
	/**
	 * Parses any command line arguments.
	 * @param [out] filenames the files given as arguments
	 *        to the app.
	 * @return false if arguments are invalid.
	 */
	bool CommandLine(std::vector<wxString>& filenames);

	/**
	 * create features. only instantiates and nothing else
	 */
	void CreateFeatures();

	public:  // temp public, should be private
	/**
	 * asks features for any windows they want to create
	 */
	void FeatureWindows();

	public:  // temp public, should be private
	/**
	 * Almost all functionality is encapsulated in features; the
	 * app just creates them and hangs on to them until the end.
	 * Each feature is created only once at app start and is
	 * deleted at app end.
	 */
	t4p::FeatureFactoryClass FeatureFactory;

	private:
	// config modified timer
	wxTimer& Timer;
};
}
