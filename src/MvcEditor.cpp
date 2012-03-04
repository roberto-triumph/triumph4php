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
#include <MvcEditor.h>
#include <wx/cmdline.h>
#include <unicode/uclean.h>

#include <windows/AppFrameClass.h>
#include <widgets/StatusBarWithGaugeClass.h>
#include <plugins/EnvironmentPluginClass.h>
#include <plugins/FindInFilesPluginClass.h>
#include <plugins/FinderPluginClass.h>
#include <plugins/ProjectPluginClass.h>
#include <plugins/OutlineViewPluginClass.h>
#include <plugins/ResourcePluginClass.h>
#include <plugins/RunConsolePluginClass.h>
#include <plugins/RunBrowserPluginClass.h>
#include <plugins/LintPluginClass.h>
#include <plugins/SqlBrowserPluginClass.h>
#include <plugins/EditorMessagesPluginClass.h>
#include <plugins/CodeIgniterPluginClass.h>
#include <plugins/ViewFilePluginClass.h>
#include <MvcEditorErrors.h>

IMPLEMENT_APP(mvceditor::AppClass)

static const int ID_FRAMEWORK_DETECTION_GAUGE = wxNewId();

mvceditor::AppClass::AppClass()
	: wxApp()
	, Environment()
	, ResourceCache()
	, PhpFrameworks(*this, Environment)
	, UrlResourceFinder()
	, Plugins()
	, Preferences()
	, Project(NULL)
	, ResourcePlugin(NULL)
	, ProjectPlugin(NULL) {
	AppFrame = NULL;
}

/**
 * when app starts, create the new app frame
 */
bool mvceditor::AppClass::OnInit() {
	Preferences.Init();
	CreatePlugins();

	// due to the way keyboard shortcuts are serialized, we need to load the
	// frame and initialize the plugin windows so that all menus are created
	// and only then can we load the keyboard shortcuts from the INI file
	// all menu items must be present in the menu bar for shortcuts to take effect
	AppFrame = new mvceditor::AppFrameClass(Plugins, *this, Environment, Preferences, ResourceCache);
	PluginWindows();

	// load any settings from .INI files
	PreferencesClass::InitConfig();
	Environment.LoadFromConfig();
	wxConfigBase* config = wxConfigBase::Get();
	for (size_t i = 0; i < Plugins.size(); ++i) {
		Plugins[i]->InitState(this);
		Plugins[i]->LoadPreferences(config);
		Plugins[i]->AddKeyboardShortcuts(Preferences.DefaultKeyboardShortcutCmds);
	}	
	Preferences.Load(AppFrame);

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
		AppFrame->OnProjectClosed(this);
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

	// we want to keep a reference to this plugin
	// to fulfill the EVENT_APP_PROJECT_RE_INDEX
	ResourcePlugin = new ResourcePluginClass();
	Plugins.push_back(ResourcePlugin);

	plugin = new EnvironmentPluginClass();
	Plugins.push_back(plugin);

	// we want to keep a reference to this plugin
	// we need to load the project options before all others
	ProjectPlugin = new ProjectPluginClass();
	Plugins.push_back(ProjectPlugin);
	plugin = new OutlineViewPluginClass();
	Plugins.push_back(plugin);
	plugin = new LintPluginClass();
	Plugins.push_back(plugin);
	plugin = new SqlBrowserPluginClass();
	Plugins.push_back(plugin);
	plugin = new mvceditor::EditorMessagesPluginClass();
	Plugins.push_back(plugin);
	plugin = new CodeIgniterPluginClass();
	Plugins.push_back(plugin);
	plugin = new RunBrowserPluginClass();
	Plugins.push_back(plugin);
	plugin = new ViewFilePluginClass();
	Plugins.push_back(plugin);
	
	// test plugin need to find a quicker way to toggling it ON / OFF
	//plugin = new TestPluginClass();
	//Plugins.push_back(plugin);
}

void mvceditor::AppClass::PluginWindows() {
	for (size_t i = 0; i < Plugins.size(); ++i) {
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
	Project = new ProjectClass(options);

	// on startup the editor is in "non-project" mode (no root path)
	if (!directoryPath.IsEmpty()) {
		bool started = PhpFrameworks.Init(directoryPath);
		if (!started) {
			mvceditor::EditorLogError(mvceditor::BAD_PHP_EXECUTABLE, Environment.Php.PhpExecutablePath); 
		}
		else {
			mvceditor::StatusBarWithGaugeClass* gauge = wxDynamicCast(AppFrame->GetStatusBar(), mvceditor::StatusBarWithGaugeClass);
			gauge->AddGauge(_("Framework Detection"), ID_FRAMEWORK_DETECTION_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE, 0);
		}
	}
	else {

		// still notify the plugins of the new project at program startup
		AppFrame->OnProjectOpened(Project, this);
		ProjectPlugin->SetProject(Project);
		for (size_t i = 0; i < Plugins.size(); ++i) {
			if (Plugins[i] != ProjectPlugin) {
				Plugins[i]->SetProject(Project);
			}
		}
	}
}

void mvceditor::AppClass::OnSavePreferences(wxCommandEvent& event) {
	Preferences.EnableSelectedProfile(AppFrame);
	wxConfigBase* config = wxConfigBase::Get();
	for (size_t i = 0; i < Plugins.size(); ++i) {
		Plugins[i]->SavePreferences(config);
	}
	config->Flush();
}

void mvceditor::AppClass::OnProjectOpen(wxCommandEvent& event) {
	wxString directoryPath = event.GetString();
	ProjectOpen(directoryPath);
}

void mvceditor::AppClass::OnFrameworkDetectionComplete(wxCommandEvent& event) {
	for (size_t i = 0; i < PhpFrameworks.Databases.size(); ++i) {
		Project->PushDatabaseInfo(PhpFrameworks.Databases[i]);
	}	
	AppFrame->OnProjectOpened(Project, this);
	ProjectPlugin->SetProject(Project);
	for (size_t i = 0; i < Plugins.size(); ++i) {
		if (Plugins[i] != ProjectPlugin) {
			Plugins[i]->SetProject(Project);
		}
	}
	mvceditor::StatusBarWithGaugeClass* gauge = wxDynamicCast(AppFrame->GetStatusBar(), mvceditor::StatusBarWithGaugeClass);
	gauge->StopGauge(ID_FRAMEWORK_DETECTION_GAUGE);
}

void mvceditor::AppClass::OnFrameworkDetectionFailed(wxCommandEvent& event) {
	EditorLogError(mvceditor::BAD_PHP_EXECUTABLE, event.GetString());

	// still need to set the project even if the project detection fails
	// pretty much all code depends on having a Project pointer
	AppFrame->OnProjectOpened(Project, this);
	ProjectPlugin->SetProject(Project);
	for (size_t i = 0; i < Plugins.size(); ++i) {
		if (Plugins[i] != ProjectPlugin) {
			Plugins[i]->SetProject(Project);
		}
	}
	mvceditor::StatusBarWithGaugeClass* gauge = wxDynamicCast(AppFrame->GetStatusBar(), mvceditor::StatusBarWithGaugeClass);
	gauge->StopGauge(ID_FRAMEWORK_DETECTION_GAUGE);
}

void mvceditor::AppClass::OnFrameworkDetectionInProgress(wxCommandEvent& event) {
	mvceditor::StatusBarWithGaugeClass* gauge = wxDynamicCast(AppFrame->GetStatusBar(), mvceditor::StatusBarWithGaugeClass);
	gauge->IncrementGauge(ID_FRAMEWORK_DETECTION_GAUGE, mvceditor::StatusBarWithGaugeClass::INDETERMINATE_MODE);
}

void mvceditor::AppClass::OnProjectReIndex(wxCommandEvent& event) {
	wxASSERT(ResourcePlugin);

	// only attempt to index when a valid project is opened
	// otherwise user will get annoying messages.
	if (Project && !Project->GetRootPath().IsEmpty()) {
		ResourcePlugin->StartIndex();
	}
}

void mvceditor::AppClass::OnOpenFile(wxCommandEvent& event) {
	std::vector<wxString> filenames;
	filenames.push_back(event.GetString());
	AppFrame->FileOpen(filenames);
}

void mvceditor::AppClass::OnProjectIndexed(mvceditor::ProjectIndexedEventClass& event) {
	

	// ATTN: tried to use wxPostEvent to avoid defining another virtual method
	// but wxWidgets was having none of that ... infinite event loops were
	// ocurring
	mvceditor::ProjectIndexedEventClass newEvt;
	for (size_t i = 0; i < Plugins.size(); i++) {
		Plugins[i]->OnProjectIndexed();
	}
}

const wxEventType mvceditor::EVENT_APP_OPEN_PROJECT = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_SAVE_PREFERENCES = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_RE_INDEX = wxNewEventType();
const wxEventType mvceditor::EVENT_APP_OPEN_FILE = wxNewEventType();

BEGIN_EVENT_TABLE(mvceditor::AppClass, wxApp)
	EVT_COMMAND(wxID_ANY, EVENT_APP_SAVE_PREFERENCES, mvceditor::AppClass::OnSavePreferences)	
	EVT_COMMAND(wxID_ANY, EVENT_APP_OPEN_PROJECT, mvceditor::AppClass::OnProjectOpen)
	EVT_COMMAND(wxID_ANY, EVENT_APP_RE_INDEX, mvceditor::AppClass::OnProjectReIndex)
	EVT_COMMAND(wxID_ANY, EVENT_APP_OPEN_FILE, mvceditor::AppClass::OnOpenFile)	
	
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_FRAMEWORK_DETECTION_COMPLETE, mvceditor::AppClass::OnFrameworkDetectionComplete)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_FRAMEWORK_DETECTION_FAILED, mvceditor::AppClass::OnFrameworkDetectionFailed)
	EVT_COMMAND(wxID_ANY, mvceditor::EVENT_PROCESS_IN_PROGRESS, mvceditor::AppClass::OnFrameworkDetectionInProgress)
	EVT_PLUGIN_PROJECT_INDEXED(mvceditor::AppClass::OnProjectIndexed)
END_EVENT_TABLE()
