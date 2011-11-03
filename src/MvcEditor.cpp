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
#include <wx/wx.h>
#include <wx/cmdline.h>
#include <unicode/uclean.h>

#include <windows/AppFrameClass.h>
#include <plugins/EnvironmentPluginClass.h>
#include <plugins/FindInFilesPluginClass.h>
#include <plugins/FinderPluginClass.h>
#include <plugins/ProjectPluginClass.h>
#include <plugins/OutlineViewPluginClass.h>
#include <plugins/ResourcePluginClass.h>
#include <plugins/RunConsolePluginClass.h>
#include <plugins/LintPluginClass.h>
#include <plugins/SqlBrowserPluginClass.h>
#include <widgets/ProcessWithHeartbeatClass.h>

namespace mvceditor {

static const int ID_FRAMEWORK_DETECT_PROCESS = wxNewId();
static const int ID_DATABASE_DETECT_PROCESS = wxNewId();

class AppClass : public wxApp {

public:

	/** 
	 * Initialize the application 
	 */
	virtual bool OnInit();
	
	AppClass();

	~AppClass();

	/**
	 * flush preferences to disk and tells all dependent windows to repaint
	 * themselves based on the new settings.
	 */
	void OnSavePreferences(wxCommandEvent& event);

	/**
	 * notify all plugins that the file has been saved.
	 */
	void OnFileSaved(wxCommandEvent& event);

	/**
	 * Opens the given directory as a project.
	 */
	void OnProjectOpen(wxCommandEvent& event);

	/**
	 * Opens the given directory as a project.
	 */
	void ProjectOpen(const wxString& directoryPath);

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

	DECLARE_EVENT_TABLE()
};

}

IMPLEMENT_APP(mvceditor::AppClass)

mvceditor::AppClass::AppClass()
	: wxApp()
	, Plugins()
	, FrameworkIdentifiersLeftToDetect()
	, Environment()
	, ProcessWithHeartbeat(*this)
	, Preferences(NULL)
	, Project(NULL)
	, AppFrame(NULL) {
}

/**
 * when app starts, create the new app frame
 */
bool mvceditor::AppClass::OnInit() {

	// plugins will need to be create first because 
	CreatePlugins();

	// due to the way keyboard shortcuts are serialized, we need to load the
	// frame and initialize the plugin windows so that all menus are created
	// and only then can we load the keyboard shortcuts from the INI file
	// all menu items must be present in the menu bar for shortcuts to take effect
	Preferences = new PreferencesClass();
	AppFrame = new mvceditor::AppFrameClass(Plugins, *this, Environment, *Preferences);
	PluginWindows();
	wxConfigBase* config = wxConfigBase::Get();
	for (size_t i = 0; i < Plugins.size(); ++i) {
		Plugins[i]->InitState(&Environment, this);
		Plugins[i]->LoadPreferences(config);
	}
	// load any settings from .INI files
	PreferencesClass::InitConfig();
	Environment.LoadFromConfig();
	Preferences->Load(AppFrame);

	// open a new project
	ProjectOpen(wxT(""));

	AppFrame->AuiManagerUpdate();
	if (CommandLine()) {
		SetTopWindow(AppFrame);
		AppFrame->Maximize(true);
		AppFrame->Show(true);
		return true;
	}
	return false;
}

mvceditor::AppClass::~AppClass() {
	DeletePlugins();
	if (Project) {
		delete Project;
		Project = NULL;
	}
	if (Preferences) {
		delete Preferences;
	}
	
	// calling cleanup here so that we can run this binary through a memory leak detector 
	// ICU will cache many things and that will cause the detector to output "possible leaks"
	// TODO: only use this during debug mode
	u_cleanup();
}


bool mvceditor::AppClass::CommandLine() {
	bool ret = true;
	wxCmdLineEntryDesc description[3];
	description[0].description = wxT("File name to open on startup");
	description[0].flags =  wxCMD_LINE_PARAM_OPTIONAL;
	description[0].kind = wxCMD_LINE_OPTION;
	description[0].longName = wxT("file");
	description[0].shortName = wxT("f");
	description[0].type = wxCMD_LINE_VAL_STRING;
	description[1].description = wxT("Project to open on startup");
	description[1].flags =  wxCMD_LINE_PARAM_OPTIONAL;
	description[1].kind = wxCMD_LINE_OPTION;
	description[1].longName = wxT("project");
	description[1].shortName = wxT("p");
	description[1].type = wxCMD_LINE_VAL_STRING;
	description[2].description = wxT("");
	description[2].flags =  0;
	description[2].kind = wxCMD_LINE_NONE;
	description[2].longName = NULL;
	description[2].shortName = NULL;
	description[2].type = wxCMD_LINE_VAL_NONE;
	wxCmdLineParser parser(description, argc, argv);
	parser.SetLogo(wxT("MVC Editor"));
	int result = parser.Parse(true);
	if (0 == result) {
		wxString filename,
			projectDirectory;
		if (parser.Found(wxT("file"), &filename)) {
			std::vector<wxString> filenames;
			filenames.push_back(filename);
			AppFrame->FileOpen(filenames);
		}
		if (parser.Found(wxT("project"), &projectDirectory)) {
			ProjectOpen(projectDirectory);
		}
	}
	else if (-1 == result) {
		ret = false;
	}
	return ret;
}



void mvceditor::AppClass::CloseProject() {
	if (AppFrame) {
		AppFrame->OnProjectClosed();
	}
	for (size_t i = 0; i < Plugins.size(); ++i) {
		Plugins[i]->SetProject(NULL);
	}
	if (Project) {
		delete Project;
		Project = NULL;
	}
}

void mvceditor::AppClass::CreatePlugins() {
	PluginClass* plugin = new RunConsolePluginClass();
	Plugins.push_back(plugin);
	plugin = new FinderPluginClass();
	Plugins.push_back(plugin);
	plugin = new FindInFilesPluginClass();
	Plugins.push_back(plugin);
	plugin = new ResourcePluginClass();
	Plugins.push_back(plugin);
	plugin = new EnvironmentPluginClass();
	Plugins.push_back(plugin);
	plugin = new ProjectPluginClass();
	Plugins.push_back(plugin);
	plugin = new OutlineViewPluginClass();
	Plugins.push_back(plugin);
	plugin = new LintPluginClass();
	Plugins.push_back(plugin);
	plugin = new SqlBrowserPluginClass();
	Plugins.push_back(plugin);

	// test plugin need to find a quicker way to toggling it ON / OFF
	//plugin = new TestPluginClass();
	//Plugins.push_back(plugin);
}

void mvceditor::AppClass::PluginWindows() {
	for (size_t i = 0; i < Plugins.size(); ++i) {
		Plugins[i]->SetProject(Project);
		AppFrame->LoadPlugin(Plugins[i]);

		// propagate GUI events to plugins, so that they can handle menu events themselves
		// their own menus
		AppFrame->PushEventHandler(Plugins[i]);
	}
}

void mvceditor::AppClass::DeletePlugins() {
	// Since this is called in the destructor
	// the AppFrame pointer has been deleted, can't pop
	// the plugins that were pushed.
	//for (size_t i = 0; i < Plugins.size(); ++i) {
	//	AppFrame->PopEventHandler();
	//}
	
	// if i delete in the same loop as the PopEventHandler, wx assertions fail.
	for (size_t i = 0; i < Plugins.size(); ++i) {
		delete Plugins[i];
	}
	Plugins.clear();
}

void mvceditor::AppClass::ProjectOpen(const wxString& directoryPath) {
	ProjectOptionsClass options;
	options.RootPath = directoryPath;
	CloseProject();
	Project = new ProjectClass(options, Environment);
	FrameworkIdentifiersLeftToDetect.clear();
	wxString cmd = Project->DetectFrameworkCommand();
	long pid = 0;
	if (!ProcessWithHeartbeat.Init(cmd, ID_FRAMEWORK_DETECT_PROCESS, pid)) {
		wxMessageBox(_("Error in PHP framework detection. Is PHP location correct?"));
	}
}

void mvceditor::AppClass::OnSavePreferences(wxCommandEvent& event) {
	Preferences->LoadKeyboardShortcuts(AppFrame);
	wxConfigBase* config = wxConfigBase::Get();
	for (size_t i = 0; i < Plugins.size(); ++i) {
		Plugins[i]->SavePreferences(config);
	}
	config->Flush();
}

void mvceditor::AppClass::OnFileSaved(wxCommandEvent& event) {
	for (size_t i = 0; i < Plugins.size(); i++) {
		wxPostEvent(Plugins[i], event);
	}
}

void mvceditor::AppClass::OnProjectOpen(wxCommandEvent& event) {
	wxString directoryPath = event.GetString();
	ProjectOpen(directoryPath);
}

void mvceditor::AppClass::OnProcessComplete(wxCommandEvent& event) {
	bool continueProjectOpen = true;
	if (event.GetId() == ID_FRAMEWORK_DETECT_PROCESS) {

		// framework detection complete.  if a known framework 
		// was detected get the DB info, else just continue
		// opening the project
		Project->DetectFrameworkResponse(event.GetString());
		std::vector<wxString> frameworks = Project->FrameworkIdentifiers();
		if (!frameworks.empty()) {
			for (size_t i = 0; i < frameworks.size(); i++) {
				wxString cmd = Project->DetectDatabaseCommand(frameworks[i]);
				long pid = 0;
				if (ProcessWithHeartbeat.Init(cmd, ID_DATABASE_DETECT_PROCESS, pid)) {
					FrameworkIdentifiersLeftToDetect.push_back(frameworks[i]);
				}
			}
			continueProjectOpen = false;
		}
	}
	else if (event.GetId() == ID_DATABASE_DETECT_PROCESS) {
		
		// detection of database settings for ONE framework has completed.
		wxString output = event.GetString();
		Project->DetectDatabaseResponse(output);

		// at this point we dont use the framework name here; just need a 'counter'
		// to know when all frameworks have been detected
		FrameworkIdentifiersLeftToDetect.pop_back();
		if (!FrameworkIdentifiersLeftToDetect.empty()) {
			continueProjectOpen = false;
		}
	}
	if (continueProjectOpen) {
		AppFrame->OnProjectOpened(Project);
		for (size_t i = 0; i < Plugins.size(); ++i) {
			Plugins[i]->SetProject(Project);
		}
	}
}

void mvceditor::AppClass::OnProcessFailed(wxCommandEvent& event) {
	wxMessageBox(event.GetString());
}

BEGIN_EVENT_TABLE(mvceditor::AppClass, wxApp)
	EVT_COMMAND(wxID_ANY, EVENT_APP_SAVE_PREFERENCES, mvceditor::AppClass::OnSavePreferences)
	EVT_COMMAND(wxID_ANY, EVENT_PLUGIN_FILE_SAVED, mvceditor::AppClass::OnFileSaved)
	EVT_COMMAND(wxID_ANY, EVENT_APP_OPEN_PROJECT, mvceditor::AppClass::OnProjectOpen)
	EVT_COMMAND(mvceditor::ID_FRAMEWORK_DETECT_PROCESS, mvceditor::EVENT_PROCESS_COMPLETE, mvceditor::AppClass::OnProcessComplete)
	EVT_COMMAND(mvceditor::ID_DATABASE_DETECT_PROCESS, mvceditor::EVENT_PROCESS_COMPLETE, mvceditor::AppClass::OnProcessComplete)
	EVT_COMMAND(mvceditor::ID_FRAMEWORK_DETECT_PROCESS, mvceditor::EVENT_PROCESS_FAILED, mvceditor::AppClass::OnProcessFailed)
	EVT_COMMAND(mvceditor::ID_DATABASE_DETECT_PROCESS, mvceditor::EVENT_PROCESS_FAILED, mvceditor::AppClass::OnProcessFailed)
END_EVENT_TABLE()
