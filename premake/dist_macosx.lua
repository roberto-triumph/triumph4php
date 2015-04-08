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
-- The dist_macosx action will build a Mac OS X application bundle for
-- Triumph. The app bundle will contain all of the dependant shared 
-- libraries, this is to avoid needing end users having to install
-- brew / mac ports.
--
-- Building the application bundle involves the use of the install_name_tool
-- so that we make the triumph application use the shared libraries that 
-- are located inside the app bundle.
--
newaction {
	trigger = "distmacosx",
	description = "Build the Triumph distributable Mac OS X application bundle",
	execute = function()
		tag = disttag()
		version = distversion()
		
		if (not tag or not version) then
			print("Cannot determine what branch or version to build. Please set them manually using the " ..
				"T4P_TAG and T4P_TAG_VERSION environment variables\n");
			os.exit(-1)
		end
		
		-- if we only want to print out the name of the dmg file
		-- this may be used by buildbot (when/if we have it for mac)
		-- so that it knows the name of
		-- the package file to copy it out of the slave and to the
		-- master (because slaves are in EC2 and they are terminated)
		-- this is not a command line option because I don't like how
		-- premake deals with options (options cannot be tied to a 
		-- single action)
		onlyFilename = os.getenv('T4P_PKG_FILENAME_ONLY');
		if (onlyFilename == '1') then
			print(path.getabsolute("../triumph4php-" .. tag .."/triumph4php-" .. version .. ".dmg"))
			os.exit(0)
		end
		
		printf("creating Mac OS X app bundle for branch %s using version number %s\n", tag, version)
		
		rootDir = normalizepath("./")
		workDir = path.getabsolute("../triumph4php-" .. tag)
		binDir = workDir .. "/Release/triumph4php.app/Contents/MacOS"

		-- the asset dir will be inside the app bundle, need to make it in a 
		-- directory adjacent to the "Contents/MacOS" directory
		assetDir = "../Resources"
		assetSrcFullPath = workDir .. "/assets"
		assetDestFullPath = workDir .. "/Release/triumph4php.app/Contents/Resources"
		
		batchexecute(rootDir, {
				
			-- clone the project into a new, temp directory
			string.format("git clone . %s", workDir),
			string.format("cd %s", workDir),
			"git submodule init",
			"git submodule update lib/pelet",
			
			-- build triumph, using wx widgets and soci from our dev 
			-- directory
			"cd " .. workDir,	
			"T4P_WXCONFIG=" .. normalizepath(WX_CONFIG) .. " " ..
			"T4P_SOCI_DEBUG_INCLUDE_DIR=" .. normalizepath(SOCI_DEBUG_INCLUDE_DIR) .. " " ..
			"T4P_SOCI_DEBUG_LIB_DIR=" .. normalizepath(SOCI_DEBUG_LIB_DIR)  .. " "  ..
			"T4P_SOCI_RELEASE_INCLUDE_DIR=" .. normalizepath(SOCI_RELEASE_INCLUDE_DIR) .. " " ..
			"T4P_SOCI_RELEASE_LIB_DIR=" .. normalizepath(SOCI_RELEASE_LIB_DIR) .. " " ..  
			"T4P_ASSET_DIR=" .. assetDir .. " " ..
			"./premake4-macosx gmake",
			"cd build/gmake",
			"make -j 5 triumph4php",
			
			-- copy assets to the app bundle dir
			string.format('mkdir -p %s', assetDestFullPath),
			string.format('cp -r %s/* %s', assetSrcFullPath, assetDestFullPath),
			string.format('cp -r %s/icons/triumph4php.icns %s/triumph4php.icns', assetDestFullPath, assetDestFullPath)
		})
		
		cmdOutput = execoutput(ICU_CONFIG .. ' --prefix')
		icuLibDir = trim(cmdOutput) .. "/lib"
		
		cmdOutput = execoutput(WX_CONFIG .. ' --prefix')
		wxWidgetsLibDir = trim(cmdOutput) .. "/triumph/lib" -- because we do not "install" wx widgets; see action_wxwidgets
			
		-- move the dependant libraries to the "Contents/MacOS" directory right
		-- next to the triumph binary
		os.execute("mkdir -p " .. workDir .. "/Release/triumph4php.app/Contents/MacOS")
		batchexecute(rootDir, {

			-- -P and -R so that symlinks are copied as links, and files 
			-- are not duplicated
			string.format("cp -P -R %s %s", normalizepath(wxWidgetsLibDir .. '/libwx*.dylib'), binDir),
			string.format("cp -P -R %s %s", normalizepath(SOCI_RELEASE_LIB_DIR .. '/libsoci*.dylib'), binDir),
			string.format("cp -P -R %s %s", normalizepath(icuLibDir .. '/libicu*.dylib'), binDir),
			
			-- here since were are copying single lines, we want symlinks to be followed
			string.format("cp %s %s", normalizepath(CURL_LIB_DIR .. '/' .. CURL_LIB_NAME), binDir),
			string.format("cp %s %s", normalizepath(workDir .. '/Release/libpelet.dylib'), binDir),
			string.format("cp %s %s", normalizepath(workDir .. '/Release/libkeybinder.dylib'), binDir),
			string.format("cp %s %s", normalizepath(SQLITE_LIB_DIR .. '/' .. SQLITE_LIB_NAME), binDir),
			string.format("cp %s %s", normalizepath(MYSQL_LIB_DIR .. '/' .. MYSQL_LIB_NAME), binDir),

			-- we only need this one for now asio is header only
			string.format("cp %s %s", normalizepath(BOOST_RELEASE_LIB_DIR .. '/libboost_system-mt.dylib'), binDir)
		})
		
		-- go through each shared library and change it so that the ID of
		-- the library is relative to the root of the app bundle
		allLibs = os.matchfiles(workDir .. "/Release/triumph4php.app/Contents/MacOS/*.dylib");
		for i, lib in pairs(allLibs) do
			libName = string.match(lib, "/(lib[%w_%._%-]+%.dylib)")
			batchexecute(rootDir, {
				string.format("chmod 640 %s", lib),
				string.format("install_name_tool -id @executable_path/../MacOS/%s %s", libName, lib)	
			})
		end
		
		-- go through all of the libs, but this time we want to make sure that 
		-- the libs themselves only refer to the libs inside of the app bundle
		-- for example wx_aui lib must use the wx_base from the app bundle; soci
		-- lib must use sqlite lib from the app bundle
		-- note that here, we change the libs and the executable
		allLibs = os.matchfiles(workDir .. "/Release/triumph4php.app/Contents/MacOS/*");
		for i, lib in pairs(allLibs) do
			out = execoutput(string.format('otool -L %s', lib));			
			lineNumber = 0;
			for line in string.gmatch(out, "([^\n]+)\n") do
				
				-- skip the first line, it is the lib id
				if lineNumber ~= 0 then
					line = trim(line)
					if (string.find(line, '^/System') == nil
						and string.find(line, '^/usr/lib') == nil
						and string.find(line, '^@') == nil) then
						libToChange = string.match(line, '^(.+%.dylib)')
						if libToChange ~= nil then
							libBaseName = string.match(libToChange, '(lib[^/]+%.dylib)$')
							cmd = string.format('install_name_tool -change %s @executable_path/../MacOS/%s %s', libToChange, libBaseName, lib)
							batchexecute(rootDir, {
								cmd
							})
						end
					end
				end
				lineNumber = lineNumber + 1
			end
		end

		-- now create a DMG file
		-- i followed instructions from 
		-- https://wiki.wxwidgets.org/Distributing_WxWidgets_Applications-Distributing_WxMac_Programs

		-- we need to get the directory size of the volume to create
		cmdOutput = execoutput("du -hs " .. workDir .. "/Release/triumph4php.app")
		sizeMb = trim(cmdOutput)
		size = string.match(sizeMb, "(%d+)")
		size = size + 4  -- add some padding, round up
		batchexecute(workDir, {
			string.format("hdiutil create -megabytes %d -layout NONE triumph4php-%s.dmg", size, version),
		})

		-- need to read the disk that was created
		cmdOutput = execoutput(string.format("hdid -nomount %s/triumph4php-%s.dmg", workDir, version))
		disk = trim(cmdOutput)
		print("disk=" .. disk)

		batchexecute(workDir, {
			string.format("newfs_hfs -v triumph4php %s", disk),
			string.format("hdiutil eject %s", disk),
			string.format("hdid triumph4php-%s.dmg", version),
			"cp -P -R Release/triumph4php.app /Volumes/triumph4php"
		})
	end
}
