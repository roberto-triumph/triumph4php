<?php
/**
 * This software is released under the terms of the MIT License
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */

/**
 * The goal of this script is to discover all of the URLs that a PHP code igniter project. This means
 * that it will list all of the URLs that correspond all of the project's controller functions. This script
 * is a bit smarter because it is able to detect default controllers and also read the code igniter
 * config for url suffixes.
 *
 * This script is part of Triumph's URL Detection feature; it enables the editor to have a 
 * list of all of a project's urls so that the user can easily open / jump to URLs. More
 * info can be about Triumph's URL detector feature can be found at 
 * http://code.google.com/p/triumph4php/wiki/URLDetectors
 */

// the bootstrap file setups up the include path and autoload mechanism so that
// all libraries are accessible without needing to be required.
require_once('bootstrap.php');

/**
 * This function will look at the given program arguments and will validate them.
 * If the required arguments have been passed then it will call the URL detector function,
 * otherwise it will stop the program.
 * This function is called at the end of this script (see last line of this file).
 * The help text below describes the arguments that are needed.
 */
function parseArgs() {
	$rules = array(
		'sourceDir|d=s' => 'Required. The base directory that the project resides in',
		'resourceDbFileName|i=s' => 'Required. SQLite file that contains the project\'s files, classes, and methods. This file is created by ' .
			'Triumph; Triumph performs INSERTs as it indexes a project.',
		'host|s=s' => 'Required. Host name to start the URLs with. Triumph determines the host name by using the configured ' .
			'virtual host mappings that the user adds via the menu Edit -> Preferences -> Apache',
		'outputDbFileName|o-s' => 'Optional. If given, the output will be written to the given file. If not given, output goes to STDOUT.',
		'help|h' => 'This help message'
	);
	$opts = new Zend_Console_Getopt($rules);

	// trim any quotes
	$sourceDir = trim($opts->getOption('sourceDir'), " \r\n\t'\"");
	$resourceDbFileName = trim($opts->getOption('resourceDbFileName'), " \r\n\t'\"");
	$host = trim($opts->getOption('host'), " \r\n\t'\"");
	$outputDbFileName = trim($opts->getOption('outputDbFileName'), " \r\n\t'\"");
	$help = trim($opts->getOption('help'), " \r\n\t'\"");

	if ($help) {
		$helpMessage = <<<EOF
The goal of this script is to discover all of the URLs that a PHP code igniter project. This means
that it will list all of the URLs that correspond all of the project's controller functions. This script
is a bit smarter because it is able to detect default controllers and also read the code igniter
config for url suffixes.

This script is part of Triumph's URL Detection feature; it enables the editor to have a 
list of all of a project's urls so that the user can easily open / jump to URLs. More
info can be about Triumph's URL detector feature can be found at 
http://code.google.com/p/triumph4php/wiki/URLDetectors

When a required argument is invalid or missing, the program will exit with an error code (-1)

EOF;

		echo $helpMessage;
		echo "\n";
		echo $opts->getUsageMessage();
		exit(-1);
	}

	if (!$sourceDir) {
		echo "Missing argument: --sourceDir. See --help for details.\n";
		exit(-1);
	}
	if (!is_dir($sourceDir)) {
		echo "sourceDir is not a valid directory. Is \"{$sourceDir}\" a directory? Is it readable? \n";
		exit(-1);
	}

	if (!$resourceDbFileName) {
		echo "Missing argument: --resourceDbFileName. See --help for details.\n";
		exit(-1);
	}

	if (!$host) {
		echo "Missing argument: --host. See --help for details.\n";
		exit(-1);
	}
	
	if (!extension_loaded('pdo_sqlite') || !extension_loaded('PDO')) {
		echo "The script " . basename(__FILE__) . " requires the PDO and pdo_sqlite PHP extensions.";
		exit(-1);
	}

	// call the function that will return all detected URLs
	$doSkip = TRUE;
	$arrUrls = detectUrls($sourceDir, $resourceDbFileName, $host, $doSkip);
	if ($doSkip) {
		echo "Detector " . basename(__FILE__, '.php') . " cannot detect URLs {$sourceDir} ... skipping\n";
	}
	else if ($outputDbFileName) {

		// now send the detected URLs to either STDOUT or store in the 
		// sqlite DB	
		$pdo = Zend_Db::factory('Pdo_Sqlite', array("dbname" => $outputDbFileName));
		$urlResourceTable = new Triumph_UrlTagTable($pdo);
		$urlResourceTable->saveUrls($arrUrls, $sourceDir);
		echo "Url dectection complete, written to {$outputDbFileName}\n";
	}
	else {
		if (!empty($arrUrls)) {
			echo str_pad("URL", 60) . str_pad("File name", 90) . str_pad("Class", 20) . str_pad("Method", 20) . "\n";
			foreach ($arrUrls as $url) {
				echo str_pad($url->url, 60);
				echo str_pad($url->fileName, 90);
				echo str_pad($url->className, 20);
				echo str_pad($url->methodName, 20);
				echo "\n";
			}
		}
		else {
			echo "No URLs were detected for {$sourceDir}\n";
		}
	}
}

/**
 * This function will use the resource cache to lookup all controllers and their methods.  Then it
 * will create a Triumph_Url instance for each method; note that the routes file is 
 * also consulted and we will generate URLs for the default controller.
 *
 * @param  string  $sourceDir            the root directory of the project in question
 * @param  string  $resourceDbFileName   the location of the resource cache SQLite file; as created by Triumph
 * @param  string  $host                 the hostname of the application; this will be used a the prefix on all URLs
 * @param  boolean $doSkip               out parameter; if TRUE then this detector does not know how
 *                                       to detect URLs for the given source directory; this situation
 *                                       is different than zero URLs being detected.
 * @return Triumph_Url[]               array of Triumph_Url instances the detected URLs
 */
function detectUrls($sourceDir, $resourceDbFileName, $host, &$doSkip) {
	$doSkip = TRUE;
	if (!is_file($resourceDbFileName)) {
		return array();
	}
	$sourceDir = \opstring\ensure_ends_with($sourceDir, DIRECTORY_SEPARATOR);
	$host = \opstring\ensure_ends_with($host, '/');

	// TODO: handle multiple apps
	// need this define so that we can include code igniter files directly
	if (!defined('BASEPATH')) {
		define('BASEPATH', '');
	}
	if (!is_file($sourceDir . 'application/config/routes.php') ||
		!is_file($sourceDir . 'application/config/config.php')) {
		
		// this source directory does not contain a code igniter directory.
		return array();
	}
	$doSkip = FALSE;
	
	// get the code igniter configuration so that we can use it to build the
	// correct routes
	$config = array();
	$route = array();
	include($sourceDir . 'application/config/routes.php');
	include($sourceDir . 'application/config/config.php');

	$allUrls = array();	

	// lookup all controller files from the resource cache, only controllers are accessible via URLs
	// since file names in the cache are OS dependant, need to use the correct directory separators
	$pdo = Zend_Db::factory('Pdo_Sqlite', array("dbname" => $resourceDbFileName));
	$fileItemTable = new Triumph_FileItemTable($pdo);
	$controllerDir = $sourceDir . 'application' . DIRECTORY_SEPARATOR . 'controllers';
	$matchingFiles = $fileItemTable->MatchingFiles($controllerDir);
	
	// lookup all of the methods for all controller files.
	$resourceTable = new Triumph_ResourceTable($pdo);
	$methods = $resourceTable->PublicMethodsFromFiles($matchingFiles);
	foreach ($methods as $resource) {
		
		// need to handle any sub-directories underneath the controllers; as the subdirectory
		// is propagated in the URL
		$controllerFile = \opstring\after($resource->fullPath, $controllerDir);
		$subDirectory = dirname($controllerFile);
		if ('\\' == $subDirectory || '/' == $subDirectory) {
		
			// hack to work around special case when there is no subdirectory
			$subDirectory = '';
		}
		
		// constructors are not web-accessible
		// code igniter makes methods that start with underscore
		if (\opstring\compare_case('__construct', $resource->identifier) && !\opstring\begins_with($resource->identifier, '_')) {
			
			// TODO: any controller arguments ... should get these from the user somehow
			$extra = '';
			$appUrl = makeUrl($route, $config, $subDirectory, $resource->fullPath, $resource->className, $resource->identifier, $extra);
			$appUrl->url = $host . $appUrl->url;
			$allUrls[] = $appUrl;
		}
	}
	return $allUrls;
}

/**
 * Resolves the given class name - method name pair into the URL.
 *
 * @param  array  $route the code igniter $routes array
 * @param  array  $config the code igniter $config array
 * @param  string $subDirectory  if non-empty means that the controller is in a sub-directory of application/controllers
 * @param  string $fileName the name of the controller file
 * @param  string $className the name of the controller class
 * @param  string $methodName the name of the method
 * @param  string $extra any extra segments to append to the URL
 * @return Triumph_Url the full URL after routing rules have been applied.
 */
function makeUrl($route, $config, $subDirectory, $fileName, $className, $methodName, $extra) {
	
	// this function was taken from http://codeigniter.com/forums/viewthread/102438
	// with the following changes:
	// 1. including routes directly instead of using CI object
	//    as I was having problems including the bootstap file here
	$uri = '';
	if ($subDirectory) {
		$uri .= $subDirectory . '/';
	}
	$uri .= $className . '/' . $methodName;
	if ($extra) {
		$uri .= '/' . $extra;
	}
	
	// in case subdir has leading slash; routes usually don't
	$uri = ltrim($uri, '/');
	$uri = strtolower($uri);
	
	if (in_array($uri, $route)) {
		$uri = array_search($uri, $route);
	}    
	else {
		foreach ($route as $singleRoute => $replace) {
			$singleRoute   = preg_split('/(\(.+?\))/', $singleRoute, -1, PREG_SPLIT_DELIM_CAPTURE);
			$replace = preg_split('/(\$\d+)/', $replace, -1, PREG_SPLIT_DELIM_CAPTURE);
			if (count($singleRoute) != count($replace)) {
				continue;
			}
			$newroute = $newreplace = '';
			for ($i = 0; $i < count($singleRoute); $i++) {
				if ($i % 2) {
					$newroute .= $replace[$i];
					$newreplace .= $singleRoute[$i];
				}
				else {
					$newroute .= $singleRoute[$i];
					$newreplace .= $replace[$i];
				}
				$newreplace = str_replace(':any', '.+', str_replace(':num', '[0-9]+', $newreplace));
				if (preg_match("#^$newreplace\$#", $uri)) {
					$uri = preg_replace("#^$newreplace\$#", $newroute, $uri);
					break;
				}
			}
		}
	}
	
	if (0 == \opstring\compare('default_controller', $uri)) {
		$uri = '';
	}
	
	// respect the url suffix and index page from the config. for urls, make sure there is only one ending '/'
	// url already has leading slash
	// make sure url never has leading slash
	$indexPage = $config['index_page'];
	$indexPage = \opstring\ensure_ends_with($indexPage, '/');
	if (!empty($uri)) {
		$url = $indexPage . trim($uri . '/');
		$url = ltrim($url, '/');
	}
	else {
		$url = '';
	}
	if (isset($config['url_suffix']) && $config['url_suffix']) {
		$url .= $config['url_suffix'];
	}
	$triumphUrl = new Triumph_UrlTag($url, $fileName, $className, $methodName);
	return $triumphUrl;
}


// start running this script
parseArgs();