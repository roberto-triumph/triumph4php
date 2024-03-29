
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

-- location to the icu-config executable. In linux systems we use this 
-- binary to get the various compiler linker flags to use the ICU library. 
-- Note that if you change this then it's because you installed ICU in a 
-- non-standard location; this means you will also need to add the ICU 
-- lib directory to your LD_LIBRARY_PATH, otherwise the ICU library 
-- will not be found at runtime (ie. when you try to execute triumph.exe).
ICU_CONFIG = 'icu-config'

-- location to the wx-config executable. In linux systems we use this 
-- binary to get the various compiler linker flags to use the wxWidgets library. 
-- Note that if you change this then it's because you installed wxWidgets in a 
-- non-standard location; this means you will also need to add the wxWidgets 
-- lib directory to your LD_LIBRARY_PATH, otherwise the wxWidgets library 
-- will not be found at runtime (ie. when you try to execute triumph.exe).
-- Also, if you build wxWidgets yourself, you will need to build the Unicode
-- version
-- Your command line to build wxWidgets should look like this (for Release mode):
--
--    ./configure --prefix="/path/to/wx/dest" --disable-debug --disable-debug_gdb \
--                --with-gtk --enable-unicode --enable-optimize
--    make
--    make install
--
-- For debug mode:
--
--    ./configure --prefix="/path/to/wx/dest" --with-gtk --enable-unicode --enable-debug
--    make && make install
--
-- Consult the wxWidgets for more info if necessary
--
WX_CONFIG = os.getenv("T4P_WXCONFIG")
if (not WX_CONFIG) then
    WX_CONFIG = 'lib/wxWidgets/triumph/bin/wx-config'
    print "Using default location of lib/wxWidgets/triumph/bin/wx-config for wxWidgets release dir"
end

WX_CONFIG_DEBUG = os.getenv("T4P_WXCONFIG_DEBUG")
if (not WX_CONFIG_DEBUG) then
    WX_CONFIG_DEBUG = 'lib/wxWidgets/triumph_debug/bin/wx-config'
    print "Using default location of lib/wxWidgets/triumph_debug/bin/wx-config for wxWidgets debug dir"
end

-- location of the Git executable. this is used by the
-- setupdev action to get all submodule
GIT = 'git';

-- location of the cmake executable. cmake is used to build the SOCI
-- library (Database Access wrapper)
CMAKE = 'cmake';

-- location of 7-zip, used to unzip downloaded binaries of Triumph
-- dependencies from the Internet
SEVENZIP = 'unzip';

-- location of wget, used to retrieve some of Triumph
-- dependencies from the Internet
WGET = 'wget';


SOCI_DEBUG_INCLUDE_DIR = os.getenv("T4P_SOCI_DEBUG_INCLUDE_DIR");
if (not SOCI_DEBUG_INCLUDE_DIR) then
    SOCI_DEBUG_INCLUDE_DIR = 'lib/soci/triumph/Debug/include';
    print "Using default location of lib/soci/triumph/Debug/include for SOCI debug include dir"
end

SOCI_DEBUG_LIB_DIR = os.getenv("T4P_SOCI_DEBUG_LIB_DIR");
if (not SOCI_DEBUG_LIB_DIR) then
    SOCI_DEBUG_LIB_DIR = 'lib/soci/triumph/Debug/lib64';
    print "Using default location of lib/soci/triumph/Debug/lib64 for SOCI debug lib dir"
end

SOCI_RELEASE_INCLUDE_DIR = os.getenv("T4P_SOCI_RELEASE_INCLUDE_DIR");
if (not SOCI_RELEASE_INCLUDE_DIR) then
    SOCI_RELEASE_INCLUDE_DIR = 'lib/soci/triumph/Release/include';
    print "Using default location of lib/soci/triumph/Release/include for SOCI release include dir"
end

SOCI_RELEASE_LIB_DIR = os.getenv("T4P_SOCI_RELEASE_LIB_DIR");
if (not SOCI_RELEASE_LIB_DIR) then
    SOCI_RELEASE_LIB_DIR = 'lib/soci/triumph/Release/lib64';
    print "Using default location of lib/soci/triumph/Release/lib64 for SOCI release lib dir"
end

-- location of the cmake executable. cmake is used to build the SOCI
-- library (Database Access wrapper)
CMAKE = 'cmake';

-- will look for MySQL files in these directories
-- on linux we will assume it is installed system-wide
MYSQL_INCLUDE_DIR = '/usr/include/mysql/'
MYSQL_LIB_NAME = 'libmysqlclient.so'

-- try serveral locations that way we can support ubuntu 9.0-12.10 without needing
-- changes to this file
MYSQL_LIB_DIR = os.pathsearch('libmysqlclient.so', 
	"/usr/lib/",
	"/usr/lib/mysql",
	"/usr/lib64", 
	"/usr/lib64/mysql", 
	"/usr/lib/x86_64-linux-gnu/",
	"/usr/lib/i386-linux-gnu/"
);
if (not MYSQL_LIB_DIR) then
	print("libmsysqlclient.so NOT FOUND. Please install libmysqlclient.so or change\n".. 
		"the directories being searched by modifying premake_opts_linux.lua.\n" ..
		"See docs/compiling.md for info about installing dependencies on linux.")
	os.exit(-1)
end

-- On some unit tests, Triumph attempt to connect to a database
-- Set the username and password to use here.
-- Triumph will create (and drop) the schema that it uses
T4P_DB_USER = 'triumph';
T4P_DB_PASSWORD = '';

-- will look for SQLite in these directories
SQLITE_INCLUDE_DIR = '/usr/include/'
SQLITE_LIB_NAME = 'libsqlite3.so'

-- try serveral locations that way we can support ubuntu 9.0-12.10 without needing
-- changes to this file
SQLITE_LIB_DIR = os.pathsearch('libsqlite3.so', 
	"/usr/lib", 
	"/usr/lib64", 
	"/usr/lib/x86_64-linux-gnu/",
	"/usr/lib/i386-linux-gnu/"
);

if (not SQLITE_LIB_DIR) then
	print("libsqlite3.so NOT FOUND. Please install libsqlite3.so or change\n".. 
		"the directories being searched by modifying premake_opts_linux.lua.\n" ..
		"See docs/compiling.md for info about installing dependencies on linux.")
	os.exit(-1)
end

-- will look for CURL in these directories
CURL_INCLUDE_DIR = '/usr/include/'
CURL_LIB_NAME = 'libcurl.so'
CURL_LIB_DIR = os.pathsearch('libcurl.so', 
	"/usr/lib",
	"/usr/lib64", 
	"/usr/lib/x86_64-linux-gnu/",
	"/usr/lib/i386-linux-gnu/"
);

if (not CURL_LIB_DIR) then
	print("libcurl.so NOT FOUND. Please install libcurl.so or change\n".. 
		"the directories being searched by modifying premake_opts_linux.lua.\n" ..
		"See docs/compiling.md for info about installing dependencies on linux."
	)
	os.exit(-1)
end

-- will look for boost in these directories
-- dont bother looking for debug version of boost for now
-- assume release libs are installed
BOOST_DEBUG_INCLUDE_DIR = '/usr/include'
BOOST_DEBUG_LIB_DIR = os.pathsearch('libboost_system.so', 
	"/usr/lib",
	"/usr/lib64", 
	"/usr/lib/x86_64-linux-gnu/",
	"/usr/lib/i386-linux-gnu/"
);

if (not BOOST_DEBUG_LIB_DIR) then
	print("libboost_system.so NOT FOUND. Please install libboost_system.so or change\n".. 
		"the directories being searched by modifying premake_opts_linux.lua.\n" ..
		"See docs/compiling.md for info about installing dependencies on linux.")
	os.exit(-1)
end

BOOST_DEBUG_LIB = BOOST_DEBUG_LIB_DIR.. '/libboost_system.so'
BOOST_DEBUG_BIN_DIR = BOOST_DEBUG_LIB_DIR

-- same version for release as debug
BOOST_RELEASE_INCLUDE_DIR = BOOST_DEBUG_INCLUDE_DIR
BOOST_RELEASE_LIB_DIR = BOOST_DEBUG_LIB_DIR
BOOST_RELEASE_LIB = BOOST_DEBUG_LIB
BOOST_RELEASE_BIN_DIR = BOOST_DEBUG_BIN_DIR

-- location where the makefiles / codelist solution files will be placed
BUILD_SCRIPTS_DIR = os.getenv("T4P_BUILD_SCRIPTS_DIR");
if (not BUILD_SCRIPTS_DIR) then
    BUILD_SCRIPTS_DIR = 'build/';
    if (_ACTION) then
        BUILD_SCRIPTS_DIR = 'build/' .. _ACTION
    end
    print ("Using default location of " .. BUILD_SCRIPTS_DIR .. " for build scripts location")
end


-- location of the final lib directory
-- all of the dependant shared libraries (*.so) will be placed here
-- by default this will be the same directory as the directory where
-- the executable is located. in linux, we want our version patched
-- version of wxWidgets and SOCI to be used, so we compile them,  place 
-- them in a directory, and we add a compile flag to look for
-- so files in a specific location (-rpath) so that we don't 
-- use the system version of the libraries. Furthermore, when we
-- make distribution packages (DEB files), we place them in a separate
-- location as well.
-- lib dir can be relative, it is relative it is assumed to be
-- relative to the executable location
T4P_LIB_DIR = os.getenv("T4P_LIB_DIR");
if (not T4P_LIB_DIR) then
    T4P_LIB_DIR = ".";
    print ("Using default location of " .. T4P_LIB_DIR .. " for shared libraries location")
end


-- location of the asset directory
-- the asset directory contains non-source code files needed
-- by Triumph to function properly.  Assets include
-- images
-- sql scripts (to create the tag cache)
-- PHP scripts (PHP detectors)
--
-- in linux, assets live right in a separate directory that is
-- adjacent to the executable. Furthermore, when we
-- make distribution packages (DEB files), we place them in a separate
-- location as well.
-- asset dir can be relative, it is relative it is assumed to be
-- relative to the executable location
T4P_ASSET_DIR = os.getenv("T4P_ASSET_DIR")
if (not T4P_ASSET_DIR) then
    T4P_ASSET_DIR = '../assets'
    print("Using default location of " .. T4P_ASSET_DIR .. " for assets location")
end

