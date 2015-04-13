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
-- The distdeb action creates a DEBian package of Triumph so that it can
-- be distributed and downloaded easily.  This action can only be run
-- on Debian-based linux distributions as it requires that debian packaging
-- tools be present. The DEB package will contain our executable plus
-- the wxWidgets libraries, as we use a fork of wxWidgets, and the version
-- of wxWidgets in the stable repos can be pretty old. Most other shared
-- libraries, like mysql, sqlite, curl will need to be in the system; the 
-- user will need to install them via apt-get.
--
-- The process to build a deb pretty complicated, below is a brief summary:
-- 1. create a new directory to start fresh
-- 2. clone triumph into the new directory
-- 3. create makefiles for triumph. This is different for releasing, as 
--    we need to point to the assets directory to the directory
--    where it will be located in the user's sytem (/usr/share)
-- 4. Copy the wxWigets and SOCI libraries from the dev directory into the
--    package directory, as we don't want to compile them on every release
--    since it would take tens of minutes to compile them.
-- 5. run dh_make
-- 6. create the desktop and menu files, so that there are menu items
--    created for triumph 
-- 7. create the install file; this is the file that lists all files
--    that need to be in the DEB archive.  It must list all files individually,
--    including shared libraries and assets. The files are laid out
--    in the proper manner; assets are put in usr/share, libs are placed
--    in usr/libs/triumph4php
-- 8. copy other files needed for the DEB archive, like the version file and
--    the LICENSE files, remove other boilerplate files created by dh_make
-- 9. Run dpkg-buildpackage it will build the final executable using our Makefile
--    and will package it.
--
newaction {
	trigger = "distdeb",
	description = "Build the Triumph distributable DEB package",
	execute = function()
		tag = disttag()
		version = distversion()
		
		if (not tag or not version) then
			print("Cannot determine what branch or version to build. Please set them manually using the " ..
				"T4P_TAG and T4P_TAG_VERSION environment variables\n");
			os.exit(-1)
		end
		
		-- if we only want to print out the name of the deb file
		-- this is used by buildbot so that it knows the name of
		-- the package file to copy it out of the slave and to the
		-- master (because slaves are in EC2 and they are terminated)
		-- this is not a command line option because I don't like how
		-- premake deals with options (options cannot be tied to a 
		-- single action)
		onlyFilename = os.getenv('T4P_PKG_FILENAME_ONLY');
		if (onlyFilename == '1') then
			print(path.getabsolute("../triumph4php_" .. version .. "_amd64.deb"))
			os.exit(0)
		end
		
		printf("creating DEBIAN package for branch %s using version number %s\n", tag, version)

		--
		-- linux distribution: we package a .DEB file
		--
		--
		workDir = path.getabsolute("../triumph4php-" .. tag)
		workLibDir = "/usr/lib/triumph4php"
		rootDir = normalizepath("./")
		libWildcards =  normalizepath("./Release/*.so*")
		assetDir = "/usr/share/triumph4php"
		debianControl = path.getabsolute("./package/debian.control");
		
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
				"yes | dh_make --single --email roberto@triumph4php.com  --native --packagename triumph4php_" .. version,
				
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
		
			string.format("git checkout %s", tag),
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
			"echo 'Release/libpelet.so usr/lib/triumph4php' >> debian/install",
			"echo 'Release/libkeybinder.so usr/lib/triumph4php' >> debian/install",
			
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
			-- we need to set the LD_LIBRARY_PATH so that dpkg finds our dependencies
			-- -b means we create the binary only package, also -us -uc
			-- creates an unsigned version
			-- The custom build options are there so that the build slaves can successfully
			-- create the DEB packages. It seems that the hardening and optimization features are 
			-- pretty memory intensive, and the buildslaves only have 1 GB RAM, so building 
			-- the packages fails with the machine running out of memory. Of course, we'd rather 
			-- not have to do this, but we really want the DEB file generation to be an
			-- automatic process.
			"LD_LIBRARY_PATH=$LD_LIBRARY_PATH:Release/libs:Release DEB_CXXFLAGS_STRIP='-O2 -g' DEB_CFLAGS_STRIP='-O2 -g' DEB_BUILD_MAINT_OPTIONS=hardening=-fortify,-stackprotector dpkg-buildpackage -b -us -uc"
		});
	end
}
