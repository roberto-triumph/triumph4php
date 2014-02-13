<?php

/**
 * The goal of this script is to discover all of the URLs for your PHP projects. This means
 * that it will list all of the URLs that correspond all of the project's controller functions. This script
 * will be called via a command line; it is a normal command line script.
 *
 * This script is part of MVC Editor's URL Detection feature; it enables the editor to have a 
 * list of all of a project's urls so that the user can easily open / jump to URLs. More
 * info can be about MVC Editor's URL detector feature can be found at 
 * http://code.google.com/p/mvc-editor/wiki/URLDetectors
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
			'MVC Editor; MVC Editor performs INSERTs as it indexes a project.',
		'host|s=s' => 'Required. Host name to start the URLs with. MVC Editor determines the host name by using the configured ' .
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
The goal of this script is to discover all of the URLs for your PHP projecta. This means
that it will list all of the URLs that correspond all of the project's controller functions. This script
will be called via a command line; it is a normal command line script.

This script is part of MVC Editor's URL Detection feature; it enables the editor to have a 
list of all of a project's urls so that the user can easily open / jump to URLs. More
info can be about MVC Editor's URL detector feature can be found at 
http://code.google.com/p/mvc-editor/wiki/URLDetectors

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
		$urlResourceTable = new MvcEditor_UrlTagTable($pdo);
		$urlResourceTable->saveUrls($arrUrls, $sourceDir);
		echo "Url detection complete, written to {$outputDbFileName}\n";
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
 * will create a MvcEditor_Url instance for each method; note that the routes file is 
 * also consulted and we will generate URLs for the default controller.
 *
 * @param  string $sourceDir            the root directory of the project in question
 * @param  string $resourceDbFileName   the location of the resource cache SQLite file; as created by MVC Editor
 * @param  string $host                 the hostname of the application; this will be used a the prefix on all URLs
 * @param  boolean $doSkip               out parameter; if TRUE then this detector does not know how
 *                                       to detect URLs for the given source directory; this situation
 *                                       is different than zero URLs being detected.
 * @return MvcEditor_UrlTag[]           array of MvcEditor_UrlTag instances the detected URLs
 */
function detectUrls($sourceDir, $resourceDbFileName, $host, &$doSkip) {
	$allUrls = array();
	if (!is_file($resourceDbFileName)) {
		return $allUrls;
	}
	
	// need to check that this detector is able to recognize the directory structure of sourceDir
	// if not, then we need to skip detection by returning immediately and setting $doSkip to TRUE.
	// by skipping detection, we prevent any detected URLs from the previous detection script
	// from being deleted.
	$doSkip = TRUE;
	
	// add your logic here; usually it will consist of querying the SQLIte database in $resourceDbFileName
	// getting all controller - method pairs.
	return $allUrls;
}


// start running this script
parseArgs();