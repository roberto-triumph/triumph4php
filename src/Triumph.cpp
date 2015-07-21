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

// include boost asio before any other file to prevent  //
// fatal error C1189: #error :  WinSock.h has already been included  //
// in MSW
#include "Triumph.h"
#include <boost/asio.hpp>
#include <soci/mysql/soci-mysql.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <unicode/uclean.h>
#include <wx/cmdline.h>
#include <wx/fileconf.h>
#include "features/FeatureClass.h"
#include "globals/Assets.h"
#include "globals/Errors.h"
#include "main_frame/MacCommonMenuBarClass.h"
#include "main_frame/MainFrameClass.h"
#include "views/FeatureViewClass.h"

t4p::AppClass::AppClass(wxTimer& configModifiedTimer)
	: Globals()
	, RunningThreads()
	, SqliteRunningThreads()
	, EventSink()
	, GlobalsChangeHandler(Globals)
	, Sequences(Globals, SqliteRunningThreads)
	, Preferences()
	, ConfigLastModified()
	, FeatureFactory(*this)
	, Timer(configModifiedTimer) {
}

/**
 * when app starts, create the new app frame
 */
void t4p::AppClass::Init() {
	// 1 ==> to make sure any queued items are done one at a time
	SqliteRunningThreads.SetMaxThreads(1);
	Globals.Environment.Init();
	Preferences.Init();
	SqliteRunningThreads.AddEventHandler(&GlobalsChangeHandler);
	CreateFeatures();
}

t4p::AppClass::~AppClass() {
	RunningThreads.RemoveEventHandler(&GlobalsChangeHandler);
	RunningThreads.Shutdown();
	SqliteRunningThreads.Shutdown();
	DeleteFeatures();
	DeleteFeatureViews();
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

void t4p::AppClass::AddPreferencesWindows(wxBookCtrlBase* parent) {
	for (size_t i = 0; i < FeatureFactory.FeatureViews.size(); ++i) {
		FeatureFactory.FeatureViews[i]->AddPreferenceWindow(parent);
	}
}

void t4p::AppClass::UpdateConfigModifiedTime() {
	wxFileName configFileName(t4p::ConfigDirAsset().GetPath(), wxT("triumph4php.ini"));
	if (configFileName.FileExists()) {
		ConfigLastModified = configFileName.GetModificationTime();
	}
	Timer.Start();
}

bool t4p::AppClass::IsActive() {
	return wxTheApp? wxTheApp->IsActive() : false;
}
