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

function prepBoost()
	
	if os.is "windows" then
	
		boostZip = "lib/boost_1_55_0.7z";
		boostDownload = "http://triumph4php.com/boost_1_55_0.7z"
		extractedDir = 'lib/boost_1_55_0'
		existenceOrDownloadExtract(boostZip, extractedDir, boostDownload, "Downloading boost dependency");
		
		-- on windows, we compile boost
		-- we use asio which is a header-only library
		-- but asio needs boost.system, and boost.system is 
		-- not header only
		batchexecute(normalizepath("lib/boost_1_55_0"), {

			-- wrap around quotes in case path has spaces
			"\"" .. VSVARS .. "\"",
			'bootstrap.bat',
			'.\\b2 --with-system --link=shared --variant=debug,release --toolset=msvc-9.0'
		})
	else  
	
		-- BOOST_LIB_DIR is already the result of a os.searchpath
		-- which searched the default locations for the boost library
		boostLib = BOOST_LIB_DIR
		if boostLib == nil then
			error (
				"Boost libraries not found.  " .. 
				"Please install the boost libraries, or change the location of \n" ..
				"BOOST_LIB_DIR in premake_opts_linux.lua.\n" ..
				"You can install the boost libraries via your package manager; ie. sudo apt-get install libboost-dev\n"
			)
		end
	end
end

newaction {
	trigger = "boost",
	description = "Fetch and compile the boost libraries or check for their existence",
	execute = prepBoost
}