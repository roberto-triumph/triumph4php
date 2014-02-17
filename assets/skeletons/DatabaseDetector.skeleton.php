<?php

/**
 * The goal of this script is to discover all of the database connections for your PHP projects. This means
 * that it will list all of the datbase connections that all of the projects use. This script
 * will be called via a command line; it is a normal command line script.
 *
 * This script is part of Triumph's Database Detection feature; it enables the editor to have a 
 * list of all of a project's datatabase connections so that the user can easily open / jump to tables
 * without needing to re-enter them in the editor. More
 * info can be about Triumph's Database detector feature can be found at 
 * http://code.google.com/p/triumph4php/wiki/DatabaseDetectors
 */

// the bootstrap file setups up the include path and autoload mechanism so that
// all libraries are accessible without needing to be required.
require_once('bootstrap.php');

/**
 * This function will look at the given program arguments and will validate them.
 * If the required arguments have been passed then it will call the Database detector function,
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
The goal of this script is to discover all of the database connections for your PHP projects. This means
that it will list all of the datbase connections that all of the projects use. This script
will be called via a command line; it is a normal command line script.

This script is part of Triumph's Database Detection feature; it enables the editor to have a 
list of all of a project's datatabase connections so that the user can easily open / jump to tables
without needing to re-enter them in the editor. More
info can be about Triumph's Database detector feature can be found at 
http://code.google.com/p/triumph4php/wiki/DatabaseDetectors


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

	// call the function that will return all detected databases
	$doSkip = TRUE;
	$arrDatabases = detectDatabases($sourceDir, $doSkip);
	if ($doSkip) {
		echo "Detector " . basename(__FILE__, '.php') . " cannot detect databases for {$sourceDir} ... skipping\n";
	}
	else if ($outputDbFileName) {

		// now send the detected databases to either STDOUT or store in the 
		// sqlite DB	
		$pdo = Zend_Db::factory('Pdo_Sqlite', array("dbname" => $outputDbFileName));
		$databaseTagTable = new Triumph_DatabaseTagTable($pdo);
		$databaseTagTable->saveDatabaseTags($arrDatabases, $sourceDir);
		echo "Database detection complete, written to {$outputDbFileName}\n";
	}
	else {
		if (!empty($arrDatabases)) {
			echo str_pad("Schema", 20) . str_pad("Driver", 20) . 
			str_pad("Host", 20) . str_pad("Port", 10) . str_pad("File", 60) . 
			str_pad("Username", 20) . str_pad("Password", 20) . "\n";
			foreach ($arrDatabases as $database) {
				echo str_pad($database->schema, 30);
				echo str_pad($database->driver, 20);
				echo str_pad($database->host, 20);
				echo str_pad($database->port, 10);
				echo str_pad($database->fileName, 60);
				echo str_pad($database->user, 20);
				echo str_pad($database->password, 20);
				echo "\n";
			}
		}
		else {
			echo "No databases were detected for {$sourceDir}\n";
		}
	}
}

/**
 * This function will lookup all database connections for the given source.  Then it
 * will create a Triumph_DatabaseTag connection.
 *
 * @param  string $sourceDir            the root directory of the project in question
 * @param  boolean $doSkip              out parameter; if TRUE then this detector does not know how
 *                                      to detect databases for the given source directory; this situation
 *                                      is different than zero databases being detected.
 * @return Triumph_DatabaseTag[]      array of Triumph_DatabaseTag instances the detected databases
 */
function detectDatabases($sourceDir, &$doSkip) {
	$allDatabases = array();
	
	// need to check that this detector is able to recognize the directory structure of sourceDir
	// if not, then we need to skip detection by returning immediately and setting $doSkip to TRUE.
	// by skipping detection, we prevent any detected databases from the previous detection script
	// from being deleted.
	$doSkip = TRUE;
	
	// add your logic here; usually it will consist of looking at the file system for the correct 
	// config file.
	return $allDatabases;
}


// start running this script
parseArgs();