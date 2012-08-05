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

require_once __DIR__ . '/../src/MvcEditorFileItem.php';

class MvcEditorFileItemTest extends PHPUnit_Framework_TestCase {

	/**
	 * @var MvcEditorFileItem
	 */
	private $fileItem;
	
	/**
	 * connection to use
	 */
	private $dbAdapter;
	
	function setUp() {
		$this->dbAdapter = Zend_Db::factory('Pdo_Sqlite', array("dbname" => ":memory:"));
		Zend_Db_Table_Abstract::setDefaultAdapter($this->dbAdapter);
		$this->initOutputPdo($this->dbAdapter);
	}
	
	function testMatchingFiles() {
		$fileItem = new MvcEditorFileItemClass();
		$fileItemId1 = $fileItem->insert(array('full_path' => 'c:\users\john\file.php'));
		$fileItemId2 = $fileItem->insert(array('full_path' => 'c:\users\john\lib\class.file.php'));
		
		$fileItems = $fileItem->MatchingFiles('c:\users\john');
		$this->assertEquals(2, count($fileItems));
		$this->assertEquals('c:\users\john\file.php', $fileItems[0]['full_path']);
		$this->assertEquals('c:\users\john\lib\class.file.php', $fileItems[1]['full_path']);
		
		$fileItems = $fileItem->MatchingFiles('c:\users\john\li');
		$this->assertEquals(1, count($fileItems));
		$this->assertEquals('c:\users\john\lib\class.file.php', $fileItems[0]['full_path']);
	}
	
	private function initOutputPdo(Zend_Db_Adapter_Abstract $dbAdapter) {
		$dbAdapter->query(
			'CREATE TABLE IF NOT EXISTS file_items ( ' .
			'  file_item_id INTEGER PRIMARY KEY, full_path TEXT, last_modified DATETIME, is_parsed INTEGER, is_new INTEGER ' .
			')'
		);
		$dbAdapter->query(
			'CREATE TABLE IF NOT EXISTS resources ( ' .
			'  file_item_id INTEGER, key TEXT, identifier TEXT, class_name TEXT, ' .
			'  type INTEGER, namespace_name TEXT, signature TEXT, comment TEXT, ' .
			'  return_type TEXT, is_protected INTEGER, is_private INTEGER, ' .
			'  is_static INTEGER, is_dynamic INTEGER, is_native INTEGER ' .
			')'
		);
	}
		
}