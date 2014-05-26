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
#include <Triumph.h>
#include <wx/cmdline.h>
#include <wx/fileconf.h>
#include <unicode/uclean.h>
#include <soci/mysql/soci-mysql.h>
#include <soci/sqlite3/soci-sqlite3.h>

#include <main_frame/MainFrameClass.h>
#include <features/EnvironmentFeatureClass.h>
#include <features/FindInFilesFeatureClass.h>
#include <features/FinderFeatureClass.h>
#include <features/ProjectFeatureClass.h>
#include <features/OutlineViewFeatureClass.h>
#include <features/TagFeatureClass.h>
#include <features/RunConsoleFeatureClass.h>
#include <features/RunBrowserFeatureClass.h>
#include <features/LintFeatureClass.h>
#include <features/SqlBrowserFeatureClass.h>
#include <features/EditorMessagesFeatureClass.h>
#include <features/RecentFilesFeatureClass.h>
#include <features/DetectorFeatureClass.h>
#include <features/TemplateFilesFeatureClass.h>
#include <features/ConfigFilesFeatureClass.h>
#include <features/FileModifiedCheckFeatureClass.h>
#include <features/FileWatcherFeatureClass.h>
#include <features/ExplorerFeatureClass.h>
#include <features/NewUserFeatureClass.h>
#include <features/VersionUpdateFeatureClass.h>
#include <features/TotalSearchFeatureClass.h>
#include <features/DocCommentFeatureClass.h>
#include <features/SyntaxHighlightFeatureClass.h>
#include <features/EditorBehaviorFeatureClass.h>
#include <features/ChangelogFeatureClass.h>
#include <features/BookmarkFeatureClass.h>
#include <globals/Errors.h>
#include <globals/Assets.h>

IMPLEMENT_APP(t4p::AppClass)

/**
 * class that will cleanup mysql thread data.  we will give an instance
 * of this class to RunningThreads.
 * The mysql driver allocates data per thread; we need to clean it up
 * when the thread dies.
 */
namespace t4p {

class MysqlThreadCleanupClass : public t4p::ThreadCleanupClass {

public:

	MysqlThreadCleanupClass() 
	: ThreadCleanupClass() {
		
	}

	void ThreadEnd() {

		// clean up the MySQL library. 
		// mysql has stuff that gets created per each thread
		mysql_thread_end();
	
	}

	t4p::ThreadCleanupClass* Clone() {
		return new MysqlThreadCleanupClass();
	}
};

}

static int ID_EVENT_CONFIG_FILE_CHECK = wxNewId();

t4p::AppClass::AppClass()
	: wxApp()
	, Globals()
	, RunningThreads()
	, SqliteRunningThreads()
	, EventSink()
	, GlobalsChangeHandler(Globals)
	, Sequences(Globals, SqliteRunningThreads)
	, Preferences()
	, ConfigLastModified()
	, Features()
	, Timer(*this)
	, EditorMessagesFeature(NULL) 
	, IsAppReady(false) {
	MainFrame = NULL;
}

/**
 * when app starts, create the new app frame
 */
bool t4p::AppClass::OnInit() {
	
	// 1 ==> to make sure any queued items are done one at a time
	SqliteRunningThreads.SetMaxThreads(1);
	
	RunningThreads.SetThreadCleanup(new t4p::MysqlThreadCleanupClass);
	
	// not really needed since we will use this running threads for sqlite actions only,
	// but just in case
	SqliteRunningThreads.SetThreadCleanup(new t4p::MysqlThreadCleanupClass);
	Globals.Environment.Init();
	Preferences.Init();
	SqliteRunningThreads.AddEventHandler(&GlobalsChangeHandler);
	RunningThreads.AddEventHandler(&Timer);
	CreateFeatures();

	// initialize the  mysql library
	// do it now for 2 reasons
	// 1. this function should not be called inside a thread, and our
	//    threads may use SOCI (which calls the mysql library init function)
	// 2. there could be a case where the thread cleanup class (above)
	//    tries to call mysql_thread_end() before mysql_library_init()
	//    is called. for example when the user opens then closes
	//    the program real quick.  specifying the init here prevents
	//    crashes.
	mysql_library_init(0, NULL, NULL);
	sqlite_api::sqlite3_initialize();

	// due to the way keyboard shortcuts are serialized, we need to load the
	// frame and initialize the feature windows so that all menus are created
	// and only then can we load the keyboard shortcuts from the INI file
	// all menu items must be present in the menu bar for shortcuts to take effect
	MainFrame = new t4p::MainFrameClass(Features, *this, Preferences);
	FeatureWindows();
	LoadPreferences();
	MainFrame->AuiManagerUpdate();
	if (CommandLine()) {
		MainFrame->SetIcon(t4p::IconImageAsset(wxT("triumph4php")));
		SetTopWindow(MainFrame);
		MainFrame->Show(true);

		// maximize only after showing, that way the size event gets propagated and
		// the main frame is drawn correctly at app start.
		// if we don't do this, there is a nasty effect on windows OS that shows 
		// the status bar in the middle of the page until the user re-maximizes the app
		MainFrame->Maximize();

		// this line is needed so that we get all the wxLogXXX messages
		// pointer will be managed by wxWidgets
		// need to put this here because the logger needs an initialized window state
		if (EditorMessagesFeature) {
			wxLog::SetActiveTarget(new t4p::EditorMessagesLoggerClass(*EditorMessagesFeature));
		}
		Timer.Start(1000, wxTIMER_CONTINUOUS);
		return true;
	}
	return false;
}

t4p::AppClass::~AppClass() {
	Timer.Stop();
	RunningThreads.RemoveEventHandler(&GlobalsChangeHandler);
	RunningThreads.RemoveEventHandler(&Timer);
	DeleteFeatures();

	// must close all soci sessions before shutting down sqlite library
	Globals.Close();

	// calling cleanup here so that we can run this binary through a memory leak detector 
	// ICU will cache many things and that will cause the detector to output "possible leaks"
	// TODO: only use this during debug mode
	u_cleanup();
	mysql_library_end();
	sqlite_api::sqlite3_shutdown();
}


bool t4p::AppClass::CommandLine() {
	bool ret = true;
	wxCmdLineEntryDesc description[3];
	description[0].description = "File name to open on startup";
	description[0].flags =  wxCMD_LINE_PARAM_OPTIONAL;
	description[0].kind = wxCMD_LINE_OPTION;
	description[0].longName = "file";
	description[0].shortName = "f";
	description[0].type = wxCMD_LINE_VAL_STRING;
	description[1].description = "Project to open on startup";
	description[1].flags =  wxCMD_LINE_PARAM_OPTIONAL;
	description[1].kind = wxCMD_LINE_OPTION;
	description[1].longName = "project";
	description[1].shortName = "p";
	description[1].type = wxCMD_LINE_VAL_STRING;
	description[2].description = "";
	description[2].flags =  0;
	description[2].kind = wxCMD_LINE_NONE;
	description[2].longName = NULL;
	description[2].shortName = NULL;
	description[2].type = wxCMD_LINE_VAL_NONE;
	wxCmdLineParser parser(description, argc, argv);
	parser.SetLogo(wxT("Triumph"));
	int result = parser.Parse(true);
	if (0 == result) {
		wxString filename,
			projectDirectory;
		if (parser.Found(wxT("file"), &filename)) {
			std::vector<wxString> filenames;
			filenames.push_back(filename);
			MainFrame->FileOpen(filenames);
		}
	}
	else if (-1 == result) {
		ret = false;
	}
	return ret;
}

void t4p::AppClass::CreateFeatures() {
	FeatureClass* feature;
	feature = new RunConsoleFeatureClass(*this);
	Features.push_back(feature);
	feature = new FinderFeatureClass(*this);
	Features.push_back(feature);
	feature = new FindInFilesFeatureClass(*this);
	Features.push_back(feature);
	feature = new TagFeatureClass(*this);
	Features.push_back(feature);
	feature = new EnvironmentFeatureClass(*this);
	Features.push_back(feature);	
	feature = new ProjectFeatureClass(*this);
	Features.push_back(feature);
	feature = new OutlineViewFeatureClass(*this);
	Features.push_back(feature);
	feature = new LintFeatureClass(*this);
	Features.push_back(feature);
	feature = new SqlBrowserFeatureClass(*this);
	Features.push_back(feature);
	EditorMessagesFeature = new t4p::EditorMessagesFeatureClass(*this);
	Features.push_back(EditorMessagesFeature);
	feature = new RunBrowserFeatureClass(*this);
	Features.push_back(feature);
	feature = new RecentFilesFeatureClass(*this);
	Features.push_back(feature);
	feature = new DetectorFeatureClass(*this);
	Features.push_back(feature);
	feature =  new TemplateFilesFeatureClass(*this);
	Features.push_back(feature);
	feature = new ConfigFilesFeatureClass(*this);
	Features.push_back(feature);
	feature = new FileModifiedCheckFeatureClass(*this);
	Features.push_back(feature);
	feature = new FileWatcherFeatureClass(*this);
	Features.push_back(feature);
	feature = new ExplorerFeatureClass(*this);
	Features.push_back(feature);
	feature = new NewUserFeatureClass(*this);
	Features.push_back(feature);
	feature = new VersionUpdateFeatureClass(*this);
	Features.push_back(feature);
	feature = new TotalSearchFeatureClass(*this);
	Features.push_back(feature);
	feature = new DocCommentFeatureClass(*this);
	Features.push_back(feature);
	feature = new SyntaxHighlightFeatureClass(*this);
	Features.push_back(feature);
	feature = new EditorBehaviorFeatureClass(*this);
	Features.push_back(feature);
	feature = new ChangelogFeatureClass(*this);
	Features.push_back(feature);
	feature = new BookmarkFeatureClass(*this);
	Features.push_back(feature);
	
	// TODO test feature need to find a quicker way to toggling it ON / OFF
	//feature = new TestFeatureClass(*this);
	//Features.push_back(feature);

	// connect the features to the event sink so that they can
	// receive app events
	for (size_t i = 0; i < Features.size(); ++i) {
		EventSink.PushHandler(Features[i]);
		RunningThreads.AddEventHandler(Features[i]);
		SqliteRunningThreads.AddEventHandler(Features[i]);
	}
}

void t4p::AppClass::FeatureWindows() {
	for (size_t i = 0; i < Features.size(); ++i) {
		MainFrame->LoadFeature(Features[i]);
	}
	MainFrame->RealizeToolbar();
}

void t4p::AppClass::DeleteFeatures() {
	for (size_t i = 0; i < Features.size(); ++i) {
		RunningThreads.RemoveEventHandler(Features[i]);
		SqliteRunningThreads.RemoveEventHandler(Features[i]);
	}

	// disconnect from events so that events dont get sent after
	// features are destroyed
	EventSink.RemoveAllHandlers();

	// now it should be safe to 
	for (size_t i = 0; i < Features.size(); ++i) {
		delete Features[i];
	}
	Features.clear();
	EditorMessagesFeature = NULL;
}

void t4p::AppClass::LoadPreferences() {

	// load any settings from .INI files
	bool validConfigDir = PreferencesClass::InitConfig();
	if (!validConfigDir) {
		wxFileName configDir = t4p::ConfigDirAsset();
		t4p::EditorLogError(t4p::ERR_INVALID_SETTINGS_DIRECTORY, configDir.GetPath());
	}
	wxConfigBase* config = wxConfigBase::Get();
	Globals.Environment.LoadFromConfig(config);

	// tell each feature to load their settings from the INI file
	for (size_t i = 0; i < Features.size(); ++i) {
		Features[i]->LoadPreferences(config);
		Features[i]->AddKeyboardShortcuts(Preferences.DefaultKeyboardShortcutCmds);
	}	
	Preferences.Load(config, MainFrame);
}

void t4p::AppClass::SavePreferences(const wxFileName& settingsDir, bool changedDirectory) {
	if (changedDirectory) {

		// write the location of the settings dir to the bootstrap file
		Preferences.SetSettingsDir(settingsDir);

		// close the connections to the tag cache files
		Globals.Close();

		// read the config; it now point to the newly chosen dir
		Globals.TagCacheDbFileName = t4p::TagCacheAsset();
		Globals.DetectorCacheDbFileName = t4p::DetectorCacheAsset();

		// perform the app start sequence, which will open the tag caches
		Sequences.AppStart();
	}
	// save global preferences; keyboard shortcuts / syntax colors
	Preferences.Save();

	// tell each feature to save their own config 
	wxCommandEvent evt(t4p::EVENT_APP_PREFERENCES_SAVED);
	EventSink.Publish(evt);

	// sine the event handlers have updated the config; lets persist the changes
	wxConfig::Get()->Flush();

	// signal that this app has modified the config file, that way the external
	// modification check fails and the user will not be prompted to reload the config
	UpdateConfigModifiedTime();
}

void t4p::AppClass::StopConfigModifiedCheck() {
	Timer.Stop();	
}

void t4p::AppClass::UpdateConfigModifiedTime() {
	wxFileName configFileName(t4p::ConfigDirAsset().GetPath(), wxT("triumph4php.ini"));
	if (configFileName.FileExists()) {
		ConfigLastModified = configFileName.GetModificationTime();
	}
	Timer.Start();
}

void t4p::AppClass::OnActivateApp(wxActivateEvent& event) {
	if (IsActive()) {
		wxCommandEvent cmdEvent(t4p::EVENT_APP_ACTIVATED);
		EventSink.Publish(cmdEvent);
	}
}

t4p::AppTimerClass::AppTimerClass(t4p::AppClass& app)
	: wxTimer()
	, App(app) {
	SetOwner(this, ID_EVENT_CONFIG_FILE_CHECK);
}

void t4p::AppTimerClass::Notify() {
	
	// tell all features that the app is ready to use
	// the features will do / should do  their grunt
	// work in their event handler
	if (!App.IsAppReady) {
		App.IsAppReady = true;
		wxFileName settingsDir = t4p::SettingsDirAsset();
		wxCommandEvent evt(t4p::EVENT_APP_READY);
		App.EventSink.Publish(evt);

		if (settingsDir.IsOk()) {
			App.Sequences.AppStart();
		}
		wxFileName configFileName(t4p::ConfigDirAsset().GetPath(), wxT("triumph4php.ini"));
		if (configFileName.FileExists()) {
			App.ConfigLastModified = configFileName.GetModificationTime();
		}
	}
	else {
		wxFileName configFileName(t4p::ConfigDirAsset().GetPath(), wxT("triumph4php.ini"));
		if (configFileName.FileExists()) {
			t4p::FileModifiedCheckActionClass* action = 
				new t4p::FileModifiedCheckActionClass(App.RunningThreads, ID_EVENT_CONFIG_FILE_CHECK);
			std::vector<t4p::FileModifiedTimeClass> fileMods;
			t4p::FileModifiedTimeClass fileMod;
			fileMod.FileName = configFileName;
			fileMod.ModifiedTime = App.ConfigLastModified;
			fileMods.push_back(fileMod);
			action->SetFiles(fileMods);

			App.RunningThreads.Queue(action);
		}
	}
}

void t4p::AppTimerClass::OnConfigFileModified(t4p::FilesModifiedEventClass& event) {

	// stop the timer so that we dont show this prompt multiple times
	App.Timer.Stop();
	if (event.Modified.empty()) {
		return;
	}
	wxFileName configFileName = event.Modified[0];
	wxString msg = wxString::FromAscii(
		"Preferences have been modified externally.\n"
		"Reload the preferences?"	
	);
	msg = wxGetTranslation(msg);
	int res = wxMessageBox(msg, _("Reload preferences"), wxCENTRE | wxYES_NO, App.MainFrame);
	if (wxYES == res) {

		// delete the old config ourselves
		wxConfigBase* oldConfig = wxConfig::Get();
		delete oldConfig;
		wxConfig::Set(NULL);
		App.Preferences.ClearAllShortcuts();
		App.LoadPreferences();

		App.ConfigLastModified = event.ModifiedTimes[0];
	}
	else {
		
		// update the time so that we dont continually ask the user the prompt
		App.ConfigLastModified = event.ModifiedTimes[0];
	}
	App.Timer.Start(1000, wxTIMER_CONTINUOUS);
}

BEGIN_EVENT_TABLE(t4p::AppTimerClass, wxTimer)
	EVT_FILES_EXTERNALLY_MODIFIED_COMPLETE(ID_EVENT_CONFIG_FILE_CHECK, t4p::AppTimerClass::OnConfigFileModified)
END_EVENT_TABLE()


BEGIN_EVENT_TABLE(t4p::AppClass, wxApp)
	EVT_ACTIVATE_APP(t4p::AppClass::OnActivateApp)
END_EVENT_TABLE()