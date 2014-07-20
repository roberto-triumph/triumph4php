#Triumph Architecture#

This document describes the architecture of Triumph's source code. The document
is intended for people who want to contribute to Triumph. This is a very high-level
summary as I don't want it to become obsolete right away.

##What Language is Triumph written in?##
Triumph is an application mostly written in C++, with a bit of XML, PHP, SQL, and Lua.
The breakdown, as of July 214, is as follows: 

<table>
<tr>
	<th>Language</th>
	<th>files</th>
	<th>blank</th>
	<th>comment</th>
	<th>code</th>
</tr>
<tr><th>C++                 </td><td>   173   </td><td>   9371    </td><td>    8073   </td><td>   59769</td> </tr>
<tr><th>PHP                 </td><td>   110   </td><td>   2691    </td><td>   11709   </td><td>   13560</td> </tr>
<tr><th>C/C++ Header        </td><td>   139   </td><td>   5789    </td><td>   15674   </td><td>    9361</td> </tr>
<tr><th>Lua                 </td><td>    16   </td><td>    270    </td><td>     787   </td><td>    1633</td> </tr>
<tr><th>XML                 </td><td>    27   </td><td>      0    </td><td>       0   </td><td>    1198</td> </tr>
<tr><th>SQL                 </td><td>     2   </td><td>    112    </td><td>     409   </td><td>     126</td> </tr>
<tr><th>SUM:                </td><td>   467   </td><td>  18233    </td><td>   36652   </td><td>   85647</td> </tr>
</tr>
</table>

Don't worry, the XML is not hand-written :-)

##What libraries does Triumph use?##
Triumph uses the following libraries:

* __wxWidgets__
  is the main library that Triumph uses.  wxWidgets is a cross-platform
  GUI toolkit; it handles all aspects of window creation and user input. wxWidgets
  is a mature, stable toolkit.

* __Scintilla__
  is the source code editing component; it contains mechanisms for
  syntax highlighting, auto completion, call tips.  Scintilla is the control
  that uses interact with the most; it's where the users type in their code 
  into.

* __SOCI__
  is a database wrapper library for C++. It is a (small) abstraction 
  layer over MySQL and SQLite drivers.

* __SQLite__
  is used to store the parsed PHP class names, method names, and
  files.  Triumph uses SQLite database for fast source code lookups.

* __libmysql__
  is used to implement the SQL Browser (run queries against a MySQL database).

* __ICU__
  is a C++ string library that specifically supports the Unicode standard.

* __cURL__
  is a C++ library for creating client HTTP connections. Triumph uses cURL
  to check for new Triumph versions.

* __Boost ASIO__
  is a C++ socket library. Triumph uses the ASIO library to start server sockets to
  accept Xdebug connections.

##What tools does Triumph use##
Triumph developers also use the following programs.

* __re2c__
  is a lexer generator. Triumph uses re2c to build PHP tokenizers. The PHP 
  tokenizers are used by the parsers.

* __Bison__
  is a parser generator. Triumph uses bison to generate the PHP parsers.
  Triumph uses the PHP parsers for linting PHP source code and for indexing
  source code.

* __premake__
	is a build configuration tool.  Premake can generate Makefiles, or
  Visual Studio solution files.  It also serves as the entry point into the
  build system.  premake is a stand-alone binary; premake is configured
  via lua files.

* __wxFormBuilder__
  wxFormBuilder is a RAD (Rapid Application Development) program.  It is used
  to build most of the GUI (dialogs and panels). wxFormBuilder is a real
  time-saver because you can build dialogs pretty quickly with it.  wxFormBuilder
  creates C++ code from the designs that you create. wxFormBuilder stores its
  designs in XML (thankfully, you never have to see the XML).

##What editor do I have to use?##
Because Triumph uses premake, we are able to create Visual Studio solution files
(including Express editions), GNU Makefiles, CodeLite, and Code::Blocks workspace files. 
Any code editor that can handle any of those files.

##Explain Each directory##

* __ROOT__
  The root directory of the project contains the build configuration (premake) files.

* __Debug, Release__
  these directories hold the results of compilation (object files).

* __assets__
  This directory holds images used in Triumph, as well as the SQLite database that
  holds the docs for the native PHP functions.

* __build__
  This directory holds the results of premake. Here you will find the generated
  solution files / Makefiles.

* __docs__
  This directory contains the user manual.

* __lib__
  This directory contains the dependant libraries that Triumph uses. This directory
  is mostly empty in source control; most libraries are brought in via Git 
  sub-modules.
 
* __package__
  This directory contains files for generating the final package files (RPM, DEB).

* __php_samples__
  This directory contains PHP files that are useful when testing Triumph.
  For example, there is a PHP file that contains UTF-8 characters, for
  use in testing to make sure Triumph can render UTF-8 properly.

* __profilers__
  This directory contains a couple of 1-file programs that exercise 
  crucial editor functionality, like PHP parsing entire directory or
  searching of an entire directory. These programs are usually run
  under a profiler like valgrind or Very Sleepy.

* __src__
  This directory contains the C++ source code.    

* __tests__
  This directory contains Triumph unit tests.  The directory structure
  mirrors the structure of the src directory. The tests/ directory
  contain the test runner and a couple of fixture classes, and the 
  tests themselves are located inside the sub-directories.

* __tutorials__
  This directory contains several small 1-file programs that I initially
  used when starting Triumph.  For example, the tutorials directory contains
  a very simple wxWidgets program, a very simple program that uses SOCI, and
  a simple program that uses ICU.  These 1-file programs help when learning
  the libraries and they also aid in debugging; you can quickly test things
  out to see if there is a bug in one of the libraries.


##How is the source organized##

* __actions__
Contains classes that execute logic in background threads. Triumph utilizes
threads in many places so that we keep the main thread from being too
busy.

* __code_control__
Contains classes related to managing and configuring Scintilla. Scintilla handles 
a multitude of languages, but we must configure it to turn functionality on or
off depending on what language the file contains.

* __features__
A feature is a distinct, user-noticeable functionality. For example, there is a finder feature which
takes care of showing the finder panel, there is a project feature that
takes of showing the projects dialogs. Each features is pretty isolated from
all others and does not interact directly with another feature.

* __features/wxformbuilder__
This directory contains files with extension fbp, cpp, and h.  The fbp files are wxformbuilder
files; they contain the designs of the features, There is 1 fbp file per feature; each feature
may have multiple panels and dialogs in the same design file.  The cpp and h are generated from 
the design files.  The C++ files in this directory are never manually edited; they are
always generated when the design changes.

* __globals__
Globals are classes that are shared between features; globals are the "domain
models" of Triumph.  For example, there is a ProjectClass that contains project
info (root directory), and there is a TagClass that holds the tag (parsed PHP
class name / filename).

* __language__
Anything that is PHP-specific. PHP linters, PHP tag cache, PHP debugger objects
are all here.

* __main_frame__
The main frame is the Triumph's top-level window; it will display all of the 
toolbars and menus.

* __main_frame/wxformbuilder__
This directory contains files with extension fbp, cpp, and h.  The fbp file is a wxformbuilder
file; it contains the design of the main frame.  The cpp and h are generated from 
the design file.  The C++ files in this directory are never manually edited; they are
always generated when the design changes.

* __search__
search and IO-related classes; classes to search a string, recurse a directory.

* __widgets__
UI-specific classes that are shared among many features; for example file picker
validator, string validators.

* __widgets/wxformbuilder__
This directory contains files with extension fbp, cpp, and h.  The fbp file is a wxformbuilder
file; it contains the design of widgets.  The cpp and h are generated from 
the design files.  The C++ files in this directory are never manually edited; they are
always generated when the design changes.


##Describe Triumph's architecture in as few words as possible##
Basically, it goes like this:

User performs an action, like a choosing a menu item or pressing a keyboard
shortcut.  The features listen for these UI events; the features then modify
the appropriate globals classes or bring up new dialogs.  The features may also
trigger new Action instances due to user UI events.



