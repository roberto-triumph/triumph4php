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
-- The distdeb action creates a RPM package of Triumph so that it can
-- be distributed and downloaded easily.  This action can only be run
-- on RedHat-based linux distributions as it requires that RPM packaging
-- tools be present. The RPM package will contain our executable plus
-- the wxWidgets libraries, as we use a fork of wxWidgets, and the version
-- of wxWidgets in the stable repos can be pretty old. Most other shared
-- libraries, like mysql, sqlite, curl will need to be in the system; the 
-- user will need to install them via yum.
--
-- The process to build a deb pretty complicated, below is a brief summary:
-- 1. create a new directory to start fresh in ~/rpmbuild/sources. This directory
--    is NOT configurable
-- 2. clone triumph into the new directory
-- 3. Copy the wxWigets and SOCI libraries from the dev directory into the
--    package directory, as we don't want to compile them on every release
--    since it would take tens of minutes to compile them.
-- 4. Create the version file  
-- 5. create the desktop and menu files, so that there are menu items
--    created for triumph. Copy the spec file as well. 
-- 6. create makefiles for triumph. This is different for releasing, as 
--    we need to point to the assets directory to the directory
--    where it will be located in the user's sytem (/usr/share)
-- 7. Run rpmbuild, it will build the final executable using our Makefile
--    and will package it.
newaction {
	trigger = "distrpm",
	description = "Build the Triumph distributable RPM package.",
	execute = function()
		
		tag = os.getenv('T4P_TAG');
		if (not tag) then
			print "Need to set the T4P_TAG environment variable to know which tag to package.\n";
			os.exit(-1)
		end
		versionNumber = tag;
		if tag == 'master' then
			versionNumber = '9.9.9'
		end
		
		--
		-- linux distribution: we package a .RPM file
		--
		cmd = "echo ~"
		cmdStream = io.popen(cmd)
		cmdOutput = cmdStream:read("*l")
		cmdStream:close()
		
		userRoot = cmdOutput
		workDir = userRoot .. "/rpmbuild/SOURCES/triumph4php-" ..tag
		desktopFile = userRoot .. "/rpmbuild/SPECS/triumph4php.desktop"
		finalLibDir = "/usr/lib64/triumph4php"
		rootDir = normalizepath("./")
		libWildcards =  normalizepath("./Release/*.so*")
		assetDir = "/usr/share/triumph4php"
		specFile = path.getabsolute("package/triumph4php.spec");
		specLinkFile = userRoot .. "/rpmbuild/SPECS/triumph4php.spec";
		
		batchexecute(rootDir, {
			string.format("mkdir -p \"%s\"", userRoot .. "/rpmbuild/SPECS"),
			string.format("rm -rf \"%s\"",  specLinkFile),
			string.format("ln -s \"%s\" \"%s\"", specFile, specLinkFile),
			string.format("sed -i 's/0.0.0/%s/g' %s", versionNumber, specLinkFile)
		});
		
		if (not os.isdir(workDir)) then
			batchexecute(rootDir, {
			
				-- clone the project into a new, temp directory
                string.format("rm -rf %s", workDir),
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
		
			string.format("git checkout %s", tag),
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
