[Table Of Contents](/#toc) | [Next - PHP Code Completion](/php-code-completion/)

#Total Search#

One of the most useful features on an IDE is the abilty to jump around the
source code without needing to manually opening files.  Once you
have added your projects, you will be able to jump to any class, method, function
or file in your class.

Go to Search .. Total Search.  A dialog shows up prompting you the item to
look for.  As soon as you start typing, the results list will update 
with the matching classes, functions, methods, or files.

##Matching logic##
A few notes about matching logic:
  
  * If the input is an exact match of something, it will only display the exact match
  * If the input has a period, it will only display file name results
  * If the input has a double colon "::" it will only display results where the
    class name matched and the method name starts with the input
  * If the input starts with a double colon "::" only methods that start with
    the input are displayed. This is a special mode; all classes will be searched.
  * Matching for class names, methods, or functions is done on a "begins with"
    basis; the results displayed are items that start with the input.
  * Matching for file names is done on a "contains" basis; the results displayed
    are file names that contain the input.
  * Matching is always case-insensitive

Once results are displayed, you can hit the UP / DOWN arrow keys to change
selection.  You can also check multiple items; each checked item will be opened.
You can also double-click on an item in the list; in this case the item that
was double-clicked on will be opened.


[Table Of Contents](/#toc) | [Next - PHP Code Completion](/php-code-completion/)