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
#include <environment/ApacheClass.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/textfile.h>
#include <wx/utils.h>

/**
 FROM THE APACHE WEBSITE (http://httpd.apache.org/docs/2.0/mod/core.html#include)

Include Directive
Description:	Includes other configuration files from within the server configuration files
Syntax:	Include file-path|directory-path
Context:	server config, virtual host, directory
Status:	Core
Module:	core
Compatibility:	Wildcard matching available in 2.0.41 and later

This directive allows inclusion of other configuration files from within the server configuration files.

Shell-style (fnmatch()) wildcard characters can be used to include several files at once, in alphabetical order. In addition, if Include 
points to a directory, rather than a file, Apache will read all files in that directory and any subdirectory. But including entire 
directories is not recommended, because it is easy to accidentally leave temporary files in a directory that can cause httpd to fail.

The file path specified may be an absolute path, or may be relative to the ServerRoot directory.

Examples:

Include /usr/local/apache2/conf/ssl.conf
Include /usr/local/apache2/conf/vhosts/ *.conf

Or, providing paths relative to your ServerRoot directory:

Include conf/ssl.conf
Include conf/vhosts/ *.conf 

*/

mvceditor::ApacheClass::ApacheClass()
	: ManualConfiguration(false)
	, VirtualHostMappings()
	, HttpdPath()
	, ServerRoot()
	, DocumentRoot()
	, Port(0) { 
}

void mvceditor::ApacheClass::ClearMappings() {
	VirtualHostMappings.clear();
}

bool mvceditor::ApacheClass::Walk(const wxString& file) {
	// in MSW MSI install of apache, there are two sets of configs. one in an "original" directory
	// let's skip this for now
	return !file.Contains(wxT("\\original\\")) && (file.EndsWith(wxT("httpd.conf")) || file.EndsWith(wxT("apache2.conf")));
}

bool mvceditor::ApacheClass::SetHttpdPath(const wxString& httpdPath) {
	HttpdPath = httpdPath;
	if (!ManualConfiguration && Walk(httpdPath)) {
		VirtualHostMappings.clear();
		
		// get the virtual hosts from the file
		ServerRoot = wxT("");
		DocumentRoot.Clear();
		Port = 0;
		ParseApacheConfigFile(httpdPath);		
		return !VirtualHostMappings.empty() || DocumentRoot.IsOk();
	}
	return false;
}

void mvceditor::ApacheClass::SetVirtualHostMapping(const wxString& fileSystemPath, wxString hostName) {
	if (!hostName.EndsWith(wxT("/"))) {
		hostName += wxT("/");
	}
	if (!hostName.StartsWith(wxT("http://")) && !hostName.StartsWith(wxT("https://"))) {
		hostName = wxT("http://") + hostName;
	}
	wxFileName filename;
	filename.AssignDir(fileSystemPath);

	// when inserting into the map, normalize the host document root
	VirtualHostMappings[filename.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME)] = hostName;
}

void mvceditor::ApacheClass::RemoveVirtualHostMapping(const wxString& fileSystemPath) {
	
	// normalize just like the SetVirtualHostMapping() method 
	wxFileName filename;
	filename.AssignDir(fileSystemPath);
	
	std::map<wxString, wxString>::iterator it = VirtualHostMappings.find(filename.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
	if (it != VirtualHostMappings.end()) {
		VirtualHostMappings.erase(it);
	}
}

wxString mvceditor::ApacheClass::GetUrl(const wxString& fileSystemPath) const {
	wxFileName fileToGet(fileSystemPath);
	wxString url;
	for (std::map<wxString, wxString>::const_iterator it = VirtualHostMappings.begin(); it != VirtualHostMappings.end(); ++it) {
		wxString hostRoot = it->first;
		if (0 == fileToGet.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME).Find(hostRoot)) {
			
			// file is inside this virtual host. remove the root and append to host
			wxString baseUrl = it->second;
			if (Port > 0 && Port != 80) {
				// host already has a slash
				baseUrl.RemoveLast();
				baseUrl += wxString::Format(wxT(":%d/"), Port);
			}
			url = baseUrl + fileSystemPath.Mid(hostRoot.length());
			break;
		}
	}
	
	if(url.IsEmpty() && DocumentRoot.IsOk() && 0 == fileToGet.GetPath().Find(DocumentRoot.GetPath())) {
		
		// file is inside Document Root. remove the root and append to host
		wxString baseUrl = wxT("http://localhost/");
		if (Port > 0 && Port != 80) {
			baseUrl = wxString::Format(wxT("http://localhost:%d/"), Port);
		}
		
		url = baseUrl + fileSystemPath.Mid(DocumentRoot.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME).length());
		
		// in case fileSystemPath is a Windows path
		url.Replace(wxT("\\"), wxT("/"));
	}
	return url;
}

wxString mvceditor::ApacheClass::GetHttpdPath() const {
	return HttpdPath;
}

std::map<wxString, wxString> mvceditor::ApacheClass::GetVirtualHostMappings() const {
	std::map<wxString, wxString>mappings;
	for (std::map<wxString, wxString>::const_iterator it = VirtualHostMappings.begin(); it != VirtualHostMappings.end(); ++it) {
		mappings[it->first] = it->second;
	}
	return mappings;
}

void mvceditor::ApacheClass::ParseApacheConfigFile(const wxString& includedFile) {
	if (wxFileExists(includedFile)) {
		wxTextFile file;
		if (file.Open(includedFile)) {
			bool inVirtualHost = false;
			wxString currentServerName,
					currentDocumentRoot,
					currentPort;
			for (wxString line = file.GetFirstLine(); !file.Eof(); line = file.GetNextLine()) {
				
				// apache directives are case insensitive, but file names are not
				// so we need 2 variables for this ...
				line = line.Trim(false).Trim(true);
				wxString lineLower = line.Lower();
				if (0 == lineLower.Find(wxT("<virtualhost"))) {
					currentServerName = wxT("");
					currentDocumentRoot = wxT("");
					inVirtualHost = true;
				}
				if (0 == lineLower.Find(wxT("</virtualhost>"))) {
					currentServerName = wxT("");
					currentDocumentRoot = wxT("");
					inVirtualHost = false;
				}
				if (0 == lineLower.Find(wxT("serverroot"))) {
					ServerRoot = line.Mid(10).Trim(false).Trim(true); //10= length of 'ServerRoot'
				}
				if (0 == lineLower.Find(wxT("include "))) {
					wxString nextFile = line.Mid(8).Trim(false).Trim(true); //8= length of 'Include'
					nextFile = MakeAbsolute(nextFile);
					ParseApacheConfigFile(nextFile);
				}
				if (0 == lineLower.Find(wxT("servername"))) {

					// must get the line that has not been modified to lowercase
					currentServerName = line.Mid(10).Trim(false).Trim(true); //10=length of "ServerName"
				}
				if (0 == lineLower.Find(wxT("documentroot"))) {
					currentDocumentRoot = line.Mid(12).Trim(false).Trim(true); //12=length of "DocumentRoot"
					currentDocumentRoot.Replace(wxT("\""), wxT(""));
					currentDocumentRoot.Replace(wxT("'"), wxT(""));
					if (!inVirtualHost) {
						DocumentRoot.AssignDir(currentDocumentRoot);
						DocumentRoot.Normalize();
					}
				}
				if (0 == lineLower.Find(wxT("listen"))) {
					currentPort = line.Mid(6).Trim(false).Trim(true); //6=length of "listen"

					// handle listen 127.0.0.1:8080
					// hadle listen 8080 
					if (currentPort.Contains(wxT(":"))) {
						currentPort = currentPort.AfterLast(wxT(':'));
					}
					Port = 0;
					long portLong;
					bool parsed = currentPort.ToLong(&portLong);
					if (parsed && portLong <= 65535) {
						Port = portLong;
					}
				}
				if (inVirtualHost && !currentServerName.IsEmpty() && !currentDocumentRoot.IsEmpty()) {
					SetVirtualHostMapping(currentDocumentRoot, currentServerName);
					currentServerName = wxT("");
					currentDocumentRoot = wxT("");
				}
			}
		}
	}
	
	// wxIsWild(wxT("C:\\*.conf")) returns false, maybe it doesn't do wilcards in windows??
	else if (includedFile.Contains(wxT("*"))) {
		wxFileName fileName(includedFile);
		if (fileName.IsOk()) {

			// ATTN: careful, in windows we need the volume in case the executable
			// is in a different drive than the config file
			wxString dirString = fileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME);
			wxString wildCard = fileName.GetFullName();
			wxDir dir;
			if (wxFileName::IsDirReadable(dirString) && dir.Open(dirString)) {
				wxString file;
				if (dir.GetFirst(&file, wildCard)) { 
					ParseApacheConfigFile(dirString + file);
					while (dir.GetNext(&file)) {
						ParseApacheConfigFile(dirString + file);
					}
				}
			}
		}
	}
	else if (wxDirExists(includedFile)) {
		wxDir dir;
		if (wxFileName::IsDirReadable(includedFile) && dir.Open(includedFile)) {
			wxString file;
			wxString newConfigDir = includedFile + wxFileName::GetPathSeparator();
			if (dir.GetFirst(&file)) { 				
				ParseApacheConfigFile(newConfigDir + file);
				while (dir.GetNext(&file)) {
					ParseApacheConfigFile(newConfigDir + file);
				}
			}
		}
	}
}

wxString mvceditor::ApacheClass::MakeAbsolute(const wxString& configPath) {
	wxFileName fileName(configPath);
	if (!fileName.IsAbsolute()) {
		fileName.MakeAbsolute(ServerRoot);
	}
	return fileName.GetFullPath();
}

wxString mvceditor::ApacheClass::GetDocumentRoot() const {
	return DocumentRoot.GetPath(wxPATH_GET_SEPARATOR |  wxPATH_GET_VOLUME);
}

int mvceditor::ApacheClass::GetListenPort() const {
	return Port;
}