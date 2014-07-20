[Table Of Contents](/#toc) | [Next - Bookmarks](/bookmarks/)

Triumph is a full-fledged Xdebug client.  It listens for incoming Xdebug connections and 
can be used to step through code. Triumph also exposes an "eval" pane that allows you to 
enter any arbitrary expressions
while you are stepping through a script.

#Requirements#
You should first ensure that Xdebug is installed properly. You may want to read the
[Xdebug docs](http://xdebug.org/docs/remote) on remote debugging.

#Background#
A bit about how Xdebug debugging works:

1. The debugging client (Triumph) starts listening on port 9000
2. The user sends a request to a web page via a web browser
3. The web server gets the request and will initiate a connection
   with the debugging client.
4. Triumph sends commands to enable breakpoints and start the
   script.
5. The script starts, when it hits a break-point then 
   Xdebug tells the debugging client that the script
   is paused. Xdebug also sends the name of file and line number that the script 
   is paused on.
6. The user can inspect variables, set new breakpoints, or
   evaluate expressions. These actions are turned into
   Xdebug commands by Triumph.
7. Xdebug responds to the Triumph commands, and Triumph updates the
   various debugging panels appropriately.


#Starting the debugger#
By default, Triumph will *NOT* start listening for Xdebug connections on program startup. You
must click on the "Start Debugger" button in the toolbar.  You can change it so that
Triumph starts listening for Xdebug connections if you want, but fair warning: this
will cause Triumph to pause a little bit when exiting (because it has to stop
listening for connections).

#Debugging Web pages#
You can use any number of browser extensions to enable Xdebug on-demand. Clicking on
the debug button on those extensions enables Xdebug to be active on that request. The
general sequence is: 

1. Enable breakpoints by clicking on the left-most margin in Triumph, or using the
   keyboard shortcut `CTRL+K`
2. In the web browser, click the debug button (from the Xdebug browser extensions)
   and reload the web page.
3. When Triumph receives an Xdebug connection, the Debug panel will be active in
   the Tools (bottom) notebook. By default, Triumph will tell Xdebug to start 
   the script immediately.  You can configure Triumph to break on script start
   so that the script being debugged does not run at all before pausing.
4. When the script is paused, you can use any of the debugger panels to examine the
   state of the program.

##Stepping Through Code##
The toolbar has buttons to step into, step over, and step out of the current
line being executed. The toolbar also has a "finish session" button; this button
will disable all breakpoints (for the current request only) and will allow the 
debugged script *to run normally to completion*. The "finish session" button 
still leaves Triumph listening for more Xdebug connections.

##Stack Panel##
The stack panel is initially located on the left hand side and will list the
current stack trace (what method is being called, and which method called it). 
The first method is the current method.

##Variables Panel##
The variables panel shows the variables that are defined in the local
and the global scope. You can expand an item to get more information
about that item. By default, variable values are truncated to a max length
of 80 characters. You can also double click on an item to get the entire
value of the variable.
Note that there is a limit to how much variable data is initially retrieved,
in both the number of items and the depth of items to get.  Triumph
does not yet have a way to page through items; it will show 100 items
max. You can set this value to be higher if need be.

##Breakpoints Panel##
The breakpoints panel shows all of the breakpoints that have been set on
all files.  Each break-point has a flag to turn on/off the break-point
without deleting it. The breakpoints panel has buttons to toggle all
breakpoints (turn them on or off).  You can double click on a break-point
to open the file that the break-point is for.

##Eval Panel##
The eval panel allows you to enter any expression to be executed while the script 
is paused. Triumph will send the PHP code to Xdebug to be executed, and Triumph
will display the result in the right hand side of the eval panel.

Be careful here, you must enter an expression. An expression is basically anything
that returns a variable. For example, `echo` is *NOT* an expression. Examples
of expressions

    $this
    $arrNames
    $this->myMethod()

You can only enter 1 expression at a time.  For example, if you enter `$arrNames`
then the key-value pairs of the array will be shown in the right hand side of
the panel.

Triumph will display as much of a variable as is configured in  "Max Children
On Initial Load" and "Max Depth on Initial Load". See below for more info.

*__TIP__: code completion and call tips are also functional in the eval source code
panel!*

##Log Panel##
The log panel shows the exact commands that are sent to Xdebug and the 
exact responses that Triumph receives.  This is useful when determining if 
Xdebug is attempting to connect to Triumph.

#True Remote Debugging#
Triumph can achieve true remote debugging because it allows you to define
where to look for file-paths that Xdebug is running.  Since Xdebug is running
remotely, it can be running on a remote machine, where the debugged
script is not on the same location as your dev environment.  Another scenario
is when you are using a Virtual Machine as a dev environment; you may be
running Ubuntu in a VM that is using a  MSW machine as a host; Triumph
can be running on the MSW host and accept commands from Xdebug running in 
Ubuntu. Triumph will look up user-defined Source Code mappings to 
turn Xdebug file paths (in this case Linux paths) into paths on the local
system (in this case Windows paths). 

#Options#
You can go to Edit ... Preferences to configure a few Xdebug settings.

- Xdebug Port: This is the port that Triumph listens on for incoming Xdebug connections.
  This should be the same as the xdebug.remote_port value in your php.ini.
- Max Children On Initial Load: The number of "children" that are fetched for each variable. 
  If this number is 100, then initially only the first 100 array key-value pairs are shown, 
  or the first 100 member variables for an object. You can set this to a high number
  (>= 100 <= 1000) because Triumph cannot currently does not implement paging
  of children.
- Max Depth On Initial Load: The number of levels that are fetched for each variable. If 
  this number is 2, then for arrays of objects, we initially load the array's key-value 
  pairs AND each object's children.
- Listen On IDE Start: If checked, then Triumph will start listening for Xdebug connections
  when Triumph starts.
- Break On Script Start: If checked, then Triumph will pause the debugged script at
  the first line of the script.
- Debugger Source Code Mappings: Here, you can edit the source code mappings. You
  should set source code mappings when the path of the script being debugged and
  the path on the machine that Triumph is running on are different.  In a source
  code mapping, the local path is a directory on the local machine where Triumph
  is running, and the remote path is the directory where Xdebug is running.
  Source code mappings cascade to sub-directories, if there is a mapping for
    - local path: c:\wamp\
    - remote path: /var/www/

  And the script being debugged is located in /var/www/products/show.php then
  the local path is calculated to be c:\wamp\products\show.php.
  __WARNING__ do note that Xdebug always uses the "real path", in the case
  of symbolic links Xdebug returns the directory the symlink points to and
  not the symlink.  For example, if apache is serving /var/www/site, but
  /var/www/site is actually a symlink to /home/site, then your source code
  mapping should be "/home/site" and not "/var/www".

#Help! I can't get debugging to work!#
You should ensure the following:

- Xdebug is configured in php.ini (the php.ini that the web server is using)
- Use `phpinfo()` to confirm that Xdebug is enabled, remote debugging 
  is enabled, and the appropriate remote host, remote port are being used. It 
  may also be helpful to turn on Xdebug logs.
- Make sure that Triumph is listening on the correct port (the port that is 
  set in the php.ini `xdebug.remote_port` setting).  Use `netstat` to
  confirm that Triumph is listening for connections. If not, you need to
  click on the "Start Debugger" button.
- if the above three items check out OK, you can then inspect the 
  Xdebug logs and/or Triumph's debugger log panel to see the contents
  being sent back and forth. Specifically, look at the file paths being
  returned by Xdebug or sent over by Triumph, you may need to add a source
  code mapping.


[Table Of Contents](/#toc) | [Next - Bookmarks](/bookmarks/)