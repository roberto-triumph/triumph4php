<?php

/**
 * The goal of this script is to discover all of the database connections for your PHP projects. This means
 * that it will list all of the datbase connections that all of the projects use. This script
 * will be called via a command line; it is a normal command line script.
 *
 * This script is part of MVC Editor's Database Detection feature; it enables the editor to have a 
 * list of all of a project's datatabase connections so that the user can easily open / jump to tables
 * without needing to re-enter them in the editor. More
 * info can be about MVC Editor's Database detector feature can be found at 
 * http://code.google.com/p/mvc-editor/wiki/DatabaseDetectors
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

This script is part of MVC Editor's Database Detection feature; it enables the editor to have a 
list of all of a project's datatabase connections so that the user can easily open / jump to tables
without needing to re-enter them in the editor. More
info can be about MVC Editor's Database detector feature can be found at 
http://code.google.com/p/mvc-editor/wiki/DatabaseDetectors


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
		$databaseTagTable = new MvcEditor_DatabaseTagTable($pdo);
		$databaseTagTable->saveDatabaseTags($arrDatabases, $sourceDir);
		echo "Database dectection complete, written to {$outputDbFileName}\n";
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
 * will create a MvcEditor_DatabaseTag connection.
 *
 * @param  string $sourceDir            the root directory of the project in question
 * @param  boolean $doSkip              out parameter; if TRUE then this detector does not know how
 *                                      to detect databases for the given source directory; this situation
 *                                      is different than zero databases being detected.
 * @return MvcEditor_DatabaseTag[]      array of MvcEditor_DatabaseTag instances the detected databases
 */
function detectDatabases($sourceDir, &$doSkip) {
	$doSkip = TRUE;
	$allDatabases = array();
	$sourceDir = \opstring\ensure_ends_with($sourceDir, DIRECTORY_SEPARATOR);
	if (!is_file($sourceDir . 'application/config/development/database.php') &&
		!is_file($sourceDir . 'application/config/database.php')) {
		return $allDatabases;
	}
	
	// need to check that this detector is able to recognize the directory structure of sourceDir
	// if not, then we need to skip detection by returning immediately and setting $doSkip to TRUE.
	// by skipping detection, we prevent any detected databases from the previous detection script
	// from being deleted.
	$doSkip = FALSE;
	// need this define so that we can include code igniter files directly
	define('BASEPATH', '');
	
	// database config file can be in the environment directory
	// for now just get the development environment info
	if (is_file($sourceDir . 'application/config/development/database.php')) {
		include ($sourceDir . 'application/config/development/database.php');
		if ($db) {
			foreach ($db as $groupName => $groupConnection) {
				if (strcasecmp('mysql', $groupConnection['dbdriver']) == 0) {
					$tag = tagFromDbArray($groupName, $groupConnection);
					$allDatabases[] = $tag;
				}
			}
		}
	}
	else if (is_file($sourceDir . 'application/config/database.php')) {
		$db = array();
		include ($sourceDir . 'application/config/database.php');
		if ($db) {
			foreach ($db as $groupName => $groupConnection) {
				if (\opstring\compare_case('mysql', $groupConnection['dbdriver']) == 0 || 
					\opstring\compare_case('mysqli', $groupConnection['dbdriver']) == 0) {
					$tag = tagFromDbArray($groupName, $groupConnection);
					$allDatabases[] = $tag;
				}
			}
		}
	}
	return $allDatabases;
}

function tagFromDbArray($groupName, $groupConnection) {
	// port is not there by default
	$port = 0;
	if (isset($groupConnection['port'])) {
		$port = $groupConnection['port'];
	}

	$tag = new MvcEditor_DatabaseTag(MvcEditor_DatabaseTag::DRIVER_MYSQL, 
		$groupName, $groupConnection['hostname'], 
		$port, $groupConnection['database'], 
		'', 
		$groupConnection['username'], 
		$groupConnection['password']);
	return $tag;
}



// start running this script
parseArgs();