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
 * @author     $Author: robertop2004@gmail.com $
 * @date       $Date: 2011-06-10 17:19:31 -0700 (Fri, 10 Jun 2011) $
 * @version    $Rev: 555 $ 
 */
#include <plugins/WebBrowserPanelClass.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

int ID_WEB_BROWSER_GAUGE = wxNewId();
int ID_WEB_BROWSER_TIMER = wxNewId();
int ID_MENU_RUN_BROWSER = mvceditor::PluginClass::newMenuId();
	
mvceditor::WebBrowserPanelClass::WebBrowserPanelClass(wxWindow* parent, mvceditor::StatusBarWithGaugeClass* statusBarWithGauge)
		: WebBrowserPanelGeneratedClass(parent)
		, StatusBarWithGauge(statusBarWithGauge)
		, Timer(this, ID_WEB_BROWSER_TIMER) {

}

void mvceditor::WebBrowserPanelClass::OnGoButton(wxCommandEvent& event) {
	WebControl->OpenURI(Url->GetValue());
}

void mvceditor::WebBrowserPanelClass::SetUrl(const wxString& url) {
	Url->SetValue(url);
	WebControl->OpenURI(Url->GetValue());
	
	//kill any previous gauge
	StatusBarWithGauge->StopGauge(ID_WEB_BROWSER_GAUGE);
	if (Timer.Start(TIMER_INTERVAL, wxTIMER_CONTINUOUS)) {
		StatusBarWithGauge->AddGauge(_("Web Browser"), ID_WEB_BROWSER_GAUGE, StatusBarWithGaugeClass::INDETERMINATE_MODE, 0);
	}
}

void mvceditor::WebBrowserPanelClass::OnLocationChange(wxWebEvent& event) {
	Url->SetValue(event.GetString());

	// add a gauge when the URL is being loaded
	if (Timer.Start(TIMER_INTERVAL, wxTIMER_CONTINUOUS)) {	
		//kill any previous gauge
		StatusBarWithGauge->StopGauge(ID_WEB_BROWSER_GAUGE);
		StatusBarWithGauge->AddGauge(_("Web Browser"), ID_WEB_BROWSER_GAUGE, StatusBarWithGaugeClass::INDETERMINATE_MODE, 0);
	}
}

void mvceditor::WebBrowserPanelClass::OnDomContentLoaded(wxWebEvent& event) {
	Timer.Stop();
	StatusBarWithGauge->StopGauge(ID_WEB_BROWSER_GAUGE);
}

void mvceditor::WebBrowserPanelClass::OnTimer(wxTimerEvent& event) {
	StatusBarWithGauge->IncrementGauge(ID_WEB_BROWSER_GAUGE, StatusBarWithGaugeClass::INDETERMINATE_MODE);
}
void mvceditor::WebBrowserPanelClass::OnStatusText(wxWebEvent& evt) {
	wxString statusText = evt.GetString();
	if (statusText.IsEmpty()) {
		statusText = _("Ready");
	}
	StatusBarWithGauge->SetStatusText(statusText);
}

void mvceditor::WebBrowserPanelClass::OnStatusChange(wxWebEvent& evt) {
	StatusBarWithGauge->SetStatusText(evt.GetString());
}

void mvceditor::WebBrowserPanelClass::OnStateChange(wxWebEvent& evt) {

	// clear the status bar hear since OnStatusChange() doesn't
	// contain an empty string and we don't want "stuck" text in the statusbar
	int state = evt.GetState();
	wxString status_text = _("Ready");
	if ((state & wxWEB_STATE_STOP) && (state & wxWEB_STATE_IS_REQUEST)) {
		StatusBarWithGauge->SetStatusText(status_text);
	}
	if ((state & wxWEB_STATE_STOP) && (state & wxWEB_STATE_IS_REQUEST)) {
		StatusBarWithGauge->SetStatusText(status_text);
	}
}

mvceditor::WebBrowserPluginClass::WebBrowserPluginClass()
		: PluginClass()
		, XulRunnerInitialized(false)
		, XulRunnerGood(false) {
}

void mvceditor::WebBrowserPluginClass::AddToolsMenuItems(wxMenu* toolsMenu) {
	RunBrowserMenuItem = toolsMenu->Append(ID_MENU_RUN_BROWSER, _("Run In Web Browser\tF5"), _("Run Script In a Web Browser"));
}

void mvceditor::WebBrowserPluginClass::OnRunInBrowser(wxCommandEvent& event) {
	if (!XulRunnerInitialized) {
		XulRunnerGood = InitializeXulRunner();
		XulRunnerInitialized = true;
		if (!XulRunnerGood) {
			wxMessageBox(_("Error initializing xulrunner"));
		}
	}
	if (XulRunnerGood) {
		WebBrowserPanelClass* webBrowserPanel = new WebBrowserPanelClass(GetToolsParentWindow(), GetStatusBarWithGauge());
		if (AddToolsWindow(webBrowserPanel, _("Web Browser"))) {

			// open the selected file
			CodeControlClass* sourceCode = GetCurrentCodeControl();
			wxString url;
			if (sourceCode) {
				wxString fileName = sourceCode->GetFileName();
				url = GetEnvironment()->Apache.GetUrl(fileName);
			}
			webBrowserPanel->SetUrl(url);
		}
	}
}

bool mvceditor::WebBrowserPluginClass::InitializeXulRunner() {
	wxStandardPaths paths;
	wxFileName pathExecutableFileName(paths.GetExecutablePath());
	wxString xulrunnerPath = pathExecutableFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) +
		wxT("..") + wxFileName::GetPathSeparator() +
		wxT("lib") + wxFileName::GetPathSeparator() +
		wxT("xulrunner");
	wxFileName fileName(xulrunnerPath);
	fileName.Normalize();
	xulrunnerPath = fileName.GetFullPath();

	// Locate some common paths and initialize the control with the plugin paths; add these common plugin directories to
	// MOZ_PLUGIN_PATH
	wxString programFilesDir;
	wxGetEnv(wxT("ProgramFiles"), &programFilesDir);
	if (programFilesDir.Length() == 0 || programFilesDir.Last() != '\\') {
		programFilesDir += wxT("\\");
	}
	wxWebControl::AddPluginPath(programFilesDir + wxT("Mozilla Firefox\\plugins"));

	// Finally, initialize the engine; Calling wxWebControl::InitEngine()
	// is very important and has to be made before using wxWebControl.
	// It instructs wxWebConnect where the xulrunner directory is
	return wxWebControl::InitEngine(xulrunnerPath);
}

BEGIN_EVENT_TABLE(mvceditor::WebBrowserPanelClass, WebBrowserPanelGeneratedClass)
	EVT_WEB_DOMCONTENTLOADED(ID_WEB_BROWSER, mvceditor::WebBrowserPanelClass::OnDomContentLoaded)
	EVT_WEB_LOCATIONCHANGE(ID_WEB_BROWSER, mvceditor::WebBrowserPanelClass::OnLocationChange)
	EVT_WEB_STATUSTEXT(ID_WEB_BROWSER, mvceditor::WebBrowserPanelClass::OnStatusText)
    EVT_WEB_STATUSCHANGE(ID_WEB_BROWSER, mvceditor::WebBrowserPanelClass::OnStatusChange)
    EVT_WEB_STATECHANGE(ID_WEB_BROWSER, mvceditor::WebBrowserPanelClass::OnStateChange)
	EVT_TIMER(ID_WEB_BROWSER_TIMER, mvceditor::WebBrowserPanelClass::OnTimer)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mvceditor::WebBrowserPluginClass, wxEvtHandler)
	EVT_MENU(ID_MENU_RUN_BROWSER, mvceditor::WebBrowserPluginClass::OnRunInBrowser)
END_EVENT_TABLE()
