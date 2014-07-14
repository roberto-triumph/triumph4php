[Table Of Contents](/#toc) | [Next - new user wizard](/new-user-wizard/)

# System Requirements #
The following software is required to run triumph4php 

  1. Operating System:  triumph4php  can run on Windows XP, Windows Vista, Windows 7, most Linux distributions 
     (Ubuntu, Fedora, SusE).
  2. PHP (5+). While not having PHP will not prevent the Editor from starting up and being able to edit 
     files, not having PHP installed will render some of the editor's features unusable. For example, the 
    "Run" feature needs the PHP interpreter to run a PHP script.
  3. The MS Windows version requires that the Visual C++ 2008 SP1 Redistributable (vcredist_x86.exe) be 
     installed on the target machine. It can be downloaded from the 
    [Microsoft web site](http://www.microsoft.com/en-us/download/details.aspx?id=26368)
  4. The Linux version requires the following libraries be installed. These can
     usually be installed fairly easily from the package manager (yum, apt)
      - MySQL client library libmysqlclient
	  - SQLite client library libsqlite3
	  - ICU libicu
	  - CURL libcurl

# Hardware Requirements #
triumph4php will take moderate resources on the system.

# Installation #
The latest version of triumph4php is available in binary form at the project's [main site](http://triumph4php.com).  
The source code is also made available in the same place. You can choose to compile from source if you wish but you will 
need to [compile from source](/compiling/). 

# Installation On MS Windows Systems #
The Windows version of triumph4php does not have an installation process; it is available as a standalone executable 
that can be run on its own. Just download the distribution archive, extract, and run the triumph4php binary. The
downloaded archive is a 7-zip archive (not a regular zip file). On you extract the archive, triumph4php.exe is
located inside the bin directory.

# Installation On Linux Systems #
There are Debian and RPM archives of triumph4php available at the triumph4php main site. Once downloaded,
the Debian package can be run via `dpkg -i` and the RPM can be installed via `rpm -i`

##Installation On Debian-based Systems##

    sudo apt-get install libmysqlclient18 sqlite3 libicu48 libcurl3
	dpkg -i triumph4php_0.4_amd64.deb

##Installation On Red Hat-based Systems##

    sudo yum install libicu community-mysql-common sqlite
    rpm -iVh triumph4php_0.4-1.fc20.x86_64.rpm

##Installation On Other Linux Systems##

triumph4php will run on most other Linux distributions, but you will need to compile it from source.
See [compiling](/compiling/) for more info.

[Table Of Contents](/#toc) | [Next - new user wizard](/new-user-wizard/)
