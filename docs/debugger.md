
Triumph is a full-fledged Xdebug client.  It can listen for incoming Xdebug connections, and can be used to step through code.  In this form, you can configure a few Xdebug settings.

Xdebug Port: This is the port that triumph listens on for incoming Xdebug connections.  This should be the same as the xdebug.remote_port value in your php.ini.

Max Children On Initial Load: The numbe of "children" that are fetched for each variable. If this number is 100, then initially only the first 100 array key-value pairs are shown, or the first 100 member variables for an object.

Max Depth On Initial Load: The number of levels that are fetched for each variable. If this number is 2, then for arrays of objects, we initially load the array's key-value pairs AND each object's chidren.
