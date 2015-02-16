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
-- @copyright  2009-2011 Roberto Perpuly
-- @license    http://www.opensource.org/licenses/mit-license.php The MIT License
-------------------------------------------------------------------

--
-- Prepare the SOCI library
-- SOCI is a database access library. Triumph uses SOCI to be able to query
-- MySQL and SQLite databases using a single API. 
--
-- The SOCI code is included as a git submodule. On all platforms the 
-- SOCI code is fetched and compiled.  We do this because some linux 
-- distributions might have a really old version of SOCI.
--
-- The building instructions are in the code, in lib/soci/doc.
-- SOCI requires cmake, so the system needs to have cmake installed.
--
-- See http://soci.sourceforge.net/
--
function prepSoci() 
	SOCI_BUILD_DIR =  normalizepath("lib/soci/triumph");
	SOCI_ROOT = normalizepath("lib/soci")
	SOCI_SRC = normalizepath("lib/soci/src")
	if os.is "windows" then

		-- compile SOCI
		-- exclude SOCI from linking against Boost. we don't use it
		sociPath = normalizepath("lib/soci")
		sociBuildDir =  normalizepath("lib/soci/triumph")
		sociSrc = normalizepath("lib/soci/src")
		rootPath = normalizepath("")
		batchexecute(sociSrc, {
			CMAKE ..
				" -G \"Visual Studio 9 2008\"" ..
				" -DMYSQL_INCLUDE_DIR=" .. normalizepath(MYSQL_INCLUDE_DIR) ..
				" -DMYSQL_LIBRARY=" .. normalizepath(MYSQL_LIB) ..
				" -DCMAKE_INSTALL_PREFIX=" .. sociBuildDir .. 
				" -DSQLITE3_INCLUDE_DIR=" .. normalizepath(SQLITE_INCLUDE_DIR) ..
				" -DSQLITE3_LIBRARY=" .. normalizepath(SQLITE_LIB) ..
				" -DWITH_MYSQL=YES " ..
				" -DWITH_ODBC=NO " ..
				" -DWITH_ORACLE=NO " ..
				" -DWITH_POSTGRESQL=NO " ..
				" -DWITH_SQLITE3=YES " ..
				" -DWITH_BOOST=NO " ..
				" -DSOCI_TESTS=YES ",
			
			-- wrap around quotes in case path has spaces
			"\"" .. VSVARS .. "\"",
			"vcbuild SOCI.sln \"Debug|Win32\"",
			"vcbuild SOCI.sln \"Release|Win32\"",
			"vcbuild INSTALL.vcproj \"Debug|Win32\"",
			"vcbuild INSTALL.vcproj \"Release|Win32\"",
			"cd " .. rootPath,
			"xcopy /S /Y " .. normalizepath('lib/soci/src/bin/Debug/*.dll') .. " \"Debug\\\"",
			"xcopy /S /Y " .. normalizepath('lib/soci/src/bin/Release/*.dll') .. " \"Release\\\"",
			
		})
	elseif os.is "linux" then 
		sociInstallDebugDir = normalizepath("lib/soci/triumph/Debug")
		sociInstallReleaseDir = normalizepath("lib/soci/triumph/Release")

		batchexecute(normalizepath(""), {
			"mkdir -p " .. sociInstallDebugDir,
			"mkdir -p " .. sociInstallReleaseDir
		})
		
		-- now build SOCI
		-- exclude SOCI from linking against Boost. we don't use it
		batchexecute(sociInstallDebugDir, {
			CMAKE ..
				" -G \"Unix Makefiles\"" ..
				" -DMYSQL_INCLUDE_DIR=" .. MYSQL_INCLUDE_DIR ..
				" -DMYSQL_LIBRARY=" .. MYSQL_LIB_DIR .. "/" .. MYSQL_LIB_NAME ..
				" -DCMAKE_INSTALL_PREFIX=" .. sociInstallDebugDir ..
				" -DSQLITE3_INCLUDE_DIR=" .. SQLITE_INCLUDE_DIR ..
				" -DSQLITE3_LIBRARIES=" .. normalizepath(SQLITE_LIB_DIR) ..
				" -DWITH_MYSQL=YES " ..
				" -DWITH_MYSQL=ODBC " ..
				" -DWITH_ORACLE=NO " ..
				" -DWITH_POSTGRESQL=NO " ..
				" -DWITH_SQLITE3=YES " ..
				" -DWITH_BOOST=NO" ..
				" -DSOCI_TESTS=YES " ..
				" -DCMAKE_BUILD_TYPE=Debug " .. 
				" " .. SOCI_SRC,
			"make",
			"make install"
		})
		
		-- build release mode
		batchexecute(sociInstallReleaseDir, {
			CMAKE ..
				" -G \"Unix Makefiles\"" ..
				" -DMYSQL_INCLUDE_DIR=" .. MYSQL_INCLUDE_DIR ..
				" -DMYSQL_LIBRARY=" .. MYSQL_LIB_DIR .. "/" .. MYSQL_LIB_NAME ..
				" -DCMAKE_INSTALL_PREFIX=" .. sociInstallReleaseDir ..
				" -DSQLITE3_INCLUDE_DIR=" .. SQLITE_INCLUDE_DIR ..
				" -DSQLITE3_LIBRARIES=" .. normalizepath(SQLITE_LIB_DIR) ..
				" -DWITH_MYSQL=YES " ..
				" -DWITH_MYSQL=ODBC " ..
				" -DWITH_ORACLE=NO " ..
				" -DWITH_POSTGRESQL=NO " ..
				" -DWITH_SQLITE3=YES " ..
				" -DWITH_BOOST=NO" ..
				" -DSOCI_TESTS=YES " ..
				" -DCMAKE_BUILD_TYPE=Debug " .. 
				" " .. SOCI_SRC,
			"make",
			"make install"
		});
		
		-- soci lib dirs are named according to architecture
		foundCount = 0;
		libs = os.matchfiles("lib/soci/triumph/Debug/lib64/*.so*");
		if #libs > 0 then
			os.execute("cp -r " .. os.getcwd() .. "/lib/soci/triumph/Debug/lib64/*.so* Debug/");
			foundCount = foundCount + 1;
		else 
			libs = os.matchfiles("lib/soci/triumph/Debug/lib/*.so*");
			if #libs > 0 then
				os.execute("cp -r " .. os.getcwd() .. "/lib/soci/triumph/Debug/lib/*.so* Debug/");
				foundCount = foundCount + 1;
			end
		end
		libs = os.matchfiles("lib/soci/triumph/Release/lib64/*.so*");
		if #libs > 0 then
			os.execute("cp -r " .. os.getcwd() .. "/lib/soci/triumph/Release/lib64/*.so* Release/");
			foundCount = foundCount + 1;
		else
			libs = os.matchfiles("lib/soci/triumph/Release/lib/*.so*");
			if #libs > 0 then
				os.execute("cp -r " .. os.getcwd() .. "/lib/soci/triumph/Release/lib/*.so* Release/");
				foundCount = foundCount + 1;
			end
		end
	elseif os.is "macosx" then 
		sociInstallDebugDir = normalizepath("lib/soci/triumph/Debug")
		sociInstallReleaseDir = normalizepath("lib/soci/triumph/Release")

		batchexecute(normalizepath(""), {
			"mkdir -p " .. sociInstallDebugDir,
			"mkdir -p " .. sociInstallReleaseDir
		})
		
		-- now build SOCI
		-- exclude SOCI from linking against Boost. we don't use it
		batchexecute(sociInstallDebugDir, {
			CMAKE ..
				" -G \"Unix Makefiles\"" ..
				" -DMYSQL_INCLUDE_DIR=" .. MYSQL_INCLUDE_DIR ..
				" -DMYSQL_LIBRARY=" .. MYSQL_LIB_DIR .. "/" .. MYSQL_LIB_NAME ..
				" -DCMAKE_INSTALL_PREFIX=" .. sociInstallDebugDir ..
				" -DSQLITE3_INCLUDE_DIR=" .. SQLITE_INCLUDE_DIR ..
				" -DSQLITE3_LIBRARY=" .. normalizepath(SQLITE_LIB_DIR) .. '/'  .. SQLITE_LIB_NAME ..
				" -DWITH_MYSQL=YES " ..
				" -DWITH_MYSQL=ODBC " ..
				" -DWITH_ORACLE=NO " ..
				" -DWITH_POSTGRESQL=NO " ..
				" -DWITH_SQLITE3=YES " ..
				" -DWITH_BOOST=NO" ..
				" -DSOCI_TESTS=YES " ..
				" -DCMAKE_BUILD_TYPE=Debug " ..
				" " .. SOCI_SRC,
			"make",
			"make install"
		})
		
		
		-- build release mode
		batchexecute(sociInstallReleaseDir, {
			CMAKE ..
				" -G \"Unix Makefiles\"" ..
				" -DMYSQL_INCLUDE_DIR=" .. MYSQL_INCLUDE_DIR ..
				" -DMYSQL_LIBRARY=" .. MYSQL_LIB_DIR .. "/" .. MYSQL_LIB_NAME ..
				" -DCMAKE_INSTALL_PREFIX=" .. sociInstallReleaseDir ..
				" -DSQLITE3_INCLUDE_DIR=" .. SQLITE_INCLUDE_DIR ..
				" -DSQLITE3_LIBRARY=" .. normalizepath(SQLITE_LIB_DIR) .. '/'  .. SQLITE_LIB_NAME ..
				" -DWITH_MYSQL=YES " ..
				" -DWITH_MYSQL=ODBC " ..
				" -DWITH_ORACLE=NO " ..
				" -DWITH_POSTGRESQL=NO " ..
				" -DWITH_SQLITE3=YES " ..
				" -DWITH_BOOST=NO" ..
				" -DSOCI_TESTS=YES " ..
				" -DCMAKE_BUILD_TYPE=Release " ..
				" " .. SOCI_SRC,
			"make",
			"make install"
		});

		-- copy dynamic libs to appropriate output dir
		foundCount = 0;
		libs = os.matchfiles("lib/soci/triumph/Debug/lib/*.dylib");
		if #libs > 0 then
			os.execute("cp -r " .. os.getcwd() .. "/lib/soci/triumph/Debug/lib/*.dylib Debug/");
			foundCount = foundCount + 1;
		end
		libs = os.matchfiles("lib/soci/triumph/Release/lib/*.dylib");
		if #libs > 0 then
			os.execute("cp -r " .. os.getcwd() .. "/lib/soci/triumph/Release/lib/*.dylib Release/");
			foundCount = foundCount + 1;
		end
	else 
		print "Triumph does not support building SOCI on this operating system.\n"
	end
end

newaction {
	trigger = "soci",
	description = "Build the SOCI (database access) library",
	execute = prepSoci
}
