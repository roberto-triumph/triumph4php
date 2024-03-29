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
-- @copyright  2009-2012 Roberto Perpuly
-- @license    http://www.opensource.org/licenses/mit-license.php The MIT License
-------------------------------------------------------------------

--
-- Create the PHP parser and lexer C++ code
-- from the grammar files (bison, re2c). This only needs to be
-- done when the grammar files change, as the resulting C++ code
-- is checked into source control.
--
newaction {
	trigger = "generate",
	description = "Regenerate the source code for the PHP parser and PHP lexer.",
	execute = function()
	
		-- regenerate the parser and lexer implementation files
		-- re2c does not escape windows paths and leads to compile errors, turn off debug info
		cmd = "re2c -c -i --no-generation-date " ..
			"-o " .. normalizepath("lib/pelet/src/Php53LexicalAnalyzer.cpp") .. " " ..
			normalizepath("lib/pelet/src/Php53LexicalAnalyzer.re");
		code = os.execute(cmd) 
		if code ~= 0 then
			print("re2c command failed for file lib/pelet/src/Php53LexicalAnalyzer.re. Is re2c installed? Is it in the PATH?");	
		end
		cmd = "re2c -c -i --no-generation-date " ..
			"-o " .. normalizepath("lib/pelet/src/Php54LexicalAnalyzer.cpp") .. " " ..
			normalizepath("lib/pelet/src/Php54LexicalAnalyzer.re");
		code = os.execute(cmd) 
		if code ~= 0 then
			print("re2c command failed for file lib/pelet/src/Php54LexicalAnalyzer.re. Is re2c installed? Is it in the PATH?");	
		end
		cmd = "re2c -c -i --no-generation-date " ..
			"-o " .. normalizepath("lib/pelet/src/LanguageDiscoveryClass.cpp") .. " " ..
			normalizepath("lib/pelet/src/LanguageDiscoveryClass.re");
		code = os.execute(cmd) 
		if code ~= 0 then
			print("re2c command failed for file lib/pelet/src/LanguageDiscoveryClass.re. Is re2c installed? Is it in the PATH?");	
		end
		code = os.execute(cmd) 
		if os.is("windows") then
			-- in windows line macros dont work correctly
			cmd = "bison --no-lines --warnings=error " ..
				" -o " .. normalizepath("lib/pelet/src/Php53FullParser.cpp") .. " " ..
				normalizepath("lib/pelet/src/Php53FullParser.y")
		else 
			cmd = "bison  --warnings=error " ..
				" -o " .. normalizepath("lib/pelet/src/Php53FullParser.cpp") .. " " ..
				normalizepath("lib/pelet/src/Php53FullParser.y")
		end
		code = os.execute(cmd) 
		if code ~= 0 then
			print("Bison command failed for file lib/pelet/src/Php53FullParser.y. Is bison installed? Is it in the PATH?");	
		end
		
		if os.is("windows") then
			-- in windows line macros dont work correctly
			cmd = "bison --no-lines --warnings=error " ..
				" -o " .. normalizepath("lib/pelet/src/Php53LintParser.cpp") .. " " ..
				normalizepath("lib/pelet/src/Php53LintParser.y")
		else 
			cmd = "bison  --warnings=error " ..
				" -o " .. normalizepath("lib/pelet/src/Php53LintParser.cpp") .. " " ..
				normalizepath("lib/pelet/src/Php53LintParser.y")
		end
		code = os.execute(cmd) 
		if code ~= 0 then
			print("Bison command failed for file lib/pelet/src/Php53LintParser.y. Is bison installed? Is it in the PATH?");	
		end
		
		if os.is("windows") then
			-- in windows line macros dont work correctly
			cmd = "bison --no-lines --warnings=error "  ..
				" -o " .. normalizepath("lib/pelet/src/Php54FullParser.cpp") .. " " ..
				normalizepath("lib/pelet/src/Php54FullParser.y")
		else
			cmd = "bison --warnings=error "  ..
				" -o " .. normalizepath("lib/pelet/src/Php54FullParser.cpp") .. " " ..
				normalizepath("lib/pelet/src/Php54FullParser.y")
		end
		code = os.execute(cmd) 
		if code ~= 0 then
			print("Bison command failed for file lib/pelet/src/Php54FullParser.y. Is bison installed? Is it in the PATH?");	
		end
		
		if os.is("windows") then
			-- in windows line macros dont work correctly
			cmd = "bison --no-lines --warnings=error "  ..
				" -o " .. normalizepath("lib/pelet/src/Php54LintParser.cpp") .. " " ..
				normalizepath("lib/pelet/src/Php54LintParser.y")
		else
			cmd = "bison --warnings=error "  ..
				" -o " .. normalizepath("lib/pelet/src/Php54LintParser.cpp") .. " " ..
				normalizepath("lib/pelet/src/Php54LintParser.y")
		end
		code = os.execute(cmd) 
		if code ~= 0 then
			print("Bison command failed for file lib/pelet/src/Php54LintParser.y. Is bison installed? Is it in the PATH?");	
		end
		
		if os.is("windows") then
			-- in windows line macros dont work correctly
			cmd = "bison --warnings=error " ..
				" -o " .. normalizepath("lib/pelet/src/Php53ResourceParser.cpp") .. " " ..
				normalizepath("lib/pelet/src/Php53ResourceParser.y")
		else 
			cmd = "bison  --warnings=error " ..
				" -o " .. normalizepath("lib/pelet/src/Php53ResourceParser.cpp") .. " " ..
				normalizepath("lib/pelet/src/Php53ResourceParser.y")
		end
		code = os.execute(cmd) 
		if code ~= 0 then
			print("Bison command failed for file lib/pelet/src/Php53ResourceParser.y. Is bison installed? Is it in the PATH?");	
		end
		
		if os.is("windows") then
			-- in windows line macros dont work correctly
			cmd = "bison --warnings=error " ..
				" -o " .. normalizepath("lib/pelet/src/Php54ResourceParser.cpp") .. " " ..
				normalizepath("lib/pelet/src/Php54ResourceParser.y")
		else 
			cmd = "bison  --warnings=error " ..
				" -o " .. normalizepath("lib/pelet/src/Php54ResourceParser.cpp") .. " " ..
				normalizepath("lib/pelet/src/Php54ResourceParser.y")
		end
		code = os.execute(cmd) 
		if code ~= 0 then
			print("Bison command failed for file lib/pelet/src/Php54ResourceParser.y. Is bison installed? Is it in the PATH?");	
		end
	end
}
