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

if os.is("windows") then
	dofile "premake_opts_windows.lua"
elseif os.is("linux") then
	dofile "premake_opts_linux.lua"
else
	error "You are running on a non-supported operating system. MVC Editor cannot be built.\n"
end
dofile "premake_functions.lua"
dofile "premake_action_prep.lua"
dofile "premake_action_dist.lua"
dofile "premake_action_generate.lua"
dofile "premake_action_soci.lua"


-- this configuration uses the icu-config binary to get the ICU header & library locations
-- this is usually the case on linux
function icuconfiguration(config, action)
	if config == "Debug" and action == "vs2008" then
		includedirs { ICU_INCLUDE_DIR }
		libdirs { ICU_LIB_DIR }
		links { ICU_LIBS_DEBUG }
	elseif config == "Debug" and (action == "gmake" or action == "codelite") then
		buildoptions { string.format("`%s --cppflags`", ICU_CONFIG) }
		linkoptions { string.format("`%s --ldflags --ldflags-icuio`", ICU_CONFIG) }
	elseif config == "Release" and action ==  "vs2008" then
		includedirs { ICU_INCLUDE_DIR }
		libdirs { ICU_LIB_DIR }
		links { ICU_LIBS_RELEASE }
	elseif config == "Release" and (action == "gmake" or action == "codelite") then
		buildoptions { string.format("`%s --cppflags`", ICU_CONFIG) }
		linkoptions { string.format("`%s --ldflags --ldflags-icuio`", ICU_CONFIG) }
	end
end

-- this configuration uses the wx-config binary to get the wx header & library locations
-- this is usually the case on linux
-- this configuration sets up the WX library and include files for Visual Studio projects

function wxconfiguration(config, action)
	if config == "Debug" and action == "vs2008" then
		libdirs { WX_LIB_DIR }
		includedirs { WX_INCLUDE_DIRS_DEBUG }

		-- wxWidgets framework needs these
		-- tell wxWidgets to import DLL symbols
		defines { "WIN32", "_DEBUG", "_WINDOWS", "WXUSINGDLL" }

		-- enable the "Use Unicode Character Set" option under General .. Character Set
		-- wxWidgets needs this in order to link properly
		flags { "Unicode" }
		links { WX_LIBS_DEBUG }
	elseif config == "Debug" and (action == "gmake" or action == "codelite") then
		buildoptions { string.format("`%s --cxxflags --debug=yes --unicode=yes`", WX_CONFIG) }
		linkoptions { string.format("`%s --debug=yes --unicode=yes --libs core,base,net`", WX_CONFIG) }
	elseif config == "Release" and action ==  "vs2008" then
		libdirs { WX_LIB_DIR }
		includedirs { WX_INCLUDE_DIRS_RELEASE }

		-- wxWidgets framework needs these
		-- tell wxWidgets to import DLL symbols
		defines { "WIN32", "_WINDOWS", "__WXMSW__", "WXUSINGDLL" }

		-- enable the "Use Unicode Character Set" option under General .. Character Set
		-- wxWidgets needs this in order to link properly
		flags { "Unicode", "Optimize" }
		links { WX_LIBS_RELEASE }
	elseif config == "Release" and (action == "gmake" or action == "codelite") then
		buildoptions { string.format("`%s --cxxflags --debug=no --unicode=yes`", WX_CONFIG) }
		linkoptions { string.format("`%s --debug=no --unicode=yes --libs core,base,net`", WX_CONFIG) }
	end
end

function wxappconfiguration(config, action)

	if action == "vs2008" then

		-- prevent the  "error LNK2019: unresolved external symbol _main referenced in function ___tmainCRTStartup
		flags { "WinMain" }
	end

	if config == "Debug" and (action == "gmake" or action == "codelite") then
		linkoptions { string.format("`%s --debug=yes --unicode=yes --libs aui,adv`", WX_CONFIG) }
	elseif config == "Debug" and action ==  "vs2008" then
		links { WX_LIBS_WINDOW_DEBUG }
	elseif config == "Release" and (action == "gmake" or action == "codelite") then
		linkoptions { string.format("`%s --debug=no --unicode=yes --libs aui,adv`", WX_CONFIG) }
	elseif config == "Release" and action ==  "vs2008" then
		links { WX_LIBS_WINDOW_RELEASE }
	end
end

function sociconfiguration()
	includedirs {
		"lib/soci/mvc-editor/include",
		"lib/soci/mvc-editor/include/soci",
		MYSQL_INCLUDE_DIR,
		SQLITE_INCLUDE_DIR
	}
	libdirs {
		MYSQL_LIB_DIR,
		SQLITE_LIB_DIR
	}
	if os.is "windows" then 
	
		links { 
			"soci_core_3_1", 
			"soci_mysql_3_1", 
			"soci_sqlite3_3_1" 
		}
		-- premake adds the lib prefix, we must take it away
		links { 
			string.match(MYSQL_LIB_NAME, "^([%w_]+)%.lib$"),
			string.match(SQLITE_LIB_NAME , "^([%w_]+)%.lib$") 
		}
		
		-- grab the libraries from the build directory.  the ones from
		-- from the install directory (lib/soci/mvc-editor) do not work (programs 
		-- that use them crash)
		libdirs { "lib/soci/src/lib/Release" }
	else 
		-- soci creates lib directory with the architecture name
		if os.isdir "lib/soci/mvc-editor/lib64" then
			libdirs { "lib/soci/mvc-editor/lib64" }
		else
			libdirs { "lib/soci/mvc-editor/lib" }
		end
		links { 
			"soci_core", 
			"soci_mysql", 
			"soci_sqlite3" 
		}
		links { 
			string.match(MYSQL_LIB_NAME, "^lib([%w_]+)%.so$"),
			string.match(SQLITE_LIB_NAME , "^lib([%w_]+)%.so$") 
		}
	end
end

function pickywarnings(action)
	if action == "vs2008" then
		flags { "FatalWarnings" }
	elseif action == "gmake" or action == "codelite" then

		-- when compiling strict warning checks; also check against variable length arrays
		-- since Visual Studio is not C99 compliant
		buildoptions { "-Wall", "-Werror", "-Wvla" }
	end
end

-- solution directory structure
-- the toolset files will be deposited in the build/ directory
-- each toolset will have its own directory
-- the executable files will be placed in the configuration directory (Debug/ or Release/)
-- compile flags will be set to be stricter than normal
solution "mvc-editor"
	if _ACTION then
		location ("build/" .. _ACTION)
	end
	configurations { "Debug", "Release"}
	configuration "Debug"
		objdir "Debug"
		targetdir "Debug"
		flags { "Symbols" }
	configuration "Release"
		objdir "Release"
		targetdir "Release"
	configuration "gmake or codelite"

		-- link against our own version of wxWidgets / ICU instead of any installed in the system
		linkoptions { "-Wl,-rpath=./" }
	configuration { "Debug", "vs2008" }

		-- prevent "warning LNK4098: defaultlib 'MSVCRTD' conflicts with use of other libs; use /NODEFAULTLIB:library"
		buildoptions { "/MDd" }

	project "mvc-editor"
		language "C++"
		kind "WindowedApp"
		files { "src/**.cpp", "src/**.h", "*.lua", "src/**.re, src/**.y, src/**.hpp", "README.md" }
		includedirs { "src/", "lib/keybinder/include/", "lib/pelet/include" }
		links { "tests", "keybinder", "pelet" }

		sociconfiguration()
		configuration "Debug"
			pickywarnings(_ACTION)
			icuconfiguration("Debug", _ACTION)
			wxconfiguration("Debug", _ACTION)
			wxappconfiguration("Debug", _ACTION)

		configuration "Release"
			pickywarnings(_ACTION)
			icuconfiguration("Release", _ACTION)
			wxconfiguration("Release", _ACTION)
			wxappconfiguration("Release", _ACTION)
		configuration { "Debug", "vs2008" }
			includedirs { "$(WXWIN)/contrib/include/" }
			links { "wxmsw28ud_stc" }
			postbuildcommands { "cd " .. normalizepath("Debug") .. " && tests.exe" }
		configuration { "Debug", "gmake or codelite" }
			links { "wx_gtk2ud_stc-2.8" }
			postbuildcommands { "cd " .. normalizepath("Debug") .. " && ./tests" }
		configuration { "Release", "vs2008" }
			includedirs { "$(WXWIN)/contrib/include/" }
			links { "wxmsw28u_stc" }
			postbuildcommands { "cd " .. normalizepath("Release") .. " && tests.exe"  }
		configuration { "Release", "gmake or codelite" }
			links { "wx_gtk2u_stc-2.8" }
			postbuildcommands { "cd " .. normalizepath("Release") .. " && ./tests" }

	project "tests"
		language "C++"
		kind "ConsoleApp"
		files {
			"tests/**.cpp",
			"tests/**.h",
			"src/globals/*.cpp",
			"src/language/SqlLexicalAnalyzerClass.cpp",
			"src/language/SymbolTableClass.cpp",
			"src/language/ResourceCacheClass.cpp",
			"src/php_frameworks/FrameworkDetectorClass.cpp",
			"src/php_frameworks/CallStackClass.cpp",
			"src/search/DirectorySearchClass.cpp",
			"src/search/FinderClass.cpp",
			"src/search/FindInFilesClass.cpp",
			"src/search/ResourceFinderClass.cpp",
			"src/widgets/ThreadWithHeartbeatClass.cpp",
			"src/widgets/ProcessWithHeartbeatClass.cpp"
		}

		-- these will be used by the SqlResourceFinder tests
		-- note the use of single quotes, MSW builds will fail if we
		-- use double quotes because visual studio already escapes
		-- defines with double quotes
		defines {
			string.format("MVCEDITOR_DB_USER=%s", MVCEDITOR_DB_USER)
		}

		-- handle empty password correctly; just don't define the macro
		-- this is so that the next word of the generated command line does
		-- not become the password
		if string.len(MVCEDITOR_DB_PASSWORD) > 0 then
			defines {
				string.format("MVCEDITOR_DB_PASSWORD=%s", MVCEDITOR_DB_PASSWORD)
			}
		end;
		includedirs { 
			"lib/UnitTest++/src/", 
			"lib/pelet/include",
			"src/", 
			"tests/", 
		}
		links { 
			"unit_test++", 
			"pelet"
		}

		sociconfiguration()
		configuration "Debug"
			pickywarnings(_ACTION)
			icuconfiguration("Debug", _ACTION)
			wxconfiguration("Debug", _ACTION)
		configuration "Release"
			pickywarnings(_ACTION)
			icuconfiguration("Release", _ACTION)
			wxconfiguration("Release", _ACTION)

	project "resource_finder_profiler"
		language "C++"
		kind "ConsoleApp"
		files {
			"profilers/resource_finder_profiler.cpp",
			"src/globals/Errors.cpp",
			"src/globals/Assets.cpp",
			"src/language/*.cpp",
			"src/search/ResourceFinderClass.cpp",
			"src/search/DirectorySearchClass.cpp",
			"src/search/FinderClass.cpp",
			"src/search/FindInFilesClass.cpp",
			"src/globals/String.cpp"
		}
		includedirs { "src", "lib/pelet/include" }
		links { "pelet" }

		sociconfiguration()
		configuration "Debug"
			pickywarnings(_ACTION)
			icuconfiguration("Debug", _ACTION)
			wxconfiguration("Debug", _ACTION)
		configuration { "Release"}
			pickywarnings(_ACTION)
			icuconfiguration("Release", _ACTION)
			wxconfiguration("Release", _ACTION)


	project "call_stack_profiler"
		language "C++"
		kind "ConsoleApp"
		files {
			"profilers/call_stack_profiler.cpp",
			"src/language/*.cpp",
			"src/php_frameworks/*.cpp",
			"src/globals/*.cpp",
			"src/search/ResourceFinderClass.cpp",
			"src/search/DirectorySearchClass.cpp",
			"src/search/FinderClass.cpp",
			"src/search/FindInFilesClass.cpp",
			"src/widgets/ThreadWithHeartbeatClass.cpp",
			"src/widgets/ProcessWithHeartbeatClass.cpp"
		}
		includedirs { "src", "lib/pelet/include" }
		links { "pelet" }

		sociconfiguration()
		configuration "Debug"
			pickywarnings(_ACTION)
			icuconfiguration("Debug", _ACTION)
			wxconfiguration("Debug", _ACTION)
		configuration { "Release"}
			pickywarnings(_ACTION)
			icuconfiguration("Release", _ACTION)
			wxconfiguration("Release", _ACTION)


	project "find_in_files_profiler"
		language "C++"
		kind "ConsoleApp"
		files {
			"profilers/find_in_files_profiler.cpp",
			"src/search/FindInFilesClass.cpp",
			"src/search/DirectorySearchClass.cpp",
			"src/search/FinderClass.cpp",
			"src/globals/Errors.cpp",
			"src/globals/String.cpp"
		}
		includedirs { "src/" }
		configuration "Debug"
			pickywarnings(_ACTION)
			icuconfiguration("Debug", _ACTION)
			wxconfiguration("Debug", _ACTION)
		configuration { "Release"}
			pickywarnings(_ACTION)
			icuconfiguration("Release", _ACTION)
			wxconfiguration("Release", _ACTION)

	project "code_control_profiler"
		language "C++"
		kind "WindowedApp"
		files {
			"profilers/code_control_profiler.cpp",
			"src/code_control/*.cpp",
			"src/widgets/ThreadWithHeartbeatClass.cpp",
			"src/widgets/ProcessWithHeartbeatClass.cpp",
			"src/widgets/StatusBarWithGaugeClass.cpp",
			"src/globals/*.cpp",
			"src/php_frameworks/*.cpp",
			"src/language/*.cpp",
			"src/search/FinderClass.cpp",
			"src/search/FindInFilesClass.cpp",
			"src/search/ResourceFinderClass.cpp",
			"src/search/DirectorySearchClass.cpp"
		}
		includedirs { "src/", "lib/pelet/include" }
		links { "pelet" }
		sociconfiguration()
		configuration "Debug"
			pickywarnings(_ACTION)
			wxconfiguration("Debug", _ACTION)
			wxappconfiguration("Debug", _ACTION)
			icuconfiguration("Debug", _ACTION)
		configuration "Release"
			pickywarnings(_ACTION)
			wxconfiguration("Release", _ACTION)
			wxappconfiguration("Release", _ACTION)
			icuconfiguration("Release", _ACTION)
		configuration { "Debug", "vs2008" }
			includedirs { "$(WXWIN)/contrib/include/" }
			links {  "wxmsw28ud_stc" }
		configuration {"Debug", "gmake or codelite"}
			links { "wx_gtk2ud_stc-2.8" }
		configuration { "Release", "vs2008" }
			includedirs { "$(WXWIN)/contrib/include/" }
			links {  "wxmsw28u_stc" }
		configuration {"Release", "gmake or codelite"}
			links { "wx_gtk2u_stc-2.8" }

	project "unit_test++"
		language "C++"
		kind "StaticLib"
		files { "lib/UnitTest++/src/*.cpp" }

		-- enable the "Use Unicode Character Set" option under General .. Character Set
		-- enable Stuctured Exception Handling needed by UnitTest++
		flags { "Unicode", "SEH" }

		-- For this project, no need to differentiate between Debug or Release
		configuration { "vs2008" }
			files { "lib/UnitTest++/src/Win32/*.cpp" }

			-- dont bother with warnings  with using 'unsafe' strcopy
			defines { "_CRT_SECURE_NO_WARNINGS", "_LIB" }

		configuration { "gmake or codelite" }
			files { "lib/UnitTest++/src/Posix/*.cpp" }

	project "unit_test++_test"
		language "C++"
		kind "ConsoleApp"
		links { "unit_test++" }
		files { "lib//UnitTest++/src/tests/*.cpp" }

		-- enable Stuctured Exception Handling needed by UnitTest++
		flags { "SEH" }

		-- enable the "Use Unicode Character Set" option under General .. Character Set
		flags { "Unicode" }
		configuration { "vs2008" }

			-- dont bother with warnings  with using 'unsafe' strcopy
			defines { "_CRT_SECURE_NO_WARNINGS" }
			flags { "WinMain" }

		-- For this project, no need to differentiate between Debug or Release

	project "keybinder"
		language "C++"
		kind "SharedLib"
		files { "lib/keybinder/include/wx/*.h", "lib/keybinder/src/*.cpp" }

		-- this is needed so that symbols are exported
		defines { "DLL_EXPORTS", "WXMAKINGDLL_KEYBINDER" }
		includedirs { "lib/keybinder/include" }
		configuration { "Debug" }
			wxconfiguration("Debug", _ACTION)
		configuration { "Release" }
			wxconfiguration("Release", _ACTION)
		configuration { "vs2008" }
			-- prevent warning from killing build: warning C4018: '<' : signed/unsigned mismatch
			buildoptions { "/W1" }

	project "keybinder_test"
		language "C++"
		kind "WindowedApp"
		files { "lib/keybinder/sample/*.cpp" }
		includedirs { "lib/keybinder/include" }
		links { "keybinder" }
		configuration "Debug"
			wxconfiguration("Debug", _ACTION)
			wxappconfiguration("Debug", _ACTION)
		configuration "Release"
			wxconfiguration("Release", _ACTION)
			wxappconfiguration("Release", _ACTION)

	project "pelet"
		language "C++"
		kind "SharedLib"
		files {
			"lib/pelet/src/*",
			"lib/pelet/include/**",
			"lib/pelet/*.lua",
			"lib/pelet/src/**.re",
			"lib/pelet/src/**.y",
			"lib/pelet/README.md"
		}
		includedirs { "lib/pelet/include" }
		defines { "PELET_MAKING_DLL" }
		pickywarnings(_ACTION)
		configuration "Release"
			icuconfiguration("Release", _ACTION)
		configuration { "Debug" }
			icuconfiguration("Debug", _ACTION)

	project "pelet_tests"
		language "C++"
		kind "ConsoleApp"
		files {
			"lib/pelet/tests/**.cpp",
			"lib/pelet/tests/**.h"
		}
		includedirs { "lib/pelet/include/", "lib/UnitTest++/src/", "lib/pelet/tests/" }
		links { "pelet", "unit_test++" }

		configuration "vs2008"
			-- dont bother with warnings  with using 'unsafe' fopen
			defines { "_CRT_SECURE_NO_WARNINGS" }

		configuration "Debug"
			pickywarnings(_ACTION)
			icuconfiguration("Debug", _ACTION)
		configuration "Release"
			pickywarnings(_ACTION)
			icuconfiguration("Release", _ACTION)
			
	project "icu_file_tutorial"
		language "C++"
		kind "ConsoleApp"
		files { "tutorials/icu_file_tutorial.cpp" }
		configuration { "Debug" }
			pickywarnings(_ACTION)
			icuconfiguration("Debug", _ACTION)
		configuration { "Release" }
			pickywarnings(_ACTION)
			icuconfiguration("Release", _ACTION)

	project "generic_dir_tutorial"
		language "C++"
		kind "WindowedApp"
		files { "tutorials/generic_dir_tutorial.cpp" }
		configuration "Debug"
			pickywarnings(_ACTION)
			wxconfiguration("Debug", _ACTION)
			wxappconfiguration("Debug", _ACTION)
	configuration "Release"
			pickywarnings(_ACTION)
			wxconfiguration("Release", _ACTION)
			wxappconfiguration("Release", _ACTION)

	project "wx_styled_text_control_tutorial"
		language "C++"
		kind "WindowedApp"
		files { "tutorials/wx_styled_text_control_tutorial.cpp" }
		configuration "Debug"
			pickywarnings(_ACTION)
			wxconfiguration("Debug", _ACTION)
			wxappconfiguration("Debug", _ACTION)
		configuration "Release"
			pickywarnings(_ACTION)
			wxconfiguration("Release", _ACTION)
			wxappconfiguration("Release", _ACTION)
		configuration { "Debug", "vs2008" }
			includedirs { WX_STC_INCLUDE_DIRS }
			links {  "wxmsw28ud_stc" }
		configuration {"Debug", "gmake or codelite"}
			links { "wx_gtk2ud_stc-2.8" }
		configuration { "Release", "vs2008" }
			includedirs { WX_STC_INCLUDE_DIRS }
			links {  "wxmsw28u_stc" }
		configuration {"Release", "gmake or codelite"}
			links { "wx_gtk2u_stc-2.8" }

	project "wx_window_tutorial"
		language "C++"
		kind "WindowedApp"
		files { "tutorials/wx_window_tutorial.cpp" }
		configuration "Debug"
			pickywarnings(_ACTION)
			wxconfiguration("Debug", _ACTION)
			wxappconfiguration("Debug", _ACTION)
		configuration "Release"
			pickywarnings(_ACTION)
			wxconfiguration("Release", _ACTION)
			wxappconfiguration("Release", _ACTION)

	project "wx_aui_tutorial"
		language "C++"
		kind "WindowedApp"
		files { "tutorials/wx_aui_tutorial.cpp" }
		configuration "Debug"
			pickywarnings(_ACTION)
			wxconfiguration("Debug", _ACTION)
			wxappconfiguration("Debug", _ACTION)
		configuration "Release"
			pickywarnings(_ACTION)
			wxconfiguration("Release", _ACTION)
			wxappconfiguration("Release", _ACTION)

	project "soci_tutorial"
		language "C++"
		kind "ConsoleApp"
		files { "tutorials/soci_tutorial.cpp" }
		sociconfiguration()
		configuration "Debug"
			pickywarnings(_ACTION)
		configuration "Release"
			pickywarnings(_ACTION)

	project "running_threads_tutorial"
		language "C++"
		kind "WindowedApp"
		files { 
			"tutorials/running_threads_tutorial.cpp",
			"src/widgets/ThreadWithHeartbeatClass.cpp"
		}
		includedirs {
			"src"
		}
		configuration "Debug"
			pickywarnings(_ACTION)
			wxconfiguration("Debug", _ACTION)
			wxappconfiguration("Debug", _ACTION)
	configuration "Release"
			pickywarnings(_ACTION)
			wxconfiguration("Release", _ACTION)
			wxappconfiguration("Release", _ACTION)
