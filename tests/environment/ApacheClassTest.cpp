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
#include <UnitTest++.h>
#include <environment/ApacheClass.h>
#include <FileTestFixtureClass.h>
#include <wx/filename.h>
#include <wx/platinfo.h>
#include <wx/utils.h>

class ApacheTestClass : public FileTestFixtureClass {
public:	
	ApacheTestClass() 
		: FileTestFixtureClass(wxT("apache_test")) {
		Apache = new mvceditor::ApacheClass();
	}
	
	virtual ~ApacheTestClass() {
		delete Apache;
	}
	
	wxFileName TestProjectDirFileName() {
		wxFileName fileName(TestProjectDir);
		return fileName;
	}
	
	wxFileName FileName(wxString file) {
		wxFileName fileName = TestProjectDirFileName();
		fileName.SetFullName(file);
		return fileName;
	}
	
	wxFileName FileName(wxString subDirectory, wxString file) {
		wxFileName fileName = TestProjectDirFileName();
		fileName.AppendDir(subDirectory);
		fileName.SetFullName(file);
		return fileName;
	}
	
	void CreateSubDirectoryFile(const wxString& subDirectory, const wxString& fileName, const wxString& fileContents) {
		wxString path = subDirectory;
		path.Append(wxFileName::GetPathSeparator()).Append(fileName);
		CreateFixtureFile(path, fileContents);
	}
	
	mvceditor::ApacheClass* Apache;
};

SUITE(ApacheClassTest) {

TEST_FIXTURE(ApacheTestClass, WalkShouldFindApacheConfigFile) {
	wxString phpFile = wxT("test.php");
	wxString httpdFile = wxT("httpd.conf");
	CreateFixtureFile(phpFile, wxString::FromAscii(
		"<?php"
	));
	CreateFixtureFile(httpdFile, wxString::FromAscii(
		"<VirtualHost *.80>\n"
		"</VirtualHost>"
	));
	CHECK_EQUAL(false, Apache->Walk(FileName(phpFile).GetFullPath()));
	CHECK(Apache->Walk(FileName(httpdFile).GetFullPath()));
}

TEST_FIXTURE(ApacheTestClass, WalkShouldFindApacheConfigFileWithinSubDirectory) {
	wxString phpFile = wxT("test.php");
	wxString subDirectory = wxT("apache_config");
	wxString httpdFile = wxT("httpd.conf");
	CreateFixtureFile(phpFile, wxString::FromAscii(
		"<?php"
	));	
	CreateSubDirectory(subDirectory);
	CreateSubDirectoryFile(subDirectory, httpdFile, wxString::FromAscii(
		"<VirtualHost *.80>\n"
		"</VirtualHost>"
	));
	mvceditor::DirectorySearchClass search;
	CHECK(search.Init(TestProjectDir));
	search.Walk(*Apache);
	search.Walk(*Apache);
	CHECK_EQUAL(1, (int)search.GetMatchedFiles().size());
}

TEST_FIXTURE(ApacheTestClass, GetUrlShouldWorkWhenVirtualHostIsInHttpConfFile) {
	wxString phpFile = wxT("test.php");
	
	// make sub directory name a mixture of lower/uppercase there
	// was once a bug for this.
	wxString subDirectory = wxT("apache_CONFIG");
	wxString httpdFile = wxT("httpd.conf");
	CreateFixtureFile(phpFile, wxString::FromAscii(
		"<?php"
	));
	CreateSubDirectory(subDirectory);
	CreateSubDirectoryFile(subDirectory, wxT("httpd.conf"), wxString::FromAscii(
		"<IfModule mod_ssl.c>\n"
		"	# SSL name based virtual hosts are not yet supported, therefore no\n"
		"	# NameVirtualHost statement here\n"
		"</IfModule>\n"
		"<VirtualHost *:80>\n"
		"  ServerAdmin webmaster@localhost\n"
		"  ServerName localhost.testing.com\n"
		"  DocumentRoot ") + TestProjectDir + wxString::FromAscii("\n"
		"  CustomLog /var/log/apache2/access.log combined\n"
		"</VirtualHost>\n"
	));
	mvceditor::DirectorySearchClass search;
	CHECK(search.Init(TestProjectDir));
	while (search.More()) {
		search.Walk(*Apache);
	}
	CHECK_EQUAL((size_t)1, search.GetMatchedFiles().size());
	wxString actualHttpdPath = search.GetMatchedFiles()[0];
	CHECK_EQUAL(FileName(subDirectory, httpdFile).GetFullPath(), actualHttpdPath);
	CHECK(Apache->SetHttpdPath(actualHttpdPath));
	wxString s = TestProjectDir + phpFile;
	wxString url = Apache->GetUrl(TestProjectDir + phpFile);
	wxString expectedUrl = wxT("http://localhost.testing.com/test.php");
	CHECK_EQUAL(expectedUrl, url);
}

TEST_FIXTURE(ApacheTestClass, GetUrlShouldWorkWhenListeningOnANonStandardPortAndIsVirtualHost) {
	wxString phpFile = wxT("test.php");
	
	// make sub directory name a mixture of lower/uppercase there
	// was once a bug for this.
	wxString subDirectory = wxT("apache_CONFIG");
	wxString httpdFile = wxT("httpd.conf");
	CreateFixtureFile(phpFile, wxString::FromAscii(
		"<?php"
	));
	CreateSubDirectory(subDirectory);
	CreateSubDirectoryFile(subDirectory, wxT("httpd.conf"), wxString::FromAscii(
		"Listen 8080\n"
		"<VirtualHost *:8080>\n"
		"  ServerAdmin webmaster@localhost\n"
		"  ServerName localhost.testing.com\n"
		"  DocumentRoot ") + TestProjectDir + wxString::FromAscii("\n"
		"  CustomLog /var/log/apache2/access.log combined\n"
		"</VirtualHost>\n"
	));
	mvceditor::DirectorySearchClass search;
	CHECK(search.Init(TestProjectDir));
	while (search.More()) {
		search.Walk(*Apache);
	}
	CHECK_EQUAL((size_t)1, search.GetMatchedFiles().size());
	wxString actualHttpdPath = search.GetMatchedFiles()[0];
	CHECK_EQUAL(FileName(subDirectory, httpdFile).GetFullPath(), actualHttpdPath);
	CHECK(Apache->SetHttpdPath(actualHttpdPath));
	wxString s = TestProjectDir + phpFile;
	wxString url = Apache->GetUrl(TestProjectDir + phpFile);
	wxString expectedUrl = wxT("http://localhost.testing.com:8080/test.php");
	CHECK_EQUAL(expectedUrl, url);
}

TEST_FIXTURE(ApacheTestClass, GetUrlShouldWorkWhenVirtualHostHasAWindowsPath) {
	int major, 
		minor;
	wxOperatingSystemId systemId = wxGetOsVersion(&major, &minor);
	if (wxOS_WINDOWS_NT == systemId) {

		// paths in apache config file have forward slashes
		// windows paths are not like regular windows path c:\\dir\\dir2
		// they are  c:/dir1/dir2
		wxString documentRoot = TestProjectDir;
		documentRoot.Replace(wxT("\\"), wxT("/"));

		wxString phpFile = wxT("test.php");
		
		// make sub directory name a mixture of lower/uppercase there
		// was once a bug for this.
		wxString subDirectory = wxT("apache_CONFIG");
		wxString httpdFile = wxT("httpd.conf");
		CreateFixtureFile(phpFile, wxString::FromAscii(
			"<?php"
		));
		CreateSubDirectory(subDirectory);
		CreateSubDirectoryFile(subDirectory, wxT("httpd.conf"), wxString::FromAscii(
			"<IfModule mod_ssl.c>\n"
			"	# SSL name based virtual hosts are not yet supported, therefore no\n"
			"	# NameVirtualHost statement here\n"
			"</IfModule>\n"
			"<VirtualHost *:80>\n"
			"  ServerAdmin webmaster@localhost\n"
			"  ServerName localhost.testing.com\n"
			"  DocumentRoot ") + documentRoot + wxString::FromAscii("\n"
			"  CustomLog /var/log/apache2/access.log combined\n"
			"</VirtualHost>\n"
		));
		mvceditor::DirectorySearchClass search;
		CHECK(search.Init(TestProjectDir));
		while (search.More()) {
			search.Walk(*Apache);
		}
		CHECK_EQUAL((size_t)1, search.GetMatchedFiles().size());
		wxString actualHttpdPath = search.GetMatchedFiles()[0];
		CHECK_EQUAL(FileName(subDirectory, httpdFile).GetFullPath(), actualHttpdPath);
		CHECK(Apache->SetHttpdPath(actualHttpdPath));
		wxString s = TestProjectDir + phpFile;
		wxString url = Apache->GetUrl(TestProjectDir + phpFile);
		wxString expectedUrl = wxT("http://localhost.testing.com/test.php");
		CHECK_EQUAL(expectedUrl, url);
	}
}

TEST_FIXTURE(ApacheTestClass, GetUrlShouldWorkWhenVirtualHostIsAnIncludedFile) {
	wxString phpFile = wxT("test.php");
	wxString subDirectory = wxT("apache_config");
	wxString httpdFile = wxT("httpd.conf");
	wxString hostsFile = wxT("host.conf");
	wxString hostsFilePath(TestProjectDir);
	hostsFilePath.Append(subDirectory).Append(wxFileName::GetPathSeparator()).Append(hostsFile);
	CreateFixtureFile(phpFile, wxString::FromAscii(
		"<?php"
	));
	CreateSubDirectory(subDirectory);
	CreateSubDirectoryFile(subDirectory, httpdFile,wxString::FromAscii(
		"Include ") + hostsFilePath + wxString::FromAscii("\n")
	);
	CreateSubDirectoryFile(subDirectory, hostsFile, wxString::FromAscii(
		"<VirtualHost *:80>\n"
		"  ServerAdmin webmaster@localhost\n"
		"  ServerName localhost.testing.com\n"
		"  DocumentRoot ") + TestProjectDir + wxString::FromAscii("\n"
		"</VirtualHost>\n"
	));
	mvceditor::DirectorySearchClass search;
	CHECK(search.Init(TestProjectDir));
	while (search.More()) {
		search.Walk(*Apache);
	}
	CHECK_EQUAL((size_t)1, search.GetMatchedFiles().size());
	wxString actualHttpdPath = search.GetMatchedFiles()[0];
	CHECK_EQUAL(FileName(subDirectory, httpdFile).GetFullPath(), actualHttpdPath);
	CHECK(Apache->SetHttpdPath(actualHttpdPath));
	wxString url = Apache->GetUrl(TestProjectDir + phpFile);
	wxString expectedUrl = wxT("http://localhost.testing.com/test.php");
	CHECK_EQUAL(expectedUrl, url);	
}

TEST_FIXTURE(ApacheTestClass, GetUrlShouldWorkWhenVirtualHostIsAnIncludedDirectory) {
	wxString phpFile = wxT("test.php");
	wxString subDirectory = wxT("apache_config");
	wxString httpdFile = wxT("httpd.conf");
	wxString hostsFile = wxT("host.conf");
	wxString hostsSubDirectory;
	hostsSubDirectory.Append(subDirectory).Append(wxFileName::GetPathSeparator()).Append(wxT("hosts")).Append(
		wxFileName::GetPathSeparator());
	CreateFixtureFile(phpFile, wxString::FromAscii(
		"<?php"
	));
	CreateSubDirectory(subDirectory);
	CreateSubDirectoryFile(subDirectory, httpdFile, wxString::FromAscii(
		"  Include ") + TestProjectDir + hostsSubDirectory + wxT("\n"
		));
	CreateSubDirectory(hostsSubDirectory);
	CreateSubDirectoryFile(hostsSubDirectory, hostsFile, wxString::FromAscii(
		"<VirtualHost *:80>\n"
		"  ServerAdmin webmaster@localhost\n"
		"  ServerName localhost.testing.com\n"
		"  DocumentRoot ") + TestProjectDir + wxString::FromAscii("\n"
		"\n"
		"</VirtualHost>\n"
	));
	mvceditor::DirectorySearchClass search;
	CHECK(search.Init(TestProjectDir));
	while (search.More()) {
		search.Walk(*Apache);
	}
	CHECK_EQUAL((size_t)1, search.GetMatchedFiles().size());
	wxString actualHttpdPath = search.GetMatchedFiles()[0];
	wxString expectedPath = TestProjectDir;
	expectedPath.Append(subDirectory).Append(wxFileName::GetPathSeparator()).Append(httpdFile);
	CHECK_EQUAL(expectedPath, actualHttpdPath);
	CHECK(Apache->SetHttpdPath(actualHttpdPath));
	wxString url = Apache->GetUrl(TestProjectDir + wxT("test.php"));
	CHECK_EQUAL(wxT("http://localhost.testing.com/test.php"), url);	
}

TEST_FIXTURE(ApacheTestClass, GetUrlShouldWorkWhenVirtualHostIsAnIncludedRelativeDirectory) {
	wxString phpFile = wxT("test.php");
	wxString subDirectory = wxT("apache_config");
	wxString httpdFile = wxT("httpd.conf");
	wxString hostsFile = wxT("host.conf");
	wxString hostsSubDirectory;
	hostsSubDirectory.Append(subDirectory).Append(wxFileName::GetPathSeparator()).Append(wxT("hosts")).Append(
		wxFileName::GetPathSeparator());
	CreateFixtureFile(phpFile, wxString::FromAscii(
		"<?php"
	));
	CreateSubDirectory(subDirectory);
	CreateSubDirectoryFile(subDirectory, httpdFile, wxString::FromAscii(
		"  ServerRoot ") + TestProjectDir + subDirectory + wxString::FromAscii("\n"
		"  Include ") + wxT("hosts") + wxString::FromAscii("\n"
	));
	CreateSubDirectory(hostsSubDirectory);
	CreateSubDirectoryFile(hostsSubDirectory, hostsFile, wxString::FromAscii(
		"<VirtualHost *:80>\n"
		"  ServerAdmin webmaster@localhost\n"
		"  ServerName localhost.testing.com\n"
		"  DocumentRoot ") + TestProjectDir + wxString::FromAscii("\n"
		"\n"
		"</VirtualHost>\n"
	));
	mvceditor::DirectorySearchClass search;
	CHECK(search.Init(TestProjectDir));
	while (search.More()) {
		search.Walk(*Apache);
	}
	CHECK_EQUAL((size_t)1, search.GetMatchedFiles().size());
	wxString actualHttpdPath = search.GetMatchedFiles()[0];
	wxString expectedPath = TestProjectDir;
	expectedPath.Append(subDirectory).Append(wxFileName::GetPathSeparator()).Append(httpdFile);
	CHECK_EQUAL(expectedPath, actualHttpdPath);
	CHECK(Apache->SetHttpdPath(actualHttpdPath));
	wxString url = Apache->GetUrl(TestProjectDir + wxT("test.php"));
	CHECK_EQUAL(wxT("http://localhost.testing.com/test.php"), url);	
}

TEST_FIXTURE(ApacheTestClass, GetUrlShouldWorkWhenVirtualHostIsAnIncludedWildcard) {
	wxString phpFile = wxT("test.php");
	wxString subDirectory = wxT("apache_config");
	wxString httpdFile = wxT("httpd.conf");
	wxString hostsFile = wxT("host.conf");
	wxString hostsSubDirectory;
	hostsSubDirectory.Append(subDirectory).Append(wxFileName::GetPathSeparator()).Append(wxT("hosts")).Append(
		wxFileName::GetPathSeparator());
	wxString hostsWildCard = wxT("hosts");
	hostsWildCard.Append(wxFileName::GetPathSeparator()).Append(wxT("*.conf"));
	CreateFixtureFile(phpFile, wxString::FromAscii(
		"<?php"
	));
	CreateSubDirectory(subDirectory);
	CreateSubDirectoryFile(subDirectory, httpdFile, wxString::FromAscii(
		"  ServerRoot ") + TestProjectDir + subDirectory + wxString::FromAscii("\n"
		"  Include ") + hostsWildCard + wxString::FromAscii("\n"
	));
	CreateSubDirectory(hostsSubDirectory);
	CreateSubDirectoryFile(hostsSubDirectory, hostsFile, wxString::FromAscii(
		"<VirtualHost *:80>\n"
		"  ServerAdmin webmaster@localhost\n"
		"  ServerName localhost.testing.com\n"
		"  DocumentRoot ") + TestProjectDir + wxString::FromAscii("\n"
		"\n"
		"</VirtualHost>\n"
	));
	mvceditor::DirectorySearchClass search;
	CHECK(search.Init(TestProjectDir));
	while (search.More()) {
		search.Walk(*Apache);
	}
	CHECK_EQUAL((size_t)1, search.GetMatchedFiles().size());
	wxString actualHttpdPath = search.GetMatchedFiles()[0];
	wxString expectedPath = TestProjectDir;
	expectedPath.Append(subDirectory).Append(wxFileName::GetPathSeparator()).Append(httpdFile);
	CHECK_EQUAL(expectedPath, actualHttpdPath);
	CHECK(Apache->SetHttpdPath(actualHttpdPath));
	wxString url = Apache->GetUrl(TestProjectDir + wxT("test.php"));
	CHECK_EQUAL(wxT("http://localhost.testing.com/test.php"), url);	
}

TEST_FIXTURE(ApacheTestClass, SetHttpdPathShouldParseDocumentRoot) {
	wxString phpFile = wxT("test.php");
	CreateFixtureFile(phpFile, wxString::FromAscii(
		"<?php"
	));
	wxString subDirectory = wxT("apache_config");
	CreateSubDirectory(subDirectory);
	wxString httpdFile = wxT("httpd.conf");
	CreateSubDirectoryFile(subDirectory, httpdFile, wxString::FromAscii(
		"DocumentRoot \"") + TestProjectDir + wxString::FromAscii("\"\n"
		"\n"
	));
	mvceditor::DirectorySearchClass search;
	CHECK(search.Init(TestProjectDir));
	while (search.More()) {
		search.Walk(*Apache);
	}
	CHECK_EQUAL((size_t)1, search.GetMatchedFiles().size());
	wxString actualHttpdPath = search.GetMatchedFiles()[0];
	wxString expectedPath = TestProjectDir;
	expectedPath.Append(subDirectory).Append(wxFileName::GetPathSeparator()).Append(httpdFile);
	CHECK_EQUAL(expectedPath, actualHttpdPath);
	CHECK(Apache->SetHttpdPath(actualHttpdPath));
	CHECK_EQUAL(TestProjectDir, Apache->GetDocumentRoot());
	wxString url = Apache->GetUrl(TestProjectDir + wxT("test.php"));
	CHECK_EQUAL(wxT("http://localhost/test.php"), url);	
}

TEST_FIXTURE(ApacheTestClass, SetHttpdPathShouldParseDocumentRootWhenRootIsMissingEndingSeparator) {
	wxString phpFile = wxT("test.php");
	CreateFixtureFile(phpFile, wxString::FromAscii(
		"<?php"
	));
	wxString subDirectory = wxT("apache_config");
	CreateSubDirectory(subDirectory);
	wxString httpdFile = wxT("httpd.conf");

	// document root without ending separator
	wxString documentRoot(TestProjectDir);
	documentRoot.RemoveLast();
	CreateSubDirectoryFile(subDirectory, httpdFile, wxString::FromAscii(
		"DocumentRoot \"") + documentRoot + wxString::FromAscii("\"\n"
		"\n"
	));
	mvceditor::DirectorySearchClass search;
	CHECK(search.Init(TestProjectDir));
	while (search.More()) {
		search.Walk(*Apache);
	}
	CHECK_EQUAL((size_t)1, search.GetMatchedFiles().size());
	wxString actualHttpdPath = search.GetMatchedFiles()[0];
	wxString expectedPath = TestProjectDir;
	expectedPath.Append(subDirectory).Append(wxFileName::GetPathSeparator()).Append(httpdFile);
	CHECK_EQUAL(expectedPath, actualHttpdPath);
	CHECK(Apache->SetHttpdPath(actualHttpdPath));
	CHECK_EQUAL(TestProjectDir, Apache->GetDocumentRoot());
	wxString url = Apache->GetUrl(TestProjectDir + wxT("test.php"));
	CHECK_EQUAL(wxT("http://localhost/test.php"), url);	
}

TEST_FIXTURE(ApacheTestClass, SetHttpdPathShouldParsePort) {
	wxString phpFile = wxT("test.php");
	CreateFixtureFile(phpFile, wxString::FromAscii(
		"<?php"
	));
	wxString subDirectory = wxT("apache_config");
	CreateSubDirectory(subDirectory);
	wxString httpdFile = wxT("httpd.conf");
	CreateSubDirectoryFile(subDirectory, httpdFile, wxString::FromAscii(
		"DocumentRoot \"") + TestProjectDir + wxString::FromAscii("\"\n"
		"Listen 8080 \n"
		"\n"
	));
	mvceditor::DirectorySearchClass search;
	CHECK(search.Init(TestProjectDir));
	while (search.More()) {
		search.Walk(*Apache);
	}
	CHECK_EQUAL((size_t)1, search.GetMatchedFiles().size());
	wxString actualHttpdPath = search.GetMatchedFiles()[0];
	wxString expectedPath = TestProjectDir;
	expectedPath.Append(subDirectory).Append(wxFileName::GetPathSeparator()).Append(httpdFile);
	CHECK_EQUAL(expectedPath, actualHttpdPath);
	CHECK(Apache->SetHttpdPath(actualHttpdPath));
	CHECK_EQUAL(TestProjectDir, Apache->GetDocumentRoot());
	CHECK_EQUAL(8080, Apache->GetListenPort());
	wxString url = Apache->GetUrl(TestProjectDir + wxT("test.php"));
	CHECK_EQUAL(wxT("http://localhost:8080/test.php"), url);	
}

TEST_FIXTURE(ApacheTestClass, SetHttpdPathShouldWorkForWindowsPaths) {
	int major, 
		minor;
	wxOperatingSystemId systemId = wxGetOsVersion(&major, &minor);
	if (wxOS_WINDOWS_NT == systemId) {

		// paths in apache config file have forward slashes
		// windows paths are not like regular windows path c:\\dir\\dir2
		// they are  c:/dir1/dir2
		wxString documentRoot = TestProjectDir;
		documentRoot.Replace(wxT("\\"), wxT("/"));
		wxString phpFile = wxT("test.php");
		CreateFixtureFile(phpFile, wxString::FromAscii(
			"<?php"
		));
		wxString subDirectory = wxT("apache_config");
		CreateSubDirectory(subDirectory);
		wxString httpdFile = wxT("httpd.conf");
		CreateSubDirectoryFile(subDirectory, httpdFile, wxString::FromAscii(
			"DocumentRoot \"") + documentRoot + wxString::FromAscii("\"\n"
			"Listen 80 \n"
			"\n"
		));
		mvceditor::DirectorySearchClass search;
		CHECK(search.Init(TestProjectDir));
		while (search.More()) {
			search.Walk(*Apache);
		}
		CHECK_EQUAL((size_t)1, search.GetMatchedFiles().size());
		wxString actualHttpdPath = search.GetMatchedFiles()[0];
		wxString expectedPath = TestProjectDir;
		expectedPath.Append(subDirectory).Append(wxFileName::GetPathSeparator()).Append(httpdFile);
		CHECK_EQUAL(expectedPath, actualHttpdPath);
		CHECK(Apache->SetHttpdPath(actualHttpdPath));
		CHECK_EQUAL(TestProjectDir, Apache->GetDocumentRoot());
		wxString url = Apache->GetUrl(TestProjectDir + wxT("test.php"));
		CHECK_EQUAL(wxT("http://localhost/test.php"), url);	
	}
}


}