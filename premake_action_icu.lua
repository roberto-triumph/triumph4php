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

function prepIcu()
	if os.is "windows" then
		
		-- getting 4.2 on windows, 4.8 requires a newer visual studio
		icuZip = "lib/icu4c-4_2_1-src.zip";
		icuDownload = "http://download.icu-project.org/files/icu4c/4.2.1/icu4c-4_2_1-src.zip"
		existenceOrDownloadExtract(icuZip, icuDownload, "Downloading ICU dependency");
		
		-- compile ICU
		icuPath = normalizepath("lib/icu/source/allinone");
		rootPath = normalizepath("");
		batchexecute(rootDir, {
		
			-- wrap around quotes in case path has spaces
			"\"" .. VSVARS .. "\"",
			"cd " .. icuPath,
			
			-- rebuild (clean) the solutions
			-- otherwise there is a link error the second time 
			"vcbuild allinone.sln  \"Debug|Win32\"",
			"vcbuild allinone.sln  \"Release|Win32\"",
			"cd " .. rootPath,
			"xcopy /S /Y " .. normalizepath(ICU_LIB_DIR .. "../bin/*42d.dll") .. " \"Debug\\\"",
			"xcopy /S /Y " .. normalizepath(ICU_LIB_DIR .. "../bin/*41d.dll") .. " \"Debug\\\"",
			"xcopy /S /Y " .. normalizepath(ICU_LIB_DIR .. "../bin/icudt42.dll") .. " \"Debug\\\"",
			"xcopy /S /Y " .. normalizepath(ICU_LIB_DIR .. "../bin/*42.dll") .. " \"Release\\\"",
			"xcopy /S /Y " .. normalizepath(ICU_LIB_DIR .. "../bin/*41d.dll") .. " \"Release\\\""
		});
	else  
	
		-- on linux, we check that the icu config binary exists, as we use that
		-- binary to get the location of the shared libraries
		print(ICU_CONFIG .. " --exists")
		if 0 ~= os.execute(ICU_CONFIG .. " --exists") then
			error ("Could not execute icu-config. \n" ..
				"Please install the ICU library, or change the location of \n" ..
				"ICU_CONFIG in premake_opts_linux.lua.\n" ..
				"You can install ICU via your package manager; ie. sudo apt-get install libicu-dev\n"
			)
		end
	end
end

newaction {
	trigger = "curl",
	description = "Build the ICU (Unicode) library",
	execute = prepCurl
}
