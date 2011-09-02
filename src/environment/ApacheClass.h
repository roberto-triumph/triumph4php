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
#ifndef __apacheclass__
#define __apacheclass__

#include <search/DirectorySearchClass.h>
#include <wx/filename.h>
#include <wx/string.h>
#include <map>

namespace mvceditor {

/**
 * The ApacheClass searches through Apache config files for virtual host entries.  It enables
 * the user of this class to transform file system paths into web-browser accessible URLs. This class does not
 * actually recurse through directories; this Apache class can be used in conjunction with the DirectorySearch class
 * in order to search the file system for apache config files. 
 * 
 * Usage: The following code will search the entire "/etc/" directory for apache config files
 * 
 * <code>
 *   ApacheClass apache;
 *   DirectorySearchClass search;
 *   wxString startDir = wxT("/etc/");
 *   if (search.Init(startDir)) {
 *     // iterate through all files including sub-directories
 * 	  while (search.More()) {
 * 	    search.Walk(apache);
 * 	  }
 *    std::vector<wxString> possibleConfigFiles = search.GetMatchedFiles();
 *    // there may be multiple files, at this point just print out the number of defined virtual hosts
 *    for (int i = 0; i <  possibleConfigFiles.size(); ++i) {
 *      if (apache.SetHttpdPath(possibleConfigFiles[i])) {
 *        printf("Number of virtual hosts in config file:%s  :%d\n", (const char*)possibleConfigFiles[i].ToUTF8(), apache.GetVirtualHostMappings().size());
 *      }
 *      else {
 *        printf("No virtual hosts found in config file:%s\n", (const char*)possibleConfigFiles[i].ToUTF8());
 *      }
 *    }
 *  }
 *  else {
 *    puts("/etc/ not readable??");
 *  }
 * </code>
 */
class ApacheClass : public DirectoryWalkerClass {
public:

	ApacheClass();

	/**
	 * Checks the given file to see if it is a valid apache configuration file.
	 * 
	 * @return bool true if this file is an apache config file (check is done via name only)
	 */
	virtual bool Walk(const wxString& file);
	
	/**
	 * Set the path (manually). Will parse the config file for any virtual host mappings. Included files will get parsed as well.
	 * 
	 * @param wxString httpdPath the path
	 * @return bool true if the given files (or included files) contain at least one virtual host entry or a DocumentRoot declaration.
	 */
	bool SetHttpdPath(const wxString& httpdPath);
	
	/**
	 * Gets the last set path. This may be invalid if SetHttpdPath() method returned false
	 * @return wxString
	 */
	wxString GetHttpdPath() const;

	/**
	 * Get the parsed port where apache is listening.
	 * @return int, may be zero if SetHttpdPath() method returned false
	 */
	int GetListenPort() const;

	/**
	 * Get the parsed documetn root where apache is serving files.
	 * @return wxString This is a path; it will always have the trailing separator.
	 */
	wxString GetDocumentRoot() const;
	
	/**
	 * Add a virtual host mapping.  Usually, the SetHttpdPath does this automatically.
	 * 
	 * @param wxString fileSystemPath the directory that serves files for the host
	 * @param wxString hostName the name of the host
	 */
	void SetVirtualHostMapping(wxString fileSystemPath, wxString hostName);
	
	/**
	 * Get the virtual host mappings that were parsed from the apache config files.
	 * 
	 * @return std::map<wxString, wxString>  map keys=File System Document Root, map values=Host name
	 * The file system document root will always have the  trailing path separator
	 */
	std::map<wxString, wxString> GetVirtualHostMappings() const;
	
	/**
	 * This method will resolve a file in the file system to a URL that can be accessed.  The URL is 
	 * built using the apache virtual host entry.  For example, let's say that there is a virtual host entry for
	 * ServerName: www.company.com whose directory root is /var/www/company/pub/. Then, if this method
	 * is given a file system path og /var/www/company/pub/folder/index.php, then this method will return
	 * http://www.company.com/folder/index.php
	 * 
	 * @param wxString fileSystemPath the file that needs to be accessed via a URL
	 * @return wxxString the URL based on the httpd.conf file that was set
	 */
	wxString GetUrl(const wxString& fileSystemPath) const;
	
private:
	
	/**
	 * Open an apache config file extract virtual host information.  This method will correctly handle
	 * files, directories, as well as wilcard characters.
	 * @param wxString includedFile the file or directory path specified in the Include directive.
	 */
	void ParseApacheConfigFile(const wxString& includedFile);
	
	/**
	 * Make a given path absolute.  If path is already absolute, nothing is done.  If path is relative
	 * then we will use ServerRoot to make the path absolute.
	 * 
	 * @param wxString configPath the path to check
	 * @return wxString the absolute path
	 */
	wxString MakeAbsolute(const wxString& configPath);
	
	/**
	 * The directory name => host name.  We will use these mappings to turn a file system path into a 
	 * URL. The key is the file system path.  The value is the hostname.  
	 * 
	 * @var map<wxString, wxString> The directory name is always guaranteed to end with the separator
	 *   The hostname is always guaranteed to start with "http://" end with a '/'
	 */
	std::map<wxString, wxString> VirtualHostMappings;
	
	/**
	 * File path to the httpd.conf file
	 * 
	 * @var wxString
	 */
	wxString HttpdPath;

	/**
	 * The server root directive. This is used in case any config paths are relative.
	 * @var wxString
	 */
	 wxString ServerRoot;

	 /**
	 * The document root directive. This is where the web server files are located.
	 * @var wxString
	 */
	 wxFileName DocumentRoot;

	 /**
	  * The Listen directive.  This is the port at which apache is listening.
	  */
	 int Port;
};

}
#endif // __apacheclass__
