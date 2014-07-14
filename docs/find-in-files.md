[Table Of Contents](/#toc) | [Next - Explorer](/explorer/)

# Find In Files#

The Find in files feature works similarly to most other editors. You can search,
replace, and replace all in many files at once.

You can find an exact match, a case-insensitive match, or a regular
expression.  The full regular expression syntax is described at 
[http://www.unicode.org/reports/tr18/](http://www.unicode.org/reports/tr18/). To the
left of the find input, there is a pop-up menu that shows you the regular 
expression meta-characters that are available.  Clicking on one of them
will add it to the find expression text input.

Find in files is recursive; it will search the given directory and all of its sub-directories.
By default only PHP files are searched. You can change this at any time by entering
new extensions.  Multiple extensions can be separated with a semicolon; for example
`*.php;*.phtml` will search inside of files that have a php or phtml extension.
Note that extension wild cards are checked in a case-insensitive manner.

By default, find in files will **not** search hidden files.  This can be overwritten
by clicking on the "Search hidden files" check box.

When you perform a find in files search, triumph4php will search the contents of 
the opened buffers and **not** the file system if a file is opened.  That way, there
are no stale matches.

Find in files will always perform searches one line at at time, meaning that it 
won't match on  multi-line regular expressions.

A running search can be stopped by clicking on the stop (x) button; also by
closing the find in files results panel the search will be stopped.

If find in files finds matches in more than 100 files the search will terminate.


[Table Of Contents](/#toc) | [Next - Explorer](/explorer/)