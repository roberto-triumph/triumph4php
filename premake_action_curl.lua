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

function prepCurl()
	curlDir =  normalizepath("lib/curl");
	if os.is "windows" then
		
		-- compile curl
		curlPath = normalizepath("lib/curl");
		curlDebugBinPath = normalizepath(CURL_DEBUG_BIN_DIR .. "/*.dll")
		curlReleaseBinPath = normalizepath(CURL_RELEASE_BIN_DIR .. "/*.dll")
		rootPath = normalizepath("")
		batchexecute(curlPath, {
		
			-- wrap around quotes in case path has spaces
			"\"" .. VSVARS .. "\"",
			"buildconf.bat",
			"cd winbuild",
			"nmake /f Makefile.vc mode=dll DEBUG=yes USE_IDN=no",
			"nmake /f Makefile.vc mode=dll DEBUG=no USE_IDN=no",
			"cd " .. rootPath,
			"xcopy /S /Y " .. curlDebugBinPath .. " \"Debug\\\"",
			"xcopy /S /Y " .. curlReleaseBinPath .. " \"Release\\\""
		});		
	elseif os.is "linux" then
	
		-- CURL_LIB_DIR is already the result of a os.searchpath
		-- which searched the default locations for the curl library
		curlLib = CURL_LIB_DIR
		if curlLib == nil then
			error (
				"CURL libraries not found.  " .. 
				"Please install the CURL client library, or change the location of \n" ..
				"CURL_LIB_DIR in premake_opts_linux.lua.\n" ..
				"You can install curl via your package manager; ie. sudo apt-get install libcurl-dev\n"
			)
		end
	elseif os.is "macosx" then
	
		-- CURL_LIB_DIR is already the result of a os.searchpath
		-- which searched the default locations for the curl library
		curlLib = CURL_LIB_DIR
		if curlLib == nil then
			error (
				"CURL libraries not found.  " .. 
				"Please install the CURL client library, or change the location of \n" ..
				"CURL_LIB_DIR in premake_opts_macosx.lua.\n" ..
				"Curl is usually installed in macosx by default. \n"
			)
		end
	else 
		print "Triumph does not support building wxwidgets on this operating system.\n"
	end
end

newaction {
	trigger = "curl",
	description = "Build the CURL (HTTP) library",
	execute = prepCurl
}
