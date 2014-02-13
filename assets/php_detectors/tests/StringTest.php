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

class StringTest extends PHPUnit_Framework_TestCase {

	function testEnsureEnds() {
		$this->assertEquals('/home/user/', \opstring\ensure_ends_with('/home/user', '/'));
		$this->assertEquals('/home/user/', \opstring\ensure_ends_with('/home/user/', '/'));
		$this->assertEquals('/home/user//', \opstring\ensure_ends_with('/home/user//', '/'));
	}
	
	function testBeginsWithShouldReturnTrue() {
		$this->assertEquals(TRUE, \opstring\begins_with('/home/users', '/home'));
		$this->assertEquals(TRUE, \opstring\begins_with('/home/users', '/'));
	}
	
	function testBeginsWithShouldReturnFalse() {
		$this->assertEquals(FALSE, \opstring\begins_with('/home/users', 'users'));
		$this->assertEquals(FALSE, \opstring\begins_with('/home/users', 'root'));
	}

	function testAfterShouldReturnText() {
		$this->assertEquals('gmail.com', \opstring\after('hello@gmail.com', '@'));
		$this->assertEquals('gmail.com', \opstring\after('hello@gmail.com', 'hello@'));
	}
	
	function testAfterShouldNotReturnText() {
		$this->assertEquals('', \opstring\after('hello@gmail.com', '.com'));
		$this->assertEquals(FALSE, \opstring\after('hello@gmail.com', 'csu.edu'));
	}
	
	function testBeforeShouldReturnText() {
		$this->assertEquals('hello', \opstring\before('hello@gmail.com', '@'));
		$this->assertEquals('hello', \opstring\before('hello@gmail.com', '@gmail.com'));
	}
	
	function testBeforeShouldNotReturnText() {
		$this->assertEquals('', \opstring\before('hello@gmail.com', 'hello'));
		$this->assertEquals(FALSE, \opstring\before('hello@gmail.com', 'csu.edu'));
	}
	
	function testReplaceShouldChangeAll() {
		$this->assertEquals('these ese a string', \opstring\replace('this is a string', 'is', 'ese'));
	}
	
	function testLength() {
		$this->assertEquals(5, \opstring\length('this '));
	}
	
	function testCompare() {
		$this->assertEquals(-1, \opstring\compare('that', 'this'));
		$this->assertEquals(0, \opstring\compare('this', 'this'));
		$this->assertEquals(1, \opstring\compare('this', 'that'));
		$this->assertEquals(1, \opstring\compare('this', 'thIS'));
	}
	
	function testCompareCase() {
		$this->assertEquals(-1, \opstring\compare_case('that', 'this'));
		$this->assertEquals(-1, \opstring\compare_case('that', 'thIS'));
		$this->assertEquals(0, \opstring\compare_case('this', 'this'));
		$this->assertEquals(0, \opstring\compare_case('this', 'thIS'));
		$this->assertEquals(1, \opstring\compare_case('this', 'that'));
		$this->assertEquals(1, \opstring\compare_case('this', 'thAT'));
		
	}
}