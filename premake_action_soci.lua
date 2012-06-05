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

newaction {
	trigger = "soci",
	description = "Build the SOCI (database access) library",
	execute = function()
		batchexecute(os.getcwd(), { string.format("%s --version", CMAKE) }, "cmake not found. Compiling SOCI requires CMake. SOCI cannot be built.")
		if os.is "windows" then
		
			-- on windows, use MSYS Git which may not be in the path
			SOCI_BUILD_DIR =  normalizepath("lib/soci/mvc-editor/")
			SOCI_ROOT = "lib\\soci"
			SOCI_SRC = "lib\\soci\\src"
			GIT_ROOT = os.getenv("USERPROFILE") .. "\\software\\Git\\bin"
			GIT = GIT_ROOT .. "\\git.exe"
			TAR = GIT_ROOT .. "\\tar.exe"
		else 
			SOCI_BUILD_DIR =  normalizepath("lib/soci/mvc-editor/")
			SOCI_ROOT = "lib/soci"
			SOCI_SRC = "lib/soci/src/"
			GIT = "git"
			TAR = "tar"
		end
		if os.is "windows" then
			
			-- extract the C (binary) library
			existence "lib/mysql-connector-c-noinstall-6.0.2-win32.zip"
			batchexecute(os.getcwd(), {
				"winrar x lib\\mysql-connector-c-noinstall-6.0.2-win32.zip lib\\"
			})
		else
			-- get from the system 
		end
		
		-- always get  a specific version so that we are not developing against a moving target
		batchexecute(os.getcwd(), { "mkdir " .. SOCI_ROOT })
		batchexecute(SOCI_ROOT, {
			GIT .. " archive" ..
			" --format=tar" ..
			" --remote git://soci.git.sourceforge.net/gitroot/soci/soci acba467dce5c6f4df618f3a816b9f9fc56bd6543" .. 
			"| " .. TAR .. " -xf - "
		}, "Failed to fetch SOCI source code from the remote Git repo. Do you have network connectivity?")
		if os.is "windows" then
			
			-- exclude SOCI from linking against Boost. we don't use it
			-- generate a solution file
			batchexecute(SOCI_SRC, {
				CMAKE ..
					" -G \"Visual Studio 9 2008\"" ..
					" -DMYSQL_INCLUDE_DIR=" .. normalizepath(MYSQL_INCLUDE_DIR) ..
					" -DMYSQL_LIBRARY=" .. normalizepath(MYSQL_LIB_DIR .. '/libmysql.lib') ..
					" -DCMAKE_INSTALL_PREFIX=" .. SOCI_BUILD_DIR .. 
					" -DWITH_MYSQL=YES " ..
					" -DWITH_ODBC=NO " ..
					" -DWITH_ORACLE=NO " ..
					" -DWITH_POSTGRESQL=NO " ..
					" -DWITH_SQLITE3=NO " ..
					" -DWITH_BOOST=NO"
			})
			print "Check the output above.  If it reads \"SOCI_MYSQL = OFF\" then you will need to do some investigation."
			print "Otherwise, you will now need to open the generated solution file and build it from there."
			print ("Open the solution found at " .. SOCI_SRC .. "\\SOCI.sln")
			print "Build the solution in Debug configuration"
			print "Build the solution in Release configuration"
		else 

			-- now build SOCI
			-- exclude SOCI from linking against Boost. we don't use it
			batchexecute(SOCI_SRC, {
				CMAKE ..
					" -G \"Unix Makefiles\"" ..
					" -DMYSQL_INCLUDE_DIR=" .. MYSQL_INCLUDE_DIR ..
					" -DMYSQL_LIBRARY=" .. MYSQL_LIB_DIR .. "/" .. MYSQL_LIB_NAME ..
					" -DCMAKE_INSTALL_PREFIX=" .. SOCI_BUILD_DIR .. 
					" -DWITH_MYSQL=YES " ..
					" -DWITH_MYSQL=ODBC " ..
					" -DWITH_ORACLE=NO " ..
					" -DWITH_POSTGRESQL=NO " ..
					" -DWITH_SQLITE3=NO " ..
					" -DWITH_BOOST=NO",
				"make",
				"make install"
			})
		end
	end
}
