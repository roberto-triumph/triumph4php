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

#include <main_frame/MainFrameClass.h>
#include <features/EnvironmentFeatureClass.h>
#include <features/FindInFilesFeatureClass.h>
#include <features/FinderFeatureClass.h>
#include <features/ProjectFeatureClass.h>
#include <features/OutlineViewFeatureClass.h>
#include <features/ResourceFeatureClass.h>
#include <features/RunConsoleFeatureClass.h>
#include <features/RunBrowserFeatureClass.h>
#include <features/LintFeatureClass.h>
#include <features/SqlBrowserFeatureClass.h>
#include <features/EditorMessagesFeatureClass.h>
#include <features/RecentFilesFeatureClass.h>
#include <globals/Errors.h>
#include <globals/Assets.h>

IMPLEMENT_APP(mvceditor::AppClass)

mvceditor::AppClass::AppClass()
	: wxApp()
	, Globals()
	, RunningThreads()
	, EventSink()
	, GlobalsChangeHandler(Globals)
	, Sequences(Globals, RunningThreads)
	, ConfigLastModified()
	, Features()
	, Preferences()
	, Timer(*this)
	, EditorMessagesFeature(NULL) 
	, IsAppReady(false) {
	MainFrame = NULL;
}

/**
 * when app starts, create the new app frame
 */
bool mvceditor::AppClass::OnInit() {
	Preferences.Init();
	RunningThreads.AddEventHandler(&GlobalsChangeHandler);
	CreateFeatures();

	// due to the way keyboard shortcuts are serialized, we need to load the
	// frame and initialize the feature windows so that all menus are created
	// and only then can we load the keyboard shortcuts from the INI file
	// all menu items must be present in the menu bar for shortcuts to take effect
	MainFrame = new mvceditor::MainFrameClass(Features, *this, Preferences);
	FeatureWindows();
	LoadPreferences();
	MainFrame->AuiManagerUpdate();
	if (CommandLine()) {
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
		wxLog::SetActiveTarget(new mvceditor::EditorMessagesLoggerClass(*EditorMessagesFeature));
		Timer.Start(1000, wxTIMER_CONTINUOUS);
		return true;
	}
	return false;
}

mvceditor::AppClass::~AppClass() {
	Timer.Stop();
	RunningThreads.RemoveEventHandler(&GlobalsChangeHandler);
	DeleteFeatures();
	
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
			MainFrame->FileOpen(filenames);
		}
	}
	else if (-1 == result) {
		ret = false;
	}
	return ret;
}

void mvceditor::AppClass::CreateFeatures() {
	FeatureClass* feature = new RunConsoleFeatureClass(*this);
	Features.push_back(feature);
	feature = new FinderFeatureClass(*this);
	Features.push_back(feature);
	feature = new FindInFilesFeatureClass(*this);
	Features.push_back(feature);
	feature = new ResourceFeatureClass(*this);
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

	EditorMessagesFeature = new mvceditor::EditorMessagesFeatureClass(*this);
	Features.push_back(EditorMessagesFeature);

	feature = new RunBrowserFeatureClass(*this);
	Features.push_back(feature);
	feature = new RecentFilesFeatureClass(*this);
	Features.push_back(feature);
	
	// TODO test feature need to find a quicker way to toggling it ON / OFF
	//feature = new TestFeatureClass(*this);
	//Features.push_back(feature);

	// connect the features to the event sink so that they can
	// receive app events
	for (size_t i = 0; i < Features.size(); ++i) {
		EventSink.PushHandler(Features[i]);
		RunningThreads.AddEventHandler(Features[i]);
	}
}

void mvceditor::AppClass::FeatureWindows() {
	for (size_t i = 0; i < Features.size(); ++i) {
		MainFrame->LoadFeature(Features[i]);
	}
}

void mvceditor::AppClass::DeleteFeatures() {
	for (size_t i = 0; i < Features.size(); ++i) {
		RunningThreads.RemoveEventHandler(Features[i]);
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

void mvceditor::AppClass::LoadPreferences() {

	// load any settings from .INI files
	PreferencesClass::InitConfig();
	wxConfigBase* config = wxConfigBase::Get();
	Globals.Environment.LoadFromConfig(config);
	for (size_t i = 0; i < Features.size(); ++i) {
		Features[i]->LoadPreferences(config);
		Features[i]->AddKeyboardShortcuts(Preferences.DefaultKeyboardShortcutCmds);
	}	
	Preferences.Load(config, MainFrame);
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
	
	// tell all features that the app is ready to use
	// the features will do / should do  their grunt
	// work in their event handler
	if (!App.IsAppReady) {
		App.IsAppReady = true;
		wxCommandEvent evt(mvceditor::EVENT_APP_READY);
		App.EventSink.Publish(evt);
		App.Sequences.AppStart();
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
				int res = wxMessageBox(msg, _("Reload preferences"), wxCENTRE | wxYES_NO, App.MainFrame);
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