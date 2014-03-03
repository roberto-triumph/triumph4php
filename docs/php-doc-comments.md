[Table Of Contents](/#toc) | [Next - PHP Linter](/php-linter/)


# PHP Doc Comments #

You can have triumph4php display the Doc Comment of a class, function,
or method by hovering over the class, function or method name while
holding the ALT key. If a doc comment is very big, the panel will
be scrollable so that you can scroll down to view the entire comment.

##Help! I can't get Doc comments to show up##
  * Doc comments only work on files that are inside a [project](/projects/). 
  * Doc comments will be parsed out of the code.  This means that the 
    file must not have PHP sytax errors.
  * For methods, the parser must know the class of a variable.  This
    is controlled with either a PHP type hint or a PHP DocBlock comment.
	The parser can also recognize the class if the variable was instantiated
	in the same scope where the method call is located.

[Table Of Contents](/#toc) | [Next - PHP Linter](/php-linter/)

