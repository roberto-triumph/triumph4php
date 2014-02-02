<?php

/**
 * The goal of this script is to discover all of the config files for your PHP projects. This means
 * that it will list all of the confi files for all of the projects. This script
 * will be called via a command line; it is a normal command line script.
 *
 * This script is part of MVC Editor's Config Detection feature; it enables the editor to have a 
 * list of all of a project's config files so that the user can easily open / jump to them. More
 * info can be about MVC Editor's config detector feature can be found at 
 * http://code.google.com/p/mvc-editor/wiki/ConfigDetectors
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

This script is part of MVC Editor's Config Detection feature; it enables the editor to have a 
list of all of a project's config files so that the user can easily open / jump to them. More
info can be about MVC Editor's config detector feature can be found at 
http://code.google.com/p/mvc-editor/wiki/ConfigDetectors


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
		$configTagTable = new MvcEditor_ConfigTagTable($pdo);
		$configTagTable->saveConfigTags($arrConfigTags, $sourceDir);
		echo "Config dectection complete, written to {$outputDbFileName}\n";
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
 * will create a MvcEditor_ConfigTag instance for each config file that it finds.
 *
 * @param  string $sourceDir            the root directory of the project in question
 * @param  boolean $doSkip              out parameter; if TRUE then this detector does not know how
 *                                      to detect config for the given source directory; this situation
 *                                      is different than zero condifs being detected.
 * @return MvcEditor_ConfigTag[]        array of MvcEditor_ConfigTag instances the detected configs
 */
function detectConfigs($sourceDir, &$doSkip) {
	$allConfigs = array();
	$doSkip = TRUE;
	
	// need to check that this detector is able to recognize the directory structure of sourceDir
	// if not, then we need to skip detection by returning immediately and setting $doSkip to TRUE.
	// by skipping detection, we prevent any detected URLs from the previous detection script
	// from being deleted.
	$sourceDir = \opstring\ensure_ends_with($sourceDir, DIRECTORY_SEPARATOR);
	if (!is_file($sourceDir . 'application/config/database.php')) {	
		return $allConfigs;
	}
	$doSkip = FALSE;
	$allConfigs = array(
		new MvcEditor_ConfigTag('AutoLoad', realpath($sourceDir . 'application/config/autoload.php')),
		new MvcEditor_ConfigTag('Config', realpath($sourceDir . 'application/config/config.php')),
		new MvcEditor_ConfigTag('Constants', realpath($sourceDir . 'application/config/constants.php')), 
		new MvcEditor_ConfigTag('Database', realpath($sourceDir . 'application/config/database.php')), 
		new MvcEditor_ConfigTag('DocTypes', realpath($sourceDir . 'application/config/doctypes.php')),
		new MvcEditor_ConfigTag('Foreign Characters', realpath($sourceDir . 'application/config/foreign_chars.php')),
		new MvcEditor_ConfigTag('Hooks', realpath($sourceDir . 'application/config/hooks.php')),
		new MvcEditor_ConfigTag('Mime Types', realpath($sourceDir . 'application/config/mimes.php')),
		new MvcEditor_ConfigTag('Profiler', realpath($sourceDir . 'application/config/profiler.php')),
		new MvcEditor_ConfigTag('Routes', realpath($sourceDir . 'application/config/routes.php')),
		new MvcEditor_ConfigTag('Smileys', realpath($sourceDir . 'application/config/smileys.php')),
		new MvcEditor_ConfigTag('User Agents', realpath($sourceDir . 'application/config/user_agents.php'))
	);
	return $allConfigs; 
}


// start running this script
parseArgs();