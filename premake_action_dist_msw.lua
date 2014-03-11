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
	trigger = "distmsw",
	description = "Build the Triumph distributable for MS Windows.",
	execute = function()
		tag = os.getenv('T4P_TAG');
		if (not tag) then
			print "Need to set the T4P_TAG environment variable to know which tag to package.\n";
			os.exit(-1)
		end
		destDir = normalizepath("..\\triumph4php-" .. tag)
		
		-- MSW version, we just zip up the compiled executable
		-- the shared libs and assets
		--
		if os.isdir(destDir) then
			os.execute("rmdir /s /q " .. destDir)
		end
		batchexecute(normalizepath(""), {
			string.format("\"%s\" clone %s %s", GIT, ".", destDir),
			string.format("cd %s", destDir),
			string.format("\"%s\" checkout %s", GIT, tag),
			string.format("\"%s\" submodule init", GIT),
			string.format("\"%s\" submodule update lib/pelet", GIT)
		});
		
		-- next we create the make file, pointing the soci and 
		-- wxwidgets locations to inside this project, so that we dont
		-- have to re-compile curl, wxWidgets, and SOCI every time we
		-- release
		batchexecute(destDir, {
			"SET T4P_ICU_DIR=" .. "..\\triumph4php\\" .. ICU_DIR,
			"SET T4P_WXWIDGETS_DIR=" .. "..\\triumph4php\\" .. WXWIDGETS_DIR, 
			"SET T4P_MYSQL_CONNECTOR_DIR=" .. "..\\triumph4php\\" .. MYSQL_CONNECTOR_DIR,
			"SET T4P_SQLITE_INCLUDE_DIR=" .. "..\\triumph4php\\" .. SQLITE_INCLUDE_DIR, 
			"SET T4P_SQLITE_LIB_DIR=" .. "..\\triumph4php\\" .. SQLITE_LIB_DIR,
			"SET T4P_CURL_RELEASE_DIR=" .. "..\\triumph4php\\" .. CURL_RELEASE_DIR, 
			"SET T4P_SOCI_INCLUDE_DIR=" .. "..\\triumph4php\\" .. SOCI_INCLUDE_DIR, 
			"SET T4P_SOCI_LIB_DIR=" .. "..\\triumph4php\\" .. SOCI_LIB_DIR,
			"premake4.exe vs2008",
			"\"" .. VSVARS .. "\"",
			"cd build\\vs2008",
			"vcbuild keybinder.vcproj \"Release|Win32\"",
			"vcbuild pelet.vcproj \"Release|Win32\"",
			"vcbuild triumph4php.vcproj \"Release|Win32\"",
		});
		
		if (os.isdir("..\\triumph4php-" .. tag .. "\\dist")) then
			os.execute("rmdir /s /q " .. "..\\triumph4php-" .. tag .. "\\dist")
		end
		batchexecute(destDir, {
			"mkdir dist\\triumph4php\\bin",
			"mkdir dist\\triumph4php\\assets",
			
			-- careful, dont copy over pelet from this dir, leave the one we just compiled
			"xcopy /S /Y " .. normalizepath("Release\\wx*.dll") .. " dist\\triumph4php\\bin",
			"xcopy /S /Y " .. normalizepath("Release\\icu*.dll") .. " dist\\triumph4php\\bin",
			"xcopy /S /Y " .. normalizepath("Release\\libcurl*.dll") .. " dist\\triumph4php\\bin",
			"xcopy /S /Y " .. normalizepath("Release\\libmysql*.dll") .. " dist\\triumph4php\\bin",
			"xcopy /S /Y " .. normalizepath("Release\\soci*.dll") .. " dist\\triumph4php\\bin",
			"xcopy /S /Y " .. normalizepath("Release\\sqlite*.dll") .. " dist\\triumph4php\\bin",
			
			-- copy over keybinder, pelet that we just compiled
			"xcopy /S /Y " .. "Release\\*.dll  dist\\triumph4php\\bin",
			"copy Release\\triumph4php.exe dist\\triumph4php\\bin",
			"xcopy  /S /Y assets\\* dist\\triumph4php\\assets"
		});
		
		-- get the version info from git and populate the version file
		-- if we have no tags yet, use the -all flag
		batchexecute(destDir, {
			string.format("\"%s\" describe --long > dist\\triumph4php\\assets\\version.txt", GIT),
			"cd dist",
			string.format("%s a triumph4php-%s.7z triumph4php\\*", SEVENZIP, tag)
		})
	end
}
