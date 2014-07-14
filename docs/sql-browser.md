[Table Of Contents](/#toc) | [Next - Find](/find/)

# SQL Browser #

triumph4php comes with a SQL browser that is geared towards
using it during development.

* Click the SQL Browser. It will open a new code tab for
  you to enter SQL.  SQL can then be executed by going to
  SQL ... Run Queries in SQL Browser
* When you have an existing SQL file opened, activating the 
  SQL ... Run Queries in SQL Browser menu will run all of the 
  queries in the file.

The SQL Browser will run **all** of the queries in the 
code tab. If previous queries contained errors
the subsequent queries still get executed.

Code completion works in a SQL browser tab; triumph4php
can code completion on the table names and column names that were
fetched from the enabled connections.

A running query can be stopped by closing the SQL results
panel.

#Viewing Results#
After you execute a query, the query results show up in the
tools notebook.  If multiple SELECT queries were run, the results
of each query are placed in a new results tab, and there will be
an additional tab that shows you any errors encountered. In the
results grid, you can do the following:

* Copy all rows: This copies all rows of data into the clipboard,
  in CSV format
* Copy cell data: This copies the selected cell of data into the clipboard
* Copy a row of data into the clipboard in CSV format
* Copy a row of data into the clipboard as a SQL statement
* Copy a row of data into the clipboard in PHP array format
* Open cell in new buffer: This opens the selected cell into a new editor
  code tab.  This option is useful when you store blob/text data
  that is too big to see in the grid.


[Table Of Contents](/#toc) | [Next - Find](/find/)