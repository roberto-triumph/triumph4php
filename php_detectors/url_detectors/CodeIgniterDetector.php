<?php

require_once(dirname(__FILE__) . '/../bootstrap.php');

function detectUrls($sourceDir, $resourceDbFileName, $host) {
	if (!is_file($resourceDbFileName)) {
		return array();
	}
	$sourceDir = \opstring\ensure_ends_with($sourceDir, '/');
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
	$config = array();
	$route = array();
	include($sourceDir . 'application/config/routes.php');
	include($sourceDir . 'application/config/config.php');

	$allUrls = array();	

	$pdo = Zend_Db::factory('Pdo_Sqlite', array("dbname" => $resourceDbFileName));
	Zend_Db_Table_Abstract::setDefaultAdapter($pdo);
	$fileItemTable = new MvcEditor_FileItemTable();

	// get all controller files, only controllers are accessible via URLs
	$controllerDir = $sourceDir . 'application/controllers';
	$matchingFiles = $fileItemTable->MatchingFiles($controllerDir);
	if (empty($matchingFiles)) {
		$controllerDir = $sourceDir . 'application\controllers';
		$matchingFiles = $fileItemTable->MatchingFiles(realpath($controllerDir));
	}

	$resource = new MvcEditor_Resource();
	$methods = $resource->MethodsFromFiles($pdo, $matchingFiles);
	foreach ($methods as $resource) {
		
		// normalize any paths to have forward slash
		// doing this so that this logic works on MSW, linux, AND the vfs:// stream (tests)
		$controllerDir = \opstring\replace($controllerDir, DIRECTORY_SEPARATOR, '/');
		$resourceFullPath = \opstring\replace($resource->fullPath, DIRECTORY_SEPARATOR, '/');
		
		
		$controllerFile = \opstring\after($resourceFullPath, $controllerDir);
		$subDirectory = dirname($controllerFile);
		if ('\\' == $subDirectory) {
		
			// hack to work around special case when there is no subdirectory
			$subDirectory = '';
		}
		
		// TODO: any controller arguments ... should get these from the user
		// constructors are not web-accessible
		// code igniter makes methods that start with underscore
		if (\opstring\compare_case('__construct', $resource->identifier) && !\opstring\begins_with($resource->identifier, '_')) {
			$extra = '';
			$appUrl = makeUrl($route, $config, $subDirectory, $resource->fullPath, $resource->className, $resource->identifier, $extra);
			$appUrl->url = $host . $appUrl->url;
			$allUrls[] = $appUrl;
		}
	}
	return $allUrls;
}

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
	$mvcUrl = new MvcEditor_Url($url, $fileName, $className, $methodName);
	return $mvcUrl;
}

$rules = array(
	'sourceDir|d=s' => 'The base directory that the project resides in [Required]',
	'resourceDbFileName|i=s' => 'SQLite file that contains the project\'s files, classes, and methods [Required]',
	'host|s=s' => 'Host name to start the URLs with [Required]',
	'outputDbFileName|o=s' => 'If given, the output will be written to the given file [optional, if not given, output goes to STDOUT)',
	'help|h' => 'A help message'
);
$opts = new Zend_Console_Getopt($rules);

// trim any quotes
$sourceDir = trim($opts->getOption('sourceDir'), " \r\n\t'\"");
$resourceDbFileName = trim($opts->getOption('resourceDbFileName'), " \r\n\t'\"");
$host = trim($opts->getOption('host'), " \r\n\t'\"");
$outputDbFileName = trim($opts->getOption('outputDbFileName'), " \r\n\t'\"");
$help = trim($opts->getOption('help'), " \r\n\t'\"");

if ($help) {
	echo <<<EOF
This is the gateway that will be responsible for communicating with the editor app.
This is a strict command line interface; the editor will invoke this script via a command (and 
with some arguments) and this app will parse the arguments and respond accordingly.
For example, let's say the user opened a SQL browser.  The editor needs to know the SQL credentials to
use. The editor will run this script asking for the credentials.  

   php CodeIgniterDetector.php --dir="/home/user/projects/ci_project/" 

When any argument is invalid or missing, the program will exit with an error code (-1)
EOF;
	exit(0);
}

if (!$sourceDir) {
	echo "Missing argument: --dir. See --help for details.\n";
	exit (-1);
}

if (!$resourceDbFileName) {
	echo "Missing argument: --file. See --help for details.\n";
	exit (-1);
}

if (!$host) {
	echo "Missing argument: --host. See --help for details.\n";
	exit (-1);
}

$arrUrls = detectUrls($sourceDir, $resourceDbFileName, $host);

// now send the detected URLs to either STDOUT or store in the 
// sqlite DB
if ($outputDbFileName) {
	$pdo = Zend_Db::factory('Pdo_Sqlite', array("dbname" => $outputDbFileName));
	$urlResourceTable = new MvcEditor_UrlResourceTable($pdo);
	$urlResourceTable->saveUrls($arrUrls, $sourceDir);
	echo "Url dectection complete, written to {$outputDbFileName}\n";
}
else {
	echo str_pad("URL", 60) . str_pad("File name", 90) . str_pad("Class", 20) . str_pad("Method", 20) . "\n";
	foreach ($arrUrls as $url) {
		echo str_pad($url->url, 60);
		echo str_pad($url->fileName, 90);
		echo str_pad($url->className, 20);
		echo str_pad($url->methodName, 20);
		echo "\n";
	}
}