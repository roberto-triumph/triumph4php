# triumph4php manual #

# Introduction #
The idea for triumph4php was borne out of a need to remove the tediousness out of PHP programming. PHP itself is a 
simple language that is easy to learn; however there currently does not exist an editor that is tailor-made for it.  

## Yet Another Editor? ##
There are many, many editors that people use to develop their PHP applications.  This fact alone highlights the 
need for a tight, consistent PHP development environment. Most editors can be customized to a degree to support 
PHP; but since they were not built with PHP in mind the customization still does not create a tight, controlled 
interface.

## Core Values ##
  1. This will be an editor designed for PHP web applications.  It will recognize and handle dependencies on a 
     web server and a RDBMS. It will handle issues that are common to PHP web applications such as URL routing, 
	 SQL queries, HTML escaping, and model / controller / view separation.
  2. The editor will support many of the current generation of convention-over-configuration frameworks.  The editor
     will recognize that a project built using a framework; and will use knowledge of the framework to perform tasks 
	 such as opening database connections and resolve URL mappings.
  3. The editor will have a simple interface; it will let the user focus on writing the application and not clutter 
     the tool bars and menu bars with options that are seldom used.

## Project License ##
triumph4php is distributed under the terms of the MIT license, meaning that it is free to download, use and modify.
However, the software is provided as-is, and without any warranty. See the 
[full license](http://www.opensource.org/licenses/mit-license.php) for more details.

## Audience ##
This manual is intended for advanced computer users - software developers that are very familiar with other Integrated 
Development Environments (Eclipse, Netbeans, etc...).  If, after reading this manual, you still have questions, 
please ask a question in the [support forums](http://support.triumph4php.com/forums).

## System Requirements ##
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

## Hardware Requirements ##
triumph4php will take moderate resources on the system.

# Installation #
The latest version of triumph4php is available in binary form at the project's [main site](http://triumph4php.com).  
The source code is also made available in the same place. You can choose to compile from source if you wish but you will 
need to setup your developer environment. 

# Installation On MS Windows Systems #
The Windows verson of triumph4php does not have an installation process; it is available as a standalone executable 
that can be run on its own. Just download the distribution archive, extract, and run the triumph4php binary.

# Installation On Linux Systems #
There are debian and RPM archives of triumph4php available at the triumph4php main site. 

# Features: #
  * [New User Wizard](/new-user-wizard)
  * [Project Management](/projects)
  * [Total Search](/total-search)
  * [PHP Linter](/php-linter)
  * [PHP Code Completion](/php-code-completion)
  * [Find In Files](/find-in-files)
  * [Find](/find)
  * [SQL Browser](/sql-browser)
  * [Database Connections](/database-connections)
  * [PHP Doc Comments](/php-doc-comments)

