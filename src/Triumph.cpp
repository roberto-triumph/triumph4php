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

// include boost asio before any other file to prevent
//
// fatal error C1189: #error :  WinSock.h has already been included
//
// in MSW
#include <boost/asio.hpp>

#include <Triumph.h>
#include <wx/cmdline.h>
#include <wx/fileconf.h>
#include <unicode/uclean.h>
#include <soci/mysql/soci-mysql.h>
#include <soci/sqlite3/soci-sqlite3.h>

#include <main_frame/MainFrameClass.h>
#include <main_frame/MacCommonMenuBarClass.h>
#include <features/FeatureClass.h>
#include <views/FeatureViewClass.h>
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
	, FeatureFactory(*this)
	, Timer(*this)
	, MacCommonMenuBar(NULL)
	, IsAppReady(false) {
	MainFrame = NULL;
}

/**
 * when app starts, create the new app frame
 */
bool t4p::AppClass::OnInit() {
	
	// 1 ==> to make sure any queued items are done one at a time
	SqliteRunningThreads.SetMaxThreads(1);
	MacCommonMenuBar = new t4p::MacCommonMenuBarClass(*this);
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
	
	std::vector<wxString> filenames;
	if (CommandLine(filenames)) {
		CreateFrame();
		if (!filenames.empty()) {
			MainFrame->FileOpen(filenames);
		}
		Timer.Start(1000, wxTIMER_CONTINUOUS);
		return true;
	}
	return false;
}

void t4p::AppClass::CreateFrame() {
	wxASSERT_MSG(MainFrame == NULL, "main frame must not have been created");
	
	// due to the way keyboard shortcuts are serialized, we need to load the
	// frame and initialize the feature windows so that all menus are created
	// and only then can we load the keyboard shortcuts from the INI file
	// all menu items must be present in the menu bar for shortcuts to take effect
	MainFrame = new t4p::MainFrameClass(FeatureFactory.FeatureViews, *this);
	FeatureWindows();
	LoadPreferences();
	MainFrame->AuiManagerUpdate();
	MainFrame->SetIcon(t4p::IconImageAsset(wxT("triumph4php")));
	SetTopWindow(MainFrame);
	MainFrame->Show(true);

	// maximize only after showing, that way the size event gets propagated and
	// the main frame is drawn correctly at app start.
	// if we don't do this, there is a nasty effect on windows OS that shows 
	// the status bar in the middle of the page until the user re-maximizes the app
	MainFrame->Maximize();	
}

void t4p::AppClass::OnCommonMenuFileNew(wxCommandEvent& event)
{
	CreateFrame();
	
	wxCommandEvent appReady(t4p::EVENT_APP_READY);
	for (size_t i = 0; i < FeatureFactory.FeatureViews.size(); ++i) {
		FeatureFactory.FeatureViews[i]->ProcessEvent(appReady);
	}
	
	CreateNewCodeCtrl();
}

void t4p::AppClass::OnCommonMenuFileOpen(wxCommandEvent& event) {
	wxFileDialog dialog(
		NULL, 
		_("Select file to open"),
		"", "", "", wxFD_OPEN | wxFD_FILE_MUST_EXIST
	);
	if (wxOK == dialog.ShowModal()) {
		CreateFrame();
		
		wxCommandEvent appReady(t4p::EVENT_APP_READY);
		for (size_t i = 0; i < FeatureFactory.FeatureViews.size(); ++i) {
			FeatureFactory.FeatureViews[i]->ProcessEvent(appReady);
		}

		wxString name = dialog.GetPath();
		wxCommandEvent evt(t4p::EVENT_CMD_FILE_OPEN);
		evt.SetString(name);
		EventSink.Publish(evt);
	}
}

void t4p::AppClass::CreateNewCodeCtrl() {
	MainFrame->CreateNewCodeCtrl();
}

t4p::AppClass::~AppClass() {
	Timer.Stop();
	RunningThreads.RemoveEventHandler(&GlobalsChangeHandler);
	RunningThreads.RemoveEventHandler(&Timer);
	
	RunningThreads.Shutdown();
	SqliteRunningThreads.Shutdown();
		
	DeleteFeatures();
	DeleteFeatureViews();
	
	delete MacCommonMenuBar;

	// must close all soci sessions before shutting down sqlite library
	Globals.Close();

	// calling cleanup here so that we can run this binary through a memory leak detector 
	// ICU will cache many things and that will cause the detector to output "possible leaks"
	// TODO: only use this during debug mode
	u_cleanup();
	mysql_library_end();
	sqlite_api::sqlite3_shutdown();
}


bool t4p::AppClass::CommandLine(std::vector<wxString>& filenames) {
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
			filenames.push_back(filename);
		}
	}
	else if (-1 == result) {
		ret = false;
	}
	return ret;
}

void t4p::AppClass::CreateFeatures() {

	
	// connect the features to the event sink so that they can
	// receive app events
	FeatureFactory.CreateFeatures();
	for (size_t i = 0; i < FeatureFactory.Features.size(); ++i) {
		EventSink.PushHandler(FeatureFactory.Features[i]);
		RunningThreads.AddEventHandler(FeatureFactory.Features[i]);
		SqliteRunningThreads.AddEventHandler(FeatureFactory.Features[i]);
	}
}

void t4p::AppClass::FeatureWindows() {
	FeatureFactory.CreateViews();
	for (size_t i = 0; i < FeatureFactory.FeatureViews.size(); ++i) {
		EventSink.PushHandler(FeatureFactory.FeatureViews[i]);
		RunningThreads.AddEventHandler(FeatureFactory.FeatureViews[i]);
		SqliteRunningThreads.AddEventHandler(FeatureFactory.FeatureViews[i]);
	}
	for (size_t i = 0; i < FeatureFactory.FeatureViews.size(); ++i) {
		MainFrame->LoadFeatureView(*FeatureFactory.FeatureViews[i]);
	}
	MainFrame->RealizeToolbar();
}

void t4p::AppClass::DeleteFeatures() {
	for (size_t i = 0; i < FeatureFactory.Features.size(); ++i) {
		RunningThreads.RemoveEventHandler(FeatureFactory.Features[i]);
		SqliteRunningThreads.RemoveEventHandler(FeatureFactory.Features[i]);
	
		// disconnect from events so that events dont get sent after
		// features are destroyed
		EventSink.RemoveHandler(FeatureFactory.Features[i]);
	}
	FeatureFactory.DeleteFeatures();
}

void t4p::AppClass::DeleteFeatureViews() {
	for (size_t i = 0; i < FeatureFactory.FeatureViews.size(); ++i) {
		RunningThreads.RemoveEventHandler(FeatureFactory.FeatureViews[i]);
		SqliteRunningThreads.RemoveEventHandler(FeatureFactory.FeatureViews[i]);
		EventSink.RemoveHandler(FeatureFactory.FeatureViews[i]);
	}
	FeatureFactory.DeleteViews();
	MainFrame = NULL;
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
	for (size_t i = 0; i < FeatureFactory.Features.size(); ++i) {
		FeatureFactory.Features[i]->LoadPreferences(config);
	}
	for (size_t i = 0; i < FeatureFactory.FeatureViews.size(); ++i) {
		FeatureFactory.FeatureViews[i]->AddKeyboardShortcuts(Preferences.DefaultKeyboardShortcutCmds);
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

void t4p::AppClass::AddPreferencesWindows(wxBookCtrlBase* parent) {
	for (size_t i = 0; i < FeatureFactory.FeatureViews.size(); ++i) {
		FeatureFactory.FeatureViews[i]->AddPreferenceWindow(parent);
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
	EVT_MENU(t4p::MacCommonMenuBarClass::ID_COMMON_MENU_NEW, t4p::AppClass::OnCommonMenuFileNew)
	EVT_MENU(t4p::MacCommonMenuBarClass::ID_COMMON_MENU_OPEN, t4p::AppClass::OnCommonMenuFileOpen)
END_EVENT_TABLE()