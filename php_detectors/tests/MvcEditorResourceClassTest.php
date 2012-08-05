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
require_once __DIR__ . '/../src/MvcEditorResource.php';
require_once __DIR__ . '/../src/MvcEditorFileItem.php';

class ResourceTest extends PHPUnit_Framework_TestCase {

	/**
	 * @var MvcEditorResource
	 */
	private $resource;
	
	/**
	 * connection to use
	 * @var Zend_Db_Adapter_Abstract
	 */
	private $db;
	
	function setUp() {
		$this->db = Zend_Db::factory('Pdo_Sqlite', array("dbname" => ":memory:"));
		Zend_Db_Table_Abstract::setDefaultAdapter($this->db);
		$this->initOutputPdo($this->db);
		
		$this->resource = new MvcEditorResource();
	}

	function testMakeFunction() {
		$this->resource->MakeFunction('testFunction', '$expeced, $actual');
		$this->assertEquals('testFunction', $this->resource->resource);
		$this->assertEquals('testFunction', $this->resource->identifier);
	}
	
	function testMethodsFromFiles() {
		$fileItem = new MvcEditorFileItemClass();
		$fileItemId1 = $fileItem->insert(array('full_path' => 'c:\users\john\file.php'));
		$fileItemId2 = $fileItem->insert(array('full_path' => 'c:\users\john\class.file.php'));
		
		$this->db->insert('resources', array(
			'key' => 'testFunction',
			'file_item_id' => $fileItemId1,
			'identifier' => 'testFunction',
			'type' => MvcEditorResource::TYPE_FUNCTION			
		));
		$this->db->insert('resources', array(
			'key' => 'MyClass',
			'file_item_id' => $fileItemId2,
			'identifier' => 'MyClass',
			'class_name' => 'MyClass',
			'type' => MvcEditorResource::TYPE_CLASS
		));
		
		// for properties, insert 2 rows, this is how MVC editor does it
		// see ResourceFinderClas for more info
		$this->db->insert('resources', array(
			'key' => 'MyClass::myProperty',
			'file_item_id' => $fileItemId2,
			'identifier' => 'myProperty',
			'class_name' => 'MyClass',
			'type' => MvcEditorResource::TYPE_MEMBER
		));
		$this->db->insert('resources', array(
			'key' => 'myProperty',
			'file_item_id' => $fileItemId2,
			'identifier' => 'myProperty',
			'class_name' => 'MyClass',
			'type' => MvcEditorResource::TYPE_MEMBER
		));
		
		// for methods, insert 2 rows, this is how MVC editor does it
		// see ResourceFinderClas for more info
		$this->db->insert('resources', array(
			'key' => 'MyClass::myMethod',
			'file_item_id' => $fileItemId2,
			'identifier' => 'myMethod',
			'class_name' => 'MyClass',
			'type' => MvcEditorResource::TYPE_METHOD
		));
		$this->db->insert('resources', array(
			'key' => 'myMethod',
			'file_item_id' => $fileItemId2,
			'identifier' => 'myMethod',
			'class_name' => 'MyClass',
			'type' => MvcEditorResource::TYPE_METHOD
		));
		
		$fileItemIds = array($fileItemId1, $fileItemId2);
		$methods = $this->resource->MethodsFromFiles($this->db, $fileItemIds);
		
		$this->assertEquals(1, count($methods));
		$method = $methods[0];
		$this->assertEquals('myMethod', $method->identifier);
		$this->assertEquals('c:\users\john\class.file.php', $method->fullPath);
		$this->assertEquals(MvcEditorResource::TYPE_METHOD, $method->type);
	}
	
	private function initOutputPdo(Zend_Db_Adapter_Abstract $dbAdapter) {
	
		// get the 'raw' connection because it can handle multiple statements at once
		$dbAdapter->getConnection()->exec(file_get_contents(__DIR__ . './../../resources/sql/resources.sql'));
	}

}