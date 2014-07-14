[Table Of Contents](/#toc) | [Next - SQL Browser](/sql-browser/)

# Database Connections #

You can add a database connection to triumph4php. The database
connection is used in the [SQL Browser](/sql-browser/).

Go to SQL ... SQL Connection to see the list of connections that
exist.  You can, edit, or remove connections from that dialog.

You can connect to a MySQL database or a SQLite file.

triump4php can hold many SQL connections.  Each SQL connection
can be enabled or disabled.  When a SQL connection is enabled, several 
things happen:

1. The connection can be used to perform queries 
2. The database meta data is grabbed from the connection. This allows the
   [total search](/total-search/) dialog to show a list of matching
   database tables; the total search dialog is able to quickly
   open a database table or show the database table's schema.
3. The discovered table and column names become available for
   code completion in SQL queries AND in strings in PHP code . For example,
   when you type in text in a PHP string like `$arrItems['us` , code completion 
   can complete database table and column names that start with `us`.
    
	

**Note: passwords are stored in plain text right now**. You should
not add your production db credentials here.


[Table Of Contents](/#toc) | [Next - SQL Browser](/sql-browser/)