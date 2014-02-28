[Home](/#toc) | [Next - Projects](/projects)

# New User Wizard #

The very first time that triumph4php runs on your system, it will ask you 
to configure it to your liking.  The new user wizard prompts for the 
following:

1. Check for new versions

	Triumph can check for new versions of the editor.  Version checking requires an 
	active internet connection. If enabled, version update checks will be done once a week.

2. Location of settings

	The location where triumph4php can be store its settings is configurable.
	By default, triumph will store its configuration in the user data directory
	(/home/user/.triumph4php on linux, C:\Users\user\AppData\Roaming\.triumph4php
	on windows). You can choose to store the configuration in another directory,
	or in the same directory as the executable.  Storing the configuration
	in the same directory as the executable is known as "portable" executable; you
	are able to move the installation directory into any place at any time, or
	you can store triumph4php into a USB stick and carry it with you!

3. Editor Associations

	Triumph uses file extensions to determine how to display the source code contained within 
	them. Triumph's syntax highlighting and source code parsing features use these settings 
	to determine which files contain PHP, SQL, and CSS source code. You can change the 
	file extensions if desired.
	
	Each input can contain 1 or more wilcard strings. 
	
	The allowed wildcard characters are:
	
	* \* : Will match on zero or more characters
	* ? : Will match on zero or one character
	* 	; : Separator among multiple wildcards
	
	
	For example, the value `"*.php;*.phtml` will match files with the extension .php or the 
	extension .phtml.
	
	Note that the new setup wizard will show only the editor associations for the
	most relevant file types; even more file types can be configured via the
	Edit ... Preferences menu.

4. PHP settings

	The PHP executable is used by Triumph when running scripts. It is also used when 
	running framework detection scripts. If you are running on a Windows PC, choose 
	php-win.exe. On linux, choose php.
	
	The version dropdown will determine which version of PHP Triumph will use for source 
	code parsing. Choose `Auto` to have Triumph use the same version as the PHP 
	executable; or one of the other versions to use have Triumph use a different version.
	Choosing PHP 5.4 will enable parsing of traits, it will also parse the `use`
	statements and allow PHP code completion to work on inherited traits!



[Home](/#toc) | [Next - Projects](/projects)