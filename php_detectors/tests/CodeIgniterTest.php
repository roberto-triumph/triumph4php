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

class CodeIgniterTest extends PHPUnit_Framework_TestCase {

	/**
	 * @var CodeIgniterFixtureClass
	 */
	private $fs;
	
	/**
	 * @var MvcEditorFrameworkCodeIgniter
	 */
	private $detector;

	function setUp() {
		$this->fs = new CodeIgniterFixtureClass();
		$this->fs->skeleton();
		$this->detector = new MvcEditorFrameworkCodeIgniter();
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
		
		// test a controller inside of a sub directory
		$cacheContents = <<<'EOF'
METHOD,vfs://application/controllers/welcome.php,Welcome,index
METHOD,vfs://application/controllers/welcome.php,Welcome,about
METHOD,vfs://application/controllers/admin/admin.php,Admin,index
EOF;
		$tmpDir = vfsStream::newDirectory('tmp');
		vfsStreamWrapper::getRoot()->addChild($tmpDir);
		$cacheFile = vfsStream::newFile('resources.csv');
		$cacheFile->withContent($cacheContents);
		$tmpDir->addChild($cacheFile);
		$this->fs->config();
		$this->fs->database();
		$this->fs->routes();
		$urls = $this->detector->makeUrls(vfsStream::url(''), vfsStream::url('tmp/resources.csv'), 'http://localhost');
		$expected = array(
			new MvcEditorUrlClass('http://localhost/index.php/welcome/index/', 'vfs://application/controllers/welcome.php', 'Welcome', 'index'),
			new MvcEditorUrlClass('http://localhost/index.php/welcome/about/', 'vfs://application/controllers/welcome.php', 'Welcome', 'about'),
			new MvcEditorUrlClass('http://localhost/index.php/admin/admin/index/', 'vfs://application/controllers/admin/admin.php', 'Admin', 'index')
		);
		$this->assertEquals($expected, $urls);
		
		$urls = $this->detector->makeUrls(vfsStream::url(''), vfsStream::url('tmp/resources.csv'), 'http://localhost/codeigniterapp/');
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
}

