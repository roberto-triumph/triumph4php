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
#ifndef T4P_VERSIONUPDATEFEATURECLASS_H
#define T4P_VERSIONUPDATEFEATURECLASS_H

#include <features/FeatureClass.h>
#include <actions/ActionClass.h>

namespace t4p {

/**
 * This is the feature that check for new versions of Triumph
 * Version checking is done by posting to
 * http://updates.triumph4php.com/updates.php and reading the
 * latest version from the response.
 */
class VersionUpdateFeatureClass : public FeatureClass {
	public:

	VersionUpdateFeatureClass(t4p::AppClass& app);

	void LoadPreferences(wxConfigBase* config);

	wxString GetCurrentVersion() const;

	/**
	 * the next time we are to check for a new version
	 */
	wxDateTime NextCheckTime;

	private:


	void OnAppReady(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};


/**
 * This is the action that checks for new versions of Triumph
 * Version checking is done by posting to
 * http://updates.triumph4php.com/updates.php and reading the
 * latest version from the response. Will post an event when the
 * response is returned from the server.
 */
class VersionUpdateActionClass : public t4p::ActionClass {

	public:

	VersionUpdateActionClass(t4p::RunningThreadsClass& runningThreads,
		int eventId, const wxString& currentVersion);

	protected:

	void BackgroundWork();

	wxString GetNewVersion(const wxString& currentVersion, long& statusCode);

	wxString GetLabel() const;

	private:

	wxString CurrentVersion;

	void OnAppReady(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

}

#endif
