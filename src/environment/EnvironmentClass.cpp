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
#include <wx/utils.h>

mvceditor::WebBrowserClass::WebBrowserClass() 
	: Name()
	, FullPath() {

}


mvceditor::WebBrowserClass::WebBrowserClass(const mvceditor::WebBrowserClass& other) 
	: Name(other.Name)
	, FullPath(other.FullPath) {

}

mvceditor::WebBrowserClass::WebBrowserClass(wxString name, wxFileName fullPath) 
	: Name(name)
	, FullPath(fullPath) {

}

mvceditor::EnvironmentClass::EnvironmentClass()
		: Apache()
		, Php()
		, WebBrowsers() {
	wxPlatformInfo info;
	wxString userHome = wxGetUserHome();

	// these are the default installation locations
	switch (info.GetOperatingSystemId()) {
		case wxOS_UNIX_LINUX:
			WebBrowsers.push_back(mvceditor::WebBrowserClass(wxT("Mozilla Firefox"), wxFileName(wxT("/usr/bin/firefox"))));
			WebBrowsers.push_back(mvceditor::WebBrowserClass(wxT("Google Chrome"), wxFileName(wxT("/usr/bin/google-chrome"))));
			WebBrowsers.push_back(mvceditor::WebBrowserClass(wxT("Opera"), wxFileName(wxT("/usr/bin/opera"))));
			break;
		case wxOS_WINDOWS_NT:
			WebBrowsers.push_back(mvceditor::WebBrowserClass(wxT("Mozilla Firefox"), wxFileName(wxT("C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe"))));

			// by default google installs itself in the user home
			WebBrowsers.push_back(mvceditor::WebBrowserClass(wxT("Google Chrome"), wxFileName(userHome + wxT("\\AppData\\Local\\Google\\Chrome\\Application\\chrome.exe"))));
			WebBrowsers.push_back(mvceditor::WebBrowserClass(wxT("Internet Explorer"), wxFileName(wxT("C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe"))));
			WebBrowsers.push_back(mvceditor::WebBrowserClass(wxT("Opera"), wxFileName(wxT("C:\\Program Files (x86)\\Opera\\opera.exe"))));
			WebBrowsers.push_back(mvceditor::WebBrowserClass(wxT("Safari"), wxFileName(wxT("C:\\Program Files (x86)\\Safari\\Safari.exe"))));
			break;
		default:
			break;
	}
}

mvceditor::EnvironmentClass::~EnvironmentClass() {
}

mvceditor::PhpEnvironmentClass::PhpEnvironmentClass() 
	: PhpExecutablePath(wxT(""))
	, Version(pelet::PHP_53) 
	, IsAuto(true) {
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

void mvceditor::PhpEnvironmentClass::AutoDetermine() {
	if (PhpExecutablePath.IsEmpty()) {
		Version = pelet::PHP_53;
		return;
	}
	if (!IsAuto) {
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
	}
	else {
		
		// if version string changes
		Version = pelet::PHP_53;
	}
}

void mvceditor::EnvironmentClass::LoadFromConfig(wxConfigBase* config) {
	
	int version = 0;
	config->Read(wxT("Environment/PhpExecutablePath"), &Php.PhpExecutablePath);
	config->Read(wxT("Environment/PhpVersionIsAuto"), &Php.IsAuto);
	config->Read(wxT("Environment/PhpVersion"), &version);
	
	if (1 == version) {
		Php.Version = pelet::PHP_53;
	}
	else if (2 == version) {
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
			WebBrowsers.push_back(mvceditor::WebBrowserClass(browserName, browserFileName));
		}
		else if (groupName.Find(wxT("VirtualHost_")) >= 0 && Apache.ManualConfiguration) {
			
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

void mvceditor::EnvironmentClass::SaveToConfig(wxConfigBase* config) const {
	int version = 0;
	if (pelet::PHP_53 == Php.Version) {
		version = 1;
	}
	else if (pelet::PHP_54) {
		version = 2;
	}
	config->Write(wxT("Environment/PhpExecutablePath"), Php.PhpExecutablePath);
	config->Write(wxT("Environment/PhpVersionIsAuto"), Php.IsAuto);
	config->Write(wxT("Environment/PhpVersion"), version);
	
	
	config->Write(wxT("Environment/ApacheHttpdPath"), Apache.GetHttpdPath());
	config->Write(wxT("Environment/ManualConfiguration"), Apache.ManualConfiguration);
	int i = 0;
	for(std::vector<mvceditor::WebBrowserClass>::const_iterator it = WebBrowsers.begin(); it != WebBrowsers.end(); ++it) {
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

bool mvceditor::EnvironmentClass::FindBrowserByName(const wxString& name, wxFileName& fileName) const {
	bool found = false;
	for(std::vector<mvceditor::WebBrowserClass>::const_iterator it = WebBrowsers.begin(); it != WebBrowsers.end(); ++it) {
		if (it->Name == name) {
			found = true;
			fileName = it->FullPath;
			break;
		}
	}
	return found;
}