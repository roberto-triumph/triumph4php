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

function setupDev()
	failMsg = "This program is required to setup the development environment.\n" ..
		"You can set the proper location for the file by editing\n" ..
		"premake_opts_windows.lua. Or if you don't have it you will have to\n" ..
		"install it.  One nice way to install it is by using Chocolatey for " ..
		"windows\n\n" .. 
		"@powershell -NoProfile -ExecutionPolicy unrestricted " ..
		"-Command \"iex ((new-object net.webclient).DownloadString('https://chocolatey.org/install.ps1'))\" " ..
		" && SET PATH=%PATH%;%systemdrive%\\chocolatey\\bin\n" ..
		"cinst git\n" ..
		"cinst cmake\n" ..
		"cinst 7zip.commandline\n" ..
		"cinst wget\n" ..
		"\n\n" ..
		"However, be sure to uninstall any previous versions first.\n\n" ..
		"See https://github.com/chocolatey/chocolatey/wiki for more info on chocolatey\n"
	programexistence(GIT, "--version", failMsg);
	programexistence(CMAKE, "--version", failMsg);
	programexistence(SEVENZIP, "", failMsg);
	programexistence(WGET, "--version", failMsg);
	
	failMsg = "This batch file is needed to setup compilation variables in order\n" ..
		"to compile from the command line.  This batch file comes with Visual\n" ..
		"Studio Express. You can set the proper location for the file by editing\n" ..
		"premake_opts_windows.lua. Or if you don't have Visual Studio installed\n" ..
		"you will have to install it. Currently, triumph4php requires use of the\n" ..
		"Visual Studio compiler on windows\n"
	existence({
		VSVARS
	}, failMsg)
	rootDir = normalizepath("");
	
	print "Getting submodules; this will take a several minutes..."
	
	batchexecute(rootDir, {		
		string.format("%s submodule init", GIT),
		string.format("%s submodule update", GIT)
	});
	
	-- create the output directories if they don't exist yet
	if os.is "windows" then
		os.execute("mkdir Debug")
		os.execute("mkdir Release")
	elseif os.is "linux" then
		os.execute("mkdir -p Debug")
		os.execute("mkdir -p Release")
	end
	
	-- download or build the dependencies	
	prepCurl();
	prepIcu();
	prepMysql();
	prepSqlite();
	prepSoci();
	prepWxWidgets();
	
	print "SUCCESS! All dependencies are met. Next step is to build triumph4php in your environment.";
	if os.is "windows" then
		print "premake4.exe vs2008\n"
		print "Then open the Visual Studio solution in build\\vs2008\\triumph4php.sln" 
	elseif os.is "linux" then
		print "./premake gmake\n  OR \n ./premake codelite"
	end
end

newaction {
	trigger = "setupdev",
	description = "Initial setup of the development environment. Run this first.",
	execute = setupDev
}
