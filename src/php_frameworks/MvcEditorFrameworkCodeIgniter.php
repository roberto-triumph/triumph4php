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
require_once 'MvcEditorDatabaseInfo.php';

class MvcEditorFrameworkCodeIgniter extends MvcEditorFrameworkBaseClass {

	public function getName() {
		return 'Code Igniter v.2.0.3';
	}
	
	public function getIdentifier() { 
		return 'code-igniter'; 
	}
	
	public function isUsedBy($dir) {
		$ret = false;
		
		// this will onlt work if the system directory is in the same directory as the app
		// the system directory can be anywhere ...
		
		if (is_file($dir . DIRECTORY_SEPARATOR . 'index.php')) {
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
			$ret = is_file($dir . '/' . $systemDir . '/core/CodeIgniter.php')
				|| is_file($systemDir . '/core/CodeIgniter.php');
		}
		return $ret;
	}

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
}