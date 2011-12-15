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
#include <php_frameworks/ProjectClass.h>
#include <widgets/ProcessWithHeartbeatClass.h>
#include <widgets/ResourceUpdateThreadClass.h>
#include <windows/AppFrameClass.h>
#include <environment/EnvironmentClass.h>
#include <plugins/ResourcePluginClass.h>
#include <PluginClass.h>
#include <PreferencesClass.h>
#include <wx/app.h>
#include <wx/event.h>
#include <wx/string.h>
#include <vector>

namespace mvceditor {

/**
 * These events can be published by plugins; the application 
 * will listen for these events and act accordingly.
*/

/**
 * Tell the app to open a new project.
 * The command event should set the project root path with event.SetString()
 * Note that the app will do NOTHING if the path is invalid; the plugin should
 * make sure the path is valid.
 */
extern const wxEventType EVENT_APP_OPEN_PROJECT;

/**
 * Tell the app to save its state to the file system, and will 
 * also repaint any windows that are affected by the changes.
 */
extern const wxEventType EVENT_APP_SAVE_PREFERENCES;

/**
 * Tell the app to re-index the current project.
 */
extern const wxEventType EVENT_APP_RE_INDEX;

class AppClass : public wxApp {

public:

	/** 
	 * Initialize the application 
	 */
	virtual bool OnInit();
	
	AppClass();

	~AppClass();

private:

	/**
	 * flush preferences to disk and tells all dependent windows to repaint
	 * themselves based on the new settings.
	 */
	void OnSavePreferences(wxCommandEvent& event);

	/**
	 * Opens the given directory as a project.
	 */
	void OnProjectOpen(wxCommandEvent& event);

	/**
	 * Opens the given directory as a project.
	 */
	void ProjectOpen(const wxString& directoryPath);

	/**
	 * method that will trigger the re-indexing of the current project.
	 */
	void OnProjectReIndex(wxCommandEvent& event);

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
	 * create a project and initialize all objects that depend on project
	 */
	void CreateProject(const ProjectOptionsClass& options);
	
	/**
	 * close project and all resources that depend on it
	 */
	void CloseProject();

	/**
	 * method that gets called when one of the external processes finishes
	 */
	void OnProcessComplete(wxCommandEvent& event);

	/**
	 * method that gets called when one of the external processes fails
	 */
	void OnProcessFailed(wxCommandEvent& event);

	/**
	 * Additional functionality
	 */
	std::vector<PluginClass*> Plugins;

	/**
	 * a project may be using more than one framework. This vector
	 * will be used as a 'queue' so that we can know when all framework
	 * info has been discovered. This queue is needed because the
	 * detection process is asynchronous.
	 */
	std::vector<wxString> FrameworkIdentifiersLeftToDetect;
	
	/**
	 * The environment stack.
	 * 
	 * @var EnvironmentClass
	 */
	EnvironmentClass Environment;

	/**
	 * Any running external processes are tracked here.  These external
	 * processes are usually calls to the PHP framework detectiong scripts.
	 */
	ProcessWithHeartbeatClass ProcessWithHeartbeat;
	
	/**
	 * This object will be used to parse the resources of files that are currently open.
	 */
	ResourceUpdateThreadClass ResourceUpdates;

	/**
	 * The user preferences
	 * 
	 * @var PreferencesClass;
	 */
	PreferencesClass* Preferences;

	/**
	 * The open project
	 * @var ProjectClass*
 	 */
	ProjectClass* Project;

	/**
	 * The main application frame.
	 */
	AppFrameClass* AppFrame;

	/**
	 * This is used to trigger project indexing.  This pointer just points to one of the Plugins
	 * in the Plugin vector; no need to delete.
	 */ 
	ResourcePluginClass* ResourcePlugin;

	DECLARE_EVENT_TABLE()
};

}