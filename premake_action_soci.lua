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

-- will look for MySQL files in these directories
MYSQL_INSTALL_DIR = "/usr/"

newaction {
	trigger = "soci",
	description = "Build the SOCI (database access) library",
	execute = function()
		if os.is "windows" then
			--  TODO
		else
			ret = os.execute("cmake --version")
			if ret ~= 0 then
				print("cmake not found. Compiling SOCI requires CMake. SOCI cannot be built.")
			else
				SOCI_BUILD =  os.getcwd() .. "/lib/soci/mvc_editor/"
				-- always get  a specific version so that we are not developing against a moving target
				cmd = "mkdir lib/soci && " ..
					"cd lib/soci &&" ..
					"git archive " ..
					" --format=tar " ..
					" --remote git://soci.git.sourceforge.net/gitroot/soci/soci acba467dce5c6f4df618f3a816b9f9fc56bd6543" ..
					"| tar -xf - "
				if 0 == os.execute(cmd) then
				
					-- exclude SOCI from linking against Boost. we don't use it
					cmd = "cd lib/soci/src && " ..
						"cmake -DWITH_MYSQL=" .. MYSQL_INSTALL_DIR .. 
						" -DCMAKE_INSTALL_PREFIX=" .. SOCI_BUILD .. 
						" -DWITH_BOOST=NO " ..
						"&& make " ..
						"&& make install"
					if 0 ~= os.execute(cmd) then
						print("Building SOCI failed")
					end
				else
					print ("SOCI cannot be built. ")
					print("Failed to fetch SOCI source code from the remote Git repo. Do you have network connectivity?")
				end
			end
		end
	end
}
