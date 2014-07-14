[Table Of Contents](/#toc) | [Next - What Next](/what-next/)

#Template File Detectors#
If one of your projects is using a [supported framework](/framework-detection/#supported-frameworks), triumphp4php
will attempt to parse out the template variables in controllers and make them available
in PHP code completion.


If this seems kind of weird, it is because not many editors do this. Let me clarify with an
example:

Let's say you create a CodeIgniter project in triumph4php.  When you create the project,
triumph4php will detect that you are using CodeIgniter. 

Then, you open the templates panel by going to View ... PHP Template Files.  
triumph4php shows then allows you to choose your controller and action, and 
triumph4php will show you the templates that the action uses.  It will also show
you the templates variables that are available to the templates.  The shown
variables are also made available to you in the PHP code completion!


#Note 1#
Template files detectors detectors only work if you tell triump4php the virtual 
host entry your project uses by going to Edit ... Preferences then going to 
the "Apache" tab.

#Note 2#
Template files detectors only work if you have tagged your project.

#Note 3#
Template files detectors only work if you URL detectors have successfully run.

#Note 4#
You can trigger the URL detection manually by going to 
Detectors ... Run URL Detection.

#Note 5#
For template file detectors to work, you must have PHP installed.  Also, you must 
have PDO and PDO SQLite extensions enabled.

[Table Of Contents](/#toc) | [Next - What Next](/what-next/)