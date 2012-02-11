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
 * This is a class that will take care of deserializing a single CallClass from the call stack file.
 * Usage:
 * The user of this class will open the file using the PHP file functions, the call the fromFile method
 * giving it the opened file resource.  The fromFile method will then reset and fill in the public members
 * according to the contents of the file.
 */
class MvcEditorCallClass {

	const TYPE_METHOD = 'METHOD';
	
	const TYPE_FUNCTION = 'FUNCTION';
	
	/** 
	 * @var string the type of call, either TYPE_METHOD or TYPE_FUNCTION
	 */
	public $type;

	/**
	 * @var string the fully qualified name of the resource being called; in the form ClassName::MethodName if this is a method
	 * or functionName if this is a function.
	 */
	public $resource;
	
	/**
	 * @var array of strings, each item in the array is the argument to the function / method call, as it was in the source 
	 * code.  This means that each item may be a string (ie. "news/view" without the quotes), a variable (ie. $data, or $this->data). In the
	 * case that one of the arguments is the result of another function / method call; then for now this is not supported.
	 */
	public $arguments;
	
	/**
	 * sets all public members to the empty string / array.
	 */
	public function clear() {
		$this->type = '';
		$this->resource = '';
		$this->arguments = array();
	}

	/**
	 * Read a single instance of CallClass from the given file.
	
	 * @param resource $fileResource the file to read from
	 * @return boolean TRUE if the call instance was successfully read from the given file. If false, either
	 * file is not a call stack file or the file did not contain proper data.
	 */
	public function fromFile($fileResource) {
		$this->clear();
		$ret = false;
		if (!feof($fileResource)) {
			$line = fgets($fileResource);
			if (!empty($line)) {
			
				// file format: a CSV file with variable columns. Format is as follows:
				// ResourceType,Identifier,Resource, Arg1 lexeme, Arg2 lexeme, ... Arg N lexeme
				//  
				// where
				// ResourceType = FUNCTION | METHOD
				// Identifier is the name of the function / method
				// Resource is the fully qualified name (ie. ClassName::MethodName)
				// ArgN Lexeme is the lexeme (string) of the Nth argument; lexeme is either the constant (when argument is a string / number)
				// or it can be a variable name.
				$columns  = explode(',', $line);
				if (count($columns) >= 3 && ($columns[0] == self::TYPE_METHOD || $columns[0] == self::TYPE_FUNCTION)) {
					$this->type = $columns[0];
					$this->resource = $columns[2];
					for ($i = 3; $i < count($columns); $i++) {
						
						// trim the ending newline that ends the line too
						$this->arguments[] = trim($columns[$i], "\"\n");
					}
					$ret = true;
				}
			}
		}
		return $ret;
	}

}