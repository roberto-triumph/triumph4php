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
-- @author     $Author$
-- @date       $Date$
-- @version    $Rev$ 
-------------------------------------------------------------------
newaction {
	trigger = "wxwidgets",
	description = "Build the wxWidgets libraries in Debug and Release mode",
	execute = function()
			if os.is "windows" then
				wxEnv = os.getenv("WXWIN")
				if wxEnv then
					print ("WXWIN environment variable: " .. wxEnv)
					print("Use the Visual Studio solution provided by wxWidgets to build the libraries \n" .. 
							"1) Download wxPack. Install to a C:\\users\\[user]\\wxWidgets directory \n" ..
							"2) Open Solution wx.dsw located build\msw directory\n" .. 
							"3) Select 'Debug Unicode' from Configuration Manager\n" .. 
							"4) Build Entire Solution\n" .. 
							"5) Select 'Release Unicode' from Configuration Manager\n" .. 
							"6) Build Entire Solution\n" .. 
							"7) Open Solution stc.dsw located in contrib\build\msw directory\n" .. 
							"8) Select 'Debug Unicode' from Configuration Manager\n" .. 
							"9) Build Entire Solution\n" .. 
							"10) Select 'Release Unicode' from Configuration Manager\n" .. 
							"11) Build Entire Solution\n"
					);
				else 
					print "WXWIN environment variable not found. Generated Solution file WILL NOT WORK. Please install wxPack."
				end
			else
				-- build the debug version 
				-- wxWidgets already has a build directory ... need to use another convention
				-- Patch is needed in order to compile wxWidgets-2.8.10 under Unbunt 9.10.  For more info, and to see the patch, go to
				-- http://trac.wxwidgets.org/ticket/10883
				-- build the wxStyledTextCtrl (it is not in the default build)
				WX_BUILD =  os.getcwd() .. "/lib/wxWidgets-2.8.10/mvc_editor/Debug"
				os.execute(
					"cd lib/ && " ..
					"tar xzf wxWidgets-2.8.10.tar.gz && " ..
					"cd wxWidgets-2.8.10 && " ..
					"patch -Np0 -i ../wxWidgets-2.8.10.patch && "  ..
					"mkdir -p build_mvc_editor_debug && " .. 
					"cd build_mvc_editor_debug && " .. 
					".././configure --prefix=\"" .. WX_BUILD .. "\" --with-gtk=2 --enable-unicode --enable-debug && " .. 
					"make && " .. 
					"make install && " .. 
					"cd contrib/src/stc/ && " .. 
					"make && " .. 
					"make install"
				);
				
				-- build the release version
				-- don't need to patch again
				WX_BUILD =  os.getcwd() .. "/lib/wxWidgets-2.8.10/mvc_editor/Release"					
				os.execute(
					"cd lib/wxWidgets-2.8.10 && " ..
					"mkdir -p build_mvc_editor_release && " .. 
					"cd build_mvc_editor_release && " .. 
					".././configure --prefix=\"" .. WX_BUILD .. "\" --disable-debug --disable-debug_gdb " ..
					"               --with-gtk=2 --enable-unicode --enable-optimize  && " .. 
					"make && " .. 
					"make install && " .. 
					"cd contrib/src/stc/ && " .. 
					"make && " .. 
					"make install"
				 );
			end
	end
}
