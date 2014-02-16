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
		batchexecute(rootDir, {
			
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
		wxBuild = normalizepath("lib/wxWidgets/triumph/")
		batchexecute('lib/wxWidgets', {
			"mkdir -p " .. wxBuild,
			"./configure --enable-debug --with-gtk --enable-unicode --prefix=" .. wxBuild,
			"make",
			"make install"
		})
		
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
		end
end

newaction {
	trigger = "wxwidgets",
	description = "Build the wxWidgets library",
	execute = prepWxWidgets
}