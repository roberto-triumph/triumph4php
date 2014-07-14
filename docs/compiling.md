[Table Of Contents](/#toc) 

  * [MS Windows Instructions](/compiling/#msw)
  * [Ubuntu Instructions](/compiling/#ubuntu)
  * [Fedora Code Instructions](/compiling/#fedora) 


triumph4php uses premake for build/configuration.  Premake makes it easier to perform cross-platform 
development between Linux and Windows by generating Codelite / Makefile / Visual Studio Solution Files. 

#Project Dependencies#
 * [wxWidgets](http://www.wxwidgets.org) - This is the GUI toolkit used. Currently using version 2.9.5.
 * [ICU](http://site.icu-project.org/) - This is a Unicode string library for C++.  Currently using version 4.2.1.
 * [re2c](http://re2c.org/) - This is a lexer generator that is used to generate the PHP tokenizer. Currently using version 0.13.5.
 * [Bison](http://www.gnu.org/software/bison/) -  This is a parser generator used to generate the PHP parser. Currently using version 2.4.1.
 * [SOCI](http://soci.sourceforge.net/) - A C++ database access library. Currently using a snapshot version (NOT the stable release; the stable release is quite dated).
 * [MySQL Connector C](http://dev.mysql.com/downloads/connector/c/). The MySQL C (not C++) Client library. Currently using version 6.0.2
 * [CMake](http://www.cmake.org/): CMake is a cross-platform build system. triumph4php itself won't use CMake, some of the required libraries (SOCI) use it as a build system.
 * [SQLite](http://www.sqlite.org)  triumph4php uses SQLite to store parsed classes and methods.
 * [CURL](http://curl.haxx.se/) triumph4php uses curl to check for new versions of triumph4php
 * [Git Client](http://git-scm.com/): triumph4php uses Git for source control.
 * [Boost ASIO](http://think-async.com/): triumph4php uses the ASIO library to open socket connections to listen for Xdebug callbacks.

#Development Tools#
   * [codelite](http://codelite.org) - C++ IDE for Linux
   * [Visual Studio Express 2008](http://go.microsoft.com/?linkid=7729279‎) - C++ IDE for Windows
   * [valgrind](http://valgrind.org/) - profiling tools for Linux
   * [Dr. Memory](http://www.drmemory.org/) - profiling tools for Windows
   * [Very Sleepy](http://www.codersnotes.com/sleepy) - an easy-to-use profiler for Windows
   * [wxFormBuilder](http://wxformbuilder.org) - This is the WYSIWYG tool for rapid form creation. Currently using version 3.1.57.


<a id="msw"></a>
#MS Windows#

##Step 1: Download Visual Studio 2008 Express##
From [Visual Studio Express 2008](http://go.microsoft.com/?linkid=7729279‎)

##Step 2: Download external tools##
triumph4php has a build script that will automatically download and compile all needed dependencies.
However, this script needs some external programs: wget, 7zip, cmake, git.
You can set the proper location of the external programs by editing
premake_opts_windows.lua. Or if you don't have them you will have to
install them.  One nice way to install them is by using Chocolatey for windows

    @powershell -NoProfile -ExecutionPolicy unrestricted \
        -Command "iex ((new-object net.webclient).DownloadString('https://chocolatey.org/install.ps1'))" \ 
         && SET PATH=%PATH%;%systemdrive%\\chocolatey\\bin
    cinst git
    cinst cmake
    cinst 7zip.commandline
    cinst wget

##Step 3: Download the triumph4php repo##

    git clone https://code.google.com/p/triumph4php triumph4php
	cd triumph4php
	
##Step 4: Run the build script that sets up the dependent libraries##

    premake4.exe setupdev
	
The setupdev script will download the git submodules, download
libraries that don't have git repos, and compile all of the 
dependent libraries.  Note: The setupdev command will take a
long time; it may take a good 30 min to an hour depending on your
Internet connection speed and machine processor power.

## Step 5: Build triumph4php##

    premake4.exe vs2008
	
Now go into the build/vs2008 directory, you should see a Visual Studio solution file. Open
the solution file and compile. This will also take a good 30 minutes
to an hour, depending on your machine's processing power.

<a id="ubuntu"></a>
#Ubuntu (12.04 LTS)#

##Step 1: Download dependent packages##

    sudo apt-get install libicu48 libicu-dev libicu48-dbg libmysqlclient-dev \
	                     re2c bison CMake git libtool php5-cli \
						ia32-libs sqlite3 libsqlite3-dev libsqlite3-0-dbg \
						libcurl3-dev libgtk2.0-dev libasio-dev libboost-system-dev
						
##Step 2: Download the development tools##
						
    sudo apt-get install codelite wxformbuilder

##Step 3: Download the triumph4php repo##

    git clone https://code.google.com/p/triumph4php triumph4php
	cd triumph4php
	
##Step 4: Run the build script that sets up the dependent libraries##

    ./premake4 setupdev
	
The setupdev script will download the git submodules and compile 
them.  Note: The setupdev command will take a long time; it may take a 
good 30 min to an hour depending on your internet connection speed 
and machine processor power.

## Step 5: Build triumph4php##

    ./premake4 codelite
	
OR, if you want to use Makefiles

    ./premake4 gmake
	
Now go into the build/ directory, you should see a either a Codelite directory or a
gmake directory. Open the Codelite workspace and compile, or use Make
to compile triumph4php. This will also take a good 30 minutes
to an hour, depending on your machine's processing power.

<a id="fedora"></a>
#Fedora Core (20)#

##Step 1: Download dependent packages##

    sudo yum install libicu libicu-devel community-mysql-common \
	community-mysql-devel re2c bison cmake git php-cli php-pdo \
    glibc.i686 sqlite sqlite-devel libcurl libcurl-devel \
    gtk2-devel

##Step 2: Download the development tools##
Use your favorite IDE / editor, or get [Codelite](http://codelite.org)
and [wxFormBuilder](http://wxformbuilder.org)						


##Step 3: Download the triumph4php repo##

    git clone https://code.google.com/p/triumph4php triumph4php
	cd triumph4php
	
##Step 4: Run the build script that sets up the dependent libraries##

    ./premake4 setupdev
	
The setupdev script will download the git submodules and compile 
them.  Note: The setupdev command will take a long time; it may take a 
good 30 min to an hour depending on your Internet connection speed 
and machine processor power.

## Step 5: Build triumph4php##

    ./premake4 codelite
	
OR, if you want to use Makefiles

    ./premake4 gmake
	
Now go into the build/ directory, you should see a either a Codelite directory or a
gmake directory. Open the Codelite workspace and compile, or use Make
to compile triumph4php. This will also take a good 30 minutes
to an hour, depending on your machine's processing power.

[Table Of Contents](/#toc) 