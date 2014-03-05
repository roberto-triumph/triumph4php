<?php

/**
 * The goal of this script is to discover all of the config files for your PHP projects. This means
 * that it will list all of the confi files for all of the projects. This script
 * will be called via a command line; it is a normal command line script.
 *
 * This script is part of Triumph's Config Detection feature; it enables the editor to have a 
 * list of all of a project's config files so that the user can easily open / jump to them. More
 * info can be about Triumph's config detector feature can be found at 
 * http://docs.triumph4php.com/config-detectors/
 */

// the bootstrap file setups up the include path and autoload mechanism so that
// all libraries are accessible without needing to be required.
require_once('bootstrap.php');

/**
 * This function will look at the given program arguments and will validate them.
 * If the required arguments have been passed then it will call the config detector function,
 * otherwise it will stop the program.
 * This function is called at the end of this script (see last line of this file).
 * The help text below describes the arguments that are needed.
 */
function parseArgs() {
	$rules = array(
		'sourceDir|d=s' => 'Required. The base directory that the project resides in',
		'outputDbFileName|o-s' => 'Optional. If given, the output will be written to the given file. If not given, output goes to STDOUT.',
		'help|h' => 'This help message'
	);
	$opts = new Zend_Console_Getopt($rules);

	// trim any quotes
	$sourceDir = trim($opts->getOption('sourceDir'), " \r\n\t'\"");
	$outputDbFileName = trim($opts->getOption('outputDbFileName'), " \r\n\t'\"");
	$help = trim($opts->getOption('help'), " \r\n\t'\"");

	if ($help) {
		$helpMessage = <<<EOF
The goal of this script is to discover all of the config files for your PHP projects. This means
that it will list all of the confi files for all of the projects. This script
will be called via a command line; it is a normal command line script.

This script is part of Triumph's Config Detection feature; it enables the editor to have a 
list of all of a project's config files so that the user can easily open / jump to them. More
info can be about Triumph's config detector feature can be found at 
http://docs.triumph4php.com/config-detectors/


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
	if (!extension_loaded('pdo_sqlite') || !extension_loaded('PDO')) {
		echo "The script " . basename(__FILE__) . " requires the PDO and pdo_sqlite PHP extensions.";
		exit(-1);
	}

	// call the function that will return all detected configs
	$doSkip = TRUE;
	$arrConfigTags = detectConfigs($sourceDir, $doSkip);
	if ($doSkip) {
		echo "Detector " . basename(__FILE__, '.php') . " cannot detect configs for {$sourceDir} ... skipping\n";
	}
	else if ($outputDbFileName) {

		// now send the detected URLs to either STDOUT or store in the 
		// sqlite DB	
		$pdo = Zend_Db::factory('Pdo_Sqlite', array("dbname" => $outputDbFileName));
		$configTagTable = new Triumph_ConfigTagTable($pdo);
		$configTagTable->saveConfigTags($arrConfigTags, $sourceDir);
		echo "Config detection complete, written to {$outputDbFileName}\n";
	}
	else {
		if (!empty($arrConfigTags)) {
			echo str_pad("Label", 60) . str_pad("Full Path", 90) . "\n";
			foreach ($arrConfigTags as $url) {
				echo str_pad($url->label, 60);
				echo str_pad($url->fullPath, 90);
				echo "\n";
			}
		}
		else {
			echo "No configs were detected for {$sourceDir}\n";
		}
	}
}

/**
 * This function will look for config files in the given source directory. Then it
 * will create a Triumph_ConfigTag instance for each config file that it finds.
 *
 * @param  string $sourceDir            the root directory of the project in question
 * @param  boolean $doSkip              out parameter; if TRUE then this detector does not know how
 *                                      to detect config for the given source directory; this situation
 *                                      is different than zero condifs being detected.
 * @return Triumph_ConfigTag[]        array of Triumph_ConfigTag instances the detected configs
 */
function detectConfigs($sourceDir, &$doSkip) {
	$allConfigs = array();
	
	
	// need to check that this detector is able to recognize the directory structure of sourceDir
	// if not, then we need to skip detection by returning immediately and setting $doSkip to TRUE.
	// by skipping detection, we prevent any detected URLs from the previous detection script
	// from being deleted.
	$doSkip = TRUE;
	
	// add your logic here; usually it will consist of querying the SQLIte database in $resourceDbFileName
	// getting all controller - method pairs.
	return $allConfigs;
}


// start running this script
parseArgs();