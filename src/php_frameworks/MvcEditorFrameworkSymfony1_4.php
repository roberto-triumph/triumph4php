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

class MvcEditorFrameworkSymfony1_4 extends MvcEditorFrameworkBaseClass {

	public function getName() {
		return 'Symfony v.1.4';
	}
	
	public function getIdentifier() { 
		return 'symfony-1-4'; 
	}
	
	public function isUsedBy($dir) {
		return is_dir($dir . '/lib/vendor/symfony');
	}
	
	public function databaseInfo($dir) {
	
		// use the symfony YAML parser to parse out the databases.yml
		require_once($dir .'/lib/vendor/symfony/lib/yaml/sfYaml.php');
		$dbs = sfYaml::load($dir . '/config/databases.yml');
		$infos = array();
		foreach ($dbs as $environment => $ormConfig) {
			
			// environment is 'all', 'dev', 'prod' ...
			foreach ($ormConfig as $orm => $ormConfig) {
				
				// $orm is 'doctrine', or 'propel' $ormConfig is the settings for that ORM
				$dsn = $ormConfig['param']['dsn'];
				$user = $ormConfig['param']['username'];
				$password = $ormConfig['param']['password'];
				
				// parse out the DSN
				$pair = explode(':', $dsn);
				$driver = $pair[0];
				$dbParams = $pair[1];
				$databaseName = '';
				if (strcasecmp($driver, 'mysql') == 0) {
					$exploded = explode(';', $dbParams);
					foreach ($exploded  as $itemPair) {
						$explodedPair = explode('=', $itemPair);
						if (strcasecmp('host', $explodedPair[0]) == 0) {
							$host = $explodedPair[1];
							$port = 0;
							if (strpos($host, ':') !== FALSE) {
								
								// parse out the port
								$pair = explode(':', $host);
								$host = $pair[0];
								$port = $pair[1];
							}
						}
						else if (strcasecmp('dbname', $explodedPair[0]) == 0) {
							$databaseName = $explodedPair[1];
						}
					}
					$name = $environment . ' ' . $orm;
					$infos[] = new MvcEditorDatabaseInfo(MvcEditorDatabaseInfo::DRIVER_MYSQL, 
						$environment, $name, $host, $port, $databaseName, '', $user, $password);
				}				
			}
		}
		return $infos;
	}
}