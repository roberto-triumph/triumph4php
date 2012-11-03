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
#include <plugins/RecentFilesPluginClass.h>
#include <globals/Errors.h>
#include <globals/Assets.h>

IMPLEMENT_APP(mvceditor::AppClass)

mvceditor::AppClass::AppClass()
	: wxApp()
	, Structs()
	, RunningThreads()
	, EventSink()
	, ConfigLastModified()
	, Plugins()
	, Preferences()
	, Timer(*this)
	, EditorMessagesPlugin(NULL) 
	, IsAppReady(false) {
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
	AppFrame = new mvceditor::AppFrameClass(Plugins, *this, Preferences);
	PluginWindows();
	LoadPreferences();
	AppFrame->AuiManagerUpdate();
	if (CommandLine()) {
		SetTopWindow(AppFrame);
		AppFrame->Show(true);

		// maximize only after showing, that way the size event gets propagated and
		// the main frame is drawn correctly at app start.
		// if we don't do this, there is a nasty effect on windows OS that shows 
		// the status bar in the middle of the page until the user re-maximizes the app
		AppFrame->Maximize();

		// this line is needed so that we get all the wxLogXXX messages
		// pointer will be managed by wxWidgets
		// need to put this here because the logger needs an initialized window state
		wxLog::SetActiveTarget(new mvceditor::EditorMessagesLoggerClass(*EditorMessagesPlugin));
		Timer.Start(1000, wxTIMER_CONTINUOUS);
		return true;
	}
	return false;
}

mvceditor::AppClass::~AppClass() {
	Timer.Stop();
	DeletePlugins();
	
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
	}
	else if (-1 == result) {
		ret = false;
	}
	return ret;
}

void mvceditor::AppClass::CreatePlugins() {
	PluginClass* plugin = new RunConsolePluginClass(*this);
	Plugins.push_back(plugin);
	plugin = new FinderPluginClass(*this);
	Plugins.push_back(plugin);
	plugin = new FindInFilesPluginClass(*this);
	Plugins.push_back(plugin);
	plugin = new ResourcePluginClass(*this);
	Plugins.push_back(plugin);
	plugin = new EnvironmentPluginClass(*this);
	Plugins.push_back(plugin);	
	plugin = new ProjectPluginClass(*this);
	Plugins.push_back(plugin);
	plugin = new OutlineViewPluginClass(*this);
	Plugins.push_back(plugin);
	plugin = new LintPluginClass(*this);
	Plugins.push_back(plugin);
	plugin = new SqlBrowserPluginClass(*this);
	Plugins.push_back(plugin);

	EditorMessagesPlugin = new mvceditor::EditorMessagesPluginClass(*this);
	Plugins.push_back(EditorMessagesPlugin);

	plugin = new CodeIgniterPluginClass(*this);
	Plugins.push_back(plugin);
	plugin = new RunBrowserPluginClass(*this);
	Plugins.push_back(plugin);
	plugin = new ViewFilePluginClass(*this);
	Plugins.push_back(plugin);
	plugin = new RecentFilesPluginClass(*this);
	Plugins.push_back(plugin);
	
	// TODO test plugin need to find a quicker way to toggling it ON / OFF
	//plugin = new TestPluginClass(*this);
	//Plugins.push_back(plugin);

	// connect the plugins to the event sink so that they can
	// receive app events
	for (size_t i = 0; i < Plugins.size(); ++i) {
		EventSink.PushHandler(Plugins[i]);
	}
}

void mvceditor::AppClass::PluginWindows() {
	for (size_t i = 0; i < Plugins.size(); ++i) {
		AppFrame->LoadPlugin(Plugins[i]);
	}
}

void mvceditor::AppClass::DeletePlugins() {

	// if i delete in the same loop as the PopEventHandler, wx assertions fail.
	for (size_t i = 0; i < Plugins.size(); ++i) {
		delete Plugins[i];
	}
	Plugins.clear();
	EditorMessagesPlugin = NULL;
}

void mvceditor::AppClass::LoadPreferences() {

	// load any settings from .INI files
	PreferencesClass::InitConfig();
	wxConfigBase* config = wxConfigBase::Get();
	Structs.Environment.LoadFromConfig(config);
	for (size_t i = 0; i < Plugins.size(); ++i) {
		Plugins[i]->LoadPreferences(config);
		Plugins[i]->AddKeyboardShortcuts(Preferences.DefaultKeyboardShortcutCmds);
	}	
	Preferences.Load(config, AppFrame);
}

void mvceditor::AppClass::StopConfigModifiedCheck() {
	Timer.Stop();	
}

void mvceditor::AppClass::UpdateConfigModifiedTime() {
	wxFileName configFileName(mvceditor::ConfigDirAsset().GetPath(), wxT("mvc-editor.ini"));
	if (configFileName.FileExists()) {
		ConfigLastModified = configFileName.GetModificationTime();
	}
	Timer.Start();
}

mvceditor::AppTimerClass::AppTimerClass(mvceditor::AppClass& app)
	: wxTimer()
	, App(app) {
}

void mvceditor::AppTimerClass::Notify() {
	
	// tell all plugins that the app is ready to use
	// the plugins will do / should do  their grunt
	// work in their event handler
	if (!App.IsAppReady) {
		App.IsAppReady = true;
		wxCommandEvent evt(mvceditor::EVENT_APP_READY);
		App.EventSink.Publish(evt);
		wxFileName configFileName(mvceditor::ConfigDirAsset().GetPath(), wxT("mvc-editor.ini"));
		if (configFileName.FileExists()) {
			App.ConfigLastModified = configFileName.GetModificationTime();
		}
	}
	else {
		wxFileName configFileName(mvceditor::ConfigDirAsset().GetPath(), wxT("mvc-editor.ini"));
		if (configFileName.FileExists()) {
			wxDateTime lastModified = configFileName.GetModificationTime();
			if (lastModified.IsLaterThan(App.ConfigLastModified)) {

				// stop the timer so that we dont show this prompt multiple times
				App.Timer.Stop();
				wxString msg = wxString::FromAscii(
					"Preferences have been modified externally.\n"
					"Reload the preferences?"	
				);
				msg = wxGetTranslation(msg);
				int res = wxMessageBox(msg, _("Reload preferences"), wxCENTRE | wxYES_NO, App.AppFrame);
				if (wxYES == res) {

					// delete the old config ourselves
					wxConfigBase* oldConfig = wxConfig::Get();
					delete oldConfig;
					wxConfig::Set(NULL);
					App.Preferences.ClearAllShortcuts();
					App.LoadPreferences();

					App.ConfigLastModified = configFileName.GetModificationTime();
				}
				App.Timer.Start(1000, wxTIMER_CONTINUOUS);
			}
		}
	}
}