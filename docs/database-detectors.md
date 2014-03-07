[Table Of Contents](/#toc) | [Next - url detectors](/url-detectors/)

#Database Detectors#

If one of your projects is using a [supported framework](/framework-detection/#supported-frameworks), triumphp4php
will put the read the framework's configured database connection and create a SQL connection
in triumph4php. This means, without you having to do anything, you get all of the same benefits
as you would by creating a SQL connection manually:

1. The connection can be used to perform queries 
2. The database metadata is grabbed from the connection. This allows the
   [total search](/total-search/) dialog to show a list of matching
   database tables; the total search dialog is able to quickly
   open a database table or show the database table's schema.
3. The discovered table and column names become available for
   code completion in SQL queries AND in strings in PHP code . For example,
   when you type in text in a PHP string like `$arrItems['us` , code completion 
   can complete database table and column names that start with `us`.


#Note#
For database detectors to work, you must have PHP installed.  Also, you must 
have PDO and PDO Sqlite extensions enabled.

[Table Of Contents](/#toc) | [Next - url detectors](/url-detectors/)