<?php

// Zend framework is written using relative require statements
// need to make sure that zed framework classes can require
// their dependencies properly
set_include_path(get_include_path() . PATH_SEPARATOR .  
	realpath(dirname(__FILE__) . '/lib/') // zend dir
	);

require_once dirname(__FILE__) . '/lib/opportunity/array.php';
require_once dirname(__FILE__) . '/lib/opportunity/string.php';

/**
 * autoload the Triumph classes.
 */
function autoload($className) {	
	$className = ltrim($className, '\\');
	$fileName  = '';
	$namespace = '';
	if ($lastNsPos = strrpos($className, '\\')) {
		$namespace = substr($className, 0, $lastNsPos);
		$className = substr($className, $lastNsPos + 1);
		$fileName  = str_replace('\\', DIRECTORY_SEPARATOR, $namespace) . DIRECTORY_SEPARATOR;
	}
	
	
	$dir = dirname(__FILE__) . '\\src\\';
	if (\opstring\begins_with($className, 'Zend_')) {
	
		// Triumph code will not use requires to load in zend framework files
		// we will use this autoloader too
		$dir = dirname(__FILE__) . '\\lib\\';
	}
	$dir = str_replace('\\', DIRECTORY_SEPARATOR, $dir);
	$fileName .= str_replace('_', DIRECTORY_SEPARATOR, $className) . '.php';
	
	require $dir . $fileName;
}

spl_autoload_register('autoload');