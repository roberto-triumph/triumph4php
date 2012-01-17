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
 * This is the gateway that will be responsible for communicating with the editor app.
 * This is a strict command line interface; the editor will invoke this script via a command (and 
 * with some arguments) and this app will parse the arguments and respond accordingly.
 * For example, let's say the user opened a SQL browser.  The editor needs to know the SQL credentials to
 * use. The editor will run this script asking for the credentials.  
 *
 *     php MvcEditorFrameworkApp.php --identifier="CI-1.4" --dir="/home/user/projects/my_blog/" --action=databaseInfo
 *
 * The script will parse the arguments. In this case, this script will know to ask the 
 * framework identified by 'CI-1.4' and will ask the framework for its model credentials. The script will create the
 * proper framework class, execute the proper method, and return the answer.
 */

define('FRAMEWORK_CLASS_PREFIX', 'MvcEditorFramework');

set_include_path(get_include_path() . PATH_SEPARATOR .  __DIR__ . '/lib/');

require_once 'Zend/Console/Getopt.php';
require_once 'Zend/Config.php';
require_once 'Zend/Config/Writer/Ini.php';

/**
 * Loads all of the framework plugins into memory
 * @return array of MvcEditorFrameworkBaseClass objects
 */
function loadFrameworks() {
	$frameworks = array();
	$dir = opendir(__DIR__);
	while ($file = readdir($dir)) {
		
		if (0 === stripos($file, FRAMEWORK_CLASS_PREFIX) && strcasecmp($file, 'MvcEditorFrameworkBase.php') && $file != basename(__FILE__)) {
			require_once($file);
			$clazz = substr($file, 0, stripos($file, '.php'));
			$frameworks[] = new $clazz;
		}
	}
	return $frameworks;
}

/**
 * Runs the given action and outputs an appropriate response.  
 * It is better that the framework objects do NOT perform any 
 * prints and we do it here instead; that way we can
 * write the data in one place.
 * note that the receiver will need to be smart and ignore
 * any prior inputs that the framework object may do.
 * action is assumed to be valid at this point
 */
function runAction($framework, $dir, $action, $outputFile) {
	if (strcasecmp($action, 'databaseInfo') == 0) {
		runDatabaseInfo($framework, $dir, $outputFile);
	}
	else if (strcasecmp($action, 'isUsedBy') == 0) {
		runIsUsedBy($dir, $outputFile);
	}
	else if (strcasecmp($action, 'configFiles') == 0) {
		runConfigFiles($framework, $dir, $outputFile);
	}
	else if (strcasecmp($action, 'resources') == 0) {
		runResources($framework, $dir, $outputFile);
	}
}

function runDatabaseInfo($framework, $dir, $outputFile) {
	$infoList = $framework->databaseInfo($dir);
	if ($infoList) {
		$writer = new Zend_Config_Writer_Ini();
		$config = new Zend_Config(array(), true);
		$writer->setConfig($config);
		foreach ($infoList as $info) {
		
			// write an INI entry for each connection. make sure to replace any possible
			// characters that may conflict with INI parsing.
			$key = iniEscapeKey($info->name);
				
			$config->{$key} = array();
			foreach ($info as $prop => $value) {
				$prop = ucfirst($prop);
				$config->{$key}->{$prop} = $value;
			}				
		}
		initPrint($writer, $outputFile);
	}
}

function runIsUsedBy($dir, $outputFile) {
	$frameworks = loadFrameworks();
	$identifiers = array();
	foreach ($frameworks as $framework) {
		if ($framework->isUsedBy($dir)) { 
			$identifiers[] = $framework->getIdentifier();
		}
	}
	$writer = new Zend_Config_Writer_Ini();
	$config = new Zend_Config(array(), true);
	$writer->setConfig($config);
	$i = 0;
	foreach ($identifiers as $identifier) {
		$prop = "framework_{$i}";
		$config->{$prop} = $identifier;
		$i++;
	}
	initPrint($writer, $outputFile);
}

function runConfigFiles($framework, $dir, $outputFile) {
	$configFiles = $framework->configFiles($dir);
	if ($configFiles) {
		$writer = new Zend_Config_Writer_Ini();
		$outputConfig = new Zend_Config(array(), true);
		$writer->setConfig($outputConfig);
		foreach ($configFiles as $configName => $configPath) {
		
			// write an INI entry for each config file. make sure to replace any possible
			// characters that may conflict with INI parsing.
			// at the moment I don't know how to make wxFileConfig not try to 
			// unescap backslashes; will escape backslashes here
			$key = iniEscapeKey($configName);
			$configPath = iniEscapeValue($configPath);
			$outputConfig->{$key} = $configPath;
		}
		initPrint($writer, $outputFile);
	}
}

/**
 * @param MvcEditorFrameworkBaseClass $framework the framework specific code
 * @param string $dir the directory where the project source code is located
 */
function runResources(MvcEditorFrameworkBaseClass $framework, $dir, $outputFile) {
	$resources = $framework->resources($dir);
	if ($resources) {
		$writer = new Zend_Config_Writer_Ini();
		$outputConfig = new Zend_Config(array(), true);
		$writer->setConfig($outputConfig);
		$keyIndex = 0;
		foreach ($resources as $resource) {

			// write an INI entry for each config file. make sure to replace any possible
			// characters that may conflict with INI parsing.
			$key = iniEscapeKey('Resource_' . $keyIndex);
			$outputConfig->{$key} = array(
				'Resource' => iniEscapeValue($resource->resource),
				'Identifier' => iniEscapeValue($resource->identifier),
				'ReturnType' => iniEscapeValue($resource->returnType),
				'Signature' => iniEscapeValue($resource->signature),
				'Comment' => iniEscapeValue($resource->comment),
				'Type' => iniEscapeValue($resource->type)
			);
			$keyIndex++;
		}
		initPrint($writer, $outputFile);
	}
}


// Replaces any possible characters that may conflict with INI parsing of KEYS.
function iniEscapeKey($str) {
	return str_replace(array("/", "\n", " "), array("_", "_", "_"), $str);
}

// Replaces any possible characters that may conflict with INI parsing of VALUES.
// at the moment I don't know how to make wxFileConfig not try to 
// unescape backslashes; will escape backslashes here
function iniEscapeValue($str) {
	return str_replace("\\", "\\\\", $str);
}

function initPrint(Zend_Config_Writer_Ini $writer, $outputFile) {
	if ($outputFile) {
		file_put_contents($outputFile, $writer->render());
	}
	else {
		print "\n-----START-MVC-EDITOR-----\n";
		echo $writer->render();
	}
}

$rules = array(
	'identifier|i=s' => 'The framework to query',
	'dir|d=s' => 'The base directory that the project resides in',
	'action|a=s' => 'The data that is being queried',
	'output|o=s' => 'If given, the output will be written to the given file (by default, output does to STDOUT)',
	'help|h' => 'A help message'
);
$opts = new Zend_Console_Getopt($rules);
$action = $opts->getOption('action');
$identifier = $opts->getOption('identifier');
$dir = $opts->getOption('dir');
$help = $opts->getOption('help');
$outputFile = $opts->getOption('output');

// trim any quotes
$action = trim($action, " '\"");
$identifier = trim($identifier, " '\"");
$dir = trim($dir, " '\"");
$outputFile = trim($outputFile, " '\"");

if ($help) {
	echo <<<EOF
This is the gateway that will be responsible for communicating with the editor app.
This is a strict command line interface; the editor will invoke this script via a command (and 
with some arguments) and this app will parse the arguments and respond accordingly.
For example, let's say the user opened a SQL browser.  The editor needs to know the SQL credentials to
use. The editor will run this script asking for the credentials.  

   php MvcEditorFrameworkApp.php --identifier="CI-1.4" --dir="/home/user/projects/my_blog/" --action=databaseInfo

When any argument is invalid or missing, the program will exit with an error code (-1)
EOF;
	exit(0);
}
if ($outputFile) {
	print "Running action=$action on identifier=$identifier with dir=$dir output to $outputFile \n";
}
else {
	print "Running action=$action on identifier=$identifier with dir=$dir output to STDOUT \n";
}

// go through all the available frameworks and pick the one with the correct identifier
$frameworks = loadFrameworks();
$chosenFramework = NULL;
foreach ($frameworks as $framework) {
	if (strcasecmp($framework->getIdentifier(), $identifier) == 0) {
		$chosenFramework = $framework;
		break;
	}
}
if ($chosenFramework && $dir) {

	// pick the correct method call on the framework
	if (method_exists($chosenFramework, $action)) {
		runAction($chosenFramework, $dir, $action, $outputFile);
	}
	else {
		print "Invalid action: '{$action}.' Program will now exit.\n";
		exit(-1);
	}
}
else if ($dir && strcasecmp($action, 'isUsedBy') == 0) {
	runAction(NULL, $dir, $action, $outputFile);
}
else if ($dir) {
	print "Invalid identifier: '{$identifier}.' Program will now exit.\n";
	exit(-1);
}
else {
	print "dir argument must be given.  Program will now exit.\n";
	exit(-1);
}