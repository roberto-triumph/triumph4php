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
require_once __DIR__ . '/../src/MvcEditorCallClass.php';

/**
 * Test class for MvcEditorCallClass.
 */
class MvcEditorCallClassTest extends PHPUnit_Framework_TestCase
{
    /**
     * @var MvcEditorCallClass
     */
    protected $object;
	
	/**
	 * @var resource temporary file to use in testing; will contain the call stack we are testing
	 */
	protected $file;

    protected function setUp() {
        $this->object = new MvcEditorCallClass;
		$this->file = tmpfile();
		fwrite($this->file, <<<'EOF'
METHOD,view,CI_Loader::view,"index","$data"
FUNCTION,stripos,stripos,"$data","find me"
EOF
		);
		fseek($this->file, 0);
    }

    public function testClear() {
		$this->object->arguments = array(
			'news/index',
			'$data'
		);
		$this->object->resource = 'CI_Loader::view';
		$this->object->type = MvcEditorCallClass::TYPE_METHOD;
		$this->object->clear();
		$this->assertEquals('', $this->object->resource);
		$this->assertEquals('', $this->object->type);
		$this->assertEquals(TRUE, empty($this->object->arguments));
	}

    public function testFromFile() {
		
		// first line
		$this->assertEquals(TRUE, $this->object->fromFile($this->file));
		$this->assertEquals(MvcEditorCallClass::TYPE_METHOD, $this->object->type);
		$this->assertEquals('CI_Loader::view', $this->object->resource);
		$this->assertEquals(2, count($this->object->arguments));
		$this->assertEquals('index', $this->object->arguments[0]);
		$this->assertEquals('$data', $this->object->arguments[1]);
		
		// second line
		$this->assertEquals(TRUE, $this->object->fromFile($this->file));
		$this->assertEquals(MvcEditorCallClass::TYPE_FUNCTION, $this->object->type);
		$this->assertEquals('stripos', $this->object->resource);
		$this->assertEquals(2, count($this->object->arguments));
		$this->assertEquals('$data', $this->object->arguments[0]);
		$this->assertEquals('find me', $this->object->arguments[1]);
		
		// no more lines, check that file pointer was advanced past the end because it shows 
		// that the file was read.
		$this->assertEquals(FALSE, $this->object->fromFile($this->file));
		$this->assertTrue(feof($this->file));
    }
}
?>
