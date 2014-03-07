[Table Of Contents](/#toc) | [Next - Config Detectors](/config-detectors/)

#Framework Detection#

PHP developers use a framework to aid in the development of their PHP projects.  triumph4php 
recognizes this and attempts to help developers even more by providing custom-tailored
experiences based on the PHP framework that a project uses.

**Note: this feature is experimental.  Only CodeIgniter is supported at the moment**
<a id="supported-frameworks"></a>
#Supported Frameworks#
  * Code Igniter 2

When triumph4php detects that a project is using a PHP framework, it attempts to:

  1. Find out the location of the framework's config files by using [config detectors](/config-detectors/)
  2. Find out the database credentials in the framework's config files by using [database detectors](/database-detectors/)
  3. Find out the URLs that the application provides by using [URL detectors](/url-detectors/)
  4. Find out the variables that your controllers assign to templates by using [template file detectors](/template-file-detectors/)

#Note#
For framework detection to work, you must have PHP installed.  Also, you must 
have PDO and PDO Sqlite extensions enabled.

[Table Of Contents](/#toc) | [Next - Config Detectors](/config-detectors/)