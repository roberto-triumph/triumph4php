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
newaction {
	trigger = "distrpm",
	description = "Build the Triumph distributable RPM package.",
	execute = function()
		
		--
		-- linux distribution: we package a .RPM file
		--
		cmd = "echo ~"
		cmdStream = io.popen(cmd)
		cmdOutput = cmdStream:read("*l")
		cmdStream:close()
		
		userRoot = cmdOutput
		workDir = userRoot .. "/rpmbuild/SOURCES/triumph4php-0.4"
		desktopFile = userRoot .. "/rpmbuild/SPECS/triumph4php.desktop"
		finalLibDir = "/usr/lib64/triumph4php"
		rootDir = normalizepath("./")
		libWildcards =  normalizepath("./Release/*.so*")
		assetDir = "/usr/share/triumph4php"
		branch = "0.4.1";
		specFile = path.getabsolute("package/triumph4php.spec");
		specLinkFile = userRoot .. "/rpmbuild/SPECS/triumph4php.spec";
		
		os.execute(
			string.format("rm -rf \"%s\" && " ..
				"ln -s \"%s\" \"%s\"",
				specLinkFile, specFile, specLinkFile)
		);
		
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
				-- of the assets (in the system where the .rpm file will be installed
				-- ie the end user machine)
				"T4P_WXCONFIG=" .. normalizepath(WX_CONFIG) .. " " ..
				"T4P_SOCI_DEBUG_INCLUDE_DIR=" .. normalizepath(SOCI_DEBUG_INCLUDE_DIR) .. " " ..
				"T4P_SOCI_DEBUG_LIB_DIR=" .. normalizepath(SOCI_DEBUG_LIB_DIR)  .. " "  ..
				"T4P_SOCI_RELEASE_INCLUDE_DIR=" .. normalizepath(SOCI_RELEASE_INCLUDE_DIR) .. " " ..
				"T4P_SOCI_RELEASE_LIB_DIR=" .. normalizepath(SOCI_RELEASE_LIB_DIR) .. " " .. 
				"T4P_BUILD_SCRIPTS_DIR=. " .. 
				"T4P_LIB_DIR=" .. finalLibDir .. " " ..
				"T4P_ASSET_DIR=" .. assetDir .. " " ..
				"./premake4 gmake",
				
			});
		end

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
					
			-- create a new desktop file; so that fedora creates a 
			-- desktop icon 
			string.format("echo \"%s\" > \"%s\"", desktopItem, desktopFile),
		
			string.format("git checkout %s", branch),
			"git submodule init",
			"git submodule update lib/pelet",
			
			-- copy the compiled 3rd party dependencies into the work dir
			-- remove any libs that were removed since the last time
			-- this script ran
			string.format("install -d Release"),
			string.format("cp -r %s %s", os.getcwd() .. "/Release/*.so*", "Release/")
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
			"T4P_LIB_DIR=" .. finalLibDir .. " " ..
			"T4P_ASSET_DIR=" .. assetDir .. " " ..
			"./premake4 gmake",
			
			-- finally, create the .rpm this command will basically run our makefile
			-- gather all of the 3rd party libs and assets and package them
			"T4P_WXCONFIG=" .. normalizepath(WX_CONFIG) .. " " ..
			"T4P_SOCI_DEBUG_INCLUDE_DIR=" .. normalizepath(SOCI_DEBUG_INCLUDE_DIR) .. " " ..
			"T4P_SOCI_DEBUG_LIB_DIR=" .. normalizepath(SOCI_DEBUG_LIB_DIR)  .. " "  ..
			"T4P_SOCI_RELEASE_INCLUDE_DIR=" .. normalizepath(SOCI_RELEASE_INCLUDE_DIR) .. " " ..
			"T4P_SOCI_RELEASE_LIB_DIR=" .. normalizepath(SOCI_RELEASE_LIB_DIR) .. " " .. 
			"T4P_BUILD_SCRIPTS_DIR=. " .. 
			"T4P_LIB_DIR=" .. finalLibDir .. " " ..
			"T4P_ASSET_DIR=" .. assetDir .. " " ..
			"rpmbuild -bb " .. specFile
		});
	end
}
