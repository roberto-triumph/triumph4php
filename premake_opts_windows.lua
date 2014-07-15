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
-- Triumph. These can be changed when the dependant libraries (ICU,
-- wxWidgets, cmake) are not installed system-wide
--

-- location to the ICU release libraries. Make sure to copy the
-- ICU DLLs to the Release directory, otherwise the ICU library
-- will not be found at runtime (ie. when you try to execute triumph.exe).
-- Use the "prep" premake action to copy the DLLs.
ICU_LIBS_RELEASE = {
       "icudt", "icuin", "icuio", "icule",
       "iculx", "icutu", "icuuc"
}

-- location to the ICU debug libraries. Make sure to copy the
-- ICU DLLs to the Debug directory, otherwise the ICU library
-- will not be found at runtime (ie. when you try to execute triumph.exe).
-- Use the "prep" premake action to copy the DLLs.
ICU_LIBS_DEBUG = {
       "icudt", "icuind", "icuiod", "iculed",
       "iculxd", "icutud", "icuucd"
}

ICU_DIR = os.getenv("T4P_ICU_DIR")
if (not ICU_DIR) then
	ICU_DIR = "lib/icu"
	print "Using default location of lib/icu for ICU dir"
end
ICU_DIR = trim(ICU_DIR)

-- location where the ICU lib files are located
ICU_LIB_DIR = ICU_DIR .. "/lib/"

-- location where the ICU header files are located
ICU_INCLUDE_DIR = ICU_DIR .. "/include/"

-- location to the wxWidgets libraries. Make sure to copy the
-- wxWidgets DLLs to the Debug directory, otherwise the wxWidgets library
-- will not be found at runtime (ie. when you try to execute triumph.exe).
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

WXWIDGETS_DIR = os.getenv("T4P_WXWIDGETS_DIR")
if (not WXWIDGETS_DIR) then
	WXWIDGETS_DIR = "lib/wxWidgets"
	print "Using default location of lib/wxWidgets for wxWidgets dir"
end
WXWIDGETS_DIR = trim(WXWIDGETS_DIR )

WX_LIB_DIR = WXWIDGETS_DIR .. "/lib/vc_dll/"
WX_INCLUDE_DIRS_DEBUG = { WXWIDGETS_DIR .. "/include/", WXWIDGETS_DIR .. "/lib/vc_dll/mswud/" }
WX_INCLUDE_DIRS_RELEASE =  { WXWIDGETS_DIR .. "/include/", WXWIDGETS_DIR .. "/lib/vc_dll/mswu/" }

-- the styled text control library
WX_LIB_STC_RELEASE = "wxmsw29u_stc"

-- location of the Git executable. this is used by the
-- setupdev action to get all submodule
GIT = 'C:\\Program Files (x86)\\Git\\bin\\git.exe';

-- location of the cmake executable. cmake is used to build the SOCI
-- library (Database Access wrapper)
CMAKE = 'cmake';

-- location of 7-zip, used to unzip downloaded binaries of Triumph
-- dependencies from the Internet
SEVENZIP = 'C:\\Chocolatey\\bin\\7za';

-- location of wget, used to retrieve some of Triumph
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

MYSQL_CONNECTOR_DIR = os.getenv("T4P_MYSQL_CONNECTOR_DIR")
if (not MYSQL_CONNECTOR_DIR) then

	MYSQL_CONNECTOR_DIR = "lib/mysql-connector-c-noinstall-6.0.2-win32"
	print("Using default dir for MySQL Connector: " .. MYSQL_CONNECTOR_DIR)
end
MYSQL_CONNECTOR_DIR  = trim(MYSQL_CONNECTOR_DIR)

MYSQL_INCLUDE_DIR = MYSQL_CONNECTOR_DIR .. '/include'
MYSQL_LIB = MYSQL_CONNECTOR_DIR .. '/lib/libmysql.lib'
MYSQL_LIB_DIR = MYSQL_CONNECTOR_DIR .. '/lib/'
MYSQL_LIB_NAME = 'libmysql.lib'

-- On some unit tests, Triumph attempt to connect to a database
-- Set the username and password to use here.
-- Triumph will create (and drop) the schema that it uses
T4P_DB_USER = 'triumph'
T4P_DB_PASSWORD = ''

-- will look for SQLite in these directories
-- read lib/sqlite/README  for more info
SQLITE_INCLUDE_DIR = os.getenv("T4P_SQLITE_INCLUDE_DIR")
if (not SQLITE_INCLUDE_DIR) then
	SQLITE_INCLUDE_DIR = "lib/sqlite-amalgamation-3071300";
	print("Using default dir for SQLITE INCLUDE DIR: " .. SQLITE_INCLUDE_DIR)
end
SQLITE_INCLUDE_DIR = trim(SQLITE_INCLUDE_DIR)

SQLITE_LIB_DIR = os.getenv("T4P_SQLITE_LIB_DIR")
if (not SQLITE_LIB_DIR) then
	SQLITE_LIB_DIR = "lib/sqlite-dll-win32-x86-3071300";
	print("Using default dir for SQLITE INCLUDE DIR: " .. SQLITE_LIB_DIR)
end
SQLITE_LIB_DIR = trim(SQLITE_LIB_DIR)
SQLITE_LIB = SQLITE_LIB_DIR .. '/sqlite3.lib'
SQLITE_LIB_NAME = 'sqlite3.lib'

-- will look for SOCI in these directories
-- so that we can move the SOCI files to an outside dir if we want to
SOCI_INCLUDE_DIR = os.getenv("T4P_SOCI_INCLUDE_DIR");
if (not SOCI_INCLUDE_DIR) then
    SOCI_INCLUDE_DIR = "lib/soci/triumph/include",
    print "Using default location of lib/soci/triumph/include for SOCI debug include dir"
end
SOCI_INCLUDE_DIR = trim(SOCI_INCLUDE_DIR)

SOCI_LIB_DIR = os.getenv("T4P_SOCI_LIB_DIR");
if (not SOCI_LIB_DIR) then
    SOCI_LIB_DIR = 'lib/soci/src/lib/Release';
    print "Using default location of lib/soci/src/lib/Release for SOCI debug lib dir"
end
SOCI_LIB_DIR = trim(SOCI_LIB_DIR);

-- will look for CURL in these directories
-- these are the directories where
curlDebugDir = os.getenv("T4P_CURL_DEBUG_DIR")
if (not curlDebugDir) then
	curlDebugDir = "lib/curl/builds/libcurl-debug-dll-ipv6-sspi";
	print("Using default dir for CURL debug: " .. curlDebugDir)
end
CURL_DEBUG_INCLUDE_DIR = curlDebugDir .. '/include'
CURL_DEBUG_LIB = curlDebugDir .. '/lib/libcurl.lib'
CURL_DEBUG_LIB_DIR = curlDebugDir .. '/lib'
CURL_DEBUG_BIN_DIR = curlDebugDir .. '/bin'

CURL_RELEASE_DIR = os.getenv("T4P_CURL_RELEASE_DIR")
if (not CURL_RELEASE_DIR) then
	CURL_RELEASE_DIR = "lib/curl/builds/libcurl-release-dll-ipv6-sspi";
	print("Using default dir for CURL Release: " .. CURL_RELEASE_DIR)
end
CURL_RELEASE_DIR = trim(CURL_RELEASE_DIR)
CURL_RELEASE_INCLUDE_DIR = CURL_RELEASE_DIR .. '/include'
CURL_RELEASE_LIB = CURL_RELEASE_DIR .. '/lib/libcurl.lib'
CURL_RELEASE_LIB_DIR = CURL_RELEASE_DIR .. '/lib'
CURL_RELEASE_BIN_DIR = CURL_RELEASE_DIR .. '/bin'

-- will look for boost in these directories
boostDebugDir = os.getenv("T4P_BOOST_DEBUG_DIR")
if (not boostDebugDir) then
	boostDebugDir = "lib/boost_1_46_0";
	print("Using default dir for boost debug: " .. boostDebugDir)
end  
BOOST_DEBUG_INCLUDE_DIR = boostDebugDir
BOOST_DEBUG_LIB_DIR = boostDebugDir .. '/stage/lib'
BOOST_DEBUG_LIB = BOOST_DEBUG_LIB_DIR .. '/boost_system-vc90-mt-gd-1_46.lib'
BOOST_DEBUG_BIN_DIR = boostDebugDir .. '/bin.v2/libs/system/build/msvc-9.0/debug/threading-multi'

boostReleaseDir = os.getenv("T4P_BOOST_RELEASE_DIR")
if (not boostReleaseDir) then
	boostReleaseDir = "lib/boost_1_46_0/";
	print("Using default dir for boost release: " .. boostReleaseDir)
end  
BOOST_RELEASE_INCLUDE_DIR = boostReleaseDir
BOOST_RELEASE_LIB_DIR = boostReleaseDir .. '/stage/lib'
BOOST_RELEASE_LIB = BOOST_RELEASE_LIB_DIR .. '/boost_system-vc90-mt-1_46.lib'
BOOST_RELEASE_BIN_DIR = boostReleaseDir .. '/bin.v2/libs/system/build/msvc-9.0/release/threading-multi'
BOOST_RELEASE_DIR = boostReleaseDir 

-- location of the final lib directory
-- all of the dependant shared libraries (DLLs) will be placed here
-- by default this will be the same directory as the directory where
-- the executable is located. in MSW, DLLs are searched in the
-- same directory as the executable; it is easiest to place them
-- together.
-- lib dir can be relative, it is relative it is assumed to be
-- relative to the executable location
T4P_LIB_DIR = os.getenv("T4P_LIB_DIR");
if (not T4P_LIB_DIR) then
    T4P_LIB_DIR = ".";
    print ("Using default location of " .. T4P_LIB_DIR .. " for shared libraries location")
end

-- location where the vs solution files will be placed
BUILD_SCRIPTS_DIR = os.getenv("T4P_BUILD_SCRIPTS_DIR");
if (not BUILD_SCRIPTS_DIR) then
    BUILD_SCRIPTS_DIR = 'build/';
    if (_ACTION) then
        BUILD_SCRIPTS_DIR = 'build/' .. _ACTION
    end
    print ("Using default location of " .. BUILD_SCRIPTS_DIR .. " for build scripts location")
end

-- location of the asset directory
-- the asset directory contains non-source code files needed
-- by Triumph fto function properly.  Assets include
-- images
-- sql scripts (to create the tag cache)
-- PHP scripts (PHP detectors)
--
-- in MSW, assets live right in a sub directory of where
-- the executable is located
-- asset dir can be relative, it is relative it is assumed to be
-- relative to the executable location
T4P_ASSET_DIR = os.getenv("T4P_ASSET_DIR")
if (not T4P_ASSET_DIR) then
    T4P_ASSET_DIR = '../assets'
    print("Using default location of " .. T4P_ASSET_DIR .. " for assets location")
end

