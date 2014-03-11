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

-- takes a path relative to the project root and returns an absolute path that 
-- this function will also enclose the path in quotes; allowing file paths with spaces to be used.
-- main purpose of this function is to generate paths that can be embedded into strings that consist
-- of system commands.
-- be careful, the result of this function is NOT suitable for file functions
-- ie. os.isfile() since the result is already enclosed in quotes!
function normalizepath(filepath) 
	if not path.isabsolute(filepath) then
		filepath = os.getcwd() .. "/" .. filepath
	end
	if os.is "windows" then
	
		-- Windows XP doesn't like forward slashes
		filepath = "\"" .. string.gsub(filepath, "/", "\\") .. "\"";
	else 
		filepath = "\"" .. filepath .. "\"";
	end
	return filepath
end

--
-- Checks that a program exists in the system by taking the binary 
-- and the arg and executing it.  If the system call fails,
-- then the program does not exist, and the script ends.
-- most of the time, the arg will be '--version' as this 
-- is the easiest way to check for program existence.
--
-- @param pathsToCheck a table of strings or a single string
--
function programexistence(prog, arg, extraMessage)
	cmdString = string.format("\"%s\" %s", prog, arg)
	if 0 ~= os.execute(cmdString) then
		print (prog .. " not found. " .. extraMessage)
		print "Program will now exit"
		os.exit(1)
	end
end

--
-- takes a path relative to the project root and checks that it exists in the file system. if
-- the file does not exist the program will exit.
--
-- @param pathsToCheck a table of strings or a single string
--
function existence(pathsToCheck, extraMessage)
	paths = {}
	if 'string' == type(pathsToCheck) then
		table.insert(paths, pathsToCheck)
	elseif 'table' == type(pathsToCheck) then
		for key, value in pairs(pathsToCheck) do
			table.insert(paths, value)
		end
	end
	for key, value in pairs(paths) do
		if not os.isfile(value) then
			print("Required file not found: " .. value)
			print(extraMessage)
			print "Program will now exit"
			os.exit(1)
		end
	end
end

function existenceOrDownloadExtract(zipFile, downloadUrl, extraMessage)
	zipFullPath = normalizepath(zipFile);
	if (not os.isfile(zipFile)) then
		print(extraMessage)
		downloadCmd = string.format("%s --output-document=%s %s",
			WGET,
			zipFullPath,
			downloadUrl);
		extractCmd = string.format("%s x %s",
			SEVENZIP, zipFullPath
		);
		batchexecute(normalizepath("lib"), {
			downloadCmd,
			extractCmd
		})		
	end
end
		
--
-- execute a table of commands
-- if a single command fails then the program will exit
--
-- @param basedir the directory where the commands will be executed from
-- @param cmds the list of commands
-- 
function batchexecute(basedir, cmds, errorMessage) 
	if #cmds then
		cmdString = "cd " .. basedir .. " && ";
		for key, cmd in ipairs(cmds) do
			cmdString = cmdString .. cmd
			if next(cmds, key)  then
				cmdString = cmdString .. " && ";
			end
		end
		if #cmdString then
			print(cmdString)
			if 0 ~= os.execute(cmdString) then
				if errorMessage then 
					print(errorMessage)
				end
				print "Command Failed"
				print(cmdString)
				print "Program will now exit"
				os.exit(1)
			end
		end
	end
end

-- 
-- changes into destdir and unzips file
--
function decompress(destdir, file)
	batchexecute(destdir, {
		"tar zxf " .. file
	});
end

-- trim whitespace from both ends of string
function trim(s)
	return s:find'^%s*$' and '' or s:match'^%s*(.*%S)'
end
