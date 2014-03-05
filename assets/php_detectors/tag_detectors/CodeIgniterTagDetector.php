<?php

/**
 * The goal of this script is to discover all of the 'dynamic' tags for any PHP projects that use the
 * CodeIgniter framework. In a nutshell, Triumph cannot provide Code completion for calling the
 * core CodeIgniter classes within a controller (ie "$this->input" will not trigger code completion").
 * This script will add he CI core objects and libraries to the Triumph tag cache so that 
 * Triumph can know about them during code completion. It will also
 * detect any user-created libraries as well. This script.
 * will be called via a command line; it is a normal command line script.
 *
 * This script is part of Triumph's Tag Detection feature; it enables the editor to have a 
 * more useful list when performing code completion. More
 * info can be about Triumph's Tag detector feature can be found at 
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
The goal of this script is to discover all of the 'dynamic' tags for any PHP projects that use the
CodeIgniter framework. In a nutshell, Triumph cannot provide Code completion for calling the
core CodeIgniter classes within a controller (ie "$this->input" will not trigger code completion").
This script will add he CI core objects and libraries to the Triumph tag cache so that 
Triumph can know about them during code completion. It will also
detect any user-created libraries as well. This script.
will be called via a command line; it is a normal command line script.

This script is part of Triumph's Tag Detection feature; it enables the editor to have a 
more useful list when performing code completion. More
info can be about Triumph's Tag detector feature can be found at 
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
 * @return Triumph_DetectedTag[]         array of Triumph_DetectedTag instances the detected tags
 */
function detectTags($sourceDir, &$doSkip) {
	$allTags = array();
	
	// need to check that this detector is able to recognize the directory structure of sourceDir
	// if not, then we need to skip detection by returning immediately and setting $doSkip to TRUE.
	// by skipping detection, we prevent any detected URLs from the previous detection script
	// from being deleted.
	$doSkip = TRUE;
	$sourceDir = \opstring\ensure_ends_with($sourceDir, DIRECTORY_SEPARATOR);
	$codeIgniterSystemDir = locateSystemDir($sourceDir);
	if (\opstring\length($codeIgniterSystemDir) <= 0) {
		return $allTags;
	}
	$doSkip = FALSE;
	
	// need this define so that we can include code igniter files directly
	define('BASEPATH', '');

	coreResources($sourceDir, $codeIgniterSystemDir, $allTags);
	libraryResources($sourceDir, $codeIgniterSystemDir, $allTags);

	// TODO: handle more than one application
	$appDir =  $sourceDir . DIRECTORY_SEPARATOR . 'application';
	$modelDir = $appDir . DIRECTORY_SEPARATOR . 'models';
	modelResources($sourceDir, $modelDir, $allTags);

	// the "super" object
	$allTags[] = Triumph_DetectedTag::CreateMethod('CI_Controller', 'get_instance', '\CI_Controller');

	return $allTags;
}

/**
 * @return string the directory (full path) where CodeIgniter files (the 'system' directory) is
 * located.  This could be empty if the code igniter system directory could not be found. The returned
 * directory has OS-specific separators (it uses the realpath() function)
 */
function locateSystemDir($dir) {

	// we will locate the system directory by readind the code igniter bootstrap file
	// and getting the $systemDir variable
	if (!is_file($dir . 'index.php')) {
		return '';
	}
	$tokens = token_get_all(file_get_contents($dir . 'index.php'));
	
	// look for the system variable; it will contain the location of the system dir
	$index = 0;
	$systemDir = '';
	
	while (($index + 5) < count($tokens)) {
		$offset = 0;
		
		// tokenizer returns whitespace as a token; need to account when there is no whitespace
		// will need to skip any whitespace tokens
		if(is_array($tokens[$index + $offset]) && $tokens[$index + $offset][0] == T_WHITESPACE) {
			$offset++;
		}
		$token = $tokens[$index + $offset];
		$offset++;
		
		if(is_array($tokens[$index + $offset]) && $tokens[$index + $offset][0] == T_WHITESPACE) {
			$offset++;
		}
		$lookaheadToken = $tokens[$index + $offset];
		$offset++;
		
		if(is_array($tokens[$index + $offset]) && $tokens[$index + $offset][0] == T_WHITESPACE) {
			$offset++;
		}
		$nextLookaheadToken = $tokens[$index + $offset];
		
		// look for the line "$system_path = 'DDDDD'
		if (is_array($token) && $token[0] == T_VARIABLE &&  $token[1] == '$system_path' 
				&& $lookaheadToken == '='
				&& is_array($nextLookaheadToken) && $nextLookaheadToken[0] == T_CONSTANT_ENCAPSED_STRING) {
			$systemDir = $nextLookaheadToken[1];
			break;
		}
		$index++;
	}
	
	// remove the quotes and normalize
	$systemDir = \opstring\sub($systemDir, 1, -1);
	$systemDir = \opstring\ensure_ends_with($systemDir, DIRECTORY_SEPARATOR);
	
	// systemDir may be an absolute path
	if (is_file($dir . $systemDir . 'core/CodeIgniter.php')) {
		$systemDir = realpath($dir . $systemDir);
		return $systemDir;
	}
	else if (is_file($systemDir . 'core/CodeIgniter.php')) {
		$systemDir = realpath($systemDir); 
		return $systemDir;
	}
	return '';
}

function coreResources($dir, $codeIgniterSystemDir, &$allTags) {
	$comment = '';
	
	// the core libraries are automatically added to the CI super object
	$codeIgniterSystemDir = \opstring\ensure_ends_with($codeIgniterSystemDir, DIRECTORY_SEPARATOR);
	foreach (glob($codeIgniterSystemDir . DIRECTORY_SEPARATOR . 'core' . DIRECTORY_SEPARATOR . '*.php') as $libFile) {
		$baseName = basename($libFile, '.php');
		if ('CodeIgniter' != $baseName && 'Common' != $baseName && 'Controller' != $baseName
				&& 'Exceptions' != $baseName && 'Exceptions' != $baseName && 'Model' != $baseName) {
			$propertyType = 'CI_' . $baseName;
			$propertyName = strtolower($baseName);
			
			$allTags[] = Triumph_DetectedTag::CreateMember('CI_Controller', $propertyName, $propertyType, $comment);
			$allTags[] = Triumph_DetectedTag::CreateMember('CI_Model', $propertyName, $propertyType, $comment);
		}
	}
	
	// the DB library, this requires checking to see if active record is enabled
	if (is_file($dir . '/application/config/database.php')) {
		$db = array();
		include ($dir . '/application/config/database.php');
		$propertyType = isset($active_record) && $active_record ? 'CI_DB_active_record' : 'CI_DB_driver';
		$allTags[] = Triumph_DetectedTag::CreateMember('CI_Controller', 'db', $propertyType, $comment);
		$allTags[] = Triumph_DetectedTag::CreateMember('CI_Model', 'db', $propertyType, $comment);
	}
	
	// alias the Loader library; seems that there is two properties
	$allTags[] = Triumph_DetectedTag::CreateMember('CI_Controller', 'load', 'CI_Loader', $comment);
	$allTags[] = Triumph_DetectedTag::CreateMember('CI_Model', 'load', 'CI_Loader', $comment);
	
}

function libraryResources($dir, $codeIgniterSystemDir, &$allTags) {
	
	// TODO: handle more than one application
	$appDir =  $dir . DIRECTORY_SEPARATOR . 'application';
	
	// the system libraries are automatically added to the CI super object,
	// the class name is named the same as the file name but with the CI_ prefix
	foreach(glob($codeIgniterSystemDir . DIRECTORY_SEPARATOR . 'libraries' . DIRECTORY_SEPARATOR . '*.php', 0) as $libFile) {
		$key = basename($libFile, '.php');
		$propertyType = 'CI_' . $key;
		$propertyName = strtolower($key);
		$allTags[] = Triumph_DetectedTag::CreateMember('CI_Controller', $propertyName, $propertyType);
		$allTags[] = Triumph_DetectedTag::CreateMember('CI_Model', $propertyName, $propertyType);
	}
	
	// user-created libraries. need to get the configured prefix
	// user-created library class names have the same prefix as the files themselves
	include $appDir . DIRECTORY_SEPARATOR . 'config' . DIRECTORY_SEPARATOR . 'config.php';
	$prefix = $config['subclass_prefix'];
	foreach (glob($appDir . DIRECTORY_SEPARATOR . 'libraries' . DIRECTORY_SEPARATOR . $prefix . '*.php', 0) as $libFile) {		
		$key = basename($libFile, '.php');
		$propertyType = $key;
		
		// the property name will NOT have the prefix
		$propertyName = strtolower(substr($key, strlen($prefix)));
		$allTags[] = Triumph_DetectedTag::CreateMember('CI_Controller', $propertyName, $propertyType);
		$allTags[] = Triumph_DetectedTag::CreateMember('CI_Model', $propertyName, $propertyType);
	}
}

function modelResources($dir, $modelDir, &$allTags) {
	
	// the models are added to the Controller super object by the loader
	
	foreach (glob($modelDir . DIRECTORY_SEPARATOR . '*.php') as $modelFile) {
		$key = basename($modelFile, '.php');
		$propertyType = ucfirst($key);
		
		// the property name will NOT have the prefix
		$propertyName = strtolower($key);
		$allTags[] = Triumph_DetectedTag::CreateMember('CI_Controller', $propertyName, $propertyType);
	}
	
	// models can be located in sub-directories need to recurse down sub-dirs
	$dirId = opendir($modelDir);
	if ($dirId) {
		$item = readdir($dirId);
		while($item !== FALSE) {
		
			// ignore the parent dir, also any hidden dirs
			if ($item != '.' && $item != '..' && !substr($item, 0, 1) != '.') {
				$modelSubDir = $modelDir . DIRECTORY_SEPARATOR . $item;
				if (is_dir($modelSubDir)) {
					modelResources($dir, $modelSubDir, $allTags);
				}
			}
			$item = readdir($dirId);
		}
	}
	closedir($dirId);
}

// start running this script
parseArgs();