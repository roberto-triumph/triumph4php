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

require_once 'MvcEditorFrameworkBase.php';
require_once 'MvcEditorResource.php';
require_once 'MvcEditorDatabaseInfo.php';
require_once 'MvcEditorCallClass.php';
require_once 'MvcEditorUrlClass.php';
require_once 'MvcEditorViewInfo.php';
require_once 'MvcEditorFileItem.php';
require_once __DIR__ . '/../lib/opportunity/array.php';
require_once __DIR__ . '/../lib/opportunity/string.php';

class MvcEditorFrameworkCodeIgniter extends MvcEditorFrameworkBaseClass {

	public function getName() {
		return 'Code Igniter v.2.0.3';
	}
	
	public function getIdentifier() { 
		return 'code-igniter'; 
	}
	
	public function isUsedBy($dir) {
		$dir = \opstring\ensure_ends_with($dir, '/');
		$systemDir = $this->locateSystemDir($dir);
		return strlen($systemDir) > 0;
	}

	/**
	 * @return array of MvcEditorDatabaseInfo class objects.
	 */
	public function databaseInfo($dir) {
		$list = array();
		$db = array();
		
		// need this define so that we can include code igniter files directly
		define('BASEPATH', '');
		
		// database config file can be in the environment directory
		// for now just get the development environment info
		if (is_file($dir . '/application/config/development/database.php')) {
			include ($dir . 'application/config/development/database.php');
			if ($db) {
				foreach ($db as $groupName => $groupConnection) {
					if (strcasecmp('mysql', $groupConnection['dbdriver']) == 0) {
						$info = $this->infoFromDbArray('development', $groupName, $groupConnection);
						$list[] = $info;
					}
				}
			}
		}
		else if (is_file($dir . '/application/config/database.php')) {
			$db = array();
			include ($dir . '/application/config/database.php');
			if ($db) {
				foreach ($db as $groupName => $groupConnection) {
					if (strcasecmp('mysql', $groupConnection['dbdriver']) == 0) {
						$info = $this->infoFromDbArray('development', $groupName, $groupConnection);
						$list[] = $info;
					}
				}
			}
		}		
		return $list;
	}
	
	/**
	 * @return assoc array key = Name of config Value =  full path to config file
	 */
	public function configFiles($dir) {
		if (is_file($dir . '/application/config/database.php')) {
			return array( 
				'AutoLoad' => realpath($dir . '/application/config/autoload.php'),
				'Config' => realpath($dir . '/application/config/config.php'),
				'Constants' => realpath($dir . '/application/config/constants.php'),
				'Database' => realpath($dir . '/application/config/database.php'),
				'DocTypes' => realpath($dir . '/application/config/doctypes.php'),
				'Foreign Characters' => realpath($dir . '/application/config/foreign_chars.php'),
				'Hooks' => realpath($dir . '/application/config/hooks.php'),
				'Mime Types' => realpath($dir . '/application/config/mimes.php'),
				'Profiler' => realpath($dir . '/application/config/profiler.php'),
				'Routes' => realpath($dir . '/application/config/routes.php'),
				'Smileys' => realpath($dir . '/application/config/smileys.php'),
				'User Agents' => realpath($dir . '/application/config/user_agents.php')
			);
		}
		return array();
	}
	
	/**
	 * @return array of MvcEditorResource objects.
 	 */
	public function resources($dir) {
		if (!is_file($dir . DIRECTORY_SEPARATOR . 'index.php')) {
			return array();
		}
		
		// need this define so that we can include code igniter files directly
		define('BASEPATH', '');


		$resources = array();
		$codeIgniterSystemDir = $this->locateSystemDir($dir);

		$this->coreResources($dir, $codeIgniterSystemDir, $resources);
		$this->libraryResources($dir, $codeIgniterSystemDir, $resources);
		
		// TODO: handle more than one application
		$appDir =  $dir . DIRECTORY_SEPARATOR . 'application';
		$modelDir = $appDir . DIRECTORY_SEPARATOR . 'models';
		$this->modelResources($dir, $modelDir, $resources);

		// the "super" object
		$comment = '';
		$resources[] = MvcEditorResource::CreateMethod('CI_Controller', 'get_instance', '', 'CI_Controller', $comment);
		
		return $resources;
	}
	
	/**
	 * @return the array of URLs for the given class name / file name.
	 */
	public function makeUrls($dir, $resourceCacheFileName, $host) {
		if (!is_file($resourceCacheFileName)) {
			return array();
		}
		$dir = \opstring\ensure_ends_with($dir, '/');
		$host = \opstring\ensure_ends_with($host, '/');
		
		// TODO: handle multiple apps
		// need this define so that we can include code igniter files directly
		if (!defined('BASEPATH')) {
			define('BASEPATH', '');
		}
		$config = array();
		$route = array();
		include($dir . 'application/config/routes.php');
		include($dir . 'application/config/config.php');
		
		$allUrls = array();	
		
		$pdo = Zend_Db::factory('Pdo_Sqlite', array("dbname" => $resourceCacheFileName));
		Zend_Db_Table_Abstract::setDefaultAdapter($pdo);
		$fileItem = new MvcEditorFileItemClass();
		
		// get all controller files, only controllers are accessible via URLs
		$controllerDir = $dir . 'application/controllers';
		$matchingFiles = $fileItem->MatchingFiles($controllerDir);
		if (empty($matchingFiles)) {
			$controllerDir = $dir . 'application\controllers';
			$matchingFiles = $fileItem->MatchingFiles(realpath($controllerDir));
		}

		
		$resource = new MvcEditorResource();
		$methods = $resource->MethodsFromFiles($pdo, $matchingFiles);
		foreach ($methods as $resource) {
			
			// normalize any paths to have forward slash
			// doing this so that this logic works on MSW, linux, AND the vfs:// stream (tests)
			$controllerDir = \opstring\replace($controllerDir, DIRECTORY_SEPARATOR, '/');
			$resourceFullPath = \opstring\replace($resource->fullPath, DIRECTORY_SEPARATOR, '/');
			
			
			$controllerFile = \opstring\after($resourceFullPath, $controllerDir);
			$subDirectory = dirname($controllerFile);
			if ('\\' == $subDirectory) {
			
				// hack to work around special case when there is no subdirectory
				$subDirectory = '';
			}
			
			// TODO: any controller arguments ... should get these from the user
			// constructors are not web-accessible
			// code igniter makes methods that start with underscore
			if (\opstring\compare_case('__construct', $resource->identifier) && !\opstring\begins_with($resource->identifier, '_')) {
				$extra = '';
				$appUrl = $this->makeUrl($route, $config, $subDirectory, $resource->fullPath, $resource->className, $resource->identifier, $extra);
				$appUrl->url = $host . $appUrl->url;
				$allUrls[] = $appUrl;
			}
		}
		return $allUrls;
	}
	
	/**
	 * @return array
	 */
	public function viewInfos($dir, $url, $callStackFile) {
		
		// CodeIgniter controllers will call $this->load->view('file', $data) 
		// we will look for calls to CI_Loader::view that have at least 2 arguments
		// we wont need to look at the URL to determine the view
		$viewInfos = array();
		$fp = fopen($callStackFile, 'rb');
		if (!$fp) {
			return $viewInfos;
		}
		$call = new MvcEditorCallClass();
		$inViewMethod = FALSE;
		$paramIndex = 0;
		$currentView = null;
		while (!feof($fp)) {
			if ($call->fromFile($fp)) {
				if (MvcEditorCallClass::BEGIN_METHOD == $call->type && strcasecmp('CI_Loader::view', $call->resource) == 0) {
					$inViewMethod = TRUE;
					$paramIndex = 0;
				}
				else if (MvcEditorCallClass::PARAM == $call->type && $inViewMethod) {
					if (0 == $paramIndex) {
						$currentView = new MvcEditorViewInfoClass();
						$currentView->fileName = $call->scalar;
						
						// most of the time views are given as relative relatives; starting from the application/views/ directory
						// for now ignore variable arguments
						if ($currentView->fileName[0] != '$' && !empty($currentView->fileName)) {
							
							// view file may have an extesion; if it has an extension use that; otherwise use the default (.php)
							if (stripos($currentView->fileName, '.') === FALSE) {
								$currentView->fileName .= '.php';
							}
							
							// not using realpath() so that MVCEditor can know that a template file has not yet been created
							// or the programmer has a bug in the project.
							$currentView->fileName = \opstring\replace($currentView->fileName, '/', DIRECTORY_SEPARATOR);
							$currentView->fileName = \opstring\replace($currentView->fileName, '\\', DIRECTORY_SEPARATOR);
							$currentView->fileName = $dir . DIRECTORY_SEPARATOR . 'application' . DIRECTORY_SEPARATOR . 'views' . DIRECTORY_SEPARATOR . $currentView->fileName;
							
							// push it now just in case the template does not get any variables
							$viewInfos[] = $currentView;
						}
					}
					else if (1 == $paramIndex) {
						$currentView->variables = array();
						if (MvcEditorCallClass::T_ARRAY == $call->paramType) {
							foreach ($call->arrayKeys as $key) {
							
								// add the siguil here; editor expects us to return variables
								$currentView->variables[] = '$' . $key;
							}
						}
						$viewInfos[count($viewInfos) - 1] = $currentView;
					}
					$paramIndex++;
				}
				else {
					$inViewMethod = FALSE;
					$paramIndex = 0;
					$currentView = NULL;
				}
			}
		}
		return $viewInfos;
	}
	
	private function infoFromDbArray($environment, $groupName, $groupConnection) {
		// port is not there by default
		$port = 0;
		if (isset($groupConnection['port'])) {
			$port = $groupConnection['port'];
		}
	
		$info = new MvcEditorDatabaseInfo(MvcEditorDatabaseInfo::DRIVER_MYSQL, 
			$environment, $groupName, $groupConnection['hostname'], 
			$port, $groupConnection['database'], 
			'', 
			$groupConnection['username'], 
			$groupConnection['password']);
		return $info;
	}
	
	private function coreResources($dir, $codeIgniterSystemDir, &$resources) {
		$comment = '';
		
		// the core libraries are automatically added to the CI super object
		foreach (glob($codeIgniterSystemDir . DIRECTORY_SEPARATOR . 'core' . DIRECTORY_SEPARATOR . '*.php') as $libFile) {
			$baseName = basename($libFile, '.php');
			if ('CodeIgniter' != $baseName && 'Common' != $baseName && 'Controller' != $baseName
					&& 'Exceptions' != $baseName && 'Exceptions' != $baseName && 'Model' != $baseName) {
				$propertyType = 'CI_' . $baseName;
				$propertyName = strtolower($baseName);
				
				$resources[] = MvcEditorResource::CreateMember('CI_Controller', $propertyName, $propertyType, $comment);
				$resources[] = MvcEditorResource::CreateMember('CI_Model', $propertyName, $propertyType, $comment);
			}
		}
		
		// the DB library, this requires checking to see if active record is enabled
		if (is_file($dir . '/application/config/database.php')) {
			$db = array();
			include ($dir . '/application/config/database.php');
			$propertyType = isset($active_record) && $active_record ? 'CI_DB_active_record' : 'CI_DB_driver';
			$resources[] = MvcEditorResource::CreateMember('CI_Controller', 'db', $propertyType, $comment);
			$resources[] = MvcEditorResource::CreateMember('CI_Model', 'db', $propertyType, $comment);
		}
		
		// alias the Loader library; seems that there is two properties
		$resources[] = MvcEditorResource::CreateMember('CI_Controller', 'load', 'CI_Loader', $comment);
		$resources[] = MvcEditorResource::CreateMember('CI_Model', 'load', 'CI_Loader', $comment);
		
	}
	
	private function libraryResources($dir, $codeIgniterSystemDir, &$resources) {
		
		// TODO: handle more than one application
		$appDir =  $dir . DIRECTORY_SEPARATOR . 'application';
		
		// the system libraries are automatically added to the CI super object,
		// the class name is named the same as the file name but with the CI_ prefix
		foreach(glob($codeIgniterSystemDir . DIRECTORY_SEPARATOR . 'libraries' . DIRECTORY_SEPARATOR . '*.php', 0) as $libFile) {
			$key = basename($libFile, '.php');
			$propertyType = 'CI_' . $key;
			$propertyName = strtolower($key);
			$comment = '';
			$resources[] = MvcEditorResource::CreateMember('CI_Controller', $propertyName, $propertyType, $comment);
			$resources[] = MvcEditorResource::CreateMember('CI_Model', $propertyName, $propertyType, $comment);
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
			$comment = '';
			$resources[] = MvcEditorResource::CreateMember('CI_Controller', $propertyName, $propertyType, $comment);
			$resources[] = MvcEditorResource::CreateMember('CI_Model', $propertyName, $propertyType, $comment);
		}
	}
	
	private function modelResources($dir, $modelDir, &$resources) {
		
		// the models are added to the Controller super object by the loader
		
		foreach (glob($modelDir . DIRECTORY_SEPARATOR . '*.php') as $modelFile) {
			$key = basename($modelFile, '.php');
			$propertyType = ucfirst($key);
			
			// the property name will NOT have the prefix
			$propertyName = strtolower($key);
			$comment = '';
			$resources[] = MvcEditorResource::CreateMember('CI_Controller', $propertyName, $propertyType, $comment);
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
						$this->modelResources($dir, $modelSubDir, $resources);
					}
				}
				$item = readdir($dirId);
			}
		}
		closedir($dirId);
	}
	
	/**
	 * @return string the directory (full path) where CodeIgniter files (the 'system' directory) is
	 * located.  This could be empty if the code igniter system directory could not be found. The returned
	 * directory has OS-specific separators (it uses the realpath() function)
	 */
	private function locateSystemDir($dir) {
	
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
		$systemDir = substr($systemDir, 1, -1);
		$systemDir = \opstring\ensure_ends_with($systemDir, '/');
		
		// systemDir may be an absolute path
		// dont use realpath() because we are using vfsStream for testing
		if (is_file($dir . $systemDir . 'core/CodeIgniter.php')) {
			$systemDir = str_replace('/', DIRECTORY_SEPARATOR, $dir . $systemDir); 
			$systemDir = str_replace('\\', DIRECTORY_SEPARATOR, $dir . $systemDir); 
			return $systemDir;
		}
		else if (is_file($systemDir . 'core/CodeIgniter.php')) {
			$systemDir = str_replace('/', DIRECTORY_SEPARATOR, $systemDir ); 
			$systemDir = str_replace('\\', DIRECTORY_SEPARATOR, $systemDir); 
			return $systemDir;
		}
		return '';
	}
	
	function makeUrl($route, $config, $subDirectory, $fileName, $className, $methodName, $extra) {
	
		// this function was taken from http://codeigniter.com/forums/viewthread/102438
		// with the following changes:
		// 1. including routes directly instead of using CI object
		//    as I was having problems including the bootstap file here
		$uri = '';
		if ($subDirectory) {
			$uri .= $subDirectory . '/';
		}
		$uri .= $className . '/' . $methodName;
		if ($extra) {
			$uri .= '/' . $extra;
		}
		
		// in case subdir has leading slash; routes usually don't
		$uri = ltrim($uri, '/');
		$uri = strtolower($uri);
		
		if (in_array($uri, $route)) {
			$uri = array_search($uri, $route);
		}    
		else {
			foreach ($route as $singleRoute => $replace) {
				$singleRoute   = preg_split('/(\(.+?\))/', $singleRoute, -1, PREG_SPLIT_DELIM_CAPTURE);
				$replace = preg_split('/(\$\d+)/', $replace, -1, PREG_SPLIT_DELIM_CAPTURE);
				if (count($singleRoute) != count($replace)) {
					continue;
				}
				$newroute = $newreplace = '';
				for ($i = 0; $i < count($singleRoute); $i++) {
					if ($i % 2) {
						$newroute .= $replace[$i];
						$newreplace .= $singleRoute[$i];
					}
					else {
						$newroute .= $singleRoute[$i];
						$newreplace .= $replace[$i];
					}
					$newreplace = str_replace(':any', '.+', str_replace(':num', '[0-9]+', $newreplace));
					if (preg_match("#^$newreplace\$#", $uri)) {
						$uri = preg_replace("#^$newreplace\$#", $newroute, $uri);
						break;
					}
				}
			}
		}
		
		if (0 == \opstring\compare('default_controller', $uri)) {
			$uri = '';
		}
		
		// respect the url suffix and index page from the config. for urls, make sure there is only one ending '/'
		// url already has leading slash
		// make sure url never has leading slash
		$indexPage = $config['index_page'];
		$indexPage = \opstring\ensure_ends_with($indexPage, '/');
		if (!empty($uri)) {
			$url = $indexPage . trim($uri . '/');
			$url = ltrim($url, '/');
		}
		else {
			$url = '';
		}
		if (isset($config['url_suffix']) && $config['url_suffix']) {
			$url .= $config['url_suffix'];
		}
		$mvcUrl = new MvcEditorUrlClass($url, $fileName, $className, $methodName);
		return $mvcUrl;
	}
}