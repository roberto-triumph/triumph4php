[Table Of Contents](/#toc) | [Next - Database Connections](/database-connections/)

#PHP Linter#

triumph4php comes with a PHP source code linter that will notify you 
when you have a file that contains PHP errors. The PHP linter can
detect the following errors:

* Syntax error
* Uninitialized variable
* Unknown function/methods
* function call argument count mismatch

#Philosophy#
The triumph4php linter was built with 2 goals in mind: follow the rules of
the PHP linter (`php -l`) to the letter; and prevent showing false positives
where ever possible.  Most IDEs either show too many errors or not enough, 
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

##Uninitialized variables##
triumph4php can check if a variable is being used before being
initialized.  Such cases are not labeled as errors by PHP, you
would need to run the code in order for PHP to raise a warning. triumph4php's
linter checks for Uninitialized variables and labels it as an error.
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
this is because most template systems use global variables that are defined 
elsewhere; if triumph4php checked variables in the global scope then all
of you template files would have errors!

Another instance where uninitialized variable detection has special rules is 
for variables used in function calls. Consider the following code:

	function performWork() {
		preg_match('string', 'this is a long string', $arrMatches);
		var_dump($arrMatches);
	}

Here, you might think that `$arrMatches` should be labeled as uninitialized, but 
triump4php's linter does not do so.  There is a valid reason for this: PHP allows 
uninitialized variables to be passed by reference and initialized
by the function that is called. In this example, `$arrMatches` is not initialized 
in the code, but the function that is called initializes it.  triumph4php is 
smart enough to lookup function signatures to determine if a function argument
is passed-by-reference, and if so then triumph4php will assume that the variable
is being initialized by the calling function.

**NOTE** when a function signature is NOT found, triumph4php will skip this extra
check. This is useful when you open a file that is not part of a project, 
triumph4php's linter will lookup function calls, but the functions
won't be found, but triumph4php will "skip over" the function's arguments since
it cannot definitely know if a variable could be initialized by the calling 
function.

#Unknown identifiers#
triumph4php's linter can also warn you about calling functions, methods, or 
classes that do not exist.  In order for this check to work, you need to create 
and enable a [project](/projects/). 

**NOTE** The unknown identifier checks will be disabled when linting files outside
of enabled projects; since triumph4php will not be able to verify if the 
identifiers are valid. This is good, since triumph4php will still be able to show 
you (some) uninitialized variable errors and PHP syntax errors; but will not
bother you showing false positives.

#Function Call Argument Count#
triumph4php's linter will wan you when calling a function or method and the
number of arguments of the function call are different that the number of 
arguments in the function signature. Consider the following code:


	function performWork($contents) {
		preg_match('string', $contents, $arrMatches);
		var_dump($arrMatches);
	}
	performWork();
	performWork('a long string');
	performWork('a long string', 'another long string');
	
Here, triumph4php will alert you that the first call to `performWork` is missing
an argument, and that the third call to `performWork` has too many arguments.
triumph4php will take into account default parameter values when counting
arguments. triumph4php is also smart enough to skip these checks  when it finds
a call to `func_get_args`, since this signifies that the function takes a 
variable amount of arguments.

<a id="suppressions"></a>
#Suppressions#
Even though triumph4php tries it hardest to not show false positives, there are 
times when linting will show you false positives; for example, when
a custom PHP extension is loaded. There are other times that you don't care 
about lint errors; like for example if a third party library that you use has
an uninitialized variable error. For these cases, triumph4php has a built-in
suppression feature that you can use to tell triumph4php to not show you lint
errors from a file or directory.

A lint suppression consists of the following:

* Rule type - the type of error to suppress (unknown class, uninitialized
  variable).  There is also a "skip all" to supress all lint errors.
* Rule target - The name of the item to suppress (variable name, function / 
  class name). May be empty for "skip all" suppressions.
* Location - The file or directory to suppress.

Example:
The very handy suppression rule 

* Rule type - Skip all
* Rule target - N/A
* Rule location - /home/user/project/vendor

This rule will suppress all lint errors from the third party directory where
composer puts the third-party code.  Suppressing the entire vendor directory is
great because it means that the linter will only show you errors from your own
code, plus the linter will be that much faster since it does not have to 
check the vendor directory (which can bring in thousands of files).  This 
suppression rule is so handy that triumph4php will add it when you create a
project.

Example 2:
Suppressing unknown function errors

* Rule type - Skip Unknown function
* Rule target - couchbase_init
* Rule location - /home/user/project/

This rule will suppress the lint errors "unknown function couchbase_init" that
occur in /home/user/project/.  This type of rule is useful when you have a 
PHP extension loaded; triumph4php will does not currently know functions
from custom PHP extensions that are loaded.

#Help! The linter is not showing me any errors and I know my code has errors!#
* You may have created a suppression rule to suppress errors.
* The file being checked is not part of an enabled project.  When a file is
  not part of a project, triumph4php will not perform unknown identifier
  checks, function call argument count checks, and will skip uninitialized
  variable checks for variables inside function calls.
* Note that the core value of triumph4php's linter is to not show errors unless 
  they truly are errors; in other words the number of false positives are 
  kept at a mininum.  This ensures that the linter will be used; and that the
  linter shows you real errors as opposed to possible errors.

#Help! The linter is showing me errors and I know my code has no errors!#

* You may have a class / function / method that is defined in a custom PHP 
  extension.  Some PHP extensions will come with "PHP stubs", PHP code that
  defines functions.  You can use these "PHP stubs" as source directories so
  that triumph4php recognizes that the functions exist.
* Make sure your project defines all source directories completely, and that
  there are no files are mistakely ignored.  
* Make sure that your project is properly tagged. Click on "Tag projects" to
  re-tag all of the enabled projects.
* If you have entered a suppression rule, make sure that the rule target
  is correct.  The rule target is case-insensitive, watch out for extra spaces.

[Table Of Contents](/#toc) | [Next - Database Connections](/database-connections/)
