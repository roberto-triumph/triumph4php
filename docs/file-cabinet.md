[Table Of Contents](/#toc) | [Next - Framework Detection](/framework-detection/)

#File Cabinet#

triumph4php's file cabinet feature stores files and directories for later opening.
Interesting facts about the file cabinet:

* Any file or directory can be stored; it need not be inside a defined project
* When a file is activated in the file cabinet, it is opened 
* When a directory is activated in the file cabinet, a triumph4php explorer panel is 
  opened to the directory (i.e. the contents of the directory are shown).
* The files / directories in the file cabinet persist across editor restarts.

#Why would I use this? I have already created a project#
The file cabinet is most useful to access files or directories that are not in
a project, like apache config files or php.ini files. The main
motivation was that you can add the php.ini in the file cabinet, then easily 
open it when you need to edit it. Another motivation was to save the apache
config directory to the file cabinet, so that virtual host entries can be
edited with ease.

*__TIP__: triumph4php can open and save files that are writable by an 
administrator / root user !*


[Table Of Contents](/#toc) | [Next - Framework Detection](/framework-detection/)