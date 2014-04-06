<?php

/**
 * The goal of this script is to discover template files for your PHP Laravel projects. This means
 * that given a triumph call stack, this script will list all of the template files that are used by the 
 * given controller functions. This script will be called via a command line; it is a normal command line script.
 *
 * This script is part of Triumph's Template Files Detection feature; it enables the editor to have a 
 * list of all of a project's template files so that the user can easily open / jump to templates. More
 * info can be about Triumph's template files detector feature can be found at 
 * http://docs.triumph4php.com/template-file-detectors/
 */

// the bootstrap file setups up the include path and autoload mechanism so that
// all libraries are accessible without needing to be required.
require_once('bootstrap.php');

/**
 * This function will look at the given program arguments and will validate them.
 * If the required arguments have been passed then it will call the template detector function,
 * otherwise it will stop the program.
 * This function is called at the end of this script (see last line of this file).
 * The help text below describes the arguments that are needed.
 */
function parseArgs() {
	$rules = array(
		'sourceDir|d=s' => 'Required. The base directory that the project resides in',
		'detectorDbFileName|i=s' => 'Required. SQLite file that contains the call stack to examine. This file is created by ' .
			'Triumph; Triumph performs INSERTs as it generates a call stack.',
		'outputDbFileName|o-s' => 'Optional. If given, the output will be written to the given file. If not given, output goes to STDOUT.',
		'help|h' => 'This help message'
	);
	$opts = new Zend_Console_Getopt($rules);

	// trim any quotes
	$sourceDir = trim($opts->getOption('sourceDir'), " \r\n\t'\"");
	$detectorDbFileName = trim($opts->getOption('detectorDbFileName'), " \r\n\t'\"");
	$outputDbFileName = trim($opts->getOption('outputDbFileName'), " \r\n\t'\"");
	$help = trim($opts->getOption('help'), " \r\n\t'\"");

	if ($help) {
		$helpMessage = <<<EOF
The goal of this script is to discover template files for your PHP Laravel projects. This means
that given a triumph call stack, this script will list all of the template files that are used by the 
given controller functions. This script will be called via a command line; it is a normal command line script.

This script is part of Triumph's Template Files Detection feature; it enables the editor to have a 
list of all of a project's template files so that the user can easily open / jump to templates. More
info can be about Triumph's template file detector feature can be found at 
http://docs.triumph4php.com/template-file-detectors/

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

	if (!$detectorDbFileName) {
		echo "Missing argument: --detectorDbFileName. See --help for details.\n";
		exit(-1);
	}
	
	if (!extension_loaded('pdo_sqlite') || !extension_loaded('PDO')) {
		echo "The script " . basename(__FILE__) . " requires the PDO and pdo_sqlite PHP extensions.";
		exit(-1);
	}

	// call the function that will return all detected templates
	$doSkip = TRUE;
	$arrTemplates = detectTemplates($sourceDir, $detectorDbFileName, $doSkip);
	if ($doSkip) {
		echo "Detector " . basename(__FILE__, '.php') . " cannot detect template files for {$sourceDir} ... skipping\n";
	}
	else if ($outputDbFileName) {

		// now send the detected templates to either STDOUT or store in the 
		// sqlite DB	
		$pdo = Zend_Db::factory('Pdo_Sqlite', array("dbname" => $outputDbFileName));
		$templateFileTable = new Triumph_TemplateFileTagTable($pdo);
		$templateFileTable->saveTemplateFiles($arrTemplates, $sourceDir);
		echo "Template file detection complete, written to {$outputDbFileName}\n";
	}
	else {
		if (!empty($arrTemplates)) {
			echo str_pad("Template File", 60) . str_pad("Variables", 90) . "\n";
			foreach ($arrTemplates as $template) {
				echo str_pad($template->fullPath, 60);
				echo str_pad(join(',', $template->variables), 90);
				echo "\n";
			}
		}
		else {
			echo "No template files were detected for {$sourceDir}\n";
		}
	}
}

/**
 * This function will use the resource cache to lookup all controllers and their methods.  Then it
 * will create a Triumph_TemplateFileTag instance for each method.
 *
 * @param  string $sourceDir            the root directory of the project in question
 * @param  string $detectorDbFileName   the location of the resource cache SQLite file; as created by Triumph
 * @param  boolean $doSkip              out parameter; if TRUE then this detector does not know how
 *                                      to detect templates for the given source directory; this situation
 *                                      is different than zero templates being detected.
 * @return Triumph_TemplateFileTag[]  array of Triumph_TemplateFileTag instances the detected template files and their variables
 */
function detectTemplates($sourceDir, $detectorDbFileName, &$doSkip) {
	$allTemplates = array();
	if (!is_file($detectorDbFileName)) {
		return $allTemplates;
	}
	
	// need to check that this detector is able to recognize the directory structure of sourceDir
	// if not, then we need to skip detection by returning immediately and setting $doSkip to TRUE.
	// by skipping detection, we prevent any detected templates from the previous detection script
	// from being deleted.
	$doSkip = TRUE;
	
	// for laravel, we look for the artisan script. if we don't have the artisan script assume
	// that this source is not a laravel project.
	$sourceDir = \opstring\ensure_ends_with($sourceDir, DIRECTORY_SEPARATOR);
	if (!is_file($sourceDir . 'artisan')) {	
		return $allTemplates;
	}
	$doSkip = FALSE;
	
	// add your logic here; usually it will consist of querying the SQLite database in $detectorDbFileName
	// recursing though the call stack and picking the method calls for templates.
	$pdo = Zend_Db::factory('Pdo_Sqlite', array("dbname" => $detectorDbFileName));
	$callStackTable = new Triumph_CallStackTable($pdo);
	$callStacks = $callStackTable->load();
	
	// figure out the variables
	$scopes = $callStackTable->splitScopes($callStacks);
	
	// now go through each scope, looking for calls to $this->load->view
	foreach ($scopes as $scope) {
		
		// list of all method calls used to find calls to view method
		$methodCalls = $callStackTable->getMethodCalls($scope);
		
		// list of all property accesses, make sure that calls to view method
		// are used on the loader member variables
		$propertyCalls = $callStackTable->getPropertyCalls($scope);
		$variableCalls = $callStackTable->getVariables($scope);

		foreach ($methodCalls as $destinationVariable => $call) {
			if (\opstring\compare_case($call->methodName, 'make') == 0 
				&& \opstring\compare_case($call->objectName, 'View') == 0 ) {
				
				// get the function arguments to View::make
				// the first argument to make is the template file
				// the second argument is an array of data (the template vars)
				$template = $variableCalls[$call->functionArguments[0]];
				$data = $variableCalls[$call->functionArguments[1]];
				if ($template) {					
					$currentTemplate = new Triumph_TemplateFileTag();
					$currentTemplate->variables = array();
					$currentTemplate->fullPath = '';
					$currentTemplate->fullPath = $template->scalarValue;
					
					// view file may have an extension; if it has an extension use that; otherwise use the default (.php)
					if (stripos($currentTemplate->fullPath, '.') === FALSE) {
						$currentTemplate->fullPath .= '.php';
					}
			
					// not using realpath() so that Triumph can know that a template file has not yet been created
					// or the programmer has a bug in the project.
					$currentTemplate->fullPath = \opstring\replace($currentTemplate->fullPath, '/', DIRECTORY_SEPARATOR);
					$currentTemplate->fullPath = \opstring\replace($currentTemplate->fullPath, '\\', DIRECTORY_SEPARATOR);
					$currentTemplate->fullPath = $sourceDir . 'app' . DIRECTORY_SEPARATOR . 
						'views' . DIRECTORY_SEPARATOR . $currentTemplate->fullPath;
					$allTemplates[] = $currentTemplate;
				}
				
				// argument 2 is an array of template variables
				if (isset($variableCalls[$call->functionArguments[1]])) {
					$variableCall = $variableCalls[$call->functionArguments[1]];
					$arrayKeys = $callStackTable->getArrayKeys($scope, $variableCall->destinationVariable);
					foreach ($arrayKeys as $key) {
			
						// add the siguil here; editor expects us to return variables
						$currentTemplate->variables[] = '$' . $key;
					}
					$allTemplates[count($allTemplates) - 1] = $currentTemplate;
				}
			}
		}
	}
	return $allTemplates;
}


// start running this script
parseArgs();