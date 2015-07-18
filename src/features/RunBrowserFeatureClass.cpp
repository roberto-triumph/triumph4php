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
#include <features/RunBrowserFeatureClass.h>
#include <actions/UrlTagDetectorActionClass.h>
#include <globals/Errors.h>
#include <globals/Assets.h>
#include <globals/Number.h>
#include <widgets/ChooseUrlDialogClass.h>
#include <Triumph.h>
#include <wx/artprov.h>
#include <algorithm>

t4p::RunBrowserFeatureClass::RunBrowserFeatureClass(t4p::AppClass& app)
	: FeatureClass(app)
	, RecentUrls() {
}

void  t4p::RunBrowserFeatureClass::ExternalBrowser(const wxString& browserName, const wxURI& url) {
	wxFileName webBrowserPath;
	bool found = App.Globals.Environment.FindBrowserByName(browserName, webBrowserPath);
	if (!found || !webBrowserPath.IsOk()) {
		t4p::EditorLogWarning(t4p::ERR_BAD_WEB_BROWSER_EXECUTABLE, webBrowserPath.GetFullPath());
		return;
	}
	wxString cmd = wxT("\"") + webBrowserPath.GetFullPath() + wxT("\"");

	wxPlatformInfo info;
	if (info.GetOperatingSystemId() == wxOS_MAC_OSX_DARWIN && browserName.CmpNoCase(wxT("Safari")) == 0) {

		// safari on Mac does not handle command line URL arguments
		// need to use the "open" program
		// see http://superuser.com/questions/459268/run-safari-from-command-line-with-url-parameter
		cmd = wxT("open -a safari ");
	}
	cmd += wxT(" \"");
	cmd += url.BuildURI();
	cmd += wxT("\"");

	// dont track this PID, let the browser stay open if the editor is closed.
	// if we dont do pass the make group leader flag the browser thinks it crashed and will tell the user
	// that the browser crashed.
	long pid = wxExecute(cmd, wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER);
	if (pid <= 0) {
		t4p::EditorLogWarning(t4p::ERR_BAD_WEB_BROWSER_EXECUTABLE, cmd);
	}
}
