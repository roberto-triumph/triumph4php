[Table Of Contents](/#toc) | [Next - total search](/total-search/)

# Projects #
Projects in triumph4php are defined as the following:

  * A Project is 1 or more sources 
  * A Project has a friendly label
  * A Project can be enabled or disabled; multiple projects may be enabled at any time
  * A Source is a directory, a set of include wild cards and a set of exclude wild cards. 

A triumph4php source directory is recursive; any sub-directories under the source directory also become 
part of the Source. Files within a source can be ignored by setting the exclude wild card;
any file that matches the include wild card AND DOES NOT MATCH the exclude Willard 
will be part of the project.

Exclude wild cards are useful to ignore "cache" directories that many PHP frameworks use; 
frameworks place amalgamations (many PHP files concatenated together) for use
in deployment, but that are not used during development.

Telling triumph4php where your PHP projects are is very useful, as it will allow the 
[Total Search](/total-search/) and [PHP code completion](/php-code-completion/) features to suggest 
classes, methods, and functions for your projects.

#Adding A Project#
There are 3 ways to add a project to triumph4php.  

##Adding a Project with a single Source Directory##
The easiest way to get up and running is by going to File ... New Project, and then choosing
the root directory of your project.  After you choose the root of your project, triumph4php will 

  * create a project, with the directory name as the label of the project
  * create 1 source within that project pointing to the chosen directory
  * all files will be included (*), no files will be excluded

##Adding a Project with Many Source Directories##
Some projects are split in separate directories. For example, you may have
your project in /home/user/projects/blog and common framework files in /home/user/libs/framework.
By creating a project with two source directories, triumph4php will recurse
both directories when tagging your project.

  * Go to File ... Show Projects
  * Click the Add button
  * Enter a label for your project
  * Click the Add Source button
  * Add one of source directories
  * Click the Add Source button again
  * Add the other source directory
  * Click OK, the OK again to exit the projects dialog

##Adding multiple projects at Once##
The final way to add a project is through the "Add Multiple" button in the projects
dialog. This is a bulk action; it will create 1 project for all directories under a chosen
directory. For example, you have 3 projects, like so

/home/user/projects/blog
/home/user/projects/messenger
/home/user/projects/fb_app

Then, in triumph4php, you go to File ... Show Projects.  Then click on the Add Multiple
button.  Then, you choose "/home/user/projects".  After you click OK, triumph4php will
create 3 projects: one named blog, one named messenger, and one named fb_app. The
blog project will have 1 source: /home/user/projects/blog. The messenger will have
1 source /home/user/projects/messenger. The fb_app project will have 1 source
/home/user/projects/fb_app.

#Deleting Projects#
Removing projects from triumph4php is simple: just go to File ... Show Projects, select
and project (ie make sure its highlighted, not just checked) and click the Remove
Selected button. Note that triumph4php will NOT delete the source directories in the file
system; the source directory will still exist, it's just that triumph4php will not
attempt to tag it.

#Enabling Projects#
Enabling or disabling projects affects the tags that the PHP code completion and 
total search features use.  You might want to disable / enable projects when you
are switching and working on separate projects.


[Table Of Contents](/#toc) | [Next - total search](/total-search/)