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
-- @copyright  2013 Roberto Perpuly
-- @license    http://www.opensource.org/licenses/mit-license.php The MIT License
-------------------------------------------------------------------

function prepWxWidgets()
	if os.is "windows" then
		print "Compiling wxWidgets; this will take a several minutes..."
		wxBuildDir = normalizepath("lib/wxWidgets/build/msw");		
		wxSamplesDir = normalizepath("lib/wxWidgets/samples");
		rootPath = normalizepath("");
		batchexecute(rootPath, {
			
			-- wrap around quotes in case path has spaces
			"\"" .. VSVARS .. "\"",
			
			-- debug version of wxWidgets
			"cd " .. wxBuildDir,
			"nmake -f makefile.vc BUILD=debug SHARED=1",
			"cd " .. wxSamplesDir, 
			"nmake -f makefile.vc BUILD=debug SHARED=1",
			
			-- release version of wxWidgets
			"cd " .. wxBuildDir,
			"nmake -f makefile.vc BUILD=release SHARED=1",
			"cd " .. wxSamplesDir,
			"nmake -f makefile.vc BUILD=release SHARED=1",
			
			-- copy DLLs to the output directory
			"cd " .. rootPath,
			"xcopy /S /Y " .. normalizepath(WX_LIB_DIR .. "wxbase*ud_*.dll") .. " \"Debug\\\"",
			"xcopy /S /Y " .. normalizepath(WX_LIB_DIR .. "wxmsw*ud_*.dll") .. " \"Debug\\\"",
			"xcopy /S /Y " .. normalizepath(WX_LIB_DIR .. "wxbase*u_*.dll") .. " \"Release\\\"",
			"xcopy /S /Y " .. normalizepath(WX_LIB_DIR .. "wxmsw*u_*.dll") .. " \"Release\\\""
		});
	else 

		-- build wxWidgets using make
        -- remove libs that we don't use
		wxInstallDebug = normalizepath("lib/wxWidgets/triumph_debug/")
        wxBuildDebug = normalizepath("lib/wxWidgets/triumph_build_debug")
		wxInstallRelease = normalizepath("lib/wxWidgets/triumph/")
        wxBuildRelease = normalizepath("lib/wxWidgets/triumph_build")
		
		batchexecute('lib/wxWidgets', {
			"mkdir -p " .. wxBuildDebug,
			"mkdir -p " .. wxBuildRelease,
			"cd " .. wxBuildDebug,
			"../configure " .. 
				"--enable-debug --without-libjpeg --without-libtiff " ..
				"--without-zlib --with-gtk --enable-unicode " ..
				"--prefix=" .. wxInstallDebug,
			"make",
			"make install",

			-- now build the release version
			"cd " .. wxBuildRelease,
			"../configure " .. 
				"--without-libjpeg --without-libtiff " ..
				"--without-zlib --with-gtk --enable-unicode " ..
				"--prefix=" .. wxInstallRelease,
			"make",
			"make install"
		})
		
		-- copy wxWidgets libraries to the same dir as our executable
		-- use the wx-config to get the location of the libraries
		cmd = WX_CONFIG .. ' --prefix'
		cmdStream = io.popen(cmd)
		cmdOutput = cmdStream:read("*l")
		cmdStream:close()
		
		debugLibDir = wxInstallDebug .. '/lib'
		releaseLibDir = wxInstallRelease .. '/lib'
		batchexecute(normalizepath(""), {
			"mkdir -p Debug",
			"mkdir -p Release",
			"cp -r " .. debugLibDir .. "/*.so* Debug/",
			"cp -r " .. releaseLibDir .. "/*.so* Release/"
		});
		end
end

newaction {
	trigger = "wxwidgets",
	description = "Build the wxWidgets library",
	execute = prepWxWidgets
}
