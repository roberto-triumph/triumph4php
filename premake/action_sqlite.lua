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
-- @copyright  2014 Roberto Perpuly
-- @license    http://www.opensource.org/licenses/mit-license.php The MIT License
-------------------------------------------------------------------

--
-- Prepare the SQLite shared library.
-- Triumph uses SQLite in order to store source code meta-data (classes, methods, files)
-- so that they can be looked up easily and quickly. SQLite is a fundamental part
-- of Triumph's total search and code completion functionality.
--
-- SQLite is prepped in different ways.  In Linux or Mac OS X, we tell the developer
-- to get it from the package manager apt or homebrew. In MSW, we fetch a binary version
-- that is hosted in Triumph's servers; this was done so that we use the same version
-- of SQLite in all platforms; as the SQLite site does not seem to have an archive of
-- previous versions.
--
-- See http://sqlite.org/
--
function prepSqlite()
	
	if os.is "windows" then
		sqliteZip = "lib/sqlite-dll-win32-x86-3071300.zip";
		sqliteDownload = "http://www.triumph4php.com/sqlite-dll-win32-x86-3071300.zip"
		extractedDir = 'lib/sqlite-dll-win32-x86-3071300'
		existenceOrDownloadExtract(sqliteZip, extractedDir, sqliteDownload, "Downloading SQLITE dependency");

		--  the amalgamation contains the header files, necessary for compilation
		sqliteZip = "lib/sqlite-amalgamation-3071300.zip";
		sqliteDownload = "http://www.triumph4php.com/sqlite-amalgamation-3071300.zip"
		extractedDir = 'lib/sqlite-amalgamation-3071300'
		existenceOrDownloadExtract(sqliteZip, extractedDir, sqliteDownload, "Downloading SQLITE dependency");
		 
		 -- copy the sqlite dll into its own dir
		 --  we dont care if mkdir fails
		 dllPath = normalizepath('lib/sqlite-dll-win32-x86-3071300');
		 os.execute("mkdir " .. dllPath);
		 batchexecute(normalizepath("lib"), {
			"copy sqlite3.def sqlite-dll-win32-x86-3071300",
			"copy sqlite3.dll sqlite-dll-win32-x86-3071300"
		 });

		-- need to create a .lib file that is needed to build against the sqlite
		-- driver. the .lib file does not come in the sqlite archive
		sqliteLibPath = normalizepath(SQLITE_LIB_DIR .. "/*.dll")
		rootPath = normalizepath("")
		batchexecute(dllPath, {
			
			-- wrap around quotes in case path has spaces
			"\"" .. VSVARS .. "\"",
			"lib.exe /DEF:sqlite3.def /MACHINE:x86",
			"cd " .. rootPath,
			"xcopy /S /Y " .. sqliteLibPath .. " \"Debug\\\"",
			"xcopy /S /Y " .. sqliteLibPath .. " \"Release\\\""
		});
	
	elseif os.is "linux" then  
	
		-- SQLITE_LIB_DIR is already the result of a os.searchpath
		-- which searched the default locations for the sqlite library
		sqliteLib = SQLITE_LIB_DIR
		if sqliteLib == nil then
			error (
				"SQLite client libraries not found.  " .. 
				"Please install the SQLite client library, or change the location of \n" ..
				"SQLITE_LIB_DIR in premake_opts_linux.lua.\n" ..
				"You can install SQLite client via your package manager; ie. sudo apt-get install libsqlite3-dev\n"
			)
		end
	elseif os.is "macosx" then  
	
		-- SQLITE_LIB_DIR is already the result of a os.searchpath
		-- which searched the default locations for the sqlite library
		sqliteLib = SQLITE_LIB_DIR
		if sqliteLib == nil then
			error (
				"SQLite client libraries not found.  " .. 
				"Please install the SQLite client library, or change the location of \n" ..
				"SQLITE_LIB_DIR in premake_opts_macosx.lua.\n" ..
				"You can install SQLite client via a package manager; ie. sudo brew install sqlite\n" ..
				"Note that we use a homebrewed' version, since the system version sqlite was not \n" ..
				"compiled with SQLITE_ENABLE_COLUMN_METADATA and Triumph makes use of this function\n"
			)
		end
	else 
		print "Triumph does not support building SQLite3 on this operating system.\n"
	end
end

newaction {
	trigger = "sqlite",
	description = "Fetch the SQLite client library or check for its existence",
	execute = prepSqlite
}