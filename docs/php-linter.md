[Table Of Contents](/#toc) | [Next - Database Connections](/database-connections/)

#PHP Linter#

triumph4php comes with a PHP source code linter that will notify you 
when you have a file that contains PHP errors. The PHP linter can
detect the following errors:

* Syntax error
* Uninitialized variable
* Unknow function/methods

#Philosophy#
The triumph4php linter was built with 2 goals in mind: follow the rules of
the PHP linter (`php -l`) to the letter; and prevent showing false positives
whereever possible.  Most IDEs either show too many errors or not enough, 
leaving you to not use their linter feature.

This philosophy is achieved:
 
* The triumph4php linters are extracted from the actual PHP 
    source code.
* The linters have special rules to handle the more dynamic
    aspects of PHP (see below)
	
#Triggering the linter#
By default trumph4php's linter will process a file right after
a file save.  If the file has an error, you will see an error
message.  The message will be located right next to the location of
the error if the error is visible. If the error is not visible, you
will see an error message right next to the cursor; the error
will show you the actual error.  You can then press the space
bar to jump to the error location.

You can also trigger a lint of the entire project by clicking on the
Lint Check button on the toolbar. The lint check will perform
a lint check on all PHP files in all of your enabled projects.  

##Syntax errors##
triumph4php can check that source code conforms to either PHP 5.3
or PHP 5.4 syntax.  The PHP version can be changed by going to
Edit ... Preferences then choosing the PHP Lint Check tab.

##Unitialized variables##
triumph4php can check if a variable is being used before being
initialized.  Such cases are not labeled as errors by PHP, you
would need to run the code in order for PHP to raise a warning. triumph4php's
linter checks for unitialized variables and labels it as an error.
It makes it very easy to spot bugs in the code.

PHP is a very dynamic language, and you can create code in a way
to "fool" most other PHP linters. triumph4php's linter will look 
for function calls that add variables to the current scope and will 
disable the linter for that scope. It can best be seen with this
example:

	function performWork($arrItems) {
		extract($arrItems);
		$a = $b + $c;
		print $a;
	}

For those unfamiliar with the `extract()` function, this function creates
a variable for each item in the array; the key is used as
the variable name and the value is used as the item value. triumph4php
knows this, and assumes that `$b` and `$c` were extracted out of
the array. triumph4php will not complain about them being 
uninitialized.  

Now, you might say "the extract function is bad; I never use it!" That
may be the case, but you will probably encounter code that uses it,
and you can be relieved knowing that you can still use triumph4php's linter
because it will not warn about errors that you know are false
positives.

The functions that triumph4php knows will bring in local variables:

* extract() 
* eval()
* include() / include_once()

triumph4php will also not label variables in the global scope as uninitialized;
this is because most templating systems use global variables that are defined 
elsewhere; if triumph4php checked variables in the global scope then all
of you template files would have errors!

##The linter does not look like its showing me uninitialized variables. What gives?##

There is one instance where uninitialized variable detection is not working
right now: during function calls.  For example:

	function performWork() {
		logDebug($myVar);
	}

Here, `$myVar` should be labelled as uninitialized, but triump4php's linter
does not do so.  There is a valid reason for this: PHP allows use of
uninitialized variables that to be passed by reference and initialized
by the function that is called. An example of this is the preg_match function


	preg_match('string', 'this is a long string', $arrMatches);
	var_dump($arrMatches);

In this example, `$arrMatches` is not initialized in the code, but the
function that is called initializes it.  

Initially, triumph4php's linter had labelled this as an error. 
However, while testing the linter against various open source projects, there
was quite a bit of false positives since preg_match is a pretty
common function and many people use the function like in the example.
Later on, the triumph4php linter will be updated to take into account the fact
that the argument is passed by reference or not, and flag errors
accordingly.

#Unknown identifiers#
triumph4php's linter can also warn you about calling functions, methods, or classes
that do not exist.  In order for this check to work, you need to create and 
enable a [project](/projects/). 

**Note: This functionality is disabled by default**


[Table Of Contents](/#toc) | [Next - Database Connections](/database-connections/)