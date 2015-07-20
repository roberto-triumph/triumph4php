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
#include <globals/ApacheClass.h>
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

t4p::ApacheClass::ApacheClass()
	: ManualConfiguration(false)
	, VirtualHostMappings()
	, HttpdPath()
	, ServerRoot()
	, Port(0) {
}

t4p::ApacheClass::ApacheClass(const t4p::ApacheClass& src)
	: ManualConfiguration()
	, VirtualHostMappings()
	, HttpdPath()
	, ServerRoot()
	, Port() {
	Copy(src);
}

t4p::ApacheClass& t4p::ApacheClass::operator=(const t4p::ApacheClass& src) {
	Copy(src);
	return *this;
}

void t4p::ApacheClass::Copy(const t4p::ApacheClass& src) {
	// make sure to completely copy wxStrings
	ManualConfiguration = src.ManualConfiguration;
	VirtualHostMappings.clear();
	HttpdPath = src.HttpdPath.c_str();
	ServerRoot = src.ServerRoot.c_str();
	Port = src.Port;
	std::map<wxString, wxString>::const_iterator it;
	for (it = src.VirtualHostMappings.begin(); it != src.VirtualHostMappings.end(); ++it) {
		VirtualHostMappings[it->first.c_str()] = it->second.c_str();
	}
}

void t4p::ApacheClass::ClearMappings() {
	VirtualHostMappings.clear();
}

bool t4p::ApacheClass::Walk(const wxString& file) {
	// in MSW MSI install of apache, there are two sets of configs. one in an "original" directory
	// let's skip this for now
	return !file.Contains(wxT("\\original\\")) && (file.EndsWith(wxT("httpd.conf")) || file.EndsWith(wxT("apache2.conf")));
}

bool t4p::ApacheClass::SetHttpdPath(const wxString& httpdPath) {
	HttpdPath = httpdPath;
	if (!ManualConfiguration && Walk(httpdPath)) {
		VirtualHostMappings.clear();

		// get the virtual hosts from the file
		ServerRoot = wxT("");
		Port = 0;
		ParseApacheConfigFile(httpdPath);
		return !VirtualHostMappings.empty();
	}
	return false;
}

void t4p::ApacheClass::SetVirtualHostMapping(const wxString& fileSystemPath, wxString hostName) {
	if (!hostName.EndsWith(wxT("/"))) {
		hostName += wxT("/");
	}
	if (!hostName.StartsWith(wxT("http://")) && !hostName.StartsWith(wxT("https://"))) {
		hostName = wxT("http://") + hostName;
	}
	wxFileName filename;
	filename.AssignDir(fileSystemPath);

	// when inserting into the map, normalize the host document root
	VirtualHostMappings[filename.GetFullPath()] = hostName;
}

void t4p::ApacheClass::RemoveVirtualHostMapping(const wxString& fileSystemPath) {
	// normalize just like the SetVirtualHostMapping() method
	wxFileName filename;
	filename.AssignDir(fileSystemPath);

	std::map<wxString, wxString>::iterator it = VirtualHostMappings.find(filename.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
	if (it != VirtualHostMappings.end()) {
		VirtualHostMappings.erase(it);
	}
}

wxString t4p::ApacheClass::GetUrl(const wxString& fileSystemPath) const {
	// normalize the given path; also convert to lower case so that we make case-insesitive
	// comparisons (to handle windows paths). also add a trailing slash if fileSystemPath
	// is a directory; when comparing fileSystemPaths the ending separator is irrelevant
	wxFileName fileToGet(fileSystemPath);
	wxString dir = fileToGet.GetFullPath();
	if (wxFileName::DirExists(dir) && !dir.EndsWith(wxFileName::GetPathSeparators())) {
		dir += wxFileName::GetPathSeparators();
	}
	dir.LowerCase();
	wxString url;
	for (std::map<wxString, wxString>::const_iterator it = VirtualHostMappings.begin(); it != VirtualHostMappings.end(); ++it) {
		wxString hostRoot = it->first;
		hostRoot.LowerCase();

		// don't use wxFileName::GetPathSeparators() it produces bad results on Win32
		// seems to return '\/'
		if (hostRoot[hostRoot.Len() - 1] != wxFileName::GetPathSeparator()) {
			hostRoot += wxFileName::GetPathSeparator();
		}
		if (0 == dir.Find(hostRoot)) {
			// file is inside this virtual host. remove the root and append to host
			wxString baseUrl = it->second;

			// look for a port (but careful of the protocol http://
			bool urlHasPort = baseUrl.find_last_of(wxT(":")) > 4;
			if (Port > 0 && Port != 80 && !urlHasPort) {
				// host already has a slash; only append port if virtual host does not have it
				baseUrl.RemoveLast();
				baseUrl += wxString::Format(wxT(":%d/"), Port);
			}
			url = baseUrl + fileSystemPath.Mid(hostRoot.length());
			break;
		}
	}
	return url;
}

wxString t4p::ApacheClass::GetUri(const wxString& fileSystemPath, const wxString& uriPath) const {
	// normalize the given path; also convert to lower case so that we make case-insesitive
	// comparisons (to handle windows paths)
	wxFileName fileToGet(fileSystemPath);
	wxString dir = fileToGet.GetFullPath();
	dir.LowerCase();
	wxString url;
	for (std::map<wxString, wxString>::const_iterator it = VirtualHostMappings.begin(); it != VirtualHostMappings.end(); ++it) {
		wxString hostRoot = it->first;
		hostRoot.LowerCase();
		if (0 == dir.Find(hostRoot)) {
			// file is inside this virtual host. remove the root and append to host
			wxString baseUrl = it->second;

			// look for a port (but careful of the protocol http://
			bool urlHasPort = baseUrl.find_last_of(wxT(":")) > 4;
			if (Port > 0 && Port != 80 && !urlHasPort) {
				// host already has a slash; only append port if virtual host does not have it
				baseUrl.RemoveLast();
				baseUrl += wxString::Format(wxT(":%d/"), Port);
			}
			if (!uriPath.IsEmpty() && uriPath[0] == wxT('/')) {
				url = baseUrl + uriPath.Mid(1);
			} else {
				url = baseUrl + uriPath;
			}
			break;
		}
	}
	return url;
}

wxString t4p::ApacheClass::GetHttpdPath() const {
	return HttpdPath;
}

std::map<wxString, wxString> t4p::ApacheClass::GetVirtualHostMappings() const {
	std::map<wxString, wxString>mappings;
	for (std::map<wxString, wxString>::const_iterator it = VirtualHostMappings.begin(); it != VirtualHostMappings.end(); ++it) {
		mappings[it->first] = it->second;
	}
	return mappings;
}

void t4p::ApacheClass::ParseApacheConfigFile(const wxString& includedFile) {
	if (wxFileExists(includedFile)) {
		wxTextFile file;
		if (file.Open(includedFile)) {
			bool inVirtualHost = false;

			// this flag will be used to skip directives inside a conditional
			// directive. we want to skip this to avoid getting confused when
			// a config has an SSL binding hidden behind a conditional
			bool skipParsing = false;
			wxString currentServerName,
					currentDocumentRoot,
					currentPort;
			for (wxString line = file.GetFirstLine(); !file.Eof(); line = file.GetNextLine()) {
				// apache directives are case insensitive, but file names are not
				// so we need 2 variables for this ...
				line = line.Trim(false).Trim(true);
				wxString lineLower = line.Lower();

				if (0 == lineLower.Find(wxT("<ifmodule"))) {
					skipParsing = true;
				}
				if (0 == lineLower.Find(wxT("</ifmodule>"))) {
					skipParsing = false;
				}
				if (!skipParsing && 0 == lineLower.Find(wxT("<virtualhost"))) {
					currentServerName = wxT("");
					currentDocumentRoot = wxT("");
					currentPort = wxT("");
					inVirtualHost = true;

					// handle the virtual host port
					size_t colonPos = lineLower.find_last_of(wxT(":"));
					if (colonPos != std::string::npos) {
						size_t endPos = lineLower.find_first_of(wxT(">"), colonPos);
						if (endPos != std::string::npos) {
							wxString portString = lineLower.Mid(colonPos + 1, endPos - colonPos - 1);
							portString.Trim();
							long portLong = 0;
							bool parsed = portString.ToLong(&portLong);

							// dont use port on default HTTP port; makes URLs 'prettier'
							if (parsed && portLong <= 65535 && portLong != 80) {
								currentPort = portString;
							}
						}
					}
				}
				if (!skipParsing && 0 == lineLower.Find(wxT("</virtualhost>"))) {
					currentServerName = wxT("");
					currentDocumentRoot = wxT("");
					inVirtualHost = false;
				}
				if (!skipParsing && 0 == lineLower.Find(wxT("serverroot"))) {
					ServerRoot = line.Mid(10).Trim(false).Trim(true);  // 10= length of 'ServerRoot'

					// trim any quotes that the path may have
					ServerRoot.Replace(wxT("\""), wxT(""));
					ServerRoot.Replace(wxT("'"), wxT(""));
				}
				if (!skipParsing && 0 == lineLower.Find(wxT("include "))) {
					wxString nextFile = line.Mid(8).Trim(false).Trim(true);  // 8= length of 'Include'
					nextFile = MakeAbsolute(nextFile);
					ParseApacheConfigFile(nextFile);
				}
				if (!skipParsing && 0 == lineLower.Find(wxT("servername"))) {
					// must get the line that has not been modified to lowercase
					currentServerName = line.Mid(10).Trim(false).Trim(true);  // 10=length of "ServerName"
					if (inVirtualHost && !currentPort.IsEmpty()) {
						// append the port information; sometimes virtual hosts have their own port
						currentServerName.Append(wxT(":")).Append(currentPort);
					}
				}
				if (!skipParsing && 0 == lineLower.Find(wxT("documentroot"))) {
					currentDocumentRoot = line.Mid(12).Trim(false).Trim(true);  // 12=length of "DocumentRoot"
					currentDocumentRoot.Replace(wxT("\""), wxT(""));
					currentDocumentRoot.Replace(wxT("'"), wxT(""));
				}
				if (!skipParsing && 0 == lineLower.Find(wxT("listen"))) {
					currentPort = line.Mid(6).Trim(false).Trim(true);  // 6=length of "listen"

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
				if (!currentDocumentRoot.IsEmpty() && !inVirtualHost) {
					// this is the case for the server document root (outside a virtual host tag)
					currentServerName = wxT("localhost");
					SetVirtualHostMapping(currentDocumentRoot, currentServerName);
					currentServerName = wxT("");
					currentDocumentRoot = wxT("");
					currentPort = wxT("");
				} else if (inVirtualHost && !currentDocumentRoot.IsEmpty() && !currentServerName.IsEmpty()) {
					SetVirtualHostMapping(currentDocumentRoot, currentServerName);
					currentServerName = wxT("");
					currentDocumentRoot = wxT("");
					currentPort = wxT("");
				}
			}
		}
	} else if (includedFile.Contains(wxT("*"))) {
		// wxIsWild(wxT("C:\\*.conf")) returns false, maybe it doesn't do wilcards in windows??
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
	} else if (wxDirExists(includedFile)) {
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

wxString t4p::ApacheClass::MakeAbsolute(wxString configPath) {
	wxString serverRootNative(ServerRoot);

	// in windows apache installs, paths use forward slashes ie. "c:/wamp/bin/apache/2.2.17/conf"
	// in linux this code does nothing
	wxString sep(wxFileName::GetPathSeparator());
	serverRootNative.Replace(wxT("/"), sep);
	configPath.Replace(wxT("/"), sep);

	wxFileName fileName(configPath);
	wxFileName serverRootFileName;
	serverRootFileName.AssignDir(serverRootNative);
	if (!fileName.IsAbsolute()) {
		fileName.Assign(serverRootFileName.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) + configPath);
	}
	return fileName.GetFullPath();
}

int t4p::ApacheClass::GetListenPort() const {
	return Port;
}
