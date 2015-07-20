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
#include <globals/EnvironmentClass.h>
#include <wx/platinfo.h>
#include <wx/confbase.h>
#include <wx/utils.h>

t4p::WebBrowserClass::WebBrowserClass()
	: Name()
	, FullPath() {
}

t4p::WebBrowserClass::WebBrowserClass(const t4p::WebBrowserClass& src)
	: Name()
	, FullPath() {
	Copy(src);
}

t4p::WebBrowserClass::WebBrowserClass(wxString name, wxFileName fullPath)
	: Name(name.c_str())
	, FullPath(fullPath) {
}

t4p::WebBrowserClass& t4p::WebBrowserClass::operator=(const t4p::WebBrowserClass& src) {
	Copy(src);
	return *this;
}

void t4p::WebBrowserClass::Copy(const t4p::WebBrowserClass& src) {
	// using c_str() to completely clone a wxString, as by default
	// the assignment operator is a shallow copy
	// this makes the copies thread-safe
	Name = src.Name.c_str();
	FullPath = src.FullPath;
}

t4p::PhpEnvironmentClass::PhpEnvironmentClass()
	: PhpExecutablePath(wxT(""))
	, Version(pelet::PHP_53)
	, IsAuto(true)
	, Installed(false) {
}

t4p::PhpEnvironmentClass::PhpEnvironmentClass(const t4p::PhpEnvironmentClass& src)
	: PhpExecutablePath()
	, Version(pelet::PHP_53)
	, IsAuto(true)
	, Installed(false) {
	Copy(src);
}

t4p::PhpEnvironmentClass& t4p::PhpEnvironmentClass::operator=(const t4p::PhpEnvironmentClass& src) {
	Copy(src);
	return *this;
}

void t4p::PhpEnvironmentClass::Init() {
	wxPlatformInfo info;
	switch (info.GetOperatingSystemId()) {
		case wxOS_UNIX_LINUX:
			PhpExecutablePath = wxT("php");
			break;
		case wxOS_WINDOWS_NT:
			PhpExecutablePath = wxT("php-win.exe");
			break;
		case wxOS_MAC_OSX_DARWIN:
			PhpExecutablePath = wxT("php");
			break;
		default:
			break;
	}
}

void t4p::PhpEnvironmentClass::Copy(const t4p::PhpEnvironmentClass &src) {
	// using c_str() to completely clone a wxString, as by default
	// the assignment operator is a shallow copy
	// this makes the copies thread-safe
	PhpExecutablePath = src.PhpExecutablePath.c_str();
	Version = src.Version;
	IsAuto = src.IsAuto;
	Installed = src.Installed;
}

void t4p::PhpEnvironmentClass::AutoDetermine() {
	if (!IsAuto) {
		// stick with the configured version
		return;
	}
	if (NotInstalled()) {
		// default to the first version
		Version = pelet::PHP_53;
		return;
	}
	wxString cmd = PhpExecutablePath + wxT(" -v");
	wxArrayString output;
	wxExecute(cmd, output);
	bool is53 = false;
	bool is54 = false;
	for (size_t i = 0; i < output.GetCount(); ++i) {
		if (output[i].Find(wxT("PHP 5.3")) >= 0) {
			is53 = true;
			break;
		}
		if (output[i].Find(wxT("PHP 5.4")) >= 0) {
			is54 = true;
			break;
		}
	}
	if (is54) {
		Version = pelet::PHP_54;
	}
	if (is53) {
		Version = pelet::PHP_53;
	} else {
		// if version string changes
		Version = pelet::PHP_53;
	}
}

bool t4p::PhpEnvironmentClass::NotInstalled() const {
	return !Installed || PhpExecutablePath.IsEmpty();
}

t4p::EnvironmentClass::EnvironmentClass()
	: Apache()
	, Php()
	, WebBrowsers() {
}

void t4p::EnvironmentClass::Init() {
	Php.Init();
	AddDefaults();
}

t4p::EnvironmentClass::EnvironmentClass(const t4p::EnvironmentClass& src)
	: Apache()
	, Php()
	, WebBrowsers() {
	Copy(src);
}

t4p::EnvironmentClass::~EnvironmentClass() {
}

t4p::EnvironmentClass& t4p::EnvironmentClass::operator=(const t4p::EnvironmentClass &src) {
	Copy(src);
	return *this;
}

void t4p::EnvironmentClass::Copy(const t4p::EnvironmentClass& src) {
	Apache = src.Apache;
	Php = src.Php;
	WebBrowsers = src.WebBrowsers;
}

void t4p::EnvironmentClass::AddDefaults() {
	wxPlatformInfo info;
	wxString userHome = wxGetUserHome();

	// these are the default installation locations
	switch (info.GetOperatingSystemId()) {
		case wxOS_UNIX_LINUX:
			WebBrowsers.push_back(t4p::WebBrowserClass(wxT("Mozilla Firefox"), wxFileName(wxT("/usr/bin/firefox"))));
			WebBrowsers.push_back(t4p::WebBrowserClass(wxT("Google Chrome"), wxFileName(wxT("/usr/bin/google-chrome"))));
			WebBrowsers.push_back(t4p::WebBrowserClass(wxT("Opera"), wxFileName(wxT("/usr/bin/opera"))));
			break;
		case wxOS_WINDOWS_NT:
			WebBrowsers.push_back(t4p::WebBrowserClass(wxT("Mozilla Firefox"), wxFileName(wxT("C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe"))));

			// by default google installs itself in the user home
			WebBrowsers.push_back(t4p::WebBrowserClass(wxT("Google Chrome"), wxFileName(userHome + wxT("\\AppData\\Local\\Google\\Chrome\\Application\\chrome.exe"))));
			WebBrowsers.push_back(t4p::WebBrowserClass(wxT("Internet Explorer"), wxFileName(wxT("C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe"))));
			WebBrowsers.push_back(t4p::WebBrowserClass(wxT("Opera"), wxFileName(wxT("C:\\Program Files (x86)\\Opera\\opera.exe"))));
			WebBrowsers.push_back(t4p::WebBrowserClass(wxT("Safari"), wxFileName(wxT("C:\\Program Files (x86)\\Safari\\Safari.exe"))));
			break;
		case wxOS_MAC_OSX_DARWIN:
			WebBrowsers.push_back(t4p::WebBrowserClass(wxT("Safari"), wxFileName(wxT("/Applications/Safari.app/Contents/MacOS/Safari"))));
			WebBrowsers.push_back(t4p::WebBrowserClass(wxT("Mozilla Firefox"), wxFileName(wxT("/Applications/Firefox.app/Contents/MacOS/firefox"))));
			WebBrowsers.push_back(t4p::WebBrowserClass(wxT("Google Chrome"), wxFileName(wxT("/Applications/Google Chrome.app/Contents/MacOS/Google Chrome"))));
			WebBrowsers.push_back(t4p::WebBrowserClass(wxT("Opera"), wxFileName(wxT("/Applications/Opera.app/Contents/MacOS/Opera"))));
			break;
		default:
			break;
	}
}

void t4p::EnvironmentClass::LoadFromConfig(wxConfigBase* config) {
	int version = 0;
	config->Read(wxT("Environment/PhpExecutablePath"), &Php.PhpExecutablePath);
	config->Read(wxT("Environment/PhpVersionIsAuto"), &Php.IsAuto);
	config->Read(wxT("Environment/PhpVersion"), &version);
	config->Read(wxT("Environment/PhpInstalled"), &Php.Installed);

	if (1 == version) {
		Php.Version = pelet::PHP_53;
	} else if (2 == version) {
		Php.Version = pelet::PHP_54;
	}

	if (Php.IsAuto) {
		Php.AutoDetermine();
	}

	wxString httpdPath;
	config->Read(wxT("Environment/ApacheHttpdPath"), &httpdPath);
	config->Read(wxT("Environment/ManualConfiguration"), &Apache.ManualConfiguration);
	if(!httpdPath.IsEmpty()) {
		Apache.SetHttpdPath(httpdPath);
	}
	wxString oldPath = config->GetPath();
	long index;
	wxString groupName;
	config->SetPath(wxT("Environment"));
	bool found = config->GetFirstGroup(groupName, index);
	if (found) {
		// only remove the defaults when the config file has something
		WebBrowsers.clear();
	}
	while (found) {
		// the web browsers; there are many groups; each group is named "WebBrowser_#"
		if (groupName.Find(wxT("WebBrowser_")) >= 0) {
			wxString key = groupName + wxT("/Name");
			wxString browserName = config->Read(key);
			key = groupName + wxT("/Path");
			wxString browserPath = config->Read(key);
			wxFileName browserFileName(browserPath);
			WebBrowsers.push_back(t4p::WebBrowserClass(browserName, browserFileName));
		} else if (groupName.Find(wxT("VirtualHost_")) >= 0 && Apache.ManualConfiguration) {
			// the manual virtual host entries there are many groups; each group is named "VirtualHost_#"
			// only fill in when Manual flag is off (otherwise we want to parse from the file
			// in case settings have changed)
			wxString key = groupName + wxT("/RootDirectory");
			wxString rootDirectory = config->Read(key);
			key = groupName + wxT("/HostName");
			wxString hostName = config->Read(key);
			Apache.SetVirtualHostMapping(rootDirectory, hostName);
		}
		found = config->GetNextGroup(groupName, index);
	}
	config->SetPath(oldPath);
}

void t4p::EnvironmentClass::SaveToConfig(wxConfigBase* config) const {
	int version = 0;
	if (pelet::PHP_53 == Php.Version) {
		version = 1;
	} else if (pelet::PHP_54) {
		version = 2;
	}
	config->Write(wxT("Environment/PhpExecutablePath"), Php.PhpExecutablePath);
	config->Write(wxT("Environment/PhpVersionIsAuto"), Php.IsAuto);
	config->Write(wxT("Environment/PhpVersion"), version);
	config->Write(wxT("Environment/PhpInstalled"), Php.Installed);

	config->Write(wxT("Environment/ApacheHttpdPath"), Apache.GetHttpdPath());
	config->Write(wxT("Environment/ManualConfiguration"), Apache.ManualConfiguration);
	int i = 0;
	for(std::vector<t4p::WebBrowserClass>::const_iterator it = WebBrowsers.begin(); it != WebBrowsers.end(); ++it) {
		wxString key = wxString::Format(wxT("Environment/WebBrowser_%d/Name"), i);
		config->Write(key, it->Name);
		key = wxString::Format(wxT("Environment/WebBrowser_%d/Path"), i);
		config->Write(key, it->FullPath.GetFullPath());
		i++;
	}
	if (Apache.ManualConfiguration) {
		std::map<wxString, wxString> mappings = Apache.GetVirtualHostMappings();
		i = 0;
		for (std::map<wxString, wxString>::const_iterator it = mappings.begin(); it != mappings.end(); ++it) {
			wxString key = wxString::Format(wxT("Environment/VirtualHost_%d/RootDirectory"), i);
			config->Write(key, it->first);
			key = wxString::Format(wxT("Environment/VirtualHost_%d/HostName"), i);
			config->Write(key, it->second);
			i++;
		}
	}
}

std::vector<wxString> t4p::EnvironmentClass::BrowserNames() const {
	std::vector<wxString> names;
	std::vector<t4p::WebBrowserClass>::const_iterator browser;
	for (browser = WebBrowsers.begin(); browser != WebBrowsers.end(); ++browser) {
		names.push_back(browser->Name);
	}
	return names;
}

bool t4p::EnvironmentClass::FindBrowserByName(const wxString& name, wxFileName& fileName) const {
	bool found = false;
	for(std::vector<t4p::WebBrowserClass>::const_iterator it = WebBrowsers.begin(); it != WebBrowsers.end(); ++it) {
		if (it->Name == name) {
			found = true;
			fileName = it->FullPath;
			break;
		}
	}
	return found;
}
