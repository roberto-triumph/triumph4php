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
	description = "Build the Triumph distributable.",
	execute = function()
		if os.is "windows" then
			
			--
			-- MSW version, we just zip up the compiled executable
			-- the shared libs and assets
			--
			if os.isdir("dist") then
				os.execute("rmdir /s /q dist")
			end
			batchexecute(normalizepath(""), {
				"mkdir dist",
				"mkdir dist\\triumph4php",
				"mkdir dist\\triumph4php\\bin",
				"mkdir dist\\triumph4php\\assets",
				"xcopy /S /Y Release\\*.dll dist\\triumph4php\\bin",
				"copy Release\\triumph4php.exe dist\\triumph4php\\bin",
				"xcopy  /S /Y assets\\* dist\\triumph4php\\assets"
			});
			
			-- get the version info from git and populate the version file
			-- if we have no tags yet, use the -all flag
			cmd = "git describe --long > dist\\triumph4php\\assets\\version.txt"
			if 0 ~= os.execute(cmd) then
				cmd = "git describe --all --long > dist\\triumph4php\\assets\\version.txt"
				os.execute(cmd) 
			end
		else
		
			--
			-- linux distribution: we package a .DEB file
			--
			--
			workDir = path.getabsolute("../triumph4php-0.4")
			workLibDir = path.getabsolute("../triumph4php-0.4/Release/libs")
			rootDir = normalizepath("./")
			libWildcards =  normalizepath("./Release/*.so*")
			assetDir = "/usr/share/triumph4php"
			branch = "master";
			debianControl = path.getabsolute("../debian.control");
			
			if (not os.isdir(workDir)) then
				batchexecute(rootDir, {
				
					-- clone the project into a new, temp directory
					string.format("git clone . %s", workDir),
					string.format("cd %s", workDir),
					"git submodule init",
					"git submodule update lib/pelet",
					
						
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
					"T4P_WXCONFIG=" .. normalizepath(WX_CONFIG) .. " " ..
					"T4P_SOCI_DEBUG_INCLUDE_DIR=" .. normalizepath(SOCI_DEBUG_INCLUDE_DIR) .. " " ..
					"T4P_SOCI_DEBUG_LIB_DIR=" .. normalizepath(SOCI_DEBUG_LIB_DIR)  .. " "  ..
					"T4P_SOCI_RELEASE_INCLUDE_DIR=" .. normalizepath(SOCI_RELEASE_INCLUDE_DIR) .. " " ..
					"T4P_SOCI_RELEASE_LIB_DIR=" .. normalizepath(SOCI_RELEASE_LIB_DIR) .. " " .. 
					"T4P_BUILD_SCRIPTS_DIR=. " .. 
					"T4P_LIB_DIR=" .. workLibDir .. " " ..
					"T4P_ASSET_DIR=" .. assetDir .. " " ..
					"./premake4 gmake",
					
					-- this will prep the dir to be a debian work dir
					"dh_make -s --email roberto@triumph4php.com  --native",
					
					-- remove any example files put there by dh_make
					 "rm -rf debian/*.ex",
					 "rm -rf debian/*.EX",
				});
			end
			
			menuItem = "?package(triumph4php):needs=\"X11|text|vc|wm\" " ..
				"section=\"Applications/Programming\" " ..
				"title=\"triumph4php\" " ..
				"command=\"/usr/bin/triumph4php\" " ..
				"icon=\"/usr/share/triumph4php/icons/triumph4php.png\"";
			desktopItem = "[Desktop Entry]\n" ..
				"Encoding=UTF-8\n" ..
				"Name=triumph4php\n" ..
				"Exec=triumph4php %f\n" ..
				"Icon=/usr/share/triumph4php/icons/triumph4php.png\n" ..
				"Terminal=false\n" ..
				"Type=Application\n" ..
				"Categories=Development;\n" ..
				"StartupNotify=true\n"

			batchexecute(workDir, {
			
				-- add the menu file to add a menu to the system bar
				"echo '" .. menuItem .. "' > debian/menu",
				
				-- create a new desktop file; so that ubuntu creates a 
				-- desktop icon 
				string.format("echo \"%s\" > triumph4php.desktop", desktopItem),
			
				string.format("git checkout %s", branch),
				"git submodule init",
				"git submodule update lib/pelet",
				
				-- copy the compiled 3rd party dependencies into the work dir
				-- remove any libs that were removed since the last time
				-- this script ran
				"install -d Release/libs",
				"rm -fr Release/libs/*",
				string.format("cp -r %s %s", os.getcwd() .. "/Release/*.so*", "Release/libs")
			})
			
			-- get the version info from git and populate the version file
			-- if we have no tags yet, use the -all flag
			cmd = "cd " .. workDir .. " && git describe --long > assets/version.txt"
			if 0 ~= os.execute(cmd) then
				cmd = "cd " .. workDir .. " && git describe --all --long > assets/version.txt"
				os.execute(cmd) 
			end


			batchexecute(workDir, {
				"T4P_WXCONFIG=" .. normalizepath(WX_CONFIG) .. " " ..
				"T4P_SOCI_DEBUG_INCLUDE_DIR=" .. normalizepath(SOCI_DEBUG_INCLUDE_DIR) .. " " ..
				"T4P_SOCI_DEBUG_LIB_DIR=" .. normalizepath(SOCI_DEBUG_LIB_DIR)  .. " "  ..
				"T4P_SOCI_RELEASE_INCLUDE_DIR=" .. normalizepath(SOCI_RELEASE_INCLUDE_DIR) .. " " ..
				"T4P_SOCI_RELEASE_LIB_DIR=" .. normalizepath(SOCI_RELEASE_LIB_DIR) .. " " .. 
				"T4P_BUILD_SCRIPTS_DIR=. " .. 
				"T4P_LIB_DIR=" .. workLibDir .. " " ..
				"T4P_ASSET_DIR=" .. assetDir .. " " ..
				"./premake4 gmake",

				-- populate the install file, the file that tells which files to 
				-- include in the .deb file
				-- double quote the backslash since we have to escape the backslash in the shell
				"echo 'Release/triumph4php usr/bin\\n' > debian/install",
				
				"find Release/libs -type f -name \"*\\\\.so*\" | awk '{ print $1  \" usr/lib/triumph4php\" }' >> debian/install",
				"find Release/libs -type l -name \"*\\\\.so*\" | awk '{ print $1  \" usr/lib/triumph4php\" }' >> debian/install",

				-- regex is complicated
				-- find the basename from the asset file, then remove it
				-- for example, from  "assets/icons/database-delete.png"
				-- we want to end up with the line 
				-- "assets/icons/database-delete.png usr/share/triumph4php/icons"
				-- double quote the backslash since we have to escape the backslash in the shell
				"find assets/  -type f -name \"*\" -printf '%p /%h\n' | sed 's/\\/assets/usr\\/share\\/triumph4php/g' >> debian/install",

				-- mofiy the makefile to only build the main app not all of the examples, profilers, 
				"sed -i 's/all: \$(PROJECTS)/all: triumph4php/g' Makefile",
				
				-- modify the license file to be our own
				"cat LICENSE > debian/copyright",
				
				-- remove unused debian package files
				"rm -f debian/README.source",
				"rm -f debian/README.debian",
				
				-- copy the debian control file, it gives the debian package
				-- a description of our project
				string.format("cat \"%s\" > debian/control", debianControl),
				
				-- make the desktop file install into the user's applitcations directory
				string.format("echo \"%s\" >> %s", "\ntriumph4php.desktop usr/share/applications\n", "debian/install"),
				
				
				
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
