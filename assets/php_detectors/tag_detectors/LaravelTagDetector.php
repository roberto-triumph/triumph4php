<?php

/**
 * The goal of this script is to discover all of the 'dynamic' tags for your PHP projects. This means
 * that it will list all of the tags that correspond any properties or methods added to classes at runtime. This script
 * will be called via a command line; it is a normal command line script.
 *
 * This script is part of Triumph's Tag Detection feature; it enhances Triumph's code
 * completion and call tips because a tag detector is coded to each PHP framework's
 * dynamic "magic" and Triumph is made aware of new methods/properties that are added
 * at run-time by a framework. More info can be about Triumph's Tag detector feature can be 
 * found at http://docs.triumph4php.com/tag-detectors/
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
		'resourceDbFileName|i=s' => 'Required. SQLite file that contains the project\'s files, classes, and methods. This file is created by ' .
			'Triumph; Triumph performs INSERTs as it indexes a project.',
		'outputDbFileName|o-s' => 'Optional. If given, the output will be written to the given file. If not given, output goes to STDOUT.',
		'help|h' => 'This help message'
	);
	$opts = new Zend_Console_Getopt($rules);

	// trim any quotes
	$sourceDir = trim($opts->getOption('sourceDir'), " \r\n\t'\"");
	$resourceDbFileName = trim($opts->getOption('resourceDbFileName'), " \r\n\t'\"");
	$outputDbFileName = trim($opts->getOption('outputDbFileName'), " \r\n\t'\"");
	$help = trim($opts->getOption('help'), " \r\n\t'\"");

	if ($help) {
		$helpMessage = <<<EOF
The goal of this script is to discover all of the 'dynamic' tags for your PHP projects. This means
that it will list all of the tags that correspond any properties or methods added to classes at runtime. This script
will be called via a command line; it is a normal command line script.
 
This script is part of Triumph's Tag Detection feature; it enhances Triumph's code
completion and call tips because a tag detector is coded to each PHP framework's
dynamic "magic" and Triumph is made aware of new methods/properties that are added
at run-time by a framework. More info can be about Triumph's Tag detector feature can be 
found at http://docs.triumph4php.com/tag-detectors/

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
	
	if (!extension_loaded('pdo_sqlite') || !extension_loaded('PDO')) {
		echo "The script " . basename(__FILE__) . " requires the PDO and pdo_sqlite PHP extensions.";
		exit(-1);
	}

	// call the function that will return all detected URLs
	$doSkip = TRUE;
	$arrTags = detectTags($sourceDir, $resourceDbFileName, $doSkip);
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
				echo str_pad($tag->signature, 25);
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
 * @param  string $resourceDbFileName   the location of the resource cache SQLite file; as created by Triumph
 * @param  boolean $doSkip              out parameter; if TRUE then this detector does not know how
 *                                      to detect URLs for the given source directory; this situation
 *                                      is different than zero URLs being detected.
 * @return Triumph_Resource[]         array of Triumph_Resource instances the detected tags
 */
function detectTags($sourceDir, $resourceDbFileName, &$doSkip) {
	$allTags = array();
	if (!is_file($resourceDbFileName)) {
		return $allTags;
	}
	
	// need to check that this detector is able to recognize the directory structure of sourceDir
	// if not, then we need to skip detection by returning immediately and setting $doSkip to TRUE.
	// by skipping detection, we prevent any detected tags from the previous detection script
	// from being deleted.
	$doSkip = TRUE;
	
	// for laravel, we look for the artisan script. if we don't have the artisan script assume
	// that this source is not a laravel project.
	$sourceDir = \opstring\ensure_ends_with($sourceDir, DIRECTORY_SEPARATOR);
	if (!is_file($sourceDir . 'artisan')) {	
		return $allTags;
	}
	$doSkip = FALSE;
	
	// get all of the configured aliases, laravel automatically imports
	// certain classes into the root namespace.
	
	// load the laravel bootstrap
	$bootstrapFile = $sourceDir . 'bootstrap' . DIRECTORY_SEPARATOR . 'autoload.php';
	$startFile = $sourceDir . 'bootstrap' . DIRECTORY_SEPARATOR . 'start.php'; 
	if (is_file($bootstrapFile) && is_file($startFile)) {
	
		require $bootstrapFile;
		$app = require_once $startFile;	
		
		// laravel defines a set of aliases that get added in to the 
		// root namespace at run-time. we loop through the aliases
		// and add a method with the class name being the alias name
		// and the method name being the method from the class  being
		// aliased
		$pdo = Zend_Db::factory('Pdo_Sqlite', array("dbname" => $resourceDbFileName));
		$resourceTable = new Triumph_ResourceTable($pdo);
		
		// I want to read the aliases from the config, but unfortunately
		// we need more info.  laravel facades work in this manner:
		// facade => ioc key => class name
		// the config gives us the facade names, but the ioc keys are hidden in the
		// facade.
		// for now, we extract the default ioc keys from the Application		
		// the default aliases, IOC keys, and classes are in 
		// laravel/framework/src/Illuminate/Foundation/Application.php
		// ideally, we should be able to find "bind" calls in the service
		// providers
		$arrDefaultAliases = defaultAliases();
		$arrIocKeyToFacade = defaultBindings();
		
		foreach ($arrDefaultAliases as $strIocKey => $strFullClassName) {			
			
			// the full names in the laravel config file do not start
			// with the root namespace, but we want Triumph to know
			// that these are fully qualified class names
			$strFullClassName = \opstring\ensure_begins_with($strFullClassName, '\\');
			$strParent = get_parent_class($strFullClassName);
			$strAlias = $arrIocKeyToFacade[$strIocKey];
			
			// if the alias is a facade, get the methods of the facade class that is
			// being adapted
			// the facade is created by asking the app for a 
			
			$arrResources = $resourceTable->FindPublicMethodsMembersForClass($strFullClassName, $sourceDir);
			foreach ($arrResources as $resource) {
				if (Triumph_Resource::TYPE_METHOD == $resource->type) {
					$detectedTag = Triumph_DetectedTag::CreateMethod(
						$strAlias,
						$resource->identifier,
						$resource->returnType,
						'\\',
						$resource->comment
					);
					
					// set the member as being static
					// laravel implements facades using the __callStatic
					// magic method
					// since triumph is smart enough to only show static methods
					// when a static call is being made, we must set the static flag
					// to true, else code completion will not work
					$detectedTag->isStatic = TRUE;
					$detectedTag->signature = $resource->signature;
					$allTags []= $detectedTag;
				}
				else if (Triumph_Resource::TYPE_MEMBER == $resource->type) {
					$detectedTag = Triumph_DetectedTag::CreateMember(
						$strAlias,
						$resource->identifier,
						$resource->returnType,
						'\\',
						$resource->comment
					);
					$detectedTag->signature = $resource->signature;
					$detectedTag->isStatic = TRUE;
					$allTags []= $detectedTag;
				}
			}
		}
		
	}
	return $allTags;
}

/**
 * @return array key-value pairs, where key is the IOC key for a
 *         default laravel facades.  The key is the IOC key (the key used 
 *         by the facade ie. getFacadeAccessor) and the value is the full
 *         name of the "complex" class that is being abstracted
 *
 */
function defaultAliases() {
	return array(
		'app'            => 'Illuminate\Foundation\Application',
		'artisan'        => 'Illuminate\Console\Application',
		'auth'           => 'Illuminate\Auth\AuthManager',
		//'auth.reminder.repository' => 'Illuminate\Auth\Reminders\ReminderRepositoryInterface',
		'blade.compiler' => 'Illuminate\View\Compilers\BladeCompiler',
		'cache'          => 'Illuminate\Cache\CacheManager',
		//'cache.store'    => 'Illuminate\Cache\Repository',
		'config'         => 'Illuminate\Config\Repository',
		'cookie'         => 'Illuminate\Cookie\CookieJar',
		'encrypter'      => 'Illuminate\Encryption\Encrypter',
		'db'             => 'Illuminate\Database\DatabaseManager',
		'events'         => 'Illuminate\Events\Dispatcher',
		'files'          => 'Illuminate\Filesystem\Filesystem',
		'form'           => 'Illuminate\Html\FormBuilder',
		'hash'           => 'Illuminate\Hashing\HasherInterface',
		'html'           => 'Illuminate\Html\HtmlBuilder',
		'translator'     => 'Illuminate\Translation\Translator',
		'log'            => 'Illuminate\Log\Writer',
		'mailer'         => 'Illuminate\Mail\Mailer',
		'paginator'      => 'Illuminate\Pagination\Environment',
		'auth.reminder'  => 'Illuminate\Auth\Reminders\PasswordBroker',
		'queue'          => 'Illuminate\Queue\QueueManager',
		'redirect'       => 'Illuminate\Routing\Redirector',
		'redis'          => 'Illuminate\Redis\Database',
		'request'        => 'Illuminate\Http\Request',
		'router'         => 'Illuminate\Routing\Router',
		'session'        => 'Illuminate\Session\SessionManager',
		//'session.store'  => 'Illuminate\Session\Store',
		'remote'         => 'Illuminate\Remote\RemoteManager',
		'url'            => 'Illuminate\Routing\UrlGenerator',
		'validator'      => 'Illuminate\Validation\Factory',
		'view'           => 'Illuminate\View\Environment',	
	);
}

/**
 * @return array key-value pairs, where key is the IOC key for a
 *         default laravel facades.  The key is the IOC key (the key used 
 *         by the facade ie. getFacadeAccessor) and the value is the facade 
 *         class that the developer uses. 
 */
function defaultBindings() {
	return array(
		'app' => 'App',
		'artisan' => 'Artisan',
		'auth' => 'Auth',
		'blade.compiler' => 'Blade',
		'cache' => 'Cache',
		'config' => 'Config',
		'cookie' => 'Cookie',
		'encrypter' => 'Crypt',
		'db' => 'DB',
		'events' => 'Event',
		'files' => 'File',
		'form' => 'Form',
		'hash' => 'Hash',
		'html' => 'HTML',
		'request' => 'Input',
		'translator' => 'Lang',
		'log' => 'Log',
		'mailer' => 'Mail',
		'paginator' => 'Paginator',
		'auth.reminder' => 'Password',
		'queue' => 'Queue',
		'redirect' => 'Redirect',
		'redis' => 'Redis',
		'request' => 'Request',
		'router' => 'Route',
		'session' => 'Session',
		'remote' => 'SSH',
		'url' => 'URL',
		'validator' => 'Validator',
		'view' => 'View',
	);
}

// start running this script
parseArgs();