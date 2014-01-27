-------------------------------------------------------------------
-- This software is released under the terms of the MIT License
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
-- THE SOFTWARE.
--
-- @copyright  2012 Roberto Perpuly
-- @license    http://www.opensource.org/licenses/mit-license.php The MIT License
-------------------------------------------------------------------

--
-- Below are constants for the various external binaries needed for compiling
-- MVC Editor. These can be changed when the dependant libraries (ICU,
-- wxWidgets, cmake) are not installed system-wide
--

-- location to the ICU release libraries. Make sure to copy the
-- ICU DLLs to the Release directory, otherwise the ICU library
-- will not be found at runtime (ie. when you try to execute mvc-editor.exe).
-- Use the "prep" premake action to copy the DLLs.
ICU_LIBS_RELEASE = {
       "icudt", "icuin", "icuio", "icule",
       "iculx", "icutu", "icuuc"
}

-- location to the ICU debug libraries. Make sure to copy the
-- ICU DLLs to the Debug directory, otherwise the ICU library
-- will not be found at runtime (ie. when you try to execute mvc-editor.exe).
-- Use the "prep" premake action to copy the DLLs.
ICU_LIBS_DEBUG = {
       "icudt", "icuind", "icuiod", "iculed",
       "iculxd", "icutud", "icuucd"
}

icuDir = os.getenv("MVCEDITOR_ICU_DIR")
if (not icuDir) then
	icuDir = "lib/icu"
	print "Using default location of lib/icu for ICU dir"
end

-- location where the ICU lib files are located
ICU_LIB_DIR = icuDir .. "/lib/"

-- location where the ICU header files are located
ICU_INCLUDE_DIR = icuDir .. "/include/"

-- location to the wxWidgets libraries. Make sure to copy the
-- wxWidgets DLLs to the Debug directory, otherwise the wxWidgets library
-- will not be found at runtime (ie. when you try to execute mvc-editor.exe).
-- Also, if you build wxWidgets yourself, you will need to build the Unicode
-- DLL versions
--
-- Consult the wxWidgets for more info if necessary
--
-- these are the core wxWidgets debug libraries and their Win32 dependencies (win dependencies listed first)
WX_LIBS_DEBUG = {
	"winmm", "comctl32", "rpcrt4", "wsock32", "wininet",
	"wxmsw29ud_core", "wxbase29ud_net", "wxbase29ud", "wxexpatd", "wxjpegd", "wxpngd", "wxregexud",
	"wxtiffd", "wxzlibd", "wxbase29ud_xml"
}

-- these are the core wxWidgets Release libraries and their Win32 dependencies (win dependencies listed first)
WX_LIBS_RELEASE = {
	"winmm", "comctl32", "rpcrt4", "wsock32", "wininet",
	"wxmsw29u_core", "wxbase29u_net", "wxbase29u", "wxexpat", "wxjpeg", "wxpng", "wxregexu",
	"wxtiff", "wxzlib", "wxbase29u_xml"
}

-- the wxWidgets GUI debug libraries
WX_LIBS_WINDOW_DEBUG = { "wxmsw29ud_adv", "wxmsw29ud_aui", "wxmsw29ud_html", "wxmsw29ud_richtext" }

-- the wxWidgets GUI release libraries
WX_LIBS_WINDOW_RELEASE = { "wxmsw29u_adv", "wxmsw29u_aui", "wxmsw29u_html", "wxmsw29u_richtext" }

-- the styled text control library
WX_LIB_STC_DEBUG = "wxmsw29ud_stc"

wxWidgetsDir = os.getenv("MVCEDITOR_WXWIDGETS_DIR")
if (not wxWidgetsDir) then
	wxWidgetsDir = "lib/wxWidgets"
	print "Using default location of lib/wxWidgets for wxWidgets dir"
end

WX_LIB_DIR = wxWidgetsDir .. "/lib/vc_dll/"
WX_INCLUDE_DIRS_DEBUG = { wxWidgetsDir .. "/include/", wxWidgetsDir .. "/lib/vc_dll/mswud/" }
WX_INCLUDE_DIRS_RELEASE =  { wxWidgetsDir .. "/include/", wxWidgetsDir .. "/lib/vc_dll/mswu/" }

-- the styled text control library
WX_LIB_STC_RELEASE = "wxmsw29u_stc"

-- location of the Git executable. this is used by the
-- setupdev action to get all submodule
GIT = 'git.exe';

-- location of the cmake executable. cmake is used to build the SOCI
-- library (Database Access wrapper)
CMAKE = 'cmake';

-- location of 7-zip, used to unzip downloaded binaries of MVC Editor
-- dependencies from the Internet
SEVENZIP = '7za';

-- location of wget, used to retrieve some of MVC Editor
-- dependencies from the Internet
WGET = 'wget';

-- location of the batch file that setups the Visual Studio
-- dev environment (adds compiler, linker to the PATH). this 
-- i used by the setupdev action to compile some of the 
-- dependencies
VSVARS = "C:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\Common7\\Tools\\vsvars32.bat";

-- will look for MySQL files in these directories
-- not easy to get MySQL Connector C libs in windows
-- doing a manual, binary install

mysqlDir = os.getenv("MVCEDITOR_MYSQL_CONNECTOR_DIR")
if (not mysqlDir) then

	mysqlDir = "lib/mysql-connector-c-noinstall-6.0.2-win32"
	print("Using default dir for MySQL Connector: " .. mysqlDir)
end
MYSQL_INCLUDE_DIR = mysqlDir .. '/include'
MYSQL_LIB = mysqlDir .. '/lib/libmysql.lib'
MYSQL_LIB_DIR = mysqlDir .. '/lib/'
MYSQL_LIB_NAME = 'libmysql.lib'

-- On some unit tests, MVC Editor attempt to connect to a database
-- Set the username and password to use here.
-- MVC Editor will create (and drop) the schema that it uses
MVCEDITOR_DB_USER = 'mvc-editor'
MVCEDITOR_DB_PASSWORD = ''

-- will look for SQLite in these directories
-- read lib/sqlite/README  for more info
sqliteIncludeDir = os.getenv("MVCEDITOR_SQLITE_INCLUDE_DIR")
if (not sqliteIncludeDir) then
	sqliteIncludeDir = "lib/sqlite-amalgamation-3071300";
	print("Using default dir for SQLITE INCLUDE DIR: " .. sqliteIncludeDir)
end
SQLITE_INCLUDE_DIR = sqliteIncludeDir

sqliteLibDir = os.getenv("MVCEDITOR_SQLITE_LIB_DIR")
if (not sqliteLibDir) then
	sqliteLibDir = "lib/sqlite-dll-win32-x86-3071300";
	print("Using default dir for SQLITE INCLUDE DIR: " .. sqliteLibDir)
end
SQLITE_LIB = sqliteLibDir.. '/sqlite3.lib'
SQLITE_LIB_DIR = sqliteLibDir 
SQLITE_LIB_NAME = 'sqlite3.lib'

-- will look for CURL in these directories
-- these are the directories where
curlDebugDir = os.getenv("MVCEDITOR_CURL_DEBUG_DIR")
if (not curlDebugDir) then
	curlDebugDir = "lib/curl/builds/libcurl-debug-dll-ipv6-sspi";
	print("Using default dir for CURL debug: " .. curlDebugDir)
end
CURL_DEBUG_INCLUDE_DIR = curlDebugDir .. '/include'
CURL_DEBUG_LIB = curlDebugDir .. '/lib/libcurl.lib'
CURL_DEBUG_LIB_DIR = curlDebugDir .. '/lib'
CURL_DEBUG_BIN_DIR = curlDebugDir .. '/bin'

curlReleaseDir = os.getenv("MVCEDITOR_CURL_RELEASE_DIR")
if (not curlReleaseDir) then
	curlReleaseDir = "lib/curl/builds/libcurl-release-dll-ipv6-sspi";
	print("Using default dir for CURL Release: " .. curlReleaseDir)
end
CURL_RELEASE_INCLUDE_DIR = curlReleaseDir .. '/include'
CURL_RELEASE_LIB = curlReleaseDir .. '/lib/libcurl.lib'
CURL_RELEASE_LIB_DIR = curlReleaseDir .. '/lib'
CURL_RELEASE_BIN_DIR = curlReleaseDir .. '/bin'
