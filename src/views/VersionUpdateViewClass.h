/**
 * @copyright  2013 Roberto Perpuly
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
#ifndef SRC_VIEWS_VERSIONUPDATEVIEWCLASS_H_
#define SRC_VIEWS_VERSIONUPDATEVIEWCLASS_H_

#include "actions/ActionClass.h"
#include "features/VersionUpdateFeatureClass.h"
#include "views/FeatureViewClass.h"
#include "views/wxformbuilder/VersionUpdateFeatureForms.h"

namespace t4p {
/**
 * This is the feature that check for new versions of Triumph
 * Version checking is done by posting to
 * http://updates.triumph4php.com/updates.php and reading the
 * latest version from the response.
 */
class VersionUpdateViewClass : public FeatureViewClass {
	public:
	VersionUpdateViewClass(t4p::VersionUpdateFeatureClass& feature);

	void AddHelpMenuItems(wxMenu* helpMenu);

	void AddPreferenceWindow(wxBookCtrlBase* parent);

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

	/**
	 * contains application logic
	 */
	t4p::VersionUpdateFeatureClass& Feature;

	/**
	 * the timer is used to see if its time to do a version check
	 */
	wxTimer Timer;

	DECLARE_EVENT_TABLE()
};

class VersionUpdateDialogClass : public VersionUpdateGeneratedDialogClass {
	public:
	VersionUpdateDialogClass(wxWindow* parent, int id,
		t4p::RunningThreadsClass& runningThreads,
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

	t4p::RunningThreadsClass& RunningThreads;

	bool StartedCheck;

	DECLARE_EVENT_TABLE()
};

class VersionUpdatePreferencesPanelClass : public VersionUpdatePreferencesGeneratedPanelClass {
	public:
	VersionUpdatePreferencesPanelClass(wxWindow* parent, t4p::PreferencesClass& preferences);
};
}  // namespace t4p

#endif  // SRC_VIEWS_VERSIONUPDATEVIEWCLASS_H_
