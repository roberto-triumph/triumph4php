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
require_once 'CodeIgniterFixtureClass.php';
require_once __DIR__ . '/../src/MvcEditorFrameworkCodeIgniter.php';
require_once __DIR__ . '/../src/MvcEditorFileItem.php';

class CodeIgniterTest extends PHPUnit_Framework_TestCase {

	/**
	 * @var CodeIgniterFixtureClass
	 */
	private $fs;
	
	/**
	 * @var MvcEditorFrameworkCodeIgniter
	 */
	private $detector;
	
	/**
	 * connection to use
	 * @var Zend_Db_Adapter_Abstract
	 */
	private $db;
	
	/**
	 * full path to the sqlite database file.
	 */
	private $dbFileName;

	function setUp() {
		$this->dbFileName = sys_get_temp_dir() . '/urls.sqlite';
		$this->db = Zend_Db::factory('Pdo_Sqlite', array("dbname" => $this->dbFileName));
		Zend_Db_Table_Abstract::setDefaultAdapter($this->db);
		$this->initOutputPdo($this->db);
		
		$this->fs = new CodeIgniterFixtureClass();
		$this->fs->skeleton();
		$this->detector = new MvcEditorFrameworkCodeIgniter();
	}
	
	function tearDown() {
		if ($this->db) {
			$this->db->closeConnection(); 
		}
		if (file_exists($this->dbFileName)) {
			unlink($this->dbFileName);
		}
	}
	
	function testIsUsedByWithRelativeSystemPath() {
		file_put_contents(vfsStream::url('index.php'), '<?php $system_path = "lib/system"; ');
		$this->assertEquals(TRUE, $this->detector->isUsedBy(vfsStream::url('')));
	}
	
	function testIsUsedByWithAbsoluteSystemPath() {
		$absolute = vfsStream::url('lib/system');
		file_put_contents(vfsStream::url('index.php'), "<?php \$system_path = '$absolute'; ");
		$this->assertEquals(TRUE, $this->detector->isUsedBy(vfsStream::url('')));
	}
	
	function testUrlsSimple() {
		
		$fileItem = new MvcEditorFileItemClass();		
		$fileItemId1 = $fileItem->insert(array('full_path' => 'vfs://application/controllers/welcome.php'));
		$fileItemId2 = $fileItem->insert(array('full_path' => 'vfs://application/controllers/admin/admin.php'));
		
		// test a controller inside of a sub directory
		$this->db->insert('resources', array(
			'key' => 'index',
			'file_item_id' => $fileItemId1,
			'identifier' => 'index',
			'class_name' => 'Welcome',
			'type' => MvcEditorResource::TYPE_METHOD
		));
		$this->db->insert('resources', array(
			'key' => 'about',
			'file_item_id' => $fileItemId1,
			'identifier' => 'about',
			'class_name' => 'Welcome',
			'type' => MvcEditorResource::TYPE_METHOD
		));
		$this->db->insert('resources', array(
			'key' => 'index',
			'file_item_id' => $fileItemId2,
			'identifier' => 'index',
			'class_name' => 'Admin',
			'type' => MvcEditorResource::TYPE_METHOD
		));
		

		$this->fs->config();
		$this->fs->database();
		$this->fs->routes();
		$urls = $this->detector->makeUrls(vfsStream::url(''), $this->dbName, 'http://localhost');
		$expected = array(
			new MvcEditorUrlClass('http://localhost/index.php/welcome/index/', 'vfs://application/controllers/welcome.php', 'Welcome', 'index'),
			new MvcEditorUrlClass('http://localhost/index.php/welcome/about/', 'vfs://application/controllers/welcome.php', 'Welcome', 'about'),
			new MvcEditorUrlClass('http://localhost/index.php/admin/admin/index/', 'vfs://application/controllers/admin/admin.php', 'Admin', 'index')
		);
		$this->assertEquals($expected, $urls);
		
		$urls = $this->detector->makeUrls(vfsStream::url(''), $this->dbName, 'http://localhost/codeigniterapp/');
		$expected = array(
			new MvcEditorUrlClass('http://localhost/index.php/admin/admin/about/', 'vfs://application/controllers/welcome.php', 'Welcome', 'about')
		);
	}
	
	function testViewFilesSimple() {
		
		$cacheContents = <<<'EOF'
BEGIN_METHOD,MyController,index
ARRAY,$data,name,address
OBJECT,$user
SCALAR,"null user"
RETURN
BEGIN_METHOD,CI_Loader,view
PARAM,SCALAR,"index"
PARAM,ARRAY,$data,name,address
RETURN
BEGIN_FUNCTION,stripos
PARAM,$data
PARAM,"find me"
RETURN
EOF;
		$tmpDir = vfsStream::newDirectory('tmp');
		vfsStreamWrapper::getRoot()->addChild($tmpDir);
		$cacheFile = vfsStream::newFile('call_stack.csv');
		$cacheFile->withContent($cacheContents);
		$tmpDir->addChild($cacheFile);
		$this->fs->config();
		$this->fs->database();
		$this->fs->routes();
		$viewInfos = $this->detector->viewInfos(vfsStream::url(''), 'http://localhost/mycontroller/index', vfsStream::url('tmp/call_stack.csv'));
		$expected = array(
			new MvcEditorViewInfoClass(vfsStream::url('/application/views/index.php'), array('$name', '$address'))
		);
		$this->assertEquals($expected, $viewInfos);
	}

	private function initOutputPdo(Zend_Db_Adapter_Abstract $dbAdapter) {
	
		// get the 'raw' connection because it can handle multiple statements at once
		$this->assertNotNull($dbAdapter->getConnection()->exec(file_get_contents(__DIR__ . './../../resources/sql/resources.sql')));
	}
}

