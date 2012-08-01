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

-- location where the ICU lib files are located
ICU_LIB_DIR = "lib/icu/lib/"

-- location where the ICU header files are located
ICU_INCLUDE_DIR = "lib/icu/include/"

-- location to the wxWidgets libraries. Make sure to copy the
-- wxWidgets DLLs to the Debug directory, otherwise the wxWidgets library
-- will not be found at runtime (ie. when you try to execute mvc-editor.exe).
-- Also, if you build wxWidgets yourself, you will need to build the Unicode
-- DLL versions, AND make sure to compile the wxStyledTextCtrl that is located
-- in the contrib directory of the wxWidgets distribution
--
-- Consult the wxWidgets for more info if necessary
--
-- these are the core wxWidgets debug libraries and their Win32 dependencies (win dependencies listed first)
WX_LIBS_DEBUG = {
	"winmm", "comctl32", "rpcrt4", "wsock32", "odbc32",
	"wxmsw28ud_core", "wxbase28ud_net", "wxbase28ud", "wxexpatd", "wxjpegd", "wxpngd", "wxregexud",
	"wxtiffd", "wxzlibd"
}

-- these are the core wxWidgets Release libraries and their Win32 dependencies (win dependencies listed first)
WX_LIBS_RELEASE = {
	"winmm", "comctl32", "rpcrt4", "wsock32", "odbc32",
	"wxmsw28u_core", "wxbase28u_net", "wxbase28u", "wxexpat", "wxjpeg", "wxpng", "wxregexu",
	"wxtiff", "wxzlib"
}

-- the wxWidgets GUI debug libraries
WX_LIBS_WINDOW_DEBUG = { "wxmsw28ud_adv", "wxmsw28ud_aui", "wxmsw28ud_html" }

-- the wxWidgets GUI release libraries
WX_LIBS_WINDOW_RELEASE = { "wxmsw28u_adv", "wxmsw28u_aui", "wxmsw28u_html" }

-- NOTE: for this configuration to work correctly a WXWIN environment variable must be defined and
-- must point to the location of wxWidgets
WX_LIB_DIR = "$(WXWIN)/lib/vc_dll/"
WX_INCLUDE_DIRS_DEBUG = { "$(WXWIN)/include/", "$(WXWIN)/lib/vc_dll/mswud/" }

-- NOTE: for this configuration to work correctly a WXWIN environment variable must be defined and
-- must point to the location of wxWidgets
WX_INCLUDE_DIRS_RELEASE =  { "$(WXWIN)/include/", "$(WXWIN)/lib/vc_dll/mswu/" }

-- NOTE: for this configuration to work correctly a WXWIN environment variable must be defined and
-- must point to the location of wxWidgets
WX_STC_INCLUDE_DIRS = "$(WXWIN)/contrib/include/"


-- location of the cmake executable. cmake is used to build the SOCI
-- library (Database Access wrapper)
CMAKE = 'cmake';

-- will look for MySQL files in these directories
-- not easy to get MySQL Connector C libs in windows
-- doing a manual, binary install
MYSQL_INCLUDE_DIR = 'lib/mysql-connector-c-noinstall-6.0.2-win32/include'
MYSQL_LIB = 'lib/mysql-connector-c-noinstall-6.0.2-win32/lib/libmysql.lib'
MYSQL_LIB_DIR = 'lib/mysql-connector-c-noinstall-6.0.2-win32/lib/'
MYSQL_LIB_NAME = 'libmysql.lib'

-- On some unit tests, MVC Editor attempt to connect to a database
-- Set the username and password to use here.
-- MVC Editor will create (and drop) the schema that it uses
MVCEDITOR_DB_USER = 'root';
MVCEDITOR_DB_PASSWORD = '';

-- will look for SQLite in these directories
-- read lib/sqlite/README  for more info
SQLITE_INCLUDE_DIR = 'lib/sqlite/include'
SQLITE_LIB = 'lib/sqlite/lib/sqlite3.lib'
SQLITE_LIB_DIR = 'lib/sqlite/lib'
SQLITE_LIB_NAME = 'sqlite3.lib'
