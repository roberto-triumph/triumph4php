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
You can use any source code editor that can handle any of those files.

##Describe Triumph's architecture in as few words as possible##
Basically, it goes like this:

View => Feature ==> Action (optional) ==> Globals

Triumph has 4 main subjects: views, features, actions, and globals.
A __view__ is the piece of code that displays a dialog, describes menu items, toolbar
buttons, keyboard shortcuts, and other UI aspects for a single feature. A __feature__ 
is a single discrete, user-noticeable functionality (like say, find in files). An
__action__ is a piece of code that is executed in a background thread.  A __global__
is the "domain model" of Triumph; for example a class that represents a project, 
a class that represents a tag (parsed class / methods), and so on.

The user (PHP coder) generates a UI event like a choosing a menu item or pressing a keyboard
shortcut.  The views listen for UI events; the view then calls the appropriate
method on a feature. The features then modifies the appropriate globals classes or
triggers new Action instances to be run in the background. If an Action generates 
events in the background, both the feature or the view may respond to them. 

###Example: New Project###

1. PHP code activates the File ... New Project menu
2. The Project view responds to the menu event by showing the PHP coder 
   a dialog where they choose the root of the new project.
3. Once the PHP coder selects the directory, the project view then tells
   the project feature to create a new project. The project feature responds
   by creating a new "global" project, adding it to the projects list, persisting
   the project in the config file.

###Example: Find In Files###

1. PHP coder activates the Search...Find in files menu
2. The FindInFiles view responds to the menu event by showing the find in files dialog.
3. PHP coder enters the term to search for and what directory to search in.
4. The FindInFiles view them tells the FindInFiles feature to start a search
   of the chosen search term in the chosen directory.
5. The FindInFiles feature then creates a new action FindInFilesBackgroundReader to 
   perform file searching in a background thread.
6. The FindInFilesBackgroundReader action communicates with the FindInFiles feature
   by "posting" events to the FindInFiles feature.
7. The FindInFiles view responds to the FindInFilesBackgroundReader events by
   updating the search results panel.

##I just cloned the repo and compiled Triumph. Any pointers about next steps?##

* Look at the tutorial projects; wx_window_tutorial, soci_tutorial, and the others 
  to understand how each of the different libraries work.
* Look at src/features/TestFeatureClass.cpp.  This class is a stripped-down, bare-bones
  feature that shows how you can create toolbar buttons or menu items.

##How is the source organized##

* __ROOT__ of the project contains the build configuration (premake) files.

* __Debug, Release__  hold the results of compilation (object files, executables).

* __assets__  holds images used in Triumph, as well as the SQLite database that
  holds the docs for the native PHP functions.

* __build__ holds the results of premake. Here you will find the generated
  solution files / Makefiles.

* __docs__ contains the user manual, in markdown format.

* __lib__ contains the libraries that Triumph uses. This directory
  is mostly empty in source control; most libraries are brought in via Git 
  sub-modules.
 
* __package__ contains files for generating the final package files (RPM, DEB).

* __php_samples__ contains PHP files that are useful when testing Triumph.
  For example, there is a PHP file that contains UTF-8 characters, for
  use in testing to make sure Triumph can render UTF-8 properly.

* __profilers__ contains a couple of 1-file programs that exercise 
  crucial editor functionality, like PHP parsing entire directory or
  searching of an entire directory. These programs are usually run
  under a profiler like valgrind or Very Sleepy.

* __src__ contains the C++ source code.    

* __tests__ contains Triumph unit tests.  The directory structure
  mirrors the structure of the src directory. The tests/ directory
  contain the test runner and a couple of fixture classes, and the 
  tests themselves are located inside the sub-directories.

* __tutorials__ contains several small 1-file programs that I initially
  used when starting Triumph.  For example, the tutorials directory contains
  a very simple wxWidgets program, a very simple program that uses SOCI, and
  a simple program that uses ICU.  These 1-file programs help when learning
  the libraries and they also aid in debugging; you can quickly test things
  out to see if there is a bug in one of the libraries.

* __src/actions__ contains classes that execute logic in background threads. Triumph utilizes
threads in many places so that we keep the main thread from being too
busy.

* __src/code_control__ contains classes related to managing and configuring Scintilla. Scintilla handles 
a multitude of languages, but we must configure it to turn functionality on or
off depending on what language the file contains.

* __src/features__ Features are distinct, user-noticeable functionality. For example, there is a 
finder feature which takes care of searching for a string in text, there is a project feature that
takes of storing project source directories. Each feature is pretty isolated from
all others and does not interact directly with another feature. A feature does NOT contain any UI
related code.

* __src/views__ Views are the GUI for a single feature. Each feature has 1 and only 1 view. For 
example, the finder feature has a a finder view, the project feature has a project view, and 
so on. Views are where the GUI logic lives; views add menu items, buttons, panels, and
keyboard shortcuts.  They also bring up dialogs.

* __src/views/wxformbuilder__ contains files with extensions .fbp, .cpp, or .h.  The 
.fbp files are wxformbuilder files; they contain the designs of the features. There is 1 
.fbp file per feature; each feature may have multiple panels and dialogs in the same design 
file.  The .cpp and .h files are generated from the design files.  The C++ files in this directory 
are never manually edited; they are always generated when the design changes.

* __src/globals__ holds classes that are shared between features; globals are the "domain
models" of Triumph.  For example, there is a ProjectClass that contains project
info (root directory), and there is a TagClass that holds the tag (parsed PHP
class name / filename).

* __src/language__ Anything that is PHP-specific is put in this directory. PHP linters, 
PHP tag cache, and PHP debugger objects are all here.

* __src/main_frame__ The main frame is the Triumph's top-level window; it will display all of the 
toolbars and menus. Also, the top-level preferences panel is stored in this directory.

* __src/main_frame/wxformbuilder__ contains files with extension .fbp, .cpp, and .h.  The 
.fbp file is a wxformbuilder file; it contains the design of the main frame.  The .cpp 
and .h files are generated from  the design file.  The C++ files in this directory are 
never manually edited; they are always generated when the design changes.

* __src/search__ contains search and IO-related classes; classes to search a string, recurse a directory.

* __src/widgets__ contains UI-specific classes that are shared among many features; for example file picker
validators, string validators.

* __src/widgets/wxformbuilder__ contains files with extension fb.p, .cpp, and .h.  The 
.fbp file is a wxformbuilder file; it contains the design of widgets.  The .cpp and 
.h files are generated from  the design files.  The C++ files in this directory are never 
manually edited; they are always generated when the design changes.

##I've got a clear understanding of Triumph's architecture, what now?##
Stay tuned for docs that will examine the build system, the action (thread pool),
and the feature system in depth. In the mean, you can ask questions in the [forums](http://support.triumph4php.com/).