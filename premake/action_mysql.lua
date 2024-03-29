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
-- Prepare the MySQL connection (client) library
-- Triumph uses the MySQL client library so that the user can explore database tables
-- and write ad-hoc queries against MySQL databases.
-- 
-- On MSW, MySQL C connector binary is fetched from the MySQL dev site. On linux 
-- or Mac OS X, the developer is told to install it via a package manager.
--
-- See http://dev.mysql.com/downloads/connector/c/
--
function prepMysql()
	
	if os.is "windows" then
	
		mysqlZip = "lib/mysql-connector-c-noinstall-6.0.2-win32.zip";
		mysqlDownload = "http://dev.mysql.com/get/Downloads/Connector-C/mysql-connector-c-noinstall-6.0.2-win32.zip"
		extractedDir = 'lib/mysql-connector-c-noinstall-6.0.2-win32'
		existenceOrDownloadExtract(mysqlZip, extractedDir, mysqlDownload, "Downloading libMySQL dependency");
		mysqlLibPath = normalizepath(MYSQL_LIB_DIR .. "*.dll")
		batchexecute(normalizepath(""), {
			"xcopy /S /Y " .. mysqlLibPath  .. " \"Debug\\\"",
			"xcopy /S /Y " .. mysqlLibPath  .. " \"Release\\\""
		})
	elseif os.is "linux" then  
	
		-- MYSQL_LIB_DIR is already the result of a os.searchpath
		-- which searched the default locations for the mysql library
		mysqlLi = MYSQL_LIB_DIR
		if mysqlLi == nil then
			error (
				"MySQL client libraries not found.  " .. 
				"Please install the MySQL client library, or change the location of \n" ..
				"MYSQL_LIB_DIR in premake_opts_linux.lua.\n" ..
				"You can install mysql client via your package manager; ie. sudo apt-get install libmysqlclient-dev\n"
			)
		end
	elseif os.is "macosx" then  
	
		-- MYSQL_LIB_DIR is already the result of a os.searchpath
		-- which searched the default locations for the mysql library
		mysqlLi = MYSQL_LIB_DIR
		if mysqlLi == nil then
			error (
				"MySQL client libraries not found.  " .. 
				"Please install the MySQL client library, or change the location of \n" ..
				"MYSQL_LIB_DIR in premake_opts_linux.lua.\n" ..
				"You can install mysql client via a package manager; ie. sudo brew install mysql-connector-c\n"
			)
		end
	else 
		print "Triumph does not support building MySQL on this operating system.\n"
	end
end

newaction {
	trigger = "mysql",
	description = "Fetch the MySQL client library or check for its existence",
	execute = prepMysql
}