#Triumph's Build System#

Triumph's build system prepares the dev environment, manages dependencies,
and builds the final distributable pacakges. Triumph is a cross-platform 
application; the build process takes this into account. Further complicating 
things is the fact that there are no cross-platform C++ module / dependency 
managers; meaning we must do a lot of the work ourselves.

#Guidelines#
* Triumph tries to make it easy for a new developer to get started;
  setting up a dev environment should be as automatic as possible.
* Triumph tries to build final distributions that are simple for
  the user to install; they will use the operating system's package
  manager if one exists.

##MSW##
1. The build system will compile all dependant libraries
2. Pre-built binares of dependencies will be hosted in Triumph's servers for 
   future use.
3. Some tools will need to be installed: git, wget, 7zip, but the 
   number should be kept at a minimum.
4. Triumph will be easy to build on MS Windows; this means that 
   the user will NOT be required to install MSYS or Cygwin; Triumph
   can be easily compiled using Visual Studio.
5. The final distribution will not require any other downloads
   apart from the C++ runtime.
6. The final distribution will not need adminstrative privileges
   to run.

##Linux##
1. The build system will require the developer to install the 
   required dev packages, however setting up a dev environment
   should not be more than 1 line of "sudo apt-get install ..."
   or "sudo yum install ..."
2. Triumph will be easy to build on linux using make and GCC.
3. The final distribution will be in DEB or RPM form to make it
   easy on the end user to install.

##Mac OS X##
1. The build system will require the developer to install the 
   required dev packages however setting up a dev environment
   should not be more than 1 line of "sudo port install ..."
   or "brew install ..."
2. Triumph will be easy to build on Mac OS X using make and
   CLANG.
3. The final distribution will be in DMG form to make it
   easy on the end user to install.

##FAQ##
1. Why do the premake scripts read options from environment variables?
The reason for this is that when we make final distributable
packages, the location of assets and shared libraries on the machines
where triumph will be installed on are different than the location
of the libs / assets of the dev machines. 