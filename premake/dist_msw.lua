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
-- Builds a package for MS Windows; just a plain a 7-zip archive with the
-- following structure;
--
-- ROOT
--  |
--	--> bin (holds triumph and all shared libraries)
--  |
--  --> assets (holds all images, php native functions db)
--
-- The package is a 7-zip archive because ZIP files with executables are labelled as
-- "potentially dangerous" by web browsers (Chrome), and Chrome won't download them.
-- The process of making the final archive is as follows:
--
-- 1. create a new directory to hold the final relase
-- 2. clone triumph into the new directory
-- 3. compile triumph
-- 4. copy the shared libraries into the final destination
-- 5. copy the assets into their final destination
-- 6. create the version file
-- 7. create the archive
--
newaction {
	trigger = "distmsw",
	description = "Build the Triumph distributable for MS Windows.",
	execute = function()
		tag = disttag()
		version = distversion()
		
		if (not tag or not version) then
			print("Cannot determine what branch or version to build. Please set them manually using the " ..
				"T4P_TAG and T4P_TAG_VERSION environment variables\n");
			os.exit(-1)
		end
		
		-- if we only want to print out the name of the 7zip file
		-- this is used by buildbot so that it knows the name of
		-- the package file to copy it out of the slave and to the
		-- master (because slaves are in EC2 and they are terminated)
		-- this is not a command line option because I don't like how
		-- premake deals with options (options cannot be tied to a 
		-- single action)
		onlyFilename = os.getenv('T4P_PKG_FILENAME_ONLY');
		if (onlyFilename == '1') then
			print(path.getabsolute("..\\triumph4php-" .. version .. "\\triumph4php-" .. version .. ".7z"))
			os.exit(0)
		end
		
		printf("creating MSW 7-zip archive for branch %s using version number %s\n", tag, version)

		destDir = normalizepath("..\\triumph4php-" .. version)
		
		-- MSW version, we just zip up the compiled executable
		-- the shared libs and assets
		-- rmdir fails on MSW if the directory does not exist, we want this
		-- script to continue if the dest dir does not exist
		if (os.isdir(destDir)) then
			batchexecute(normalizepath(""), {
				"rmdir /s /q " .. destDir
			});
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
			"SET T4P_ICU_DIR=" .. normalizepath(ICU_DIR),
			"SET T4P_WXWIDGETS_DIR=" .. normalizepath(WXWIDGETS_DIR), 
			"SET T4P_MYSQL_CONNECTOR_DIR=" .. normalizepath(MYSQL_CONNECTOR_DIR),
			"SET T4P_SQLITE_INCLUDE_DIR=" .. normalizepath(SQLITE_INCLUDE_DIR), 
			"SET T4P_SQLITE_LIB_DIR=" .. normalizepath(SQLITE_LIB_DIR),
			"SET T4P_CURL_RELEASE_DIR=" .. normalizepath(CURL_RELEASE_DIR), 
			"SET T4P_SOCI_INCLUDE_DIR=" .. normalizepath(SOCI_INCLUDE_DIR), 
			"SET T4P_SOCI_LIB_DIR=" .. normalizepath(SOCI_LIB_DIR),
			"SET T4P_BOOST_RELEASE_DIR=" .. normalizepath(BOOST_RELEASE_DIR),
			"premake4.exe vs2008",
			"\"" .. VSVARS .. "\"",
			"cd build\\vs2008",
			"vcbuild keybinder.vcproj \"Release|Win32\"",
			"vcbuild pelet.vcproj \"Release|Win32\"",
			"vcbuild triumph4php.vcproj \"Release|Win32\"",
		});
		
		if (os.isdir("..\\triumph4php-" .. version .. "\\dist")) then
			os.execute("rmdir /s /q " .. "..\\triumph4php-" .. version .. "\\dist")
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
			"xcopy /S /Y " .. normalizepath("Release\\boost*.dll") .. " dist\\triumph4php\\bin",
			
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
