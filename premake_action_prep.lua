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
	trigger = "prep",
	description = "Check to make sure all dependencies are installed. Also, copies the shared libs to the same location as the binary so that the binary can find them",
	execute = function ()
	
		-- create the directories if they don't exist yet
		if os.is "windows" then
			os.execute("mkdir Debug")
			os.execute("mkdir Release")
		elseif os.is "linux" then
			os.execute("mkdir -p Debug")
			os.execute("mkdir -p Release")
		end
		checkWxWidgets()
		checkIcu()
		checkCMake()
		checkSoci()
		checkMysql()
		checkSqlite()
		print "SUCCESS! All dependencies are met. Next step is to build MVC Editor in your environment.";
		if os.is "windows" then
			print "premake4.exe vs2008\n"
			print "Then open the Visual Studio solution in build\\vs2008\\mvc-editor.sln" 
		elseif os.is "linux" then
			print "./premake gmake\n  OR \n ./premake codelite"
		end
	end
}

-- makes sure that the WXWIN environment variable is defined
-- makes sure wxWidgets DLLs are placed in the same directory as mvc-editor.exe
function checkWxWidgets() 
	if os.is "windows" then
		dlls = os.matchfiles(WX_LIB_DIR .. "*.dll")
		if #dlls > 0 then
			batchexecute(normalizepath(""), {
				"xcopy /S /Y " .. normalizepath(WX_LIB_DIR .. "wxbase*ud_*.dll") .. " \"Debug\\\"",
				"xcopy /S /Y " .. normalizepath(WX_LIB_DIR .. "wxmsw*ud_*.dll") .. " \"Debug\\\"",
				"xcopy /S /Y " .. normalizepath(WX_LIB_DIR .. "wxbase*u_*.dll") .. " \"Release\\\"",
				"xcopy /S /Y " .. normalizepath(WX_LIB_DIR .. "wxmsw*u_*.dll") .. " \"Release\\\""
			});
		else 
			error("wxWidgets DLLs not found.  You need to build the wxWidgets library (Unicode Debug and Unicode Release configurations). " ..
					"Use the Visual Studio solution provided by wxWidgets to build the libraries \n" .. 
					"1) Intialize the wxWidgets submodule:  git submodule init lib\\wxWidgets \n" ..
					"2) Checkout the wxWidgets submodule:  git submodule update lib\\wxWidgets \n" ..
					"3) Open Solution wx_vc9.sln located lib\\wxWidgets\\build\msw directory\n" .. 
					"4) Select 'DLL Debug' from Configuration Manager\n" .. 
					"5) Build Entire Solution\n" .. 
					"6) Select 'DLL Release' from Configuration Manager\n" .. 
					"7) Build Entire Solution\n" ..
					"Note that this a fork of wxWidgets 2.9.5.  It has a couple of bug fixes. For more info, see https://github.com/robertop/wxWidgets")
		end
	elseif os.is "linux" then
		print(WX_CONFIG .. " --version")
		if 0 ~= os.execute(WX_CONFIG .. " --version") then
			error "Could not execute wx-config. Change the location of WX_CONFIG in premake_opts_linux.lua.\n"
		end

        -- copy wxWidgets libraries to the same dir as our executable
        -- use the wx-config to get the location of the libraries
        cmd = WX_CONFIG .. ' --prefix'
        cmdStream = io.popen(cmd)
        cmdOutput = cmdStream:read("*l")
        cmdStream:close()
        
        libDir = cmdOutput .. '/lib'
        batchexecute(normalizepath(""), {
            "cp -r " .. libDir .. "/*.so* Debug/",
            "cp -r " .. libDir .. "/*.so* Release/"
        });
	else
		error "You are running on a non-supported operating system. MVC Editor cannot be built.\n"
	end
end


-- makes sure that the ICU DLLs are found in the configured location
-- makes sure ICU  DLLs are placed in the same directory as mvc-editor.exe
function checkIcu() 
	if os.is "windows" then
		icuDllPath = normalizepath(ICU_LIB_DIR .. "../bin")
		dlls = os.matchfiles(ICU_LIB_DIR .. "/../bin/*.dll")
		if #dlls > 0 then
			batchexecute(normalizepath(""), {
					"xcopy /S /Y " .. normalizepath(ICU_LIB_DIR .. "../bin/*42d.dll") .. " \"Debug\\\"",
					"xcopy /S /Y " .. normalizepath(ICU_LIB_DIR .. "../bin/*41d.dll") .. " \"Debug\\\"",
					"xcopy /S /Y " .. normalizepath(ICU_LIB_DIR .. "../bin/icudt42.dll") .. " \"Debug\\\"",
					"xcopy /S /Y " .. normalizepath(ICU_LIB_DIR .. "../bin/*42.dll") .. " \"Release\\\"",
					"xcopy /S /Y " .. normalizepath(ICU_LIB_DIR .. "../bin/*41d.dll") .. " \"Release\\\""
				});
		else 
			error ("ICU DLLs not found in " .. icuDllPath .. ".  Either build the ICU library " ..
				"\nor set the MVCEDITOR_ICU_DIR environment variable to the location of ICU." ..
				"To build the ICU library:" ..
				"\nExtract the ICU library into " .. normalizepath(ICU_LIB_DIR .. "../") ..
				"open the following solution file in Visual Studio: " .. normalizepath(ICU_LIB_DIR .. "../source/allinone/allinone.sln") ..
				"\nChoose Debug|Win32 Build. Go to Build .. Rebuild Solution" ..
				"\nChoose Release|Win32 Build. Go to Build .. Rebuild Solution" ..
				"\n(Note: Batch Build \"Select All\" won't work on Visual Studio 2008 Express Edition because it does not support 64 bit compilation)")
		end
	elseif os.is "linux" then
		print(ICU_CONFIG .. " --exists")
		if 0 ~= os.execute(ICU_CONFIG .. " --exists") then
			error "Could not execute icu-config. Change the location of ICU_CONFIG in premake_opts_linux.lua.\n"
		end
	else 
		error "You are running on a non-supported operating system. MVC Editor cannot be built.\n"
	end
end

function checkCMake() 

end

-- makes sure that the SOCI has been built
-- makes sure SOCI DLLs are placed in the same directory as mvc-editor.exe
function checkSoci() 
	if os.is "windows" then
		SOCI_SRC = normalizepath("lib/soci/src")
		
		-- grab the libraries from the build directory.  the ones from
		-- from the install directory (lib/soci/mvc-editor) do not work (programs 
		-- that use them crash)
		dlls = os.matchfiles("lib/soci/src/bin/Debug/*.dll")
		if #dlls > 0 then
			sociLibPath = normalizepath("lib/soci/src/bin/Debug/*.dll")
			batchexecute(normalizepath(""), {
				"xcopy /S /Y " .. sociLibPath .. " \"Debug\\\""
			});
		else 
			print "SOCI Debug libraries have not been built. You need to build the SOCI library in Debug configuration. "
			print ("Open the solution found at " .. SOCI_SRC .. "\\SOCI.sln")
			print "Build the solution using the Debug configuration."
			error ""
		end
		
		dlls = os.matchfiles("lib/soci/src/bin/Release/*.dll")
		if #dlls > 0 then
			sociLibPath = normalizepath("lib/soci/src/bin/Release/*.dll")
			batchexecute(normalizepath(""), {
				"xcopy /S /Y " .. sociLibPath .. " \"Release\\\""
			});
			print(cmd)
			os.execute(cmd)
		else 
			print "SOCI Release libraries have not been built. You need to build the SOCI library in Release configuration. "
			print ("Open the solution found at " .. SOCI_SRC .. "\\SOCI.sln")
			print "Build the solution using the Release configuration"
			error ""
		end	
	elseif os.is "linux" then
		
		-- soci lib dirs are named according to architecture
		if os.isdir "lib/soci/mvc-editor/lib64" then
			libs = os.matchfiles("lib/soci/mvc-editor/lib64/*.so*");
			if #libs > 0 then
				os.execute("cp -r " .. os.getcwd() .. "/lib/soci/mvc-editor/lib64/*.so* Debug/");
				os.execute("cp -r " .. os.getcwd() .. "/lib/soci/mvc-editor/lib64/*.so* Release/");
			else 
				error "SOCI library has not been built.  Execute the premake soci action: ./premake4 soci"
			end
		else
			libs = os.matchfiles("lib/soci/mvc-editor/lib/*.so*");
			if #libs > 0 then
				os.execute("cp -r " .. os.getcwd() .. "/lib/soci/mvc-editor/lib/*.so* Debug/");
				os.execute("cp -r " .. os.getcwd() .. "/lib/soci/mvc-editor/lib/*.so* Release/");
			else
				error "SOCI library has not been built.  Execute the premake soci action: ./premake4 soci"
			end
		end
	else 
		error "You are running on a non-supported operating system. MVC Editor cannot be built.\n"
	end
end

-- makes sure that the MySQL Connector DLLs have been extracted
-- makes sure MySQL Connector DLLs are placed in the same directory as mvc-editor.exe
function checkMysql() 
	if os.is "windows" then
		dlls = os.matchfiles(MYSQL_LIB_DIR .. "*.dll")
		if  #dlls > 0 then
			mysqlLibPath = normalizepath(MYSQL_LIB_DIR .. "*.dll")
			cmd = "xcopy /S /Y " .. mysqlLibPath  .. " \"Debug\\\""
			print(cmd)
			os.execute(cmd)
			
			cmd = "xcopy /S /Y " .. mysqlLibPath  .. " \"Release\\\""
			print(cmd)
			os.execute(cmd)
		else 
			error ("MySQL libraries not found in " ..  MYSQL_LIB_DIR ..
				"\nPlease download the MySQL Connector/C from http://dev.mysql.com/downloads/connector/c/\n" ..
				"and extract to " .. MYSQL_LIB_DIR .. 
				"\nOR" ..
				"set the environment variable MVCEDITOR_MYSQL_CONNECTOR_DIR to location of MySQL connector.")
		end
	elseif os.is "linux" then
		mysqlLib = MYSQL_LIB_DIR .. MYSQL_LIB_NAME
		cmd = "ls " .. mysqlLib
		print(cmd)
		found = os.execute(cmd)
		if 0 ~= found  then
			error ("MySQL libraries not found (" .. mysqlLib .. 
				"). Please install the MySQL C client library, or change the location of " ..
				" MYSQL_LIB_DIR in premake_opts_linux.lua")
		end
	else 
		error "You are running on a non-supported operating system. MVC Editor cannot be built.\n"
	end
end

-- makes sure that the SQLite3 DLLs have been extracted
-- makes sure SQLite3 DLLs are placed in the same directory as mvc-editor.exe
function checkSqlite() 
	if os.is "windows" then
		dlls = os.matchfiles(SQLITE_LIB_DIR .. "/*.dll")
		if  #dlls > 0 then
			sqliteLibPath = normalizepath(SQLITE_LIB_DIR .. "/*.dll")
			cmd = "xcopy /S /Y " .. sqliteLibPath .. " \"Debug\\\""
			print(cmd)
			os.execute(cmd)
			
			cmd = "xcopy /S /Y " .. sqliteLibPath .. " \"Release\\\""
			print(cmd)
			os.execute(cmd)
		else 
			error ("SQLite libraries not found in " ..  SQLITE_LIB_DIR ..
				"\nPlease download the SQLite3 DLLs from http://sqlite.org/download.html\n" ..
				"and extract to " .. SQLITE_LIB_DIR ..
				"\nOR set the environment variable MVCEDITOR_SQLITE_DIR to the location" .. 
				"of the SQLITE files.  See lib/sqlite/README for more info.")
		end
	elseif os.is "linux" then
		sqliteLib = SQLITE_LIB_DIR .. SQLITE_LIB_NAME
		cmd = "ls " .. sqliteLib
		print(cmd)
		found = os.execute(cmd)
		if 0 ~= found  then
			error ("SQLite libraries not found (" .. sqliteLib .. 
				"). Please install the SQLite3 client library, or change the location of " ..
				" SQLITE_LIB_DIR in premake_opts_linux.lua")
		end
	else 
		error "You are running on a non-supported operating system. MVC Editor cannot be built.\n"
	end
end
