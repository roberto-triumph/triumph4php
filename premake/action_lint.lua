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
-- This action perfoms linting of Triumph's source code by using
-- google's C++ source code style linter.  This action should
-- be run before every push to the central repo. 
--
function lint()
	if os.is "windows" then
		print "lint only works on linux or Mac OS X systems.\n";
		os.exit(1);
	elseif os.is "linux" or os.is "macosx" then  
		failMsg = "The lint action requires that python be installed."
		programexistence("python", "--version", failMsg);

		cmd = 'find -f profilers src tests tutorials view_tests -name "*.cpp" -or -name "*.h" | xargs python lib/googleStyleGuide/cpplint/cpplint.py';
		ret = os.execute(cmd);
		if ret > 0 then
			os.exit(1)
		end
	else 
		print "Triumph does not support linting on this operating system.\n"
	end
end

newaction {
	trigger = "lint",
	description = "Perform code style checks against the current working copy of the code.",
	execute = lint
}
