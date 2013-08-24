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

newaction {
	trigger = "wxwidgets",
	description = "Build the wxWidgets library",
	execute = function()
		WXWIDGETS_BUILD_DIR =  normalizepath("lib/wxWidgets/mvc-editor");
		WXWIDGETS_ROOT = normalizepath("lib/wxWidgets")
		WXWIDGETS_SRC = normalizepath("lib/wxWidgets/src")
		if os.is "windows" then

			-- create a setup.h if it does not exist
			-- the repo does not have it
			-- but it has a template (setup0.h) that contains the recommended settings
			setupSrc = "lib/wxWidgets/include/wx/msw/setup0.h"
			setupDest = "lib/wxWidgets/include/wx/msw/setup.h"
			if not os.copyfile(setupSrc, setupDest) then
				error("could not create setup.h at " .. setupDest .. " from " .. setupSrc)
			end

			print ("Open the solution (for your Visual Studio version) found at " .. normalizepath("lib/wxWidgets/build/msw/"))
			print "Build the solution in DLL Debug configuration"
			print "Build the solution in DLL Release configuration"
			print "Note that you will have the builds will fail. Retry a couple of times"
			print "This is due to the dependencies not being setup correctly."
			print "see http://wiki.wxwidgets.org/Microsoft_Visual_C%2B%2B_Guide"
		else 

			-- now build wxWidgets
            WX_BUILD = normalizepath("lib/wxWidgets/mvc-editor/")
			batchexecute('lib/wxWidgets', {
            "mkdir -p " .. WX_BUILD,
			"./configure --enable-debug --with-gtk --enable-unicode --prefix=" .. WX_BUILD,
			"make",
			"make install"
			})
		end
	end
}
