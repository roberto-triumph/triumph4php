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
}

