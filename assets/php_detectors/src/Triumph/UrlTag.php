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

/**
 * This is a class that represents a URL tag. Triumph will use these URL
 * objects to open files, run web pages, and inspect controllers and view files.
 */
class Triumph_UrlTag {

	/**
	 * These are relative URLs; they are relative to the server root and may contain a query string.
	 * For example, if a user would type in "http://localhost.codeigniter/index.php/news/index" the
	 * this URL should contain "index.php/news/index"
	 *
	 * @var string
	 */
	public $url;
	
	/**
	 * The file where the source code of the URL is located in.  This is the entry point
	 * to the URL; for a framework $fileName will be the location of the controller.
	 * fileName is full path (os-dependant).
	 * @var string
	 */
	public $fileName;
	
	/**
	 * The name of the controller class that handles this URL.  If a project does not
	 * use a framework, this will be empty.
	 *
	 * @var string
	 */
	public $className;
	
	/**
	 * The name of the controller method that handles this URL.  If a project does not
	 * use a framework, this will be empty.
	 *
	 * @var string
	 */
	public $methodName;
	

	public function __construct($url, $fileName, $className, $methodName) {
		$this->url = $url;
		$this->fileName = $fileName;
		$this->className = $className;
		$this->methodName = $methodName;
	}
}