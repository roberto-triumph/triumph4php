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
newaction {
	trigger = "distmsw",
	description = "Build the Triumph distributable for MS Windows.",
	execute = function()
			
		--
		-- MSW version, we just zip up the compiled executable
		-- the shared libs and assets
		--
		if os.isdir("dist") then
			os.execute("rmdir /s /q dist")
		end
		batchexecute(normalizepath(""), {
			"mkdir dist",
			"mkdir dist\\triumph4php",
			"mkdir dist\\triumph4php\\bin",
			"mkdir dist\\triumph4php\\assets",
			"xcopy /S /Y Release\\*.dll dist\\triumph4php\\bin",
			"copy Release\\triumph4php.exe dist\\triumph4php\\bin",
			"xcopy  /S /Y assets\\* dist\\triumph4php\\assets"
		});
		
		-- get the version info from git and populate the version file
		-- if we have no tags yet, use the -all flag
		cmd = "git describe --long > dist\\triumph4php\\assets\\version.txt"
		if 0 ~= os.execute(cmd) then
			cmd = "git describe --all --long > dist\\triumph4php\\assets\\version.txt"
			os.execute(cmd) 
		end
	end
}
