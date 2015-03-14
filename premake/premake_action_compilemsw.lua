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
-- @copyright  2015 Roberto Perpuly
-- @license    http://www.opensource.org/licenses/mit-license.php The MIT License
-------------------------------------------------------------------

--
-- this function will compile the triumph solution using the
-- visual studio command line tools.  This exists so that 
-- the buildbot slave can compile triumph; there currently
-- exists a bug with the buildbot shell command build step; it has trouble
-- escaping command line arguments
-- See http://trac.buildbot.net/ticket/2878
--
function compileMsw() 
	
	rootPath = normalizepath("build/vs2008")
	batchexecute(rootPath, {
		
		-- wrap around quotes in case path has spaces
		"\"" .. VSVARS .. "\"",
		"vcbuild /rebuild triumph4php.sln \"Debug|Win32\"",
		"vcbuild /rebuild triumph4php.sln \"Release|Win32\""
	})
end

newaction {
	trigger = "compilemsw",
	description = "Compiles triumph using Visual Studio command line tools.",
	execute = compileMsw
}
