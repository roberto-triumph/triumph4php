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
	trigger = "dist",
	description = "Build the MVC Editor distributable.",
	execute = function()
		if os.is "windows" then
			if os.isdir("dist") then
				os.execute("rmdir /s /q dist")
			end
			batchexecute(normalizepath(""), {
				"mkdir dist",
				"mkdir dist\\mvc-editor\\bin",
				"mkdir dist\\mvc-editor\\assets",
				"xcopy /S /Y Release\\*.dll dist\\mvc-editor\\bin",
				"copy Release\\mvc-editor.exe dist\\mvc-editor\\bin",
				"xcopy  /S /Y assets\\* dist\\mvc-editor\\assets"
			});
			
			-- get the version info from git and populate the version file
			-- if we have no tags yet, use the -all flag
			cmd = "git describe --long > dist\\mvc-editor\\assets\\version.txt"
			if 0 ~= os.execute(cmd) then
				cmd = "git describe --all --long > dist\\mvc-editor\\assets\\version.txt"
				os.execute(cmd) 
			end
		else
			workDir = normalizepath("../mvc-editor-0.6")
			workLibDir = normalizepath("../mvc-editor-0.6/Release/libs")
			rootDir = normalizepath("./")
			libWildcards =  normalizepath("./Release/*.so*")
			assetDir = "/usr/share/mvc-editor/assets"
			branch = "master";
			
			os.execute(string.format("rm -rf %s", workDir))
						
			batchexecute(rootDir, {
			
				-- clone the project into a new, temp directory
				string.format("git clone . %s", workDir),
				string.format("cd %s", workDir),
				string.format("git checkout %s", branch),
				
				
				-- copy the compiled 3rd party dependencies into the work dir
				"install -d Release/libs",
				string.format("cp -r %s %s", os.getcwd() .. "/Release/*.so*", "Release/libs"),
				
				"git submodule init",
				"git submodule update lib/pelet"
			})
			
			-- get the version info from git and populate the version file
			-- if we have no tags yet, use the -all flag
			cmd = "cd " .. workDir .. " && git describe --long > version.txt"
			if 0 ~= os.execute(cmd) then
				cmd = "cd " .. workDir .. " && git describe --all --long > version.txt"
				os.execute(cmd) 
			end


			batchexecute(workDir, {
			
				-- create the makefiles, which will be used to build
				-- the release version of the project
				-- basically, this command makes the work directory use
				-- the wxWidgets, SOCI installation in the main project dir (which
				-- are already compiled, the work dir does not have them and we
				-- want to avoid re-compiling them)
				-- also, we generate the Makefiles in the root of the project
				-- also, we want to set the assets dir to the final location
				-- of the assets (in the system where the .deb file will be installed
				-- ie the end user machine)
				"MVCEDITOR_WXCONFIG=" .. normalizepath(WX_CONFIG) .. " " ..
				"MVCEDITOR_SOCI_DEBUG_INCLUDE_DIR=" .. normalizepath(SOCI_DEBUG_INCLUDE_DIR) .. " " ..
				"MVCEDITOR_SOCI_DEBUG_LIB_DIR=" .. normalizepath(SOCI_DEBUG_LIB_DIR)  .. " "  ..
				"MVCEDITOR_SOCI_RELEASE_INCLUDE_DIR=" .. normalizepath(SOCI_RELEASE_INCLUDE_DIR) .. " " ..
				"MVCEDITOR_SOCI_RELEASE_LIB_DIR=" .. normalizepath(SOCI_RELEASE_LIB_DIR) .. " " .. 
				"MVCEDITOR_BUILD_SCRIPTS_DIR=. " .. 
				"MVCEDITOR_LIB_DIR=" .. workLibDir .. " " ..
				"MVCEDITOR_ASSET_DIR=" .. assetDir .. " " ..
				"./premake4 gmake",
				
				-- this will prep the dir to be a debian work dir
				"dh_make -s --email roberto@mvceditor.com  --native",

				-- populate the install file, the file that tells which files to 
				-- include in the .deb file
				-- double quote the backslash since we have to escape the backslash in the shell
				"echo 'Release/mvc-editor usr/bin\\n' > debian/install",
				"find Release/libs -type f -name \"*\\\\.so*\" | awk '{ print $1  \" usr/lib/mvc-editor\" }' >> debian/install",

				-- regex is complicated
				-- find the basename from the asset file, then remove it
				-- for example, from  "assets/icons/database-delete.png"
				-- we want to end up with the line 
				-- "assets/icons/database-delete.png usr/share/mvc-editor/icons"
				-- double quote the backslash since we have to escape the backslash in the shell
				"find assets/  -type f -name \"*\" -printf '%p /%h\n' | sed 's/\\/assets/usr\\/share\\/mvc-editor/g' >> debian/install",

				-- mofiy the makefile to only build the main app not all of the examples, profilers, 
				"sed -i 's/all: \$(PROJECTS)/all: mvc-editor/g' Makefile",
				
				-- finally, create the .deb this command will basically run our makefile
				-- gather all of the 3rd party libs and assets and package them
				-- we need to set the LD_LIBRAR_PATH so that dpkg finds our dependencies
				-- -b means we create the binary only package, also -us -uc
				-- creates an unsigned version
				"LD_LIBRARY_PATH=$LD_LIBRARY_PATH:Release/libs:Release dpkg-buildpackage -b -us -uc"
			});
		end	
	end
}
