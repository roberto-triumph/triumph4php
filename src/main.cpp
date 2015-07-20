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

static int ID_EVENT_CONFIG_FILE_CHECK = wxNewId();

// forward declaration, defined below
class GuiAppClass;

namespace t4p {
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
	AppTimerClass(GuiAppClass& guiApp);

	/**
	 * when the timer ends, generate an EVENT_APP_READY event
	 */
	void Notify();

	private:
	GuiAppClass& GuiApp;

	void OnConfigFileModified(t4p::FilesModifiedEventClass& event);

	DECLARE_EVENT_TABLE()
};
}  // namespace t4p

/**
 * The GuiApp is the class that represents to GUI App.
 * It inherits from wxWidgets' wxApp
 *
 * This class should not really perform any logic related to
 * the editor whatsoever, its only purpose is to
 * create the main frame, create the features and let them
 * "do their thing".
 */
class GuiAppClass : public wxApp {
	public:
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
	 * The App contains the non-UI data structures that
	 * need to be shared by all features
	 */
	t4p::AppClass App;

	GuiAppClass();
	~GuiAppClass();

	/**
	 * Initialize the application
	 */
	virtual bool OnInit();

	/**
	 * creates the application main frame
	 */
	t4p::MainFrameClass* CreateFrame();

	/**
	 * asks features for any windows they want to create
	 */
	void FeatureWindows(t4p::MainFrameClass* mainFrame);

	private:
	void LoadPreferences(t4p::MainFrameClass* mainFrame);

	/**
	 * Parses any command line arguments.
	 * @param [out] filenames the files given as arguments
	 *        to the app.
	 * @return false if arguments are invalid.
	 */
	bool CommandLine(std::vector<wxString>& filenames);
	/**
	 * when the app is re-activated, tell the features about it
	 */
	void OnActivateApp(wxActivateEvent& event);

	/**
	 * handlers for the "common" menu; the menu that is shown when
	 * the user has closed the main frame but the app is still running.
	 */
	void OnCommonMenuFileOpen(wxCommandEvent& event);
	void OnCommonMenuFileNew(wxCommandEvent& event);

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
	 * mac-specific functionality when main frame has been closed but
	 * app is still running.
	 */
	t4p::MacCommonMenuBarClass* MacCommonMenuBar;

	/**
	 * TRUE if the EVENT_APP_READY has already been generated.
	 */
	bool IsAppReady;

	friend class t4p::AppTimerClass;

	DECLARE_EVENT_TABLE()
};

IMPLEMENT_APP(GuiAppClass)

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
}  // namespace t4p

GuiAppClass::GuiAppClass()
	: wxApp()
	, Timer(*this)
	, App(Timer)
	, MacCommonMenuBar(NULL) {
}

/**
 * when app starts, create the new app frame
 */
bool GuiAppClass::OnInit() {
	App.Init();
	MacCommonMenuBar = new t4p::MacCommonMenuBarClass(*this);
	App.RunningThreads.SetThreadCleanup(new t4p::MysqlThreadCleanupClass);

	// not really needed since we will use this running threads for sqlite actions only,
	// but just in case
	App.SqliteRunningThreads.SetThreadCleanup(new t4p::MysqlThreadCleanupClass);


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
			for (size_t i = 0; i < filenames.size(); i++) {
				wxCommandEvent evt(t4p::EVENT_CMD_FILE_OPEN);
				evt.SetString(filenames[i]);
				App.EventSink.Post(evt);
			}
		}
		Timer.Start(1000, wxTIMER_CONTINUOUS);
		return true;
	}
	return false;
}

t4p::MainFrameClass* GuiAppClass::CreateFrame() {
	// due to the way keyboard shortcuts are serialized, we need to load the
	// frame and initialize the feature windows so that all menus are created
	// and only then can we load the keyboard shortcuts from the INI file
	// all menu items must be present in the menu bar for shortcuts to take effect
	t4p::MainFrameClass* frame = new t4p::MainFrameClass(
		App.FeatureFactory.FeatureViews, App, *this, Timer
	);
	FeatureWindows(frame);
	LoadPreferences(frame);
	SetTopWindow(frame);
	frame->Show(true);

	// maximize only after showing, that way the size event gets propagated and
	// the main frame is drawn correctly at app start.
	// if we don't do this, there is a nasty effect on windows OS that shows
	// the status bar in the middle of the page until the user re-maximizes the app
	frame->Maximize();
	return frame;
}

void GuiAppClass::OnCommonMenuFileNew(wxCommandEvent& event) {
	t4p::MainFrameClass* frame = CreateFrame();

	wxCommandEvent appReady(t4p::EVENT_APP_READY);
	for (size_t i = 0; i < App.FeatureFactory.FeatureViews.size(); ++i) {
		App.FeatureFactory.FeatureViews[i]->ProcessEvent(appReady);
	}

	frame->CreateNewCodeCtrl();
}

void GuiAppClass::OnCommonMenuFileOpen(wxCommandEvent& event) {
	wxFileDialog dialog(
		NULL,
		_("Select file to open"),
		"", "", "", wxFD_OPEN | wxFD_FILE_MUST_EXIST
	);
	if (wxOK == dialog.ShowModal()) {
		CreateFrame();

		wxCommandEvent appReady(t4p::EVENT_APP_READY);
		for (size_t i = 0; i < App.FeatureFactory.FeatureViews.size(); ++i) {
			App.FeatureFactory.FeatureViews[i]->ProcessEvent(appReady);
		}

		wxString name = dialog.GetPath();
		wxCommandEvent evt(t4p::EVENT_CMD_FILE_OPEN);
		evt.SetString(name);
		App.EventSink.Publish(evt);
	}
}

GuiAppClass::~GuiAppClass() {
	Timer.Stop();
	delete MacCommonMenuBar;

	// must close all soci sessions before shutting down sqlite library
	App.Globals.Close();

	// calling cleanup here so that we can run this binary through a memory leak detector
	// ICU will cache many things and that will cause the detector to output "possible leaks"
	// TODO(roberto): only use this during debug mode
	u_cleanup();
	mysql_library_end();
	sqlite_api::sqlite3_shutdown();
}


bool GuiAppClass::CommandLine(std::vector<wxString>& filenames) {
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
	} else if (-1 == result) {
		ret = false;
	}
	return ret;
}


void GuiAppClass::FeatureWindows(t4p::MainFrameClass* mainFrame) {
	App.FeatureWindows();
	for (size_t i = 0; i < App.FeatureFactory.FeatureViews.size(); ++i) {
		mainFrame->LoadFeatureView(*App.FeatureFactory.FeatureViews[i]);
	}
	mainFrame->RealizeToolbar();
}

void GuiAppClass::LoadPreferences(t4p::MainFrameClass* mainFrame) {
	App.LoadPreferences();

	// pointer created by App.LoadPreferences
	wxConfigBase* config = wxConfigBase::Get();
	App.Preferences.Load(config, mainFrame);
}

void GuiAppClass::StopConfigModifiedCheck() {
	Timer.Stop();
}

void GuiAppClass::OnActivateApp(wxActivateEvent& event) {
	if (IsActive()) {
		wxCommandEvent cmdEvent(t4p::EVENT_APP_ACTIVATED);
		App.EventSink.Publish(cmdEvent);
	}
}

t4p::AppTimerClass::AppTimerClass(GuiAppClass& guiApp)
	: wxTimer()
	, GuiApp(guiApp) {
	SetOwner(this, ID_EVENT_CONFIG_FILE_CHECK);
}

void t4p::AppTimerClass::Notify() {
	// tell all features that the app is ready to use
	// the features will do / should do  their grunt
	// work in their event handler
	if (!GuiApp.IsAppReady) {
		GuiApp.IsAppReady = true;
		wxFileName settingsDir = t4p::SettingsDirAsset();
		wxCommandEvent evt(t4p::EVENT_APP_READY);
		GuiApp.App.EventSink.Publish(evt);

		if (settingsDir.IsOk()) {
			GuiApp.App.Sequences.AppStart();
		}
		wxFileName configFileName(t4p::ConfigDirAsset().GetPath(), wxT("triumph4php.ini"));
		if (configFileName.FileExists()) {
			GuiApp.App.ConfigLastModified = configFileName.GetModificationTime();
		}
	} else {
		wxFileName configFileName(t4p::ConfigDirAsset().GetPath(), wxT("triumph4php.ini"));
		if (configFileName.FileExists()) {
			t4p::FileModifiedCheckActionClass* action =
				new t4p::FileModifiedCheckActionClass(GuiApp.App.RunningThreads, ID_EVENT_CONFIG_FILE_CHECK);
			std::vector<t4p::FileModifiedTimeClass> fileMods;
			t4p::FileModifiedTimeClass fileMod;
			fileMod.FileName = configFileName;
			fileMod.ModifiedTime = GuiApp.App.ConfigLastModified;
			fileMods.push_back(fileMod);
			action->SetFiles(fileMods);

			GuiApp.App.RunningThreads.Queue(action);
		}
	}
}

void t4p::AppTimerClass::OnConfigFileModified(t4p::FilesModifiedEventClass& event) {
	// stop the timer so that we dont show this prompt multiple times
	GuiApp.Timer.Stop();
	if (event.Modified.empty()) {
		return;
	}
	wxFileName configFileName = event.Modified[0];
	wxString msg = wxString::FromAscii(
		"Preferences have been modified externally.\n"
		"Reload the preferences?"
	);
	msg = wxGetTranslation(msg);
	int res = wxMessageBox(msg, _("Reload preferences"), wxCENTRE | wxYES_NO);
	if (wxYES == res) {
		// delete the old config ourselves
		wxConfigBase* oldConfig = wxConfig::Get();
		delete oldConfig;
		wxConfig::Set(NULL);
		GuiApp.App.Preferences.ClearAllShortcuts();

		wxWindow* top = GuiApp.GetTopWindow();
		t4p::MainFrameClass* frame = NULL;
		if (top) {
			frame = (t4p::MainFrameClass*)top;
		}
		GuiApp.LoadPreferences(frame);

		GuiApp.App.ConfigLastModified = event.ModifiedTimes[0];
	} else {
		// update the time so that we don't continually ask the user the prompt
		GuiApp.App.ConfigLastModified = event.ModifiedTimes[0];
	}
	GuiApp.Timer.Start(1000, wxTIMER_CONTINUOUS);
}

BEGIN_EVENT_TABLE(t4p::AppTimerClass, wxTimer)
	EVT_FILES_EXTERNALLY_MODIFIED_COMPLETE(ID_EVENT_CONFIG_FILE_CHECK, t4p::AppTimerClass::OnConfigFileModified)
END_EVENT_TABLE()


BEGIN_EVENT_TABLE(GuiAppClass, wxApp)
	EVT_ACTIVATE_APP(GuiAppClass::OnActivateApp)
	EVT_MENU(t4p::MacCommonMenuBarClass::ID_COMMON_MENU_NEW, GuiAppClass::OnCommonMenuFileNew)
	EVT_MENU(t4p::MacCommonMenuBarClass::ID_COMMON_MENU_OPEN, GuiAppClass::OnCommonMenuFileOpen)
END_EVENT_TABLE()
