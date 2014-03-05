<?php

/**
 * The goal of this script is to discover all of the 'dynamic' tags for your PHP projects. This means
 * that it will list all of the tags that correspond any properties or methods added to classes at runtime. This script
 * will be called via a command line; it is a normal command line script.
 *
 * This script is part of Triumph's Tag Detection feature; it enables the editor to have a 
 * list of all of a project's urls so that the user can easily open / jump to URLs. More
 * info can be about Triumph's URL detector feature can be found at 
 * http://docs.triumph4php.com/tag-detectors/
 */

// the bootstrap file setups up the include path and autoload mechanism so that
// all libraries are accessible without needing to be required.
require_once('bootstrap.php');

/**
 * This function will look at the given program arguments and will validate them.
 * If the required arguments have been passed then it will call the Tag detector function,
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
 The goal of this script is to discover all of the 'dynamic' tags for your PHP projects. This means
 that it will list all of the tags that correspond any properties or methods added to classes at runtime. This script
 will be called via a command line; it is a normal command line script.
 
 This script is part of Triumph's Tag Detection feature; it enables the editor to have a 
 list of all of a project's urls so that the user can easily open / jump to URLs. More
 info can be about Triumph's URL detector feature can be found at 
 http://docs.triumph4php.com/tag-detectors/

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

	// call the function that will return all detected URLs
	$doSkip = TRUE;
	$arrTags = detectTags($sourceDir, $doSkip);
	if ($doSkip) {
		echo "Detector " . basename(__FILE__, '.php') . " cannot detect tags for  {$sourceDir} ... skipping\n";
	}
	else if ($outputDbFileName) {

		// now send the detected URLs to either STDOUT or store in the 
		// sqlite DB	
		$pdo = Zend_Db::factory('Pdo_Sqlite', array("dbname" => $outputDbFileName));
		$resourceTable = new Triumph_DetectedTagTable($pdo);
		$resourceTable->saveTags($arrTags, $sourceDir);
		echo "Tag detection complete, written to {$outputDbFileName}\n";
	}
	else {
		if (!empty($arrTags)) {
			echo 
				str_pad("Type", 10) . 
				str_pad("Class", 25) . str_pad("Member", 25) . 
				str_pad("Return Type", 20)  . "\n";
			foreach ($arrTags as $tag) {
				echo str_pad(Triumph_DetectedTag::typeString($tag->type), 10);
				echo str_pad($tag->className, 25);
				echo str_pad($tag->identifier, 25);
				echo str_pad($tag->returnType, 25);
				echo "\n";
			}
		}
		else {
			echo "No tags were detected for {$sourceDir}\n";
		}
	}
}

/**
 *
 * @param  string $sourceDir            the root directory of the project in question
 * @param  boolean $doSkip              out parameter; if TRUE then this detector does not know how
 *                                      to detect URLs for the given source directory; this situation
 *                                      is different than zero URLs being detected.
 * @return Triumph_Resource[]         array of Triumph_Resource instances the detected tags
 */
function detectTags($sourceDir, &$doSkip) {
	$allTags = array();
	
	// need to check that this detector is able to recognize the directory structure of sourceDir
	// if not, then we need to skip detection by returning immediately and setting $doSkip to TRUE.
	// by skipping detection, we prevent any detected URLs from the previous detection script
	// from being deleted.
	$doSkip = TRUE;
	
	// add your logic here; 
	return $allTags;
}


// start running this script
parseArgs();