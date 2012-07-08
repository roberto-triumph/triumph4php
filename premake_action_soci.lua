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
		 SOCI_BUILD_DIR =  normalizepath("lib/soci/mvc-editor")
		 SOCI_ROOT = normalizepath("lib/soci")
		 SOCI_SRC = normalizepath("lib/soci/src")
		if os.is "windows" then
			
			-- exclude SOCI from linking against Boost. we don't use it
			-- generate a solution file
			batchexecute(SOCI_SRC, {
				CMAKE ..
					" -G \"Visual Studio 9 2008\"" ..
					" -DMYSQL_INCLUDE_DIR=" .. normalizepath(MYSQL_INCLUDE_DIR) ..
					" -DMYSQL_LIBRARY=" .. normalizepath(MYSQL_LIB) ..
					" -DCMAKE_INSTALL_PREFIX=" .. SOCI_BUILD_DIR .. 
					" -DSQLITE3_INCLUDE_DIR=" .. normalizepath(SQLITE_INCLUDE_DIR) ..
					" -DSQLITE3_LIBRARY=" .. normalizepath(SQLITE_LIB) ..
					" -DWITH_MYSQL=YES " ..
					" -DWITH_ODBC=NO " ..
					" -DWITH_ORACLE=NO " ..
					" -DWITH_POSTGRESQL=NO " ..
					" -DWITH_SQLITE3=YES " ..
					" -DWITH_BOOST=NO " ..
					" -DSOCI_TESTS=NO "
			})
			print "Check the output above.  If it reads \"SOCI_MYSQL = OFF\" or \"SOCI_SQLITE3 = OFF\" then you will need to do some investigation."
			print "Otherwise, you will now need to open the generated solution file and build it from there."
			print ("Open the solution found at " .. normalizepath("lib/soci/src/SOCI.sln"))
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
					" -DSQLITE3_INCLUDE_DIR=" .. SQLITE_INCLUDE_DIR ..
					" -DSQLITE3_LIBRARIES=" .. normalizepath(SQLITE_LIB_DIR) ..
					" -DWITH_MYSQL=YES " ..
					" -DWITH_MYSQL=ODBC " ..
					" -DWITH_ORACLE=NO " ..
					" -DWITH_POSTGRESQL=NO " ..
					" -DWITH_SQLITE3=YES " ..
					" -DWITH_BOOST=NO",
				"make",
				"make install"
			})
		end
	end
}
