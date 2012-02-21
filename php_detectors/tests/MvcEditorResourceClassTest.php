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

class ResourceTest extends PHPUnit_Framework_TestCase {

	/**
	 * @var MvcEditorResource
	 */
	private $resource;
	
	function setUp() {
		$this->resource = new MvcEditorResource();
	}

	function testMakeFunction() {
		$this->resource->MakeFunction('testFunction', '$expeced, $actual');
		$this->assertEquals('testFunction', $this->resource->resource);
		$this->assertEquals('testFunction', $this->resource->identifier);
	}
	
	function testFromFile() {
		$contents = <<<EOF
FUNCTION,/home/user/file.php,testFunction,
CLASS,/home/user/class.file.php,MyClass
METHOD,/home/user/class.file.php,MyClass,myMethod
MEMBER,/home/user/class.file.php,MyClass,myProperty
EOF;
		$file = tmpfile();
		fputs($file, $contents);
		fseek($file, 0);
		$this->assertEquals(TRUE, $this->resource->FromFile($file));
		$this->assertEquals('testFunction', $this->resource->resource);
		$this->assertEquals('testFunction', $this->resource->identifier);
		$this->assertEquals('/home/user/file.php', $this->resource->fileName);
		$this->assertEquals(MvcEditorResource::TYPE_FUNCTION, $this->resource->type);
		
		$this->assertEquals(TRUE, $this->resource->FromFile($file));
		$this->assertEquals('MyClass', $this->resource->resource);
		$this->assertEquals('MyClass', $this->resource->identifier);
		$this->assertEquals('/home/user/class.file.php', $this->resource->fileName);
		$this->assertEquals(MvcEditorResource::TYPE_CLASS, $this->resource->type);
		
		$this->assertEquals(TRUE, $this->resource->FromFile($file));
		$this->assertEquals('MyClass::myMethod', $this->resource->resource);
		$this->assertEquals('myMethod', $this->resource->identifier);
		$this->assertEquals('/home/user/class.file.php', $this->resource->fileName);
		$this->assertEquals(MvcEditorResource::TYPE_METHOD, $this->resource->type);
		
		$this->assertEquals(TRUE, $this->resource->FromFile($file));
		$this->assertEquals('MyClass::myProperty', $this->resource->resource);
		$this->assertEquals('myProperty', $this->resource->identifier);
		$this->assertEquals('/home/user/class.file.php', $this->resource->fileName);
		$this->assertEquals(MvcEditorResource::TYPE_MEMBER, $this->resource->type);
		
		$this->assertEquals(FALSE, $this->resource->FromFile($file));
	}

}