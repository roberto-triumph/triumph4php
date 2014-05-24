Triumph 4 PHP ChangeLog
=======================

#Version 0.4.6#
* Editor colors dialog under Edit -> preferences -> Editor Styles & Colors
  was not saving colors. (Linux, MSW)
* Updated the default color for selection foreground on some editor color 
  themes (Linux, MSW)
* New explorer panel that is shown on the left side of the screen and 
  not on the bottom. This helps people with wide-screen monitors.
  (Linux, MSW)
* Better handling of renamed files: when a user renames a file, the 
  user is asked 3 choices: open the new & old files, close both
  files, or open the new and close the old file. (Linux, MSW)
* Enhancement to new file /rename file dialogs.  By default,
  only the name of the file is selected and not the extension. This
  makes it easier for the user to edit the name. (Linux, MSW).
* Fixed Copy & Paste with multiple selections. When the user pastes
  into multiple selections, the paste is applied to all selections.
  (Linux, MSW)
* Code completion now works for newly-created files. (Linux, MSW)
* Code completion fix: methods that are private and
  static were not showing as suggestions. (Linux, MSW)
* Code completion fix: methods that are "static public" 
  were not showing as suggestions. (Linux, MSW)
* Program would hang when iterating through bookmarks of recently
  closed files. (Linux, MSW)

#Version 0.4.5#
  * Support for laravel framework (Linux, MSW): 
     - database connections are read from laravel config
	 - config files for laravel projects are added to the Project 
	   Configs menu
	 - Laravel routes are enumerated
	 - Laravel template variables are available in code completion
	 - Code completion of Laravel facades can be done.
  * Files with the UTF-8 BOM (begin of file marker) are now read
    and saved properly. (Linux, MSW) 
  * PHP Lint Check no longer generates false positives when using
    variable methods in static method calls; as in `self::{$prop}()`
	(Linux, MSW) 
  * Lines that have a find hit are no longer rendered with a white background;
	they are rendered using the default background. Only affected non-PHP files.
	(Linux, MSW) 
  * Jump to method was not putting cursor in the start of the matched
	method. (Linux, MSW) 
  * Show Projects Dialog was not showing OK/cancel buttons (Linux)
  * Performance optimization: iterating through files during find in
    files, lint check, and tag parsing is now much faster. (MSW)
  * Call tips for class constructors was not being shown. (Linux, MSW)
  * User can now create bookmarks and cycle through them. CTRL+B creates
    a bookmark, F2 jumps to the next bookmark.  (Linux, MSW)

#Version 0.4.4#
  *	The source code editor now supports multiple selections, virtual space,
    zoom in/out, and displaying of whitespace. (Linux, MSW) 
  * Lint Check no longer generates false positives when using the 
    `extract()` function
    in a file that declares a namespace (Linux, MSW).
  * Lint Check no longer generates false positives when using `isset()` 
    (Linux, MSW)
  * Fixed a crash bug when using the Doc Comment widget (Linux) 
  * Doc Comment widgets now shows a link to the PHP.net site for native 
    functions (Linux, MSW)
  * Update the color of the text inside the PHP Doc comment widget to make
    it more visible. (Linux, MSW) 
  * Added links to the online documentation in various editor dialogs.
    (Linux, MSW) 
	
#Version 0.4.2#
Fixed file modified detection for files located in network drives.

#Version 0.4.1#
Initial Release

