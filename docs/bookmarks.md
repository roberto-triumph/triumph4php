[Table Of Contents](/#toc) | [Next - PHP Doc Comments](/php-doc-comments/)

#Bookmarks#

triumph4php has a bookmarks feature similar to most editors.  Press CTRL+B to
toggle bookmarks on or off.  Then press F2 to cycle through all of your bookmarks.

Note that bookmarks represent a location in the code and not a file number. If you
type in text that changes the line of the bookmark, the bookmark will update itself
and will point to the new line and not the line that was originally bookmarked. For
example, let's say you bookmarked the beginning of a  function `printUser` but 
you end up adding another function `printGroups` before `printUser`. When you cycle
bookmarks, the bookmark will take you to the beginning of the `printUser` function.

Bookmarks persist even if you close files. As you cycle through bookmarks, if the
bookmark is for a file that is not opened the file will be opened.

Currently, bookmarks do not persist across triumph4php restarts.

[Table Of Contents](/#toc) | [Next - PHP Doc Comments](/php-doc-comments/)