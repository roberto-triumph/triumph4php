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
class MvcEditorCallClassTest extends PHPUnit_Framework_TestCase {
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
BEGIN_METHOD,CI_Loader,view
PARAM,"index"
PARAM,$data
RETURN
BEGIN_FUNCTION,stripos
PARAM,$data
PARAM,"find me"
RETURN
EOF
		);
		fseek($this->file, 0);
    }

    public function testClear() {
		$this->object->argument = 'news/index';
		$this->object->resource = 'CI_Loader::view';
		$this->object->type = MvcEditorCallClass::BEGIN_METHOD;
		$this->object->clear();
		$this->assertEquals('', $this->object->resource);
		$this->assertEquals('', $this->object->type);
		$this->assertEquals('', $this->object->argument);
	}

    public function testFromFile() {
		
		// first line
		$this->assertEquals(TRUE, $this->object->fromFile($this->file));
		$this->assertEquals(MvcEditorCallClass::BEGIN_METHOD, $this->object->type);
		$this->assertEquals('CI_Loader::view', $this->object->resource);
		
		// parameter 1
		$this->assertEquals(TRUE, $this->object->fromFile($this->file));
		$this->assertEquals(MvcEditorCallClass::PARAM, $this->object->type);
		$this->assertEquals('index', $this->object->argument);
		
		// parameter 2
		$this->assertEquals(TRUE, $this->object->fromFile($this->file));
		$this->assertEquals(MvcEditorCallClass::PARAM, $this->object->type);
		$this->assertEquals('$data', $this->object->argument);
		
		$this->assertEquals(FALSE, $this->object->fromFile($this->file));
		
		// second function
		$this->assertEquals(TRUE, $this->object->fromFile($this->file));
		$this->assertEquals(MvcEditorCallClass::BEGIN_FUNCTION, $this->object->type);
		$this->assertEquals('stripos', $this->object->resource);
		
		// parameter 1
		$this->assertEquals(TRUE, $this->object->fromFile($this->file));
		$this->assertEquals(MvcEditorCallClass::PARAM, $this->object->type);
		$this->assertEquals('$data', $this->object->argument);
		
		// parameter 1
		$this->assertEquals(TRUE, $this->object->fromFile($this->file));
		$this->assertEquals(MvcEditorCallClass::PARAM, $this->object->type);
		$this->assertEquals('find me', $this->object->argument);
		
		$this->assertEquals(FALSE, $this->object->fromFile($this->file));
		
		// no more lines, check that file pointer was advanced past the end because it shows 
		// that the file was read.
		$this->assertEquals(FALSE, $this->object->fromFile($this->file));
		$this->assertTrue(feof($this->file));
    }
}
?>