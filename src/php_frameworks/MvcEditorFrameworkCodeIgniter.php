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

class MvcEditorFrameworkCodeIgniter extends MvcEditorFrameworkBaseClass {

	public function getName() {
		return 'Code Igniter v.2.0.3';
	}
	
	public function getIdentifier() { 
		return 'code-igniter'; 
	}
	
	public function isUsedBy($dir) {
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
		$resources[] = MvcEditorResource::MakeMethod('CI_Controller', 'get_instance', '', 'CI_Controller', $comment);
		
		return $resources;
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
		
		// the core libraries are automatically added to the CI super object
		foreach (glob($codeIgniterSystemDir . DIRECTORY_SEPARATOR . 'core' . DIRECTORY_SEPARATOR . '*.php') as $libFile) {
			$baseName = basename($libFile, '.php');
			if ('CodeIgniter' != $baseName && 'Common' != $baseName && 'Controller' != $baseName
					&& 'Exceptions' != $baseName && 'Exceptions' != $baseName && 'Model' != $baseName) {
				$propertyType = 'CI_' . $baseName;
				$propertyName = strtolower($baseName);
				$comment = '';
				$resources[] = MvcEditorResource::MakeProperty('CI_Controller', $propertyName, $propertyType, $comment);
				$resources[] = MvcEditorResource::MakeProperty('CI_Model', $propertyName, $propertyType, $comment);
			}
		}
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
			$resources[] = MvcEditorResource::MakeProperty('CI_Controller', $propertyName, $propertyType, $comment);
			$resources[] = MvcEditorResource::MakeProperty('CI_Model', $propertyName, $propertyType, $comment);
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
			$resources[] = MvcEditorResource::MakeProperty('CI_Controller', $propertyName, $propertyType, $comment);
			$resources[] = MvcEditorResource::MakeProperty('CI_Model', $propertyName, $propertyType, $comment);
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
			$resources[] = MvcEditorResource::MakeProperty('CI_Controller', $propertyName, $propertyType, $comment);
			$resources[] = MvcEditorResource::MakeProperty('CI_Model', $propertyName, $propertyType, $comment);
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
		if (!is_file($dir . DIRECTORY_SEPARATOR . 'index.php')) {
			return '';
		}
		$tokens = token_get_all(file_get_contents($dir . DIRECTORY_SEPARATOR . 'index.php'));
		
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
		
		// remove the quotes
		$systemDir = substr($systemDir, 1, -1);
		
		// systemDir may be an absolute path
		if (is_file($dir . '/' . $systemDir . '/core/CodeIgniter.php')) {
			return realpath($dir . '/' . $systemDir); 
		}
		else if (is_file($systemDir . '/core/CodeIgniter.php')) {
			return realpath($systemDir);
		}
		return '';
	}
}