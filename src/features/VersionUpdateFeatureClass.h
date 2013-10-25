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
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
#ifndef _MVCEDITOR_VERSIONUPDATEFEATURECLASS_H__
#define _MVCEDITOR_VERSIONUPDATEFEATURECLASS_H__

#include <features/FeatureClass.h>
#include <features/wxformbuilder/VersionUpdateFeatureForms.h>
#include <actions/ActionClass.h>

namespace mvceditor {

/**
 * This is the feature that check for new versions of MVC Editor
 * Version checking is done by posting to
 * http://updates.mvceditor.com/updates.php and reading the 
 * latest version from the response.
 */
class VersionUpdateFeatureClass : public FeatureClass {
public:

	VersionUpdateFeatureClass(mvceditor::AppClass& app);

	void AddHelpMenuItems(wxMenu* helpMenu);

	void AddPreferenceWindow(wxBookCtrlBase* parent);

	void LoadPreferences(wxConfigBase* config);
	
private:

	/**
	 * when the app starts, start the update check timer
	 */
	void OnAppReady(wxCommandEvent& event);

	/**
	 * when preferences are saved, start or stop the
	 * version check timer depending on user preferences
	 */	 
	void OnPreferencesSaved(wxCommandEvent& event);

	/**
	 * when preferences are updated, start or stop the
	 * version check timer depending on user preferences
	 */	 
	void OnPreferencesExternallyUpdated(wxCommandEvent& event);

	/**
	 * the actual code that will get executed when the "Check for updates" menu is selected
	 */
	void OnHelpCheckForUpdates(wxCommandEvent& event);

	/**
	 * when the timer is hit, we check to see if we need to do a version check
	 */
	void OnTimer(wxTimerEvent& event);

	/**
	 * read the response from the update server.  if there is a new version, 
	 * tell the user.
	 */
	void OnUpdateCheckComplete(wxCommandEvent& event);

	wxString GetCurrentVersion() const;

	/**
	 * the nexy time we are to check for a new version
	 */
	wxDateTime NextCheckTime;

	/**
	 * the timer is used to see if its time to do a version check
	 */
	wxTimer Timer;
	
	DECLARE_EVENT_TABLE()
};

class VersionUpdateDialogClass : public VersionUpdateGeneratedDialogClass {

public:

	VersionUpdateDialogClass(wxWindow* parent, int id, 
		mvceditor::RunningThreadsClass& runningThreads,
		const wxString& currentVersion, 
		bool showNewVersion,
		wxString newVersion);

protected:

	void OnOkButton(wxCommandEvent& event);

private:

	void OnTimer(wxTimerEvent& event);

	void OnUpdateCheckComplete(wxCommandEvent& event);

	void ConnectToUpdateServer();

	wxTimer Timer;

	mvceditor::RunningThreadsClass& RunningThreads;

	bool StartedCheck;

	DECLARE_EVENT_TABLE()

};

class VersionUpdatePreferencesPanelClass : public VersionUpdatePreferencesGeneratedPanelClass {

public:

	VersionUpdatePreferencesPanelClass(wxWindow* parent, mvceditor::PreferencesClass& preferences);

};

/**
 * This is the action that checks for new versions of MVC Editor
 * Version checking is done by posting to
 * http://updates.mvceditor.com/updates.php and reading the 
 * latest version from the response. Will post an event when the
 * response is returned from the server.
 */
class VersionUpdateActionClass : public mvceditor::ActionClass {

public:

	VersionUpdateActionClass(mvceditor::RunningThreadsClass& runningThreads, 
		int eventId, const wxString& currentVersion);

protected:

	void BackgroundWork();

	wxString GetNewVersion(const wxString& currentVersion, long& statusCode);

	wxString GetLabel() const;

private:

	wxString CurrentVersion;
};

}

#endif