[Table Of Contents](/#toc) | [Next - Template File Detectors](/template-file-detectors/)

#URL Detectors#

If one of your projects is using a [supported framework](/framework-detection/#supported-frameworks), triumphp4php
will attempt to enumerate all of the URLs that your project exposes. With detected URLs, you can quickly
search through your project's URLs and quickly open them from within triumph4php. This happens even
if you use seo-friendly URLs.

If this seems kind of weird, it is because not many editors do this. Let me clarify with an
example:

Let's say you create a CodeIgniter project in triumph4php.  When you create the project,
triumph4php will detect that you are using CodeIgniter.  triumph4php will read your project's
`config/routes.php` and will create one URL for each entry in routes.php. Then you can
click on the "Search For URLs" button in triumph4php; it will show you all of your projects'
URLs.  Clicking on a URL will open it in a web browser.

#Note 1#
URL detectors only work if you tell triump4php the virtual host entry your project uses
by going to Edit ... Preferences then going to the "Apache" tab.

#Note 2#
URL detectors only work if you have tagged your project.

#Note 3#
You can trigger the URL detection manually by going to Detectors ... Run URL Detection.

#Note 4#
For URL detectors to work, you must have PHP installed.  Also, you must 
have PDO and PDO SQLite extensions enabled.


[Table Of Contents](/#toc) | [Next - Template File Detectors](/template-file-detectors/)