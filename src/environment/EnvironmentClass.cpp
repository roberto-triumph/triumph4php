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
#include <environment/EnvironmentClass.h>
#include <wx/platinfo.h>
#include <wx/confbase.h>

mvceditor::EnvironmentClass::EnvironmentClass()
		: Apache()
		, Php()
		, WebBrowsers() {
	wxPlatformInfo info;
	switch (info.GetOperatingSystemId()) {
		case wxOS_UNIX_LINUX:
			WebBrowsers[wxT("Mozilla Firefox")] = wxFileName(wxT("/usr/bin/firefox"));
			WebBrowsers[wxT("Google Chrome")] = wxFileName(wxT("/usr/bin/google-chrome"));
			WebBrowsers[wxT("Opera")] = wxFileName(wxT("/usr/bin/opera"));
			break;
		case wxOS_WINDOWS_NT:
			WebBrowsers[wxT("Mozilla Firefox")] = wxFileName(wxT("C:\\Program Files\\Mozilla Firefox\\firefox.exe"));
			WebBrowsers[wxT("Google Chrome")] = wxFileName(wxT("C:\\Program Files\\Google\\chrome.exe"));
			WebBrowsers[wxT("Internet Explorer")] = wxFileName(wxT("C:\\Windows\\iexplore.exe"));
			WebBrowsers[wxT("Opera")] = wxFileName(wxT("C:\\Program Files\\Opera\\Opera.exe"));
			WebBrowsers[wxT("Safari")] = wxFileName(wxT("C:\\Program Files\\Safari\\Safari.exe"));
			break;
		default:
			break;
	}
}

mvceditor::EnvironmentClass::~EnvironmentClass() {
}

mvceditor::PhpEnvironmentClass::PhpEnvironmentClass() 
	: PhpExecutablePath(wxT("")) {
	wxPlatformInfo info;
	switch (info.GetOperatingSystemId()) {
		case wxOS_UNIX_LINUX:
			PhpExecutablePath = wxT("php");
			break;
		case wxOS_WINDOWS_NT:
			PhpExecutablePath = wxT("php-win.exe");
			break;
		default:
			break;
	}

}

void mvceditor::EnvironmentClass::LoadFromConfig() {
	wxConfigBase* config = wxConfigBase::Get();
	wxString httpdPath;
	config->Read(wxT("Environment/PhpExecutablePath"), &Php.PhpExecutablePath);
	config->Read(wxT("Environment/ApacheHttpdPath"), &httpdPath);
	if(!httpdPath.IsEmpty()) {
		Apache.SetHttpdPath(httpdPath);
	}
	long index;
	wxString groupName;
	wxString oldPath = config->GetPath();
	config->SetPath(wxT("Environment"));
	bool found = config->GetFirstGroup(groupName, index);
	if (found) {
		
		// only remove the defaults when the config file has something
		WebBrowsers.clear();
	}
	while (found) {
		if (groupName.Find(wxT("WebBrowser_")) >= 0) {
			wxString key = groupName + wxT("/Name");
			wxString browserName = config->Read(key);
			key = groupName + wxT("/Path");
			wxString browserPath = config->Read(key);
			wxFileName browserFileName(browserPath);
			WebBrowsers[browserName] = browserFileName;
		}
		found = config->GetNextGroup(groupName, index);
	}
	config->SetPath(oldPath);
}

void mvceditor::EnvironmentClass::SaveToConfig() const {
	wxConfigBase* config = wxConfigBase::Get();
	config->Write(wxT("Environment/PhpExecutablePath"), Php.PhpExecutablePath);
	config->Write(wxT("Environment/ApacheHttpdPath"), Apache.GetHttpdPath());
	int i = 0;
	for(std::map<wxString, wxFileName>::const_iterator it = WebBrowsers.begin(); it != WebBrowsers.end(); ++it) {
		wxString key = wxString::Format(wxT("Environment/WebBrowser_%d/Name"), i);
		config->Write(key, it->first);
		key = wxString::Format(wxT("Environment/WebBrowser_%d/Path"), i);
		config->Write(key, it->second.GetFullPath()); 
		i++;
	}
	config->Flush();
}
