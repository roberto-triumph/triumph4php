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
dofile "premake_action_wxwidgets.lua"


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
		-- wxwidgets uses "unsafe" functions, kill these warnings
		-- although the flag is technically not needed since wx/defs.h sets the #define, 
		-- we do it here because not all profiler programs include the entire wxWidgets framework
		defines { "WIN32", "_DEBUG", "_WINDOWS", "WXUSINGDLL", "_CRT_SECURE_NO_WARNINGS" }

		-- enable the "Use Unicode Character Set" option under General .. Character Set
		-- wxWidgets needs this in order to link properly
		flags { "Unicode" }
		links { WX_LIBS_DEBUG }
	elseif config == "Debug" and (action == "gmake" or action == "codelite") then
		buildoptions { string.format("`%s --cxxflags --debug=yes --unicode=yes`", normalizepath(WX_CONFIG)) }
		linkoptions { string.format("`%s --debug=yes --unicode=yes --libs core,base,net`", normalizepath(WX_CONFIG)) }
	elseif config == "Release" and action ==  "vs2008" then
		libdirs { WX_LIB_DIR }
		includedirs { WX_INCLUDE_DIRS_RELEASE }

		-- wxWidgets framework needs these
		-- tell wxWidgets to import DLL symbols
		-- wxwidgets uses "unsafe" functions, kill these warnings
		defines { "WIN32", "_WINDOWS", "__WXMSW__", "WXUSINGDLL", "NDEBUG", "NOPCH", "_CRT_SECURE_NO_WARNINGS" }

		-- enable the "Use Unicode Character Set" option under General .. Character Set
		-- wxWidgets needs this in order to link properly
		flags { "Unicode", "Optimize" }
		links { WX_LIBS_RELEASE }
	elseif config == "Release" and (action == "gmake" or action == "codelite") then
		buildoptions { string.format("`%s --cxxflags --debug=no --unicode=yes`", normalizepath(WX_CONFIG)) }
		linkoptions { string.format("`%s --debug=no --unicode=yes --libs core,base,net`", normalizepath(WX_CONFIG)) }
	end
end

function wxappconfiguration(config, action)

	if action == "vs2008" then

		-- prevent the  "error LNK2019: unresolved external symbol _main referenced in function ___tmainCRTStartup
		flags { "WinMain" }
		resdefines { "__WXMSW__", "NDEBUG", "_UNICODE", "WXUSINGDLL", "_WINDOWS", "NOPCH" }
		resincludedirs { "src/" }
	end

	if config == "Debug" and (action == "gmake" or action == "codelite") then
		linkoptions { string.format("`%s --debug=yes --unicode=yes --libs aui,adv,stc`", normalizepath(WX_CONFIG)) }
	elseif config == "Debug" and action ==  "vs2008" then
		links { WX_LIBS_WINDOW_DEBUG, WX_LIB_STC_DEBUG }
	elseif config == "Release" and (action == "gmake" or action == "codelite") then
		linkoptions { string.format("`%s --debug=no --unicode=yes --libs aui,adv,stc`", normalizepath(WX_CONFIG)) }
	elseif config == "Release" and action ==  "vs2008" then
		links { WX_LIBS_WINDOW_RELEASE, WX_LIB_STC_RELEASE }
	end
end

function sociconfiguration(config)
	libdirs {
		MYSQL_LIB_DIR,
		SQLITE_LIB_DIR
	}
	if os.is "windows" then 
		includedirs {
			"lib/soci/mvc-editor/include",
			"lib/soci/mvc-editor/include/soci",
			MYSQL_INCLUDE_DIR,
			SQLITE_INCLUDE_DIR
		}
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
		if config == "Debug" then
			includedirs {
				"lib/soci/mvc-editor/Debug/include",
				"lib/soci/mvc-editor/Debug/include/soci",
				MYSQL_INCLUDE_DIR,
				SQLITE_INCLUDE_DIR
			}
		end
		if config == "Release" then
			includedirs {
				"lib/soci/mvc-editor/Release/include",
				"lib/soci/mvc-editor/Release/include/soci",
				MYSQL_INCLUDE_DIR,
				SQLITE_INCLUDE_DIR
			}
		end

		-- soci creates lib directory with the architecture name
		if (os.isdir("lib/soci/mvc-editor/Debug/lib64") and config == "Debug") then
			libdirs { "lib/soci/mvc-editor/Debug/lib64" }
		elseif (os.isdir("lib/soci/mvc-editor/Release/lib64") and config == "Release") then
			libdirs { "lib/soci/mvc-editor/Release/lib64" }
		elseif (os.isdir("lib/soci/mvc-editor/Debug/lib") and config == "Debug") then
			libdirs { "lib/soci/mvc-editor/Debug/lib" }
		elseif (os.isdir("lib/soci/mvc-editor/Release/lib") and config == "Release") then
			libdirs { "lib/soci/mvc-editor/Release/lib" }
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

-- the curl configuration
-- for MSW, we use the precompiled binaries
-- for linux, we use the system wide version
function curlconfiguration(action)
	if action == "vs2008" then
		includedirs {
			"lib/curl/include"
		}
		links {
			"curl"
		}
	elseif action == "gmake" or action == "codelite" then
		includedirs {
			CURL_INCLUDE_DIR
		}
		libdirs {
			CURL_LIB_DIR
		}
		links {
			"curl"
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
		
		if os.is "windows" then
			files { "src/**.cpp", "src/**.h", "*.lua", "README.md", "src/MvcEditor.rc" }
		else 
			files { "src/**.cpp", "src/**.h", "*.lua", "README.md" }
		end
		includedirs { 
			"src/", 
			"lib/keybinder/include/", 
			"lib/pelet/include"
		}
		links { "tests", "keybinder", "pelet" }

		configuration "Debug"
			pickywarnings(_ACTION)
			sociconfiguration("Debug")
			icuconfiguration("Debug", _ACTION)
			wxconfiguration("Debug", _ACTION)
			wxappconfiguration("Debug", _ACTION)
			curlconfiguration(_ACTION)
			
			-- use the local update server in debug  
			defines { 
				string.format("MVCEDITOR_UPDATE_HOST=%s", 'updates.localhost')
			}

		configuration "Release"
			pickywarnings(_ACTION)
			sociconfiguration("Release")
			icuconfiguration("Release", _ACTION)
			wxconfiguration("Release", _ACTION)
			wxappconfiguration("Release", _ACTION)
			curlconfiguration(_ACTION)
			
			-- use the public update server in release
			defines { 
				string.format("MVCEDITOR_UPDATE_HOST=%s", 'updates.mvceditor.com')
			}

	project "tests"
		language "C++"
		kind "ConsoleApp"
		files {
			"tests/**.cpp",
			"tests/**.h",
			"src/globals/*.cpp",
			"src/actions/*.cpp",
			"src/language/*.cpp",
			"src/search/*.cpp",
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

		configuration "Debug"
			pickywarnings(_ACTION)
			sociconfiguration("Debug")
			icuconfiguration("Debug", _ACTION)
			wxconfiguration("Debug", _ACTION)
		configuration "Release"
			pickywarnings(_ACTION)
			sociconfiguration("Release")
			icuconfiguration("Release", _ACTION)
			wxconfiguration("Release", _ACTION)

	project "tag_finder_profiler"
		language "C++"
		kind "ConsoleApp"
		files {
			"profilers/tag_finder_profiler.cpp",
			"src/globals/Errors.cpp",
			"src/globals/Assets.cpp",
			"src/globals/Sqlite.cpp",
			"src/globals/TagClass.cpp",
			"src/language/*.cpp",
			"src/search/*.cpp",
			"src/globals/String.cpp"
		}
		includedirs { "src", "lib/pelet/include" }
		links { "pelet" }

		configuration "Debug"
			pickywarnings(_ACTION)
			sociconfiguration("Debug")
			icuconfiguration("Debug", _ACTION)
			wxconfiguration("Debug", _ACTION)
		configuration { "Release"}
			pickywarnings(_ACTION)
			sociconfiguration("Release")
			icuconfiguration("Release", _ACTION)
			wxconfiguration("Release", _ACTION)


	project "call_stack_profiler"
		language "C++"
		kind "ConsoleApp"
		files {
			"profilers/call_stack_profiler.cpp",
			"src/language/*.cpp",
			"src/globals/*.cpp",
			"src/search/*.cpp",
			"src/actions/ActionClass.cpp",
			"src/actions/TagDetectorActionClass.cpp",
			"src/actions/ActionClass.cpp",
			"src/actions/GlobalActionClass.cpp",
			"src/widgets/ProcessWithHeartbeatClass.cpp"
		}
		includedirs { "src", "lib/pelet/include" }
		links { "pelet" }

		configuration "Debug"
			pickywarnings(_ACTION)
			sociconfiguration("Debug")
			icuconfiguration("Debug", _ACTION)
			wxconfiguration("Debug", _ACTION)
		configuration { "Release"}
			pickywarnings(_ACTION)
			sociconfiguration("Release")
			icuconfiguration("Release", _ACTION)
			wxconfiguration("Release", _ACTION)
                        
	project "action_profiler"
		language "C++"
		kind "WindowedApp"
		files {
			"src/actions/*.cpp",
			"src/globals/*.cpp",
			"src/language/*.cpp",
			"src/search/*.cpp",
			"src/widgets/ProcessWithHeartbeatClass.cpp",
			"profilers/action_profiler.cpp"
		}
		includedirs { "src", "lib/pelet/include" }
		links { "pelet" }
		
		configuration "Debug"
			pickywarnings(_ACTION)
			sociconfiguration("Debug")
			icuconfiguration("Debug", _ACTION)
			wxconfiguration("Debug", _ACTION)
			wxappconfiguration("Debug", _ACTION)
		configuration { "Release" }
			pickywarnings(_ACTION)
			sociconfiguration("Release")
			icuconfiguration("Release", _ACTION)
			wxconfiguration("Release", _ACTION)
			wxappconfiguration("Release", _ACTION)

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
			"src/widgets/ProcessWithHeartbeatClass.cpp",
			"src/widgets/StatusBarWithGaugeClass.cpp",
			"src/globals/*.cpp",
			"src/language/*.cpp",
			"src/search/*.cpp",
			"src/actions/ActionClass.cpp"
		}
		includedirs { "src/", "lib/pelet/include" }
		links { "pelet" }
		
		configuration "Debug"
			pickywarnings(_ACTION)
			sociconfiguration("Debug")
			wxconfiguration("Debug", _ACTION)
			wxappconfiguration("Debug", _ACTION)
			icuconfiguration("Debug", _ACTION)
		configuration "Release"
			pickywarnings(_ACTION)
			sociconfiguration("Release")
			wxconfiguration("Release", _ACTION)
			wxappconfiguration("Release", _ACTION)
			icuconfiguration("Release", _ACTION)

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
		configuration { "gmake or codelite" }
			-- prevent warning: deprecated stuff from wxWidgets 2.8 -> 2.9
			buildoptions { "-Wno-deprecated" }

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
		configuration { "gmake or codelite" }
			-- prevent warning: deprecated conversion from string constant to char*
			buildoptions { "-Wno-write-strings" }

	
	if (os.is("windows")) then 
		project "curl"
			language "C++"
			kind "SharedLib"
			includedirs {
				"lib/wxcurl/thirdparty/curl/include",
				"lib/wxcurl/thirdparty/curl/lib/"
			}
			files {
				"lib/wxcurl/thirdparty/curl/lib/*"
			}
			defines {
				"NDEBUG",
				"BUILDING_LIBCURL"
			}
			configuration "vs2008"
				defines {
					"WIN32",
					"_LIB",
					"CURL_LDAP_WIN"
				}
				links {
					"ws2_32",
					"wldap32"
				}
	end
	
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
		
		configuration "Debug"
			pickywarnings(_ACTION)
			sociconfiguration("Debug")
		configuration "Release"
			pickywarnings(_ACTION)
			sociconfiguration("Release")

	project "running_threads_tutorial"
		language "C++"
		kind "WindowedApp"
		files { 
			"tutorials/running_threads_tutorial.cpp",
			"src/actions/ActionClass.cpp"
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
