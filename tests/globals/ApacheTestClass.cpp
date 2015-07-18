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
#include <UnitTest++.h>
#include <globals/ApacheClass.h>
#include <FileTestFixtureClass.h>
#include <TriumphChecks.h>
#include <wx/filename.h>
#include <wx/platinfo.h>
#include <wx/utils.h>

class ApacheTestClass : public FileTestFixtureClass {
public:

	wxString PhpFile;
	wxString HttpdFile;
	wxString ConfigSubDirectory;
	wxString VirtualHostFile;
	t4p::DirectorySearchClass Search;
	t4p::ApacheClass Apache;

	ApacheTestClass()
		: FileTestFixtureClass(wxT("apache_test"))
		, PhpFile(wxT("test.php"))
		, HttpdFile(wxT("httpd.conf"))
		, ConfigSubDirectory(wxT("apache_config"))
		, VirtualHostFile(wxT("host.conf"))
		, Search()
		, Apache() {

		// all tests will need a file to resolve
		CreateFixtureFile(PhpFile, wxString::FromAscii(
			"<?php"));
	}

	/**
	 * the method that executes the method being tested; reads the TestProjectDir and parses any
	 * apache config files in that directory (and recurses sub-directories too).
	 */
	void Walk() {
		Search.Init(TestProjectDir, t4p::DirectorySearchClass::PRECISE);
		while (Search.More()) {
			Search.Walk(Apache);
		}
		std::vector<wxString> matchedFiles = Search.GetMatchedFiles();
		if (!matchedFiles.empty()) {
			Apache.SetHttpdPath(matchedFiles[0]);
		}
	}

	/**
	 * creates a 'plain' httpd.conf file (no virtual hosts)
	 * @param subDirectory sub-directory to put file in (relative to TestProjectDir)
	 */
	void CreateHttpdFile(const wxString& subDirectory) {
		CreateSubDirectoryFile(subDirectory, HttpdFile, wxString::FromAscii(
			"ServerRoot \"/etc/apache2\" \n"
			"NameVirtualHost *:8080 \n"
			"DocumentRoot \"") + TestProjectDir + wxString::FromAscii("\" \n"
			"Listen 8080 \n"));
	}

	/**
	 * Creates a httpd.conf file with 1 virtual host entry
	 */
	void CreateHttpdFileWithVirtualHost(const wxString& subDirectory, const wxString& documentRoot, int serverPort = 80, int virtualHostPort = 80) {
		wxString serverPortString = wxString::Format(wxT("%d"), serverPort);
		wxString virtualHostPortString = wxString::Format(wxT("%d"), virtualHostPort);

		CreateSubDirectoryFile(subDirectory, HttpdFile, wxString::FromAscii(
			"Listen ") + serverPortString + wxString::FromAscii(" \n"
			"<IfModule mod_ssl.c>\n"
			"	# SSL name based virtual hosts are not yet supported, therefore no\n"
			"	# NameVirtualHost statement here\n"
			"</IfModule>\n"
			"<VirtualHost *:") + virtualHostPortString + wxString::FromAscii(">\n"
			"  ServerAdmin webmaster@localhost\n"
			"  ServerName localhost.testing.com\n"
			"  DocumentRoot ") + documentRoot + wxString::FromAscii("\n"
			"  CustomLog /var/log/apache2/access.log combined\n"
			"</VirtualHost>\n"));
	}

	/**
	 * Creates a httpd.conf file  with an "include" directive to the given file
	 * @param includeFile the name of the file to include
	 * @param subDirectory the sub directory where the HTTPD.CONF file will be written to
	 */
	void CreateHttpdFileWithIncludeFile(const wxString& includeFile, const wxString& subDirectory) {
		CreateSubDirectoryFile(subDirectory, HttpdFile, wxString::FromAscii(
			"ServerRoot \"/etc/apache2\" \n"
			"NameVirtualHost *:80 \n"
			"DocumentRoot \"") + TestProjectDir + wxString::FromAscii("\" \n"
			"Listen 80 \n"
			"Include ") + includeFile + wxString::FromAscii("\n"));
	}

	/**
	 * Creates a file that contains only virtual host config
	 */
	void CreateVirtualHostFileInSubDirectory(const wxString& subDirectory) {
		CreateSubDirectoryFile(subDirectory, VirtualHostFile, wxString::FromAscii(
			"<VirtualHost *:80>\n"
			"  ServerAdmin webmaster@localhost\n"
			"  ServerName localhost.testing.com\n"
			"  DocumentRoot ") + TestProjectDir + wxString::FromAscii("\n"
			"</VirtualHost>\n"));
	}


	/**
	 * Creates a file that contains only virtual host config with the
	 * ServerName directive After the DocumentRoot directive.
	 */
	void CreateVirtualHostDifferentOrder(const wxString& subDirectory) {
		CreateSubDirectoryFile(subDirectory, VirtualHostFile, wxString::FromAscii(
			"<VirtualHost *:80>\n"
			"  ServerAdmin webmaster@localhost\n"
			"  DocumentRoot ") + TestProjectDir + wxString::FromAscii("\n"
			"  ServerName localhost.testing.com\n"
			"</VirtualHost>\n"));
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
		CreateSubDirectory(subDirectory);
		wxString path = subDirectory;
		path.Append(wxFileName::GetPathSeparator()).Append(fileName);
		CreateFixtureFile(path, fileContents);
	}
};

SUITE(ApacheTestClass) {

TEST_FIXTURE(ApacheTestClass, WalkShouldFindApacheConfigFile) {
	CreateHttpdFile(wxT(""));
	Walk();
	CHECK_EQUAL(wxT("http://localhost/"), Apache.GetVirtualHostMappings()[TestProjectDir]);
	CHECK_EQUAL(8080, Apache.GetListenPort());
}

TEST_FIXTURE(ApacheTestClass, WalkShouldNotParseConditionalPorts) {
	CreateSubDirectoryFile(ConfigSubDirectory, HttpdFile, wxString::FromAscii(
		"Listen 80\n"
		"DocumentRoot ") + TestProjectDir + wxString::FromAscii("\n"
		"<IfModule mod_ssl.c>\n"
		"	Listen 443\n"
		"</IfModule>\n"
	));
	Walk();
	CHECK_EQUAL(wxT("http://localhost/"), Apache.GetVirtualHostMappings()[TestProjectDir]);
	CHECK_EQUAL(80, Apache.GetListenPort());
}


TEST_FIXTURE(ApacheTestClass, WalkShouldFindApacheConfigFileWithinSubDirectory) {
	CreateHttpdFile(ConfigSubDirectory);
	Walk();
	CHECK_EQUAL(wxT("http://localhost/"), Apache.GetVirtualHostMappings()[TestProjectDir]);
	CHECK_EQUAL(8080, Apache.GetListenPort());
}

TEST_FIXTURE(ApacheTestClass, GetUrlShouldWorkWithoutVirtualHosts) {
	CreateHttpdFile(ConfigSubDirectory);
	Walk();
	CHECK_EQUAL(wxT("http://localhost/"), Apache.GetVirtualHostMappings()[TestProjectDir]);
	CHECK_EQUAL(8080, Apache.GetListenPort());
	wxString url = Apache.GetUrl(TestProjectDir + PhpFile);
	wxString expectedUrl = wxT("http://localhost:8080/test.php");
	CHECK_EQUAL(expectedUrl, url);
}

TEST_FIXTURE(ApacheTestClass, GetUrlShouldWorkHostRoot) {
	CreateHttpdFile(ConfigSubDirectory);
	Walk();
	CHECK_EQUAL(wxT("http://localhost/"), Apache.GetVirtualHostMappings()[TestProjectDir]);
	CHECK_EQUAL(8080, Apache.GetListenPort());


	//remove the trailing slash, make sure it still works
	wxString test = TestProjectDir;
	test = test.Mid(0, test.Len() - 1);
	wxString url = Apache.GetUrl(test);
	wxString expectedUrl = wxT("http://localhost:8080/");
	CHECK_EQUAL(expectedUrl, url);
}


TEST_FIXTURE(ApacheTestClass, GetUrlShouldWorkWhenDocumentRootHasAWindowsPath) {
	int major,
		minor;
	wxOperatingSystemId systemId = wxGetOsVersion(&major, &minor);
	if (wxOS_WINDOWS_NT == systemId) {

		// paths in apache config file have forward slashes
		// windows paths are not like regular windows path c:\\dir\\dir2
		// they are  c:/dir1/dir2
		// also test that apache class handles case insesitive paths
		wxString trueDir = TestProjectDir;
		TestProjectDir.Replace(wxT("\\"), wxT("/"));
		TestProjectDir.Replace(wxT("a"), wxT("A"));


		CreateHttpdFile(ConfigSubDirectory);
		Walk();
		CHECK_EQUAL(8080, Apache.GetListenPort());
		wxString url = Apache.GetUrl(TestProjectDir + PhpFile);
		wxString expectedUrl = wxT("http://localhost:8080/test.php");
		CHECK_EQUAL(expectedUrl, url);
	}
}

TEST_FIXTURE(ApacheTestClass, GetUrlShouldWorkWhenVirtualHostIsInHttpConfFile) {

	// make sub directory name a mixture of lower/uppercase there
	// was once a bug for this
	ConfigSubDirectory = wxT("apache_CONFIG");
	CreateHttpdFileWithVirtualHost(ConfigSubDirectory, TestProjectDir);
	Walk();

	CHECK_VECTOR_SIZE(1, Search.GetMatchedFiles());
	wxString actualHttpdPath = Search.GetMatchedFiles()[0];
	CHECK_EQUAL(FileName(ConfigSubDirectory, HttpdFile).GetFullPath(), actualHttpdPath);
	CHECK(Apache.SetHttpdPath(actualHttpdPath));
	wxString url = Apache.GetUrl(TestProjectDir + PhpFile);
	wxString expectedUrl = wxT("http://localhost.testing.com/test.php");
	CHECK_EQUAL(expectedUrl, url);
}

TEST_FIXTURE(ApacheTestClass, GetUrlShouldWorkWhenListeningOnANonStandardPortAndIsVirtualHost) {

	// make sub directory name a mixture of lower/uppercase there
	// was once a bug for this.
	ConfigSubDirectory = wxT("apache_CONFIG");
	CreateHttpdFileWithVirtualHost(ConfigSubDirectory, TestProjectDir, 8080, 8080);
	Walk();

	CHECK_VECTOR_SIZE(1, Search.GetMatchedFiles());
	wxString actualHttpdPath = Search.GetMatchedFiles()[0];
	CHECK_EQUAL(FileName(ConfigSubDirectory, HttpdFile).GetFullPath(), actualHttpdPath);
	CHECK(Apache.SetHttpdPath(actualHttpdPath));
	wxString url = Apache.GetUrl(TestProjectDir + PhpFile);
	wxString expectedUrl = wxT("http://localhost.testing.com:8080/test.php");
	CHECK_EQUAL(expectedUrl, url);
}

TEST_FIXTURE(ApacheTestClass, GetUrlShouldWorkWhenVirtualHostIsOnDifferentPort) {

	// make sub directory name a mixture of lower/uppercase there
	// was once a bug for this.
	ConfigSubDirectory = wxT("apache_CONFIG");
	CreateHttpdFileWithVirtualHost(ConfigSubDirectory, TestProjectDir, 8000, 9000);
	Walk();

	CHECK_VECTOR_SIZE(1, Search.GetMatchedFiles());
	wxString actualHttpdPath = Search.GetMatchedFiles()[0];
	CHECK_EQUAL(FileName(ConfigSubDirectory, HttpdFile).GetFullPath(), actualHttpdPath);
	CHECK(Apache.SetHttpdPath(actualHttpdPath));
	wxString url = Apache.GetUrl(TestProjectDir + PhpFile);
	wxString expectedUrl = wxT("http://localhost.testing.com:9000/test.php");
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

		// make sub directory name a mixture of lower/uppercase there
		// was once a bug for this.
		ConfigSubDirectory = wxT("apache_CONFIG");
		CreateHttpdFileWithVirtualHost(ConfigSubDirectory, documentRoot);
		Walk();

		CHECK_VECTOR_SIZE(1, Search.GetMatchedFiles());
		wxString actualHttpdPath = Search.GetMatchedFiles()[0];
		CHECK_EQUAL(FileName(ConfigSubDirectory, HttpdFile).GetFullPath(), actualHttpdPath);
		CHECK(Apache.SetHttpdPath(actualHttpdPath));
		wxString s = TestProjectDir + PhpFile;
		wxString url = Apache.GetUrl(TestProjectDir + PhpFile);
		wxString expectedUrl = wxT("http://localhost.testing.com/test.php");
		CHECK_EQUAL(expectedUrl, url);
	}
}

TEST_FIXTURE(ApacheTestClass, GetUrlShouldWorkWhenVirtualHostIsAnIncludedFile) {
	wxString hostsFilePath(TestProjectDir);
	hostsFilePath.Append(ConfigSubDirectory).Append(wxFileName::GetPathSeparator()).Append(VirtualHostFile);
	CreateHttpdFileWithIncludeFile(hostsFilePath, ConfigSubDirectory);
	CreateVirtualHostFileInSubDirectory(ConfigSubDirectory);
	Walk();

	CHECK_VECTOR_SIZE(1, Search.GetMatchedFiles());
	wxString actualHttpdPath = Search.GetMatchedFiles()[0];
	CHECK_EQUAL(FileName(ConfigSubDirectory, HttpdFile).GetFullPath(), actualHttpdPath);
	wxString url = Apache.GetUrl(TestProjectDir + PhpFile);
	wxString expectedUrl = wxT("http://localhost.testing.com/test.php");
	CHECK_EQUAL(expectedUrl, url);
}

TEST_FIXTURE(ApacheTestClass, GetUrlShouldWorkWhenVirtualHostIsDefinedInDifferentOrder) {
	wxString hostsFilePath(TestProjectDir);
	hostsFilePath.Append(ConfigSubDirectory).Append(wxFileName::GetPathSeparator()).Append(VirtualHostFile);
	CreateHttpdFileWithIncludeFile(hostsFilePath, ConfigSubDirectory);
	CreateVirtualHostDifferentOrder(ConfigSubDirectory);
	Walk();
	CHECK_VECTOR_SIZE(1, Apache.GetVirtualHostMappings());
	wxString url = Apache.GetUrl(TestProjectDir + PhpFile);
	wxString expectedUrl = wxT("http://localhost.testing.com/test.php");
	CHECK_EQUAL(expectedUrl, url);
}

TEST_FIXTURE(ApacheTestClass, GetUrlShouldWorkWhenVirtualHostIsAnIncludedDirectory) {
	wxString hostsSubDirectory;
	hostsSubDirectory.Append(ConfigSubDirectory).Append(wxFileName::GetPathSeparator()).Append(wxT("hosts")).Append(
		wxFileName::GetPathSeparator());
	CreateSubDirectory(ConfigSubDirectory);
	CreateSubDirectory(ConfigSubDirectory + wxFileName::GetPathSeparator() + wxT("hosts"));
	CreateHttpdFileWithIncludeFile(TestProjectDir + hostsSubDirectory, ConfigSubDirectory);
	CreateVirtualHostFileInSubDirectory(hostsSubDirectory);
	Walk();

	CHECK_VECTOR_SIZE(1, Search.GetMatchedFiles());
	wxString actualHttpdPath = Search.GetMatchedFiles()[0];
	wxString expectedPath = TestProjectDir;
	expectedPath.Append(ConfigSubDirectory).Append(wxFileName::GetPathSeparator()).Append(HttpdFile);
	CHECK_EQUAL(expectedPath, actualHttpdPath);
	wxString url = Apache.GetUrl(TestProjectDir + wxT("test.php"));
	CHECK_EQUAL(wxT("http://localhost.testing.com/test.php"), url);
}

TEST_FIXTURE(ApacheTestClass, GetUrlShouldWorkWhenVirtualHostIsAnIncludedRelativeDirectory) {
	wxString hostsSubDirectory;
	hostsSubDirectory.Append(ConfigSubDirectory).Append(wxFileName::GetPathSeparator()).Append(wxT("hosts")).Append(
		wxFileName::GetPathSeparator());
	CreateSubDirectoryFile(ConfigSubDirectory, HttpdFile, wxString::FromAscii(
		"  ServerRoot ") + TestProjectDir + ConfigSubDirectory + wxString::FromAscii("\n"
		"  Include ") + wxT("hosts") + wxString::FromAscii("\n"
	));
	CreateVirtualHostFileInSubDirectory(hostsSubDirectory);
	Walk();

	CHECK_VECTOR_SIZE(1, Search.GetMatchedFiles());
	wxString actualHttpdPath = Search.GetMatchedFiles()[0];
	wxString expectedPath = TestProjectDir;
	expectedPath.Append(ConfigSubDirectory).Append(wxFileName::GetPathSeparator()).Append(HttpdFile);
	CHECK_EQUAL(expectedPath, actualHttpdPath);
	CHECK(Apache.SetHttpdPath(actualHttpdPath));
	wxString url = Apache.GetUrl(TestProjectDir + wxT("test.php"));
	CHECK_EQUAL(wxT("http://localhost.testing.com/test.php"), url);
}

TEST_FIXTURE(ApacheTestClass, GetUrlShouldWorkWhenVirtualHostIsAnIncludedWildcard) {
	wxString hostsWildCard = wxT("hosts");
	hostsWildCard.Append(wxFileName::GetPathSeparator()).Append(wxT("*.conf"));

	wxString hostsSubDirectory;
	hostsSubDirectory.Append(ConfigSubDirectory).Append(wxFileName::GetPathSeparator()).Append(wxT("hosts")).Append(
		wxFileName::GetPathSeparator());
	CreateSubDirectoryFile(ConfigSubDirectory, HttpdFile, wxString::FromAscii(
		"  ServerRoot ") + TestProjectDir + ConfigSubDirectory + wxString::FromAscii("\n"
		"  Include ") + hostsWildCard + wxString::FromAscii("\n"
	));
	CreateVirtualHostFileInSubDirectory(hostsSubDirectory);
	Walk();

	CHECK_VECTOR_SIZE(1, Search.GetMatchedFiles());
	wxString actualHttpdPath = Search.GetMatchedFiles()[0];
	wxString expectedPath = TestProjectDir;
	expectedPath.Append(ConfigSubDirectory).Append(wxFileName::GetPathSeparator()).Append(HttpdFile);
	CHECK_EQUAL(expectedPath, actualHttpdPath);
	CHECK(Apache.SetHttpdPath(actualHttpdPath));
	wxString url = Apache.GetUrl(TestProjectDir + wxT("test.php"));
	CHECK_EQUAL(wxT("http://localhost.testing.com/test.php"), url);
}

TEST_FIXTURE(ApacheTestClass, SetHttpdPathShouldParseDocumentRoot) {
	CreateHttpdFile(ConfigSubDirectory);

	// no Walk(); just use SetHttpPath() when we already know the location of the config
	// this will happen most of the time; once the user scans for the config directory
	// on subsequent app runs we wont have to scan for the config directory.
	wxString actualHttpdPath = FileName(ConfigSubDirectory, HttpdFile).GetFullPath();
	CHECK(Apache.SetHttpdPath(actualHttpdPath));

	wxString expectedPath = TestProjectDir;
	expectedPath.Append(ConfigSubDirectory).Append(wxFileName::GetPathSeparator()).Append(HttpdFile);
	wxString url = Apache.GetUrl(TestProjectDir + wxT("test.php"));
	CHECK_EQUAL(wxT("http://localhost:8080/test.php"), url);
}

TEST_FIXTURE(ApacheTestClass, SetHttpdPathShouldParseDocumentRootWhenRootIsMissingEndingSeparator) {

	// document root without ending separator
	wxString documentRoot(TestProjectDir);
	documentRoot.RemoveLast();
	CreateSubDirectoryFile(ConfigSubDirectory, HttpdFile, wxString::FromAscii(
		"DocumentRoot \"") + documentRoot + wxString::FromAscii("\"\n"
		"\n"
	));
	Walk();

	CHECK_VECTOR_SIZE(1, Search.GetMatchedFiles());
	wxString actualHttpdPath = Search.GetMatchedFiles()[0];
	wxString expectedPath = TestProjectDir;
	expectedPath.Append(ConfigSubDirectory).Append(wxFileName::GetPathSeparator()).Append(HttpdFile);
	CHECK_EQUAL(expectedPath, actualHttpdPath);
	CHECK(Apache.SetHttpdPath(actualHttpdPath));
	wxString url = Apache.GetUrl(TestProjectDir + wxT("test.php"));
	CHECK_EQUAL(wxT("http://localhost/test.php"), url);
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

		CreateSubDirectoryFile(ConfigSubDirectory, HttpdFile, wxString::FromAscii(
			"DocumentRoot \"") + documentRoot + wxString::FromAscii("\"\n"
			"Listen 80 \n"
			"\n"));
		Walk();

		CHECK_VECTOR_SIZE(1, Search.GetMatchedFiles());
		wxString actualHttpdPath = Search.GetMatchedFiles()[0];
		wxString expectedPath = TestProjectDir;
		expectedPath.Append(ConfigSubDirectory).Append(wxFileName::GetPathSeparator()).Append(HttpdFile);
		CHECK_EQUAL(expectedPath, actualHttpdPath);
		CHECK(Apache.SetHttpdPath(actualHttpdPath));
		wxString url = Apache.GetUrl(TestProjectDir + wxT("test.php"));
		CHECK_EQUAL(wxT("http://localhost/test.php"), url);
	}
}

TEST_FIXTURE(ApacheTestClass, GetUriShouldWorkWithoutVirtualHosts) {
	CreateHttpdFile(ConfigSubDirectory);
	Walk();
	CHECK_EQUAL(wxT("http://localhost/"), Apache.GetVirtualHostMappings()[TestProjectDir]);
	CHECK_EQUAL(8080, Apache.GetListenPort());
	wxString url = Apache.GetUri(TestProjectDir + PhpFile, wxT("/news/index"));
	wxString expectedUrl = wxT("http://localhost:8080/news/index");
	CHECK_EQUAL(expectedUrl, url);
}

}
